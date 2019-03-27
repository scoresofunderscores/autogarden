#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <strings.h>
#include <chrono>
#include <sstream>
#include <functional>
#include <thread>
#include <list>
#include <iostream>
#include <stdexcept>

// EXTERNAL LIBS ////////////////////////////////
#include <wiringPi.h>
#include "date/tz.h"

#define ON 1
#define OFF 0

using namespace date;
using namespace std::chrono;

const static std::string SCHEDULE_PATH{"/autogarden/tasks"};
const static std::string LOG_PATH{"/autogarden/log"};
const static std::string PINS_PATH{"/autogarden/pins"};
const static std::vector<int> DEFAULT_PINS = {0,1,2,3,4,5}; //default manual toggle pins in absence of pins file.

void log(const std::string & what)
{
  std::ofstream ofs(LOG_PATH, std::ios::app);
  if(!ofs.fail())
  {
    ofs << "autogardend@/autogarden/d) " << what << std::endl;
  }
}

void err(const std::string & what)
{
  std::ofstream ofs(LOG_PATH, std::ios::app);
  if(!ofs.fail())
  {
    ofs << "autogardend@/autogarden/d) ERROR: " << what << std::endl;
  }
}

bool schedule_changed()
{
  static size_t lastHash{};
  std::hash<std::string> h;
  size_t currentHash;

  std::ifstream sched(SCHEDULE_PATH);
  if(sched.fail())
  {
    log("Schedule could not be opened when performing hash check!");
    return false;
  }
  if(sched.eof())
  {
    log("Empty schedule file detected during hash check!");
    return false;
  }
  std::stringstream schedBuf;
  schedBuf << sched.rdbuf();
  currentHash = h(schedBuf.str()); //get hash of file now
  if(currentHash != lastHash) //changed
  {
    log("Schedule changed on disk. Loading in new tasks.");
    lastHash = currentHash;
    return true;
  }
  else
  {
    lastHash = currentHash;
    return false;
  }
}

struct Task
{ 
  Task(std::ifstream & TASKFILE)
  {
    if(TASKFILE.fail() || TASKFILE.eof())
    {
      log("Taskfile could not be opened for parsing. Check permissions or if it is openable.");
      fail = true;
    }
    else if(!(TASKFILE >> device >> time >> duration))
    {
      log("Taskfile syntax error detected. Is it using the form: PIN(int) hh:mm DURATION(int) on each line?");
      fail = true;
    }
    TASKFILE.get(); //\n
  }
  
  int device{};
  unsigned int duration{};
  std::string time;
  
  bool fail = false;
  
  bool perform(const std::string & now) //will toggle pins if the task is ready to be performed. Otherwise this does nothing.
  {
    if(!fail)
    {
      return (now == time);
    }
    return false;
  }

  bool operator==(const Task & t)
  {
    return(device == t.device && duration == t.duration && time == t.time);
  }
};

void toggle(Task & task) //toggle gpio and die whenever
{
  pinMode(task.device, OUTPUT);
  digitalWrite(task.device, 1);
  std::this_thread::sleep_for(seconds(task.duration));
  digitalWrite(task.device, 0);
}

void manual_toggle(const bool status)
{
  std::ifstream ifs("/autogarden/pins");
  std::vector<int> pinsToToggle;
  if(ifs.fail() || ifs.peek() == std::ifstream::traits_type::eof())
  {
    pinsToToggle = DEFAULT_PINS;
  }
  else
  {
    std::string ext;
    while(std::getline(ifs,ext))
    {
      try
      {
	pinsToToggle.push_back(std::stoi(ext));
      }
      catch(const std::exception & ex)
      {
	err(std::string(ex.what()));
	break;
      }
    }
  }
  if(status)
  {
    for(auto & pin : pinsToToggle)
    {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, 1);
    }
  }
  else
  {
    for(auto & pin : pinsToToggle)
    {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, 0);
    }
  }
}

bool load_schedule(std::list<Task> & tasks)
{
  std::ifstream ifs(SCHEDULE_PATH);
  if(ifs.fail() || ifs.eof())
  {
    return false;
  }
  else //attempt to load in tasks
  {
    tasks.erase(tasks.begin(),tasks.end());
    while(!ifs.eof() && !ifs.bad())
    {
      if(ifs.peek() == std::ifstream::traits_type::eof())
	break;
      tasks.push_back(Task(ifs));
    }
  }
  tasks.unique();
  return true;
}

int main(int argc, char ** argv)
{
  wiringPiSetup();
  if(argc == 1) //daemon mode
  {
    std::list<Task> tasks;
    for(;;)
    {
      std::chrono::system_clock::time_point rn = std::chrono::system_clock::now();
      if(schedule_changed())
      {	
	if(load_schedule(tasks))
	{
	  log("Loaded in new schedule.");
	}
	else log("Schedule file could not load tasks! Any valid tasks before the syntax error in the /autogarden/tasks file will be attempted.");
      }
      if(!tasks.empty())
      {
	std::ostringstream oss;
	oss << date::format("%H:%M", date::make_zoned(date::current_zone(), rn));
	for(Task & task : tasks)
	{
	  if(task.perform(oss.str()))
	  {
	    if(system("/autogarden/raincheck") > 0) //not raining, send signal
	    {
	      std::thread job(toggle, std::ref(task));
	      job.detach();
	    }
	  }
	}
      }
      std::this_thread::sleep_until(rn + 1min); //sleep until next minute
    }
  }
  else //do some user defined task NOW. Disregard the schedule.
  {
    //you can add whatever tasks you want the daemon to handle here.
    if(argc == 2)
    {
      if(strcasecmp(argv[1], "off") == 0)
      {
	manual_toggle(OFF);
	return 0;
      }
      if(strcasecmp(argv[1], "on") == 0)
      {
	manual_toggle(ON);
	return 0;
      }
    }
    else return 1;
  }
}

