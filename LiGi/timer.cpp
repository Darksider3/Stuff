#include "timer.h"

int main()
{
  std::atomic_bool Stop(false);
  auto Goal = std::chrono::milliseconds(5000);
  Timer1 timer1(Stop, callout, Goal);

  std::thread thread1(&Timer1::RunnerFunc, &timer1);

  timer1.elapsed_lock();
  uint64_t curElapsed = timer1.get_elapsed();
  timer1.elapsed_unlock();

  uint64_t uintGoal = static_cast<uint64_t>(Goal.count());
  while(true)
  {
    if(uintGoal <= curElapsed)
    {
      std::cout << curElapsed;
      break;
    }
    timer1.elapsed_lock();
    curElapsed = timer1.get_elapsed();
    timer1.elapsed_unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "curElapsed: " << curElapsed << ", uintGoal: " << uintGoal << std::endl;
  }

  Stop = true;
  thread1.join();

  return 0;
}
