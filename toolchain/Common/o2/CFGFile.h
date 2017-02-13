/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef CFGFILE_H_INCLUDED
#define CFGFILE_H_INCLUDED
#include "Tools.h"
#include "Stream.h"			
#include "Str.h" 
#include "Map.h"
namespace o2d{
	class O2D_API c_cfg_file
	{
	private:
		c_map<c_string , c_map<c_string , c_string> > m_sc_map;
	public:
		c_cfg_file();
		c_cfg_file(const utf16* filename_);
		~c_cfg_file();
	public:
		c_string get_profile_string(const c_string& section_, const c_string& key_, const c_string& defstr_) const;
		void set_profile_string(const c_string& section_, const c_string& key_, const c_string& value_);
		void write(const utf16* filename_);
	private:
		void _prase(const utf8* buf_);
	};
}

#endif