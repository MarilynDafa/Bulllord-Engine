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
#ifndef __vgentry_h_
#define __vgentry_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
	BL_API BLVoid blVGOpenEXT(
		IN BLAnsi* _Version,
		...);

	BL_API BLVoid blVGCloseEXT();

	BL_API BLVoid blVGRectangleEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _Width,
		IN BLF32 _Height,
		IN BLF32 _Rotate,
		IN BLBool _Fill,
		IN BLU32 _Color);

	BL_API BLVoid blVGEllipseEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _XRadius,
		IN BLF32 _YRadius,
		IN BLF32 _Rotate,
		IN BLBool _Fill,
		IN BLU32 _Color);

	BL_API BLVoid blVGArcEXT(
		IN BLGuid _ID,
		IN BLF32 _X,
		IN BLF32 _Y,
		IN BLF32 _XRadius,
		IN BLF32 _YRadius,
		IN BLF32 _StartAngle,
		IN BLF32 _EndAngle,
		IN BLF32 _Rotate,
		IN BLU32 _Color);

	BL_API BLVoid blVGSplineEXT(
		IN BLGuid _ID,
		IN BLF32* _XPath,
		IN BLF32* _YPath,
		IN BLU32 _PathNum,
		IN BLU32 _Divisions,
		IN BLU32 _Color);

	BL_API BLVoid blVGBezierEXT(
		IN BLGuid _ID,
		IN BLF32 _X1,
		IN BLF32 _Y1,
		IN BLF32 _CX1,
		IN BLF32 _CY1,
		IN BLF32 _X2,
		IN BLF32 _Y2,
		IN BLF32 _CX2,
		IN BLF32 _CY2,
		IN BLU32 _Divisions,
		IN BLU32 _Color);
#ifdef __cplusplus
}
#endif
#endif/* __vg_h_ */
