/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Tools.h"
#include "ManagedRes.h"
namespace o2d{
	c_resource::c_resource()
		:m_state(RS_UNSET) , 
		m_type(RT_INVALID) , 
		m_id(-1)
	{
		memset(m_filename , 0 , FILENAME_MAX_LENGTH*sizeof(utf16));
	}
	//--------------------------------------------------------
	c_resource::c_resource(const utf16* filename_ , e_resource_type type_)
		:m_ref_count(1) , 
		m_state(RS_UNSET) ,
		m_type(type_) , 
		m_id(-1)
	{
		m_id = hash(filename_);
		memset(m_filename ,0 , FILENAME_MAX_LENGTH*sizeof(utf16));
		u32 len = utf16_len(filename_);
		assert(len < FILENAME_MAX_LENGTH);
		for(u32 i = 0 ; i < len ; ++i)
			m_filename[i] = filename_[i];
	}
	//--------------------------------------------------------
	c_resource::~c_resource(){}
	//--------------------------------------------------------
	bool c_resource::load()
	{
		if(m_state!=RS_UNSET)
		{
			LOG(LL_ERROR , "resource file %s load fail" , (const ansi*)c_string(m_filename).to_utf8());
			return false;
		}
		if(_load_impl())
			m_state = RS_LOADED;
		else
		{
			LOG(LL_ERROR , "resource file %s load fail" , (const ansi*)c_string(m_filename).to_utf8());
			m_state = RS_UNSET;
			return false;
		}
		LOG(LL_INFORMATION , "resource file %s load sucessful" , (const ansi*)c_string(m_filename).to_utf8());
		return true;
	}
	//--------------------------------------------------------
	void c_resource::unload()
	{
		if(m_state!=RS_SETUPED)
			return;    
		_unload_impl();			
		m_state = RS_UNSET;
	}
	//--------------------------------------------------------
	void c_resource::increment()
	{
#ifdef O2D_COMPILER_MSVC
		::InterlockedIncrement((LONG*)&m_ref_count);
#else
		__sync_fetch_and_sub(&m_ref_count,1);  
#endif
	}
	//--------------------------------------------------------
	void c_resource::decrement()
	{
#ifdef O2D_COMPILER_MSVC
		::InterlockedDecrement((LONG*)&m_ref_count);
#else
		__sync_fetch_and_add(&m_ref_count,1);  
#endif
	}
	//--------------------------------------------------------
	void c_resource::setup()
	{
		m_state = RS_SETUPED;
	}
}
