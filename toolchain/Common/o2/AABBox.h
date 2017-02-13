/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef AABBOX_H_INCLUDE
#define AABBOX_H_INCLUDE
#include "Float3.h"
namespace o2d{
	class O2D_API c_box
	{
	public:
		const static c_box Infinite;
		const static c_box Empty;
	private:
		c_float3 m_minpt;
		c_float3 m_maxpt;
	public:
		inline c_box();
		inline c_box(const c_float3& minpt_ , const c_float3& maxpt_);
	public:
		inline bool operator==(const c_box& box_) const;
		inline bool operator!=(const c_box& box_) const;
	public:
		inline c_float3& minpt();
		inline const c_float3& minpt() const;
		inline c_float3& maxpt();
		inline const c_float3& maxpt() const;
		inline c_float3 corner_point(u32 index_) const;
	};
	//--------------------------------------------------------
	c_box::c_box(){}
	//--------------------------------------------------------
	c_box::c_box(const c_float3& minpt_ , const c_float3& maxpt_):m_minpt(minpt_) , m_maxpt(maxpt_){}
	//--------------------------------------------------------
	bool c_box::operator==(const c_box& box_) const
	{
		return ( m_minpt == box_.m_minpt && m_maxpt == box_.m_maxpt );
	}
	//--------------------------------------------------------
	bool c_box::operator!=(const c_box& box_) const
	{
		return ( m_minpt != box_.m_minpt || m_maxpt != box_.m_maxpt );
	}
	//--------------------------------------------------------
	c_float3& c_box::minpt(){ return m_minpt;}
	//--------------------------------------------------------
	const c_float3& c_box::minpt() const{ return m_minpt;}
	//--------------------------------------------------------
	c_float3& c_box::maxpt(){ return m_maxpt;}
	//--------------------------------------------------------
	const c_float3& c_box::maxpt() const{ return m_maxpt;}
	//--------------------------------------------------------
	c_float3 c_box::corner_point(u32 index_) const
	{
		switch (index_)
		{
		case 0: return m_minpt;
		case 1: return c_float3(m_minpt.x(), m_maxpt.y(), m_minpt.z());
		case 2: return c_float3(m_maxpt.x(), m_maxpt.y(), m_minpt.z());
		case 3: return c_float3(m_maxpt.x(), m_minpt.y(), m_minpt.z());
		case 4: return m_maxpt;
		case 5: return c_float3(m_minpt.x(), m_maxpt.y(), m_maxpt.z());
		case 6: return c_float3(m_minpt.x(), m_minpt.y(), m_maxpt.z());
		default: return c_float3(m_maxpt.x(), m_minpt.y(), m_maxpt.z());
		}
	}
}
#endif