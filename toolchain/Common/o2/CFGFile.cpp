/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "CFGFile.h"
#include "System.h"
namespace o2d{
	c_cfg_file::c_cfg_file(){}
	//--------------------------------------------------------
	c_cfg_file::c_cfg_file(const utf16* filename_)
	{
		if(!filename_)
			return;
		if(utf16_cmp(get_ext(filename_) , L"cfg"))
			return;
#ifdef O2D_PLATFORM_WINDOWS
		FILE* cfg = _wfopen(filename_ , L"rb");
#else
		FILE* cfg = fopen((const ansi*)c_string(filename_).to_utf8() , "rb");
#endif
		if(!cfg)
			return;
		fseek(cfg , 0 , SEEK_END);
		u32 sz = (u32)ftell(cfg);
		fseek(cfg , 0 , SEEK_SET);
		utf8* buf = (utf8*)malloc(sz*sizeof(utf8)+1);
		fread(buf , sizeof(utf8) , sz , cfg);
		buf[sz] = 0;
		_prase(buf);
		free(buf);
	}
	//--------------------------------------------------------
	c_cfg_file::~c_cfg_file()
	{
		m_sc_map.clear();
	}
	//--------------------------------------------------------
	c_string c_cfg_file::get_profile_string(const c_string& section_, const c_string& key_, const c_string& defstr_) const
	{
		AUTO(TYPEOF(c_map<c_string , c_map<c_string , c_string> >)::const_iterator , iter ,m_sc_map.find(section_));
		if(iter == m_sc_map.end())
			return defstr_;
		AUTO(TYPEOF(c_map<c_string , c_string>)::const_iterator , iter1 ,iter->second().find(key_));
		if(iter1 == iter->second().end())
			return defstr_;
		return iter1->second();
	}
	//--------------------------------------------------------
	void c_cfg_file::set_profile_string(const c_string& section_, const c_string& key_, const c_string& value_)
	{
		AUTO(TYPEOF(c_map<c_string , c_map<c_string , c_string> >)::iterator , iters , m_sc_map.find(section_));
		if(iters == m_sc_map.end())
		{
			c_map<c_string , c_string> tmp;
			tmp.insert(key_ , value_);
			m_sc_map.insert(section_ , tmp);
		}
		else
		{
			AUTO(TYPEOF(c_map<c_string , c_string>)::iterator , iterk , iters->second().find(key_));
			if(iterk == iters->second().end())
			{
				iters->second().insert(key_, value_);
			}
			else
			{
				iterk->second() = value_;
			}
		}
	}
	//--------------------------------------------------------
	void c_cfg_file::write(const utf16* filename_)
	{
#ifdef O2D_PLATFORM_WINDOWS
		FILE* fp = _wfopen(filename_ , L"wb");
#else
		FILE* fp = fopen((const ansi*)c_string(filename_).to_utf8() , "wb");
#endif
		if(!fp)
			return;
		FOREACH(TYPEOF(c_map<c_string , c_map<c_string , c_string> >)::iterator , iter , m_sc_map)
		{
			const ansi* sec = (const ansi*)iter->first().to_utf8();
			fwrite("[" , sizeof(ansi) , 1 , fp);
			fwrite(sec , sizeof(ansi) , iter->first().utf8_size() , fp);
			fwrite("]" , sizeof(ansi) , 1 , fp);
			fwrite("\n" , sizeof(ansi) , 1 , fp);
			FOREACH(TYPEOF(c_map<c_string , c_string>)::iterator , iters , iter->second())
			{
				const ansi* key = (const ansi*)iters->first().to_utf8();
				fwrite(key , sizeof(ansi) , iters->first().utf8_size() , fp);
				fwrite("=" , sizeof(ansi) , 1 , fp);
				const ansi* val = (const ansi*)iters->second().to_utf8();
				fwrite(val , sizeof(ansi) , iters->second().utf8_size() , fp);
				fwrite("\n" , sizeof(ansi) , 1 , fp);
			}
		}
		fclose(fp);
	}
	//--------------------------------------------------------
	void c_cfg_file::_prase(const utf8* buf_)
	{
		c_string all(buf_);
		c_vector<c_string> temp1;
		all.tokenize(L"[" , temp1);
		FOREACH(TYPEOF(c_vector<c_string>)::iterator , iter1 , temp1)
		{
			c_vector<c_string> temp2;
			iter1->tokenize(L"]" , temp2);
			c_string section = temp2[0];
			c_vector<c_string> temp3;
			temp2[1].tokenize(L"\n" , temp3);
			c_map<c_string , c_string> item;
			FOREACH(TYPEOF(c_vector<c_string>)::iterator , iter3 , temp3)
			{
				if(iter3->get_length()<3)
					continue;
				c_vector<c_string> temp4;
				iter3->tokenize(L"=" , temp4);
				c_string key = temp4[0];
				key.trim();
				c_string value = temp4[1];
				value.trim();
				item.insert(key , value);
			}
			m_sc_map.insert(section , item);
		}
	}
}

