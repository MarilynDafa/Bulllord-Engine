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
#ifndef __audio_h_
#define __audio_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif

BL_API BLVoid blUseMusicVolume(
	IN BLF32 _Vol);
    
BL_API BLVoid blUseEnvironmentVolume(
	IN BLF32 _Vol);
    
BL_API BLVoid blUseSystemVolume(
	IN BLF32 _Vol);

BL_API BLVoid blQueryAudioVolume(
	OUT BLF32* _Music,
	OUT BLF32* _System,
	OUT BLF32* _Env
	);

BL_API BLVoid blSetListenerPos(
	IN BLF32 _Xpos,
	IN BLF32 _Ypos,
	IN BLF32 _Zpos);
    
BL_API BLVoid blSetListenerUp(
	IN BLF32 _Xpos,
	IN BLF32 _Ypos,
	IN BLF32 _Zpos);
    
BL_API BLVoid blSetListenerDir(
	IN BLF32 _Xpos,
	IN BLF32 _Ypos,
	IN BLF32 _Zpos);

BL_API BLVoid blSetEmitterPos(
	IN BLGuid _ID,
	IN BLF32 _Xpos,
	IN BLF32 _Ypos,
	IN BLF32 _Zpos);

BL_API BLGuid blGenAudio(
	IN BLAnsi* _Filename,
	IN BLAnsi* _Archive,
	IN BLBool _Loop,
	IN BLBool _3D,
	IN BLF32 _Xpos,
	IN BLF32 _Ypos,
	IN BLF32 _Zpos);

BL_API BLVoid blDeleteAudio(
	IN BLGuid _ID);
#ifdef __cplusplus
}
#endif

#endif/* __audio_h_ */
