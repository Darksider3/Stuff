#include "Assertions.h"
__attribute__((noreturn)) void __assertion_failed(const char* msg, const char* file, unsigned line, const char* func)
{
  printf("%s:%s:%d; %s\n", file, func, line, msg);
  std::fflush(stdout); // 0 => stdout
  std::abort();
}
