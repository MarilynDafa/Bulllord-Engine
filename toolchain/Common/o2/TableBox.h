/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef TABLEBOX_H_INCLUDE
#define TABLEBOX_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_tablebox:public c_widget
	{
		struct s_table_cell
		{
			c_string text;
			c_string broken_text;
			void* data;
		};
		struct s_table_raw
		{
			c_vector<s_table_cell> items;
		};
		struct s_table_column
		{
			c_string name;
			u32 width;
		};
	private:
		c_vector<s_table_column> m_columns;
		c_vector<s_table_raw> m_rows;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_string m_bg_img;
		s32 m_row_height;
		s32 m_total_height;
		s32 m_total_width;
		s32 m_selected_row;
		s32 m_cell_height_padding;
		s32 m_cell_width_padding;
		s32 m_active_column;
		s32 m_scrollpos;
		u32 m_txt_color;
		bool m_row_separator;
		bool m_col_separator;
		bool m_selecting;
		c_rect m_stencil_rect;
		c_string m_stencil_img;
		c_rect m_odd_rect;
		c_string m_odd_img;
		c_rect m_even_rect;
		c_string m_even_img;
	SIGNALS:
		c_signal<void , s32> _sig_header_changed;
	public:
		c_tablebox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_tablebox();
	public:
		inline s32 get_active_column() const { return m_active_column; }
		inline s32 get_selected_row() const { return m_selected_row; }
		inline bool is_row_separator() const {return m_row_separator;}
		inline bool is_col_separator() const {return m_col_separator;}
		inline u32 get_text_clr() const { return m_txt_color;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
	public:
		void set_bg_image(const c_string& img_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_text_color(const c_color& color_);
		void set_separator(bool row_, bool col_);
		void set_cell_text(u32 rindex_, u32 cindex_, const c_string& text_);
		void set_cell_data(u32 rindex_, u32 cindex_, void* data_);
		const c_string& get_cell_text(u32 rindex_, u32 cindex_ ) const;
		void* get_cell_data(u32 rindex_, u32 cindex_ ) const;
		void add_column(const c_string& caption_, s32 cindex_);
		void remove_column(u32 cindex_);
		bool set_active_column(s32 idx_);
		void set_column_width(u32 cindex_, u32 width_);
		u32 get_column_width(u32 cindex_) const;
		s32 get_column_count() const;
		void set_caption(u32 idx_, const c_string& caption_);
		const c_string& get_caption(u32 idx_) const;
		u32 add_row(u32 rindex_);
		void remove_row(u32 rindex_);
		void set_selected_row( s32 index_ );
		s32 get_row_count() const;
		void clear_rows();
		void clearall();
		void set_stencil_image(const c_string& img_);
		void set_odd_image(const c_string& img_);
		void set_even_image(const c_string& img_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _update_position();
		bool _lost_focus(c_widget* widget_);
		void _on_touch_move();
		void _on_touch_start();
		void _on_touch_finish();
		void _on_pan();
		void _redraw();	
	private:
		void _refresh_controls();
		void _recalculate_widths();
		void _recalculate_heights();
		bool _select_column(s32 xpos_, s32 ypos_);
		void _select_new(s32 ypos_, bool onlyhover_);
		void _break_text(const c_string& text_, c_string& brokentext_, u32 cwidth_);
		void _check_scroll();
	};
}
#endif