#include "../timer.h"
#include <ncurses.h>


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
      mvprintw(3, 12, "GOT NOTHING");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return(0);
}
