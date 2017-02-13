/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef CAMERA_H_INCLUDE
#define CAMERA_H_INCLUDE
#include "Float3.h"
#include "Matrix4.h"
#include "Frustum.h"
namespace o2d{
	class O2D_API c_camera
	{
	private:
		c_matrix4 m_view_mat;
		c_matrix4 m_proj_mat;
		c_matrix4 m_pv_mat;
		c_frustum m_frustum;
		c_float3 m_eye;
		c_float3 m_lookat;
		c_float3 m_up;
		f32 m_fov;
		f32 m_aspect;
		f32 m_near;
		f32 m_far;
	public:
		c_camera();
	public:
		inline void set_eye(const c_float3& eye_){ m_eye = eye_;}
		inline const c_float3& get_eye() const { return m_eye; }
		inline void set_lookat(const c_float3& lookat_) { m_lookat = lookat_;}
		inline const c_float3& get_lookat() const { return m_lookat; }
		inline void set_upvec(const c_float3& up_) { m_up = up_;}
		inline const c_float3& get_upvec() const  { return m_up; }
		inline void set_fov(f32 fov_) {m_fov = fov_;}
		inline f32 get_fov() const { return m_fov; }
		inline void set_ratio(f32 as_) {m_aspect = as_;}
		inline f32 get_ratio() const { return m_aspect; }
		inline void set_near_plane(f32 n_) {m_near = n_;}
		inline f32 get_near_plane() const { return m_near; }
		inline void set_far_plane(f32 f_) {m_far = f_;}
		inline f32 get_far_plane() const { return m_far; }
		inline const c_matrix4& get_view_matrix() const {return m_view_mat;}
		inline const c_matrix4& get_proj_matrix() const {return m_proj_mat;}
		inline const c_matrix4& get_pv_matrix() const {return m_pv_mat;}
		inline const c_frustum& get_frustum() const {return m_frustum;}
	public:
		void build_proj_matrix();
		void build_view_matrix();
	private:
		void _calc_frustum();
	};
}
#endif