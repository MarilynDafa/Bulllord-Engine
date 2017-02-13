/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SHADER_H_INCLUDE
#define SHADER_H_INCLUDE
#include "ManagedRes.h"
#include "Technique.h"
namespace o2d{	
	class O2D_API c_shader:public c_resource
	{
		FRIEND_CLASS(c_shader_loader)
		struct s_unifrom_desc
		{
			c_string name;
			c_string type;
			u32 count;
			u32 handle;
		};
		struct s_sampler_desc
		{
			c_string name;
			c_string type;
			u32 unit;
		};
		struct s_attrib_desc
		{
			e_vertex_element_semantic semantic;
			c_string name;
			c_string type;
			u8 index;
		};
	private:
		c_map<u32 , c_technique*> m_teches;
		c_map<u32 , c_string> m_vs_prog;
		c_map<u32 , c_string> m_fs_prog;
	public:
		c_shader(const utf16* filename_);
		virtual ~c_shader();
	public:
		c_technique* get_tech(const c_string& name_);
		void active(const c_string& name_);
	protected:
		bool _load_impl();
		void _unload_impl();
	private:
		s32 _str_2_glenum(const c_string& str_);
	};
	//////////////////////////////////////////////////////////////////////////
	//!c_shader_loader
	class O2D_API c_shader_loader:public c_manual_loader
	{
	private:
		c_stream m_data;
	public:
		c_shader_loader(const u8* mem_);
		virtual ~c_shader_loader();
	public:
		bool load();
		void unload();
	};
}
#endif