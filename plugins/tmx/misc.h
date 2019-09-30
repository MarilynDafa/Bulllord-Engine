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
#ifndef __misc_h_
#define __misc_h_
#include "prerequisites.h"
#include "dictionary.h"
#include "array.h"
#include "xml/ezxml.h"
#ifdef __cplusplus
extern "C" {
#endif
	typedef enum _Type
	{
		/// no value is wrapped, an empty Value
		TypeNONE = 0,
		/// wrap byte
		TypeBYTE,
		/// wrap integer
		TypeINTEGER,
		/// wrap unsigned
		TypeUNSIGNED,
		/// wrap float
		TypeFLOAT,
		/// wrap double
		TypeDOUBLE,
		/// wrap bool
		TypeBOOLEAN,
		/// wrap string
		TypeSTRING,
		/// wrap vector
		TypeVECTOR,
		/// wrap ValueMap
		TypeMAP,
		/// wrap ValueMapIntKey
		TypeINT_KEY_MAP
	} ValueType;

	typedef enum _Orientation
	{
		/** Orthogonal orientation. */
		TMXOrientationOrtho,

		/** Hexagonal orientation. */
		TMXOrientationHex,

		/** Isometric orientation. */
		TMXOrientationIso,

		/** Isometric staggered orientation. */
		TMXOrientationStaggered,
	}TMXOrientation;

	/** Possible stagger axis of the TMX map. */
	typedef enum _Stagger
	{
		/** Stagger Axis x. */
		TMXStaggerAxis_X,

		/** Stagger Axis y. */
		TMXStaggerAxis_Y,
	}TMXStagger;

	/** Possible stagger index of the TMX map. */
	typedef enum _StaggerIndex
	{
		/** Stagger Index: Odd */
		TMXStaggerIndex_Odd,

		/** Stagger Index: Even */
		TMXStaggerIndex_Even,
	}TMXStaggerIndex;


	typedef enum _EncodeType {
		TMXLayerAttribNone = 1 << 0,
		TMXLayerAttribBase64 = 1 << 1,
		TMXLayerAttribGzip = 1 << 2,
		TMXLayerAttribZlib = 1 << 3,
		TMXLayerAttribCSV = 1 << 4,
	} TMXEncodeType;

	typedef enum _PropertyType {
		TMXPropertyNone,
		TMXPropertyMap,
		TMXPropertyLayer,
		TMXPropertyObjectGroup,
		TMXPropertyObject,
		TMXPropertyTile
	} TMXPropertyType;

	typedef enum TMXTileFlags_ {
		kTMXTileHorizontalFlag = 0x80000000,
		kTMXTileVerticalFlag = 0x40000000,
		kTMXTileDiagonalFlag = 0x20000000,
		kTMXFlipedAll = (kTMXTileHorizontalFlag | kTMXTileVerticalFlag | kTMXTileDiagonalFlag),
		kTMXFlippedMask = ~(kTMXFlipedAll)
	} TMXTileFlags;
#ifdef WINDLL
	typedef unsigned char bool;
#endif
	typedef struct _Vec2 {
		float x;
		float y;
	} Vec2;


	typedef struct _Size {
		float width;
		float height;
	} Size;

	typedef struct _Rect {
		/**Low left point of rect.*/
		Vec2 origin;
		/**Width and height of the rect.*/
		Size  size;
	} Rect;

	typedef struct _Value {
		union
		{
			unsigned char byteVal;
			int intVal;
			unsigned int unsignedVal;
			float floatVal;
			double doubleVal;
			bool boolVal;
			char* strVal;
			float* arrayVal;
			BLDictionary* mapVal;
		}_field;

		ValueType _type;
	}Value;

	Value* newUChar(unsigned char v);
	Value* newInt(int v);
	Value* newUInt(unsigned int v);
	Value* newFloat(float v);
	Value* newDouble(double v);
	Value* newBool(bool v);
	Value* newStr(const char* v);
	Value* newVMap(BLDictionary* v);
	Value* newArray(float* v);

	void freeValueMap(BLDictionary* dct);

	char* strdcopy(const char* str);
	unsigned int inflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int outLengthHint);
	int inflateMemoryWithHint2(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLengthHint);

#define StrFree(str) if (str) {free(str); str = NULL;}
#define VecInit(vec) vec.x = vec.y = 0;
#define SizeInit(sz) sz.width = sz.height = 0;

	unsigned int ezxml_attru(ezxml_t node, const char* att);
	int ezxml_attrd(ezxml_t node, const char* att);
	float ezxml_attrf(ezxml_t node, const char* att);
	bool ezxml_attrb(ezxml_t node, const char* att, bool def);

#ifdef __cplusplus
}
#endif
#endif/* __misc_h_ */