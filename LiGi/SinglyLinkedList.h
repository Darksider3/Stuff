#ifndef LIGISINGLYLIST_H
#define LIGISINGLYLIST_H
#include <memory>

namespace Li {
template<typename T>
class SingleNode {
public:
	SingleNode()
	{
		setnull();
	}
	T* next()
	{
		return static_cast<T*>(this)->m_next;
	}

	bool comparer(T& other) const
	{
		return static_cast<const T&>(*this)->cmpr(other);
	}

	bool bigger(T& other) const
	{
		return static_cast<const T&>(*this)->bigger(other);
	}

	void set_next(T* r) const
	{
		static_cast<T*>(this)->m_next = r;
	}

public:
	void setnull()
	{
		static_cast<T*>(this)->set_next(nullptr);
	}

	static std::unique_ptr<T> newPtr(T& data)
	{
		return std::make_unique(data);
	}

private:
	T* m_next;
};

template<typename T>
class SinglyLinkedList;

template<typename T>
class SinglyLinkedListIterator {
public:
	SinglyLinkedListIterator(T* ptr)
	{
		m_current = ptr;
	}

	SinglyLinkedListIterator& operator=(SingleNode<T>* ptr)
	{
		m_current = static_cast<SingleNode<T*>>(ptr);
		return *this;
	}

	SinglyLinkedListIterator& operator++()
    {
		if (m_current) {
			m_current = m_current->next();
		}
		return *this;
    }

	SinglyLinkedListIterator& operator++(int) // postfix?
	{
		operator++();
		return *this;
	}

	bool operator!=(const SinglyLinkedListIterator<T>& it) const
	{
		return m_current != it.m_current;
	}

	T& operator*() const
	{
		return *m_current;
	}

	T* operator->() const
	{
		return m_current;
	}

private:
	friend SinglyLinkedList<T>;

	T* m_current;
};

template<typename T>
class SinglyLinkedList {
public:
	bool empty()
	{
		return (m_head == nullptr);
	}

	T* head() { return m_head; }
	T* tail() { return m_tail; }

	using Iterator = SinglyLinkedListIterator<T>;
	friend Iterator;

	Iterator begin()
	{
		return Iterator(m_head);
	}

	Iterator end()
	{
		return Iterator(nullptr);
	}

	using ConstIt = SinglyLinkedListIterator<const T>;
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
		m_head = Node;
    }

	virtual size_t size() const
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

private:
	T* m_tail { nullptr };
	T* m_head { nullptr };
};

}

#endif
