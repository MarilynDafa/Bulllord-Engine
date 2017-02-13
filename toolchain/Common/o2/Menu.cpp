/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Menu.h"
namespace o2d{
	c_menu::c_menu(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_MENU , parent_, property_) , 
		m_bg_zoom_region(property_.dim) , 
		m_txt_color(0xFFFFFFFF) , 
		m_selected(-1) , 
		m_interaction(false)
	{
		if(c_ui_manager::get_singleton_ptr()->get_environment() == m_parent)
			c_ui_manager::get_singleton_ptr()->set_focus(this);
		else
			m_interaction = true;
	}
	//--------------------------------------------------------
	c_menu::~c_menu(){}
	//--------------------------------------------------------
	void c_menu::set_arrow_image(const c_string& name_)
	{
		if(m_arrow_img != name_)
		{
			m_arrow_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_menu::set_bg_image(const c_string& name_)
	{
		if(m_bg_img != name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_menu::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_menu::set_item(u32 idx_ , const c_string& text_ , s32 cmd_ , bool enabled_)
	{
		assert(idx_ <m_items.size());
		m_items[idx_].context = text_;
		m_items[idx_].cmd = cmd_;
		m_items[idx_].enabled = enabled_;
		_update_size();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	u32 c_menu::add_item(const c_string& text_, s32 cmdid_,bool enabled_)
	{
		return insert_item(m_items.size(), text_, cmdid_, enabled_);
	}
	//--------------------------------------------------------
	u32 c_menu::insert_item(u32 idx_, const c_string& text_, s32 cmdid_, bool enabled_)
	{
		s_menu_item s;
		s.enabled = enabled_;
		s.context = text_;
		s.separator = text_.is_empty();
		s.sub_menu = 0;
		s.cmd = cmdid_;
		u32 result = idx_;
		if(idx_ < m_items.size())
			m_items.insert(m_items.begin() + idx_ , s);
		else
		{
			m_items.push_back(s);
			result = m_items.size() - 1;
		}
		_update_size();
		return result;
	}
	//--------------------------------------------------------
	s32 c_menu::find_item(s32 cmdid_, u32 idx_) const
	{
		for(u32 i=idx_; i<m_items.size(); ++i )
		{
			if( m_items[i].cmd == cmdid_ )
				return (s32)i;
		}
		return -1;
	}
	//--------------------------------------------------------
	void c_menu::add_separator()
	{
		add_item(c_string::EmptyStr, -1, false);
	}
	//--------------------------------------------------------
	void c_menu::remove_item(u32 idx_)
	{
		if(idx_ >= m_items.size())
			return;
		if(m_items[idx_].sub_menu)
		{
			delete m_items[idx_].sub_menu;
			m_items[idx_].sub_menu = nullptr;
		}
		m_items.erase(m_items.begin() + idx_);
		_update_size();
	}
	//--------------------------------------------------------
	void c_menu::remove_all()
	{
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->sub_menu)
			{
				delete iter->sub_menu;
				iter->sub_menu = nullptr;
			}
		}
		m_items.clear();
		_update_size();
	}
	//--------------------------------------------------------
	c_menu* c_menu::add_sub_menu(u32 idx_ , const c_string& name_)
	{
		if(idx_ >= m_items.size())
		{
			LOG(LL_ERROR , "the menu index is out of range");
			return nullptr;
		}
		c_widget::s_widget_property p;
		p.dim = c_rect(0.f ,0.f , 100.f , 100.f);
		p.name = name_;
		p.font_height = m_fnt_height;
		c_menu* menu = new c_menu(this , p);
		menu->m_skinid = m_skinid;
		menu->m_fntid = m_fntid;
		if(m_items[idx_].sub_menu)
		{
			delete m_items[idx_].sub_menu;
			m_items[idx_].sub_menu = nullptr;
		}
		menu->m_interaction = true;
		menu->m_no_clip = true;
		for(u32 i = 0 ; i < UC_NUM ;++i)
			menu->m_bg_img[i] = m_bg_img[i];
		m_items[idx_].sub_menu = menu;
		_hide_sub_menu(menu);
		menu->set_visible(false);
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == menu)
			c_ui_manager::get_singleton_ptr()->set_focus( this );
		_update_size();
		return menu;
	}
	//--------------------------------------------------------
	c_menu* c_menu::get_sub_menu(u32 idx_) const
	{
		if(idx_ >= m_items.size())
			return 0;
		return m_items[idx_].sub_menu;
	}
	//--------------------------------------------------------
	void c_menu::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	}
	//--------------------------------------------------------
	void c_menu::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_menu::_on_touch_move()
	{
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() != this)
			return;
		_select(c_input_manager::get_singleton_ptr()->get_touch_pos() , true);
	}
	//--------------------------------------------------------
	void c_menu::_on_touch_finish()
	{
		u32 t = _send_click(c_input_manager::get_singleton_ptr()->get_touch_pos());
		if ((t==0 || t==1) && c_ui_manager::get_singleton_ptr()->get_focus_widget() == this)
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	bool c_menu::_lost_focus(c_widget* widget_)
	{
		if (!check_my_child(widget_))
		{
			m_parent->_remove_child(this);
			c_ui_manager::get_singleton_ptr()->reset_hovered();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
			return false;
		}
		return c_widget::_lost_focus(widget_);
	}
	//--------------------------------------------------------
	bool c_menu::_gain_focus(c_widget* widget_)
	{
		if(m_parent)
			m_parent->bring_to_front(this);
		if(m_interaction)
		{
			c_widget::_gain_focus(widget_);
			return true;
		}
		else
			return c_widget::_gain_focus(widget_);
	}
	//--------------------------------------------------------
	void c_menu::_redraw()
	{
		if(!m_visible)
			return;
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
		c_rect rect = m_absolute_rect;
		s32 y = m_absolute_rect.lt_pt().y();
		u32 index = 0;
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->separator)
			{
				rect = m_absolute_rect;
				rect.lt_pt().y() += iter->pos_y + 3;
				rect.rb_pt().y() = rect.lt_pt().y() + 1;
				rect.lt_pt().x() += 5;
				rect.rb_pt().x() -= 5;
				c_ui_manager::get_singleton_ptr()->cache_batch(UUC_MENU_SEPARATOR , rect , rect);
				y += 10;
			}
			else
			{
				rect = m_absolute_rect;
				rect.lt_pt().y() += (*iter).pos_y;
				rect.rb_pt().y() = rect.lt_pt().y() + (*iter).dim.y();
				rect.lt_pt().x() += 20;
				if(index == m_selected && iter->enabled)
				{
					c_rect r = m_absolute_rect;
					r.rb_pt().y() = rect.rb_pt().y();
					r.lt_pt().y() = rect.lt_pt().y();
					r.rb_pt().x() -= 5;
					r.lt_pt().x() += 5;
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_MENU_SELECTING , r , r);
				}
				u32 c = (iter->enabled)?m_txt_color:c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TXT_DISABLE_CLR).rgba();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid, m_fnt_height, TA_H_LEFT|TA_V_CENTER, true, iter->context, rect, rect, c);
				if(iter->sub_menu)
				{
					c_rect arrect;
					arrect.lt_pt().x() = (rect.rb_pt().x() - rect.height());
					arrect.lt_pt().y() = rect.lt_pt().y();
					arrect.rb_pt().x() = rect.rb_pt().x();
					arrect.rb_pt().y() = rect.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_arrow_img , arrect , arrect, m_flipx, m_flipy);
				}
			}
			++index;
		}
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	u32 c_menu::_send_click(const c_float2& pt_)
	{
		u32 t = 0;
		s32 openmenu = -1;
		s32 j = 0;
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->sub_menu && iter->sub_menu->is_visible())
			{
				openmenu = j;
				break;
			}
			++j;
		}
		if(openmenu != -1)
		{
			t = m_items[j].sub_menu->_send_click(pt_);
			if (t != 0)
				return t;
		}
		if(rect_contains(m_absolute_rect , pt_) && (u32)m_selected < m_items.size())
		{
			if(!m_items[m_selected].enabled ||m_items[m_selected].separator ||m_items[m_selected].sub_menu)
				return 2;
			_sig_item_selected.emit(m_items[m_selected].cmd);
			return 1;
		}
		return 0;
	}
	//--------------------------------------------------------
	void c_menu::_update_size()
	{
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(!ft)
			return;
		c_rect rect;
		rect.lt_pt() = m_relative_rect.lt_pt();
		u32 width = 100;
		u32 height = 3;
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->separator)
			{
				iter->dim.x() = 100;
				iter->dim.y() = 10;
			}
			else
			{
				ft->measure_text(iter->context , iter->dim.x() , iter->dim.y());
				iter->dim.x() += 40;
				if(iter->dim.x() > width)
					width = iter->dim.x();
			}
			iter->pos_y = height;
			height += iter->dim.y();
		}
		height += 5;
		if(height < 10)
			height = 10;
		rect.rb_pt().x() = m_relative_rect.lt_pt().x() + width;
		rect.rb_pt().y() = m_relative_rect.lt_pt().y() + height;
		_relative_position(rect);
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->sub_menu)
			{
				const s32 w = iter->sub_menu->get_region().width();
				const s32 h = iter->sub_menu->get_region().height();
				c_rect subrect(width-5, iter->pos_y, width+w-5, iter->pos_y+h);
				c_widget* root = c_ui_manager::get_singleton_ptr()->get_environment();
				if(root)
				{
					c_rect ro( root->get_region() );
					if(m_absolute_rect.lt_pt().x() + subrect.rb_pt().x() > ro.rb_pt().x())
					{
						subrect.lt_pt().x() = -w;
						subrect.rb_pt().x() = 0;
					}
				}
				iter->sub_menu->_relative_position(subrect);
			}
		}
	}
	//--------------------------------------------------------
	bool c_menu::_select(const c_float2& p_, bool canopensubmenu_)
	{
		s32 openmenu = -1;
		s32 i = 0;
		s32 lastsel = m_selected;
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->enabled && iter->sub_menu && iter->sub_menu->is_visible())
			{
				openmenu = i;
				break;
			}
			++i;
		}
		if(openmenu != -1)
		{
			if(m_items[openmenu].enabled && m_items[openmenu].sub_menu->_select(p_, canopensubmenu_))
			{
				m_selected = openmenu;
				if(m_selected!=lastsel)
					c_ui_manager::get_singleton_ptr()->set_dirty(true);
				return true;
			}
		}
		i = 0;
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , m_items)
		{
			if(iter->enabled && rect_contains(_get_selected_rect(*iter, m_absolute_rect) , p_))
			{
				m_selected = i;
				s32 j = 0;
				FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter2 , m_items)
				{
					if(iter2->sub_menu)
					{
						if(j == i && canopensubmenu_ && iter2->enabled )
						{
							_hide_sub_menu(iter2->sub_menu);
							iter2->sub_menu->set_visible(true);
						}
						else if(j != i)
						{
							_hide_sub_menu(iter2->sub_menu);
							iter2->sub_menu->set_visible(false);
						}
						else;
						c_ui_manager::get_singleton_ptr()->set_dirty(true);
					}
					if(m_selected!=lastsel)
						c_ui_manager::get_singleton_ptr()->set_dirty(true);
					++j;
				}
				return true;
			}
			++i;
		}
		m_selected = openmenu;
		if(m_selected!=lastsel)
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return false;
	}
	//--------------------------------------------------------
	c_rect c_menu::_get_selected_rect(const s_menu_item& i_, const c_rect& absolute_) const
	{
		c_rect r = absolute_;
		r.lt_pt().y() += i_.pos_y;
		r.rb_pt().y() = r.lt_pt().y() + i_.dim.y();
		return r;
	}
	//--------------------------------------------------------
	void c_menu::_hide_sub_menu(c_menu* menu_)
	{
		m_selected = -1;
		FOREACH(TYPEOF(c_vector<s_menu_item>)::iterator , iter , menu_->m_items)
		{
			if(iter->sub_menu)
			{
				_hide_sub_menu(iter->sub_menu);
				iter->sub_menu->set_visible(false);
			}
		}
	}
}

