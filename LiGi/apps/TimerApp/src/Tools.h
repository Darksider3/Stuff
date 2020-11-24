#ifndef TOOLS_H
#define TOOLS_H
#include "LiGi/concepts_additions.h"
#include <concepts>
#include <cstddef>
#include <ncurses.h>

namespace TimerApp {

template<concepts::Numeric num, concepts::Numeric To>
To xMiddle(num const& full, num const& sub) noexcept
{
	return static_cast<To>((full - sub) / 2);
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
