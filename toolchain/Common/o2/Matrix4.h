/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MATRIX4_H_INCLUDE
#define MATRIX4_H_INCLUDE
#include "Prerequisites.h"
namespace o2d{
	class O2D_API c_matrix4
	{
	public:
		const static c_matrix4 Identity;
	private:
		union
		{
			f32 m_data[4][4];
			f32 m_ele[16];
		};
	public:
		inline c_matrix4();
		inline c_matrix4(const c_matrix4& mx_);
		inline c_matrix4(f32 m00_, f32 m01_, f32 m02_, f32 m03_, 
			f32 m10_, f32 m11_, f32 m12_, f32 m13_, 
			f32 m20_, f32 m21_, f32 m22_, f32 m23_,
			f32 m30_, f32 m31_, f32 m32_, f32 m33_);
	public:
		inline c_matrix4& operator=(const c_matrix4& rhs_);
		inline bool operator==(const c_matrix4& rhs_) const;
		inline bool operator!=(const c_matrix4& rhs_) const;
		inline f32* operator [] (u32 col_);
		inline const f32* operator [] (u32 col_) const;
	public:
		inline const f32* glmat() const;
	};
	//--------------------------------------------------------
	c_matrix4::c_matrix4(){}
	//--------------------------------------------------------
	c_matrix4::c_matrix4(const c_matrix4& mx_)
	{
		for(u32 i = 0 ; i < 16 ; ++i)
			m_ele[i] = mx_.m_ele[i];
	}
	//--------------------------------------------------------
	c_matrix4::c_matrix4(f32 m00_, f32 m01_, f32 m02_, f32 m03_, 
		f32 m10_, f32 m11_, f32 m12_, f32 m13_, 
		f32 m20_, f32 m21_, f32 m22_, f32 m23_,
		f32 m30_, f32 m31_, f32 m32_, f32 m33_)
	{
		m_data[0][0] = m00_;
		m_data[1][0] = m01_;
		m_data[2][0] = m02_;
		m_data[3][0] = m03_;
		m_data[0][1] = m10_;
		m_data[1][1] = m11_;
		m_data[2][1] = m12_;
		m_data[3][1] = m13_;
		m_data[0][2] = m20_;
		m_data[1][2] = m21_;
		m_data[2][2] = m22_;
		m_data[3][2] = m23_;
		m_data[0][3] = m30_;
		m_data[1][3] = m31_;
		m_data[2][3] = m32_;
		m_data[3][3] = m33_;
	}	
	//--------------------------------------------------------
	c_matrix4& c_matrix4::operator = (const c_matrix4& rhs_)
	{
		for(u32 i = 0 ; i < 16 ; ++i)
			m_ele[i] = rhs_.m_ele[i];
		return *this;
	}
	//--------------------------------------------------------
	bool c_matrix4::operator==(const c_matrix4& mx_) const
	{
		if(m_data[0][0] != mx_.m_data[0][0] || m_data[0][1] != mx_.m_data[0][1] || 
			m_data[0][2] != mx_.m_data[0][2] || m_data[0][3] != mx_.m_data[0][3] ||
			m_data[1][0] != mx_.m_data[1][0] || m_data[1][1] != mx_.m_data[1][1] ||
			m_data[1][2] != mx_.m_data[1][2] || m_data[1][3] != mx_.m_data[1][3] ||
			m_data[2][0] != mx_.m_data[2][0] || m_data[2][1] != mx_.m_data[2][1] || 
			m_data[2][2] != mx_.m_data[2][2] || m_data[2][3] != mx_.m_data[2][3] ||
			m_data[3][0] != mx_.m_data[3][0] || m_data[3][1] != mx_.m_data[3][1] ||
			m_data[3][2] != mx_.m_data[3][2] || m_data[3][3] != mx_.m_data[3][3] )
			return false;
		return true;
	}
	//--------------------------------------------------------
	bool c_matrix4::operator!=(const c_matrix4& mx_) const
	{
		if(m_data[0][0] != mx_.m_data[0][0] || m_data[0][1] != mx_.m_data[0][1] || 
			m_data[0][2] != mx_.m_data[0][2] || m_data[0][3] != mx_.m_data[0][3] ||
			m_data[1][0] != mx_.m_data[1][0] || m_data[1][1] != mx_.m_data[1][1] ||
			m_data[1][2] != mx_.m_data[1][2] || m_data[1][3] != mx_.m_data[1][3] ||
			m_data[2][0] != mx_.m_data[2][0] || m_data[2][1] != mx_.m_data[2][1] || 
			m_data[2][2] != mx_.m_data[2][2] || m_data[2][3] != mx_.m_data[2][3] ||
			m_data[3][0] != mx_.m_data[3][0] || m_data[3][1] != mx_.m_data[3][1] ||
			m_data[3][2] != mx_.m_data[3][2] || m_data[3][3] != mx_.m_data[3][3] )
			return true;
		return false;
	}
	//--------------------------------------------------------
	f32* c_matrix4::operator [] (u32 col_)
	{
		assert( col_ < 4 );
		return m_data[col_];
	}
	//--------------------------------------------------------
	const f32* c_matrix4::operator [] (u32 col_) const
	{
		assert( col_ < 4 );
		return m_data[col_];
	}
	//--------------------------------------------------------
	const f32* c_matrix4::glmat() const
	{
		return m_ele;
	}
}
#endif