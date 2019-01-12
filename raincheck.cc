#include <curl/curl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "options.hh"

using namespace std;

static Options opt;
// raincheck returns FALSE to shell when not raining, and TRUE to shell if currently raining.
// It also is responsible for pulling out the weather data out of the JSON reply.

static size_t write(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

float temperature(const float kelvin)
{
  if(opt["OUTPUT_TEMPERATURE_IN_F"] == "true")
    return (kelvin * (1.8)-459.67);
  else return (kelvin - 273.15);
}

int raining(string json)
{
  istringstream iss(json);
  string extraction = "";
  int status = 0x00;
  string weather;
  
  while(getline(iss,extraction,'"'))
  {
    getline(iss,extraction,'"'); //get the data in quotes
    if(extraction == "main")
    {
      getline(iss,extraction,'"');
      getline(iss,extraction,'"');
      if(extraction != "temp")
      {
	weather += extraction;
      }
      if(extraction == "Rain" || extraction == "Thunderstorm")
	status = 0xFF; //let mother nature take care of the problem.
    }
    if(extraction == "temp")
    {
      getline(iss,extraction,':');
      getline(iss,extraction,',');
      cout << temperature(stof(extraction)) << " degrees" << endl; //also for php/logs
    }
  }
  cout << weather << endl;
  return status;
}

string api_link()
{
  return "http://api.openweathermap.org/data/" + opt["API_VERSION"] + "/weather?precise&lat=" + opt["GPS_COORDINATE_LATITUDE"] + "&lon=" + opt["GPS_COORDINATE_LONGITUDE"] + "&appid=" + opt["API_ID_KEY"];
}

int main(int argc, char ** argv)
{
  if(opt.fail)
  {
    return 0xDEADBEEF;
  }
  
  if(opt["ALLOW_WEATHER_SERVICE"] == "false")
  {
    cout << endl;    
    cout << "Weather API Offline" << endl;
    return 0;
  }
  else
  {
    CURL * curl;
    string json;
    string apiLink = api_link();

    curl = curl_easy_init();
    if(curl && apiLink != "null")
    {
      curl_easy_setopt(curl, CURLOPT_URL, api_link().c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json);

      auto res = curl_easy_perform(curl);
      if(res != CURLE_OK)
      {
	ofstream ofs("/autogarden/log", ios::ate);
	ofs << "\nweather) Could not receive weather from OpenWeatherMap API! Did you edit PROGRAM.OPTIONS? autogarden will water your plants even if it is raining outside because of this.";
	return 1;
      }
      curl_easy_cleanup(curl);
      return raining(json);
    }
  }
  return 0xABAD1DEA; //error unrelated to weather patterns  
}
