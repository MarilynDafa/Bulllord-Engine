/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "ManualLoader.h"
namespace o2d{
	c_manual_loader::c_manual_loader():m_res(nullptr){}
	//--------------------------------------------------------
	c_manual_loader::~c_manual_loader(){}
	//--------------------------------------------------------
	c_resource* c_manual_loader::get_res()
	{
		return m_res;
	}
}