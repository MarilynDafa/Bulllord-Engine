/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "Font.h"
#include "EditBox.h"
namespace o2d{
	c_editbox::c_editbox(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_EDITBOX , parent_, property_) ,
		m_bg_zoom_region(property_.dim) , 
		m_gb(nullptr) , 
		m_frame_rect(property_.dim) , 
		m_min_value(0) , 
		m_max_value(999999) , 
		m_max_chars(999999) ,
		m_select_begin(0) , 
		m_select_end(0) , 
		m_cursor_pos(0) , 
		m_h_scroll_pos(0) , 
		m_v_scroll_pos(0) , 
		m_last_record(0) , 
		m_v_align(TA_V_CENTER) , 
		m_h_align(TA_H_LEFT) ,
		m_txt_color(0xFFFFFFFF) , 
		m_selecting(false) , 
		m_autoscroll(true) , 
		m_wordwrap(false) , 
		m_multiline(false) , 
		m_password_box(false) , 
		m_numeric_box(false) , 
		m_enable(true)
	{
		m_gb = new c_geometry_buffer(RO_TRIANGLE_STRIP);
		c_vector<c_geometry_buffer::s_vertex_element> fmts;
		c_geometry_buffer::s_vertex_element fmt1;
		fmt1.semantic = VES_POSITION;
		fmt1.index = 0;
		fmt1.type = VET_FLOAT2;
		fmts.push_back(fmt1);
		c_geometry_buffer::s_vertex_element fmt2;
		fmt2.semantic = VES_TEXTURE_COORD;
		fmt2.index = 0;
		fmt2.type = VET_FLOAT2;
		fmts.push_back(fmt2);
		c_geometry_buffer::s_vertex_element fmt3;
		fmt3.semantic = VES_DIFFUSE;
		fmt3.index = 0;
		fmt3.type = VET_FLOAT4;
		fmts.push_back(fmt3);
		m_gb->bind_vertex_buffer(BU_STREAM_DRAW , nullptr , 32*sizeof(f32) , fmts);
		_calculate_frame_rect();
	}
	//--------------------------------------------------------
	c_editbox::~c_editbox()
	{
		delete m_gb;
	}
	//--------------------------------------------------------
	void c_editbox::set_enable(bool enable_)
	{
		if(m_enable!=enable_)
		{
			m_enable = enable_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_wordwrap(bool flag_)
	{
		if(m_wordwrap!= flag_)
		{
			assert(!m_password_box);
			assert(!m_numeric_box);
			m_wordwrap = flag_;
			_break_text();
			_sig_input.emit(0);
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_multiline(bool flag_)
	{
		if(m_multiline!= flag_)
		{
			assert(!m_password_box);
			assert(!m_numeric_box);
			m_multiline = flag_;
			_break_text();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_autoscroll(bool flag_)
	{
		if(m_autoscroll!= flag_)
		{
			m_autoscroll = flag_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_password_box(bool flag_)
	{
		if(m_password_box!= flag_)
		{
			assert(!m_multiline);
			assert(!m_wordwrap);
			m_password_box = flag_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_numeric_box(bool flag_)
	{
		if(m_numeric_box!=flag_)
		{
			assert(!m_multiline);
			assert(!m_wordwrap);
			m_numeric_box = flag_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_max(u32 maxc_)
	{
		if(m_max_chars!=maxc_)
		{
			m_max_chars = maxc_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_margin(const c_float2& margin_)
	{
		if(m_margin.x()!=margin_.x() || m_margin.y()!= margin_.y())
		{
			m_margin.x() = margin_.x();
			m_margin.y() = margin_.y();
			_calculate_frame_rect();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_text(const c_string& text_)
	{
		m_text = text_;
		if(u32(m_cursor_pos) > m_text.get_length())
			m_cursor_pos = m_text.get_length();
		m_h_scroll_pos = 0;
		_break_text();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_editbox::set_text_color(const c_color& clr_)
	{
		if(m_txt_color != clr_.rgba())
		{
			c_color xx = clr_;
			m_txt_color = xx.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_text_align(e_text_alignment ha_, e_text_alignment va_)
	{
		if(m_v_align != va_ || m_h_align != ha_)
		{
			m_v_align = va_;
			m_h_align = ha_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_bg_image(const c_string& image_)
	{
		if(m_bg_img!=image_)
		{
			m_bg_img = image_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_editbox::set_stencil_image(const c_string& img_)
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
	void c_editbox::set_range(const c_float2& range_)
	{
		if(m_min_value != range_.x() || m_max_value != range_.y())
		{
			m_min_value = range_.x();
			m_max_value = range_.y();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_editbox::_update_position()
	{
		c_rect absrect(m_absolute_rect);
		c_widget::_update_position();
		if((absrect.lt_pt()!=m_absolute_rect.lt_pt()) || (absrect.rb_pt()!=m_absolute_rect.rb_pt()))
		{
			_calculate_frame_rect();
			_break_text();
			_calculate_scroll_pos();
		}
	}
	//--------------------------------------------------------
	bool c_editbox::_lost_focus(c_widget* widget_)
	{
		m_selecting = false;
		_set_text_select(0,0);
		return c_widget::_lost_focus(widget_);
	}
	//--------------------------------------------------------
	void c_editbox::_on_touch_move()
	{
		if(m_selecting)
		{
			m_cursor_pos = _get_cursor_pos(c_input_manager::get_singleton_ptr()->get_touch_pos());
			_set_text_select(m_select_begin, m_cursor_pos);
			_calculate_scroll_pos();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::_on_touch_start()
	{
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget()!=this)
		{
			m_selecting = true;
			m_cursor_pos = _get_cursor_pos(c_input_manager::get_singleton_ptr()->get_touch_pos());
			_set_text_select(m_cursor_pos, m_cursor_pos );
			_calculate_scroll_pos();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
		else
		{
			if(!rect_contains(m_clipping_rect , c_input_manager::get_singleton_ptr()->get_touch_pos()))
				return;
			else
			{
				m_cursor_pos = _get_cursor_pos(c_input_manager::get_singleton_ptr()->get_touch_pos());
				s32 newbegin = m_select_begin;
				if(!m_selecting)
					newbegin = m_cursor_pos;
				m_selecting = true;
				_set_text_select(newbegin, m_cursor_pos);
				_calculate_scroll_pos();
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
			}
		}
	}
	//--------------------------------------------------------
	void c_editbox::_on_touch_finish()
	{
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget() == this)
		{
			m_cursor_pos = _get_cursor_pos(c_input_manager::get_singleton_ptr()->get_touch_pos());
			if(m_selecting)
				_set_text_select(m_select_begin, m_cursor_pos );
			m_selecting = false;
			_calculate_scroll_pos();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_editbox::_on_pan()
	{
		f32 delta = c_input_manager::get_singleton_ptr()->get_pan();
		s32 newmarkbegin = m_select_begin;
		s32 newmarkend = m_select_end;
		if(delta>=0)
		{
			if(m_multiline || (m_wordwrap && m_broken_text.size() > 1))
			{
				s32 lineno = _get_line(m_cursor_pos);
				if(lineno > 0)
				{
					s32 cp = m_cursor_pos - m_broken_positions[lineno];
					if(!m_broken_text[lineno+1].is_empty() && (s32)m_broken_text[lineno-1].get_size() < cp)
						m_cursor_pos = m_broken_positions[lineno-1] + MAX_VALUE((u32)1, m_broken_text[lineno-1].get_size()) - 1;
					else
						m_cursor_pos = m_broken_positions[lineno-1] + cp;
				}
				newmarkbegin = 0;
				newmarkend = 0;
			}
		}
		else
		{
			if(m_multiline || (m_wordwrap && m_broken_text.size() > 1))
			{
				s32 lineno = _get_line(m_cursor_pos);
				if(lineno < (s32)m_broken_text.size()-1)
				{
					s32 cp = m_cursor_pos - m_broken_positions[lineno];
					if(!m_broken_text[lineno+1].is_empty() && (s32)m_broken_text[lineno+1].get_size() < cp)
						m_cursor_pos = m_broken_positions[lineno+1] + MAX_VALUE((u32)1, m_broken_text[lineno+1].get_size()) - 1;
					else
						m_cursor_pos = m_broken_positions[lineno+1] + cp;
				}
				newmarkbegin = 0;
				newmarkend = 0;
			}
		}
		_set_text_select(newmarkbegin, newmarkend);
		_calculate_scroll_pos();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_editbox::_on_char()
	{
		if(!m_enable)
			return;
		c_string str = c_input_manager::get_singleton_ptr()->get_string();
		if(!_is_printable(str[0]))
		{
			if(!_function_key(str[0]))
				return;
		}
		for(u32 i = 0 ; i < str.get_length() ; ++i)
		{
			utf16 ch = str[i];
			c_string txt = m_text;
			if(ch != 0)
			{
				if(m_text.get_length() < m_max_chars)
				{
					c_string s;
					if(m_select_begin != m_select_end)
					{
						s32 realmbgn = m_select_begin < m_select_end ? m_select_begin : m_select_end;
						s32 realmend = m_select_begin < m_select_end ? m_select_end : m_select_begin;
						s = m_text.sub_str(0, realmbgn);
						s.append(ch);
						s.append(m_text.sub_str(realmend, m_text.get_length() - realmend));
						m_text = s;
						m_cursor_pos = realmbgn + 1;
					}
					else
					{
						s = m_text.sub_str(0, m_cursor_pos);
						s.append(ch);
						s.append(m_text.sub_str(m_cursor_pos, m_text.get_length() - m_cursor_pos));
						m_text = s;
						++m_cursor_pos;
					}
					_set_text_select(0, 0);
				}
			}
			_break_text();
			_calculate_scroll_pos();
			_check_numeric(txt);
			_sig_input.emit(ch);
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_editbox::_redraw()
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
		bool focus = (c_ui_manager::get_singleton_ptr()->get_focus_widget() == this);
		_calculate_frame_rect();
		c_rect localcliprect = m_frame_rect;
		rect_clip(localcliprect , m_absolute_rect);
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(ft)
		{
			c_string curline = m_text;
			s32 startpos = 0;
			c_string s1, s2;
			bool ml = (!m_password_box && (m_wordwrap || m_multiline));
			s32 realmbgn = m_select_begin < m_select_end ? m_select_begin : m_select_end;
			s32 realmend = m_select_begin < m_select_end ? m_select_end : m_select_begin;
			s32 hlinestart = ml ? _get_line(realmbgn) : 0;
			s32 hlinecount = ml ? _get_line(realmend) - hlinestart + 1 : 1;
			s32 linecount = ml ? m_broken_text.size() : 1;
			if(m_text.get_length())
			{
				for(s32 i=0; i < linecount; ++i)
				{
					_set_text_rect(i);
					curline = ml ? m_broken_text[i] : m_text;
					startpos = ml ? m_broken_positions[i] : 0;
					u32 fmt = m_h_align|m_v_align;
					u32 clr = m_enable?m_txt_color:c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TXT_DISABLE_CLR).rgba();
					c_color xxx = clr;
					c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid , m_fnt_height , fmt  , true , curline , m_cur_rect , localcliprect , xxx.rgba(), m_password_box, m_password_box ? 0x00F0 : m_txt_flag);
					if(focus && m_select_begin != m_select_end && i >= hlinestart && i < hlinestart + hlinecount)
					{
						f32 mbegin = 0, mend = 0;
						f32 outy;
						s32 linestartpos = 0, lineendpos = curline.get_length();
						if(i == hlinestart)
						{
							s1 = curline.sub_str(0, realmbgn - startpos);
							ft->measure_text(m_password_box?c_string(s1.get_length() , L'*'):s1 , mbegin , outy);
							linestartpos = realmbgn - startpos;
						}
						if(i == hlinestart + hlinecount - 1)
						{
							s2 = curline.sub_str(0, realmend - startpos);
							ft->measure_text(m_password_box?c_string(s2.get_length() , L'*'):s2 , mend , outy);
							lineendpos = (s32)s2.get_length();
						}
						else
							ft->measure_text(m_password_box?c_string(curline.get_length() , L'*'):curline , mend , outy);
						m_cur_rect.lt_pt().x() += mbegin;
						m_cur_rect.rb_pt().x() = m_cur_rect.lt_pt().x() + mend - mbegin;
						c_ui_manager::get_singleton_ptr()->cache_batch(UUC_EDIT_HIGHLIGHT , m_cur_rect , localcliprect);
					}
				}
			}			
		}
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	void c_editbox::_update(c_technique* uit_, u32 dt_)
	{
		if(!m_visible)
			return;
		if(c_ui_manager::get_singleton_ptr()->get_focus_widget()!=this)
		{
			c_widget::_update(uit_, dt_);
			return;
		}
		if(m_last_record < 600)
		{
			m_last_record+=dt_;
			c_widget::_update(uit_, dt_);
			return;
		}
		else if(m_last_record < 1200)
			m_last_record+=dt_;
		else
			m_last_record = 0;
		if(m_enable)
		{
			c_rect cv;
			s32 cursorline = 0;
			s32 startpos = 0;
			f32 charcursorpos;
			c_string curline = m_text;
			c_rect localcliprect = m_frame_rect;
			rect_clip(localcliprect , m_absolute_rect);
			if(m_wordwrap && m_multiline)
			{
				cursorline = _get_line(m_cursor_pos);
				curline = m_broken_text[cursorline];
				startpos = m_broken_positions[cursorline];
			}
			c_string s1 = curline.sub_str(0 , m_cursor_pos - startpos);
			f32 outy;
			c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
			ft->measure_text(m_password_box?c_string(s1.get_length() , L'*'):s1 , charcursorpos , outy);
			_set_text_rect(cursorline);
			m_cur_rect.lt_pt().x() += charcursorpos;
			cv = m_cur_rect;
			rect_clip(cv, localcliprect);	
			cv.rb_pt().x() = cv.lt_pt().x() + 1;
			cv.rb_pt().y() = cv.lt_pt().y() + m_fnt_height;
			c_color clr = c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_EDIT_CURSOR);
			c_stream data;
			data.resize(4*8*sizeof(f32));
			data.write_f32(cv.lt_pt().x());
			data.write_f32(cv.lt_pt().y());
			data.write_f32(0.f);
			data.write_f32(0.f);
			data.write_f32(clr.r());
			data.write_f32(clr.g());
			data.write_f32(clr.b());
			data.write_f32(clr.a());
			data.write_f32(cv.rb_pt().x());
			data.write_f32(cv.lt_pt().y());
			data.write_f32(1.f);
			data.write_f32(0.f);
			data.write_f32(clr.r());
			data.write_f32(clr.g());
			data.write_f32(clr.b());
			data.write_f32(clr.a());
			data.write_f32(cv.lt_pt().x());
			data.write_f32(cv.rb_pt().y());
			data.write_f32(0.f);
			data.write_f32(1.f);
			data.write_f32(clr.r());
			data.write_f32(clr.g());
			data.write_f32(clr.b());
			data.write_f32(clr.a());
			data.write_f32(cv.rb_pt().x());
			data.write_f32(cv.rb_pt().y());
			data.write_f32(1.f);
			data.write_f32(1.f);
			data.write_f32(clr.r());
			data.write_f32(clr.g());
			data.write_f32(clr.b());
			data.write_f32(clr.a());
			m_gb->uploadvb(0, data.get_pointer(), data.get_size());
			uit_->get_pass(0)->get_sampler(L"tex")->tex = c_ui_manager::get_singleton_ptr()->get_placeholder_tex();
			c_device::get_singleton_ptr()->draw(uit_, m_gb);
		}
		c_widget::_update(uit_, dt_);
	}
	//--------------------------------------------------------
	s32 c_editbox::_get_line(s32 pos_)
	{
		if(!m_wordwrap && !m_multiline)
			return 0;
		s32 i=0;
		while(i < (s32)m_broken_positions.size())
		{
			if(m_broken_positions[i] > pos_)
				return i-1;
			++i;
		}
		return (s32)m_broken_positions.size() - 1;
	}
	//--------------------------------------------------------
	s32 c_editbox::_get_cursor_pos(const c_float2& pos_)
	{
		u32 linecount = (m_wordwrap || m_multiline) ? m_broken_text.size() : 1;
		c_string curline;
		s32 startpos=0;
		f32 x , y;
		x = pos_.x() + 3;
		y = pos_.y();
		for(u32 i=0; i < linecount; ++i)
		{
			_set_text_rect(i);
			if(i == 0 && y< m_cur_rect.lt_pt().y())
				y = m_cur_rect.lt_pt().y();
			if(i == linecount - 1 && y > m_cur_rect.rb_pt().y() )
				y = m_cur_rect.rb_pt().y();
			if(y >= m_cur_rect.lt_pt().y() && y <= m_cur_rect.rb_pt().y())
			{
				curline = (m_wordwrap || m_multiline) ? m_broken_text[i] : m_text;
				startpos = (m_wordwrap || m_multiline) ? m_broken_positions[i] : 0;
				break;
			}
		}
		if(x < m_cur_rect.lt_pt().x())
			x = m_cur_rect.lt_pt().x();
		s32 idx;
		if(curline.is_empty())
			idx = -1;
		else
		{
			c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
			idx = ft->get_char_index(m_password_box?c_string(curline.get_length() , L'*'):curline, x - m_cur_rect.lt_pt().x());
		}
		if(idx != -1)
			return idx + startpos;
		return curline.get_length() + startpos;
	}
	//--------------------------------------------------------
	void c_editbox::_set_text_select(s32 begin_, s32 end_)
	{
		if(begin_ != m_select_begin || end_ != m_select_end)
		{
			m_select_begin = begin_;
			m_select_end = end_;
			_sig_select_change.emit();
		}
	}
	//--------------------------------------------------------
	void c_editbox::_calculate_scroll_pos()
	{
		if(!m_autoscroll)
			return;
		s32 curline = _get_line(m_cursor_pos);
		if(curline < 0)
			return;
		_set_text_rect(curline);
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(!ft)
			return;
		u32 cursorwidth = 1;
		c_string curtxt = (m_multiline || m_wordwrap) ? m_broken_text[curline] : m_text;
		s32 cpos = (m_multiline || m_wordwrap) ? m_cursor_pos - m_broken_positions[curline] : m_cursor_pos;
		f32 cbegin , cend , txtw , tmp;
		ft->measure_text(curtxt.sub_str(0, cpos) , cbegin , tmp);
		cend = cbegin + cursorwidth;
		ft->measure_text(curtxt , txtw , tmp);
		if(txtw < m_frame_rect.width())
		{
			m_h_scroll_pos = 0;
			_set_text_rect(curline);
		}
		if(m_cur_rect.lt_pt().x() + cbegin < m_frame_rect.lt_pt().x())
		{
			m_h_scroll_pos -= m_frame_rect.lt_pt().x() - (m_cur_rect.lt_pt().x() + cbegin);
			_set_text_rect(curline);
		}
		else if(m_cur_rect.lt_pt().x() + cend > m_frame_rect.rb_pt().x())
		{
			m_h_scroll_pos += (m_cur_rect.lt_pt().x() + cend)-m_frame_rect.rb_pt().x();
			_set_text_rect(curline);
		}
		if(m_multiline || m_wordwrap)
		{
			f32 lineheight;
			ft->measure_text(L"A" , tmp , lineheight);
			if(lineheight >= m_frame_rect.height())
			{
				m_v_scroll_pos = 0;
				_set_text_rect(curline);
				s32 unscrolledpos = m_cur_rect.lt_pt().y();
				s32 pivot = m_frame_rect.lt_pt().y();
				switch(m_v_align)
				{
				case TA_V_CENTER:
					pivot += m_frame_rect.height()/2;
					unscrolledpos += lineheight/2;
					break;
				case TA_V_BOTTOM:
					pivot += m_frame_rect.height();
					unscrolledpos += lineheight;
					break;
				default:break;
				}
				m_v_scroll_pos = unscrolledpos-pivot;
				_set_text_rect(curline);
			}
			else
			{
				_set_text_rect(0);
				if(m_cur_rect.lt_pt().y() > m_frame_rect.lt_pt().y() && m_v_align != TA_V_BOTTOM)
					m_v_scroll_pos = 0;
				else if(m_v_align != TA_V_TOP)
				{
					u32 lastline = m_broken_positions.empty() ? 0 : m_broken_positions.size()-1;
					_set_text_rect(lastline);
					if(m_cur_rect.rb_pt().y() < m_frame_rect.rb_pt().y())
						m_v_scroll_pos -= m_frame_rect.rb_pt().y() - m_cur_rect.rb_pt().y();
				}
				_set_text_rect(curline);
				if(m_cur_rect.lt_pt().y() < m_frame_rect.lt_pt().y() )
				{
					m_v_scroll_pos -= m_frame_rect.lt_pt().y() - m_cur_rect.lt_pt().y();
					_set_text_rect(curline);
				}
				else if(m_cur_rect.rb_pt().y() > m_frame_rect.rb_pt().y())
				{
					m_v_scroll_pos += m_cur_rect.rb_pt().y() - m_frame_rect.rb_pt().y();
					_set_text_rect(curline);
				}
			}
		}
	}
	//--------------------------------------------------------
	void c_editbox::_calculate_frame_rect()
	{
		m_frame_rect = m_absolute_rect;
		m_frame_rect.lt_pt().x() += m_margin.x();
		m_frame_rect.lt_pt().y() += m_margin.y();
		m_frame_rect.rb_pt().x() -= m_margin.x();
		m_frame_rect.rb_pt().y() -= m_margin.y();
	}
	//--------------------------------------------------------
	void c_editbox::_set_text_rect(s32 line_)
	{
		if(line_ < 0)
			return;
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(!ft)
			return;
		c_float2 d;
		u32 linecount = (m_wordwrap || m_multiline) ? m_broken_text.size() : 1;
		if(m_wordwrap || m_multiline)
		{
			ft->measure_text(m_broken_text[line_] , d.x() , d.y());
			d.y() = (d.y())?d.y():m_fnt_height;
		}
		else
		{
			ft->measure_text(m_password_box?c_string(m_text.get_length() , L'*'):m_text , d.x() , d.y());
			d.y() = (d.y())?d.y():m_fnt_height;
		}
		switch(m_h_align)
		{
		case TA_H_LEFT:
			m_cur_rect.lt_pt().x() = 0;
			m_cur_rect.rb_pt().x() = d.x();
			break;
		case TA_H_CENTER:
			m_cur_rect.lt_pt().x() = (m_frame_rect.width()/2.f) - (d.x()/2.f);
			m_cur_rect.rb_pt().x() = (m_frame_rect.width()/2.f) + (d.x()/2.f);
			break;
		case TA_H_RIGHT:
			m_cur_rect.lt_pt().x() = m_frame_rect.width() - d.x();
			m_cur_rect.rb_pt().x() = m_frame_rect.width();
			break;
		default:assert(0);break;
		}
		switch(m_v_align)
		{
		case TA_V_TOP:
			m_cur_rect.lt_pt().y() = d.y()*line_;
			break;
		case TA_V_CENTER:
			m_cur_rect.lt_pt().y() = (m_frame_rect.height()/2.f) - (linecount*d.y())/2.f + d.y()*line_;
			break;
		case TA_V_BOTTOM:
			m_cur_rect.lt_pt().y() = m_frame_rect.height() - linecount*d.y() + d.y()*line_;
			break;
		default:assert(0);break;
		}
		m_cur_rect.lt_pt().x() -= m_h_scroll_pos;
		m_cur_rect.rb_pt().x() -= m_h_scroll_pos;
		m_cur_rect.lt_pt().y() -= m_v_scroll_pos;
		m_cur_rect.rb_pt().y() = m_cur_rect.lt_pt().y() + d.y();
		m_cur_rect.lt_pt() += m_frame_rect.lt_pt();
		m_cur_rect.rb_pt() += m_frame_rect.lt_pt();
	}
	//--------------------------------------------------------
	void c_editbox::_break_text()
	{
		if(!m_wordwrap && !m_multiline)
			return;
		m_broken_text.clear();
		m_broken_positions.clear();
		c_string line;
		c_string word;
		c_string whitespace;
		s32 lastlinestart = 0;
		s32 size = m_text.get_length();
		s32 length = 0;
		s32 elwidth = m_relative_rect.width() - m_margin.x()*2.f - 6;
		utf16 c;
		for(s32 i=0; i < size; ++i)
		{
			c = m_text[i];
			bool linebreak = false;
			if(c == L'\r')
			{
				linebreak = true;
				c = 0;
				if(m_text[i+1] == L'\n')
				{
					m_text.erase(i+1);
					--size;
					if(m_cursor_pos > i)
						--m_cursor_pos;
				}
			}
			else if(c == L'\n')
			{
				linebreak = true;
				c = 0;
			}
			if(!m_multiline)
				linebreak = false;
			if(c == L' ' || c == 0 || i == (size-1))
			{
				f32 whitelgth , worldlgth , tmp;
				c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
				if(!ft)
					continue;
				ft->measure_text(whitespace , whitelgth , tmp);
				ft->measure_text(word , worldlgth , tmp);
				if(m_wordwrap && length + worldlgth + whitelgth > elwidth && line.get_length() > 0)
				{
					length = worldlgth;
					m_broken_text.push_back(line);
					m_broken_positions.push_back(lastlinestart);
					lastlinestart = i - (s32)word.get_length();
					line = word;
				}
				else
				{
					line += whitespace;
					line += word;
					length += whitelgth + worldlgth;
				}
				word = L"";
				whitespace = L"";
				if(c)
					whitespace += c;
				if(linebreak)
				{
					line += whitespace;
					line += word;
					m_broken_text.push_back(line);
					m_broken_positions.push_back(lastlinestart);
					lastlinestart = i+1;
					line = L"";
					word = L"";
					whitespace = L"";
					length = 0;
				}
			}
			else
				word += c;
		}
		line += whitespace;
		line += word;
		m_broken_text.push_back(line);
		m_broken_positions.push_back(lastlinestart);
	}
	//--------------------------------------------------------
	void c_editbox::_check_numeric(const c_string& old_)
	{
		if(m_multiline||!m_numeric_box)
			return;
		if(!m_text.is_numeric())
		{
			m_text = old_;
			m_cursor_pos--;
			return;
		}
		if(m_text[0] == L'.')
			m_text = old_;
		f32 val = m_text.to_float();
		if(val != clamp(val , m_min_value , m_max_value))
		{
			m_text = c_string(clamp(val , m_min_value , m_max_value));
			m_text.trim(L'0');
			if(m_text[m_text.get_length() - 1] == L'.')
				m_text.erase(m_text.get_length() - 1);
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	bool c_editbox::_function_key(utf16 ch_)
	{		
		switch(ch_)
		{
		case FC_BACKSPACE:
			{
				bool textchanged = false;
				s32 newbegin = m_select_begin;
				s32 newend = m_select_end;
				if((!m_text.is_empty())&&m_enable)
				{
					c_string s;
					if(m_select_begin != m_select_end)
					{
						s32 realmbgn = m_select_begin < m_select_end ? m_select_begin : m_select_end;
						s32 realmend = m_select_begin < m_select_end ? m_select_end : m_select_begin;
						s = m_text.sub_str(0, realmbgn);
						s.append( m_text.sub_str(realmend, m_text.get_length() - realmend));
						m_text = s;
						m_cursor_pos = realmbgn;
					}
					else
					{
						if(m_cursor_pos>0)
							s = m_text.sub_str(0, m_cursor_pos-1);
						else
							s = L"";
						s.append(m_text.sub_str(m_cursor_pos, m_text.get_length() - m_cursor_pos));
						m_text = s;
						--m_cursor_pos;
					}
					if(m_cursor_pos < 0)
						m_cursor_pos = 0;
					newbegin = 0;
					newend = 0;
					textchanged = true;
					c_ui_manager::get_singleton_ptr()->set_dirty(true);
					_set_text_select(newbegin, newend);
					if(textchanged)
					{
						_break_text();
						_sig_input.emit(0);
					}
					_calculate_scroll_pos();
				}
			}
			return false;
		case FC_COPY:
			if(!m_password_box && m_select_begin != m_select_end)
			{
				s32 realmbgn = m_select_begin < m_select_end ? m_select_begin : m_select_end;
				s32 realmend = m_select_begin < m_select_end ? m_select_end : m_select_begin;
				m_clipboard = m_text.sub_str(realmbgn, realmend - realmbgn);
			}
			return false;
		case FC_CUT:
			if(!m_password_box && m_select_begin != m_select_end)
			{
				bool textchanged = false;
				s32 newbegin = m_select_begin;
				s32 newend = m_select_end;
				const s32 realmbgn = m_select_begin < m_select_end ? m_select_begin : m_select_end;
				const s32 realmend = m_select_begin < m_select_end ? m_select_end : m_select_begin;
				m_clipboard = m_text.sub_str(realmbgn, realmend - realmbgn);
				if(m_enable)
				{
					c_string s = m_text.sub_str(0, realmbgn);
					s.append(m_text.sub_str(realmend, m_text.get_length()-realmend));
					m_text = s;
					m_cursor_pos = realmbgn;
					newbegin = 0;
					newend = 0;
					textchanged = true;
				}
				_set_text_select(newbegin, newend);
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
				if(textchanged)
				{
					_break_text();
					_sig_input.emit(0);
				}
				_calculate_scroll_pos();
			}
			return false;
		case FC_PASTE:
			if(m_enable)
			{
				bool textchanged = false;
				s32 newbegin = m_select_begin;
				s32 newend = m_select_end;
				s32 realmbgn = m_select_begin < m_select_end ? m_select_begin : m_select_end;
				s32 realmend = m_select_begin < m_select_end ? m_select_end : m_select_begin;
				const utf16* p = m_clipboard.c_str();
				if(p)
				{
					if(m_select_begin == m_select_end)
					{
						c_string s = m_text.sub_str(0, m_cursor_pos);
						s.append(p);
						s.append( m_text.sub_str(m_cursor_pos, m_text.get_length() - m_cursor_pos) );
						if (!m_max_chars || s.get_length() <= m_max_chars)
						{
							m_text = s;
							s = p;
							m_cursor_pos += s.get_length();
						}
					}
					else
					{
						c_string s = m_text.sub_str(0, realmbgn);
						s.append(p);
						s.append( m_text.sub_str(realmend, m_text.get_length() - realmend));
						if (!m_max_chars || s.get_length()<=m_max_chars)
						{
							m_text = s;
							s = p;
							m_cursor_pos = realmbgn + s.get_length();
						}
					}
				}
				newbegin = 0;
				newend = 0;
				textchanged = true;
				c_ui_manager::get_singleton_ptr()->set_dirty(true);
				_set_text_select(newbegin, newend);
				if(textchanged)
				{
					_break_text();
					_sig_input.emit(0);
				}
				_calculate_scroll_pos();
			}
			return false;
		case FC_RETURN:
			if(!m_multiline)
				_sig_input_enter.emit();
			return true;
		default:
			return false;
		}
	}
	//--------------------------------------------------------
	bool c_editbox::_is_printable(utf16 ch_)
	{
		if(ch_>255)
			return true;
		else
		{
			if(ch_>=0 && ch_ <=31)
				return false;
			else if(ch_ == 255)
				return false;
			else
				return true;
		}
	}
}

