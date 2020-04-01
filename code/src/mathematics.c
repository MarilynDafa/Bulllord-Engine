/*
 Bulllord Game Engine
 Copyright (C) 2010-2017 Trix

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.
 */
#include "internal/mathematics.h"
BLS32
blRandI()
{
	return rand();
}
BLS32
blRandRangeI(IN BLS32 _Min, IN BLS32 _Max)
{
	return rand() % (_Max - _Min) + _Min;
}
BLF32
blRandF()
{
	return (BLF32)rand() / (BLF32)RAND_MAX;
}
BLF32
blRandRangeF(IN BLF32 _Min, IN BLF32 _Max)
{
	return (_Max - _Min) * (BLF32)rand() / (BLF32)RAND_MAX + _Min;
}
BLBool
blScalarApproximate(IN BLF32 _V1, IN BLF32 _V2)
{
	return fabs(_V1 - _V2) < 1e-6;
}
BLF32
blScalarClamp(IN BLF32 _Val, IN BLF32 _Min, IN BLF32 _Max)
{
	if (_Val < _Min)
		return _Min;
	else if (_Val > _Max)
		return _Max;
	else
		return _Val;
}
BLBool
blRectApproximate(IN BLRect* _R1, IN BLRect* _R2)
{
	if (fabs(_R1->sLT.fX - _R2->sLT.fX) > 1e-6)
		return FALSE;
	if (fabs(_R1->sLT.fY - _R2->sLT.fY) > 1e-6)
		return FALSE;
	if (fabs(_R1->sRB.fX - _R2->sRB.fX) > 1e-6)
		return FALSE;
	if (fabs(_R1->sRB.fY - _R2->sRB.fY) > 1e-6)
		return FALSE;
	return TRUE;
}
BLRect
blRectIntersects(IN BLRect* _R1, IN BLRect* _R2)
{
	BLRect _ret;
	_ret.sLT.fX = _R1->sLT.fX > _R2->sLT.fX ? _R1->sLT.fX : _R2->sLT.fX;
	_ret.sLT.fY = _R1->sLT.fY > _R2->sLT.fY ? _R1->sLT.fY : _R2->sLT.fY;
	_ret.sRB.fX = _R1->sRB.fX < _R2->sRB.fX ? _R1->sRB.fX : _R2->sRB.fX;
	_ret.sRB.fY = _R1->sRB.fY < _R2->sRB.fY ? _R1->sRB.fY : _R2->sRB.fY;
	return _ret;
}
BLBool
blRectContains(IN BLRect* _Rc, IN BLVec2* _Pt)
{
	BLBool _b1 = _Pt->fX > _Rc->sLT.fX && _Pt->fX < _Rc->sRB.fX;
	BLBool _b2 = _Pt->fY > _Rc->sLT.fY && _Pt->fY < _Rc->sRB.fY;
	return _b1&&_b2;
}
BLRect
blRectClip(IN BLRect* _R1, IN BLRect* _R2)
{
	BLRect _ret = { {0.f, 0.f}, {0.f, 0.f} };
	if (_R2->sRB.fX < _R1->sRB.fX)
		_ret.sRB.fX = _R2->sRB.fX;
	if (_R2->sRB.fY < _R1->sRB.fY)
		_ret.sRB.fY = _R2->sRB.fY;
	if (_R2->sLT.fX > _R1->sLT.fX)
		_ret.sLT.fX = _R2->sLT.fX;
	if (_R2->sLT.fY > _R1->sLT.fY)
		_ret.sLT.fY = _R2->sLT.fY;
	if (_R1->sLT.fY > _R1->sRB.fY)
		_ret.sLT.fY = _R1->sRB.fY;
	if (_R1->sLT.fX > _R1->sRB.fX)
		_ret.sLT.fX = _R1->sRB.fX;
	return _ret;
}
BLVoid
blRectExtend(INOUT BLRect* _Tar, IN BLVec2* _Pt)
{
	if (_Tar->sLT.fX > _Pt->fX)
		_Tar->sLT.fX = _Pt->fX;
	if (_Tar->sLT.fY > _Pt->fY)
		_Tar->sLT.fY = _Pt->fY;
	if (_Tar->sRB.fX < _Pt->fX)
		_Tar->sRB.fX = _Pt->fX;
	if (_Tar->sRB.fY < _Pt->fY)
		_Tar->sRB.fY = _Pt->fY;
}
BLBool
blVec2Approximate(IN BLVec2* _V1, IN BLVec2* _V2)
{
	if (fabs(_V1->fX - _V2->fX) > 1e-6)
		return FALSE;
	if (fabs(_V1->fY - _V2->fY) > 1e-6)
		return FALSE;
	return TRUE;
}
BLF32
blVec2Length(IN BLVec2* _V)
{
	return sqrtf(_V->fX * _V->fX + _V->fY * _V->fY);
}
BLF32
blVec2Sqlength(IN BLVec2* _V)
{
	return _V->fX * _V->fX + _V->fY * _V->fY;
}
BLVoid
blVec2Normalize(INOUT BLVec2* _V)
{
	BLF32 _veclength = sqrtf(_V->fX * _V->fX + _V->fY * _V->fY);
	if (_veclength > 1e-6)
	{
		_V->fX /= _veclength;
		_V->fY /= _veclength;
	}
}
BLF32
blVec2DotProduct(IN BLVec2* _V1, IN BLVec2* _V2)
{
	return _V1->fX * _V2->fX + _V1->fY * _V2->fY;
}
BLF32
blVec2CrossProduct(IN BLVec2* _V1, IN BLVec2* _V2)
{
	return _V1->fX * _V2->fY - _V1->fY * _V2->fX;
}
BLBool
blVec3Approximate(IN BLVec3* _V1, IN BLVec3* _V2)
{
	if (fabs(_V1->fX - _V2->fX) > 1e-6)
		return FALSE;
	if (fabs(_V1->fY - _V2->fY) > 1e-6)
		return FALSE;
	if (fabs(_V1->fZ - _V2->fZ) > 1e-6)
		return FALSE;
	return TRUE;
}
BLF32
blVec3Length(IN BLVec3* _V)
{
	return sqrtf(_V->fX * _V->fX + _V->fY * _V->fY + _V->fZ * _V->fZ);
}
BLF32
blVec3Sqlength(IN BLVec3* _V)
{
	return _V->fX * _V->fX + _V->fY * _V->fY + _V->fZ * _V->fZ;
}
BLVoid
blVec3Normalize(INOUT BLVec3* _V)
{
	BLF32 _len = sqrtf(_V->fX * _V->fX + _V->fY * _V->fY + _V->fZ * _V->fZ);
	if (_len > 0.0f)
	{
		BLF32 _invlen = 1.0f / _len;
		_V->fX *= _invlen;
		_V->fY *= _invlen;
		_V->fZ *= _invlen;
	}
}
BLF32
blVec3DotProduct(IN BLVec3* _V1, IN BLVec3* _V2)
{
	return _V1->fX * _V2->fX + _V1->fY * _V2->fY + _V1->fZ * _V2->fZ;
}
BLVec3
blVec3CrossProduct(IN BLVec3* _V1, IN BLVec3* _V2)
{
	BLVec3 _ret;
	_ret.fX = _V1->fY * _V2->fZ - _V1->fZ * _V2->fY;
	_ret.fY = _V1->fZ * _V2->fX - _V1->fX * _V2->fZ;
	_ret.fZ = _V1->fX * _V2->fY - _V1->fY * _V2->fX;
	return _ret;
}
BLBool
blQuatApproximate(IN BLQuaternion* _Q1, IN BLQuaternion* _Q2)
{
	if (fabs(_Q1->fX - _Q2->fX) > 1e-6)
		return FALSE;
	if (fabs(_Q1->fY - _Q2->fY) > 1e-6)
		return FALSE;
	if (fabs(_Q1->fZ - _Q2->fZ) > 1e-6)
		return FALSE;
	if (fabs(_Q1->fW - _Q2->fW) > 1e-6)
		return FALSE;
	return TRUE;
}
BLQuaternion
blQuatFromAxNRad(IN BLVec3* _Axis, IN BLF32 _Rad)
{
	BLQuaternion _ret;
	BLF32 _fsin = sinf(0.5f * _Rad);
	_ret.fW = cosf(0.5f * _Rad);
	_ret.fX = _fsin * _Axis->fX;
	_ret.fY = _fsin * _Axis->fY;
	_ret.fZ = _fsin * _Axis->fZ;
	return _ret;
}
BLQuaternion
blQuatFromRPY(IN BLF32 _Roll, IN BLF32 _Pitch, IN BLF32 _Yaw)
{
	BLF32 _sr, _cr, _sp, _cp, _sy, _cy, _cpcy, _spcy, _cpsy, _spsy, _angle, _len, _factor;
	BLQuaternion _ret;
	_angle = _Roll * 0.5f;
	_sr = sinf(_angle);
	_cr = cosf(_angle);
	_angle = _Pitch * 0.5f;
	_sp = sinf(_angle);
	_cp = cosf(_angle);
	_angle = _Yaw * 0.5f;
	_sy = sinf(_angle);
	_cy = cosf(_angle);
	_cpcy = _cp * _cy;
	_spcy = _sp * _cy;
	_cpsy = _cp * _sy;
	_spsy = _sp * _sy;
	_ret.fX = (BLF32)(_sr * _cpcy - _cr * _spsy);
	_ret.fY = (BLF32)(_cr * _spcy + _sr * _cpsy);
	_ret.fZ = (BLF32)(_cr * _cpsy - _sr * _spcy);
	_ret.fW = (BLF32)(_cr * _cpcy + _sr * _spsy);
	_len = _ret.fW * _ret.fW + _ret.fX * _ret.fX + _ret.fY * _ret.fY + _ret.fZ * _ret.fZ;
	_factor = 1.0f / sqrtf(_len);
	_ret.fX = _ret.fX * _factor;
	_ret.fY = _ret.fY * _factor;
	_ret.fZ = _ret.fZ * _factor;
	_ret.fW = _ret.fW * _factor;
	return _ret;
}
BLQuaternion
blQuatFrom2Vec(IN BLVec3* _Src, IN BLVec3* _Dest)
{
	BLQuaternion _ret;
	BLVec3 _v0 = *_Src;
	BLVec3 _v1 = *_Dest;
	BLF32 _len = sqrtf(_v0.fX * _v0.fX + _v0.fY * _v0.fY + _v0.fZ * _v0.fZ), _d;
	if (_len > 0.0f)
	{
		BLF32 _invlen = 1.0f / _len;
		_v0.fX *= _invlen;
		_v0.fY *= _invlen;
		_v0.fZ *= _invlen;
	}
	_len = sqrtf(_v1.fX * _v1.fX + _v1.fY * _v1.fY + _v1.fZ * _v1.fZ);
	if (_len > 0.0f)
	{
		BLF32 _invlen = 1.0f / _len;
		_v1.fX *= _invlen;
		_v1.fY *= _invlen;
		_v1.fZ *= _invlen;
	}
	_d = _v0.fX * _v1.fX + _v0.fY * _v1.fY + _v0.fZ * _v1.fZ;
	if (_d >= 1.0f)
	{
		_ret.fX = _ret.fY = _ret.fZ = 0.f;
		_ret.fW = 1.f;
		return _ret;
	}
	if (_d < (1e-6f - 1.0f))
	{
		BLF32 _sqlen, _fsin;
		BLVec3 _axis;
		_axis.fX = 0.f;
		_axis.fY = -_v0.fZ;
		_axis.fZ = _v0.fY;
		_sqlen = (_axis.fX * _axis.fX) + (_axis.fY * _axis.fY) + (_axis.fZ * _axis.fZ);
		if (_sqlen < (1e-6 * 1e-6))
		{
			_axis.fX = _v0.fZ;
			_axis.fY = 0.f;
			_axis.fZ = _v0.fX;
		}
		_len = sqrtf(_axis.fX * _axis.fX + _axis.fY * _axis.fY + _axis.fZ * _axis.fZ);
		if (_len > 0.0f)
		{
			BLF32 _invlen = 1.0f / _len;
			_axis.fX *= _invlen;
			_axis.fY *= _invlen;
			_axis.fZ *= _invlen;
		}
		_fsin = sinf(0.5f * 3.141592653589793f);
		_ret.fW = cosf(0.5f * 3.141592653589793f);
		_ret.fX = _fsin * _axis.fX;
		_ret.fY = _fsin * _axis.fY;
		_ret.fZ = _fsin * _axis.fZ;
	}
	else
	{
		BLF32 _s = sqrtf((1 + _d) * 2);
		BLF32 _invs = 1 / _s, _factor;
		BLVec3 _c;
		_c.fX = _v0.fY * _v1.fZ - _v0.fZ * _v1.fY;
		_c.fY = _v0.fZ * _v1.fX - _v0.fX * _v1.fZ;
		_c.fZ = _v0.fX * _v1.fY - _v0.fY * _v1.fX;
		_ret.fX = _c.fX * _invs;
		_ret.fY = _c.fY * _invs;
		_ret.fZ = _c.fZ * _invs;
		_ret.fW = _s * 0.5f;
		_len = _ret.fW * _ret.fW + _ret.fX * _ret.fX + _ret.fY * _ret.fY + _ret.fZ * _ret.fZ;
		_factor = 1.0f / sqrtf(_len);
		_ret.fX = _ret.fX * _factor;
		_ret.fY = _ret.fY * _factor;
		_ret.fZ = _ret.fZ * _factor;
		_ret.fW = _ret.fW * _factor;
	}
	return _ret;
}
BLQuaternion
blQuatFromTran(IN BLVec3* _Xa, IN BLVec3* _Ya, IN BLVec3* _Za)
{
	BLQuaternion _ret;
	BLF32 _trace , _root;
	BLF32 _krot[3][3];
	_krot[0][0] = _Xa->fX;
	_krot[1][0] = _Xa->fY;
	_krot[2][0] = _Xa->fZ;
	_krot[0][1] = _Ya->fX;
	_krot[1][1] = _Ya->fY;
	_krot[2][1] = _Ya->fZ;
	_krot[0][2] = _Za->fX;
	_krot[1][2] = _Za->fY;
	_krot[2][2] = _Za->fZ;
	_trace = _krot[0][0] + _krot[1][1] + _krot[2][2];
	if (_trace > 0.0f)
	{
		_root = sqrtf(_trace + 1.0f);
		_ret.fW = 0.5f * _root;
		_root = 0.5f / _root;
		_ret.fX = (_krot[2][1] - _krot[1][2])*_root;
		_ret.fY = (_krot[0][2] - _krot[2][0])*_root;
		_ret.fZ = (_krot[1][0] - _krot[0][1])*_root;
	}
	else
	{
		BLU32 _inext[3] = { 1, 2, 0 };
		BLU32 _i = 0 , _j , _k;
		BLF32* _apkquat[3];
		if (_krot[1][1] > _krot[0][0])
			_i = 1;
		if (_krot[2][2] > _krot[_i][_i])
			_i = 2;
		_j = _inext[_i];
		_k = _inext[_j];
		_root = sqrtf(_krot[_i][_i] - _krot[_j][_j] - _krot[_k][_k] + 1.0f);
		_apkquat[0] = &_ret.fX;
		_apkquat[1] = &_ret.fY;
		_apkquat[2] = &_ret.fZ;
		*_apkquat[_i] = 0.5f * _root;
		_root = 0.5f / _root;
		_ret.fW = (_krot[_k][_j] - _krot[_j][_k]) * _root;
		*_apkquat[_j] = (_krot[_j][_i] + _krot[_i][_j]) * _root;
		*_apkquat[_k] = (_krot[_k][_i] + _krot[_i][_k]) * _root;
	}
	return _ret;
}
BLVoid
blQuat2Rotation(IN BLQuaternion* _Q, OUT BLF32* _Angle, OUT BLVec3* _Axis)
{
	BLF32 _sqrlength = _Q->fX * _Q->fX + _Q->fY * _Q->fY + _Q->fZ * _Q->fZ;
	if (_sqrlength > 0.0)
	{
		BLF32 _invlength;
		if (-1.0 < _Q->fW)
		{
			if (_Q->fW < 1.0)
				*_Angle = acosf(_Q->fW) * 2.f;
			else
				*_Angle = 0.f;
		}
		else
			*_Angle = 6.283185307179586f;
		_invlength = 1.f / sqrtf(_sqrlength);
		_Axis->fX = _invlength * _Q->fX;
		_Axis->fY = _invlength * _Q->fY;
		_Axis->fZ = _invlength * _Q->fZ;
	}
	else
	{
		*_Angle = 0.f;
		_Axis->fX = 1.f;
		_Axis->fY = 0.f;
		_Axis->fZ = 0.f;
	}
}
BLVoid
blQuatNormalize(INOUT BLQuaternion* _Q)
{
	BLF32 _len = _Q->fW * _Q->fW + _Q->fX * _Q->fX + _Q->fY * _Q->fY + _Q->fZ * _Q->fZ;
	BLF32 _factor = 1.0f / sqrtf(_len);
	_Q->fX = _Q->fX * _factor;
	_Q->fY = _Q->fY * _factor;
	_Q->fZ = _Q->fZ * _factor;
	_Q->fW = _Q->fW * _factor;
}
BLVoid
blQuatInverse(INOUT BLQuaternion* _Q)
{
	BLF32 _norm = _Q->fW * _Q->fW + _Q->fX * _Q->fX + _Q->fY * _Q->fY + _Q->fZ * _Q->fZ;
	if (_norm > 0.0f)
	{
		BLF32 _invnorm = 1.0f / _norm;
		_Q->fX = -_Q->fX * _invnorm;
		_Q->fY = -_Q->fY * _invnorm;
		_Q->fZ = -_Q->fZ * _invnorm;
		_Q->fW = _Q->fW * _invnorm;
	}
	else
	{
		_Q->fX = 0.f;
		_Q->fY = 0.f;
		_Q->fZ = 0.f;
		_Q->fW = 0.f;
	}
}
BLF32
blQuatDotProduct(IN BLQuaternion* _Q1, IN BLQuaternion* _Q2)
{
	return _Q1->fW * _Q2->fW + _Q1->fX * _Q2->fX + _Q1->fY * _Q2->fY + _Q1->fZ * _Q2->fZ;
}
BLQuaternion
blQuatCrossproduct(IN BLQuaternion* _Q1, IN BLQuaternion* _Q2)
{
	BLQuaternion _ret;
	_ret.fX = _Q1->fW * _Q2->fX + _Q1->fX * _Q2->fW + _Q1->fY * _Q2->fZ - _Q1->fZ * _Q2->fY;
	_ret.fY = _Q1->fW * _Q2->fY + _Q1->fY * _Q2->fW + _Q1->fZ * _Q2->fX - _Q1->fX * _Q2->fZ;
	_ret.fZ = _Q1->fW * _Q2->fZ + _Q1->fZ * _Q2->fW + _Q1->fX * _Q2->fY - _Q1->fY * _Q2->fX;
	_ret.fW = _Q1->fW * _Q2->fW - _Q1->fX * _Q2->fX - _Q1->fY * _Q2->fY - _Q1->fZ * _Q2->fZ;
	return _ret;
}
BLQuaternion
blQuatSlerp(IN BLQuaternion* _Q1, IN BLQuaternion* _Q2, IN BLF32 _T)
{
	BLF32 _fcos = _Q1->fW * _Q2->fW + _Q1->fX * _Q2->fX + _Q1->fY * _Q2->fY + _Q1->fZ * _Q2->fZ;
	BLQuaternion _rkt , _ret;
	if (_fcos < 0.0f)
	{
		_fcos = -_fcos;
		_rkt.fX = -_Q2->fX;
		_rkt.fY = -_Q2->fY;
		_rkt.fZ = -_Q2->fZ;
		_rkt.fW = -_Q2->fW;
	}
	else
	{
		_rkt = *_Q2;
	}
	if (fabs(_fcos) < 1 - 1e-6)
	{
		BLF32 _fsin = sqrtf(1 - _fcos * _fcos);
		BLF32 _fangle = atan2f(_fsin, _fcos);
		BLF32 _finvsin = 1.0f / _fsin;
		BLF32 _fcoeff0 = sinf((1.0f - _T) * _fangle) * _finvsin;
		BLF32 _fcoeff1 = sinf(_T * _fangle) * _finvsin;
		_ret.fX = _fcoeff0 * _Q1->fX + _fcoeff1 * _rkt.fX;
		_ret.fY = _fcoeff0 * _Q1->fY + _fcoeff1 * _rkt.fY;
		_ret.fZ = _fcoeff0 * _Q1->fZ + _fcoeff1 * _rkt.fZ;
		_ret.fW = _fcoeff0 * _Q1->fW + _fcoeff1 * _rkt.fW;
		return _ret;
	}
	else
	{
		_ret.fX = (1.0f - _T) * _Q1->fX + _T * _rkt.fX;
		_ret.fY = (1.0f - _T) * _Q1->fY + _T * _rkt.fY;
		_ret.fZ = (1.0f - _T) * _Q1->fZ + _T * _rkt.fZ;
		_ret.fW = (1.0f - _T) * _Q1->fW + _T * _rkt.fW;
		return _ret;
	}
}
BLVec3
blQuatTransform(IN BLQuaternion* _Q, IN BLVec3* _Vec)
{
	BLVec3 _ret , _uv , _uuv , _qvec;
	_qvec.fX = _Q->fX;
	_qvec.fY = _Q->fY;
	_qvec.fZ = _Q->fZ;
	_uv.fX = _qvec.fY * _Vec->fZ - _qvec.fZ * _Vec->fY;
	_uv.fY = _qvec.fZ * _Vec->fX - _qvec.fX * _Vec->fZ;
	_uv.fZ = _qvec.fX * _Vec->fY - _qvec.fY * _Vec->fX;
	_uuv.fX = _qvec.fY * _uv.fZ - _qvec.fZ * _uv.fY;
	_uuv.fY = _qvec.fZ * _uv.fX - _qvec.fX * _uv.fZ;
	_uuv.fZ = _qvec.fX * _uv.fY - _qvec.fY * _uv.fX;
	_uv.fX *= (2.0f * _Q->fW);
	_uv.fY *= (2.0f * _Q->fW);
	_uv.fZ *= (2.0f * _Q->fW);
	_uuv.fX *= 2.0f;
	_uuv.fY *= 2.0f;
	_uuv.fZ *= 2.0f;
	_ret.fX = _Vec->fX + _uv.fX + _uuv.fX;
	_ret.fY = _Vec->fY + _uv.fY + _uuv.fY;
	_ret.fZ = _Vec->fZ + _uv.fZ + _uuv.fZ;
	return _ret;
}
BLBool
blMatApproximate(IN BLMatrix* _M1, IN BLMatrix* _M2)
{
	for (BLU32 _idx = 0; _idx < 16; ++_idx)
		if (fabs(_M1->fEle[_idx] - _M1->fEle[_idx]) > 1e-6)
			return FALSE;
	return TRUE;
}
BLMatrix
blMatFromTransform(IN BLVec3* _Trans, IN BLVec3* _Scale, IN BLQuaternion* _Rotate)
{
	BLMatrix _ret;
	BLF32 _tx = _Rotate->fX + _Rotate->fX;
	BLF32 _ty = _Rotate->fY + _Rotate->fY;
	BLF32 _tz = _Rotate->fZ + _Rotate->fZ;
	BLF32 _twx = _tx * _Rotate->fW;
	BLF32 _twy = _ty * _Rotate->fW;
	BLF32 _twz = _tz * _Rotate->fW;
	BLF32 _txx = _tx * _Rotate->fX;
	BLF32 _txy = _ty * _Rotate->fX;
	BLF32 _txz = _tz * _Rotate->fX;
	BLF32 _tyy = _ty * _Rotate->fY;
	BLF32 _tyz = _tz * _Rotate->fY;
	BLF32 _tzz = _tz * _Rotate->fZ;
	_ret.fData[0][0] = _Scale->fX * (1.0f - (_tyy + _tzz));
	_ret.fData[1][0] = _Scale->fY * (_txy - _twz);
	_ret.fData[2][0] = _Scale->fZ * (_txz + _twy);
	_ret.fData[3][0] = _Trans->fX;
	_ret.fData[0][1] = _Scale->fX * (_txy + _twz);
	_ret.fData[1][1] = _Scale->fY * (1.0f - (_txx + _tzz));
	_ret.fData[2][1] = _Scale->fZ * (_tyz - _twx);
	_ret.fData[3][1] = _Trans->fY;
	_ret.fData[0][2] = _Scale->fX * (_txz - _twy);
	_ret.fData[1][2] = _Scale->fY * (_tyz + _twx);
	_ret.fData[2][2] = _Scale->fZ * (1.0f - (_txx + _tyy));
	_ret.fData[3][2] = _Trans->fZ;
	_ret.fData[0][3] = 0.f;
	_ret.fData[1][3] = 0.f;
	_ret.fData[2][3] = 0.f;
	_ret.fData[3][3] = 1.f;
	return _ret;
}
BLMatrix
blMatMultiply(IN BLMatrix* _M1, IN BLMatrix* _M2)
{
	BLMatrix _ret;
	_ret.fData[0][0] = _M1->fData[0][0] * _M2->fData[0][0] + _M1->fData[1][0] * _M2->fData[0][1] + _M1->fData[2][0] * _M2->fData[0][2] + _M1->fData[3][0] * _M2->fData[0][3];
	_ret.fData[1][0] = _M1->fData[0][0] * _M2->fData[1][0] + _M1->fData[1][0] * _M2->fData[1][1] + _M1->fData[2][0] * _M2->fData[1][2] + _M1->fData[3][0] * _M2->fData[1][3];
	_ret.fData[2][0] = _M1->fData[0][0] * _M2->fData[2][0] + _M1->fData[1][0] * _M2->fData[2][1] + _M1->fData[2][0] * _M2->fData[2][2] + _M1->fData[3][0] * _M2->fData[2][3];
	_ret.fData[3][0] = _M1->fData[0][0] * _M2->fData[3][0] + _M1->fData[1][0] * _M2->fData[3][1] + _M1->fData[2][0] * _M2->fData[3][2] + _M1->fData[3][0] * _M2->fData[3][3];
	_ret.fData[0][1] = _M1->fData[0][1] * _M2->fData[0][0] + _M1->fData[1][1] * _M2->fData[0][1] + _M1->fData[2][1] * _M2->fData[0][2] + _M1->fData[3][1] * _M2->fData[0][3];
	_ret.fData[1][1] = _M1->fData[0][1] * _M2->fData[1][0] + _M1->fData[1][1] * _M2->fData[1][1] + _M1->fData[2][1] * _M2->fData[1][2] + _M1->fData[3][1] * _M2->fData[1][3];
	_ret.fData[2][1] = _M1->fData[0][1] * _M2->fData[2][0] + _M1->fData[1][1] * _M2->fData[2][1] + _M1->fData[2][1] * _M2->fData[2][2] + _M1->fData[3][1] * _M2->fData[2][3];
	_ret.fData[3][1] = _M1->fData[0][1] * _M2->fData[3][0] + _M1->fData[1][1] * _M2->fData[3][1] + _M1->fData[2][1] * _M2->fData[3][2] + _M1->fData[3][1] * _M2->fData[3][3];
	_ret.fData[0][2] = _M1->fData[0][2] * _M2->fData[0][0] + _M1->fData[1][2] * _M2->fData[0][1] + _M1->fData[2][2] * _M2->fData[0][2] + _M1->fData[3][2] * _M2->fData[0][3];
	_ret.fData[1][2] = _M1->fData[0][2] * _M2->fData[1][0] + _M1->fData[1][2] * _M2->fData[1][1] + _M1->fData[2][2] * _M2->fData[1][2] + _M1->fData[3][2] * _M2->fData[1][3];
	_ret.fData[2][2] = _M1->fData[0][2] * _M2->fData[2][0] + _M1->fData[1][2] * _M2->fData[2][1] + _M1->fData[2][2] * _M2->fData[2][2] + _M1->fData[3][2] * _M2->fData[2][3];
	_ret.fData[3][2] = _M1->fData[0][2] * _M2->fData[3][0] + _M1->fData[1][2] * _M2->fData[3][1] + _M1->fData[2][2] * _M2->fData[3][2] + _M1->fData[3][2] * _M2->fData[3][3];
	_ret.fData[0][3] = _M1->fData[0][3] * _M2->fData[0][0] + _M1->fData[1][3] * _M2->fData[0][1] + _M1->fData[2][3] * _M2->fData[0][2] + _M1->fData[3][3] * _M2->fData[0][3];
	_ret.fData[1][3] = _M1->fData[0][3] * _M2->fData[1][0] + _M1->fData[1][3] * _M2->fData[1][1] + _M1->fData[2][3] * _M2->fData[1][2] + _M1->fData[3][3] * _M2->fData[1][3];
	_ret.fData[2][3] = _M1->fData[0][3] * _M2->fData[2][0] + _M1->fData[1][3] * _M2->fData[2][1] + _M1->fData[2][3] * _M2->fData[2][2] + _M1->fData[3][3] * _M2->fData[2][3];
	_ret.fData[3][3] = _M1->fData[0][3] * _M2->fData[3][0] + _M1->fData[1][3] * _M2->fData[3][1] + _M1->fData[2][3] * _M2->fData[3][2] + _M1->fData[3][3] * _M2->fData[3][3];
	return _ret;
}
BLVoid
blMatDecompose(IN BLMatrix* _Mat, OUT BLVec3* _Trans, OUT BLVec3* _Scale, OUT BLQuaternion* _Rotate)
{
	BLF32 _invlen, _dot, _det, _trace, _root;
	BLMatrix _matq;
	_Trans->fX = _Mat->fData[3][0];
	_Trans->fY = _Mat->fData[3][1];
	_Trans->fZ = _Mat->fData[3][2];
	_invlen = _Mat->fData[0][0] * _Mat->fData[0][0] + _Mat->fData[0][1] * _Mat->fData[0][1] + _Mat->fData[0][2] * _Mat->fData[0][2];
	if (fabs(_invlen) > 1e-6)
		_invlen = 1.f / sqrtf(_invlen);
	_matq.fData[0][0] = _Mat->fData[0][0] * _invlen;
	_matq.fData[0][1] = _Mat->fData[0][1] * _invlen;
	_matq.fData[0][2] = _Mat->fData[0][2] * _invlen;
	_dot = _matq.fData[0][0] * _Mat->fData[1][0] + _matq.fData[0][1] * _Mat->fData[1][1] + _matq.fData[0][2] * _Mat->fData[1][2];
	_matq.fData[1][0] = _Mat->fData[1][0] - _dot * _matq.fData[0][0];
	_matq.fData[1][1] = _Mat->fData[1][1] - _dot * _matq.fData[0][1];
	_matq.fData[1][2] = _Mat->fData[1][2] - _dot * _matq.fData[0][2];
	_invlen = _matq.fData[1][0] * _matq.fData[1][0] + _matq.fData[1][1] * _matq.fData[1][1] + _matq.fData[1][2] * _matq.fData[1][2];
	if (fabs(_invlen) > 1e-6)
		_invlen = 1.f / sqrtf(_invlen);
	_matq.fData[1][0] *= _invlen;
	_matq.fData[1][1] *= _invlen;
	_matq.fData[1][2] *= _invlen;
	_dot = _matq.fData[0][0] * _Mat->fData[2][0] + _matq.fData[0][1] * _Mat->fData[2][1] + _matq.fData[0][2] * _Mat->fData[2][2];
	_matq.fData[2][0] = _Mat->fData[2][0] - _dot * _matq.fData[0][0];
	_matq.fData[2][1] = _Mat->fData[2][1] - _dot * _matq.fData[0][1];
	_matq.fData[2][2] = _Mat->fData[2][2] - _dot * _matq.fData[0][2];
	_dot = _matq.fData[1][0] * _Mat->fData[2][0] + _matq.fData[1][1] * _Mat->fData[2][1] + _matq.fData[1][2] * _Mat->fData[2][2];
	_matq.fData[2][0] -= _dot * _matq.fData[1][0];
	_matq.fData[2][1] -= _dot * _matq.fData[1][1];
	_matq.fData[2][2] -= _dot * _matq.fData[1][2];
	_invlen = _matq.fData[2][0] * _matq.fData[2][0] + _matq.fData[2][1] * _matq.fData[2][1] + _matq.fData[2][2] * _matq.fData[2][2];
	if (fabs(_invlen) > 1e-6)
		_invlen = 1.f / sqrtf(_invlen);
	_matq.fData[2][0] *= _invlen;
	_matq.fData[2][1] *= _invlen;
	_matq.fData[2][2] *= _invlen;
	_det = _matq.fData[0][0] * _matq.fData[1][1] * _matq.fData[2][2] + _matq.fData[1][0] * _matq.fData[2][1] * _matq.fData[0][2] +
		_matq.fData[2][0] * _matq.fData[0][1] * _matq.fData[1][2] - _matq.fData[2][0] * _matq.fData[1][1] * _matq.fData[0][2] -
		_matq.fData[1][0] * _matq.fData[0][1] * _matq.fData[2][2] - _matq.fData[0][0] * _matq.fData[2][1] * _matq.fData[1][2];
	if (_det < 0.f)
	{
		BLU32 _row, _col;
		for (_row = 0; _row < 3; _row++)
		for (_col = 0; _col < 3; _col++)
			_matq.fData[_col][_row] = -_matq.fData[_col][_row];
	}
	_Scale->fX = _matq.fData[0][0] * _Mat->fData[0][0] + _matq.fData[0][1] * _Mat->fData[0][1] + _matq.fData[0][2] * _Mat->fData[0][2];
	_Scale->fY = _matq.fData[1][0] * _Mat->fData[1][0] + _matq.fData[1][1] * _Mat->fData[1][1] + _matq.fData[1][2] * _Mat->fData[1][2];
	_Scale->fZ = _matq.fData[2][0] * _Mat->fData[2][0] + _matq.fData[2][1] * _Mat->fData[2][1] + _matq.fData[2][2] * _Mat->fData[2][2];
	_trace = _matq.fData[0][0] + _matq.fData[1][1] + _matq.fData[2][2];
	if (_trace > 0.0)
	{
		_root = sqrtf(_trace + 1.0f);
		_Rotate->fW = 0.5f * _root;
		_root = 0.5f / _root;
		_Rotate->fX = (_matq.fData[1][2] - _matq.fData[2][1]) * _root;
		_Rotate->fY = (_matq.fData[2][0] - _matq.fData[0][2]) * _root;
		_Rotate->fZ = (_matq.fData[0][1] - _matq.fData[1][0]) * _root;
	}
	else
	{
		BLF32* _apkquat[3];
		BLU32 _inext[3] = { 1, 2, 0 };
		BLU32 _i = 0 , _j , _k;
		if (_matq.fData[1][1] > _matq.fData[0][0])
			_i = 1;
		if (_matq.fData[2][2] > _matq.fData[_i][_i])
			_i = 2;
		_j = _inext[_i];
		_k = _inext[_j];
		_root = sqrtf(_matq.fData[_i][_i] - _matq.fData[_j][_j] - _matq.fData[_k][_k] + 1.0f);
		_apkquat[0] = &_Rotate->fX;
		_apkquat[1] = &_Rotate->fY;
		_apkquat[2] = &_Rotate->fZ;
		*_apkquat[_i] = 0.5f * _root;
		_root = 0.5f / _root;
		_Rotate->fW = (_matq.fData[_j][_k] - _matq.fData[_k][_j]) * _root;
		*_apkquat[_j] = (_matq.fData[_i][_j] + _matq.fData[_j][_i]) * _root;
		*_apkquat[_k] = (_matq.fData[_i][_k] + _matq.fData[_k][_i]) * _root;
	}
}
BLVoid
blMatTranspose(INOUT BLMatrix* _Mat)
{
	BLF32 _m01 = _Mat->fData[0][1];
	BLF32 _m02 = _Mat->fData[0][2];
	BLF32 _m03 = _Mat->fData[0][3];
	BLF32 _m10 = _Mat->fData[1][0];
	BLF32 _m12 = _Mat->fData[1][2];
	BLF32 _m13 = _Mat->fData[1][3];
	BLF32 _m20 = _Mat->fData[2][0];
	BLF32 _m21 = _Mat->fData[2][1];
	BLF32 _m23 = _Mat->fData[2][3];
	BLF32 _m30 = _Mat->fData[3][0];
	BLF32 _m31 = _Mat->fData[3][1];
	BLF32 _m32 = _Mat->fData[3][2];
	_Mat->fData[1][0] = _m01;
	_Mat->fData[2][0] = _m02;
	_Mat->fData[3][0] = _m03;
	_Mat->fData[0][1] = _m10;
	_Mat->fData[2][1] = _m12;
	_Mat->fData[3][1] = _m13;
	_Mat->fData[0][2] = _m20;
	_Mat->fData[1][2] = _m21;
	_Mat->fData[3][2] = _m23;
	_Mat->fData[0][3] = _m30;
	_Mat->fData[1][3] = _m31;
	_Mat->fData[2][3] = _m32;
}
BLVec3
blMatTransform(IN BLMatrix* _Mat, IN BLVec3* _Vec)
{
	BLVec3 _ret;
	BLF32 _x = _Vec->fX;
	BLF32 _y = _Vec->fY;
	BLF32 _z = _Vec->fZ;
	BLF32 _finvw = 1.0f / (_Mat->fData[0][3] * _x + _Mat->fData[1][3] * _y + _Mat->fData[2][3] * _z + _Mat->fData[3][3]);
	_ret.fX = (_Mat->fData[0][0] * _x + _Mat->fData[1][0] * _y + _Mat->fData[2][0] * _z + _Mat->fData[3][0]) * _finvw;
	_ret.fY = (_Mat->fData[0][1] * _x + _Mat->fData[1][1] * _y + _Mat->fData[2][1] * _z + _Mat->fData[3][1]) * _finvw;
	_ret.fZ = (_Mat->fData[0][2] * _x + _Mat->fData[1][2] * _y + _Mat->fData[2][2] * _z + _Mat->fData[3][2]) * _finvw;
	return _ret;
}
BLVoid
blMatPerspectiveL(OUT BLMatrix* _Mat, IN BLF32 _Fov, IN BLF32 _Aspect, IN BLF32 _Near, IN BLF32 _Far)
{
	BLF32 _h = 1.f / tanf(_Fov*0.5f);
	_Mat->fData[0][0] = _h / _Aspect;
	_Mat->fData[0][1] = 0.f;
	_Mat->fData[0][2] = 0.f;
	_Mat->fData[0][3] = 0.f;
	_Mat->fData[1][0] = 0.f;
	_Mat->fData[1][1] = _h;
	_Mat->fData[1][2] = 0.f;
	_Mat->fData[1][3] = 0.f;
	_Mat->fData[2][0] = 0.f;
	_Mat->fData[2][1] = 0.f;
	_Mat->fData[2][2] = _Far / (_Far - _Near);
	_Mat->fData[2][3] = 1.f;
	_Mat->fData[3][0] = 0.f;
	_Mat->fData[3][1] = 0.f;
	_Mat->fData[3][2] = -_Near*_Far / (_Far - _Near);
	_Mat->fData[3][3] = 0.f;
}
BLVoid
blMatLookatL(OUT BLMatrix* _Mat, IN BLVec3* _Eye, IN BLVec3* _Focus, IN BLVec3* _Up)
{
	BLF32 _len;
	BLVec3 _zaxis, _xaxis, _yaxis;
	_zaxis.fX = _Focus->fX - _Eye->fX;
	_zaxis.fY = _Focus->fY - _Eye->fY;
	_zaxis.fZ = _Focus->fZ - _Eye->fZ;
	_len = sqrtf(_zaxis.fX * _zaxis.fX + _zaxis.fY * _zaxis.fY + _zaxis.fZ * _zaxis.fZ);
	if (_len > 0.0f)
	{
		BLF32 _invlen = 1.0f / _len;
		_zaxis.fX *= _invlen;
		_zaxis.fY *= _invlen;
		_zaxis.fZ *= _invlen;
	}
	_xaxis.fX = _Up->fY * _zaxis.fZ - _Up->fZ * _zaxis.fY;
	_xaxis.fY = _Up->fZ * _zaxis.fX - _Up->fX * _zaxis.fZ;
	_xaxis.fZ = _Up->fX * _zaxis.fY - _Up->fY * _zaxis.fX;
	_len = sqrtf(_xaxis.fX * _xaxis.fX + _xaxis.fY * _xaxis.fY + _xaxis.fZ * _xaxis.fZ);
	if (_len > 0.0f)
	{
		BLF32 _invlen = 1.0f / _len;
		_xaxis.fX *= _invlen;
		_xaxis.fY *= _invlen;
		_xaxis.fZ *= _invlen;
	}
	_yaxis.fX = _zaxis.fY * _xaxis.fZ - _zaxis.fZ * _xaxis.fY;
	_yaxis.fY = _zaxis.fZ * _xaxis.fX - _zaxis.fX * _xaxis.fZ;
	_yaxis.fZ = _zaxis.fX * _xaxis.fY - _zaxis.fY * _xaxis.fX;
	_Mat->fData[0][0] = _xaxis.fX;
	_Mat->fData[0][1] = _yaxis.fX;
	_Mat->fData[0][2] = _zaxis.fX;
	_Mat->fData[0][3] = 0.f;
	_Mat->fData[1][0] = _xaxis.fY;
	_Mat->fData[1][1] = _yaxis.fY;
	_Mat->fData[1][2] = _zaxis.fY;
	_Mat->fData[1][3] = 0.f;
	_Mat->fData[2][0] = _xaxis.fZ;
	_Mat->fData[2][1] = _yaxis.fZ;
	_Mat->fData[2][2] = _zaxis.fZ;
	_Mat->fData[2][3] = 0.f;
	_Mat->fData[3][0] = -_xaxis.fX * _Eye->fX + -_xaxis.fY * _Eye->fY + -_xaxis.fZ * _Eye->fZ;
	_Mat->fData[3][1] = -_yaxis.fX * _Eye->fX + -_yaxis.fY * _Eye->fY + -_yaxis.fZ * _Eye->fZ;
	_Mat->fData[3][2] = -_zaxis.fX * _Eye->fX + -_zaxis.fY * _Eye->fY + -_zaxis.fZ * _Eye->fZ;
	_Mat->fData[3][3] = 1.f;
}
BLBool
blPlaneApproximate(IN BLPlane* _P1, IN BLPlane* _P2)
{
	if (fabs(_P1->sNormal.fX - _P2->sNormal.fX) > 1e-6)
		return FALSE;
	if (fabs(_P1->sNormal.fY - _P2->sNormal.fY) > 1e-6)
		return FALSE;
	if (fabs(_P1->sNormal.fZ - _P2->sNormal.fZ) > 1e-6)
		return FALSE;
	if (fabs(_P1->fDistance - _P2->fDistance) > 1e-6)
		return FALSE;
	return TRUE;
}
BLPlane
blPlaneFrom3P(IN BLVec3* _P1, IN BLVec3* _P2, IN BLVec3* _P3)
{
	BLF32 _len;
	BLPlane _ret;
	BLVec3 _kedge1, _kedge2;
	_kedge1.fX = _P2->fX - _P1->fX;
	_kedge1.fY = _P2->fY - _P1->fY;
	_kedge1.fZ = _P2->fZ - _P1->fZ;
	_kedge2.fX = _P3->fX - _P1->fX;
	_kedge2.fY = _P3->fY - _P1->fY;
	_kedge2.fZ = _P3->fZ - _P1->fZ;
	_ret.sNormal.fX = _kedge1.fY * _kedge2.fZ - _kedge1.fZ * _kedge2.fY;
	_ret.sNormal.fY = _kedge1.fZ * _kedge2.fX - _kedge1.fX * _kedge2.fZ;
	_ret.sNormal.fZ = _kedge1.fX * _kedge2.fY - _kedge1.fY * _kedge2.fX;
	_len = sqrtf(_ret.sNormal.fX * _ret.sNormal.fX + _ret.sNormal.fY * _ret.sNormal.fY + _ret.sNormal.fZ * _ret.sNormal.fZ);
	if (_len > 0.0f)
	{
		BLF32 _invlen = 1.0f / _len;
		_ret.sNormal.fX *= _invlen;
		_ret.sNormal.fY *= _invlen;
		_ret.sNormal.fZ *= _invlen;
	}
	_ret.fDistance = -_ret.sNormal.fX * _P1->fX + -_ret.sNormal.fY * _P1->fY + -_ret.sNormal.fZ * _P1->fZ;
	return _ret;
}
BLPlane
blPlaneFromPN(IN BLVec3* _Pt, IN BLVec3* _Nor)
{
	BLPlane _ret;
	_ret.sNormal = *_Nor;
	_ret.fDistance = -_ret.sNormal.fX * _Pt->fX + -_ret.sNormal.fY * _Pt->fY + -_ret.sNormal.fZ * _Pt->fZ;
	return _ret;
}
BLBool
blBoxApproximate(IN BLBox* _B1, IN BLBox* _B2)
{
	if (fabs(_B1->sMinPt.fX - _B2->sMinPt.fX) > 1e-6)
		return FALSE;
	if (fabs(_B1->sMinPt.fY - _B2->sMinPt.fY) > 1e-6)
		return FALSE;
	if (fabs(_B1->sMinPt.fZ - _B2->sMinPt.fZ) > 1e-6)
		return FALSE;
	if (fabs(_B1->sMaxPt.fX - _B2->sMaxPt.fX) > 1e-6)
		return FALSE;
	if (fabs(_B1->sMaxPt.fY - _B2->sMaxPt.fY) > 1e-6)
		return FALSE;
	if (fabs(_B1->sMaxPt.fZ - _B2->sMaxPt.fZ) > 1e-6)
		return FALSE;
	return TRUE;
}
BLVoid
blBoxMerge(INOUT BLBox* _Box, IN BLVec3* _Pt)
{
	if (_Pt->fX > _Box->sMaxPt.fX) _Box->sMaxPt.fX = _Pt->fX;
	if (_Pt->fY > _Box->sMaxPt.fY) _Box->sMaxPt.fY = _Pt->fY;
	if (_Pt->fZ > _Box->sMaxPt.fZ) _Box->sMaxPt.fZ = _Pt->fZ;
	if (_Pt->fX < _Box->sMinPt.fX) _Box->sMinPt.fX = _Pt->fX;
	if (_Pt->fY < _Box->sMinPt.fY) _Box->sMinPt.fY = _Pt->fY;
	if (_Pt->fZ < _Box->sMinPt.fZ) _Box->sMinPt.fZ = _Pt->fZ;
}
BLBool
blBoxContains(IN BLBox* _Box, IN BLVec3* _Pt)
{
	if (_Pt->fX > _Box->sMaxPt.fX || _Pt->fX < _Box->sMinPt.fX) return FALSE;
	if (_Pt->fY > _Box->sMaxPt.fY || _Pt->fY < _Box->sMinPt.fY) return FALSE;
	if (_Pt->fZ > _Box->sMaxPt.fZ || _Pt->fZ < _Box->sMinPt.fZ) return FALSE;
	return TRUE;
}
BLBool
blBoxIntersects(IN BLBox* _B1, IN BLBox* _B2)
{
	if (_B1->sMinPt.fX > _B2->sMinPt.fX &&
		_B1->sMinPt.fX < _B2->sMaxPt.fX &&
		_B1->sMinPt.fY > _B2->sMinPt.fY &&
		_B1->sMinPt.fY < _B2->sMaxPt.fY &&
		_B1->sMinPt.fZ > _B2->sMinPt.fZ &&
		_B1->sMinPt.fZ < _B2->sMaxPt.fZ)
		return TRUE;
	if (_B1->sMaxPt.fX > _B2->sMinPt.fX &&
		_B1->sMaxPt.fX < _B2->sMaxPt.fX &&
		_B1->sMaxPt.fY > _B2->sMinPt.fY &&
		_B1->sMaxPt.fY < _B2->sMaxPt.fY &&
		_B1->sMaxPt.fZ > _B2->sMinPt.fZ &&
		_B1->sMaxPt.fZ < _B2->sMaxPt.fZ)
		return TRUE;
	if (_B2->sMinPt.fX > _B1->sMinPt.fX &&
		_B2->sMinPt.fX < _B1->sMaxPt.fX &&
		_B2->sMinPt.fY > _B1->sMinPt.fY &&
		_B2->sMinPt.fY < _B1->sMaxPt.fY &&
		_B2->sMinPt.fZ > _B1->sMinPt.fZ &&
		_B2->sMinPt.fZ < _B1->sMaxPt.fZ)
		return TRUE;
	if (_B2->sMaxPt.fX > _B1->sMinPt.fX &&
		_B2->sMaxPt.fX < _B1->sMaxPt.fX &&
		_B2->sMaxPt.fY > _B1->sMinPt.fY &&
		_B2->sMaxPt.fY < _B1->sMaxPt.fY &&
		_B2->sMaxPt.fZ > _B1->sMinPt.fZ &&
		_B2->sMaxPt.fZ < _B1->sMaxPt.fZ)
		return TRUE;
	return FALSE;
}
BLBool
blSphereContains(IN BLSphere* _Sp, IN BLVec3* _Pt)
{
	BLF32 _x, _y, _z;
	_x = _Sp->sPt.fX - _Pt->fX;
	_y = _Sp->sPt.fY - _Pt->fY;
	_z = _Sp->sPt.fZ - _Pt->fZ;
	if (_x * _x + _y * _y + _z * _z > _Sp->fR * _Sp->fR)
		return FALSE;
	else
		return TRUE;
}
BLBool
blSphereIntersects(IN BLSphere* _S1, IN BLSphere* _S2)
{
	BLF32 _x, _y, _z;
	_x = _S1->sPt.fX - _S2->sPt.fX;
	_y = _S1->sPt.fY - _S2->sPt.fY;
	_z = _S1->sPt.fZ - _S2->sPt.fZ;
	if (_x * _x + _y * _y + _z * _z > (_S1->fR + _S2->fR) * (_S1->fR + _S2->fR))
		return FALSE;
	else
		return TRUE;
}
BLBool
blBoxSphereIntersects(IN BLBox* _Box, IN BLSphere* _Sp)
{
	BLF32 _s, _d = 0;
	if (_Sp->sPt.fX < _Box->sMinPt.fX)
	{
		_s = _Sp->sPt.fX - _Box->sMinPt.fX;
		_d += _s * _s;
	}
	else if (_Sp->sPt.fX > _Box->sMaxPt.fX)
	{
		_s = _Sp->sPt.fX - _Box->sMaxPt.fX;
		_d += _s * _s;
	}
	if (_Sp->sPt.fY < _Box->sMinPt.fY)
	{
		_s = _Sp->sPt.fY - _Box->sMinPt.fY;
		_d += _s * _s;
	}
	else if (_Sp->sPt.fY > _Box->sMaxPt.fY)
	{
		_s = _Sp->sPt.fY - _Box->sMaxPt.fY;
		_d += _s * _s;
	}
	if (_Sp->sPt.fZ < _Box->sMinPt.fZ)
	{
		_s = _Sp->sPt.fZ - _Box->sMinPt.fZ;
		_d += _s * _s;
	}
	else if (_Sp->sPt.fZ > _Box->sMaxPt.fZ)
	{
		_s = _Sp->sPt.fZ - _Box->sMaxPt.fZ;
		_d += _s * _s;
	}
	return _d <= _Sp->fR * _Sp->fR;
}
BLBool
blFrustumCullBox(IN BLFrustum* _Fru, IN BLBox* _Box)
{
	BLU32 _idx;
	BLVec3 _centre;
	BLVec3 _halfsz;
	_centre.fX = 0.5f * (_Box->sMinPt.fX + _Box->sMaxPt.fX);
	_centre.fY = 0.5f * (_Box->sMinPt.fY + _Box->sMaxPt.fY);
	_centre.fZ = 0.5f * (_Box->sMinPt.fZ + _Box->sMaxPt.fZ);
	_halfsz.fX = 0.5f * (_Box->sMaxPt.fX - _Box->sMinPt.fX);
	_halfsz.fY = 0.5f * (_Box->sMaxPt.fY - _Box->sMinPt.fY);
	_halfsz.fZ = 0.5f * (_Box->sMaxPt.fZ - _Box->sMinPt.fZ);
	for (_idx = 0; _idx < 6; ++_idx)
	{
		BLVec3 _no = _Fru->sPlanes[_idx].sNormal;
		BLF32 _dist = _no.fX * _centre.fX + _no.fY * _centre.fY + _no.fZ * _centre.fZ + _Fru->sPlanes[_idx].fDistance;
		BLF32 _maxabsdist = (BLF32)fabs(_no.fX * _halfsz.fX) + (BLF32)fabs(_no.fY * _halfsz.fY) + (BLF32)fabs(_no.fZ * _halfsz.fZ);
		if (_dist < -_maxabsdist)
			return FALSE;
	}
	return TRUE;
}
BLBool
blFrustumCullSphere(IN BLFrustum* _Fru, IN BLSphere* _Sp)
{
	BLU32 _idx;
	for (_idx = 0; _idx < 6; ++_idx)
	{
		BLVec3 _no = _Fru->sPlanes[_idx].sNormal;
		BLF32 _dist = _no.fX * _Sp->sPt.fX + _no.fY * _Sp->sPt.fY + _no.fZ * _Sp->sPt.fZ + _Fru->sPlanes[_idx].fDistance;
		if (_dist < -_Sp->fR)
			return FALSE;
	}
	return TRUE;
}
