/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef DRAGDROPSLOT_H_INCLUDE
#define DRAGDROPSLOT_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_dragdrop_slot:public c_widget
	{
		struct s_icon
		{
			u32 id;
			c_texture* tex;
		};
	private:
		s_icon m_icon;
		c_geometry_buffer* m_clock_gb;
		c_geometry_buffer* m_triangle_gb[7];
		c_texture* m_mask_tex;
		c_string m_bg_img;
		c_string m_text;
		e_text_alignment m_v_align;
		e_text_alignment m_h_align;
		u32 m_cd;
		u32 m_last_record;
		u32 m_txt_color;
		bool m_in_cd;
		bool m_pressed;
		bool m_dragging;
	SIGNALS:
		c_signal<void> _sig_click;
		c_signal<void> _sig_dtap;
		c_signal<void , f32 , f32> _sig_release;
	public:
		c_dragdrop_slot(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_dragdrop_slot();
	public:
		inline bool is_in_cd() const {return m_in_cd;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
		inline const c_string& get_text() const {return m_text;}
		inline u32 get_text_clr() const {return m_txt_color;}
		inline e_text_alignment get_text_valign() const {return m_v_align;}
		inline e_text_alignment get_text_halign() const {return m_h_align;}
	public:
		void set_text(const c_string& text_);
		void set_text_color(const c_color& color_);
		void set_text_align(e_text_alignment ha_, e_text_alignment va_);
		void set_bg_image(const c_string& name_);
		void set_mask_image(const c_string& mtex_);
		void set_cd(u32 ms_);
		void set_icon(u32 id_, const c_string& tex_);
		void reset_icon();
		void cooldown();
	protected:
		void _on_touch_move();
		void _on_touch_start();
		void _on_touch_finish();
		void _on_double_tap();
		void _redraw();
		void _update(c_technique* uit_, u32 dt_);
	private:		
		s32 _calc_pt_list(const c_rect& r_, f32 theta_, c_stream& data_);
	};
}
#endif