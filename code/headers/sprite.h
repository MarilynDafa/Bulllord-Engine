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
#ifndef __sprite_h_
#define __sprite_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLVoid blSpriteRegistExternal(
	IN BLAnsi* _Suffix,
	IN BLBool(*_LoadCB)(BLGuid, const BLAnsi*, BLVoid**),
	IN BLBool(*_SetupCB)(BLGuid, BLVoid**),
	IN BLBool(*_UnloadCB)(BLGuid, BLVoid**),
	IN BLBool(*_ReleaseCB)(BLGuid, BLVoid**),
	IN BLBool(*_DrawCB)(BLU32, BLGuid, BLF32[6], BLF32, BLF32, BLVoid**),
	IN BLVoid(*_Step)(BLU32));

BL_API BLGuid blSpriteImageGen(
	IN BLAnsi* _Filename,
	IN BLAnsi* _Tag,
    IN BLF32 _Width,
    IN BLF32 _Height,
    IN BLF32 _Zv,
	IN BLU32 _FPS,
	IN BLBool _AsTile);

BL_API BLGuid blSpriteTextGen(
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor,
	IN BLEnum _TxtAlignmentH,
	IN BLEnum _TxtAlignmentV,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics,
	IN BLF32 _Zv);

BL_API BLGuid blSpriteAssignGen(
	IN BLGuid _ID);

BL_API BLVoid blSpriteDelete(
	IN BLGuid _ID);

BL_API BLGuid blSpriteRenderTextureGen(
	IN BLGuid _ID,
	IN BLU32 _Width,
	IN BLU32 _Height);

BL_API BLVoid blSpriteRenderTextureDelete(
	IN BLGuid _ID,
	IN BLGuid _Tex);

BL_API BLVoid* blSpriteExternalData(
	IN BLGuid _ID,
	IN BLVoid* _ExtData);
    
BL_API BLVoid blSpriteClear(
    IN BLBool _Tiles,
    IN BLBool _Actors);
    
BL_API BLBool blSpriteAttach(
	IN BLGuid _Parent,
	IN BLGuid _Child,
    IN BLBool _AttrInherit);

BL_API BLBool blSpriteDetach(
	IN BLGuid _Parent,
	IN BLGuid _Child);

BL_API BLBool blSpriteValid(
	IN BLGuid _ID);

BL_API BLBool blSpriteQuery(
	IN BLGuid _ID,
    OUT BLF32* _Width,
    OUT BLF32* _Height,
	OUT BLF32* _XPos,
	OUT BLF32* _YPos,
	OUT BLF32* _Zv,
	OUT BLF32* _Rotate,
	OUT BLF32* _XScale,
	OUT BLF32* _YScale,
	OUT BLF32* _Alpha,
	OUT BLU32* _DyeClr,
	OUT BLBool* _FlipX,
	OUT BLBool* _FlipY,
	OUT BLBool* _Show);

BL_API BLBool blSpriteVisibility(
	IN BLGuid _ID,
	IN BLBool _Show,
    IN BLBool _Passdown);

BL_API BLVoid blSpriteLocked(
	IN BLGuid _ID,
	IN BLBool _Locked);

BL_API BLBool blSpriteDimension(
	IN BLGuid _ID,
	IN BLF32 _Width,
	IN BLF32 _Height,
	IN BLBool _Passdown);

BL_API BLBool blSpriteFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY,
	IN BLBool _Passdown);
    
BL_API BLBool blSpritePivot(
    IN BLGuid _ID,
    IN BLF32 _PivotX,
    IN BLF32 _PivotY,
    IN BLBool _Passdown);

BL_API BLBool blSpriteAlpha(
	IN BLGuid _ID,
	IN BLF32 _Alpha,
    IN BLBool _Passdown);

BL_API BLBool blSpriteZValue(
	IN BLGuid _ID,
    IN BLF32 _Zv,
    IN BLBool _Passdown);

BL_API BLBool blSpriteText(
	IN BLGuid _ID,
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor,
	IN BLEnum _TxtAlignmentH,
	IN BLEnum _TxtAlignmentV,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics);

BL_API BLBool blSpriteStroke(
	IN BLGuid _ID,
	IN BLU32 _Color,
    IN BLU32 _Pixel,
    IN BLBool _Passdown);

BL_API BLBool blSpriteGlow(
	IN BLGuid _ID,
	IN BLU32 _Color,
    IN BLU32 _Brightness,
    IN BLBool _Passdown);

BL_API BLBool blSpriteDyeing(
	IN BLGuid _ID,
	IN BLU32 _Color,
    IN BLBool _Dye,
    IN BLBool _Passdown);
    
BL_API BLBool blSpriteTransformReset(
    IN BLGuid _ID,
    IN BLF32 _XPos,
    IN BLF32 _YPos,
    IN BLF32 _Rotate,
    IN BLF32 _ScaleX,
    IN BLF32 _ScaleY);

BL_API BLBool blSpriteMove(
	IN BLGuid _ID,
	IN BLF32 _XVec,
	IN BLF32 _YVec);

BL_API BLBool blSpriteMoveTo(
	IN BLGuid _ID,
	IN BLF32 _XPos,
	IN BLF32 _YPos);

BL_API BLBool blSpriteScale(
	IN BLGuid _ID,
	IN BLF32 _XScale,
	IN BLF32 _YScale);

BL_API BLBool blSpriteScaleTo(
	IN BLGuid _ID,
	IN BLF32 _XScale,
	IN BLF32 _YScale);

BL_API BLBool blSpriteRotate(
	IN BLGuid _ID,
	IN BLS32 _Rotate);

BL_API BLBool blSpriteRotateTo(
	IN BLGuid _ID,
	IN BLS32 _Rotate);

BL_API BLBool blSpriteSkew(
	IN BLGuid _ID,
	IN BLS32 _XSkew,
	IN BLS32 _YSkew);

BL_API BLBool blSpriteSkewTo(
	IN BLGuid _ID,
	IN BLS32 _XSkew,
	IN BLS32 _YSkew);
    
BL_API BLBool blSpriteScissor(
    IN BLGuid _ID,
    IN BLF32 _XPos,
    IN BLF32 _YPos,
    IN BLF32 _Width,
    IN BLF32 _Height);
    
BL_API BLVoid blSpriteTile(
    IN BLGuid _ID,
    IN BLAnsi* _ImageFile,
    IN BLF32 _TexLTx,
    IN BLF32 _TexLTy,
    IN BLF32 _TexRBx,
    IN BLF32 _TexRBy,
	IN BLF32 _PosX,
	IN BLF32 _PosY, 
	IN BLF32 _Alpha,
	IN BLBool _Show);
    
BL_API BLBool blSpriteAsEmit(
    IN BLGuid _ID,
	IN BLAnsi* _Code,
	IN BLBool _Off);
    
BL_API BLBool blSpriteAsCursor(
    IN BLGuid _ID);

BL_API BLBool blSpriteActionBegin(
	IN BLGuid _ID);

BL_API BLBool blSpriteActionEnd(
	IN BLGuid _ID,
    IN BLBool _Delete,
	IN BLBool _Loop);

BL_API BLBool blSpriteParallelBegin(
	IN BLGuid _ID);

BL_API BLBool blSpriteParallelEnd(
	IN BLGuid _ID);

BL_API BLBool blSpriteActionPlay(
	IN BLGuid _ID);

BL_API BLBool blSpriteActionStop(
	IN BLGuid _ID);

BL_API BLBool blSpriteActionMove(
	IN BLGuid _ID,
	IN BLF32* _XPath,
	IN BLF32* _YPath,
	IN BLU32 _PathNum,
	IN BLF32 _Acceleration,
	IN BLF32 _Time,
	IN BLBool _Loop);

BL_API BLBool blSpriteActionRotate(
	IN BLGuid _ID,
	IN BLS32 _Angle,
    IN BLBool _ClockWise, 
	IN BLF32 _Time,
	IN BLBool _Loop);

BL_API BLBool blSpriteActionScale(
	IN BLGuid _ID,
	IN BLF32 _XScale,
	IN BLF32 _YScale,
    IN BLBool _Reverse,
	IN BLF32 _Time,
	IN BLBool _Loop);

BL_API BLBool blSpriteActionSkew(
	IN BLGuid _ID,
	IN BLS32 _XSkew,
	IN BLS32 _YSkew,
	IN BLBool _Reverse,
	IN BLF32 _Time,
	IN BLBool _Loop);

BL_API BLBool blSpriteActionAlpha(
	IN BLGuid _ID,
	IN BLF32 _Alpha,
	IN BLBool _Reverse,
	IN BLF32 _Time,
	IN BLBool _Loop);
    
BL_API BLVoid blSpriteViewportQuery(
    OUT BLF32* _LTPosX,
    OUT BLF32* _LTPosY,
    OUT BLF32* _RBPosX,
    OUT BLF32* _RBPosY);

BL_API BLVoid blSpriteViewportMoveTo(
	IN BLF32 _XPos,
	IN BLF32 _YPos);

BL_API BLVoid blSpriteViewportShake(
	IN BLF32 _Time,
	IN BLBool _Vertical,
	IN BLF32 _Force);
#ifdef __cplusplus
}
#endif
#endif/*__sprite_h_*/
