# autogarden
#### an automatic or manual watering software solution for low cost GPIO enabled SBCs.
![auto_garden screenshot](https://occc.co/de/img/autogarden_scr.png)

## What in the world is this?

In simple terms, autogarden turns your cheapo ARM board into a basic irrigation controller. It uses the weather data from your area to figure out whether it should water your plants. However, if you wish, it can act offline and only on your local network - just by using the manual control panel. All you have to do is install a debian based image onto an sd card, install this on it, and wire up some hardware for it to control.

autogarden sends power to the desired 'devices' by toggling GPIO pins (usually you want to connect the outputs to 3.3v -> 12v relays). This is accomplished by a crontab like file that can be edited via the web interface and the terminal. This means that you can simply install autogarden on a board with GPIO pins, create hardware to drive your pumps or whatever, and choose when your autogarden server waters your plants through the web frontend. In addition, a 'barebones' target is available for building that only requires libcurl, crontab, and a C++ compiler. Further details are below.

## How do I install this?

Firstly a warning: the Makefile included in this repo is designed to install all dependencies for you and set up pretty much everything - however it uses some executables that are only on Debian based distributions. This is because of the prevalence of Debian based images used for small SBCs. You may need to alter the Makefile if you are not using a Debian based image.

1) Log in as root via ssh or locally on your machine.

2) Navigate to /

> cd /

3) Grab the latest packages for your server using a package manager (optional but recommended anyway)

> apt-get update && apt-get upgrade

4) If you do not have 'make' or 'git' installed already, run this to grab these.

> apt-get install make git

5) In the / directory, run this to create an /autogarden dir with all of the sourcecode.

> git clone https://git.sr.ht/~westmeal/autogarden

> git clone https://github.com/scoresofunderscores/autogarden

6) Go into the autogarden directory.

> cd /autogarden

7) Alter your config file now: PROGRAM.OPTIONS. Config values are whitespace delimited and # at the beginning of a line is a comment telling you about a certain value.

> You'll need to set up an account on https://openweathermap.org/ to receive an API key. Paste this API key in the option for an API key.

>Next up, you'll need to know your GPS coordinates. Go on OSM or google maps and look up what coordinates your address is at for this information. Place these coordinates in the GPS_COORDINATE options. Make sure you don't mix em up. After that, now you can edit the WEATHER_API_ENABLED option to true. That's it as far as config goes, unless you want to change the temperature display to Celsius.

8) Run this to compile and set up autogarden. It should run all necessary commands on a debian OS.

> make install

9) When it's done doing it's thing, test out your web interface by opening a web browser when connected to your LAN, and type in the hostname of your device. In addition, you can use the IPs that the makefile spits out after installation is complete. You should see the web interface and you should be able to configure everything through it from here on out.

10) Read the help.html page in the web control panel for more information and (coming soon) hardware setup ideas. As far as software goes, you're done!

## What about if I just want a no frills option that works on anything?

If you don't want any frontend stuff, simply run **make barebones** instead of **make install** and this will only set up an executable in /autogarden/raincheck as well as installing gpio. You'll still need to set up your /autogarden/PROGRAM.OPTIONS file with all of the correct information if you want to use the weather API, otherwise raincheck will always return 0 (dumb mode, will ALWAYS attempt to send a signal). You can then 'schedule' your tasks by adjusting your root's crontab, with something like this to control your devices:

0 7 * * * /autogarden/raincheck && gpio mode 0 out && gpio write 0 1 && sleep 30 && gpio write 0 0;

This entry will check the weather at 7 AM every day, and as long as it is not raining, output a digital signal through the gpio pin '0' for 30 seconds before turning off.
You can then delete pretty much everything from /autogarden/ except for the raincheck executable and the PROGRAM.OPTIONS.

## How do I make the hardware...?

Soon I will write a detailed guide for a simple set up with pictures in help.html. You can get to the help page by clicking on the 'need some help?' link on the web interface's control panel.

**TL;DR** You're going hook up jumpers to the GPIO pins you want to use and have them control 12 volt relays. These relays will provide 12v power to a water pump or electronic valve near or in some sort of reservoir that stores water from rain and from your hose. Of course, you can really make it anyway you want, as long as the signals controlling your things are GPIO voltage (I think 3.3V or 5V).


