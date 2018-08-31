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
#include "spineentry.h"
#include "spine.h"
#include "extension.h"
#include "spine.h"
#include "system.h"
#include "streamio.h"
#include "util.h"
#include "sprite.h"
#include "gpu.h"
typedef struct _TexCacheData {
	BLU8* pData;
	BLEnum eFormat;
	BLU32 nWidth;
	BLU32 nHeight;
	BLBool bRepeatWrap;
} _BLTexCacheDataExt;
typedef struct _SpineCacheData {
	BLU32 nType;
	union {
		struct _AniSet {
			BLAnsi aAniName[64];
			BLS32 nTrack;
			BLBool bLoop;
		}sAniSet;
		struct _AniAdd {
			BLAnsi aAniName[64];
			BLS32 nTrack;
			BLBool bLoop;
			BLF32 fDelay;
		}sAniAdd;
		struct _AniAddE {
			BLS32 nTrack;
			BLF32 fDuration;
			BLF32 fDelay;
		}sAniAddE;
		struct _AniMix {
			BLAnsi aFromAniName[64];
			BLAnsi aToAniName[64];
			BLF32 fDuration;
		}sAniMix;
		struct _AniEmpty {
			BLU32 nTrackIndex;
			BLF32 fDuration;
		}sAniEmpty;
		struct _TrackEmpty {
			BLU32 nTrackIndex;
		}sTrackEmpty;
	} uData;
} _BLSpineCacheData;
typedef struct _SpineData {
	BLGuid nID;
	spAnimationState* pState;
	spSkeleton* pSkeleton;
	spSkeletonData* pSkeletonData;
	spAtlas* pAtlas;
	BLF32* pWorldVertices;
	BLGuid nTexture;	
	BLF32 fMinX;
	BLF32 fMinY;
	BLF32 fMaxX;
	BLF32 fMaxY;
} _BLSpineDataExt;
typedef struct _SpineMember {
	BLGuid nTech;
	BLS32 nBlendMode;
} _BLSpineMemberExt;
static _BLSpineMemberExt* _PrSpineMem = NULL;
void 
_spAtlasPage_createTexture(spAtlasPage* _Self, const BLAnsi* _Filename)
{
	BLAnsi _source[260] = { 0 };
	strcpy(_source, _Filename);
	BLU32 _sourcelen = (BLU32)strlen(_Filename);
	for (BLS32 _idx = (BLS32)_sourcelen - 1; _idx >= 0; --_idx)
	{
		if (_source[_idx] == '.')
			break;
		else
			_source[_idx] = 0;
	}
	strcat(_source, "bmg");
	BLGuid _stream = blGenStream(_source);
	if (INVALID_GUID == _stream)
		return;
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
		return;
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
	while (blStreamTell(_stream) < _offset)
	{
		BLS32 _ltx, _lty, _rbx, _rby, _offsetx, _offsety;
		BLU32 _taglen;
		blStreamRead(_stream, sizeof(BLU32), &_taglen);
		BLAnsi _tag[256] = { 0 };
		blStreamRead(_stream, _taglen, _tag);
		blStreamRead(_stream, sizeof(BLU32), &_ltx);
		blStreamRead(_stream, sizeof(BLU32), &_lty);
		blStreamRead(_stream, sizeof(BLU32), &_rbx);
		blStreamRead(_stream, sizeof(BLU32), &_rby);
		blStreamRead(_stream, sizeof(BLU32), &_offsetx);
		blStreamRead(_stream, sizeof(BLU32), &_offsety);
	}
	BLEnum _format = BL_TF_COUNT;
	BLU8* _texdata = NULL;
	blStreamSeek(_stream, _offset);
	BLU32 _imagesz;
	switch (_fourcc)
	{
	case (('B' << 0) + ('M' << 8) + ('G' << 16) + ('T' << 24)):
		blStreamRead(_stream, sizeof(BLU32), &_imagesz);
		_format = (_channels == 4) ? BL_TF_RGBA8 : BL_TF_RGB8;
		break;
	case (('S' << 0) + ('3' << 8) + ('T' << 16) + ('1' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
		_format = BL_TF_BC1;
		break;
	case (('S' << 0) + ('3' << 8) + ('T' << 16) + ('2' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
		_format = BL_TF_BC1A1;
		break;
	case (('S' << 0) + ('3' << 8) + ('T' << 16) + ('3' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
		_format = BL_TF_BC3;
		break;
	case (('A' << 0) + ('S' << 8) + ('T' << 16) + ('1' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
		_format = BL_TF_ASTC;
		break;
	case (('A' << 0) + ('S' << 8) + ('T' << 16) + ('2' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
		_format = BL_TF_ASTC;
		break;
	case (('A' << 0) + ('S' << 8) + ('T' << 16) + ('3' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
		_format = BL_TF_ASTC;
		break;
	case (('E' << 0) + ('T' << 8) + ('C' << 16) + ('1' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
		_format = BL_TF_ETC2;
		break;
	case (('E' << 0) + ('T' << 8) + ('C' << 16) + ('2' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 8;
		_format = BL_TF_ETC2A1;
		break;
	case (('E' << 0) + ('T' << 8) + ('C' << 16) + ('3' << 24)):
		_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
		_format = BL_TF_ETC2A;
		break;
	default:assert(0); break;
	}
	if (_fourcc == (('B' << 0) + ('M' << 8) + ('G' << 16) + ('T' << 24)))
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
	_BLTexCacheDataExt* _tcd = (_BLTexCacheDataExt*)malloc(sizeof(_BLTexCacheDataExt));
	_tcd->eFormat = _format;
	_tcd->nWidth = _width;
	_tcd->nHeight = _height;
	_tcd->pData = _texdata; 
	if (_Self->uWrap == SP_ATLAS_REPEAT && _Self->vWrap == SP_ATLAS_REPEAT)
		_tcd->bRepeatWrap = TRUE;
	else
		_tcd->bRepeatWrap = FALSE;
	_Self->rendererObject = _tcd;
	_Self->width = _width;
	_Self->height = _height;
	blDeleteStream(_stream);
}
void
_spAtlasPage_disposeTexture(spAtlasPage* _Self) 
{
}
BLAnsi* 
_spUtil_readFile(const BLAnsi* _Filename, BLS32* _Length)
{
	BLGuid _stream = blGenStream(_Filename);
	if (INVALID_GUID == _stream)
		return NULL;
	*_Length = blStreamLength(_stream);
	BLAnsi* _ret = (BLAnsi*)malloc(*_Length);
	blStreamRead(_stream, *_Length, _ret);
	blDeleteStream(_stream);
	return _ret;
}
static void 
_AnimationCallback(spAnimationState* _State, spEventType _Type, spTrackEntry* _Entry, spEvent* _Event) 
{
	if (!_State->rendererObject)
		return;
	if (_Type == SP_ANIMATION_COMPLETE && !_Entry->loop)
		blInvokeEvent(BL_ET_SPRITE, 0xFFFFFFFF, 0, NULL, ((_BLSpineDataExt*)_State->rendererObject)->nID);
}
static const BLBool
_LoadSpine(BLGuid _ID, const BLAnsi* _Filename, BLVoid** _ExtData)
{
	BLAnsi _dir[260] = { 0 };
	strcpy(_dir, _Filename);
	BLU32 _tmplen = (BLU32)strlen(_Filename);
	for (BLS32 _idx = _tmplen - 1; _idx >= 0; --_idx)
	{
		if (_dir[_idx] == '.')
			break;
		else
			_dir[_idx] = 0;
	}
	strcat(_dir, "atlas");
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)malloc(sizeof(_BLSpineDataExt));
	_sd->pAtlas = spAtlas_createFromFile(_dir, 0);
	_sd->pWorldVertices = (BLF32*)malloc(1000 * sizeof(BLF32));
	spBone_setYDown(TRUE);
	if (!strcmp((const BLAnsi*)blFileSuffixUtf8((const BLUtf8*)_Filename), "json"))
	{
		spSkeletonJson* _json = spSkeletonJson_create(_sd->pAtlas);
		_json->scale = 1.f;
		_sd->pSkeletonData = spSkeletonJson_readSkeletonDataFile(_json, _Filename);
		if (!_sd->pSkeletonData)
		{
			free(_sd->pWorldVertices);
			free(_sd);
			return FALSE;
		}
		spSkeletonJson_dispose(_json);
	}
	else
	{
		spSkeletonBinary* _binary = spSkeletonBinary_create(_sd->pAtlas);
		_binary->scale = 1.f;
		_sd->pSkeletonData = spSkeletonBinary_readSkeletonDataFile(_binary, _Filename);
		if (!_sd->pSkeletonData)
		{
			free(_sd->pWorldVertices);
			free(_sd);
			return FALSE;
		}
		spSkeletonBinary_dispose(_binary);
	}	
	_sd->pSkeleton = spSkeleton_create(_sd->pSkeletonData);
	_sd->pState = spAnimationState_create(spAnimationStateData_create(_sd->pSkeletonData));
	_sd->pSkeleton->x = 0.f;
	_sd->pSkeleton->y = _sd->pSkeletonData->height * 0.5f;
	blSpriteDimension(_ID, _sd->pSkeletonData->width, _sd->pSkeletonData->height, FALSE);
	spSkeleton_updateWorldTransform(_sd->pSkeleton);
	if (*_ExtData)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)*_ExtData;
		if (_scd->nType == 1)
			spAnimationState_setAnimationByName(_sd->pState, _scd->uData.sAniSet.nTrack, _scd->uData.sAniSet.aAniName, _scd->uData.sAniSet.bLoop);
		else if (_scd->nType == 2)
			spAnimationState_addAnimationByName(_sd->pState, _scd->uData.sAniAdd.nTrack, _scd->uData.sAniAdd.aAniName, _scd->uData.sAniAdd.bLoop, _scd->uData.sAniAdd.fDelay);
		else if (_scd->nType == 3)
			spAnimationState_addEmptyAnimation(_sd->pState, _scd->uData.sAniAddE.nTrack, _scd->uData.sAniAddE.fDuration, _scd->uData.sAniAddE.fDelay);
		else if (_scd->nType == 4)
			spAnimationStateData_setMixByName(_sd->pState->data, _scd->uData.sAniMix.aFromAniName, _scd->uData.sAniMix.aToAniName, _scd->uData.sAniMix.fDuration);
		else if (_scd->nType == 5)
		{
			if (_scd->uData.sAniEmpty.nTrackIndex != 0xFFFFFFFF)
				spAnimationState_setEmptyAnimation(_sd->pState, _scd->uData.sAniEmpty.nTrackIndex, _scd->uData.sAniEmpty.fDuration);
			else
				spAnimationState_setEmptyAnimations(_sd->pState, _scd->uData.sAniEmpty.fDuration);
		}
		else if (_scd->nType == 6)
		{
			if (_scd->uData.sTrackEmpty.nTrackIndex != 0xFFFFFFFF)
				spAnimationState_clearTrack(_sd->pState, _scd->uData.sTrackEmpty.nTrackIndex);
			else
				spAnimationState_clearTracks(_sd->pState);
		}
		free(*_ExtData);
	}	
	*_ExtData = _sd;
	return TRUE;
}
static const BLBool
_SpineSetup(BLGuid _ID, BLVoid** _ExtData)
{
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)*_ExtData;
	_sd->nID = _ID;
	_BLTexCacheDataExt* _tcd = (_BLTexCacheDataExt*)_sd->pAtlas->pages->rendererObject;
	_sd->nTexture = blGenTexture(((BLU32)(((BLU64)(_ID)) & 0xFFFFFFFF)), BL_TT_2D, _tcd->eFormat, FALSE, TRUE, FALSE, 1, 1, _tcd->nWidth, _tcd->nHeight, 1, _tcd->pData);
	blTextureFilter(_sd->nTexture, BL_TF_LINEAR, BL_TF_LINEAR, _tcd->bRepeatWrap ? BL_TW_REPEAT : BL_TW_CLAMP, _tcd->bRepeatWrap ? BL_TW_REPEAT : BL_TW_CLAMP, FALSE);
	free(_tcd);
	_sd->pAtlas->pages->rendererObject = NULL;
	_sd->pState->listener = _AnimationCallback;
	_sd->pState->rendererObject = _sd;
	return TRUE;
}
static const BLBool
_UnloadSpine(BLGuid _ID, BLVoid** _ExtData)
{
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)*_ExtData;
	free(_sd->pWorldVertices);
	spAnimationStateData_dispose(_sd->pState->data);
	spSkeletonData_dispose(_sd->pSkeletonData);
	spAnimationState_dispose(_sd->pState);
	spSkeleton_dispose(_sd->pSkeleton);
	free(_sd);
	*_ExtData = NULL;
	return TRUE;
}
static const BLBool
_SpineRelease(BLGuid _ID, BLVoid** _ExtData)
{
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)*_ExtData;
	blDeleteTexture(_sd->nTexture);
	return TRUE;
}
static const BLVoid
_SpineDraw(BLU32 _Delta, BLGuid _ID, BLF32 _Mat[6], BLF32 _OffsetX, BLF32 _OffsetY, BLVoid** _ExtData)
{
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)*_ExtData;
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	spSkeleton_update(_sd->pSkeleton, (BLF32)_Delta / 1000.f);
	spAnimationState_update(_sd->pState, (BLF32)_Delta / 1000.f);
	spAnimationState_apply(_sd->pState, _sd->pSkeleton);
	spSkeleton_updateWorldTransform(_sd->pSkeleton);
	BLU32 _gbcap = _sd->pSkeletonData->bonesCount * 32;
	BLF32* _gbdata = (BLF32*)malloc(_gbcap * sizeof(BLF32));
	BLU32 _gbi = 0;
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	_sd->pSkeleton->flipX = _flipx;
	_sd->pSkeleton->flipY = _flipy;
	_sd->fMinY = _sd->fMinX = 999999.f;
	_sd->fMaxY = _sd->fMaxX = -999999.f;
	blTechSampler(_PrSpineMem->nTech, "Texture0", _sd->nTexture, 0);
	for (BLS32 _idx = 0; _idx < _sd->pSkeleton->slotsCount; ++_idx)
	{
		spSlot* _slot = _sd->pSkeleton->drawOrder[_idx];
		spAttachment* _attachment = _slot->attachment;
		if (!_attachment)
			continue;
		BLU8 _blendfactor[4] = { 0 };
		BLS32 _mode;
		switch (_slot->data->blendMode) 
		{
		case SP_BLEND_MODE_ADDITIVE:
			_mode = SP_BLEND_MODE_ADDITIVE;
			blBlendState(FALSE, TRUE, BL_BF_SRCALPHA, BL_BF_ONE, BL_BF_ONE, BL_BF_ONE, BL_BO_ADD, BL_BO_ADD, _blendfactor, FALSE);
			break;
		case SP_BLEND_MODE_MULTIPLY:
			_mode = SP_BLEND_MODE_MULTIPLY;
			blBlendState(FALSE, TRUE, BL_BF_DESTCOLOR, BL_BF_ZERO, BL_BF_DESTCOLOR, BL_BF_ZERO, BL_BO_ADD, BL_BO_ADD, _blendfactor, FALSE);
			break;
		case SP_BLEND_MODE_SCREEN:
			_mode = SP_BLEND_MODE_SCREEN;
			blBlendState(FALSE, TRUE, BL_BF_SRCALPHA, BL_BF_INVSRCALPHA, BL_BF_INVSRCALPHA, BL_BF_ONE, BL_BO_ADD, BL_BO_ADD, _blendfactor, FALSE);
			break;
		default:
			_mode = 0;
			blBlendState(FALSE, TRUE, BL_BF_SRCALPHA, BL_BF_INVSRCALPHA, BL_BF_INVDESTALPHA, BL_BF_ONE, BL_BO_ADD, BL_BO_ADD, _blendfactor, FALSE);
		}
		if (_PrSpineMem->nBlendMode != _mode)
		{
			BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _gbdata, _gbi * sizeof(BLF32), NULL, 0, BL_IF_INVALID);
			blDraw(_PrSpineMem->nTech, _geo, 1);
			blDeleteGeometryBuffer(_geo);
			_gbcap = _sd->pSkeletonData->bonesCount * 32;
			_gbi = 0;
			_PrSpineMem->nBlendMode = _mode;
		}
		BLF32 _dyeclr[4];
		blDeColor4F(_clr, _dyeclr);
		if (_attachment->type == SP_ATTACHMENT_REGION)
		{
			spRegionAttachment* _regionattachment = (spRegionAttachment*)_attachment;
			spRegionAttachment_computeWorldVertices(_regionattachment, _slot->bone, _sd->pWorldVertices);
			BLF32 _r = _sd->pSkeleton->r * _slot->r * _dyeclr[0];
			BLF32 _g = _sd->pSkeleton->g * _slot->g * _dyeclr[1];
			BLF32 _b = _sd->pSkeleton->b * _slot->b * _dyeclr[2];
			BLF32 _a = _sd->pSkeleton->a * _slot->a * _dyeclr[3] * _alpha;
			if (_gbi + 50 > _gbcap)
			{
				_gbcap *= 2;
				_gbdata = (BLF32*)realloc(_gbdata, _gbcap * sizeof(BLF32));
			}
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X1] * _Mat[0]) + (_sd->pWorldVertices[SP_VERTEX_Y1] * _Mat[2]) + _Mat[4] + _OffsetX;
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X1] * _Mat[1]) + (_sd->pWorldVertices[SP_VERTEX_Y1] * _Mat[3]) + _Mat[5] + _OffsetY;
			_gbdata[_gbi++] = _r;
			_gbdata[_gbi++] = _g;
			_gbdata[_gbi++] = _b;
			_gbdata[_gbi++] = _a;
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_X1];
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_Y1];
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X2] * _Mat[0]) + (_sd->pWorldVertices[SP_VERTEX_Y2] * _Mat[2]) + _Mat[4] + _OffsetX;
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X2] * _Mat[1]) + (_sd->pWorldVertices[SP_VERTEX_Y2] * _Mat[3]) + _Mat[5] + _OffsetY;
			_gbdata[_gbi++] = _r;
			_gbdata[_gbi++] = _g;
			_gbdata[_gbi++] = _b;
			_gbdata[_gbi++] = _a;
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_X2];
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_Y2];
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X3] * _Mat[0]) + (_sd->pWorldVertices[SP_VERTEX_Y3] * _Mat[2]) + _Mat[4] + _OffsetX;
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X3] * _Mat[1]) + (_sd->pWorldVertices[SP_VERTEX_Y3] * _Mat[3]) + _Mat[5] + _OffsetY;
			_gbdata[_gbi++] = _r;
			_gbdata[_gbi++] = _g;
			_gbdata[_gbi++] = _b;
			_gbdata[_gbi++] = _a;
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_X3];
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_Y3];
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X1] * _Mat[0]) + (_sd->pWorldVertices[SP_VERTEX_Y1] * _Mat[2]) + _Mat[4] + _OffsetX;
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X1] * _Mat[1]) + (_sd->pWorldVertices[SP_VERTEX_Y1] * _Mat[3]) + _Mat[5] + _OffsetY;
			_gbdata[_gbi++] = _r;
			_gbdata[_gbi++] = _g;
			_gbdata[_gbi++] = _b;
			_gbdata[_gbi++] = _a;
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_X1];
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_Y1];
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X3] * _Mat[0]) + (_sd->pWorldVertices[SP_VERTEX_Y3] * _Mat[2]) + _Mat[4] + _OffsetX;
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X3] * _Mat[1]) + (_sd->pWorldVertices[SP_VERTEX_Y3] * _Mat[3]) + _Mat[5] + _OffsetY;
			_gbdata[_gbi++] = _r;
			_gbdata[_gbi++] = _g;
			_gbdata[_gbi++] = _b;
			_gbdata[_gbi++] = _a;
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_X3];
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_Y3];
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X4] * _Mat[0]) + (_sd->pWorldVertices[SP_VERTEX_Y4] * _Mat[2]) + _Mat[4] + _OffsetX;
			_gbdata[_gbi++] = (_sd->pWorldVertices[SP_VERTEX_X4] * _Mat[1]) + (_sd->pWorldVertices[SP_VERTEX_Y4] * _Mat[3]) + _Mat[5] + _OffsetY;
			_gbdata[_gbi++] = _r;
			_gbdata[_gbi++] = _g;
			_gbdata[_gbi++] = _b;
			_gbdata[_gbi++] = _a;
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_X4];
			_gbdata[_gbi++] = _regionattachment->uvs[SP_VERTEX_Y4];
		}
		else if (_attachment->type == SP_ATTACHMENT_MESH)
		{
			spMeshAttachment* _mesh = (spMeshAttachment*)_attachment;
			if (_mesh->super.worldVerticesLength > 1000)
				continue;
			spMeshAttachment_computeWorldVertices(_mesh, _slot, _sd->pWorldVertices);
			BLF32 _r = _sd->pSkeleton->r * _slot->r * _dyeclr[0];
			BLF32 _g = _sd->pSkeleton->g * _slot->g * _dyeclr[1];
			BLF32 _b = _sd->pSkeleton->b * _slot->b * _dyeclr[2];
			BLF32 _a = _sd->pSkeleton->a * _slot->a * _dyeclr[3] * _alpha;
			if (_gbi + _mesh->trianglesCount * 8 + 4 > _gbcap)
			{
				_gbcap *= 2;
				_gbdata = (BLF32*)realloc(_gbdata, _gbcap * sizeof(BLF32));
			}
			for (BLS32 _jdx = 0; _jdx < _mesh->trianglesCount; ++_jdx)
			{
				BLS32 _index = _mesh->triangles[_jdx] << 1;
				_gbdata[_gbi++] = (_sd->pWorldVertices[_index] * _Mat[0]) + (_sd->pWorldVertices[_index + 1] * _Mat[2]) + _Mat[4] + _OffsetX;
                _sd->fMinX = (_sd->fMinX > _gbdata[_gbi - 1]) ? _gbdata[_gbi - 1] : _sd->fMinX;
                _sd->fMaxX = (_sd->fMaxX > _gbdata[_gbi - 1]) ? _sd->fMaxX : _gbdata[_gbi - 1];
				_gbdata[_gbi++] = (_sd->pWorldVertices[_index] * _Mat[1]) + (_sd->pWorldVertices[_index + 1] * _Mat[3]) + _Mat[5] + _OffsetY;
                _sd->fMinY = (_sd->fMinY > _gbdata[_gbi - 1]) ? _gbdata[_gbi - 1] : _sd->fMinY;
                _sd->fMaxY = (_sd->fMaxY > _gbdata[_gbi - 1]) ? _sd->fMaxY : _gbdata[_gbi - 1];
				_gbdata[_gbi++] = _r;
				_gbdata[_gbi++] = _g;
				_gbdata[_gbi++] = _b;
				_gbdata[_gbi++] = _a;
				_gbdata[_gbi++] = _mesh->uvs[_index];
				_gbdata[_gbi++] = _mesh->uvs[_index + 1];
			}
		}
	}
	BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _gbdata, _gbi * sizeof(BLF32), NULL, 0, BL_IF_INVALID);
	blDraw(_PrSpineMem->nTech, _geo, 1);
	blDeleteGeometryBuffer(_geo);
	free(_gbdata);
}
BLVoid 
blSpineOpenEXT(IN BLAnsi* _Version, ...)
{
	_PrSpineMem = (_BLSpineMemberExt*)malloc(sizeof(_BLSpineMemberExt));
	_PrSpineMem->nTech = blGainTechnique(blHashString((const BLUtf8*)"shaders/2D.bsl"));
	_PrSpineMem->nBlendMode = 0;
	blRegistExternalMethod("json", _LoadSpine, _SpineSetup, _UnloadSpine, _SpineRelease, _SpineDraw);
	blRegistExternalMethod("skel", _LoadSpine, _SpineSetup, _UnloadSpine, _SpineRelease, _SpineDraw);
}
BLVoid 
blSpineCloseEXT()
{
	blDeleteTechnique(_PrSpineMem->nTech);
	free(_PrSpineMem);
}
BLVoid
blSpriteAnimationSetEXT(IN BLGuid _ID, IN BLAnsi* _Animation, IN BLS32 _Track, IN BLBool _Loop)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)malloc(sizeof(_BLSpineCacheData));
		_scd->nType = 1;		
		memset(_scd->uData.sAniSet.aAniName, 0, sizeof(_scd->uData.sAniSet.aAniName));
		strcpy(_scd->uData.sAniSet.aAniName, _Animation);
		_scd->uData.sAniSet.bLoop = _Loop;
		_scd->uData.sAniSet.nTrack = _Track;
		blSpriteExternalData(_ID, _scd);
	}
	else
		spAnimationState_setAnimationByName(_sd->pState, _Track, _Animation, _Loop);
}
BLVoid 
blSpriteAnimationAddEXT(IN BLGuid _ID, IN BLAnsi* _Animation, IN BLS32 _Track, IN BLBool _Loop, IN BLF32 _Delay)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)malloc(sizeof(_BLSpineCacheData));
		_scd->nType = 2;
		memset(_scd->uData.sAniAdd.aAniName, 0, sizeof(_scd->uData.sAniAdd.aAniName));
		strcpy(_scd->uData.sAniAdd.aAniName, _Animation);
		_scd->uData.sAniAdd.bLoop = _Loop;
		_scd->uData.sAniAdd.nTrack = _Track;
		_scd->uData.sAniAdd.fDelay = _Delay;
		blSpriteExternalData(_ID, _scd);
	}
	else
		spAnimationState_addAnimationByName(_sd->pState, _Track, _Animation, _Loop, _Delay);
}
BLVoid
blSpriteAnimationAddEmptyEXT(IN BLGuid _ID, IN BLS32 _Track, IN BLF32 _Duration, IN BLF32 _Delay)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)malloc(sizeof(_BLSpineCacheData));
		_scd->nType = 3;
		_scd->uData.sAniAddE.nTrack = _Track;
		_scd->uData.sAniAddE.fDuration = _Duration;
		_scd->uData.sAniAddE.fDelay = _Delay;
		blSpriteExternalData(_ID, _scd);
	}
	else
		spAnimationState_addEmptyAnimation(_sd->pState, _Track, _Duration, _Delay);
}
BLVoid 
blSpriteAnimationMixEXT(IN BLGuid _ID, IN BLAnsi* _From, IN BLAnsi* _To, IN BLF32 _Duration)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)malloc(sizeof(_BLSpineCacheData));
		_scd->nType = 4;
		memset(_scd->uData.sAniMix.aFromAniName, 0, sizeof(_scd->uData.sAniMix.aFromAniName));
		strcpy(_scd->uData.sAniMix.aFromAniName, _From);
		memset(_scd->uData.sAniMix.aToAniName, 0, sizeof(_scd->uData.sAniMix.aToAniName));
		strcpy(_scd->uData.sAniMix.aToAniName, _To);
		_scd->uData.sAniMix.fDuration = _Duration;
		blSpriteExternalData(_ID, _scd);
	}
	else
		spAnimationStateData_setMixByName(_sd->pState->data, _From, _To, _Duration);
}
BLVoid 
blSpriteAnimationEmptyEXT(IN BLGuid _ID, IN BLU32 _TrackIndex, IN BLF32 _Duration)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)malloc(sizeof(_BLSpineCacheData));
		_scd->nType = 5;
		_scd->uData.sAniEmpty.nTrackIndex = _TrackIndex;
		_scd->uData.sAniEmpty.fDuration = _Duration;
		blSpriteExternalData(_ID, _scd);
	}
	else
	{
		if (_TrackIndex == 0xFFFFFFFF)
			spAnimationState_setEmptyAnimations(_sd->pState, _Duration);
		else
			spAnimationState_setEmptyAnimation(_sd->pState, _TrackIndex, _Duration);
	}
}
BLVoid 
blSpriteTrackEmpty(IN BLGuid _ID, IN BLU32 _TrackIndex)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
	{
		_BLSpineCacheData* _scd = (_BLSpineCacheData*)malloc(sizeof(_BLSpineCacheData));
		_scd->nType = 6;
		_scd->uData.sTrackEmpty.nTrackIndex = _TrackIndex;
		blSpriteExternalData(_ID, _scd);
	}
	else
	{
		if (_TrackIndex == 0xFFFFFFFF)
			spAnimationState_clearTracks(_sd->pState);
		else
			spAnimationState_clearTrack(_sd->pState, _TrackIndex);
	}
}
BLBool 
blSpriteBoundingBoxEXT(IN BLGuid _ID, OUT BLU32* _Width, OUT BLU32* _Height)
{
	_BLSpineDataExt* _sd = blSpriteExternalData(_ID, NULL);
	if (!_sd)
		return FALSE;
	*_Width = (BLU32)(_sd->fMaxX - _sd->fMinX);
	*_Height = (BLU32)(_sd->fMaxY - _sd->fMinY);
	return TRUE;
}
