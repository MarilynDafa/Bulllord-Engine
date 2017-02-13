/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Device.h"
#include "InputMgr.h"
#include "System.h"
namespace o2d{
	c_input_manager::c_input_manager()
		:m_event(IE_NOTHING) , 
		m_pan_delta(0.f) , 
		m_cur_code_idx(0) 
	{
		memset(&m_multi_clicks , 0 , sizeof(m_multi_clicks));
		m_multi_clicks.double_click_time = 500;
    }
	//--------------------------------------------------------
	c_input_manager::~c_input_manager(){}
	//--------------------------------------------------------
	bool c_input_manager::initialize()
	{
		memset(m_code_point , 0 , MAX_CACHE_CP*sizeof(utf16));
		return true;
	}
	//--------------------------------------------------------
	void c_input_manager::shutdown(){}
	//--------------------------------------------------------
	void c_input_manager::inject_event(e_input_event event_, utf16 cp_)
	{
		m_event = event_;
		m_code_point[m_cur_code_idx] = cp_;
		m_cur_code_idx++;
	}
	//--------------------------------------------------------
	void c_input_manager::inject_event(e_input_event event_, f32 delta_)
	{
		m_event = event_;
		m_pan_delta = delta_;
	}
	//--------------------------------------------------------
	void c_input_manager::inject_event(e_input_event event_, const c_float2& pos_)
	{
		m_event = event_;
		m_touch_pos = pos_;
	}
	//--------------------------------------------------------
	void c_input_manager::reset()
	{
		m_event = IE_NOTHING;
		memset(m_code_point , 0 , MAX_CACHE_CP*sizeof(utf16));
		m_cur_code_idx = 0;
		m_pan_delta = 0.f;
	}
	//--------------------------------------------------------
    u32 c_input_manager::check_successive_clicks(s32 x_, s32 y_)
	{
		u32 clicktime = TIME;
		if((clicktime-m_multi_clicks.last_click_time) < m_multi_clicks.double_click_time
           && fabs(m_multi_clicks.last_click.x() - x_) <= 3
           && fabs(m_multi_clicks.last_click.y() - y_) <= 3
           && m_multi_clicks.count_successive_clicks < 2
           )
			++m_multi_clicks.count_successive_clicks;
		else
			m_multi_clicks.count_successive_clicks = 1;
		m_multi_clicks.last_click_time = clicktime;
		m_multi_clicks.last_click.x() = x_;
		m_multi_clicks.last_click.y() = y_;
		return m_multi_clicks.count_successive_clicks;
	}
}