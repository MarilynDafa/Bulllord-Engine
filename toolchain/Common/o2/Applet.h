/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef APPLET_H_INCLUDE
#define APPLET_H_INCLUDE
#include "UIMgr.h"
#include "System.h"
#include "FileMgr.h"
#include "ResMgr.h"
#include "Device.h"
#include "InputMgr.h"
#include "CFGFile.h"
#include "InputMgr.h"
namespace o2d{
	class O2D_API c_applet
	{
	protected:
		e_socket_type m_socket_type;
		u32 m_cur_time;
		u32 m_prev_time;
	public:
		c_applet(e_socket_type stype_ = ST_TCP);
		virtual ~c_applet();
    public:
        inline e_socket_type get_socket_type() const {return m_socket_type;}
	public:
		bool build(const c_string& cfg_);
		s32 exec();
    public:
		virtual void step(u32 dt_) = 0;
		virtual void draw(u32 dt_) = 0;
		virtual bool contruct() = 0;
		virtual void destruct() = 0;
	private:
		void _dump();
		void _update_timer(u32 dt_);
	};
}
#endif