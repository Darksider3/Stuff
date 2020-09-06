/**
 ** This file is part of the Stuff project.
 ** Copyright 2020 darksider3 <github@darksider3.de>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <concepts>

namespace Li
{
namespace Literals
{

template<class T>
concept TimeValue  = requires(T a) {
      std::is_integral<T>::value && std::is_arithmetic<T>::value && std::is_unsigned<T>::value;
    };
}

using namespace std::literals;

template<class T, Literals::TimeValue RValType = uint64_t>
class Timer
{
private:
  T& u_ = static_cast<T&>(*this);
  const T& u_c = static_cast<const T&>(*this);
  /**
   * @brief stopper atomic stopping variable to control flow
   */
  std::atomic_bool &M_stopper;
  /**
   * @brief goal The time we actually want to reach!
   */
  uint64_t M_goal;

  /**
   * @brief delay Delay in which we actually increase variables
   */
  uint64_t M_delay = 100;

  /**
   * @brief elapsed Stores, after delay and sleep, the already slept/waited time
   */
  std::atomic_uint64_t M_timeLeft;

  /**
   * @brief sleep How long we sleep between checks
   */
  uint64_t M_sleep = 20;

public:

  operator RValType() const {return u_c.M_timeLeft;}

  void setTimeLeft(const Literals::TimeValue auto &set)
  {
    u_.M_timeLeft = set;
  }

  void setDelay(const Literals::TimeValue auto &set)
  {
    u_.M_delay = set;
  }

  void setSleep(const Literals::TimeValue auto &set)
  {
    u_.M_sleep = set;
  }

  void setGoal(Literals::TimeValue auto Goal)
  {
    u_.M_goal = Goal;
  }

  RValType getTimeLeft() const
  {
    uint64_t val = u_c.M_timeLeft;
    return val;
  }
  RValType getGoal() const
  {
    return u_c.M_goal;
  }

  RValType getDelay() const
  {
    return u_c.M_delay;
  }

  RValType getSleep() const
  {
    return u_c.M_sleep;
  }

  std::atomic_bool &getStopper() const
  {
    return u_c.M_stopper;
  }

  void Pause()
  {
    u_.M_stopper = true;
  }

  void ToggleStopper()
  {
    u_.M_stopper = !u_.M_stopper;
  }

  void Unpause()
  {
    u_.M_stopper = false;
  }

  void ResetTime()
  {
    u_.M_timeLeft = u_c.M_goal;
  }

  void Stop()
  {
    u_.M_stopper = true;
    u_.M_Pause();
    u_.M_ResetTime();
  }

  void Resume()
  {
    u_.M_stopper = false;
    // prevent deadlock - by decreasing `time_left` in RunTimer
    //it's possible we overflow on the lower

    // spectrum.
    if(u_c.M_timeLeft <= 0 || u_c.M_timeLeft == UINT64_MAX)
    {
      u_.M_timeLeft = u_c.M_goal;
    }
    u_.RunTimer();
  }

  void RunTimer()
  {
    auto t_start = std::chrono::steady_clock::now();
    std::chrono::milliseconds t_delay(u_c.M_delay);

    while(true)
    {
      if(u_c.M_stopper)
      {
        // @TODO: Need 2 states here:
        //        Either stopped by the global
        //        OR     stopped by time running out(which is important for some tasks)
        break;
      }
      std::this_thread::sleep_for(
            std::chrono::milliseconds(u_c.M_sleep));

      auto t_now = std::chrono::steady_clock::now();

      std::chrono::milliseconds t_elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
      if(t_delay <= t_elapsed)
      {
        t_start = t_now;
        u_.M_timeLeft = u_.M_timeLeft - t_elapsed.count();

        if(u_c.M_timeLeft <= 0 || u_c.M_timeLeft == UINT64_MAX)
          break; // we done!
      }
    }
    return;
  }
  Timer(std::atomic_bool &stop, const Literals::TimeValue auto &Goal) :
    M_stopper(stop), M_goal(Goal), M_timeLeft(Goal){}

  virtual ~Timer() = default;

  // Copy Policy - disable
  // It does no make sense at all to allow these. The timers have no single benefit of it
  Timer &operator=(const Timer& rhs) = delete;
  Timer (const Timer& src) = delete;

  // Move Policy - disable
  // again, no benefit

  Timer &operator=(Timer&& rhs) = delete;
  Timer (const Timer&& src) = delete;
};
}
#endif // TIMER_H
