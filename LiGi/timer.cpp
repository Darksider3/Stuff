#include "timer.h"

int main()
{
  std::atomic_bool Stop(false);
  Timer1 timer1(Stop, callout);

  std::thread thread1(&Timer1::RunnerFunc, &timer1);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  Stop = true;
  thread1.join();

  return 0;
}
