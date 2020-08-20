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
  std::atomic_bool &stopper;
  /**
   * @brief goal The time we actually want to reach!
   */
  uint64_t goal;

  /**
   * @brief delay Delay in which we actually increase variables
   */
  uint64_t delay = 100;

  /**
   * @brief elapsed Stores, after delay and sleep, the already slept/waited time
   */
  std::atomic_uint64_t time_left;

  /**
   * @brief sleep How long we sleep between checks
   */
  uint64_t sleep = 20;

public:

  void setTimeLeft(const Literals::TimeValue auto &set)
  {
    u_.time_left = set;
  }

  void setDelay(const Literals::TimeValue auto &set)
  {
    u_.delay = set;
  }

  void setSleep(const Literals::TimeValue auto &set)
  {
    u_.sleep = set;
  }

  void setGoal(Literals::TimeValue auto Goal)
  {
    u_.goal = Goal;
  }

  RValType getTimeLeft() const
  {
    uint64_t val = u_c.time_left;
    return val;
  }
  RValType getGoal() const
  {
    return u_c.goal;
  }

  RValType getDelay() const
  {
    return u_c.delay;
  }

  RValType getSleep() const
  {
    return u_c.sleep;
  }

  std::atomic_bool &getStopper() const
  {
    return u_c.stopper;
  }

  void Pause()
  {
    u_.stopper = true;
  }

  void ToggleStopper()
  {
    u_.stopper = !u_.stopper;
  }

  void Unpause()
  {
    u_.stopper = false;
  }

  void ResetTime()
  {
    u_.time_left = u_c.goal;
  }

  void Stop()
  {
    u_.stopper = true;
    u_.Pause();
    u_.ResetTime();
  }

  void Resume()
  {
    u_.stopper = false;
    // prevent deadlock - by decreasing `time_left` in RunTimer
    //it's possible we overflow on the lower

    // spectrum.
    if(u_c.time_left <= 0 || u_c.time_left == UINT64_MAX)
    {
      u_.time_left = u_c.goal;
    }
    u_.RunTimer();
  }

  void RunTimer()
  {
    auto t_start = std::chrono::steady_clock::now();
    std::chrono::milliseconds t_delay(u_c.delay);

    while(!u_c.stopper)
    {
      std::this_thread::sleep_for(
            std::chrono::milliseconds(u_c.sleep));

      auto t_now = std::chrono::steady_clock::now();

      std::chrono::milliseconds t_elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
      if(t_delay <= t_elapsed)
      {
        t_start = t_now;
        u_.time_left = u_.time_left - t_elapsed.count();

        if(u_c.time_left <= 0 || u_c.time_left == UINT64_MAX)
          break; // we done!
      }
    }
    return;
  }
  Timer(std::atomic_bool &stop, const Literals::TimeValue auto &Goal) :
    stopper(stop), goal(Goal), time_left(Goal){}

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
