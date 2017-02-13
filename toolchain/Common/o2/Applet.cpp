/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Algorithm.h"
#include "Applet.h"
namespace o2d{
	c_applet::c_applet(e_socket_type stype_/* = ST_TCP*/)
		:m_socket_type(stype_) ,
		m_cur_time(0) ,
		m_prev_time(0)
	{
		new c_system;
		new c_device;
		new c_file_manager;
		new c_resource_manager;
		new c_ui_manager;
		new c_input_manager;
	}
	//--------------------------------------------------------
	c_applet::~c_applet()
	{
		delete c_input_manager::get_singleton_ptr();
		delete c_ui_manager::get_singleton_ptr();
		delete c_resource_manager::get_singleton_ptr();			
		delete c_file_manager::get_singleton_ptr();
		delete c_device::get_singleton_ptr();
		delete c_system::get_singleton_ptr();	
	}
	//--------------------------------------------------------
	bool c_applet::build(const c_string& cfg_)
	{
		c_string path = cfg_;
        path.append(L".cfg");
		c_cfg_file cf(path.c_str());
		c_string appname = cf.get_profile_string(L"sys" , L"app_name" , L"Overdrive");
		bool dump = cf.get_profile_string(L"sys" , L"has_dump" , L"false").to_boolean();
		m_socket_type = (cf.get_profile_string(L"net" , L"type" , L"tcp") == L"tcp")?ST_TCP:ST_UDP;
		f32 timeout = cf.get_profile_string(L"net" , L"timeout" , L"10.0").to_float();
		bool sendlzo = cf.get_profile_string(L"net" , L"sendlzo" , L"false").to_boolean();
		bool recvlzo = cf.get_profile_string(L"net" , L"recvlzo" , L"true").to_boolean();
		s32 vol = cf.get_profile_string(L"audio" , L"volume" , L"70").to_integer();
		s32 width = cf.get_profile_string(L"gfx" , L"width" , L"800").to_integer();
		s32 height = cf.get_profile_string(L"gfx" , L"height" , L"480").to_integer();
		bool sync = cf.get_profile_string(L"gfx" , L"sync" , L"false").to_boolean();
		if(!c_system::get_singleton_ptr()->initialize())
			return false;
		if(!c_file_manager::get_singleton_ptr()->initialize())
			return false;
		if(!c_input_manager::get_singleton_ptr()->initialize())
			return false;
#if defined(O2D_PLATFORM_OSX) || defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_LINUX)
		if(!c_device::get_singleton_ptr()->initialize(appname.c_str() , width , height , sync , this))
			return false;
		if(!c_resource_manager::get_singleton_ptr()->initialize(L"StdEquip"))
			return false;
		if(!c_ui_manager::get_singleton_ptr()->initialize())
			return false;
		contruct();
#else
		if(!c_device::get_singleton_ptr()->initialize(appname.c_str() , width , height , sync , this))
			return false;
#endif
		if(dump)
			_dump();
		return true;		
	}
	//--------------------------------------------------------
	s32 c_applet::exec()
	{
#if defined(O2D_PLATFORM_MAC) || defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_LINUX)
		c_window* win = c_device::get_singleton_ptr()->get_window();
		while(!win->is_closed())
		{
            m_cur_time = TIME;
            u32 dt = clamp((m_cur_time - m_prev_time), 0, 1000);			
            m_prev_time = m_cur_time;
            c_resource_manager::get_singleton_ptr()->step(dt);
            c_ui_manager::get_singleton_ptr()->step(dt);
            step(dt);
            if(win->is_active())
            {
                c_device::get_singleton_ptr()->set_framebuffer(c_device::get_singleton_ptr()->get_window()->get_dim());
                win->clear(CM_COLOR);
                draw(dt);
                c_ui_manager::get_singleton_ptr()->draw(dt);
                win->swap_buffers();
				c_input_manager::get_singleton_ptr()->reset();
            }
            win->poll_event();
        }
		destruct();
		c_input_manager::get_singleton_ptr()->shutdown();
		c_ui_manager::get_singleton_ptr()->shutdown();
		c_resource_manager::get_singleton_ptr()->shutdown();
		c_file_manager::get_singleton_ptr()->shutdown();
		c_device::get_singleton_ptr()->shutdown();
		c_system::get_singleton_ptr()->shutdown();
#else
		c_window* win = c_device::get_singleton_ptr()->get_window();
		win->poll_event();
#endif
		return 0xdead;
	}
	//--------------------------------------------------------
	void c_applet::_dump()
	{
	}
}