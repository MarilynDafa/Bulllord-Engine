/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SAFEMAP_H_INCLUDE
#define SAFEMAP_H_INCLUDE
#include "Map.h"
#include "Mutex.h"
#include "ConditionVariable.h"
namespace o2d{
	template<typename _Kt , typename _Vt>
	class c_safe_map: public c_map<_Kt , _Vt>
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
	template<typename _Kt , typename _Vt>
	void c_safe_map<_Kt , _Vt>::notify()
	{
		m_cv.notify();
	}
	//--------------------------------------------------------
	template<typename _Kt , typename _Vt>
	void c_safe_map<_Kt , _Vt>::wait()
	{
		m_cv.wait(m_mutex);
	}
	//--------------------------------------------------------
	template<typename _Kt , typename _Vt>
	void c_safe_map<_Kt , _Vt>::lock()
	{
		m_mutex.lock();
	}
	//--------------------------------------------------------
	template<typename _Kt , typename _Vt>
	void c_safe_map<_Kt , _Vt>::unlock()
	{
		m_mutex.unlock();
	}
}
#endif