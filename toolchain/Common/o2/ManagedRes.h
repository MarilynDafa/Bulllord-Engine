/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MANAGEDRES_H_INCLUDED
#define MANAGEDRES_H_INCLUDED
#include "Mutex.h"
#include "Prerequisites.h"
namespace o2d{
	class O2D_API c_resource
	{
	protected:
		utf16 m_filename[FILENAME_MAX_LENGTH];
		u32 m_id;
		e_resource_state m_state;
		e_resource_type m_type;
		volatile s32 m_ref_count;
	public:
		c_resource();
		c_resource(const utf16* filename_, e_resource_type type_);
		virtual ~c_resource();
	public:
		inline const utf16* get_filename() const{return m_filename;}
		inline e_resource_type get_res_type() const{return m_type;}
		inline e_resource_state get_res_state() const{return m_state;}
		inline s32 ref_count() const{return m_ref_count;}	
		inline u32 get_id() const {return m_id;}
	public:
		bool load();
		void unload();		
		void increment();
		void decrement();		
		virtual void setup();
	protected:
		virtual bool _load_impl()=0;
		virtual void _unload_impl()=0;
	};
}
#endif
