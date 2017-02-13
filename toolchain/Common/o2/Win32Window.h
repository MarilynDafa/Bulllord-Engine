/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef WIN32WINDOW_H_INCLUDE
#define WIN32WINDOW_H_INCLUDE
#include "Window.h"
#if defined(O2D_PLATFORM_WINDOWS)
namespace o2d{
	class O2D_API c_win32_window:public c_window
	{
	private:
		HWND m_hwnd;
		HDC m_hdc;
		s32 m_top_pos;
		s32 m_left_pos;
		bool m_vsync;
	public:
		c_win32_window(const c_string& name_, const s_settings& settings_, void* profile_);
		virtual ~c_win32_window();
	public:
		inline HWND get_handle() const {return m_hwnd;}
	public:
		void resize(u32 width_, u32 height_);
		void swap_buffers();
        void poll_event();
        void reset_backbuffer();
		LRESULT msg_proc(HWND hwnd_, UINT umsg_, WPARAM wparam_, LPARAM lparam_);
	};
}
#endif
#endif