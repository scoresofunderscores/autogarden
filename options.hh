#pragma once

#include <fstream>
#include <map>
#include <string>
#include <array>
#include <utility>
#include <sstream>

const static std::string NIL = "null";

typedef struct Options
{
  Options()
  {
    std::ifstream ifs("/autogarden/PROGRAM.OPTIONS");
    std::string line;    
    if(!ifs.fail())
    {      
      while(std::getline(ifs, line))
      {
	if(line.length() > 1)
	{
	  if(line[0] != '#')
	  {
	    std::istringstream iss{line};
	    std::string k,v;
	    std::getline(iss, k, ' ');
	    std::getline(iss, v, '\n');
	    data.emplace(make_pair(k,v));
	  }
	}
      }
    }
    else fail = true;
  }
  
  const std::string & operator [](const char * what) const
  {
    auto res = data.find(std::string(what));
    if(res != data.end())
    {
      return res->second;
    }
    else return NIL;
  }
  
  std::map<std::string,std::string> data;
  bool fail = false;
}
  Options;
