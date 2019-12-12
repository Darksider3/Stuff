#ifndef STACK_H
#define STACK_H
#include "LinkedList.h"

namespace Li {

template<typename T>
class Stack : private LinkedList<T>
{
public:
  bool empty();
  T* top();
  const T* top() const;
  void pushd(T* Node);
  void popd();
  size_t height(){return this->size();}
};
template<typename T>
bool Stack<T>::empty()
{
  return (this->head() == nullptr);
}

template<typename T>
T* Stack<T>::top()
{
  return this->tail();
}

template<typename T>
const T* Stack<T>::top() const
{
  return this->tail();
}

template<typename T>
void Stack<T>::pushd(T* Node)
{
  this->append(Node);
}

template<typename T>
void Stack<T>::popd()
{
  this->remove(this->tail());
}

}
#endif // STACK_H
