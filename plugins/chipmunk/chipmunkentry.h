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

	BL_API BLBool blChipmunkSpritePhysicalQuantitiesEXT(
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
		OUT BLF32* _Torque);

	BL_API BLVoid blChipmunkSpriteVelocityUpdateFunc(
		IN BLGuid _ID,
		IN BLVoid(*_VelocityFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32));

	BL_API BLVoid blChipmunkSpritePositionUpdateFunc(
		IN BLGuid _ID,
		IN BLVoid(*_PositionFunc)(BLGuid, BLF32));

	BL_API BLVoid blChipmunkSpriteUpdateVelocity(
		IN BLGuid _ID,
		IN BLF32 _GravityX,
		IN BLF32 _GravityY, 
		IN BLF32 _Damping, 
		IN BLF32 _Dt);

	BL_API BLVoid blChipmunkSpriteUpdatePosition(
		IN BLGuid _ID,
		IN BLF32 _Dt);

	BL_API BLVoid blChipmunkSpriteLocalToWorld(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _OX,
		OUT BLF32* _OY);

	BL_API BLVoid blChipmunkSpriteWorldToLocal(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		OUT BLF32* _OX,
		OUT BLF32* _OY);

	BL_API BLVoid blChipmunkSpriteApplyForceAtWorld(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ForceX,
		IN BLF32 _ForceY);

	BL_API BLVoid blChipmunkSpriteApplyForceAtLocal(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ForceX,
		IN BLF32 _ForceY);

	BL_API BLVoid blChipmunkSpriteApplyImpulseAtWorld(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ImpulseX,
		IN BLF32 _ImpulseY);

	BL_API BLVoid blChipmunkSpriteApplyImpulseAtLocal(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _ImpulseX,
		IN BLF32 _ImpulseY);

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
		OUT BLF32* _XVelocityS,
		OUT BLF32* _YVelocityS,
		OUT BLU32* _CollisionType,
		OUT BLU32* _Group,
		OUT BLU32* _Category,
		OUT BLU32* _Mask);

	//constraint
	BL_API BLGuid blChipmunkConstraintGearEXT(
		IN BLGuid _A, 
		IN BLGuid _B, 
		IN BLF32 _Phase, 
		IN BLF32 _Ratio);

	BL_API BLGuid blChipmunkConstraintGrooveEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AGrooveX,
		IN BLF32 _AGrooveY,
		IN BLF32 _BGrooveX,
		IN BLF32 _BGrooveY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLGuid blChipmunkConstraintPinEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLGuid blChipmunkConstraintPivotEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _APivotX,
		IN BLF32 _APivotY,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLBool _UsePivot);

	BL_API BLGuid blChipmunkConstraintRatchetEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _Phase,
		IN BLF32 _Ratchet);

	BL_API BLGuid blChipmunkConstraintRotaryEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLGuid blChipmunkConstraintSlideEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLGuid blChipmunkConstraintSpringEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _RestLength,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLGuid blChipmunkConstraintRotarySpringEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLS32 _RestAngle,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLGuid blChipmunkConstraintMotorEXT(
		IN BLGuid _A,
		IN BLGuid _B,
		IN BLF32 _Rate);

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
		IN BLF32 _CollideBodies);

	BL_API BLVoid blChipmunkConstraintGetParamEXT(
		IN BLGuid _Constraint,
		OUT BLF32* _RestLength,
		OUT BLF32* _Stiffness,
		OUT BLF32* _Damping);

	BL_API BLVoid blChipmunkConstraintGearParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _Phase,
		IN BLF32 _Ratio);

	BL_API BLVoid blChipmunkConstraintGrooveParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AGrooveX,
		IN BLF32 _AGrooveY,
		IN BLF32 _BGrooveX,
		IN BLF32 _BGrooveY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLVoid blChipmunkConstraintPinParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _Dist);

	BL_API BLVoid blChipmunkConstraintPivotParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY);

	BL_API BLVoid blChipmunkConstraintRatchetParamEXT(
		IN BLGuid _Constraint,
		IN BLS32 _Angle,
		IN BLF32 _Phase,
		IN BLF32 _Ratchet);

	BL_API BLVoid blChipmunkConstraintRotaryParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLGuid blChipmunkConstraintSlideParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _Min,
		IN BLF32 _Max);

	BL_API BLGuid blChipmunkConstraintSpringParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _AAnchorX,
		IN BLF32 _AAnchorY,
		IN BLF32 _BAnchorX,
		IN BLF32 _BAnchorY,
		IN BLF32 _RestLength,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLGuid blChipmunkConstraintRotarySpringParamEXT(
		IN BLGuid _Constraint,
		IN BLS32 _RestAngle,
		IN BLF32 _Stiffness,
		IN BLF32 _Damping);

	BL_API BLGuid blChipmunkConstraintMotorParamEXT(
		IN BLGuid _Constraint,
		IN BLF32 _Rate);

	BL_API BLGuid blChipmunkConstraintSolveFuncEXT(
		IN BLGuid _Constraint,
		IN BLVoid(*_PreSolveFunc)(BLGuid),
		IN BLVoid(*_PostSolveFunc)(BLGuid),
		IN BLF32(*_SpringForceFunc)(BLGuid, BLF32),
		IN BLF32(*_SpringTorqueFunc)(BLGuid, BLF32));
	//arbiter
#ifdef __cplusplus
}
#endif
#endif/* __chipmunk_h_ */
