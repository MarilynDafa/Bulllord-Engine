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
#include "spine/spine.h"
#include "spine/extension.h"
#include "system.h"
#include "streamio.h"
#include "util.h"
#include "sprite.h"
#include "gpu.h"
#include "webp/src/webp/decode.h"
#include "astc/astc.h"
_SP_ARRAY_DECLARE_TYPE(spColorArray, BLU32)
_SP_ARRAY_IMPLEMENT_TYPE(spColorArray, BLU32)
typedef struct _TexCacheData {
	BLU8* pData;
	BLEnum eFormat;
	BLU32 nWidth;
	BLU32 nHeight;
	BLBool bRepeatWrap;
	BLBool bSmooth;
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
	spSkeletonClipping* pClipping;
	spAtlas* pAtlas;
	BLF32* pWorldVertices;
	BLF32* pVertexArray;
	BLU32 nVertexArrayNum;
	BLGuid nTexture;
	BLU32 nTexW;
	BLU32 nTexH;
	BLF32 fMinX;
	BLF32 fMinY;
	BLF32 fMaxX;
	BLF32 fMaxY;
} _BLSpineDataExt;
typedef struct _SpineMember {
	BLGuid nTech;
	spBlendMode eBlendMode;
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
	BLGuid _stream = blStreamGen(_source);
	if (INVALID_GUID == _stream)
		return;
	BLBool _texsupport[BL_TF_COUNT];
	blGpuCapsQuery(NULL, NULL, NULL, NULL, NULL, _texsupport);
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
		blStreamDelete(_stream);
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
	case(('M' << 0) + ('O' << 8) + ('N' << 16) + ('O' << 24)):
		_imagesz = _width * _height;
		break;
	case (('B' << 0) + ('M' << 8) + ('G' << 16) + ('T' << 24)):
		blStreamRead(_stream, sizeof(BLU32), &_imagesz);
		_format = (_channels == 4) ? BL_TF_RGBA8 : BL_TF_RGB8;
		break;
	case (('A' << 0) + ('S' << 8) + ('T' << 16) + ('C' << 24)):
		if (_texsupport[BL_TF_ASTC] == 1)
		{
			_imagesz = ((_width + 3) / 4) * ((_height + 3) / 4) * 16;
			_format = BL_TF_ASTC;
		}
		else
		{
			_imagesz = _width * _height * _channels;
			_format = (_channels == 4) ? BL_TF_RGBA8 : BL_TF_RGB8;
		}
		break;
	default:assert(0); break;
	}
	if (_fourcc == (('B' << 0) + ('M' << 8) + ('G' << 16) + ('T' << 24)))
	{
		BLU8* _data = (BLU8*)malloc(_imagesz);
		blStreamRead(_stream, _imagesz, _data);
		if (_channels == 4)
			_texdata = WebPDecodeRGBA(_data, _imagesz, (BLS32*)&_width, (BLS32*)&_height);
		else
			_texdata = WebPDecodeRGB(_data, _imagesz, (BLS32*)&_width, (BLS32*)&_height);
		free(_data);
	}
	else if (_fourcc == (('M' << 0) + ('O' << 8) + ('N' << 16) + ('O' << 24)))
	{
		BLU8* _data = (BLU8*)malloc(_imagesz);
		blStreamRead(_stream, _imagesz, _data);
		_texdata = _data;
	}
	else
	{
		if (_texsupport[BL_TF_ASTC] == 1)
		{
			BLU8* _data = (BLU8*)malloc(_imagesz);
			blStreamRead(_stream, _imagesz, _data);
			_texdata = _data;
		}
		else if (_texsupport[BL_TF_ASTC] == 2)
		{
			BLU8* _data = (BLU8*)malloc(_imagesz);
			BLU8* _cdata = (BLU8*)malloc(((_width + 3) / 4) * ((_height + 3) / 4) * 16);
			const BLAnsi* _argv[] = { "astcdec", "-d", "in.astc", "out.tga" };
			astcmain(4, _argv, _cdata, _data, _width, _height, _channels);
			free(_cdata);
			_texdata = _data;
		}
		else
			_texdata = NULL;
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
	if (_Self->magFilter == SP_ATLAS_LINEAR)
		_tcd->bSmooth = TRUE;
	else
		_tcd->bSmooth = FALSE;
	_Self->rendererObject = _tcd;
	_Self->width = _width;
	_Self->height = _height;
	blStreamDelete(_stream);
}
void
_spAtlasPage_disposeTexture(spAtlasPage* _Self) 
{
}
BLAnsi* 
_spUtil_readFile(const BLAnsi* _Filename, BLS32* _Length)
{
	BLGuid _stream = blStreamGen(_Filename);
	if (INVALID_GUID == _stream)
		return NULL;
	*_Length = blStreamLength(_stream);
	BLAnsi* _ret = (BLAnsi*)malloc(*_Length);
	blStreamRead(_stream, *_Length, _ret);
	blStreamDelete(_stream);
	return _ret;
}
static void 
_AnimationCallback(spAnimationState* _State, spEventType _Type, spTrackEntry* _Entry, spEvent* _Event) 
{
	if (!_State->rendererObject)
		return;
	blSysInvokeEvent(BL_ET_SPRITE, 0xFFFFFFFF, _Entry->trackIndex, NULL, ((_BLSpineDataExt*)_State->rendererObject)->nID);
}
static BLVoid
_BlendState(spBlendMode _Mode)
{
	switch (_Mode)
	{
	case SP_BLEND_MODE_NORMAL:
		blGpuBlendState(FALSE, TRUE, BL_BF_SRCALPHA, BL_BF_INVSRCALPHA, BL_BF_SRCALPHA, BL_BF_INVSRCALPHA, BL_BO_ADD, BL_BO_ADD, FALSE);
		break;
	case SP_BLEND_MODE_ADDITIVE:
		blGpuBlendState(FALSE, TRUE, BL_BF_SRCALPHA, BL_BF_ONE, BL_BF_SRCALPHA, BL_BF_ONE, BL_BO_ADD, BL_BO_ADD, FALSE);
		break;
	case SP_BLEND_MODE_MULTIPLY:
		blGpuBlendState(FALSE, TRUE, BL_BF_DESTCOLOR, BL_BF_INVSRCALPHA, BL_BF_DESTCOLOR, BL_BF_INVSRCALPHA, BL_BO_ADD, BL_BO_ADD, FALSE);
		break;
	case SP_BLEND_MODE_SCREEN:
		blGpuBlendState(FALSE, TRUE, BL_BF_ONE, BL_BF_INVSRCCOLOR, BL_BF_ONE, BL_BF_INVSRCCOLOR, BL_BO_ADD, BL_BO_ADD, FALSE);
		break;
	default:
		blGpuBlendState(FALSE, TRUE, BL_BF_ONE, BL_BF_INVSRCALPHA, BL_BF_ONE, BL_BF_INVSRCALPHA, BL_BO_ADD, BL_BO_ADD, FALSE);
		break;
	}
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
	_sd->pClipping = spSkeletonClipping_create();
	_sd->pSkeleton->x = 0.f;
	_sd->pSkeleton->y = 0.f;
	_sd->pVertexArray = (BLF32*)malloc(sizeof(BLF32) * _sd->pSkeleton->bonesCount * 8 * 24);
	_sd->nVertexArrayNum = 0;
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
	_sd->nTexture = blTextureGen(((BLU32)(((BLU64)(_ID)) & 0xFFFFFFFF)), BL_TT_2D, _tcd->eFormat, FALSE, TRUE, FALSE, 1, 1, _tcd->nWidth, _tcd->nHeight, 1, _tcd->pData);
	blTextureFilter(_sd->nTexture, _tcd->bSmooth ? BL_TF_LINEAR : BL_TF_NEAREST, _tcd->bSmooth ? BL_TF_LINEAR : BL_TF_NEAREST, _tcd->bRepeatWrap ? BL_TW_REPEAT : BL_TW_CLAMP, _tcd->bRepeatWrap ? BL_TW_REPEAT : BL_TW_CLAMP, FALSE);
	_sd->nTexW = _tcd->nWidth;
	_sd->nTexH = _tcd->nHeight;
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
	free(_sd->pVertexArray);
	free(_sd->pWorldVertices);
	spAnimationStateData_dispose(_sd->pState->data);
	spSkeletonData_dispose(_sd->pSkeletonData);
	spAnimationState_dispose(_sd->pState);
	spSkeletonClipping_dispose(_sd->pClipping);
	spSkeleton_dispose(_sd->pSkeleton);
	free(_sd);
	*_ExtData = NULL;
	return TRUE;
}
static const BLBool
_SpineRelease(BLGuid _ID, BLVoid** _ExtData)
{
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)*_ExtData;
	blTextureDelete(_sd->nTexture);
	return TRUE;
}
static const BLBool
_SpineDraw(BLU32 _Delta, BLGuid _ID, BLF32 _Mat[6], BLF32 _OffsetX, BLF32 _OffsetY, BLVoid** _ExtData)
{
	_PrSpineMem->eBlendMode = SP_BLEND_MODE_NORMAL;
	_BLSpineDataExt* _sd = (_BLSpineDataExt*)*_ExtData;
	BLF32 _ltx, _lty, _rbx, _rby;
	blSpriteViewportQuery(&_ltx, &_lty, &_rbx, &_rby);
	blTechniqueSampler(_PrSpineMem->nTech, "Texture0", _sd->nTexture, 0);
	_sd->nVertexArrayNum = 0;
	spSkeleton_update(_sd->pSkeleton, (BLF32)_Delta / 1000.f);
	spAnimationState_update(_sd->pState, (BLF32)_Delta / 1000.f);
	spAnimationState_apply(_sd->pState, _sd->pSkeleton);
	spSkeleton_updateWorldTransform(_sd->pSkeleton);
	BLF32 _width, _height, _xpos, _ypos, _zv, _rot, _scalex, _scaley, _alpha;
	BLU32 _clr;
	BLBool _show, _flipx, _flipy;
	blSpriteQuery(_ID, &_width, &_height, &_xpos, &_ypos, &_zv, &_rot, &_scalex, &_scaley, &_alpha, &_clr, &_flipx, &_flipy, &_show);
	if (_sd->pSkeleton->color.a < 1e-6 || !_show || _alpha < 1e-6)
		return FALSE;
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	BLU16 _quadindices[6] = { 0, 1, 2, 2, 3, 0 };
	_sd->pSkeleton->scaleX = _scalex;
	_sd->pSkeleton->scaleY = _scaley;
	_sd->pSkeleton->root->rotation = -_rot;
	spBone_updateWorldTransform(_sd->pSkeleton->root);
	_sd->fMinY = _sd->fMinX = 999999.f;
	_sd->fMaxY = _sd->fMaxX = -999999.f;
	for (BLS32 _idx = 0; _idx < _sd->pSkeleton->slotsCount; ++_idx)
	{
		BLF32* _vertices = _sd->pWorldVertices;
		BLU32 _verticescount = 0;
		BLF32* _uvs = 0;
		BLU16* _indices = 0;
		BLS32 _indicescount = 0;
		spSlot* _slot = _sd->pSkeleton->drawOrder[_idx];
		spAttachment* _attachment = _slot->attachment;
		if (!_attachment)
			continue;
		if (_slot->color.a <= 1e-6 || !_slot->bone->active)
		{
			spSkeletonClipping_clipEnd(_sd->pClipping, _slot);
			continue;
		}
		BLF32 _dyeclr[4];
		blDeColor4F(_clr, _dyeclr);
		if (_attachment->type == SP_ATTACHMENT_REGION)
		{
			spRegionAttachment* _regionattachment = (spRegionAttachment*)_attachment;
			spColor* _attachmentcolor = &_regionattachment->color;
			if (_attachmentcolor->a < 1e-6) {
				spSkeletonClipping_clipEnd(_sd->pClipping, _slot);
				continue;
			}
			spRegionAttachment_computeWorldVertices(_regionattachment, _slot->bone, _vertices, 0, 2);
			_dyeclr[0] = _sd->pSkeleton->color.r * _slot->color.r * _attachmentcolor->r * _dyeclr[0];
			_dyeclr[1] = _sd->pSkeleton->color.g * _slot->color.g * _attachmentcolor->g * _dyeclr[1];
			_dyeclr[2] = _sd->pSkeleton->color.b * _slot->color.b * _attachmentcolor->b * _dyeclr[2];
			_dyeclr[3] = _sd->pSkeleton->color.a * _slot->color.a * _attachmentcolor->a * _dyeclr[3] * _alpha;
			_verticescount = 4;
			_uvs = _regionattachment->uvs;
			_indices = _quadindices;
			_indicescount = 6;
		}
		else if (_attachment->type == SP_ATTACHMENT_MESH)
		{
			spMeshAttachment* _mesh = (spMeshAttachment*)_attachment;
			spColor* _attachmentcolor = &_mesh->color;
			if (_attachmentcolor->a < 1e-6) {
				spSkeletonClipping_clipEnd(_sd->pClipping, _slot);
				continue;
			}
			if (_mesh->super.worldVerticesLength > 1000)
				continue;
			spVertexAttachment_computeWorldVertices(SUPER(_mesh), _slot, 0, _mesh->super.worldVerticesLength, _sd->pWorldVertices, 0, 2);
			_dyeclr[0] = _sd->pSkeleton->color.r * _slot->color.r * _attachmentcolor->r * _dyeclr[0];
			_dyeclr[1] = _sd->pSkeleton->color.g * _slot->color.g * _attachmentcolor->g * _dyeclr[1];
			_dyeclr[2] = _sd->pSkeleton->color.b * _slot->color.b * _attachmentcolor->b * _dyeclr[2];
			_dyeclr[3] = _sd->pSkeleton->color.a * _slot->color.a * _attachmentcolor->a * _dyeclr[3] * _alpha;
			_verticescount = _mesh->super.worldVerticesLength >> 1;
			_uvs = _mesh->uvs;
			_indices = _mesh->triangles;
			_indicescount = _mesh->trianglesCount;
		}
		else if (_attachment->type == SP_ATTACHMENT_CLIPPING) {
			spClippingAttachment* _clip = (spClippingAttachment*)_slot->attachment;
			spSkeletonClipping_clipStart(_sd->pClipping, _slot, _clip);
			continue;
		}
		else continue;
		if (_PrSpineMem->eBlendMode != _slot->data->blendMode && _sd->nVertexArrayNum)
		{
			BLGuid _geo = blGeometryBufferGen(0xFFFFFFFF, BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _sd->pVertexArray, _sd->nVertexArrayNum * sizeof(BLF32), NULL, 0, BL_IF_INVALID);
			blTechniqueDraw(_PrSpineMem->nTech, _geo, 1);
			blGeometryBufferDelete(_geo);
			_sd->nVertexArrayNum = 0;
			_BlendState(_slot->data->blendMode);
			_PrSpineMem->eBlendMode = _slot->data->blendMode;
		}
		if (spSkeletonClipping_isClipping(_sd->pClipping))
		{
			spSkeletonClipping_clipTriangles(_sd->pClipping, _vertices, _verticescount << 1, _indices, _indicescount, _uvs, 2);
			_vertices = _sd->pClipping->clippedVertices->items;
			_verticescount = _sd->pClipping->clippedVertices->size >> 1;
			_uvs = _sd->pClipping->clippedUVs->items;
			_indices = _sd->pClipping->clippedTriangles->items;
			_indicescount = _sd->pClipping->clippedTriangles->size;
		}
		for (BLU32 _j = 0; _j < (BLU32)_indicescount; ++_j)
		{
			BLU32 _index = _indices[_j] << 1;
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _vertices[_index] - _ltx;
			_sd->fMinX = (_sd->fMinX > _vertices[_index]) ? _vertices[_index] : _sd->fMinX;
			_sd->fMaxX = (_sd->fMaxX > _vertices[_index]) ? _sd->fMaxX : _vertices[_index];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _vertices[_index + 1] - _lty;
			_sd->fMinY = (_sd->fMinY > _vertices[_index + 1]) ? _vertices[_index + 1] : _sd->fMinY;
			_sd->fMaxY = (_sd->fMaxY > _vertices[_index + 1]) ? _sd->fMaxY : _vertices[_index + 1];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _dyeclr[0];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _dyeclr[1];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _dyeclr[2];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _dyeclr[3];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _uvs[_index];
			_sd->pVertexArray[_sd->nVertexArrayNum++] = _uvs[_index + 1];
		}
		spSkeletonClipping_clipEnd(_sd->pClipping, _slot);
	}
	_BlendState(_PrSpineMem->eBlendMode);
	BLGuid _geo = blGeometryBufferGen(0xFFFFFFFF, BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _sd->pVertexArray, _sd->nVertexArrayNum * sizeof(BLF32), NULL, 0, BL_IF_INVALID);
	blTechniqueDraw(_PrSpineMem->nTech, _geo, 1);
	blGeometryBufferDelete(_geo);
	spSkeletonClipping_clipEnd2(_sd->pClipping);
	return 1;
}
BLVoid 
blSpineOpenEXT(IN BLAnsi* _Version, ...)
{
	_PrSpineMem = (_BLSpineMemberExt*)malloc(sizeof(_BLSpineMemberExt));
	_PrSpineMem->nTech = blTechniqueGain(blHashString((const BLUtf8*)"shaders/2D.bsl"));
	_PrSpineMem->eBlendMode = SP_BLEND_MODE_NORMAL;
	blSpriteRegistExternal("json", _LoadSpine, _SpineSetup, _UnloadSpine, _SpineRelease, _SpineDraw, NULL);
	blSpriteRegistExternal("skel", _LoadSpine, _SpineSetup, _UnloadSpine, _SpineRelease, _SpineDraw, NULL);
}
BLVoid 
blSpineCloseEXT()
{
	blTechniqueDelete(_PrSpineMem->nTech);
	free(_PrSpineMem);
}
BLVoid
blSpriteAnimationEXT(IN BLGuid _ID, IN BLAnsi* _Animation, IN BLS32 _Track, IN BLBool _Loop)
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
blSpriteTrackEmptyEXT(IN BLGuid _ID, IN BLU32 _TrackIndex)
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
