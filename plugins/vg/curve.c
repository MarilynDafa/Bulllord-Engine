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
#include "curve.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
Curve* InitCurve()
{
	Curve* curve = (Curve*)malloc(sizeof(Curve));
	curve->_needsUpdate = 0;
	curve->_cacheArcLengths.size = 0;
	curve->_cacheArcLengths.cap = 20;
	curve->_cacheArcLengths._Head = (float*)malloc(curve->_cacheArcLengths.cap * sizeof(float));
	return curve;
}
freeCruve(Curve* cur)
{
	free(cur->_cacheArcLengths._Head);
	free(cur);
}
void Vector2normalize(Vector2* vec)
{
	float len = sqrtf(vec->x * vec->x + vec->y * vec->y);
	if (len > 0)
	{
		vec->x /= len;
		vec->y /= len;
	}
}
float distanceTo(Vector2* v1, Vector2* v2)
{
	float dx = v1->x - v2->x, dy = v1->y - v2->y;
	float squaredDistance = dx * dx + dy * dy;
	return (float)sqrt(squaredDistance);
}
void pushArray(ArrayNode* arr, float val)
{
	if (arr->size >= arr->cap+1)
	{
		arr->cap *= 2;
		arr->_Head = (float*)realloc(arr->_Head, arr->cap * sizeof(float));
	}
	arr->_Head[arr->size] = val;
	arr->size++;
}
void freeArray(ArrayNode* arr)
{
	arr->size = 0;
	free(arr->_Head);
	arr->_Head = 0;
}
float getLength(Curve* curve)
{
	return curve->_cacheArcLengths._Head[curve->_cacheArcLengths.size-1];
}
ArrayNode getLengths(Curve* curve, unsigned divisions)
{
	if (curve->_cacheArcLengths.size == divisions + 1 && !curve->_needsUpdate) {

		return curve->_cacheArcLengths;
	}

	curve->_needsUpdate = 0;
	freeArray(&curve->_cacheArcLengths);
	Vector2 current;
	Vector2 last = curve->getPointFunc(curve->child, 0);
	float sum = 0.f;
	pushArray(&curve->_cacheArcLengths, 0.f);

	for (unsigned p = 1; p <= divisions; p++) {

		current = curve->getPointFunc(curve->child, (float)p / divisions);
		sum += distanceTo(&current, &last);
		pushArray(&curve->_cacheArcLengths, sum);
		last = current;
	}

	return curve->_cacheArcLengths;
}
float getUtoTmapping(Curve* curve, float u, float distance)
{
	float* arcLengths = curve->_cacheArcLengths._Head;

	float targetArcLength; // The targeted u distance value to get

	if (distance > 0) {
		targetArcLength = distance;
	}
	else {
		targetArcLength = u * curve->_cacheArcLengths._Head[curve->_cacheArcLengths.size - 1];
	}

	// binary search for the index with largest value smaller than target u distance
	float low = 0, high = (float)curve->_cacheArcLengths.size - 1;
	while (low <= high) {

		float i = low + (high - low) / 2; // less likely to overflow

		float comparison = arcLengths[(int)i] - targetArcLength;

		if (comparison < 0) {

			low = i + 1;
		}
		else if (comparison > 0) {

			high = i - 1;
		}
		else {

			high = i;
			break;
			// DONE
		}
	}

	float i = high;

	if (arcLengths[(int)i] == targetArcLength) {

		return i / (curve->_cacheArcLengths.size - 1);
	}

	// we could get finer grain at lengths, or use simple interpolation between two points
	float lengthBefore = arcLengths[(int)i];
	float lengthAfter = arcLengths[(int)i + 1];

	float segmentLength = lengthAfter - lengthBefore;

	// determine where we are between the 'before' and 'after' points
	float segmentFraction = (targetArcLength - lengthBefore) / segmentLength;

	// add that fractional amount to t
	return (i + segmentFraction) / (curve->_cacheArcLengths.size - 1);
}
Vector2 getPointAt(Curve* curve, float u)
{
	return curve->getPointFunc(curve->child, getUtoTmapping(curve, u, 0));
}
void updateArcLengths(Curve* curve)
{
	curve->_needsUpdate = 1;
	getLengths(curve, arcLengthDivisions);
}
Vector2 getTangent(Curve* curve, float t)
{
	float delta = 0.0001f;
	float t1 = t - delta;
	float t2 = t + delta;

	// Capping in case of danger
	if (t1 < 0) t1 = 0;
	if (t2 > 1) t2 = 1;

	Vector2 pt1 = curve->getPointFunc(curve->child, t1);
	Vector2 pt2 = curve->getPointFunc(curve->child, t2);

	Vector2 vec;
	vec.x = pt2.x - pt1.x;
	vec.y = pt2.y - pt1.y;
	Vector2normalize(&vec);
	return vec;
}
void getPoints(Curve* curve, unsigned int divisions, float** x, float** y)
{
	*x = (float*)malloc((1+ divisions)*sizeof(float));
	*y = (float*)malloc((1 + divisions) * sizeof(float));
	for (unsigned d = 0; d <= divisions; d++) {

		Vector2 v = getPointAt(curve, (float)d / divisions);
		(*x)[d] = v.x;
		(*y)[d] = v.y;
	}
}
