/**************************************************************************************/
//   Copyright (C) 2009-2012	Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Algorithm.h"
#include "FileMgr.h"
#include "NameGen.h"
#include "Texture.h"
#include "System.h"
#include "Device.h"
#include "Mapping.h"
namespace o2d{
	c_texture::c_texture(const utf16* filename_)
		:c_resource(filename_ , RT_TEXTURE) , 
#if !defined(O2D_USE_DX_API)
		m_handle(0) ,
#else
#endif
		m_min_filter(FO_POINT) ,
		m_mag_filter(FO_POINT) , 
		m_internal_fmt(0x8058) , 
		m_num_mipmaps(1) , 
		m_width(0) , 
		m_height(0) , 
		m_from_disk(false)
	{
		m_dirty[0] = false;
		m_dirty[1] = false;
	}
	//--------------------------------------------------------
	c_texture::~c_texture(){}
	//--------------------------------------------------------
	void c_texture::set_filter(e_filter_options minfilter_, e_filter_options magfilter_)
	{
		if(m_min_filter!= minfilter_)
		{
			m_dirty[0] = true;
			m_min_filter = minfilter_;
		}
		if(m_mag_filter!= magfilter_)
		{
			m_dirty[1] = true;
			m_mag_filter = magfilter_;
		}
	}
	//--------------------------------------------------------
	void c_texture::upload(u32 xoffset_, u32 yoffset_, u32 lvl_, u32 w_, u32 h_, void* data_, s32 sz_)
	{
		if(!sz_)
			return;
#if !defined(O2D_USE_DX_API)
		glBindTexture(GL_TEXTURE_2D, m_handle);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexSubImage2D(GL_TEXTURE_2D, lvl_, xoffset_ , yoffset_ , w_ , h_ , GL_RGBA , GL_UNSIGNED_BYTE, data_);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();	
		D3D11_BOX destregion;
		destregion.left = xoffset_;
		destregion.right = xoffset_ + w_;
		destregion.top = yoffset_;
		destregion.bottom = yoffset_ + h_;
		destregion.front = 0;
		destregion.back = 1;
		context->UpdateSubresource(m_d3d_tex, 0, &destregion, data_, sz_/h_, 0);
#endif
	}
	//--------------------------------------------------------
	void c_texture::setup()
	{
#if !defined(O2D_USE_DX_API)
		glGenTextures(1, &m_handle);
		glBindTexture(GL_TEXTURE_2D, m_handle);
		GLenum magf , minf;
		map(m_mag_filter , m_min_filter , magf , minf);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minf);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magf);
#	if defined(O2D_USE_GL_API)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_num_mipmaps - 1);
#   else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL_APPLE, m_num_mipmaps - 1);
#	endif
		u32 faces = 1;
		u32 w = m_width;
		u32 h = m_height;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for(u32 face = 0 ; face < faces ; ++face)
		{
			for (u32 level = 0; level < m_num_mipmaps; ++ level)
			{
				GLsizei imagesz;
				switch(m_internal_fmt)
				{
				case 0x8058: 
					imagesz = w * h * 4;
					break;
				case 0x83F1:
					imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
					break;
				case 0x83F3: 
					imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
					break;
				case 0x8D64:
					imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
					break;
				case 0x8C92:
					imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
					break;
				case 0x87EE:
					imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
					break;
				case 0x9138:
					imagesz = ceil(w*0.25f) * ceil(h*0.25f) * 8.0f;
					break;
				default:assert(0);break;
				}			
				if(m_internal_fmt == 0x8058)
#	if defined(O2D_USE_GL_API)
					glTexImage2D(GL_TEXTURE_2D, level, m_internal_fmt,w, h, 0, GL_RGBA , GL_UNSIGNED_BYTE, m_data[level]);
#   else
                    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA,w, h, 0, GL_RGBA , GL_UNSIGNED_BYTE, m_data[level]);
#   endif
				else
					glCompressedTexImage2D(GL_TEXTURE_2D, level, m_internal_fmt,w, h, 0, imagesz, m_data[level]);
				w = MAX_VALUE(1U, w / 2);
				h = MAX_VALUE(1U, h / 2);
			}
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = m_width;
		desc.Height = m_height;
		desc.MipLevels = m_num_mipmaps;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		u32 faces = 1;
		u32 imagesz;
		for(u32 face = 0 ; face < faces ; ++face)
		{
			for(u32 level = 0; level < m_num_mipmaps; ++ level)
			{
				switch(m_internal_fmt)
				{
				case 0x8058: 
					imagesz = m_width * m_height * 4;
					desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;
				case 0x83F1:
					imagesz = ((m_width + 3) / 4) * ((m_height + 3) / 4) * 8;
					desc.Format = DXGI_FORMAT_BC1_UNORM;
					break;
				case 0x83F3: 
					imagesz = ((m_width + 3) / 4) * ((m_height + 3) / 4) * 16;
					desc.Format = DXGI_FORMAT_BC3_UNORM;
					break;
				default:
					assert(0);
					break;
				}
			}
		}
		D3D11_SUBRESOURCE_DATA subresdata;
		ZeroMemory(&subresdata , sizeof(subresdata));
		subresdata.pSysMem = m_data[0];
		switch(m_internal_fmt)
		{
		case 0x8058: 
			subresdata.SysMemPitch = m_width * 4;
			break;
		case 0x83F1:
			subresdata.SysMemPitch = ((m_width + 3) / 4) * 8;
			break;
		case 0x83F3: 
			subresdata.SysMemPitch = ((m_width + 3) / 4) * 16;
			break;
		default:
			assert(0);
			break;
		}
		subresdata.SysMemSlicePitch = imagesz;
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		HRESULT hr = device->CreateTexture2D(&desc, &subresdata, &m_d3d_tex);
		assert(!FAILED(hr));
		D3D11_SHADER_RESOURCE_VIEW_DESC rvdesc;
		ZeroMemory(&rvdesc, sizeof(rvdesc));
		rvdesc.Format = desc.Format;
		rvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		rvdesc.Texture2D.MostDetailedMip = 0;
		rvdesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(m_d3d_tex, &rvdesc, &m_d3d_srview);
		assert(!FAILED(hr));
#endif
		m_datablock.clear();
		m_data.clear();
		c_resource::setup();
	}
	//--------------------------------------------------------
	void c_texture::apply()
	{
#if !defined(O2D_USE_DX_API)
		glBindTexture(GL_TEXTURE_2D, m_handle);
		GLenum minf , magf;
		map(m_mag_filter , m_min_filter , magf , minf);
		if(m_dirty[0])
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minf);
			m_dirty[0] = false;
		}
		if(m_dirty[1])
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magf);
			m_dirty[1] = false;
		}
#else
		ID3D11DeviceContextN* context = c_device::get_singleton_ptr()->get_profile()->get_context();
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		D3D11_SAMPLER_DESC sdesc;
		ZeroMemory(&sdesc, sizeof(sdesc));
		D3D11_FILTER filter;
		map(m_mag_filter , m_min_filter , filter);
		sdesc.Filter = filter;
		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sdesc.MinLOD = 0;
		sdesc.MaxLOD = D3D11_FLOAT32_MAX;
		ID3D11SamplerState* sstate;
		HRESULT hr = device->CreateSamplerState(&sdesc, &sstate);
		assert(!FAILED(hr));
		context->PSSetSamplers(c_device::get_singleton_ptr()->get_renderstate_list().texture_unit, 1, &sstate);
		sstate->Release();
#endif
	}
	//--------------------------------------------------------
	bool c_texture::_load_impl()
	{
		m_from_disk = true;
		c_stream bs;
		if(!c_file_manager::get_singleton_ptr()->open_file(m_filename , bs))
			return false;
		s_ktx_header header;
		bs.read_buf(&header, sizeof(header));
		const u8 identifier[] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };
		if(memcmp(&header.identifier , identifier , sizeof(identifier)))
			return false;
		m_width = header.pixel_width;
		m_height = header.pixel_height;
		m_internal_fmt = header.gl_internal_format;
		m_num_mipmaps = header.number_of_mipmap_levels;
		if(header.pixel_height == 0)
		{
			assert(0);
			return false;
		}
		else
		{
			if(header.pixel_depth)
				return false;
		}
		bool valid;
		switch(header.bytes_of_key_value_data)
		{
		case 0x8888:
			valid = _decode_rgba(bs, m_datablock , m_data);
			break;
		case 0xACCC:
			valid = _decode_atitc(bs, m_datablock , m_data);
			break;
		case 0xCCCC:
			valid = _decode_pvrtc(bs, m_datablock , m_data);
			break;
		case 0xDCCC:
			valid = _decode_s3tc(bs, m_datablock , m_data);
			break;
		case 0xECCC:
			valid = _decode_etc(bs, m_datablock , m_data);
			break;
		default:assert(0);break;
		}
		if(!valid)
			return false;
		return true;
	}
	//--------------------------------------------------------
	void c_texture::_unload_impl()
	{
#if !defined(O2D_USE_DX_API)
		glDeleteTextures(1, &m_handle);
#else
		m_d3d_tex->Release();
		m_d3d_srview->Release();
#endif
	}
	//--------------------------------------------------------
	bool c_texture::_decode_rgba(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_)
	{
		c_vector<u32> base(m_num_mipmaps);
		s32 width = m_width;
		s32 height = m_height;
		for(u32 level = 0; level < m_num_mipmaps; ++ level)
		{
			u32 imagesz = width * height * 4;
			base[level] = datablock_.size();
			datablock_.resize(base[level] + imagesz);
			bs_.read_buf(&datablock_[base[level]], imagesz);
			width = MAX_VALUE(width / 2, 1);
			height = MAX_VALUE(height / 2, 1);
		}
		for(u32 i = 0; i < base.size(); ++ i)
			data_.push_back(&datablock_[base[i]]);
		return true;
	}
	//--------------------------------------------------------
	bool c_texture::_decode_atitc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_)
	{	
		c_vector<u32> base(m_num_mipmaps);
		s32 width = m_width;
		s32 height = m_height;
		for(u32 level = 0; level < m_num_mipmaps; ++ level)
		{
			u32 imagesz = ((width + 3) / 4) * ((height + 3) / 4) * ((0x8C92 == m_internal_fmt)?8:16);
			base[level] = datablock_.size();
			datablock_.resize(base[level] + imagesz);
			bs_.read_buf(&datablock_[base[level]], imagesz);
			width = MAX_VALUE(width / 2, 1);
			height = MAX_VALUE(height / 2, 1);
		}
		for(u32 i = 0; i < base.size(); ++ i)
			data_.push_back(&datablock_[base[i]]);
		return true;
	}
	//--------------------------------------------------------
	bool c_texture::_decode_s3tc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_)
	{		
		c_vector<u32> base(m_num_mipmaps);
		s32 width = m_width;
		s32 height = m_height;
		for(u32 level = 0; level < m_num_mipmaps; ++ level)
		{
			u32 imagesz = ((width + 3) / 4) * ((height + 3) / 4) * ((0x83F1 == m_internal_fmt)?8:16);
			base[level] = datablock_.size();
			datablock_.resize(base[level] + imagesz);
			bs_.read_buf(&datablock_[base[level]], imagesz);
			width = MAX_VALUE(width / 2, 1);
			height = MAX_VALUE(height / 2, 1);
		}
		for(u32 i = 0; i < base.size(); ++ i)
			data_.push_back(&datablock_[base[i]]);
		return true;
	}
	//--------------------------------------------------------
	bool c_texture::_decode_etc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_)
	{
		c_vector<u32> base(m_num_mipmaps);
		s32 width = m_width;
		s32 height = m_height;
		for(u32 level = 0; level < m_num_mipmaps; ++ level)
		{
			u32 imagesz = ((width + 3) / 4) * ((height + 3) / 4) * 8;
			base[level] = datablock_.size();
			datablock_.resize(base[level] + imagesz);
			bs_.read_buf(&datablock_[base[level]], imagesz);
			width = MAX_VALUE(width / 2, 1);
			height = MAX_VALUE(height / 2, 1);
		}
		for(u32 i = 0; i < base.size(); ++ i)
			data_.push_back(&datablock_[base[i]]);
		return true;
	}
	//--------------------------------------------------------
	bool c_texture::_decode_pvrtc(c_stream& bs_, c_vector<u8>& datablock_, c_vector<void*>& data_)
	{
		c_vector<u32> base(m_num_mipmaps);
		s32 width = m_width;
		s32 height = m_height;
		for(u32 level = 0; level < m_num_mipmaps; ++ level)
		{
			u32 imagesz;
			imagesz = ceil(width*0.25f) * ceil(height*0.25f) * 8.f;
			base[level] = datablock_.size();
			datablock_.resize(base[level] + imagesz);
			bs_.read_buf(&datablock_[base[level]], imagesz);
			width = MAX_VALUE(width / 2, 1);
			height = MAX_VALUE(height / 2, 1);
		}
		for(u32 i = 0; i < base.size(); ++ i)
			data_.push_back(&datablock_[base[i]]);
		return true;
	}
	//--------------------------------------------------------
	c_tex_loader::c_tex_loader(bool usert_, u32 w_, u32 h_ /*= 0*/)
		:m_usert(usert_)
	{
		static c_name_generator gen(L"manual_tex");
		m_res = new c_texture(gen.generate().c_str());
		((c_texture*)m_res)->m_width = w_;
		((c_texture*)m_res)->m_height = h_;
		((c_texture*)m_res)->m_internal_fmt = 0x8058;
		((c_texture*)m_res)->m_num_mipmaps = 1;
		((c_texture*)m_res)->m_from_disk = false;
	}
	//--------------------------------------------------------
	c_tex_loader::~c_tex_loader(){}
	//--------------------------------------------------------
	bool c_tex_loader::load()
	{
		u32 w = ((c_texture*)m_res)->m_width;
		u32 h = ((c_texture*)m_res)->m_height;
#if !defined(O2D_USE_DX_API)
		glGenTextures(1, &((c_texture*)m_res)->m_handle);
		glBindTexture(GL_TEXTURE_2D, ((c_texture*)m_res)->m_handle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#	if defined(O2D_USE_GL_API)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, ((c_texture*)m_res)->m_internal_fmt, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#   else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL_APPLE, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#	endif
#else
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_usert?DXGI_FORMAT_R8G8B8A8_UNORM:DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = m_usert?D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE:D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ID3D11DeviceN* device = c_device::get_singleton_ptr()->get_profile()->get_device();
		HRESULT hr = device->CreateTexture2D(&desc, NULL, &((c_texture*)m_res)->m_d3d_tex);
		assert(!FAILED(hr));
		D3D11_SHADER_RESOURCE_VIEW_DESC rvdesc;
		ZeroMemory(&rvdesc, sizeof(rvdesc));
		rvdesc.Format = m_usert?DXGI_FORMAT_R8G8B8A8_UNORM:DXGI_FORMAT_R8G8B8A8_UNORM;
		rvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		rvdesc.Texture2D.MostDetailedMip = 0;
		rvdesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(((c_texture*)m_res)->m_d3d_tex, &rvdesc, &((c_texture*)m_res)->m_d3d_srview);
		assert(!FAILED(hr));
#endif
		return true;
	}
	//--------------------------------------------------------
	void c_tex_loader::unload()
	{
#if !defined(O2D_USE_DX_API)
		glDeleteTextures(1, &((c_texture*)m_res)->m_handle);
#else
		((c_texture*)m_res)->m_d3d_tex->Release();
		((c_texture*)m_res)->m_d3d_srview->Release();
#endif
		delete m_res;
		m_res = nullptr;
	}
}
