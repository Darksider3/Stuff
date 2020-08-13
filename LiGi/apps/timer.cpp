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
  using Timer<PomodoroTimer>::Timer;
  PomodoroTimer(std::atomic_bool &stop, const Li::Literals::TimeValue auto &Goal) : Timer<PomodoroTimer>(stop, Goal)
  {
    this->setDelay(100);
    this->setSleep(50);
  }

  void RunPomo(Li::Literals::TimeValue auto const Goal = POMODORO_TIME)
  {
    this->run(Goal);
  }

  void RunShortBreak(Li::Literals::TimeValue auto const Goal = SHORT_BREAK_TIME)
  {
    this->run(Goal);
  }

  void RunBigBreak(Li::Literals::TimeValue auto const Goal = BIG_BREAK_TIME)
  {
    this->run(Goal);
  }

  const std::string getTimeStr() const noexcept
  {
    namespace Format = Li::TimerTools::Format;
    Li::Literals::TimeValue  auto t = this->getTimeLeft();
    return Format::getMinutes(t)+":"+Format::getSeconds(t);
  }
};

WINDOW *w;

void dummyFunc(uint64_t const &tt, Li::STATE const &state)
{
  return;
}

void quitter()
{
  endwin();
}

void init()
{
  w = initscr();
  cbreak();
  noecho();
  nodelay(w, true);
  atexit(quitter);
  curs_set(0);
}

void ViewRunningMenue()
{
  mvaddstr(11, 2, "Press q to exit");
  mvaddstr(10, 2, "Press b to take a break");
}

void ViewMode(Li::STATE const &state)
{
  using Li::STATE;

  if(state.mode == STATE::BREAK)
    mvaddstr(5, 1, "Taking a break");
  else if(state.mode == STATE::BIGBREAK)
    mvaddstr(5, 1, "Taking a big break!");
  else if(state.mode == STATE::POMO)
    mvaddstr(5, 1, "Working on a Pomodoro!");
  else if(state.mode == STATE::PAUSE)
    mvaddstr(5, 1, "Taking a manual pause!");

}

int main()
{
#ifdef TEST_NEW_TIMER
  // TEST
  std::atomic_bool stop = false;
  PomodoroTimer bla(stop, 1000*3);
  bla.RunTimer();
  bla.Pause();
  std::cout << "PAUSED; CONTINUE" << std::endl;

  std::string testans = "";

  std::cin >> testans;
  std::cout << "RUNNING RESUME" << std::endl;
  bla.setTimeLeft(1000*1);
  std::cout << bla.getTimeStr() << "\n";
  bla.Resume();
  bla.Pause();
  std::cout << "DONE RESUME&PAUSE; CONTINUE" << std::endl;

  std::cin >> testans;

  // \TEST
#endif
  init();

  //int x, y;
  Li::STATE State;

  State.pomodoro_time = 1000 * 60 * 30;
  State.short_break_time = 1000 * 60 * 6;
  State.big_break_time = 1000 * 60 * 18;

  Li::Pomodoro Timer(State);

  std::thread PomoThread(&Li::Pomodoro::RunPomo, &Timer, dummyFunc, Li::STATE::POMO);

  int c;
  while(true)
  {
    c = getch();
      mvprintw(2, 5, Li::TimerTools::Format::getFullTimeString(State.elapsed).c_str());
    ViewRunningMenue();
    ViewMode(State);
    refresh();
    if(c == 'q')
    {
      Timer.stop = true;
      PomoThread.join();
      quitter();
      return(0);
    }
    else if(c == 'p' && !Timer.isPaused())
    {
      Timer.stop = true;
      PomoThread.join();
      PomoThread = std::thread(&Li::Pomodoro::RunPomo, &Timer, dummyFunc, Li::STATE::PAUSE);
      erase();
      ViewRunningMenue();
      ViewMode(State);
      refresh();
    }
    else if(c == 'p' && Timer.isPaused())
    {
      Timer.stop = true;
      PomoThread.join();
      PomoThread = std::thread(&Li::Pomodoro::RunPomo, &Timer, dummyFunc, Li::STATE::RESUME);
    }
    else if(c == 'o')
    {
      Timer.stop = true;
      PomoThread.join();
      PomoThread = std::thread(&Li::Pomodoro::RunPomo, &Timer, dummyFunc, Li::STATE::POMO);
      erase();
    }
    else if(c == 'b')
    {
      Timer.stop = true;
      PomoThread.join();
      PomoThread = std::thread(&Li::Pomodoro::RunPomo, &Timer, dummyFunc, Li::STATE::BREAK);
      erase();
    }
    else if(c == 'g')
    {
      Timer.stop = true;
      PomoThread.join();
      PomoThread = std::thread(&Li::Pomodoro::RunPomo, &Timer, dummyFunc, Li::STATE::BIGBREAK);
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
