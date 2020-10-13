/**
 ** This file is part of the Stuff project.
 ** Copyright 2020 darksider3 <github@darksider3.de>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#pragma once
#ifndef LIGILL_H
#    define LIGILL_H
#    include "Assertions.h"
#    include <memory>

namespace Li {
template<typename T>
class LinkedList;

template<typename T>
class LLNode {
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

    bool comparer(T& other)
    {
        return static_cast<const T&>(*this)->cmpr(other);
    }

    bool bigger(T& other)
    {
        return static_cast<const T&>(*this)->bigger(other);
    }

    void set_next(T* r)
    {
        static_cast<T*>(this)->m_next = r;
    }

    void set_prev(T* r)
    {
        static_cast<T*>(this)->m_prev = r;
    }

public:
    void setnull()
    {
        static_cast<T*>(this)->set_next(nullptr);
        static_cast<T*>(this)->set_prev(nullptr);
    }
    static std::unique_ptr<T> newPtr(T& data)
    {
        return std::make_unique(data);
    }

private:
    T* m_next;
    T* m_prev;
};

template<typename T>
class LLIterator {
public:
    explicit LLIterator(T* ptr)
    {
        m_current = ptr;
    }

    LLIterator& operator=(LLNode<T>* ptr)
    {
        m_current = static_cast<LLNode<T*>>(ptr);
        return *this;
    }

    LLIterator& operator++()
    {
        if (m_current) {
            m_current = m_current->next();
        }
        return *this;
    }

    LLIterator& operator++(int) // postfix?
    {
        operator++();
        return *this;
    }

    bool operator!=(const LLIterator<T>& it)
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
    friend LinkedList<T>;

    T* m_current;
};

template<typename T>
class LinkedList {
public:
    bool empty()
    {
        return (m_head == nullptr);
    }

    T* head() { return m_head; }
    T* tail() { return m_tail; }

    using Iterator = LLIterator<T>;
    friend Iterator;

    Iterator begin()
    {
        return Iterator(m_head);
    }

    Iterator end()
    {
        return Iterator(nullptr);
    }

    using ConstIt = LLIterator<const T>;
    friend ConstIt;

    ConstIt begin() const
    {
        return ConstIt(m_head);
    }

    ConstIt end() const
    {
        return ConstIt(nullptr);
    }

    void append(std::unique_ptr<T> Node)
    {
        T* ins = Node.get();
        append(ins);
    }

    void prepend(std::unique_ptr<T> Node)
    {
        T* ins = Node.get();
        prepend(ins);
    }

    void append(T* Node)
    {
        if (m_head == nullptr) {
            m_head = Node;
            m_tail = Node;
            m_head->setnull();
            return;
        }
        assert(m_tail);
        m_tail->set_next(Node);
        Node->set_prev(m_tail);
        Node->set_next(nullptr);
        m_tail = Node;
    }

    void prepend(T* Node)
    {
        if (!m_head) {
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
        for (T* p = m_head; p; p = p->next()) {
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

    void remove(T* Node)
    {
        if (Node->prev()) {
            assert(Node != m_head);
            Node->prev()->set_next(Node->next());
        } else {
            assert(Node == m_head);
            m_head = Node->next();
        }

        if (Node->next()) {
            assert(Node != m_tail);
            Node->next()->set_prev(Node->prev());
        } else {
            assert(Node == m_tail);
            m_tail = Node->prev();
        }
    }

    T* remove_head()
    {
        T* Node = head();
        if (Node != nullptr) {
            remove(Node);
        }

        return Node;
    }

    T* remove_tail()
    {
        T* Node = tail();
        if (Node != nullptr) {
            remove(Node);
        }
        return Node;
    }

    virtual ~LinkedList() = default;

private:
    T* m_tail { nullptr };
    T* m_head { nullptr };
};

}

#endif
