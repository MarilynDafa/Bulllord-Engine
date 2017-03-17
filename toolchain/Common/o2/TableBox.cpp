/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Algorithm.h"
#include "InputMgr.h"
#include "UIMgr.h"
#include "TableBox.h"
namespace o2d{
	c_tablebox::c_tablebox(c_widget* parent_, const c_widget::s_widget_property& property_)
		:c_widget(WT_TABLEBOX , parent_, property_) ,
		m_row_height(14) , 
		m_total_height(0) , 
		m_total_width(0) , 
		m_selected_row(-1) , 
		m_cell_height_padding(2) , 
		m_cell_width_padding(5) , 
		m_active_column(-1) , 
		m_scrollpos(0) , 
		m_txt_color(0xFFFFFFFF) ,
		m_row_separator(false) , 
		m_col_separator(false) ,
		m_selecting(false)
	{
		m_fnt_height = 14;
		c_widget::s_widget_property p = property_;
		_refresh_controls();
	}
	//--------------------------------------------------------
	c_tablebox::~c_tablebox(){}
	//--------------------------------------------------------
	void c_tablebox::set_bg_image(const c_string& img_)
	{
		if(m_bg_img!=img_)
		{
			m_bg_img = img_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tablebox::set_bg_fragment(const c_float2& anc_, const c_float2& dim_)
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
	void c_tablebox::set_text_color(const c_color& color_)
	{
		if(m_txt_color != color_.rgba())
		{
			m_txt_color = color_.rgba();
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tablebox::set_separator(bool row_, bool col_)
	{
		if(m_col_separator != col_ || m_row_separator != row_)
		{
			m_col_separator = col_;
			m_row_separator = row_;
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tablebox::set_cell_text(u32 rindex_, u32 cindex_, const c_string& text_)
	{
		if(rindex_ < m_rows.size() && cindex_ < m_columns.size())
		{
			m_rows[rindex_].items[cindex_].text = text_;
			_break_text( m_rows[rindex_].items[cindex_].text, m_rows[rindex_].items[cindex_].broken_text, m_columns[cindex_].width );
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tablebox::set_cell_data(u32 rindex_, u32 cindex_, void* data_)
	{
		if(rindex_ < m_rows.size() && cindex_ < m_columns.size())
			m_rows[rindex_].items[cindex_].data = data_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::set_row_height(u32 h)
	{
		m_row_height = h;
		_refresh_controls();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	const c_string& c_tablebox::get_cell_text(u32 rindex_, u32 cindex_) const
	{
		if(rindex_ < m_rows.size() && cindex_ < m_columns.size())
			return m_rows[rindex_].items[cindex_].text;
		return c_string::EmptyStr;
	}
	//--------------------------------------------------------
	void* c_tablebox::get_cell_data(u32 rindex_, u32 cindex_) const
	{
		if(rindex_ < m_rows.size() && cindex_ < m_columns.size())
			return m_rows[rindex_].items[cindex_].data;
		LOG(LL_ERROR , "the rindex or the cindex is out of range");
		return nullptr;
	}
	//--------------------------------------------------------
	void c_tablebox::add_column(const c_string& caption_, s32 cindex_)
	{
		s_table_column header;
		header.name = caption_;
		f32 txtw , txth;
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		if(!ft)
			return;
		ft->measure_text(caption_ , txtw , txth);
		header.width = txtw + (m_cell_width_padding * 2) + 15;
		if(cindex_ < 0 || cindex_ >= (s32)m_columns.size())
		{
			m_columns.push_back(header);
			FOREACH(TYPEOF(c_vector<s_table_raw>)::iterator , iter , m_rows)
			{
				s_table_cell cell;
				iter->items.push_back(cell);
			}
		}
		else
		{
			m_columns.insert(m_columns.begin() + cindex_ , header);
			FOREACH(TYPEOF(c_vector<s_table_raw>)::iterator , iter , m_rows)
			{
				s_table_cell cell;
				iter->items.insert(iter->items.begin() + cindex_ , cell);
			}
		}
		if(m_active_column == -1)
			m_active_column = 0;
		_recalculate_widths();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::remove_column(u32 cindex_)
	{
		if(cindex_ < m_columns.size())
		{
			m_columns.erase(m_columns.begin() + cindex_);
			FOREACH(TYPEOF(c_vector<s_table_raw>)::iterator , iter , m_rows)
				iter->items.erase(iter->items.begin() + cindex_);
		}
		if((s32)cindex_ <= m_active_column)
			m_active_column = m_columns.size() ? 0 : -1;
		_recalculate_widths();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	bool c_tablebox::set_active_column(s32 idx_)
	{
		if(idx_ < 0 || idx_ >= (s32)m_columns.size())
			return false;
		bool changed = (m_active_column != idx_);
		m_active_column = idx_;
		if(m_active_column < 0 )
			return false;
		if(changed)
			_sig_header_changed.emit(m_active_column);
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return true;
	}
	//--------------------------------------------------------
	void c_tablebox::set_column_width(u32 cindex_, u32 width_)
	{
		if(cindex_ < m_columns.size())
		{
			f32 txtw , txth;
			c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
			ft->measure_text(m_columns[cindex_].name , txtw , txth);
			if(width_ < txtw + (m_cell_width_padding * 2))
				width_ = txtw + (m_cell_width_padding * 2);
			m_columns[cindex_].width = width_;
			for(u32 i=0; i < m_rows.size(); ++i)
				_break_text(m_rows[i].items[cindex_].text, m_rows[i].items[cindex_].broken_text, m_columns[cindex_].width);
		}
		_recalculate_widths();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	u32 c_tablebox::get_column_width(u32 cindex_) const
	{
		if(cindex_ >= m_columns.size())
			return 0;
		return m_columns[cindex_].width;
	}
	//--------------------------------------------------------
	s32 c_tablebox::get_column_count() const
	{
		return m_columns.size();
	}
	//--------------------------------------------------------
	void c_tablebox::set_caption(u32 idx_ , const c_string& caption_)
	{
		m_columns[idx_].name = caption_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	const c_string& c_tablebox::get_caption(u32 idx_) const
	{
		return m_columns[idx_].name;
	}
	//--------------------------------------------------------
	u32 c_tablebox::add_row(u32 rindex_)
	{
		if(rindex_ > m_rows.size())
			rindex_ = m_rows.size();
		s_table_raw row;
		if(rindex_ == m_rows.size())
			m_rows.push_back(row);
		else
			m_rows.insert(m_rows.begin() + rindex_ , row);
		FOREACH(TYPEOF(c_vector<s_table_column>)::iterator , iter , m_columns)
			m_rows[rindex_].items.push_back(s_table_cell());
		_recalculate_heights();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
		return rindex_;
	}
	//--------------------------------------------------------
	void c_tablebox::remove_row(u32 rindex_)
	{
		if(rindex_ > m_rows.size())
			return;
		m_rows.erase(m_rows.begin() + rindex_);
		if(!(m_selected_row < s32(m_rows.size())))
			m_selected_row = m_rows.size() - 1;
		_recalculate_heights();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::set_selected_row(s32 index_)
	{
		m_selected_row = -1;
		if(index_ >= 0 && index_ < (s32) m_rows.size())
			m_selected_row = index_;
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	s32 c_tablebox::get_row_count() const
	{
		return m_rows.size();
	}
	//--------------------------------------------------------
	void c_tablebox::clear_rows()
	{
		m_scrollpos = 0;
		m_selected_row = -1;
		m_rows.clear();
		_recalculate_heights();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::clearall()
	{
		m_scrollpos = 0;
		m_selected_row = -1;
		m_rows.clear();
		m_columns.clear();
		_recalculate_heights();
		_recalculate_widths();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::set_stencil_image(const c_string& img_)
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
	void c_tablebox::set_odd_image(const c_string& img_)
	{
		if (img_.get_length() == 0)
			return;
		m_odd_img = img_;
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		if (sk)
		{
			const c_rect& tc = sk->get_texcoord(m_odd_img);
			f32 w = sk->get_skin_tex()->get_width();
			f32 h = sk->get_skin_tex()->get_height();
			m_odd_rect.lt_pt().x() = tc.lt_pt().x() / w;
			m_odd_rect.lt_pt().y() = tc.lt_pt().y() / h;
			m_odd_rect.rb_pt().x() = tc.rb_pt().x() / w;
			m_odd_rect.rb_pt().y() = tc.rb_pt().y() / h;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::set_even_image(const c_string& img_)
	{
		if (img_.get_length() == 0)
			return;
		m_even_img = img_;
		c_skin* sk = c_ui_manager::get_singleton_ptr()->get_skin(m_skinid);
		if (sk)
		{
			const c_rect& tc = sk->get_texcoord(m_even_img);
			f32 w = sk->get_skin_tex()->get_width();
			f32 h = sk->get_skin_tex()->get_height();
			m_even_rect.lt_pt().x() = tc.lt_pt().x() / w;
			m_even_rect.lt_pt().y() = tc.lt_pt().y() / h;
			m_even_rect.rb_pt().x() = tc.rb_pt().x() / w;
			m_even_rect.rb_pt().y() = tc.rb_pt().y() / h;
		}
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::_zoom_update(const c_float2& v1_, const c_float2& v2_)
	{
		m_bg_zoom_region.lt_pt() += v1_;
		m_bg_zoom_region.rb_pt() += v2_;
		c_widget::_zoom_update(v1_, v2_);
	}
	//--------------------------------------------------------
	void c_tablebox::_update_position()
	{
		c_widget::_update_position();
		_recalculate_heights();
		_recalculate_widths();
	}
	//--------------------------------------------------------
	bool c_tablebox::_lost_focus(c_widget* widget_)
	{
		m_selecting = false;
		return c_widget::_lost_focus(widget_);
	}
	//--------------------------------------------------------
	void c_tablebox::_on_touch_move()
	{
		if(m_selecting)
		{
			c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
			if(rect_contains(m_absolute_rect , pos))
				_select_new(pos.y() , false);
		}
	}
	//--------------------------------------------------------
	void c_tablebox::_on_touch_start()
	{
		c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
		if(_select_column(pos.x(), pos.y()))
			return;
		m_selecting = true;
		c_ui_manager::get_singleton_ptr()->set_focus(this);
	}
	//--------------------------------------------------------
	void c_tablebox::_on_touch_finish()
	{
		c_float2 pos = c_input_manager::get_singleton_ptr()->get_touch_pos();
		m_selecting = false;
		if(!rect_contains(m_absolute_rect , pos))
			c_ui_manager::get_singleton_ptr()->remove_focus(this);
		_select_new(pos.y() , false);
	}
	//--------------------------------------------------------
	void c_tablebox::_on_pan()
	{
		f32 delta = c_input_manager::get_singleton_ptr()->get_pan();
		m_scrollpos = m_scrollpos + (delta < 0.f ? -1 : 1)*-10;
		_check_scroll();
		c_ui_manager::get_singleton_ptr()->set_dirty(true);
	}
	//--------------------------------------------------------
	void c_tablebox::_redraw()
	{
		if(!m_visible)
			return;
		_refresh_controls();
		c_rect tabler(m_absolute_rect);
		tabler.lt_pt().x() += 1;
		tabler.lt_pt().y() += 1;
		s32 headerbottom = tabler.lt_pt().y();
		c_rect clientc(tabler);
		clientc.lt_pt().y() = headerbottom + 1;
		rect_clip(clientc , m_clipping_rect);
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
		c_rect scrolledclient(tabler);
		scrolledclient.lt_pt().y() = headerbottom + 1;
		scrolledclient.rb_pt().y() = scrolledclient.lt_pt().y() + m_total_height;
		scrolledclient.rb_pt().x() = scrolledclient.lt_pt().x() + m_total_width;
		scrolledclient.lt_pt().y() -= m_scrollpos;
		scrolledclient.rb_pt().y() -= m_scrollpos;		
		c_rect rowr(scrolledclient);
		rowr.rb_pt().y() = rowr.lt_pt().y() + m_row_height;
		u32 pos;
		for(u32 i = 0 ; i < m_rows.size() ; ++i)
		{
			if(rowr.rb_pt().y() >= m_absolute_rect.lt_pt().y() && rowr.lt_pt().y() <= m_absolute_rect.rb_pt().y())
			{
				c_rect liner(rowr);
				liner.lt_pt().y() = liner.rb_pt().y() - 1.f;
				if (m_odd_img.get_length() > 0 && i % 2 == 1)
				{
					c_rect rc = rowr;
					rc.rb_pt().x() = rc.lt_pt().x() + m_absolute_rect.width();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_odd_img, m_odd_rect, rc, m_clipping_rect, m_flipx, m_flipy);
				}
				if (m_even_img.get_length() > 0 && i % 2 == 0)
				{
					c_rect rc = rowr;
					rc.rb_pt().x() = rc.lt_pt().x() + m_absolute_rect.width();
					c_ui_manager::get_singleton_ptr()->cache_batch(m_skinid, m_even_img, m_even_rect, rc, m_clipping_rect, m_flipx, m_flipy);
				}
				
				c_rect textr(rowr);
				pos = rowr.lt_pt().x();
				if((s32)i == m_selected_row)
				{
					c_rect tmp = rowr;
					tmp.lt_pt().x() += 2.f;
					tmp.rb_pt().x() -= 2.f;
					c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TABLE_SELECTING, tmp, clientc);
				}
				for(u32 j = 0 ; j < m_columns.size() ; ++j)
				{
					textr.lt_pt().x() = pos + m_cell_width_padding;
					textr.rb_pt().x() = pos + m_columns[j].width - m_cell_width_padding;
					u32 clr;
					if((s32)i == m_selected_row)
						clr = c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TABLE_HIGHLIGHT).rgba();
					else
						clr = m_txt_color;
					c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid, m_fnt_height, TA_H_LEFT|TA_V_CENTER, true, m_rows[i].items[j].broken_text, textr, clientc, clr);
					pos += m_columns[j].width;
				}				
			}
			rowr.lt_pt().y() += m_row_height;
			rowr.rb_pt().y() += m_row_height;
		}
		/*
		c_rect cseparator(clientc);
		pos = scrolledclient.lt_pt().x();
		c_rect tablec(tabler);
		rect_clip(tablec , m_clipping_rect);
		for(u32 i = 0 ; i < m_columns.size() ; ++i)
		{
			c_string text = m_columns[i].name;
			u32 colw = m_columns[i].width;
			c_rect columnrect(pos, tabler.lt_pt().y(), pos + colw, headerbottom);
			if(m_col_separator&&i)
			{
				cseparator.lt_pt().x() = pos;
				cseparator.rb_pt().x() = pos + 1;
				c_ui_manager::get_singleton_ptr()->cache_batch(UUC_TABLE_SEPARATOR, cseparator, tablec);
			}
			columnrect.lt_pt().x() += m_cell_width_padding; 
			c_ui_manager::get_singleton_ptr()->cache_batch(m_fntid, m_fnt_height, TA_H_LEFT|TA_V_CENTER, true, text, columnrect, tablec, m_txt_color);
			pos += colw;
		}*/
		c_widget::_redraw();
	}
	//--------------------------------------------------------
	void c_tablebox::_refresh_controls()
	{
		_update_position();
		_recalculate_heights();
		_recalculate_widths();
	}
	//--------------------------------------------------------
	void c_tablebox::_recalculate_widths()
	{
		m_total_width=0;
		FOREACH(TYPEOF(c_vector<s_table_column>)::iterator , iter , m_columns)
			m_total_width += iter->width;
		_check_scroll();
	}
	//--------------------------------------------------------
	void c_tablebox::_recalculate_heights()
	{
		m_total_height = 0;
		//m_row_height = m_fnt_height + (m_cell_height_padding * 2);
		m_total_height = m_row_height * m_rows.size();
		_check_scroll();
	}
	//--------------------------------------------------------
	bool c_tablebox::_select_column(s32 xpos_, s32 ypos_)
	{
		if(ypos_ > ( m_absolute_rect.lt_pt().y() + m_row_height))
			return false;
		s32 pos = m_absolute_rect.lt_pt().x()+1;
		for(u32 i = 0 ; i < m_columns.size() ; ++i)
		{
			u32 cwidth = m_columns[i].width;
			if(xpos_ >= pos && xpos_ < ( pos + s32(cwidth)))
			{
				set_active_column(i);
				return true;
			}
			pos += cwidth;
		}
		return false;
	}
	//--------------------------------------------------------
	void c_tablebox::_select_new(s32 ypos_, bool onlyhover_)
	{
		s32 oldsel = m_selected_row;
		if(ypos_ < (m_absolute_rect.lt_pt().y() + m_row_height))
			return;
		if(m_row_height!=0)
			m_selected_row = ((ypos_ - m_absolute_rect.lt_pt().y() - m_row_height - 1) + m_scrollpos) / m_row_height;
		if(m_selected_row >= (s32)m_rows.size())
			m_selected_row = m_rows.size() - 1;
		else if (m_selected_row<0)
			m_selected_row = 0;
		if(m_selected_row != oldsel)
		{
			_sig_header_changed.emit(m_selected_row);
			c_ui_manager::get_singleton_ptr()->set_dirty(true);
		}
	}
	//--------------------------------------------------------
	void c_tablebox::_break_text(const c_string& text_, c_string& brokentext_, u32 cwidth_)
	{
		c_string line, linedots;
		utf16 c[2];
		c[1] = L'\0';
		u32 maxlen = cwidth_ - (m_cell_width_padding * 2);
		f32 txtw , txth;
		c_font* ft = c_ui_manager::get_singleton_ptr()->get_font(m_fntid);
		ft->measure_text(L"..." , txtw , txth);
		u32 dontsmaxlLen = cwidth_ - (m_cell_width_padding * 2) - txtw;
		u32 size = text_.get_length();
		u32 pos = 0;
		u32 i;
		for(i=0; i<size; ++i)
		{
			c[0] = text_[i];
			if(c[0] == L'\n')
				break;
			ft->measure_text(c , txtw , txth);
			pos += txtw;
			if(pos > maxlen)
				break;
			c_string tmp = line;
			tmp.append(c);
			ft->measure_text(tmp , txtw , txth);
			if(txtw > dontsmaxlLen)
				linedots = line;
			line += c[0];
		}
		if(i < size)
			brokentext_ = linedots.sub_str(0 , linedots.get_length()) + L"...";
		else
			brokentext_ = line;
	}
	//--------------------------------------------------------
	void c_tablebox::_check_scroll()
	{
		c_rect tabler(m_absolute_rect);
		tabler.lt_pt().x() += 1;
		tabler.lt_pt().y() += 1;
		s32 headerbottom = tabler.lt_pt().y() + m_row_height;
		c_rect clientclip(tabler);
		clientclip.lt_pt().y() = headerbottom + 1;
		if(m_total_height > clientclip.height())
			m_scrollpos = clamp(m_scrollpos , 0.f , m_total_height - clientclip.height());
	}
}