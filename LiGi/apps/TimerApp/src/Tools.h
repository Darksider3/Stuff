#ifndef TOOLS_H
#define TOOLS_H
#include <cstddef>
#include <ncurses.h>

namespace TimerApp {

size_t xMiddle(size_t const& full, size_t const& sub) noexcept
{
    size_t x = (full - sub) / 2;
	return x;
}

void EraseSpecific(WINDOW* win, size_t Y, size_t X)
{
    int Old_y, Old_x;
    getyx(win, Old_y, Old_x);
    wmove(win, static_cast<int>(Y), static_cast<int>(X));
    wclrtoeol(win);
    wmove(win, Old_y, Old_x);
    wrefresh(win);
}

}
#endif // TOOLS_H
