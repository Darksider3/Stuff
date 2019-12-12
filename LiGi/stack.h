#ifndef STACK_H
#define STACK_H
#include "LinkedList.h"

namespace Li {

template<typename T>
class Stack : private LinkedList<T>
{
public:
  Stack();
  bool empty();
  T& top();
  const T& top() const;
  void push(T* Node);
  void pop();
};

}
#endif // STACK_H
