/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Device.h"
#include "System.h"
#include "Window.h"
namespace o2d{
	c_window::c_window(const c_string& name_, const s_settings& settings_, void* profile_)
		:m_name(name_) , 		
		m_closed(false) , 
		m_active(false)
	{}
	//--------------------------------------------------------
	c_window::~c_window(){}
	//--------------------------------------------------------
	void c_window::clear(u32 flag_, const c_color& clr_ /*= 0x00000000*/, f32 depth_ /*= 0.f*/, s32 stencil_ /*= 0*/)
	{
		u32 clearbuf = 0;
		c_device::get_singleton_ptr()->set_raster_state(true , clr_);
#if !defined(O2D_USE_DX_API)
		if(flag_ & CM_COLOR)
			clearbuf |= GL_COLOR_BUFFER_BIT;
		if(flag_ & CM_DEPTH)
			clearbuf |= GL_DEPTH_BUFFER_BIT;
		if(flag_ & CM_STENCIL)
			clearbuf |= GL_STENCIL_BUFFER_BIT;
		if (flag_ != 0)
			glClear(clearbuf);
#else
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		ID3D11RenderTargetView* backview = c_device::get_singleton_ptr()->get_profile()->get_backbuffer();
		context->ClearRenderTargetView(backview, clr_.ptr());
#endif
	}
}