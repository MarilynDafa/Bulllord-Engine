/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef COLOR_H_INCLUDE
#define COLOR_H_INCLUDE
#include "Prerequisites.h"
namespace o2d{
	class O2D_API c_color
	{
	private:
		f32 m_rgba[4];
	public:
		static c_color Transparent;
	public:
		c_color();
		c_color(u32 r_, u32 g_, u32 b_, u32 a_);
		c_color(u32 rgba_);
		c_color(f32 r_, f32 g_, f32 b_, f32 a_);
	public:
		bool operator==(const c_color& clr_);
		bool operator!=(const c_color& clr_);
		c_color operator-(const c_color& clr_) const;
		c_color operator+(const c_color& clr_) const;
		c_color operator/(f32 val_) const;
		c_color operator*(f32 val_) const;
	public:
		inline f32& r() {return m_rgba[0];}
		inline f32 r() const {return m_rgba[0];}
		inline f32& g() {return m_rgba[1];}
		inline f32 g() const {return m_rgba[1];}
		inline f32& b() {return m_rgba[2];}
		inline f32 b() const {return m_rgba[2];}
		inline f32& a() {return m_rgba[3];}
		inline f32 a() const {return m_rgba[3];}
		inline f32* ptr() {return m_rgba;}
		inline const f32* ptr() const {return m_rgba;}
	public:
		u32 rgba() const;
	};
}
#endif