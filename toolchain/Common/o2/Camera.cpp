/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Algorithm.h"
#include "Device.h"
#include "Camera.h"
namespace o2d{
	c_camera::c_camera()
		:m_eye(0.0f ,0.0f , 0.0f) , 
		m_lookat(0.0f , 0.0f , 1.0f) , 
		m_up(0.0f, 1.0f, 0.0f) , 
		m_fov(0.f) , 
		m_aspect(0.f) , 
		m_near(0.f) , 
		m_far(0.f)
	{}
	//--------------------------------------------------------
	void c_camera::build_proj_matrix()
	{
		mat_perspective_lh(m_proj_mat , m_fov*DEG_2_RAD , m_aspect , m_near , m_far);
		m_pv_mat = mat_multiply(m_proj_mat , m_view_mat);	
		_calc_frustum();
	}
	//--------------------------------------------------------
	void c_camera::build_view_matrix()
	{
		mat_lookat_lh(m_view_mat , m_eye , m_lookat , m_up);
		m_pv_mat = mat_multiply(m_proj_mat , m_view_mat);	
		_calc_frustum();
	}
	//--------------------------------------------------------
	void c_camera::_calc_frustum()
	{
		m_frustum.plane(FP_LEFT).normal().x() = m_pv_mat[0][3] + m_pv_mat[0][0];
		m_frustum.plane(FP_LEFT).normal().y() = m_pv_mat[1][3] + m_pv_mat[1][0];
		m_frustum.plane(FP_LEFT).normal().z() = m_pv_mat[2][3] + m_pv_mat[2][0];
		m_frustum.plane(FP_LEFT).d() = m_pv_mat[3][3] + m_pv_mat[3][0];
		m_frustum.plane(FP_RIGHT).normal().x() = m_pv_mat[0][3] - m_pv_mat[0][0];
		m_frustum.plane(FP_RIGHT).normal().y() = m_pv_mat[1][3] - m_pv_mat[1][0];
		m_frustum.plane(FP_RIGHT).normal().z() = m_pv_mat[2][3] - m_pv_mat[2][0];
		m_frustum.plane(FP_RIGHT).d() = m_pv_mat[3][3] - m_pv_mat[3][0];
		m_frustum.plane(FP_TOP).normal().x() = m_pv_mat[0][3] - m_pv_mat[0][1];
		m_frustum.plane(FP_TOP).normal().y() = m_pv_mat[1][3] - m_pv_mat[1][1];
		m_frustum.plane(FP_TOP).normal().z() = m_pv_mat[2][3] - m_pv_mat[2][1];
		m_frustum.plane(FP_TOP).d() = m_pv_mat[3][3] - m_pv_mat[3][1];
		m_frustum.plane(FP_BOTTOM).normal().x() = m_pv_mat[0][3] + m_pv_mat[0][1];
		m_frustum.plane(FP_BOTTOM).normal().y() = m_pv_mat[1][3] + m_pv_mat[1][1];
		m_frustum.plane(FP_BOTTOM).normal().z() = m_pv_mat[2][3] + m_pv_mat[2][1];
		m_frustum.plane(FP_BOTTOM).d() = m_pv_mat[3][3] + m_pv_mat[3][1];
		m_frustum.plane(FP_NEAR).normal().x() = m_pv_mat[0][3] + m_pv_mat[0][2];
		m_frustum.plane(FP_NEAR).normal().y() = m_pv_mat[1][3] + m_pv_mat[1][2];
		m_frustum.plane(FP_NEAR).normal().z() = m_pv_mat[2][3] + m_pv_mat[2][2];
		m_frustum.plane(FP_NEAR).d() = m_pv_mat[3][3] + m_pv_mat[3][2];
		m_frustum.plane(FP_FAR).normal().x() = m_pv_mat[0][3] - m_pv_mat[0][2];
		m_frustum.plane(FP_FAR).normal().y() = m_pv_mat[1][3] - m_pv_mat[1][2];
		m_frustum.plane(FP_FAR).normal().z() = m_pv_mat[2][3] - m_pv_mat[2][2];
		m_frustum.plane(FP_FAR).d() = m_pv_mat[3][3] - m_pv_mat[3][2];
		for(u32 i=0; i<FP_NUM; i++) 
		{
			f32 length = 1 / vec_length(m_frustum.plane((e_frustum_plane)i).normal());
			m_frustum.plane((e_frustum_plane)i).d() *= length;
			vec_normalize(m_frustum.plane((e_frustum_plane)i).normal());
		}
	}
}