/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef TABCTL_H_INCLUDE
#define TABCTL_H_INCLUDE
#include "Vector.h"
#include "Color.h"
#include "Texture.h"
#include "Font.h"
#include "Widget.h"
namespace o2d{
	struct sGeo
	{
		c_geometry_buffer* geo;
		u32 clolor;
		bool fill;
	};

	struct geoCache
	{
		c_vector<c_float2> points;
		u32 col;
		bool closed;
		bool fill;
	};

	class O2D_API c_tabctl:public c_widget
	{
	public:
		struct s_tab
		{
			c_vector<u32> children;
			c_string name;
			u32 width;
		};
	SIGNALS:
		c_signal<void , s32> _sig_tab_change;
	private:	
		c_vector<s_tab> m_tabs;
		c_rect m_bg_rect[UC_NUM];
		c_rect m_bg_zoom_region;
		c_string m_bg_img;
		c_string m_tab_nimage;
		c_string m_tab_aimage;
		u32 m_txt_color;
		u32 m_page_offset;
		u32 m_active_tab;




		unsigned int            _VtxCurrentIdx;     // [Internal] == VtxBuffer.Size
		geoCache _geocache;
		sGeo _geos;
		bool m_tab_posup;
	public:
		c_tabctl(c_widget* parent_, const c_widget::s_widget_property& property_);
		virtual ~c_tabctl();
	public:
		inline bool is_tab_posup() const {return m_tab_posup;}
		inline u32 get_active_tab() const {return m_active_tab;}
		inline u32 get_text_clr() const {return m_txt_color;}
		inline u32 get_page_offset() const {return m_page_offset;}
		inline const c_string& get_bg_image() const {return m_bg_img;}
		inline const c_string& get_tab_nimage() const {return m_tab_nimage;}
		inline const c_string& get_tab_aimage() const {return m_tab_aimage;}
		inline const s_tab& get_tab(u32 idx_) const {return m_tabs[idx_];}
	public:
		void set_tab_posup(bool up_);
		void set_page_offset(u32 offset_);
		void set_bg_image(const c_string& name_);
		void set_bg_fragment(const c_float2& anc_, const c_float2& dim_);
		void set_tab_nimage(const c_string& name_);
		void set_tab_aimage(const c_string& name_);
		void set_active(u32 idx_);
		void set_belong(u32 idx_, u32 id_);
		void add_tab(const c_string& name_);
		void rename_tab(u32 idx_, const c_string& name_);
		void remove_tab(u32 idx_);
		void set_tab_width(u32 idx_, u32 width_);
		void set_text_color(const c_color& color_);
		void clear_all();
		u32 get_tab_count() const;


		void clear();
		void AddPolyline(const c_vector<c_float2>& points, u32 col, bool closed, bool fill, bool cache = true);
		void AddPolylineFilled(const c_vector<c_float2>& points, u32 col, bool closed, bool fill, bool cache = true);

	protected:
		void _zoom_update(const c_float2& v1_, const c_float2& v2_);
		void _on_touch_start();
		void _on_touch_finish();
		void _redraw();	
	};
}
#endif