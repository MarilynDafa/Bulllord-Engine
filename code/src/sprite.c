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
#include "../headers/streamio.h"
#include "internal/dictionary.h"
#include "internal/array.h"
#include "internal/internal.h"
#include "internal/mathematics.h"
#include "../externals/duktape/duktape.h"
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
            BLBool bClockWise;
		}sRotate;
		struct _Scale {
			BLF32 fXInitScale;
			BLF32 fYInitScale;
			BLF32 fXScale;
			BLF32 fYScale;
            BLBool bReverse;
		}sScale;
		struct _Alpha {
			BLF32 fInitAlpha;
			BLF32 fAlpha;
			BLBool bReverse;
		}sAlpha;
		struct _Dead {
			struct _SpriteAction* pBegin;
			BLF32 fAlphaCache;
			BLF32 fScaleXCache;
			BLF32 fScaleYCache;
			BLF32 fOffsetXCache;
			BLF32 fOffsetYCache;
			BLF32 fRotateCache;
			BLBool bDead;
		}sDead;
	} uAction;
}_BLSpriteAction;
typedef struct _SpriteRecorder {
	BLBool bParallelEdit;
	BLF32 fAlphaRecord;
	BLF32 fScaleXRecord;
	BLF32 fScaleYRecord;
	BLF32 fPosXRecord;
	BLF32 fPosYRecord;
} _BLSpriteRecorder;
typedef struct _EmitParam {
	BLF32 fEmitterRadius;
	BLU32 nLife;
	BLU32 nMaxAlive;
    BLU32 nCurAlive;
	BLF32 fGenPerMSec;
	BLF32 fDirectionX;
	BLF32 fDirectionY;
    BLF32 fVelocity;
    BLF32 fVelVariance;
	BLF32 fEmitAngle;
    BLF32 fRotation;
    BLF32 fRotVariance;
    BLF32 fScale;
    BLF32 fScaleVariance;
    BLU32 nFadeColor;
    BLU32* pAge;
    BLF32* pPositionX;
    BLF32* pPositionY;
    BLF32* pEmitAngle;
    BLF32* pVelocity;
    BLU32* pRotScale;
}_BLEmitParam;
typedef struct _SpriteSheet{
	BLS32 nTag;
	BLS32 nLTx;
	BLS32 nLTy;
	BLS32 nRBx;
	BLS32 nRBy;
	BLS32 nOffsetX;
	BLS32 nOffsetY;
}_BLSpriteSheet;
typedef struct _TileInfo{
	BLGuid nID;
	BLAnsi aFilename[260];
	BLGuid nTex;
	BLVec2 sSize;
	BLVec2 sPos;
	BLF32 fTexLTx;
	BLF32 fTexLTy;
	BLF32 fTexRBx;
	BLF32 fTexRBy;
	BLF32 fAlpha;
	BLBool bShow;
}_BLTileInfo;
typedef struct _ExternalMethod {
	BLAnsi aSuffix[16];
	const BLBool(*pLoadCB)(BLGuid, const BLAnsi*, BLVoid**);
	const BLBool(*pSetupCB)(BLGuid, BLVoid**);
	const BLBool(*pUnloadCB)(BLGuid, BLVoid**);
	const BLBool(*pReleaseCB)(BLGuid, BLVoid**);
	const BLVoid(*pDrawCB)(BLU32, BLGuid, BLF32[6], BLF32, BLF32, BLVoid**);
} _BLExternalMethod;
typedef struct _SpriteNode{
	duk_context* pDukContext;
    struct _SpriteNode* pParent;
    struct _SpriteNode** pChildren;
	_BLSpriteAction* pAction;
	_BLSpriteAction* pCurAction;
	_BLExternalMethod* pExternal;
	BLVoid* pExternalData;
	_BLEmitParam* pEmitParam;
	BLDictionary* pTagSheet;
	BLU8* pTexData;
	BLEnum eTexFormat;
	BLU32 nTexWidth;
	BLU32 nTexHeight;
    BLGuid nID;
    BLU32 aTag[64];
	BLU32 nFrameNum;
	BLU32 nCurFrame;
	BLU32 nTimePassed;
	BLAnsi aFilename[260];
    BLGuid nGBO;
    BLGuid nTex;
    BLVec2 sSize;
    BLVec2 sPos;
    BLVec2 sPivot;
	BLVec2 sAbsLT;
	BLVec2 sAbsRT;
	BLVec2 sAbsLB;
	BLVec2 sAbsRB;
    BLRect sScissor;
    BLU32 nFPS;
    BLF32 fScaleX;
    BLF32 fScaleY;
    BLF32 fRotate;
    BLF32 fZValue;
    BLF32 fAlpha;
    BLU32 nDyeColor;
    BLBool bDye;
	BLBool bFlipX;
	BLBool bFlipY;
    BLU32 nStrokeColor;
    BLU32 nStrokePixel;
    BLU32 nGlowColor;
    BLU32 nBrightness;
    BLU32 nChildren;
	BLBool bValid;
    BLBool bShow;
}_BLSpriteNode;
typedef struct _SpriteMember {
	duk_context* pDukContext;
    BLGuid nSpriteTech;
    BLGuid nSpriteInstTech;
	BLGuid nSpriteStrokeTech;
	BLGuid nSpriteGlowTech;
	BLGuid nFBO;
	BLGuid nFBOTex;
	BLGuid nQuadGeo;
	BLU32 nFboWidth;
	BLU32 nFboHeight;
    _BLSpriteNode** pNodeList;
	_BLSpriteNode* pCursor;
	_BLSpriteRecorder sActionRecorder;
	BLArray* pTileArray[8];
	BLRect sViewport;
    BLU32 nNodeNum;
    BLU32 nNodeCap;
    BLBool bShaking;
    BLF32 fShakingTime;
    BLBool bShakingVertical;
    BLF32 fShakingForce;
	_BLExternalMethod aExternalMethod[8];
}_BLSpriteMember;
static _BLSpriteMember* _PrSpriteMem = NULL;
extern BLBool _FetchResource(const BLAnsi*, BLVoid**, BLGuid, BLBool(*)(BLVoid*, const BLAnsi*), BLBool(*)(BLVoid*), BLBool);
extern BLBool _DiscardResource(BLGuid, BLBool(*)(BLVoid*), BLBool(*)(BLVoid*));
BLBool
_UseCustomCursor()
{
	if (_PrSpriteMem)
		return _PrSpriteMem->pCursor ? TRUE : FALSE;
	else
		return FALSE;
}
static const BLBool
_MouseSubscriber(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	if (_Type == BL_ME_MOVE)
	{
        if (_PrSpriteMem->pCursor)
        {
            _PrSpriteMem->pCursor->sPos.fX = LOWU16(_UParam);
            _PrSpriteMem->pCursor->sPos.fY = HIGU16(_UParam);
        }
	}
	else if (_Type == BL_ME_LDOWN || _Type == BL_ME_RDOWN)
	{
		BLU32 _oriw, _orih, _aw, _ah;
		BLF32 _rx, _ry;
		blWindowQuery(&_oriw, &_orih, &_aw, &_ah, &_rx, &_ry);
		BLF32 _x = (BLF32)LOWU16(_UParam) / _rx;
		BLF32 _y = (BLF32)HIGU16(_UParam) / _ry;
		BLGuid _id = INVALID_GUID;
		for (BLU32 _idx = 0; _idx < _PrSpriteMem->nNodeNum; ++_idx)
		{
			_BLSpriteNode* _node = _PrSpriteMem->pNodeList[_idx];
			BLS32 _a = (BLS32)((_node->sAbsLT.fX - _node->sAbsLB.fX) * (_y - _node->sAbsLB.fY) - (_node->sAbsLT.fY - _node->sAbsLB.fY) * (_x - _node->sAbsLB.fX));
			BLS32 _b = (BLS32)((_node->sAbsRT.fX - _node->sAbsLT.fX) * (_y - _node->sAbsLT.fY) - (_node->sAbsRT.fY - _node->sAbsLT.fY) * (_x - _node->sAbsLT.fX));
			BLS32 _c = (BLS32)((_node->sAbsRB.fX - _node->sAbsRT.fX) * (_y - _node->sAbsRT.fY) - (_node->sAbsRB.fY - _node->sAbsRT.fY) * (_x - _node->sAbsRT.fX));
			BLS32 _d = (BLS32)((_node->sAbsLB.fX - _node->sAbsRB.fX) * (_y - _node->sAbsRB.fY) - (_node->sAbsLB.fY - _node->sAbsRB.fY) * (_x - _node->sAbsRB.fX));
			if ((_a > 0 && _b > 0 && _c > 0 && _d > 0) || (_a < 0 && _b < 0 && _c < 0 && _d < 0)) 
			{
				_id = _node->nID;
				break;
			}
		}
		blInvokeEvent(BL_ET_SPRITE, MAKEU32(_y + _PrSpriteMem->sViewport.sLT.fY, _x + _PrSpriteMem->sViewport.sLT.fX), _Type, NULL, _id);
	}
	return TRUE;
}
static const BLBool
_SystemSubscriber(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	if (_UParam == BL_SE_RESOLUTION)
	{
		blFrameBufferDetach(_PrSpriteMem->nFBO, FALSE);
		blDeleteTexture(_PrSpriteMem->nFBOTex);
		BLU32 _width, _height;
		BLF32 _rx, _ry;
		blWindowQuery(&_width, &_height, &_PrSpriteMem->nFboWidth, &_PrSpriteMem->nFboHeight, &_rx, &_ry);
		BLF32 _centerx = (_PrSpriteMem->sViewport.sLT.fX + _PrSpriteMem->sViewport.sRB.fX) * 0.5f;
		BLF32 _centery = (_PrSpriteMem->sViewport.sLT.fY + _PrSpriteMem->sViewport.sRB.fY) * 0.5f;
		_PrSpriteMem->sViewport.sLT.fX = _centerx - _PrSpriteMem->nFboWidth * 0.5f;
		_PrSpriteMem->sViewport.sLT.fY = _centery - _PrSpriteMem->nFboHeight * 0.5f;
		_PrSpriteMem->sViewport.sRB.fX = _centerx + _PrSpriteMem->nFboWidth * 0.5f;
		_PrSpriteMem->sViewport.sRB.fY = _centery + _PrSpriteMem->nFboHeight * 0.5f;
		_PrSpriteMem->nFBOTex = blGenTexture(0xFFFFFFFF, BL_TT_2D, BL_TF_RGBA8, FALSE, FALSE, TRUE, 1, 1, _PrSpriteMem->nFboWidth, _PrSpriteMem->nFboHeight, 1, NULL);
		blFrameBufferAttach(_PrSpriteMem->nFBO, _PrSpriteMem->nFBOTex, 0, BL_CTF_IGNORE);
	}
	return FALSE;
}
static BLVoid
_AddToNodeList(_BLSpriteNode* _Node)
{
    BLS32 _mid = -1, _left = 0, _right = _PrSpriteMem->nNodeNum;
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
static BLVoid
_RemoveFromNodeList(_BLSpriteNode* _Node)
{
    BLBool _found = FALSE;
    BLS32 _mid = -1, _left = 0, _right = _PrSpriteMem->nNodeNum;
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
    if (_node != _PrSpriteMem->pCursor && !_node->pParent)
        _AddToNodeList(_node);
	if (_node->pExternal)
	{
		_node->bValid = _node->pExternal->pSetupCB(_node->nID, &_node->pExternalData);
		return _node->bValid;
	}
	else
	{
		BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
		BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
		BLF32 _rgba[4];
		blDeColor4F(_node->nDyeColor, _rgba);
		BLF32 _vbo[] = {
			-_node->sSize.fX * 0.5f,
			-_node->sSize.fY * 0.5f,
			_rgba[0],
			_rgba[1],
			_rgba[2],
			_node->fAlpha,
			0.f,
			0.f,
			_node->sSize.fX * 0.5f,
			-_node->sSize.fY * 0.5f,
			_rgba[0],
			_rgba[1],
			_rgba[2],
			_node->fAlpha,
			1.f,
			0.f,
			-_node->sSize.fX * 0.5f,
			_node->sSize.fY * 0.5f,
			_rgba[0],
			_rgba[1],
			_rgba[2],
			_node->fAlpha,
			0.f,
			1.f,
			_node->sSize.fX * 0.5f,
			_node->sSize.fY * 0.5f,
			_rgba[0],
			_rgba[1],
			_rgba[2],
			_node->fAlpha,
			1.f,
			1.f
		};
		_node->nGBO = blGenGeometryBuffer(URIPART_INTERNAL(_node->nID), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
		if (_node->pEmitParam)
		{
			BLEnum _semantice[] = { BL_SL_COLOR1, BL_SL_INSTANCE1 };
			BLEnum _decle[] = { BL_VD_FLOATX4, BL_VD_FLOATX4 };
			blGeometryBufferInstance(_node->nGBO, _semantice, _decle, 2, _node->pEmitParam->nMaxAlive);
		}
		_node->nTex = blGenTexture(blHashUtf8(_node->aFilename), BL_TT_2D, _node->eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->nTexWidth, _node->nTexHeight, 1, _node->pTexData);
		free(_node->pTexData);
		_node->pTexData = NULL;
		_node->bValid = TRUE;
		return TRUE;
	}   
}
static BLBool
_SpriteRelease(BLVoid* _Src)
{
    _BLSpriteNode* _node = (_BLSpriteNode*)_Src;
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
	if (_node->pExternal)
		_node->pExternal->pReleaseCB(_node->nID, &_node->pExternalData);
	else
	{
		blDeleteGeometryBuffer(_node->nGBO);
		blDeleteTexture(_node->nTex);
	}
    return TRUE;
}
static BLBool
_LoadSprite(BLVoid* _Src, const BLAnsi* _Filename)
{
	_BLSpriteNode* _node = (_BLSpriteNode*)_Src;
	if (_node->pExternal)
		return _node->pExternal->pLoadCB(_node->nID, _Filename, &_node->pExternalData);
	else
	{
		BLGuid _stream = blGenStream(_Filename);
		BLU8 _identifier[12];
		blStreamRead(_stream, sizeof(_identifier), _identifier);
		if (_identifier[0] != 0xDD ||
			_identifier[1] != 0xDD ||
			_identifier[2] != 0xDD ||
			_identifier[3] != 0xEE ||
			_identifier[4] != 0xEE ||
			_identifier[5] != 0xEE ||
			_identifier[6] != 0xAA ||
			_identifier[7] != 0xAA ||
			_identifier[8] != 0xAA ||
			_identifier[9] != 0xDD ||
			_identifier[10] != 0xDD ||
			_identifier[11] != 0xDD)
		{
			blDeleteStream(_stream);
			return FALSE;
		}
		BLU32 _width, _height, _depth;
		blStreamRead(_stream, sizeof(BLU32), &_width);
		blStreamRead(_stream, sizeof(BLU32), &_height);
		blStreamRead(_stream, sizeof(BLU32), &_depth);
		BLU32 _array, _faces, _mipmap;
		blStreamRead(_stream, sizeof(BLU32), &_array);
		blStreamRead(_stream, sizeof(BLU32), &_faces);
		blStreamRead(_stream, sizeof(BLU32), &_mipmap);
		BLU32 _fourcc, _channels, _offset;
		blStreamRead(_stream, sizeof(BLU32), &_fourcc);
		blStreamRead(_stream, sizeof(BLU32), &_channels);
		blStreamRead(_stream, sizeof(BLU32), &_offset);
		_node->nTexWidth = _width;
		_node->nTexHeight = _height;
		BLEnum _type;
		if (_height == 1)
			_type = BL_TT_1D;
		else
		{
			if (_depth == 1)
				_type = (_faces != 6) ? BL_TT_2D : BL_TT_CUBE;
			else
				_type = BL_TT_3D;
		}
		if (_type != BL_TT_2D)
		{
			blDeleteStream(_stream);
			return FALSE;
		}
		_node->pTagSheet = blGenDict(FALSE);
		while (blStreamTell(_stream) < _offset)
		{
			_BLSpriteSheet* _ss = (_BLSpriteSheet*)malloc(sizeof(_BLSpriteSheet));
			BLU32 _taglen;
			blStreamRead(_stream, sizeof(BLU32), &_taglen);
			BLAnsi _tag[256] = { 0 };
			blStreamRead(_stream, _taglen, _tag);
			_ss->nTag = blHashUtf8(_tag);
			blStreamRead(_stream, sizeof(BLU32), &_ss->nLTx);
			blStreamRead(_stream, sizeof(BLU32), &_ss->nLTy);
			blStreamRead(_stream, sizeof(BLU32), &_ss->nRBx);
			blStreamRead(_stream, sizeof(BLU32), &_ss->nRBy);
			blStreamRead(_stream, sizeof(BLU32), &_ss->nOffsetX);
			blStreamRead(_stream, sizeof(BLU32), &_ss->nOffsetY);
			blDictInsert(_node->pTagSheet, _ss->nTag, _ss);
			if (_node->nFrameNum == 1)
				_node->aTag[0] = _ss->nTag;
		}
		blStreamSeek(_stream, _offset);
		BLU32 _imagesz;
		switch (_fourcc)
		{
		case FOURCC_INTERNAL('B', 'M', 'G', 'T'):
			blStreamRead(_stream, sizeof(BLU32), &_imagesz);
			_node->eTexFormat = (_channels == 4) ? BL_TF_RGBA8 : BL_TF_RGB8;
			break;
		case FOURCC_INTERNAL('S', '3', 'T', '1'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
			_node->eTexFormat = BL_TF_BC1;
			break;
		case FOURCC_INTERNAL('S', '3', 'T', '2'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
			_node->eTexFormat = BL_TF_BC1A1;
			break;
		case FOURCC_INTERNAL('S', '3', 'T', '3'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
			_node->eTexFormat = BL_TF_BC3;
			break;
		case FOURCC_INTERNAL('A', 'S', 'T', '1'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
			_node->eTexFormat = BL_TF_ASTC;
			break;
		case FOURCC_INTERNAL('A', 'S', 'T', '2'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
			_node->eTexFormat = BL_TF_ASTC;
			break;
		case FOURCC_INTERNAL('A', 'S', 'T', '3'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
			_node->eTexFormat = BL_TF_ASTC;
			break;
		case FOURCC_INTERNAL('E', 'T', 'C', '1'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
			_node->eTexFormat = BL_TF_ETC2;
			break;
		case FOURCC_INTERNAL('E', 'T', 'C', '2'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
			_node->eTexFormat = BL_TF_ETC2A1;
			break;
		case FOURCC_INTERNAL('E', 'T', 'C', '3'):
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
			_node->eTexFormat = BL_TF_ETC2A;
			break;
		default:assert(0); break;
		}
		if (_fourcc == FOURCC_INTERNAL('B', 'M', 'G', 'T'))
		{
			BLU8* _data = (BLU8*)malloc(_imagesz);
			blStreamRead(_stream, _imagesz, _data);
			BLU8* _data2 = (BLU8*)malloc(_width * _height * _channels);
			blRLEDecode(_data, _width * _height * _channels, _data2);
			free(_data);
			_node->pTexData = _data2;
		}
		else
		{
			BLU8* _data = (BLU8*)malloc(_imagesz);
			blStreamRead(_stream, _imagesz, _data);
			_node->pTexData = _data;
		}
		blDeleteStream(_stream);
		return TRUE;
	}	
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
	if (_node->pExternal)
		_node->pExternal->pUnloadCB(_node->nID, &_node->pExternalData);
	else
	{
		if (_node->pEmitParam)
		{
			free(_node->pEmitParam->pPositionX);
			free(_node->pEmitParam->pPositionY);
			free(_node->pEmitParam->pAge);
			free(_node->pEmitParam);
		}
		{
			FOREACH_DICT(_BLSpriteSheet*, _iter, _node->pTagSheet)
			{
				free(_iter);
			}
			blDeleteDict(_node->pTagSheet);
		}
	}
	_node->bValid = FALSE;
    return TRUE;
}
static BLVoid
_SpriteUpdate(BLU32 _Delta)
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
				_action->fCurTime += (BLF32)(_Delta) / 1000.f;
				switch (_action->eActionType)
				{
				case SPACTION_MOVE_INTERNAL:
				{
					BLF32 _s = _action->uAction.sMove.fVelocity * _action->fCurTime + 0.5f * _action->uAction.sMove.fAcceleration * _action->fCurTime * _action->fCurTime;
					BLU32 _jdx = 1;
					BLF32* _xp = _action->uAction.sMove.pXPath;
					BLF32* _yp = _action->uAction.sMove.pYPath;
					BLF32 _subs = 0.f;
					while (_s > 0.f && _jdx < _action->uAction.sMove.nPathNum)
					{
						_subs = sqrtf((_xp[_jdx] - _xp[_jdx - 1]) * (_xp[_jdx] - _xp[_jdx - 1]) + (_yp[_jdx] - _yp[_jdx - 1]) * (_yp[_jdx] - _yp[_jdx - 1]));
						_s -= _subs;
						++_jdx;
					}	
					BLF32 _ratio = (_s + _subs) / _subs;
					_node->sPos.fX = _xp[_jdx - 2] + _ratio * (_xp[_jdx - 1] - _xp[_jdx - 2]);
					_node->sPos.fY = _yp[_jdx - 2] + _ratio * (_yp[_jdx - 1] - _yp[_jdx - 2]);
				}
				break;
				case SPACTION_ROTATE_INTERNAL:
				{
					_node->fRotate = _action->fCurTime / _action->fTotalTime * _action->uAction.sRotate.fAngle * PI_INTERNAL / 180.0f;
                    _node->fRotate = _action->uAction.sRotate.bClockWise ? _node->fRotate : 2 * PI_INTERNAL - _node->fRotate;
				}
				break;
				case SPACTION_SCALE_INTERNAL:
				{
                    if (_action->uAction.sScale.bReverse)
                    {
                        if (_action->fCurTime * 2 <= _action->fTotalTime)
                        {
                            _node->fScaleX = 2 * _action->fCurTime / _action->fTotalTime * (_action->uAction.sScale.fXScale - _action->uAction.sScale.fXInitScale) + _action->uAction.sScale.fXInitScale;
                            _node->fScaleY = 2 * _action->fCurTime / _action->fTotalTime * (_action->uAction.sScale.fYScale - _action->uAction.sScale.fYInitScale) + _action->uAction.sScale.fYInitScale;
                        }
                        else
                        {
                            _node->fScaleX = (2 * _action->fCurTime - _action->fTotalTime) / _action->fTotalTime * (_action->uAction.sScale.fXInitScale - _action->uAction.sScale.fXScale) + _action->uAction.sScale.fXScale;
							_node->fScaleY = (2 * _action->fCurTime - _action->fTotalTime) / _action->fTotalTime * (_action->uAction.sScale.fYInitScale - _action->uAction.sScale.fYScale) + _action->uAction.sScale.fYScale;
                        }
                    }
                    else
                    {
                        _node->fScaleX = _action->fCurTime / _action->fTotalTime * (_action->uAction.sScale.fXScale - _action->uAction.sScale.fXInitScale) + _action->uAction.sScale.fXInitScale;
                        _node->fScaleY = _action->fCurTime / _action->fTotalTime * (_action->uAction.sScale.fYScale - _action->uAction.sScale.fYInitScale) + _action->uAction.sScale.fYInitScale;
                    }
                }
				break;
				case SPACTION_ALPHA_INTERNAL:
				{
					if (_action->uAction.sAlpha.bReverse)
					{
						if (_action->fCurTime * 2 <= _action->fTotalTime)
							_node->fAlpha = 2 * _action->fCurTime / _action->fTotalTime * (_action->uAction.sAlpha.fAlpha - _action->uAction.sAlpha.fInitAlpha) + _action->uAction.sAlpha.fInitAlpha;
						else
							_node->fAlpha = (2 * _action->fCurTime - _action->fTotalTime) / _action->fTotalTime * (_action->uAction.sAlpha.fInitAlpha - _action->uAction.sAlpha.fAlpha) + _action->uAction.sAlpha.fAlpha;
					}
					else
						_node->fAlpha = _action->fCurTime / _action->fTotalTime * (_action->uAction.sAlpha.fAlpha - _action->uAction.sAlpha.fInitAlpha) + _action->uAction.sAlpha.fInitAlpha;
					_node->fAlpha = blScalarClamp(_node->fAlpha, 0.f, 1.f);
				}
				break;
				case SPACTION_DEAD_INTERNAL:
				{
					if (_action->uAction.sDead.pBegin)
					{
						_node->pCurAction = _action->uAction.sDead.pBegin;
						_node->fAlpha = _action->uAction.sDead.fAlphaCache;
						_node->fScaleX = _action->uAction.sDead.fScaleXCache;
						_node->fScaleY = _action->uAction.sDead.fScaleYCache;
						_node->sPos.fX = _action->uAction.sDead.fOffsetXCache;
						_node->sPos.fY = _action->uAction.sDead.fOffsetYCache;
						_node->fRotate = _action->uAction.sDead.fRotateCache;
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
										_tmp->fCurTime = 0.f;
										_tmp = _tmpneb;
										_tmpneb = _tmp ? _tmp->pNeighbor : NULL;
									}
								}
								else
									_tmp->fCurTime = 0.f;
								_tmp = _tmpnext;
							}
						}
						_node->pCurAction->fCurTime = 0.f;
					}
					else
					{
						blInvokeEvent(BL_ET_SPRITE, 0xFFFFFFFF, 0, NULL, _node->nID);
						blDeleteSprite(_node->nID);
						_delete = TRUE;
					}
				}
				break;
				default:
					break;
				}
                if (_delete)
                    break;
				_action = _action->pNeighbor;
			} while (_action);
			if (!_delete && _node->pCurAction->fCurTime >= _node->pCurAction->fTotalTime)
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
						blInvokeEvent(BL_ET_SPRITE, 0xFFFFFFFF, 0, NULL, _node->nID);
					}
					else
						_node->pCurAction = _node->pCurAction->pNext;
				}
				else
					_node->pCurAction->fCurTime = 0.f;
			}
		}
		if (!_delete)
			_idx++;
	}
}
static BLVoid
_SpriteDraw(BLU32 _Delta, _BLSpriteNode* _Node, BLF32 _Mat[6])
{
    if (!_Node->bValid)
        return;
	if (_Node->sScissor.sLT.fX < 0.f)
		blRasterState(BL_CM_CW, 0, 0.f, TRUE, 0, 0, _PrSpriteMem->nFboWidth, _PrSpriteMem->nFboHeight, FALSE);
	else
		blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLS32)_Node->sScissor.sLT.fX, (BLS32)_Node->sScissor.sLT.fY, (BLU32)(_Node->sScissor.sRB.fX - _Node->sScissor.sLT.fX), (BLU32)(_Node->sScissor.sRB.fY - _Node->sScissor.sLT.fY), FALSE);
	if (_Node->nFrameNum > 1)
	{
		_Node->nTimePassed += _Delta;
		if (_Node->nTimePassed >= 1000 / _Node->nFPS)
		{
			_Node->nTimePassed = 0;
			_Node->nCurFrame++;
			if (_Node->nCurFrame >= _Node->nFrameNum)
				_Node->nCurFrame = 0;
		}
	}
	if (INVALID_GUID != _Node->nTex && _Node->bShow && !_Node->pExternal)
	{
		_BLSpriteSheet* _ss = blDictElement(_Node->pTagSheet, _Node->aTag[_Node->nCurFrame]);
		BLF32 _minx, _miny, _maxx, _maxy;
		if (!_ss)
		{
			_minx = -_Node->sSize.fX * 0.5f;
			_miny = -_Node->sSize.fY * 0.5f;
			_maxx = _Node->sSize.fX * 0.5f;
			_maxy = _Node->sSize.fY * 0.5f;
		}
		else
		{
			BLF32 _dif = (BLF32)MAX_INTERNAL(_Node->nStrokePixel, 3);
			_maxx = _Node->sSize.fX * 0.5f + _dif - _ss->nOffsetX;
			_minx = _maxx - _ss->nRBx + _ss->nLTx - _dif;
			_maxy = _Node->sSize.fY * 0.5f + _dif - _ss->nOffsetY;
			_miny = _maxy - _ss->nRBy + _ss->nLTy - _dif;
		}
		BLF32 _ltx = (_minx * _Mat[0]) + (_miny * _Mat[2]) + _Mat[4];
		BLF32 _lty = (_minx * _Mat[1]) + (_miny * _Mat[3]) + _Mat[5];
		BLF32 _rtx = (_maxx * _Mat[0]) + (_miny * _Mat[2]) + _Mat[4];
		BLF32 _rty = (_maxx * _Mat[1]) + (_miny * _Mat[3]) + _Mat[5];
		BLF32 _lbx = (_minx * _Mat[0]) + (_maxy * _Mat[2]) + _Mat[4];
		BLF32 _lby = (_minx * _Mat[1]) + (_maxy * _Mat[3]) + _Mat[5];
		BLF32 _rbx = (_maxx * _Mat[0]) + (_maxy * _Mat[2]) + _Mat[4];
		BLF32 _rby = (_maxx * _Mat[1]) + (_maxy * _Mat[3]) + _Mat[5];
		_Node->sAbsLT.fX = _ltx;
		_Node->sAbsLT.fY = _lty;
		_Node->sAbsRT.fX = _rtx;
		_Node->sAbsRT.fY = _rty;
		_Node->sAbsLB.fX = _lbx;
		_Node->sAbsLB.fY = _lby;
		_Node->sAbsRB.fX = _rbx;
		_Node->sAbsRB.fY = _rby;
		if (_Node->pEmitParam)
		{
			BLU32 _gen = (BLU32)(_Node->pEmitParam->fGenPerMSec * _Delta);
			_Node->pEmitParam->nCurAlive = MIN_INTERNAL(_Node->pEmitParam->nMaxAlive, _gen + _Node->pEmitParam->nCurAlive);
			BLF32* _clrbuf = (BLF32*)alloca(_Node->pEmitParam->nMaxAlive * 4 * sizeof(BLF32));
			BLF32* _tranbuf = (BLF32*)alloca(_Node->pEmitParam->nMaxAlive * 4 * sizeof(BLF32));
			for (BLU32 _idx = 0; _idx < _Node->pEmitParam->nCurAlive; ++_idx)
			{
				if (_Node->pEmitParam->pAge[_idx] == 0)
				{
					_Node->pEmitParam->pPositionX[_idx] = _Node->sPos.fX + cosf(blRandRangeF(0.f, PI_INTERNAL * 2.f)) * _Node->pEmitParam->fEmitterRadius * blRandF();
					_Node->pEmitParam->pPositionY[_idx] = _Node->sPos.fY + sinf(blRandRangeF(0.f, PI_INTERNAL * 2.f)) * _Node->pEmitParam->fEmitterRadius * blRandF();
					_Node->pEmitParam->pEmitAngle[_idx] = blRandRangeF(-_Node->pEmitParam->fEmitAngle, _Node->pEmitParam->fEmitAngle);
					_Node->pEmitParam->pVelocity[_idx] = _Node->pEmitParam->fVelocity + blRandRangeF(-_Node->pEmitParam->fVelVariance, _Node->pEmitParam->fVelVariance);
					BLS32 _initrot = 100 * (BLS32)(_Node->pEmitParam->fRotation + blRandRangeF(-_Node->pEmitParam->fRotVariance, _Node->pEmitParam->fRotVariance));
					BLS32 _initscale = 100 * (BLS32)(_Node->pEmitParam->fScale + blRandRangeF(-_Node->pEmitParam->fScaleVariance, _Node->pEmitParam->fScaleVariance));
					_Node->pEmitParam->pRotScale[_idx] = MAKEU32(MAX_INTERNAL(0, _initrot), MAX_INTERNAL(0, _initscale));
				}
				else
				{
					BLF32 _s = _Node->pEmitParam->pVelocity[_idx] * _Delta / 1000.f;
					BLF32 _xvec, _yvec;
					_xvec = _Node->pEmitParam->fDirectionX * cosf(_Node->pEmitParam->pEmitAngle[_idx]) - _Node->pEmitParam->fDirectionY * sinf(_Node->pEmitParam->pEmitAngle[_idx]);
					_yvec = _Node->pEmitParam->fDirectionX * sinf(_Node->pEmitParam->pEmitAngle[_idx]) + _Node->pEmitParam->fDirectionY * cosf(_Node->pEmitParam->pEmitAngle[_idx]);
					_Node->pEmitParam->pPositionX[_idx] += _xvec * _s;
					_Node->pEmitParam->pPositionY[_idx] += _yvec * _s;
				}
				BLF32 _fadeclr[4], _dyeclr[4];
				blDeColor4F(_Node->pEmitParam->nFadeColor, _fadeclr);
				blDeColor4F(_Node->nDyeColor, _dyeclr);
				BLF32 _rot = (BLF32)HIGU16(_Node->pEmitParam->pRotScale[_idx]) / 100.f;
				BLF32 _scale = (BLF32)LOWU16(_Node->pEmitParam->pRotScale[_idx]) / 100.f;
				BLF32 _ratio = (BLF32)_Node->pEmitParam->pAge[_idx] / (BLF32)_Node->pEmitParam->nLife;
				_clrbuf[4 * _idx + 0] = (_fadeclr[0] - _dyeclr[0]) * _ratio + _dyeclr[0];
				_clrbuf[4 * _idx + 1] = (_fadeclr[1] - _dyeclr[1]) * _ratio + _dyeclr[1];
				_clrbuf[4 * _idx + 2] = (_fadeclr[2] - _dyeclr[2]) * _ratio + _dyeclr[2];
				_clrbuf[4 * _idx + 3] = (_fadeclr[3] - _dyeclr[3]) * _ratio + _dyeclr[3];
				_tranbuf[4 * _idx + 0] = _Node->pEmitParam->pPositionX[_idx];
				_tranbuf[4 * _idx + 1] = _Node->pEmitParam->pPositionY[_idx];
				_tranbuf[4 * _idx + 2] = _rot * _ratio;
				_tranbuf[4 * _idx + 3] = (_scale - 1.f) * _ratio + 1.f;
				_Node->pEmitParam->pAge[_idx] += _Delta;
			}
			if (_Node->pEmitParam->nCurAlive)
			{
				blGeometryInstanceUpdate(_Node->nGBO, BL_SL_COLOR1, _clrbuf, _Node->pEmitParam->nCurAlive * 4 * sizeof(BLF32));
				blGeometryInstanceUpdate(_Node->nGBO, BL_SL_INSTANCE1, _tranbuf, _Node->pEmitParam->nCurAlive * 4 * sizeof(BLF32));
			}
			for (BLU32 _idx = 0; _idx < _Node->pEmitParam->nMaxAlive; ++_idx)
			{
				if (_Node->pEmitParam->pAge[_idx] < _Node->pEmitParam->nLife)
				{
					memmove(_Node->pEmitParam->pAge, _Node->pEmitParam->pAge + _idx, (_Node->pEmitParam->nMaxAlive - _idx) * sizeof(BLU32));
					memmove(_Node->pEmitParam->pPositionX, _Node->pEmitParam->pPositionX + _idx, (_Node->pEmitParam->nMaxAlive - _idx) * sizeof(BLF32));
					memmove(_Node->pEmitParam->pPositionY, _Node->pEmitParam->pPositionY + _idx, (_Node->pEmitParam->nMaxAlive - _idx) * sizeof(BLF32));
					memmove(_Node->pEmitParam->pEmitAngle, _Node->pEmitParam->pEmitAngle + _idx, (_Node->pEmitParam->nMaxAlive - _idx) * sizeof(BLF32));
					memmove(_Node->pEmitParam->pVelocity, _Node->pEmitParam->pVelocity + _idx, (_Node->pEmitParam->nMaxAlive - _idx) * sizeof(BLF32));
					memmove(_Node->pEmitParam->pRotScale, _Node->pEmitParam->pRotScale + _idx, (_Node->pEmitParam->nMaxAlive - _idx) * sizeof(BLU32));
					memset(_Node->pEmitParam->pAge + _Node->pEmitParam->nMaxAlive - _idx, 0, _idx * sizeof(BLU32));
					break;
				}
			}
			blTechSampler(_PrSpriteMem->nSpriteInstTech, "Texture0", _Node->nTex, 0);
		}
		else if (_Node->nBrightness > 0)
		{
			blTechSampler(_PrSpriteMem->nSpriteGlowTech, "Texture0", _Node->nTex, 0);
			BLF32 _glow[4];
			blDeColor4F(_Node->nGlowColor, _glow);
			_glow[3] = (BLF32)_Node->nBrightness;
			blTechUniform(_PrSpriteMem->nSpriteGlowTech, BL_UB_F32X4, "Glow", _glow, sizeof(_glow));
			BLF32 _texsize[] = { (BLF32)_Node->nTexWidth, (BLF32)_Node->nTexHeight };
			blTechUniform(_PrSpriteMem->nSpriteGlowTech, BL_UB_F32X2, "TexSize", _texsize, sizeof(_texsize));
			BLF32 _border[] = { (BLF32)_ss->nLTx, (BLF32)_ss->nLTy, (BLF32)_ss->nRBx, (BLF32)_ss->nRBy };
			blTechUniform(_PrSpriteMem->nSpriteGlowTech, BL_UB_F32X4, "Border", _border, sizeof(_border));
		}
		else if (_Node->nStrokePixel > 0)
		{
			blTechSampler(_PrSpriteMem->nSpriteStrokeTech, "Texture0", _Node->nTex, 0);
			BLF32 _stroke[4];
			blDeColor4F(_Node->nStrokeColor, _stroke);
			_stroke[3] = (BLF32)_Node->nStrokePixel;
			blTechUniform(_PrSpriteMem->nSpriteStrokeTech, BL_UB_F32X4, "Stroke", _stroke, sizeof(_stroke));
			BLF32 _texsize[] = { (BLF32)_Node->nTexWidth, (BLF32)_Node->nTexHeight };
			blTechUniform(_PrSpriteMem->nSpriteStrokeTech, BL_UB_F32X2, "TexSize", _texsize, sizeof(_texsize));
			BLF32 _border[] = { (BLF32)_ss->nLTx - (BLF32)_Node->nStrokePixel, (BLF32)_ss->nLTy - (BLF32)_Node->nStrokePixel, (BLF32)_ss->nRBx + (BLF32)_Node->nStrokePixel, (BLF32)_ss->nRBy + (BLF32)_Node->nStrokePixel };
			blTechUniform(_PrSpriteMem->nSpriteStrokeTech, BL_UB_F32X4, "Border", _border, sizeof(_border));
		}
		else
			blTechSampler(_PrSpriteMem->nSpriteTech, "Texture0", _Node->nTex, 0);
		if (_ss)
		{
			BLF32 _lttx, _ltty, _rttx, _rtty, _lbtx, _lbty, _rbtx, _rbty;
			BLS32 _dif = MAX_INTERNAL(_Node->nStrokePixel, 3);
			_lbtx = _lttx = (BLF32)((BLS32)_ss->nLTx - _dif) / (BLF32)_Node->nTexWidth;
			_rtty = _ltty = (BLF32)((BLS32)_ss->nLTy - _dif) / (BLF32)_Node->nTexHeight;
			_rbtx = _rttx = (BLF32)((BLS32)_ss->nRBx + _dif) / (BLF32)_Node->nTexWidth;
			_rbty = _lbty = (BLF32)((BLS32)_ss->nRBy + _dif) / (BLF32)_Node->nTexHeight;
			_lttx += 0.01f;
			_lbtx += 0.01f;
			_rttx -= 0.01f;
			_rbtx -= 0.01f;
			_ltty += 0.01f;
			_lbty -= 0.01f;
			_rtty += 0.01f;
			_rbty -= 0.01f;
			if (_Node->bFlipX)
			{
				BLF32 _tmp;
				_tmp = _lbtx;
				_lbtx = _lttx = _rbtx;
				_rbtx = _rttx = _tmp;
			}
			if (_Node->bFlipY)
			{
				BLF32 _tmp;
				_tmp = _ltty;
				_rtty = _ltty = _rbty;
				_rbty = _lbty = _tmp;
			}
			BLF32 _rgba[4];
			blDeColor4F(_Node->nDyeColor, _rgba);
			BLF32 _vbo[] = {
				_ltx + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_lty + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_Node->fAlpha,
				_lttx,
				_ltty,
				_rtx + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_rty + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_Node->fAlpha,
				_rttx,
				_rtty,
				_lbx + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_lby + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_Node->fAlpha,
				_lbtx,
				_lbty,
				_rbx + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_rby + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_Node->fAlpha,
				_rbtx,
				_rbty
			};
			blGeometryBufferUpdate(_Node->nGBO, 0, (const BLU8*)_vbo, sizeof(_vbo), 0, NULL, 0);
			if (_Node->pEmitParam)
				blDraw(_PrSpriteMem->nSpriteInstTech, _Node->nGBO, _Node->pEmitParam->nCurAlive);
			else if (_Node->nBrightness > 0)
				blDraw(_PrSpriteMem->nSpriteGlowTech, _Node->nGBO, 1);
			else if (_Node->nStrokePixel > 0)
				blDraw(_PrSpriteMem->nSpriteStrokeTech, _Node->nGBO, 1);
			else
				blDraw(_PrSpriteMem->nSpriteTech, _Node->nGBO, 1);
		}
		else
			blDraw(_PrSpriteMem->nSpriteTech, _Node->nGBO, 1);
	}
	else if(_Node->pExternal)
		_Node->pExternal->pDrawCB(_Delta, _Node->nID, _Mat, ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f), ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f), &_Node->pExternalData);
    for (BLU32 _idx = 0; _idx < _Node->nChildren; ++_idx)
	{
		_BLSpriteNode* _chnode = _Node->pChildren[_idx];
		BLF32 _mat[6], _rmat[6];
		BLF32 _cos = cosf(_chnode->fRotate);
		BLF32 _sin = sinf(_chnode->fRotate);
		BLF32 _pivotx = (_chnode->sPivot.fX - 0.5f) * _chnode->sSize.fX;
		BLF32 _pivoty = (_chnode->sPivot.fY - 0.5f) * _chnode->sSize.fY;
		_mat[0] = (_chnode->fScaleX * _cos);
		_mat[1] = (_chnode->fScaleX * _sin);
		_mat[2] = (-_chnode->fScaleY * _sin);
		_mat[3] = (_chnode->fScaleY * _cos);
		_mat[4] = ((-_pivotx * _chnode->fScaleX) * _cos) + ((_pivoty * _chnode->fScaleY) * _sin) + _chnode->sPos.fX;
		_mat[5] = ((-_pivotx * _chnode->fScaleX) * _sin) + ((-_pivoty * _chnode->fScaleY) * _cos) + _chnode->sPos.fY;
		_rmat[0] = (_mat[0] * _Mat[0]) + (_mat[1] * _Mat[2]);
		_rmat[1] = (_mat[0] * _Mat[1]) + (_mat[1] * _Mat[3]);
		_rmat[2] = (_mat[2] * _Mat[0]) + (_mat[3] * _Mat[2]);
		_rmat[3] = (_mat[2] * _Mat[1]) + (_mat[3] * _Mat[3]);
		_rmat[4] = (_mat[4] * _Mat[0]) + (_mat[5] * _Mat[2]) + _Mat[4];
		_rmat[5] = (_mat[4] * _Mat[1]) + (_mat[5] * _Mat[3]) + _Mat[5];
		_SpriteDraw(_Delta, _chnode, _rmat);
	}
}
BLVoid
_SpriteInit(duk_context* _DKC)
{
    _PrSpriteMem = (_BLSpriteMember*)malloc(sizeof(_BLSpriteMember));
	_PrSpriteMem->pDukContext = _DKC;
    _PrSpriteMem->pNodeList = NULL;
    _PrSpriteMem->nNodeNum = 0;
    _PrSpriteMem->nNodeCap = 0;
	_PrSpriteMem->pCursor = NULL;
    _PrSpriteMem->bShaking = FALSE;
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
		_PrSpriteMem->pTileArray[_idx] = blGenArray(FALSE);
    _PrSpriteMem->nSpriteTech = blGenTechnique("shaders/2D.bsl", FALSE);
    _PrSpriteMem->nSpriteInstTech = blGenTechnique("shaders/2DInstance.bsl", FALSE);
	_PrSpriteMem->nSpriteStrokeTech = blGenTechnique("shaders/2DStroke.bsl", FALSE);
    _PrSpriteMem->nSpriteGlowTech = blGenTechnique("shaders/2DGlow.bsl", FALSE);
	_PrSpriteMem->nFBO = blGenFrameBuffer();
	_PrSpriteMem->aExternalMethod[0].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[1].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[2].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[3].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[4].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[5].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[6].aSuffix[0] = 0;
	_PrSpriteMem->aExternalMethod[7].aSuffix[0] = 0;
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	_PrSpriteMem->nQuadGeo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"#@quadgeosprite@#"), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, NULL, sizeof(BLF32) * 32, NULL, 0, BL_IF_INVALID);
    blSubscribeEvent(BL_ET_MOUSE, _MouseSubscriber);
	blSubscribeEvent(BL_ET_SYSTEM, _SystemSubscriber);
	BLU32 _width, _height;
	BLF32 _rx, _ry;
	blWindowQuery(&_width, &_height, &_PrSpriteMem->nFboWidth, &_PrSpriteMem->nFboHeight, &_rx, &_ry);
	_PrSpriteMem->sViewport.sLT.fX = _PrSpriteMem->sViewport.sLT.fY = 0.f;
	_PrSpriteMem->sViewport.sRB.fX = (BLF32)_PrSpriteMem->nFboWidth;
	_PrSpriteMem->sViewport.sRB.fY = (BLF32)_PrSpriteMem->nFboHeight;
	_PrSpriteMem->nFBOTex = blGenTexture(0xFFFFFFFF, BL_TT_2D, BL_TF_RGBA8, FALSE, FALSE, TRUE, 1, 1, _PrSpriteMem->nFboWidth, _PrSpriteMem->nFboHeight, 1, NULL);
	blFrameBufferAttach(_PrSpriteMem->nFBO, _PrSpriteMem->nFBOTex, 0, BL_CTF_IGNORE);
}
BLVoid
_SpriteStep(BLU32 _Delta, BLBool _Cursor)
{
	BLU32 _width, _height;
	BLF32 _rx, _ry;
	blWindowQuery(&_width, &_height, &_PrSpriteMem->nFboWidth, &_PrSpriteMem->nFboHeight, &_rx, &_ry);
	BLU8 _blendfactor[4] = { 0 };
	blDepthStencilState(FALSE, TRUE, BL_CF_LESS, FALSE, 0xFF, 0xFF, BL_SO_KEEP, BL_SO_KEEP, BL_SO_KEEP, BL_CF_ALWAYS, BL_SO_KEEP, BL_SO_KEEP, BL_SO_KEEP, BL_CF_ALWAYS, FALSE);
	blBlendState(FALSE, TRUE, BL_BF_SRCALPHA, BL_BF_INVSRCALPHA, BL_BF_INVDESTALPHA, BL_BF_ONE, BL_BO_ADD, BL_BO_ADD, _blendfactor, FALSE);
    if (_Cursor)
    {
		BLF32 _screensz[] = { 2.f / (BLF32)_width, 2.f / (BLF32)_height };
		blTechUniform(_PrSpriteMem->nSpriteTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
        if (!_PrSpriteMem->pCursor)
            return;
        if (!_PrSpriteMem->pCursor->bValid)
            return;
		BLF32 _mat[6];
		BLF32 _pivotx = (_PrSpriteMem->pCursor->sPivot.fX - 0.5f) * _PrSpriteMem->pCursor->sSize.fX;
		BLF32 _pivoty = (_PrSpriteMem->pCursor->sPivot.fY - 0.5f) * _PrSpriteMem->pCursor->sSize.fY;
		_mat[0] = _PrSpriteMem->pCursor->fScaleX;
		_mat[1] = 0;
		_mat[2] = 0;
		_mat[3] = _PrSpriteMem->pCursor->fScaleY;
		_mat[4] = (-_pivotx * _PrSpriteMem->pCursor->fScaleX) + _PrSpriteMem->pCursor->sPos.fX;
		_mat[5] = (-_pivoty * _PrSpriteMem->pCursor->fScaleY) + _PrSpriteMem->pCursor->sPos.fY;
		blRasterState(BL_CM_CW, 0, 0.f, TRUE, 0, 0, 0, 0, FALSE);
		_SpriteDraw(_Delta, _PrSpriteMem->pCursor, _mat);
    }
    else
    {
		blBindFrameBuffer(_PrSpriteMem->nFBO);
		BLF32 _screensz[2] = { 2.f / (BLF32)_PrSpriteMem->nFboWidth, 2.f / (BLF32)_PrSpriteMem->nFboHeight };
		blTechUniform(_PrSpriteMem->nSpriteTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
		blTechUniform(_PrSpriteMem->nSpriteInstTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
		blTechUniform(_PrSpriteMem->nSpriteStrokeTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
		blTechUniform(_PrSpriteMem->nSpriteGlowTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
		blFrameBufferClear(TRUE, FALSE, FALSE);
		BLRect _scalevp = _PrSpriteMem->sViewport;
		_scalevp.sLT.fX -= (_PrSpriteMem->sViewport.sRB.fX - _PrSpriteMem->sViewport.sLT.fX) * 0.25f;
		_scalevp.sRB.fX += (_PrSpriteMem->sViewport.sRB.fX - _PrSpriteMem->sViewport.sLT.fX) * 0.25f;
		_scalevp.sLT.fY -= (_PrSpriteMem->sViewport.sRB.fY - _PrSpriteMem->sViewport.sLT.fY) * 0.25f;
		_scalevp.sRB.fY += (_PrSpriteMem->sViewport.sRB.fY - _PrSpriteMem->sViewport.sLT.fY) * 0.25f;
		_BLTileInfo* _first = blArrayFrontElement(_PrSpriteMem->pTileArray[0]);
		if (_first)
		{
			BLU32 _totalnum = (BLU32)((_scalevp.sRB.fX - _scalevp.sLT.fX) * (_scalevp.sRB.fY - _scalevp.sLT.fY) / (_first->sSize.fX * _first->sSize.fY)) * 2;
			BLGuid _layertex;
			blRasterState(BL_CM_CW, 0, 0.f, TRUE, 0, 0, _PrSpriteMem->nFboWidth, _PrSpriteMem->nFboHeight, FALSE);
			BLU8* _geomem = (BLU8*)alloca(_totalnum * 48 * sizeof(BLF32));
			for (BLS32 _idx = 0; _idx < 8; ++_idx)
			{
				BLU32 _tilenum = 0;
				BLBool _show = FALSE;
				_first = blArrayFrontElement(_PrSpriteMem->pTileArray[_idx]);
				if (_first)
					_show = _first->bShow;
				if (!_show)
					continue;
				memset(_geomem, 0, _totalnum * 48 * sizeof(BLF32));
				FOREACH_ARRAY(_BLTileInfo*, _tile, _PrSpriteMem->pTileArray[_idx])
				{
					if (blRectContains(&_scalevp, &_tile->sPos) && _tile->aFilename[0] != 0)
					{
						_layertex = blGainTexture(blHashUtf8((const BLUtf8*)_tile->aFilename));
						if (_layertex == INVALID_GUID)
						{
							BLGuid _stream = blGenStream(_tile->aFilename);
							BLU8 _identifier[12];
							blStreamRead(_stream, sizeof(_identifier), _identifier);
							if (_identifier[0] != 0xDD ||
								_identifier[1] != 0xDD ||
								_identifier[2] != 0xDD ||
								_identifier[3] != 0xEE ||
								_identifier[4] != 0xEE ||
								_identifier[5] != 0xEE ||
								_identifier[6] != 0xAA ||
								_identifier[7] != 0xAA ||
								_identifier[8] != 0xAA ||
								_identifier[9] != 0xDD ||
								_identifier[10] != 0xDD ||
								_identifier[11] != 0xDD)
							{
								blDeleteStream(_stream);
								continue;
							}
							BLU32 _width, _height, _depth;
							blStreamRead(_stream, sizeof(BLU32), &_width);
							blStreamRead(_stream, sizeof(BLU32), &_height);
							blStreamRead(_stream, sizeof(BLU32), &_depth);
							BLU32 _array, _faces, _mipmap;
							blStreamRead(_stream, sizeof(BLU32), &_array);
							blStreamRead(_stream, sizeof(BLU32), &_faces);
							blStreamRead(_stream, sizeof(BLU32), &_mipmap);
							BLU32 _fourcc, _channels, _offset;
							blStreamRead(_stream, sizeof(BLU32), &_fourcc);
							blStreamRead(_stream, sizeof(BLU32), &_channels);
							blStreamRead(_stream, sizeof(BLU32), &_offset);
							BLEnum _type = BL_TT_2D;
							blStreamSeek(_stream, _offset);
							BLU32 _imagesz;
							BLEnum _format;
							switch (_fourcc)
							{
							case FOURCC_INTERNAL('B', 'M', 'G', 'T'):
								blStreamRead(_stream, sizeof(BLU32), &_imagesz);
								_format = (_channels == 4) ? BL_TF_RGBA8 : BL_TF_RGB8;
								break;
							case FOURCC_INTERNAL('S', '3', 'T', '1'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
								_format = BL_TF_BC1;
								break;
							case FOURCC_INTERNAL('S', '3', 'T', '2'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
								_format = BL_TF_BC1A1;
								break;
							case FOURCC_INTERNAL('S', '3', 'T', '3'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
								_format = BL_TF_BC3;
								break;
							case FOURCC_INTERNAL('A', 'S', 'T', '1'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
								_format = BL_TF_ASTC;
								break;
							case FOURCC_INTERNAL('A', 'S', 'T', '2'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
								_format = BL_TF_ASTC;
								break;
							case FOURCC_INTERNAL('A', 'S', 'T', '3'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
								_format = BL_TF_ASTC;
								break;
							case FOURCC_INTERNAL('E', 'T', 'C', '1'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
								_format = BL_TF_ETC2;
								break;
							case FOURCC_INTERNAL('E', 'T', 'C', '2'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
								_format = BL_TF_ETC2A1;
								break;
							case FOURCC_INTERNAL('E', 'T', 'C', '3'):
								_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
								_format = BL_TF_ETC2A;
								break;
							default:assert(0); break;
							}
							BLU8* _texdata;
							if (_fourcc == FOURCC_INTERNAL('B', 'M', 'G', 'T'))
							{
								BLU8* _data = (BLU8*)malloc(_imagesz);
								blStreamRead(_stream, _imagesz, _data);
								BLU8* _data2 = (BLU8*)malloc(_width * _height * _channels);
								blRLEDecode(_data, _width * _height * _channels, _data2);
								free(_data);
								_texdata = _data2;
							}
							else
							{
								BLU8* _data = (BLU8*)malloc(_imagesz);
								blStreamRead(_stream, _imagesz, _data);
								_texdata = _data;
							}
							_layertex = blGenTexture(blHashUtf8(_tile->aFilename), BL_TT_2D, _format, FALSE, TRUE, FALSE, 1, 1, _width, _height, 1, _texdata);
							free(_texdata);
							blDeleteStream(_stream);
						}
						_tile->nTex = _layertex;
						BLF32 _vbo[] = {
							-_tile->sSize.fX * 0.5f + _tile->sPos.fX - _PrSpriteMem->sViewport.sLT.fX + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							-_tile->sSize.fY * 0.5f + _tile->sPos.fY - _PrSpriteMem->sViewport.sLT.fY + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							1.f,
							1.f,
							1.f,
							_tile->fAlpha,
							_tile->fTexLTx,
							_tile->fTexLTy,
							_tile->sSize.fX * 0.5f + _tile->sPos.fX - _PrSpriteMem->sViewport.sLT.fX + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							-_tile->sSize.fY * 0.5f + _tile->sPos.fY - _PrSpriteMem->sViewport.sLT.fY + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							1.f,
							1.f,
							1.f,
							_tile->fAlpha,
							_tile->fTexRBx,
							_tile->fTexLTy,
							-_tile->sSize.fX * 0.5f + _tile->sPos.fX - _PrSpriteMem->sViewport.sLT.fX + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							_tile->sSize.fY * 0.5f + _tile->sPos.fY - _PrSpriteMem->sViewport.sLT.fY + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							1.f,
							1.f,
							1.f,
							_tile->fAlpha,
							_tile->fTexLTx,
							_tile->fTexRBy,
							-_tile->sSize.fX * 0.5f + _tile->sPos.fX - _PrSpriteMem->sViewport.sLT.fX + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							_tile->sSize.fY * 0.5f + _tile->sPos.fY - _PrSpriteMem->sViewport.sLT.fY + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							1.f,
							1.f,
							1.f,
							_tile->fAlpha,
							_tile->fTexLTx,
							_tile->fTexRBy,
							_tile->sSize.fX * 0.5f + _tile->sPos.fX - _PrSpriteMem->sViewport.sLT.fX + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							-_tile->sSize.fY * 0.5f + _tile->sPos.fY - _PrSpriteMem->sViewport.sLT.fY + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							1.f,
							1.f,
							1.f,
							_tile->fAlpha,
							_tile->fTexRBx,
							_tile->fTexLTy,
							_tile->sSize.fX * 0.5f + _tile->sPos.fX - _PrSpriteMem->sViewport.sLT.fX + ((_PrSpriteMem->bShaking && !_PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							_tile->sSize.fY * 0.5f + _tile->sPos.fY - _PrSpriteMem->sViewport.sLT.fY + ((_PrSpriteMem->bShaking && _PrSpriteMem->bShakingVertical) ? _PrSpriteMem->fShakingForce : 0.f),
							1.f,
							1.f,
							1.f,
							_tile->fAlpha,
							_tile->fTexRBx,
							_tile->fTexRBy
						};
						memcpy(_geomem + _tilenum * 48 * sizeof(BLF32), _vbo, sizeof(_vbo));
						++_tilenum;
					}
				}
				if (_tilenum)
				{
					blTechSampler(_PrSpriteMem->nSpriteTech, "Texture0", _layertex, 0);
					BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
					BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
					BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _geomem, _tilenum * 48 * sizeof(BLF32), NULL, 0, BL_IF_INVALID);
					blDraw(_PrSpriteMem->nSpriteTech, _geo, 1);
					blDeleteGeometryBuffer(_geo);
				}
			}
		}
		_SpriteUpdate(_Delta);
		for (BLU32 _idx = 0; _idx < _PrSpriteMem->nNodeNum; ++_idx)
		{
			_BLSpriteNode* _node = _PrSpriteMem->pNodeList[_idx];
			BLF32 _mat[6];
			BLF32 _cos = cosf(_node->fRotate);
			BLF32 _sin = sinf(_node->fRotate);
			BLF32 _pivotx = (_node->sPivot.fX - 0.5f) * _node->sSize.fX;
			BLF32 _pivoty = (_node->sPivot.fY - 0.5f) * _node->sSize.fY;
			_mat[0] = (_node->fScaleX * _cos);
			_mat[1] = (_node->fScaleX * _sin);
			_mat[2] = (-_node->fScaleY * _sin);
			_mat[3] = (_node->fScaleY * _cos);
			_mat[4] = ((-_pivotx * _node->fScaleX) * _cos) + ((_pivoty * _node->fScaleY) * _sin) + _node->sPos.fX - _PrSpriteMem->sViewport.sLT.fX;
			_mat[5] = ((-_pivotx * _node->fScaleX) * _sin) + ((-_pivoty * _node->fScaleY) * _cos) + _node->sPos.fY - _PrSpriteMem->sViewport.sLT.fY;
			_SpriteDraw(_Delta, _node, _mat);
		}
		blFrameBufferResolve(_PrSpriteMem->nFBO);
		if (_PrSpriteMem->bShaking)
		{
			_PrSpriteMem->fShakingForce = -_PrSpriteMem->fShakingForce;
			_PrSpriteMem->fShakingTime -= _Delta / 1000.f;
			if (_PrSpriteMem->fShakingTime < 0.f)
				_PrSpriteMem->bShaking = FALSE;
		}
		blBindFrameBuffer(INVALID_GUID);
		_screensz[0] = 2.f / (BLF32)_width;
		_screensz[1] = 2.f / (BLF32)_height;
		blTechUniform(_PrSpriteMem->nSpriteTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
		BLF32 _vbo[] = {
			PIXEL_ALIGNED_INTERNAL(0.f),
			PIXEL_ALIGNED_INTERNAL(0.f),
			1.f,
			1.f,
			1.f,
			1.f,
			0.f,
			1.f,
			PIXEL_ALIGNED_INTERNAL(_width),
			PIXEL_ALIGNED_INTERNAL(0.f),
			1.f,
			1.f,
			1.f,
			1.f,
			1.f,
			1.f,
			PIXEL_ALIGNED_INTERNAL(0.f),
			PIXEL_ALIGNED_INTERNAL(_height),
			1.f,
			1.f,
			1.f,
			1.f,
			0.f,
			0.f,
			PIXEL_ALIGNED_INTERNAL(_width),
			PIXEL_ALIGNED_INTERNAL(_height),
			1.f,
			1.f,
			1.f,
			1.f,
			1.f,
			0.f
		};
		blTechSampler(_PrSpriteMem->nSpriteTech, "Texture0", _PrSpriteMem->nFBOTex, 0);
		blGeometryBufferUpdate(_PrSpriteMem->nQuadGeo, 0, (BLU8*)_vbo, sizeof(_vbo), 0, NULL, 0);
		blRasterState(BL_CM_CW, 0, 0.f, TRUE, 0, 0, 0, 0, FALSE);
		blDraw(_PrSpriteMem->nSpriteTech, _PrSpriteMem->nQuadGeo, 1);
    }
}
BLVoid
_SpriteDestroy()
{
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
	{		
		FOREACH_ARRAY(_BLTileInfo*, _tileiter, _PrSpriteMem->pTileArray[_idx])
		{
			blDeleteTexture(_tileiter->nTex);
			blDeleteGuid(_tileiter->nID);
			free(_tileiter);
		}
		blDeleteArray(_PrSpriteMem->pTileArray[_idx]);
	}
    if (_PrSpriteMem->pNodeList)
        free(_PrSpriteMem->pNodeList);
	blDeleteGeometryBuffer(_PrSpriteMem->nQuadGeo);
	blFrameBufferDetach(_PrSpriteMem->nFBO, FALSE);
	blDeleteTexture(_PrSpriteMem->nFBOTex);
	blDeleteFrameBuffer(_PrSpriteMem->nFBO);
    blDeleteTechnique(_PrSpriteMem->nSpriteTech);
    blDeleteTechnique(_PrSpriteMem->nSpriteInstTech);
	blDeleteTechnique(_PrSpriteMem->nSpriteStrokeTech);
	blDeleteTechnique(_PrSpriteMem->nSpriteGlowTech);
    free(_PrSpriteMem);
	_PrSpriteMem = NULL;
}
BLVoid 
blRegistExternalMethod(IN BLAnsi* _Suffix, IN BLBool(*_LoadCB)(BLGuid, const BLAnsi*, BLVoid**), IN BLBool(*_SetupCB)(BLGuid, BLVoid**), IN BLBool(*_UnloadCB)(BLGuid, BLVoid**), IN BLBool(*_ReleaseCB)(BLGuid, BLVoid**), IN BLVoid(*_DrawCB)(BLU32, BLGuid, BLF32[6], BLF32, BLF32, BLVoid**))
{
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
	{
		if (_PrSpriteMem->aExternalMethod[_idx].aSuffix[0] == 0)
		{
			memset(_PrSpriteMem->aExternalMethod[_idx].aSuffix, 0, sizeof(_PrSpriteMem->aExternalMethod[_idx].aSuffix));
			strcpy(_PrSpriteMem->aExternalMethod[_idx].aSuffix, _Suffix);
			_PrSpriteMem->aExternalMethod[_idx].pLoadCB = _LoadCB;
			_PrSpriteMem->aExternalMethod[_idx].pSetupCB = _SetupCB;
			_PrSpriteMem->aExternalMethod[_idx].pUnloadCB = _UnloadCB;
			_PrSpriteMem->aExternalMethod[_idx].pReleaseCB = _ReleaseCB;
			_PrSpriteMem->aExternalMethod[_idx].pDrawCB = _DrawCB;
			break;
		}
	}
}
BLGuid
blGenSprite(IN BLAnsi* _Filename, IN BLAnsi* _Tag, IN BLF32 _Width, IN BLF32 _Height, IN BLF32 _Zv, IN BLU32 _FPS, IN BLBool _AsTile)
{
	if (_AsTile)
	{
		_BLTileInfo* _tile = (_BLTileInfo*)malloc(sizeof(_BLTileInfo));
		_tile->nTex = INVALID_GUID;
		_tile->sSize.fX = _Width;
		_tile->sSize.fY = _Height;
		_tile->sPos.fX = _tile->sPos.fY = 0.f;
		_tile->bShow = TRUE;
		memset(_tile->aFilename, 0, sizeof(_tile->aFilename));
		strcpy(_tile->aFilename, _Filename);
		_tile->nID = blGenGuid(_tile, blHashUtf8((const BLUtf8*)_Filename));
		blArrayPushBack(_PrSpriteMem->pTileArray[strtol(_Tag, NULL, 10)], _tile);
		return _tile->nID;
	}
	else
	{
		_BLSpriteNode* _node = (_BLSpriteNode*)malloc(sizeof(_BLSpriteNode));
		_node->nTex = INVALID_GUID;
		_node->nGBO = INVALID_GUID;
		_node->pExternal = NULL;
		_node->pExternalData = NULL;
		_node->sSize.fX = _Width;
		_node->sSize.fY = _Height;
		_node->sPos.fX = _node->sPos.fY = 0.f;
		_node->sPivot.fX = _node->sPivot.fY = 0.5f;
		_node->sScissor.sLT.fX = _node->sScissor.sLT.fY = -1.f;
		_node->sScissor.sRB.fX = _node->sScissor.sRB.fY = -1.f;
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
		_node->nDyeColor = 0xFFFFFFFF;
		_node->bDye = FALSE;
		_node->nStrokeColor = 0xFFFFFFFF;
		_node->nStrokePixel = 0;
		_node->nGlowColor = 0xFFFFFFFF;
		_node->nBrightness = 0;
		_node->bShow = TRUE;
		_node->bValid = FALSE;
		_node->nTimePassed = 0;
		_node->nCurFrame = 0;
		_node->nFrameNum = 1;
		_node->bFlipX = FALSE;
		_node->bFlipY = FALSE;
		memset(_node->aTag, 0, sizeof(_node->aTag));
		memset(_node->aFilename, 0, sizeof(_node->aFilename));
		if (_Tag)
		{
			BLAnsi* _tmp;
			BLAnsi* _tag = (BLAnsi*)alloca(strlen(_Tag) + 1);
			memset(_tag, 0, strlen(_Tag) + 1);
			strcpy(_tag, _Tag);
			_node->nFrameNum = 0;
			_tmp = strtok((BLAnsi*)_tag, ",");
			while (_tmp)
			{
				_node->aTag[_node->nFrameNum] = blHashUtf8(_tmp);
				_tmp = strtok(NULL, ",");
				_node->nFrameNum++;
				if (_node->nFrameNum >= 63)
					break;
			}
		}
		if (_Filename)
		{
			strcpy(_node->aFilename, _Filename);
			_node->nID = blGenGuid(_node, blHashUtf8((const BLUtf8*)_Filename));
			if (!strcmp(blFileSuffixUtf8(_Filename), "bmg"))
				_FetchResource(_Filename, (BLVoid**)&_node, _node->nID, _LoadSprite, _SpriteSetup, TRUE);
			else
			{
				for (BLU32 _idx = 0; _idx < 8; ++_idx)
				{
					if (_PrSpriteMem->aExternalMethod[_idx].aSuffix[0] && !strcmp(blFileSuffixUtf8(_Filename), _PrSpriteMem->aExternalMethod[_idx].aSuffix))
					{
						_node->pExternal = &_PrSpriteMem->aExternalMethod[_idx];
						_FetchResource(_Filename, (BLVoid**)&_node, _node->nID, _LoadSprite, _SpriteSetup, TRUE);
						break;
					}
				}
			}
		}
		else
		{
			_node->pTexData = NULL;
			_node->bValid = TRUE;
			_node->nID = blGenGuid(_node, blUniqueUri());
			_AddToNodeList(_node);
		}
		return _node->nID;
	}
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
BLVoid* 
blSpriteExternalData(IN BLGuid _ID, IN BLVoid* _ExtData)
{
	if (_ID == INVALID_GUID)
		return NULL;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return NULL;
	if (_ExtData)
		_node->pExternalData = (BLVoid*)_ExtData;
	return _node->pExternalData;
}
BLVoid
blSpriteClear(IN BLBool _Tiles, IN BLBool _Actors)
{
    if (_Tiles)
    {
		for (BLU32 _idx = 0; _idx < 8; ++_idx)
		{
			FOREACH_ARRAY(_BLTileInfo*, _tileiter, _PrSpriteMem->pTileArray[_idx])
			{
				blDeleteTexture(_tileiter->nTex);
				blDeleteGuid(_tileiter->nID);
				free(_tileiter);
			}
			blClearArray(_PrSpriteMem->pTileArray[_idx]);
		}
    }
    if (_Actors)
    {
        for (BLU32 _idx = 0; _idx < _PrSpriteMem->nNodeNum; ++_idx)
        {
            blDeleteSprite(_PrSpriteMem->pNodeList[_idx]->nID);
        }
        free(_PrSpriteMem->pNodeList);
        _PrSpriteMem->nNodeNum = 0;
    }
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
        blSpriteGlow(_Child, _parent->nGlowColor, _parent->nBrightness, TRUE);
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
blSpriteQuery(IN BLGuid _ID, OUT BLF32* _Width, OUT BLF32* _Height,OUT BLF32* _XPos, OUT BLF32* _YPos, OUT BLF32* _Zv, OUT BLF32* _Rotate, OUT BLF32* _XScale, OUT BLF32* _YScale, OUT BLF32* _Alpha, OUT BLU32* _DyeClr, OUT BLBool* _FlipX, OUT BLBool* _FlipY, OUT BLBool* _Show)
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
	*_DyeClr = _node->nDyeColor;
	*_FlipX = _node->bFlipX;
	*_FlipY = _node->bFlipY;
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
blSpriteFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY, IN BLBool _Passdown)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return FALSE;
	_node->bFlipX = _FlipX;
	_node->bFlipY = _FlipY;
	if (_Passdown)
	{
		for (BLU32 _idx = 0; _idx < _node->nChildren; ++_idx)
			blSpriteFlip(_node->pChildren[_idx]->nID, _FlipX, _FlipY, _Passdown);
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
blSpriteGlow(IN BLGuid _ID, IN BLU32 _Color, IN BLU32 _Brightness, IN BLBool _Passdown)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
    _node->nGlowColor = _Color;
    _node->nBrightness = _Brightness;
    if (_Passdown)
    {
        for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
            blSpriteGlow(_node->pChildren[_idx]->nID, _Color, _Brightness, _Passdown);
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
    _node->sScissor.sLT.fX = _XPos;
    _node->sScissor.sLT.fY = _YPos;
    _node->sScissor.sRB.fX = _XPos + _Width;
    _node->sScissor.sRB.fY = _YPos + _Height;
    for (BLU32 _idx = 0; _idx < _node->nChildren ; ++_idx)
		blSpriteScissor(_node->pChildren[_idx]->nID, _XPos, _YPos, _Width, _Height);
    return TRUE;
}
BLVoid
blSpriteTile(IN BLGuid _ID, IN BLAnsi* _ImageFile, IN BLF32 _TexLTx, IN BLF32 _TexLTy, IN BLF32 _TexRBx, IN BLF32 _TexRBy, IN BLF32 _PosX, IN BLF32 _PosY, IN BLF32 _Alpha, IN BLBool _Show)
{
    if (_ID == INVALID_GUID)
        return;
	_BLTileInfo* _tile = (_BLTileInfo*)blGuidAsPointer(_ID);
    if (!_tile)
        return;
	if (!_ImageFile)
		_tile->bShow = _Show;
	else
	{
		memset(_tile->aFilename, 0, sizeof(_tile->aFilename));
		strcpy(_tile->aFilename, _ImageFile);
		_tile->fTexLTx = _TexLTx;
		_tile->fTexLTy = _TexLTy;
		_tile->fTexRBx = _TexRBx;
		_tile->fTexRBy = _TexRBy;
		_tile->sPos.fX = _PosX;
		_tile->sPos.fY = _PosY;
		_tile->fAlpha = _Alpha;
		_tile->bShow = _Show;
	}
}
BLBool
blSpriteAsEmit(IN BLGuid _ID, IN BLF32 _EmitAngle, IN BLF32 _EmitterRadius, IN BLF32 _Life, IN BLU32 _MaxAlive, IN BLU32 _GenPerSec, IN BLF32 _DirectionX, IN BLF32 _DirectionY, IN BLF32 _Velocity, IN BLF32 _VelVariance, IN BLF32 _Rotation, IN BLF32 _RotVariance, IN BLF32 _Scale, IN BLF32 _ScaleVariance, IN BLU32 _FadeColor, IN BLBool _Off)
{
    if (_ID == INVALID_GUID)
        return FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	if (!_node)
		return FALSE;
	if (_Off)
	{
		if (_node->pEmitParam)
		{
			free(_node->pEmitParam);
			_node->pEmitParam = NULL;
		}
		return TRUE;
	}
	if (!_node->pEmitParam)
    {
        _node->pEmitParam = (_BLEmitParam*)malloc(sizeof(_BLEmitParam));
        _node->pEmitParam->pAge = (BLU32*)malloc(_MaxAlive * sizeof(BLU32));
        memset(_node->pEmitParam->pAge, 0, _MaxAlive * sizeof(BLU32));
        _node->pEmitParam->pPositionX = (BLF32*)malloc(_MaxAlive * sizeof(BLF32));
        _node->pEmitParam->pPositionY = (BLF32*)malloc(_MaxAlive * sizeof(BLF32));
        _node->pEmitParam->pEmitAngle = (BLF32*)malloc(_MaxAlive * sizeof(BLF32));
        _node->pEmitParam->pVelocity = (BLF32*)malloc(_MaxAlive * sizeof(BLF32));
        _node->pEmitParam->pRotScale = (BLU32*)malloc(_MaxAlive * sizeof(BLU32));
    }
	_node->pEmitParam->fEmitterRadius = _EmitterRadius;
	_node->pEmitParam->nLife = (BLU32)(_Life * 1000);
	_node->pEmitParam->nMaxAlive = _MaxAlive;
	_node->pEmitParam->fGenPerMSec = (BLF32)_GenPerSec / 1000.f;
    BLVec2 _dir;
    _dir.fX = _DirectionX;
    _dir.fY = _DirectionY;
    blVec2Normalize(&_dir);
	_node->pEmitParam->fDirectionX = _dir.fX;
    _node->pEmitParam->fDirectionY = _dir.fY;
    _node->pEmitParam->fVelocity = _Velocity;
    _node->pEmitParam->fVelVariance = _VelVariance;
	_node->pEmitParam->fEmitAngle = _EmitAngle * PI_INTERNAL / 180.f;
    _node->pEmitParam->fRotation = _Rotation * PI_INTERNAL / 180.f;
    _node->pEmitParam->fRotVariance = _RotVariance * PI_INTERNAL / 180.f;
    _node->pEmitParam->fScale = _Scale;
    _node->pEmitParam->fScaleVariance = _ScaleVariance;
    _node->pEmitParam->nFadeColor = _FadeColor;
    _node->pEmitParam->nCurAlive = 0;
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
	_node->pAction = NULL;
	_PrSpriteMem->sActionRecorder.bParallelEdit = FALSE;
	_PrSpriteMem->sActionRecorder.fAlphaRecord = _node->fAlpha;
	_PrSpriteMem->sActionRecorder.fScaleXRecord = _node->fScaleX;
	_PrSpriteMem->sActionRecorder.fScaleYRecord = _node->fScaleY;
	_PrSpriteMem->sActionRecorder.fPosXRecord = _node->sPos.fX;
	_PrSpriteMem->sActionRecorder.fPosYRecord = _node->sPos.fY;
    return TRUE;
}
BLBool
blSpriteActionEnd(IN BLGuid _ID, IN BLBool _Delete, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
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
		_act->uAction.sDead.pBegin = NULL;
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		_lastact->pNext = _act;
	}
	else if (_Loop)
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
		_act->uAction.sDead.pBegin = _node->pAction;
		_act->uAction.sDead.fAlphaCache = _node->fAlpha;
		_act->uAction.sDead.fScaleXCache = _node->fScaleX;
		_act->uAction.sDead.fScaleYCache = _node->fScaleY;
		_act->uAction.sDead.fOffsetXCache = _node->sPos.fX;
		_act->uAction.sDead.fOffsetYCache = _node->sPos.fY;
		_act->uAction.sDead.fRotateCache = _node->fRotate;
		_BLSpriteAction* _lastact = _node->pAction;
		while (_lastact->pNext)
			_lastact = _lastact->pNext;
		_lastact->pNext = _act;
	}
	_node->pCurAction = NULL;
	_PrSpriteMem->sActionRecorder.bParallelEdit = FALSE;
    return TRUE;
}
BLBool
blSpriteParallelBegin(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	if (_PrSpriteMem->sActionRecorder.bParallelEdit)
		return FALSE;
	_PrSpriteMem->sActionRecorder.bParallelEdit = TRUE;
	return TRUE;
}
BLBool
blSpriteParallelEnd(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	if (!_PrSpriteMem->sActionRecorder.bParallelEdit)
		return FALSE;
	_PrSpriteMem->sActionRecorder.bParallelEdit = FALSE;
	_BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
	_BLSpriteAction* _lastact = _node->pAction;
	while (_lastact->pNext)
		_lastact = _lastact->pNext;
	_lastact->bParallel = FALSE;
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
	if (!_node->pAction)
		return FALSE;
	_node->pCurAction = NULL;
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
	if (_PathNum <= 0)
		return FALSE;
	if (blScalarApproximate(_Time, 0.f))
	{
		_node->sPos.fX = _XPath[_PathNum - 1];
		_node->sPos.fY = _YPath[_PathNum - 1];
		_PrSpriteMem->sActionRecorder.fPosXRecord = _node->sPos.fX;
		_PrSpriteMem->sActionRecorder.fPosYRecord = _node->sPos.fY;
		return FALSE;
	}
	else
	{
		_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
		_act->pNeighbor = NULL;
		_act->pNext = NULL;
		_act->bLoop = _Loop;
		_act->bParallel = _PrSpriteMem->sActionRecorder.bParallelEdit;
		_act->eActionType = SPACTION_MOVE_INTERNAL;
		_act->fCurTime = 0.f;
		_act->fTotalTime = _Time;
		_act->uAction.sMove.fAcceleration = _Acceleration;
		BLF32 _s = sqrtf((_XPath[0] - _PrSpriteMem->sActionRecorder.fPosXRecord) * (_XPath[0] - _PrSpriteMem->sActionRecorder.fPosXRecord) + (_YPath[0] - _PrSpriteMem->sActionRecorder.fPosYRecord) * (_YPath[0] - _PrSpriteMem->sActionRecorder.fPosYRecord));
		for (BLU32 _idx = 1; _idx < _PathNum; ++_idx)
			_s += sqrtf((_XPath[_idx] - _XPath[_idx - 1]) * (_XPath[_idx] - _XPath[_idx - 1]) + (_YPath[_idx] - _YPath[_idx - 1]) * (_YPath[_idx] - _YPath[_idx - 1]));
		_act->uAction.sMove.fVelocity = _s / _Time - 0.5f * _Acceleration * _Time;
		_act->uAction.sMove.pXPath = (BLF32*)malloc((_PathNum + 1) * sizeof(BLF32));
		_act->uAction.sMove.pYPath = (BLF32*)malloc((_PathNum + 1) * sizeof(BLF32));
		_act->uAction.sMove.pXPath[0] = _PrSpriteMem->sActionRecorder.fPosXRecord;
		_act->uAction.sMove.pYPath[0] = _PrSpriteMem->sActionRecorder.fPosYRecord;
		_act->uAction.sMove.nPathNum = _PathNum + 1;
		memcpy(_act->uAction.sMove.pXPath + 1, _XPath, _PathNum * sizeof(BLF32));
		memcpy(_act->uAction.sMove.pYPath + 1, _YPath, _PathNum * sizeof(BLF32));
		_PrSpriteMem->sActionRecorder.fPosXRecord = _XPath[_PathNum - 1];
		_PrSpriteMem->sActionRecorder.fPosYRecord = _YPath[_PathNum - 1];
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
			if (_PrSpriteMem->sActionRecorder.bParallelEdit && _lastact->bParallel)
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
}
BLBool
blSpriteActionRotate(IN BLGuid _ID, IN BLF32 _Angle, IN BLBool _ClockWise, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (blScalarApproximate(_Time, 0.f))
	{
		_node->fRotate = _ClockWise ? _Angle * PI_INTERNAL / 180.0f : 2 * PI_INTERNAL - _Angle * PI_INTERNAL / 180.0f;
		return FALSE;
	}
	else
	{
		_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
		_act->pNeighbor = NULL;
		_act->pNext = NULL;
		_act->bLoop = _Loop;
		_act->bParallel = _PrSpriteMem->sActionRecorder.bParallelEdit;
		_act->eActionType = SPACTION_ROTATE_INTERNAL;
		_act->fCurTime = 0.f;
		_act->fTotalTime = _Time;
		_act->uAction.sRotate.bClockWise = _ClockWise;
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
			if (_PrSpriteMem->sActionRecorder.bParallelEdit && _lastact->bParallel)
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
}
BLBool
blSpriteActionScale(IN BLGuid _ID, IN BLF32 _XScale, IN BLF32 _YScale, IN BLBool _Reverse, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (blScalarApproximate(_Time, 0.f))
	{
		_node->fScaleX = _XScale;
		_node->fScaleY = _YScale;
		_PrSpriteMem->sActionRecorder.fScaleXRecord = _XScale;
		_PrSpriteMem->sActionRecorder.fScaleYRecord = _YScale;
		return FALSE;
	}
	else
	{
		_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
		_act->pNeighbor = NULL;
		_act->pNext = NULL;
		_act->bLoop = _Loop;
		_act->bParallel = _PrSpriteMem->sActionRecorder.bParallelEdit;
		_act->eActionType = SPACTION_SCALE_INTERNAL;
		_act->fCurTime = 0.f;
		_act->fTotalTime = _Time;
		_act->uAction.sScale.fXInitScale = _PrSpriteMem->sActionRecorder.fScaleXRecord;
		_act->uAction.sScale.fYInitScale = _PrSpriteMem->sActionRecorder.fScaleYRecord;
		_act->uAction.sScale.fXScale = _XScale;
		_act->uAction.sScale.fYScale = _YScale;
		if (!_Reverse)
		{
			_PrSpriteMem->sActionRecorder.fScaleXRecord = _XScale;
			_PrSpriteMem->sActionRecorder.fScaleYRecord = _YScale;
		}
		_act->uAction.sScale.bReverse = _Reverse;
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
			if (_PrSpriteMem->sActionRecorder.bParallelEdit && _lastact->bParallel)
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
}
BLBool
blSpriteActionAlpha(IN BLGuid _ID, IN BLF32 _Alpha, IN BLBool _Reverse, IN BLF32 _Time, IN BLBool _Loop)
{
    if (_ID == INVALID_GUID)
        return FALSE;
    _BLSpriteNode* _node = (_BLSpriteNode*)blGuidAsPointer(_ID);
    if (!_node)
        return FALSE;
	if (blScalarApproximate(_Time, 0.f))
	{
		_node->fAlpha = _Alpha;
		_PrSpriteMem->sActionRecorder.fAlphaRecord = _Alpha;
		return FALSE;
	}
	else
	{
		_BLSpriteAction* _act = (_BLSpriteAction*)malloc(sizeof(_BLSpriteAction));
		_act->pNeighbor = NULL;
		_act->pNext = NULL;
		_act->bLoop = _Loop;
		_act->bParallel = _PrSpriteMem->sActionRecorder.bParallelEdit;
		_act->eActionType = SPACTION_ALPHA_INTERNAL;
		_act->fCurTime = 0.f;
		_act->fTotalTime = _Time;
		_act->uAction.sAlpha.fInitAlpha = _PrSpriteMem->sActionRecorder.fAlphaRecord;
		_act->uAction.sAlpha.fAlpha = _Alpha;
		if (!_Reverse)
			_PrSpriteMem->sActionRecorder.fAlphaRecord = _Alpha;
		_act->uAction.sAlpha.bReverse = _Reverse;
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
			if (_PrSpriteMem->sActionRecorder.bParallelEdit && _lastact->bParallel)
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
}
BLVoid
blViewportQuery(OUT BLF32* _LTPosX, OUT BLF32* _LTPosY, OUT BLF32* _RBPosX, OUT BLF32* _RBPosY)
{
    *_LTPosX = _PrSpriteMem->sViewport.sLT.fX;
    *_LTPosY = _PrSpriteMem->sViewport.sLT.fY;
    *_RBPosX = _PrSpriteMem->sViewport.sRB.fX;
    *_RBPosY = _PrSpriteMem->sViewport.sRB.fY;
}
BLVoid
blViewportMoveTo(IN BLF32 _XPos, IN BLF32 _YPos)
{
	BLF32 _x = (_PrSpriteMem->sViewport.sLT.fX + _PrSpriteMem->sViewport.sRB.fX) * 0.5f;
	BLF32 _y = (_PrSpriteMem->sViewport.sLT.fY + _PrSpriteMem->sViewport.sRB.fY) * 0.5f;
	_PrSpriteMem->sViewport.sLT.fX += _XPos - _x;
	_PrSpriteMem->sViewport.sLT.fY += _YPos - _y;
	_PrSpriteMem->sViewport.sRB.fX += _XPos - _x;
	_PrSpriteMem->sViewport.sRB.fY += _YPos - _y;
}
BLVoid 
blViewportShake(IN BLF32 _Time, IN BLBool _Vertical, IN BLF32 _Force)
{
    _PrSpriteMem->bShaking = TRUE;
    _PrSpriteMem->fShakingTime = _Time;
    _PrSpriteMem->bShakingVertical = _Vertical;
    _PrSpriteMem->fShakingForce = _Force;
}
