/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef SYSTEM_H_INCLUDE
#define SYSTEM_H_INCLUDE
#include "Singleton.h"
#include "Str.h"
namespace o2d{
	class O2D_API c_system:public c_singleton<c_system>
	{
	private:
		c_string m_writable_path;
#if defined(O2D_PLATFORM_WINDOWS)
		LARGE_INTEGER m_freq;
		BOOL m_multicore;
#elif defined(O2D_PLATFORM_WPHONE)
		LARGE_INTEGER m_frequency;
		LARGE_INTEGER m_base;
#elif defined(O2D_PLATFORM_LINUX)
		timeval m_tv;
#elif defined(O2D_PLATFORM_OSX)
        f64 m_base;
        f64 m_resolution;
#elif defined(O2D_PLATFORM_IOS)
        f64 m_base;
        f64 m_resolution;
#endif
	public:
		c_system();
		virtual ~c_system();
	public:
		bool initialize();
		void shutdown();
		u32 time_elapsed();
        void set_writeable_path(const c_string& path_);
		const c_string& get_writable_path() const;
		void log(e_logging_level level_, const ansi* message_, ...);
		void openurl(const ansi* url_);
	};
}
#endif