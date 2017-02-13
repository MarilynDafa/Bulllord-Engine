/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef PASS_H_INCLUDE
#define PASS_H_INCLUDE		  
#include "Str.h"
#include "Map.h"
#include "Texture.h"
namespace o2d{
	class O2D_API c_pass
	{
		FRIEND_CLASS(c_technique)
		FRIEND_CLASS(c_shader)
		FRIEND_CLASS(c_shader_loader)
		FRIEND_CLASS(c_post_process)
	public:
		struct s_sampler
		{
			c_string name;
			c_texture* tex;
			u32 handle;
			u32 unit;
		};
		struct s_uniform
		{
			c_string name;
			e_uniform_type type;
			u32 handle;
			u32 count;
			union {
				s32* spvar;
				f32* fpvar;
			}var;
#if defined(O2D_USE_DX_API)
			bool vs;
#endif
		};
		struct s_attribute
		{
			c_string name;
			u16 id;
			s32 handle;
			e_vertex_element_type type;
		};
	private:
		c_map<u32 , s_uniform*> m_uniforms;
		c_map<u32 , s_sampler*> m_samplers;
		c_map<u16 , s_attribute*> m_attributes;
#if !defined(O2D_USE_DX_API)
		GLuint m_handle;
#else
		ID3D11VertexShader* m_vs;
		ID3D11PixelShader* m_ps;
		ID3D11InputLayout* m_layout;
		ID3D11Buffer* m_vcbuf;
		ID3D11Buffer* m_pcbuf;
#endif
		s32 m_states[RS_NUM];
		c_string m_name;
		u32 m_vs_id;
		u32 m_fs_id;
	public:
		c_pass(const c_string& name);
		~c_pass();
	public:
		inline const c_string& get_name() const {return m_name;}
#if !defined(O2D_USE_DX_API)
		inline GLuint get_handle() const {return m_handle;}
#else
		inline ID3D11InputLayout* get_layout() {return m_layout;}
#endif
	public:
		s_uniform* add_uniform(const c_string& name_, const c_string& type_, u32 count_, bool vs_, u32 handle_ = -1);
		s_uniform* get_uniform(const c_string& name_);
		s_sampler* add_sampler(const c_string& name_, u32 unit_);
		s_sampler* get_sampler(const c_string& name_);
		s_attribute* add_attribute(e_vertex_element_semantic semantic_, u8 index_, e_vertex_element_type type_, const c_string& name_, u32 handle_ = -1);
		s_attribute* get_attribute(e_vertex_element_semantic semantic_, u8 index_);
		bool compile(const ansi* vs_, const ansi* fs_);
		void use();
		void unuse();
	};
}
#endif