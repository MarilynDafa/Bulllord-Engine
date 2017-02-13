/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "Font.h"
#include "UIMgr.h"
#include "InputMgr.h"
#include "ScrollBox.h"
namespace o2d{
	c_scrollbox::c_scrollbox(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_SCROLLBOX , parent_, property_) , 
		m_last_recordpos(0) , 
		m_scroll_height(0) , 
		m_scrollpos(0) , 
		m_tolerance(0) ,
		m_pan_delta(0.f) , 
		m_dragging(false) , 
		m_strict(false) ,
		m_elastic(false)
	{}
	//--------------------------------------------------------
	c_scrollbox::~c_scrollbox(){}
	//--------------------------------------------------------
	void c_scrollbox::set_strict(bool stri_)
	{
		m_strict = stri_;
		m_tolerance = m_strict?0:m_absolute_rect.height()*0.8f;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_scrollbox::_on_touch_move()
	{
		if(m_dragging)
		{
			c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
			f32 newspos = clamp(m_scrollpos + pos.y() - m_last_recordpos , m_scroll_height -m_tolerance , m_tolerance);
			m_pan_delta = newspos - m_scrollpos;
			m_scrollpos = newspos;	
			m_last_recordpos = pos.y();		
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_scrollbox::_on_touch_start()
	{
		m_last_recordpos = c_input_manager::get_singleton_ptr()->get_touch_pos().y();
		m_dragging = true;
		f32 upc = 99999.f , downc = -99999.f;
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator, iter, m_children)
		{
			upc = MIN_VALUE((*iter)->get_region().lt_pt().y() , upc);
			downc = MAX_VALUE((*iter)->get_region().rb_pt().y() , downc);
		}
		m_scroll_height = (downc - upc < m_absolute_rect.height())?0.f:(upc + m_absolute_rect.height() - downc);
	}
	//--------------------------------------------------------
	void c_scrollbox::_on_touch_finish()
	{
		m_dragging = false;
	}
	//--------------------------------------------------------
	void c_scrollbox::_redraw()
	{
		if(m_scrollpos <= 0 && m_scrollpos >= m_scroll_height)
			m_elastic = false;
		else
			m_elastic = true;
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator, iter, m_children)
			(*iter)->move(c_float2(0.f, m_pan_delta));
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	void c_scrollbox::_update(c_technique* uit_, u32 dt_)
	{
		if(m_elastic && !m_dragging)
		{
			if(m_scrollpos >= 0)
			{
				f32 newspos = MAX_VALUE(0.f , m_scrollpos - 1.f*dt_);
				m_pan_delta = newspos - m_scrollpos;
				m_scrollpos = newspos;
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
			}
			else if(m_scrollpos <= m_scroll_height)
			{
				f32 newspos = MIN_VALUE(m_scroll_height , m_scrollpos + 1.f*dt_);
				m_pan_delta = newspos - m_scrollpos;
				m_scrollpos = newspos;
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
			}
			else
				m_elastic = false;
		}
		else
		{
			c_widget::_update(uit_, dt_);
			return;
		}
	}
}