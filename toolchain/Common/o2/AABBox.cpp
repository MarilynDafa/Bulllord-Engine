/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "AABBox.h"
namespace o2d{
	const c_box c_box::Infinite = c_box(c_float3(-MAX_FLOAT) , c_float3(MAX_FLOAT));
	const c_box c_box::Empty = c_box(c_float3(0.f) , c_float3(0.f));
}