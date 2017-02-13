/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef LIST_H_INCLUDE
#define LIST_H_INCLUDE
#include "Macro.h"
#include "Prerequisites.h"
namespace o2d{
	template<typename _Ty>
	class c_list
	{
	private:
		struct s_node
		{
			s_node(const _Ty& e_) : next(nullptr), prev(nullptr), element(e_) {}
			s_node* next;
			s_node* prev;
			_Ty element;
		};
	public:
		class const_iterator;
		class iterator
		{
		public:
			inline iterator():m_current(nullptr) {}
			inline iterator& operator++() { m_current = m_current->next; return *this; }
			inline iterator& operator--() { m_current = m_current->prev; return *this; }
			inline iterator operator++(s32) { iterator tmp = *this; m_current = m_current->next; return tmp; }
			inline iterator operator--(s32) { iterator tmp = *this; m_current = m_current->prev; return tmp; }
			inline iterator& operator+=(s32 num_)
			{
				if(num_ > 0) while (num_-- && m_current != 0) ++(*this);
				else while(num_++ && m_current != 0) --(*this);
				return *this;
			}
			inline iterator operator+(s32 num_) const { iterator tmp = *this; tmp += num_; return tmp; }
			inline iterator& operator-=(s32 num_) const { return (*this)+=(-num_); }
			inline iterator operator-(s32 num_) const { iterator tmp = *this; tmp -=num_; return tmp; }
			inline bool operator==(const iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const iterator& other_) const { return m_current != other_.m_current; }
			inline bool operator==(const const_iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const const_iterator& other_) const { return m_current != other_.m_current; }
			inline _Ty& operator* (){ return m_current->element; }
			inline _Ty* operator->(){ return &m_current->element; }
			inline iterator& operator=(const const_iterator& iter_) { m_current = iter_.m_current; return *this; }
		private:
			inline explicit iterator(s_node* node_) : m_current(node_) {}
			s_node* m_current;
			friend class c_list<_Ty>;
			friend class const_iterator;
		};
		class const_iterator
		{
		public:
			inline const_iterator() : m_current(nullptr) {}
			inline const_iterator(const iterator& iter_) : m_current(iter_.m_current)  {}
			inline const_iterator& operator++(){ m_current = m_current->next; return *this; }
			inline const_iterator& operator--(){ m_current = m_current->prev; return *this; }
			inline const_iterator operator++(s32) { const_iterator tmp = *this; m_current = m_current->next; return tmp; }
			inline const_iterator operator--(s32) { const_iterator tmp = *this; m_current = m_current->prev; return tmp; }
			inline const_iterator& operator+=(s32 num_)
			{
				if(num_ > 0) while(num_-- && m_current != 0) ++(*this);
				else while(num_++ && m_current != 0) --(*this);
				return *this;
			}
			inline const_iterator operator+(s32 num_) const { const_iterator tmp = *this; tmp += num_; return tmp;}
			inline const_iterator& operator-=(s32 num_) const { return (*this)+=(-num_); }
			inline const_iterator operator-(s32 num_) const { const_iterator tmp = *this; tmp -= num_; return tmp;}
			inline bool operator==(const const_iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const const_iterator& other_) const { return m_current != other_.m_current; }
			inline bool operator==(const iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const iterator& other_) const { return m_current != other_.m_current; }
			inline const _Ty& operator*() const { return m_current->element; }
			inline const _Ty* operator->() const { return &m_current->element; }
			inline const_iterator& operator=(const iterator& iter_) { m_current = iter_.m_current; return *this; }
		private:
			inline explicit const_iterator(s_node* node_) : m_current(node_) {}
			s_node* m_current;
			friend class iterator;
			friend class c_list<_Ty>;
		};
	private:
		s_node* m_first;
		s_node* m_end;
		u32 m_size;
	public:
		c_list();
		c_list(const c_list<_Ty>& rhs_);
		~c_list();
	public:
		void operator=(const c_list<_Ty>& rhs_);
	public:
		iterator begin();
		const_iterator begin() const;
		iterator rbegin();
		const_iterator rbegin() const;
		iterator end();
		const_iterator end() const;
		iterator rend();
		const_iterator rend() const;
		_Ty& front();
		const _Ty& front() const;
		_Ty& back();
		const _Ty& back() const;
		bool empty() const;
		u32 size() const;
		void clear();
		void push_back(const _Ty& e_);
		void push_front(const _Ty& e_);
		_Ty pop_back();
		_Ty pop_front();
		void append(const c_list<_Ty>& l_);
		void insert(iterator iter_, const _Ty& e_);
		void insert(const_iterator iter_, const _Ty& e_);
		void erase(iterator iter_);
		void erase(const_iterator iter_);
	};
	/////////////////////////////////////////////////////////////////////
	template<typename _Ty>
	c_list<_Ty>::c_list():m_first(nullptr) , m_end(nullptr) , m_size(0){}
	//--------------------------------------------------------
	template<typename _Ty>
	c_list<_Ty>::c_list(const c_list<_Ty>& rhs_):m_first(nullptr),m_end(nullptr)
	{
		*this = rhs_;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	c_list<_Ty>::~c_list()
	{
		clear();
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::operator=(const c_list<_Ty>& rhs_)
	{
		if(&rhs_ == this)
			return;
		clear();
		append(rhs_);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::iterator c_list<_Ty>::begin()
	{
		return iterator(m_first);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::const_iterator c_list<_Ty>::begin() const
	{
		return const_iterator(m_first);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::iterator c_list<_Ty>::rbegin()
	{
		return iterator(m_end);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::const_iterator c_list<_Ty>::rbegin() const
	{
		return const_iterator(m_end);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::iterator c_list<_Ty>::end()
	{
		return iterator(0);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::const_iterator c_list<_Ty>::end() const
	{
		return const_iterator(0);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::iterator c_list<_Ty>::rend()
	{
		return iterator(0);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_list<_Ty>::const_iterator c_list<_Ty>::rend() const
	{
		return const_iterator(0);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty& c_list<_Ty>::front()
	{
		return *iterator(m_first);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	const _Ty& c_list<_Ty>::front() const
	{
		return *const_iterator(m_first);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty& c_list<_Ty>::back()
	{
		return *iterator(m_end);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	const _Ty& c_list<_Ty>::back() const
	{
		return *const_iterator(m_end);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	bool c_list<_Ty>::empty() const
	{
		return (0 == m_first);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	u32 c_list<_Ty>::size() const
	{
		return m_size;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::clear()
	{
		m_size = 0;
		while(m_first)
		{
			s_node* next = m_first->next;
			delete m_first;
			m_first = next;
		}
		m_end = 0;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::push_back(const _Ty& e_)
	{
		++m_size;
		s_node* n = new s_node(e_);
		if (m_first == 0)
			m_first = n;
		n->prev = m_end;
		if (m_end != 0)
			m_end->next = n;
		m_end = n;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::push_front(const _Ty& e_)
	{
		++m_size;
		s_node* n = new s_node(e_);
		if (m_first == 0)
		{
			m_first = n;
			m_end = n;
		}
		else
		{
			n->next =m_first;
			m_first->prev = n;
			m_first = n;
		}
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty c_list<_Ty>::pop_back()
	{
		_Ty e = back();
		erase(iterator(m_end));
		return e;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty c_list<_Ty>::pop_front()
	{
		_Ty e = front();
		erase(iterator(m_first));
		return e;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::append(const c_list<_Ty>& l_)
	{
		s_node* node = l_.m_first;
		while(node)
		{
			push_back(node->element);
			node = node->next;
		}
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::insert(iterator it_, const _Ty& e_)
	{
		s_node* n = new s_node(e_);
		m_size++;
		if(it_ == end())
		{
			if (m_first == 0)
				m_first = n;
			n->prev = m_end;
			if (m_end != 0)
				m_end->next = n;
			m_end = n;
		}
		else
		{
			n->prev = it_.m_current->prev;
			if (it_.m_current->prev)
				it_.m_current->prev->next = n;
			n->next = it_.m_current;
			it_.m_current->prev = n;
			if (it_.m_current == m_first)
				m_first = n;
		}
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::insert(const_iterator it_, const _Ty& e_)
	{
		s_node* n = new s_node(e_);
		m_size++;
		if(it_ == end())
		{
			if (m_first == 0)
				m_first = n;
			n->prev = m_end;
			if (m_end != 0)
				m_end->next = n;
			m_end = n;
		}
		else
		{
			n->prev = it_.m_current->prev;
			if (it_.m_current->prev)
				it_.m_current->prev->next = n;
			n->next = it_.m_current;
			it_.m_current->prev = n;
			if (it_.m_current == m_first)
				m_first = n;
		}
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::erase(iterator it_)
	{
		--m_size;
		iterator returniter(it_);
		++returniter;
		if(it_.m_current == m_first)
			m_first = it_.m_current->next;
		else
			it_.m_current->prev->next = it_.m_current->next;
		if(it_.m_current == m_end)
			m_end = it_.m_current->prev;
		else
			it_.m_current->next->prev = it_.m_current->prev;
		delete it_.m_current;
		it_.m_current = 0;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_list<_Ty>::erase(const_iterator it_)
	{
		--m_size;
		iterator returniter(it_);
		++returniter;
		if(it_.m_current == m_first)
			m_first = it_.m_current->next;
		else
			it_.m_current->prev->next = it_.m_current->next;
		if(it_.m_current == m_end)
			m_end = it_.m_current->prev;
		else
			it_.m_current->next->prev = it_.m_current->prev;
		delete it_.m_current;
		it_.m_current = 0;
	}
}
#endif