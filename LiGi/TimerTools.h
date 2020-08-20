#ifndef TIMERTOOLS_H
#define TIMERTOOLS_H
#include "timer.h"
namespace Li
{
namespace TimerTools
{
namespace Format
{
std::string getSeconds(uint64_t const t)
{
  std::stringstream ss("");
  int secs = (
               (t + 100)
               / 1000)
             % 60;
  if(secs < 10)
    ss << "0" << secs;
  else
    ss << secs;

  return ss.str();
}

std::string getMinutes(uint64_t const t)
{
  std::stringstream ss("");
  int mins = ((
                (t + 100)
                / 1000)
              / 60)
             % 60;
  if(mins < 10)
    ss << "0" << mins;
  else
    ss << mins;

  return ss.str();
}

std::string getHours(uint64_t const t)
{
  std::stringstream ss("");
  int hours = (((
                  (t + 100)
                  / 1000)
                / 60)
               / 60)
              % 60;
  if(hours < 10)
    ss << "0" << hours;
  else
    ss << hours;

  return ss.str();
}

std::string getFullTimeString(uint64_t const t)
{
  return (
            getHours(t)+":"+getMinutes(t)+":"+getSeconds(t)
            );
}
}
}
}
#endif // TIMERTOOLS_H
