#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <thread>
#include <ctime>
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>

void callout(uint64_t dd)
{
  std::cout << "CurElaps: " << std::to_string(dd) << "ms." << std::endl;
  return;
}

class Timer1
{
  std::atomic_bool &stop;
  uint64_t elapser;
  uint64_t goal;
  std::mutex Elaps_Guard;
  void (*func)(uint64_t);
public:
  Timer1(std::atomic_bool &stopper, void (*f)(uint64_t)):  stop(stopper), func(f)
  {
    this->elapser = 0;
    this->goal = 500;
  };
  void run(){
    auto t_start = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds delay(100);
    while(!stop) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      auto t_now = std::chrono::high_resolution_clock::now();
      std::chrono::milliseconds elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(t_now - t_start);
      if(delay <= elapsed) {
        t_start = t_now;
        this->Elaps_Guard.lock();
        this->elapser += 100;
        this->Elaps_Guard.unlock();
        callout(21);
      }
    }
  }

  void RunnerFunc()
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
          break;
        }
      }
    }
  }

  ~Timer1()
  {
    std::cout << std::to_string(this->elapser) << "ms run!" << std::endl;
  }

};

#endif // TIMER_H
