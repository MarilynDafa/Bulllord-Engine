/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "ResMgr.h"
#include "TreeBox.h"
namespace o2d{
	c_treebox::c_treebox(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_TREEBOX , parent_, property_) , 
		m_root(nullptr) ,  
		m_selected(nullptr) ,
		m_item_height(0) ,
		m_indent_width(0) ,
		m_total_height(0) ,
		m_total_width(0) ,
		m_xoffset(12) , 
		m_scrollpos(0) , 
		m_txt_color(0xFFFFFFFF) , 
		m_lines_visible(true)
	{
		m_root = new s_tree_node;
		m_root->parent = nullptr;
		m_root->expanded = true;
		_recalculate_item_height();
	}
	//--------------------------------------------------------
	c_treebox::~c_treebox()
	{
		clear_children(m_root);
		delete m_root;
	}
	//--------------------------------------------------------
	void c_treebox::set_xoffset(s32 off_)
	{
		if(m_xoffset != off_)
		{
			m_xoffset = off_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_treebox::set_lines_visible(bool vis_)
	{
		if(m_lines_visible != vis_)
		{
			m_lines_visible = vis_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_treebox::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_treebox::set_bg_image(const c_string& name_)
	{
		if(m_bg_img!= name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_treebox::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_treebox::set_selected(s_tree_node* node_, bool selected_)
	{
		if(selected_)
			m_selected = node_;
		else
		{
			if(m_selected == node_)
				m_selected = nullptr;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_treebox::set_expanded(s_tree_node* node_, bool expanded_)
	{
		node_->expanded = expanded_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	c_treebox::s_tree_node* c_treebox::add_child(const c_string& text_, const utf16* icon_, s_tree_node* parent_, void* data_ /*= nullptr*/)
	{
		if(!parent_)
			return nullptr;
		s_tree_node* newchild = new s_tree_node;
		parent_->children.push_back(newchild);
		newchild->text = text_;
		newchild->icon = icon_?(c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(icon_ , false):nullptr;
		newchild->data = data_;
		newchild->parent = parent_;
		newchild->expanded = false;
		_recalculate_item_height();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return newchild;
	}
	//--------------------------------------------------------
	void c_treebox::remove_child(s_tree_node* parent_, s_tree_node* child_)
	{
		FOREACH(TYPEOF(c_list<s_tree_node*>)::iterator , iter , parent_->children)
		{
			if(child_ == *iter)
			{
				if(child_ == m_selected)
					set_selected(child_, false);
				clear_children(child_);
				delete *iter;
				parent_->children.erase(iter);
				break;
			}
		}
		_recalculate_item_height();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_treebox::clear_children(s_tree_node* parent_)
	{
		FOREACH(TYPEOF(c_list<s_tree_node*>)::iterator , iter , parent_->children)
		{
			if(*iter == m_selected)
				set_selected(*iter, false);
			clear_children(*iter);
			delete *iter;
		}
		parent_->children.clear();
		_recalculate_item_height();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	bool c_treebox::move_up(s_tree_node* parent_, s_tree_node* child_)
	{
		c_list<s_tree_node*>::iterator itother;
		s_tree_node* tmp;
		bool moved = false;
		FOREACH(TYPEOF(c_list<s_tree_node*>)::iterator , itchild , parent_->children)
		{
			if(child_ == *itchild)
			{
				if(itchild != parent_->children.begin())
				{
					tmp = *itchild;
					*itchild = *itother;
					*itother = tmp;
					moved = true;
				}
				break;
			}
			itother = itchild;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return moved;
	}
	//--------------------------------------------------------
	bool c_treebox::move_down(s_tree_node* parent_, s_tree_node* child_)
	{
		c_list<s_tree_node*>::iterator itother;
		s_tree_node* tmp;
		bool moved = false;
		FOREACH(TYPEOF(c_list<s_tree_node*>)::iterator , itchild , parent_->children)
		{
			if(child_ == *itchild)
			{
				if(*itchild != parent_->children.back())
				{
					itother = itchild;
					++itother;
					tmp = *itchild;
					*itchild = *itother;
					*itother = tmp;
					moved = true;
				}
				break;
			}
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return moved;
	}
	//--------------------------------------------------------
	s32 c_treebox::get_level(s_tree_node* node_) const
	{
		if(node_->parent)
			return get_level(node_->parent) + 1;
		else
			return 0;
	}
	//--------------------------------------------------------
	void c_treebox::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_treebox::_on_touch_start()
	{
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == this &&!rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
			return;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		c_ui_manager::get_singleton_ptr()->set_focus(this);
	}
	//--------------------------------------------------------
	void c_treebox::_on_touch_finish()
	{
		s32 xpos = c_input_manager::get_singleton_ptr()->get_touch_pos().x();
		s32 ypos = c_input_manager::get_singleton_ptr()->get_touch_pos().y();
		s_tree_node* oldselected = m_selected;
		s_tree_node* hitnode = nullptr;
		s32 selidx=-1;
		s32 n;
		s_tree_node* node;
		xpos -= m_absolute_rect.lt_pt().x();
		ypos -= m_absolute_rect.lt_pt().y();
		if(m_item_height != 0)
			selidx = (ypos - 1 + m_scrollpos)/m_item_height;
		hitnode = nullptr;
		node = _get_first_child(m_root);
		n = 0;
		while(node)
		{
			if(selidx == n)
			{
				hitnode = node;
				break;
			}
			node = _travel(node);
			++n;
		}
		if(hitnode && xpos > get_level(hitnode) * m_indent_width +m_xoffset)
			m_selected = hitnode;
		if(hitnode && xpos < get_level(hitnode) * m_indent_width + m_xoffset
			&& xpos > (get_level(hitnode) - 1) * m_indent_width + m_xoffset
			&& !hitnode->children.empty())
		{
			hitnode->expanded = (!hitnode->expanded);
			if(hitnode->expanded)
				_sig_node_expand.emit(hitnode);
			else
				_sig_node_collaps.emit(hitnode);
		}
		if(m_selected && !_is_visible(m_selected))
			m_selected = 0;
		if(m_selected != oldselected)
		{
			if(oldselected)
				_sig_node_selected.emit(oldselected);
			if(m_selected)
				_sig_node_selected.emit(m_selected);
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_treebox::_on_pan()
	{
		f32 delta = c_input_manager::get_singleton_ptr()->get_pan();
		m_scrollpos  = m_scrollpos + (delta < 0.f ? -1 : 1)*-10.f;
		m_scrollpos = clamp(m_scrollpos , 0 , m_total_height - m_absolute_rect.height());
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_treebox::_redraw()
	{
		if(!m_visible)
			return;
		c_rect rc;
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.lt_pt().y() = m_absolute_rect.lt_pt().y();
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.lt_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.lt_pt().y();
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
		rc.rb_pt().y() = m_bg_zoom_region.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_absolute_rect.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.lt_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
		rc.rb_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
		rc.lt_pt().x() = m_bg_zoom_region.rb_pt().x();
		rc.lt_pt().y() = m_bg_zoom_region.rb_pt().y();
		rc.rb_pt().x() = m_absolute_rect.rb_pt().x();
		rc.rb_pt().y() = m_absolute_rect.rb_pt().y();
		c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid , m_bg_img, m_bg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
		c_rect clientclip(m_absolute_rect);
		clientclip.lt_pt().y() += 2.f;
		clientclip.lt_pt().x() += 1.f;
		clientclip.rb_pt().x() = m_absolute_rect.rb_pt().x() - 1.f;
		clientclip.rb_pt().y() -= 2.f;
		c_rect framer = m_absolute_rect;
		framer.rb_pt().y() = m_absolute_rect.lt_pt().y() + m_item_height;
		framer.lt_pt().y() -= m_scrollpos;
		framer.rb_pt().y() -= m_scrollpos;
		s_tree_node* node = _get_first_child(m_root);
		while(node)
		{
			framer.lt_pt().x() = m_absolute_rect.lt_pt().x()+ m_xoffset + get_level(node) * m_indent_width;
			if(framer.rb_pt().y() >= m_absolute_rect.lt_pt().y() && framer.lt_pt().y() <= m_absolute_rect.rb_pt().y())
			{
				if(node == m_selected)
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_SELECTING, framer, clientclip);
				if(!node->children.empty())
				{
					c_rect rc;
					c_rect expander;   
					expander.lt_pt().x() = framer.lt_pt().x() - m_indent_width + 2.f;
					expander.lt_pt().y() = framer.lt_pt().y() + (((s32)framer.height() - (m_indent_width - 4)) >> 1);
					expander.rb_pt().x() = expander.lt_pt().x() + m_indent_width - 4.f;
					expander.rb_pt().y() = expander.lt_pt().y() + m_indent_width - 4.f;
					rc.lt_pt().x() = expander.lt_pt().x();
					rc.lt_pt().y() = expander.lt_pt().y();
					rc.rb_pt().x() = expander.rb_pt().x();
					rc.rb_pt().y() = rc.lt_pt().y() + 1.f;
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE, rc, clientclip);
					rc.lt_pt().x() = expander.lt_pt().x();
					rc.lt_pt().y() = expander.lt_pt().y();
					rc.rb_pt().x() = rc.lt_pt().x() + 1.f;
					rc.rb_pt().y() = expander.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE, rc, clientclip);
					rc.lt_pt().x() = expander.rb_pt().x() - 1.f;
					rc.lt_pt().y() = expander.lt_pt().y();
					rc.rb_pt().x() = rc.lt_pt().x() + 1.f;
					rc.rb_pt().y() = expander.rb_pt().y();
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE, rc, clientclip);
					rc.lt_pt().x() = expander.lt_pt().x();
					rc.lt_pt().y() = expander.rb_pt().y() - 1.f;
					rc.rb_pt().x() = expander.rb_pt().x();
					rc.rb_pt().y() = rc.lt_pt().y() + 1.f;
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE, rc, clientclip);
					rc.lt_pt().x() = expander.lt_pt().x() + 2.f;
					rc.lt_pt().y() = expander.lt_pt().y() + ((s32)expander.height() >> 1);
					rc.rb_pt().x() = rc.lt_pt().x()+ expander.width() - 4.f;
					rc.rb_pt().y() = rc.lt_pt().y() + 1.f;
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE, rc, clientclip);
					if(!node->expanded)
					{
						rc.lt_pt().x() = expander.lt_pt().x() + ((s32)expander.width() >> 1);
						rc.lt_pt().y() = expander.lt_pt().y() + 2.f;
						rc.rb_pt().x() = rc.lt_pt().x() + 1.f;
						rc.rb_pt().y() = rc.lt_pt().y() + expander.height() - 4.f;
						c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE, rc, clientclip);
					}
				}
				c_rect textr = framer;
				s32 iconw = 0;
				if(node->icon)
				{
					c_rect tmp = textr;
					tmp.rb_pt().x() = tmp.lt_pt().x() + m_fnt_height + 2.f;
					tmp.rb_pt().y() = tmp.lt_pt().y() + m_fnt_height + 2.f;
					c_ui_manager::get_singleton_ptr()->cache_batch(node->icon, tmp, clientclip, m_flipx, m_flipy);
					textr.lt_pt().x() += m_fnt_height + 2.f;
				}
				c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid, m_fnt_height, TA_V_CENTER|TA_H_LEFT, true, node->text, textr, clientclip, m_txt_color);
				textr.lt_pt().x()-= iconw;
				if(m_lines_visible)
				{
					c_rect rc;
					rc.lt_pt().x() = framer.lt_pt().x() - m_indent_width - (m_indent_width >> 1) - 1.f;
					rc.lt_pt().y() = framer.lt_pt().y() + (((s32)framer.height() ) >> 1);
					if(!node->children.empty())
						rc.rb_pt().x() = framer.lt_pt().x()- m_indent_width;
					else
						rc.rb_pt().x() = framer.lt_pt().x() - 2.f;
					rc.rb_pt().y() = rc.lt_pt().y() + 1.f;
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE , rc , clientclip);
					if(node->parent != m_root)
					{
						if(node == _get_first_child(node->parent))
							rc.lt_pt().y() = framer.lt_pt().y() - (((s32)framer.height() - m_indent_width ) >> 1);
						else
							rc.lt_pt().y() = framer.lt_pt().y() - ((s32)framer.height() >> 1);
						rc.rb_pt().x()= rc.lt_pt().x() + 1.f;
						c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE , rc , clientclip);
						s_tree_node* nodetmp = node->parent;
						rc.lt_pt().y() = framer.lt_pt().y() - ((s32)framer.height() >> 1);
						for(s32 n = 0; n < get_level(node) - 2; ++n)
						{
							rc.lt_pt().x()-= m_indent_width;
							rc.rb_pt().x()-= m_indent_width;
							if(nodetmp != _get_last_child(nodetmp->parent))
								c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TREE_LINE , rc , clientclip);
							nodetmp = nodetmp->parent;
						}
					}
				}
			}
			framer.lt_pt().y() += m_item_height;
			framer.rb_pt().y() += m_item_height;
			node = _travel(node);
		}
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	bool c_treebox::_is_visible(s_tree_node* node_)
	{
		if(node_->parent)
			return node_->parent->expanded && _is_visible(node_->parent);
		else
			return true;
	}
	//--------------------------------------------------------
	void c_treebox::_recalculate_item_height()
	{
		s_tree_node* node;
		m_item_height = m_fnt_height + 4;
		m_indent_width = m_item_height;
		if(m_indent_width < 10)
			m_indent_width = 10;
		else if(m_indent_width > 16)
			m_indent_width = 16;
		else
		{
			if(((m_indent_width >> 1 ) << 1) - m_indent_width == 0)
				--m_indent_width;
		}
		m_total_height = 0;
		m_total_width = m_absolute_rect.width() * 2.f;
		node = _get_first_child(m_root);
		while(node)
		{
			m_total_height += m_item_height;
			node = _travel(node);
		}
	}	
	//--------------------------------------------------------
	c_treebox::s_tree_node* c_treebox::_get_first_child(s_tree_node* node_) const
	{
		if(node_->children.empty())
			return nullptr;
		else
			return node_->children.front();
	}
	//--------------------------------------------------------
	c_treebox::s_tree_node* c_treebox::_get_last_child(s_tree_node* node_) const
	{
		if(node_->children.empty())
			return nullptr;
		else
			return node_->children.back();
	}
	//--------------------------------------------------------
	c_treebox::s_tree_node* c_treebox::_get_prev_sibling(s_tree_node* node_) const
	{
		c_list<s_tree_node*>::iterator itother;
		s_tree_node* other = nullptr;
		if(node_->parent)
		{
			FOREACH(TYPEOF(c_list<s_tree_node*>)::iterator , itthis , node_->parent->children)
			{
				if(node_ == *itthis)
				{
					if(itthis != node_->parent->children.begin())
						other = *itother;
					break;
				}
				itother = itthis;
			}
		}
		return other;
	}
	//--------------------------------------------------------
	c_treebox::s_tree_node* c_treebox::_get_next_sibling(s_tree_node* node_) const
	{
		s_tree_node* other = nullptr;
		if(node_->parent)
		{
			FOREACH(TYPEOF(c_list<s_tree_node*>)::iterator , itthis , node_->parent->children)
			{
				if(node_ == *itthis)
				{
					if(*itthis != node_->parent->children.back())
						other = *(++itthis);
					break;
				}
			}
		}
		return other;
	}
	//--------------------------------------------------------
	c_treebox::s_tree_node* c_treebox::_travel(s_tree_node* node_) const
	{
		s_tree_node* next = nullptr;
		s_tree_node* node = node_;
		if(node->expanded && !node->children.empty())
			next = _get_first_child(node);
		else
			next = _get_next_sibling(node);
		while(!next && node->parent)
		{
			next = _get_next_sibling(node->parent);
			if(!next)
				node = node->parent;
		}
		return next;
	}
}