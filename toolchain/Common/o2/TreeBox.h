/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef TREEBOX_H_INCLUDE
#define TREEBOX_H_INCLUDE
#include "Vector.h"
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	class O2D_API c_treebox : public c_widget
	{
	public:
		struct s_tree_node
		{
			c_list<s_tree_node*> children;
			s_tree_node* parent;
			c_texture* icon;
			c_string text;
			void* data;
			bool expanded;
		};
	private:
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_string m_bg_img;
		s_tree_node* m_root;
		s_tree_node* m_selected;
		s32	m_item_height;
		s32 m_indent_width;
		s32	m_total_height;
		s32 m_total_width;
		s32 m_xoffset;
		s32 m_scrollpos;
		u32 m_txt_color;
		bool m_lines_visible;
	SIGNALS:
		c_signal<void , s_tree_node*> _sig_node_expand;
		c_signal<void , s_tree_node*> _sig_node_collaps;
		c_signal<void , s_tree_node*> _sig_node_selected;
		c_signal<void , s_tree_node*> _sig_node_dselected;
	public:
		c_treebox(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_treebox();
	public:
		inline u32 get_text_clr() const {return m_txt_color;}
		inline s32 get_xoffset() const {return m_xoffset;}
		inline bool is_lines_visible() const { return m_lines_visible; }
		inline s_tree_node* get_root() const { return m_root; }
		inline s_tree_node* get_selected() const { return m_selected; }
	public:
		void set_xoffset(s32 off_);
		void set_lines_visible(bool vis_);
		void set_text_color(const c_color& color_);
		void set_bg_image(const c_string& name_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_selected(s_tree_node* node_, bool selected_);
		void set_expanded(s_tree_node* node_, bool expanded_);
		s_tree_node* add_child(const c_string& text_, const utf16* icon_, s_tree_node* parent_, void* data_ = nullptr);
		void remove_child(s_tree_node* parent_, s_tree_node* child_);
		void clear_children(s_tree_node* parent_);
		bool move_up(s_tree_node* parent_, s_tree_node* child_);
		bool move_down(s_tree_node* parent_, s_tree_node* child_);
		s32 get_level(s_tree_node* node_) const;
	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _on_touch_start();
		void _on_touch_finish();
		void _on_pan();
		void _redraw();
	private:
		bool _is_visible(s_tree_node* node_);
		void _recalculate_item_height();
		s_tree_node* _get_first_child(s_tree_node* node_) const;
		s_tree_node* _get_last_child(s_tree_node* node_) const;
		s_tree_node* _get_prev_sibling(s_tree_node* node_) const;
		s_tree_node* _get_next_sibling(s_tree_node* node_) const;
		s_tree_node* _travel(s_tree_node* node_) const;
	};
}
#endif