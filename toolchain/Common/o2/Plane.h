/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef PLANE_H_INCLUDE
#define PLANE_H_INCLUDE
#include "Float3.h"
namespace o2d{
	class O2D_API c_plane
	{
	public:
		const static c_plane ZeroPlane;
	private:
		c_float3 m_normal;
		f32 m_d;
	public:
		inline c_plane();
		inline c_plane(f32 nx_, f32 ny_, f32 nz_, f32 d_);
	public:
		inline c_float3 normal() const;
		inline c_float3& normal();
		inline f32 d() const;
		inline f32& d();
	};
	c_plane::c_plane():m_normal(c_float3::ZeroVec) , m_d(0.f){}
	//--------------------------------------------------------
	c_plane::c_plane(f32 nx_, f32 ny_, f32 nz_, f32 d_):m_normal(nx_ , ny_ , nz_) , m_d(d_){}
	//--------------------------------------------------------
	c_float3 c_plane::normal() const
	{
		return m_normal;
	}
	//--------------------------------------------------------
	c_float3& c_plane::normal()
	{
		return m_normal;
	}
	//--------------------------------------------------------
	f32 c_plane::d() const
	{
		return m_d;
	}
	//--------------------------------------------------------
	f32& c_plane::d()
	{
		return m_d;
	}
}
#endif