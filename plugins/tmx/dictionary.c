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
#include "dictionary.h"
static struct _DictNode* 
_MinNode(struct _DictNode* _Node)
{
	while (_Node != NULL && (_Node->pLeft != NULL || _Node->pRight != NULL))
	{
		if (_Node->pLeft) 
			_Node = _Node->pLeft;
		else 
			_Node = _Node->pRight;
	}
	return _Node;
}
static struct _DictNode*
_FindNode(BLDictionary* _Dct, BLU32 _Key)
{
	struct _DictNode* _node = _Dct->pRoot;
	while (_node != NULL)
	{
		BLU32 key = _node->nKey;
		if (_Key == key)
			return _node;
		else if (_Key < key)
			_node = _node->pLeft;
		else
			_node = _node->pRight;
	}
	return NULL;
}
static BLBool
_IsLeftChild(struct _DictNode* _Dct) 
{
	return (_Dct->pParent != 0) && (_Dct->pParent->pLeft == _Dct);
}
static BLBool
_IsRightChild(struct _DictNode* _Dct)
{
	return (_Dct->pParent != 0) && (_Dct->pParent->pRight == _Dct);
}
static void
_SetLeftChild(struct _DictNode* _Dct, struct _DictNode* _Node)
{
	_Dct->pLeft = _Node;
	if (_Node)
		_Node->pParent = (_Dct);
}
static void
_SetRightChild(struct _DictNode* _Dct, struct _DictNode* _Node)
{
	_Dct->pRight = _Node;
	if (_Node)
		_Node->pParent = (_Dct);
}
static void
_SetRoot(BLDictionary* _Dct, struct _DictNode* _Root)
{
	_Dct->pRoot = _Root;
	if (_Dct->pRoot != NULL)
	{
		_Dct->pRoot->pParent = NULL;
		_Dct->pRoot->bRed = FALSE;
	}
}
static void
_RotateLeft(BLDictionary* _Dct, struct _DictNode* _Node)
{
	struct _DictNode* right = _Node->pRight;
	_SetRightChild(_Node, right->pLeft);
	if (_IsLeftChild(_Node))
		_SetLeftChild(_Node->pParent, right);
	else if (_IsRightChild(_Node))
		_SetRightChild(_Node->pParent, right);
	else
		_SetRoot(_Dct, right);
	_SetLeftChild(right, _Node);
}
static void
RotateRight(BLDictionary* _Dct, struct _DictNode* _Node)
{
	struct _DictNode* left = _Node->pLeft;
	_SetLeftChild(_Node, left->pRight);
	if (_IsLeftChild(_Node))
		_SetLeftChild(_Node->pParent, left);
	else if (_IsRightChild(_Node))
		_SetRightChild(_Node->pParent, left);
	else
		_SetRoot(_Dct, left);
	_SetRightChild(left, _Node);
}
static BLBool
_InsertNode(BLDictionary* _Dct, struct _DictNode* _Node)
{
	BLBool _result = TRUE;
	if (!_Dct->pRoot)
	{
		_SetRoot(_Dct, _Node);
		_Dct->nSize = 1;
	}
	else
	{
		struct _DictNode* _node = _Dct->pRoot;
		BLU32 _newkey = _Node->nKey;
		while (_node)
		{
			BLU32 _key = (_node->nKey);
			if (_newkey == _key)
			{
				_result = FALSE;
				_node = NULL;
			}
			else if (_newkey < _key)
			{
				if (_node->pLeft == NULL)
				{
					_SetLeftChild(_node, _Node);
					_node = NULL;
				}
				else
					_node = _node->pLeft;
			}
			else
			{
				if (_node->pRight == NULL)
				{
					_SetRightChild(_node, _Node);
					_node = NULL;
				}
				else
					_node = _node->pRight;
			}
		}
		if (_result)
			++_Dct->nSize;
	}
	return _result;
}
static void
_NodeInc(struct _DictNode** _Node)
{
	if (*_Node == NULL)
		return;
	if (_IsLeftChild(*_Node) && (*_Node)->pParent->pRight)
		*_Node = _MinNode((*_Node)->pParent->pRight);
	else
		*_Node = (*_Node)->pParent;
}
BLDictionary* 
blGenDict(IN BLBool _Withthread)
{
	BLDictionary* _ret = (BLDictionary*)malloc(sizeof(BLDictionary));
	_ret->nSize = 0;
	_ret->pRoot = NULL;
	return _ret;
}
BLVoid
blDeleteDict(INOUT BLDictionary* _Dct)
{
	struct _DictNode* _current = _MinNode(_Dct->pRoot);
	while (_current != NULL)
	{
		struct _DictNode* _node = _current;
		_NodeInc(&_current);
		free(_node);
	}
	_Dct->pRoot = NULL;
	_Dct->nSize = 0;
	free(_Dct);
	_Dct = NULL;
}
BLVoid
blClearDict(INOUT BLDictionary* _Dct)
{
	struct _DictNode* _current = _MinNode(_Dct->pRoot);
	while (_current != NULL)
	{
		struct _DictNode* _node = _current;
		_NodeInc(&_current);
		free(_node);
	}
	_Dct->pRoot = NULL;
	_Dct->nSize = 0;
}
BLVoid*
blDictElement(IN BLDictionary* _Dct, IN BLU32 _Key)
{
	struct _DictNode* _node = _FindNode((BLDictionary*)_Dct, _Key);
	if (_node)
		return _node->pValue;
	else
		return NULL;
}
BLVoid* 
blDictRootElement(IN BLDictionary* _Dct)
{
	return _Dct->pRoot->pValue;
}
BLVoid
blDictInsert(INOUT BLDictionary* _Dct, IN BLU32 _Key, IN BLVoid* _Data)
{
	struct _DictNode* _newnode = (struct _DictNode*)malloc(sizeof(struct _DictNode));
	_newnode->nKey = _Key;
	_newnode->pLeft = NULL;
	_newnode->pRight = NULL;
	_newnode->pParent = NULL;
	_newnode->bRed = TRUE;
	_newnode->pValue = (BLVoid*)_Data;
	if (!_InsertNode(_Dct, _newnode))
	{
		free(_newnode);
		return;
	}
	while (_newnode->pParent != NULL && (_newnode->pParent->bRed))
	{
		if (_IsLeftChild(_newnode->pParent))
		{
			struct _DictNode* _newnodesuncle = _newnode->pParent->pParent->pRight;
			if (_newnodesuncle != NULL && _newnodesuncle->bRed)
			{
				_newnode->pParent->bRed = FALSE;
				_newnodesuncle->bRed = FALSE;
				_newnode->pParent->pParent->bRed = TRUE;
				_newnode = _newnode->pParent->pParent;
			}
			else
			{
				if (_IsRightChild(_newnode))
				{
					_newnode = _newnode->pParent;
					_RotateLeft(_Dct, _newnode);
				}
				_newnode->pParent->bRed = FALSE;
				_newnode->pParent->pParent->bRed = TRUE;
				RotateRight(_Dct, _newnode->pParent->pParent);
			}
		}
		else
		{
			struct _DictNode* _newnodesuncle = _newnode->pParent->pParent->pLeft;
			if (_newnodesuncle != NULL && _newnodesuncle->bRed)
			{
				_newnode->pParent->bRed = FALSE;
				_newnodesuncle->bRed = FALSE;
				_newnode->pParent->pParent->bRed = TRUE;
				_newnode = _newnode->pParent->pParent;
			}
			else
			{
				if (_IsLeftChild(_newnode))
				{
					_newnode = _newnode->pParent;
					RotateRight(_Dct, _newnode);
				}
				_newnode->pParent->bRed = FALSE;
				_newnode->pParent->pParent->bRed = TRUE;
				_RotateLeft(_Dct, _newnode->pParent->pParent);
			}
		}
	}
	_Dct->pRoot->bRed = FALSE;
}
BLVoid
blDictErase(INOUT BLDictionary* _Dct, IN BLU32 _Key)
{
	struct _DictNode* _node = _FindNode(_Dct, _Key);
	if (!_node)
		return;
	while (_node->pRight)
		_RotateLeft(_Dct, _node);
	struct _DictNode* left = _node->pLeft;
	if (_IsLeftChild(_node))
		_SetLeftChild(_node->pParent, left);
	else if (_IsRightChild(_node))
		_SetRightChild(_node->pParent, left);
	else
		_SetRoot(_Dct, left);
	free(_node);
	--_Dct->nSize;
}
BLBool 
blDictTraval(INOUT BLDictionary* _Dct, OUT BLVoid** _Node, OUT BLU32* _Key, OUT BLVoid** _Data)
{
	if (!_Dct->nSize)
		return FALSE;
	struct _DictNode* _node = (struct _DictNode*)*_Node;
	if (*_Data == NULL)
	{
		_node = _MinNode(_Dct->pRoot);
		*_Key = _node->nKey;
		*_Data = _node->pValue;
		_NodeInc(&_node);
		*_Node = _node;
		return TRUE;
	}
	else if (*_Node != NULL)
	{
		*_Key = _node->nKey;
		*_Data = ((struct _DictNode*)*_Node)->pValue;
		_NodeInc(&_node);
		*_Node = _node;
		return TRUE;
	}
	else
		return FALSE;
}

