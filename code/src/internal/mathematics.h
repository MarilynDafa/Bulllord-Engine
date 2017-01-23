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
#ifndef __algorithm_h_
#define __algorithm_h_
#include "../../headers/prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Vec2{
	BLF32 fX;
	BLF32 fY;
}BLVec2;

typedef struct _Vec3{
	BLF32 fX;
	BLF32 fY;
	BLF32 fZ;
}BLVec3;

typedef struct _Vec4{
	BLF32 fX;
	BLF32 fY;
	BLF32 fZ;
	BLF32 fW;
}BLVec4;

typedef struct _Quaternion{
	BLF32 fW;
	BLF32 fX;
	BLF32 fY;
	BLF32 fZ;
}BLQuaternion;

typedef struct _Matrix{
	union
	{
		BLF32 fData[4][4];
		BLF32 fEle[16];
	};
}BLMatrix;

typedef struct _Rect{
	BLVec2 sLT;
	BLVec2 sRB;
}BLRect;

typedef struct _Plane{
	BLVec3 sNormal;
	BLF32 fDistance;
}BLPlane;

typedef struct _Sphere{
	BLVec3 sPt;
	BLF32 fR;
}BLSphere;

typedef struct _Box{
	BLVec3 sMinPt;
	BLVec3 sMaxPt;
}BLBox;

typedef struct _Frustum{
	BLPlane sPlanes[6];
}BLFrustum;

BLS32 blRandI();

BLS32 blRandRangeI(
	IN BLS32 _Min, 
	IN BLS32 _Max);

BLF32 blRandF();

BLF32 blRandRangeF(
	IN BLF32 _Min, 
	IN BLF32 _Max);

BLBool blScalarApproximate(
	IN BLF32 _V1, 
	IN BLF32 _V2);

BLF32 blScalarClamp(
	IN BLF32 _Val, 
	IN BLF32 _Min, 
	IN BLF32 _Max);

BLRect blRectIntersects(
	IN BLRect* _R1,
	IN BLRect* _R2);

BLBool blRectContains(
	IN BLRect* _Rc,
	IN BLVec2* _Pt);

BLRect blRectClip(
	IN BLRect* _R1,
	IN BLRect* _R2);

BLVoid blRectExtend(
	INOUT BLRect* _Tar, 
	IN BLVec2* _Pt);

BLF32 blVec2Length(
	IN BLVec2* _V);

BLF32 blVec2Sqlength(
	IN BLVec2* _V);

BLVoid blVec2Normalize(
	INOUT BLVec2* _V);

BLF32 blVec2DotProduct(
	IN BLVec2* _V1,
	IN BLVec2* _V2);

BLF32 blVec2CrossProduct(
	IN BLVec2* _V1,
	IN BLVec2* _V2);

BLBool blVec2Approximate(
	IN BLVec2* _V1,
	IN BLVec2* _V2);

BLF32 blVec3Length(
	IN BLVec3* _V);

BLF32 blVec3Sqlength(
	IN BLVec3* _V);

BLVoid blVec3Normalize(
	INOUT BLVec3* _V);

BLF32 blVec3DotProduct(
	IN BLVec3* _V1,
	IN BLVec3* _V2);

BLVec3 blVec3CrossProduct(
	IN BLVec3* _V1,
	IN BLVec3* _V2);

BLBool blVec3Approximate(
	IN BLVec3* _V1,
	IN BLVec3* _V2);

BLQuaternion blQuatFromAxNRad(
	IN BLVec3* _Axis, 
	IN BLF32 _Rad);

BLQuaternion blQuatFromRPY(
	IN BLF32 _Roll, 
	IN BLF32 _Pitch, 
	IN BLF32 _Yaw);

BLQuaternion blQuatFrom2Vec(
	IN BLVec3* _Src,
	IN BLVec3* _Dest);

BLQuaternion blQuatFromTran(
	IN BLVec3* _Xa,
	IN BLVec3* _Ya,
	IN BLVec3* _Za);

BLVoid blQuat2Rotation(
	IN BLQuaternion* _Q, 
	OUT BLF32* _Angle, 
	OUT BLVec3* _Axis);

BLVoid blQuatNormalize(
	INOUT BLQuaternion* _Q);

BLVoid blQuatInverse(
	INOUT BLQuaternion* _Q);

BLF32 blQuatDotProduct(
	IN BLQuaternion* _Q1,
	IN BLQuaternion* _Q2);

BLQuaternion blQuatCrossproduct(
	IN BLQuaternion* _Q1,
	IN BLQuaternion* _Q2);

BLQuaternion blQuatSlerp(
	IN BLQuaternion* _Q1,
	IN BLQuaternion* _Q2, 
	IN BLF32 _T);

BLVec3 blQuatTransform(
	IN BLQuaternion* _Q,
	IN BLVec3* _Vec);

BLMatrix blMatFromTransform(
	IN BLVec3* _Trans,
	IN BLVec3* _Scale,
	IN BLQuaternion* _Rotate);

BLMatrix blMatMultiply(
	IN BLMatrix* _M1,
	IN BLMatrix* _M2);

BLVoid blMatDecompose(
	IN BLMatrix* _Mat, 
	OUT BLVec3* _Trans, 
	OUT BLVec3* _Scale, 
	OUT BLQuaternion* _Rotate);

BLVoid blMatTranspose(
	INOUT BLMatrix* _Mat);

BLVec3 blMatTransform(
	IN BLMatrix* _Mat,
	IN BLVec3* _Vec);

BLVoid blMatPerspectiveL(
	OUT BLMatrix* _Mat, 
	IN BLF32 _Fov, 
	IN BLF32 _Aspect, 
	IN BLF32 _Near, 
	IN BLF32 _Far);

BLVoid blMatLookatL(
	OUT BLMatrix* _Mat, 
	IN BLVec3* _Eye,
	IN BLVec3* _Focus,
	IN BLVec3* _Up);

BLPlane blPlaneFrom3P(
	IN BLVec3* _P1,
	IN BLVec3* _P2,
	IN BLVec3* _P3);

BLPlane blPlaneFromPN(
	IN BLVec3* _Pt,
	IN BLVec3* _Nor);

BLVoid blBoxMerge(
	INOUT BLBox* _Box, 
	IN BLVec3* _Pt);

BLBool blBoxContains(
	IN BLBox* _Box,
	IN BLVec3* _Pt);

BLBool blBoxIntersects(
	IN BLBox* _B1,
	IN BLBox* _B2);

BLBool blSphereContains(
	IN BLSphere* _Sp,
	IN BLVec3* _Pt);

BLBool blSphereIntersects(
	IN BLSphere* _S1,
	IN BLSphere* _S2);

BLBool blBoxSphereIntersects(
	IN BLBox* _Box,
	IN BLSphere* _Sp);

BLBool blFrustumCullBox(
	IN BLFrustum* _Fru,
	IN BLBox* _Box);

BLBool blFrustumCullSphere(
	IN BLFrustum* _Fru,
	IN BLSphere* _Sp);

#ifdef __cplusplus
}
#endif

#endif/* __algorithm_h_ */
