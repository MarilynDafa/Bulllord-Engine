/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "CoreProfile.h"
#include "wglext.h"
#include "Device.h"
#include "InputMgr.h"
#include "System.h"
#include "Win32Window.h"
#include "UIMgr.h"
namespace o2d{
	namespace internal{		
#if defined(O2D_USE_GL_API)
		PFNWGLSWAPINTERVALEXTPROC wprocSwapIntervalEXT;
		HGLRC _context;
#endif
		LRESULT CALLBACK _wnd_proc(HWND hwnd_, UINT umsg_, WPARAM wparam_, LPARAM lparam_)
		{
			c_win32_window* win = (c_win32_window*)(::GetWindowLongPtr(hwnd_, GWLP_USERDATA));
			if (win)
				return win->msg_proc(hwnd_, umsg_, wparam_, lparam_);
			else
				return ::DefWindowProc(hwnd_, umsg_, wparam_, lparam_);
		}
	}
	c_win32_window::c_win32_window(const c_string& name_, const s_settings& settings_, void* profile_)
		:c_window(name_, settings_, profile_) , 
		m_top_pos(0) , 
		m_left_pos(0) , 
		m_vsync(false)
	{
		HINSTANCE hinst = ::GetModuleHandle(0);
		if(settings_.winid == -1)
		{
			WNDCLASSEXW wc;
			wc.cbSize = sizeof(wc);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = (WNDPROC)internal::_wnd_proc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = sizeof(this);
			wc.hInstance = hinst;
			wc.hIcon = 0;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
			wc.hbrBackground = ((HBRUSH)::GetStockObject(BLACK_BRUSH));
			wc.lpszMenuName = 0;
			wc.lpszClassName = name_.c_str();
			wc.hIconSm = 0;
			::RegisterClassExW(&wc);
			u32 style;
			style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
			RECT rc = { 0, 0, settings_.width, settings_.height };
			::AdjustWindowRect(&rc, style, false);
			s32 rw = rc.right - rc.left;
			s32 rh = rc.bottom - rc.top;
			s32 windowl = (GetSystemMetrics(SM_CXSCREEN) - rw) / 2;
			s32 windowt = (GetSystemMetrics(SM_CYSCREEN) - rh) / 2;
			m_left_pos = windowl < 0?0:windowl;
			m_top_pos = windowt < 0?0:windowt;
			m_hwnd = ::CreateWindowW(name_.c_str(), name_.c_str(), style, m_left_pos, m_top_pos, rw, rh, 0, 0, hinst, 0);
			::SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, LONG_PTR(this));
			::SetWindowLongPtrW(m_hwnd, GWL_STYLE, style);
			::SetWindowPos(m_hwnd, 0, m_left_pos, m_top_pos, rw, rh,SWP_SHOWWINDOW | SWP_NOZORDER);
			::ShowWindow(m_hwnd, SW_SHOWNORMAL);
			m_dim.x() = settings_.width;
			m_dim.y() = settings_.height;
		}
		else
		{
			RECT rc;
			m_hwnd = (HWND)settings_.winid;
			::GetClientRect(m_hwnd, &rc);
			m_dim.x() = settings_.width;
			m_dim.y() = settings_.height;
		}
#if defined(O2D_USE_GL_API)
		m_hdc = GetDC(m_hwnd);
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;
		s32 pf = ::ChoosePixelFormat(m_hdc, &pfd);
		assert(pf != 0);
		::SetPixelFormat(m_hdc, pf, &pfd);
		internal::_context = ::wglCreateContext(m_hdc);
		::wglMakeCurrent(m_hdc, internal::_context);
		((c_core_profile*)profile_)->make();
		internal::wprocSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		internal::wprocSwapIntervalEXT(settings_.sync?1:0);
#else
		((c_core_profile*)profile_)->make(m_hwnd , settings_.width, settings_.height);
		m_vsync = settings_.sync;
#endif
	}
	//--------------------------------------------------------
	c_win32_window::~c_win32_window()
	{
#if defined(O2D_USE_GL_API)
		if (m_hdc && internal::_context)
		{
			wglMakeCurrent(m_hdc, 0);
			wglDeleteContext(internal::_context);
		}
#endif
	}
	//--------------------------------------------------------
	void c_win32_window::resize(u32 width_, u32 height_)
	{
		m_dim.x() = width_;
		m_dim.y() = height_;	
		c_ui_manager::get_singleton_ptr()->resize(width_ , height_);
	}
	//--------------------------------------------------------
	void c_win32_window::swap_buffers()
	{
#if defined(O2D_USE_GL_API)
		::SwapBuffers(m_hdc);
#else
		IDXGISwapChainN* swapchain = c_device::get_singleton_ptr()->get_profile()->get_swapchain();
		swapchain->Present(m_vsync?1:0, 0);
#endif
	}
	//--------------------------------------------------------
    void c_win32_window::poll_event()
	{
		bool gotmsg;
		MSG msg;
		if(m_active)
			gotmsg = ::PeekMessage(&msg, 0 , 0, 0, PM_REMOVE) ? true : false;
		else
			gotmsg = ::GetMessage(&msg, 0, 0, 0) ? true : false;
		if(gotmsg)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	//--------------------------------------------------------
    void c_win32_window::reset_backbuffer()
    {
#if !defined(O2D_USE_DX_API)
        glBindFramebuffer(GL_FRAMEBUFFER , 0);
#endif
    }
	//--------------------------------------------------------
	LRESULT c_win32_window::msg_proc(HWND hwnd_, UINT umsg_, WPARAM wparam_, LPARAM lparam_)
	{
		switch (umsg_)
		{
		case WM_ACTIVATE:
			if (WA_INACTIVE == LOWORD(wparam_))
				m_active = false;
			else
				m_active = true;
			break;
		case WM_ERASEBKGND:
			return 1;
		case WM_PAINT:	
			if (m_active)
			{
				swap_buffers();
			}
			break;
		case WM_ENTERSIZEMOVE:
			m_active = false;
			break;
		case WM_EXITSIZEMOVE:
			{
				RECT rc;
				::GetClientRect(m_hwnd , &rc);
				POINT lt;
				lt.x = rc.left;
				lt.y = rc.top;
				::ClientToScreen(m_hwnd , &lt);
				m_left_pos = lt.x;
				m_top_pos = lt.y;
				m_active = true;
			}
			break;
		case WM_SIZE:
			if ((SIZE_MAXHIDE == wparam_) || (SIZE_MINIMIZED == wparam_))
				m_active = false;
			else
				m_active = true;
			break;
		case WM_DESTROY:
		case WM_CLOSE:
			m_closed = true;
			::PostQuitMessage(0);
			return 0;
		case WM_MOUSELEAVE:
		case WM_NCMOUSEMOVE:
			{
				ShowCursor(true);
			}
			break;
		case WM_MOUSEMOVE:
			{
				f32 x = (f32)(LOWORD(lparam_));
				f32 y = (f32)(HIWORD(lparam_));
				c_input_manager::get_singleton_ptr()->inject_event(IE_TOUCH_MOVE , c_float2(x , y));
			}
			break;
		case WM_MOUSEWHEEL:
			{
				c_input_manager::get_singleton_ptr()->inject_event(IE_PAN , (f32)(s16)HIWORD(wparam_) / 120.f);
			}
			break;
		case WM_LBUTTONDOWN:
			{
				f32 x = (f32)(LOWORD(lparam_));
				f32 y = (f32)(HIWORD(lparam_));				
				if(1 == c_input_manager::get_singleton_ptr()->check_successive_clicks(x , y))
					c_input_manager::get_singleton_ptr()->inject_event(IE_TOUCH_START, c_float2(x , y));
				else
					c_input_manager::get_singleton_ptr()->inject_event(IE_TOUCH_DOUBLE, c_float2(x , y));
				SetCapture(m_hwnd); 
			}
			break;
		case WM_LBUTTONUP:
			{
				f32 x = (f32)(LOWORD(lparam_));
				f32 y = (f32)(HIWORD(lparam_));
				c_input_manager::get_singleton_ptr()->inject_event(IE_TOUCH_FINISH, c_float2(x , y));
				ReleaseCapture();
			}
			break;
		case WM_CHAR:
			{
				c_input_manager::get_singleton_ptr()->inject_event(IE_CHAR, (utf16)wparam_);
			}
			break;
		}
		return ::DefWindowProc(hwnd_, umsg_, wparam_, lparam_);
	}
}