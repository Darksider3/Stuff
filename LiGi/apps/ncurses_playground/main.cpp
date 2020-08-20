#include <ncurses.h>
#include <iostream>
#include <list>

#define MIDDLE_X(WIDTH) (COLS-WIDTH)/2

WINDOW *mainw, *topsubw, *midsubw;
size_t x, y;
void windowInitError(std::string &&name)
{
  std::cerr << "Error: Couldn't initialise Window" << name << "!" << std::endl;
  exit(ERR);
}


void exit_func()
{
  delwin(mainw);
  delwin(topsubw);
  delwin(midsubw);
  endwin();
}
void init_ncurses()
{
  if( (mainw = initscr()) == nullptr)
  {
    windowInitError("main screen");
  }
  getmaxyx(mainw, y, x);
  if( (topsubw = newwin(4,COLS,0,0)) == nullptr)
  {
    windowInitError("Top");
  }
  if((midsubw = newwin(5, 15, 20, 20)) == nullptr)
  {
    windowInitError("Midsub");
  }

  atexit(exit_func);
  if(start_color() == ERR || !has_colors() || !can_change_color())
  {
    std::cerr << "Couldn't use coloring!" << std::endl;
    exit_func();
    exit(ERR);
  }
  cbreak();
  noecho();
  curs_set(0);

}

void createBoxesAround(WINDOW *ins...)
{
  WINDOW *cur;
  va_list args;
  va_start(args, ins);
  for(cur = va_arg(args, WINDOW*); cur != nullptr; cur = va_arg(args, WINDOW*))
  {
    box(cur, 0,0 );
    mvwprintw(cur, 0,0, "THIS WINDOW");
    wrefresh(cur);
  }

  va_end(args);
}


void createBoxesAround(const std::list<WINDOW *> && myArgs)
{
  for(auto &win: myArgs)
  {
    box(win, 0,0);
    wrefresh(win);
  }
}

int main(int argc, char** argv)
{
  init_ncurses();
  //box(mainw, 0,0);
  mvwprintw(topsubw, 1, 1 /* Middle */ ,"Main window");
  wrefresh(topsubw);
  mvwhline(mainw, 2, 1, chtype{'='}, COLS-1 /* border */);
  wrefresh(topsubw);
  wrefresh(mainw);
  refresh();

  //box(topsubw, 0, 0);
  //box(midsubw,0,0);
  mvwprintw(midsubw, 1, 1, "subwindow");


//  wrefresh(mainw);
//  wrefresh(midsubw);
//  wrefresh(topsubw);
  createBoxesAround({mainw, topsubw, midsubw});

  std::string changedOhNoeExclamationMark = "Oh noe! Something changed!";

  getch();

  werase(mainw);
  mvwprintw(mainw, 1,(COLS-changedOhNoeExclamationMark.length()) / 2, changedOhNoeExclamationMark.c_str());
  mvwhline(mainw,2,1, chtype{'='}, COLS-1);
  box(mainw, 0,0);
  wrefresh(mainw);
  refresh();

  getch();

  delwin(topsubw);
  delwin(midsubw);
  delwin(mainw);

  endwin();
  return 0;
}
