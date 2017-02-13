/**************************************************************************************/
//   Copyright (C) 2009-2012	Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Algorithm.h"
#include "Macro.h"
#include "System.h"
#include "Tools.h"
#include "GeometryBuffer.h"
#include "Device.h"
#include "Mapping.h"
namespace o2d{
	c_geometry_buffer::c_geometry_buffer(e_render_operation type_)
		:m_prim_type(type_)
	{
		memset(&m_vertex_buffer , 0 , sizeof(s_vertex_buffer));
		memset(&m_index_buffer , 0 , sizeof(s_index_buffer));
#if !defined(O2D_USE_DX_API)
		m_vertex_buffer.handle = GL_INVALID_BUFFER;
		m_index_buffer.handle = GL_INVALID_BUFFER;	
#else
		m_vertex_buffer.handle = nullptr;
		m_index_buffer.handle = nullptr;	
#endif
	}
	//--------------------------------------------------------
	c_geometry_buffer::~c_geometry_buffer()
	{
#if !defined(O2D_USE_DX_API)
		if(m_vertex_buffer.handle!=GL_INVALID_BUFFER)
			glDeleteBuffers(1, &m_vertex_buffer.handle);
		if(m_index_buffer.handle!=GL_INVALID_BUFFER)
			glDeleteBuffers(1, &m_index_buffer.handle);
#else
		if(m_vertex_buffer.handle)
			m_vertex_buffer.handle->Release();
		if(m_index_buffer.handle)
			m_index_buffer.handle->Release();
#endif
	}
	//--------------------------------------------------------
	u32 c_geometry_buffer::num_vertex() const
	{
		return m_vertex_buffer.size / m_vertex_buffer.stride;
	}
	//--------------------------------------------------------
	u32 c_geometry_buffer::num_index() const
	{
#if !defined(O2D_USE_DX_API)
		if(m_index_buffer.handle!=GL_INVALID_BUFFER)
#else
		if(m_index_buffer.handle)
#endif
			return m_index_buffer.size / ((m_index_buffer.format == IF_16UI)? 2:4);
		return 0;
	}
	//--------------------------------------------------------
	bool c_geometry_buffer::use_index() const
	{
		return num_index() != 0;
	}
	//--------------------------------------------------------
	void c_geometry_buffer::bind_vertex_buffer(e_buffer_usage usage_, void* data_, u32 sz_, const c_vector<s_vertex_element>& fmts_)
	{
		if(!sz_)
			return;
#if !defined(O2D_USE_DX_API)
		if(m_vertex_buffer.handle!=GL_INVALID_BUFFER)
			glDeleteBuffers(1, &m_vertex_buffer.handle);
		GLuint stride = 0;
		for (u32 i = 0; i < fmts_.size(); ++ i)
		{
			switch(fmts_[i].type)
			{
			case VET_FLOAT1:stride += 4*1;break;
			case VET_FLOAT2:stride += 4*2;break;
			case VET_FLOAT3:stride += 4*3;break;
			case VET_FLOAT4:stride += 4*4;break;
			case VET_INTEGER1:stride += 4*1;break;
			case VET_INTEGER2:stride += 4*2;break;
			case VET_INTEGER3:stride += 4*3;break;
			case VET_INTEGER4:stride += 4*4;break;
			default:assert(0);break;
			}
		}
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sz_ , data_ , map(usage_));
		m_vertex_buffer.handle = vbo;
#else
		u32 stride = 0;
		for(u32 i = 0; i < fmts_.size(); ++ i)
		{
			switch(fmts_[i].type)
			{
			case VET_FLOAT1:stride += 4*1;break;
			case VET_FLOAT2:stride += 4*2;break;
			case VET_FLOAT3:stride += 4*3;break;
			case VET_FLOAT4:stride += 4*4;break;
			case VET_INTEGER1:stride += 4*1;break;
			case VET_INTEGER2:stride += 4*2;break;
			case VET_INTEGER3:stride += 4*3;break;
			case VET_INTEGER4:stride += 4*4;break;
			default:assert(0);break;
			}
		}
		D3D11_BUFFER_DESC vbdesc;
		D3D11_SUBRESOURCE_DATA vb;
		vbdesc.Usage = map(usage_);
		vbdesc.ByteWidth = sz_;
		vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbdesc.CPUAccessFlags = (usage_ == BU_STATIC_DRAW)?0:D3D11_CPU_ACCESS_WRITE;
		vbdesc.MiscFlags = 0;
		vbdesc.StructureByteStride = 0;
		vb.pSysMem = data_;
		vb.SysMemPitch = 0;
		vb.SysMemSlicePitch = 0;
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		HRESULT hr = device->CreateBuffer(&vbdesc, data_?&vb:nullptr, &(m_vertex_buffer.handle));	
		assert(!FAILED(hr));
#endif
		m_vertex_buffer.size = sz_;
		m_vertex_buffer.format = fmts_;
		m_vertex_buffer.stride = stride;
	}
	//--------------------------------------------------------
	void c_geometry_buffer::bind_index_buffer(e_buffer_usage usage_, void* data_, u32 sz_, e_index_format fmts_)
	{
		if(!sz_)
			return;
#if !defined(O2D_USE_DX_API)
		if(m_index_buffer.handle!=GL_INVALID_BUFFER)
			glDeleteBuffers(1, &m_index_buffer.handle);
		GLuint ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sz_ , data_, map(usage_));
		m_index_buffer.handle = ibo;
#else
		D3D11_BUFFER_DESC ibdesc;
		D3D11_SUBRESOURCE_DATA ib;
		ibdesc.Usage = map(usage_);
		ibdesc.ByteWidth = sz_;
		ibdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibdesc.CPUAccessFlags = (usage_ == BU_STATIC_DRAW)?0:D3D11_CPU_ACCESS_WRITE;
		ibdesc.MiscFlags = 0;
		ibdesc.StructureByteStride = 0;
		ib.pSysMem = data_;
		ib.SysMemPitch = 0;
		ib.SysMemSlicePitch = 0;
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		HRESULT hr = device->CreateBuffer(&ibdesc, data_?&ib:nullptr, &(m_index_buffer.handle));
		assert(!FAILED(hr));
#endif
		m_index_buffer.size = sz_;
		m_index_buffer.format = fmts_;
	}
	//--------------------------------------------------------
	void c_geometry_buffer::uploadvb(u32 offset_, void* data_, s32 sz_)
	{
		if(!sz_)
			return;
		if(offset_ + sz_ > m_vertex_buffer.size)
		{
			LOG(LL_ERROR , "data size is too large , can not update");
			return;
		}
#if !defined(O2D_USE_DX_API)
		glBindBuffer(GL_ARRAY_BUFFER , m_vertex_buffer.handle);
		glBufferSubData(GL_ARRAY_BUFFER , offset_ , sz_ ,data_);
#else
		D3D11_MAPPED_SUBRESOURCE mapped;
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		context->Map(m_vertex_buffer.handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy((u8*)mapped.pData + offset_ , data_ , sz_);
		context->Unmap(m_vertex_buffer.handle, 0);
#endif
	}
	//--------------------------------------------------------
	void c_geometry_buffer::uploadib(u32 offset_, void* data_, s32 sz_)
	{
		if(!sz_)
			return;
		if(offset_ + sz_ > m_index_buffer.size)
		{
			LOG(LL_ERROR , "data size is too large , can not update");
			return;
		}
#if !defined(O2D_USE_DX_API)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , m_index_buffer.handle);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER , offset_ , sz_ ,data_);
#else
		D3D11_MAPPED_SUBRESOURCE mapped;
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		context->Map(m_index_buffer.handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy((u8*)mapped.pData + offset_ , data_ , sz_);
		context->Unmap(m_index_buffer.handle, 0);
#endif
	}
	//--------------------------------------------------------
	void c_geometry_buffer::assgin_attributes(c_pass* pass_)
	{
#if !defined(O2D_USE_DX_API)
		u8* offset = 0;
		u32 stride = m_vertex_buffer.stride;
		u32 usedstreams[MAX_VATTRIB_SIZE];
		memset(usedstreams , 0 , MAX_VATTRIB_SIZE*sizeof(u32));
		FOREACH(TYPEOF(c_vector<s_vertex_element>)::iterator , iterve , m_vertex_buffer.format)
		{
			u32 gtsize;
			GLint attrloc = pass_->get_attribute(iterve->semantic, iterve->index)->handle;
			GLint numele;
			GLenum gtype;
			switch(iterve->type)
			{
			case VET_FLOAT1:numele = 1;gtype = GL_FLOAT;gtsize = 1*4;break;
			case VET_FLOAT2:numele = 2;gtype = GL_FLOAT;gtsize = 2*4;break;
			case VET_FLOAT3:numele = 3;gtype = GL_FLOAT;gtsize = 3*4;break;
			case VET_FLOAT4:numele = 4;gtype = GL_FLOAT;gtsize = 4*4;break;
			case VET_INTEGER1:numele = 1;gtype = GL_INT;gtsize = 1*4;break;
			case VET_INTEGER2:numele = 2;gtype = GL_INT;gtsize = 2*4;break;
			case VET_INTEGER3:numele = 3;gtype = GL_INT;gtsize = 3*4;break;
			case VET_INTEGER4:numele = 4;gtype = GL_INT;gtsize = 4*4;break;
			default:assert(0);break;
			}
			if(attrloc != -1)
			{
				glEnableVertexAttribArray(attrloc);
				glBindBuffer(GL_ARRAY_BUFFER , m_vertex_buffer.handle);	
				switch(gtype)
				{
				case GL_INT:
				case GL_UNSIGNED_INT:
					assert(0);
					break;
				case GL_FLOAT:
					glVertexAttribPointer(attrloc, numele, gtype, GL_FALSE, stride, offset);
					break;
				default:assert(0);break;
				}
				usedstreams[attrloc] = 1;
			}
			offset += gtsize;
		}
		for(GLuint i = 0; i < MAX_VATTRIB_SIZE; ++ i)
		{
			if (usedstreams[i]==0)
				glDisableVertexAttribArray(i);
		}
#else
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		u32 offset = 0;
		context->IASetVertexBuffers(0, 1, &m_vertex_buffer.handle, &m_vertex_buffer.stride, &offset);
		if(m_index_buffer.handle)
			context->IASetIndexBuffer(m_index_buffer.handle, (m_index_buffer.format == IF_16UI)?DXGI_FORMAT_R16_UINT:DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(map(m_prim_type));	
		context->IASetInputLayout(pass_->get_layout());
#endif
	}
}