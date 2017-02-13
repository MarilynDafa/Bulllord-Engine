/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef WINDOW_H_INCLUDE
#define WINDOW_H_INCLUDE
#include "Str.h"
#include "Rectangle.h"
#include "Color.h"
namespace o2d{
	class O2D_API c_window
	{
	public:
		struct s_settings
		{
			s32 width;
			s32 height;
			s32 winid;
			bool sync;
		};
	protected:
		c_string m_name;
		c_float2 m_dim;
		bool m_closed;
		bool m_active;
	public:
		c_window(const c_string& name_, const s_settings& settings_, void* profile_);
		virtual ~c_window();
	public:
		inline bool is_closed() const {return m_closed;}
		inline bool is_active() const {return m_active;}
		inline void set_active(bool active_) {m_active = active_;}
		inline void set_closed(bool closed_) {m_closed = closed_;}
		inline const c_float2& get_dim() const {return m_dim;}
        inline void set_dim(const c_float2& dim_) {m_dim = dim_;}
	public:
		void clear(u32 flag_ , const c_color& clr_ = 0x00000000, f32 depth_ = 1.f, s32 stencil_ = 0);
		virtual void swap_buffers() = 0;
        virtual void poll_event() = 0;
        virtual void reset_backbuffer() = 0;
	};
}
#endif