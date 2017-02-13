/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef ALGORITHM_H_INCLUDED
#define ALGORITHM_H_INCLUDED
#include "Float2.h"
#include "Rectangle.h"
#include "Float3.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "AABBox.h"
#include "Frustum.h"
namespace o2d{
	O2D_API bool equal(f32 v1_, f32 v2_);
	O2D_API f32 clamp(f32 val_, f32 min_, f32 max_);
	O2D_API s32 randi();
	O2D_API s32 randi(s32 min_, s32 max_);
	O2D_API f32 randf();
	O2D_API f32 randf(f32 min_, f32 max_);
	O2D_API c_rect rect_intersects(const c_rect& r1_, const c_rect& r2_);
	O2D_API bool rect_contains(const c_rect& r_, const c_float2& p_);
	O2D_API void rect_clip(c_rect& target_, const c_rect& other_);
	O2D_API void rect_extend(c_rect& rc_, const c_float2& p_);
	O2D_API f32 vec_length(const c_float2& v_);
	O2D_API f32 vec_sqlength (const c_float2& v_);
	O2D_API f32 pt_distance(const c_float2& v1_, const c_float2& v2_);
	O2D_API f32 pt_sqdistance(const c_float2& v1_, const c_float2& v2_);
	O2D_API const c_float2& vec_normalize(c_float2& v_);
	O2D_API f32 vec_dotproduct(const c_float2& v1_, const c_float2& v2_);
	O2D_API f32 vec_crossproduct(const c_float2& v1_, const c_float2& v2_);
	O2D_API bool vec_iszero(const c_float2& v_);
	O2D_API bool equal(const c_float2& v1_, const c_float2& v2_);
	O2D_API f32 vec_length(const c_float3& v_);
	O2D_API const c_float3& vec_normalize(c_float3& v_);
	O2D_API c_quaternion quat_from_rotation(const c_float3& axis_, f32 rad_);
	O2D_API const c_float3& quat_transform(const c_quaternion& q_, c_float3& vec_);
	O2D_API c_matrix4 mat_multiply(const c_matrix4& m1_ , const c_matrix4& m2_);
	O2D_API c_matrix4 mat_from_transform(const c_float3& trans_, const c_float3& scale_, const c_float3& ropt_, const c_quaternion& rotate_);
	O2D_API const c_float3& mat_transform(const c_matrix4& mat_, c_float3& vec_);
	O2D_API const c_matrix4& mat_transpose(c_matrix4& mat_);
	O2D_API const c_matrix4& mat_perspective_lh(c_matrix4& mat_, f32 fov_, f32 aspect_, f32 near_, f32 far_);
	O2D_API const c_matrix4& mat_lookat_lh(c_matrix4& mat_, const c_float3& eye_, const c_float3& focus_, const c_float3& up_);
	O2D_API e_visibility_state frustum_cull(const c_frustum& fru_ , const c_box& box_);
}
#endif