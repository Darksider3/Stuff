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
  mvaddstr(11, 2, "Press Q to exit");
}

int main()
{
  init();
  //int x, y;

  bool last_was_pause = false;
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
    if(Timer.isPaused())
    {
      PomoThread.join();
      std::string answer = "";
      if(last_was_pause)
      {
        //nothing
      }
    }
    else
    {
      mvprintw(2, 5, Li::TimerTools::Format::getFullTimeString(State.elapsed).c_str());
    }
    ViewRunningMenue();
    refresh();
    if(c == 'q')
    {
      quitter();
      Timer.stop = true;
      PomoThread.join();
      return(0);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  return(0);
}
