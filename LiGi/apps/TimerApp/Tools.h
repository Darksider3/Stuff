#ifndef TOOLS_H
#define TOOLS_H
#include <cstddef>
#include <ncurses.h>

namespace TimerApp {

int xMiddle(size_t const& full, size_t const& sub) noexcept
{
    size_t x = (full - sub) / 2;
    return static_cast<int>(x);
}

void EraseSpecific(WINDOW* win, int Y, int X)
{
    int Old_y, Old_x;
    getyx(win, Old_y, Old_x);
    wmove(win, Y, X);
    wclrtoeol(win);
    wmove(win, Old_y, Old_x);
    wrefresh(win);
}

}
#endif // TOOLS_H
