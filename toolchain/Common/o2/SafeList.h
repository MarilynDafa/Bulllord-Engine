/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SAFELIST_H_INCLUDE
#define SAFELIST_H_INCLUDE
#include "List.h"
#include "Mutex.h"
#include "ConditionVariable.h"
namespace o2d{
	template<typename _Ty>
	class c_safe_list:public c_list<_Ty>
	{		
	private:
		c_mutex m_mutex;
		c_condition_variable m_cv;
	public:
		void notify();
		void wait();
		void lock();
		void unlock();
	};
	template<typename _Ty>
	void c_safe_list<_Ty>::notify()
	{
		m_cv.notify();
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_safe_list<_Ty>::wait()
	{
		m_cv.wait(m_mutex);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_safe_list<_Ty>::lock()
	{
		m_mutex.lock();
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_safe_list<_Ty>::unlock()
	{
		m_mutex.unlock();
	}
}
#endif