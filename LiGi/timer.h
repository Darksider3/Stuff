#ifndef TIMER_H
#define TIMER_H

#define DEFAULT_MS 3000

#include <chrono>
#include <thread>
#include <ctime>
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>

void callout(uint64_t const &dd)
{
  //std::cout << "CurElaps: " << std::to_string(dd) << "ms." << std::endl;
  return;
}
namespace LiGi
{
class Timer1
{
  std::atomic_bool &stop;
  uint64_t elapser;
  uint64_t goal;
  std::mutex Elaps_Guard;
  //Locker Elaps_Lock;
  void (*func)(uint64_t const&);
public:

  static void dummy(uint64_t const&){};

  // exec dummy function above - it's just something we dont need here sometimes...
  Timer1(std::atomic_bool &stopper, uint64_t timeGoal):
    stop(stopper), func(Timer1::dummy)
  {
    this->elapser = 0;
    this->goal = timeGoal;
  }

  Timer1(std::atomic_bool &stopper, void (*f)(uint64_t const&)):
    stop(stopper), func(f)
  {
    this->elapser = 0;
    this->goal = DEFAULT_MS;
  }

  Timer1(std::atomic_bool &stopper, void (*f)(uint64_t const&),
         std::chrono::milliseconds &timeGoal):  stop(stopper), func(f)
  {

    this->elapser = 0;
    this->goal = timeGoal.count();
  };

  void adjustGoal(std::chrono::milliseconds const &Newgoal)
  {
    this->goal = Newgoal.count();
    this->Elaps_Guard.lock();
    this->elapser = 0;
    this->Elaps_Guard.unlock();
  }

  void run()
  {
    auto t_started = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(100);
    while(!stop)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      auto t_now = std::chrono::high_resolution_clock::now();
      std::chrono::milliseconds elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_started);
      if(delay <= elapsed) {
        t_started = t_now;
        this->Elaps_Guard.lock();
        this->elapser += 100;
        this->Elaps_Guard.unlock();
        this->func(this->elapser);
        if(this->elapser >= this->goal)
        {
          std::cout << "Goal reached! Exiting! \n";
          return;
        }
      }
    }
    return;
  }



  std::mutex& get_guard()
  {
    return this->Elaps_Guard;
  }

  uint64_t get_elapsed()
  {
    return this->elapser;
  }

  void elapsed_lock()
  {
    this->Elaps_Guard.lock();
  }

  void elapsed_unlock()
  {
    this->Elaps_Guard.unlock();
  }

  ~Timer1()
  {
    std::cout << std::to_string(this->elapser) << "ms run!" << std::endl;
  }

};

// @TODO: Make it actually able to run the whole process until restart...
// @TODO: Hooks! Pomo- start and end!
// @TODO: Breaks
// @TODO: Data structure representing state...
struct STATE
{
  enum {
    BREAK,
    PAUSE,
    POMO,

  } mode;
  uint64_t pomodoro_time = 1000 * 60 * 30;
  uint64_t big_break_time = 1000 * 60 * 18; // 18 minutes
  uint64_t short_break_time = 1000 * 60 * 7; // 7 minutes

  size_t short_breaks = 0;
  size_t big_breaks = 0;
  size_t manual_pauses = 0;

  uint64_t goal = 0;
  uint64_t elapsed = 0;
  uint32_t pomodoros = 0;
  bool done = false;

  void bigbreak()
  {
    this->goal = this->big_break_time;
    this->mode = STATE::BREAK;
  }
  void shortbreak()
  {
    this->goal = this->short_break_time;
    this->mode = STATE::BREAK;
  }

  void PomoStart()
  {
    this->goal = this->pomodoro_time;
    this->mode = STATE::POMO;
    this->done = false;
  }
  void Done()
  {
    ++this->pomodoros;
    this->done = true;
    this->mode = STATE::PAUSE;
  }
  void manualPause()
  {
    this->mode = STATE::PAUSE;
  }
};

class Pomodoro : public Timer1
{
  std::atomic_bool StopToggle = false;
  std::mutex stateGuard;
  STATE &state;
public:
  Pomodoro(STATE &states) :
    Timer1(StopToggle, states.pomodoro_time),
    state(states)
  {
    this->adjustGoal(std::chrono::milliseconds(this->state.pomodoro_time));
  }

  void RunPomo(void (*progressui)(uint64_t const&, STATE const &))
  {
    this->state.PomoStart();
    std::thread TimerThread(&Timer1::run, this);
    bool stopWhile = false;
    while(!stopWhile)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      this->elapsed_lock();
      this->state.elapsed = this->get_elapsed();
      progressui(this->state.elapsed, this->state);
      this->elapsed_unlock();
      if(this->state.goal < this->state.elapsed)
      {
        //@TODO: Debugging message: Done
        stopWhile = true;
      }
    }

    this->StopToggle = true;
    TimerThread.join();
    this->state.Done();
    //@TODO: Debuggingmessage run through
    return;
  }

  bool isPaused()
  {
    return (this->state.mode == STATE::PAUSE);
  }

  uint32_t PomosDone()
  {
    return this->state.pomodoros;
  }
};


class ThreadedPomodoro : public Pomodoro
{
  STATE state = STATE();
public:
  ThreadedPomodoro() : Pomodoro(state)
  {

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
    return (std::string(getHours(t)+":"+getMinutes(t)+":"+getSeconds(t)));
  }
}
}
}
#endif // TIMER_H
