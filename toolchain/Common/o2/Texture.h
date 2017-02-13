/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef TEXTURE_H_INCLUDE
#define TEXTURE_H_INCLUDE
#include "CoreProfile.h"
#include "Rectangle.h"
#include "Vector.h"
#include "Stream.h"
#include "ManagedRes.h"
#include "ManualLoader.h"
namespace o2d{
	//////////////////////////////////////////////////////////////////////////
	//!c_texture
	class O2D_API c_texture:public c_resource
	{
		FRIEND_CLASS(c_tex_loader)
		struct s_ktx_header
		{
			u8 identifier[12];
			u32 endianness;
			u32 gl_type;
			u32 gl_type_size;
			u32 gl_format;
			u32 gl_internal_format;
			u32 gl_base_internal_format;
			u32 pixel_width;
			u32 pixel_height;
			u32 pixel_depth;
			u32 number_of_array_elements;
			u32 number_of_faces;
			u32 number_of_mipmap_levels;
			u32 bytes_of_key_value_data;
		};
	private:
		c_vector<u8> m_datablock;
		c_vector<void*> m_data;
#if !defined(O2D_USE_DX_API)
		GLuint m_handle;
#else
		ID3D11ShaderResourceView* m_d3d_srview;
		ID3D11Texture2D* m_d3d_tex;
#endif
		e_filter_options m_min_filter;
		e_filter_options m_mag_filter;
		s32 m_internal_fmt;
		u32 m_num_mipmaps;
		u32 m_width;
		u32 m_height;
		bool m_from_disk;
		bool m_dirty[2];
	public:
		c_texture(const utf16* filename_);
		virtual ~c_texture();	
	public:
#if !defined(O2D_USE_DX_API)
		inline GLuint get_handle() const {return m_handle;}
#else
		inline ID3D11Texture2D* get_handle() {return m_d3d_tex;}
		inline ID3D11ShaderResourceView* get_rsview() {return m_d3d_srview;}
#endif
		inline e_filter_options get_min_filter() const {return m_min_filter;}
		inline e_filter_options get_mag_filter() const {return m_mag_filter;}
		inline u32 get_num_mipmaps() const {return m_num_mipmaps;}
		inline u32 get_width() const {return m_width;}
		inline u32 get_height() const {return m_height;}
	public:
		void set_filter(e_filter_options minfilter_, e_filter_options magfilter_);
		void upload(u32 xoffset_, u32 yoffset_, u32 lvl_, u32 w_, u32 h_, void* data_, s32 sz_);
		void apply();
		void setup();
	protected:
		bool _load_impl();
		void _unload_impl();
	private:
		bool _decode_rgba(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_);
		bool _decode_atitc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_);
		bool _decode_s3tc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_);
		bool _decode_etc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_);		
		bool _decode_pvrtc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_);
	};
	//////////////////////////////////////////////////////////////////////////
	//!c_tex_loader
	class O2D_API c_tex_loader:public c_manual_loader
	{
	private:
		bool m_usert;
	public:
		c_tex_loader(bool usert_, u32 w_, u32 h_ = 0);
		virtual ~c_tex_loader();
	public:
		bool load();
		void unload();
	};
}
#endif