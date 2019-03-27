#include <fstream>
#include <iostream>
#include <cstring>
#include <list>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

//I would use fs::path but the stock compiler I have on this test rpi doesnt support
//filesystem...? Doesn't really matter anyway.
const static std::string SCHEDULE_PATH{"/autogarden/tasks"};
const static std::string LOG_PATH{"/autogarden/log"};
constexpr static size_t  TASKLEN = 16; //normal task lines are 10 chars wide but when I implement better
//scheduling that supports durations that are more than 60 seconds, this will change.

void log_info(const std::string & what)
{
  std::ofstream ofs(LOG_PATH, std::ios::app);
  if(!ofs.fail())
  {
    ofs << "schedule@/autogarden/schedule) " << what << std::endl;
  }
}

struct Task
{
  int gpio = 0;
  std::string time = "00:00";
  int duration = 0;

  bool load(std::string DATA) //from user
  {
    std::istringstream iss{DATA};
    if(iss >> gpio >> time >> duration && check_time())
      return true;
    else return false;
  }
  
  bool load(std::ifstream & ifs) //from hdd
  {
    if(ifs >> gpio >> time >> duration && check_time())
      return true;
    else return false;
  }

  bool operator>>(std::ofstream & ofs) //to hdd
  {
    //cannot deduce to bool on older versions of gcc...?
    if(ofs << gpio << " " << time << " " << duration << "\n")
      return true;
    return false;
  }

  bool check_time()
  {
    //checks if in form HH:MM
    if(time.length() == 5 && time.at(2) == ':')
    {
      int h = stoi(time.substr(0,2));
      int m = stoi(time.substr(3,2));
      return (h <= 23 && h > -1 && m <= 59 && m > -1);
    }
    else return false;
  }
};

bool add(char ** DATA)
{
  if((strlen(DATA[2]) < 2 && strlen(DATA[3]) < 6 && strlen(DATA[4]) < 3) == false)
  {
    std::cerr << "schedule) Invalid data passed in!" << std::endl;
    return false;
  }
  char * taskBuf = new char[TASKLEN];
  sprintf(taskBuf, "%s %s %s %s %s", DATA[2], " ", DATA[3], " ", DATA[4]);
  std::string requestedTask = taskBuf;
  Task task;
  if(!task.load(requestedTask))
  {
    log_info("Unable to load in new task from frontend or user!");
    delete taskBuf;
    return false;
  }
  delete taskBuf;
  std::ifstream ifs(SCHEDULE_PATH);
  std::list<Task> tasks;
  //attempt to load in all tasks in file
  while(!ifs.eof() || !ifs.fail())
  {
    if(ifs.peek() == std::ifstream::traits_type::eof())
      break;
    
    tasks.push_back(Task());
    if(!tasks.back().load(ifs))
    {
      tasks.pop_back();
    }
  }
  //todo, allow merging of tasks, e.g. 1 09:30 70 & 1 09:31 10 would become just 1 09:30 70.
  //probably need to write string -> time_point and check if they overlap. blergh
  for(const auto & t : tasks) 
  {
    if(task.time == t.time && task.gpio == t.gpio) //task collision
    {
      log_info("Task collision detected. New task dismissed.");
      return 4;
    }
  }
  ifs.close();
  tasks.push_back(task); //add it in finally
  std::ofstream ofs(SCHEDULE_PATH, std::ios::trunc);
  if(ofs.fail())
  {
    log_info("Could not open tasks file for writing!");
    return false;
  }
  for(auto & t : tasks) //write out all valid tasks
  {
    t >> ofs;
  }
  return true;
}

bool remove(char * DATA)
{
  unsigned int lineRequested = atoi(DATA);
  if(lineRequested < 0)
  {
    return false;
  }
  std::vector<std::string> lines;
  std::ifstream ifs(SCHEDULE_PATH);
  if(!ifs.fail())
  {
    for(std::string line; std::getline(ifs,line);)
    {
      lines.push_back(line);
    }
    if(lines.size() < lineRequested)
    {
      log_info("Line requested for removal does not exist in file!");
      return false;
    }
    else
    {
      std::ofstream ofs(SCHEDULE_PATH, std::ios::trunc);
      for(unsigned int i = 0; i != lines.size(); ++i)
      {
	if(i != lineRequested)
	{
	  ofs << lines[i] << '\n';
	}
      }
    }
  }
  else
  {
    log_info("Unable to open tasks file for reading!");
    return false;
  }
  return true;
}

void help()
{
  std::cout << "Usage: /autogarden/schedule [add/remove] [DATA]" << std::endl
	    << "\t /autogarden/schedule [help]" << std::endl
	    << "\n\n This executable is primarily used to alter the tasks file "
	    << "and is called by the web interface. You can use it from the CLI too.\n"
	    << "You can also just edit the /autogarden/tasks file, just make sure you\n"
	    << "dont mess up the syntax!\n"
	    << "\n\n\t add - Adds a task if it does not already exist. "
	    << "The DATA argument must be in the form:\n\n"
	    << "\t<pin number> <start time (HH:MM) > <task duration (secs)>\n"
	    << "\n\t remove - Removes a task by its line number.\n\n"
	    << "The DATA argument must be in the form:\n\n\t<line number of task in tasks file (0 indexed) >\n\n"
	    << "This really isn\'t necessary for CLI usage but more so "
	    << "for the web interface. You can simply edit the tasks file "
	    << "to accomplish the same thing if you want. "
	    << std::endl;
}

void incorrect_syntax()
{
  std::cerr << "schedule) Incorrect syntax, valid arguments are:\n\t"
	    << "/autogarden/schedule [add/remove] [DATA]\n\n"
	    << "See the help documentation in help.html or schedule help for info on the DATA argument."
	    << std::endl;
}

int main(int argc, char ** argv)
{
  if(argc == 2) //help?
  {
    if(strcasecmp(argv[1], "help") == 0)
    {
      help();
      return 0;
    }
  }
  if(argc == 5) //expecting add
  {
    if(strcasecmp(argv[1], "add") == 0)
    {
      if(!add(argv))
      {
	log_info("Unable to add new task!");
	return 2;
      }
      else return 0;
    }
  }
  if(argc == 3)//attempt to remove a task
  {
    if(strcasecmp(argv[1], "remove") == 0)
    {
      if(!remove(argv[2]))
      {
	log_info("Unable to remove a task!");
	return 3;
      }
      else return 0;
    }
  }
  incorrect_syntax();
  return 1;
}
