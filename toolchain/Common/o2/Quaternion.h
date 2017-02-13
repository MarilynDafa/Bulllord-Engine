/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef QUATERNION_H_INCLUDE
#define QUATERNION_H_INCLUDE
#include "Prerequisites.h"
namespace o2d{
	class O2D_API c_quaternion
	{
	public:
		const static c_quaternion Identity;
	private:
		f32 m_w;
		f32 m_x;
		f32 m_y;
		f32 m_z;
	public:
		inline c_quaternion();
		inline c_quaternion(f32 x_, f32 y_, f32 z_, f32 w_);
		inline c_quaternion(const c_quaternion& q_);
	public:
		inline c_quaternion& operator=(const c_quaternion& rhs_);
		inline bool operator==(const c_quaternion& rhs_) const;
		inline bool operator!=(const c_quaternion& rhs_) const;
	public:
		inline f32 x() const;
		inline f32& x();
		inline f32 y() const;
		inline f32& y();
		inline f32 z() const;
		inline f32& z();
		inline f32 w() const;
		inline f32& w();
	};
	//--------------------------------------------------------
	c_quaternion::c_quaternion():m_w(1.f) , m_x(0.f) , m_y(0.f) , m_z(0.f){}
	//--------------------------------------------------------
	c_quaternion::c_quaternion(f32 x_, f32 y_, f32 z_, f32 w_):m_w(w_) , m_x(x_) , m_y(y_) , m_z(z_){}
	//--------------------------------------------------------
	c_quaternion::c_quaternion(const c_quaternion& q_):m_w(q_.m_w) , m_x(q_.m_x) , m_y(q_.m_y) , m_z(q_.m_z){}
	//--------------------------------------------------------
	c_quaternion& c_quaternion::operator=(const c_quaternion& rhs_)
	{
		m_w = rhs_.m_w;
		m_x = rhs_.m_x;
		m_y = rhs_.m_y;
		m_z = rhs_.m_z;
		return *this;
	}
	//--------------------------------------------------------
	bool c_quaternion::operator==(const c_quaternion& rhs_) const
	{
		return (rhs_.m_x == m_x) && (rhs_.m_y == m_y) && (rhs_.m_z == m_z) && (rhs_.m_w == m_w);
	}
	//--------------------------------------------------------
	bool c_quaternion::operator!=(const c_quaternion& rhs_) const
	{
		return (rhs_.m_x != m_x) || (rhs_.m_y != m_y) || (rhs_.m_z != m_z) || (rhs_.m_w != m_w);
	}
	//--------------------------------------------------------
	f32 c_quaternion::x() const
	{
		return m_x;
	}
	//--------------------------------------------------------
	f32& c_quaternion::x()
	{
		return m_x;
	}
	//--------------------------------------------------------
	f32 c_quaternion::y() const
	{
		return m_y;
	}
	//--------------------------------------------------------
	f32& c_quaternion::y()
	{
		return m_y;
	}
	//--------------------------------------------------------
	f32 c_quaternion::z() const
	{
		return m_z;
	}
	//--------------------------------------------------------
	f32& c_quaternion::z()
	{
		return m_z;
	}
	//--------------------------------------------------------
	f32 c_quaternion::w() const
	{
		return m_w;
	}
	//--------------------------------------------------------
	f32& c_quaternion::w()
	{
		return m_w;
	}
}
#endif