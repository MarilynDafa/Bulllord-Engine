/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Algorithm.h"
#include "Tools.h"
namespace o2d{
	bool equal(f32 v1_, f32 v2_)
	{
		return fabs(v1_-v2_)<EPSILON;
	}
	//--------------------------------------------------------
	f32 clamp(f32 val_, f32 min_, f32 max_)
	{
		if ( val_ < min_ )  return min_;
		else if ( val_ > max_ )  return max_;
		else  return val_;
	}
	//--------------------------------------------------------
	s32 randi()
	{
		return rand();
	}
	//--------------------------------------------------------
	s32 randi(s32 min_, s32 max_)
	{
		if(max_ == min_)
			return min_;
		return rand()%(max_ - min_) + min_;
	}
	//--------------------------------------------------------
	f32 randf()
	{
		return f32(rand())/f32(0x7FFF);
	}
	//--------------------------------------------------------
	f32 randf(f32 min_, f32 max_)
	{
		if(max_ == min_)
			return min_;
		return (max_-min_)*f32(rand())/f32(0x7FFF) + min_;
	}
	//--------------------------------------------------------
	c_rect rect_intersects(const c_rect& r1_, const c_rect& r2_)
	{
		c_rect ret;
		ret.lt_pt().x()	= MAX_VALUE(r1_.lt_pt().x(), r2_.lt_pt().x());
		ret.lt_pt().y()	= MAX_VALUE(r1_.lt_pt().y(), r2_.lt_pt().y());
		ret.rb_pt().x() = MIN_VALUE(r1_.rb_pt().x(), r2_.rb_pt().x());
		ret.rb_pt().y() = MIN_VALUE(r1_.rb_pt().y(), r2_.rb_pt().y());
		return ret;
	}
	//--------------------------------------------------------
	bool rect_contains(const c_rect& r_, const c_float2& p_)
	{
		bool b1 = p_.x() > r_.lt_pt().x() && p_.x() < r_.rb_pt().x();
		bool b2 = p_.y() > r_.lt_pt().y() && p_.y() < r_.rb_pt().y();
		return b1&&b2;
	}
	//--------------------------------------------------------
	void rect_clip(c_rect& target_, const c_rect& other_)
	{
		if (other_.rb_pt().x() < target_.rb_pt().x())
			target_.rb_pt().x() = other_.rb_pt().x();
		if (other_.rb_pt().y() < target_.rb_pt().y())
			target_.rb_pt().y() = other_.rb_pt().y();
		if (other_.lt_pt().x() > target_.lt_pt().x())
			target_.lt_pt().x() = other_.lt_pt().x();
		if (other_.lt_pt().y() > target_.lt_pt().y())
			target_.lt_pt().y() = other_.lt_pt().y();
		if (target_.lt_pt().y() > target_.rb_pt().y())
			target_.lt_pt().y() = target_.rb_pt().y();
		if (target_.lt_pt().x() > target_.rb_pt().x())
			target_.lt_pt().x() = target_.rb_pt().x();
	}
	//--------------------------------------------------------
	void rect_extend(c_rect& rc_, const c_float2& p_)
	{
		if(rc_.lt_pt().x() > p_.x())
			rc_.lt_pt().x() = p_.x();
		if(rc_.lt_pt().y() > p_.y())
			rc_.lt_pt().y() = p_.y();
		if(rc_.rb_pt().x() < p_.x())
			rc_.rb_pt().x() = p_.x();
		if(rc_.rb_pt().y() < p_.y())
			rc_.rb_pt().y() = p_.y();
	}
	//--------------------------------------------------------
	f32 vec_length(const c_float2& v_)
	{
		return sqrt( v_.x() * v_.x() + v_.y() * v_.y() );
	}
	//--------------------------------------------------------
	f32 vec_sqlength(const c_float2& v_)
	{
		return v_.x() * v_.x() + v_.y() * v_.y();
	}
	//--------------------------------------------------------
	f32 pt_distance(const c_float2& v1_, const c_float2& v2_)
	{
		return sqrt((v1_ - v2_).x() * (v1_ - v2_).x() + (v1_ - v2_).y() * (v1_ - v2_).y());
	}
	//--------------------------------------------------------
	f32 pt_sqdistance(const c_float2& v1_, const c_float2& v2_)
	{
		return (v1_ - v2_).x() * (v1_ - v2_).x() + (v1_ - v2_).y() * (v1_ - v2_).y();
	}	
	//--------------------------------------------------------
	const c_float2& vec_normalize(c_float2& v_)
	{
		f32 veclength = sqrt( v_.x() * v_.x() + v_.y() * v_.y() );
		if (veclength > EPSILON)
		{
			v_.x() /= veclength;
			v_.y() /= veclength;
		}
		return v_;
	}
	//--------------------------------------------------------
	f32 vec_dotproduct(const c_float2& v1_, const c_float2& v2_)
	{
		return v1_.x() * v2_.x() + v1_.y() * v2_.y();
	}
	//--------------------------------------------------------
	f32 vec_crossproduct(const c_float2& v1_, const c_float2& v2_)
	{
		return v1_.x() * v2_.y() - v1_.y() * v2_.x();
	}
	//--------------------------------------------------------
	bool vec_iszero(const c_float2& v_)
	{
		return (v_.x() * v_.x() + v_.y() * v_.y())<EPSILON;
	}
	//--------------------------------------------------------
	bool equal(const c_float2& v1_, const c_float2& v2_)
	{
		return ((v1_ - v2_).x() * (v1_ - v2_).x() + (v1_ - v2_).y() * (v1_ - v2_).y())<EPSILON;
	}
	//--------------------------------------------------------
	f32 vec_length(const c_float3& v_)
	{
		return sqrt( v_.x() * v_.x() + v_.y() * v_.y() + v_.z() * v_.z());
	}
	//--------------------------------------------------------
	const c_float3& vec_normalize(c_float3& v_)
	{
		f32 len = sqrt( v_.x() * v_.x() + v_.y() * v_.y() + v_.z() * v_.z());
		if ( len > 0.0f )
		{
			f32 invlen = 1.0f / len;
			v_.x() *= invlen;
			v_.y() *= invlen;
			v_.z() *= invlen;
		}
		return v_;
	}
	//--------------------------------------------------------
	c_quaternion quat_from_rotation(const c_float3& axis_, f32 rad_)
	{
		c_quaternion ret;
		f32 fsin = sinf(0.5*rad_);
		ret.w() = cosf(0.5*rad_);
		ret.x() = fsin*axis_.x();
		ret.y() = fsin*axis_.y();
		ret.z() = fsin*axis_.z();
		return ret;
	}
	//--------------------------------------------------------
	const c_float3& quat_transform(const c_quaternion& q_, c_float3& vec_)
	{
		c_float3 uv, uuv;
		c_float3 qvec(q_.x(), q_.y(), q_.z());
		uv = c_float3(qvec.y() * vec_.z() - qvec.z() * vec_.y(), qvec.z() * vec_.x() - qvec.x() * vec_.z(), qvec.x() * vec_.y() - qvec.y() * vec_.x());
		uuv = c_float3(qvec.y() * uv.z() - qvec.z() * uv.y(), qvec.z() * uv.x() - qvec.x() * uv.z(), qvec.x() * uv.y() - qvec.y() * uv.x());
		uv *= (2.0f * q_.w());
		uuv *= 2.0f;
		vec_ =  vec_ + uv + uuv;
		return vec_;
	}	
	//--------------------------------------------------------
	c_matrix4 mat_multiply(const c_matrix4& m1_ , const c_matrix4& m2_)
	{
		c_matrix4 r;
		r[0][0] = m1_[0][0] * m2_[0][0] + m1_[1][0] * m2_[0][1] + m1_[2][0] * m2_[0][2] + m1_[3][0] * m2_[0][3];
		r[1][0] = m1_[0][0] * m2_[1][0] + m1_[1][0] * m2_[1][1] + m1_[2][0] * m2_[1][2] + m1_[3][0] * m2_[1][3];
		r[2][0] = m1_[0][0] * m2_[2][0] + m1_[1][0] * m2_[2][1] + m1_[2][0] * m2_[2][2] + m1_[3][0] * m2_[2][3];
		r[3][0] = m1_[0][0] * m2_[3][0] + m1_[1][0] * m2_[3][1] + m1_[2][0] * m2_[3][2] + m1_[3][0] * m2_[3][3];
		r[0][1] = m1_[0][1] * m2_[0][0] + m1_[1][1] * m2_[0][1] + m1_[2][1] * m2_[0][2] + m1_[3][1] * m2_[0][3];
		r[1][1] = m1_[0][1] * m2_[1][0] + m1_[1][1] * m2_[1][1] + m1_[2][1] * m2_[1][2] + m1_[3][1] * m2_[1][3];
		r[2][1] = m1_[0][1] * m2_[2][0] + m1_[1][1] * m2_[2][1] + m1_[2][1] * m2_[2][2] + m1_[3][1] * m2_[2][3];
		r[3][1] = m1_[0][1] * m2_[3][0] + m1_[1][1] * m2_[3][1] + m1_[2][1] * m2_[3][2] + m1_[3][1] * m2_[3][3];
		r[0][2] = m1_[0][2] * m2_[0][0] + m1_[1][2] * m2_[0][1] + m1_[2][2] * m2_[0][2] + m1_[3][2] * m2_[0][3];
		r[1][2] = m1_[0][2] * m2_[1][0] + m1_[1][2] * m2_[1][1] + m1_[2][2] * m2_[1][2] + m1_[3][2] * m2_[1][3];
		r[2][2] = m1_[0][2] * m2_[2][0] + m1_[1][2] * m2_[2][1] + m1_[2][2] * m2_[2][2] + m1_[3][2] * m2_[2][3];
		r[3][2] = m1_[0][2] * m2_[3][0] + m1_[1][2] * m2_[3][1] + m1_[2][2] * m2_[3][2] + m1_[3][2] * m2_[3][3];
		r[0][3] = m1_[0][3] * m2_[0][0] + m1_[1][3] * m2_[0][1] + m1_[2][3] * m2_[0][2] + m1_[3][3] * m2_[0][3];
		r[1][3] = m1_[0][3] * m2_[1][0] + m1_[1][3] * m2_[1][1] + m1_[2][3] * m2_[1][2] + m1_[3][3] * m2_[1][3];
		r[2][3] = m1_[0][3] * m2_[2][0] + m1_[1][3] * m2_[2][1] + m1_[2][3] * m2_[2][2] + m1_[3][3] * m2_[2][3];
		r[3][3] = m1_[0][3] * m2_[3][0] + m1_[1][3] * m2_[3][1] + m1_[2][3] * m2_[3][2] + m1_[3][3] * m2_[3][3];
		return r;
	}
	//--------------------------------------------------------
	c_matrix4 mat_from_transform(const c_float3& trans_, const c_float3& scale_, const c_float3& ropt_, const c_quaternion& rotate_)
	{
		c_matrix4 ret;
		f32 tx  = rotate_.x()+rotate_.x();
		f32 ty  = rotate_.y()+rotate_.y();
		f32 tz  = rotate_.z()+rotate_.z();
		f32 twx = tx*rotate_.w();
		f32 twy = ty*rotate_.w();
		f32 twz = tz*rotate_.w();
		f32 txx = tx*rotate_.x();
		f32 txy = ty*rotate_.x();
		f32 txz = tz*rotate_.x();
		f32 tyy = ty*rotate_.y();
		f32 tyz = tz*rotate_.y();
		f32 tzz = tz*rotate_.z();
		ret[0][0] = (1.0f-(tyy+tzz)) * scale_.x();
		ret[1][0] = (txy-twz) * scale_.y();
		ret[2][0] = (txz+twy) * scale_.z();
		ret[3][0] = (1.0f-(tyy+tzz)) * -ropt_.x() + (txy-twz) * -ropt_.y() + ropt_.x() + trans_.x();
		ret[0][1] = (txy+twz) * scale_.x();
		ret[1][1] = (1.0f-(txx+tzz)) * scale_.y();
		ret[2][1] = (tyz-twx) * scale_.z();
		ret[3][1] = (txy+twz) * -ropt_.x() + (1.0f-(txx+tzz)) * -ropt_.y() + ropt_.y() + trans_.y();
		ret[0][2] = (txz-twy) * scale_.x();
		ret[1][2] = (tyz+twx) * scale_.y();
		ret[2][2] = (1.0f-(txx+tyy)) * scale_.z();
		ret[3][2] = (txz-twy) * -ropt_.x() + (tyz+twx) * -ropt_.y() + ropt_.z() + trans_.z();
		ret[0][3] = 0.f;
		ret[1][3] = 0.f;
		ret[2][3] = 0.f;
		ret[3][3] = 1.f;
		return ret;
	}
	//--------------------------------------------------------
	const c_float3& mat_transform(const c_matrix4& mat_, c_float3& vec_)
	{
		f32 x = vec_.x();
		f32 y = vec_.y();
		f32 z = vec_.z();
		f32 finvw = 1.0f / ( mat_[0][3] * x + mat_[1][3] * y + mat_[2][3] * z + mat_[3][3] );
		vec_.x() = ( mat_[0][0] * x + mat_[1][0] * y + mat_[2][0] * z + mat_[3][0] ) * finvw;
		vec_.y() = ( mat_[0][1] * x + mat_[1][1] * y + mat_[2][1] * z + mat_[3][1] ) * finvw;
		vec_.z() = ( mat_[0][2] * x + mat_[1][2] * y + mat_[2][2] * z + mat_[3][2] ) * finvw;
		return vec_;
	}
	//--------------------------------------------------------
	const c_matrix4& mat_transpose(c_matrix4& mat_)
	{
		c_matrix4 mat = c_matrix4(mat_[0][0], mat_[0][1], mat_[0][2], mat_[0][3],
			mat_[1][0], mat_[1][1], mat_[1][2], mat_[1][3],
			mat_[2][0], mat_[2][1], mat_[2][2], mat_[2][3],
			mat_[3][0], mat_[3][1], mat_[3][2], mat_[3][3]);
		mat_ = mat;
		return mat_;
	}
	//--------------------------------------------------------
	const c_matrix4& mat_perspective_lh(c_matrix4& mat_, f32 fov_, f32 aspect_, f32 near_, f32 far_)
	{
		f32 h = 1.f/tan(fov_*0.5f);
		mat_[0][0] = h / aspect_;
		mat_[0][1] = 0.f;
		mat_[0][2] = 0.f;
		mat_[0][3] = 0.f;
		mat_[1][0] = 0.f;
		mat_[1][1] = h;
		mat_[1][2] = 0.f;
		mat_[1][3] = 0.f;
		mat_[2][0] = 0.f;
		mat_[2][1] = 0.f;
		mat_[2][2] = far_/(far_-near_);
		mat_[2][3] = 1.f;
		mat_[3][0] = 0.f;
		mat_[3][1] = 0.f;
		mat_[3][2] = -near_*far_/(far_-near_);
		mat_[3][3] = 0.f;
		return mat_;
	}
	//--------------------------------------------------------
	const c_matrix4& mat_lookat_lh(c_matrix4& mat_, const c_float3& eye_, const c_float3& focus_, const c_float3& up_)
	{
		c_float3 zaxis = focus_ - eye_;
		f32 len = sqrt(zaxis.x() * zaxis.x() + zaxis.y() * zaxis.y() + zaxis.z() * zaxis.z());
		if ( len > 0.0f )
		{
			f32 invlen = 1.0f / len;
			zaxis.x() *= invlen;
			zaxis.y() *= invlen;
			zaxis.z() *= invlen;
		}
		c_float3 xaxis = c_float3(up_.y() * zaxis.z() - up_.z() * zaxis.y(), up_.z() * zaxis.x() - up_.x() * zaxis.z(), up_.x() * zaxis.y() - up_.y() * zaxis.x());
		len = sqrt( xaxis.x() * xaxis.x() + xaxis.y() * xaxis.y() + xaxis.z() * xaxis.z());
		if ( len > 0.0f )
		{
			f32 invlen = 1.0f / len;
			xaxis.x() *= invlen;
			xaxis.y() *= invlen;
			xaxis.z() *= invlen;
		}
		c_float3 yaxis = c_float3(zaxis.y() * xaxis.z() - zaxis.z() * xaxis.y(), zaxis.z() * xaxis.x() - zaxis.x() * xaxis.z(), zaxis.x() * xaxis.y() - zaxis.y() * xaxis.x());
		mat_[0][0] = xaxis.x();
		mat_[0][1] = yaxis.x();
		mat_[0][2] = zaxis.x();
		mat_[0][3] = 0.f;
		mat_[1][0] = xaxis.y();
		mat_[1][1] = yaxis.y();
		mat_[1][2] = zaxis.y();
		mat_[1][3] = 0.f;
		mat_[2][0] = xaxis.z();
		mat_[2][1] = yaxis.z();
		mat_[2][2] = zaxis.z();
		mat_[2][3] = 0.f;
		mat_[3][0] = -xaxis.x() * eye_.x() + -xaxis.y() * eye_.y() + -xaxis.z() * eye_.z();
		mat_[3][1] = -yaxis.x() * eye_.x() + -yaxis.y() * eye_.y() + -yaxis.z() * eye_.z();
		mat_[3][2] = -zaxis.x() * eye_.x() + -zaxis.y() * eye_.y() + -zaxis.z() * eye_.z();
		mat_[3][3] = 1.f;
		return mat_;
	}

	//--------------------------------------------------------
	e_visibility_state frustum_cull(const c_frustum& fru_ , const c_box& box_)
	{
		if (box_ == c_box::Empty)
			return VS_NONE;
		c_float3 centre = (box_.minpt() + box_.maxpt())*0.5f;
		c_float3 halfsz = (box_.maxpt() - box_.minpt())*0.5f;
		bool allinside = true;
		for(u32 idx = 0; idx < FP_NUM; ++idx)
		{
			if (idx == FP_FAR)
				continue;
			c_float3 no = fru_.plane((e_frustum_plane)idx).normal();
			f32 d = fru_.plane((e_frustum_plane)idx).d();
			f32 dist = no.x() * centre.x() + no.y() * centre.y() + no.z() * centre.z() + d;
			f32 maxabsdist = fabs(no.x()*halfsz.x()) + fabs(no.y()*halfsz.y()) + fabs(no.z()*halfsz.z());
			e_plane_side side;
			if (dist < -maxabsdist)
				side = PS_NEGATIVE_SIDE;
			else if(dist > maxabsdist)
				side = PS_POSITIVE_SIDE;
			else
				side = PS_BOTH_SIDE;
			if(side == PS_NEGATIVE_SIDE)
				return VS_NONE;
			if(side == PS_BOTH_SIDE) 
				allinside = false;
		}
		if(allinside)
			return VS_FULL;
		else
			return VS_PARTIAL;
	}
}
