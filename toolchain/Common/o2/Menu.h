/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MENU_H_INCLUDE
#define MENU_H_INCLUDE
#include "Vector.h"
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_menu:public c_widget
	{
		struct s_menu_item
		{
			c_menu* sub_menu;
			c_string context;
			c_float2 dim;
			s32 pos_y;
			s32 cmd;
			bool separator;
			bool enabled;
		};
	private:
		c_vector<s_menu_item> m_items;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_string m_bg_img;
		c_string m_arrow_img;
		s32 m_selected;
		u32 m_txt_color;
		bool m_interaction;
		bool m_sub_menu;
	SIGNALS:
		c_signal<void , u32> _sig_item_selected;
	public:
		c_menu(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_menu();
	public:
		inline s32 get_selected_item() const{ return m_selected; }
		inline const c_vector<s_menu_item>& get_items() const {return m_items;}
		inline u32 get_text_clr() const {return m_txt_color;}
	public:
		void set_arrow_image(const c_string& name_);
		void set_bg_image(const c_string& name_);
		void set_text_color(const c_color& color_);
		void set_item(u32 idx_ , const c_string& text_ , s32 cmd_ , bool enabled_);
		u32 add_item(const c_string& text_, s32 cmdid_,bool enabled_);		
		u32 insert_item(u32 idx_, const c_string& text_, s32 cmdid_, bool enabled_);
		s32 find_item(s32 cmdid_, u32 idx_) const;
		void add_separator();
		void remove_item(u32 idx_);
		void remove_all();
		c_menu* add_sub_menu(u32 idx_ , const c_string& name_);
		c_menu* get_sub_menu(u32 idx_) const;
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _on_touch_move();
		void _on_touch_finish();
		bool _lost_focus(c_widget* widget_);
		bool _gain_focus(c_widget* widget_);
		void _redraw();	
	private:
		u32 _send_click(const c_float2& pt_);
		void _update_size();
		bool _select(const c_float2& p_, bool canopensubmenu_);
		c_rect _get_selected_rect(const s_menu_item& i_, const c_rect& absolute_) const;
		void _hide_sub_menu(c_menu* menu_);
	};
}
#endif