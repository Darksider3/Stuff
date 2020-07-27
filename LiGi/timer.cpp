#include "timer.h"

#define ERASELINE printf("\033[A\33[2K");
#define THREAD_DONE true;
#define THREAD_RUNNING false;

bool DONE = false;
int Pomos = 0;


void Pomo()
{
  DONE = THREAD_RUNNING;
  std::atomic_bool Stop(false);
  auto Goal = std::chrono::milliseconds(1000*60*30);
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
    std::cout << "curElapsed: " << curElapsed << ", uintGoal: " << uintGoal << ", Percent done: " <<
                 (curElapsed / (uintGoal/100))<< "\n";
    timer1.elapsed_lock();
    curElapsed = timer1.get_elapsed();
    timer1.elapsed_unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ERASELINE
  }


  DONE = THREAD_DONE;
  ++Pomos;
  Stop = true;
  thread1.join();
}

int main()
{
  std::string answer("");
  while(true)
  {
    Pomo();
    if(DONE)
    {
      std::cout << "Would you like to start a new pomodoro? You have done " << Pomos << " already: ";
      std::cin >> answer;
      if(answer == "yes")
        continue;
      else
        break;
    }
  }
  return 0;
}
