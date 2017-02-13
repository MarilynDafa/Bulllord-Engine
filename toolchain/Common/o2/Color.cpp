/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Tools.h"
#include "Color.h"
namespace o2d{
	c_color c_color::Transparent = c_color(0.f , 0.f , 0.f , 0.f);
	//--------------------------------------------------------
	c_color::c_color()
	{
		memset(m_rgba , 0 , 4*sizeof(f32));
	}
	//--------------------------------------------------------
	c_color::c_color(u32 r_, u32 g_, u32 b_, u32 a_)
	{
		const f32 inv = 1.0f / 255.0f;
		m_rgba[0] = r_ * inv;
		m_rgba[1] = g_ * inv;
		m_rgba[2] = b_ * inv;
		m_rgba[3] = a_ * inv;
	}
	//--------------------------------------------------------
	c_color::c_color(u32 rgba_)
	{
		const f32 inv = 1.0f / 255.0f;
		m_rgba[3] = inv * (f32)((u8)(rgba_ >> 24));
		m_rgba[2] = inv * (f32)((u8)(rgba_ >> 16));
		m_rgba[1] = inv * (f32)((u8)(rgba_ >>  8));
		m_rgba[0] = inv * (f32)((u8)(rgba_ >>  0));
	}
	//--------------------------------------------------------
	c_color::c_color(f32 r_, f32 g_, f32 b_, f32 a_)
	{
		m_rgba[0] = r_;
		m_rgba[1] = g_;
		m_rgba[2] = b_;
		m_rgba[3] = a_;
	}
	//--------------------------------------------------------
	bool c_color::operator==(const c_color& clr_)
	{
		return m_rgba[0] == clr_.m_rgba[0] && m_rgba[1] == clr_.m_rgba[1]&&
			m_rgba[2] == clr_.m_rgba[2] && m_rgba[3] == clr_.m_rgba[3];
	}
	//--------------------------------------------------------
	bool c_color::operator!=(const c_color& clr_)
	{
		return m_rgba[0] != clr_.m_rgba[0] || m_rgba[1] != clr_.m_rgba[1]||
			m_rgba[2] != clr_.m_rgba[2] || m_rgba[3] != clr_.m_rgba[3];
	}
	//--------------------------------------------------------
	c_color c_color::operator-(const c_color& clr_) const
	{
		c_color ret(m_rgba[0] - clr_.r() , m_rgba[1] - clr_.g() , m_rgba[2] - clr_.b() , m_rgba[3] - clr_.a());
		return ret;
	}
	//--------------------------------------------------------
	c_color c_color::operator+(const c_color& clr_) const
	{
		c_color ret(m_rgba[0] + clr_.r() , m_rgba[1] + clr_.g() , m_rgba[2] + clr_.b() , m_rgba[3] + clr_.a());
		return ret;
	}
	//--------------------------------------------------------
	c_color c_color::operator/(f32 val_) const
	{
		f32 inv = 1.f/val_;
		c_color ret(m_rgba[0]*inv , m_rgba[1]*inv , m_rgba[2]*inv , m_rgba[3]*inv);
		return ret;
	}
	//--------------------------------------------------------
	c_color c_color::operator*(f32 val_) const
	{
		c_color ret(m_rgba[0]*val_ , m_rgba[1]*val_ , m_rgba[2]*val_ , m_rgba[3]*val_);
		return ret;
	}
	//--------------------------------------------------------
	u32 c_color::rgba() const
	{
		u8 r = (u8)(m_rgba[0] * 255 + 0.5f);
		u8 g = (u8)(m_rgba[1] * 255 + 0.5f);
		u8 b = (u8)(m_rgba[2] * 255 + 0.5f);
		u8 a = (u8)(m_rgba[3] * 255 + 0.5f);
		return (a << 24)+(b << 16)+(g << 8)+(r << 0);
	}
}