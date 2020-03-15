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
#include "spline.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
Spline* InitSpine()
{
	Spline* sp = (Spline*)malloc(sizeof(Spline));
	sp->curve = InitCurve();
	sp->curve->child = sp;
	sp->curve->getPointFunc = _getPoint;
	sp->splinenode.size = 0;
	sp->splinenode.cap = 20;
	sp->splinenode._Head = (Vector2*)malloc(sp->splinenode.cap * sizeof(Vector2));
	return sp;
}
void freeSpine(Spline* sp)
{
	free(sp->splinenode._Head);
	freeCruve(sp->curve);
	free(sp);
}
Vector2 _getPoint(Spline* sp, float t)
{
	Vector2* points = sp->splinenode._Head;
	float p = (sp->splinenode.size - 1) * t;

	int intPoint = (int)floor(p);
	float weight = p - intPoint;

	Vector2 p0 = points[intPoint == 0 ? intPoint : intPoint - 1];
	Vector2 p1 = points[intPoint];
	Vector2 p2 = points[intPoint > sp->splinenode.size - 2 ? sp->splinenode.size - 1 : intPoint + 1];
	Vector2 p3 = points[intPoint > sp->splinenode.size - 3 ? sp->splinenode.size - 1 : intPoint + 2];
	Vector2 ret;
	ret.x = CatmullRom(weight, p0.x, p1.x, p2.x, p3.x);
	ret.y = CatmullRom(weight, p0.y, p1.y, p2.y, p3.y);
	return ret;
}
void pushSplineArray(SplineNode* arr, Vector2 val)
{
	if (arr->size >= arr->cap + 1)
	{
		arr->cap *= 2;
		arr->_Head = (Vector2*)realloc(arr->_Head, arr->cap * sizeof(Vector2));
	}
	memcpy(arr->_Head + arr->size , &val, sizeof(Vector2));
	arr->size++;
}
float CatmullRom(float t, float p0, float p1, float p2, float p3)
{
	float v0 = (p2 - p0) * 0.5f;
	float v1 = (p3 - p1) * 0.5f;
	float t2 = t * t;
	float t3 = t * t2;

	return (2 * p1 - 2 * p2 + v0 + v1) * t3 + (-3 * p1 + 3 * p2 - 2 * v0 - v1) * t2 + v0 * t + p1;
}