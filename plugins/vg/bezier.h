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
#ifndef __bezier_h_
#define __bezier_h_
#include "curve.h"
#ifdef __cplusplus
extern "C" {
#endif
	/**
		This module is ported from threepp
		CubicBezier
	*/
	typedef struct _CubicBezier {
		Curve* curve;
		Vector2 v0, v1, v2, v3;
	}CubicBezier;
	CubicBezier* InitCubicBezier();
	void freeCubicBezier(CubicBezier* cb);
	Vector2 _getPoint2(CubicBezier* cb, float t);
	float cubicBezier(float t, float p0, float p1, float p2, float p3);
#ifdef __cplusplus
}
#endif
#endif/* __bezier_h_ */