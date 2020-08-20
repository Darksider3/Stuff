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
#include <locale.h>
#include <signal.h>
#include <cstring>
#include "../stack.h"


//@TODO: In case <semaphore> ever get's released, use it for the signal handler FFS!
std::mutex ResizeMutex;


constexpr short MIN_X = 15;
constexpr short MIN_Y = 15;
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

class PomodoroTimer : public Li::Timer<PomodoroTimer, uint64_t>
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
    uint64_t oldTimeLeft = this->getTimeLeft();
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
WINDOW *TopPanel;
WINDOW *MidWin;
WINDOW *ShortcutWin;

int Fullx, Fully;

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
  atexit(quitter);

#define MIDDLE_X(WIDTH) (Fullx-WIDTH)/2
#define MIDDLE_Y() (Fully)/2
  getmaxyx(w, Fully, Fullx);

  if(Fully < MIN_Y || Fullx < MIN_X)
  {
    std::cerr << "Sorry, but your window isnt big enough!" << std::endl;
    exit(ERR);
  }

  if((TopPanel = newwin(3, COLS, 0, 0)) == nullptr)
  {
    std::cerr << "Error! Couldn't intiialise top panel!"  << std::endl;
    exit(ERR);
  }
  if((MidWin = newwin(Fully, Fullx, 1, 1)) == nullptr)
  {
    std::cerr << "Error! Couldn't initialise mid window!" << std::endl;
    exit(ERR);
  }

  if((ShortcutWin = newwin(10, COLS/2, LINES-10, 0)) == nullptr)
  {
    std::cerr << "Error! Couldn't initialise shortcut window!" << std::endl;
  }
  cbreak();
  noecho();
  nodelay(w, true);
  curs_set(0);
}

void init()
{
  init_windows();
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
  mvwaddstr(TopPanel, 1, 1, "(E)dit settings");
  //for(size_t i = COLS; i != 0; --i)
  //  mvwadd_wch(TopPanel, 2, i, WACS_T_HLINE);
  mvwhline(TopPanel, 2, 0, ACS_HLINE, COLS);
  wrefresh(TopPanel);
}

void ViewRunningMenue()
{
  std::string title = "Shortcuts";
  box(ShortcutWin, 0, 0);
  int win_x;

  win_x = getmaxx(ShortcutWin);

  mvwaddstr(ShortcutWin, 0, (win_x/2)-(title.length()-1), title.c_str());
  mvwaddstr(ShortcutWin, 1, 2, "Press q to exit");
  mvwaddstr(ShortcutWin, 2, 2, "Press b to take a break");
  wrefresh(ShortcutWin);
}

void ViewMode(PomoState const &state)
{
  int midy = MIDDLE_Y()+2;
  if(state == PomoState::SHORT)
    mvaddstr(midy, MIDDLE_X(14), "Taking a break");
  else if(state == PomoState::LONG)
    mvaddstr(midy, MIDDLE_X(18), "Taking a big break!");
  else if(state == PomoState::POMODORO)
    mvaddstr(midy, MIDDLE_X(22), "Working on a Pomodoro!");
  else if(state  == PomoState::PAUSE)
    mvaddstr(midy, MIDDLE_X(22), "Taking a manual pause!");
  else if(state == PomoState::STOP)
    mvaddstr(midy, MIDDLE_X(25), "Waiting for input what to run!!");

}

void winch_handle(int sig)
{
  std::scoped_lock<std::mutex> ResizeGuard(ResizeMutex);
  endwin();
  init();
  refresh();
  clear();
  getmaxyx(w, Fully, Fullx);
  ViewTitleBar();
  ViewRunningMenue();
  refresh();
}

int main()
{
  setlocale(LC_ALL, "");

  auto set_white = [&]() {
    color_set(1,0);
  };
  auto set_red = [&]() {
    color_set(2, 0);
  };

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
  // TEST
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

  std::atomic_bool stop = false;
  init();

  /**
   * Here lies dragons - actually handling WINCH/Resizing without segfaults. o.o
   */
  struct sigaction sa;
  std::memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = winch_handle;
  sigaction(SIGWINCH, &sa, NULL);

  // Timer - main functionality
  PomodoroTimer Timer{stop, POMODORO_TIME};

  std::thread PomoThread(&PomodoroTimer::RunPomo, &Timer, POMODORO_TIME);
  InitialPaint();
  PomoState oldState;
  while(true)
  {
    int c = wgetch(stdscr);
    set_red();
    switch(Timer.getState())
    {
      case(PomoState::PAUSE):
        EraseSpecificLine(w, MIDDLE_Y(), MIDDLE_X(7));
        mvprintw(MIDDLE_Y(), MIDDLE_X(6), "Paused!");
        break;
      case(PomoState::STOP):
        EraseSpecificLine(w, MIDDLE_Y(), MIDDLE_X(7));
        mvprintw(MIDDLE_Y(), MIDDLE_X(7), "STOPPED!");
        break;
      case(PomoState::LONG):
      case(PomoState::SHORT):
      case(PomoState::POMODORO):
        EraseSpecificLine(w, MIDDLE_Y(), MIDDLE_X(10));
        mvprintw(MIDDLE_Y(), MIDDLE_X(8), Li::TimerTools::Format::getFullTimeString(
                 Timer.getTimeLeft()).c_str());
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
    else if(c == KEY_RESIZE)
    {
      // we have to this actually, because the signal itself isn't really portable D:
      winch_handle(SIGWINCH);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return(0);
}
