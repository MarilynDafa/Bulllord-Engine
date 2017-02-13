/**************************************************************************************/
//   Copyright (C) 2009-2012	Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Device.h"
#include "FrameBuffer.h"
namespace o2d{
	c_framebuffer::c_framebuffer()
		:m_num_attachments(0) , 
#if !defined(O2D_USE_DX_API)
		m_handle(0)
#else
		m_rtview(nullptr)
#endif
	{
#if !defined(O2D_USE_DX_API)
		glGenFramebuffers(1, &m_handle);
		memset(m_clr_attachments , 0 , sizeof(m_clr_attachments));
#endif
	}
	//--------------------------------------------------------
	c_framebuffer::~c_framebuffer()
	{
#if !defined(O2D_USE_DX_API)
		if(m_handle)
			glDeleteFramebuffers(1, &m_handle);
		glDeleteRenderbuffers(1, &m_depth_stencil_rb);
		m_depth_stencil_rb = 0;
#endif
	}
	//--------------------------------------------------------
	void c_framebuffer::attach_color_buffer(u32 att_, c_texture* tex_, s32 level_)
	{
		if(!m_clr_attachments[att_])
			m_num_attachments++;
		m_clr_attachments[att_] = tex_;
#if !defined(O2D_USE_DX_API)
		GLuint handle = tex_->get_handle();
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + att_, GL_TEXTURE_2D, handle, level_);
		glGenRenderbuffers(1, &m_depth_stencil_rb);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, tex_->get_width(), tex_->get_height());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_rb);
		check_complete();
#else
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		D3D11_RENDER_TARGET_VIEW_DESC rtdesc;
		rtdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtdesc.Texture2D.MipSlice = 0;
		HRESULT result = device->CreateRenderTargetView(tex_->get_handle(), &rtdesc, &m_rtview);
		assert(!FAILED(result));
#endif
	}
	//--------------------------------------------------------
	void c_framebuffer::detach_color_buffer(u32 att_)
	{
		if(m_clr_attachments[att_])
			m_num_attachments--;
		m_clr_attachments[att_] = nullptr;
#if !defined(O2D_USE_DX_API)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+att_, GL_TEXTURE_2D, 0, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
#else
		if(m_rtview)
			m_rtview->Release();
#endif
	}
	//--------------------------------------------------------
	void c_framebuffer::clear(u32 flag_, const c_color& clr_ /*= 0x00000000*/, f32 depth_ /*= 0.f*/, s32 stencil_ /*= 0*/)
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
		//if(flag_ != 0)
			glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		context->ClearRenderTargetView(m_rtview, clr_.ptr());
#endif
	}
	//--------------------------------------------------------
	void c_framebuffer::check_complete()
	{
#if !defined(O2D_USE_DX_API)
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		s32 errorcode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(errorcode != GL_FRAMEBUFFER_COMPLETE)
			LOG(LL_ERROR , "framebuffer has error, error code%d" , errorcode);
#endif
	}
}