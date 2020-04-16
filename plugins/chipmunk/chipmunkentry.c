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
#include "chipmunkentry.h"
#include "chipmunk/chipmunk.h"
#include "chipmunk/chipmunk_unsafe.h"
#include "sprite.h"
#include "util.h"
typedef struct _BodyGuidNode {
	cpBody* pBody;
	BLGuid nID;
	BLVoid(*pVelocity)(BLGuid, BLF32, BLF32, BLF32, BLF32);
	BLVoid(*pPosition)(BLGuid, BLF32);
}_BLBodyGuidNode;
typedef struct _ChipmunkMember {
	cpSpace* pSpace;
}_BLChipmunkMember;
static _BLChipmunkMember* _PrCpMem = NULL;
static void
_VelocityUpdateFunc(cpBody* _Body, cpVect _Gravity, cpFloat _Damping, cpFloat _Dt)
{
	_BLBodyGuidNode* _bgn = cpBodyGetUserData(_Body);
	if (_bgn->pVelocity)
		_bgn->pVelocity(_bgn->nID, _Gravity.x, _Gravity.y, _Damping, _Dt);
}
static void
_PositionUpdateFunc(cpBody* _Body, cpFloat _Dt)
{
	_BLBodyGuidNode* _bgn = cpBodyGetUserData(_Body);
	if (_bgn->pPosition)
		_bgn->pPosition(_bgn->nID, _Dt);
}
static const BLBool
_BodyCp(BLU32 _Delta, BLGuid _ID, BLF32 _Mat[6], BLF32 _OffsetX, BLF32 _OffsetY, BLVoid** _ExtData)
{
	cpShape* _shape = blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return 2;
	cpBody* _body = cpShapeGetBody(_shape);
	cpVect _pos = cpBodyGetPosition(_body);
	cpFloat _rot = cpBodyGetAngle(_body);
	if (_body != cpSpaceGetStaticBody(_PrCpMem->pSpace))
	{
		blSpriteLocked(_ID, FALSE);
		blSpriteRotateTo(_ID, (BLS32)(_rot * -57.29578049044297f));
		blSpriteMoveTo(_ID, _pos.x, _pos.y);
		blSpriteLocked(_ID, TRUE);
	}
	return 2;
}
static const BLVoid
_StepCP(BLU32 _Delta)
{
	cpSpaceStep(_PrCpMem->pSpace, _Delta / 1000.f);
}
static const BLBool
_UnloadCP(BLGuid _ID, BLVoid** _ExtData)
{
	cpShape* _shape = (cpShape*)*_ExtData;
	cpBody* _sbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _bgn = cpBodyGetUserData(_body);
	if (_bgn)
		free(_bgn);
	if (_body != _sbody)
		cpBodyFree(_body);
	cpShapeFree(_shape);
	return 2;
}
BLVoid
blChipmunkOpenEXT(IN BLAnsi* _Version, ...)
{
	_PrCpMem = (_BLChipmunkMember*)malloc(sizeof(_BLChipmunkMember));
	_PrCpMem->pSpace = cpSpaceNew();
	blSpriteRegistExternal("bmg", NULL, NULL, _UnloadCP, NULL, _BodyCp, _StepCP);
}
BLVoid
blChipmunkCloseEXT()
{
	cpSpaceFree(_PrCpMem->pSpace);
	free(_PrCpMem);
}
BLVoid 
blChipmunkSpaceUseSpatialHash(IN BLF32 _Dim, IN BLS32 _Count)
{
	cpSpaceUseSpatialHash(_PrCpMem->pSpace, _Dim, _Count);
}
BLVoid 
blChipmunkSpaceIterationsEXT(IN BLU32 _Iterations)
{
	cpSpaceSetIterations(_PrCpMem->pSpace, _Iterations);
}
BLVoid 
blChipmunkSpaceGetIterationsEXT(OUT BLU32* _Iterations)
{
	*_Iterations = cpSpaceGetIterations(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpaceGravityEXT(IN BLF32 _X, IN BLF32 _Y)
{
	cpVect _vec;
	_vec.x = _X;
	_vec.y = _Y;
	cpSpaceSetGravity(_PrCpMem->pSpace, _vec);
}
BLVoid 
blChipmunkSpaceGetGravityEXT(OUT BLF32* _X, OUT BLF32* _Y)
{
	cpVect _ret = cpSpaceGetGravity(_PrCpMem->pSpace);
	*_X = _ret.x;
	*_Y = _ret.y;
}
BLVoid 
blChipmunkSpaceDampingEXT(IN BLF32 _Damping)
{
	cpSpaceSetDamping(_PrCpMem->pSpace, _Damping);
}
BLVoid 
blChipmunkSpaceGetDampingEXT(OUT BLF32* _Damping)
{
	*_Damping = cpSpaceGetDamping(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpaceSpeedThreshold(IN BLF32 _Threshold)
{
	cpSpaceSetIdleSpeedThreshold(_PrCpMem->pSpace, _Threshold);
}
BLVoid 
blChipmunkSpaceGetSpeedThreshold(OUT BLF32* _Threshold)
{
	*_Threshold = cpSpaceGetIdleSpeedThreshold(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpaceSleepTimeThresholdEXT(IN BLF32 _Threshold)
{
	cpSpaceSetSleepTimeThreshold(_PrCpMem->pSpace, _Threshold);
}
BLVoid 
blChipmunkSpaceGetSleepTimeThresholdEXT(OUT BLF32* _Threshold)
{
	*_Threshold = cpSpaceGetSleepTimeThreshold(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpaceCollisionSlopEXT(IN BLF32 _Slop)
{
	cpSpaceSetCollisionSlop(_PrCpMem->pSpace, _Slop);
}
BLVoid 
blChipmunkSpaceGetCollisionSlopEXT(OUT BLF32* _Slop)
{
	*_Slop = cpSpaceGetCollisionSlop(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpaceCollisionBiasEXT(IN BLF32 _Bias)
{
	cpSpaceSetCollisionBias(_PrCpMem->pSpace, _Bias);
}
BLVoid 
blChipmunkSpaceGetCollisionBiasEXT(OUT BLF32* _Bias)
{
	*_Bias = cpSpaceGetCollisionBias(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpaceCollisionPersistenceEXT(IN BLU32 _Persistence)
{
	cpSpaceSetCollisionPersistence(_PrCpMem->pSpace, _Persistence);
}
BLVoid 
blChipmunkSpaceGetCollisionPersistenceEXT(OUT BLU32* _Persistence)
{
	*_Persistence = cpSpaceGetCollisionPersistence(_PrCpMem->pSpace);
}
BLF32
blChipmunkMomentCircleEXT(IN BLF32 _M, IN BLF32 _R1, IN BLF32 _R2, IN BLF32 _CentroidOffsetX, IN BLF32 _CentroidOffsetY)
{
	return cpMomentForCircle(_M, _R1, _R2, cpv(_CentroidOffsetX, _CentroidOffsetY));
}
BLF32
blChipmunkAreaCircleEXT(IN BLF32 _R1, IN BLF32 _R2)
{
	return cpAreaForCircle(_R1, _R2);
}
BLF32
blChipmunkMomentSegmentEXT(IN BLF32 _M, IN BLF32 _X1, IN BLF32 _Y1, IN BLF32 _X2, IN BLF32 _Y2, IN BLF32 _Radius)
{
	return cpMomentForSegment(_M, cpv(_X1, _Y1), cpv(_X2, _Y2), _Radius);
}
BLF32
blChipmunkAreaSegmentEXT(IN BLF32 _X1, IN BLF32 _Y1, IN BLF32 _X2, IN BLF32 _Y2, IN BLF32 _Radius)
{
	return cpAreaForSegment(cpv(_X1, _Y1), cpv(_X2, _Y2), _Radius);
}
BLF32
blChipmunkMomentPolyEXT(IN BLF32 _M, IN BLF32* _Verts, IN BLU32 _VertNum, IN BLF32 _Radius)
{
	cpFloat tolerance = 2.f;
	cpVect* _verts = (cpVect *)alloca(_VertNum * sizeof(cpVect));
	BLU32 _hullcount = cpConvexHull(_VertNum / (2 * sizeof(BLF32)), (cpVect*)_Verts, _verts, NULL, tolerance);
	cpVect _offset = cpCentroidForPoly(_hullcount, (cpVect*)_verts);
	return cpMomentForPoly(_M, _hullcount, (cpVect*)_verts, _offset, _Radius);
}
BLF32
blChipmunkAreaPolyEXT(IN BLF32* _Verts, IN BLU32 _VertNum, IN BLF32 _Radius)
{
	cpFloat tolerance = 2.f;
	cpVect* _verts = (cpVect *)alloca(_VertNum * sizeof(cpVect));
	BLU32 _hullcount = cpConvexHull(_VertNum, (cpVect*)_Verts, _verts, NULL, tolerance);
	return cpAreaForPoly(_hullcount, _verts, _Radius);
}
BLF32
blChipmunkMomentBoxEXT(IN BLF32 _M, IN BLF32 _Width, IN BLF32 _Height)
{
	return cpMomentForBox(_M, _Width, _Height);
}
BLF32
blChipmunkAreaBoxEXT(IN BLF32 _Width, IN BLF32 _Height)
{
	return _Width * _Height;
}
BLBool
blChipmunkSpriteStaticPolyBodyEXT(IN BLGuid _ID, IN BLF32* _ShapeData, IN BLU32 _DataNum, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y, IN BLS32 _Angle)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _static = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	blSpriteLocked(_ID, FALSE);
	blSpriteRotateTo(_ID, _Angle);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	if (_ShapeData)
	{
		_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpPolyShapeNew(_static, _DataNum / (2 * sizeof(BLF32)), (cpVect*)_ShapeData, cpTransformRigid(cpv(_X, _Y), -_Angle * 0.0174532922222222f), _Radius));
	}
	else
	{
		cpVect _tris[] = {
			cpv(-_width * 0.5f, -_height * 0.5f),
			cpv(-_width * 0.5f, _height * 0.5f),
			cpv(_width * 0.5f, _height * 0.5f),
			cpv(_width * 0.5f, -_height * 0.5f),
		};
		_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpPolyShapeNew(_static, 4, _tris, cpTransformRigid(cpv(_X, _Y), -_Angle * 0.0174532922222222f), _Radius));
	}
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteStaticCircleBodyEXT(IN BLGuid _ID, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _static = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	blSpriteLocked(_ID, FALSE);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpCircleShapeNew(_static, _Radius, cpv(_X, _Y)));
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteStaticBoxBodyEXT(IN BLGuid _ID, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _static = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	blSpriteLocked(_ID, FALSE);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpBoxShapeNew2(_static, cpBBNew(_X - _Width * 0.5f, _Y - _Height * 0.5f, _X + _Width * 0.5f, _Y + _Height * 0.5f), _Radius));
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteStaticSegmentBodyEXT(IN BLGuid _ID, IN BLF32 _AX, IN BLF32 _AY, IN BLF32 _BX, IN BLF32 _BY, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y, IN BLS32 _Angle)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _static = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	blSpriteLocked(_ID, FALSE);
	blSpriteRotateTo(_ID, _Angle);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	cpVect _pta = cpv(_AX, _AY);
	cpVect _ptb = cpv(_BX, _BY);
	cpTransform _tm = cpTransformRigid(cpv(_X, _Y), -_Angle * 0.0174532922222222f);
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpSegmentShapeNew(_static, cpTransformPoint(_tm, _pta), cpTransformPoint(_tm, _ptb), _Radius));
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteKinematicPolyBodyEXT(IN BLGuid _ID, IN BLF32* _ShapeData, IN BLU32 _DataNum, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y, IN BLS32 _Angle)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _kinematic = cpBodyNewKinematic();
	blSpriteLocked(_ID, FALSE);
	blSpriteRotateTo(_ID, _Angle);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	if (_ShapeData)
	{
		_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpPolyShapeNew(_kinematic, _DataNum / (2 * sizeof(BLF32)), (cpVect*)_ShapeData, cpTransformRigid(cpv(_X, _Y), -_Angle * 0.0174532922222222f), _Radius));
	}
	else
	{
		cpVect _tris[] = {
			cpv(-_width * 0.5f, -_height * 0.5f),
			cpv(-_width * 0.5f, _height * 0.5f),
			cpv(_width * 0.5f, _height * 0.5f),
			cpv(_width * 0.5f, -_height * 0.5f),
		};
		_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpPolyShapeNew(_kinematic, 4, _tris, cpTransformRigid(cpv(_X, _Y), -_Angle * 0.0174532922222222f), _Radius));
	}
	cpSpaceAddBody(_PrCpMem->pSpace, _kinematic);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteKinematicCircleBodyEXT(IN BLGuid _ID, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _kinematic = cpBodyNewKinematic();
	blSpriteLocked(_ID, FALSE);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpCircleShapeNew(_kinematic, _Radius, cpv(_X, _Y)));
	cpSpaceAddBody(_PrCpMem->pSpace, _kinematic);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteKinematicBoxBodyEXT(IN BLGuid _ID, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _kinematic = cpBodyNewKinematic();
	blSpriteLocked(_ID, FALSE);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpBoxShapeNew2(_kinematic, cpBBNew(_X - _Width * 0.5f, _Y - _Height * 0.5f, _X + _Width * 0.5f, _Y + _Height * 0.5f), _Radius));
	cpSpaceAddBody(_PrCpMem->pSpace, _kinematic);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteKinematicSegmentBodyEXT(IN BLGuid _ID, IN BLF32 _AX, IN BLF32 _AY, IN BLF32 _BX, IN BLF32 _BY, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y, IN BLS32 _Angle)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpShape* _shape;
	cpBody* _kinematic = cpBodyNewKinematic();
	blSpriteLocked(_ID, FALSE);
	blSpriteRotateTo(_ID, _Angle);
	blSpriteMoveTo(_ID, _X, _Y);
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	cpVect _pta = cpv(_AX, _AY);
	cpVect _ptb = cpv(_BX, _BY);
	cpTransform _tm = cpTransformRigid(cpv(_X, _Y), -_Angle * 0.0174532922222222f);
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpSegmentShapeNew(_kinematic, cpTransformPoint(_tm, _pta), cpTransformPoint(_tm, _ptb), _Radius));
	cpSpaceAddBody(_PrCpMem->pSpace, _kinematic);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteDynamicPolyBodyEXT(IN BLGuid _ID, IN BLF32 _Mass, IN BLF32 _Moment, IN BLF32* _ShapeData, IN BLU32 _DataNum, IN BLF32 _Radius)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpBody* _body = cpBodyNew(_Mass, _Moment);
	cpShape* _shape;
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	cpBodySetPosition(_body, cpv(_xpos, _ypos));
	if (_ShapeData)
	{
		_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpPolyShapeNewRaw(_body, _DataNum / (2 * sizeof(BLF32)), (cpVect*)_ShapeData, _Radius));
	}
	else 
	{
		cpVect _tris[] = {
			cpv(-_width * 0.5f, -_height * 0.5f),
			cpv(-_width * 0.5f, _height * 0.5f),
			cpv(_width * 0.5f, _height * 0.5f),
			cpv(_width * 0.5f, -_height * 0.5f),
		};
		_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpPolyShapeNewRaw(_body, 4, _tris, _Radius));
	}
	cpSpaceAddBody(_PrCpMem->pSpace, _body);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteDynamicCircleBodyEXT(IN BLGuid _ID, IN BLF32 _Mass, IN BLF32 _Moment, IN BLF32 _Radius)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpBody* _body = cpBodyNew(_Mass, _Moment);
	cpShape* _shape;
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	cpBodySetPosition(_body, cpv(_xpos, _ypos));
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpCircleShapeNew(_body, _Radius, cpvzero));
	cpSpaceAddBody(_PrCpMem->pSpace, _body);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteDynamicBoxBodyEXT(IN BLGuid _ID, IN BLF32 _Mass, IN BLF32 _Moment, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Radius)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpBody* _body = cpBodyNew(_Mass, _Moment);
	cpShape* _shape;
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	cpBodySetPosition(_body, cpv(_xpos, _ypos));
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpBoxShapeNew(_body, _Width, _Height, _Radius));
	cpSpaceAddBody(_PrCpMem->pSpace, _body);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteDynamicSegmentBodyEXT(IN BLGuid _ID, IN BLF32 _Mass, IN BLF32 _Moment, IN BLF32 _AX, IN BLF32 _AY, IN BLF32 _BX, IN BLF32 _BY, IN BLF32 _Radius)
{
	if (blSpriteExternalData(_ID, NULL))
		return FALSE;
	cpBody* _body = cpBodyNew(_Mass, _Moment);
	cpShape* _shape;
	blSpriteLocked(_ID, TRUE);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	cpBodySetPosition(_body, cpv(_xpos, _ypos));
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpSegmentShapeNew(_body, cpv(_AX, _AY), cpv(_BX, _BY), _Radius));
	cpSpaceAddBody(_PrCpMem->pSpace, _body);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteSleepEXT(IN BLGuid _ID, IN BLBool _Sleep)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpBody* _body = cpShapeGetBody(_shape);
	if (_Sleep)
		cpBodySleep(_body);
	else
		cpBodyActivate(_body);
	return TRUE;
}
BLBool 
blChipmunkSpriteIsSleepEXT(IN BLGuid _ID)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpBody* _body = cpShapeGetBody(_shape);
	return cpBodyIsSleeping(_body);
}
BLBool
blChipmunkSpriteStatesEXT(IN BLGuid _ID, IN BLF32 _XPos, IN BLF32 _YPos, IN BLS32 _Angle)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	if (_body != cpSpaceGetStaticBody(_PrCpMem->pSpace))
	{
		blSpriteLocked(_ID, FALSE);
		cpBodySetPosition(_body, cpv(_XPos, _YPos));
		cpBodySetAngle(_body, -_Angle * 0.0174532922222222f);
		blSpriteRotateTo(_ID, _Angle);
		blSpriteMoveTo(_ID, _XPos, _YPos);
		blSpriteLocked(_ID, TRUE);
	}
	return TRUE;
}
BLBool 
blChipmunkSpriteMassEXT(IN BLGuid _ID, IN BLF32 _Mass)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetMass(_body, _Mass);
	return TRUE;
}
BLBool 
blChipmunkSpriteMomentEXT(IN BLGuid _ID, IN BLF32 _Moment)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetMoment(_body, _Moment);
	return TRUE;
}
BLBool 
blChipmunkSpriteCenterOfGravityEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetCenterOfGravity(_body, cpv(_X, _Y));
	return TRUE;
}
BLBool 
blChipmunkSpriteVelocityEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetVelocity(_body, cpv(_X, _Y));
	return TRUE;
}
BLBool 
blChipmunkSpriteForceEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetForce(_body, cpv(_X, _Y));
	return TRUE;
}
BLBool 
blChipmunkSpriteAngularVelocityEXT(IN BLGuid _ID, IN BLF32 _Vel)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetAngularVelocity(_body, _Vel);
	return TRUE;
}
BLBool 
blChipmunkSpriteTorqueEXT(IN BLGuid _ID, IN BLF32 _Tor)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodySetTorque(_body, _Tor);
	return TRUE;
}
BLBool 
blChipmunkSpritePhysicalQuantitiesEXT(IN BLGuid _ID, OUT BLF32* _Mass, OUT BLF32* _Moment, OUT BLF32* _CenterX, OUT BLF32* _CenterY, OUT BLF32* _VelocityX, OUT BLF32* _VelocityY, OUT BLF32* _VelocityA, OUT BLF32* _ForceX, OUT BLF32* _ForceY, OUT BLF32* _Torque)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return FALSE;
	cpBody* _body = cpShapeGetBody(_shape);
	*_Mass = cpBodyGetMass(_body);
	*_Moment = cpBodyGetMoment(_body);
	*_CenterX = cpBodyGetCenterOfGravity(_body).x;
	*_CenterY = cpBodyGetCenterOfGravity(_body).y;
	*_VelocityX = cpBodyGetVelocity(_body).x;
	*_VelocityY = cpBodyGetVelocity(_body).y;
	*_VelocityA = cpBodyGetAngularVelocity(_body);
	*_ForceX = cpBodyGetForce(_body).x;
	*_ForceY = cpBodyGetForce(_body).y;
	*_Torque = cpBodyGetTorque(_body);
	return TRUE;
}
BLVoid 
blChipmunkSpriteVelocityUpdateFunc(IN BLGuid _ID, IN BLVoid(*_VelocityFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32))
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _body;
	_node->nID = _ID;
	_node->pPosition = NULL;
	_node->pVelocity = _VelocityFunc;
	cpBodySetUserData(_body, _node);
	cpBodySetVelocityUpdateFunc(_body, _VelocityUpdateFunc);
}
BLVoid 
blChipmunkSpritePositionUpdateFunc(IN BLGuid _ID, IN BLVoid(*_PositionFunc)(BLGuid, BLF32))
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _body;
	_node->nID = _ID;
	_node->pPosition = _PositionFunc;
	_node->pVelocity = NULL;
	cpBodySetUserData(_body, _node);
	cpBodySetPositionUpdateFunc(_body, _PositionUpdateFunc);
}
BLVoid 
blChipmunkSpriteUpdateVelocity(IN BLGuid _ID, IN BLF32 _GravityX, IN BLF32 _GravityY, IN BLF32 _Damping, IN BLF32 _Dt)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyUpdateVelocity(_body, cpv(_GravityX, _GravityY), _Damping, _Dt);
}
BLVoid 
blChipmunkSpriteUpdatePosition(IN BLGuid _ID, IN BLF32 _Dt)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyUpdatePosition(_body, _Dt);
}
BLVoid 
blChipmunkSpriteLocalToWorld(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _OX, OUT BLF32* _OY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpVect _out = cpBodyLocalToWorld(_body, cpv(_X, _Y));
	*_OX = _out.x;
	*_OY = _out.y;
}
BLVoid 
blChipmunkSpriteWorldToLocal(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _OX, OUT BLF32* _OY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpVect _out = cpBodyWorldToLocal(_body, cpv(_X, _Y));
	*_OX = _out.x;
	*_OY = _out.y;
}
BLVoid 
blChipmunkSpriteApplyForceAtWorld(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ForceX, IN BLF32 _ForceY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyForceAtWorldPoint(_body, cpv(_ForceX, _ForceY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteApplyForceAtLocal(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ForceX, IN BLF32 _ForceY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyForceAtLocalPoint(_body, cpv(_ForceX, _ForceY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteApplyImpulseAtWorld(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ImpulseX, IN BLF32 _ImpulseY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyImpulseAtWorldPoint(_body, cpv(_ImpulseX, _ImpulseY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteApplyImpulseAtLocal(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ImpulseX, IN BLF32 _ImpulseY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyImpulseAtLocalPoint(_body, cpv(_ImpulseX, _ImpulseY), cpv(_X, _Y));
}
BLVoid
blChipmunkShapeMassEXT(IN BLGuid _ID, IN BLF32 _Mass)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetMass(_shape, _Mass);
}
BLVoid
blChipmunkShapeDensityEXT(IN BLGuid _ID, IN BLF32 _Density)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetDensity(_shape, _Density);
}
BLVoid
blChipmunkShapeSensorEXT(IN BLGuid _ID, IN BLBool _Sensor)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetSensor(_shape, _Sensor);
}
BLVoid
blChipmunkShapeElasticityEXT(IN BLGuid _ID, IN BLF32 _Elasticity)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetElasticity(_shape, _Elasticity);
}
BLVoid
blChipmunkShapeFrictionEXT(IN BLGuid _ID, IN BLF32 _Friction)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetFriction(_shape, _Friction);
}
BLVoid 
blChipmunkShapeSurfaceVelocityEXT(IN BLGuid _ID, IN BLF32 _XVelocity, IN BLF32 _YVelocity)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetSurfaceVelocity(_shape, cpv(_XVelocity, _YVelocity));
}
BLVoid
blChipmunkShapeCollisionTypeEXT(IN BLGuid _ID, IN BLU32 _CollisionType)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeSetCollisionType(_shape, (cpCollisionType)_CollisionType);
}
BLVoid
blChipmunkShapeFilterEXT(IN BLGuid _ID, IN BLU32 _Group, IN BLU32 _Category, IN BLU32 _Mask)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpShapeFilter _filter;
	_filter.group = _Group;
	_filter.categories = _Category;
	_filter.mask = _Mask;
	cpShapeSetFilter(_shape, _filter);
}
BLVoid 
blChipmunkQueryEXT(IN BLGuid _ID, OUT BLF32* _Mass, OUT BLF32* _Density, OUT BLBool* _Sensor, OUT BLF32* _Elasticity, OUT BLF32* _Friction, OUT BLF32* _XVelocityS, OUT BLF32* _YVelocityS, OUT BLU32* _CollisionType, OUT BLU32* _Group, OUT BLU32* _Category, OUT BLU32* _Mask)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	*_Mass = cpShapeGetMass(_shape);
	*_Density = cpShapeGetDensity(_shape);
	*_Sensor = cpShapeGetSensor(_shape);
	*_Elasticity = cpShapeGetElasticity(_shape);
	*_Friction = cpShapeGetFriction(_shape);
	*_XVelocityS = cpShapeGetSurfaceVelocity(_shape).x;
	*_YVelocityS = cpShapeGetSurfaceVelocity(_shape).y;
	*_CollisionType = cpShapeGetCollisionType(_shape);
	*_Group = cpShapeGetFilter(_shape).group;
	*_Category = cpShapeGetFilter(_shape).categories;
	*_Mask = cpShapeGetFilter(_shape).mask;
}