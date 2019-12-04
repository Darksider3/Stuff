#pragma once
#ifndef LIGILL_H
#define LIGILL_H
#include <memory>
#include "Assertions.h"
namespace Li
{

template<typename T>
class LLIterator
{

};

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

template<typename T>
class LinkedList
{
public:
  LinkedList()
  {
    m_tester = std::make_unique<T>();
    m_tester->data = 1;
  }

  T* head() { return m_head; }
  void append(T* Node)
  {
    assert(Node);
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

};


}

#endif
