/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef LABEL_H_INCLUDE
#define LABEL_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_label:public c_widget
	{
	private:
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_geometry_buffer* m_triangle_gb;
		c_string m_bg_img;
		c_rect m_commmon_rect;
		c_string m_text;
		c_float2 m_margin;
		e_text_alignment m_v_align;
		e_text_alignment m_h_align;
		c_rect m_stencil_rect;
		c_string m_stencil_img;
		u32 m_txt_color;
		bool m_wordwrap;
		bool m_clock_wise;
		int m_start_angle;
		int m_end_angle;
		bool m_cut_angle;
	public:
		c_label(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_label();
	public:
		inline bool is_wordwrap() const {return m_wordwrap;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
		inline const c_string& get_text() const {return m_text;}
		inline u32 get_text_clr() const {return m_txt_color;}
		inline e_text_alignment get_text_valign() const {return m_v_align;}
		inline e_text_alignment get_text_halign() const {return m_h_align;}
		inline const c_float2 get_margin() const {return m_margin;}
		inline int get_start_angle() const { return m_start_angle; }
		inline int get_end_angle() const { return m_end_angle; }
		inline bool get_clockwise() const { return m_clock_wise; }
		inline bool get_curAngle() const { return m_cut_angle; }
	public:
		void set_wordwrap(bool wrap_);
		void set_text(const c_string& text_);
		void set_text_color(const c_color& color_);
		void set_text_align(e_text_alignment ha_, e_text_alignment va_);
		void set_margin(const c_float2& margin_);
		void set_bg_image(const c_string& name_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_start_angle(int angle);
		void set_end_angle(int angle);
		void set_stencil_image(const c_string& img_);
		void set_common_image(const c_string& img_);
		void set_clock_wise(bool flag);
		void set_cutAngle(bool falg);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void xiangxian(int angle, c_float2& pt, int& xx, bool& addpt, c_float2& texpt);
		void _redraw();
	};
}
#endif