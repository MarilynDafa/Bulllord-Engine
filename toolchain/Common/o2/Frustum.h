/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef FRUSTUM_H_INCLUDE
#define FRUSTUM_H_INCLUDE
#include "Plane.h"
namespace o2d{
	class c_frustum
	{
	private:
		c_plane m_planes[FP_NUM];
	public:
		inline c_frustum();
	public:
		inline const c_plane& plane(e_frustum_plane fp_) const;
		inline c_plane& plane(e_frustum_plane fp_);
	};
	//--------------------------------------------------------
	c_frustum::c_frustum(){}
	//--------------------------------------------------------
	const c_plane& c_frustum::plane(e_frustum_plane fp_) const
	{
		return m_planes[fp_];
	}
	//--------------------------------------------------------
	c_plane& c_frustum::plane(e_frustum_plane fp_)
	{
		return m_planes[fp_];
	}
}
#endif