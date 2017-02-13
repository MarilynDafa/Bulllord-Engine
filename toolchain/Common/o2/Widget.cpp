/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Widget.h"
namespace o2d{
	c_widget::c_widget(e_widget_type type_, c_widget* parent_, const s_widget_property& property_) 
		: m_parent(parent_) , 
		m_relative_rect(property_.dim) ,
		m_absolute_rect(property_.dim) ,
		m_clipping_rect(property_.dim) ,
		m_desired_rect(property_.dim) ,
		m_max_size(0,0) ,
		m_min_size(1,1) ,
		m_class(type_) ,
		m_id(-1) ,
		m_skinid(-1) ,
		m_fntid(-1) , 
		m_fnt_height(property_.font_height) , 
		m_need_delete(false) ,
		m_visible(true) ,
		m_no_clip(false) , 
		m_transmit(false) , 
		m_penetrate(false), 
		m_abs_scissor(false),
		m_flipx(false), 
		m_flipy(false)
	{
		m_batch_gbo = nullptr;
		m_id = hash((const char*)property_.name.to_utf8());
		m_skinid = property_.skinid;
		m_fntid = property_.fntid;
		if(m_parent)
		{
			FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_parent->m_children)
			{
				if((*iter) == this)
				{
					m_parent->m_children.erase(iter);
					break;
				}
			}
			m_parent->m_children.push_back(this);
			_recalculate_region(true);
		}
		c_font* fnt = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(fnt)
		{
			fnt->serialize_height(m_fnt_height);
			fnt->active_height(m_fnt_height);
		}
	}
	//--------------------------------------------------------
	c_widget::~c_widget()
	{
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
		{
			(*iter)->m_parent = nullptr;
			delete *iter;
		}
	}
	//--------------------------------------------------------
	void c_widget::set_visible(bool visible_)
	{
		m_visible = visible_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_widget::set_no_clipped(bool noclip_)
	{
		if(m_no_clip!=noclip_)
		{
			m_no_clip = noclip_;
			_update_position();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_transmit(bool trans_)
	{
		if(m_transmit!=trans_)
		{
			m_transmit = trans_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_penetrate(bool per_)
	{
		if(m_penetrate!=per_)
		{
			m_penetrate = per_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_abs(bool abs_)
	{
		if (m_abs_scissor != abs_)
		{
			m_abs_scissor = abs_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_max_size(const c_float2& size_)
	{
		if(m_max_size!=size_)
		{
			m_max_size = size_;
			_update_position();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_min_size(const c_float2& size_)
	{
		if(m_min_size!=size_)
		{
			m_min_size = size_;
			if (m_min_size.x() < 1)
				m_min_size.x() = 1;
			if (m_min_size.y() < 1)
				m_min_size.y() = 1;
			_update_position();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_tooltip(const c_string& text_)
	{
		if(m_tooltip != text_)
		{
			m_tooltip = text_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_skin(const c_string& skiname_)
	{
		if(m_skinid != hash(skiname_.c_str()))
		{
			m_skinid = hash(skiname_.c_str());
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_font(const c_string& fntname_, u32 height_, bool added_/* = false*/, bool outline , bool shadow, bool bold, bool intly)
	{
		//if(m_fntid != hash(fntname_.c_str()) || m_fnt_height != height_)
		{
			m_fntid = hash(fntname_.c_str());
			m_fnt_height = height_;
			m_txt_flag = 0;
			m_txt_flag = 0;
			u16 flag = 0;
			if (outline)
				flag |= 0x000F;
			if (bold)
				flag |= 0x00F0;
			if (shadow)
				flag |= 0x0F00;
			if (intly)
				flag |= 0xF000;
			m_txt_flag = flag;
			if(added_)
			{
				c_font* fnt = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
				fnt->serialize_height(height_);
				fnt->active_height(height_);
			}
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_widget::set_property(const s_widget_property& property_)
	{
		m_id = hash(property_.name.c_str());
		m_skinid = property_.skinid;
		m_fntid = property_.fntid;
		m_fnt_height = property_.font_height;
		m_relative_rect = property_.dim;
		m_absolute_rect = property_.dim;
		m_clipping_rect = property_.dim;
		m_desired_rect = property_.dim;
		if(m_parent)
			_recalculate_region(true);
	}
	//--------------------------------------------------------
	void c_widget::move(const c_float2& absolutemovement_)
	{
		c_rect r = m_desired_rect;
		r.lt_pt()+=absolutemovement_;
		r.rb_pt()+=absolutemovement_;
		_relative_position(r);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_widget::resize(f32 w_, f32 h_)
	{
		c_rect r = m_desired_rect;
		c_float2 vec = c_float2(w_*0.5 - r.width()*0.5, h_*0.5 - r.height()*0.5);
		c_float2 midpt = (r.lt_pt() + r.rb_pt())*0.5;
		r.lt_pt().x() = midpt.x() - w_*0.5;
		r.lt_pt().y() = midpt.y() - h_*0.5;
		r.rb_pt().x() = midpt.x() + w_*0.5;
		r.rb_pt().y() = midpt.y() + h_*0.5;
		_relative_position(r);
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator, iter, m_children)
		{
			(*iter)->move(vec);
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	bool c_widget::bring_to_front(c_widget* element_)
	{
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
		{
			if (element_ == (*iter))
			{
				m_children.erase(iter);
				m_children.push_back(element_);
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
				return true;
			}
		}
		return false;
	}
	//--------------------------------------------------------
	bool c_widget::check_my_child(c_widget* child_) const
	{
		if(!child_)
			return false;
		do
		{
			if(child_->m_parent)
				child_ = child_->m_parent;

		} while (child_->m_parent && child_ != this);
		return child_ == this;
	}
	//--------------------------------------------------------
	void c_widget::set_flipx(bool flag)
	{
		m_flipx = flag;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_widget::set_flipy(bool flag)
	{
		m_flipy = flag;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	c_widget* c_widget::get_widget(const c_float2& point_)
	{
		c_widget* target = nullptr;
		RFOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
		if(is_visible())
		{
			target = (*iter)->get_widget(point_);
			if(target)
				return target;
		}
		if(m_visible && rect_contains(m_clipping_rect , point_))
			target = this;
		return target;
	}
	//--------------------------------------------------------
	c_widget* c_widget::get_widget(s32 id_, bool searchchildren_/*=false*/) const
	{
		c_widget* e = nullptr;
		FOREACH(TYPEOF(c_list<c_widget*>)::const_iterator , iter , m_children)
		{
			if((*iter)->get_id() == id_)
				return (*iter);
			if(searchchildren_)
				e = (*iter)->get_widget(id_, true);
			if(e)
				return e;
		}
		if(!e)
			LOG(LL_ERROR , "can not find the widget id %d" , id_);
		return e;
	}
	//--------------------------------------------------------
	void c_widget::_update_position()
	{
		_recalculate_region(false);
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
			(*iter)->_update_position();
	}
	//--------------------------------------------------------
	void c_widget::_zoom_update(const c_float2& v1_, const c_float2& v2_){}
	//--------------------------------------------------------
	bool c_widget::_gain_focus(c_widget* widget_)
	{
		_sig_gain_foucs.emit();
		return false;
	}
	//--------------------------------------------------------
	bool c_widget::_lost_focus(c_widget* widget_)
	{
		_sig_lost_foucs.emit();
		return false;
	}
	//--------------------------------------------------------
	void c_widget::_on_touch_move(){}
	//--------------------------------------------------------
	void c_widget::_on_touch_start(){}
	//--------------------------------------------------------
	void c_widget::_on_touch_finish(){}
	//--------------------------------------------------------
	void c_widget::_on_double_tap(){}
	//--------------------------------------------------------
	void c_widget::_on_pan(){}
	//--------------------------------------------------------
	void c_widget::_on_char(){}
	//--------------------------------------------------------
	void c_widget::_redraw()
	{
		if(m_visible)
		{
			c_list<c_widget*> tmplst;
			while(m_children.size())
			{
				c_widget* wid = m_children.pop_front();
				if(wid->m_need_delete)
				{
					c_ui_manager::get_singleton_ptr()->reset_hovered();
					wid->m_parent = nullptr;
					delete wid;
				}
				else
				{
					tmplst.push_back(wid);
					wid->_redraw();
				}
			}
			m_children = tmplst;
		}
	}
	//--------------------------------------------------------
	void c_widget::_update(c_technique* uit_, u32 dt_)
	{
		if(m_visible)
		{
			FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
				(*iter)->_update(uit_, dt_);
		}
	}
	//--------------------------------------------------------
	void c_widget::_add_child(c_widget* child_)
	{
		if(child_)
		{
			if(child_->m_parent)
			{
				FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , child_->m_parent->m_children)
				{
					if((*iter) == child_)
					{
						child_->m_parent->m_children.erase(iter);
						break;
					}
				}
			}
			child_->m_parent = this;
			m_children.push_back(child_);
			child_->_update_position();
		}
	}
	//--------------------------------------------------------
	void c_widget::_remove_child(c_widget* child_)
	{
		FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
		{
			if((*iter) == child_)
			{
				(*iter)->m_parent = nullptr;
				m_children.erase(iter);
				delete child_;
				return;
			}
		}
	}
	//--------------------------------------------------------
	void c_widget::_recalculate_region(bool recursive_)
	{
		c_rect parentabs(0.f ,0.f ,0.f ,0.f);
		c_rect parentabsclip;
		if(m_parent)
		{
			parentabs = m_parent->m_absolute_rect;
			if(m_no_clip)
			{
				c_widget* p=this;
				while (p && p->m_parent)
				{
					p = p->m_parent;
					if (p->m_abs_scissor)
						break;
				}
				parentabsclip = p->m_clipping_rect;
			}
			else
				parentabsclip = m_parent->m_clipping_rect;
		}
		m_relative_rect = m_desired_rect;
		s32 w = m_relative_rect.width();
		s32 h = m_relative_rect.height();
		if(w < (s32)m_min_size.x())
			m_relative_rect.rb_pt().x() = m_relative_rect.lt_pt().x() + m_min_size.x();
		if(h < (s32)m_min_size.y())
			m_relative_rect.rb_pt().y() = m_relative_rect.lt_pt().y() + m_min_size.y();
		if(m_max_size.x() && w > (s32)m_max_size.x())
			m_relative_rect.rb_pt().x() = m_relative_rect.lt_pt().x() + m_max_size.x();
		if(m_max_size.y() && h > (s32)m_max_size.y())
			m_relative_rect.rb_pt().y() = m_relative_rect.lt_pt().y() + m_max_size.y();
		if(m_relative_rect.rb_pt().x() < m_relative_rect.lt_pt().x())
		{
			f32 tmp = m_relative_rect.lt_pt().x();
			m_relative_rect.lt_pt().x() = m_relative_rect.rb_pt().x();
			m_relative_rect.rb_pt().x() = tmp;
		}
		if(m_relative_rect.rb_pt().y() < m_relative_rect.lt_pt().y())
		{
			f32 tmp = m_relative_rect.lt_pt().y();
			m_relative_rect.lt_pt().y() = m_relative_rect.rb_pt().y();
			m_relative_rect.rb_pt().y() = tmp;
		}
		c_float2 ltvec = m_relative_rect.lt_pt() + parentabs.lt_pt() - m_absolute_rect.lt_pt();
		c_float2 rbvec = m_relative_rect.rb_pt() + parentabs.lt_pt() - m_absolute_rect.rb_pt();
		_zoom_update(ltvec , rbvec);
		m_absolute_rect.lt_pt() = m_relative_rect.lt_pt() + parentabs.lt_pt();
		m_absolute_rect.rb_pt() = m_relative_rect.rb_pt() + parentabs.lt_pt();
		if(!m_parent)
			parentabsclip = m_absolute_rect;
		m_clipping_rect = m_absolute_rect;
		rect_clip(m_clipping_rect ,parentabsclip);
		if(recursive_)
		{
			FOREACH(TYPEOF(c_list<c_widget*>)::iterator , iter , m_children)
				(*iter)->_recalculate_region(recursive_);
		}
	}
	//--------------------------------------------------------
	void c_widget::_relative_position(const c_rect& r_)
	{
		m_desired_rect = r_;
		_update_position();
	}
	//--------------------------------------------------------
	void c_widget::_draw_stencil(const c_string& stencilimg, const c_rect& stencilrect)
	{
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		c_rect rc;
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x()*1.05;
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y()*1.05;
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x()*0.95;
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y()*0.95;
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, stencilimg, stencilrect, rc, m_clipping_rect, m_flipx, m_flipy);
		glStencilFunc(GL_EQUAL, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		//glDisable(GL_STENCIL_TEST);
	}
}