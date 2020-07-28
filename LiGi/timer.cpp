#include "timer.h"
#include <sstream>

#define ERASELINE printf("\033[A\33[2K");


using namespace LiGi;


typedef struct POMODORO_STATE
{
  bool done = false;
  uint64_t Pomos = 0;
  std::thread TimerThread;


  void toggleDone()
  {
    this->done = !this->done;
  }

  void CountPomo()
  {
    ++this->Pomos;
  }

  std::string getCount()
  {
    std::stringstream ss("");

    ss << this->Pomos;
    return ss.str();
  }
} POMODORO_STATE;
POMODORO_STATE OUR_STATE;
bool DONE = false;
int Pomos = 0;


std::string getMinutes(uint64_t const &elaps)
{
  std::stringstream ss("");
  ss << ((elaps+100)/1000)/60;
  return ss.str();
}

std::string getSeconds(uint64_t const &elaps)
{
  std::stringstream ss("");
  ss << ((elaps+100)/1000);
  return ss.str();
}

std::string getPercent(uint64_t const &elaps, uint64_t const &to)
{
  std::stringstream ss("");
  ss << ((elaps+100) / (to / 100));
  return ss.str();
}

void Pomo()
{
  OUR_STATE.done = false;
  std::atomic_bool Stop(false);
  auto Goal = std::chrono::milliseconds(1000*60*30);
  Timer1 timer1(Stop, callout, Goal);

  std::thread thread1(&Timer1::run, &timer1);

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
    std::cout << "curElapsed: " << curElapsed << ", uintGoal: " << uintGoal <<
                 ", Percent done: " << getPercent(curElapsed, uintGoal) <<
                 " or " << getMinutes(curElapsed) << " Minutes and " <<
                 getSeconds(curElapsed) << " seconds" << "\n";
    std::cout << "And total Pomos this run: " << OUR_STATE.getCount() << "\n";
    timer1.elapsed_lock();
    curElapsed = timer1.get_elapsed();
    timer1.elapsed_unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ERASELINE;
    ERASELINE;
  }


  Stop = true;
  thread1.join();
  OUR_STATE.done = true;
  OUR_STATE.CountPomo();
  return;
}

int main()
{
  std::string answer("");
  while(true)
  {
    answer = "";
    Pomo();
    if(OUR_STATE.done)
    {
      std::cout << "\n" << "Would you like to start a new pomodoro? You have done "
                << OUR_STATE.getCount() << " already: ";
      std::cin >> answer;
      if(answer == "yes")
      {
        ERASELINE;
        continue;
      }
      else
        break;
    }
  }
  return 0;
}
