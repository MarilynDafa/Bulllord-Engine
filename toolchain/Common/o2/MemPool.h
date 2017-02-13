/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MEMPOOL_H_INCLUDE
#define MEMPOOL_H_INCLUDE
#include "Mutex.h"
namespace o2d{
	class O2D_API c_mem_pool
	{
		struct s_mem_node
		{
			s_mem_node* next;
			s_mem_node* prev;
			u32 index;
			u32 size;
			u32 use_time;
			u32 mem[1];
		};
	private:
		struct
		{
			s_mem_node* first;
			s_mem_node* last;
		} m_pool[16];
		c_mutex m_mutex;
		u32 m_maxsz;	
		u32 m_malloc;
		u32 m_gc_times;
		volatile u32 m_currentsz;
	public:
		c_mem_pool();
		~c_mem_pool();
	public:
		void* alloc(u32 bytes_);
		void dealloc(void* mem);
	private:
		void _gc(u32 expectsz_, u32 usetime_);
		u32 _size2idx(u32 size_, u32& realsz_);
	};
}
#endif