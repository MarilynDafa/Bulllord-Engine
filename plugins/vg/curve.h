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
#ifndef __curve_h_
#define __curve_h_
#ifdef __cplusplus
extern "C" {
#endif
	/**
		This module is ported from threepp
		Curve
	*/
	#define arcLengthDivisions 200
	typedef struct _Vector2{
		float x, y;
	}Vector2;
	typedef Vector2(*getPoint)(void*, float);
	typedef struct _ArrayNode {
		float* _Head;
		int size;
		int cap;
	}ArrayNode;
	typedef struct _Curve
	{
		ArrayNode _cacheArcLengths;
		int _needsUpdate;
		getPoint getPointFunc;
		void* child;
	} Curve;
	Curve* InitCurve();
	freeCruve(Curve* cur);
	void Vector2normalize(Vector2* vec);
	float distanceTo(Vector2* v1, Vector2* v2);
	void pushArray(ArrayNode* arr, float val);
	void freeArray(ArrayNode* arr);
	// Get total curve arc length
	float getLength(Curve* curve);
	// Get list of cumulative segment lengths
	ArrayNode getLengths(Curve* cruve, unsigned divisions);
	// Given u ( 0 .. 1 ), get a t to find p. This gives you points which are equidistant
	float getUtoTmapping(Curve* curve, float u, float distance);
	// Get point at relative position in curve according to arc length
	// - u [0 .. 1]
	Vector2 getPointAt(Curve* curve, float u);
	void updateArcLengths(Curve* curve);
	Vector2 getTangent(Curve* curve, float t); 
	void getPoints(Curve* curve, unsigned int divisions, float** x, float** y);
#ifdef __cplusplus
}
#endif
#endif/* __curve_h_ */