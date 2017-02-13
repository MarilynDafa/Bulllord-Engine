/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef RICHBOX_H_INCLUDE
#define RICHBOX_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_richbox:public c_widget
	{
	public:
		struct s_text_element
		{
			c_float2 dim;
			c_string text;
			c_texture* texture;	
			u32 id;
			u32 color;
			u32 fontid;
			u32 txtflag;
			u32 font_height;
			u32 line_width;
			s32 type;		
		};
		struct s_line
		{
			c_list<s_text_element*> elements;
			e_text_alignment halignment;
			e_text_alignment valignment;
			s32 height;
			s32 width;
			s32 left_margin;
			s32 right_margin;
		};
		struct s_anchored_element
		{
			s_text_element* element;
			s32 farx;
			s32 bottomy;
			bool is_left;
		};
		struct s_inline_img
		{
			c_texture* img;
			c_float2 size;
		};
	SIGNALS:
		c_signal<void, u32> _sig_click;
	private:
		c_map<u32, s_inline_img> m_inline_imgs;
		c_vector<s_anchored_element> m_anchored_elements;
		c_vector<s_line*> m_lines;
		s_text_element* m_last_text_ele;
		c_string m_text;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_float2 m_margin;
		c_string m_bg_img;
		s32 m_line_space_remaining;
		s32 m_fullline_space;
		s32 m_line_space;
		s32 m_cur_line;
		s32 m_cur_left_margin;
		s32 m_cur_right_margin;
		s32 m_total_height;
		s32 m_scrollpos;
		u32 m_txt_color;
		u32 m_cur_txt_color;
		u32 m_cur_fnt_height;
		u32 m_cur_txt_flag;
		u32 m_cur_fntid;
		u32 m_cur_linkid;
		c_string m_stencil_img;
		c_rect m_stencil_rect;
	public:
		c_richbox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_richbox();
	public:
		inline const c_string& get_bg_image() const {return m_bg_img;}
		inline const c_string& get_text() const {return m_text;}
		inline u32 get_text_clr() const {return m_txt_color;}
		inline const c_float2 get_margin() const {return m_margin;}
	public:
		void set_text(const c_string& text_);
		void set_text_color(const c_color& color_);
		void set_bg_image(const c_string& name_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_margin(const c_float2& margin_);
		void insert_image(u32 id_, const c_string& path_, const c_float2& sz_);
		void set_stencil_image(const c_string& img_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _on_touch_start();
		void _on_touch_finish();
		void _on_pan();
		void _redraw();
	private:
		void _parse(const c_string& text_);
		c_string _parse_word(const utf16*& c_);
		c_string _parse_whitespace(const utf16*& c_);
		bool _parse_token(const utf16*& c_, c_string& tokenname_, c_vector<c_string>& params_);
		s32 _parse_line_break(const utf16*& c_);
		void _append_text(const c_string& text_, const c_float2& dim_);
		void _insert_text_element(s_text_element* element_);
		void _insert_anchored_element(s_text_element* element_, bool left_);
		void _insert_line_break();
		void _update_margins();
		void _clear_lines();
	};
}
#endif