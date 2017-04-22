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
#ifndef __tmx_h_
#define __tmx_h_
#include "../../code/headers/prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLVoid blTMXOpenEXT();

BL_API BLVoid blTMXCloseEXT();

//Base64 Zip
BL_API BLBool blTMXFileEXT(
	IN BLAnsi* _Filename,
	IN BLAnsi* _Archive);

BL_API BLVoid blTMXQueryEXT(
	OUT BLAnsi _Orientation[32], 
	OUT BLU32* _Width,
	OUT BLU32* _Height,
	OUT BLU32* _TileWidth,
	OUT BLU32* _TileHeight);

BL_API BLVoid blTMXObjectGroupQueryEXT(
	IN BLAnsi* _GroupName,
	OUT BLU32* _ObjectNum,
	OUT BLBool* _Visible);

//name0:valye0,name1:value1,name2:value2
BL_API BLVoid blTMXObjectQueryEXT(
	IN BLAnsi* _GroupName,
	IN BLU32 _Index,
	OUT BLAnsi _Name[256],
	OUT BLS32* _XPos,
	OUT BLS32* _YPos,
	OUT BLU32* _Width,
	OUT BLU32* _Height,
	OUT BLAnsi** _Properties);

BL_API BLVoid blTMXLayerVisibilityEXT(
	IN BLAnsi* _Layer,
	IN BLBool _Show);

BL_API BLBool blTMXLayerTileAtEXT(
	IN BLAnsi* _Layer,
	IN BLS32 _XPos,
	IN BLS32 _YPos);

BL_API BLBool blTMXLayerAsNavigationEXT(
	IN BLAnsi* _Layer,
	IN BLBool _Barrier);

BL_API BLBool blTMXPathFindEXT(
	IN BLS32 _StartX,
	IN BLS32 _StartY,
	IN BLS32 _EndX,
	IN BLS32 _EndY, 
	IN BLBool _Strict,
	OUT BLF32** _XPath,
	OUT BLF32** _YPath,
	OUT BLU32* _PathNum);
#ifdef __cplusplus
}
#endif
#endif/* __tmx_h_ */
