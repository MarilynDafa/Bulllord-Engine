/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MUTEX_H_INCLUDE
#define MUTEX_H_INCLUDE
#include "Prerequisites.h"
#include "Macro.h"
namespace o2d{
	class O2D_API c_mutex 
	{
		FRIEND_CLASS(c_condition_variable)
	private:
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		CRITICAL_SECTION m_handle;
#else
		pthread_mutex_t m_handle;
#endif
	public:
		c_mutex();
		~c_mutex();
		void lock();
		void unlock();	
	};
}
#endif