/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef GEOMETRYBUFFER_H_INCLUDE
#define GEOMETRYBUFFER_H_INCLUDE
#include "CoreProfile.h"
#include "Vector.h"
#include "Map.h"
#include "Pass.h"
namespace o2d{
	class O2D_API c_geometry_buffer
	{
	public:
		struct s_vertex_element
		{
			e_vertex_element_semantic semantic;
			e_vertex_element_type type;
			u8 index;
		};
		struct s_vertex_buffer
		{
			c_vector<s_vertex_element> format;
#if !defined(O2D_USE_DX_API)
			GLuint handle;
#else
			ID3D11Buffer* handle;
#endif
			u32 stride;
			u32 size;			
		};
		struct s_index_buffer
		{
			e_index_format format;
#if !defined(O2D_USE_DX_API)
			GLuint handle;
#else
			ID3D11Buffer* handle;
#endif
			u32 size;
		};
	private:
		s_vertex_buffer m_vertex_buffer;
		s_index_buffer m_index_buffer;
		e_render_operation m_prim_type;
	public:
		c_geometry_buffer(e_render_operation type_);
		virtual ~c_geometry_buffer();
	public:
		inline bool is_32_index() const{ return m_index_buffer.format == IF_32UI; }
		inline e_render_operation get_prim_type() const {return m_prim_type;}
#if !defined(O2D_USE_DX_API)
		inline GLuint get_vertex_buffer() const {return m_vertex_buffer.handle;}
		inline GLuint get_index_buffer() const {return m_index_buffer.handle;}
#endif
		inline const c_vector<s_vertex_element>& get_vertex_fmt() const { return m_vertex_buffer.format;}
	public:
		u32 num_vertex() const;
		u32 num_index() const;
		bool use_index() const;
		void bind_vertex_buffer(e_buffer_usage usage_, void* data_, u32 sz_, const c_vector<s_vertex_element>& fmts_);
		void bind_index_buffer(e_buffer_usage usage_, void* data_, u32 sz_, e_index_format fmts_);	
		void uploadvb(u32 offset_, void* data_, s32 sz_);
		void uploadib(u32 offset_, void* data_, s32 sz_);
		void assgin_attributes(c_pass* pass_);
	};
}
#endif