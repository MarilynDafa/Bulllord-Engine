/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef THREAD_H_INCLUDE
#define THREAD_H_INCLUDE
#include "Mutex.h"
#include "ConditionVariable.h"
namespace o2d{
	class O2D_API c_thread
	{
	private:
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		typedef u32 (__stdcall *thread_cb)(void*);
		typedef void (*wakeup_cb)(void*);
		HANDLE m_handle;
#else
		typedef void*(*thread_cb)(void*);
		typedef void (*wakeup_cb)(void*);
		pthread_t m_handle;
#endif
		thread_cb m_thread_func;
		wakeup_cb m_wakeup_func;
		c_mutex m_mutex;
		void* m_user_data;
		bool m_running;
	public:
		c_thread(thread_cb threadfunc, wakeup_cb wakeupfunc, void* ud);
		~c_thread();
	public:
		inline bool is_running() const {return m_running;} 
	public:
		void yield();
		void* lock_user_data();
		void unlock_user_data();
	};
}
#endif