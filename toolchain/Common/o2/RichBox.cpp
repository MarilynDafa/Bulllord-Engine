/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "Font.h"
#include "UIMgr.h"
#include "ResMgr.h"
#include "InputMgr.h"
#include "RichBox.h"
namespace o2d{
	c_richbox::c_richbox(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_RICHBOX , parent_, property_) , 
		m_last_text_ele(nullptr) , 
		m_line_space_remaining(0) , 
		m_fullline_space(0) , 
		m_line_space(0) , 
		m_cur_line(0) , 
		m_cur_left_margin(0) , 
		m_cur_right_margin(0) , 
		m_total_height(0) ,
		m_txt_color(0xFFFFFFFF) ,
		m_cur_txt_color(0xFFFFFFFF) , 
		m_scrollpos(0) , 
		m_cur_fntid(-1) , 
		m_cur_linkid(-1) , 
		m_cur_fnt_height(0)
	{}
	//--------------------------------------------------------
	c_richbox::~c_richbox()
	{
		FOREACH(TYPEOF(c_map<u32, s_inline_img>)::iterator , iter , m_inline_imgs)
			c_resource_manager::get_singleton_ptr()->discard_resource(iter->second().img->get_filename());
		_clear_lines();
	}
	//--------------------------------------------------------
	void c_richbox::set_text(const c_string& text_)
	{
		if(m_text != text_)
		{
			m_text = text_;
			_parse(m_text);
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_richbox::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_richbox::set_bg_image(const c_string& name_)
	{
		if(m_bg_img!= name_)
		{
			m_bg_img = name_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_richbox::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_richbox::set_margin(const c_float2& margin_)
	{
		if(m_margin.x()!=margin_.x() || m_margin.y()!= margin_.y())
		{
			m_margin.x() = margin_.x();
			m_margin.y() = margin_.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_richbox::insert_image(u32 id_, const c_string& path_, const c_float2& sz_)
	{
		AUTO(TYPEOF(c_map<u32, s_inline_img>)::iterator , iter , m_inline_imgs.find(id_));
		if(iter!=m_inline_imgs.end())
		{
			delete iter->second().img;
			m_inline_imgs.erase(iter);
		}
		s_inline_img img;
		img.img = (c_texture*)c_resource_manager::get_singleton_ptr()->fetch_resource(path_.c_str(), false);
		img.size = sz_;
		m_inline_imgs.insert(id_, img);
	}

	//--------------------------------------------------------
	void c_richbox::set_stencil_image(const c_string& img_)
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
	void c_richbox::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_richbox::_on_touch_start()
	{
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == this &&!rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
		{
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
			return;
		}
		c_ui_manager::get_singleton_ptr()->set_focus(this);
	}
	//--------------------------------------------------------
	void c_richbox::_on_touch_finish()
	{
		c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
		if(!rect_contains(m_clipping_rect , pos))
		{
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
			return;
		}
		c_rect r = m_desired_rect;
		r.lt_pt()+=c_float2(0.f , -m_scrollpos);
		r.rb_pt()+=c_float2(0.f , -m_scrollpos);
		_relative_position(r);
		s32 y = m_absolute_rect.lt_pt().y() + m_margin.y();
		FOREACH(TYPEOF(c_vector<s_line*>)::iterator , iter , m_lines)
		{
			e_text_alignment halign = (*iter)->halignment;
			e_text_alignment valign = (*iter)->valignment;
			s32 width = (*iter)->width;
			s32 height = (*iter)->height;
			s32 leftx = m_absolute_rect.lt_pt().x() + m_margin.x() + (*iter)->left_margin;
			s32 rightx = m_absolute_rect.rb_pt().x() - m_margin.x() - (*iter)->right_margin;
			s32 xwidth = rightx - leftx;
			s32 x;
			switch(halign)
			{
			case TA_H_LEFT:
				x = leftx;
				break;
			case TA_H_CENTER:
				x = leftx + (xwidth - width)*0.5f;
				break;
			case TA_H_RIGHT:
				x = leftx + xwidth - width;
				break;
            default:
                x = leftx;
                break;
			}
			if (m_lines.size() == 1)
			{
				s32 lefty = m_absolute_rect.lt_pt().y() + m_margin.y();
				s32 righty = m_absolute_rect.rb_pt().y() - m_margin.y();
				s32 yheight = righty - lefty;
				switch (valign)
				{
				case TA_V_TOP:
					y = lefty;
					break;
				case TA_V_CENTER:
					y = lefty + (yheight - height)*0.5f;
					break;
				case TA_V_BOTTOM:
					y = lefty + yheight - height;
					break;
				default:
					y = lefty;
					break;
				}
			}
			FOREACH(TYPEOF(c_list<s_text_element*>)::iterator, iter2, (*iter)->elements)
			{
				s_text_element* element = (*iter2);
				c_rect dstrect = c_rect(x, y, x + element->dim.x()+4.f, y + element->dim.y());
				switch(element->type)
				{
				case 3:
					{
						if(element->texture&&rect_contains(dstrect, pos))
							_sig_click.emit(element->id);
					}
					break;
				case 4:
					{
						if(element->id != 0xFFFFFFFF&&rect_contains(dstrect, pos))
							_sig_click.emit(element->id);
					}
					break;
				default:
					break;
				}
				x += element->dim.x();
			}
			y += (*iter)->height;
		}
		FOREACH(TYPEOF(c_vector<s_anchored_element>)::iterator , iter , m_anchored_elements)
		{
			s_text_element* element = iter->element;
			s32 y = m_absolute_rect.lt_pt().y() - m_margin.y() + iter->bottomy - element->dim.y();
			s32 x;
			if(iter->is_left)
				x = m_absolute_rect.lt_pt().x() + m_margin.x();
			else
				x = m_absolute_rect.rb_pt().x() - m_margin.x() - element->dim.x();
			c_rect dstrect = c_rect(x, y, x + element->dim.x(), y + element->dim.y());
			switch(element->type)
			{
			case 3:
				{
					if(element->texture&&rect_contains(dstrect, pos))
						_sig_click.emit(element->id);
				}
				break;
			case 4:
				{
					if(element->id != 0xFFFFFFFF&&rect_contains(dstrect, pos))
						_sig_click.emit(element->id);
				}
				break;
			default:
				break;
			}
		}
		r = m_desired_rect;
		r.lt_pt()+=c_float2(0.f , m_scrollpos);
		r.rb_pt()+=c_float2(0.f , m_scrollpos);
		_relative_position(r);
	}
	//--------------------------------------------------------
	void c_richbox::_on_pan()
	{
		f32 delta = c_input_manager::get_singleton_ptr()->get_pan();
		m_scrollpos  = m_scrollpos + (delta < 0.f ? -1 : 1)*-10.f;
		m_scrollpos = clamp(m_scrollpos, 0, m_total_height - m_absolute_rect.height());
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_richbox::_redraw()
	{
		if(!m_visible)
			return;
		c_rect abstmp = m_absolute_rect;
		c_rect zoomtmp = m_bg_zoom_region;
		c_rect cliptmp = m_clipping_rect;
		c_rect r = m_desired_rect;
		r.lt_pt()+=c_float2(0.f , -m_scrollpos);
		r.rb_pt()+=c_float2(0.f , -m_scrollpos);
		_relative_position(r);
		m_clipping_rect = cliptmp;
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
				rc.lt_pt().x() = abstmp.lt_pt().x();
				rc.lt_pt().y() = abstmp.lt_pt().y();
				rc.rb_pt().x() = zoomtmp.lt_pt().x();
				rc.rb_pt().y() = zoomtmp.lt_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_LT], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = zoomtmp.lt_pt().x();
				rc.lt_pt().y() = abstmp.lt_pt().y();
				rc.rb_pt().x() = zoomtmp.rb_pt().x();
				rc.rb_pt().y() = zoomtmp.lt_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_T], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = zoomtmp.rb_pt().x();
				rc.lt_pt().y() = abstmp.lt_pt().y();
				rc.rb_pt().x() = abstmp.rb_pt().x();
				rc.rb_pt().y() = zoomtmp.lt_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_RT], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = abstmp.lt_pt().x();
				rc.lt_pt().y() = zoomtmp.lt_pt().y();
				rc.rb_pt().x() = zoomtmp.lt_pt().x();
				rc.rb_pt().y() = zoomtmp.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_L], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = zoomtmp.lt_pt().x();
				rc.lt_pt().y() = zoomtmp.lt_pt().y();
				rc.rb_pt().x() = zoomtmp.rb_pt().x();
				rc.rb_pt().y() = zoomtmp.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_MID], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = zoomtmp.rb_pt().x();
				rc.lt_pt().y() = zoomtmp.lt_pt().y();
				rc.rb_pt().x() = abstmp.rb_pt().x();
				rc.rb_pt().y() = zoomtmp.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_R], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = abstmp.lt_pt().x();
				rc.lt_pt().y() = zoomtmp.rb_pt().y();
				rc.rb_pt().x() = zoomtmp.lt_pt().x();
				rc.rb_pt().y() = abstmp.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_LB], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = zoomtmp.lt_pt().x();
				rc.lt_pt().y() = zoomtmp.rb_pt().y();
				rc.rb_pt().x() = zoomtmp.rb_pt().x();
				rc.rb_pt().y() = abstmp.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_B], rc, m_clipping_rect, m_flipx, m_flipy);
				rc.lt_pt().x() = zoomtmp.rb_pt().x();
				rc.lt_pt().y() = zoomtmp.rb_pt().y();
				rc.rb_pt().x() = abstmp.rb_pt().x();
				rc.rb_pt().y() = abstmp.rb_pt().y();
				c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_bg_img, m_bg_rect[UC_RB], rc, m_clipping_rect, m_flipx, m_flipy);
			}
		}
		m_clipping_rect.rb_pt() -= c_float2(0.f , m_margin.y());
		m_clipping_rect.lt_pt() += c_float2(0.f , m_margin.y());
		s32 y = m_absolute_rect.lt_pt().y() + m_margin.y();
		FOREACH(TYPEOF(c_vector<s_line*>)::iterator , iter , m_lines)
		{
			if(y <= m_clipping_rect.rb_pt().y() && y + (*iter)->height >= m_clipping_rect.lt_pt().y())
			{
				e_text_alignment halign = (*iter)->halignment;
				e_text_alignment valign = (*iter)->valignment;
				s32 width = (*iter)->width;
				s32 height = (*iter)->height;
				s32 leftx = m_absolute_rect.lt_pt().x() + m_margin.x() + (*iter)->left_margin;
				s32 rightx = m_absolute_rect.rb_pt().x() - m_margin.x() - (*iter)->right_margin;
				s32 xwidth = rightx - leftx;
				s32 x;
				switch(halign)
				{
				case TA_H_LEFT:
					x = leftx;
					break;
				case TA_H_CENTER:
					x = leftx + (xwidth - width)*0.5f;
					break;
				case TA_H_RIGHT:
					x = leftx + xwidth - width;
                    break;
                default:
                    x = leftx;
                    break;
				}
				if (m_lines.size() == 1)
				{
					s32 lefty = m_absolute_rect.lt_pt().y() + m_margin.y();
					s32 righty = m_absolute_rect.rb_pt().y() - m_margin.y();
					s32 yheight = righty - lefty;
					switch (valign)
					{
					case TA_V_TOP:
						y = lefty;
						break;
					case TA_V_CENTER:
						y = lefty + (yheight - height)*0.5f;
						break;
					case TA_V_BOTTOM:
						y = lefty + yheight - height;
						break;
					default:
						y = lefty;
						break;
					}
				}
				FOREACH(TYPEOF(c_list<s_text_element*>)::iterator, iter2, (*iter)->elements)
				{
					s_text_element* element = (*iter2);
					c_rect dstrect = c_rect(x, y, x + element->dim.x(), y + element->dim.y());
 					switch(element->type)
 					{
 					case 1:
 						break;
 					case 2:
						{
							s32 h = dstrect.height();
							c_rect drawarea = c_rect(dstrect.lt_pt().x() ,
								dstrect.lt_pt().y() + (h - element->line_width)*0.5f ,
								dstrect.rb_pt().x() ,
								dstrect.lt_pt().y() + (h - element->line_width)*0.5f + element->line_width);
							c_ui_manager::get_singleton_ptr()->cache_batch(element->color, drawarea, m_clipping_rect);
						}
 						break;
 					case 3:
						{
							if(element->texture)
								c_ui_manager::get_singleton_ptr()->cache_batch(element->texture, dstrect, m_clipping_rect, false, false);
						}
 						break;
 					case 4:
						{
							c_ui_manager::get_singleton_ptr()->cache_batch(element->fontid, element->font_height, 
								TA_H_LEFT|TA_V_CENTER, true, element->text, dstrect, m_absolute_rect, element->color, false, element->txtflag);
						}
 						break;
					default:
						assert(0);
						break;
 					}
					x += element->dim.x();
				}
			}
			y += (*iter)->height;
		}
		FOREACH(TYPEOF(c_vector<s_anchored_element>)::iterator , iter , m_anchored_elements)
		{
			s_text_element* element = iter->element;
			y = m_absolute_rect.lt_pt().y() - m_margin.y() + iter->bottomy - element->dim.y();
			s32 x;
			if(iter->is_left)
				x = m_absolute_rect.lt_pt().x() + m_margin.x();
			else
				x = m_absolute_rect.rb_pt().x() - m_margin.x() - element->dim.x();
			c_rect dstrect = c_rect(x, y, x + element->dim.x(), y + element->dim.y());
			switch(element->type)
			{
			case 1:
				break;
			case 2:
				{
					s32 h = dstrect.height();
					c_rect drawarea = c_rect(dstrect.lt_pt().x() ,
						dstrect.lt_pt().y() + (h - element->line_width)*0.5f ,
						dstrect.rb_pt().x() ,
						dstrect.lt_pt().y() + (h - element->line_width)*0.5f + element->line_width);
					c_ui_manager::get_singleton_ptr()->cache_batch(element->color, drawarea, m_clipping_rect);
				}
				break;
			case 3:
				{
					if(element->texture)
						c_ui_manager::get_singleton_ptr()->cache_batch(element->texture, dstrect, m_clipping_rect, false, false);
				}
				break;
			case 4:
				{
					c_ui_manager::get_singleton_ptr()->cache_batch(element->fontid, element->font_height, 
						TA_H_LEFT|TA_V_CENTER, true, element->text, dstrect, m_clipping_rect, element->color);
				}
				break;
			default:
				assert(0);
				break;
			}
		}
		r = m_desired_rect;
		r.lt_pt()+=c_float2(0.f , m_scrollpos);
		r.rb_pt()+=c_float2(0.f , m_scrollpos);
		_relative_position(r);
		m_clipping_rect = cliptmp;
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	void c_richbox::_parse(const c_string& text_)
	{
		m_cur_fntid = m_fntid;
		m_cur_fnt_height = m_fnt_height;
		m_cur_txt_color = m_txt_color;
		m_cur_left_margin = 0;
		m_cur_right_margin = 0;
		m_total_height = m_margin.y()*2;
		m_fullline_space = m_relative_rect.width() - m_margin.x()*2;
		m_line_space = m_fullline_space;
		m_line_space_remaining = m_line_space;
		m_cur_line = 0;
		_clear_lines();
		s_line* lin = new s_line;
		lin->halignment = TA_H_LEFT;
		lin->valignment = TA_V_CENTER;
		lin->height = 8;
		lin->width = 0;
		lin->left_margin = 0;
		lin->right_margin = 0;
		m_lines.push_back(lin);
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_cur_fntid);
		f32 x , y;
		ft->active_height(m_cur_fnt_height);
		ft->measure_text(L" " , x , y);
		s32 whitespacesz = x;
		ft->measure_text(L"#" , x , y);
		c_float2 squaredim = c_float2(x , y);
		m_last_text_ele = nullptr;
		c_vector<c_string> tokenparams;
		const utf16* c = text_.c_str();
		while(*c != 0)
		{
			c_string str = _parse_word(c);
			if(!str.is_empty())
			{
				ft->active_height(m_cur_fnt_height);
				ft->measure_text(str , x , y);
				_append_text(str, c_float2(x , y));
			}
			str = _parse_whitespace(c);
			if(!str.is_empty())
			{
				s32 size = whitespacesz * str.get_length();
				if(m_line_space_remaining < size)
					_insert_line_break();
				else
					_append_text(str, c_float2(size, 1));
			}
			s32 numlb = _parse_line_break(c);
			for(s32 i=0; i<numlb; i++)
				_insert_line_break();
			tokenparams.clear();
			if(_parse_token(c, str, tokenparams))
			{
				if(str == L"")
					_append_text(L"#", squaredim);
				else
				{
					if(str == L"align")
					{
						e_text_alignment halign = TA_H_LEFT;
						e_text_alignment valign = TA_V_TOP;
						if(tokenparams.size() > 0)
						{
							if (tokenparams[0] == L"LT")
							{
								halign = TA_H_LEFT;
								valign = TA_V_TOP;
							}
							else if (tokenparams[0] == L"LM")
							{
								halign = TA_H_LEFT;
								valign = TA_V_CENTER;
							}
							else if (tokenparams[0] == L"LB")
							{
								halign = TA_H_LEFT;
								valign = TA_V_BOTTOM;
							}
							else if (tokenparams[0] == L"MT")
							{
								halign = TA_H_CENTER;
								valign = TA_V_TOP;
							}
							else if (tokenparams[0] == L"MM")
							{
								halign = TA_H_CENTER;
								valign = TA_V_CENTER;
							}
							else if (tokenparams[0] == L"MB")
							{
								halign = TA_H_CENTER;
								valign = TA_V_BOTTOM;
							}
							else if (tokenparams[0] == L"RT")
							{
								halign = TA_H_RIGHT;
								valign = TA_V_TOP;
							}
							else if (tokenparams[0] == L"RM")
							{
								halign = TA_H_RIGHT;
								valign = TA_V_CENTER;
							}
							else
							{
								halign = TA_H_RIGHT;
								valign = TA_V_BOTTOM;
							}
						}
						if(m_lines[m_cur_line]->halignment != halign || m_lines[m_cur_line]->valignment != valign)
						{
							if(m_lines[m_cur_line]->width > 0)
								_insert_line_break();
							m_lines[m_cur_line]->halignment = halign;
							m_lines[m_cur_line]->valignment = valign;
						}
					}
					else if(str == L"color")
					{
						if(tokenparams.empty())
						{
							m_cur_linkid = -1;
							m_cur_txt_color = m_txt_color;
							m_last_text_ele = nullptr;
						}
						else
						{
							m_cur_txt_color = tokenparams[0].to_uinteger();
							m_cur_linkid = tokenparams[1].to_uinteger();
							m_last_text_ele = nullptr;
						}
					}
					else if(str == L"font")
					{
						if(tokenparams.empty())
						{
							m_cur_fntid = m_fntid;
							m_cur_fnt_height = m_fnt_height;
							m_last_text_ele = nullptr;
						}
						else
						{
							m_cur_fntid = hash(tokenparams[0].c_str());
							m_cur_fnt_height = tokenparams[1].to_integer();
							m_cur_txt_flag = tokenparams[2].to_integer();
							m_last_text_ele = nullptr;
						}
					}
					else if(str == L"line")
					{
						s32 size = 1;
						if(!tokenparams.empty())
						{
							size = tokenparams[0].to_integer();
							if(size <= 0)
								size = 1;
						}
						s32 linespace = m_line_space;
						s32 lineheight = m_lines[m_cur_line]->height;
						if(lineheight < size)
							lineheight = size;
						s_text_element* element = new s_text_element;
						element->dim = c_float2(linespace, lineheight);
						element->color = m_cur_txt_color;
						element->texture = nullptr;
						element->id = -1;
						element->fontid = -1;
						element->font_height = 0;
						element->line_width = size;
						element->type = 2;
						_insert_text_element(element);
					}
					else if(str == L"image")
					{
						if(tokenparams.empty())
							return;
						u32 id = tokenparams[0].to_uinteger();
						AUTO(TYPEOF(c_map<u32, s_inline_img>)::iterator , iter , m_inline_imgs.find(id));
						if(iter != m_inline_imgs.end())
						{
							s_text_element* element = new s_text_element;
							element->texture = iter->second().img;
							element->dim = iter->second().size;
							element->id = id;
							element->fontid = -1;
							element->font_height = 0;
							element->line_width = 0;
							element->color = 0xFFFFFFFF;
							element->type = 3;
							if(tokenparams.size() > 1 && tokenparams[1] == L"left")
								_insert_anchored_element(element, true);
							else if(tokenparams.size() > 1 && tokenparams[1] == L"right")
								_insert_anchored_element(element, false);
							else
								_insert_text_element(element);
						}
					}
					else if(str == L"tab")
					{
						s32 size = 40;
						if(!tokenparams.empty())
							size = tokenparams[0].to_integer();
						s32 remaining = m_line_space_remaining;
						if(remaining <= size)
							_insert_line_break();
						else
						{
							s32 used = m_line_space - remaining;
							s32 indent = size - (used % size);
							s_text_element* element = new s_text_element;
							element->dim = c_float2(indent , 1);
							element->color = 0xFFFFFFFF;
							element->fontid = -1;
							element->id = -1;
							element->font_height = 0;
							element->line_width = 0;
							element->texture = nullptr;
							element->type = 1;
							_insert_text_element(element);
						}    
					}
				}
			}
		}
		m_relative_rect.rb_pt().y() = m_relative_rect.lt_pt().y() + m_total_height;
	}
	//--------------------------------------------------------
	c_string c_richbox::_parse_word(const utf16*& c_)
	{
		c_string str;
		if(*c_ != 0 && *c_ != L' ' && *c_ != L'\n' && *c_ != L'\r' && *c_ != L'#')
		{
			str += *c_;
			c_++;
		}
		return str;
	}
	//--------------------------------------------------------
	c_string c_richbox::_parse_whitespace(const utf16*& c_)
	{
		c_string str;
		while(*c_ == L' ')
		{
			str += *c_;
			c_++;
		}
		return str;
	}
	//--------------------------------------------------------
	bool c_richbox::_parse_token(const utf16*& c_, c_string& tokenname_, c_vector<c_string>& params_)
	{
		if(*c_ != L'#')
			return false;
		++c_;
		bool tokennamefound = false;
		c_string str;
		while(*c_ != 0 && *c_ != L'#')
		{
			if(*c_ == L':')
			{
				if(!tokennamefound)
				{
					tokennamefound = true;
					tokenname_ = str;
				}
				else
					params_.push_back(str);
				str = L"";
			}
			else
				str += *c_;
			c_++;
		}
		if(!tokennamefound)
			tokenname_ = str;
		else
			params_.push_back(str);
		if(*c_ == L'#')
		{
			c_++;
			return true;
		}
		return false;
	}
	//--------------------------------------------------------
	s32 c_richbox::_parse_line_break(const utf16*& c_)
	{
		s32 numn = 0;
		s32 numr = 0;
		while(*c_ == L'\n' || *c_ == L'\r')
		{
			if(*c_ == L'\n')
				numn++;
			else
				numr++;
			c_++;
		}
		if(numr > numn)
			return numr;
		return numn;
	}
	//--------------------------------------------------------
	void c_richbox::_append_text(const c_string& text_, const c_float2& dim_)
	{
		if(dim_.x() > m_line_space_remaining)
			_insert_line_break();
		if(m_last_text_ele)
		{
			m_last_text_ele->text += text_;
			m_last_text_ele->dim.x() += dim_.x();
			if(m_last_text_ele->dim.y() < dim_.y())
				m_last_text_ele->dim.y() = dim_.y();
			m_line_space_remaining -= dim_.x();
			m_lines[m_cur_line]->width += dim_.x();
			if(m_lines[m_cur_line]->height < dim_.y())
				m_lines[m_cur_line]->height = dim_.y();
		}
		else
		{
			m_last_text_ele = new s_text_element();
			m_last_text_ele->text = text_;
			m_last_text_ele->dim = dim_;
			m_last_text_ele->color = m_cur_txt_color;
			m_last_text_ele->id = m_cur_linkid;
			m_last_text_ele->fontid = m_cur_fntid;
			m_last_text_ele->txtflag = m_cur_txt_flag;
			m_last_text_ele->font_height = m_cur_fnt_height;
			m_last_text_ele->line_width = 0;
			m_last_text_ele->texture = nullptr;
			m_last_text_ele->type = 4;
			_insert_text_element(m_last_text_ele);
		}
	}
	//--------------------------------------------------------
	void c_richbox::_insert_text_element(s_text_element* element_)
	{
		c_float2 dim = element_->dim;
		if(dim.x() > m_line_space_remaining)
			_insert_line_break();
		m_line_space_remaining -= dim.x();
		if(dim.y() > m_lines[m_cur_line]->height)
			m_lines[m_cur_line]->height = dim.y();
		m_lines[m_cur_line]->elements.push_back(element_);
		m_lines[m_cur_line]->width += dim.x();
		m_last_text_ele = nullptr;
	}
	//--------------------------------------------------------
	void c_richbox::_insert_anchored_element(s_text_element* element_, bool left_)
	{
		c_float2 dim = element_->dim;
		if(m_fullline_space < dim.x())
			return;
		while(dim.x() > m_line_space_remaining)
			_insert_line_break();
		s_anchored_element anch;
		anch.farx = dim.x() + (left_? m_cur_left_margin : m_cur_right_margin);
		anch.bottomy = m_total_height + dim.y();
		anch.is_left = left_;
		anch.element = element_;
		m_anchored_elements.push_back(anch);
		_update_margins();
		m_lines[m_cur_line]->left_margin = m_cur_left_margin;
		m_lines[m_cur_line]->right_margin = m_cur_right_margin;
		m_line_space -= dim.x();
		m_line_space_remaining -= dim.x();
	}
	//--------------------------------------------------------
	void c_richbox::_insert_line_break()
	{
		s_line* lin = new s_line;
		lin->halignment = TA_H_LEFT;
		lin->valignment = TA_H_CENTER;
		lin->height = 8;
		lin->width = 0;
		lin->left_margin = 0;
		lin->right_margin = 0;
		m_lines.push_back(lin);
		m_cur_line++;
		m_lines[m_cur_line]->halignment = m_lines[m_cur_line - 1]->halignment;
		m_lines[m_cur_line]->valignment = m_lines[m_cur_line - 1]->valignment;
		m_total_height += m_lines[m_cur_line-1]->height;
		_update_margins();
		m_lines[m_cur_line]->left_margin = m_cur_left_margin;
		m_lines[m_cur_line]->right_margin = m_cur_right_margin;
		m_line_space = m_fullline_space - m_cur_left_margin - m_cur_right_margin;
		m_line_space_remaining = m_line_space;
		m_last_text_ele = nullptr;
	}
	//--------------------------------------------------------
	void c_richbox::_update_margins()
	{
		m_cur_left_margin = 0;
		m_cur_right_margin = 0;
		FOREACH(TYPEOF(c_vector<s_anchored_element>)::iterator, iter, m_anchored_elements)
		{
			if(iter->bottomy >= m_total_height)
			{
				if(iter->is_left && iter->farx > m_cur_left_margin)
					m_cur_left_margin = iter->farx;
				else
				{
					if((!iter->is_left) && iter->farx > m_cur_right_margin)
						m_cur_right_margin = iter->farx;
				}
			}
		}
	}
	//--------------------------------------------------------
	void c_richbox::_clear_lines()
	{
		FOREACH(TYPEOF(c_vector<s_line*>)::iterator , iter , m_lines)
		{
			FOREACH(TYPEOF(c_list<s_text_element*>)::iterator , it , (*iter)->elements)
				delete *it;
			(*iter)->elements.clear();
			delete *iter;
		}
		m_lines.clear();
		FOREACH(TYPEOF(c_vector<s_anchored_element>)::iterator , iter , m_anchored_elements)
			delete iter->element;
		m_anchored_elements.clear();
	}
}