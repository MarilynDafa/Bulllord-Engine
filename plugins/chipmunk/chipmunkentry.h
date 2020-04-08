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
#ifndef __chipmunkentry_h_
#define __chipmunkentry_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
	BL_API BLVoid blChipmunkOpenEXT(
		IN BLAnsi* _Version,
		...);

	BL_API BLVoid blChipmunkCloseEXT();

	//space
	BL_API BLVoid blChipmunkSpaceUseSpatialHash(
		IN BLF32 _Dim,
		IN BLS32 _Count);

	BL_API BLVoid blChipmunkSpaceIterationsEXT(
		IN BLU32 _Iterations);

	BL_API BLVoid blChipmunkSpaceGetIterationsEXT(
		OUT BLU32* _Iterations);

	BL_API BLVoid blChipmunkSpaceGravityEXT(
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLVoid blChipmunkSpaceGetGravityEXT(
		OUT BLF32* _X,
		OUT BLF32* _Y);

	BL_API BLVoid blChipmunkSpaceDampingEXT(
		IN BLF32 _Damping);

	BL_API BLVoid blChipmunkSpaceGetDampingEXT(
		OUT BLF32* _Damping);

	BL_API BLVoid blChipmunkSpaceSpeedThreshold(
		IN BLF32 _Threshold);

	BL_API BLVoid blChipmunkSpaceGetSpeedThreshold(
		OUT BLF32* _Threshold);

	BL_API BLVoid blChipmunkSpaceSleepTimeThresholdEXT(
		IN BLF32 _Threshold);

	BL_API BLVoid blChipmunkSpaceGetSleepTimeThresholdEXT(
		OUT BLF32* _Threshold);

	BL_API BLVoid blChipmunkSpaceCollisionSlopEXT(
		IN BLF32 _Slop);

	BL_API BLVoid blChipmunkSpaceGetCollisionSlopEXT(
		OUT BLF32* _Slop);

	BL_API BLVoid blChipmunkSpaceCollisionBiasEXT(
		IN BLF32 _Bias);

	BL_API BLVoid blChipmunkSpaceGetCollisionBiasEXT(
		OUT BLF32* _Bias);

	BL_API BLVoid blChipmunkSpaceCollisionPersistenceEXT(
		IN BLU32 _Persistence);

	BL_API BLVoid blChipmunkSpaceGetCollisionPersistenceEXT(
		OUT BLU32* _Persistence);

	//Moment & Area
	BL_API BLF32 blChipmunkMomentCircleEXT(
		IN BLF32 _M,
		IN BLF32 _R1,
		IN BLF32 _R2,
		IN BLF32 _CentroidOffsetX,
		IN BLF32 _CentroidOffsetY);

	BL_API BLF32 blChipmunkAreaCircleEXT(
		IN BLF32 _R1,
		IN BLF32 _R2);

	BL_API BLF32 blChipmunkMomentSegmentEXT(
		IN BLF32 _M,
		IN BLF32 _X1,
		IN BLF32 _Y1,
		IN BLF32 _X2,
		IN BLF32 _Y2,
		IN BLF32 _Radius);

	BL_API BLF32 blChipmunkAreaSegmentEXT(
		IN BLF32 _X1,
		IN BLF32 _Y1,
		IN BLF32 _X2,
		IN BLF32 _Y2,
		IN BLF32 _Radius);

	BL_API BLF32 blChipmunkMomentPolyEXT(
		IN BLF32 _M,
		IN BLF32* _Verts,
		IN BLU32 _VertNum,
		IN BLF32 _Radius);

	BL_API BLF32 blChipmunkAreaPolyEXT(
		IN BLF32* _Verts,
		IN BLU32 _VertNum,
		IN BLF32 _Radius);

	BL_API BLF32 blChipmunkMomentBoxEXT(
		IN BLF32 _M,
		IN BLF32 _Width,
		IN BLF32 _Height);

	BL_API BLF32 blChipmunkAreaBoxEXT(
		IN BLF32 _Width,
		IN BLF32 _Height);

	//body
	BL_API BLBool blChipmunkSpriteStaticPolyBodyEXT(
		IN BLGuid _ID,
		IN BLF32* _ShapeData,
		IN BLU32 _DataNum,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteStaticCircleBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _OffsetX,
		IN BLF32 _OffsetY,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteStaticBoxBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Width,
		IN BLF32 _Height,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteStaticSegmentBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _AX,
		IN BLF32 _AY,
		IN BLF32 _BX,
		IN BLF32 _BY,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteDynamicPolyBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Mass,
		IN BLF32 _Moment,
		IN BLF32* _ShapeData,
		IN BLU32 _DataNum,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteDynamicCircleBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Mass,
		IN BLF32 _Moment,
		IN BLF32 _OffsetX,
		IN BLF32 _OffsetY,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteDynamicBoxBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Mass,
		IN BLF32 _Moment,
		IN BLF32 _Width,
		IN BLF32 _Height,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteDynamicSegmentBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Mass,
		IN BLF32 _Moment,
		IN BLF32 _AX,
		IN BLF32 _AY,
		IN BLF32 _BX,
		IN BLF32 _BY,
		IN BLF32 _Radius);

	BL_API BLBool blChipmunkSpriteStateEXT(
		IN BLGuid _ID,
		IN BLF32 _XPos,
		IN BLF32 _YPos,
		IN BLF32 _Angle,
		IN BLF32 _XVel,
		IN BLF32 _YVel);

	//shape
	BL_API BLVoid blChipmunkShapeMassEXT(
		IN BLGuid _ID,
		IN BLF32 _Mass);

	BL_API BLVoid blChipmunkShapeDensityEXT(
		IN BLGuid _ID,
		IN BLF32 _Density);

	BL_API BLVoid blChipmunkShapeSensorEXT(
		IN BLGuid _ID,
		IN BLBool _Sensor);

	BL_API BLVoid blChipmunkShapeElasticityEXT(
		IN BLGuid _ID,
		IN BLF32 _Elasticity);

	BL_API BLVoid blChipmunkShapeFrictionEXT(
		IN BLGuid _ID,
		IN BLF32 _Friction);

	BL_API BLVoid blChipmunkShapeSurfaceVelocityEXT(
		IN BLGuid _ID,
		IN BLF32 _XVelocity,
		IN BLF32 _YVelocity);

	BL_API BLVoid blChipmunkShapeCollisionTypeEXT(
		IN BLGuid _ID,
		IN BLU32 _CollisionType);

	BL_API BLVoid blChipmunkShapeFilterEXT(
		IN BLGuid _ID,
		IN BLU32 _Group,
		IN BLU32 _Category,
		IN BLU32 _Mask);

	BL_API BLVoid blChipmunkShapeQueryEXT(
		IN BLGuid _ID,
		OUT BLF32* _Mass,
		OUT BLF32* _Density,
		OUT BLBool* _Sensor,
		OUT BLF32* _Elasticity,
		OUT BLF32* _Friction,
		OUT BLF32* _XVelocity,
		OUT BLF32* _YVelocity,
		OUT BLU32* _CollisionType,
		OUT BLU32* _Group,
		OUT BLU32* _Category,
		OUT BLU32* _Mask);

	//constraint
	//arbiter
#ifdef __cplusplus
}
#endif
#endif/* __chipmunk_h_ */
