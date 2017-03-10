/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Slider.h"
namespace o2d{
	c_slider::c_slider(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_SLIDER , parent_, property_) ,
		m_bg_zoom_region(property_.dim) , 
		m_position(0) ,
		m_draw_pos(0) , 
		m_min_value(0) ,
		m_max_value(100) ,
		m_step(10) ,
		m_desired_pos(0) ,
		m_dragging(false) ,
		m_horizontal(true) ,
		m_thumb_dragged(false) , 
		m_tray_click(false) , 
		m_enable(true)
	{
		m_thumb_size = c_float2(10,10);
		set_position(0);
	}
	//--------------------------------------------------------
	c_slider::~c_slider(){}
	//--------------------------------------------------------
	void c_slider::set_range(const c_float2& range_)
	{
		if(m_min_value != range_.x() || m_max_value != range_.y())
		{
			m_min_value = range_.x();
			m_max_value = range_.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_step(s32 step_)
	{
		if(m_step!=step_)
		{
			m_step = step_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_horizontal(bool flag_)
	{
		if(m_horizontal!=flag_)
		{
			m_horizontal =flag_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_position(s32 pos_)
	{
		m_position = clamp(pos_, m_min_value, m_max_value);
		if(m_horizontal)
		{
			f32 f = (m_relative_rect.width() - m_thumb_size.x())/ (f32)(m_max_value - m_min_value);
			m_draw_pos = (m_position-m_min_value)*f;
		}
		else
		{
			f32 f = (m_relative_rect.height() - m_thumb_size.y()) / (f32)(m_max_value - m_min_value);
			m_draw_pos = (m_position-m_min_value)*f;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_slider::set_bg_image(const c_string& name_)
	{
		if(m_bg_img != name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_disable_image(const c_string& name_)
	{
		if (m_dis_img != name_)
		{
			m_dis_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_dis_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if (sk)
		{
			const c_rect& tc = sk->get_texcoord(m_dis_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f, midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f, midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f, midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f, midpt.y() + dim_.y()*0.5f);
			m_dimg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w, tc.lt_pt().y() / h, lt.x() / w, lt.y() / h);
			m_dimg_rect[UC_T] = c_rect(lt.x() / w, tc.lt_pt().y() / h, rt.x() / w, lt.y() / h);
			m_dimg_rect[UC_RT] = c_rect(rt.x() / w, tc.lt_pt().y() / h, tc.rb_pt().x() / w, lt.y() / h);
			m_dimg_rect[UC_L] = c_rect(tc.lt_pt().x() / w, lt.y() / h, lt.x() / w, lb.y() / h);
			m_dimg_rect[UC_MID] = c_rect(lt.x() / w, lt.y() / h, rb.x() / w, rb.y() / h);
			m_dimg_rect[UC_R] = c_rect(rt.x() / w, rt.y() / h, tc.rb_pt().x() / w, rb.y() / h);
			m_dimg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w, lb.y() / h, lb.x() / w, tc.rb_pt().y() / h);
			m_dimg_rect[UC_B] = c_rect(lb.x() / w, lb.y() / h, rb.x() / w, tc.rb_pt().y() / h);
			m_dimg_rect[UC_RB] = c_rect(rb.x() / w, rb.y() / h, tc.rb_pt().x() / w, tc.rb_pt().y() / h);
			m_dimg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_dimg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR, "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_slider::set_stencil_image(const c_string& img_)
	{
		if (img_.get_length() == 0)
			return;
		m_stencil_img = img_;
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		if (sk)
		{
			const c_rect& tc = sk->get_texcoord(m_stencil_img);
			f32 w = sk->get_skin_tex()->get_width();
			f32 h = sk->get_skin_tex()->get_height();
			m_stencil_rect.lt_pt().x() = tc.lt_pt().x() / w;
			m_stencil_rect.lt_pt().y() = tc.lt_pt().y() / h;
			m_stencil_rect.rb_pt().x() = tc.rb_pt().x() / w;
			m_stencil_rect.rb_pt().y() = tc.rb_pt().y() / h;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_slider::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if(sk)
		{
			const c_rect& tc = sk->get_texcoord(m_bg_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			m_bg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w , tc.lt_pt().y() / h , lt.x() / w , lt.y() / h);
			m_bg_rect[UC_T] = c_rect(lt.x() / w , tc.lt_pt().y() / h , rt.x() / w , lt.y() / h);
			m_bg_rect[UC_RT] = c_rect(rt.x() / w , tc.lt_pt().y() / h , tc.rb_pt().x() / w , lt.y() / h);
			m_bg_rect[UC_L] = c_rect(tc.lt_pt().x() / w , lt.y() / h , lt.x() / w , lb.y() / h);
			m_bg_rect[UC_MID] = c_rect(lt.x() / w , lt.y() / h , rb.x() / w , rb.y() / h);
			m_bg_rect[UC_R] = c_rect(rt.x() / w , rt.y() / h , tc.rb_pt().x() / w , rb.y() / h);
			m_bg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w , lb.y() / h , lb.x() / w , tc.rb_pt().y() / h);
			m_bg_rect[UC_B] = c_rect(lb.x() / w , lb.y() / h , rb.x() / w , tc.rb_pt().y() / h);
			m_bg_rect[UC_RB] = c_rect(rb.x() / w , rb.y() / h , tc.rb_pt().x() / w , tc.rb_pt().y() / h);
			m_bg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_bg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR , "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_slider::set_thumb_image(const c_string& img_)
	{
		if(m_thumb_img != img_)
		{
			m_thumb_img = img_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_thumb_disable_image(const c_string& img_)
	{
		if(m_thumb_disable_img != img_)
		{
			m_thumb_disable_img = img_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_thumb_size(const c_float2& sz_)
	{
		if(m_thumb_size.x() != sz_.x() || m_thumb_size.y() != sz_.y())
		{
			m_thumb_size.x() = sz_.x();
			m_thumb_size.y() = sz_.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::set_enable(bool enable_)
	{
		if(m_enable!=enable_)
		{
			m_enable = enable_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_slider::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{		
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_slider::_update_position()
	{
		c_widget::_update_position();
		set_position (m_position);
	}
	//--------------------------------------------------------
	void c_slider::_on_touch_move()
	{
		if(!m_enable)
			return;
		if(!m_dragging)
			return;
		s32 newp = _get_pos_from_mouse(c_input_manager::get_singleton_ptr()->get_touch_pos());
		s32 oldp = m_position;
		if(!m_thumb_dragged)
		{
			if(rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
			{
				m_thumb_dragged = rect_contains(m_thumb_rect , c_input_manager::get_singleton_ptr()->get_touch_pos());
				m_tray_click = !m_thumb_dragged;
			}
			if(m_thumb_dragged)
				set_position(newp);
			else
			{
				m_tray_click = false;
				return;
			}
		}
		if(m_thumb_dragged)
			set_position(newp);
		else
			m_desired_pos = newp;
		if(m_position != oldp && m_parent)
			_sig_value_change.emit(m_position);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_slider::_on_touch_start()
	{
		if(!m_enable)
			return;
		if(rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			m_dragging = true;
			m_thumb_dragged = rect_contains(m_thumb_rect , c_input_manager::get_singleton_ptr()->get_touch_pos());
			m_tray_click = !m_thumb_dragged;
			m_desired_pos = _get_pos_from_mouse(c_input_manager::get_singleton_ptr()->get_touch_pos());			
			c_ui_manager::get_singleton_ptr()->set_focus(this);
		}
	}
	//--------------------------------------------------------
	void c_slider::_on_touch_finish()
	{
		if(!m_enable)
			return;
		if(m_tray_click)
		{		
			if(m_desired_pos >= m_position + m_step)
				set_position(m_position + m_step);
			else
			{
				if(m_desired_pos <= m_position - m_step)
					set_position(m_position - m_step);
				else
				{
					if(m_desired_pos >= m_position - m_step && m_desired_pos <= m_position + m_step)
						set_position(m_desired_pos);
				}
			}
			_sig_value_change.emit(m_position);
		}
		m_dragging = false;
	}
	//--------------------------------------------------------
	void c_slider::_redraw()
	{
		if(!m_visible)
			return;
		if (m_stencil_img != c_string((const utf8*)"") && m_stencil_img != c_string((const utf8*)"Nil"))
		{
			c_rect tr;
			tr.lt_pt() = m_bg_rect[UC_LT].lt_pt();
			tr.rb_pt() = m_bg_rect[UC_RB].rb_pt();
			c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, tr, m_absolute_rect, m_clipping_rect, m_stencil_rect, m_flipx, m_flipy);
		}
		else
		{
			if (m_enable)
			{
				if (equal(m_absolute_rect.lt_pt(), m_bg_zoom_region.lt_pt()) && equal(m_bg_zoom_region.rb_pt(), m_absolute_rect.rb_pt()))
				{
					c_rect rc;
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
				}
				else
				{
					c_rect rc;
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
				}
			}
			else
			{
				if (equal(m_absolute_rect.lt_pt(), m_dimg_zoom_region.lt_pt()) && equal(m_dimg_zoom_region.rb_pt(), m_absolute_rect.rb_pt()))
				{
					c_rect rc;
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
				}
				else
				{
					c_rect rc;
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_dis_img, m_dimg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
				}
			}
		}
		c_rect r;
		m_thumb_rect = m_absolute_rect;
		if(m_horizontal)
		{
			m_thumb_rect.lt_pt().x() = m_absolute_rect.lt_pt().x() + m_draw_pos;
			m_thumb_rect.rb_pt().x() = m_thumb_rect.lt_pt().x() + m_thumb_size.x();
			r = m_thumb_rect;
			r.lt_pt().y() += (m_absolute_rect.height() - m_thumb_size.y())/2.f;
			r.rb_pt().y() -= (m_absolute_rect.height() - m_thumb_size.y())/2.f;
		}
		else
		{
			m_thumb_rect.lt_pt().y() = m_absolute_rect.rb_pt().y() - m_thumb_size .y() - m_draw_pos;
			m_thumb_rect.rb_pt().y() = m_thumb_rect.lt_pt().y() + m_thumb_size.y();
			r = m_thumb_rect;
			r.lt_pt().x() += (m_absolute_rect.width() - m_thumb_size.x())/2.f;
			r.rb_pt().x() -= (m_absolute_rect.width() - m_thumb_size.x())/2.f;
		}
		if(m_enable)
			c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_thumb_img , r , r, false, false);
		else
		{
			if (m_thumb_disable_img.get_length() > 0)
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid,  m_thumb_disable_img , r, r, false, false);
			else
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_thumb_img, r, r, false, false, true);
		}
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	s32 c_slider::_get_pos_from_mouse(const c_float2& pos_) const
	{
		f32 w, p;
		if(m_horizontal)
		{
			w = m_relative_rect.width() - m_thumb_size.x();
			p = pos_.x() - m_absolute_rect.lt_pt().x() - m_thumb_size.x()*0.5f;
		}
		else
		{
			w = m_relative_rect.height() - m_thumb_size.y();
			p = pos_.y() - m_absolute_rect.lt_pt().y() - m_thumb_size.y()*0.5f;
		}
		return (s32)(p/w*(f32)(m_max_value - m_min_value)) + m_min_value;
	}
}