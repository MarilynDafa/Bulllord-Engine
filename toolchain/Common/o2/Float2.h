/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef FLOAT2_H_INCLUDED
#define FLOAT2_H_INCLUDED
#include "Prerequisites.h"
#include "Macro.h"
#include "Tools.h"
namespace o2d{
	class O2D_API c_float2
	{
	public:
		const static c_float2 OneVec;
		const static c_float2 ZeroVec;
	private:
		f32 m_x , m_y;
	public:
		inline c_float2();
		inline c_float2(f32 x_ , f32 y_);
		inline c_float2(const c_float2& rhs);
	public:
		inline bool operator<(const c_float2& rhs) const;
		inline bool operator>(const c_float2& rhs) const;
		inline c_float2& operator=(const c_float2& vec2_);
		inline bool operator==(const c_float2& vec2_) const;
		inline bool operator!=(const c_float2& vec2_) const;
		inline c_float2 operator+(f32 scale_) const;
		inline c_float2 operator+(const c_float2& vec2_) const;
		inline c_float2& operator+=(f32 scale_);
		inline c_float2& operator+=(const c_float2& vec2_);
		inline c_float2 operator-(f32 scale_) const;
		inline c_float2 operator-(const c_float2& vec2_) const;
		inline c_float2& operator-=(f32 scale_);
		inline c_float2& operator-=(const c_float2& vec2 );
		inline c_float2 operator*(f32 scale_) const;
		inline c_float2& operator*=(f32 scale_);
	public:
		inline f32 x() const;
		inline f32& x();
		inline f32 y() const;
		inline f32& y();
	};
	//--------------------------------------------------------
	c_float2::c_float2():m_x(0.0f) , m_y(0.0f){}
	//--------------------------------------------------------
	c_float2::c_float2(f32 x_ , f32 y_):m_x(x_) , m_y(y_){}
	//--------------------------------------------------------
	c_float2::c_float2(const c_float2& rhs_):m_x(rhs_.m_x) , m_y(rhs_.m_y){}
	//--------------------------------------------------------
	bool c_float2::operator<(const c_float2& rhs_) const
	{
		if(m_x < rhs_.x() && m_y <rhs_.y())
			return true;
		return false;
	}
	//--------------------------------------------------------
	bool c_float2::operator>(const c_float2& rhs_) const
	{
		if(m_x > rhs_.x() && m_y > rhs_.y())
			return true;
		return false;
	}
	//--------------------------------------------------------
	c_float2& c_float2::operator=(const c_float2& vec2_)
	{
		m_x = vec2_.m_x;
		m_y = vec2_.m_y;
		return *this;
	}
	//--------------------------------------------------------
	bool c_float2::operator==(const c_float2& vec2_) const
	{
		return ( m_x == vec2_.m_x && m_y == vec2_.m_y );
	}
	//--------------------------------------------------------
	bool c_float2::operator!=(const c_float2& vec2_) const
	{
		return ( m_x != vec2_.m_x || m_y != vec2_.m_y  );
	}
	//--------------------------------------------------------
	c_float2 c_float2::operator+(f32 scale_)const
	{
		c_float2 ret(m_x + scale_, m_y + scale_);
		return ret;
	}
	//--------------------------------------------------------
	c_float2 c_float2::operator+(const c_float2& vec2_) const
	{
		c_float2 ret(m_x + vec2_.m_x, m_y + vec2_.m_y);
		return ret;
	}
	//--------------------------------------------------------
	c_float2& c_float2::operator+=(f32 scale_)
	{
		m_x += scale_;
		m_y += scale_;
		return *this;
	}
	//--------------------------------------------------------
	c_float2& c_float2::operator+=(const c_float2& rkvector_)
	{
		m_x += rkvector_.m_x;
		m_y += rkvector_.m_y;
		return *this;
	}
	//--------------------------------------------------------
	c_float2 c_float2::operator-(f32 scale_) const
	{
		c_float2 ret(m_x - scale_,m_y - scale_);
		return ret;
	}
	//--------------------------------------------------------
	c_float2 c_float2::operator-(const c_float2& vec2_) const
	{
		c_float2 ret(m_x - vec2_.m_x,m_y - vec2_.m_y);
		return ret;
	}
	//--------------------------------------------------------
	c_float2& c_float2::operator-=(f32 scale_)
	{
		m_x -= scale_;
		m_y -= scale_;
		return *this;
	}
	//--------------------------------------------------------
	c_float2& c_float2::operator-=(const c_float2& vec2_)
	{
		m_x -= vec2_.m_x;
		m_y -= vec2_.m_y;
		return *this;
	}
	//--------------------------------------------------------
	c_float2 c_float2::operator*(f32 scale_) const
	{
		c_float2 ret(m_x * scale_, m_y * scale_);
		return ret;
	}
	//--------------------------------------------------------
	c_float2& c_float2::operator*=(f32 scale_)
	{
		m_x *= scale_;
		m_y *= scale_;
		return *this;
	}
	//--------------------------------------------------------
	f32 c_float2::x() const
	{
		return m_x;
	}
	//--------------------------------------------------------
	f32& c_float2::x()
	{
		return m_x;
	}
	//--------------------------------------------------------
	f32 c_float2::y() const
	{
		return m_y;
	}
	//--------------------------------------------------------
	f32& c_float2::y()
	{
		return m_y;
	}
}

#endif 