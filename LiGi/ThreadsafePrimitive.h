#ifndef THREADSAFEPRIMITIVE_H
#define THREADSAFEPRIMITIVE_H
#include <cinttypes>
#include <mutex>
template<typename T = uint64_t>
class ThreadsafePrimitive
{
public:

  explicit ThreadsafePrimitive(T x) : val(x){};

  ThreadsafePrimitive(const ThreadsafePrimitive &other) : val(other.val){}
  ThreadsafePrimitive(ThreadsafePrimitive &&other) : val(other.val) {}

  ThreadsafePrimitive &operator=(const ThreadsafePrimitive &other)
  {
    return *this = ThreadsafePrimitive(other);
  }

  ThreadsafePrimitive &operator=(ThreadsafePrimitive &&other) noexcept
  {
    std::swap(val, other.val);
    return *this;
  }

  void operator++()
  {
    std::scoped_lock L(M_RW_Lock);
    val++;
  }

  void operator=(T const x)
  {
    std::scoped_lock L(M_RW_Lock);
    val=x;
  }

  T operator+(T const x) const
  {
    std::scoped_lock L(M_RW_Lock);
    return val + x;
  }

  T operator-(T const x) const
  {
    std::scoped_lock L(M_RW_Lock);
    return val - x;
  }

  T get()
  {
    std::scoped_lock K(M_RW_Lock);
    return val;
  }

private:
  std::mutex M_RW_Lock;
  T val;

};



#endif // THREADSAFEPRIMITIVE_H
