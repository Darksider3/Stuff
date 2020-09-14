#ifndef THREADSAFEPRIMITIVE_H
#define THREADSAFEPRIMITIVE_H
#include <mutex>
#include <functional>


template<typename T>
class LockingPrimivite
{
public:
};


/**
 * @brief ThreadsafePrimitive is a class for simple one-way operations. In case you ever want to do something more useful then just adding or substracting in one step, it's going to be more efficient to just lock the variable yourself or inherit and add your operation
 */
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

  ThreadsafePrimitive& operator++()
  {
    std::scoped_lock L(M_RW_Lock);
    ++val;
    return *this;
  }

  ThreadsafePrimitive operator++(int)
  {
    ThreadsafePrimitive<T> result(*this);
    operator++();
    return result;
  }

  void operator*(T const x)
  {
    std::scoped_lock L(M_RW_Lock);
    return val * x;
  }

  void operator=(T const x)
  {
    std::scoped_lock L(M_RW_Lock);
    val=x;
  }

  T operator+(T const x)
  {
    std::scoped_lock L(M_RW_Lock);
    return val + x;
  }

  T operator-(T const x)
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
