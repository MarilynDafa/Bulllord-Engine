/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Device.h"
#include "System.h"
#include "Win32Window.h"
#include "Mapping.h"
namespace o2d{
	c_device::c_device()
		:m_win(nullptr) ,
		m_profile(nullptr) ,
		m_cur_fbo(nullptr) , 
		m_tex_support(TS_RGBA)
	{}
	//--------------------------------------------------------
	c_device::~c_device(){}
	//--------------------------------------------------------
	bool c_device::initialize(const c_string& name_,s32 w_, s32 h_, bool sync_, void* userdata_, s32 winid_ /*= -1*/)
	{
		c_window::s_settings setting;
		setting.width = w_;
		setting.height = h_;
		setting.winid = winid_;
		setting.sync = sync_;
		m_profile = new c_core_profile();
#if defined(O2D_PLATFORM_WINDOWS)
		m_win = new c_win32_window(name_ , setting , m_profile);
		_set_default_renderstate();
		_query_sys_cap();
#elif defined(O2D_PLATFORM_LINUX)
		m_win = new c_x11_window(name_ , setting , m_profile);
		_set_default_renderstate();
		_query_sys_cap();
#elif defined(O2D_PLATFORM_OSX)
		m_win = new c_cocoa_window(name_ , setting , m_profile);
		_set_default_renderstate();
		_query_sys_cap();
#elif defined(O2D_PLATFORM_ANDROID)
		m_win = new c_android_window(name_ , setting , m_profile);
		_set_default_renderstate();
		_query_sys_cap();
#elif defined(O2D_PLATFORM_WPHONE)
		m_win = new c_wp_window(name_ , setting , userdata_);
		_set_default_renderstate();
		_query_sys_cap();
#elif defined(O2D_PLATFORM_IOS)
        m_win = new c_ios_window(name_ , setting , userdata_);
#else
#endif
		m_win->set_active(true);
		return true;
	}
	//--------------------------------------------------------
	void c_device::shutdown()
	{
		delete m_win;
		delete m_profile;
	}
	//--------------------------------------------------------
	bool c_device::is_tex_support(e_texture_support ts_) const
	{
		if(m_tex_support&ts_)
			return true;
		else
			return false;
	}
	//--------------------------------------------------------
	void c_device::set_raster_state(bool cullback_, const c_color& clr_)
	{
		if(m_rs_list.clear_clr != clr_ || m_rs_list.cull_back != cullback_)
		{
#if !defined(O2D_USE_DX_API)
			glClearColor(clr_.r() , clr_.g() , clr_.b() , clr_.a());
#else
			HRESULT hr;
			ID3D11DeviceContextN* context = m_profile->get_context();
			ID3D11DeviceN* device = m_profile->get_device();
			D3D11_RASTERIZER_DESC rasterdesc;
			ZeroMemory(&rasterdesc, sizeof(rasterdesc));
			rasterdesc.FillMode = D3D11_FILL_SOLID;
			rasterdesc.CullMode = cullback_?D3D11_CULL_BACK:D3D11_CULL_NONE;
			rasterdesc.FrontCounterClockwise = FALSE;
			rasterdesc.DepthBias = 0;
			rasterdesc.DepthBiasClamp = 0.0f;
			rasterdesc.SlopeScaledDepthBias = 0.0f;
			rasterdesc.DepthClipEnable = TRUE;
			rasterdesc.ScissorEnable = TRUE;
			rasterdesc.MultisampleEnable = FALSE;
			rasterdesc.AntialiasedLineEnable = FALSE;
			ID3D11RasterizerState* rasterstate;
			hr = device->CreateRasterizerState(&rasterdesc, &rasterstate);
			context->RSSetState(rasterstate);
#endif
			m_rs_list.clear_clr=clr_;
			m_rs_list.cull_back = cullback_;
		}
	}
	//--------------------------------------------------------
	void c_device::set_ds_state(bool de_, e_compare_function df_, f32 dv_)
	{
		if(m_rs_list.depth_enable != de_ || m_rs_list.depth_func != df_)
		{
#if !defined(O2D_USE_DX_API)
			if(de_)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			glDepthFunc(map(df_));
#else
			D3D11_DEPTH_STENCIL_DESC dsdesc;
			ZeroMemory(&dsdesc, sizeof(dsdesc));
			dsdesc.DepthEnable = map(de_);
			dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsdesc.DepthFunc = map(df_);
			dsdesc.StencilEnable = FALSE;
			dsdesc.StencilReadMask = 0xFF;
			dsdesc.StencilWriteMask = 0xFF;
			dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			ID3D11DeviceContextN* context = m_profile->get_context();
			ID3D11DeviceN* device = m_profile->get_device();
			ID3D11DepthStencilState* dsstate;
			HRESULT hr = device->CreateDepthStencilState(&dsdesc, &dsstate);
			assert(!FAILED(hr));
			context->OMSetDepthStencilState(dsstate , 1);
			dsstate->Release();
#endif
			m_rs_list.depth_enable=de_;
			m_rs_list.depth_func=df_;
		}
	}
	//--------------------------------------------------------
	void c_device::set_blend_state(bool be_, e_blend_operation bo_, e_blend_operation boa_, e_blend_factor sf_, e_blend_factor df_, e_blend_factor saf_, e_blend_factor daf_)
	{
		if(m_rs_list.blend_enable!=be_ || m_rs_list.blend_op!=bo_ || m_rs_list.blend_op_alpha!=boa_ ||
			m_rs_list.src_factor!=sf_ || m_rs_list.dest_factor!=df_ || m_rs_list.src_alpha_factor!=saf_ || m_rs_list.dest_alpha_factor!=daf_)
		{
#if !defined(O2D_USE_DX_API)
			if(be_)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
			glBlendEquationSeparate(map(bo_) , map(boa_));
			glBlendFuncSeparate(map(sf_), map(df_), map(saf_), map(daf_));
#else
			D3D11_BLEND_DESC blenddesc;
			blenddesc.RenderTarget[0].BlendEnable = be_;
            blenddesc.RenderTarget[0].SrcBlend = map(sf_);
            blenddesc.RenderTarget[0].DestBlend = map(df_);
            blenddesc.RenderTarget[0].SrcBlendAlpha = map(saf_);
            blenddesc.RenderTarget[0].DestBlendAlpha = map(daf_);
            blenddesc.RenderTarget[0].BlendOp = map(bo_);
			blenddesc.RenderTarget[0].BlendOpAlpha = map(boa_);
			blenddesc.AlphaToCoverageEnable = FALSE;
			blenddesc.IndependentBlendEnable = FALSE;
			blenddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			ID3D11DeviceContextN* context = m_profile->get_context();
			ID3D11DeviceN* device = m_profile->get_device();
			ID3D11BlendState* blendstate;
			HRESULT hr = device->CreateBlendState(&blenddesc, &blendstate);
			assert(!FAILED(hr));
			context->OMSetBlendState(blendstate , nullptr, 0xFFFFFFFF);
			blendstate->Release();
#endif
			m_rs_list.blend_enable = be_;
			m_rs_list.blend_op=bo_;
			m_rs_list.blend_op_alpha=boa_;
			m_rs_list.src_factor=sf_;
			m_rs_list.dest_factor=df_;
			m_rs_list.src_alpha_factor=saf_;
			m_rs_list.dest_alpha_factor=daf_;
		}
	}
	//--------------------------------------------------------
	void c_device::set_texture_unit(u32 unit_)
	{
#if !defined(O2D_USE_DX_API)
		if(m_rs_list.texture_unit!=(GL_TEXTURE0 + unit_))
		{
			m_rs_list.texture_unit = GL_TEXTURE0 + unit_;
			glActiveTexture(GL_TEXTURE0 + unit_);
		}
#endif
	}
	//--------------------------------------------------------
	void c_device::set_framebuffer(const c_float2& vp_, c_framebuffer* fbo_ /*= nullptr*/)
	{
		if(m_cur_fbo != fbo_)
		{
			m_cur_fbo = fbo_;
#if !defined(O2D_USE_DX_API)
			if(!fbo_)
			{
                m_win->reset_backbuffer();
            }
			else
			{
#ifdef O2D_DEBUG_MODE
				fbo_->check_complete();
#endif
				glBindFramebuffer(GL_FRAMEBUFFER , fbo_->get_handle());
			}
			if(m_rs_list.viewport_w != (u32)vp_.x() || m_rs_list.viewport_h != (u32)vp_.y())
			{
				glViewport(0 , 0 , vp_.x() , vp_.y());
				glScissor(0 , 0 , vp_.x() , vp_.y());
				m_rs_list.viewport_w = vp_.x();
				m_rs_list.viewport_h = vp_.y();
			}
#else
			ID3D11DeviceContextN* context = m_profile->get_context();
			if(!fbo_)
			{
				ID3D11RenderTargetView* rtview = m_profile->get_backbuffer();
				context->OMSetRenderTargets(1, &rtview, NULL);
			}
			else
			{			
				ID3D11RenderTargetView* rtview = fbo_->get_handle();
				context->OMSetRenderTargets(1, &rtview, NULL);
			}
			if(m_rs_list.viewport_w != (u32)vp_.x() || m_rs_list.viewport_h != (u32)vp_.y())
			{
				D3D11_VIEWPORT viewport;
				viewport.Width = (f32)vp_.x();
				viewport.Height = (f32)vp_.y();
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				viewport.TopLeftX = 0.0f;
				viewport.TopLeftY = 0.0f;
				context->RSSetViewports(1, &viewport);
				D3D11_RECT rc;
				rc.left = 0;
				rc.top = 0;
				rc.right = vp_.x();
				rc.bottom = vp_.y();
				context->RSSetScissorRects(1 , &rc);
				m_rs_list.viewport_w = vp_.x();
				m_rs_list.viewport_h = vp_.y();
			}
#endif
		}
	}
	//--------------------------------------------------------
	void c_device::draw(c_technique* tech_, c_geometry_buffer* gbo_)
	{
		c_pass* basepass = tech_->get_pass(0);
		gbo_->assgin_attributes(basepass);	
		if(gbo_->use_index())
		{
			for(u32 i = 0; i < tech_->num_pass(); ++ i)
			{
				c_pass* pass = tech_->get_pass(i);
				pass->use();
#if !defined(O2D_USE_DX_API)
				u8* ioffset = nullptr;
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , gbo_->get_index_buffer());
				glDrawElements(map(gbo_->get_prim_type()), gbo_->num_index(), gbo_->is_32_index()?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,ioffset);
#else
				m_profile->get_context()->DrawIndexed(gbo_->num_index(), 0, 0);
#endif
				pass->unuse();
			}
		}
		else
		{
			for(u32 i = 0; i < tech_->num_pass(); ++ i)
			{
				c_pass* pass = tech_->get_pass(i);
				pass->use();
#if !defined(O2D_USE_DX_API)
				glBindBuffer(GL_ARRAY_BUFFER , gbo_->get_vertex_buffer());
				glDrawArrays(map(gbo_->get_prim_type()), 0 , gbo_->num_vertex());
#else
				m_profile->get_context()->Draw(gbo_->num_vertex() , 0);
#endif
				pass->unuse();
			}
		}	
	}
	//--------------------------------------------------------
	void c_device::_set_default_renderstate()
	{
		m_rs_list.depth_enable = true;
		m_rs_list.depth_func = CF_ALWAYS_PASS;
		m_rs_list.clear_depth = 1.f;
		m_rs_list.blend_enable = false;
		m_rs_list.blend_op = BO_ADD;
		m_rs_list.blend_op_alpha = BO_ADD;
		m_rs_list.src_factor = BF_ONE;
		m_rs_list.dest_factor = BF_ZERO;
		m_rs_list.src_alpha_factor = BF_ONE;
		m_rs_list.dest_alpha_factor = BF_ZERO;
		m_rs_list.texture_unit = 0;
#if defined(O2D_USE_GL_API)
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
		glDepthFunc(GL_LESS);
		glClearDepth(1.f);
		glClearStencil(0x00);
		glDepthMask(GL_TRUE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		glActiveTexture(GL_TEXTURE0);
		//glEnable(GL_LINE_SMOOTH);
#elif defined(O2D_USE_GLES_API)
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
		glDepthFunc(GL_LESS);
		glClearDepthf(1.f);
		glDepthMask(GL_TRUE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		glActiveTexture(GL_TEXTURE0);
#else
		HRESULT hr;
		ID3D11DeviceContextN* context = m_profile->get_context();
		ID3D11DeviceN* device = m_profile->get_device();
		D3D11_RASTERIZER_DESC rasterdesc;
		ZeroMemory(&rasterdesc, sizeof(rasterdesc));
		rasterdesc.FillMode = D3D11_FILL_SOLID;
		rasterdesc.CullMode = D3D11_CULL_BACK;
		rasterdesc.FrontCounterClockwise = FALSE;
		rasterdesc.DepthBias = 0;
		rasterdesc.DepthBiasClamp = 0.0f;
		rasterdesc.SlopeScaledDepthBias = 0.0f;
		rasterdesc.DepthClipEnable = TRUE;
		rasterdesc.ScissorEnable = TRUE;
		rasterdesc.MultisampleEnable = FALSE;
		rasterdesc.AntialiasedLineEnable = FALSE;
		ID3D11RasterizerState* rasterstate;
		hr = device->CreateRasterizerState(&rasterdesc, &rasterstate);
		context->RSSetState(rasterstate);
		D3D11_DEPTH_STENCIL_DESC dsdesc;
		ZeroMemory(&dsdesc, sizeof(dsdesc));
		dsdesc.DepthEnable = map(m_rs_list.depth_enable);
		dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsdesc.DepthFunc = map(m_rs_list.depth_func);
		dsdesc.StencilEnable = FALSE;
		dsdesc.StencilReadMask = 0xFF;
		dsdesc.StencilWriteMask = 0xFF;
		dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* dsstate;
		hr = device->CreateDepthStencilState(&dsdesc, &dsstate);
		context->OMSetDepthStencilState(dsstate , 1);
		dsstate->Release();
		rasterstate->Release();
#endif
	}
	//--------------------------------------------------------
	void c_device::_query_sys_cap()
	{
#if defined(O2D_PLATFORM_WINDOWS)
		m_sys_info.os = L"windows";			
		u32 result[4];
		u32 nexids;
		ansi cpustring[0x20];
		ansi cpubrand[0x40];
		s32 cpuinfo[4];
		__cpuid(cpuinfo, 0);
		result[0] = cpuinfo[0];
		result[1] = cpuinfo[1];
		result[3] = cpuinfo[2];
		result[2] = cpuinfo[3];
		if(result[0])
		{
			memset(cpustring, 0, sizeof(cpustring));
			memset(cpubrand, 0, sizeof(cpubrand));
			*((s32*)cpustring) = result[1];
			*((s32*)(cpustring+4)) = result[2];
			*((s32*)(cpustring+8)) = result[3];
			m_sys_info.cpu = (const utf8*)cpustring;
			__cpuid(cpuinfo, 0x80000000);
			result[0] = cpuinfo[0];
			result[1] = cpuinfo[1];
			result[3] = cpuinfo[2];
			result[2] = cpuinfo[3];
			nexids = result[0];
			for(u32 i=0x80000000; i <= nexids; ++i)
			{
				__cpuid(cpuinfo, i);
				result[0] = cpuinfo[0];
				result[1] = cpuinfo[1];
				result[3] = cpuinfo[2];
				result[2] = cpuinfo[3];
				if(i == 0x80000002)
                {
					memcpy(cpubrand + 0, &result[0], sizeof(result[0]));
					memcpy(cpubrand + 4, &result[1], sizeof(result[1]));
					memcpy(cpubrand + 8, &result[3], sizeof(result[3]));
					memcpy(cpubrand + 12, &result[2], sizeof(result[2]));
                }
				else if(i == 0x80000003)
                {
					memcpy(cpubrand + 16 + 0, &result[0], sizeof(result[0]));
					memcpy(cpubrand + 16 + 4, &result[1], sizeof(result[1]));
					memcpy(cpubrand + 16 + 8, &result[3], sizeof(result[3]));
					memcpy(cpubrand + 16 + 12, &result[2], sizeof(result[2]));
                }
				else if(i == 0x80000004)
                {
					memcpy(cpubrand + 32 + 0, &result[0], sizeof(result[0]));
					memcpy(cpubrand + 32 + 4, &result[1], sizeof(result[1]));
					memcpy(cpubrand + 32 + 8, &result[3], sizeof(result[3]));
					memcpy(cpubrand + 32 + 12, &result[2], sizeof(result[2]));
                }
			}
			m_sys_info.cpu += L":";
			m_sys_info.cpu += (const utf8*)cpubrand;
		}
#	if defined(O2D_USE_GL_API)
		m_sys_info.gpu = glGetString(GL_RENDERER);
#	else
		m_sys_info.gpu = L"directx";
#	endif
		m_tex_support = TS_RGBA|TS_S3TC;
#elif defined(O2D_PLATFORM_OSX) || defined(O2D_PLATFORM_LINUX)
		m_sys_info.os = L"osx";
		u32 result[4];
		u32 nexids;
		ansi cpustring[0x20];
		ansi cpubrand[0x40];
#   if defined(O2D_ARCHITECTURE_64)
        __asm__
        (
            "cpuid": "=a" (result[0]), "=b" (result[1]), "=c" (result[3]), "=d" (result[2]) : "a" (0)
         );
#   else
        __asm__
        (
            "pushl  %%ebx           \n\t"
            "cpuid                  \n\t"
            "movl   %%ebx, %%edi    \n\t"
            "popl   %%ebx           \n\t"
            : "=a" (result[0]), "=D" (result[1]), "=c" (result[3]), "=d" (result[2])
            : "a" (0)
        );
#   endif
		if(result[0])
		{
			memset(cpustring, 0, sizeof(cpustring));
			memset(cpubrand, 0, sizeof(cpubrand));
			*((s32*)cpustring) = result[1];
			*((s32*)(cpustring+4)) = result[2];
			*((s32*)(cpustring+8)) = result[3];
			m_sys_info.cpu = (const utf8*)cpustring;
#   if defined(O2D_ARCHITECTURE_64)
            __asm__
            (
             "cpuid": "=a" (result[0]), "=b" (result[1]), "=c" (result[3]), "=d" (result[2]) : "a" (0x80000000)
             );
#   else
            __asm__
            (
             "pushl  %%ebx           \n\t"
             "cpuid                  \n\t"
             "movl   %%ebx, %%edi    \n\t"
             "popl   %%ebx           \n\t"
             : "=a" (result[0]), "=D" (result[1]), "=c" (result[3]), "=d" (result[2])
             : "a" (0x80000000)
             );
#   endif
			nexids = result[0];
			for(u32 i=0x80000000; i <= nexids; ++i)
			{
#   if defined(O2D_ARCHITECTURE_64)
                __asm__
                (
                 "cpuid": "=a" (result[0]), "=b" (result[1]), "=c" (result[3]), "=d" (result[2]) : "a" (i)
                 );
#   else
                __asm__
                (
                 "pushl  %%ebx           \n\t"
                 "cpuid                  \n\t"
                 "movl   %%ebx, %%edi    \n\t"
                 "popl   %%ebx           \n\t"
                 : "=a" (result[0]), "=D" (result[1]), "=c" (result[3]), "=d" (result[2])
                 : "a" (i)
                 );
#   endif
				if(i == 0x80000002)
                {
					memcpy(cpubrand + 0, &result[0], sizeof(result[0]));
					memcpy(cpubrand + 4, &result[1], sizeof(result[1]));
					memcpy(cpubrand + 8, &result[3], sizeof(result[3]));
					memcpy(cpubrand + 12, &result[2], sizeof(result[2]));
                }
				else if(i == 0x80000003)
                {
					memcpy(cpubrand + 16 + 0, &result[0], sizeof(result[0]));
					memcpy(cpubrand + 16 + 4, &result[1], sizeof(result[1]));
					memcpy(cpubrand + 16 + 8, &result[3], sizeof(result[3]));
					memcpy(cpubrand + 16 + 12, &result[2], sizeof(result[2]));
                }
				else if(i == 0x80000004)
                {
					memcpy(cpubrand + 32 + 0, &result[0], sizeof(result[0]));
					memcpy(cpubrand + 32 + 4, &result[1], sizeof(result[1]));
					memcpy(cpubrand + 32 + 8, &result[3], sizeof(result[3]));
					memcpy(cpubrand + 32 + 12, &result[2], sizeof(result[2]));
                }
			}
			m_sys_info.cpu += L":";
			m_sys_info.cpu += (const utf8*)cpubrand;
		}
		m_sys_info.gpu = glGetString(GL_RENDERER);
		m_tex_support = TS_RGBA|TS_S3TC;
#elif defined(O2D_PLATFORM_ANDROID)
		m_tex_support = TS_RGBA|TS_ETC1;
#elif defined(O2D_PLATFORM_IOS)
		//m_sys_info.gpu = glGetString(GL_RENDERER);
		m_tex_support = TS_RGBA|TS_PVRTC|TS_ETC1;
#elif defined(O2D_PLATFORM_WPHONE)
		m_tex_support = TS_RGBA|TS_S3TC;
#else	
#	error unsupport platform
#endif
	}
}