#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <thread>
#include <ctime>
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>

void callout()
{
  std::cout << "HEREHEREHERE" << std::endl;
}

class Timer1
{
  std::atomic_bool &stop;
  std::uint64_t elapser;
  std::mutex Elaps_Guard;
public:
  Timer1(std::atomic_bool &stopper): elapser(0), stop(stopper)
  {
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
        callout();
      }
    }
  }

  ~Timer1()
  {
    std::cout << std::to_string(this->elapser) << "ms run!" << std::endl;
  }

};

#endif // TIMER_H
