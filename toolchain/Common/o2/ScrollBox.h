/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SCROLLBOX_H_INCLUDE
#define SCROLLBOX_H_INCLUDE
#include "Vector.h"
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_scrollbox:public c_widget
	{
	private:
		s32 m_last_recordpos;
		s32 m_scroll_height;
		s32 m_scrollpos;
		s32 m_tolerance;
		f32 m_pan_delta;
		bool m_dragging;
		bool m_strict;
		bool m_elastic;
	public:
		c_scrollbox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_scrollbox();
	public:
		inline bool is_strict() const {return m_strict;}
	public:
		void set_strict(bool stri_);
	protected:
		void _on_touch_move();
		void _on_touch_start();
		void _on_touch_finish();
		void _redraw();	
		void _update(c_technique* uit_, u32 dt_);
	};
}
#endif