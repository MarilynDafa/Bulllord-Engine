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
#include "internal/dictionary.h"
static BLVoid
_FreeNodes(struct _DictNode *_Node)
{
	if (!_Node) 
		return;
	if (_Node->pLeft)
		_FreeNodes(_Node->pLeft);
	if (_Node->pRight)
		_FreeNodes(_Node->pRight);
	if (_Node)
	{
		free(_Node->pData);
		free(_Node);
	}
}
static BLBool
_IsNodeRed(struct _DictNode* _Node)
{
	return (_Node != NULL) ? _Node->bRed : FALSE;
}
static struct _DictNode*
_FlipNodeClr(struct _DictNode* _Node)
{
	_Node->bRed = !(_Node->bRed);
	_Node->pLeft->bRed = !(_Node->pLeft->bRed);
	_Node->pRight->bRed = !(_Node->pRight->bRed);
	return _Node;
}
static struct _DictNode*
_RotateLeft(struct _DictNode* _Node)
{
	struct _DictNode* _x = _Node->pRight;
	_Node->pRight = _x->pLeft;
	_x->pLeft = _Node;
	_x->bRed = _x->pLeft->bRed;
	_x->pLeft->bRed = TRUE;
	return _x;
}
static struct _DictNode*
_RotateRight(struct _DictNode* _Node)
{
	struct _DictNode* _x = _Node->pLeft;
	_Node->pLeft = _x->pRight;
	_x->pRight = _Node;
	_x->bRed = _x->pRight->bRed;
	_x->pRight->bRed = TRUE;
	return _x;
}
static struct _DictNode*
_MoveRedLeft(struct _DictNode* _Node)
{
	_FlipNodeClr(_Node);
	if (_Node->pRight && _IsNodeRed(_Node->pRight->pLeft)) 
	{
		_Node->pRight = _RotateRight(_Node->pRight);
		_Node = _RotateLeft(_Node);
		_FlipNodeClr(_Node);
	}
	return _Node;
}
static struct _DictNode*
_MoveRedRight(struct _DictNode* _Node)
{
	_FlipNodeClr(_Node);
	if (_Node->pLeft && _IsNodeRed(_Node->pLeft->pLeft)) 
	{
		_Node = _RotateRight(_Node);
		_FlipNodeClr(_Node);
	}
	return _Node;
}
static struct _DictNode*
_FixNode(struct _DictNode* _Node)
{
	if (_IsNodeRed(_Node->pRight))
		_Node = _RotateLeft(_Node);
	if (_Node->pLeft && _IsNodeRed(_Node->pLeft) && _IsNodeRed(_Node->pLeft->pLeft))
		_Node = _RotateRight(_Node);
	if (_IsNodeRed(_Node->pLeft) && _IsNodeRed(_Node->pRight))
		_FlipNodeClr(_Node);
	return _Node;
}
static struct _DictNode*
_EraseMinNode(struct _DictNode* _Node)
{
	if (_Node->pLeft == NULL) 
	{
		free(_Node->pData);
		return NULL;
	}
	if (!_IsNodeRed(_Node->pLeft) && !_IsNodeRed(_Node->pLeft->pLeft))
		_Node = _MoveRedLeft(_Node);
	_Node->pLeft = _EraseMinNode(_Node->pLeft);
	return _FixNode(_Node);
}
static struct _DictNode*
_EraseNode(BLDictionary* _Dct, struct _DictNode* _Node, BLU32 _Key)
{
	if (_Key < _Node->nKey)
	{
		if (_Node->pLeft && (!_IsNodeRed(_Node->pLeft) && !_IsNodeRed(_Node->pLeft->pLeft)))
			_Node = _MoveRedLeft(_Node);
		_Node->pLeft = _EraseNode(_Dct, _Node->pLeft, _Key);
	} 
	else
	{
		if (_IsNodeRed(_Node->pLeft))
			_Node = _RotateRight(_Node);
		if (_Key == _Node->nKey&& _Node->pRight == NULL) 
		{
			free(_Node->pData);
			free(_Node);
			_Dct->nSize--;
			return NULL;
		}
		if (_Node->pRight != NULL && (!_IsNodeRed(_Node->pRight) && !_IsNodeRed(_Node->pRight->pLeft))) 
			_Node = _MoveRedRight(_Node);
		if (_Key == _Node->nKey)
		{
			struct _DictNode* _minnode;
			for (_minnode = _Node->pRight; _minnode->pLeft != NULL; _minnode = _minnode->pLeft)
				;
			_Node->nKey = _minnode->nKey;
			memcpy(_Node->pData ,_minnode->pData, sizeof(BLVoid*));
			_Node->pRight = _EraseMinNode(_Node->pRight);
			_Dct->nSize--;
		} 
		else 
			_Node->pRight = _EraseNode(_Dct, _Node->pRight, _Key);
	}
	return _FixNode(_Node);
}
static struct _DictNode*
_InsertNode(BLDictionary* _Dct, struct _DictNode* _Node, BLU32 _Key, const BLVoid* _Data)
{
	struct _DictNode* _ret;
	if (_Node == NULL)
	{
		_Dct->nSize++;
		_ret = (struct _DictNode*)malloc(sizeof(struct _DictNode));
		_Node = _ret;
		_Node->bRed = TRUE;
		_Node->nKey = _Key;
		_Node->pData = (BLVoid**)malloc(sizeof(BLVoid*));
		_Node->pNext = _Node->pLeft = _Node->pRight = NULL;
		_Node->nTid = 0;
		memcpy(_Node->pData, &_Data, sizeof(BLVoid*));
		return _Node;
	}
	if (_IsNodeRed(_Node->pLeft) && _IsNodeRed(_Node->pRight))
		_FlipNodeClr(_Node);
	if (_Node->nKey == _Key)
		memcpy(_Node->pData, &_Data, sizeof(BLVoid*));
	else if (_Node->nKey < _Key)
		_Node->pRight = _InsertNode(_Dct, _Node->pRight,_Key, _Data);
	else
		_Node->pLeft = _InsertNode(_Dct, _Node->pLeft, _Key, _Data);
	if (_IsNodeRed(_Node->pRight))
		_Node = _RotateLeft(_Node);
	if (_IsNodeRed(_Node->pLeft) && _IsNodeRed(_Node->pLeft->pLeft))
		_Node = _RotateRight(_Node);
	return _Node;
}
BLDictionary* 
blGenDict(IN BLBool _Withthread)
{
	BLDictionary* _ret = (BLDictionary*)malloc(sizeof(BLDictionary));
	_ret->nSize = 0;
	_ret->nTid = 1;
	_ret->pRoot = NULL;
	if (_Withthread)
		_ret->pMutex = blGenMutex();
	else
		_ret->pMutex = NULL;
	return _ret;
}
BLVoid
blDeleteDict(INOUT BLDictionary* _Dct)
{
	_FreeNodes(_Dct->pRoot);
	_Dct->pRoot = NULL;
	_Dct->nSize = 0;
	if (_Dct->pMutex)
		blDeleteMutex(_Dct->pMutex);
	free(_Dct);
	_Dct = NULL;
}
BLVoid
blClearDict(INOUT BLDictionary* _Dct)
{
	_FreeNodes(_Dct->pRoot);
	_Dct->pRoot = NULL;
	_Dct->nSize = 0;
}
BLVoid*
blDictElement(IN BLDictionary* _Dct, IN BLU32 _Key)
{
	struct _DictNode* _node;
	for (_node = _Dct->pRoot; _node != NULL;) 
	{
		if (_Key == _node->nKey)
			return *_node->pData;
		_node = (_Key < _node->nKey) ? _node->pLeft : _node->pRight;
	}
	return _node?*_node->pData:NULL;
}
BLVoid
blDictInsert(INOUT BLDictionary* _Dct, IN BLU32 _Key, IN BLVoid* _Data)
{
	struct _DictNode* _root;
	_root = _InsertNode(_Dct, _Dct->pRoot, _Key, _Data);
	_root->bRed = FALSE;
	_Dct->pRoot = _root;
}
BLVoid
blDictErase(INOUT BLDictionary* _Dct, IN BLU32 _Key)
{
	_Dct->pRoot = _EraseNode(_Dct, _Dct->pRoot, _Key);
	if (_Dct->pRoot)
		_Dct->pRoot->bRed = FALSE;
}
BLBool 
blDictTraval(INOUT BLDictionary* _Dct, OUT BLVoid* _Node, OUT BLVoid** _Data)
{
	struct _DictNode* _cursor;
	struct _DictNode* _node = (struct _DictNode*) _Node;
	BLU8 _tid = _node->nTid;
	if (!_node->pNext)
	{
		if (!_Dct->pRoot)
			return FALSE;
		_tid = (++_Dct->nTid);
	}
	_cursor = ((_node->pNext) ? _node->pNext : _Dct->pRoot);
	while (_cursor)
	{
		if (_cursor->pLeft && _cursor->pLeft->nTid != _tid)
		{
			_cursor->pLeft->pNext = _cursor;
			_cursor = _cursor->pLeft;
			continue;
		}
		else if (_cursor->nTid != _tid)
		{
			_cursor->nTid = _tid;
			*_node = *_cursor;
			*_Data = *_node->pData;
			_node->pNext = _cursor;
			return TRUE;
		}
		else if (_cursor->pRight && _cursor->pRight->nTid != _tid)
		{
			_cursor->pRight->pNext = _cursor;
			_cursor = _cursor->pRight;
			continue;
		}
		_cursor = _cursor->pNext;
	}
	(++_Dct->nTid);
	return FALSE;
}

