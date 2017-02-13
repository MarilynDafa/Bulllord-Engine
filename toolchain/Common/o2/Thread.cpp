/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Thread.h"
namespace o2d{
	c_thread::c_thread(thread_cb threadfunc, wakeup_cb wakeupfunc, void* ud)
		:m_thread_func(threadfunc) , 
		m_wakeup_func(wakeupfunc) , 
		m_user_data(ud) , 
		m_running(true)
	{
#if defined(O2D_PLATFORM_WINDOWS)
		u32 id;
		m_handle = (HANDLE)_beginthreadex(nullptr, 0, m_thread_func, this, 0, &id);
#elif defined(O2D_PLATFORM_WPHONE)
		m_handle = CreateEventEx(nullptr , nullptr , CREATE_EVENT_MANUAL_RESET , EVENT_ALL_ACCESS);
		HANDLE compevent;
		if(!DuplicateHandle(GetCurrentProcess(), m_handle, GetCurrentProcess(), &compevent, 0, false, DUPLICATE_SAME_ACCESS))
		{
			CloseHandle(m_handle);
			return;
		}
		auto workitemhandler = ref new WorkItemHandler([=](IAsyncAction^)
		{
			m_thread_func(this);
			SetEvent(compevent);
			CloseHandle(compevent);
		}, CallbackContext::Any);
		ThreadPool::RunAsync(workitemhandler, WorkItemPriority::Normal, WorkItemOptions::TimeSliced);
#else
		if(pthread_create(&m_handle, nullptr, m_thread_func, this) != 0)
			m_handle = 0;
#endif
	}
	//--------------------------------------------------------	
	c_thread::~c_thread()
	{
		m_mutex.lock();
		m_running = false;
		m_mutex.unlock();
		if(m_wakeup_func)
			m_wakeup_func(this);
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		WaitForSingleObjectEx(m_handle, INFINITE , TRUE);
		CloseHandle(m_handle);
		m_handle = 0;
#else
		pthread_join(m_handle, nullptr);
		m_handle = 0;
#endif		
	}
	//--------------------------------------------------------	
	void c_thread::yield()
	{
#if defined(O2D_PLATFORM_WINDOWS)
		Sleep(1);
#elif defined(O2D_PLATFORM_WPHONE)
		static HANDLE singletonevent = nullptr;
		HANDLE sleepevent = singletonevent;
		if(!sleepevent)
		{
			sleepevent = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
			if(!sleepevent)
				return;
			HANDLE previousevent = InterlockedCompareExchangePointerRelease(&singletonevent, sleepevent, nullptr);
			if(previousevent)
			{
				CloseHandle(sleepevent);
				sleepevent = previousevent;
			}
		}
		WaitForSingleObjectEx(sleepevent, 1, false);
#else
		struct timespec ts = {0,0};
		nanosleep(&ts, nullptr);
#endif
	}
	//--------------------------------------------------------	
	void* c_thread::lock_user_data()
	{
		if(m_user_data)
			m_mutex.lock();
		return m_user_data;
	}
	//--------------------------------------------------------	
	void c_thread::unlock_user_data()
	{
		m_mutex.unlock();
	}
}