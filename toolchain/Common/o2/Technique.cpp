/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Technique.h"
namespace o2d{
	c_technique::c_technique(){}
	//--------------------------------------------------------
	c_technique::~c_technique()
	{
		FOREACH(TYPEOF(c_vector<c_pass*>)::iterator , iter , m_passes)
			delete (*iter);
	}
	//--------------------------------------------------------
	c_pass* c_technique::add_pass(const c_string& name_)
	{
		c_pass* pass = new c_pass(name_);
		m_passes.push_back(pass);
		return pass;
	}
	//--------------------------------------------------------
	u32 c_technique::num_pass() const
	{
		return m_passes.size();
	}
	//--------------------------------------------------------
	c_pass* c_technique::get_pass(u32 index_)
	{
		assert(index_ < m_passes.size());
		return m_passes[index_];
	}
	//--------------------------------------------------------
	c_pass* c_technique::get_pass(const c_string& name_)
	{
		FOREACH(TYPEOF(c_vector<c_pass*>)::iterator , iter , m_passes)
		{
			if((*iter)->get_name() == name_)
				return *iter;
		}
		LOG(LL_ERROR , "can not get pass %s" , (const ansi*)name_.to_utf8());
		return nullptr;
	}
}