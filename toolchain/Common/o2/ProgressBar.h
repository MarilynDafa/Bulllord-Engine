/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef PROGRESSBAR_H_INCLUDE
#define PROGRESSBAR_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_progressbar:public c_widget
	{
	private:
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_rect m_stencil_rect;
		c_string m_stencil_img;
		c_float2 m_offset;
		c_string m_bg_img;
		c_string m_fill_img;
		c_string m_text;
		u32 m_txt_color;
		u32 m_percent;
	SIGNALS:
		c_signal<void , u32> _sig_value_change;
	public:
		c_progressbar(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_progressbar();
	public:
		inline u32 get_percent() const {return m_percent;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
		inline const c_string& get_fill_image() const {return m_fill_img;}
		inline const c_float2& get_offset() const {return m_offset;}
		inline const c_string& get_text() const {return m_text;}
		inline u32 get_text_clr() const {return m_txt_color;}
	public:
		void set_percent(u32 per_);
		void set_offset(const c_float2& offset_);
		void set_text(const c_string& text_);
		void set_text_color(const c_color& color_);
		void set_fill_image(const c_string& name_);
		void set_bg_image(const c_string& name_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_stencil_image(const c_string& img_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _redraw();	
	};
}
#endif