/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "NameGen.h"
namespace o2d{
	c_name_generator::c_name_generator(const c_name_generator& rhs_)
		:m_prefix(rhs_.m_prefix), m_next(rhs_.m_next)
	{}
	//--------------------------------------------------------
	c_name_generator::c_name_generator(const c_string& prefix_)
		: m_prefix(prefix_), m_next(1)
	{}
	//--------------------------------------------------------
	c_name_generator::~c_name_generator(){}
	//--------------------------------------------------------
	c_string c_name_generator::generate()
	{
		m_mutex.lock();
		c_string ret;
		ret<<m_prefix<<(m_next++);
		m_mutex.unlock();
		return ret;
	}
	//--------------------------------------------------------
	void c_name_generator::reset()
	{
		m_mutex.lock();
		m_next = 1ULL;
		m_mutex.unlock();
	}
	//--------------------------------------------------------
	void c_name_generator::set_next(u64 val_)
	{
		m_mutex.lock();
		m_next = val_;
		m_mutex.unlock();
	}
	//--------------------------------------------------------
	u64 c_name_generator::get_next() const
	{
		return m_next;
	}
}