/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Button.h"
namespace o2d{
	c_button::c_button(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_BUTTON , parent_, property_) , 
		m_nimg_zoom_region(property_.dim) , 
		m_himg_zoom_region(property_.dim) , 
		m_pimg_zoom_region(property_.dim) , 
		m_dimg_zoom_region(property_.dim) , 
		m_v_align(TA_V_CENTER) , 
		m_h_align(TA_H_CENTER) ,
		m_txt_color(0xFFFFFFFF) , 
		m_state(0) , m_enable(true)
	{
		_sig_enter.connect(this , &c_button::_on_enter);
		_sig_leave.connect(this , &c_button::_on_leave);
	}
	//--------------------------------------------------------
	c_button::~c_button()
	{
		_sig_enter.disconnect(this , &c_button::_on_enter);
		_sig_leave.disconnect(this , &c_button::_on_leave);
	}
	//--------------------------------------------------------
	void c_button::set_enable(bool enable_)
	{
		if(m_enable !=enable_)
		{
			m_enable = enable_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_text(const c_string& text_)
	{
		if(m_text != text_)
		{
			m_text = text_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_text_align(e_text_alignment ha_, e_text_alignment va_)
	{
		if(m_v_align != va_ || m_h_align != ha_)
		{
			m_v_align = va_;
			m_h_align = ha_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_normal_image(const c_string& name_)
	{
		if(m_normal_img != name_)
		{
			m_normal_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_pressed_image(const c_string& name_)
	{
		if(m_pressed_img != name_)
		{
			m_pressed_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_hovered_image(const c_string& name_)
	{
		if(m_hovered_img != name_)
		{
			m_hovered_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_disable_image(const c_string& name_)
	{
		if(m_disable_img != name_)
		{
			m_disable_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::set_nor_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if(sk)
		{
			const c_rect& tc = sk->get_texcoord(m_normal_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			m_nimg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w , tc.lt_pt().y() / h , lt.x() / w , lt.y() / h);
			m_nimg_rect[UC_T] = c_rect(lt.x() / w , tc.lt_pt().y() / h , rt.x() / w , lt.y() / h);
			m_nimg_rect[UC_RT] = c_rect(rt.x() / w , tc.lt_pt().y() / h , tc.rb_pt().x() / w , lt.y() / h);
			m_nimg_rect[UC_L] = c_rect(tc.lt_pt().x() / w , lt.y() / h , lt.x() / w , lb.y() / h);
			m_nimg_rect[UC_MID] = c_rect(lt.x() / w , lt.y() / h , rb.x() / w , rb.y() / h);
			m_nimg_rect[UC_R] = c_rect(rt.x() / w , rt.y() / h , tc.rb_pt().x() / w , rb.y() / h);
			m_nimg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w , lb.y() / h , lb.x() / w , tc.rb_pt().y() / h);
			m_nimg_rect[UC_B] = c_rect(lb.x() / w , lb.y() / h , rb.x() / w , tc.rb_pt().y() / h);
			m_nimg_rect[UC_RB] = c_rect(rb.x() / w , rb.y() / h , tc.rb_pt().x() / w , tc.rb_pt().y() / h);
			m_nimg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_nimg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR , "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_button::set_pre_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if(sk)
		{
			const c_rect& tc = sk->get_texcoord(m_pressed_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			m_pimg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w , tc.lt_pt().y() / h , lt.x() / w , lt.y() / h);
			m_pimg_rect[UC_T] = c_rect(lt.x() / w , tc.lt_pt().y() / h , rt.x() / w , lt.y() / h);
			m_pimg_rect[UC_RT] = c_rect(rt.x() / w , tc.lt_pt().y() / h , tc.rb_pt().x() / w , lt.y() / h);
			m_pimg_rect[UC_L] = c_rect(tc.lt_pt().x() / w , lt.y() / h , lt.x() / w , lb.y() / h);
			m_pimg_rect[UC_MID] = c_rect(lt.x() / w , lt.y() / h , rb.x() / w , rb.y() / h);
			m_pimg_rect[UC_R] = c_rect(rt.x() / w , rt.y() / h , tc.rb_pt().x() / w , rb.y() / h);
			m_pimg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w , lb.y() / h , lb.x() / w , tc.rb_pt().y() / h);
			m_pimg_rect[UC_B] = c_rect(lb.x() / w , lb.y() / h , rb.x() / w , tc.rb_pt().y() / h);
			m_pimg_rect[UC_RB] = c_rect(rb.x() / w , rb.y() / h , tc.rb_pt().x() / w , tc.rb_pt().y() / h);
			m_pimg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_pimg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR , "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_button::set_hov_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if(sk)
		{
			const c_rect& tc = sk->get_texcoord(m_hovered_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			m_himg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w , tc.lt_pt().y() / h , lt.x() / w , lt.y() / h);
			m_himg_rect[UC_T] = c_rect(lt.x() / w , tc.lt_pt().y() / h , rt.x() / w , lt.y() / h);
			m_himg_rect[UC_RT] = c_rect(rt.x() / w , tc.lt_pt().y() / h , tc.rb_pt().x() / w , lt.y() / h);
			m_himg_rect[UC_L] = c_rect(tc.lt_pt().x() / w , lt.y() / h , lt.x() / w , lb.y() / h);
			m_himg_rect[UC_MID] = c_rect(lt.x() / w , lt.y() / h , rb.x() / w , rb.y() / h);
			m_himg_rect[UC_R] = c_rect(rt.x() / w , rt.y() / h , tc.rb_pt().x() / w , rb.y() / h);
			m_himg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w , lb.y() / h , lb.x() / w , tc.rb_pt().y() / h);
			m_himg_rect[UC_B] = c_rect(lb.x() / w , lb.y() / h , rb.x() / w , tc.rb_pt().y() / h);
			m_himg_rect[UC_RB] = c_rect(rb.x() / w , rb.y() / h , tc.rb_pt().x() / w , tc.rb_pt().y() / h);
			m_himg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_himg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR , "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_button::set_dis_fragment(const c_float2& anc_, const c_float2& dim_)
	{
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		f32 w = sk->get_skin_tex()->get_width();
		f32 h = sk->get_skin_tex()->get_height();
		if(sk)
		{
			const c_rect& tc = sk->get_texcoord(m_disable_img);
			c_float2 midpt = (tc.lt_pt() + tc.rb_pt())*0.5f + anc_;
			c_float2 lt = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 rt = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() - dim_.y()*0.5f);
			c_float2 lb = c_float2(midpt.x() - dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			c_float2 rb = c_float2(midpt.x() + dim_.x()*0.5f , midpt.y() + dim_.y()*0.5f);
			m_dimg_rect[UC_LT] = c_rect(tc.lt_pt().x() / w , tc.lt_pt().y() / h , lt.x() / w , lt.y() / h);
			m_dimg_rect[UC_T] = c_rect(lt.x() / w , tc.lt_pt().y() / h , rt.x() / w , lt.y() / h);
			m_dimg_rect[UC_RT] = c_rect(rt.x() / w , tc.lt_pt().y() / h , tc.rb_pt().x() / w , lt.y() / h);
			m_dimg_rect[UC_L] = c_rect(tc.lt_pt().x() / w , lt.y() / h , lt.x() / w , lb.y() / h);
			m_dimg_rect[UC_MID] = c_rect(lt.x() / w , lt.y() / h , rb.x() / w , rb.y() / h);
			m_dimg_rect[UC_R] = c_rect(rt.x() / w , rt.y() / h , tc.rb_pt().x() / w , rb.y() / h);
			m_dimg_rect[UC_LB] = c_rect(tc.lt_pt().x() / w , lb.y() / h , lb.x() / w , tc.rb_pt().y() / h);
			m_dimg_rect[UC_B] = c_rect(lb.x() / w , lb.y() / h , rb.x() / w , tc.rb_pt().y() / h);
			m_dimg_rect[UC_RB] = c_rect(rb.x() / w , rb.y() / h , tc.rb_pt().x() / w , tc.rb_pt().y() / h);
			m_dimg_zoom_region.lt_pt() = m_absolute_rect.lt_pt() + lt - tc.lt_pt();
			m_dimg_zoom_region.rb_pt() = m_absolute_rect.rb_pt() - tc.rb_pt() + rb;
		}
		else
			LOG(LL_ERROR , "can not set skin fragment, because skin is not found");
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_button::set_stencil_image(const c_string& img_)
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
	void c_button::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{		
		m_nimg_zoom_region.lt_pt() += v1_;
		m_nimg_zoom_region.rb_pt() += v2_;
		m_himg_zoom_region.lt_pt() += v1_;
		m_himg_zoom_region.rb_pt() += v2_;
		m_pimg_zoom_region.lt_pt() += v1_;
		m_pimg_zoom_region.rb_pt() += v2_;
		m_dimg_zoom_region.lt_pt() += v1_;
		m_dimg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_button::_on_touch_start()
	{
		if(!m_enable)
			return;
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == this &&!rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
			return;
		}
		m_state = 1;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		c_ui_manager::get_singleton_ptr()->set_focus(this);
	}
	//--------------------------------------------------------
	void c_button::_on_touch_finish()
	{
		if(!m_enable)
			return;
		bool waspressed = (m_state == 1);
		if(!rect_contains(m_clipping_rect ,  c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			m_state = 0;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
			return;
		}
		m_state = 0;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		if(waspressed && get_parent())
			_sig_click.emit();
	}
	//--------------------------------------------------------
	void c_button::_on_enter()
	{
		if(!m_enable)
			return;
		if(m_state != 2)
		{
			m_state = 2;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::_on_leave()
	{
		if(!m_enable)
			return;
		if(m_state == 2)
		{
			m_state = 0;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_button::_redraw()
	{
		if(!m_visible)
			return;
		c_rect rc;
		if(m_enable)
		{
			switch(m_state)
			{
			case 0:
				{
					if (m_stencil_img != c_string((const utf8*)"") && m_stencil_img != c_string((const utf8*)"Nil"))
					{
						c_rect tr;
						tr.lt_pt() = m_nimg_rect[UC_LT].lt_pt();
						tr.rb_pt() = m_nimg_rect[UC_RB].rb_pt();
						c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, tr, m_absolute_rect, m_clipping_rect, m_stencil_rect, m_flipx, m_flipy);
					}
					else
					{
						if (equal(m_absolute_rect.lt_pt(), m_nimg_zoom_region.lt_pt()) && equal(m_nimg_zoom_region.rb_pt(), m_absolute_rect.rb_pt()))
						{
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
						}
						else
						{
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_nimg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_nimg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_nimg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_nimg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_nimg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_nimg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_nimg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_nimg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_nimg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_nimg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_nimg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_nimg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_nimg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_nimg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_nimg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_nimg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_nimg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_nimg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_nimg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_nimg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_nimg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_nimg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_nimg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_nimg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_normal_img, m_nimg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
						}
					}
				}
				break;
			case 2:
				{
					if (m_stencil_img != c_string((const utf8*)"") && m_stencil_img != c_string((const utf8*)"Nil"))
					{
						c_rect tr;
						tr.lt_pt() = m_himg_rect[UC_LT].lt_pt();
						tr.rb_pt() = m_himg_rect[UC_RB].rb_pt();
						c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, tr, m_absolute_rect, m_clipping_rect, m_stencil_rect, m_flipx, m_flipy);
					}
					else
					{
						if (equal(m_absolute_rect.lt_pt(), m_himg_zoom_region.lt_pt()) && equal(m_himg_zoom_region.rb_pt(), m_absolute_rect.rb_pt()))
						{
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
						}
						else
						{
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_himg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_himg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_himg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_himg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_himg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_himg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_himg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_himg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_himg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_himg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_himg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_himg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_himg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_himg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_himg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_himg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_himg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_himg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_himg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_himg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_himg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_himg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_himg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_himg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_hovered_img, m_himg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
						}
					}
				}
				break;
			case 1:
				{
					if (m_stencil_img != c_string((const utf8*)"") && m_stencil_img != c_string((const utf8*)"Nil"))
					{
						c_rect tr;
						tr.lt_pt() = m_pimg_rect[UC_LT].lt_pt();
						tr.rb_pt() = m_pimg_rect[UC_RB].rb_pt();
						c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, tr, m_absolute_rect, m_clipping_rect, m_stencil_rect, m_flipx, m_flipy);
					}
					else
					{
						if (equal(m_absolute_rect.lt_pt(), m_pimg_zoom_region.lt_pt()) && equal(m_pimg_zoom_region.rb_pt(), m_absolute_rect.rb_pt()))
						{
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
						}
						else
						{
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_pimg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_pimg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_pimg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_pimg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_pimg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_pimg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_pimg_zoom_region.lt_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_pimg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_pimg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_pimg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_pimg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_pimg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_pimg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_pimg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_pimg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_pimg_zoom_region.lt_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_pimg_zoom_region.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
							rc.lt_pt().y() = m_pimg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_pimg_zoom_region.lt_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_pimg_zoom_region.lt_pt().x();
							rc.lt_pt().y() = m_pimg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_pimg_zoom_region.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
							rc.lt_pt().x() = m_pimg_zoom_region.rb_pt().x();
							rc.lt_pt().y() = m_pimg_zoom_region.rb_pt().y();
							rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
							rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
							c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_pressed_img, m_pimg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
						}
					}
				}
				break;
			default:assert(0);break;
			}
		}
		else
		{
			if (m_stencil_img != c_string((const utf8*)"") && m_stencil_img != c_string((const utf8*)"Nil"))
			{
				c_rect tr;
				tr.lt_pt() = m_dimg_rect[UC_LT].lt_pt();
				tr.rb_pt() = m_dimg_rect[UC_RB].rb_pt();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, tr, m_absolute_rect, m_clipping_rect, m_stencil_rect, m_flipx, m_flipy);
			}
			else
			{
				if (equal(m_absolute_rect.lt_pt(), m_dimg_zoom_region.lt_pt()) && equal(m_dimg_zoom_region.rb_pt(), m_absolute_rect.rb_pt()))
				{
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
				}
				else
				{
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.lt_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.lt_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_dimg_zoom_region.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.lt_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
					rc.lt_pt().x() = m_dimg_zoom_region.rb_pt().x();
					rc.lt_pt().y() = m_dimg_zoom_region.rb_pt().y();
					rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
					rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_disable_img, m_dimg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
				}
			}
		}
		u32 clr = m_enable?m_txt_color:c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TXT_DISABLE_CLR).rgba();
		u32 fmt = m_v_align|m_h_align;
		c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid, m_fnt_height, fmt, true, m_text, m_absolute_rect, m_clipping_rect, clr, false, m_txt_flag);
		c_widget::_redraw();
	}
}