/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef INPUTMGR_H_INCLUDE
#define INPUTMGR_H_INCLUDE
#include "Singleton.h"
#include "Float2.h"
namespace o2d{
	class O2D_API c_input_manager:public c_singleton<c_input_manager>
	{
		struct s_multi_clicks
		{
			c_float2 last_click;
			u32 double_click_time;
			u32 count_successive_clicks;
			u32 last_click_time;
		};
	private:
		s_multi_clicks m_multi_clicks;
		e_input_event m_event;
		c_float2 m_touch_pos;
		f32 m_pan_delta;
		u32 m_cur_code_idx;
		utf16 m_code_point[MAX_CACHE_CP];
	public:
		c_input_manager();
		virtual ~c_input_manager();
	public:
		inline e_input_event get_input_type() const {return m_event;}
		inline const utf16* get_string() const {return m_code_point;}		
		inline const c_float2& get_touch_pos() const {return m_touch_pos;}
		inline f32 get_pan() const {return m_pan_delta;}
	public:
		bool initialize();
		void shutdown();
		void inject_event(e_input_event event_, const c_float2& pos_);
		void inject_event(e_input_event event_, f32 delta_);
		void inject_event(e_input_event event_, utf16 cp_);
		void reset();
        u32 check_successive_clicks(s32 x_, s32 y_);
	};
}
#endif