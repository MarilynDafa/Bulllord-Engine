/**************************************************************************************/
//   Copyright (C) 2009-2012	Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
#include "Pass.h"
#include "Device.h"
#include "NameGen.h"
namespace o2d{
	c_pass::c_pass(const c_string& name_)
		:m_name(name_) , 
		m_vs_id(-1) , 
		m_fs_id(-1) , 
#if !defined(O2D_USE_DX_API)
		m_handle(0)
#else
		m_vs(nullptr) , 
		m_ps(nullptr) , 
		m_layout(nullptr) , 
		m_vcbuf(nullptr) , 
		m_pcbuf(nullptr)
#endif
	{
		memset(m_states , -1 , RS_NUM*sizeof(s32));
	}
	//--------------------------------------------------------
	c_pass::~c_pass()
	{
#if !defined(O2D_USE_DX_API)
		glUseProgram(0);
		glDeleteProgram(m_handle);
#else
		m_vs->Release();
		m_ps->Release();
		m_layout->Release();
		if(m_vcbuf)
			m_vcbuf->Release();
		if(m_pcbuf)
			m_pcbuf->Release();
#endif
		FOREACH(TYPEOF(c_map<u32 , s_sampler*>)::iterator , iter , m_samplers)
			delete iter->second();
		FOREACH(TYPEOF(c_map<u16 , s_attribute*>)::iterator , iter , m_attributes)
			delete iter->second();
		FOREACH(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter , m_uniforms)
		{
			switch(iter->second()->type)
			{
			case UT_INTEGER1:
			case UT_INTEGER2:
			case UT_INTEGER3:
			case UT_INTEGER4:
			case UT_INTEGER1_ARRAY:
			case UT_INTEGER2_ARRAY:
			case UT_INTEGER3_ARRAY:
			case UT_INTEGER4_ARRAY:
				free(iter->second()->var.spvar);
				break;
			case UT_FLOAT1:
			case UT_FLOAT2:
			case UT_FLOAT3:
			case UT_FLOAT4:
			case UT_FLOAT1_ARRAY:
			case UT_FLOAT2_ARRAY:
			case UT_FLOAT3_ARRAY:
			case UT_FLOAT4_ARRAY:
			case UT_MATRIX2X2:
			case UT_MATRIX3X3:
			case UT_MATRIX4X4:
			case UT_MATRIX2X2_ARRAY:
			case UT_MATRIX3X3_ARRAY:
			case UT_MATRIX4X4_ARRAY:
				free(iter->second()->var.fpvar);
				break;
			default:
				break;
			}
			delete iter->second();
		}
	}
	//--------------------------------------------------------
	c_pass::s_uniform* c_pass::add_uniform(const c_string& name_, const c_string& type_, u32 count_, bool vs_, u32 handle_/* = -1*/)
	{
		s_uniform* uni = new s_uniform;
		uni->name = name_;
		uni->count = count_;
#if defined(O2D_USE_DX_API)
		uni->vs = vs_;
#endif
		m_uniforms.insert(hash(name_.c_str()) , uni);
		if(type_ == L"int")
		{
			uni->var.spvar = (s32*)malloc(count_*sizeof(s32)*1);
			memset(uni->var.spvar , 0 , count_*sizeof(s32)*1);
			uni->type = (count_ == 1)?UT_INTEGER1:UT_INTEGER1_ARRAY;
		}
		else if(type_ == L"ivec2")
		{
			uni->var.spvar = (s32*)malloc(count_*sizeof(s32)*2);
			memset(uni->var.spvar , 0 , count_*sizeof(s32)*2);
			uni->type = (count_ == 1)?UT_INTEGER2:UT_INTEGER2_ARRAY;
		}
		else if(type_ == L"ivec3")
		{
			uni->var.spvar = (s32*)malloc(count_*sizeof(s32)*3);
			memset(uni->var.spvar , 0 , count_*sizeof(s32)*3);
			uni->type = (count_ == 1)?UT_INTEGER3:UT_INTEGER3_ARRAY;
		}
		else if(type_ == L"ivec4")
		{
			uni->var.spvar = (s32*)malloc(count_*sizeof(s32)*4);
			memset(uni->var.spvar , 0 , count_*sizeof(s32)*4);
			uni->type = (count_ == 1)?UT_INTEGER4:UT_INTEGER4_ARRAY;
		}
		else if(type_ == L"float")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*1);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*1);
			uni->type = (count_ == 1)?UT_FLOAT1:UT_FLOAT1_ARRAY;
		}
		else if(type_ == L"vec2")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*2);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*2);
			uni->type = (count_ == 1)?UT_FLOAT2:UT_FLOAT2_ARRAY;
		}
		else if(type_ == L"vec3")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*3);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*3);
			uni->type = (count_ == 1)?UT_FLOAT3:UT_FLOAT3_ARRAY;
		}
		else if(type_ == L"vec4")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*4);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*4);
			uni->type = (count_ == 1)?UT_FLOAT4:UT_FLOAT4_ARRAY;
		}
		else if(type_ == L"mat2")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*4);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*4);
			uni->type = (count_ == 1)?UT_MATRIX2X2:UT_MATRIX2X2_ARRAY;
		}
		else if(type_ == L"mat3")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*9);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*9);
			uni->type = (count_ == 1)?UT_MATRIX3X3:UT_MATRIX3X3_ARRAY;
		}
		else if(type_ == L"mat4")
		{
			uni->var.fpvar = (f32*)malloc(count_*sizeof(f32)*16);
			memset(uni->var.fpvar , 0 , count_*sizeof(f32)*16);
			uni->type = (count_ == 1)?UT_MATRIX4X4:UT_MATRIX4X4_ARRAY;
		}
		else
			assert(0);
		uni->handle = handle_;
		return uni;
	}
	//--------------------------------------------------------
	c_pass::s_uniform* c_pass::get_uniform(const c_string& name_)
	{
		AUTO(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter ,m_uniforms.find(hash(name_.c_str())));
		if(iter != m_uniforms.end())
			return iter->second();
		else
			return nullptr;
	}
	//--------------------------------------------------------
	c_pass::s_sampler* c_pass::add_sampler(const c_string& name_, u32 unit)
	{
		s_sampler* var = new s_sampler;
		var->name = name_;
		var->unit = unit;
		m_samplers.insert(hash(name_.c_str()) , var);
		return var;
	}
	//--------------------------------------------------------
	c_pass::s_sampler* c_pass::get_sampler(const c_string& name_)
	{
		AUTO(TYPEOF(c_map<u32 , s_sampler*>)::iterator , iter ,m_samplers.find(hash(name_.c_str())));
		if(iter != m_samplers.end())
			return iter->second();
		else
			return nullptr;
	}
	//--------------------------------------------------------
	c_pass::s_attribute* c_pass::add_attribute(e_vertex_element_semantic semantic_, u8 index_, e_vertex_element_type type_, const c_string& name_, u32 handle_/*=1*/)
	{
		s_attribute* att = new s_attribute;
		att->name = name_;
		att->id = MAKE_U16(semantic_,index_);
		att->handle = handle_;
		att->type = type_;
		m_attributes.insert(att->id , att);
		return att;
	}
	//--------------------------------------------------------
	c_pass::s_attribute* c_pass::get_attribute(e_vertex_element_semantic semantic_, u8 index_)
	{
		u16 id = MAKE_U16(semantic_,index_);
		AUTO(TYPEOF(c_map<u16 , s_attribute*>)::iterator , iter ,m_attributes.find(id));
		if(iter != m_attributes.end())
			return iter->second();
		else
		{
			LOG(LL_ERROR , "can not find the attribute semantic: %d , index %d" , (s32)semantic_ , (s32)index_);
			return nullptr;
		}
	}
	//--------------------------------------------------------
	bool c_pass::compile(const ansi* vs_, const ansi* fs_)
	{
		assert(strlen(vs_));
		assert(strlen(fs_));
#if !defined(O2D_USE_DX_API)
		GLint compiled = 0;
		GLint linked = 0;
		m_handle = glCreateProgram();
		GLuint vobject = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vobject, 1, &vs_, nullptr);
		glCompileShader(vobject);
		glGetShaderiv(vobject, GL_COMPILE_STATUS, &compiled);
		if(!compiled)
		{
			GLint len = 0;
			glGetShaderiv(vobject, GL_INFO_LOG_LENGTH, &len);
			if(len)
			{
				ansi* info = (ansi*)malloc((len+1)*sizeof(ansi));
				glGetShaderInfoLog(vobject, len, &len, info);
				c_string msg((utf8*)info);
				LOG(LL_ERROR , (const ansi*)msg.to_utf8());
				free(info);
			}
			return false;
		}
		glAttachShader(m_handle, vobject);
		GLuint fobject = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fobject, 1, &fs_, nullptr);
		glCompileShader(fobject);
		glGetShaderiv(fobject, GL_COMPILE_STATUS, &compiled);
		if(!compiled)
		{
			GLint len = 0;
			glGetShaderiv(fobject, GL_INFO_LOG_LENGTH, &len);
			if(len)
			{
				ansi* info = (ansi*)malloc((len+1)*sizeof(ansi));
				glGetShaderInfoLog(fobject, len, &len, info);
				c_string msg((utf8*)info);
				LOG(LL_ERROR , (const ansi*)msg.to_utf8());
				free(info);
			}
			return false;
		}
		glAttachShader(m_handle, fobject);
		glLinkProgram(m_handle);
		glGetProgramiv(m_handle, GL_LINK_STATUS, &linked);
		if(!linked)
		{
			GLint len = 0;
			glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &len);
			if(len > 0)
			{
				ansi* info = (ansi*)malloc((len+1)*sizeof(ansi));
				glGetProgramInfoLog(m_handle, len, &len, info);
				c_string msg((utf8*)info);
				LOG(LL_ERROR , (const ansi*)msg.to_utf8());
				free(info);
			}
			return false;
		}
		FOREACH(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter , m_uniforms)
		{
			const ansi* uni = (const ansi*)iter->second()->name.to_utf8();
			GLint location = glGetUniformLocation(m_handle, uni);
			if(location != -1)
				iter->second()->handle = location;
		}
		FOREACH(TYPEOF(c_map<u32 , s_sampler*>)::iterator , iter , m_samplers)
		{
			const ansi* sam = (const ansi*)iter->second()->name.to_utf8();
			GLint location = glGetUniformLocation(m_handle, sam);
			if(location != -1)
				iter->second()->handle = location;
		}
		FOREACH(TYPEOF(c_map<u16 , s_attribute*>)::iterator , iter , m_attributes)
		{
			const ansi* att = (const ansi*)iter->second()->name.to_utf8();
			GLint location = glGetAttribLocation(m_handle, att);
			if(location != -1)
				iter->second()->handle = location;
		}
		glDeleteShader(vobject);
		glDeleteShader(fobject);
#else
		HRESULT hr;
		u32 vscbuffersz = 0;
		u32 pscbuffersz = 0;
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		c_stream vscode , fscode;
		vscode.from_base64(vs_);
		fscode.from_base64(fs_);
		hr = device->CreateVertexShader(vscode.get_pointer(), vscode.get_size(), NULL, &m_vs);
		assert(!FAILED(hr));
		hr = device->CreatePixelShader(fscode.get_pointer(), fscode.get_size(), NULL, &m_ps);
		assert(!FAILED(hr));
		FOREACH(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter , m_uniforms)
		{
			bool vs = iter->second()->vs;
			switch(iter->second()->type)
			{
			case UT_INTEGER1:
			case UT_FLOAT1:
				vs?vscbuffersz += 4:pscbuffersz += 4;
				break;
			case UT_INTEGER1_ARRAY:
			case UT_FLOAT1_ARRAY:
				vs?vscbuffersz += iter->second()->count*4:pscbuffersz += iter->second()->count*4;
				break;
			case UT_INTEGER2:
			case UT_FLOAT2:
				vs?vscbuffersz += 8:pscbuffersz += 8;
				break;
			case UT_INTEGER2_ARRAY:
			case UT_FLOAT2_ARRAY:
				vs?vscbuffersz += iter->second()->count*8:pscbuffersz += iter->second()->count*8;
				break;
			case UT_INTEGER3:
			case UT_FLOAT3:
				vs?vscbuffersz += 12:pscbuffersz += 12;
				break;
			case UT_INTEGER3_ARRAY:
			case UT_FLOAT3_ARRAY:
				vs?vscbuffersz += iter->second()->count*12:pscbuffersz += iter->second()->count*12;
				break;
			case UT_INTEGER4:
			case UT_FLOAT4:
				vs?vscbuffersz += 16:pscbuffersz += 16;
				break;
			case UT_INTEGER4_ARRAY:
			case UT_FLOAT4_ARRAY:
				vs?vscbuffersz += iter->second()->count*16:pscbuffersz += iter->second()->count*16;
				break;
			case UT_MATRIX2X2:
				vs?vscbuffersz += 32:pscbuffersz += 32;
				break;
			case UT_MATRIX2X2_ARRAY:
				vs?vscbuffersz += iter->second()->count*32:pscbuffersz += iter->second()->count*32;
				break;
			case UT_MATRIX3X3:
				vs?vscbuffersz += 36:pscbuffersz += 36;
				break;
			case UT_MATRIX3X3_ARRAY:
				vs?vscbuffersz += iter->second()->count*36:pscbuffersz += iter->second()->count*36;
				break;
			case UT_MATRIX4X4:
				vs?vscbuffersz += 64:pscbuffersz += 64;
				break;
			case UT_MATRIX4X4_ARRAY:
				vs?vscbuffersz += iter->second()->count*64:pscbuffersz += iter->second()->count*64;
				break;
			default:assert(0);break;
			};
		}
		u32 offset = 0;
		D3D11_INPUT_ELEMENT_DESC* polygonlayout = new D3D11_INPUT_ELEMENT_DESC[m_attributes.size()];
		s_attribute** attrs = new s_attribute*[m_attributes.size()];
		FOREACH(TYPEOF(c_map<u16 , s_attribute*>)::iterator , iter , m_attributes)
			attrs[iter->second()->handle] = iter->second();
		for(u32 i = 0 ; i < m_attributes.size() ; ++i)
		{			
			polygonlayout[i].SemanticIndex = HI8_BITS(attrs[i]->id);
			polygonlayout[i].InputSlot = 0;
			polygonlayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonlayout[i].InstanceDataStepRate = 0;
			polygonlayout[i].AlignedByteOffset = offset;
			switch(LO8_BITS(attrs[i]->id))
			{
			case VES_POSITION:
				polygonlayout[i].SemanticName = "POSITION";
				break;
			case VES_DIFFUSE:
				polygonlayout[i].SemanticName = "COLOR";
				break;
			case VES_TEXTURE_COORD:
				polygonlayout[i].SemanticName = "TEXCOORD";
				break;
			}
			switch(attrs[i]->type)
			{
			case VET_FLOAT1:
				polygonlayout[i].Format = DXGI_FORMAT_R32_FLOAT;
				offset+=1*sizeof(f32);
				break;
			case VET_FLOAT2:
				polygonlayout[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				offset+=2*sizeof(f32);
				break;
			case VET_FLOAT3:
				polygonlayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				offset+=3*sizeof(f32);
				break;
			case VET_FLOAT4:
				polygonlayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				offset+=4*sizeof(f32);
				break;
			case VET_INTEGER1:
				polygonlayout[i].Format = DXGI_FORMAT_R32_UINT;
				offset+=1*sizeof(s32);
				break;
			case VET_INTEGER2:
				polygonlayout[i].Format = DXGI_FORMAT_R32G32_UINT;
				offset+=2*sizeof(s32);
				break;
			case VET_INTEGER3:
				polygonlayout[i].Format = DXGI_FORMAT_R32G32B32_UINT;
				offset+=3*sizeof(s32);
				break;
			case VET_INTEGER4:
				polygonlayout[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
				offset+=4*sizeof(s32);
				break;
			default:assert(0);break;
			}
		}
		hr = device->CreateInputLayout(polygonlayout, m_attributes.size(), vscode.get_pointer(), vscode.get_size(), &m_layout);
		assert(!FAILED(hr));
		delete []attrs;
		delete []polygonlayout;
		if(vscbuffersz)
		{
			D3D11_BUFFER_DESC bufferdesc;
			bufferdesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferdesc.ByteWidth = (vscbuffersz + 15) / 16 * 16;
			bufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferdesc.MiscFlags = 0;
			bufferdesc.StructureByteStride = 0;
			hr = device->CreateBuffer(&bufferdesc, NULL, &m_vcbuf);
			assert(!FAILED(hr));
		}
		if(pscbuffersz)
		{
			D3D11_BUFFER_DESC bufferdesc;
			bufferdesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferdesc.ByteWidth = (pscbuffersz + 15) / 16 * 16;
			bufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferdesc.MiscFlags = 0;
			bufferdesc.StructureByteStride = 0;
			hr = device->CreateBuffer(&bufferdesc, NULL, &m_pcbuf);
			assert(!FAILED(hr));
		}
#endif
		return true;
	}
	//--------------------------------------------------------
	void c_pass::use()
	{
		c_device::get_singleton_ptr()->set_ds_state(m_states[RS_DEPTH_ENABLE] , (e_compare_function)m_states[RS_DEPTH_FUNC] , 1.f);
		c_device::get_singleton_ptr()->set_blend_state(m_states[RS_BLEND_ENABLE] , 
			(e_blend_operation)m_states[RS_BLEND_OP] , (e_blend_operation)m_states[RS_BLEND_OP_ALPHA] , 
			(e_blend_factor)m_states[RS_SRC_FACTOR] , (e_blend_factor)m_states[RS_DST_FACTOR] , (e_blend_factor)m_states[RS_SRC_ALPHA_FACTOR] , (e_blend_factor)m_states[RS_DST_ALPHA_FACTOR]);
#if !defined(O2D_USE_DX_API)
		glUseProgram(m_handle);
		FOREACH(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter , m_uniforms)
		{
			s_uniform* uni = iter->second();
			switch(uni->type)
			{ 
			case UT_INTEGER1:glUniform1i(uni->handle, uni->var.spvar[0]);break;
			case UT_INTEGER1_ARRAY:glUniform1iv(uni->handle, uni->count, uni->var.spvar);break;
			case UT_INTEGER2:glUniform2i(uni->handle, uni->var.spvar[0] , uni->var.spvar[1]);break;
			case UT_INTEGER2_ARRAY:glUniform2iv(uni->handle, uni->count, uni->var.spvar);break;
			case UT_INTEGER3:glUniform3i(uni->handle, uni->var.spvar[0] , uni->var.spvar[1] , uni->var.spvar[2]);break;
			case UT_INTEGER3_ARRAY:glUniform3iv(uni->handle, uni->count, uni->var.spvar);break;
			case UT_INTEGER4:glUniform4i(uni->handle, uni->var.spvar[0] , uni->var.spvar[1] , uni->var.spvar[2] , uni->var.spvar[3]);break;
			case UT_INTEGER4_ARRAY:glUniform4iv(uni->handle, uni->count, uni->var.spvar);break;
			case UT_FLOAT1:glUniform1f(uni->handle, uni->var.fpvar[0]);break;
			case UT_FLOAT1_ARRAY:glUniform1fv(uni->handle, uni->count, uni->var.fpvar);break;
			case UT_FLOAT2:glUniform2f(uni->handle, uni->var.fpvar[0] , uni->var.fpvar[1]);break;
			case UT_FLOAT2_ARRAY:glUniform2fv(uni->handle, uni->count, uni->var.fpvar);break;
			case UT_FLOAT3:glUniform3f(uni->handle, uni->var.fpvar[0] , uni->var.fpvar[1] , uni->var.fpvar[2]);break;
			case UT_FLOAT3_ARRAY:glUniform3fv(uni->handle, uni->count, uni->var.fpvar);break;
			case UT_FLOAT4:glUniform4f(uni->handle, uni->var.fpvar[0] , uni->var.fpvar[1] , uni->var.fpvar[2] , uni->var.fpvar[3]);break;
			case UT_FLOAT4_ARRAY:glUniform4fv(uni->handle, uni->count, uni->var.fpvar);break;
			case UT_MATRIX2X2:glUniformMatrix2fv(uni->handle , 1 , GL_FALSE , uni->var.fpvar);break;
			case UT_MATRIX3X3:glUniformMatrix3fv(uni->handle , 1 , GL_FALSE , uni->var.fpvar);break;
			case UT_MATRIX4X4:glUniformMatrix4fv(uni->handle , 1 , GL_FALSE , uni->var.fpvar);break;
			case UT_MATRIX2X2_ARRAY:glUniformMatrix2fv(uni->handle , uni->count , GL_FALSE , uni->var.fpvar);break;
			case UT_MATRIX3X3_ARRAY:glUniformMatrix3fv(uni->handle , uni->count , GL_FALSE , uni->var.fpvar);break;
			case UT_MATRIX4X4_ARRAY:glUniformMatrix4fv(uni->handle , uni->count , GL_FALSE , uni->var.fpvar);break;
			default:assert(0);break;
			}
		}
		FOREACH(TYPEOF(c_map<u32 , s_sampler*>)::iterator , iter , m_samplers)
		{
			c_device::get_singleton_ptr()->set_texture_unit(iter->second()->unit);
			iter->second()->tex->apply();
			glUniform1i(iter->second()->handle, iter->second()->unit);
		}
#	ifdef O2D_DEBUG_MODE
		glValidateProgram(m_handle);
		GLint validated = 0;
		glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &validated);
		if(!validated)
		{
			GLint len = 0;
			glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &len);
			if(len)
			{
				ansi* info = (ansi*)malloc((len+1)*sizeof(ansi));
				glGetProgramInfoLog(m_handle, len, &len, info);
				c_string msg((utf8*)info);
				LOG(LL_ERROR , (const ansi*)msg.to_utf8());
				free(info);
			}
		}
#	endif
#else
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		D3D11_MAPPED_SUBRESOURCE mappedres;
		if(m_vcbuf)
		{
			context->Map(m_vcbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedres);
			u8* dataptr = (u8*)mappedres.pData;
			FOREACH(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter , m_uniforms)
			{
				if(iter->second()->vs)
				{
					s_uniform* uni = iter->second();
					switch(uni->type)
					{ 
					case UT_INTEGER1:memcpy(dataptr + uni->handle, uni->var.spvar , 1*sizeof(s32));break;
					case UT_INTEGER1_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*1*sizeof(s32));break;
					case UT_INTEGER2:memcpy(dataptr + uni->handle, uni->var.spvar , 2*sizeof(s32));break;
					case UT_INTEGER2_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*2*sizeof(s32));break;
					case UT_INTEGER3:memcpy(dataptr + uni->handle, uni->var.spvar , 3*sizeof(s32));break;
					case UT_INTEGER3_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*3*sizeof(s32));break;
					case UT_INTEGER4:memcpy(dataptr + uni->handle, uni->var.spvar , 4*sizeof(s32));break;
					case UT_INTEGER4_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*4*sizeof(s32));break;
					case UT_FLOAT1:memcpy(dataptr + uni->handle, uni->var.fpvar , 1*sizeof(f32));break;
					case UT_FLOAT1_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*1*sizeof(f32));break;
					case UT_FLOAT2:memcpy(dataptr + uni->handle, uni->var.fpvar , 2*sizeof(f32));break;
					case UT_FLOAT2_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*2*sizeof(f32));break;
					case UT_FLOAT3:memcpy(dataptr + uni->handle, uni->var.fpvar , 3*sizeof(f32));break;
					case UT_FLOAT3_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*3*sizeof(f32));break;
					case UT_FLOAT4:memcpy(dataptr + uni->handle, uni->var.fpvar , 4*sizeof(f32));break;
					case UT_FLOAT4_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*4*sizeof(f32));break;
					case UT_MATRIX2X2:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*2*2);break;
					case UT_MATRIX3X3:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*3*3);break;
					case UT_MATRIX4X4:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4);break;
					case UT_MATRIX2X2_ARRAY:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4*uni->count);break;
					case UT_MATRIX3X3_ARRAY:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4*uni->count);break;
					case UT_MATRIX4X4_ARRAY:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4*uni->count);break;
					default:assert(0);break;
					}
				}
			}
			context->Unmap(m_vcbuf, 0);
			context->VSSetConstantBuffers(0, 1, &m_vcbuf);
		}
		if(m_pcbuf)
		{
			context->Map(m_pcbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedres);
			u8* dataptr = (u8*)mappedres.pData;
			FOREACH(TYPEOF(c_map<u32 , s_uniform*>)::iterator , iter , m_uniforms)
			{
				if(!iter->second()->vs)
				{
					s_uniform* uni = iter->second();
					switch(uni->type)
					{ 
					case UT_INTEGER1:memcpy(dataptr + uni->handle, uni->var.spvar , 1*sizeof(s32));break;
					case UT_INTEGER1_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*1*sizeof(s32));break;
					case UT_INTEGER2:memcpy(dataptr + uni->handle, uni->var.spvar , 2*sizeof(s32));break;
					case UT_INTEGER2_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*2*sizeof(s32));break;
					case UT_INTEGER3:memcpy(dataptr + uni->handle, uni->var.spvar , 3*sizeof(s32));break;
					case UT_INTEGER3_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*3*sizeof(s32));break;
					case UT_INTEGER4:memcpy(dataptr + uni->handle, uni->var.spvar , 4*sizeof(s32));break;
					case UT_INTEGER4_ARRAY:memcpy(dataptr + uni->handle, uni->var.spvar , uni->count*4*sizeof(s32));break;
					case UT_FLOAT1:memcpy(dataptr + uni->handle, uni->var.fpvar , 1*sizeof(f32));break;
					case UT_FLOAT1_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*1*sizeof(f32));break;
					case UT_FLOAT2:memcpy(dataptr + uni->handle, uni->var.fpvar , 2*sizeof(f32));break;
					case UT_FLOAT2_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*2*sizeof(f32));break;
					case UT_FLOAT3:memcpy(dataptr + uni->handle, uni->var.fpvar , 3*sizeof(f32));break;
					case UT_FLOAT3_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*3*sizeof(f32));break;
					case UT_FLOAT4:memcpy(dataptr + uni->handle, uni->var.fpvar , 4*sizeof(f32));break;
					case UT_FLOAT4_ARRAY:memcpy(dataptr + uni->handle, uni->var.fpvar , uni->count*4*sizeof(f32));break;
					case UT_MATRIX2X2:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*2*2);break;
					case UT_MATRIX3X3:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*3*3);break;
					case UT_MATRIX4X4:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4);break;
					case UT_MATRIX2X2_ARRAY:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4*uni->count);break;
					case UT_MATRIX3X3_ARRAY:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4*uni->count);break;
					case UT_MATRIX4X4_ARRAY:memcpy(dataptr + uni->handle , uni->var.fpvar , sizeof(f32)*4*4*uni->count);break;
					default:assert(0);break;
					}
				}
			}
			context->Unmap(m_pcbuf, 0);
			context->PSSetConstantBuffers(0, 1, &m_pcbuf);
		}
		FOREACH(TYPEOF(c_map<u32 , s_sampler*>)::iterator , iter , m_samplers)
		{
			ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
			c_device::get_singleton_ptr()->set_texture_unit(iter->second()->unit);
			iter->second()->tex->apply();
			ID3D11ShaderResourceView* tex = iter->second()->tex->get_rsview();
			context->PSSetShaderResources(iter->second()->unit , 1, &tex);
		}
		context->VSSetShader(m_vs, NULL, 0);
		context->PSSetShader(m_ps, NULL, 0);
#endif
	}
	//--------------------------------------------------------
	void c_pass::unuse()
	{
#if !defined(O2D_USE_DX_API)
		glUseProgram(0);
#endif
	}
}