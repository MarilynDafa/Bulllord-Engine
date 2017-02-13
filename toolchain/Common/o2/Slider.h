/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SLIDER_H_INCLUDE
#define SLIDER_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_slider:public c_widget
	{
	private:
		c_rect m_thumb_rect;
		c_float2 m_thumb_size;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_string m_bg_img;
		c_string m_thumb_img;
		c_string m_thumb_disable_img;
		c_string m_stencil_img;
		c_rect m_stencil_rect;
		s32 m_position;
		s32 m_draw_pos;
		s32 m_min_value;
		s32 m_max_value;
		s32 m_step;
		s32 m_desired_pos;
		bool m_dragging;
		bool m_horizontal;
		bool m_thumb_dragged;
		bool m_tray_click;
		bool m_enable;
	SIGNALS:
		c_signal<void , u32> _sig_value_change;
	public:
		c_slider(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_slider();
	public:
		inline bool is_enable() const {return m_enable;}
		inline bool is_horizontal() const {return m_horizontal;}
		inline u32 get_position() const {return m_position;}
		inline s32 get_max_value() const {return m_max_value;}
		inline s32 get_min_value() const {return m_min_value;}
		inline s32 get_step() const {return m_step;}
		inline const c_float2& get_thumb_size() const {return m_thumb_size;}
		inline const c_string& get_thumb_image() const {return m_thumb_img;}
		inline const c_string& get_thumb_disable_image() const {return m_thumb_disable_img;}
	public:
		void set_stencil_image(const c_string& img_);
		void set_range(const c_float2& range_);
		void set_step(s32 step_);
		void set_horizontal(bool flag_);
		void set_position(s32 pos_);
		void set_bg_image(const c_string& name_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_thumb_image(const c_string& img_);
		void set_thumb_disable_image(const c_string& img_);
		void set_thumb_size(const c_float2& sz_);
		void set_enable(bool enable_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _update_position();
		void _on_touch_move();
		void _on_touch_start();
		void _on_touch_finish();
		void _redraw();	
	private:
		s32 _get_pos_from_mouse(const c_float2& pos_) const;
	};
}
#endif