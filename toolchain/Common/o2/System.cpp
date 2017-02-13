/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "System.h"
namespace o2d{
	c_system::c_system()
#if defined(O2D_PLATFORM_WINDOWS)
        :m_multicore(TRUE)
#elif defined(O2D_PLATFORM_OSX)
        :m_base(0.0) ,
        m_resolution(0.0)
#elif defined(O2D_PLATFORM_IOS)
        :m_base(0.0) ,
        m_resolution(0.0)
#endif
	{}
	//--------------------------------------------------------
	c_system::~c_system(){}
	//--------------------------------------------------------
	bool c_system::initialize()
	{
#if defined(O2D_PLATFORM_WINDOWS)
		SYSTEM_INFO sysinfo;
		::GetSystemInfo(&sysinfo);
		m_multicore = (sysinfo.dwNumberOfProcessors > 1);
		::QueryPerformanceFrequency(&m_freq);
		utf16 buf[FILENAME_MAX_LENGTH];
		::GetModuleFileNameW(0, buf, FILENAME_MAX_LENGTH + 1);
		m_writable_path = buf;
		m_writable_path = m_writable_path.sub_str(0 , m_writable_path.rfind(L'\\') + 1);
		c_string tmp;
		for(u32 i = 0 ; i < m_writable_path.get_length() ; ++i)
		{
			if(m_writable_path[i] == L'\\')
			{
				tmp.append(L'/');
				++i;
			}
			else
				tmp.append(m_writable_path[i]);
		}
		m_writable_path = tmp;
#elif defined(O2D_PLATFORM_WPHONE)
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_base);
		StorageFolder^ localfolder = ApplicationData::Current->LocalFolder;
		Platform::String^ folderpath = localfolder->Path + L'/';
		m_writable_path = folderpath->Data();
#elif defined(O2D_PLATFORM_LINUX)
#elif defined(O2D_PLATFORM_OSX)
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        m_resolution = (f64)info.numer / (info.denom * 1.0e9);
        m_base = mach_absolute_time();
#elif defined(O2D_PLATFORM_IOS)
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        m_resolution = (f64)info.numer / (info.denom * 1.0e9);
        m_base = mach_absolute_time();
#else
#endif
		log(LL_SYSTEM , "Overdrive2D Engine Console");
		log(LL_INFORMATION , "overdrive console initialize sucessfully");
		return true;
	}
	//--------------------------------------------------------
	void c_system::shutdown()
	{
		log(LL_INFORMATION , "overdrive console shutdown");
	}
	//--------------------------------------------------------
	u32 c_system::time_elapsed()
	{
#if defined(O2D_PLATFORM_WINDOWS)
		DWORD_PTR affinitymask=0;
		if(m_multicore)
			affinitymask = ::SetThreadAffinityMask(GetCurrentThread(), 1);
		LARGE_INTEGER ntime;
		BOOL queried = ::QueryPerformanceCounter(&ntime);
		if(m_multicore)
			::SetThreadAffinityMask(GetCurrentThread(), affinitymask);
		if(queried)
			return u32((ntime.QuadPart) * 1000 / m_freq.QuadPart);
		else 
			return 0;
#elif defined(O2D_PLATFORM_WPHONE)
		LARGE_INTEGER curtime;
		QueryPerformanceCounter(&curtime);
		return (f64)(curtime.QuadPart - m_base.QuadPart)/(f64)(m_frequency.QuadPart);
#elif defined(O2D_PLATFORM_LINUX)
		gettimeofday(&m_tv, 0);
		return (u32)(m_tv.tv_sec * 1000) + (m_tv.tv_usec / 1000);
#elif defined(O2D_PLATFORM_OSX) || defined(O2D_PLATFORM_IOS)
        return 1000*((mach_absolute_time() - m_base) * m_resolution);
#else
        return 0;
#endif
	}
	//--------------------------------------------------------
    void c_system::set_writeable_path(const c_string& path_)
    {
        m_writable_path = path_;
    }
	//--------------------------------------------------------
	const c_string& c_system::get_writable_path() const
	{
		return m_writable_path;
	}
	//--------------------------------------------------------
	void c_system::log(e_logging_level level_, const ansi* message_, ...)
	{
		va_list args;
		va_start(args, message_);
		ansi temptextbuf[2048];
		vsnprintf(temptextbuf , (u32)-1 , message_, args);
		va_end(args);
		c_string context;
		switch(level_)
		{
		case LL_ERROR:
			context<<L"ERROR - "<<(const utf8*)temptextbuf<<L"\n";
			break;
		case LL_WARNING:				
			context<<L"WARNNING - "<<(const utf8*)temptextbuf<<L"\n";
			break;
		case LL_INFORMATION:				
			context<<L"Information - "<<(const utf8*)temptextbuf<<L"\n";
			break;
		default:
			context<<(const utf8*)temptextbuf<<L"\n";
			break;
		}
        printf("%s" , ((const ansi*)context.to_utf8()));
        fflush(stdout);
	}
	//--------------------------------------------------------
	void c_system::openurl(const ansi* url_)
	{
#if defined(O2D_PLATFORM_WINDOWS)
		ShellExecuteA(NULL, "open", url_, NULL,  NULL, SW_SHOWNORMAL);
#elif defined(O2D_PLATFORM_WPHONE)
#elif defined(O2D_PLATFORM_LINUX)
#elif defined(O2D_PLATFORM_OSX)
#elif defined(O2D_PLATFORM_IOS)
#elif defined(O2D_PLATFORM_ANDROID)
#else
		log(LL_ERROR , "the openurl method is not impl");
#endif
	}
}
