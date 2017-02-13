/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "Texture.h"
#include "ResMgr.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Font.h"
#include "ListBox.h"
namespace o2d{
	c_listbox::c_listbox( c_widget* parent_, const c_widget::s_widget_property& property_ )
		:c_widget(WT_LISTBOX , parent_, property_),
		m_selected(-1) , 
		m_row_height(property_.font_height + 4) , 
		m_total_height(0) ,
		m_txt_color(0xFFFFFFFF) ,
		m_scrollpos(0) , 
		m_selecting(false)
	{
		_update_position();
	}
	//--------------------------------------------------------
	c_listbox::~c_listbox(){}
	//--------------------------------------------------------
	void c_listbox::set_row_height( s32 height_ )
	{
		if(m_row_height != height_)
		{
			m_row_height = height_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_listbox::set_bg_image( const c_string& img_ )
	{
		if(m_bg_img!=img_)
		{
			m_bg_img = img_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_listbox::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_listbox::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_listbox::set_selected( s32 id_ )
	{
		if((u32)id_>=m_items.size())
			m_selected = -1;
		else
			m_selected = id_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_listbox::set_selected( const c_string& item_ )
	{
		s32 index = -1;
		if(!item_.is_empty())
		{
			FOREACH(TYPEOF(c_vector<s_list_item>)::iterator , iter , m_items)
			{
				index++;
				if(iter->text == item_)
					break;
			}
		}
		set_selected(index);
	}
	//--------------------------------------------------------
	const c_string& c_listbox::get_item( u32 id_ ) const
	{
		if(id_>=m_items.size())
			return c_string::EmptyStr;
		return m_items[id_].text;
	}
	//--------------------------------------------------------
	const c_string& c_listbox::get_item(s32 xpos_, s32 ypos_) const
	{
		if(xpos_ < m_absolute_rect.lt_pt().x() || xpos_ >= m_absolute_rect.rb_pt().x()||
			ypos_ < m_absolute_rect.lt_pt().y() || ypos_ >= m_absolute_rect.rb_pt().y())
			return c_string::EmptyStr;
		if(m_row_height == 0 )
			return c_string::EmptyStr;
		s32 item = ((ypos_ - m_absolute_rect.lt_pt().y()- 1)) / m_row_height;
		if(item < 0 || item >= (s32)m_items.size())
			return c_string::EmptyStr;
		return m_items[item].text;
	}
	//--------------------------------------------------------
	u32 c_listbox::get_item_count() const
	{
		return m_items.size();
	}
	//--------------------------------------------------------
	u32 c_listbox::add_item(const c_string& text_)
	{
		s_list_item i;
		i.text = text_;
		m_items.push_back(i);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return m_items.size() - 1;
	}
	//--------------------------------------------------------
	u32 c_listbox::insert_item( u32 index_, const c_string& text_)
	{
		s_list_item i;
		i.text = text_;
		m_items.insert(m_items.begin() + index_ , i);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return index_;
	}
	//--------------------------------------------------------
	void c_listbox::set_item(u32 index_, const c_string& text_)
	{
		if(index_ >= m_items.size())
			return;
		m_items[index_].text = text_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_listbox::swap(u32 index1_, u32 index2_)
	{
		if(index1_ >= m_items.size() || index2_ >= m_items.size())
			return;
		s_list_item dummmy = m_items[index1_];
		m_items[index1_] = m_items[index2_];
		m_items[index2_] = dummmy;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_listbox::remove_item(u32 id_)
	{
		if(id_ >= m_items.size())
			return;
		if((u32)m_selected==id_)
			m_selected = -1;
		else if((u32)m_selected > id_)
			m_selected -= 1;
		else;
		m_items.erase(m_items.begin() + id_);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_listbox::clearall()
	{
		m_items.clear();
		m_selected = -1;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_listbox::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_listbox::_update_position()
	{
		c_widget::_update_position();
	}
	//--------------------------------------------------------
	bool c_listbox::_lost_focus( c_widget* widget_ )
	{
		m_selecting = false;
		return c_widget::_lost_focus(widget_);
	}
	//--------------------------------------------------------
	void c_listbox::_on_touch_move()
	{
		if(m_selecting)
		{
			c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
			if(rect_contains(m_absolute_rect , pos))
			{
				_select_new(pos.y());
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
			}
		}
	}
	//--------------------------------------------------------
	void c_listbox::_on_touch_start()
	{
		m_selecting = true;
	}
	//--------------------------------------------------------
	void c_listbox::_on_touch_finish()
	{
		m_selecting = false;
		c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
		if(rect_contains(m_absolute_rect , pos))
		{
			_select_new(pos.y());
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_listbox::_on_pan()
	{
		f32 delta = c_input_manager::get_singleton_ptr()->get_pan();
		m_scrollpos  = m_scrollpos + (delta < 0.f ? -1 : 1)*m_row_height*-0.5f;
		m_scrollpos = clamp(m_scrollpos , 0, m_row_height*m_items.size() - m_absolute_rect.height());
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_listbox::_redraw()
	{
		if(!m_visible)
			return;
		c_rect framer(m_absolute_rect);
		c_rect clientcr(m_absolute_rect);
		clientcr.lt_pt().y() += 1;
		clientcr.lt_pt().x() += 1;
		clientcr.rb_pt().y() -= 1;
		rect_clip(clientcr , m_clipping_rect);
		c_rect rc;
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_LT] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_T] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_RT] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_L] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_MID] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_R] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_LB] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_B] , rc , m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_RB] , rc , m_clipping_rect, m_flipx, m_flipy);
		f32 delta =(m_row_height - m_fnt_height - 8)*0.5f;
		delta = delta > 0.f ? delta : 0.f;
		framer = m_absolute_rect;
		framer.lt_pt().x() += 2;
		framer.rb_pt().x() -= 2;
		framer.rb_pt().y() = m_absolute_rect.lt_pt().y() + m_row_height - delta;
		framer.lt_pt().y() += delta;
		framer.lt_pt().y() -= m_scrollpos;
		framer.rb_pt().y() -= m_scrollpos;
		c_widget* fc = c_ui_manager::get_singleton_ptr()->get_focus_widget();
		bool hl = (fc == this);
		for(u32 i = 0 ; i < m_items.size() ; ++i)
		{
			if(framer.rb_pt().y() >= m_absolute_rect.lt_pt().y() && framer.lt_pt().y() <= m_absolute_rect.rb_pt().y())
			{
				u32 clr;
				if(i == m_selected && hl)
				{
					clr = c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_LIST_HIGHLIGHT).rgba();
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_LIST_SELECTING , framer , clientcr);
				}
				else
					clr = m_txt_color;
				c_rect textr = framer;
				textr.lt_pt().x() += 3;
				c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid , m_fnt_height , TA_V_CENTER|TA_H_CENTER , true , m_items[i].text , textr , clientcr , clr);
			}
			framer.lt_pt().y() += m_row_height;
			framer.rb_pt().y() += m_row_height;
		}
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	void c_listbox::_select_new(s32 ypos_)
	{
		s32 oldselected = m_selected;
		s32 xpos = m_absolute_rect.lt_pt().x();
		if(xpos < m_absolute_rect.lt_pt().x() || xpos >= m_absolute_rect.rb_pt().x()||
			ypos_ < m_absolute_rect.lt_pt().y() || ypos_ >= m_absolute_rect.rb_pt().y())
			m_selected = -1;
		if(m_row_height == 0)
			m_selected = -1;
		s32 item = (ypos_ - m_absolute_rect.lt_pt().y()- 1 + m_scrollpos) / m_row_height;
		if(item < 0 || item >= (s32)m_items.size())
			m_selected = -1;
		m_selected = item;
		if(m_selected<0 && !m_items.empty())
			m_selected = 0;
		if(oldselected!=m_selected)
		{
			_sig_selected_changed.emit(m_selected);
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
}