/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef WIDGET_H_INCLUDE
#define WIDGET_H_INCLUDE
#include "Rectangle.h"
#include "Str.h"
#include "Device.h"
#include "Signals.h"
namespace o2d{
	class O2D_API c_widget
	{
		FRIEND_CLASS(c_ui_manager)
		FRIEND_CLASS(c_button)
		FRIEND_CLASS(c_cartoonbox)
		FRIEND_CLASS(c_dialog)
		FRIEND_CLASS(c_dragdrop_slot)
		FRIEND_CLASS(c_editbox)
		FRIEND_CLASS(c_label)
		FRIEND_CLASS(c_listbox)
		FRIEND_CLASS(c_menu)
		FRIEND_CLASS(c_progressbar)
		FRIEND_CLASS(c_radio)
		FRIEND_CLASS(c_richbox)
		FRIEND_CLASS(c_scrollbox)
		FRIEND_CLASS(c_slider)
		FRIEND_CLASS(c_tabctl)
		FRIEND_CLASS(c_tablebox)
		FRIEND_CLASS(c_treebox)
	public:
		struct s_widget_property
		{
			c_string name;
			c_rect dim;
			u32 skinid;
			u32 fntid;
			u32 font_height;
		};
	protected:
		c_list<c_widget*> m_children;
		e_widget_type m_class;
		c_widget* m_parent;
		c_rect m_relative_rect;
		c_rect m_absolute_rect;
		c_rect m_clipping_rect;
		c_rect m_desired_rect;
		c_float2 m_max_size;
		c_float2 m_min_size;
		c_string m_tooltip;
		c_geometry_buffer* m_batch_gbo;
		u32 m_id;
		u32 m_skinid;
		u32 m_fntid;
		u32 m_fnt_height;
		u16 m_txt_flag;
		bool m_need_delete;
		bool m_visible;
		bool m_no_clip;
		bool m_transmit;
		bool m_penetrate;
		bool m_abs_scissor;
		bool m_flipx;
		bool m_flipy;
	SIGNALS:
		c_signal<void> _sig_lost_foucs;
		c_signal<void> _sig_gain_foucs;
		c_signal<void> _sig_enter;
		c_signal<void> _sig_leave;
	public:
		c_widget(e_widget_type type_, c_widget* parent_, const s_widget_property& property_);
		virtual ~c_widget();
	public:
		inline void set_id(u32 id_) {m_id = id_;}
		inline u32 get_id() const {return m_id;}
		inline e_widget_type get_class() const {return m_class;}
		inline c_widget* get_parent() { return m_parent;}
		inline const c_list<c_widget*>& get_children() const{return m_children;}
		inline const c_rect& get_region() const{ return m_absolute_rect;}
		inline const c_string& get_tooltip() const {return m_tooltip;}
		inline const c_float2& get_maxsz() const {return m_max_size;}
		inline const c_float2& get_minsz() const {return m_min_size;}
		inline u32 get_skinid() const {return m_skinid;}
		inline u32 get_fnt_height() const {return m_fnt_height;}
		inline u32 get_fntid() const {return m_fntid;}
		inline bool is_visible() const{ return m_visible;}
		inline bool is_no_clipped() const{ return m_no_clip;}
		inline bool is_transmit() const {return m_transmit;}
		inline bool is_penetrate() const {return m_penetrate;}
		inline bool is_abs() const { return m_abs_scissor; }
		inline bool is_flipx() const { return m_flipx; }
		inline bool is_flipy() const { return m_flipy; }
	public:
		void set_visible(bool visible_);
		void set_no_clipped(bool noclip_);
		void set_transmit(bool trans_);
		void set_penetrate(bool per_);
		void set_abs(bool abs_);
		void set_max_size(const c_float2& size_);
		void set_min_size(const c_float2& size_);
		void set_tooltip(const c_string& text_);
		void set_skin(const c_string& skiname_);
		void set_font(const c_string& fntname_, u32 height_, bool added_ = false, bool outline = false, bool shadow = false, bool bold = false, bool intly = false);
		void set_property(const s_widget_property& property_);
		void move(const c_float2& absolutemovement_);
		void resize(f32 w_ , f32 h_);
		bool bring_to_front(c_widget* element_);
		bool check_my_child(c_widget* child_) const;
		c_widget* get_widget(const c_float2& point_);
		c_widget* get_widget(s32 id_, bool searchchildren_=false) const;	
		void set_flipx(bool flag);
		void set_flipy(bool flag);
	protected:
		virtual void _update_position();
		virtual void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		virtual bool _gain_focus(c_widget* widget_);
		virtual bool _lost_focus(c_widget* widget_);
		virtual void _on_touch_move();
		virtual void _on_touch_start();
		virtual void _on_touch_finish();
		virtual void _on_double_tap();
		virtual void _on_pan();
		virtual void _on_char();
		virtual void _redraw();	
		virtual void _update(c_technique* uit_, u32 dt_);
	private:
		void _add_child(c_widget* child_);
		void _remove_child(c_widget* child_);
		void _recalculate_region(bool recursive_);
		void _relative_position(const c_rect& r_);
		void _draw_stencil(const c_string& stencilimg, const c_rect& stencilrect);
	};
}
#endif