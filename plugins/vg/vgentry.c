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
#include "vgentry.h"
#include "ui.h"
#include "spline.h"
#include "bezier.h"
#include <math.h>
BLVoid
blVGOpenEXT(IN BLAnsi* _Version, ...)
{
}
BLVoid
blVGCloseEXT()
{
}
BLVoid 
blVGRectangleEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Rotate, IN BLBool _Fill, IN BLU32 _Color)
{
	blUIPrimitiveColor(_ID, _Color);
	blUIPrimitiveFill(_ID, _Fill);
	BLF32 _x[4] = { 0 };
	BLF32 _y[4] = { 0 };
	BLF32 _cosf = cosf(_Rotate * 3.141592653589793f / 180.f);
	BLF32 _sinf = sinf(_Rotate * 3.141592653589793f / 180.f);
	_x[0] = -_Width * 0.5f * _cosf + _Height * 0.5f * _sinf + _X;
	_y[0] = -_Width * 0.5f * _sinf - _Height * 0.5f * _cosf + _Y;
	_x[1] = _Width * 0.5f * _cosf + _Height * 0.5f * _sinf + _X;
	_y[1] = _Width * 0.5f * _sinf - _Height * 0.5f * _cosf + _Y;
	_x[2] = _Width * 0.5f * _cosf - _Height * 0.5f * _sinf + _X;
	_y[2] = _Width * 0.5f * _sinf + _Height * 0.5f * _cosf + _Y;
	_x[3] = -_Width * 0.5f * _cosf - _Height * 0.5f * _sinf + _X;
	_y[3] = -_Width * 0.5f * _sinf + _Height * 0.5f * _cosf + _Y;
	blUIPrimitivePath(_ID, _x, _y, 4);
}
BLVoid 
blVGEllipseEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _XRadius, IN BLF32 _YRadius, IN BLF32 _Rotate, IN BLBool _Fill, IN BLU32 _Color)
{
	blUIPrimitiveColor(_ID, _Color);
	blUIPrimitiveFill(_ID, _Fill);
	BLF32 _cosf = cosf(_Rotate * 3.141592653589793f / 180.f);
	BLF32 _sinf = sinf(_Rotate * 3.141592653589793f / 180.f);
	BLF32* _x = (BLF32*)alloca(120 * sizeof(BLF32));
	BLF32* _y = (BLF32*)alloca(120 * sizeof(BLF32));
	for (BLU32 _i = 0; _i < 120; ++_i) 
	{
		_x[_i] = _XRadius * cosf(_i * 3 * 3.141592653589793f / 180.f) * _cosf - _YRadius * sinf(_i * 3 * 3.141592653589793f / 180.f) * _sinf + _X;
		_y[_i] = _XRadius * cosf(_i * 3 * 3.141592653589793f / 180.f) * _sinf + _YRadius * sinf(_i * 3 * 3.141592653589793f / 180.f) * _cosf + _Y;
	}
	blUIPrimitivePath(_ID, _x, _y, 120);
}
BLVoid 
blVGArcEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _XRadius, IN BLF32 _YRadius, IN BLF32 _StartAngle, IN BLF32 _EndAngle, IN BLF32 _Rotate, IN BLU32 _Color)
{
	blUIPrimitiveColor(_ID, _Color);
	blUIPrimitiveFill(_ID, FALSE);
	blUIPrimitiveClosed(_ID, FALSE);
	BLF32 _cosf = cosf(_Rotate * 3.141592653589793f / 180.f);
	BLF32 _sinf = sinf(_Rotate * 3.141592653589793f / 180.f);
	BLU32 _div = (BLU32)(fabs(_EndAngle - _StartAngle) / 3);
	BLF32* _x = (BLF32*)alloca(_div * sizeof(BLF32));
	BLF32* _y = (BLF32*)alloca(_div * sizeof(BLF32));
	for (BLU32 _i = 0; _i < _div; ++_i) 
	{
		_x[_i] = _XRadius * cosf((_i * 3 + _StartAngle) * 3.141592653589793f / 180.f) * _cosf - _YRadius * sinf((_i * 3 + _StartAngle) * 3.141592653589793f / 180.f) * _sinf + _X;
		_y[_i] = _XRadius * cosf((_i * 3 + _StartAngle) * 3.141592653589793f / 180.f) * _sinf + _YRadius * sinf((_i * 3 + _StartAngle) * 3.141592653589793f / 180.f) * _cosf + _Y;
	}
	blUIPrimitivePath(_ID, _x, _y, _div);
}
BLVoid 
blVGSplineEXT(IN BLGuid _ID, IN BLF32* _XPath, IN BLF32* _YPath, IN BLU32 _PathNum, IN BLU32 _Divisions, IN BLU32 _Color)
{
	blUIPrimitiveColor(_ID, _Color);
	blUIPrimitiveFill(_ID, FALSE);
	blUIPrimitiveClosed(_ID, FALSE);
	Spline* _sp = InitSpine();
	for (BLU32 _i = 0; _i < _PathNum; ++_i) 
	{
		Vector2 _v;
		_v.x = _XPath[_i];
		_v.y = _YPath[_i];
		pushSplineArray(&_sp->splinenode, _v);
	}
	BLF32* _x;
	BLF32* _y;
	getPoints(_sp->curve, _Divisions, &_x, &_y);
	blUIPrimitivePath(_ID, _x, _y, _Divisions + 1);
	freeSpine(_sp);
	free(_x);
	free(_y);
}
BLVoid 
blVGBezierEXT(IN BLGuid _ID, IN BLF32 _X1, IN BLF32 _Y1, IN BLF32 _CX1, IN BLF32 _CY1, IN BLF32 _X2, IN BLF32 _Y2, IN BLF32 _CX2, IN BLF32 _CY2, IN BLU32 _Divisions, IN BLU32 _Color)
{
	blUIPrimitiveColor(_ID, _Color);
	blUIPrimitiveFill(_ID, FALSE);
	blUIPrimitiveClosed(_ID, FALSE);
	CubicBezier* _cb = InitCubicBezier();
	_cb->v0.x = _X1;
	_cb->v0.y = _Y1;
	_cb->v1.x = _CX1;
	_cb->v1.y = _CY1;
	_cb->v2.x = _CX2;
	_cb->v2.y = _CY2;
	_cb->v3.x = _X2;
	_cb->v3.y = _Y2;
	BLF32* _x;
	BLF32* _y;
	getPoints(_cb->curve, _Divisions, &_x, &_y);
	blUIPrimitivePath(_ID, _x, _y, _Divisions + 1);
	freeCubicBezier(_cb);
	free(_x);
	free(_y);
}