#ifndef TIMERTOOLS_H
#define TIMERTOOLS_H
#include "timer.h"
namespace Li
{
namespace TimerTools
{
namespace Format
{

template<Li::Literals::TimeValue T>
std::string getSeconds(T const &t)
{
  std::stringstream ss("");
  T secs = (
               (t + 100)
               / 1000)
             % 60;
  if(secs < 10)
    ss << "0" << secs;
  else
    ss << secs;

  return ss.str();
}

template<Li::Literals::TimeValue T>
std::string getMinutes(T const &t)
{
  std::stringstream ss("");
  T mins = ((
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

template<Li::Literals::TimeValue T>
std::string getHours(T const &t)
{
  std::stringstream ss("");
  T hours = (((
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
template<Li::Literals::TimeValue T>
std::string getFullTimeString(T const &t)
{
  return (
            getHours(t)+":"+getMinutes(t)+":"+getSeconds(t)
            );
}
}
}
}
#endif // TIMERTOOLS_H
