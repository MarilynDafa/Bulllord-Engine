/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "FileMgr.h"
#include "System.h"
#include "Macro.h"
#include "Stream.h"
#include "Tools.h"
namespace o2d{
	c_file_manager::c_file_manager(){}
	//--------------------------------------------------------
	c_file_manager::~c_file_manager(){}
	//--------------------------------------------------------
	bool c_file_manager::initialize()
	{
		LOG(LL_INFORMATION , "file manager initialize sucessfully");
		return true;
	}
	//--------------------------------------------------------
	void c_file_manager::shutdown()
	{
		LOG(LL_INFORMATION , "file manager shutdown");
	}
	//--------------------------------------------------------
	bool c_file_manager::open_file(const utf16* filename_, c_stream& out_, bool end0_ /*= 0*/)
	{	
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		FILE* file = _wfopen(filename_ , L"rb");
#else
		FILE* file = fopen((const ansi*)c_string(filename_).to_utf8() , "rb");
#endif
		if(!file)
			return false;
		fseek(file, 0, SEEK_END);
		u32 size = (u32)ftell(file);
		fseek(file, 0, SEEK_SET);
		if(end0_)
		{
			out_.reset(size +1);
			fread(out_.get_pointer(), 1, size, file);
			out_.set_zero_end();
		}
		else
		{
			out_.reset(size);
			fread(out_.get_pointer(), 1, size, file);
		}
		fclose(file);
		return true;
	}
	//--------------------------------------------------------
	bool c_file_manager::exist_file(const utf16* filename_) const
	{
#if defined(O2D_PLATFORM_WINDOWS) || defined(O2D_PLATFORM_WPHONE)
		return (_waccess(filename_ ,  0) != -1);
#else
		return (access((const ansi*)c_string(filename_).to_utf8() , 0) != -1);
#endif
	}
	//--------------------------------------------------------
	bool c_file_manager::obtain_xml_reader( const utf16* filename_, c_xml_reader& out_, bool full_ /*= false*/ )
	{
		if(!open_file(filename_ , m_xml_trunk , true))
			return false;
		out_.parse((u8*)m_xml_trunk.get_pointer() , full_);
		return true;
	}
}