/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "ConditionVariable.h"
namespace o2d{
	c_condition_variable::c_condition_variable()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		m_waiters_count = 0;
		m_event = CreateEventEx(nullptr, nullptr , 0, EVENT_MODIFY_STATE|SYNCHRONIZE);
#else
		pthread_cond_init(&m_handle, nullptr);
#endif
	}
	//--------------------------------------------------------	
	c_condition_variable::~c_condition_variable()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		CloseHandle(m_event);
#else
		pthread_cond_destroy(&m_handle);
#endif
	}
	//--------------------------------------------------------	
	void c_condition_variable::wait(c_mutex& mutex_)
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		::InterlockedIncrement(&m_waiters_count);
		mutex_.unlock();
		s32 result = WaitForSingleObjectEx(m_event, INFINITE , TRUE);
		::InterlockedDecrement(&m_waiters_count);
		mutex_.lock();
#else
		pthread_cond_wait(&m_handle, &mutex_.m_handle);
#endif
	}
	//--------------------------------------------------------	
	void c_condition_variable::notify()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		bool havewaiters = (m_waiters_count > 0);
		if(havewaiters)
			SetEvent(m_event);
#else
		pthread_cond_signal(&m_handle);
#endif
	}
}