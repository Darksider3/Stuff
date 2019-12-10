#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H
#include <memory>

namespace Li
{
template<typename T>
class SinglyLinkedList;

template<typename T>
class SinglyNode
{
public:
  SinglyNode()
  {
    static_cast<T*>(this)->setnull();
  }

  T* next()
  {
    return static_cast<T*>(this)->m_next;
  }

  void set_next(T* n)
  {
    static_cast<T*>(this)->m_next = n;
  }

  void setnull()
  {
    static_cast<T*>(this)->m_next = nullptr;
  }

private:
  T* m_next;
};

template <typename T>
class SinglyLinkedIterator
{
  SinglyLinkedIterator(T* ptr)
  {
    m_current = ptr;
  }

  SinglyLinkedIterator& operator=(SinglyLinkedIterator<T> *ptr)
  {
    m_current = static_cast<SinglyLinkedIterator<T*>>(ptr);
    return *this;
  }

  SinglyLinkedIterator& operator++()
  {
    if(m_current)
      m_current = m_current->next();
    return *this;
  }

  SinglyLinkedIterator& operator++(int)
  {
    operator++();
    return *this;
  }

  bool operator!=(const SinglyLinkedIterator<T> &it)
  {
    return m_current != it.m_current;
  }

  T& operator*()
  {
    return *m_current;
  }

  T* operator->()
  {
    return m_current;
  }

private:
  T* m_current;

  friend SinglyLinkedList<T>;
};

template <typename T>
class SinglyLinkedList
{
public:
  bool empty()
  {
    return (m_head == nullptr);
  }

  T* head(){return m_head;}
  T* tail(){return m_tail;}

  using Iterator = SinglyLinkedIterator<T>;
  friend Iterator;

  Iterator begin()
  {
    return Iterator(m_head);
  }

  Iterator end()
  {
    return Iterator(nullptr);
  }


  using ConstIterator = SinglyLinkedIterator<T>;
  friend ConstIterator;

  ConstIterator begin() const
  {
    return ConstIterator(m_head);
  }

  ConstIterator end() const
  {
    return ConstIterator(nullptr);
  }

  void append(std::unique_ptr<T> Node);
  void prepend(std::unique_ptr<T> Node);

  void append(T* Node)
  {
    if(m_head == nullptr)
    {
      m_head = Node;
      m_tail = Node;
      m_head->setnull();
      return;
    }
    assert(m_tail);
    m_tail->set_next(Node);
    Node->set_next(nullptr);
    m_tail = Node;
  }

  void prepend(T* Node)
  {
    if(!m_head)
    {
      m_head = Node;
      m_tail = Node;
      m_head->setnull();
      return;
    }

    assert(m_head);
    Node->set_next(m_head);
    m_head = Node;
  }

  virtual size_t size()
  {
    size_t i = 0;
    for(T* n = m_head; n; n = n->next())
    {
      ++i;
    }
    return i;
  }
private:
  T* m_head;
  T* m_tail;
};
}
#endif // SINGLYLINKEDLIST_H
