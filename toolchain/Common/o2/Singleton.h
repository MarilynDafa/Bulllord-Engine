/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED
#include "Prerequisites.h"
namespace o2d{
	template<typename _Ts>
	class c_singleton
	{
	protected:
		static _Ts* m_singleton;
	public:
		c_singleton()
		{
			m_singleton =(_Ts*)this;
		}
		virtual ~c_singleton()
		{
			m_singleton = 0;
		}
		static _Ts& get_singleton()
		{
			return *m_singleton;
		}
		static _Ts* get_singleton_ptr()
		{
			return m_singleton;
		}
	private:
		c_singleton(const c_singleton<_Ts>&);
		c_singleton& operator = (const c_singleton<_Ts>&);
	};
	template<typename _Ts>
	_Ts* c_singleton<_Ts>::m_singleton;
}
#endif