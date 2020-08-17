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


#include "../timer.h"
#include "../TimerTools.h"
#include "../Assertions.h"
#include <ncurses.h>
#include <deque>
#include <functional>

#include "../stack.h"

constexpr uint64_t POMODORO_TIME = 1000 * 60 * 30;
constexpr uint64_t SHORT_BREAK_TIME = 1000 * 60 * 6;
constexpr uint64_t BIG_BREAK_TIME = 1000 * 60 * 18;

class PomodoroTimer : public Li::Timer<PomodoroTimer>
{
private:

  void run(Li::Literals::TimeValue auto Goal)
  {
    this->setGoal(Goal);
    this->ResetTime();
    this->RunTimer();
  }
public:

  enum State
  {
    POMODORO,
    SHORT,
    LONG,
    PAUSE,
    STOP
  } m_state = State::PAUSE;

  using Timer<PomodoroTimer>::Timer;

  void RunPomo(uint64_t Goal = POMODORO_TIME)
  {
    this->m_state = State::POMODORO;
    this->run(Goal);
  }

  void RunShortBreak(uint64_t Goal = SHORT_BREAK_TIME)
  {
    this->m_state = State::SHORT;
    this->run(Goal);
  }

  void RunBigBreak(uint64_t Goal = BIG_BREAK_TIME)
  {
    this->m_state = State::LONG;
    this->run(Goal);
  }

  void RunPause(uint64_t Goal = 999999999999)
  {
    this->m_state = State::PAUSE;
    this->run(Goal);
  }

  State getState() { return this->m_state;}

  const std::string getTimeStr() const noexcept
  {
    namespace Format = Li::TimerTools::Format;
    Li::Literals::TimeValue  auto t = this->getTimeLeft();
    return Format::getMinutes(t)+":"+Format::getSeconds(t);
  }
};

WINDOW *w;

void quitter()
{
  endwin();
}

void init_colors()
{
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
}

void init()
{
  w = initscr();
  cbreak();
  noecho();
  nodelay(w, true);
  atexit(quitter);
  init_colors();
  curs_set(0);
}

void ViewRunningMenue()
{
  mvaddstr(11, 2, "Press q to exit");
  mvaddstr(10, 2, "Press b to take a break");
}

void ViewMode(PomodoroTimer::State const &state)
{
  using Li::STATE;

  if(state == PomodoroTimer::SHORT)
    mvaddstr(5, 1, "Taking a break");
  else if(state == PomodoroTimer::LONG)
    mvaddstr(5, 1, "Taking a big break!");
  else if(state == PomodoroTimer::POMODORO)
    mvaddstr(5, 1, "Working on a Pomodoro!");
  else if(state  == PomodoroTimer::PAUSE || state == PomodoroTimer::STOP)
    mvaddstr(5, 1, "Taking a manual pause!");

}

template<class FunctorT>
struct entry : Li::LLNode<entry<FunctorT>>
{
  std::function<FunctorT> functor;
};
template<class FunctorT>
class FunctorStack : public Li::Stack<entry<FunctorT>>
{
public:
  void append(std::function<FunctorT> funct)
  {
    auto bla = std::make_unique<entry<FunctorT>>();
    bla->functor = funct;
    this->pushd(bla);
  }
};

int main()
{

  auto set_white = [&]() {
    color_set(1,0);
  };


  auto set_red = [&]() {
    color_set(2, 0);
  };
#define TEST_NEW_TIMER
#ifdef TEST_NEW_TIMER
  // TEST
  std::atomic_bool stop = false, globalStop = false;
  /*
  std::function<void()> running = std::bind(&PomodoroTimer::Resume, std::ref(bla));
  std::deque<std::function<void()>> FunctionList;


  FunctorStack<void()> testers;
  testers.append(running);


  auto Threading = [&](){
    while(!globalStop)
    {
      if(FunctionList.empty())
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
      }
      else
      {
        //testers.top()->functor();
        std::function<void()> &thing = FunctionList.back();
        thing();
        FunctionList.pop_back();
        continue;
      }
    }
    return;
  };

  auto InsertFunc = [&](std::function<void()> func)
  {
    FunctionList.emplace_back(func);
  };

  std::thread ThreadingThingy(Threading);
  InsertFunc(std::bind(&PomodoroTimer::Resume, std::ref(bla)));
  globalStop = true;
  ThreadingThingy.join();
*/
#endif
  init();

  //int x, y;
  Li::STATE State;

  PomodoroTimer Timer{stop, POMODORO_TIME};

  std::thread PomoThread(&PomodoroTimer::RunPomo, &Timer, POMODORO_TIME);
  int c;
  while(true)
  {
    c = getch();
    set_red();
    mvprintw(2, 5, Li::TimerTools::Format::getFullTimeString(Timer.getTimeLeft()).c_str());
    set_white();
    ViewRunningMenue();
    ViewMode(Timer.getState());
    refresh();
    if(c == 'q')
    {
      Timer.Pause();
      PomoThread.join();
      quitter();
      return(0);
    }
    else if(c == 'p' && Timer.getState() != PomodoroTimer::State::STOP)
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunPause, std::ref(Timer), 999999999);
      Timer.Unpause();
    }
    else if(c == 'o')
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunPomo, std::ref(Timer),  POMODORO_TIME);
      Timer.Unpause();
      erase();
    }
    else if(c == 'b')
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunShortBreak, std::ref(Timer), SHORT_BREAK_TIME);
      Timer.Unpause();
      erase();
    }
    else if(c == 'g')
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunBigBreak, std::ref(Timer), BIG_BREAK_TIME);
      Timer.Unpause();
      erase();
    }
    else if(c == ERR)
    {
      // got no new data this run.
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return(0);
}
