/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef LISTBOX_H_INCLUDE
#define LISTBOX_H_INCLUDE
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_listbox:public c_widget
	{
		struct s_list_item
		{
			c_string text;
			u32 color;
		};
	private:
		c_vector<s_list_item> m_items;
		c_string m_bg_img;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		s32 m_selected;
		s32 m_row_height;
		s32 m_total_height;
		u32 m_txt_color;
		s32 m_scrollpos;
		bool m_selecting;
	SIGNALS:
		c_signal<void , u32> _sig_selected_changed;
	public:
		c_listbox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_listbox();
	public:
		inline s32 get_selected() const {return m_selected;}
		inline s32 get_row_height() const {return m_row_height;}
		inline u32 get_text_clr() const {return m_txt_color;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
	public:
		void set_row_height(s32 height_);
		void set_bg_image(const c_string& img_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_text_color(const c_color& color_);
		void set_selected(s32 id_);
		void set_selected(const c_string& item_);
		const c_string& get_item(u32 id_) const;
		const c_string& get_item(s32 xpos_, s32 ypos_) const;
		u32 get_item_count() const;
		u32 add_item(const c_string& text_);
		u32 insert_item(u32 index_, const c_string& text_);
		void set_item(u32 index_, const c_string& text_);
		void swap(u32 index1_, u32 index2_);
		void remove_item(u32 id_);
		void clearall();
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
		void _select_new(s32 ypos_);
	};
}
#endif