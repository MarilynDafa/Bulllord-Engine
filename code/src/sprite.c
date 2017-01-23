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
#include "../headers/sprite.h"
#include "../headers/gpu.h"
#include "../headers/system.h"
#include "../headers/utils.h"
#include "../headers/ui.h"
#include "internal/array.h"
#include "internal/internal.h"
#include "internal/mathematics.h"
typedef struct _SpriteAction{
    struct _SpriteAction* pNext;
    struct _SpriteAction* pNeighbor;
	BLBool bLoop;
	BLBool bParallel;
	BLF32 fCurTime;
	BLF32 fTotalTime;
	BLEnum eActionType;
	union {
		struct _Move {
			BLF32* pXPath;
			BLF32* pYPath;
			BLU32 nPathNum;
			BLF32 fAcceleration;
			BLF32 fVelocity;
		}sMove;
		struct _Rotate {
			BLF32 fAngle;
		}sRotate;
		struct _Scale {
			BLF32 fXInitScale;
			BLF32 fYInitScale;
			BLF32 fXScale;
			BLF32 fYScale;
		}sScale;
		struct _ZValue {
			BLF32 fZv;
		}sZValue;
		struct _Alpha {
			BLF32 fInitAlpha;
			BLF32 fAlpha;
		}sAlpha;
		struct _Dead {
			BLBool bDead;
		}sDead;
	} uAction;
}_BLSpriteAction;
typedef struct _EmitParam {
	BLEnum eEmitter;
	BLU32 nEmitterParam;
	BLF32 fLife;
	BLU32 nMaxAlive;
	BLU32 nGenPerSec;
	BLF32 fVelocityX;
	BLF32 fVelocityY;
	BLF32 fAccelerationX;
	BLF32 fAccelerationY;
	BLF32 fDisturbanceX;
	BLF32 fDisturbanceY;
	BLF32 fEmitAngle;
	BLU32 nFadeColor;
	BLF32 fFadeScale;
}_BLEmitParam;
typedef struct _SpriteNode{
    struct _SpriteNode* pParent;
    struct _SpriteNode** pChildren;
	_BLSpriteAction* pAction;
	_BLSpriteAction* pCurAction;
	_BLEmitParam* pEmitParam;
    BLGuid nID;
    BLAnsi aTag[32];
	BLAnsi aFilename[260];
	BLAnsi aArchive[260];
    BLGuid nGBO;
    BLGuid nTex;
    BLVec2 sSize;
    BLVec2 sPos;
    BLVec2 sPivot;
	BLVec2 sUVScroll;
    BLRect sScissor;
	BLU32 nFPS;
    BLF32 fRotate;
    BLF32 fScaleX;
    BLF32 fScaleY;
    BLF32 fAlpha;
    BLF32 fZValue;
    BLU32 nDyeColor;
    BLBool bDye;
    BLU32 nStrokeColor;
    BLU32 nStrokePixel;
    BLU32 nGlowColor;
    BLU32 nGlowPixel;
    BLU32 nChildren;
	BLBool bTile;
	BLBool bValid;
    BLBool bShow;
}_BLSpriteNode;
typedef struct _SpriteMember {
    BLGuid nTech;
    _BLSpriteNode** pNodeList;
	_BLSpriteNode* pCursor;
	BLArray* pTileArray;
	BLRect sViewport;
	BLVec2 sCursorPos;
	BLBool bParallelEdit;
    BLU32 nNodeNum;
    BLU32 nNodeCap;
	BLF32 nLastTime;
}_BLSpriteMember;
static _BLSpriteMember* _PrSpriteMem = NULL;
extern BLBool _FetchResource(const BLAnsi*, const BLAnsi*, BLVoid**, BLGuid, BLBool(*)(BLVoid*, const BLAnsi*, const BLAnsi*), BLBool(*)(BLVoid*), BLBool);
extern BLBool _DiscardResource(BLGuid, BLBool(*)(BLVoid*), BLBool(*)(BLVoid*));
BLBool
_UseCustomCursor()
{
	return _PrSpriteMem->pCursor ? TRUE : FALSE;
}
static const BLVoid
_MouseSubscriber(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam)
{
	if (_Type == BL_ME_MOVE)
	{
		_PrSpriteMem->sCursorPos.fX = LOWU16(_UParam);
		_PrSpriteMem->sCursorPos.fY = HIGU16(_UParam);
	}
}
static BLVoid
_AddToNodeList(_BLSpriteNode* _Node)
{
    BLS32 _mid = -1, _left = 0, _right = _PrSpriteMem->nNodeNum;
	if (_Node->bTile)
		return;
    while (_left < _right)
    {
        _mid = (_left + _right) >> 1;
        if (_PrSpriteMem->pNodeList[_mid]->fZValue > _Node->fZValue)
            _right = _mid;
        else if (_PrSpriteMem->pNodeList[_mid]->fZValue < _Node->fZValue)
        {
            _left = _mid;
            if (_right - _left == 1)
            {
                _mid = _right;
                break;
            }
        }
        else
            break;
    }
    if (_mid != -1)
    {
        if (_PrSpriteMem->nNodeCap <= _PrSpriteMem->nNodeNum)
        {
            _PrSpriteMem->nNodeCap = _PrSpriteMem->nNodeNum + 1;
            _PrSpriteMem->pNodeList = (_BLSpriteNode**)realloc(_PrSpriteMem->pNodeList, _PrSpriteMem->nNodeCap * sizeof(_BLSpriteNode*));
        }
        memmove(_PrSpriteMem->pNodeList + _mid + 1, _PrSpriteMem->pNodeList + _mid, sizeof(_BLSpriteNode*) * (_PrSpriteMem->nNodeNum - _mid));
        _PrSpriteMem->nNodeNum++;
        _PrSpriteMem->pNodeList[_mid] = _Node;
    }
    else
    {
        assert(_PrSpriteMem->nNodeNum == 0);
        _PrSpriteMem->nNodeNum++;
        _PrSpriteMem->nNodeCap++;
        _PrSpriteMem->pNodeList = (_BLSpriteNode**)realloc(_PrSpriteMem->pNodeList, _PrSpriteMem->nNodeNum * sizeof(_BLSpriteNode*));
        _PrSpriteMem->pNodeList[0] = _Node;
    }
}
static BLVoid dump2(_BLSpriteNode* _node)
{
    if(!_node)
        return;
    for(unsigned int i = 0 ;i < _node->nChildren ; ++i)
    {
        _BLSpriteNode* _tmp = _node->pChildren[i];
        while(_tmp->pParent)
        {
          printf("    >");
            _tmp = _tmp->pParent;
        }
        printf("%s:%f  x:%f y:%f\n", _node->pChildren[i]->aTag, _node->pChildren[i]->fZValue,
               _node->pChildren[i]->sPos.fX, _node->pChildren[i]->sPos.fY);
        dump2(_node->pChildren[i]);
    }
    
}
BLVoid dump()
{
    for (unsigned int i = 0 ; i < _PrSpriteMem->nNodeNum; ++i) {
        printf("%s:%f x:%f y:%f\n", _PrSpriteMem->pNodeList[i]->aTag, _PrSpriteMem->pNodeList[i]->fZValue,
               _PrSpriteMem->pNodeList[i]->sPos.fX, _PrSpriteMem->pNodeList[i]->sPos.fY);
        dump2(_PrSpriteMem->pNodeList[i]);
    }
    printf("----------------------\n");
}
BLVoid dump32(BLGuid _ID)
{
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (_node->pAction)
	{
		_BLSpriteAction* _tmp = _node->pAction;
		while (_tmp)
		{
			_BLSpriteAction* _tmpnext = _tmp->pNext;
			if (_tmp->pNeighbor)
			{
				_BLSpriteAction* _tmpneb = _tmp->pNeighbor;
				while (_tmp)
				{
					printf("%d   ", _tmp->eActionType);
					_tmp = _tmpneb;
					_tmpneb = _tmp ? _tmp->pNeighbor : NULL;
				}
				printf("\n");
			}
			else
				printf("%d\n", _tmp->eActionType);
			_tmp = _tmpnext;
		}
	}
}
static BLVoid
_RemoveFromNodeList(_BLSpriteNode* _Node)
{
    BLBool _found = FALSE;
    BLS32 _mid = -1, _left = 0, _right = _PrSpriteMem->nNodeNum;
	if (_Node->bTile)
		return;
    while (_left < _right)
    {
        _mid = (_left + _right) >> 1;
        if (_PrSpriteMem->pNodeList[_mid]->fZValue > _Node->fZValue)
            _right = _mid;
        else if (_PrSpriteMem->pNodeList[_mid]->fZValue < _Node->fZValue)
            _left = _mid;
        else
        {
            if (_PrSpriteMem->pNodeList[_mid] == _Node)
            {
                _found = TRUE;
                break;
            }
            else
            {
                BLS32 _tmp = _mid;
                while (_PrSpriteMem->pNodeList[_tmp]->fZValue == _Node->fZValue)
                {
                    _tmp++;
                    if (_tmp >= (BLS32)_PrSpriteMem->nNodeNum)
                        break;
                    if (_PrSpriteMem->pNodeList[_tmp] == _Node)
                    {
                        _found = TRUE;
                        _mid = _tmp;
                        break;
                    }
                }
                _tmp = _mid;
                while (_PrSpriteMem->pNodeList[_tmp]->fZValue == _Node->fZValue)
                {
                    _tmp--;
                    if (_tmp < 0)
                        break;
                    if (_PrSpriteMem->pNodeList[_tmp] == _Node)
                    {
                        _found = TRUE;
                        _mid = _tmp;
                        break;
                    }
                }
                break;
            }
        }
    }
    if (_found)
    {
        memmove(_PrSpriteMem->pNodeList + _mid, _PrSpriteMem->pNodeList + 1 + _mid, sizeof(_BLSpriteNode*) * (_PrSpriteMem->nNodeNum - _mid - 1));
        _PrSpriteMem->nNodeNum--;
    }
}
static BLBool
_SpriteSetup(BLVoid* _Src)
{
    _BLSpriteNode* _node = (_BLSpriteNode*)_Src;
    _AddToNodeList(_node);
	_node->bValid = TRUE;
    return TRUE;
}
static BLBool
_SpriteRelease(BLVoid* _Src)
{
    _BLSpriteNode* _node = (_BLSpriteNode*)_Src;
	if (!_node->bTile)
	{
		if (_node->pParent)
		{
			BLBool _found = FALSE;
			for (BLU32 _idx = 0; _idx < _node->pParent->nChildren; ++_idx)
			{
				if (_node->pParent->pChildren[_idx] == _node)
				{
					memmove(_node->pParent->pChildren + _idx, _node->pParent->pChildren + _idx + 1, (_node->pParent->nChildren - _idx - 1) * sizeof(_BLSpriteNode*));
					_found = TRUE;
					break;
				}
			}
			if (!_found)
				return FALSE;
			_node->pParent->nChildren--;
			_node->pParent = NULL;
		}
		else
			_RemoveFromNodeList(_node);
	}
	else
	{
		BLU32 _idx = 0;
		FOREACH_ARRAY(_BLSpriteNode*, _nodeiter, _PrSpriteMem->pTileArray)
		{			
			if (_nodeiter == _node)
			{
				blArrayErase(_PrSpriteMem->pTileArray, _idx);
				break;
			}
			_idx++;
		}
	}
    return TRUE;
}
static BLBool
_LoadSprite(BLVoid* _Src, const BLAnsi* _Filename, const BLAnsi* _Archive)
{
    return TRUE;
}
static BLBool
_UnloadSprite(BLVoid* _Src)
{
	_BLSpriteNode* _node = (_BLSpriteNode*)_Src;
	if (_node->pAction)
	{
		_BLSpriteAction* _tmp = _node->pAction;
		while (_tmp)
		{
			_BLSpriteAction* _tmpnext = _tmp->pNext;
			if (_tmp->pNeighbor)
			{
				_BLSpriteAction* _tmpneb = _tmp->pNeighbor;
				while (_tmp)
				{
					if (_tmp->eActionType == SPACTION_MOVE_INTERNAL)
					{
						free(_tmp->uAction.sMove.pXPath);
						free(_tmp->uAction.sMove.pYPath);
					}
					free(_tmp);
					_tmp = _tmpneb;
					_tmpneb = _tmp ? _tmp->pNeighbor : NULL;
				}
			}
			else
			{
				if (_tmp->eActionType == SPACTION_MOVE_INTERNAL)
				{
					free(_tmp->uAction.sMove.pXPath);
					free(_tmp->uAction.sMove.pYPath);
				}
				free(_tmp);
			}
			_tmp = _tmpnext;
		}
	}
	if (_node->pEmitParam)
		free(_node->pEmitParam);
	_node->bValid = FALSE;
    return TRUE;
}
static BLVoid
_SpriteUpdate(BLF32 _Delta)
{
	for (BLU32 _idx = 0; _idx < _PrSpriteMem->nNodeNum;)
	{
		BLBool _delete = FALSE;
		_BLSpriteNode* _node = _PrSpriteMem->pNodeList[_idx];
		if (_node->pCurAction)
		{
			_BLSpriteAction* _action = _node->pCurAction;
			do
			{
				_action->fCurTime += _Delta;
				switch (_action->eActionType)
				{
				case SPACTION_MOVE_INTERNAL:
				{
					BLF32 _s = _action->uAction.sMove.fVelocity * _action->fCurTime + 0.5f * _action->uAction.sMove.fAcceleration * _action->fCurTime * _action->fCurTime;
					for (BLU32 _idx = 1; _idx < _action->uAction.sMove.nPathNum; ++_idx)
					{
						BLF32* _xp = _action->uAction.sMove.pXPath;
						BLF32* _yp = _action->uAction.sMove.pYPath;
						BLF32 _subs = sqrtf((_xp[_idx] - _xp[_idx - 1]) * (_xp[_idx] - _xp[_idx - 1]) + (_yp[_idx] - _yp[_idx - 1]) * (_yp[_idx] - _yp[_idx - 1]));
						if (_s > _subs)
							_s -= _subs;
						else
						{
							BLF32 _ratio = _s / _subs;
							_node->sPos.fX = _xp[_idx - 1] + _ratio * (_xp[_idx] - _xp[_idx - 1]);
							_node->sPos.fY = _yp[_idx - 1] + _ratio * (_yp[_idx] - _yp[_idx - 1]);
						}
					}
				}
				break;
				case SPACTION_ROTATE_INTERNAL:
				{
					_node->fRotate += _action->fCurTime / _action->fTotalTime * _action->uAction.sRotate.fAngle * PI_INTERNAL / 180.0f;
				}
				break;
				case SPACTION_SCALE_INTERNAL:
				{
					_node->fScaleX = _action->fCurTime / _action->fTotalTime * (_action->uAction.sScale.fXScale - _action->uAction.sScale.fXInitScale) + _action->uAction.sScale.fXInitScale;
					_node->fScaleY = _action->fCurTime / _action->fTotalTime * (_action->uAction.sScale.fYScale - _action->uAction.sScale.fYInitScale) + _action->uAction.sScale.fYInitScale;
				}
				break;
				case SPACTION_ALPHA_INTERNAL:
				{
					_node->fAlpha = _action->fCurTime / _action->fTotalTime * (_action->uAction.sAlpha.fAlpha - _action->uAction.sAlpha.fInitAlpha) + _action->uAction.sAlpha.fInitAlpha;
				}
				break;
				case SPACTION_DEAD_INTERNAL:
				{
					blDeleteSprite(_node->nID);
					_delete = TRUE;
				}
				break;
				default:
					break;
				}
				_action = _action->pNeighbor;
			} while (_action);
			if (_node->pCurAction->fCurTime >= _node->pCurAction->fTotalTime && !_delete)
			{
				if (!_node->pCurAction->bLoop)
				{
					if (!_node->pCurAction->pNext)
					{
						_BLSpriteAction* _tmp = _node->pAction;
						while (_tmp)
						{
							_BLSpriteAction* _tmpnext = _tmp->pNext;
							if (_tmp->pNeighbor)
							{
								_BLSpriteAction* _tmpneb = _tmp->pNeighbor;
								while (_tmp)
								{
									if (_tmp->eActionType == SPACTION_MOVE_INTERNAL)
									{
										free(_tmp->uAction.sMove.pXPath);
										free(_tmp->uAction.sMove.pYPath);
									}
									free(_tmp);
									_tmp = _tmpneb;
									_tmpneb = _tmp ? _tmp->pNeighbor : NULL;
								}
							}
							else
							{
								if (_tmp->eActionType == SPACTION_MOVE_INTERNAL)
								{
									free(_tmp->uAction.sMove.pXPath);
									free(_tmp->uAction.sMove.pYPath);
								}
								free(_tmp);
							}
							_tmp = _tmpnext;
						}
						_node->pAction = _node->pCurAction = NULL;
					}
					else
						_node->pCurAction = _node->pCurAction->pNext;
				}
				else
				{
					_node->pCurAction->fCurTime = 0.f;
					switch (_node->pCurAction->eActionType)
					{
					case SPACTION_MOVE_INTERNAL:
						{
							_node->sPos.fX = _node->pCurAction->uAction.sMove.pXPath[0];
							_node->sPos.fY = _node->pCurAction->uAction.sMove.pYPath[0];
						}
						break;
					case SPACTION_SCALE_INTERNAL:
						{
							_node->fScaleX = _node->pCurAction->uAction.sScale.fXInitScale;
							_node->fScaleY = _node->pCurAction->uAction.sScale.fYInitScale;
						}
						break;
					case SPACTION_ALPHA_INTERNAL:
						{
							_node->fAlpha = _node->pCurAction->uAction.sAlpha.fInitAlpha;
						}
						break;
					default:
						break;
					}
				}
			}
		}
		if (!_delete)
			_idx++;
	}
}
static BLVoid
_SpriteDraw(_BLSpriteNode* _Node, BLF32 _Mat[6])
{
	BLF32 _minx = -_Node->sSize.fX * 0.5f;
	BLF32 _miny = -_Node->sSize.fY * 0.5f;
	BLF32 _maxx = _Node->sSize.fX * 0.5f;
	BLF32 _maxy = _Node->sSize.fY * 0.5f;
	BLF32 _ltx = (_minx * _Mat[0]) + (_miny * _Mat[2]) + _Mat[4];
	BLF32 _lty = (_minx * _Mat[1]) + (_miny * _Mat[3]) + _Mat[5];
	BLF32 _rtx = (_maxx * _Mat[0]) + (_miny * _Mat[2]) + _Mat[4];
	BLF32 _rty = (_maxx * _Mat[1]) + (_miny * _Mat[3]) + _Mat[5];
	BLF32 _lbx = (_minx * _Mat[0]) + (_maxy * _Mat[2]) + _Mat[4];
	BLF32 _lby = (_minx * _Mat[1]) + (_maxy * _Mat[3]) + _Mat[5];
	BLF32 _rbx = (_maxx * _Mat[0]) + (_maxy * _Mat[2]) + _Mat[4];
	BLF32 _rby = (_maxx * _Mat[1]) + (_maxy * _Mat[3]) + _Mat[5];
	for (BLU32 _idx = 0; _idx < _Node->nChildren; ++_idx)
	{
		_BLSpriteNode* _chnode = _Node->pChildren[_idx];
		BLF32 _mat[6], _rmat[6];
		BLF32 _cos = cosf(_chnode->fRotate);
		BLF32 _sin = sinf(_chnode->fRotate);
		BLF32 _pivotx = (0.5f - _chnode->sPivot.fX) * _chnode->sSize.fX;
		BLF32 _pivoty = (0.5f - _chnode->sPivot.fY) * _chnode->sSize.fY;
		_mat[0] = (_chnode->fScaleX * _cos);
		_mat[1] = (_chnode->fScaleX * _sin);
		_mat[2] = (-_chnode->fScaleY * _sin);
		_mat[3] = (_chnode->fScaleY * _cos);
		_mat[4] = ((-_pivotx * _chnode->fScaleX) * _cos) + ((_pivoty * _chnode->fScaleY) * _sin) + _pivotx + _chnode->sPos.fX;
		_mat[5] = ((-_pivotx * _chnode->fScaleX) * _sin) + ((-_pivoty * _chnode->fScaleY) * _cos) + _pivoty + _chnode->sPos.fY;
		_rmat[0] = (_mat[0] * _Mat[0]) + (_mat[1] * _Mat[2]);
		_rmat[1] = (_mat[0] * _Mat[1]) + (_mat[1] * _Mat[3]);
		_rmat[2] = (_mat[2] * _Mat[0]) + (_mat[3] * _Mat[2]);
		_rmat[3] = (_mat[2] * _Mat[1]) + (_mat[3] * _Mat[3]);
		_rmat[4] = (_mat[4] * _Mat[0]) + (_mat[5] * _Mat[2]) + _Mat[4];
		_rmat[5] = (_mat[4] * _Mat[1]) + (_mat[5] * _Mat[3]) + _Mat[5];
		_SpriteDraw(_chnode, _rmat);
	}
}
BLVoid
_SpriteInit()
{
    _PrSpriteMem = (_BLSpriteMember*)malloc(sizeof(_BLSpriteMember));
    _PrSpriteMem->pNodeList = NULL;
    _PrSpriteMem->nNodeNum = 0;
    _PrSpriteMem->nNodeCap = 0;
	_PrSpriteMem->pCursor = NULL;
	_PrSpriteMem->nLastTime = 0.f;
	_PrSpriteMem->sViewport.sLT.fX = _PrSpriteMem->sViewport.sLT.fY = 0.f;
	BLU32 _w, _h;
	blQueryResolution(&_w, &_h);
	_PrSpriteMem->sViewport.sRB.fX = (BLF32)_w;
	_PrSpriteMem->sViewport.sRB.fY = (BLF32)_h;
	_PrSpriteMem->pTileArray = blGenArray(FALSE);
	blSubscribeEvent(BL_ET_MOUSE, _MouseSubscriber);
}
BLVoid
_SpriteStep(BLF32 _Delta)
{
	_PrSpriteMem->nLastTime += _Delta;
	if (_PrSpriteMem->nLastTime >= 0.00833333f)
	{
		_SpriteUpdate(_PrSpriteMem->nLastTime);
		_PrSpriteMem->nLastTime = 0.f;
	}
	BLRect _scalevp = _PrSpriteMem->sViewport;
	_scalevp.sLT.fX -= (_PrSpriteMem->sViewport.sRB.fX - _PrSpriteMem->sViewport.sLT.fX) * 0.5f;
	_scalevp.sRB.fX += (_PrSpriteMem->sViewport.sRB.fX - _PrSpriteMem->sViewport.sLT.fX) * 0.5f;
	_scalevp.sLT.fY -= (_PrSpriteMem->sViewport.sRB.fY - _PrSpriteMem->sViewport.sLT.fY) * 0.5f;
	_scalevp.sRB.fY += (_PrSpriteMem->sViewport.sRB.fY - _PrSpriteMem->sViewport.sLT.fY) * 0.5f;
	{
		FOREACH_ARRAY(_BLSpriteNode*, _nodeiter, _PrSpriteMem->pTileArray)
		{
			BLRect _sprc;
			_sprc.sLT.fX = _nodeiter->sPos.fX;
			_sprc.sLT.fY = _nodeiter->sPos.fY;
			_sprc.sRB.fX = _nodeiter->sSize.fX + _sprc.sLT.fX;
			_sprc.sRB.fY = _nodeiter->sSize.fY + _sprc.sLT.fY;
			BLRect _ret = blRectIntersects(&_sprc, &_sprc);
			if (_ret.sRB.fY < _ret.sLT.fY || _ret.sRB.fX < _ret.sLT.fX)
			{
				_nodeiter->bShow = FALSE;
				if (_nodeiter->bValid)
					_DiscardResource(_nodeiter->nID, _UnloadSprite, _SpriteRelease);
			}
			else
			{
				_nodeiter->bShow = TRUE;
				if (!_nodeiter->bValid)
					_FetchResource(_nodeiter->aFilename, _nodeiter->aArchive, (BLVoid**)&_nodeiter, _nodeiter->nID, _LoadSprite, _SpriteSetup, TRUE);
				else
				{
					BLF32 _mat[6];
					BLF32 _cos = cosf(_nodeiter->fRotate);
					BLF32 _sin = sinf(_nodeiter->fRotate);
					BLF32 _pivotx = (0.5f - _nodeiter->sPivot.fX) * _nodeiter->sSize.fX;
					BLF32 _pivoty = (0.5f - _nodeiter->sPivot.fY) * _nodeiter->sSize.fY;
					_mat[0] = (_nodeiter->fScaleX * _cos);
					_mat[1] = (_nodeiter->fScaleX * _sin);
					_mat[2] = (-_nodeiter->fScaleY * _sin);
					_mat[3] = (_nodeiter->fScaleY * _cos);
					_mat[4] = ((-_pivotx * _nodeiter->fScaleX) * _cos) + ((_pivoty * _nodeiter->fScaleY) * _sin) + _pivotx + _nodeiter->sPos.fX;
					_mat[5] = ((-_pivotx * _nodeiter->fScaleX) * _sin) + ((-_pivoty * _nodeiter->fScaleY) * _cos) + _pivoty + _nodeiter->sPos.fY;
					_SpriteDraw(_nodeiter, _mat);
				}
			}
		}
	}
	for (BLU32 _idx = 0; _idx < _PrSpriteMem->nNodeNum; ++_idx)
	{
		_BLSpriteNode* _node = _PrSpriteMem->pNodeList[_idx];
		BLF32 _mat[6];
		BLF32 _cos = cosf(_node->fRotate);
		BLF32 _sin = sinf(_node->fRotate);
		BLF32 _pivotx = (0.5f - _node->sPivot.fX) * _node->sSize.fX;
		BLF32 _pivoty = (0.5f - _node->sPivot.fY) * _node->sSize.fY;
		_mat[0] = (_node->fScaleX * _cos);
		_mat[1] = (_node->fScaleX * _sin);
		_mat[2] = (-_node->fScaleY * _sin);
		_mat[3] = (_node->fScaleY * _cos);
		_mat[4] = ((-_pivotx * _node->fScaleX) * _cos) + ((_pivoty * _node->fScaleY) * _sin) + _pivotx + _node->sPos.fX;
		_mat[5] = ((-_pivotx * _node->fScaleX) * _sin) + ((-_pivoty * _node->fScaleY) * _cos) + _pivoty + _node->sPos.fY;
		_SpriteDraw(_node, _mat);
	}
	if (!_PrSpriteMem->pCursor)
		return;
	BLF32 _minx = -_PrSpriteMem->pCursor->sSize.fX * 0.5f;
	BLF32 _miny = -_PrSpriteMem->pCursor->sSize.fY * 0.5f;
	BLF32 _maxx = _PrSpriteMem->pCursor->sSize.fX * 0.5f;
	BLF32 _maxy = _PrSpriteMem->pCursor->sSize.fY * 0.5f;
	BLF32 _ltx = _minx + _PrSpriteMem->sCursorPos.fX;
	BLF32 _lty = _minx + _PrSpriteMem->sCursorPos.fY;
	BLF32 _rtx = _maxx + _PrSpriteMem->sCursorPos.fX;
	BLF32 _rty = _maxx + _PrSpriteMem->sCursorPos.fY;
	BLF32 _lbx = _minx + _PrSpriteMem->sCursorPos.fX;
	BLF32 _lby = _minx + _PrSpriteMem->sCursorPos.fY;
	BLF32 _rbx = _maxx + _PrSpriteMem->sCursorPos.fX;
	BLF32 _rby = _maxx + _PrSpriteMem->sCursorPos.fY;
}
BLVoid
_SpriteDestroy()
{
	blUnsubscribeEvent(BL_ET_MOUSE, _MouseSubscriber);
	{
		FOREACH_ARRAY(_BLSpriteNode*, _nodeiter, _PrSpriteMem->pTileArray)
		{
			blDeleteSprite(_nodeiter->nID);
		}
	}
	blDeleteArray(_PrSpriteMem->pTileArray);
    if (_PrSpriteMem->pNodeList)
        free(_PrSpriteMem->pNodeList);
    free(_PrSpriteMem);
}
BLGuid
blGenSprite(IN BLAnsi* _Filename, IN BLAnsi* _Archive, IN BLAnsi* _Tag, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Zv, IN BLU32 _FPS, IN BLBool _AsTile)
{
    _BLSpriteNode* _node = (_BLSpriteNode*)malloc(sizeof(_BLSpriteNode));
    _node->nGBO = INVALID_GUID;
    _node->nTex = INVALID_GUID;
    _node->sSize.fX = _Width;
    _node->sSize.fY = _Height;
    _node->sPos.fX = _node->sPos.fY = 0.f;
    _node->sPivot.fX = _node->sPivot.fY = 0.5f;
    _node->sScissor.sLT.fX = _node->sScissor.sLT.fY = 0.f;
    _node->sScissor.sRB.fX = _node->sScissor.sRB.fY = 0.f;
	_node->sUVScroll.fX = _node->sUVScroll.fY = 0.f;
    _node->pParent = NULL;
    _node->pChildren = NULL;
    _node->pAction = NULL;
	_node->pCurAction = NULL;
	_node->pEmitParam = NULL;
	_node->nFPS = _FPS;
    _node->nChildren = 0;
    _node->fRotate = 0.f;
    _node->fScaleX = 1.f;
    _node->fScaleY = 1.f;
    _node->fAlpha = 1.f;
    _node->fZValue = _Zv;
    _node->nDyeColor  = 0xFFFFFFFF;
    _node->bDye = FALSE;
    _node->nStrokeColor = 0xFFFFFFFF;
    _node->nStrokePixel = 0;
    _node->nGlowColor = 0xFFFFFFFF;
    _node->nGlowPixel = 0;
	_node->bTile = _AsTile;
    _node->bShow = _AsTile ? FALSE : TRUE;
	_node->bValid = FALSE;
    memset(_node->aTag, 0, sizeof(_node->aTag));
	memset(_node->aFilename, 0, sizeof(_node->aFilename));
	memset(_node->aArchive, 0, sizeof(_node->aArchive));
	strcpy(_node->aFilename, _Filename);
	strcpy(_node->aArchive, _Archive);
    if (_Tag)
        strcpy(_node->aTag, _Tag);
    _node->nID = blGenGuid(_node, blHashUtf8((const BLUtf8*)_Filename));
	if (!_node->bTile)
		_FetchResource(_Filename, _Archive, (BLVoid**)&_node, _node->nID, _LoadSprite, _SpriteSetup, TRUE);
	else
		blArrayPushBack(_PrSpriteMem->pTileArray, _node);
    return _node->nID;
}
BLVoid
blDeleteSprite(IN BLGuid _ID)
{
    if (_ID == INVALID_GUID)
        return;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return;
    while (_node->nChildren > 0)
        blDeleteSprite(_node->pChildren[0]->nID);
    _DiscardResource(_ID, _UnloadSprite, _SpriteRelease);
	free(_node);
    blDeleteGuid(_ID);
}
BLBool
blSpriteAttach(IN BLGuid _Parent, IN BLGuid _Child, IN BLBool _AttrInherit)
{
    if (_Parent == INVALID_GUID || _Child == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _parent = (_BLSpriteNode*)blGuidAsPointer(_Parent);
    _BLSpriteNode* _child = (_BLSpriteNode*)blGuidAsPointer(_Child);
    if (!_parent || !_child)
        return FALSE;
    if (_Parent == _Child)
        return FALSE;
	if (_child->bTile || _parent->bTile)
		return FALSE;
    for (BLU32 _idx = 0 ; _idx < _parent->nChildren ; ++_idx)
    {
        if (_parent->pChildren[_idx] == _child)
            return FALSE;
    }
    if (_child->pParent)
    {
        for (BLU32 _idx = 0 ; _idx < _child->pParent->nChildren ; ++_idx)
        {
            if (_child->pParent->pChildren[_idx] == _child)
            {
                memmove(_child->pParent->pChildren + _idx, _child->pParent->pChildren + _idx + 1, (_child->pParent->nChildren - _idx - 1) * sizeof(_BLSpriteNode*));
                _child->pParent->nChildren--;
                _child->pParent = NULL;
                break;
            }
        }
    }
    else
        _RemoveFromNodeList(_child);
    _child->pParent = _parent;
    _child->fZValue = _AttrInherit ? _parent->fZValue : _child->fZValue;
    _parent->pChildren = (_BLSpriteNode**)realloc(_parent->pChildren, (_parent->nChildren + 1) * sizeof(_BLSpriteNode*));
    BLU32 _idx = 0;
    for (; _idx < _parent->nChildren; ++_idx)
    {
        if (_parent->pChildren[_idx]->fZValue >= _child->fZValue)
            break;
    }
    memmove(_parent->pChildren + _idx + 1, _parent->pChildren + _idx, (_parent->nChildren - _idx) * sizeof(_BLSpriteNode*));
    _parent->pChildren[_idx] = _child;
    _parent->nChildren++;
    blSpriteScissor(_Child, _parent->sScissor.sLT.fX, _parent->sScissor.sLT.fY, _parent->sScissor.sRB.fX - _parent->sScissor.sLT.fX, _parent->sScissor.sRB.fY - _parent->sScissor.sLT.fY);
    if (_AttrInherit)
    {
        blSpriteDyeing(_Child, _parent->nDyeColor, _parent->bDye, TRUE);
        blSpriteVisibility(_Child, _parent->bShow, TRUE);
        blSpriteAlpha(_Child, _parent->fAlpha, TRUE);
        blSpriteGlow(_Child, _parent->nGlowColor, _parent->nGlowPixel, TRUE);
        blSpriteStroke(_Child, _parent->nStrokeColor, _parent->nStrokePixel, TRUE);
		blSpritePivot(_Child, _parent->sPivot.fX, _parent->sPivot.fY, TRUE);
        blSpriteZValue(_Child, _parent->fZValue, TRUE);
    }
    return TRUE;
}
BLBool
blSpriteDetach(IN BLGuid _Parent, IN BLGuid _Child)
{
    if (_Parent == INVALID_GUID || _Child == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _parent = (_BLSpriteNode*)blGuidAsPointer(_Parent);
    _BLSpriteNode* _child = (_BLSpriteNode*)blGuidAsPointer(_Child);
    if (!_parent || !_child)
        return FALSE;
    if (!_child->pParent)
        return FALSE;
    if (_child->pParent->nID != _Parent || _Parent == _Child)
        return FALSE;
	if (_child->bTile || _parent->bTile)
		return FALSE;
    BLBool _found = FALSE;
    for (BLU32 _idx = 0 ; _idx < _parent->nChildren ; ++_idx)
    {
        if (_parent->pChildren[_idx] == _child)
        {
            memmove(_parent->pChildren + _idx, _parent->pChildren + _idx + 1, (_parent->nChildren - _idx - 1) * sizeof(_BLSpriteNode*));
            _found = TRUE;
            break;
        }
    }
    if (!_found)
        return FALSE;
    _parent->nChildren--;
    _child->pParent = NULL;
    _AddToNodeList(_child);
    return TRUE;
}
BLBool
blSpriteQuery(IN BLGuid _ID, OUT BLF32* _Width, OUT BLF32* _Height,OUT BLF32* _XPos, OUT BLF32* _YPos, OUT BLF32* _Zv, OUT BLF32* _Rotate, OUT BLF32* _XScale, OUT BLF32* _YScale, OUT BLF32* _Alpha, OUT BLBool* _Show)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    *_Width = _node->sSize.fX;
    *_Height = _node->sSize.fY;
    *_XPos = _node->sPos.fX;
    *_YPos = _node->sPos.fY;
    *_Zv = _node->fZValue;
    *_Rotate = _node->fRotate;
    *_XScale = _node->fScaleX;
    *_YScale = _node->fScaleY;
    *_Alpha = _node->fAlpha;
    *_Show = _node->bShow;
    return TRUE;
}
BLBool
blSpriteVisibility(IN BLGuid _ID, IN BLBool _Show, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->bShow = _Show;
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteVisibility(_node->pChildren[_idx]->nID, _Show, _Passdown);
    }
    return TRUE;
}
BLBool
blSpritePivot(IN BLGuid _ID, IN BLF32 _PivotX, IN BLF32 _PivotY, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->sPivot.fX = blScalarClamp(_PivotX, 0.f, 1.f);
    _node->sPivot.fY = blScalarClamp(_PivotY, 0.f, 1.f);
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
			blSpritePivot(_node->pChildren[_idx]->nID, _PivotX, _PivotY, _Passdown);
    }
    return TRUE;
}
BLBool
blSpriteAlpha(IN BLGuid _ID, IN BLF32 _Alpha, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->fAlpha = _Alpha;
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteAlpha(_node->pChildren[_idx]->nID, _Alpha, _Passdown);
    }
    return TRUE;
}
BLBool
blSpriteZValue(IN BLGuid _ID, IN BLF32 _Zv, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    if (!_node->pParent)
    {
        _RemoveFromNodeList(_node);
        _node->fZValue = _Zv;
        _AddToNodeList(_node);
    }
    else
    {
        BLBool _append = FALSE;
        BLU32 _idx = 0;
        for (; _idx < _node->pParent->nChildren ; ++_idx)
        {
            if (_node->pParent->pChildren[_idx] == _node)
            {
                memmove(_node->pParent->pChildren + _idx, _node->pParent->pChildren + _idx + 1, (_node->pParent->nChildren - _idx - 1) * sizeof(_BLSpriteNode*));
                _node->pParent->nChildren--;
                _append = TRUE;
                break;
            }
        }
        _idx = 0;
        _node->fZValue = _Zv;
        for (; _idx < _node->pParent->nChildren; ++_idx)
        {
            if (_node->pParent->pChildren[_idx]->fZValue >= _node->fZValue)
            {
                memmove(_node->pParent->pChildren + _idx + 1, _node->pParent->pChildren + _idx, (_node->pParent->nChildren - _idx) * sizeof(_BLSpriteNode*));
                _node->pParent->pChildren[_idx] = _node;
                _node->pParent->nChildren++;
                _append = FALSE;
                break;
            }
        }
        if (_append)
        {
            _node->pParent->pChildren[_node->pParent->nChildren] = _node;
            _node->pParent->nChildren++;
        }
    }
    if (_Passdown)
    {
        _BLSpriteNode** _tmp = (_BLSpriteNode**)alloca(_node->nChildren * sizeof(_BLSpriteNode*));
        memcpy(_tmp, _node->pChildren, _node->nChildren * sizeof(_BLSpriteNode*));
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteZValue(_tmp[_idx]->nID, _Zv, _Passdown);
    }
    return TRUE;
}
BLBool 
blSpriteUV(IN BLGuid _ID, IN BLF32 _Uu, IN BLF32 _Vv, IN BLBool _Passdown)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return FALSE;
	_node->sUVScroll.fX = blScalarClamp(_Uu, 0.f, 1.f);
	_node->sUVScroll.fY = blScalarClamp(_Vv, 0.f, 1.f);
	if (_Passdown)
	{
		for (BLU32 _idx = 0; _idx < _node->nChildren; ++_idx)
			blSpriteUV(_node->pChildren[_idx]->nID, _Uu, _Vv, _Passdown);
	}
	return TRUE;
}
BLBool
blSpriteStroke(IN BLGuid _ID, IN BLU32 _Color, IN BLU32 _Pixel, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->nStrokeColor = _Color;
    _node->nStrokePixel = _Pixel;
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteStroke(_node->pChildren[_idx]->nID, _Color, _Pixel, _Passdown);
    }
    return TRUE;    
}
BLBool
blSpriteGlow(IN BLGuid _ID, IN BLU32 _Color, IN BLU32 _Pixel, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->nGlowColor = _Color;
    _node->nGlowPixel = _Pixel;
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteGlow(_node->pChildren[_idx]->nID, _Color, _Pixel, _Passdown);
    }
    return TRUE;
}
BLBool
blSpriteDyeing(IN BLGuid _ID, IN BLU32 _Color, IN BLBool _Dye, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->nDyeColor = _Color;
    _node->bDye = _Dye;
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteDyeing(_node->pChildren[_idx]->nID, _Color, _Dye, _Passdown);
    }
    return TRUE;
}
BLBool
blSpriteTransformReset(IN BLGuid _ID, IN BLF32 _XPos, IN BLF32 _YPos, IN BLF32 _Rotate, IN BLF32 _ScaleX, IN BLF32 _ScaleY)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->sPos.fX = _XPos;
    _node->sPos.fY = _YPos;
    _node->fRotate = _Rotate;
    _node->fScaleX = _ScaleX;
    _node->fScaleY = _ScaleY;
    return TRUE;
}
BLBool
blSpriteMove(IN BLGuid _ID, IN BLF32 _XVec, IN BLF32 _YVec)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->sPos.fX += _XVec;
    _node->sPos.fY += _YVec;
    return TRUE;
}
BLBool
blSpriteScale(IN BLGuid _ID, IN BLF32 _XScale, IN BLF32 _YScale)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->fScaleX *= _XScale;
    _node->fScaleY *= _YScale;
    return TRUE;
}
BLBool
blSpriteRotate(IN BLGuid _ID, IN BLF32 _Rotate)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->fRotate += _Rotate * PI_INTERNAL / 180.0f;
    return TRUE;
}
BLBool
blSpriteScissor(IN BLGuid _ID, IN BLF32 _XPos, IN BLF32 _YPos, IN BLF32 _Width, IN BLF32 _Height)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
    _node->sScissor.sLT.fX = _XPos;
    _node->sScissor.sLT.fY = _YPos;
    _node->sScissor.sRB.fX = _XPos + _Width;
    _node->sScissor.sRB.fY = _YPos + _Height;
    for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
		blSpriteScissor(_node->pChildren[_idx]->nID, _XPos, _YPos, _Width, _Height);
    return TRUE;
}
BLBool
blSpriteAsEmit(IN BLGuid _ID, IN BLEnum _Emitter, IN BLU32 _EmitterParam, IN BLF32 _Life, IN BLU32 _MaxAlive, IN BLU32 _GenPerSec, IN BLF32 _VelocityX, IN BLF32 _VelocityY, IN BLF32 _AccelerationX, IN BLF32 _AccelerationY, IN BLF32 _DisturbanceX, IN BLF32 _DisturbanceY, IN BLF32 _EmitAngle, IN BLU32 _FadeColor, IN BLF32 _FadeScale)
{
    if (_ID == INVALID_GUID)
        return FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return FALSE;
	if (_node->bTile)
		return FALSE;
	if (!_node->pEmitParam)
		_node->pEmitParam = (_BLEmitParam*)malloc(sizeof(_BLEmitParam));
	_node->pEmitParam->eEmitter = _Emitter;
	_node->pEmitParam->nEmitterParam = _EmitterParam;
	_node->pEmitParam->fLife = _Life;
	_node->pEmitParam->nMaxAlive = _MaxAlive;
	_node->pEmitParam->nGenPerSec = _GenPerSec;
	_node->pEmitParam->fVelocityX = _VelocityX;
	_node->pEmitParam->fVelocityY = _VelocityY;
	_node->pEmitParam->fAccelerationX = _AccelerationX;
	_node->pEmitParam->fAccelerationY = _AccelerationY;
	_node->pEmitParam->fDisturbanceX = _DisturbanceX;
	_node->pEmitParam->fDisturbanceY = _DisturbanceY;
	_node->pEmitParam->fEmitAngle = _EmitAngle;
	_node->pEmitParam->nFadeColor = _FadeColor;
	_node->pEmitParam->fFadeScale = _FadeScale;
    return TRUE;
}
BLBool
blSpriteAsCursor(IN BLGuid _ID)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
	if (_node->pParent)
	{
		BLBool _found = FALSE;
		for (BLU32 _idx = 0; _idx < _node->pParent->nChildren; ++_idx)
		{
			if (_node->pParent->pChildren[_idx] == _node)
			{
				memmove(_node->pParent->pChildren + _idx, _node->pParent->pChildren + _idx + 1, (_node->pParent->nChildren - _idx - 1) * sizeof(_BLSpriteNode*));
				_found = TRUE;
				break;
			}
		}
		if (!_found)
			return FALSE;
		_node->pParent->nChildren--;
		_node->pParent = NULL;
	}
	else
		_RemoveFromNodeList(_node);
	_PrSpriteMem->pCursor = _node;
    return TRUE;
}
BLBool
blSpriteActionBegin(IN BLGuid _ID)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
    if (_node->pAction)
    {
        _BLSpriteAction* _tmp = _node->pAction;
		while (_tmp)
		{
			_BLSpriteAction* _tmpnext = _tmp->pNext;
			if (_tmp->pNeighbor)
			{
				_BLSpriteAction* _tmpneb = _tmp->pNeighbor;
				while (_tmp)
				{
					if (_tmp->eActionType == SPACTION_MOVE_INTERNAL)
					{
						free(_tmp->uAction.sMove.pXPath);
						free(_tmp->uAction.sMove.pYPath);
					}
					free(_tmp);
					_tmp = _tmpneb;
					_tmpneb = _tmp ? _tmp->pNeighbor : NULL;
				}
			}
			else
			{
				if (_tmp->eActionType == SPACTION_MOVE_INTERNAL)
				{
					free(_tmp->uAction.sMove.pXPath);
					free(_tmp->uAction.sMove.pYPath);
				}
				free(_tmp);
			}
			_tmp = _tmpnext;
		}
    }
	_PrSpriteMem->bParallelEdit = FALSE;
    return TRUE;
}
BLBool
blSpriteActionEnd(IN BLGuid _ID, IN BLBool _Delete)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
    if (!_node->pAction)
        return FALSE;
	if (_Delete)
	{
		_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
		_act->pNeighbor = NULL;
		_act->pNext = NULL;
		_act->bParallel = FALSE;
		_act->bLoop = FALSE;
		_act->eActionType = SPACTION_DEAD_INTERNAL;
		_act->fCurTime = 0.f;
		_act->fTotalTime = 0.f;
		_act->uAction.sDead.bDead = TRUE;
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		_lastact->pNext = _act;
	}
	_node->pCurAction = NULL;
	_PrSpriteMem->bParallelEdit = FALSE;
    return TRUE;
}
BLBool 
blSpriteActionPlay(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return FALSE;
	if (_node->bTile)
		return FALSE;
	if (!_node->pAction)
		return FALSE;
	_node->pCurAction = _node->pAction;
	return TRUE;
}
BLBool 
blSpriteActionStop(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return FALSE;
	if (_node->bTile)
		return FALSE;
	if (!_node->pAction)
		return FALSE;
	_node->pCurAction = NULL;
	return TRUE;
}
BLBool
blSpriteParallelBegin(IN BLGuid _ID)
{
    if (_ID == INVALID_GUID)
        return FALSE;
	if (_PrSpriteMem->bParallelEdit)
		return FALSE;
	_PrSpriteMem->bParallelEdit = TRUE;
    return TRUE;
}
BLBool
blSpriteParallelEnd(IN BLGuid _ID)
{
    if (_ID == INVALID_GUID)
        return FALSE;
	if (!_PrSpriteMem->bParallelEdit)
		return FALSE;
	_PrSpriteMem->bParallelEdit = FALSE;
    return TRUE;
}
BLBool
blSpriteActionMove(IN BLGuid _ID, IN BLF32* _XPath, IN BLF32* _YPath, IN BLU32 _PathNum, IN BLF32 _Acceleration, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
	if (_PathNum <= 0 || _Time <= 0.f)
		return FALSE;
	_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
	_act->pNeighbor = NULL;
	_act->pNext = NULL;
	_act->bLoop = _Loop;
	_act->eActionType = SPACTION_MOVE_INTERNAL;
	_act->fCurTime = 0.f;
	_act->fTotalTime = _Time;
	_act->bParallel = _PrSpriteMem->bParallelEdit;
	_act->uAction.sMove.fAcceleration = _Acceleration;
	BLF32 _s = sqrtf((_XPath[0] - _node->sPos.fX) * (_XPath[0] - _node->sPos.fX) + (_YPath[0] - _node->sPos.fY) * (_YPath[0] - _node->sPos.fY));
	for (BLU32 _idx = 1; _idx < _PathNum; ++_idx)
		_s += sqrtf((_XPath[_idx] - _XPath[_idx - 1]) * (_XPath[_idx] - _XPath[_idx - 1]) + (_YPath[_idx] - _YPath[_idx - 1]) * (_YPath[_idx] - _YPath[_idx - 1]));
	_act->uAction.sMove.fVelocity = _s / _Time - 0.5f * _Acceleration * _Time;
	_act->uAction.sMove.pXPath = (BLF32*)malloc((_PathNum + 1) * sizeof(BLF32));
	_act->uAction.sMove.pYPath = (BLF32*)malloc((_PathNum + 1) * sizeof(BLF32));
	_act->uAction.sMove.pXPath[0] = _node->sPos.fX;
	_act->uAction.sMove.pYPath[0] = _node->sPos.fY;
	_act->uAction.sMove.nPathNum = _PathNum + 1;
	memcpy(_act->uAction.sMove.pXPath + sizeof(BLF32), _XPath, _PathNum * sizeof(BLF32));
	memcpy(_act->uAction.sMove.pYPath + sizeof(BLF32), _YPath, _PathNum * sizeof(BLF32));
	if (!_node->pAction)
	{
		_node->pAction = _act;
		return TRUE;
	}
	else
	{
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		if (_PrSpriteMem->bParallelEdit && _lastact->bParallel)
		{
			while (_lastact->pNeighbor)
				_lastact = _lastact->pNeighbor;
			_lastact->pNeighbor = _act;
		}
		else
			_lastact->pNext = _act;
	}
    return TRUE;
}
BLBool
blSpriteActionRotate(IN BLGuid _ID, IN BLF32 _Angle, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
	_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
	_act->pNeighbor = NULL;
	_act->pNext = NULL;
	_act->bLoop = _Loop;
	_act->bParallel = _PrSpriteMem->bParallelEdit;
	_act->eActionType = SPACTION_ROTATE_INTERNAL;
	_act->fCurTime = 0.f;
	_act->fTotalTime = _Time;
	_act->uAction.sRotate.fAngle = _Angle;
	if (!_node->pAction)
	{
		_node->pAction = _act;
		return TRUE;
	}
	else
	{
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		if (_PrSpriteMem->bParallelEdit && _lastact->bParallel)
		{
			while (_lastact->pNeighbor)
				_lastact = _lastact->pNeighbor;
			_lastact->pNeighbor = _act;
		}
		else
			_lastact->pNext = _act;
	}
    return TRUE;
}
BLBool
blSpriteActionScale(IN BLGuid _ID, IN BLF32 _XScale, IN BLF32 _YScale, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
	_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
	_act->pNeighbor = NULL;
	_act->pNext = NULL;
	_act->bLoop = _Loop;
	_act->bParallel = _PrSpriteMem->bParallelEdit;
	_act->eActionType = SPACTION_SCALE_INTERNAL;
	_act->fCurTime = 0.f;
	_act->fTotalTime = _Time;
	_act->uAction.sScale.fXInitScale = _node->fScaleX;
	_act->uAction.sScale.fYInitScale = _node->fScaleY;
	_act->uAction.sScale.fXScale = _XScale;
	_act->uAction.sScale.fYScale = _YScale;
	if (!_node->pAction)
	{
		_node->pAction = _act;
		return TRUE;
	}
	else
	{
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		if (_PrSpriteMem->bParallelEdit && _lastact->bParallel)
		{
			while (_lastact->pNeighbor)
				_lastact = _lastact->pNeighbor;
			_lastact->pNeighbor = _act;
		}
		else
			_lastact->pNext = _act;
	}
    return TRUE;
}
BLBool
blSpriteActionAlpha(IN BLGuid _ID, IN BLF32 _Alpha, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (_node->bTile)
		return FALSE;
	_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
	_act->pNeighbor = NULL;
	_act->pNext = NULL;
	_act->bLoop = _Loop;
	_act->bParallel = _PrSpriteMem->bParallelEdit;
	_act->eActionType = SPACTION_ALPHA_INTERNAL;
	_act->fCurTime = 0.f;
	_act->fTotalTime = _Time;
	_act->uAction.sAlpha.fInitAlpha = _node->fAlpha;
	_act->uAction.sAlpha.fAlpha = _Alpha;
	if (!_node->pAction)
	{
		_node->pAction = _act;
		return TRUE;
	}
	else
	{
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		if (_PrSpriteMem->bParallelEdit && _lastact->bParallel)
		{
			while (_lastact->pNeighbor)
				_lastact = _lastact->pNeighbor;
			_lastact->pNeighbor = _act;
		}
		else
			_lastact->pNext = _act;
	}
    return TRUE;
}
BLVoid
blViewportMove(IN BLF32 _XVec, IN BLF32 _YVec)
{
	_PrSpriteMem->sViewport.sLT.fX += _XVec;
	_PrSpriteMem->sViewport.sLT.fY += _YVec;
	_PrSpriteMem->sViewport.sRB.fX += _XVec;
	_PrSpriteMem->sViewport.sRB.fY += _YVec;
}
BLVoid 
blViewportShake(IN BLF32 _Time, IN BLBool _Vertical, IN BLF32 _Force)
{
}
