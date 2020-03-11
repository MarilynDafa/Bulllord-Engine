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

	BL_API BLVoid blChipmunkSpaceIterationsEXT(
		IN BLU32 _Iterations);

	BL_API BLVoid blChipmunkSpaceGetIterationsEXT(
		OUT BLU32* _Iterations);

	BL_API BLVoid blChipmunkSpaceGravityEXT(
		IN BLF64 _X,
		IN BLF64 _Y);

	BL_API BLVoid blChipmunkSpaceGetGravityEXT(
		OUT BLF64* _X,
		OUT BLF64* _Y);

	BL_API BLVoid blChipmunkSpaceDampingEXT(
		IN BLF64 _Damping);

	BL_API BLVoid blChipmunkSpaceGetDampingEXT(
		OUT BLF64* _Damping);

	BL_API BLVoid blChipmunkSpaceSpeedThreshold(
		IN BLF64 _Threshold);

	BL_API BLVoid blChipmunkSpaceGetSpeedThreshold(
		OUT BLF64* _Threshold);

	BL_API BLVoid blChipmunkSpaceSleepTimeThresholdEXT(
		IN BLF64 _Threshold);

	BL_API BLVoid blChipmunkSpaceGetSleepTimeThresholdEXT(
		OUT BLF64* _Threshold);

	BL_API BLVoid blChipmunkSpaceCollisionSlopEXT(
		IN BLF64 _Slop);

	BL_API BLVoid blChipmunkSpaceGetCollisionSlopEXT(
		OUT BLF64* _Slop);

	BL_API BLVoid blChipmunkSpaceCollisionBiasEXT(
		IN BLF64 _Bias);

	BL_API BLVoid blChipmunkSpaceGetCollisionBiasEXT(
		OUT BLF64* _Bias);

	BL_API BLVoid blChipmunkSpaceCollisionPersistenceEXT(
		IN BLU32 _Persistence);

	BL_API BLVoid blChipmunkSpaceGetCollisionPersistenceEXT(
		OUT BLU32* _Persistence);
#ifdef __cplusplus
}
#endif
#endif/* __chipmunk_h_ */
