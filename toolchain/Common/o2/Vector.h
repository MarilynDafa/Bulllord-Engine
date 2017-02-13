/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef VECTOR_H_INCLUDE
#define VECTOR_H_INCLUDE
#include "Macro.h"
#include "Prerequisites.h"
namespace o2d{
	template<typename _Ty>
	class c_vector
	{
	public:
		class const_iterator;
		class iterator
		{
		public:
			inline iterator():m_current(nullptr) {}
			inline iterator& operator++() { ++m_current; return *this; }
			inline iterator& operator--() { --m_current; return *this; }
			inline iterator operator++(s32) { iterator tmp = *this; ++m_current; return tmp; }
			inline iterator operator--(s32) { iterator tmp = *this; --m_current; return tmp; }
			inline iterator& operator+=(s32 num_)
			{
				if(num_ > 0) while (--num_ && m_current != 0) ++(*this);
				else while(++num_ && m_current != 0) --(*this);
				return *this;
			}
			inline iterator operator+(s32 num_) const { iterator tmp = *this; tmp += num_; return tmp; }
			inline iterator& operator-=(s32 num_) const { return (*this)+=(-num_); }
			inline iterator operator-(s32 num_) const { iterator tmp = *this; tmp -=num_; return tmp; }
			inline bool operator==(const iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const iterator& other_) const { return m_current != other_.m_current; }
			inline bool operator==(const const_iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const const_iterator& other_) const { return m_current != other_.m_current; }
			inline _Ty& operator* (){ return *m_current; }
			inline _Ty* operator->(){ return m_current; }
			inline iterator& operator=(const const_iterator& iter_) { m_current = iter_.m_current; return *this; }
		private:
			inline explicit iterator(_Ty* cur_) : m_current(cur_) {}
			_Ty* m_current;
			friend class c_vector<_Ty>;
			friend class const_iterator;
		};
		class const_iterator
		{
		public:
			inline const_iterator() : m_current(nullptr) {}
			inline const_iterator(const iterator& iter_) : m_current(iter_.m_current)  {}
			inline const_iterator& operator++(){ ++m_current; return *this; }
			inline const_iterator& operator--(){ --m_current; return *this; }
			inline const_iterator operator++(s32) { const_iterator tmp = *this; ++m_current; return tmp; }
			inline const_iterator operator--(s32) { const_iterator tmp = *this; --m_current; return tmp; }
			inline const_iterator& operator+=(s32 num_)
			{
				if(num_ > 0) while(--num_ && m_current != 0) ++(*this);
				else while(++num_ && m_current != 0) --(*this);
				return *this;
			}
			inline const_iterator operator+(s32 num_) const { const_iterator tmp = *this; tmp += num_; return tmp;}
			inline const_iterator& operator-=(s32 num_) const { return (*this)+=(-num_); }
			inline const_iterator operator-(s32 num_) const { const_iterator tmp = *this; tmp -= num_; return tmp;}
			inline bool operator==(const const_iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const const_iterator& other_) const { return m_current != other_.m_current; }
			inline bool operator==(const iterator& other_) const { return m_current == other_.m_current; }
			inline bool operator!=(const iterator& other_) const { return m_current != other_.m_current; }
			inline const _Ty& operator*() const { return *m_current; }
			inline const _Ty* operator->() const { return m_current; }
			inline const_iterator& operator=(const iterator& iter_) { m_current = iter_.m_current; return *this; }
		private:
			inline explicit const_iterator(_Ty* cur_) : m_current(cur_) {}
			_Ty* m_current;
			friend class iterator;
			friend class c_vector<_Ty>;
		};
	private:		
		_Ty* m_data;
		u32 m_capacity;
		u32 m_size;
	public:
		c_vector();
		c_vector(u32 sz_);
		c_vector(const c_vector<_Ty>& src_);
		~c_vector();
	public:
		c_vector<_Ty>& operator=(const c_vector<_Ty>& other_);
		_Ty& operator[](u32 index);
		const _Ty& operator [](u32 index) const;
	public:
		iterator begin();
		const_iterator begin() const;
		iterator rbegin();
		const_iterator rbegin() const;
		iterator end();
		const_iterator end() const;
		iterator rend();
		const_iterator rend() const;
		const _Ty& front() const;
		_Ty& front();
		const _Ty& back() const;
		_Ty& back();
		bool empty() const;
		u32 size() const;
		void clear();
		void push_back(const _Ty& element_);
		void push_front(const _Ty& element_);
		_Ty pop_back();
		_Ty pop_front();
		void append(const c_vector<_Ty>& rhs_);
		iterator insert(iterator iter_ , const _Ty& element_);
		iterator insert(const_iterator iter_ , const _Ty& element_);
		void erase(iterator iter_);
		void erase(const_iterator iter_);
		void resize(u32 usednow_);
	private:
		void _grow_to(u32 newsize_);
	};
	//--------------------------------------------------------
	template<typename _Ty>
	c_vector<_Ty>::c_vector():m_capacity(0), m_size(0), m_data(nullptr){}
	//--------------------------------------------------------
	template<typename _Ty>
	c_vector<_Ty>::c_vector(u32 sz_):m_capacity(0), m_size(0), m_data(nullptr)
	{
		_grow_to(sz_);
		m_size = sz_;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	c_vector<_Ty>::c_vector(const c_vector<_Ty>& src_):m_capacity(0), m_size(0), m_data(nullptr)
	{
		*this = src_;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	c_vector<_Ty>::~c_vector()
	{
		clear();
	}
	//--------------------------------------------------------
	template<typename _Ty>
	c_vector<_Ty>& c_vector<_Ty>::operator=(const c_vector<_Ty>& other_)
	{
		if(this == &other_)
			return *this;
		if(m_data)
			clear();
		if(!other_.m_capacity)
			m_data = 0;
		else
			m_data = (_Ty*)malloc(other_.m_capacity*sizeof(_Ty));
		m_size = other_.m_size;
		m_capacity = other_.m_capacity;
		for(u32 i = 0 ; i < other_.m_size ; ++i)
			new ((void*)&m_data[i]) _Ty(other_.m_data[i]);
		return *this;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty& c_vector<_Ty>::operator[](u32 index_)
	{
		return m_data[index_];
	}
	//--------------------------------------------------------
	template<typename _Ty>
	const _Ty& c_vector<_Ty>::operator [](u32 index_) const
	{
		return m_data[index_];
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::iterator c_vector<_Ty>::begin()
	{
		return iterator(m_data);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::const_iterator c_vector<_Ty>::begin() const
	{
		return const_iterator(m_data);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::iterator c_vector<_Ty>::rbegin()
	{
		return iterator(m_data + m_size - 1);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::const_iterator c_vector<_Ty>::rbegin() const
	{
		return const_iterator(m_data + m_size -1);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::iterator c_vector<_Ty>::end()
	{
		return iterator(m_data + m_size);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::const_iterator c_vector<_Ty>::end() const
	{
		return const_iterator(m_data + m_size);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::iterator c_vector<_Ty>::rend()
	{
		return iterator(m_data - 1);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::const_iterator c_vector<_Ty>::rend() const
	{
		return const_iterator(m_data - 1);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	const _Ty& c_vector<_Ty>::front() const
	{
		return m_data[0];
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty& c_vector<_Ty>::front()
	{
		return m_data[0];
	}
	//--------------------------------------------------------
	template<typename _Ty>
	const _Ty& c_vector<_Ty>::back() const
	{
		return m_data[m_size-1];
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty& c_vector<_Ty>::back()
	{
		return m_data[m_size-1];
	}
	//--------------------------------------------------------
	template<typename _Ty>
	bool c_vector<_Ty>::empty() const
	{
		return !m_size;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	u32 c_vector<_Ty>::size() const
	{
		return m_size;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::clear()
	{
		for(u32 i = 0; i < m_size ; ++i)
			(&m_data[i])->~_Ty();
		free(m_data);
		m_data = 0;
		m_size = 0;
		m_capacity = 0;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::push_back(const _Ty& element_)
	{
		insert(const_iterator(m_data + m_size) , element_);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::push_front(const _Ty& element_)
	{
		insert(iterator(m_data + m_size) , element_);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty c_vector<_Ty>::pop_back()
	{
		_Ty e = m_data[m_size - 1];
		(&m_data[m_size - 1])->~_Ty();
		--m_size;
		return e;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	_Ty c_vector<_Ty>::pop_front()
	{
		_Ty e = m_data[0];
		for(u32 i = 1; i<m_size ; ++i)
		{
			(&m_data[i-1])->~_Ty();
			new ((void*)&m_data[i-1])	_Ty(m_data[i]);
		}
		(&m_data[m_size - 1])->~_Ty();
		--m_size;
		return e;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::append(const c_vector<_Ty>& rhs_)
	{
		for(u32 i = 0; i < rhs_.size(); i++)
			push_back(rhs_[i]);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::iterator c_vector<_Ty>::insert(iterator iter_ , const _Ty& element_)
	{
		u32 idx = (u32)(iter_.m_current - m_data);
		if(m_size + 1 > m_capacity)
		{
			const _Ty e(element_);
			u32 newalloc;
			newalloc = m_size + 1 + (m_capacity < 500 ?(m_capacity < 5 ? 5 : m_size) : m_size >> 2);
			_grow_to(newalloc);
			for(u32 i = m_size; i > idx; --i)
			{
				if(i < m_size)
					(&m_data[i])->~_Ty();
				new ((void*)&m_data[i]) _Ty(m_data[i-1]);
			}
			if(m_size > idx)
				(&m_data[idx])->~_Ty();
			new ((void*)&m_data[idx]) _Ty(e);
		}
		else
		{
			if(m_size > idx)
			{
				new ((void*)&m_data[m_size]) _Ty(m_data[m_size-1]);
				for(u32 i = m_size-1 ; i > idx ; --i)
					m_data[i] = m_data[i-1];
				m_data[idx] = element_;
			}
			else
				new ((void*)&m_data[idx]) _Ty(element_);
		}
		++m_size;
		return iterator(&m_data[idx]);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	typename c_vector<_Ty>::iterator c_vector<_Ty>::insert(const_iterator iter_ , const _Ty& element_)
	{
		u32 idx = (u32)(iter_.m_current - m_data);
		if(m_size + 1 > m_capacity)
		{
			const _Ty e(element_);
			u32 newalloc;
			newalloc = m_size + 1 + (m_capacity < 500 ?(m_capacity < 5 ? 5 : m_size) : m_size >> 2);
			_grow_to(newalloc);
			for(u32 i = m_size; i > idx; --i)
			{
				if(i < m_size)
					(&m_data[i])->~_Ty();
				new ((void*)&m_data[i]) _Ty(m_data[i-1]);
			}
			if(m_size > idx)
				(&m_data[idx])->~_Ty();
			new ((void*)&m_data[idx]) _Ty(e);
		}
		else
		{
			if(m_size > idx)
			{
				new ((void*)&m_data[m_size]) _Ty(m_data[m_size-1]);
				for(u32 i = m_size-1 ; i > idx ; --i)
					m_data[i] = m_data[i-1];
				m_data[idx] = element_;
			}
			else
				new ((void*)&m_data[idx]) _Ty(element_);
		}
		++m_size;
		return iterator(&m_data[idx]);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::erase(iterator iter_)
	{
		u32 index = (u32)(iter_.m_current - m_data);
		for(u32 i = index + 1; i < m_size ; ++i)
		{
			(&m_data[i-1])->~_Ty();
			new((void*)&m_data[i-1]) _Ty(m_data[i]);
		}
		(&m_data[m_size-1])->~_Ty();
		--m_size;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::erase(const_iterator iter_)
	{
		u32 index = (u32)(iter_.m_current - m_data);
		for(u32 i = index + 1; i < m_size ; ++i)
		{
			(&m_data[i-1])->~_Ty();
			new((void*)&m_data[i-1]) _Ty(m_data[i]);
		}
		(&m_data[m_size-1])->~_Ty();
		--m_size;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::resize(u32 usednow_)
	{
		if (m_capacity < usednow_)
			_grow_to(usednow_);
		m_size = usednow_;
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_vector<_Ty>::_grow_to(u32 newsz_)
	{
		_Ty* olddata = m_data;
		m_data = (_Ty*)malloc(newsz_*sizeof(_Ty));
		m_capacity = newsz_;
		u32 end = m_size < newsz_ ? m_size : newsz_;
		for(u32 i = 0; i < end; ++i)
			new((void*)&m_data[i]) _Ty(olddata[i]);
		if (m_capacity < m_size)
			m_size = m_capacity;
		for(u32 i = 0 ; i < m_size ; ++i)
			(&olddata[i])->~_Ty();
		free(olddata);
	}
}
#endif 