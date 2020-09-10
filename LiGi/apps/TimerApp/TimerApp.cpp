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


#include <ncurses.h>
#include <deque>
#include <functional>
#include <locale.h>
#include <signal.h>
#include <cstring> // memset...

#include "timer.h"
#include "TimerTools.h"
#include "stack.h"

#include "ApplicationDefaults.h"
#include "Tools.h"

#include "SingleThreadLoop.h"

//@TODO: In case <semaphore> ever get's released, use it for the signal handler FFS!
std::atomic_bool _INTERRUPTED_ = false; // Signal Interrupt handle
WINDOW *FullWin; // full window; used to get dimensions
WINDOW *TopPanel; // Subwindow in FullWin, used for the top bar
WINDOW *MidWin; // also in the FullWindow, used for status and timer itself
WINDOW *ShortcutWin; // and again in FullWindow, here for visibility of shortcuts
WINDOW *StatisticsWin;


int FULL_COORD_X, FULL_COORD_Y;

enum PomoState
{
  POMODORO,
  SHORT,
  LONG,
  PAUSE,
  STOP
};


struct PomoStatistics
{
  using N_I = uint16_t;
  N_I ShortBreaks = 0;
  N_I LongBreaks = 0;
  N_I Pomodoros = 0;
  uint64_t TotalWorkingTime = 0; // it's not deducable, because we're going to add *any* time here we've worked on pomodoros
  uint64_t TotalShortBreakTime = 0;
  uint64_t TotalLongBreakTime = 0;
  uint64_t TotalPauseTime = 0; // Logic to stop instead of default pause;
  uint64_t TotalStopTime = 0; // Difference to Pause is that we stopped instead of paused, @todo logic for that
};

class PomodoroTimer : public Li::Timer<PomodoroTimer, uint64_t>
{
private:

  uint64_t M_pauseLeft = 0;

  void run(Li::Literals::TimeValue auto Goal) noexcept
  {
    this->setGoal(Goal);
    this->ResetTime();
    this->RunTimer();
    return;
  }
public:
  class View
  {
  private:
    PomodoroTimer const &M_Timer;
    PomoStatistics M_Stats;

  public:

    explicit View(PomodoroTimer const &timer) : M_Timer(timer) {}

    static void set_red(WINDOW &win)
    {
      wcolor_set(&win, 2, 0);
      return;
    }

    static void set_white(WINDOW &win)
    {
      wcolor_set(&win, 1, 0);
      return;
    }

    void Mode(WINDOW &win) const noexcept
    {
      const PomoState &state = M_Timer.getState();
      int midy, midx;
      getmaxyx(&win, midy, midx);
      midy = midy /2;
      if(state == PomoState::SHORT)
        mvwaddstr(&win, midy+2, TimerApp::xMiddle(midx, 14), "Taking a break");
      else if(state == PomoState::LONG)
        mvwaddstr(&win, midy+2, TimerApp::xMiddle(midx, 18), "Taking a big break!");
      else if(state == PomoState::POMODORO)
        mvwaddstr(&win, midy+2, TimerApp::xMiddle(midx, 22), "Working on a Pomodoro!");
      else if(state  == PomoState::PAUSE)
        mvwaddstr(&win, midy+2, TimerApp::xMiddle(midx, 22), "Taking a manual pause!");
      else if(state == PomoState::STOP)
        mvwaddstr(&win, midy+2, TimerApp::xMiddle(midx, 25), "Waiting for input what to run!!");
      return;
    }

    void Mid(WINDOW &win) const noexcept
    {
      int midx, midy;
      getmaxyx(&win, midy, midx);
      midy = midy / 2;

      set_red(win);
      switch(M_Timer.getState())
      {
        case(PomoState::PAUSE):
          TimerApp::EraseSpecific(&win, midy, TimerApp::xMiddle(midx, 7));
          mvwprintw(&win, midy, TimerApp::xMiddle(midx, 6), "Paused!");
          wrefresh(&win);
          break;
        case(PomoState::STOP):
          TimerApp::EraseSpecific(&win, midy, TimerApp::xMiddle(midx, 7));
          mvwprintw(&win, midy, TimerApp::xMiddle(midx, 7), "STOPPED!");
          wrefresh(&win);
          break;
        case(PomoState::LONG):
        case(PomoState::SHORT):
        case(PomoState::POMODORO):
          TimerApp::EraseSpecific(&win, midy, TimerApp::xMiddle(midx, 10));
          mvwprintw(&win, midy, TimerApp::xMiddle(midx, 8),
                    Li::TimerTools::Format::getFullTimeString(
                      M_Timer.getTimeLeft()).c_str());
          wrefresh(&win);
          break;
          //default:
          //  break;
      }
      set_white(win);
      return;
    }

    void Shortcut(WINDOW &win) const noexcept
    {

      std::string title = "Shortcuts";
      box(&win, 0, 0);
      int win_x;

      win_x = getmaxx(&win);

      mvwaddstr(&win, 0, static_cast<int>((win_x-(title.length()-1))/2), title.c_str());
      mvwaddstr(&win, 1, 2, "-> (C)lose");
      mvwaddstr(&win, 2, 2, "-> (B)reak");
      mvwaddstr(&win, 3, 2, "-> (P)ause/Un(P)ause");
      mvwaddstr(&win, 4, 2, "-> P(O)modoro");
      mvwaddstr(&win, 5, 2, "-> (L)ong Break");
      wrefresh(&win);
      return;
    }

    void Statistcs(WINDOW &win) const noexcept
    {
      std::string title = "Statistics";
      // TODO: Long "B's"
      // TODO: Times total run in those modes
      // TOOD: For the statistics themselfes, just support
      std::string pomo =    "-> Pomodoros: " + std::to_string(M_Stats.Pomodoros);
      std::string sbreaks = "-> Short B's: " + std::to_string(M_Stats.ShortBreaks);
      int win_x = getmaxx(&win);

      box(&win, 0, 0);
      mvwaddstr(&win, 0, static_cast<int>((win_x-(title.length()-1))/2), title.c_str());
      mvwaddstr(&win, 1, 1, pomo.c_str());
      mvwaddstr(&win, 2, 1, sbreaks.c_str());
      wrefresh(&win);
      return;
    }

    void TitleBar(WINDOW &win) const noexcept
    {
      mvwaddstr(&win, 1, 1, "(E)dit settings");
      mvwhline(&win, 2, 0, ACS_HLINE, COLS);
      wrefresh(&win);
      return;
    }
  };

  friend View;

  std::atomic<PomoState> M_state;
  std::atomic<PomoState> M_oldState;

  PomoStatistics M_Stats;

  using Timer<PomodoroTimer>::Timer;


  explicit PomodoroTimer(std::atomic_bool &stopper) : Li::Timer<PomodoroTimer, uint64_t>(stopper, POMODORO_TIME)
  {
    this->setDelay(50);
    this->setSleep(10);
  }

  void RunResume() noexcept
  {
    if(M_pauseLeft == 0)
    {
      RunStop();
      return;
    }

    M_state.store(M_oldState);
    setTimeLeft(M_pauseLeft);
    Resume(); // utilize parent!
    M_oldState.store(M_state);
    M_state.store(PomoState::STOP);
    return;
  }

  void RunPomo(uint64_t Goal = POMODORO_TIME) noexcept
  {
    M_state.store(PomoState::POMODORO);
    run(Goal);
    M_oldState.store(M_state);
    M_state.store(PomoState::STOP);
    return;
  }

  void RunShortBreak(uint64_t Goal = SHORT_BREAK_TIME) noexcept
  {
    M_state = PomoState::SHORT;
    run(Goal);
    M_oldState.store(M_state);
    M_state = PomoState::STOP;
    return;
  }

  void RunBigBreak(uint64_t Goal = BIG_BREAK_TIME) noexcept
  {
    M_state = PomoState::LONG;
    run(Goal);
    M_oldState.store(M_state);
    M_state = PomoState::STOP;
    return;
  }

  void RunPause(uint64_t Goal = PAUSE_STOP_VAL) noexcept
  {
    uint64_t oldTimeLeft = this->getTimeLeft();
    M_state = PomoState::PAUSE;
    run(Goal);
    M_state.store(M_oldState);
    M_pauseLeft = oldTimeLeft;
    return;
  }

  void RunStop(uint64_t Goal = PAUSE_STOP_VAL)
  {
    M_oldState.store(M_state);
    M_state = PomoState::STOP;
    run(Goal);
    return;
  }

  PomoState getState() const noexcept { return M_state.load();}

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

  if((StatisticsWin = newwin(10, COLS/2, LINES-10, COLS/2)) == nullptr)
  {
    std::cerr << "Error! Couldn't initialise statistics window!" << std::endl;
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
|    -> MODE(Pause, Long, Short, Pomo)    |
|                 TIMER                   |
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
  /**
   * Here lies dragons - actually handling WINCH/Resizing without segfaults. o.o
   */

  setlocale(LC_ALL, "");
  struct sigaction sa;
  std::memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = winch_handle;
  sigaction(SIGWINCH, &sa, NULL);

  std::atomic_bool stop = false;
  std::atomic_bool globalstop = false;
  init();

  // Timer - main functionality
  PomodoroTimer Timer{stop};
  PomodoroTimer::View AppView(Timer);

  SingleThreadLoop ThreadWrap(globalstop);

  auto interrupt_handle = [&]() {
    endwin();
    init();
    refresh();
    clear();
    getmaxyx(FullWin, FULL_COORD_Y, FULL_COORD_X);
    refresh();
    _INTERRUPTED_ = false;
  };

  auto EraseStateChangeRepaint = [&](){
    werase(MidWin);
    werase(ShortcutWin);
    werase(StatisticsWin);
    werase(TopPanel);
    AppView.Shortcut(*ShortcutWin);
    AppView.Statistcs(*StatisticsWin);
    AppView.TitleBar(*TopPanel);
  };

  auto StateChange = [&](PomodoroTimer &PomObj, std::function<void()> runFunc)
  {
    PomObj.Pause();
    App::delayedInsertion<std::chrono::milliseconds>(ThreadWrap, runFunc, 5);
    PomObj.Unpause();
    EraseStateChangeRepaint();
  };

  auto StateToStr = [](PomoState const &state){
    switch(state)
    {
      case PomoState::LONG:
        return " LONG BREAK";
      case PomoState::PAUSE:
        return " PAUSE";
      case PomoState::POMODORO:
        return " POMODORO";
      case PomoState::SHORT:
        return " SHORT BREAK";
      case PomoState::STOP:
        return " STOPPED";
      default:
        return " none";
    }
  };


  auto ThreadCopy = ThreadWrap.RunThread(App::ShareThread());


  ThreadWrap.insert(&PomodoroTimer::RunPomo, std::ref(Timer), POMODORO_TIME);
  while(true)
  {
    using namespace TimerApp;
    int c = wgetch(stdscr);
    AppView.Mid(*MidWin);
    AppView.Mode(*MidWin);
    AppView.Shortcut(*ShortcutWin);
    AppView.TitleBar(*TopPanel);
    AppView.Statistcs(*StatisticsWin);
#ifndef NDEBUG
    EraseSpecific(TopPanel, 0, COLS-30);
    std::string StateLine = "Time L: " + std::to_string(Timer.getTimeLeft());
    mvwaddstr(TopPanel, 0, COLS-40, StateLine.c_str());
    StateLine = "State: ";
    StateLine += StateToStr(Timer.getState());
    StateLine += " | Old: ";
    StateLine += StateToStr(Timer.M_oldState);
    mvwaddstr(TopPanel, 1, COLS-40, StateLine.c_str());
    wrefresh(TopPanel);
#endif
    refresh();
    if(c == 'c')
    {
      Timer.Pause();
      //PomoThread.join();
      quitter();
      return(EXIT_SUCCESS);
    }
    else if(c == 'p' && Timer.getState() != PomoState::PAUSE)
    {
      StateChange(Timer, std::bind(&PomodoroTimer::RunPause, std::ref(Timer), PAUSE_STOP_VAL));
    }
    else if(c == 'p' && Timer.getState() == PomoState::PAUSE)
    {
      StateChange(Timer, std::bind(&PomodoroTimer::RunResume, std::ref(Timer)));
    }
    else if(c == 'o')
    {
      StateChange(Timer, std::bind(&PomodoroTimer::RunPomo, std::ref(Timer), POMODORO_TIME));
    }
    else if(c == 'b')
    {
      StateChange(Timer, std::bind(&PomodoroTimer::RunShortBreak, std::ref(Timer), SHORT_BREAK_TIME));
    }
    else if(c == 'l')
    {
      StateChange(Timer, std::bind(&PomodoroTimer::RunBigBreak, std::ref(Timer), BIG_BREAK_TIME));
    }
    else if(Timer.getState() == PomoState::STOP && Timer.M_oldState != PomoState::STOP)
    {
      mvwaddstr(TopPanel, 0, COLS-20, "HERE");
      wrefresh(TopPanel);
      StateChange(Timer, std::bind(&PomodoroTimer::RunStop, std::ref(Timer), PAUSE_STOP_VAL));
    }
    else if(c == KEY_RESIZE)
    {
      // we have to do this actually, because the signal itself isn't really portable D:
      winch_handle(SIGWINCH);
    }

    if(_INTERRUPTED_)
    {
      interrupt_handle();
    }
    else{
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
  return(0);
}
