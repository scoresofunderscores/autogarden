DEPENDENCY_LISTING=apache2 libapache2-mod-php libcurl4-openssl-dev gcc build-essential ntp
DEPENDENCY_LISTING_BAREBONES=libcurl4-openssl-dev gcc build-essential ntp
WEB_SERVER_DOCUMENT_ROOT=/var/www/html/
COMPILER=g++ -Wall -O3 --std=c++17
PKG_MANAGER_INSTALL_CMD=apt-get install
WEB_SERVER_USER=www-data

install :
	@echo "We're going to set up a timezone now.";
	dpkg-reconfigure tzdata;

	make dependencies;
	@echo "Attempting to compile executables....";
	make executables;
	@echo "Moving systemd service... if you do not use systemd make sure your system is configured to start /autogarden/d upon startup.";
	-mv autogarden.service /etc/systemd/system
	systemctl daemon-reload;
	systemctl enable autogarden;
	service autogarden start;
	touch /autogarden/tasks && chown $(WEB_SERVER_USER):$(WEB_SERVER_USER) tasks; #make tasks file and log
	touch /autogarden/log && chown $(WEB_SERVER_USER):$(WEB_SERVER_USER) log;
	chmod ug+rw tasks;

	@echo "Removing default files and moving all web files over to /var/www/html/....";
	-rm $(WEB_SERVER_DOCUMENT_ROOT)*;
	mv /autogarden/frontend/* /var/www/html;
	rm -r /autogarden/frontend;

	@echo "______________________________________";
	@echo "OK, try visiting one of these IP Addresses in a web browser.";
	@echo "If you're testing on another computer or phone, then use the one that is NOT 127.0.0.1 unless you're testing on this machine.";
	-ip address | grep "inet 1"
	@echo "______________________________________";

#this target will only work for systems with apt package management.
#feel free to ctrl c ctrl v the dependency list to use it with your
#preferred package manager.
dependencies :
	$(PKG_MANAGER_INSTALL_CMD) $(DEPENDENCY_LISTING);
	@echo "Grabbing libs from github....";
	git clone "git://git.drogon.net/wiringPi";
	git clone "https://github.com/HowardHinnant/date";
	cp date/src/tz* /autogarden/tz.cc;
	-cp date/LICENSE.txt /autogarden/DATE_LIBRARY_LICENSE.txt
	cd /autogarden/wiringPi && ./build;
	-cp /wiringPi/COPYING.LESSER /autogarden/WIRING_PI_LICENSE.txt;
	@echo "Done dealing with dependencies...";

barebones: raincheck.cc options.hh PROGRAM.OPTIONS
	$(PKG_MANAGER_INSTALL_CMD) $(DEPENDENCY_LISTING_BAREBONES);
	git clone "git://git.drogon.net/wiringPi";
	cd /autogarden/wiringPi && ./build;
	$(COMPILER) raincheck.cc -o raincheck -lcurl;
	chmod ug+x /autogarden/raincheck;
	@echo "Barebones target complete. See README.md for an example line to put into your crontab.";

#for rebuilding the executable files included with autogarden.
executables : schedule.cc raincheck.cc toggle.cc tz.cc options.hh PROGRAM.OPTIONS
	$(COMPILER) schedule.cc -o schedule;
	$(COMPILER) raincheck.cc -o raincheck -lcurl;
	$(COMPILER) -I/usr/local/include -L/usr/local/lib -I/autogarden/date/include toggle.cc tz.cc -o d -lwiringPi -pthread -lcurl;
	@echo "Executables are good to go, requesting adjustment of permissions on:";
	@echo "(raincheck, schedule and daemon (d))";
	chmod ug+x /autogarden/raincheck;
	chmod ug+x /autogarden/schedule;
	chmod ug+x /autogarden/d;

	@echo "Successfully updated autogarden executables with new code.";
