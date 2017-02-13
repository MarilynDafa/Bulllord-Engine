/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef FILESYSTEM_H_INCLUDE
#define FILESYSTEM_H_INCLUDE
#include "XmlReader.h"
#include "Singleton.h"
#include "Vector.h"
#include "Stream.h"
namespace o2d{
	class O2D_API c_file_manager:public c_singleton<c_file_manager>
	{
	private:
		c_stream m_xml_trunk;	
	public:
		c_file_manager();
		virtual ~c_file_manager();
	public:
		bool initialize();
		void shutdown();		
		bool open_file(const utf16* filename_, c_stream& out_, bool end0_ = false);
		bool exist_file(const utf16* filename_) const;
		bool obtain_xml_reader(const utf16* filename_, c_xml_reader& out_, bool full_ = false);
	};
}
#endif