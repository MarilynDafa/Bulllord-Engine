/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED
#include "Float2.h"
namespace o2d{
	class O2D_API c_rect
	{
	public:
		const static c_rect Unit;
		const static c_rect Zero;
	private:
		c_float2 m_lt ,m_rb;
	public:
		inline c_rect();
		inline c_rect(f32 left_ , f32 top_ , f32 right_ , f32 bottom_);
		inline c_rect(const c_float2& lt_ , const c_float2& rb_);
		inline c_rect(const c_rect& r_);
	public:
		inline f32 width() const;
		inline f32 height() const;
		inline const c_float2& lt_pt() const;
		inline c_float2& lt_pt();
		inline const c_float2& rb_pt() const;
		inline c_float2& rb_pt();
	};
	//--------------------------------------------------------
	c_rect::c_rect(){}
	//--------------------------------------------------------
	c_rect::c_rect(f32 left_ , f32 top_ , f32 right_ , f32 bottom_):m_lt(c_float2(left_ , top_)) , m_rb(c_float2(right_ , bottom_)){}
	//--------------------------------------------------------
	c_rect::c_rect(const c_float2& lt_ , const c_float2& rb_):m_lt(lt_) , m_rb(rb_)
	{
		f32 ltx = m_lt.x();
		f32 rbx = m_rb.x();
		f32 lty = m_lt.y();
		f32 rby = m_rb.y();
		m_lt.x() = ltx<rbx?ltx:rbx;
		m_lt.y() = lty<rby?lty:rby;
		m_rb.x() = ltx>rbx?ltx:rbx;
		m_rb.y() = lty>rby?lty:rby;
	}
	//--------------------------------------------------------
	c_rect::c_rect(const c_rect& r_):m_lt(r_.m_lt) , m_rb(r_.m_rb){}
	//--------------------------------------------------------
	f32 c_rect::width() const{	return m_rb.x() - m_lt.x();}
	//--------------------------------------------------------
	f32 c_rect::height() const{	return m_rb.y() - m_lt.y();}
	//--------------------------------------------------------
	const c_float2& c_rect::lt_pt() const
	{
		return m_lt;
	}
	//--------------------------------------------------------
	c_float2& c_rect::lt_pt()
	{
		return m_lt;
	}
	//--------------------------------------------------------
	const c_float2& c_rect::rb_pt() const
	{
		return m_rb;
	}
	//--------------------------------------------------------
	c_float2& c_rect::rb_pt()
	{
		return m_rb;
	}
}
#endif