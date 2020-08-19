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
    uint64_t oldTimeLeft = this->getTimeLeft();
    this->m_state = State::PAUSE;
    this->run(Goal);
    this->setTimeLeft(oldTimeLeft);
  }

  State getState() { return this->m_state;}

  const std::string getTimeStr() const noexcept
  {
    namespace Format = Li::TimerTools::Format;
    Li::Literals::TimeValue  auto t = this->getTimeLeft();
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

  auto EraseStateChangeRepaint = [&](){
    ViewRunningMenue();
    ViewTitleBar();
  };

  auto EraseSpecificLine =  [&](int Y, int X, WINDOW *win){
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
  PomodoroTimer::State oldState;
  int c;
  while(true)
  {
    c = getch();
    set_red();
    if(Timer.getState() != PomodoroTimer::State::PAUSE)
    {
      EraseSpecificLine(3, 5, w);
      mvprintw(3, 5, Li::TimerTools::Format::getFullTimeString(Timer.getTimeLeft()).c_str());
    }
    else
    {
      EraseSpecificLine(3, 5, w);
      mvprintw(3, 5, "Paused!");
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
    else if(c == 'p' && Timer.getState() != PomodoroTimer::State::PAUSE)
    {
      oldState = Timer.getState();
      Timer.Pause();
      PomoThread.join();
      PomoThread = std::thread(&PomodoroTimer::RunPause, std::ref(Timer), 999999999);
      Timer.Unpause();
      EraseStateChangeRepaint();
    }
    else if(c == 'p' && Timer.getState() == PomodoroTimer::State::PAUSE)
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
