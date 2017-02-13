/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MANUALLOADER_H_INCLUDE
#define MANUALLOADER_H_INCLUDE
#include "ManagedRes.h"
namespace o2d{
	class O2D_API c_manual_loader
	{
	protected:
		c_resource* m_res;
	public:
		c_manual_loader();
		~c_manual_loader();
	public:
		c_resource* get_res();
		virtual bool load() = 0;
		virtual void unload() = 0;
	};
}
#endif