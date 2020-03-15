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
#include "bezier.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
CubicBezier* InitCubicBezier()
{
	CubicBezier* sp = (CubicBezier*)malloc(sizeof(CubicBezier));
	sp->curve = InitCurve();
	sp->curve->child = sp;
	sp->curve->getPointFunc = _getPoint2;
	return sp;
}
void freeCubicBezier(CubicBezier* cb)
{
	freeCruve(cb->curve);
	free(cb);
}
Vector2 _getPoint2(CubicBezier* cb, float t)
{
	Vector2 ret;
	ret.x = cubicBezier(t, cb->v0.x, cb->v1.x, cb->v2.x, cb->v3.x);
	ret.y = cubicBezier(t, cb->v0.y, cb->v1.y, cb->v2.y, cb->v3.y);
	return ret;
}
float CubicBezierP0(float t, float p) {

	float k = 1 - t;
	return k * k * k * p;
}

float CubicBezierP1(float t, float p) {

	float k = 1 - t;
	return 3 * k * k * t * p;
}

float CubicBezierP2(float t, float p) {

	return 3 * (1 - t) * t * t * p;
}

float CubicBezierP3(float t, float p) {

	return t * t * t * p;
}

float cubicBezier(float t, float p0, float p1, float p2, float p3) {

	return CubicBezierP0(t, p0) + CubicBezierP1(t, p1) + CubicBezierP2(t, p2) +
		CubicBezierP3(t, p3);
}