/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "Font.h"
#include "UIMgr.h"
#include "ProgressBar.h"
namespace o2d{
	c_progressbar::c_progressbar(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_PROGRESSBAR , parent_, property_) ,
		m_bg_zoom_region(property_.dim) , 
		m_txt_color(0xFFFFFFFF) , 
		m_percent(0)
	{}
	//--------------------------------------------------------
	c_progressbar::~c_progressbar(){}
	//--------------------------------------------------------
	void c_progressbar::set_percent(u32 per_)
	{
		if(m_percent != per_)
		{
			m_percent = per_%101;
			_sig_value_change.emit(m_percent);
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_progressbar::set_offset(const c_float2& offset_)
	{
		if(m_offset != offset_)
		{
			m_offset = offset_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_progressbar::set_text(const c_string& text_)
	{
		if(m_text != text_)
		{
			m_text = text_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_progressbar::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_progressbar::set_fill_image(const c_string& name_)
	{
		if(m_fill_img != name_)
		{
			m_fill_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_progressbar::set_bg_image(const c_string& name_)
	{
		if(m_bg_img != name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}

	//--------------------------------------------------------
	void c_progressbar::set_stencil_image(const c_string& img_)
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
	void c_progressbar::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_progressbar::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_progressbar::_redraw()
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


		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if (sk && m_fill_img.get_length() > 0)
		{
			c_rect dr = m_absolute_rect;
			dr.lt_pt() += m_offset;
			dr.rb_pt() -= m_offset;
			dr.rb_pt().x() = dr.lt_pt().x() + (dr.width() * m_percent) / 100.f;
			const c_rect& tc = sk->get_texcoord(m_fill_img);
			c_rect ttc(tc.lt_pt().x() / w, tc.lt_pt().y() / h, tc.rb_pt().x() / w, tc.rb_pt().y() / h);
			ttc.rb_pt().x() = ttc.lt_pt().x() + (ttc.width() * m_percent) / 100.f;
			c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_fill_img, ttc, dr, m_clipping_rect, false, false);
		}
		c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid, m_fnt_height, TA_H_CENTER|TA_V_CENTER, true, m_text, m_absolute_rect, m_clipping_rect, m_txt_color, false,m_txt_flag);
		c_widget::_redraw();
	}
}