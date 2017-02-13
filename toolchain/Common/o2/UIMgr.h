/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef UIMGR_H_INCLUDE
#define UIMGR_H_INCLUDE
#include "Singleton.h"
#include "Map.h"
#include "Vector.h"
#include "Stream.h"
#include "Color.h"
#include "Font.h"
#include "Widget.h"
#include "Skin.h"
namespace o2d{
	class O2D_API c_ui_manager:public c_singleton<c_ui_manager>
	{
	public:
		struct s_batch
		{
			c_texture* tex;
			c_stream vb;
		};
	private:
		c_vector<s_batch> m_batches;
		c_vector<c_skin*> m_skins;
		c_vector<c_font*> m_fonts;
		c_map<utf16 , c_rect> m_sysfnt_map;
		c_color m_uu_clr[SDT_COUNT];
		c_tex_loader* m_billboard_loader;
		c_tex_loader* m_placeholder_loader;
		c_geometry_buffer* m_batch_gbo;
		c_geometry_buffer* m_line_gbo;
		c_geometry_buffer* m_billboard_gbo;
		c_technique* m_ui_tech;
		c_framebuffer* m_fbo;
		c_widget* m_environment;
		c_widget* m_hovered_widget;
		c_widget* m_focus_widget;
		volatile bool m_dirty;
		int _width, _height;
	public:
		c_ui_manager();
		virtual ~c_ui_manager();
	public:
		inline c_technique* getTech() {
			return m_ui_tech
				;
		}
		inline c_framebuffer* getFramebuffer() {
			return m_fbo
				;
		}
		inline c_texture* getPlaceHolder()
		{
			return  (c_texture*)m_placeholder_loader->get_res();
		}
		inline int getWidth() const { return _width; }
		inline int getHeight() const { return _height; }
		inline void set_dirty(bool dirty_) {m_dirty = dirty_;}
		inline bool is_dirty() const {return m_dirty;}
		inline c_widget* get_hovered_widget() {return m_hovered_widget;}
		inline c_widget* get_focus_widget() {return m_focus_widget;}
		inline c_widget* get_environment() {return m_environment;}
		inline const c_color& get_uu_color(e_uniform_ui_color idx) const {return m_uu_clr[idx];}
	public:
		bool initialize();
		void shutdown();
		void set_uu_color(e_uniform_ui_color idx_, u32 clr_);
		c_skin* add_skin(const c_string& name_);
		c_skin* get_skin(const c_string& name_);
		bool remove_skin(const c_string& name_);
		c_font* add_font(const c_string& name_);
		bool remove_font(const c_string& name_);
		c_skin* get_skin(u32 skid_);
		c_font* get_font(u32 fntid_);
		c_texture* get_placeholder_tex();
		c_widget* get_widget(u32 id_);
		c_widget* get_widget(const c_string& name_);
		c_widget* add_widget(c_widget* wid_, c_widget* parent_);
		c_widget* add_widget(e_widget_type type_, c_widget* parent_, const c_widget::s_widget_property& property_);	
		void remove_widget(c_widget* widget_);
		void clear();		
		bool set_focus(c_widget* widget_);
		bool remove_focus(c_widget* widget_);
		void reset_hovered();
		void cache_batch(c_texture* tex_, const c_rect& vr_, const c_rect& cr_, bool flipx, bool flipy);
		void cache_batch(c_texture* tex_, const c_rect& tr_, const c_rect& vr_, const c_rect& cr_, bool flipx, bool flipy);
		void cache_batch(u32 skinid_, const c_string& img_, const c_rect& vr_, const c_rect& cr_, bool flipx, bool flipy,bool disable = false);
		void cache_batch(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, bool flipx, bool flipy,bool disable = false);
		void cache_batch_rot(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, int rot, bool clockwise, bool flipx, bool flipy);
		void cache_batch_rot(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, const c_rect& str_, int rot, bool clockwise, bool flipx, bool flipy);
		void cache_batch(u32 skinid_, const c_string& img_, const c_rect& tr_, const c_rect vr_, const c_rect& cr_, const c_rect& str_, bool flipx, bool flipy);
		void cache_batch(e_uniform_ui_color img_, const c_rect& vr_, const c_rect& cr_);
		void cache_batch(u32 clr_, const c_rect& vr_, const c_rect& cr_);
		void cache_batch(u32 fntid_, u32 fth_, u32 fmt_, bool single_, const c_string& txt_, const c_rect& r_, const c_rect& cr_, u32 clr_, bool pwd_=false, u16 flag = 0x0000);
		void cache_batch(u32 fntid_, u32 fth_, const c_string& txt_, const c_float2& pt_,  u32 clr_);
		void cache_batch_lines(c_geometry_buffer* geo, bool fill, const c_color& color);
		void resize(f32 w_, f32 h_);
		void step(u32 dt_);
		void draw(u32 dt_);
	private:
		void _inject_touch_move();
		void _inject_touch_start();
		void _inject_touch_finish();
		void _inject_double_tap();
		void _inject_pan();
		void _inject_char();
		void _update_hovered(const c_float2& pos_);
		void _build_geo_buffer(f32 w_ , f32 h_);
		c_widget* _get_penetrate_widget(c_widget* wid_);
		c_widget* _get_transmit_widget(c_widget* wid_);
	};
}
#endif