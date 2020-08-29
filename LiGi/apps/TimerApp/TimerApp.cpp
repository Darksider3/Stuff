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


#include "timer.h"
#include "TimerTools.h"
#include "stack.h"
#include <ncurses.h>
#include <deque>
#include <functional>
#include <locale.h>
#include <signal.h>
#include <cstring> // memset...

#include "ApplicationDefaults.h"
#include "Tools.h"
//@TODO: In case <semaphore> ever get's released, use it for the signal handler FFS!
std::atomic_bool _INTERRUPTED_ = false; // Signal Interrupt handle
WINDOW *FullWin; // full window; used to get dimensions
WINDOW *TopPanel; // Subwindow in FullWin, used for the top bar
WINDOW *MidWin; // also in the FullWindow, used for status and timer itself
WINDOW *ShortcutWin; // and again in FullWindow, here for visibility of shortcuts

int FULL_COORD_X, FULL_COORD_Y;

enum PomoState
{
  POMODORO,
  SHORT,
  LONG,
  PAUSE,
  STOP
};

/*
struct PomoStatistics
{
using N_I = uint16_t;
  N_I ShortBreaks;
  N_I LongBreaks;
  N_I Pomodoros;
  N_I TotalWorkingTime; // it's not deducable, because we're going to add *any* time here we've worked on pomodoros
  N_I TotalShortBreakTime;
  N_I TotalLongBreakTime;
  N_I TotalPauseTime; // Logic to stop instead of default pause;
  N_I TotalStopTime; // Difference to Pause is that we stopped instead of paused, @todo logic for that
};*/

class PomodoroTimer : public Li::Timer<PomodoroTimer, uint64_t>
{
private:

  void run(Li::Literals::TimeValue auto Goal) noexcept
  {
    this->setGoal(Goal);
    this->ResetTime();
    this->RunTimer();
  }
public:
  class View
  {
  private:
  public:
    static void printModeView(const PomoState& state, WINDOW *win) noexcept
    {
      using namespace TimerApp;
      int midy, midx;
      getmaxyx(win, midy, midx);
      midy = midy /2;
      if(state == PomoState::SHORT)
        mvaddstr(midy+2, xMiddle(midx, 14), "Taking a break");
      else if(state == PomoState::LONG)
        mvaddstr(midy+2, xMiddle(midx, 18), "Taking a big break!");
      else if(state == PomoState::POMODORO)
        mvaddstr(midy+2, xMiddle(midx, 22), "Working on a Pomodoro!");
      else if(state  == PomoState::PAUSE)
        mvaddstr(midy+2, xMiddle(midx, 22), "Taking a manual pause!");
      else if(state == PomoState::STOP)
        mvaddstr(midy+2, xMiddle(midx, 25), "Waiting for input what to run!!");
    }
  };

  std::atomic<PomoState> m_state;
  std::atomic<PomoState> m_oldState;
  using Timer<PomodoroTimer>::Timer;

  explicit PomodoroTimer(std::atomic_bool &stopper) : Li::Timer<PomodoroTimer, uint64_t>(stopper, POMODORO_TIME)
  {
    this->setDelay(500);
    this->setSleep(125);
  }


  void RunPomo(uint64_t Goal = POMODORO_TIME) noexcept
  {
    this->m_state = PomoState::POMODORO;
    this->run(Goal);
    this->m_oldState.store(this->m_state);
    this->m_state = PomoState::STOP;
  }

  void RunShortBreak(uint64_t Goal = SHORT_BREAK_TIME) noexcept
  {
    this->m_state = PomoState::SHORT;
    this->run(Goal);
    this->m_oldState.store(this->m_state);
    this->m_state = PomoState::STOP;
  }

  void RunBigBreak(uint64_t Goal = BIG_BREAK_TIME) noexcept
  {
    this->m_state = PomoState::LONG;
    this->run(Goal);
    this->m_oldState.store(this->m_state);
    this->m_state = PomoState::STOP;
  }

  void RunPause(uint64_t Goal = PAUSE_STOP_VAL) noexcept
  {
    uint64_t oldTimeLeft = this->getTimeLeft();
    this->m_state = PomoState::PAUSE;
    this->run(Goal);
    this->m_state.store(this->m_oldState);
    this->setTimeLeft(oldTimeLeft);
  }

  void RunStop(uint64_t Goal = PAUSE_STOP_VAL)
  {
    this->m_oldState.store(this->m_state);
    this->m_state = PomoState::STOP;
    this->run(Goal);
  }

  PomoState getState() const noexcept { return this->m_state.load();}

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

void quitter()
{
  delwin(FullWin);
  delwin(MidWin);
  delwin(TopPanel);
  delwin(ShortcutWin);
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
  if((FullWin = initscr()) == nullptr)
  {
    std::cerr << "Error! Couldn't initialise ncurses!" << std::endl;
    quitter();
  }
  atexit(quitter);

  getmaxyx(FullWin, FULL_COORD_Y, FULL_COORD_X);

  if(FULL_COORD_Y < MIN_Y || FULL_COORD_X < MIN_X)
  {
    std::cerr << "Sorry, but your window isnt big enough!" << std::endl;
    exit(ERR);
  }

  if((TopPanel = newwin(3, FULL_COORD_X, 0, 0)) == nullptr)
  {
    std::cerr << "Error! Couldn't intiialise top panel!"  << std::endl;
    exit(ERR);
  }
  if((MidWin = newwin(FULL_COORD_Y-13, FULL_COORD_X, 3, 1)) == nullptr)
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
  nodelay(FullWin, true);
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
  mvwaddstr(ShortcutWin, 1, 2, "-> (C)lose");
  mvwaddstr(ShortcutWin, 2, 2, "-> (B)reak");
  mvwaddstr(ShortcutWin, 3, 2, "-> (P)ause/Un(P)ause");
  mvwaddstr(ShortcutWin, 4, 2, "-> P(O)modoro");
  mvwaddstr(ShortcutWin, 5, 2, "-> (L)ong Break");
  wrefresh(ShortcutWin);
}

void ViewMode(PomoState const &state, WINDOW *win)
{
  int midy, midx;
  using namespace TimerApp;
  getmaxyx(win, midy, midx);
  midy = midy / 2;
  switch(state)
  {
    case PomoState::SHORT:
      mvaddstr(midy+2, xMiddle(midx, 14), "Taking a break");
      break;
    case PomoState::LONG:
      mvaddstr(midy+2, xMiddle(midx, 18), "Taking a big break!");
      break;
    case PomoState::POMODORO:
      mvaddstr(midy+2, xMiddle(midx, 22), "Working on a Pomodoro!");
      break;
    case PomoState::PAUSE:
      mvaddstr(midy+2, xMiddle(midx, 22), "Taking a manual pause!");
      break;
    case PomoState::STOP:
      mvaddstr(midy+2, xMiddle(midx, 25), "Waiting for input what to run!!");
      break;
  }
}

void winch_handle(int sig)
{
  if(sig == SIGWINCH)
    _INTERRUPTED_ = true;
}

int dummy(int bla)
{
  std::cout << bla;
  return bla;
}

int main()
{
  setlocale(LC_ALL, "");

  auto interrupt_handle = [&]() {
      endwin();
      init();
      refresh();
      clear();
      getmaxyx(FullWin, FULL_COORD_Y, FULL_COORD_X);
      refresh();
      _INTERRUPTED_ = false;
  };

  auto set_white = [&]() {
    color_set(1,0);
  };
  auto set_red = [&]() {
    color_set(2, 0);
  };

  auto EraseStateChangeRepaint = [&](){
    werase(MidWin);
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

  auto StateChange = [&](std::thread &ThreadObj, PomodoroTimer &PomObj, std::function<void()> runFunc)
  {
    PomObj.Pause();
    ThreadObj.join();
    ThreadObj = std::thread(runFunc);
    PomObj.Unpause();
    EraseStateChangeRepaint();
  };

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
  PomodoroTimer Timer{stop};

  std::thread PomoThread(&PomodoroTimer::RunPomo, std::ref(Timer), POMODORO_TIME);
  InitialPaint();
  while(true)
  {
    using namespace TimerApp;
    int c = wgetch(stdscr);
    set_red();
        int midx, midy;
        getmaxyx(MidWin, midy, midx);
        midy = midy / 2;
    switch(Timer.getState())
    {
      case(PomoState::PAUSE):
        EraseSpecificLine(MidWin, midy, xMiddle(midx, 7));
        mvwprintw(MidWin, midy, xMiddle(midx, 6), "Paused!");
        wrefresh(MidWin);
        break;
      case(PomoState::STOP):
        EraseSpecificLine(MidWin, midy, xMiddle(midx, 7));
        mvwprintw(MidWin, midy, xMiddle(midx, 7), "STOPPED!");
        wrefresh(MidWin);
        break;
      case(PomoState::LONG):
      case(PomoState::SHORT):
      case(PomoState::POMODORO):
        wcolor_set(MidWin, 2, 0);
        EraseSpecificLine(FullWin, midy, xMiddle(midx, 10));
        mvwprintw(MidWin, midy, xMiddle(midx, 8), Li::TimerTools::Format::getFullTimeString(
                 Timer.getTimeLeft()).c_str());
        wrefresh(MidWin);
        break;
      //default:
      //  break;
    }

    set_white();
    ViewRunningMenue();
    ViewTitleBar();
    PomodoroTimer::View::printModeView(Timer.getState(), MidWin);
    refresh();
    if(c == 'c')
    {
      Timer.Pause();
      PomoThread.join();
      quitter();
      return(EXIT_SUCCESS);
    }
    else if(c == 'p' && Timer.getState() != PomoState::PAUSE)
    {
      StateChange(PomoThread, Timer, std::bind(&PomodoroTimer::RunPause, std::ref(Timer), PAUSE_STOP_VAL));
    }
    else if(c == 'p' && Timer.getState() == PomoState::PAUSE)
    {
      StateChange(PomoThread, Timer, std::bind(&PomodoroTimer::Resume, std::ref(Timer)));
    }
    else if(c == 'o')
    {
        StateChange(PomoThread, Timer, std::bind(&PomodoroTimer::RunPomo, std::ref(Timer), POMODORO_TIME));
    }
    else if(c == 'b')
    {
        StateChange(PomoThread, Timer, std::bind(&PomodoroTimer::RunShortBreak, std::ref(Timer), SHORT_BREAK_TIME));
    }
    else if(c == 'l')
    {
        StateChange(PomoThread, Timer, std::bind(&PomodoroTimer::RunBigBreak, std::ref(Timer), BIG_BREAK_TIME));
    }
    else if(Timer.getState() == PomoState::STOP)
    {
      StateChange(PomoThread, Timer, std::bind(&PomodoroTimer::RunStop, std::ref(Timer), PAUSE_STOP_VAL));
    }
    else if(c == KEY_RESIZE)
    {
      // we have to this actually, because the signal itself isn't really portable D:
      winch_handle(SIGWINCH);
    }

    if(_INTERRUPTED_)
    {
      interrupt_handle();
    }
    else{
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  return(0);
}
