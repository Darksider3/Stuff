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

namespace Literals {

template<class T>
//concept SuitableTime  = std::is_integral<T>::value && !std::is_abstract<T>::value;
concept TimeValue  = requires(T a) {
      std::is_integral<T>::value && std::is_arithmetic<T>::value;
    };
}
using namespace std::literals;

template<class T>
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
  uint64_t time_left;

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

  Literals::TimeValue auto getTimeLeft() const
  {
    return u_c.time_left;
  }
  Literals::TimeValue auto getGoal() const
  {
    return u_c.goal;
  }

  Literals::TimeValue auto getDelay() const
  {
    return u_c.delay;
  }

  Literals::TimeValue auto getSleep() const
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

  void ResetTime()
  {
    u_.time_left = u_c.goal;
  }

  void Stop()
  {
    u_.stopper = true;
    u_c.Pause();
    u_c.ResetTime();
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
};

class GoalTimer
{

public:
  std::atomic_bool &stop;
  uint64_t elapser;
  std::atomic_uint64_t goal;
  void (*func)(uint64_t const&);

  static void dummy(uint64_t const&){};

  // exec dummy function above - it's just something we dont need here sometimes...
  GoalTimer(std::atomic_bool &stopper, uint64_t timeGoal):
    stop(stopper), func(GoalTimer::dummy)
  {
    this->elapser = 0;
    this->goal = timeGoal;
  }

  GoalTimer(std::atomic_bool &stopper, void (*f)(uint64_t const&)):
    stop(stopper), func(f)
  {
    this->elapser = 0;
    this->goal = 3000;
  }

  GoalTimer(std::atomic_bool &stopper, void (*f)(uint64_t const&),
         std::chrono::milliseconds &timeGoal):  stop(stopper), func(f)
  {

    this->elapser = 0;
    this->goal = timeGoal.count();
  };

  void adjustGoal(std::chrono::milliseconds const &Newgoal)
  {
    this->elapser = 0;
    this->goal = Newgoal.count();
    this->stop = false;
  }

  void adjustGoal(uint64_t const newgoal)
  {
    this->elapser = 0;
    this->goal = newgoal;
    this->stop = false;
  }

  void run()
  {
    auto t_started = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(100);
    while(!stop)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      auto t_now = std::chrono::high_resolution_clock::now();
      std::chrono::milliseconds elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_started);
      if(delay <= elapsed) {
        t_started = t_now;
        this->elapser += 100;
        this->func(this->elapser);
        if(this->elapser >= this->goal)
        {
          this->stop = true;
          break;
        }
        if(this->stop)
          break;
      }
    }
    return;
  }



  uint64_t get_elapsed()
  {
    return this->elapser;
  }


  ~GoalTimer()
  {
  }

};

// @TODO: Make it actually able to run the whole process until restart...
// @TODO: Hooks! Pomo- start and end!
// @TODO: Breaks
// @TODO: Data structure representing state...
struct STATE
{
  enum current {
    BREAK,
    BIGBREAK,
    PAUSE,
    POMO,
    RESUME

  } mode;

  current priorMode;
  uint64_t pomodoro_time = 1000 * 60 * 30;
  uint64_t big_break_time = 1000 * 60 * 18; // 18 minutes
  uint64_t short_break_time = 1000 * 60 * 7; // 7 minutes

  size_t short_breaks = 0;
  size_t big_breaks = 0;
  size_t manual_pauses = 0;

  uint64_t goal = 0;
  uint64_t elapsed = 0;
  uint32_t Pomo_counter = 0;
  bool done = false;

  uint64_t bigbreak()
  {
    ++this->big_breaks;
    this->goal = this->big_break_time;
    this->mode = STATE::BIGBREAK;
    this->done = false;
    return this->big_break_time;
  }
  uint64_t shortbreak()
  {
    ++this->short_breaks;
    this->goal = this->short_break_time;
    this->mode = STATE::BREAK;
    this->done = false;
    return this->short_break_time;
  }

  uint64_t Pomodoro()
  {
    ++this->Pomo_counter;
    this->goal = this->pomodoro_time;
    this->mode = STATE::POMO;
    this->done = false;
    return this->pomodoro_time;
  }
  void Done()
  {
    this->done = true;
    this->elapsed = 0;
    this->priorMode = this->mode;
    this->mode = STATE::PAUSE;
  }
  void manualPause()
  {
    this->priorMode = this->mode;
    this->mode = STATE::PAUSE;
  }

  void Resume()
  {
    this->priorMode = this->mode;
    this->mode = priorMode;
  }

  uint64_t getPriorTime()
  {
    if(this->priorMode == STATE::POMO)
    {
      return this->pomodoro_time - this->elapsed;
    }
    if(this->priorMode == STATE::BREAK)
    {
      return this->short_break_time - this->elapsed;
    }
    if(this->priorMode == STATE::BIGBREAK)
    {
      return this->big_break_time - this->elapsed;
    }

    return this->pomodoro_time;
  }
};

class Pomodoro : public GoalTimer
{
  uint64_t left = 0;
  std::atomic_bool StopToggle = false;
public:
  STATE &state;
  Pomodoro(STATE &states) :
    GoalTimer(StopToggle, states.pomodoro_time),
    state(states)
  {
    this->adjustGoal(std::chrono::milliseconds(this->state.pomodoro_time));
  }

  void RunPomo(void (*progressui)(uint64_t const&, STATE const &), STATE::current exstate = Li::STATE::POMO)
  {
    switch(exstate)
    {
      case STATE::BREAK:
        this->adjustGoal(this->state.shortbreak());
        break;
      case STATE::BIGBREAK:
        this->adjustGoal(this->state.bigbreak());
        break;
      case STATE::PAUSE:
        this->left = this->state.goal - this->state.elapsed;
        this->state.manualPause();
        this->adjustGoal(9999999);
        break;
      case STATE::RESUME:
        this->adjustGoal(this->left);
        this->state.Resume();
        break;
      default:
        this->adjustGoal(this->state.Pomodoro());
    }

    std::thread TimerThread(&GoalTimer::run, this);
    this->StopToggle = false;
    while(!this->StopToggle)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      this->state.elapsed = this->get_elapsed();
      progressui(this->state.elapsed, this->state);
    }

    this->state.Done();
    TimerThread.join();
    //@TODO: Debuggingmessage run through
    return;
  }

  bool isPaused()
  {
    return this->state.mode == STATE::PAUSE;
  }

  uint32_t PomosDone()
  {
    return this->state.Pomo_counter;
  }
};
}
#endif // TIMER_H
