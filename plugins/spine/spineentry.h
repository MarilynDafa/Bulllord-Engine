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
#ifndef __spineentry_h_
#define __spineentry_h_
#include "../../code/headers/prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLVoid blSpineOpenEXT();

BL_API BLVoid blSpineCloseEXT();

BL_API BLVoid blSpriteAnimationSetEXT(
	IN BLGuid _ID, 
	IN BLAnsi* _Animation,
	IN BLS32 _Track,
	IN BLBool _Loop);

BL_API BLVoid blSpriteAnimationAddEXT(
	IN BLGuid _ID,
	IN BLAnsi* _Animation,
	IN BLS32 _Track,
	IN BLBool _Loop,
	IN BLF32 _Delay);

BL_API BLVoid blSpriteAnimationAddEmptyEXT(
	IN BLGuid _ID,
	IN BLS32 _Track,
	IN BLF32 _Duration,
	IN BLF32 _Delay);

BL_API BLVoid blSpriteAnimationMixEXT(
	IN BLGuid _ID,
	IN BLAnsi* _From,
	IN BLAnsi* _To,
	IN BLF32 _Duration);

BL_API BLVoid blSpriteAnimationEmptyEXT(
	IN BLGuid _ID, 
	IN BLU32 _TrackIndex,
	IN BLF32 _Duration);

BL_API BLVoid blSpriteTrackEmpty(
	IN BLGuid _ID,
	IN BLU32 _TrackIndex);

BL_API BLBool blSpriteBoundingBoxEXT(
	IN BLGuid _ID,
	OUT BLU32* _Width,
	OUT BLU32* _Height);
#ifdef __cplusplus
}
#endif
#endif/* __tmx_h_ */
