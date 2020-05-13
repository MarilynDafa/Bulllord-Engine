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
#define ARBITER_PAIR(A, B) ((BLU32)(A)*3344921057ul ^ (BLU32)(B)*3344921057ul)
typedef struct _BodyGuidNode {
	cpBody* pBody;
	BLGuid nID;
	BLVoid(*pVelocity)(BLGuid, BLF32, BLF32, BLF32, BLF32);
	BLVoid(*pPosition)(BLGuid, BLF32);
	BLVoid(*pConIteratorFunc)(BLGuid, BLGuid, BLVoid*);
	BLVoid(*pArbIteratorFunc)(BLGuid, BLGuid, BLVoid*);
}_BLBodyGuidNode;
typedef struct _ConstraintGuidNode {
	cpConstraint* pConstraint;
	BLGuid nID;
	BLVoid(*pPreSolveFunc)(BLGuid);
	BLVoid(*pPostSolveFunc)(BLGuid);
	BLF32(*pSpringForceFunc)(BLGuid, BLF32);
	BLF32(*pSpringTorqueFunc)(BLGuid, BLF32);
}_BLConstraintGuidNode;
typedef struct _ArbiterGuidNode {
	cpArbiter* pArbiter;
	BLGuid nID;
}_BLArbiterGuidNode;
typedef struct _QueryGuidNode {
	cpShape* pShape;
	BLGuid nID;
	BLVoid(*pPointQueryFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32, BLF32, BLVoid*);
	BLVoid(*pSegmentQueryFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32, BLF32, BLVoid*);
	BLVoid(*pBoxQueryFunc)(BLGuid, BLVoid*);
}_BLQueryGuidNode;
typedef struct _CollisionGuidNode {
	BLGuid nID;
	BLBool(*pBeginFunc)(BLGuid);
	BLBool(*pPreSolveFunc)(BLGuid);
	BLVoid(*pPostSolveFunc)(BLGuid);
	BLVoid(*pSeparateFunc)(BLGuid);
}_BLCollisionGuidNode;
typedef struct _ChipmunkMember {
	cpSpace* pSpace;
	_BLQueryGuidNode* pQuery;
	_BLCollisionGuidNode** pHandlers;
	BLU32 nHandlerCap;
	BLU32 nHandlerSize;
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
static void
_ConstraintIteratorFunc(cpBody* _Body, cpConstraint* _Constraint, void* _Data)
{
	_BLBodyGuidNode* _bnode = (_BLBodyGuidNode*)cpBodyGetUserData(_Body);
	_BLConstraintGuidNode* _cnode = (_BLConstraintGuidNode*)cpConstraintGetUserData(_Constraint);
	if (_bnode->pConIteratorFunc)
		_bnode->pConIteratorFunc(_bnode->nID, _cnode->nID, _Data);
}
static void
_ArbiterIteratorFunc(cpBody* _Body, cpArbiter* _Arbiter, void* _Data)
{
	_BLBodyGuidNode* _bnode = (_BLBodyGuidNode*)cpBodyGetUserData(_Body);
	_BLArbiterGuidNode* _anode = (_BLArbiterGuidNode*)cpArbiterGetUserData(_Arbiter);
	if (!_anode)
	{
		_anode = (_BLArbiterGuidNode*)malloc(sizeof(_BLArbiterGuidNode));
		_anode->pArbiter = _Arbiter;
		_anode->nID = blGenGuid(_Arbiter, blUniqueUri());
		cpArbiterSetUserData(_Arbiter, _anode);
	}
	if (_bnode->pArbIteratorFunc)
		_bnode->pArbIteratorFunc(_bnode->nID, _anode->nID, _Data);
}
static void
_ConstraintPreSolveFunc(cpConstraint* _Constraint, cpSpace* _Space)
{
	_BLConstraintGuidNode* _cgn = cpConstraintGetUserData(_Constraint);
	if (_cgn->pPreSolveFunc)
		_cgn->pPreSolveFunc(_cgn->nID);
}
static void
_ConstraintPostSolveFunc(cpConstraint* _Constraint, cpSpace* _Space)
{
	_BLConstraintGuidNode* _cgn = cpConstraintGetUserData(_Constraint);
	if (_cgn->pPostSolveFunc)
		_cgn->pPostSolveFunc(_cgn->nID);
}
static cpFloat
_ConstraintDampedSpringForceFunc(cpConstraint* _Constraint, cpFloat _Dist)
{
	_BLConstraintGuidNode* _cgn = cpConstraintGetUserData(_Constraint);
	if (_cgn->pSpringForceFunc)
		return _cgn->pSpringForceFunc(_cgn->nID, _Dist);
	return 0.f;
}
static cpFloat
_ConstraintDampedRotarySpringTorqueFunc(cpConstraint* _Constraint, cpFloat _Angle)
{
	_BLConstraintGuidNode* _cgn = cpConstraintGetUserData(_Constraint);
	if (_cgn->pSpringTorqueFunc)
		return _cgn->pSpringTorqueFunc(_cgn->nID, _Angle);
	return 0.f;
}
static void 
_PostConstraintFree(cpConstraint* _Constraint, cpSpace* _Space)
{
	_BLConstraintGuidNode* _node = cpConstraintGetUserData(_Constraint);
	cpSpaceRemoveConstraint(_Space, _node->pConstraint);
	cpConstraintFree(_node->pConstraint);
	free(_node);
}
static void 
_SpacePointQueryFunc(cpShape* _Shape, cpVect _Point, cpFloat _Distance, cpVect _Gradient, void* _Data)
{
	BLGuid* _id = cpShapeGetUserData(_Shape);
	_PrCpMem->pQuery->pPointQueryFunc(*_id, _Point.x, _Point.y, _Distance, _Gradient.x, _Gradient.y, _Data);
}
static void
_SpaceSegmentQueryFunc(cpShape* _Shape, cpVect _Point, cpVect _Normal, cpFloat _Alpha, void* _Data)
{
	BLGuid* _id = cpShapeGetUserData(_Shape);
	_PrCpMem->pQuery->pSegmentQueryFunc(*_id, _Point.x, _Point.y, _Normal.x, _Normal.y, _Alpha, _Data);
}
static void
_SpaceBoxQueryFunc(cpShape* _Shape, void* _Data)
{
	BLGuid* _id = cpShapeGetUserData(_Shape);
	_PrCpMem->pQuery->pBoxQueryFunc(*_id, _Data);
}
static cpBool
_CollisionBegin(cpArbiter* _Arb, cpSpace* _Space, void* _Data) 
{
	_BLArbiterGuidNode* _anode = (_BLArbiterGuidNode*)cpArbiterGetUserData(_Arb);
	if (!_anode)
	{
		_anode = (_BLArbiterGuidNode*)malloc(sizeof(_BLArbiterGuidNode));
		_anode->pArbiter = _Arb;
		_anode->nID = blGenGuid(_Arb, blUniqueUri());
		cpArbiterSetUserData(_Arb, _anode);
	}
	cpCollisionHandler* _handler = cpArbiterCollisionHandle(_Arb);
	if (!_handler)
		return cpArbiterIgnore(_Arb);
	_BLCollisionGuidNode* _node = (_BLCollisionGuidNode*)_handler->userData;
	if (_node->nID == INVALID_GUID)
		_node->nID = _anode->nID;
	if (_node->pBeginFunc)
		return _node->pBeginFunc(_node->nID);
	return FALSE;
}
static cpBool
_CollisionPreSolve(cpArbiter* _Arb, cpSpace* _Space, void* _Data) 
{
	_BLArbiterGuidNode* _anode = (_BLArbiterGuidNode*)cpArbiterGetUserData(_Arb);
	if (!_anode)
	{
		_anode = (_BLArbiterGuidNode*)malloc(sizeof(_BLArbiterGuidNode));
		_anode->pArbiter = _Arb;
		_anode->nID = blGenGuid(_Arb, blUniqueUri());
		cpArbiterSetUserData(_Arb, _anode);
	}
	cpCollisionHandler* _handler = cpArbiterCollisionHandle(_Arb);
	if (!_handler)
		return cpArbiterIgnore(_Arb);
	_BLCollisionGuidNode* _node = (_BLCollisionGuidNode*)_handler->userData;
	if (_node->nID == INVALID_GUID)
		_node->nID = _anode->nID;
	if (_node->pPreSolveFunc)
		return _node->pPreSolveFunc(_node->nID);
	return FALSE;
}
static void
_CollisionPostSolve(cpArbiter* _Arb, cpSpace* _Space, void* _Data)
{
	_BLArbiterGuidNode* _anode = (_BLArbiterGuidNode*)cpArbiterGetUserData(_Arb);
	if (!_anode)
	{
		_anode = (_BLArbiterGuidNode*)malloc(sizeof(_BLArbiterGuidNode));
		_anode->pArbiter = _Arb;
		_anode->nID = blGenGuid(_Arb, blUniqueUri());
		cpArbiterSetUserData(_Arb, _anode);
	}
	cpCollisionHandler* _handler = cpArbiterCollisionHandle(_Arb);
	if (!_handler)
		return;
	_BLCollisionGuidNode* _node = (_BLCollisionGuidNode*)_handler->userData;
	if (_node->nID == INVALID_GUID)
		_node->nID = _anode->nID;
	if (_node->pPostSolveFunc)
		_node->pPostSolveFunc(_node->nID);
}
static void
_CollisionSeparate(cpArbiter* _Arb, cpSpace* _Space, void* _Data)
{
	_BLArbiterGuidNode* _anode = (_BLArbiterGuidNode*)cpArbiterGetUserData(_Arb);
	if (!_anode)
	{
		_anode = (_BLArbiterGuidNode*)malloc(sizeof(_BLArbiterGuidNode));
		_anode->pArbiter = _Arb;
		_anode->nID = blGenGuid(_Arb, blUniqueUri());
		cpArbiterSetUserData(_Arb, _anode);
	}
	cpCollisionHandler* _handler = cpArbiterCollisionHandle(_Arb);
	if (!_handler)
		return;
	_BLCollisionGuidNode* _node = (_BLCollisionGuidNode*)_handler->userData;
	if (_node->nID == INVALID_GUID)
		_node->nID = _anode->nID;
	if (_node->pSeparateFunc)
		_node->pSeparateFunc(_node->nID);
}
static void
_FreeArbiter(void* _Data)
{
	_BLArbiterGuidNode* _arb = (_BLArbiterGuidNode*)_Data;
	blDeleteGuid(_arb->nID);
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
	{
		cpSpaceRemoveBody(_PrCpMem->pSpace, _body);
		cpBodyFree(_body);
	}
	cpSpaceRemoveShape(_PrCpMem->pSpace, _shape);
	cpShapeFree(_shape);
	return 2;
}
BLVoid
blChipmunkOpenEXT(IN BLAnsi* _Version, ...)
{
	_PrCpMem = (_BLChipmunkMember*)malloc(sizeof(_BLChipmunkMember));
	_PrCpMem->pSpace = cpSpaceNew();
	_PrCpMem->pQuery = (_BLQueryGuidNode*)malloc(sizeof(_BLQueryGuidNode));
	_PrCpMem->nHandlerCap = 10;
	_PrCpMem->nHandlerSize = 0;
	_PrCpMem->pHandlers = (_BLCollisionGuidNode**)malloc(sizeof(BLVoid*) * _PrCpMem->nHandlerCap);
	blSpriteRegistExternal("bmg", NULL, NULL, _UnloadCP, NULL, _BodyCp, _StepCP);
}
BLVoid
blChipmunkCloseEXT()
{
	for (BLU32 _i = 0; _i < _PrCpMem->nHandlerSize; ++_i)
	{
		blDeleteGuid(_PrCpMem->pHandlers[_i]->nID);
		free(_PrCpMem->pHandlers[_i]);
	}
	free(_PrCpMem->pHandlers);
	cpSpaceEachConstraint(_PrCpMem->pSpace, _PostConstraintFree, _PrCpMem->pSpace);
	cpSpaceDestroy(_PrCpMem->pSpace, _FreeArbiter);
	cpfree(_PrCpMem->pSpace);
	free(_PrCpMem->pQuery);
	free(_PrCpMem);
}
BLVoid 
blChipmunkSpaceUseSpatialHashEXT(IN BLF32 _Dim, IN BLS32 _Count)
{
	cpSpaceUseSpatialHash(_PrCpMem->pSpace, _Dim, _Count);
}
BLVoid 
blChipmunkSpaceIterationsEXT(IN BLU32 _Iterations)
{
	cpSpaceSetIterations(_PrCpMem->pSpace, _Iterations);
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
blChipmunkSpaceDampingEXT(IN BLF32 _Damping)
{
	cpSpaceSetDamping(_PrCpMem->pSpace, _Damping);
}
BLVoid 
blChipmunkSpaceSpeedThresholdEXT(IN BLF32 _Threshold)
{
	cpSpaceSetIdleSpeedThreshold(_PrCpMem->pSpace, _Threshold);
}
BLVoid 
blChipmunkSpaceSleepTimeThresholdEXT(IN BLF32 _Threshold)
{
	cpSpaceSetSleepTimeThreshold(_PrCpMem->pSpace, _Threshold);
}
BLVoid 
blChipmunkSpaceCollisionSlopEXT(IN BLF32 _Slop)
{
	cpSpaceSetCollisionSlop(_PrCpMem->pSpace, _Slop);
}
BLVoid 
blChipmunkSpaceCollisionBiasEXT(IN BLF32 _Bias)
{
	cpSpaceSetCollisionBias(_PrCpMem->pSpace, _Bias);
}
BLVoid 
blChipmunkSpaceCollisionPersistenceEXT(IN BLU32 _Persistence)
{
	cpSpaceSetCollisionPersistence(_PrCpMem->pSpace, _Persistence);
}
BLVoid 
blChipmunkSpaceStateQueryEXT(OUT BLU32* _Iterations, OUT BLF32* _GravityX, OUT BLF32* _GravityY, OUT BLF32* _Damping, OUT BLF32* _SpeedThreshold, OUT BLF32* _SleepThreshold, OUT BLF32* _Slop, OUT BLF32* _Bias, OUT BLU32* _Persistence, OUT BLF32* _TimeStep, OUT BLBool* _Locked)
{
	*_Iterations = cpSpaceGetIterations(_PrCpMem->pSpace);
	cpVect _ret = cpSpaceGetGravity(_PrCpMem->pSpace);
	*_GravityX = _ret.x;
	*_GravityY = _ret.y;
	*_Damping = cpSpaceGetDamping(_PrCpMem->pSpace);
	*_SpeedThreshold = cpSpaceGetIdleSpeedThreshold(_PrCpMem->pSpace);
	*_SleepThreshold = cpSpaceGetSleepTimeThreshold(_PrCpMem->pSpace);
	*_Slop = cpSpaceGetCollisionSlop(_PrCpMem->pSpace);
	*_Bias = cpSpaceGetCollisionBias(_PrCpMem->pSpace);
	*_Persistence = cpSpaceGetCollisionPersistence(_PrCpMem->pSpace);
	*_TimeStep = cpSpaceGetCurrentTimeStep(_PrCpMem->pSpace);
	*_Locked = cpSpaceIsLocked(_PrCpMem->pSpace);
}
BLVoid 
blChipmunkSpacePointQueryEXT(IN BLF32 _PointX, IN BLF32 _PointY, IN BLF32 _MaxDistance, IN BLU32 _Group, IN BLU32 _Category, IN BLU32 _Mask, IN BLVoid(*_QueryFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32, BLF32, BLVoid*), IN BLVoid* _Data)
{
	_PrCpMem->pQuery->pPointQueryFunc = _QueryFunc;
	cpShapeFilter _filter;
	_filter.group = _Group;
	_filter.categories = _Category;
	_filter.mask = _Mask;
	cpSpacePointQuery(_PrCpMem->pSpace, cpv(_PointX, _PointY), _MaxDistance, _filter, _SpacePointQueryFunc, _Data);
}
BLGuid 
blChipmunkSpacePointNearestQueryEXT(IN BLF32 _PointX, IN BLF32 _PointY, IN BLF32 _MaxDistance, IN BLU32 _Group, IN BLU32 _Category, IN BLU32 _Mask, OUT BLF32* _OutX, OUT BLF32* _OutY, OUT BLF32* _Distance, OUT BLF32* _GradientX, OUT BLF32* _GradientY)
{
	cpShapeFilter _filter;
	_filter.group = _Group;
	_filter.categories = _Category;
	_filter.mask = _Mask;
	cpPointQueryInfo _out;
	cpShape* _sp = cpSpacePointQueryNearest(_PrCpMem->pSpace, cpv(_PointX, _PointY), _MaxDistance, _filter, &_out);
	*_OutX = _out.point.x;
	*_OutY = _out.point.y;
	*_Distance = _out.distance;
	*_GradientX = _out.gradient.x;
	*_GradientY = _out.gradient.y;
	BLGuid* _id = cpShapeGetUserData(_sp);
	if (_id)
		return *_id;
	else
		return INVALID_GUID;
}
BLVoid 
blChipmunkSpaceSegmentQueryEXT(IN BLF32 _StartX, IN BLF32 _StartY, IN BLF32 _EndX, IN BLF32 _ExdY, IN BLF32 _Radius, IN BLU32 _Group, IN BLU32 _Category, IN BLU32 _Mask, IN BLVoid(*_QueryFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32, BLF32, BLVoid*), IN BLVoid* _Data)
{
	_PrCpMem->pQuery->pSegmentQueryFunc = _QueryFunc;
	cpShapeFilter _filter;
	_filter.group = _Group;
	_filter.categories = _Category;
	_filter.mask = _Mask;
	cpSpaceSegmentQuery(_PrCpMem->pSpace, cpv(_StartX, _StartY), cpv(_EndX, _ExdY), _Radius, _filter, _SpaceSegmentQueryFunc, _Data);
}
BLGuid 
blChipmunkSpaceSegmentFirstQueryEXT(IN BLF32 _StartX, IN BLF32 _StartY, IN BLF32 _EndX, IN BLF32 _ExdY, IN BLF32 _Radius, IN BLU32 _Group, IN BLU32 _Category, IN BLU32 _Mask, OUT BLF32* _PointX, OUT BLF32* _PointY, OUT BLF32* _NormalX, OUT BLF32* _NormalY, OUT BLF32* _Alpha)
{
	cpShapeFilter _filter;
	_filter.group = _Group;
	_filter.categories = _Category;
	_filter.mask = _Mask;
	cpSegmentQueryInfo _out;
	cpShape* _sp = cpSpaceSegmentQueryFirst(_PrCpMem->pSpace, cpv(_StartX, _StartY), cpv(_EndX, _ExdY), _Radius, _filter, &_out);
	*_PointX = _out.point.x;
	*_PointY = _out.point.y;
	*_NormalX = _out.normal.x;
	*_NormalY = _out.normal.y;
	*_Alpha = _out.alpha;
	BLGuid* _id = cpShapeGetUserData(_sp);
	if (_id)
		return *_id;
	else
		return INVALID_GUID;
}
BLVoid 
blChipmunkSpaceBoxQueryEXT(IN BLF32 _MinX, IN BLF32 _MinY, IN BLF32 _MaxX, IN BLF32 _MaxY, IN BLU32 _Group, IN BLU32 _Category, IN BLU32 _Mask, IN BLVoid(*_QueryFunc)(BLGuid, BLVoid*), IN BLVoid* _Data)
{
	_PrCpMem->pQuery->pBoxQueryFunc = _QueryFunc;
	cpBB _box;
	_box.l = _MinX;
	_box.t = _MinY;
	_box.r = _MaxX;
	_box.b = _MaxY;
	cpShapeFilter _filter;
	_filter.group = _Group;
	_filter.categories = _Category;
	_filter.mask = _Mask;
	cpSpaceBBQuery(_PrCpMem->pSpace, _box, _filter, _SpaceBoxQueryFunc, _Data);
}
BLVoid 
blChipmunkSpaceCollisionHandler(IN BLU32 _TypeA, IN BLU32 _TypeB, IN BLBool(*_BeginFunc)(BLGuid), IN BLBool(*_PreSolveFunc)(BLGuid), IN BLVoid(*_PostSolveFunc)(BLGuid), IN BLVoid(*_SeparateFunc)(BLGuid))
{
	cpCollisionHandler* _handler = cpSpaceAddCollisionHandler(_PrCpMem->pSpace, _TypeA, _TypeB);
	if (_BeginFunc)
		_handler->beginFunc = _CollisionBegin;
	if (_PreSolveFunc)
		_handler->preSolveFunc = _CollisionPreSolve;
	if (_PostSolveFunc)
		_handler->postSolveFunc = _CollisionPostSolve;
	if (_SeparateFunc)
		_handler->separateFunc = _CollisionSeparate;
	_BLCollisionGuidNode* _node = (_BLCollisionGuidNode*)malloc(sizeof(_BLCollisionGuidNode));
	_node->nID = INVALID_GUID;
	_node->pBeginFunc = _BeginFunc;
	_node->pPreSolveFunc = _PreSolveFunc;
	_node->pPostSolveFunc = _PostSolveFunc;
	_node->pSeparateFunc = _SeparateFunc;
	_handler->userData = _node;
	if (_PrCpMem->nHandlerSize + 1 > _PrCpMem->nHandlerCap)
	{
		_PrCpMem->nHandlerCap += 10;
		_PrCpMem->pHandlers = (_BLCollisionGuidNode**)realloc(_PrCpMem->pHandlers, sizeof(BLVoid*) * _PrCpMem->nHandlerCap);
	}
	_PrCpMem->pHandlers[_PrCpMem->nHandlerSize] = _node;
}
BLVoid
blChipmunkSpaceWildcardHandler(IN BLU32 _Type, IN BLBool(*_BeginFunc)(BLGuid), IN BLBool(*_PreSolveFunc)(BLGuid), IN BLVoid(*_PostSolveFunc)(BLGuid), IN BLVoid(*_SeparateFunc)(BLGuid))
{
	cpCollisionHandler* _handler = cpSpaceAddWildcardHandler(_PrCpMem->pSpace, _Type);
	if (_BeginFunc)
		_handler->beginFunc = _CollisionBegin;
	if (_PreSolveFunc)
		_handler->preSolveFunc = _CollisionPreSolve;
	if (_PostSolveFunc)
		_handler->postSolveFunc = _CollisionPostSolve;
	if (_SeparateFunc)
		_handler->separateFunc = _CollisionSeparate;
	_BLCollisionGuidNode* _node = (_BLCollisionGuidNode*)malloc(sizeof(_BLCollisionGuidNode));
	_node->nID = INVALID_GUID;
	_node->pBeginFunc = _BeginFunc;
	_node->pPreSolveFunc = _PreSolveFunc;
	_node->pPostSolveFunc = _PostSolveFunc;
	_node->pSeparateFunc = _SeparateFunc;
	_handler->userData = _node;
	if (_PrCpMem->nHandlerSize + 1 > _PrCpMem->nHandlerCap)
	{
		_PrCpMem->nHandlerCap += 10;
		_PrCpMem->pHandlers = (_BLCollisionGuidNode**)realloc(_PrCpMem->pHandlers, sizeof(BLVoid*) * _PrCpMem->nHandlerCap);
	}
	_PrCpMem->pHandlers[_PrCpMem->nHandlerSize] = _node;
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
blChipmunkMomentBoxEXT(IN BLF32 _M, IN BLF32 _MinX, IN BLF32 _MaxX, IN BLF32 _MinY, IN BLF32 _MaxY)
{
	return cpMomentForBox2(_M, cpBBNew(_MinX, _MaxY, _MaxX, _MinY));
}
BLF32
blChipmunkAreaBoxEXT(IN BLF32 _MinX, IN BLF32 _MaxX, IN BLF32 _MinY, IN BLF32 _MaxY)
{
	return (_MaxX - _MinX) * (_MaxY - _MinY);
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
	cpShapeSetUserData(_shape, &_ID);
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
	cpShapeSetUserData(_shape, &_ID);
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
	cpShapeSetUserData(_shape, &_ID); 
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
	cpShapeSetUserData(_shape, &_ID); 
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _kinematic;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_kinematic, _node);
	cpShapeSetUserData(_shape, &_ID);
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _kinematic;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_kinematic, _node);
	cpShapeSetUserData(_shape, &_ID);
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _kinematic;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_kinematic, _node);
	cpShapeSetUserData(_shape, &_ID);
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _kinematic;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_kinematic, _node);
	cpShapeSetUserData(_shape, &_ID);
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _body;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_body, _node);
	cpShapeSetUserData(_shape, &_ID);
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _body;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_body, _node);
	cpShapeSetUserData(_shape, &_ID);
	blSpriteExternalData(_ID, _shape);
	return TRUE;
}
BLBool 
blChipmunkSpriteDynamicBoxBodyEXT(IN BLGuid _ID, IN BLF32 _Mass, IN BLF32 _Moment, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Radius, IN BLF32 _X, IN BLF32 _Y)
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
	_shape = cpSpaceAddShape(_PrCpMem->pSpace, cpBoxShapeNew2(_body, cpBBNew(_X - _Width * 0.5f, _Y - _Height * 0.5f, _X + _Width * 0.5f, _Y + _Height * 0.5f), _Radius));
	cpSpaceAddBody(_PrCpMem->pSpace, _body);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _body;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_body, _node);
	cpShapeSetUserData(_shape, &_ID);
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
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)malloc(sizeof(_BLBodyGuidNode));
	_node->pBody = _body;
	_node->nID = _ID;
	_node->pVelocity = NULL;
	_node->pPosition = NULL;
	_node->pConIteratorFunc = NULL;
	_node->pArbIteratorFunc = NULL;
	cpBodySetUserData(_body, _node);
	cpShapeSetUserData(_shape, &_ID);
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
blChipmunkSpriteQuantitiesQueryEXT(IN BLGuid _ID, OUT BLF32* _Mass, OUT BLF32* _Moment, OUT BLF32* _CenterX, OUT BLF32* _CenterY, OUT BLF32* _VelocityX, OUT BLF32* _VelocityY, OUT BLF32* _VelocityA, OUT BLF32* _ForceX, OUT BLF32* _ForceY, OUT BLF32* _Torque, OUT BLF32* _Energy)
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
	*_Energy = cpBodyKineticEnergy(_body);
	return TRUE;
}
BLVoid 
blChipmunkSpriteVelocityUpdateFuncEXT(IN BLGuid _ID, IN BLVoid(*_VelocityFunc)(BLGuid, BLF32, BLF32, BLF32, BLF32))
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)cpBodyGetUserData(_body); 
	if (_node)
		return;
	_node->pBody = _body;
	_node->nID = _ID;
	if (_VelocityFunc)
		_node->pVelocity = _VelocityFunc;
	cpBodySetVelocityUpdateFunc(_body, _VelocityUpdateFunc);
}
BLVoid 
blChipmunkSpritePositionUpdateFuncEXT(IN BLGuid _ID, IN BLVoid(*_PositionFunc)(BLGuid, BLF32))
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)cpBodyGetUserData(_body);
	if (_node)
		return;
	_node->pBody = _body;
	_node->nID = _ID;
	if (_PositionFunc)
		_node->pPosition = _PositionFunc;
	cpBodySetPositionUpdateFunc(_body, _PositionUpdateFunc);
}
BLVoid 
blChipmunkSpriteUpdateVelocityEXT(IN BLGuid _ID, IN BLF32 _GravityX, IN BLF32 _GravityY, IN BLF32 _Damping, IN BLF32 _Dt)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyUpdateVelocity(_body, cpv(_GravityX, _GravityY), _Damping, _Dt);
}
BLVoid 
blChipmunkSpriteUpdatePositionEXT(IN BLGuid _ID, IN BLF32 _Dt)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyUpdatePosition(_body, _Dt);
}
BLVoid 
blChipmunkSpriteLocalToWorldEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _OX, OUT BLF32* _OY)
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
blChipmunkSpriteWorldToLocalEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _OX, OUT BLF32* _OY)
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
blChipmunkSpriteApplyForceAtWorldEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ForceX, IN BLF32 _ForceY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyForceAtWorldPoint(_body, cpv(_ForceX, _ForceY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteApplyForceAtLocalEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ForceX, IN BLF32 _ForceY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyForceAtLocalPoint(_body, cpv(_ForceX, _ForceY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteApplyImpulseAtWorldEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ImpulseX, IN BLF32 _ImpulseY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyImpulseAtWorldPoint(_body, cpv(_ImpulseX, _ImpulseY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteApplyImpulseAtLocalEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, IN BLF32 _ImpulseX, IN BLF32 _ImpulseY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpBodyApplyImpulseAtLocalPoint(_body, cpv(_ImpulseX, _ImpulseY), cpv(_X, _Y));
}
BLVoid 
blChipmunkSpriteVelocityAtWorldEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _VelX, OUT BLF32* _VelY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpVect _v = cpBodyGetVelocityAtWorldPoint(_body, cpv(_X, _Y));
	*_VelX = _v.x;
	*_VelY = _v.y;
}
BLVoid 
blChipmunkSpriteVelocityAtLocalEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _VelX, OUT BLF32* _VelY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	cpVect _v = cpBodyGetVelocityAtLocalPoint(_body, cpv(_X, _Y));
	*_VelX = _v.x;
	*_VelY = _v.y;
}
BLVoid 
blChipmunkSpriteEachConstraintEXT(IN BLGuid _ID, IN BLVoid(*_IteratorFunc)(BLGuid, BLGuid, BLVoid*), IN BLVoid* _Data)
{
	if (!_IteratorFunc)
		return;
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)cpBodyGetUserData(_body);
	_node->pConIteratorFunc = _IteratorFunc;
	cpBodyEachConstraint(_body, _ConstraintIteratorFunc, _Data);
}
BLVoid 
blChipmunkSpriteEachArbiterEXT(IN BLGuid _ID, IN BLVoid(*_IteratorFunc)(BLGuid, BLGuid, BLVoid*), IN BLVoid* _Data)
{
	if (!_IteratorFunc)
		return;
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	if (!_shape)
		return;
	cpBody* _body = cpShapeGetBody(_shape);
	_BLBodyGuidNode* _node = (_BLBodyGuidNode*)cpBodyGetUserData(_body);
	_node->pArbIteratorFunc = _IteratorFunc;
	cpBodyEachArbiter(_body, _ArbiterIteratorFunc, _Data);
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
blChipmunkShapeGetPolyParamEXT(IN BLGuid _ID, IN BLU32 _Index, OUT BLF32* _Radius, OUT BLU32* _Count, OUT BLF32* _X, OUT BLF32* _Y)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	*_Radius = cpPolyShapeGetRadius(_shape);
	*_Count = cpPolyShapeGetCount(_shape);
	*_X = cpPolyShapeGetVert(_shape, _Index).x;
	*_Y = cpPolyShapeGetVert(_shape, _Index).y;
}
BLVoid 
blChipmunkShapeGetSegmentParamEXT(IN BLGuid _ID, OUT BLF32* _AX, OUT BLF32* _AY, OUT BLF32* _BX, OUT BLF32* _BY, OUT BLF32* _NormalX, OUT BLF32* _NormalY, OUT BLF32* _Radius)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	*_AX = cpSegmentShapeGetA(_shape).x;
	*_AY = cpSegmentShapeGetA(_shape).y;
	*_BX = cpSegmentShapeGetB(_shape).x;
	*_BY = cpSegmentShapeGetB(_shape).y;
	*_NormalX = cpSegmentShapeGetNormal(_shape).x;
	*_NormalY = cpSegmentShapeGetNormal(_shape).y;
	*_Radius = cpSegmentShapeGetRadius(_shape);
}
BLVoid 
blChipmunkShapeQuantitiesQueryEXT(IN BLGuid _ID, OUT BLF32* _Mass, OUT BLF32* _Density, OUT BLBool* _Sensor, OUT BLF32* _Elasticity, OUT BLF32* _Friction, OUT BLF32* _XVelocityS, OUT BLF32* _YVelocityS, OUT BLU32* _CollisionType, OUT BLU32* _Group, OUT BLU32* _Category, OUT BLU32* _Mask, OUT BLF32* _BBMinX, OUT BLF32* _BBMaxX, OUT BLF32* _BBMinY, OUT BLF32* _BBMaxY)
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
	*_BBMinX = cpShapeGetBB(_shape).l;
	*_BBMaxX = cpShapeGetBB(_shape).r;
	*_BBMinY = cpShapeGetBB(_shape).t;
	*_BBMaxY = cpShapeGetBB(_shape).b;
}
BLGuid 
blChipmunkShapePointQueryEXT(IN BLGuid _ID, IN BLF32 _X, IN BLF32 _Y, OUT BLF32* _PtX, OUT BLF32* _PtY, OUT BLF32* _Distance, OUT BLF32* _GradientX, OUT BLF32* _GradientY)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpPointQueryInfo _info;
	cpShapePointQuery(_shape, cpv(_X, _Y), &_info);
	*_PtX = _info.point.x;
	*_PtY = _info.point.y;
	*_Distance = _info.distance;
	*_GradientX = _info.gradient.x;
	*_GradientY = _info.gradient.y;
	BLGuid* _id = cpShapeGetUserData(_info.shape);
	return *_id;
}
BLGuid 
blChipmunkShapeSegmentQueryEXT(IN BLGuid _ID, IN BLF32 _AX, IN BLF32 _AY, IN BLF32 _BX, IN BLF32 _BY, IN BLF32 _Radius, OUT BLF32* _PtX, OUT BLF32* _PtY, OUT BLF32* _NormalX, OUT BLF32* _NormalY, OUT BLF32* _Alpha)
{
	cpShape* _shape = (cpShape*)blSpriteExternalData(_ID, NULL);
	cpSegmentQueryInfo _info;
	cpShapeSegmentQuery(_shape, cpv(_AX, _AY), cpv(_BX, _BY), _Radius, &_info);
	*_PtX = _info.point.x;
	*_PtY = _info.point.y;
	*_NormalX = _info.normal.x;
	*_NormalY = _info.normal.y;
	*_Alpha = _info.alpha;
	BLGuid* _id = cpShapeGetUserData(_info.shape);
	return *_id;
}
BLGuid 
blChipmunkConstraintGearGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _Phase, IN BLF32 _Ratio)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else 
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpGearJointNew(_abody, _bbody, _Phase, _Ratio);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintGrooveGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _AGrooveX, IN BLF32 _AGrooveY, IN BLF32 _BGrooveX, IN BLF32 _BGrooveY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpGrooveJointNew(_abody, _bbody, cpv(_AGrooveX, _AGrooveY), cpv(_BGrooveX, _BGrooveY), cpv(_BAnchorX, _BAnchorY));
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintPinGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpPinJointNew(_abody, _bbody, cpv(_AAnchorX, _AAnchorY), cpv(_BAnchorX, _BAnchorY));
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintPivotGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _APivotX, IN BLF32 _APivotY, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY, IN BLBool _UsePivot)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con;
	if (_UsePivot)
		_con = cpPivotJointNew(_abody, _bbody, cpv(_APivotX, _APivotY));
	else
		_con = cpPivotJointNew2(_abody, _bbody, cpv(_AAnchorX, _AAnchorY), cpv(_BAnchorX, _BAnchorY));
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintRatchetGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _Phase, IN BLF32 _Ratchet)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpRatchetJointNew(_abody, _bbody, _Phase, _Ratchet);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintRotaryGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _Min, IN BLF32 _Max)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpRotaryLimitJointNew(_abody, _bbody, _Min, _Max);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintSlideGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY, IN BLF32 _Min, IN BLF32 _Max)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpSlideJointNew(_abody, _bbody, cpv(_AAnchorX, _AAnchorY), cpv(_BAnchorX, _BAnchorY), _Min, _Max);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintSpringGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY, IN BLF32 _RestLength, IN BLF32 _Stiffness, IN BLF32 _Damping)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpDampedSpringNew(_abody, _bbody, cpv(_AAnchorX, _AAnchorY), cpv(_BAnchorX, _BAnchorY), _RestLength, _Stiffness, _Damping);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintRotarySpringGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLS32 _RestAngle, IN BLF32 _Stiffness, IN BLF32 _Damping)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpDampedRotarySpringNew(_abody, _bbody, -_RestAngle * 0.0174532922222222f, _Stiffness, _Damping);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLGuid 
blChipmunkConstraintMotorGenEXT(IN BLGuid _A, IN BLGuid _B, IN BLF32 _Rate)
{
	cpBody* _abody;
	cpBody* _bbody;
	if (_A == INVALID_GUID)
		_abody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _ashape = (cpShape*)blSpriteExternalData(_A, NULL);
		_abody = cpShapeGetBody(_ashape);
	}
	if (_B == INVALID_GUID)
		_bbody = cpSpaceGetStaticBody(_PrCpMem->pSpace);
	else
	{
		cpShape* _bshape = (cpShape*)blSpriteExternalData(_B, NULL);
		_bbody = cpShapeGetBody(_bshape);
	}
	if (!_abody || !_bbody)
		return INVALID_GUID;
	cpConstraint* _con = cpSimpleMotorNew(_abody, _bbody, _Rate);
	cpSpaceAddConstraint(_PrCpMem->pSpace, _con);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)malloc(sizeof(_BLConstraintGuidNode));
	_node->pConstraint = _con;
	_node->nID = blGenGuid(_con, blUniqueUri());
	_node->pPostSolveFunc = NULL;
	_node->pPreSolveFunc = NULL;
	_node->pSpringForceFunc = NULL;
	_node->pSpringTorqueFunc = NULL;
	cpConstraintSetUserData(_con, _node);
	return _node->nID;
}
BLVoid 
blChipmunkConstraintDeleteEXT(IN BLGuid _Constraint)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	_BLConstraintGuidNode* _node = cpConstraintGetUserData(_con);
	cpSpaceRemoveConstraint(_PrCpMem->pSpace, _node->pConstraint);
	cpConstraintFree(_node->pConstraint);
	blDeleteGuid(_node->nID);
	free(_node);
}
BLVoid 
blChipmunkConstraintMaxForceEXT(IN BLGuid _Constraint, IN BLF32 _MaxForce)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpConstraintSetMaxForce(_con, _MaxForce);
}
BLVoid 
blChipmunkConstraintErrorBiasEXT(IN BLGuid _Constraint, IN BLF32 _ErrorBias)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpConstraintSetErrorBias(_con, _ErrorBias);
}
BLVoid 
blChipmunkConstraintMaxBiasEXT(IN BLGuid _Constraint, IN BLF32 _MaxBias)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpConstraintSetMaxBias(_con, _MaxBias);
}
BLVoid 
blChipmunkConstraintCollideBodiesEXT(IN BLGuid _Constraint, IN BLBool _CollideBodies)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpConstraintSetCollideBodies(_con, _CollideBodies);
}
BLVoid 
blChipmunkConstraintGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _MaxForce, OUT BLF32* _ErrorBias, OUT BLF32* _MaxBias, OUT BLBool* _CollideBodies, OUT BLF32* _Impulse)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_MaxForce = cpConstraintGetMaxForce(_con);
	*_ErrorBias = cpConstraintGetErrorBias(_con);
	*_MaxBias = cpConstraintGetMaxBias(_con);
	*_CollideBodies = cpConstraintGetCollideBodies(_con);
	*_Impulse = cpConstraintGetImpulse(_con);
}
BLVoid
blChipmunkConstraintGearParamEXT(IN BLGuid _Constraint, IN BLF32 _Phase, IN BLF32 _Ratio)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpGearJointSetPhase(_con, _Phase);
	cpGearJointSetRatio(_con, _Ratio);
}
BLVoid 
blChipmunkConstraintGearGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _Phase, OUT BLF32* _Ratio)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_Phase = cpGearJointGetPhase(_con);
	*_Ratio = cpGearJointGetRatio(_con);
}
BLVoid
blChipmunkConstraintGrooveParamEXT(IN BLGuid _Constraint, IN BLF32 _AGrooveX, IN BLF32 _AGrooveY, IN BLF32 _BGrooveX, IN BLF32 _BGrooveY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpGrooveJointSetGrooveA(_con, cpv(_AGrooveX, _AGrooveY));
	cpGrooveJointSetGrooveB(_con, cpv(_BGrooveX, _BGrooveY));
	cpGrooveJointSetAnchorB(_con, cpv(_BAnchorX, _BAnchorY));
}
BLVoid 
blChipmunkConstraintGrooveGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _AGrooveX, OUT BLF32* _AGrooveY, OUT BLF32* _BGrooveX, OUT BLF32* _BGrooveY, OUT BLF32* _BAnchorX, OUT BLF32* _BAnchorY)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_AGrooveX = cpGrooveJointGetGrooveA(_con).x;
	*_AGrooveY = cpGrooveJointGetGrooveA(_con).y;
	*_BGrooveX = cpGrooveJointGetGrooveB(_con).x;
	*_BGrooveY = cpGrooveJointGetGrooveB(_con).y;
	*_BAnchorX = cpGrooveJointGetAnchorB(_con).x;
	*_BAnchorY = cpGrooveJointGetAnchorB(_con).y;
}
BLVoid
blChipmunkConstraintPinParamEXT(IN BLGuid _Constraint, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY, IN BLF32 _Dist)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint); 
	cpPinJointSetAnchorA(_con, cpv(_AAnchorX, _AAnchorY));
	cpPinJointSetAnchorB(_con, cpv(_BAnchorX, _BAnchorY));
	cpPinJointSetDist(_con, _Dist);
}
BLVoid 
blChipmunkConstraintPinGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _AAnchorX, OUT BLF32* _AAnchorY, OUT BLF32* _BAnchorX, OUT BLF32* _BAnchorY, OUT BLF32* _Dist)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_AAnchorX = cpPinJointGetAnchorA(_con).x;
	*_AAnchorY = cpPinJointGetAnchorA(_con).y;
	*_BAnchorX = cpPinJointGetAnchorB(_con).x;
	*_BAnchorY = cpPinJointGetAnchorB(_con).y;
	*_Dist = cpPinJointGetDist(_con);
}
BLVoid
blChipmunkConstraintPivotParamEXT(IN BLGuid _Constraint, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpPivotJointSetAnchorA(_con, cpv(_AAnchorX, _AAnchorY));
	cpPivotJointSetAnchorB(_con, cpv(_BAnchorX, _BAnchorY));
}
BLVoid 
blChipmunkConstraintPivotGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _AAnchorX, OUT BLF32* _AAnchorY, OUT BLF32* _BAnchorX, OUT BLF32* _BAnchorY)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_AAnchorX = cpPivotJointGetAnchorA(_con).x;
	*_AAnchorY = cpPivotJointGetAnchorA(_con).y;
	*_BAnchorX = cpPivotJointGetAnchorB(_con).x;
	*_BAnchorY = cpPivotJointGetAnchorB(_con).y;
}
BLVoid 
blChipmunkConstraintRatchetParamEXT(IN BLGuid _Constraint, IN BLS32 _Angle, IN BLF32 _Phase, IN BLF32 _Ratchet)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpRatchetJointSetAngle(_con, -_Angle * 0.0174532922222222f);
	cpRatchetJointSetPhase(_con, _Phase);
	cpRatchetJointSetRatchet(_con, _Ratchet);
}
BLVoid 
blChipmunkConstraintRatchetGetParamEXT(IN BLGuid _Constraint, OUT BLS32* _Angle, OUT BLF32* _Phase, OUT BLF32* _Ratchet)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_Angle = (BLS32)(cpRatchetJointGetAngle(_con) / -0.0174532922222222f);
	*_Phase = cpRatchetJointGetPhase(_con);
	*_Ratchet = cpRatchetJointGetRatchet(_con);
}
BLVoid
blChipmunkConstraintRotaryParamEXT(IN BLGuid _Constraint, IN BLF32 _Min, IN BLF32 _Max)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpRotaryLimitJointSetMin(_con, _Min);
	cpRotaryLimitJointSetMax(_con, _Max);
}
BLVoid 
blChipmunkConstraintRotaryGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _Min, OUT BLF32* _Max)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_Min = cpRotaryLimitJointGetMin(_con);
	*_Max = cpRotaryLimitJointGetMax(_con);
}
BLVoid 
blChipmunkConstraintSlideParamEXT(IN BLGuid _Constraint, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY, IN BLF32 _Min, IN BLF32 _Max)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpSlideJointSetAnchorA(_con, cpv(_AAnchorX, _AAnchorY));
	cpSlideJointSetAnchorB(_con, cpv(_BAnchorX, _BAnchorY));
	cpSlideJointSetMin(_con, _Min);
	cpSlideJointSetMax(_con, _Max);
}
BLVoid 
blChipmunkConstraintSlideGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _AAnchorX, OUT BLF32* _AAnchorY, OUT BLF32* _BAnchorX, OUT BLF32* _BAnchorY, OUT BLF32* _Min, OUT BLF32* _Max)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_AAnchorX = cpSlideJointGetAnchorA(_con).x;
	*_AAnchorY = cpSlideJointGetAnchorA(_con).y;
	*_BAnchorX = cpSlideJointGetAnchorB(_con).x;
	*_BAnchorY = cpSlideJointGetAnchorB(_con).y;
	*_Min = cpSlideJointGetMin(_con);
	*_Max = cpSlideJointGetMax(_con);
}
BLVoid
blChipmunkConstraintSpringParamEXT(IN BLGuid _Constraint, IN BLF32 _AAnchorX, IN BLF32 _AAnchorY, IN BLF32 _BAnchorX, IN BLF32 _BAnchorY, IN BLF32 _RestLength, IN BLF32 _Stiffness, IN BLF32 _Damping)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpDampedSpringSetAnchorA(_con, cpv(_AAnchorX, _AAnchorY));
	cpDampedSpringSetAnchorB(_con, cpv(_BAnchorX, _BAnchorY));
	cpDampedSpringSetRestLength(_con, _RestLength);
	cpDampedSpringSetStiffness(_con, _Stiffness);
	cpDampedSpringSetDamping(_con, _Damping);
}
BLVoid 
blChipmunkConstraintSpringGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _AAnchorX, OUT BLF32* _AAnchorY, OUT BLF32* _BAnchorX, OUT BLF32* _BAnchorY, OUT BLF32* _RestLength, OUT BLF32* _Stiffness, OUT BLF32* _Damping)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_AAnchorX = cpDampedSpringGetAnchorA(_con).x;
	*_AAnchorY = cpDampedSpringGetAnchorA(_con).y;
	*_BAnchorX = cpDampedSpringGetAnchorB(_con).x;
	*_BAnchorY = cpDampedSpringGetAnchorB(_con).y;
	*_RestLength = cpDampedSpringGetRestLength(_con);
	*_Stiffness = cpDampedSpringGetStiffness(_con);
	*_Damping = cpDampedSpringGetDamping(_con);
}
BLVoid
blChipmunkConstraintRotarySpringParamEXT(IN BLGuid _Constraint, IN BLS32 _RestAngle, IN BLF32 _Stiffness, IN BLF32 _Damping)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpDampedRotarySpringSetRestAngle(_con, -_RestAngle * 0.0174532922222222f);
	cpDampedRotarySpringSetStiffness(_con, _Stiffness);
	cpDampedRotarySpringSetDamping(_con, _Damping);
}
BLVoid 
blChipmunkConstraintRotarySpringGetParamEXT(IN BLGuid _Constraint, OUT BLS32* _RestAngle, OUT BLF32* _Stiffness, OUT BLF32* _Damping)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_RestAngle = (BLS32)(cpDampedRotarySpringGetRestAngle(_con) / -0.0174532922222222f);
	*_Stiffness = cpDampedRotarySpringGetStiffness(_con);
	*_Damping = cpDampedRotarySpringGetDamping(_con);
}
BLVoid
blChipmunkConstraintMotorParamEXT(IN BLGuid _Constraint, IN BLF32 _Rate)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	cpSimpleMotorSetRate(_con, _Rate);
}
BLVoid 
blChipmunkConstraintMotorGetParamEXT(IN BLGuid _Constraint, OUT BLF32* _Rate)
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	*_Rate = cpSimpleMotorGetRate(_con);
}
BLVoid
blChipmunkConstraintSolveFuncEXT(IN BLGuid _Constraint, IN BLVoid(*_PreSolveFunc)(BLGuid), IN BLVoid(*_PostSolveFunc)(BLGuid), IN BLF32(*_SpringForceFunc)(BLGuid, BLF32), IN BLF32(*_SpringTorqueFunc)(BLGuid, BLF32))
{
	cpConstraint* _con = blGuidAsPointer(_Constraint);
	_BLConstraintGuidNode* _node = (_BLConstraintGuidNode*)cpConstraintGetUserData(_con);
	if (_PreSolveFunc)
	{
		_node->pPreSolveFunc = _PreSolveFunc;
		cpConstraintSetPreSolveFunc(_con, _ConstraintPreSolveFunc);
	}
	if (_PostSolveFunc)
	{
		_node->pPostSolveFunc = _PostSolveFunc;
		cpConstraintSetPostSolveFunc(_con, _ConstraintPostSolveFunc);
	}
	if (_SpringForceFunc)
	{
		_node->pSpringForceFunc = _SpringForceFunc;
		cpDampedSpringSetSpringForceFunc(_con, _ConstraintDampedSpringForceFunc);
	}
	if (_SpringTorqueFunc)
	{
		_node->pSpringTorqueFunc = _SpringTorqueFunc;
		cpDampedRotarySpringSetSpringTorqueFunc(_con, _ConstraintDampedRotarySpringTorqueFunc);
	}
}
BLVoid 
blChipmunkArbiterRestitution(IN BLGuid _Arbiter, IN BLF32 _Restitution)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterSetRestitution(_arb, _Restitution);
}
BLVoid 
blChipmunkArbiterGetRestitution(IN BLGuid _Arbiter, OUT BLF32* _Restitution)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	*_Restitution = cpArbiterGetRestitution(_arb);
}
BLVoid 
blChipmunkArbiterFriction(IN BLGuid _Arbiter, IN BLF32 _Friction)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterSetFriction(_arb, _Friction);
}
BLVoid 
blChipmunkArbiterGetFriction(IN BLGuid _Arbiter, OUT BLF32* _Friction)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	*_Friction = cpArbiterGetFriction(_arb);
}
BLVoid 
blChipmunkArbiterSurfaceVelocity(IN BLGuid _Arbiter, IN BLF32 _VelX, IN BLF32 _VelY)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterSetSurfaceVelocity(_arb, cpv(_VelX, _VelY));
}
BLVoid 
blChipmunkArbiterGetSurfaceVelocity(IN BLGuid _Arbiter, OUT BLF32* _VelX, OUT BLF32* _VelY)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpVect _vel = cpArbiterGetSurfaceVelocity(_arb);
	*_VelX = _vel.x;
	*_VelY = _vel.y;
}
BLBool 
blChipmunkArbiterIgnore(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterIgnore(_arb);
}
BLBool 
blChipmunkArbiterIsFirstContact(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterIsFirstContact(_arb);
}
BLBool 
blChipmunkArbiterIsRemoval(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterIsRemoval(_arb);
}
BLVoid
blChipmunkArbiterTotalImpulse(IN BLGuid _Arbiter, OUT BLF32* _X, OUT BLF32* _Y)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpVect _imp = cpArbiterTotalImpulse(_arb);
	*_X = _imp.x;
	*_Y = _imp.y;
}
BLVoid
blChipmunkArbiterTotalKE(IN BLGuid _Arbiter, OUT BLF32* _KE)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	*_KE = cpArbiterTotalKE(_arb);
}
BLVoid 
blChipmunkArbiterContactPoint(IN BLGuid _Arbiter, IN BLU32 _Count, IN BLF32 _NormalX, IN BLF32 _NormalY, IN BLF32* _AX, IN BLF32* _AY, IN BLF32* _BX, IN BLF32* _BY, IN BLF32* _Depth)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpContactPointSet _ps;
	_ps.count = _Count;
	_ps.normal.x = _NormalX;
	_ps.normal.y = _NormalY;
	BLU32 _cnt = _Count <= CP_MAX_CONTACTS_PER_ARBITER ? _Count : CP_MAX_CONTACTS_PER_ARBITER;
	for (BLU32 _i = 0; _i < _cnt; ++_i)
	{
		_ps.points[_i].distance = _Depth[_i];
		_ps.points[_i].pointA.x = _AX[_i];
		_ps.points[_i].pointA.y = _AY[_i];
		_ps.points[_i].pointB.x = _BX[_i];
		_ps.points[_i].pointB.y = _AY[_i];
	}
	cpArbiterSetContactPointSet(_arb, &_ps);
}
BLU32 
blChipmunkArbiterGetCount(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterGetCount(_arb);
}
BLVoid 
blChipmunkArbiterGetContactNormal(IN BLGuid _Arbiter, OUT BLF32* _NormalX, OUT BLF32* _NormalY)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpVect _nor = cpArbiterGetNormal(_arb);
	*_NormalX = _nor.x;
	*_NormalY = _nor.y;
}
BLVoid 
blChipmunkArbiterGetContactPoint(IN BLGuid _Arbiter, IN BLU32 _Idx, OUT BLF32* _AX, OUT BLF32* _AY, OUT BLF32* _BX, OUT BLF32* _BY, OUT BLF32* _Depth)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpVect _pta = cpArbiterGetPointA(_arb, _Idx);
	cpVect _ptb = cpArbiterGetPointB(_arb, _Idx);
	*_AX = _pta.x;
	*_AY = _pta.y;
	*_BX = _ptb.x;
	*_BY = _ptb.y;
	*_Depth = cpArbiterGetDepth(_arb, _Idx);
}
BLGuid
blChipmunkArbiterGetSpriteA(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	CP_ARBITER_GET_SHAPES(_arb, a, b);
	BLGuid* _id = cpShapeGetUserData(a);
	return *_id;
}
BLGuid
blChipmunkArbiterGetSpriteB(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	CP_ARBITER_GET_SHAPES(_arb, a, b);
	BLGuid* _id = cpShapeGetUserData(b);
	return *_id;
}
BLBool 
blChipmunkArbiterCallWildcardBeginA(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterCallWildcardBeginA(_arb, _PrCpMem->pSpace);
}
BLBool 
blChipmunkArbiterCallWildcardBeginB(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterCallWildcardBeginB(_arb, _PrCpMem->pSpace);
}
BLBool 
blChipmunkArbiterCallWildcardPreSolveA(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterCallWildcardPreSolveA(_arb, _PrCpMem->pSpace);
}
BLBool 
blChipmunkArbiterCallWildcardPreSolveB(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	return cpArbiterCallWildcardPreSolveB(_arb, _PrCpMem->pSpace);
}
BLVoid 
blChipmunkArbiterCallWildcardPostSolveA(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterCallWildcardPostSolveA(_arb, _PrCpMem->pSpace);
}
BLVoid 
blChipmunkArbiterCallWildcardPostSolveB(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterCallWildcardPostSolveB(_arb, _PrCpMem->pSpace);
}
BLVoid 
blChipmunkArbiterCallWildcardSeparateA(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterCallWildcardSeparateA(_arb, _PrCpMem->pSpace);
}
BLVoid 
blChipmunkArbiterCallWildcardSeparateB(IN BLGuid _Arbiter)
{
	cpArbiter* _arb = blGuidAsPointer(_Arbiter);
	cpArbiterCallWildcardSeparateB(_arb, _PrCpMem->pSpace);
}