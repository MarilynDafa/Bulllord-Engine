/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Mutex.h"
namespace o2d{
	c_mutex::c_mutex()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		InitializeCriticalSectionEx(&m_handle , 0 , 0);
#else
		pthread_mutex_init(&m_handle, nullptr);
#endif
	}
	//--------------------------------------------------------	
	c_mutex::~c_mutex()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		DeleteCriticalSection(&m_handle);
#else
		pthread_mutex_destroy(&m_handle);
#endif
	}
	//--------------------------------------------------------	
	void c_mutex::lock()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		EnterCriticalSection(&m_handle);
#else
		pthread_mutex_lock(&m_handle);
#endif
	}
	//--------------------------------------------------------	
	void c_mutex::unlock()
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		LeaveCriticalSection(&m_handle);
#else
		pthread_mutex_unlock(&m_handle);
#endif
	}
}