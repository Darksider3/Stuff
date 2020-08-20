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
#include <ncurses.h>
#include <deque>
#include <functional>

#include "../stack.h"

constexpr uint64_t POMODORO_TIME = 1000 * 60 * 30;
constexpr uint64_t SHORT_BREAK_TIME = 1000 * 60 * 6;
constexpr uint64_t BIG_BREAK_TIME = 1000 * 60 * 18;
constexpr uint64_t PAUSE_STOP_VAL = UINT64_MAX-5;

enum PomoState
{
  POMODORO,
  SHORT,
  LONG,
  PAUSE,
  STOP
} m_state = PomoState::PAUSE;

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


  PomoState m_state;
  using Timer<PomodoroTimer>::Timer;

  void RunPomo(uint64_t Goal = POMODORO_TIME)
  {
    this->m_state = PomoState::POMODORO;
    this->run(Goal);
    this->m_state = PomoState::STOP;
  }

  void RunShortBreak(uint64_t Goal = SHORT_BREAK_TIME)
  {
    this->m_state = PomoState::SHORT;
    this->run(Goal);
    this->m_state = PomoState::STOP;
  }

  void RunBigBreak(uint64_t Goal = BIG_BREAK_TIME)
  {
    this->m_state = PomoState::LONG;
    this->run(Goal);
    this->m_state = PomoState::STOP;
  }

  void RunPause(uint64_t Goal = PAUSE_STOP_VAL)
  {
    uint64_t oldTimeLeft = this->getTimeLeft<uint64_t>();
    this->m_state = PomoState::PAUSE;
    this->run(Goal);
    this->setTimeLeft(oldTimeLeft);
  }

  void RunStop(uint64_t Goal = PAUSE_STOP_VAL)
  {
    this->m_state = PomoState::STOP;
    this->run(Goal);
  }

  PomoState getState() { return this->m_state;}

  const std::string getTimeStr() const noexcept
  {
    namespace Format = Li::TimerTools::Format;
    Li::Literals::TimeValue  auto t = this->getTimeLeft<uint64_t>();
    return Format::getMinutes(t)+":"+Format::getSeconds(t);
  }
};
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

WINDOW *w;
WINDOW *panelWin;
WINDOW *timerWin;

int x, y;

void quitter()
{
  delwin(w);
  endwin();
}

void init_colors()
{
  if(start_color() == ERR || !has_colors() || !can_change_color())
  {
    std::cerr << "Couldn't setup coloring!" << std::endl;
    quitter();
  }
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
}

void init_windows()
{
  if((w = initscr()) == nullptr)
  {
    std::cerr << "Error! Couldn't initialise ncurses!" << std::endl;
    quitter();
  }

  getmaxyx(w, y, x);
  panelWin = newwin(1, 1, 0, 0);
  timerWin = newwin(y-1, x-1, 1, 1);
}

void init()
{
  init_windows();
  cbreak();
  noecho();
  nodelay(w, true);
  atexit(quitter);
  init_colors();
  curs_set(0);
}

//general Layout Idea
/*
#C92020 <- Header Standard coloring(darkish red)
Timer background: #03A4BC, Timer Foreground: Black

+-----------------+------------+----------+
| (E)dit Settings | Statistics | ProgName |
+-----------------+------------+----------+
|                                         |
|                 TIMER                   |
|    -> MODE(Pause, Long, Short, Pomo)    |
|                                         |
+------------------+----------------------+
| Shortcuts:       | Short Statistics, e.g|
|  -> (B)reak      |                      |
|  -> (L)ong Break |  X Pomodoros this run|
|  -> (P)ause      |  X breaks till long  |
|  -> P(o)modoro   |  X Time estimate till|
|  -> (C)lose      |    done.             |
|                  |  X long breaks taken |
+------------------+----------------------+

*/
void ViewTitleBar()
{
  mvaddstr(1, 1, "(E)dit settings");
}
void ViewRunningMenue()
{
  mvaddstr(11, 2, "Press q to exit");
  mvaddstr(10, 2, "Press b to take a break");
}

void ViewMode(PomoState const &state)
{
  if(state == PomoState::SHORT)
    mvaddstr(5, 1, "Taking a break");
  else if(state == PomoState::LONG)
    mvaddstr(5, 1, "Taking a big break!");
  else if(state == PomoState::POMODORO)
    mvaddstr(5, 1, "Working on a Pomodoro!");
  else if(state  == PomoState::PAUSE)
    mvaddstr(5, 1, "Taking a manual pause!");
  else if(state == PomoState::STOP)
    mvaddstr(5, 1, "Waiting for input what to run!!");

}

int main()
{
  auto set_white = [&]() {
    color_set(1,0);
  };


  auto set_red = [&]() {
    color_set(2, 0);
  };
  // TEST
  std::atomic_bool stop = false;
  /*
  std::atomic_bool globalStop = false;
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
  init();

  //int x, y;
  PomodoroTimer Timer{stop, POMODORO_TIME};

  auto EraseStateChangeRepaint = [&](){
    werase(w);
    ViewRunningMenue();
    ViewTitleBar();
  };

  auto EraseSpecificLine =  [&](WINDOW* win, int Y, int X){
    int oldy, oldx;
    getyx(win, oldy, oldx);
    wmove(win, Y, X);
    wclrtoeol(win);
    wmove(win, oldy, oldx);
    wrefresh(win);
  };

  auto InitialPaint = [&] {
    EraseStateChangeRepaint();
  };


  std::thread PomoThread(&PomodoroTimer::RunPomo, &Timer, POMODORO_TIME);
  InitialPaint();
  PomoState oldState;
  while(true)
  {
    int c = getch();
    set_red();
    switch(Timer.getState())
    {
      case(PomoState::PAUSE):
        EraseSpecificLine(w, 3, 5);
        mvprintw(3, 5, "Paused!");
        break;
      case(PomoState::STOP):
        EraseSpecificLine(w, 3, 5);
        mvprintw(3, 5, "STOPPED!");
        break;
      case(PomoState::LONG):
      case(PomoState::SHORT):
      case(PomoState::POMODORO):
        EraseSpecificLine(w, 3, 5);
        mvprintw(3, 5, Li::TimerTools::Format::getFullTimeString(
                 Timer.getTimeLeft<uint64_t>()).c_str());
        break;
      //default:
      //  break;
    }

    set_white();
    ViewRunningMenue();
    ViewTitleBar();
    ViewMode(Timer.getState());
    refresh();
    if(c == 'q')
    {
      Timer.Pause();
      PomoThread.join();
      quitter();
      return(0);
    }
    else if(Timer.getState() == PomoState::STOP)
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunStop, std::ref(Timer), PAUSE_STOP_VAL);
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == 'p' && Timer.getState() != PomoState::PAUSE)
    {
      oldState = Timer.getState();
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunPause, std::ref(Timer), PAUSE_STOP_VAL);
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == 'p' && Timer.getState() == PomoState::PAUSE)
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::Resume, std::ref(Timer));
      Timer.m_state = oldState;
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == 'o')
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunPomo, std::ref(Timer),  POMODORO_TIME);
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == 'b')
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunShortBreak, std::ref(Timer), SHORT_BREAK_TIME);
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == 'g')
    {
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunBigBreak, std::ref(Timer), BIG_BREAK_TIME);
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == ERR)
    {
      // got no new data this run.
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return(0);
}
