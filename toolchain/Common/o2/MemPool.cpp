/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "MemPool.h"
namespace o2d{
	c_mem_pool::c_mem_pool()
		:m_maxsz(16*1024*1024) , 
		m_currentsz(0) , 
		m_malloc(0) , 
		m_gc_times(0)
	{
		memset(m_pool,0, sizeof(m_pool));
	}
	//--------------------------------------------------------
	c_mem_pool::~c_mem_pool()
	{
		for(u32 n=0; n<16; n++ )
		{
			while(m_pool[n].first)
			{
				s_mem_node* node = m_pool[n].first;
				m_pool[n].first = m_pool[n].first->next;
				free(node);
			}
		}
	}
	//--------------------------------------------------------
	void* c_mem_pool::alloc(u32 bytes_)
	{
		u32 realsz = 0;
		u32 index = _size2idx(bytes_, realsz);
		if(index!=u32(-1))
		{
			if(m_pool[index].first)	
			{
				m_mutex.lock();
				if(m_pool[index].first)	
				{
					s_mem_node* node = m_pool[index].first;
					m_pool[index].first = m_pool[index].first->next;
					if(m_pool[index].first)
						m_pool[index].first->prev = nullptr;
					else
						m_pool[index].last = nullptr;
					m_currentsz -= realsz;
					++node->use_time;
					m_mutex.unlock();
					return node->mem;	
				}
				m_mutex.unlock();
			}
		}
		++m_malloc;
		s_mem_node* node = (s_mem_node*)malloc(realsz + sizeof(s_mem_node) - sizeof(u32));
		if(!node)
			return nullptr;
		node->index = index;
		node->size = realsz;
		node->next = nullptr;
		node->prev = nullptr;
		node->use_time = 0;
		return node->mem;	
	}
	//--------------------------------------------------------
	void c_mem_pool::dealloc(void* mem)
	{
		s_mem_node* node = (s_mem_node*)(((u8*)mem) - sizeof(s_mem_node) + sizeof(u32));
		if(node->index!=u32(-1))
		{
			m_mutex.lock();
			if( node->size + m_currentsz > m_maxsz && node->use_time > 0 )
				_gc(node->size*2, node->use_time);
			if( node->size + m_currentsz <= m_maxsz )
			{
				node->prev = nullptr;
				node->next = m_pool[node->index].first;
				if( m_pool[node->index].first )
					m_pool[node->index].first->prev = node;
				else
					m_pool[node->index].last = node;

				m_pool[node->index].first = node;
				m_currentsz += node->size;
				m_mutex.unlock();
				return;
			}
			m_mutex.unlock();
		}
		free(node);
	}
	//--------------------------------------------------------
	void c_mem_pool::_gc(u32 expectsz_, u32 usetime_)
	{
		++m_gc_times;
		u32 freesz = 0;
		for(s32 n=15; n>=0; --n)
		{
			if(!m_pool[n].first)
				continue;
			s_mem_node* node = m_pool[n].last;
			while(node)
			{
				s_mem_node* temp = node;
				node = node->prev;
				if(temp->use_time < usetime_)
				{
					if(node)
						node->next = temp->next;
					if(temp->next)
						temp->next->prev = node;
					if(m_pool[n].last == temp)
						m_pool[n].last = node;
					if(m_pool[n].first == temp)
						m_pool[n].first = temp->next;
					m_currentsz -= temp->size;
					freesz += temp->size;
					free(temp);
				}
				if(freesz >= expectsz_)
					return;
			}
		}
	}
	//--------------------------------------------------------
	u32 c_mem_pool::_size2idx(u32 size_, u32& realsz_)
	{
		if(size_<=32)
		{ 
			realsz_ = 32;
			return 0; 
		}
		if(size_<=64)
		{ 
			realsz_ = 64;
			return 1; 
		}
		if(size_<=128)
		{
			realsz_ = 128;			
			return 2;
		}
		if(size_<=256)
		{ 
			realsz_ = 256;
			return 3; 
		}
		if(size_<=512)
		{
			realsz_ = 512;		
			return 4;
		}
		if(size_<=1024)
		{ 
			realsz_ = 1024;	
			return 5;
		}
		if(size_<=2*1024)
		{ 
			realsz_ = 2*1024;
			return 6; 
		}
		if(size_<=4*1024)
		{ 
			realsz_ = 4*1024;
			return 7; 
		}
		if(size_<=8*1024)
		{
			realsz_ = 8*1024;
			return 8; 
		}
		if(size_<=16*1024)
		{ 
			realsz_ = 16*1024;	
			return 9; 
		}
		if(size_<=32*1024)
		{ 
			realsz_ = 32*1024;	
			return 10; 
		}
		if(size_<=64*1024)
		{
			realsz_ = 64*1024;	
			return 11;
		}
		if(size_<=128*1024)
		{ 
			realsz_ = 128*1024;
			return 12;
		}
		if(size_<=256*1024)
		{
			realsz_ = 256*1024;
			return 13; 
		}
		if(size_<=512*1024)
		{ 
			realsz_ = 512*1024;
			return 14; 
		}
		if(size_<=1024*1024)
		{ 
			realsz_ = 1024*1024;
			return 15; 
		}
		realsz_ = size_;
		return -1;
	}
}