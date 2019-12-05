#pragma once
#include <cstdlib>
#include <cstdio>
#ifdef DEBUG
#undef assert
__attribute__((noreturn)) void __assertion_failed(const char* msg, const char* file, unsigned line, const char* func);
#    define assert(expr) ((expr) ? (void)0 : __assertion_failed(#    expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))
#    define msgassert(expr, msg) ((expr) ? (void)0 : __assertion_failed(msg, __FILE__, __LINE__, __PRETTY_FUNCTION__))
#    define ASSERT_NOT_REACHED() assert(false)
#else
#    define owassert(expr) void()
#    define msgassert(expr, msg) void()
#    define ASSERT_NOT_REACHED() void()
#endif
__attribute__((noreturn)) void __assertion_failed(const char* msg, const char* file, unsigned line, const char* func)
{
  std::printf("%s:%s:%d; %s\n", file, func, line, msg);
  std::fflush(stdout); // 0 => stdout
  std::abort();
}
