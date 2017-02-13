/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef FLOAT3_H_INCLUDED
#define FLOAT3_H_INCLUDED
#include "Prerequisites.h"
namespace o2d{
	class O2D_API c_float3
	{
	public:
		const static c_float3 OneVec;
		const static c_float3 ZeroVec;
	private:
		f32 m_x;
		f32 m_y;
		f32 m_z;
	public:
		inline c_float3();
		inline c_float3(f32 x_, f32 y_, f32 z_);
		inline explicit c_float3(f32 scaler_ );
		inline c_float3(const c_float3& m_);
	public:		
		inline bool operator<(const c_float3& rhs) const;
		inline bool operator>(const c_float3& rhs) const;
		inline c_float3& operator=(const c_float3& rhs_);
		inline bool operator==(const c_float3& rhs_) const;
		inline bool operator!=(const c_float3& rhs_) const;
		inline c_float3 operator+(f32 val_) const;
		inline c_float3 operator+(const c_float3& val_) const;
		inline c_float3& operator+=(f32 val_);
		inline c_float3& operator+=(const c_float3& val_);
		inline c_float3 operator-(f32 val_) const;
		inline c_float3 operator-(const c_float3& val_) const;
		inline c_float3& operator-=(f32 val_);
		inline c_float3& operator-=(const c_float3& val_);
		inline c_float3 operator*(f32 val_) const;
		inline c_float3& operator*=(f32 val_);
	public:
		inline f32 x() const;
		inline f32& x();
		inline f32 y() const;
		inline f32& y();
		inline f32 z() const;
		inline f32& z();		
	};
	//--------------------------------------------------------
	c_float3::c_float3():m_x(0.f) , m_y(0.f) , m_z(0.f){}
	//--------------------------------------------------------
	c_float3::c_float3(f32 x_, f32 y_, f32 z_):m_x(x_) , m_y(y_) , m_z(z_){}
	//--------------------------------------------------------
	c_float3::c_float3(f32 scaler_):m_x(scaler_) , m_y(scaler_) , m_z(scaler_){}
	//--------------------------------------------------------
	c_float3::c_float3(const c_float3& m_):m_x(m_.m_x), m_y(m_.m_y) , m_z(m_.m_z){}		
	//--------------------------------------------------------
	bool c_float3::operator<(const c_float3& rhs_) const
	{
		if(m_x < rhs_.x() && m_y <rhs_.y() && m_z <rhs_.z())
			return true;
		return false;
	}
	//--------------------------------------------------------
	bool c_float3::operator>(const c_float3& rhs_) const
	{
		if(m_x > rhs_.x() && m_y > rhs_.y() && m_z > rhs_.z())
			return true;
		return false;
	}
	//--------------------------------------------------------
	c_float3& c_float3::operator=(const c_float3& rhs_)
	{
		m_x = rhs_.m_x;
		m_y = rhs_.m_y;
		m_z = rhs_.m_z;
		return *this;
	}
	//--------------------------------------------------------
	bool c_float3::operator==(const c_float3& rhs_) const
	{		
		return ( m_x == rhs_.m_x && m_y == rhs_.m_y && m_z == rhs_.m_z );
	}
	//--------------------------------------------------------
	bool c_float3::operator!=(const c_float3& rhs_) const
	{		
		return ( m_x != rhs_.m_x || m_y != rhs_.m_y || m_z != rhs_.m_z );
	}
	//--------------------------------------------------------
	c_float3 c_float3::operator+(f32 val_) const
	{
		return c_float3(m_x + val_,m_y + val_,m_z + val_);
	}
	//--------------------------------------------------------
	c_float3 c_float3::operator+(const c_float3& val_) const
	{
		return c_float3(m_x + val_.m_x,m_y + val_.m_y,m_z + val_.m_z);
	}
	//--------------------------------------------------------
	c_float3& c_float3::operator+=(f32 val_)
	{
		m_x += val_;
		m_y += val_;
		m_z += val_;
		return *this;
	}
	//--------------------------------------------------------
	c_float3& c_float3::operator+=(const c_float3& val_)
	{
		m_x += val_.m_x;
		m_y += val_.m_y;
		m_z += val_.m_z;
		return *this;
	}
	//--------------------------------------------------------
	c_float3 c_float3::operator-(f32 val_) const
	{
		return c_float3(m_x - val_,m_y - val_,m_z - val_);
	}
	//--------------------------------------------------------
	c_float3 c_float3::operator-(const c_float3& val_) const
	{
		return c_float3(m_x - val_.m_x,m_y - val_.m_y,m_z - val_.m_z);
	}
	//--------------------------------------------------------
	c_float3& c_float3::operator-=(f32 val_)
	{
		m_x -= val_;
		m_y -= val_;
		m_z -= val_;
		return *this;
	}
	//--------------------------------------------------------
	c_float3& c_float3::operator-=(const c_float3& val_)
	{
		m_x -= val_.m_x;
		m_y -= val_.m_y;
		m_z -= val_.m_z;
		return *this;
	}
	//--------------------------------------------------------
	c_float3 c_float3::operator*(f32 val_) const
	{
		return c_float3(m_x * val_, m_y * val_, m_z * val_);
	}
	//--------------------------------------------------------
	c_float3& c_float3::operator*=(f32 val_)
	{
		m_x *= val_;
		m_y *= val_;
		m_z *= val_;
		return *this;
	}
	//--------------------------------------------------------
	f32 c_float3::x() const
	{
		return m_x;
	}
	//--------------------------------------------------------
	f32& c_float3::x()
	{
		return m_x;
	}
	//--------------------------------------------------------
	f32 c_float3::y() const
	{
		return m_y;
	}
	//--------------------------------------------------------
	f32& c_float3::y()
	{
		return m_y;
	}
	//--------------------------------------------------------
	f32 c_float3::z() const
	{
		return m_z;
	}
	//--------------------------------------------------------
	f32& c_float3::z()
	{
		return m_z;
	}
}
#endif 