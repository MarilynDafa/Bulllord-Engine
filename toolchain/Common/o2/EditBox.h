/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef EDITBOX_H_INCLUDE
#define EDITBOX_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_editbox:public c_widget
	{
	private:
		c_geometry_buffer* m_gb;
		c_vector<c_string> m_broken_text;
		c_vector<s32> m_broken_positions;
		c_rect m_cur_rect;
		c_rect m_frame_rect;
		c_string m_bg_img;
		c_string m_clipboard;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_string m_text;
		c_float2 m_margin;
		e_text_alignment m_v_align;
		e_text_alignment m_h_align;
		u32 m_txt_color;
		s32 m_min_value;
		s32 m_max_value;
		s32 m_select_begin;
		s32 m_select_end;
		s32 m_cursor_pos;
		s32 m_h_scroll_pos;
		c_rect m_stencil_rect;
		c_string m_stencil_img;
		s32 m_v_scroll_pos;
		u32 m_max_chars;
		u32 m_last_record;
		bool m_selecting;
		bool m_autoscroll;
		bool m_wordwrap;
		bool m_multiline;
		bool m_password_box;
		bool m_numeric_box;
		bool m_enable;
	SIGNALS:
		c_signal<void , utf16> _sig_input;
		c_signal<void> _sig_select_change;
		c_signal<void> _sig_input_enter;
	public:
		c_editbox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_editbox();
	public:
		inline bool is_autoscroll() const {return m_autoscroll;}
		inline bool is_wordwrap() const {return m_wordwrap;}
		inline bool is_multiline() const {return m_multiline;}
		inline bool is_password_box() const {return m_password_box;}
		inline bool is_numeric_box() const {return m_numeric_box;}
		inline bool is_enable() const {return m_enable;}
		inline u32 get_max() const {return m_max_chars;}
		inline s32 get_min_value() const {return m_min_value;}
		inline s32 get_max_value() const {return m_max_value;}
		inline const c_float2 get_margin() const {return m_margin;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
		inline const c_string& get_text() const {return m_text;}
		inline u32 get_text_clr() const {return m_txt_color;}
		inline e_text_alignment get_text_valign() const {return m_v_align;}
		inline e_text_alignment get_text_halign() const {return m_h_align;}
	public:
		void set_enable(bool enable_);
		void set_wordwrap(bool flag_);
		void set_multiline(bool flag_);
		void set_autoscroll(bool flag_);
		void set_password_box(bool flag_);
		void set_numeric_box(bool flag_);
		void set_max(u32 maxc_);
		void set_margin(const c_float2& margin_);
		void set_range(const c_float2& range_);
		void set_text(const c_string& text_);
		void set_text_color(const c_color& clr_);
		void set_text_align(e_text_alignment ha_, e_text_alignment va_);
		void set_bg_image(const c_string& image_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_stencil_image(const c_string& img_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _update_position();
		bool _lost_focus(c_widget* widget_);
		void _on_touch_move();
		void _on_touch_start();
		void _on_touch_finish();
		void _on_pan();
		void _on_char();
		void _redraw();	
		void _update(c_technique* uit_, u32 dt_);
	private:
		s32 _get_line(s32 pos_);
		s32 _get_cursor_pos(const c_float2& pos_);
		void _set_text_select(s32 begin_, s32 end_);
		void _calculate_scroll_pos();
		void _calculate_frame_rect();
		void _set_text_rect(s32 line_);	
		void _break_text();		
		void _check_numeric(const c_string& old_);
		bool _function_key(utf16 ch_);
		bool _is_printable(utf16 ch_);
	};
}
#endif