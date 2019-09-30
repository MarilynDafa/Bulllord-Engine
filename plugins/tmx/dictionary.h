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
#ifndef __dictionary_h_
#define __dictionary_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _Dictionary{
	struct _DictNode {
		struct _DictNode* pLeft;
		struct _DictNode* pRight;
		struct _DictNode* pParent;
		BLU32 nKey;
		BLVoid* pValue;
		BLBool bRed;
	}*pRoot;
	BLU32 nSize;
}BLDictionary;

BLDictionary* blGenDict();

BLVoid blDeleteDict(
	INOUT BLDictionary* _Dct);

BLVoid blClearDict(
	INOUT BLDictionary* _Dct);

BLVoid* blDictElement(
	IN BLDictionary* _Dct,
	IN BLU32 _Key);

BLVoid* blDictRootElement(
	IN BLDictionary* _Dct);

BLVoid blDictInsert(
	INOUT BLDictionary* _Dct,
	IN BLU32 _Key, 
	IN BLVoid* _Data);

BLVoid blDictErase(
	INOUT BLDictionary* _Dct,
	IN BLU32 _Key);

BLBool blDictTraval(
	INOUT BLDictionary* _Dct,
	OUT BLVoid** _Node,
	OUT BLU32* _Key,
	OUT BLVoid** _Data);

#ifdef __cplusplus
#define FOREACH_DICT(type , var , container) struct BLDictionary::_DictNode* _iterator##var;\
	type var = NULL;\
	BLU32 _iterator##key = 0;\
	while (blDictTraval(container, (BLVoid**)&_iterator##var, &_iterator##key, (BLVoid**)&var))
#else
#define FOREACH_DICT(type , var , container) struct _DictNode* _iterator##var;\
    type var = NULL;\
	BLU32 _iterator##key = 0;\
    while (blDictTraval(container, (BLVoid**)&_iterator##var, &_iterator##key, (BLVoid**)&var))
#endif

#ifdef __cplusplus
}
#endif

#endif /*__dictionary_h_*/
