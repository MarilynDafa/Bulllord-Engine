/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef CONDITIONVARIABLE_H_INCLUDE
#define CONDITIONVARIABLE_H_INCLUDE
#include "Mutex.h"
namespace o2d{
	class O2D_API c_condition_variable
	{
	private:
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		HANDLE m_event; 
		LONG m_waiters_count;
#else
		pthread_cond_t m_handle;
#endif
	public:
		c_condition_variable();
		~c_condition_variable();
		void wait(c_mutex& mutex_);
		void notify();
	};
}
#endif