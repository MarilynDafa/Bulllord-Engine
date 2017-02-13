/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SKIN_H_INCLUDE
#define SKIN_H_INCLUDE
#include "Map.h"
#include "Rectangle.h"
#include "Texture.h"
#include "ManagedRes.h"
namespace o2d{
	class O2D_API c_skin:public c_resource
	{
	private:
		c_map<u32 , c_rect> m_texcoord_map;
		c_tex_loader* m_skin_loader;
		c_stream m_tex_source;
		u32 m_id;
	public:
		c_skin(const utf16* filename_);
		virtual ~c_skin();
	public:
		inline u32 get_id() const {return m_id;}
	public:
		c_texture* get_skin_tex();
		const c_rect& get_texcoord(const c_string& name_) const;
		void setup();
	protected:
		bool _load_impl();
		void _unload_impl();
	};
}
#endif