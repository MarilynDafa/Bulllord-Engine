/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef DIALOG_H_INCLUDE
#define DIALOG_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_dialog:public c_widget
	{
	private:
		c_string m_bg_img;
		c_string m_stencil_img;
		c_rect m_stencil_rect;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_float2 m_drag_start;
		c_float2 m_title_dim;
		bool m_dragging;
	public:
		c_dialog(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_dialog();
	public:
		inline const c_float2& get_title_dim() const {return m_title_dim;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
	public:
		void set_stencil_image(const c_string& img_);
		void set_stencil_rect(const c_rect& rect_);
		void set_bg_image(const c_string& img_);
		void set_title_dim(const c_float2& sz_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		bool _gain_focus(c_widget* widget_);
		bool _lost_focus(c_widget* widget_);
		void _on_touch_move();
		void _on_touch_start();
		void _on_touch_finish();
		void _redraw();	
		void _on_close();
	};
}
#endif