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
	BL_API BLVoid blChipmunkSpaceUseSpatialHashEXT(
		IN BLF32 _Dim,
		IN BLS32 _Count);

	BL_API BLVoid blChipmunkSpaceIterationsEXT(
		IN BLU32 _Iterations);

	BL_API BLVoid blChipmunkSpaceGravityEXT(
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLVoid blChipmunkSpaceDampingEXT(
		IN BLF32 _Damping);

	BL_API BLVoid blChipmunkSpaceSpeedThresholdEXT(
		IN BLF32 _Threshold);

	BL_API BLVoid blChipmunkSpaceSleepTimeThresholdEXT(
		IN BLF32 _Threshold);

	BL_API BLVoid blChipmunkSpaceCollisionSlopEXT(
		IN BLF32 _Slop);

	BL_API BLVoid blChipmunkSpaceCollisionBiasEXT(
		IN BLF32 _Bias);

	BL_API BLVoid blChipmunkSpaceCollisionPersistenceEXT(
		IN BLU32 _Persistence);

	BL_API BLVoid blChipmunkSpaceStateQueryEXT(
		OUT BLU32* _Iterations,
		OUT BLF32* _GravityX,
		OUT BLF32* _GravityY,
		OUT BLF32* _Damping,
		OUT BLF32* _SpeedThreshold,
		OUT BLF32* _SleepThreshold,
		OUT BLF32* _Slop,
		OUT BLF32* _Bias,
		OUT BLU32* _Persistence,
		OUT BLF32* _TimeStep,
		OUT BLBool* _Locked);

	BL_API BLVoid blChipmunkSpacePointQueryEXT(
		IN BLF32 _PointX,
		IN BLF32 _PointY, 
		IN BLF32 _MaxDistance,
		IN BLU32 _Group,
		IN BLU32 _Category,
		IN BLU32 _Mask,
		IN BLVoid(*_QueryFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32, BLF32, BLVoid*),
		IN BLVoid* _Data);

	BL_API BLGuid blChipmunkSpacePointNearestQueryEXT(
		IN BLF32 _PointX,
		IN BLF32 _PointY,
		IN BLF32 _MaxDistance,
		IN BLU32 _Group,
		IN BLU32 _Category,
		IN BLU32 _Mask,
		OUT BLF32* _OutX,
		OUT BLF32* _OutY,
		OUT BLF32* _Distance,
		OUT BLF32* _GradientX,
		OUT BLF32* _GradientY);

	BL_API BLVoid blChipmunkSpaceSegmentQueryEXT(
		IN BLF32 _StartX, 
		IN BLF32 _StartY,
		IN BLF32 _EndX,
		IN BLF32 _ExdY,
		IN BLF32 _Radius,
		IN BLU32 _Group,
		IN BLU32 _Category,
		IN BLU32 _Mask,
		IN BLVoid(*_QueryFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32, BLF32, BLVoid*),
		IN BLVoid* _Data);

	BL_API BLGuid blChipmunkSpaceSegmentFirstQueryEXT(
		IN BLF32 _StartX,
		IN BLF32 _StartY,
		IN BLF32 _EndX,
		IN BLF32 _ExdY,
		IN BLF32 _Radius,
		IN BLU32 _Group,
		IN BLU32 _Category,
		IN BLU32 _Mask,
		OUT BLF32* _PointX,
		OUT BLF32* _PointY,
		OUT BLF32* _NormalX,
		OUT BLF32* _NormalY,
		OUT BLF32* _Alpha);

	BL_API BLVoid blChipmunkSpaceBoxQueryEXT(
		IN BLF32 _MinX,
		IN BLF32 _MinY,
		IN BLF32 _MaxX,
		IN BLF32 _MaxY,
		IN BLU32 _Group,
		IN BLU32 _Category,
		IN BLU32 _Mask,
		IN BLVoid(*_QueryFunc)(BLGuid, BLVoid*),
		IN BLVoid* _Data);

	BL_API BLVoid blChipmunkSpaceCollisionHandler(
		IN BLU32 _TypeA, 
		IN BLU32 _TypeB,
		IN BLBool(*_BeginFunc)(BLGuid),
		IN BLBool(*_PreSolveFunc)(BLGuid),
		IN BLVoid(*_PostSolveFunc)(BLGuid),
		IN BLVoid(*_SeparateFunc)(BLGuid));

	BL_API BLVoid blChipmunkSpaceWildcardHandler(
		IN BLU32 _Type,
		IN BLBool(*_BeginFunc)(BLGuid),
		IN BLBool(*_PreSolveFunc)(BLGuid),
		IN BLVoid(*_PostSolveFunc)(BLGuid),
		IN BLVoid(*_SeparateFunc)(BLGuid));
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
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLS32 _Angle);

	BL_API BLBool blChipmunkSpriteStaticCircleBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteStaticBoxBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Width,
		IN BLF32 _Height,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteStaticSegmentBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _AX,
		IN BLF32 _AY,
		IN BLF32 _BX,
		IN BLF32 _BY,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLS32 _Angle);

	BL_API BLBool blChipmunkSpriteKinematicPolyBodyEXT(
		IN BLGuid _ID,
		IN BLF32* _ShapeData,
		IN BLU32 _DataNum,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLS32 _Angle);

	BL_API BLBool blChipmunkSpriteKinematicCircleBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteKinematicBoxBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _Width,
		IN BLF32 _Height,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteKinematicSegmentBodyEXT(
		IN BLGuid _ID,
		IN BLF32 _AX,
		IN BLF32 _AY,
		IN BLF32 _BX,
		IN BLF32 _BY,
		IN BLF32 _Radius,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLS32 _Angle);

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

	BL_API BLBool blChipmunkSpriteSleepEXT(
		IN BLGuid _ID,
		IN BLBool _Sleep);

	BL_API BLBool blChipmunkSpriteIsSleepEXT(
		IN BLGuid _ID);

	BL_API BLBool blChipmunkSpriteStatesEXT(
		IN BLGuid _ID,
		IN BLF32 _XPos,
		IN BLF32 _YPos,
		IN BLS32 _Angle);

	BL_API BLBool blChipmunkSpriteMassEXT(
		IN BLGuid _ID,
		IN BLF32 _Mass);

	BL_API BLBool blChipmunkSpriteMomentEXT(
		IN BLGuid _ID,
		IN BLF32 _Moment);

	BL_API BLBool blChipmunkSpriteCenterOfGravityEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteVelocityEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteForceEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y);

	BL_API BLBool blChipmunkSpriteAngularVelocityEXT(
		IN BLGuid _ID,
		IN BLF32 _Vel);

	BL_API BLBool blChipmunkSpriteTorqueEXT(
		IN BLGuid _ID,
		IN BLF32 _Tor);

	BL_API BLBool blChipmunkSpriteQuantitiesQueryEXT(
		IN BLGuid _ID,
		OUT BLF32* _Mass, 
		OUT BLF32* _Moment,
		OUT BLF32* _CenterX,
		OUT BLF32* _CenterY, 
		OUT BLF32* _VelocityX,
		OUT BLF32* _VelocityY,
		OUT BLF32* _VelocityA,
		OUT BLF32* _ForceX,
		OUT BLF32* _ForceY,
		OUT BLF32* _Torque,
		OUT BLF32* _Energy);

	BL_API BLVoid blChipmunkSpriteVelocityUpdateFuncEXT(
		IN BLGuid _ID,
		IN BLVoid(*_VelocityFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32));

	BL_API BLVoid blChipmunkSpritePositionUpdateFuncEXT(
		IN BLGuid _ID,
		IN BLVoid(*_PositionFunc)(BLGuid, BLF32));

	BL_API BLVoid blChipmunkSpriteUpdateVelocityEXT(
		IN BLGuid _ID,
		IN BLF32 _GravityX,
		IN BLF32 _GravityY, 
		IN BLF32 _Damping, 
		IN BLF32 _Dt);

	BL_API BLVoid blChipmunkSpriteUpdatePositionEXT(
		IN BLGuid _ID,
		IN BLF32 _Dt);

	BL_API BLVoid blChipmunkSpriteLocalToWorldEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _OX,
		OUT BLF32* _OY);

	BL_API BLVoid blChipmunkSpriteWorldToLocalEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _OX,
		OUT BLF32* _OY);

	BL_API BLVoid blChipmunkSpriteApplyForceAtWorldEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ForceX,
		IN BLF32 _ForceY);

	BL_API BLVoid blChipmunkSpriteApplyForceAtLocalEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ForceX,
		IN BLF32 _ForceY);

	BL_API BLVoid blChipmunkSpriteApplyImpulseAtWorldEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ImpulseX,
		IN BLF32 _ImpulseY);

	BL_API BLVoid blChipmunkSpriteApplyImpulseAtLocalEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ImpulseX,
		IN BLF32 _ImpulseY);

	BL_API BLVoid blChipmunkSpriteVelocityAtWorldEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _VelX,
		OUT BLF32* _VelY);

	BL_API BLVoid blChipmunkSpriteVelocityAtLocalEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _VelX,
		OUT BLF32* _VelY);

	BL_API BLVoid blChipmunkSpriteEachConstraintEXT(
		IN BLGuid _ID,
		IN BLVoid(*_IteratorFunc)(BLGuid, BLGuid, BLVoid*),
		IN BLVoid* _Data);

	BL_API BLVoid blChipmunkSpriteEachArbiterEXT(
		IN BLGuid _ID,
		IN BLVoid(*_IteratorFunc)(BLGuid, BLGuid, BLVoid*),
		IN BLVoid* _Data);

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

	BL_API BLVoid blChipmunkShapeGetPolyParamEXT(
		IN BLGuid _ID,
		IN BLU32 _Index,
		OUT BLF32* _Radius,
		OUT BLU32* _Count,
		OUT BLF32* _X,
		OUT BLF32* _Y);

	BL_API BLVoid blChipmunkShapeGetCircleParamEXT(
		IN BLGuid _ID,
		OUT BLF32* _X,
		OUT BLF32* _Y,
		OUT BLF32* _Radius);

	BL_API BLVoid blChipmunkShapeGetSegmentParamEXT(
		IN BLGuid _ID,
		OUT BLF32* _AX,
		OUT BLF32* _AY,
		OUT BLF32* _BX,
		OUT BLF32* _BY,
		OUT BLF32* _NormalX,
		OUT BLF32* _NormalY,
		OUT BLF32* _Radius);

	BL_API BLVoid blChipmunkShapeQuantitiesQueryEXT(
		IN BLGuid _ID,
		OUT BLF32* _Mass,
		OUT BLF32* _Density,
		OUT BLBool* _Sensor,
		OUT BLF32* _Elasticity,
		OUT BLF32* _Friction,
		OUT BLF32* _XVelocityS,
		OUT BLF32* _YVelocityS,
		OUT BLU32* _CollisionType,
		OUT BLU32* _Group,
		OUT BLU32* _Category,
		OUT BLU32* _Mask,
		OUT BLF32* _BBMinX,
		OUT BLF32* _BBMaxX,
		OUT BLF32* _BBMinY,
		OUT BLF32* _BBMaxY);

	BL_API BLGuid blChipmunkShapePointQueryEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _PtX,
		OUT BLF32* _PtY,
		OUT BLF32* _Distance,
		OUT BLF32* _GradientX,
		OUT BLF32* _GradientY);

	BL_API BLGuid blChipmunkShapeSegmentQueryEXT(
		IN BLGuid _ID,
		IN BLF32 _AX,
		IN BLF32 _AY,
		IN BLF32 _BX,
		IN BLF32 _BY,
		IN BLF32 _Radius,
		OUT BLF32* _PtX,
		OUT BLF32* _PtY,
		OUT BLF32* _NormalX,
		OUT BLF32* _NormalY,
		OUT BLF32* _Alpha);

	//constraint
	BL_API BLGuid blChipmunkConstraintGearGenEXT(
		IN BLGuid _A, 
		IN BLGuid _B, 
		IN BLF32 _Phase, 
		IN BLF32 _Ratio);

	BL_API BLGuid blChipmunkConstraintGrooveGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AGrooveX,
		IN BLF32 _AGrooveY,
		IN BLF32 _BGrooveX,
		IN BLF32 _BGrooveY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLGuid blChipmunkConstraintPinGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLGuid blChipmunkConstraintPivotGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _APivotX,
		IN BLF32 _APivotY,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLBool _UsePivot);

	BL_API BLGuid blChipmunkConstraintRatchetGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _Phase,
		IN BLF32 _Ratchet);

	BL_API BLGuid blChipmunkConstraintRotaryGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLGuid blChipmunkConstraintSlideGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLGuid blChipmunkConstraintSpringGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _RestLength,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLGuid blChipmunkConstraintRotarySpringGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLS32 _RestAngle,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLGuid blChipmunkConstraintMotorGenEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _Rate);

	BL_API BLVoid blChipmunkConstraintDeleteEXT(
		IN BLGuid _Constraint);

	BL_API BLVoid blChipmunkConstraintMaxForceEXT(
		IN BLGuid _Constraint,
		IN BLF32 _MaxForce);

	BL_API BLVoid blChipmunkConstraintErrorBiasEXT(
		IN BLGuid _Constraint,
		IN BLF32 _ErrorBias);

	BL_API BLVoid blChipmunkConstraintMaxBiasEXT(
		IN BLGuid _Constraint,
		IN BLF32 _MaxBias);

	BL_API BLVoid blChipmunkConstraintCollideBodiesEXT(
		IN BLGuid _Constraint,
		IN BLBool _CollideBodies);

	BL_API BLVoid blChipmunkConstraintGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _MaxForce,
		OUT BLF32* _ErrorBias,
		OUT BLF32* _MaxBias,
		OUT BLBool* _CollideBodies,
		OUT BLF32* _Impulse);

	BL_API BLVoid blChipmunkConstraintGearParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _Phase,
		IN BLF32 _Ratio);

	BL_API BLVoid blChipmunkConstraintGearGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _Phase,
		OUT BLF32* _Ratio);

	BL_API BLVoid blChipmunkConstraintGrooveParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AGrooveX,
		IN BLF32 _AGrooveY,
		IN BLF32 _BGrooveX,
		IN BLF32 _BGrooveY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLVoid blChipmunkConstraintGrooveGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _AGrooveX,
		OUT BLF32* _AGrooveY,
		OUT BLF32* _BGrooveX,
		OUT BLF32* _BGrooveY,
		OUT BLF32* _BAnchorX,
		OUT BLF32* _BAnchorY);

	BL_API BLVoid blChipmunkConstraintPinParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _Dist);

	BL_API BLVoid blChipmunkConstraintPinGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _AAnchorX,
		OUT BLF32* _AAnchorY,
		OUT BLF32* _BAnchorX,
		OUT BLF32* _BAnchorY,
		OUT BLF32* _Dist);

	BL_API BLVoid blChipmunkConstraintPivotParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLVoid blChipmunkConstraintPivotGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _AAnchorX,
		OUT BLF32* _AAnchorY,
		OUT BLF32* _BAnchorX,
		OUT BLF32* _BAnchorY);

	BL_API BLVoid blChipmunkConstraintRatchetParamEXT(
		IN BLGuid _Constraint,
		IN BLS32 _Angle,
		IN BLF32 _Phase,
		IN BLF32 _Ratchet);

	BL_API BLVoid blChipmunkConstraintRatchetGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLS32* _Angle,
		OUT BLF32* _Phase,
		OUT BLF32* _Ratchet);

	BL_API BLVoid blChipmunkConstraintRotaryParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLVoid blChipmunkConstraintRotaryGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _Min,
		OUT BLF32* _Max);

	BL_API BLVoid blChipmunkConstraintSlideParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLVoid blChipmunkConstraintSlideGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _AAnchorX,
		OUT BLF32* _AAnchorY,
		OUT BLF32* _BAnchorX,
		OUT BLF32* _BAnchorY,
		OUT BLF32* _Min,
		OUT BLF32* _Max);

	BL_API BLVoid blChipmunkConstraintSpringParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _RestLength,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLVoid blChipmunkConstraintSpringGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _AAnchorX,
		OUT BLF32* _AAnchorY,
		OUT BLF32* _BAnchorX,
		OUT BLF32* _BAnchorY,
		OUT BLF32* _RestLength,
		OUT BLF32* _Stiffness,
		OUT BLF32* _Damping);

	BL_API BLVoid blChipmunkConstraintRotarySpringParamEXT(
		IN BLGuid _Constraint,
		IN BLS32 _RestAngle,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLVoid blChipmunkConstraintRotarySpringGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLS32* _RestAngle,
		OUT BLF32* _Stiffness,
		OUT BLF32* _Damping);

	BL_API BLVoid blChipmunkConstraintMotorParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _Rate);

	BL_API BLVoid blChipmunkConstraintMotorGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _Rate);

	BL_API BLVoid blChipmunkConstraintSolveFuncEXT(
		IN BLGuid _Constraint,
		IN BLVoid(*_PreSolveFunc)(BLGuid),
		IN BLVoid(*_PostSolveFunc)(BLGuid),
		IN BLF32(*_SpringForceFunc)(BLGuid, BLF32),
		IN BLF32(*_SpringTorqueFunc)(BLGuid, BLF32));
	//arbiter
	BL_API BLVoid blChipmunkArbiterRestitution(
		IN BLGuid _Arbiter,
		IN BLF32 _Restitution);

	BL_API BLVoid blChipmunkArbiterGetRestitution(
		IN BLGuid _Arbiter,
		OUT BLF32* _Restitution);

	BL_API BLVoid blChipmunkArbiterFriction(
		IN BLGuid _Arbiter,
		IN BLF32 _Friction);

	BL_API BLVoid blChipmunkArbiterGetFriction(
		IN BLGuid _Arbiter,
		OUT BLF32* _Friction);

	BL_API BLVoid blChipmunkArbiterSurfaceVelocity(
		IN BLGuid _Arbiter,
		IN BLF32 _VelX,
		IN BLF32 _VelY);

	BL_API BLVoid blChipmunkArbiterGetSurfaceVelocity(
		IN BLGuid _Arbiter,
		OUT BLF32* _VelX,
		OUT BLF32* _VelY);

	BL_API BLBool blChipmunkArbiterIgnore(
		IN BLGuid _Arbiter);

	BL_API BLBool blChipmunkArbiterIsFirstContact(
		IN BLGuid _Arbiter);

	BL_API BLBool blChipmunkArbiterIsRemoval(
		IN BLGuid _Arbiter);

	BL_API BLVoid blChipmunkArbiterTotalImpulse(
		IN BLGuid _Arbiter,
		OUT BLF32* _X,
		OUT BLF32* _Y);

	BL_API BLVoid blChipmunkArbiterTotalKE(
		IN BLGuid _Arbiter,
		OUT BLF32* _KE);

	BL_API BLVoid blChipmunkArbiterContactPoint(
		IN BLGuid _Arbiter,
		IN BLU32 _Count,
		IN BLF32 _NormalX,
		IN BLF32 _NormalY,
		IN BLF32* _AX,
		IN BLF32* _AY,
		IN BLF32* _BX,
		IN BLF32* _BY,
		IN BLF32* _Depth);

	BL_API BLU32 blChipmunkArbiterGetCount(
		IN BLGuid _Arbiter);

	BL_API BLVoid blChipmunkArbiterGetContactNormal(
		IN BLGuid _Arbiter,
		OUT BLF32* _NormalX,
		OUT BLF32* _NormalY);

	BL_API BLVoid blChipmunkArbiterGetContactPoint(
		IN BLGuid _Arbiter,
		IN BLU32 _Idx,
		OUT BLF32* _AX,
		OUT BLF32* _AY,
		OUT BLF32* _BX,
		OUT BLF32* _BY,
		OUT BLF32* _Depth);

	BL_API BLGuid blChipmunkArbiterGetSpriteA(
		IN BLGuid _Arbiter);

	BL_API BLGuid blChipmunkArbiterGetSpriteB(
		IN BLGuid _Arbiter);

	BL_API BLBool blChipmunkArbiterCallWildcardBeginA(
		IN BLGuid _Arbiter);
	
	BL_API BLBool blChipmunkArbiterCallWildcardBeginB(
		IN BLGuid _Arbiter);

	BL_API BLBool blChipmunkArbiterCallWildcardPreSolveA(
		IN BLGuid _Arbiter);
	
	BL_API BLBool blChipmunkArbiterCallWildcardPreSolveB(
		IN BLGuid _Arbiter);

	BL_API BLVoid blChipmunkArbiterCallWildcardPostSolveA(
		IN BLGuid _Arbiter);
	
	BL_API BLVoid blChipmunkArbiterCallWildcardPostSolveB(
		IN BLGuid _Arbiter);

	BL_API BLVoid blChipmunkArbiterCallWildcardSeparateA(
		IN BLGuid _Arbiter);
	
	BL_API BLVoid blChipmunkArbiterCallWildcardSeparateB(
		IN BLGuid _Arbiter);
#ifdef __cplusplus
}
#endif
#endif/* __chipmunk_h_ */
