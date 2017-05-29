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
#ifndef __array_h_
#define __array_h_
#include "thread.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Array{
	BLVoid** pData;
	BLU32 nSize;	
	BLU32 nCapacity;
	blMutex* pMutex;
}BLArray;

BLArray* blGenArray(
	IN BLBool _Withthread);

BLVoid blDeleteArray(
	INOUT BLArray* _Arr);

BLVoid blClearArray(
	INOUT BLArray* _Arr);

BLVoid* blArrayElement(
	IN BLArray* _Arr,
	IN BLU32 _Idx);

BLVoid* blArrayFrontElement(
	IN BLArray* _Arr);

BLVoid* blArrayBackElement(
	IN BLArray* _Arr);

BLVoid blArrayPushFront(
	INOUT BLArray* _Arr, 
	IN BLVoid* _Ele);

BLVoid blArrayPushBack(
	INOUT BLArray* _Arr, 
	IN BLVoid* _Ele);

BLVoid blArrayPopFront(
	INOUT BLArray* _Arr);

BLVoid blArrayPopBack(
	INOUT BLArray* _Arr);

BLVoid blArrayErase(
	INOUT BLArray* _Arr, 
	IN BLU32 _Idx);

#define FOREACH_ARRAY(type, var , container) type (var) = container->nSize ? (*(type*)(container->pData)) : NULL;\
	BLU32(_iterator##var) = 0;\
	for (; (_iterator##var) < (container->nSize);\
         (var) = *(type*)(container->pData + (++_iterator##var >= container->nSize ? 0 : _iterator##var)))
#ifdef __cplusplus
}
#endif

#endif/*__array_h_*/
