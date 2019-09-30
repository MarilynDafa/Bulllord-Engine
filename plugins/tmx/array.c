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
#include "array.h"
BLArray* 
blGenArray()
{
	BLArray* _ret = (BLArray*)malloc(sizeof(BLArray));
	BLVoid** _data = (BLVoid**)malloc(4 * sizeof(BLVoid*));
	if (_data == NULL)
	{
		free(_ret);
		return NULL;
	}
	_ret->pData = _data;
	_ret->nSize = 0;
	_ret->nCapacity = 4;
	return _ret;
}
BLVoid
blDeleteArray(INOUT BLArray* _Arr)
{
	if (_Arr->pData)
		free(_Arr->pData);
	_Arr->pData = NULL;
	_Arr->nSize = 0;
	_Arr->nCapacity = 0;
	free(_Arr);
	_Arr = NULL;
}
BLVoid
blClearArray(INOUT BLArray* _Arr)
{
	if (_Arr->pData)
		free(_Arr->pData);
	_Arr->pData = NULL;
	_Arr->nSize = 0;
	_Arr->nCapacity = 0;
}
BLVoid*
blArrayElement(IN BLArray* _Arr, IN BLU32 _Idx)
{
	if (_Idx >= _Arr->nSize)
		return NULL;
	return *(_Arr->pData + _Idx);
}
BLVoid*
blArrayFrontElement(IN BLArray* _Arr)
{
	if (!_Arr->nSize)
		return NULL;
	return *(_Arr->pData);
}
BLVoid*
blArrayBackElement(IN BLArray* _Arr)
{
	if (!_Arr->nSize)
		return NULL;
	return *(_Arr->pData + (_Arr->nSize - 1));
}
BLVoid
blArrayPushFront(INOUT BLArray* _Arr, IN BLVoid* _Ele)
{
	if (!_Ele)
		return;
	if (_Arr->nSize >= _Arr->nCapacity)
	{
		_Arr->nCapacity = 2 * (_Arr->nCapacity + 1);
		_Arr->pData = (BLVoid**)realloc(_Arr->pData, _Arr->nCapacity * sizeof(BLVoid*));
	}
	memmove(_Arr->pData + 1, _Arr->pData, sizeof(BLVoid*) * _Arr->nSize);
	memcpy(_Arr->pData, &_Ele, sizeof(BLVoid*));
	_Arr->nSize++;
}
BLVoid
blArrayPushBack(INOUT BLArray* _Arr, IN BLVoid* _Ele)
{
	BLVoid* _add;
	if (!_Ele)
		return;
	if (_Arr->nSize >= _Arr->nCapacity)
	{
		_Arr->nCapacity = 2 * (_Arr->nCapacity + 1);
		_Arr->pData = (BLVoid**)realloc(_Arr->pData, _Arr->nCapacity * sizeof(BLVoid*));
	}
	_add = _Arr->pData + _Arr->nSize;
	memcpy(_add, &_Ele, sizeof(BLVoid*));
	_Arr->nSize++;
}
BLVoid
blArrayPopFront(INOUT BLArray* _Arr)
{
	if (!_Arr->nSize)
		return;
	memmove(_Arr->pData, _Arr->pData + 1, sizeof(BLVoid*) * _Arr->nSize);
	_Arr->nSize--;
}
BLVoid
blArrayPopBack(INOUT BLArray* _Arr)
{
	if (!_Arr->nSize)
		return;
	_Arr->nSize--;
}
BLU32
blArrayErase(INOUT BLArray* _Arr, IN BLU32 _Idx)
{
	if (_Idx >= _Arr->nSize)
		return _Arr->nSize;
	memmove(_Arr->pData + _Idx, _Arr->pData + (1 + _Idx), sizeof(BLVoid*)*(_Arr->nSize - _Idx - 1));
	_Arr->nSize--;
	return _Idx ? _Idx - 1 : 0;
}
