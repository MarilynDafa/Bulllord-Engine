/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "FileMgr.h"
#include "ResMgr.h"
#include "Skin.h"
#include "Codec.h"
namespace o2d{
	c_skin::c_skin(const utf16* filename_)
		:c_resource(filename_ , RT_UI) , 
		m_skin_loader(nullptr) , 
		m_id(-1)
	{}
	//--------------------------------------------------------
	c_skin::~c_skin(){}
	//--------------------------------------------------------
	c_texture* c_skin::get_skin_tex()
	{
		return (c_texture*)m_skin_loader->get_res();
	}
	//--------------------------------------------------------
	const c_rect& c_skin::get_texcoord(const c_string& name_) const
	{
		u32 id = hash(name_.c_str());
		AUTO(TYPEOF(c_map<u32 , c_rect>)::const_iterator , iter , m_texcoord_map.find(id));
		if(iter!=m_texcoord_map.end())
			return iter->second();
		else
		{
			return c_rect::Unit;
		}
	}
	//--------------------------------------------------------
	void c_skin::setup()
	{
		f32 w = ((c_texture*)m_skin_loader->get_res())->get_width();
		f32 h = ((c_texture*)m_skin_loader->get_res())->get_height();
		((c_texture*)m_skin_loader->get_res())->set_filter(FO_LINEAR , FO_LINEAR);
		c_resource_manager::get_singleton_ptr()->fetch_resource(m_skin_loader);
		((c_texture*)m_skin_loader->get_res())->upload(0 , 0 , 0 , w , h ,m_tex_source.get_pointer() , m_tex_source.get_size());
		m_tex_source.reset();
		c_resource::setup();
	}
	//--------------------------------------------------------
	bool c_skin::_load_impl()
	{
		Data_block data;
		std::vector<SpriteSheet> ssv;
		BMG_header_TT header;
		Texture_target tt;
		c_string xx(m_filename);
		UnCompressKTX((const char*)xx.to_utf8() ,data, ssv, header, tt);
		//c_stream out;
		//if(!c_file_manager::get_singleton_ptr()->open_file(m_filename , out))
		//	return false;
		/*
		while(!out.is_eof())
		{
			out.read_u16(sid);
			switch(sid)
			{
			case UCID_DESC:
				out.read_u32(w);
				out.read_u32(h);
				break;
			case UCID_FRAGMENT:
				{
					s32 ltx , lty , rbx , rby;
					out.read_s32(ltx);
					out.read_s32(lty);
					out.read_s32(rbx);
					out.read_s32(rby);
					u8 tmp[128];
					out.read_u8(tmp[0]);
					u32 i = 0;
					while(tmp[i])
					{
						++i;
						out.read_u8(tmp[i]);
					}
					m_texcoord_map.insert(hash(c_string(tmp).c_str()) , c_rect(ltx , lty , rbx , rby));
				}
				break;
			case UCID_TEXTURE:
				{
					u32 sz;
					out.read_u32(sz);
					m_tex_source.resize(sz);
					out.read_buf(m_tex_source.get_pointer() , m_tex_source.get_size());
					m_skin_loader = new c_tex_loader(false , w , h);	
				}
				break;
			default:
				assert(0);
				return false;
			}
		}*/
		for (int i = 0; i < ssv.size(); ++i)
		{
			m_texcoord_map.insert(hash(c_string((const utf8*)ssv[i].tag).c_str()), c_rect(ssv[i].ltx, ssv[i].lty, ssv[i].rbx, ssv[i].rby));
		}
		m_tex_source.resize(header.pixelWidth * header.pixelHeight * 4);
		m_tex_source.write_buf(data.Data[0], header.pixelWidth * header.pixelHeight * 4);
		m_skin_loader = new c_tex_loader(false, header.pixelWidth, header.pixelHeight);
		c_string tmp = m_filename;
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmp.tokenize(L"/" , tmp2);
		tmp2.back().tokenize(L"." , tmp3);
		m_id = hash(tmp3.front().c_str());
		return true;
	}
	//--------------------------------------------------------
	void c_skin::_unload_impl()
	{
		c_resource_manager::get_singleton_ptr()->discard_resource(m_skin_loader);
		delete m_skin_loader;
	}
}