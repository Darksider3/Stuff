#ifndef TOOLS_H
#define TOOLS_H
#include <cstddef>

namespace TimerApp
{

int xMiddle(size_t const &full, size_t const &sub) noexcept
{
  size_t x = (full - sub) / 2;
  return static_cast<int>(x);
}

}
#endif // TOOLS_H
