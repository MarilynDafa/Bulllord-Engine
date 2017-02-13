/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "Font.h"
#include "UIMgr.h"
#include "Button.h"
#include "Dialog.h"
namespace o2d{
	c_dialog::c_dialog(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_DIALOG , parent_, property_) ,
		m_bg_zoom_region(property_.dim) , 
		m_dragging(false)
	{
		
		set_title_dim(c_float2(m_absolute_rect.width() , 10.f));
	}
	//--------------------------------------------------------
	c_dialog::~c_dialog(){}
	//--------------------------------------------------------
	void c_dialog::set_bg_image(const c_string& img_)
	{
		if(m_bg_img!=img_)
		{
			m_bg_img = img_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dialog::set_title_dim(const c_float2& sz_)
	{
		if(m_title_dim.x() != sz_.x() || m_title_dim.y() != sz_.y())
		{
			m_title_dim.x() = sz_.x();
			m_title_dim.y() = sz_.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dialog::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_dialog::set_stencil_image(const c_string& img_)
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
	void c_dialog::set_stencil_rect(const c_rect& rect_)
	{
	}
	//--------------------------------------------------------
	void c_dialog::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	bool c_dialog::_gain_focus(c_widget* widget_)
	{
		if(m_parent)
			m_parent->bring_to_front(this);
		return c_widget::_gain_focus(this);
	}
	//--------------------------------------------------------
	bool c_dialog::_lost_focus(c_widget* widget_)
	{
		m_dragging = false;
		return c_widget::_lost_focus(this);
	}
	//--------------------------------------------------------
	void c_dialog::_on_touch_move()
	{
		if (m_dragging)
		{
			c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
			if ((pos.x() < m_parent->get_region().lt_pt().x() + 1||
				pos.y() < m_parent->get_region().lt_pt().y() + 1||
				pos.x() > m_parent->get_region().rb_pt().x() - 1||
				pos.y() > m_parent->get_region().rb_pt().y() -1))
				return;
			move(c_float2(pos.x() - m_drag_start.x() , pos.y() - m_drag_start.y()));
			m_drag_start.x() = pos.x();
			m_drag_start.y() = pos.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_dialog::_on_touch_start()
	{
		if (m_parent)
			m_parent->bring_to_front(this);
		c_rect titlerect = m_absolute_rect;
		titlerect.rb_pt().y() = titlerect.lt_pt().y() + m_title_dim.y();
		if(!rect_contains(titlerect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
			return;
		m_drag_start.x() = c_input_manager::get_singleton_ptr()->get_touch_pos().x();
		m_drag_start.y() = c_input_manager::get_singleton_ptr()->get_touch_pos().y();
		m_dragging = true;
		if(m_parent)
			m_parent->bring_to_front(this);
	}
	//--------------------------------------------------------
	void c_dialog::_on_touch_finish()
	{
		m_dragging = false;
	}
	//--------------------------------------------------------
	void c_dialog::_redraw()
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
		c_widget::_redraw();
		//if (m_stencil_img.get_length() && m_stencil_img != c_string((const utf8*)"Nil"))
		//	glDisable(GL_STENCIL_TEST);
	}
	//--------------------------------------------------------
	void c_dialog::_on_close()
	{
		m_need_delete = true;
		c_ui_manager::get_singleton_ptr()->remove_focus(this);
		c_ui_manager::get_singleton_ptr()->reset_hovered();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
}