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
#ifndef __utils_h_
#define __utils_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLU32 blUniqueUri();
    
BL_API BLGuid blGenGuid(
	IN BLVoid* _Ptr,
	IN BLU32 _Uri);

BL_API BLVoid blDeleteGuid(
	IN BLGuid _ID);

BL_API BLVoid* blGuidAsPointer(
	IN BLGuid _ID);

BL_API const BLAnsi* blGuidAsString(
	IN BLGuid _ID);

BL_API BLGuid blStringAsGuid(
	IN BLAnsi* _String);

BL_API BLVoid blDebugOutput(
	IN BLAnsi* _Format,
	...);

BL_API const BLAnsi* blMD5String(
	IN BLAnsi* _Str);

BL_API BLS32 blNatCompare(
	IN BLAnsi* _StrA,
	IN BLAnsi* _StrB);

BL_API BLU32 blHashString(
	IN BLUtf8* _Str);

BL_API BLU32 blUtf8Length(
	IN BLUtf8* _Str);

BL_API BLU32 blUtf16Length(
	IN BLUtf16* _Str);

BL_API BLBool blUtf8Equal(
	IN BLUtf8* _Str1,
	IN BLUtf8* _Str2);

BL_API BLBool blUtf16Equal(
	IN BLUtf16* _Str1,
	IN BLUtf16* _Str2);

BL_API BLVoid blSwitchLowerUtf8(
	INOUT BLUtf8* _Str);

BL_API BLVoid blSwitchLowerUtf16(
	INOUT BLUtf16* _Str);

BL_API BLVoid blSwitchUpperUtf8(
	INOUT BLUtf8* _Str);

BL_API BLVoid blSwitchUpperUtf16(
	INOUT BLUtf16* _Str);

BL_API BLS32 blUtf8ToInteger(
	IN BLUtf8* _Str);

BL_API BLS32 blUtf16ToInteger(
	IN BLUtf16* _Str);

BL_API BLU32 blUtf8ToUInteger(
	IN BLUtf8* _Str);

BL_API BLU32 blUtf16ToUInteger(
	IN BLUtf16* _Str);

BL_API BLF32 blUtf8ToFloat(
	IN BLUtf8* _Str);

BL_API BLF32 blUtf16ToFloat(
	IN BLUtf16* _Str);

BL_API const BLUtf8* blIntegerToUtf8(
	IN BLS32 _Var);

BL_API const BLUtf16* blIntegerToUtf16(
	IN BLS32 _Var);

BL_API const BLUtf8* blFloatToUtf8(
	IN BLF32 _Var);

BL_API const BLUtf16* blFloatToUtf16(
	IN BLF32 _Var);

BL_API const BLUtf16* blGenUtf16Str(
	IN BLUtf8* _Str);

BL_API BLVoid blDeleteUtf16Str(
	INOUT BLUtf16* _Str);

BL_API const BLUtf8* blGenUtf8Str(
	IN BLUtf16* _Str);

BL_API BLVoid blDeleteUtf8Str(
	INOUT BLUtf8* _Str);

BL_API const BLUtf8* blFileSuffixUtf8(
	IN BLUtf8* _Filename);

BL_API const BLUtf16* blFileSuffixUtf16(
	IN BLUtf16* _Filename);

BL_API const BLAnsi* blGenBase64Encoder(
	IN BLU8* _Blob,
	IN BLU32 _Size);

BL_API BLVoid blDeleteBase64Encoder(
	INOUT BLAnsi* _String);

BL_API const BLU8* blGenBase64Decoder(
	IN BLAnsi* _String,
	OUT BLU32* _Size);

BL_API BLVoid blDeleteBase64Decoder(
	INOUT BLU8* _Blob);
    
BL_API BLU32 blColor4B(
	IN BLU8 _Rr, 
	IN BLU8 _Gg, 
	IN BLU8 _Bb, 
	IN BLU8 _Aa);
    
BL_API BLU32 blColor4F(
	IN BLF32 _Rr, 
	IN BLF32 _Gg, 
	IN BLF32 _Bb, 
	IN BLF32 _Aa);
    
BL_API BLVoid blDeColor4B(
	IN BLU32 _Clr, 
	OUT BLU8 _Clrb[4]);
    
BL_API BLVoid blDeColor4F(
	IN BLU32 _Clr, 
	OUT BLF32 _Clrf[4]);

BL_API BLVoid blRLEDecode(
	IN BLU8* _Src,
	IN BLU32 _Dstlen,
	INOUT BLU8* _Dst);
#ifdef __cplusplus
}
#endif

#endif/* __utils_h_ */
