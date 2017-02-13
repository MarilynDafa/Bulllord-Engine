/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef DEVICE_H_INCLUDE
#define DEVICE_H_INCLUDE
#include "Singleton.h"
#include "Vector.h"
#include "Color.h"
#include "Str.h"
#include "Window.h"
#include "Technique.h"
#include "GeometryBuffer.h"
#include "FrameBuffer.h"
#include "CoreProfile.h"
namespace o2d{	
	class O2D_API c_device:public c_singleton<c_device>
	{
	public:
		struct s_sys_info
		{
			c_string os;
			c_string cpu;
			c_string gpu;
		};
	private:
		struct s_renderstate_list
		{
			c_color clear_clr;
			bool cull_back;
			bool depth_enable;
			f32 clear_depth;
			e_compare_function depth_func;
			bool blend_enable;
			e_blend_operation blend_op;
			e_blend_operation blend_op_alpha;
			e_blend_factor src_factor;
			e_blend_factor dest_factor;
			e_blend_factor src_alpha_factor;
			e_blend_factor dest_alpha_factor;
			u32 texture_unit;
			u32 viewport_w;
			u32 viewport_h;
		};
	private:	
		s_renderstate_list m_rs_list;
		s_sys_info m_sys_info;
		c_core_profile* m_profile;
		c_framebuffer* m_cur_fbo;
		c_framebuffer* m_cache_fbo;
		c_window* m_win;
		u32 m_tex_support;
	public:
		c_device();
		virtual ~c_device();
	public:
		inline c_window* get_window() {return m_win;}
		inline c_core_profile* get_profile() {return m_profile;}
		inline c_framebuffer* get_fbo() {return m_cur_fbo;}
		inline const s_renderstate_list& get_renderstate_list() const {return m_rs_list;}
		inline const s_sys_info& get_sys_info() const {return m_sys_info;}
        inline void set_sys_info(const s_sys_info& info_) {m_sys_info = info_;}
	public:
		bool initialize(const c_string& name_,s32 w_, s32 h_, bool sync_, void* userdata_, s32 winid_= -1);
		void shutdown();
		bool is_tex_support(e_texture_support ts_) const;
		void set_raster_state(bool cullback_, const c_color& clr_);
		void set_ds_state(bool de_, e_compare_function df_, f32 dv_);
		void set_blend_state(bool be_, e_blend_operation bo_, e_blend_operation boa_, e_blend_factor sf_, e_blend_factor df_, e_blend_factor saf_, e_blend_factor daf_);
		void set_texture_unit(u32 unit_);	
		void set_framebuffer(const c_float2& vp_, c_framebuffer* fbo_ = nullptr);
		void draw(c_technique* tech_, c_geometry_buffer* gbo_);
	private:
		void _set_default_renderstate();
		void _query_sys_cap();
	};
}
#endif