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
#include "internal/list.h"
BLList* 
blGenList(IN BLBool _Withthread)
{
	BLList* _ret = (BLList *)malloc(sizeof(BLList));
	_ret->nSize = 0;
	_ret->pFirst = _ret->pLast = NULL;
	if (_Withthread)
		_ret->pMutex = blGenMutex();
	else
		_ret->pMutex = NULL;
	return _ret;
}
BLVoid
blDeleteList(INOUT BLList* _Lst)
{
	struct _ListNode* _obj;
	for (_obj = _Lst->pFirst; _obj;)
	{
		struct _ListNode* _next = _obj->pNext;
		free(_obj->pData);
		free(_obj);
		_obj = _next;
	}
	_Lst->nSize = 0;
	_Lst->pFirst = _Lst->pLast = NULL;
	if (_Lst->pMutex)
		blDeleteMutex(_Lst->pMutex);
	free(_Lst);
	_Lst = NULL;
}
BLVoid
blClearList(INOUT BLList* _Lst)
{
	struct _ListNode* _obj;
	for (_obj = _Lst->pFirst; _obj;)
	{
		struct _ListNode* _next = _obj->pNext;
		free(_obj->pData);
		free(_obj);
		_obj = _next;
	}
	_Lst->nSize = 0;
	_Lst->pFirst = _Lst->pLast = NULL;
}
BLVoid*
blListFrontElement(IN BLList* _Lst)
{
	return *_Lst->pFirst->pData;
}
BLVoid*
blListBackElement(IN BLList* _Lst)
{
	return *_Lst->pLast->pData;
}
BLVoid
blListPushFront(INOUT BLList* _Lst, IN BLVoid* _Ele)
{
	struct _ListNode* _node;
	_node = (struct _ListNode*)malloc(sizeof(struct _ListNode));
	_node->pData = (BLVoid**)malloc(sizeof(BLVoid*));
	memcpy(_node->pData, &_Ele, sizeof(BLVoid*));
	_node->pNext = _Lst->pFirst;
	_node->pPrev = NULL;
	if (_node->pNext != NULL)
		_node->pNext->pPrev = _node;
	_Lst->pFirst = _node;
	if (_Lst->pLast == NULL)
		_Lst->pLast = _node;
	_Lst->nSize++;
}
BLVoid
blListPushBack(INOUT BLList* _Lst, IN BLVoid* _Ele)
{
	struct _ListNode* _node;
	_node = (struct _ListNode*)malloc(sizeof(struct _ListNode));
	_node->pData = (BLVoid**)malloc(sizeof(BLVoid*));
	memcpy(_node->pData, &_Ele, sizeof(BLVoid*));
	_node->pPrev = _Lst->pLast;
	_node->pNext = NULL;
	if (_node->pPrev != NULL)
		_node->pPrev->pNext = _node;
	_Lst->pLast = _node;
	if (_Lst->pFirst == NULL)
		_Lst->pFirst = _node;
	_Lst->nSize++;
}
BLVoid
blListPopFront(INOUT BLList* _Lst)
{
	struct _ListNode* _node;
	_node = _Lst->pFirst;
	if (_node == _Lst->pFirst)
		_Lst->pFirst = _node->pNext;
	else
		_node->pPrev->pNext = _node->pNext;
	if (_node == _Lst->pLast)
		_Lst->pLast = _node->pPrev;
	else
		_node->pNext->pPrev = _node->pPrev;
	_Lst->nSize--;
	free(_node->pData);
	free(_node);
}
BLVoid
blListPopBack(INOUT BLList* _Lst)
{
	struct _ListNode* _node;
	_node = _Lst->pLast;
	if (_node == _Lst->pFirst)
		_Lst->pFirst = _node->pNext;
	else
		_node->pPrev->pNext = _node->pNext;
	if (_node == _Lst->pLast)
		_Lst->pLast = _node->pPrev;
	else
		_node->pNext->pPrev = _node->pPrev;
	_Lst->nSize--;
	free(_node->pData);
	free(_node);
}
BLVoid
blListInsert(INOUT BLList* _Lst, IN BLU32 _Idx, IN BLVoid* _Ele)
{
	struct _ListNode* _node;
	if (!_Ele)
		return;
	if (_Idx >= _Lst->nSize)
		return;
	_node = (struct _ListNode*)malloc(sizeof(struct _ListNode));
	_node->pData = (BLVoid**)malloc(sizeof(BLVoid*));
	memcpy(_node->pData, &_Ele, sizeof(BLVoid*));
	if (_Idx == 0)
	{
		_node->pNext = _Lst->pFirst;
		if (_node->pNext != NULL)
			_node->pNext->pPrev = _node;
		_Lst->pFirst = _node;
		if (_Lst->pLast == NULL)
			_Lst->pLast = _node;
	}
	else if (_Idx == _Lst->nSize)
	{
		_node->pPrev = _Lst->pLast;
		if (_node->pPrev != NULL)
			_node->pPrev->pNext = _node;
		_Lst->pLast = _node;
		if (_Lst->pFirst == NULL)
			_Lst->pFirst = _node;
	}
	else 
	{
		BLBool _backward;
		struct _ListNode* _obj = NULL;
		BLU32 _listidx;
		if (_Idx < _Lst->nSize / 2)
		{
			_backward = FALSE;
			_obj = _Lst->pFirst;
			_listidx = 0;
		}
		else
		{
			_backward = TRUE;
			_obj = _Lst->pLast;
			_listidx = _Lst->nSize - 1;
		}
		while (_obj) 
		{
			if (_listidx == _Idx)
				break;
			if (_backward == FALSE)
			{
				_obj = _obj->pNext;
				_listidx++;
			}
			else 
			{
				_obj = _obj->pPrev;
				_listidx--;
			}
		}
		_obj->pPrev->pNext = _node;
		_node->pPrev = _obj->pPrev;
		_node->pNext = _obj;
		_obj->pPrev = _node;
	}
	_Lst->nSize++;
}
BLVoid*
blListErase(INOUT BLList* _Lst,	IN BLVoid* _Node)
{
	struct _ListNode* _obj = (struct _ListNode*)_Node;
	struct _ListNode* _ret = _obj->pPrev ? _obj->pPrev : _obj->pNext;
	if (_obj->pPrev == NULL)
		_Lst->pFirst = _obj->pNext;
	else
		_obj->pPrev->pNext = _obj->pNext;
	if (_obj->pNext == NULL)
		_Lst->pLast = _obj->pPrev;
	else
		_obj->pNext->pPrev = _obj->pPrev;
	free(_obj->pData);
	free(_obj);
	_Lst->nSize--;
	return _ret;
}
