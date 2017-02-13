/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef FONT_H_INCLUDE
#define FONT_H_INCLUDE
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_STROKER_H
#include "Str.h"
#include "Map.h"
#include "Stream.h"
#include "Color.h"
#include "Rectangle.h"
#include "Texture.h"
#include "ManagedRes.h"
namespace o2d{
	class O2D_API c_font:public c_resource
	{
	public:
		struct s_glyph_info
		{
			c_texture* tex;
			c_rect rect;
			c_float2 offset;
			u32 adv;
			bool valid;
		};
		struct s_fix_font
		{
			c_map<u32, s_glyph_info*> glyphs;
			c_vector<c_texture*> texs;
			c_vector<c_tex_loader*> loaders;
			c_texture* cur_tex;
			u32 record_x;
			u32 record_y;
			u32 yb;
		};
		struct s_span
		{
			s32 x;
			s32 y;
			s32 width;
			s32 coverage;
		};
	private:
		c_map<u32 , s_fix_font*> m_all_fix;
		c_vector<u32> m_code_range;
		c_vector<u32> m_all_size;
		c_stream m_source;
		c_color m_outline_clr;
		FT_Face m_font_face;    
		FT_Int32 m_flags;
		u32 m_id;
		u32 m_active_height;
		bool m_antialiased;
	public:
		c_font(const utf16* filename_);
		virtual ~c_font();
	public:
		inline u32 get_id() const {return m_id;}
	public:
		s_glyph_info* get_glyph(u32 c_);
		u32 get_text_extent(const c_string& str_, u16 flag);
		s32 get_char_index(const c_string& str_ , s32 pixelx_);
		void text_break_word(c_string& text_, u32 width_, u16 flag);
		void measure_text(const c_string& text_ , f32& outx_ , f32& outy_, u16 flag = 0);
		bool active_height(u32 height_);
		bool serialize_height(u32 height_);
	protected:
		bool _load_impl();
		void _unload_impl();
	private:
		bool _add_glyph(u32 c_, s_glyph_info* gi_ , s_fix_font* ff_);
		void _draw_glyph_2_buffer(u32* buffer_, const c_rect& rc_, const c_vector<s_span>& sp_, const c_vector<s_span>& osp_) const;
		void _draw_glyph_2_buffer(u32* buffer_, u32 bufsz_, bool shadow = false, bool bold = false, bool outline = false) const;
	};
}
#endif
