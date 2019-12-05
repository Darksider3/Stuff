#pragma once
#ifndef LIGILL_H
#define LIGILL_H
#include <memory>
#include "Assertions.h"
namespace Li
{
template<typename T>
class LLNode
{
public:

  LLNode()
  {
    setnull();
  }
  T* next()
  {
    return static_cast<T*>(this)->m_next;
  }

  T* prev()
  {
    return static_cast<T*>(this)->m_prev;
  }

  bool comparer(T &other)
  {
    return static_cast<const T&>(*this)->cmpr(other);
  }

  void set_next(T* r)
  {
    static_cast<T*>(this)->m_next = r;
  }

  void set_prev(T *r)
  {
    static_cast<T*>(this)->m_prev = r;
  }

public:
  void setnull()
  {
    static_cast<T*>(this)->set_next(nullptr);
    static_cast<T*>(this)->set_prev(nullptr);
  }
  static std::unique_ptr<T> newPtr(T &data)
  {
    return std::make_unique(data);
  }
private:
  T* m_next;
  T* m_prev;
};

// FIXME: Not working right, somehow misses the start but goes directly to the end. Will see how that works later
template<typename T>
class LLIterator
{
public:
  LLIterator(T* ptr)
  {
    m_current = ptr;
  }

  LLIterator& operator=(LLNode<T> *ptr)
  {
    assert(ptr);
    m_current = static_cast<LLNode<T*>>(ptr);
    return *this;
  }

  LLIterator& operator++()
  {
    assert(m_current->next());
    if(m_current)
    {
      m_current = m_current->next();
    }
    return *this;
  }

  LLIterator& operator++(int) // postfix?
  {
    LLIterator<T> it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const LLIterator<T> &it)
  {
    return m_current != it.m_current;
  }

  int operator*()
  {
    return m_current;
  }

  const T* m_current;
};


template<typename T>
class LinkedList
{
public:
  LinkedList()
  {
  }

  bool empty()
  {
    return (m_head == nullptr);
  }

  void count(bool t)
  {
    m_store=t;
    if(m_store)
    {
      m_counter = 0;
      m_counter = size();
    }
  }

  T* head() { return m_head; }
  T* tail() { return m_tail; }

  T* begin()
  {
    return m_head;
  }

  T* end()
  {
    return m_tail;
  }
  void append(T* Node)
  {
    assert(Node);
    if(m_store)
      ++m_counter;
    if(m_head == nullptr)
    {
      m_head = Node;
      m_tail = Node;
      m_head->setnull();
      return ;
    }

    assert(m_tail);
    m_tail->set_next(Node);
    Node->set_prev(m_tail);
    Node->set_next(nullptr);
    m_tail = Node;
    return ;
  }

  void prepend(T* Node)
  {
    assert(Node);
    if(m_store)
      ++m_counter;
    if(!m_head)
    {
      m_head = Node;
      m_tail = Node;
      m_head->setnull();
      return;
    }

    assert(m_head);
    Node->set_next(m_head);
    m_head->set_prev(Node);
    Node->set_prev(nullptr);
    m_head = Node;
  }

  virtual size_t size()
  {
    size_t count = 0;
    for(T* p = m_head; p; p = p->next())
    {
      ++count;
    }
    return count;
  }

  size_t counter()
  {
    if(!m_store)
      return size();
    return m_counter;
  }
  void sort()
  {
    /*
     * Use Node<T>->comparer delivered by the struct
    */
  }
  ~LinkedList()
  {
    m_tail = nullptr;
    m_head = nullptr;
  }


  // FIXME: Just a tester.
  std::unique_ptr<T> m_tester;

private:
  T* m_tail { nullptr };
  T* m_head { nullptr };
  bool m_store{false};
  size_t m_counter{0};

};


}

#endif
