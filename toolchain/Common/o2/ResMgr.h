/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef RESMGR_H_INCLUDE
#define RESMGR_H_INCLUDE
#include "Singleton.h"
#include "SafeList.h"
#include "SafeMap.h"
#include "Thread.h"
#include "Str.h"
#include "ManualLoader.h"
#include "ManagedRes.h"
namespace o2d{
	class O2D_API c_resource_manager:public c_singleton<c_resource_manager>
	{
	private:
		c_map<e_buildin_res_id , c_resource*> m_buildins;
		c_safe_map<u32 , c_resource*> m_sound_res_pool;
		c_safe_map<u32 , c_resource*> m_texture_res_pool;
		c_safe_map<u32 , c_resource*> m_shader_res_pool;
		c_safe_map<u32 , c_resource*> m_sprite_res_pool;
		c_safe_map<u32 , c_resource*> m_ui_res_pool;
		c_safe_list<c_resource*> m_loading_queue;
		c_safe_list<c_resource*> m_setup_queue;
		c_map<u32 , c_resource*> m_processing_res;
		c_thread* m_loading_thread;
	public:
		c_resource_manager();
		virtual ~c_resource_manager();
	public:
		bool initialize(const utf16* sysrespath_);
		void shutdown();
		c_resource* fetch_buildin_res(e_buildin_res_id id_, c_manual_loader* loader_ = nullptr);
		c_resource* fetch_resource(const utf16* filename_, bool async_ = true);
		c_resource* fetch_resource(c_manual_loader* loader_);
		void discard_resource(const utf16* filename_);
		void discard_resource(c_manual_loader* loader_);
		void clear_res_pool(e_resource_type type_);
		u32 get_res_pool_size(e_resource_type type_);
		void step(u32 dt_);
	private:
		THREAD_FUNC _load_thread_func(void* thread_);
		WAKEUP_FUNC _load_wakeup_func(void* thread_);
		c_resource* _add_2_res_queue(c_resource* res_);
		c_resource* _fetch_sound(const utf16* name_ , bool async_);
		void _discard_sound(const utf16* name_);
		c_resource* _fetch_effect(const utf16* name_ , bool async_);
		void _discard_shader(const utf16* name_);
		c_resource* _fetch_texture(const utf16* name_ , bool async_);
		void _discard_texture(const utf16* name_);
		c_resource* _fetch_font(const utf16* name_ , bool async_);
		void _discard_font(const utf16* name_);
		c_resource* _fetch_skin(const utf16* name_ , bool async_);
		void _discard_skin(const utf16* name_);
		c_resource* _fetch_interface(const utf16* name_ , bool async_);
		void _discard_interface(const utf16* name_);		
		c_resource* _fetch_actor(const utf16* name_ , bool async_);
		void _discard_actor(const utf16* name_);
		c_resource* _fetch_particle(const utf16* name_ , bool async_);
		void _discard_particle(const utf16* name_);
		c_resource* _fetch_map(const utf16* name_ , bool async_);
		void _discard_map(const utf16* name_);
		void _setup_buildin_resource(const c_string& path_);
	};
}
#endif