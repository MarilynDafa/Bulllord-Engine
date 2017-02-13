/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef FRAMEBUFFER_H_INCLUDE
#define FRAMEBUFFER_H_INCLUDE
#include "Vector.h"
#include "Color.h"
#include "Texture.h"
namespace o2d{
	class O2D_API c_framebuffer
	{
	protected:
		c_texture* m_clr_attachments[MAX_COLOR_ATTACHMENTS];
		GLuint m_depth_stencil_rb;
		s32 m_num_attachments;
#if !defined(O2D_USE_DX_API)
		GLuint m_handle;
#else
		ID3D11RenderTargetView* m_rtview;
#endif
	public:
		c_framebuffer();
		~c_framebuffer();
	public:
#if !defined(O2D_USE_DX_API)
		inline GLuint get_handle() const {return m_handle;}
		inline GLuint get_stencil() const {
			return m_depth_stencil_rb
				;
		}
#else
		inline ID3D11RenderTargetView* get_handle() {return m_rtview;}
#endif
		inline s32 get_num_attachments() const {return m_num_attachments;}
	public:
		void attach_color_buffer(u32 att_, c_texture* tex_, s32 level_);
		void detach_color_buffer(u32 att_);
		void clear(u32 flag_, const c_color& clr_ = 0x00000000, f32 depth_ = 1.f, s32 stencil_ = 0);
		void check_complete();
	};
}
#endif