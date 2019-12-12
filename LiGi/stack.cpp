#include "stack.h"
#include "LinkedList.h"
namespace Li {

template<typename T>
Stack<T>::Stack()
{
}

template<typename T>
bool Stack<T>::empty()
{
  return (this->head() == nullptr);
}

template<typename T>
T& Stack<T>::top()
{
  return this->tail();
}

template<typename T>
const T& Stack<T>::top() const
{
  return this->tail();
}

template<typename T>
void Stack<T>::push(T* Node)
{
  append(Node);
}

template<typename T>
void Stack<T>::pop()
{
  remove(this->tail());
}



}
