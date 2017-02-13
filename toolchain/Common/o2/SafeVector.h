/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SAFEVECTOR_H_INCLUDE
#define SAFEVECTOR_H_INCLUDE
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Vector.h"
namespace o2d{
	template<typename _Ty>
	class c_safe_vector: public c_vector<_Ty>
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
	void c_safe_vector<_Ty>::notify()
	{
		m_cv.notify();
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_safe_vector<_Ty>::wait()
	{
		m_cv.wait(m_mutex);
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_safe_vector<_Ty>::lock()
	{
		m_mutex.lock();
	}
	//--------------------------------------------------------
	template<typename _Ty>
	void c_safe_vector<_Ty>::unlock()
	{
		m_mutex.unlock();
	}
}
#endif