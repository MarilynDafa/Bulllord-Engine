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
#include "util.h"
typedef struct _ChipmunkMember {
	cpSpace* pSpace;
}_BLChipmunkMember;
static _BLChipmunkMember* _PrCmMem = NULL;
BLVoid
blChipmunkOpenEXT(IN BLAnsi* _Version, ...)
{
	_PrCmMem = (_BLChipmunkMember*)malloc(sizeof(_BLChipmunkMember));
	_PrCmMem->pSpace = cpSpaceNew();
}
BLVoid
blChipmunkCloseEXT()
{
	cpSpaceFree(_PrCmMem->pSpace);
	free(_PrCmMem);
}
BLVoid 
blChipmunkSpaceIterationsEXT(IN BLU32 _Iterations)
{
	cpSpaceSetIterations(_PrCmMem->pSpace, _Iterations);
}
BLVoid 
blChipmunkSpaceGetIterationsEXT(OUT BLU32* _Iterations)
{
	*_Iterations = cpSpaceGetIterations(_PrCmMem->pSpace);
}
BLVoid 
blChipmunkSpaceGravityEXT(IN BLF64 _X, IN BLF64 _Y)
{
	cpVect _vec;
	_vec.x = _X;
	_vec.y = _Y;
	cpSpaceSetGravity(_PrCmMem->pSpace, _vec);
}
BLVoid 
blChipmunkSpaceGetGravityEXT(OUT BLF64* _X, OUT BLF64* _Y)
{
	cpVect _ret = cpSpaceGetGravity(_PrCmMem->pSpace);
	*_X = _ret.x;
	*_Y = _ret.y;
}
BLVoid 
blChipmunkSpaceDampingEXT(IN BLF64 _Damping)
{
	cpSpaceSetDamping(_PrCmMem->pSpace, _Damping);
}
BLVoid 
blChipmunkSpaceGetDampingEXT(OUT BLF64* _Damping)
{
	*_Damping = cpSpaceGetDamping(_PrCmMem->pSpace);
}
BLVoid 
blChipmunkSpaceSpeedThreshold(IN BLF64 _Threshold)
{
	cpSpaceSetIdleSpeedThreshold(_PrCmMem->pSpace, _Threshold);
}
BLVoid 
blChipmunkSpaceGetSpeedThreshold(OUT BLF64* _Threshold)
{
	*_Threshold = cpSpaceGetIdleSpeedThreshold(_PrCmMem->pSpace);
}
BLVoid 
blChipmunkSpaceSleepTimeThresholdEXT(IN BLF64 _Threshold)
{
	cpSpaceSetSleepTimeThreshold(_PrCmMem->pSpace, _Threshold);
}
BLVoid 
blChipmunkSpaceGetSleepTimeThresholdEXT(OUT BLF64* _Threshold)
{
	*_Threshold = cpSpaceGetSleepTimeThreshold(_PrCmMem->pSpace);
}
BLVoid 
blChipmunkSpaceCollisionSlopEXT(IN BLF64 _Slop)
{
	cpSpaceSetCollisionSlop(_PrCmMem->pSpace, _Slop);
}
BLVoid 
blChipmunkSpaceGetCollisionSlopEXT(OUT BLF64* _Slop)
{
	*_Slop = cpSpaceGetCollisionSlop(_PrCmMem->pSpace);
}
BLVoid 
blChipmunkSpaceCollisionBiasEXT(IN BLF64 _Bias)
{
	cpSpaceSetCollisionBias(_PrCmMem->pSpace, _Bias);
}
BLVoid 
blChipmunkSpaceGetCollisionBiasEXT(OUT BLF64* _Bias)
{
	*_Bias = cpSpaceGetCollisionBias(_PrCmMem->pSpace);
}
BLVoid 
blChipmunkSpaceCollisionPersistenceEXT(IN BLU32 _Persistence)
{
	cpSpaceSetCollisionPersistence(_PrCmMem->pSpace, _Persistence);
}
BLVoid 
blChipmunkSpaceGetCollisionPersistenceEXT(OUT BLU32* _Persistence)
{
	*_Persistence = cpSpaceGetCollisionPersistence(_PrCmMem->pSpace);
}