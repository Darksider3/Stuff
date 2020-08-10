#ifndef TIMER_H
#define TIMER_H

#define DEFAULT_MS 3000

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>

void callout(uint64_t const &dd)
{
  return;
}
namespace Li
{
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
    this->goal = DEFAULT_MS;
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
    return (std::string(
              getHours(t)+":"+getMinutes(t)+":"+getSeconds(t)
            ));
  }
}
}
}
#endif // TIMER_H
