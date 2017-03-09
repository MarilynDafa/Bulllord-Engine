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
#include "../headers/ui.h"
#include "../headers/system.h"
#include "../headers/streamio.h"
#include "../headers/utils.h"
#include "../headers/gpu.h"
#include "internal/mathematics.h"
#include "internal/array.h"
#include "internal/dictionary.h"
#include "internal/internal.h"
#include "../externals/xml/ezxml.h"
#include "ft2build.h"
#include FT_FREETYPE_H
typedef struct _Glyph{
	BLGuid nTexture;
	BLRect sRect;
	BLVec2 sOffset;
	BLU32 nAdv;
	BLBool bValid;
}_BLGlyph;
typedef struct _GlyphAtlas{
	BLDictionary* pGlyphs;
	BLArray* pTextures;
	BLGuid nCurTexture;
	BLU32 nRecordX;
	BLU32 nRecordY;
	BLU32 nYB;
}_BLGlyphAtlas;
typedef struct _Font {
	BLU32 nHashName;
	BLDictionary* pGlyphAtlas;
	BLGuid nFontStream;
	FT_Face sFtFace;
}_BLFont;
typedef struct _Widget {
	BLGuid nID;
	BLEnum eType;
	struct _Widget* pParent;
	BLArray* pChildren;
	BLVec2 sPosition;
	BLVec2 sDimension;
	BLEnum eReferenceH;
	BLEnum eReferenceV;
	BLEnum ePolicy;
	BLF32 fRatio;
	BLU32 nMaxWidth;
	BLU32 nMaxHeight;
	BLU32 nMinWidth;
	BLU32 nMinHeight;
	BLUtf8* pTooltip;
	BLBool bVisible;
	BLBool bCliped;
	BLBool bPenetration;
	BLBool bAbsScissor;
	BLBool bInteractive;
    BLBool bValid;
	union {
		struct _Panel {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sStenciTex;
			BLBool bDragable;
			BLBool bBasePlate;
			BLBool bModal;
			BLBool bScrollable;
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sPanel;
		struct _Button {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aHoveredMap[128];
			BLAnsi aPressedMap[128];
			BLAnsi aDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sHoveredTex9;
			BLRect sHoveredTex;
			BLRect sPressedTex9;
			BLRect sPressedTex;
			BLRect sDisableTex9;
			BLRect sDisableTex;
			BLRect sStencilTex;
			BLAnsi aFontSource[32];
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLU32 nTxtColor;
			BLEnum eTxtAlignmentH;
			BLEnum eTxtAlignmentV;
			BLBool bFlipX;
			BLBool bFlipY;
            BLU32 nState;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sButton;
		struct _Label {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sStencilTex;
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sLabel;
		struct _Check {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aPressedMap[128];
			BLAnsi aDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sPressedTex9;
			BLRect sPressedTex;
			BLRect sDisableTex9;
			BLRect sDisableTex;
			BLRect sStencilTex;
			BLU32 nFontHash;
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLU32 nTxtColor;
			BLEnum eTxtAlignmentH;
			BLEnum eTxtAlignmentV;
			BLBool bFlipX;
			BLBool bFlipY;
            BLU32 nState;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sCheck;
		struct _Text {
			BLUtf8* pText;
			BLUtf8* pPlaceholder;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sStencilTex;
			BLU32 nFontHash;
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLU32 nTxtColor;
			BLU32 nMaxLength;
			BLS32 nMinValue;
			BLS32 nMaxValue;
			BLBool bSelecting;
			BLBool bAutoscroll;
			BLBool bWordwrap;
			BLBool bMultiline;
			BLBool bPassword;
			BLBool bNumeric;
			BLEnum eTxtAlignmentH;
			BLEnum eTxtAlignmentV;
			BLS32 nScrollPosH;
			BLS32 nScrollPosV;
			BLS32 nCaretPos;
			BLS32 nSelectBegin;
			BLS32 nSelectEnd;
			BLF32 nPaddingX;
			BLF32 nPaddingY;
			BLBool bFlipX;
			BLBool bFlipY;
            BLU32 nState;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sText;
		struct _Progress {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLAnsi aFillMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sFillTex;
			BLRect sStencilTex;
			BLU32 nFontHash;
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLU32 nTxtColor;
			BLU32 nPercent;
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sProgress;
		struct _Slider {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aSliderCommonMap[128];
			BLAnsi aSliderDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sSliderCommonTex;
			BLRect sSliderDisableTex;
			BLRect sStencilTex;
			BLBool bHorizontal;
			BLBool bSliderDragged;
			BLS32 nMinValue;
			BLS32 nMaxValue;
			BLS32 nStep;
			BLU32 nSliderPosition;
			BLVec2 sSliderSize;
			BLBool bFlipX;
			BLBool bFlipY;
            BLU32 nState;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sSlider;
		struct _Table {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLAnsi aOddItemMap[128];
			BLAnsi aEvenItemMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sOddItemTex;
			BLRect sEvenItemTex;
			BLRect sStencilTex;
			BLU32 nFontHash;
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLU32 nTxtColor;
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sTable;
		struct _Dial {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex;
			BLRect sStencilTex;
			BLS32 nStartAngle;
			BLS32 nEndAngle;
			BLBool bAngleCut;
			BLBool bClockWise;
            BLGuid nPixmapTex;
			BLU32 nTexWidth;
			BLU32 nTexHeight;
			BLEnum eTexFormat;
			BLU8* pTexData;
		}sDial;
		struct _Primitive {
			BLBool bFill;
			BLBool bClosed;
			BLU32 nColor;
			BLF32* pXPath;
			BLF32* pYPath;
			BLU32 nPathNum;
		}sPrimitive;
	} uExtension;
}_BLWidget;
typedef struct _UIMember {
	_BLWidget* pRoot;
	_BLWidget* pBasePlate;
	_BLWidget* pHoveredWidget;
	_BLWidget* pFocusWidget;
	BLArray* pFonts;
	BLAnsi aDir[260];
	BLAnsi aArchive[260];
	FT_Library sFtLibrary;
	BLGuid nUITech;
	BLGuid nBlankTex;
	BLU32 nFboWidth;
	BLU32 nFboHeight;
	BLU32 nCaretColor;
	BLU32 nSelectRangeColor;
	BLU32 nTextDisableColor;
	BLBool bDirty;
}_BLUIMember;
static _BLUIMember* _PrUIMem = NULL;
extern BLBool _FetchResource(const BLAnsi*, const BLAnsi*, BLVoid**, BLGuid, BLBool(*)(BLVoid*, const BLAnsi*, const BLAnsi*), BLBool(*)(BLVoid*), BLBool);
extern BLBool _DiscardResource(BLGuid, BLBool(*)(BLVoid*), BLBool(*)(BLVoid*));
static BLBool
_RectApproximate(BLRect* _R1, BLRect* _R2)
{
	if (!blScalarApproximate(_R1->sLT.fX, _R2->sLT.fX))
		return FALSE;
	if (!blScalarApproximate(_R1->sLT.fY, _R2->sLT.fY))
		return FALSE;
	if (!blScalarApproximate(_R1->sRB.fX, _R2->sRB.fX))
		return FALSE;
	if (!blScalarApproximate(_R1->sRB.fY, _R2->sRB.fY))
		return FALSE;
	return TRUE;
}
static _BLFont*
_FontFace(const BLAnsi* _Filename, const BLAnsi* _Archive)
{
	BLGuid _font;
	if (_Archive)
		_font = blGenStream(_Filename, _Archive);
	else
	{
		BLAnsi _tmpname[260];
		strcpy(_tmpname, _Filename);
		BLAnsi _path[260] = { 0 };
		strcpy(_path, blWorkingDir(TRUE));
		strcat(_path, _tmpname);
		_font = blGenStream(_path, NULL);
		if (INVALID_GUID == _font)
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			_font = blGenStream(_path, NULL);
		}
		if (INVALID_GUID == _font)
			return FALSE;
	}
	_BLFont* _ret = (_BLFont*)malloc(sizeof(_BLFont));
	if (FT_New_Memory_Face(_PrUIMem->sFtLibrary, (FT_Bytes)blStreamData(_font), blStreamLength(_font), 0, &_ret->sFtFace))
	{
		free(_ret);
		blDeleteStream(_font);
		return NULL;
	}
	FT_Select_Charmap(_ret->sFtFace, FT_ENCODING_UNICODE);
	if (!_ret->sFtFace->charmap)
	{
		FT_Done_Face(_ret->sFtFace);
		_ret->sFtFace = 0;
		free(_ret);
		blDeleteStream(_font);
		return NULL;
	}
	BLU32 _endi = 0, _starti = 0;
	BLS32 _filelen = (BLS32)strlen(_Filename);
	for (BLS32 _idx = _filelen; _idx > 0; --_idx)
	{
		if (_Filename[_idx] == '.')
			_endi = _idx - 1;
		if (_Filename[_idx] == '/' || _Filename[_idx] == '\\')
		{
			_starti = _idx + 1;
			break;
		}
	}
	BLAnsi _basename[64] = { 0 };
	strncpy(_basename, _Filename + _starti, _endi - _starti + 1);
	_ret->nHashName = blHashUtf8((const BLUtf8*)_basename);
	_ret->pGlyphAtlas = blGenDict(FALSE);
	_ret->nFontStream = _font;
	return _ret;
}
static _BLGlyph*
_QueryGlyph(_BLFont* _Font, BLU32 _Char, BLU32 _FontHeight)
{
	_BLGlyphAtlas* _glyphatlas = blDictElement(_Font->pGlyphAtlas, _FontHeight);
	if (!_glyphatlas)
		return NULL;
	_BLGlyph* _glyph = blDictElement(_glyphatlas->pGlyphs, _Char);
	if (!_glyph)
	{
		_glyph = (_BLGlyph*)malloc(sizeof(_BLGlyph));
		memset(_glyph, 0, sizeof(_BLGlyph));
		_glyph->nAdv = 0;
		_glyph->bValid = FALSE;
		blDictInsert(_glyphatlas->pGlyphs, _Char, _glyph);
		if (_glyphatlas->nCurTexture == INVALID_GUID)
		{
			_glyphatlas->nCurTexture = blGenTexture(blUniqueUri(), BL_TT_2D, BL_TF_RG8, FALSE, FALSE, FALSE, 1, 1, 256, 256, 1, NULL);
			blTextureFilter(_glyphatlas->nCurTexture, BL_TF_LINEAR, BL_TF_LINEAR, BL_TW_CLAMP, BL_TW_CLAMP, FALSE);
			blTextureSwizzle(_glyphatlas->nCurTexture, BL_TS_RED, BL_TS_RED, BL_TS_RED, BL_TS_GREEN);
			BLGuid* _curt = (BLGuid*)malloc(sizeof(BLGuid));
			*_curt = _glyphatlas->nCurTexture;
			blArrayPushBack(_glyphatlas->pTextures, _curt);
			_glyphatlas->nRecordX = 4;
			_glyphatlas->nRecordY = 4;
			_glyphatlas->nYB = 4;
		}
		BLBool _outline = (LOWU16(_Char) & 0x000F) > 0;
		BLBool _bold = (LOWU16(_Char) & 0x00F0) > 0;
		BLBool _shadow = (LOWU16(_Char) & 0x0F00) > 0;
		BLBool _italics = (LOWU16(_Char) & 0xF000) > 0;
		BLUtf16 _cc = HIGU16(_Char);
		if (_italics)
		{
			FT_Matrix _matrix;
			_matrix.xx = 0x10000L >> 6;
			_matrix.xy = (FT_Fixed)(0.15f * 0x10000L);
			_matrix.yx = 0;
			_matrix.yy = 0x10000L;
			FT_Set_Transform(_Font->sFtFace, &_matrix, 0);
		}
		else
		{
			FT_Matrix _matrix;
			_matrix.xx = 0x10000L >> 6;
			_matrix.xy = 0;
			_matrix.yx = 0;
			_matrix.yy = 0x10000L;
			FT_Set_Transform(_Font->sFtFace, &_matrix, 0);
		}
		if (FT_Load_Char(_Font->sFtFace, _cc, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT))
		{
			free(_glyph);
			return NULL;
		}
		_glyph->nAdv = (BLU32)_Font->sFtFace->glyph->advance.x >> 6;
		BLU32 _glyphw = _Font->sFtFace->glyph->bitmap.width;
		BLU32 _glyphh = _Font->sFtFace->glyph->bitmap.rows;
		_glyphw += 4;
		_glyphh += 4;
		BLU32 _xnext = _glyphatlas->nRecordX + _glyphw;
		if (_xnext > 256)
		{
			_glyphatlas->nRecordX = 4;
			_xnext = _glyphatlas->nRecordX + _glyphw;
			_glyphatlas->nRecordY = _glyphatlas->nYB;
		}
		BLU32 _ybot = _glyphatlas->nRecordY + _glyphh + (_shadow ? 1 : 0);
		while (_ybot > 256)
		{
			_glyphatlas->nCurTexture = blGenTexture(blUniqueUri(), BL_TT_2D, BL_TF_RG8, FALSE, FALSE, FALSE, 1, 1, 256, 256, 1, NULL);
			blTextureFilter(_glyphatlas->nCurTexture, BL_TF_LINEAR, BL_TF_LINEAR, BL_TW_CLAMP, BL_TW_CLAMP, FALSE);
			blTextureSwizzle(_glyphatlas->nCurTexture, BL_TS_RED, BL_TS_RED, BL_TS_RED, BL_TS_GREEN);
			BLGuid* _curt = (BLGuid*)malloc(sizeof(BLGuid));
			*_curt = _glyphatlas->nCurTexture;
			blArrayPushBack(_glyphatlas->pTextures, _curt);
			_glyphatlas->nRecordX = 4;
			_glyphatlas->nRecordY = 4;
			_glyphatlas->nYB = 4;
			_xnext = _glyphatlas->nRecordX + _glyphw;
			if (_xnext > 256)
			{
				_glyphatlas->nRecordX = 4;
				_xnext = _glyphatlas->nRecordX + _glyphw;
				_glyphatlas->nRecordY = _glyphatlas->nYB;
			}
			_ybot = _glyphatlas->nRecordY + _glyphh;
		}
		_glyph->sRect.sLT.fX = (BLF32)_glyphatlas->nRecordX;
		_glyph->sRect.sLT.fY = (BLF32)_glyphatlas->nRecordY;
		_glyph->sOffset.fX = (BLF32)_Font->sFtFace->glyph->bitmap_left;
		_glyph->sOffset.fY = _FontHeight * 0.75f + 1 - (_Font->sFtFace->glyph->metrics.horiBearingY >> 6);
		_glyph->sRect.sRB.fX = (BLF32)_glyphatlas->nRecordX + _glyphw - 4;
		_glyph->sRect.sRB.fY = (BLF32)_glyphatlas->nRecordY + _glyphh - 4;
		_glyph->nTexture = _glyphatlas->nCurTexture;
		BLU32 _imgw = _Font->sFtFace->glyph->bitmap.width;
		BLU32 _imgh = _Font->sFtFace->glyph->bitmap.rows;
		if (_outline)
		{
			_imgw += 2; _imgh += 2;
			_glyph->sRect.sRB.fX += 2;
			_glyph->sRect.sRB.fY += 2;
			_glyph->sOffset.fX -= 1;
			_glyph->sOffset.fY -= 1;
		}
		else if (_bold)
		{
			_imgw++; _imgh++;
			_glyph->sRect.sRB.fX += 1;
			_glyph->sRect.sRB.fY += 1;
		}
		else if (_shadow)
		{
			_imgh++;
			_glyph->sRect.sRB.fY += 1;
		}
		BLU16* _bufalloca = (BLU16*)alloca(_imgw * _imgh * sizeof(BLU16));
		memset(_bufalloca, 0, _imgw * _imgh * sizeof(BLU16));
		BLU16* _buf = _bufalloca;
		FT_Bitmap* _bitmap = &_Font->sFtFace->glyph->bitmap;
		if (_outline)
		{
			BLU8 _dsttmp, _srctmp;
			BLU16* _tmpbuflt = _buf;
			BLU16* _tmpbuft = _buf + 1;
			BLU16* _tmpbufrt = _buf + 2;
			BLU16* _tmpbufl = _buf + _imgw;
			BLU16* _tmpbuf = _buf + _imgw + 1;
			BLU16* _tmpbufr = _buf + _imgw + 2;
			BLU16* _tmpbuflb = _buf + 2 * _imgw + 0;
			BLU16* _tmpbufb = _buf + 2 * _imgw + 1;
			BLU16* _tmpbufrb = _buf + 2 * _imgw + 2;
			for (BLU32 _idx = 0; _idx < _bitmap->rows; ++_idx)
			{
				BLU8* _src = _bitmap->buffer + _idx * _bitmap->pitch;
				BLU8* _dst = (BLU8*)(_tmpbuf);
				BLU8* _dstlt = (BLU8*)(_tmpbuflt);
				BLU8* _dstt = (BLU8*)(_tmpbuft);
				BLU8* _dstrt = (BLU8*)(_tmpbufrt);
				BLU8* _dstl = (BLU8*)(_tmpbufl);
				BLU8* _dstr = (BLU8*)(_tmpbufr);
				BLU8* _dstlb = (BLU8*)(_tmpbuflb);
				BLU8* _dstb = (BLU8*)(_tmpbufb);
				BLU8* _dstrb = (BLU8*)(_tmpbufrb);
				for (BLU32 _jdx = 0; _jdx < _bitmap->width; ++_jdx)
				{
					*_dstlt++ += 0x00;
					*_dstt++ += 0x00; 
					*_dstrt++ += 0x00;
					*_dstl++ += 0x00;
					*_dstr++ += 0x00;
					*_dstlb++ += 0x00;
					*_dstb++ += 0x00; 
					*_dstrb++ += 0x00;
					_srctmp = *_src;		
					*_dst++ = (BLU8)MIN_INTERNAL(255, *_dst + (0xFF) * (_srctmp / 255.f));
					*_dstlt++ = (BLU8)MIN_INTERNAL(*_dstlt + (*_src) * 0.5f, 255);
					*_dstt++ = (BLU8)MIN_INTERNAL(*_dstt + (*_src) * 0.5f, 255);
					*_dstrt++ = (BLU8)MIN_INTERNAL(*_dstrt + (*_src) * 0.5f, 255);
					*_dstl++ = (BLU8)MIN_INTERNAL(*_dstl + (*_src) * 0.5f, 255);
					*_dstr++ = (BLU8)MIN_INTERNAL(*_dstr + (*_src) * 0.5f, 255);
					*_dstlb++ = (BLU8)MIN_INTERNAL(*_dstlb + (*_src) * 0.5f, 255);
					*_dstb++ = (BLU8)MIN_INTERNAL(*_dstb + (*_src) * 0.5f, 255);
					*_dstrb++ = (BLU8)MIN_INTERNAL(*_dstrb + (*_src) * 0.5f, 255);
					_dsttmp = *(_dst);
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dsttmp + _srctmp);
					_src++;
				}
				_tmpbuflt += _imgw;
				_tmpbuft += _imgw;
				_tmpbufrt += _imgw;
				_tmpbufl += _imgw;
				_tmpbuf += _imgw;
				_tmpbufr += _imgw;
				_tmpbuflb += _imgw;
				_tmpbufb += _imgw;
				_tmpbufrb += _imgw;
			}
		}
		else if (_bold)
		{
			BLU8 _dsttmp, _srctmp;
			BLU16* _tmpbuf = _buf;
			BLU16* _tmpbuf2 = _buf + 1;
			BLU16* _tmpbuf3 = _buf + _imgw;
			BLU16* _tmpbuf4 = _buf + _imgw + 1;
			for (BLU32 _idx = 0; _idx < _bitmap->rows; ++_idx)
			{
				BLU8* _src = _bitmap->buffer + _idx * _bitmap->pitch;
				BLU8* _dst = (BLU8*)(_tmpbuf);
				BLU8* _dst2 = (BLU8*)(_tmpbuf2);
				BLU8* _dst3 = (BLU8*)(_tmpbuf3);
				BLU8* _dst4 = (BLU8*)(_tmpbuf4);
				for (BLU32 _jdx = 0; _jdx < _bitmap->width; ++_jdx)
				{
					*_dst2++ = 0xFF; 
					*_dst3++ = 0xFF; 
					*_dst4++ = 0xFF;
					_srctmp = *_src;
					*_dst++ = (BLU8)MIN_INTERNAL(255, *_dst + (0xFF) * (_srctmp / 255.f));
					*_dst2++ = (BLU8)MIN_INTERNAL((*_src) * 0.35f, 255);
					*_dst3++ = (BLU8)MIN_INTERNAL((*_src) * 0.35f, 255);
					*_dst4++ = (BLU8)MIN_INTERNAL((*_src) * 0.35f, 255);
					_dsttmp = *(_dst);
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dsttmp + _srctmp);
					_src++;
				}
				_tmpbuf += _imgw;
				_tmpbuf2 += _imgw;
				_tmpbuf3 += _imgw;
				_tmpbuf4 += _imgw;
			}
		}
		else if (_shadow)
		{
			BLU8 _dsttmp, _srctmp;
			BLU16* _tmpbuf = _buf;
			BLU16* _tmpbuf2 = _buf + _bitmap->pitch;
			for (BLU32 _idx = 0; _idx < _bitmap->rows; ++_idx)
			{
				BLU8* _src = _bitmap->buffer + _idx * _bitmap->pitch;
				BLU8* _dst = (BLU8*)(_tmpbuf);
				BLU8* _dst2 = (BLU8*)(_tmpbuf2);
				for (BLU32 _jdx = 0; _jdx < _bitmap->width; ++_jdx)
				{
					*_dst2++ = 0x40;
					_srctmp = *_src;
					*_dst++ = (BLU8)MIN_INTERNAL(255, 0x40 + (0xFF) * (_srctmp / 255.f));
					*_dst2++ = (BLU8)(0.5f * (*_src++)); 
					_dsttmp = *_dst;
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dsttmp + _srctmp);
				}
				_tmpbuf += _imgw;
				_tmpbuf2 += _imgw;
			}
		}
		else
		{
			for (BLU32 _idx = 0; _idx < _bitmap->rows; ++_idx)
			{
				BLU8* _src = _bitmap->buffer + _idx * _bitmap->pitch;
				BLU8* _dst = (BLU8*)(_buf);
				for (BLU32 _jdx = 0; _jdx < _bitmap->width; ++_jdx)
				{
					*_dst++ = 0xFF;
					*_dst++ = *_src++;
				}
				_buf += _imgw;
			}
		}
		blTextureUpdate(_glyphatlas->nCurTexture, 0, 0, BL_CTF_IGNORE, _glyphatlas->nRecordX, _glyphatlas->nRecordY, 0, _imgw, _imgh, 1, _bufalloca);
		_glyphatlas->nRecordX = _xnext;
		if (_ybot > _glyphatlas->nYB)
			_glyphatlas->nYB = _ybot;
		_glyph->bValid = TRUE;
		return _glyph;
	}
	else
	{
		if (_glyph->bValid)
			return _glyph;
		else
			return NULL;
	}
}
static BLVoid
_BreakText(_BLFont* _Font, const BLUtf8* _Text, BLU32 _Width, BLU16 _Flag)
{
}
static BLVoid
_MeasureTextDim(const BLUtf16* _Text, _BLFont* _Font, BLU32 _FontHeight, BLF32* _Width, BLF32* _Height, BLU16 _Flag)
{
	BLBool _outline = (_Flag & 0x000F) > 0;
	BLBool _bold = (_Flag & 0x00F0) > 0;
	BLBool _shadow = (_Flag & 0x0F00) > 0;
	BLBool _italics = (_Flag & 0xF000) > 0;
	if (!strlen((const BLAnsi*)_Text))
	{
		*_Width = 0.f;
		*_Height = 0.f;
		return;
	}
	*_Width = 0;
	*_Height = 0;
	BLS32 _linespace = 0;
	BLS32 _lineheight = _FontHeight;
	BLU32 _str16len = blUtf16Length(_Text);
	BLU32 _start = 0, _end = 0;
	while (_end < _str16len)
	{
		if (_start != 0)
			*_Height += _linespace;
		_end = blUtf16Length(_Text);
		for (BLU32 _idx = 0; _idx < _end; ++_idx)
		{
			if (_Text[_idx] == L'\n')
			{
				_end = _idx;
				break;
			}
		}
		BLUtf16* _tmp16 = (BLUtf16*)alloca((_end - _start + 1) * sizeof(BLUtf16));
		for (BLU32 _idx = _start; _idx < _end - _start; ++_idx)
			_tmp16[_idx - _start] = _Text[_idx];
		_tmp16[_end - _start] = 0;
		BLU32 _adv = 0;
		for (BLU32 _idx = 0; _idx < _end - _start; ++_idx)
		{
			BLUtf16 _char = _tmp16[_idx];
			if (_char == L'\r' || _char == L'\n')
				continue;
			_BLGlyph* _glyph = _QueryGlyph(_Font, MAKEU32(_char, _Flag), _FontHeight);
			if (!_glyph)
				continue;
			_adv += _glyph->nAdv;
		}
		_start = _end + 1;
		if (_adv >= *_Width)
			*_Width = (BLF32)_adv;
		*_Height += _lineheight;
	}
	if (_outline)
	{
		*_Width += 2;
		*_Height += 2;
	}
	else if (_bold)
	{
		*_Width += 1;
		*_Height += 1;
	}
	else if (_shadow)
		*_Height += 1;
}
static BLBool
_UISetup(BLVoid* _Src)
{
	_BLWidget* _node = (_BLWidget*)_Src;
	switch (_node->eType)
	{
	case BL_UT_PANEL:
		_node->uExtension.sPanel.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sPanel.aPixmap), BL_TT_2D, _node->uExtension.sPanel.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sPanel.nTexWidth, _node->uExtension.sPanel.nTexHeight, 1, _node->uExtension.sPanel.pTexData);
		free(_node->uExtension.sPanel.pTexData);
		_node->uExtension.sPanel.pTexData = NULL;
		break;
	case BL_UT_LABEL:
		_node->uExtension.sLabel.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sLabel.aPixmap), BL_TT_2D, _node->uExtension.sLabel.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sLabel.nTexWidth, _node->uExtension.sLabel.nTexHeight, 1, _node->uExtension.sLabel.pTexData);
		free(_node->uExtension.sLabel.pTexData);
		_node->uExtension.sLabel.pTexData = NULL;
		break;
	case BL_UT_BUTTON:
		_node->uExtension.sButton.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sButton.aPixmap), BL_TT_2D, _node->uExtension.sButton.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sButton.nTexWidth, _node->uExtension.sButton.nTexHeight, 1, _node->uExtension.sButton.pTexData);
		free(_node->uExtension.sButton.pTexData);
		_node->uExtension.sButton.pTexData = NULL;
		break;
	case BL_UT_CHECK:
		_node->uExtension.sCheck.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sCheck.aPixmap), BL_TT_2D, _node->uExtension.sCheck.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sCheck.nTexWidth, _node->uExtension.sCheck.nTexHeight, 1, _node->uExtension.sCheck.pTexData);
		free(_node->uExtension.sCheck.pTexData);
		_node->uExtension.sCheck.pTexData = NULL;
		break;
	case BL_UT_SLIDER:
		_node->uExtension.sSlider.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sSlider.aPixmap), BL_TT_2D, _node->uExtension.sSlider.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sSlider.nTexWidth, _node->uExtension.sSlider.nTexHeight, 1, _node->uExtension.sSlider.pTexData);
		free(_node->uExtension.sSlider.pTexData);
		_node->uExtension.sSlider.pTexData = NULL;
		break;
	case BL_UT_TEXT:
		_node->uExtension.sText.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sText.aPixmap), BL_TT_2D, _node->uExtension.sText.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sText.nTexWidth, _node->uExtension.sText.nTexHeight, 1, _node->uExtension.sText.pTexData);
		free(_node->uExtension.sText.pTexData);
		_node->uExtension.sText.pTexData = NULL;
		break;
	case BL_UT_PROGRESS:
		_node->uExtension.sProgress.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sProgress.aPixmap), BL_TT_2D, _node->uExtension.sProgress.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sProgress.nTexWidth, _node->uExtension.sProgress.nTexHeight, 1, _node->uExtension.sProgress.pTexData);
		free(_node->uExtension.sProgress.pTexData);
		_node->uExtension.sProgress.pTexData = NULL;
		break;
	case BL_UT_DIAL:
		_node->uExtension.sDial.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sDial.aPixmap), BL_TT_2D, _node->uExtension.sDial.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sDial.nTexWidth, _node->uExtension.sDial.nTexHeight, 1, _node->uExtension.sDial.pTexData);
		free(_node->uExtension.sDial.pTexData);
		_node->uExtension.sDial.pTexData = NULL;
		break;
	case BL_UT_TABLE:
		_node->uExtension.sTable.nPixmapTex = blGenTexture(blHashUtf8((const BLUtf8*)_node->uExtension.sTable.aPixmap), BL_TT_2D, _node->uExtension.sTable.eTexFormat, FALSE, TRUE, FALSE, 1, 1, _node->uExtension.sTable.nTexWidth, _node->uExtension.sTable.nTexHeight, 1, _node->uExtension.sTable.pTexData);
		free(_node->uExtension.sTable.pTexData);
		_node->uExtension.sTable.pTexData = NULL;
		break;
	default: break;
	}
	_node->bValid = TRUE;
	_PrUIMem->bDirty = TRUE;
	return TRUE;
}
static BLBool
_UIRelease(BLVoid* _Src)
{
	_BLWidget* _node = (_BLWidget*)_Src;
	switch (_node->eType)
	{
	case BL_UT_PANEL:
		blDeleteTexture(_node->uExtension.sPanel.nPixmapTex);
		break;
	case BL_UT_LABEL:
		blDeleteTexture(_node->uExtension.sLabel.nPixmapTex);
		break;
	case BL_UT_BUTTON:
		blDeleteTexture(_node->uExtension.sButton.nPixmapTex);
		break;
	case BL_UT_CHECK:
		blDeleteTexture(_node->uExtension.sCheck.nPixmapTex);
		break;
	case BL_UT_SLIDER:
		blDeleteTexture(_node->uExtension.sSlider.nPixmapTex);
		break;
	case BL_UT_TEXT:
		blDeleteTexture(_node->uExtension.sText.nPixmapTex);
		break;
	case BL_UT_PROGRESS:
		blDeleteTexture(_node->uExtension.sProgress.nPixmapTex);
		break;
	case BL_UT_DIAL:
		blDeleteTexture(_node->uExtension.sDial.nPixmapTex);
		break;
	case BL_UT_TABLE:
		blDeleteTexture(_node->uExtension.sTable.nPixmapTex);
		break;
	default: break;
	}
	return TRUE;
}
static BLBool
_LoadUI(BLVoid* _Src, const BLAnsi* _Filename, const BLAnsi* _Archive)
{
	_BLWidget* _src = (_BLWidget*)_Src;
	BLGuid _stream;
	if (_Archive)
		_stream = blGenStream(_Filename, _Archive);
	else
	{
		BLAnsi _tmpname[260];
		strcpy(_tmpname, _Filename);
		BLAnsi _path[260] = { 0 };
		strcpy(_path, blWorkingDir(TRUE));
		strcat(_path, _tmpname);
		_stream = blGenStream(_path, NULL);
		if (INVALID_GUID == _stream)
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			_stream = blGenStream(_path, NULL);
		}
		if (INVALID_GUID == _stream)
			return FALSE;
	}
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
	BLEnum _type = BL_TT_2D;
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
		switch (_src->eType)
		{
		case BL_UT_PANEL:
			if (!strcmp(_tag, _src->uExtension.sPanel.aCommonMap))
			{
				_src->uExtension.sPanel.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sPanel.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sPanel.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sPanel.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sPanel.sCommonTex9.sLT.fX += (BLF32)_ltx;
				_src->uExtension.sPanel.sCommonTex9.sLT.fY += (BLF32)_lty;
				_src->uExtension.sPanel.sCommonTex9.sRB.fX += (BLF32)_ltx;
				_src->uExtension.sPanel.sCommonTex9.sRB.fY += (BLF32)_lty;
			}
			if (!strcmp(_tag, _src->uExtension.sPanel.aStencilMap))
			{
				_src->uExtension.sPanel.sStenciTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sPanel.sStenciTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sPanel.sStenciTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sPanel.sStenciTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_LABEL:
			if (!strcmp(_tag, _src->uExtension.sLabel.aCommonMap))
			{
				_src->uExtension.sLabel.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sLabel.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sLabel.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sLabel.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sLabel.sCommonTex9.sLT.fX += (BLF32)_ltx;
				_src->uExtension.sLabel.sCommonTex9.sLT.fY += (BLF32)_lty;
				_src->uExtension.sLabel.sCommonTex9.sRB.fX += (BLF32)_ltx;
				_src->uExtension.sLabel.sCommonTex9.sRB.fY += (BLF32)_lty;
			}
			if (!strcmp(_tag, _src->uExtension.sLabel.aStencilMap))
			{
				_src->uExtension.sLabel.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sLabel.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sLabel.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sLabel.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_BUTTON:
			if (!strcmp(_tag, _src->uExtension.sButton.aCommonMap))
			{
				_src->uExtension.sButton.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sButton.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sButton.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sButton.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sButton.sCommonTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sCommonTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sButton.sCommonTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sCommonTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sButton.aHoveredMap))
			{
				_src->uExtension.sButton.sHoveredTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sButton.sHoveredTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sButton.sHoveredTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sButton.sHoveredTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sButton.sHoveredTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sHoveredTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sButton.sHoveredTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sHoveredTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sButton.aPressedMap))
			{
				_src->uExtension.sButton.sPressedTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sButton.sPressedTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sButton.sPressedTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sButton.sPressedTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sButton.sPressedTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sPressedTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sButton.sPressedTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sPressedTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sButton.aDisableMap))
			{
				_src->uExtension.sButton.sDisableTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sButton.sDisableTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sButton.sDisableTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sButton.sDisableTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sButton.sDisableTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sDisableTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sButton.sDisableTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sButton.sDisableTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sButton.aStencilMap))
			{
				_src->uExtension.sButton.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sButton.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sButton.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sButton.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_CHECK:
			if (!strcmp(_tag, _src->uExtension.sCheck.aCommonMap))
			{
				_src->uExtension.sCheck.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sCheck.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sCheck.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sCheck.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sCheck.sCommonTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sCommonTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sCheck.sCommonTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sCommonTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sCheck.aPressedMap))
			{
				_src->uExtension.sCheck.sPressedTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sCheck.sPressedTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sCheck.sPressedTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sCheck.sPressedTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sCheck.sPressedTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sPressedTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sCheck.sPressedTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sPressedTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sCheck.aDisableMap))
			{
				_src->uExtension.sCheck.sDisableTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sCheck.sDisableTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sCheck.sDisableTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sCheck.sDisableTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sCheck.sDisableTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sDisableTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sCheck.sDisableTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sDisableTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sCheck.aStencilMap))
			{
				_src->uExtension.sCheck.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sCheck.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sCheck.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sCheck.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_SLIDER:
			if (!strcmp(_tag, _src->uExtension.sSlider.aCommonMap))
			{
				_src->uExtension.sSlider.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sSlider.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sSlider.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sSlider.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sSlider.sCommonTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sSlider.sCommonTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sSlider.sCommonTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sSlider.sCommonTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sSlider.aSliderCommonMap))
			{
				_src->uExtension.sSlider.sSliderCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sSlider.sSliderCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sSlider.sSliderCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sSlider.sSliderCommonTex.sRB.fY = (BLF32)_rby;
			}
			if (!strcmp(_tag, _src->uExtension.sSlider.aSliderDisableMap))
			{
				_src->uExtension.sSlider.sSliderDisableTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sSlider.sSliderDisableTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sSlider.sSliderDisableTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sSlider.sSliderDisableTex.sRB.fY = (BLF32)_rby;
			}
			if (!strcmp(_tag, _src->uExtension.sSlider.aStencilMap))
			{
				_src->uExtension.sSlider.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sSlider.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sSlider.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sSlider.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_TEXT:
			if (!strcmp(_tag, _src->uExtension.sText.aCommonMap))
			{
				_src->uExtension.sText.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sText.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sText.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sText.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sText.sCommonTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sText.sCommonTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sText.sCommonTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sText.sCommonTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sText.aStencilMap))
			{
				_src->uExtension.sText.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sText.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sText.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sText.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_PROGRESS:
			if (!strcmp(_tag, _src->uExtension.sProgress.aCommonMap))
			{
				_src->uExtension.sProgress.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sProgress.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sProgress.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sProgress.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sProgress.sCommonTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sProgress.sCommonTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sProgress.sCommonTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sProgress.sCommonTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sProgress.aFillMap))
			{
				_src->uExtension.sProgress.sFillTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sProgress.sFillTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sProgress.sFillTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sProgress.sFillTex.sRB.fY = (BLF32)_rby;
			}
			if (!strcmp(_tag, _src->uExtension.sProgress.aStencilMap))
			{
				_src->uExtension.sProgress.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sProgress.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sProgress.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sProgress.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_DIAL:
			if (!strcmp(_tag, _src->uExtension.sDial.aCommonMap))
			{
				_src->uExtension.sDial.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sDial.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sDial.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sDial.sCommonTex.sRB.fY = (BLF32)_rby;
			}
			if (!strcmp(_tag, _src->uExtension.sDial.aStencilMap))
			{
				_src->uExtension.sDial.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sDial.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sDial.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sDial.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		case BL_UT_TABLE:
			if (!strcmp(_tag, _src->uExtension.sTable.aCommonMap))
			{
				_src->uExtension.sTable.sCommonTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sTable.sCommonTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sTable.sCommonTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sTable.sCommonTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sTable.sCommonTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sTable.sCommonTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sTable.sCommonTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sTable.sCommonTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
			}
			if (!strcmp(_tag, _src->uExtension.sTable.aEvenItemMap))
			{
				_src->uExtension.sTable.sEvenItemTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sTable.sEvenItemTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sTable.sEvenItemTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sTable.sEvenItemTex.sRB.fY = (BLF32)_rby;
			}
			if (!strcmp(_tag, _src->uExtension.sTable.aOddItemMap))
			{
				_src->uExtension.sTable.sOddItemTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sTable.sOddItemTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sTable.sOddItemTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sTable.sOddItemTex.sRB.fY = (BLF32)_rby;
			}
			if (!strcmp(_tag, _src->uExtension.sTable.aStencilMap))
			{
				_src->uExtension.sTable.sStencilTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sTable.sStencilTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sTable.sStencilTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sTable.sStencilTex.sRB.fY = (BLF32)_rby;
			}
			break;
		default: break;
		}
	}
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
	BLBool _loadfont = TRUE;
	BLAnsi _texfilettf[260] = { 0 };
	BLAnsi _texfilettc[260] = { 0 };
	blDeleteStream(_stream);
	blMutexLock(_PrUIMem->pFonts->pMutex);
	switch (_src->eType)
	{
	case BL_UT_PANEL:
		_src->uExtension.sPanel.eTexFormat = _format;
		_src->uExtension.sPanel.pTexData = _texdata;
		_src->uExtension.sPanel.nTexWidth = _width;
		_src->uExtension.sPanel.nTexHeight = _height;
		break;
	case BL_UT_LABEL:
		_src->uExtension.sLabel.eTexFormat = _format;
		_src->uExtension.sLabel.pTexData = _texdata;
		_src->uExtension.sLabel.nTexWidth = _width;
		_src->uExtension.sLabel.nTexHeight = _height;
		break;
	case BL_UT_BUTTON:
		{
			_src->uExtension.sButton.eTexFormat = _format;
			_src->uExtension.sButton.pTexData = _texdata;
			_src->uExtension.sButton.nTexWidth = _width;
			_src->uExtension.sButton.nTexHeight = _height;
			FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
			{
				if (_iter->nHashName == blHashUtf8((const BLUtf8*)(_src->uExtension.sButton.aFontSource)))
				{
					_loadfont = FALSE;
					break;
				}
			}
			sprintf(_texfilettf, "%s/font/%s.ttf", _PrUIMem->aDir, _src->uExtension.sButton.aFontSource);
			sprintf(_texfilettc, "%s/font/%s.ttc", _PrUIMem->aDir, _src->uExtension.sButton.aFontSource);
		}
		break;
	case BL_UT_CHECK:
		_src->uExtension.sCheck.eTexFormat = _format;
		_src->uExtension.sCheck.pTexData = _texdata;
		_src->uExtension.sCheck.nTexWidth = _width;
		_src->uExtension.sCheck.nTexHeight = _height;
		break;
	case BL_UT_SLIDER:
		_src->uExtension.sSlider.eTexFormat = _format;
		_src->uExtension.sSlider.pTexData = _texdata;
		_src->uExtension.sSlider.nTexWidth = _width;
		_src->uExtension.sSlider.nTexHeight = _height;
		break;
	case BL_UT_TEXT:
		_src->uExtension.sText.eTexFormat = _format;
		_src->uExtension.sText.pTexData = _texdata;
		_src->uExtension.sText.nTexWidth = _width;
		_src->uExtension.sText.nTexHeight = _height;
		break;
	case BL_UT_PROGRESS:
		_src->uExtension.sProgress.eTexFormat = _format;
		_src->uExtension.sProgress.pTexData = _texdata;
		_src->uExtension.sProgress.nTexWidth = _width;
		_src->uExtension.sProgress.nTexHeight = _height;
		break;
	case BL_UT_DIAL:
		_src->uExtension.sDial.eTexFormat = _format;
		_src->uExtension.sDial.pTexData = _texdata;
		_src->uExtension.sDial.nTexWidth = _width;
		_src->uExtension.sDial.nTexHeight = _height;
		break;
	case BL_UT_TABLE:
		_src->uExtension.sTable.eTexFormat = _format;
		_src->uExtension.sTable.pTexData = _texdata;
		_src->uExtension.sTable.nTexWidth = _width;
		_src->uExtension.sTable.nTexHeight = _height;
		break;
	default: break;
	}	
	blMutexUnlock(_PrUIMem->pFonts->pMutex);
	if (_loadfont)
	{
		_BLFont* _newfont = _FontFace(_texfilettf, _Archive);
		if (!_newfont)
			_newfont = _FontFace(_texfilettc, _Archive);
		if (_newfont)
			blArrayPushBack(_PrUIMem->pFonts, _newfont);
	}
	return TRUE;
}
static BLBool
_UnloadUI(BLVoid* _Src)
{
	_BLWidget* _widget = (_BLWidget*)_Src;
	if (!_widget)
		return FALSE;
	BLU32 _idx = 0;
	if (_widget->pParent)
	{
		FOREACH_ARRAY(_BLWidget*, _iter, _widget->pParent->pChildren)
		{
			if (_iter == _widget)
				break;
			_idx++;
		}
		blArrayErase(_widget->pParent->pChildren, _idx);
	}
	if (_widget->pTooltip)
		free(_widget->pTooltip);
	switch (_widget->eType)
	{
	case BL_UT_LABEL:
	{
		if (_widget->uExtension.sLabel.pText)
			free(_widget->uExtension.sLabel.pText);
	}
	break;
	case BL_UT_BUTTON:
	{
		if (_widget->uExtension.sButton.pText)
			free(_widget->uExtension.sButton.pText);
	}
	break;
	case BL_UT_CHECK:
	{
		if (_widget->uExtension.sCheck.pText)
			free(_widget->uExtension.sCheck.pText);
	}
	break;
	case BL_UT_TEXT:
	{
		if (_widget->uExtension.sText.pText)
			free(_widget->uExtension.sText.pText);
		if (_widget->uExtension.sText.pPlaceholder)
			free(_widget->uExtension.sText.pPlaceholder);
	}
	break;
	case BL_UT_PROGRESS:
	{
		if (_widget->uExtension.sProgress.pText)
			free(_widget->uExtension.sProgress.pText);
	}
	break;
	case BL_UT_PRIMITIVE:
	{
		if (_widget->uExtension.sPrimitive.pXPath)
			free(_widget->uExtension.sPrimitive.pXPath);
		if (_widget->uExtension.sPrimitive.pYPath)
			free(_widget->uExtension.sPrimitive.pYPath);
	}
	break;
	default:break;
	}
	_widget->bValid = FALSE;
	return TRUE;
}
static _BLWidget*
_QueryWidget(_BLWidget* _Node, BLU32 _HashName, BLBool _SearchChildren_)
{
	_BLWidget* _ret = _PrUIMem->pRoot;
	FOREACH_ARRAY (_BLWidget*, _iter, _Node->pChildren)
	{
		if (URIPART_INTERNAL(_iter->nID) == _HashName)
			return _iter;
		if (_SearchChildren_)
			_ret = _QueryWidget(_iter, _HashName, TRUE);
		if (_ret)
			return _ret;
	}
	return _ret;
}
static _BLWidget*
_LocateWidget(_BLWidget* _Node, BLF32 _XPos, BLF32 _YPos, BLF32 _XOffset, BLF32 _YOffset)
{
	BLF32 _x = 0.f, _y = 0.f, _w = 0.f, _h = 0.f;
	_BLWidget* _target = NULL;
	if (_Node->pParent)
	{
		BLF32 _pw = _Node->pParent->sDimension.fX > 0.f ? _Node->pParent->sDimension.fX : _PrUIMem->nFboWidth;
		BLF32 _ph = _Node->pParent->sDimension.fY > 0.f ? _Node->pParent->sDimension.fY : _PrUIMem->nFboHeight;
		if (_Node->eReferenceH == BL_UA_LEFT && _Node->eReferenceV == BL_UA_TOP)
		{
			_x = _Node->sPosition.fX - 0.5f * _pw + _XOffset;
			_y = _Node->sPosition.fY - 0.5f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_LEFT && _Node->eReferenceV == BL_UA_VCENTER)
		{
			_x = _Node->sPosition.fX - 0.5f * _pw + _XOffset;
			_y = _Node->sPosition.fY + 0.0f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_LEFT && _Node->eReferenceV == BL_UA_BOTTOM)
		{
			_x = _Node->sPosition.fX - 0.5f * _pw + _XOffset;
			_y = _Node->sPosition.fY + 0.5f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_HCENTER && _Node->eReferenceV == BL_UA_TOP)
		{
			_x = _Node->sPosition.fX + 0.0f * _pw + _XOffset;
			_y = _Node->sPosition.fY - 0.5f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_HCENTER && _Node->eReferenceV == BL_UA_VCENTER)
		{
			_x = _Node->sPosition.fX + 0.0f * _pw + _XOffset;
			_y = _Node->sPosition.fY + 0.0f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_HCENTER && _Node->eReferenceV == BL_UA_BOTTOM)
		{
			_x = _Node->sPosition.fX + 0.0f * _pw + _XOffset;
			_y = _Node->sPosition.fY + 0.5f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_RIGHT && _Node->eReferenceV == BL_UA_TOP)
		{
			_x = _Node->sPosition.fX + 0.5f * _pw + _XOffset;
			_y = _Node->sPosition.fY - 0.5f * _ph + _YOffset;
		}
		else if (_Node->eReferenceH == BL_UA_RIGHT && _Node->eReferenceV == BL_UA_VCENTER)
		{
			_x = _Node->sPosition.fX + 0.5f * _pw + _XOffset;
			_y = _Node->sPosition.fY + 0.0f * _ph + _YOffset;
		}
		else
		{
			_x = _Node->sPosition.fX + 0.5f * _pw + _XOffset;
			_y = _Node->sPosition.fY + 0.5f * _ph + _YOffset;
		}
		if (_Node->ePolicy == BL_UP_FIXED)
		{
			_w = _Node->sDimension.fX;
			_h = _Node->sDimension.fY;
		}
		else if (_Node->ePolicy == BL_UP_HMatch)
		{
			_w = _pw;
			_h = _pw / _Node->fRatio;
		}
		else if (_Node->ePolicy == BL_UP_VMatch)
		{
			_w = _ph * _Node->fRatio;
			_h = _ph;
		}
		else
		{
			_w = _pw;
			_h = _ph;
		}
	}
	else
	{
		_x = _PrUIMem->nFboWidth * 0.5f;
		_y = _PrUIMem->nFboHeight * 0.5f;
	}
	FOREACH_ARRAY(_BLWidget*, _iter, _Node->pChildren)
	{
		if (_iter->bVisible)
		{
			_target = _LocateWidget(_iter, _XPos, _YPos, _x, _y);
			if (_target)
				return _target;
		}
	}
	if (_XPos >= _x - _w * 0.5f && _XPos <= _x + _w * 0.5f && _YPos >= _y - _h * 0.5f && _YPos <= _y + _h * 0.5f) 
		_target = _Node;
	return _target;
}
static BLVoid
_FrontWidget(_BLWidget* _Node)
{
	if (!_Node->pParent)
		return;
	BLU32 _idx = 0;
	FOREACH_ARRAY (_BLWidget*, _iter, _Node->pParent->pChildren)
	{
		if (_Node == _iter)
		{
			blArrayErase(_Node->pParent->pChildren, _idx);
			blArrayPushBack(_Node->pParent->pChildren, _Node);
			_PrUIMem->bDirty = TRUE;
			return;
		}
	}
}
static BLVoid
_WidgetScissorRect(_BLWidget* _Node, BLRect* _Rect)
{
	if (!_Node->pParent)
	{
		_Rect->sLT.fX = 0.f;
		_Rect->sLT.fY = 0.f;
		_Rect->sRB.fX = (BLF32)_PrUIMem->nFboWidth;
		_Rect->sRB.fY = (BLF32)_PrUIMem->nFboHeight;
		return;
	}
	if (_Node->bCliped)
	{
		if (_Node->pParent == _PrUIMem->pRoot)
		{
			_Rect->sLT.fX = 0.f;
			_Rect->sLT.fY = 0.f;
			_Rect->sRB.fX = (BLF32)_PrUIMem->nFboWidth;
			_Rect->sRB.fY = (BLF32)_PrUIMem->nFboHeight;
		}
		else
		{
			_BLWidget* _tmp = _Node;
			BLF32 _x = _Node->pParent->sPosition.fX;
			BLF32 _y = _Node->pParent->sPosition.fY;
			BLF32 _pw;
			BLF32 _ph;
			while (_tmp != _PrUIMem->pRoot)
			{
				_tmp = _tmp->pParent;
				_pw = _tmp->pParent->sDimension.fX > 0.f ? _tmp->pParent->sDimension.fX : _PrUIMem->nFboWidth;
				_ph = _tmp->pParent->sDimension.fY > 0.f ? _tmp->pParent->sDimension.fY : _PrUIMem->nFboHeight;
				if (_tmp->eReferenceH == BL_UA_LEFT && _tmp->eReferenceV == BL_UA_TOP)
				{
					_x = 0.0f * _pw + _x;
					_y = 0.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_LEFT && _tmp->eReferenceV == BL_UA_VCENTER)
				{
					_x = 0.0f * _pw + _x;
					_y = 0.5f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_LEFT && _tmp->eReferenceV == BL_UA_BOTTOM)
				{
					_x = 0.0f * _pw + _x;
					_y = 1.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_HCENTER && _tmp->eReferenceV == BL_UA_TOP)
				{
					_x = 0.5f * _pw + _x;
					_y = 0.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_HCENTER && _tmp->eReferenceV == BL_UA_VCENTER)
				{
					_x = 0.5f * _pw + _x;
					_y = 0.5f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_HCENTER && _tmp->eReferenceV == BL_UA_BOTTOM)
				{
					_x = 0.5f * _pw + _x;
					_y = 1.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_RIGHT && _tmp->eReferenceV == BL_UA_TOP)
				{
					_x = 1.0f * _pw + _x;
					_y = 0.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_RIGHT && _tmp->eReferenceV == BL_UA_VCENTER)
				{
					_x = 1.0f * _pw + _x;
					_y = 0.5f * _ph + _y;
				}
				else
				{
					_x = 1.0f * _pw + _x;
					_y = 1.0f * _ph + _y;
				}
			}
			BLF32 _w, _h;
			if (_tmp->pParent->ePolicy == BL_UP_FIXED)
			{
				_w = _pw;
				_h = _ph;
			}
			else if (_tmp->pParent->ePolicy == BL_UP_HMatch)
			{
				_w = _pw;
				_h = _pw / _tmp->pParent->fRatio;
			}
			else if (_tmp->pParent->ePolicy == BL_UP_VMatch)
			{
				_w = _ph * _tmp->pParent->fRatio;
				_h = _ph;
			}
			else
			{
				_w = _pw;
				_h = _ph;
			}
			_Rect->sLT.fX = _x - _pw * 0.5f;
			_Rect->sLT.fY = _y - _ph * 0.5f;
			_Rect->sRB.fX = _x + _pw * 0.5f;
			_Rect->sRB.fY = _y + _ph * 0.5f;
		}
	}
	else
	{
		_BLWidget* _tmp = _Node;
		while (_tmp != _PrUIMem->pRoot)
		{
			_tmp = _tmp->pParent;
			if (_tmp->bAbsScissor)
				break;
		}
		if (_tmp == _PrUIMem->pRoot)
		{
			_Rect->sLT.fX = 0.f;
			_Rect->sLT.fY = 0.f;
			_Rect->sRB.fX = (BLF32)_PrUIMem->nFboWidth;
			_Rect->sRB.fY = (BLF32)_PrUIMem->nFboHeight;
		}
		else
		{
			BLF32 _x = _tmp->pParent->sPosition.fX;
			BLF32 _y = _tmp->pParent->sPosition.fY;
			BLF32 _pw;
			BLF32 _ph;
			while (_tmp != _PrUIMem->pRoot)
			{
				_tmp = _tmp->pParent;
				_pw = _tmp->pParent->sDimension.fX > 0.f ? _tmp->pParent->sDimension.fX : _PrUIMem->nFboWidth;
				_ph = _tmp->pParent->sDimension.fY > 0.f ? _tmp->pParent->sDimension.fY : _PrUIMem->nFboHeight;
				if (_tmp->eReferenceH == BL_UA_LEFT && _tmp->eReferenceV == BL_UA_TOP)
				{
					_x = 0.0f * _pw + _x;
					_y = 0.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_LEFT && _tmp->eReferenceV == BL_UA_VCENTER)
				{
					_x = 0.0f * _pw + _x;
					_y = 0.5f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_LEFT && _tmp->eReferenceV == BL_UA_BOTTOM)
				{
					_x = 0.0f * _pw + _x;
					_y = 1.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_HCENTER && _tmp->eReferenceV == BL_UA_TOP)
				{
					_x = 0.5f * _pw + _x;
					_y = 0.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_HCENTER && _tmp->eReferenceV == BL_UA_VCENTER)
				{
					_x = 0.5f * _pw + _x;
					_y = 0.5f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_HCENTER && _tmp->eReferenceV == BL_UA_BOTTOM)
				{
					_x = 0.5f * _pw + _x;
					_y = 1.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_RIGHT && _tmp->eReferenceV == BL_UA_TOP)
				{
					_x = 1.0f * _pw + _x;
					_y = 0.0f * _ph + _y;
				}
				else if (_tmp->eReferenceH == BL_UA_RIGHT && _tmp->eReferenceV == BL_UA_VCENTER)
				{
					_x = 1.0f * _pw + _x;
					_y = 0.5f * _ph + _y;
				}
				else
				{
					_x = 1.0f * _pw + _x;
					_y = 1.0f * _ph + _y;
				}
			}
			BLF32 _w, _h;
			if (_tmp->pParent->ePolicy == BL_UP_FIXED)
			{
				_w = _pw;
				_h = _ph;
			}
			else if (_tmp->pParent->ePolicy == BL_UP_HMatch)
			{
				_w = _pw;
				_h = _pw / _tmp->pParent->fRatio;
			}
			else if (_tmp->pParent->ePolicy == BL_UP_VMatch)
			{
				_w = _ph * _tmp->pParent->fRatio;
				_h = _ph;
			}
			else
			{
				_w = _pw;
				_h = _ph;
			}
			_Rect->sLT.fX = _x - _pw * 0.5f;
			_Rect->sLT.fY = _y - _ph * 0.5f;
			_Rect->sRB.fX = _x + _pw * 0.5f;
			_Rect->sRB.fY = _y + _ph * 0.5f;
		}
	}
}
static BLVoid
_WriteText(const BLUtf8* _Text, const BLAnsi* _Font, BLU32 _FontHeight, BLEnum _AlignmentH, BLEnum _AlignmentV, BLBool _Wrap, BLRect* _Area, BLRect* _ClipArea, BLU32 _Color, BLU16 _Flag, BLBool _Pwd)
{
	BLU32 _txtlen = blUtf8Length(_Text);
	if (!_txtlen)
		return;
	_BLFont* _ft = NULL;
	BLUtf8* _str = (BLUtf8*)alloca(_txtlen + 1);
	memset(_str, 0, _txtlen + 1);
	if (_Pwd)
	{
		BLUtf8 _placeholder = '*';
		memcpy(_str, &_placeholder, _txtlen);
	}
	else
		strcpy(_str, _Text);
	{
		FOREACH_ARRAY (_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_Font))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return;
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_ClipArea->sLT.fX, (BLU32)_ClipArea->sLT.fY, (BLU32)(_ClipArea->sRB.fX - _ClipArea->sLT.fX), (BLU32)(_ClipArea->sRB.fY - _ClipArea->sLT.fY));
	_BLGlyphAtlas* _gatlas = blDictElement(_ft->pGlyphAtlas, _FontHeight);
	if (!_gatlas)
	{
		_gatlas = (_BLGlyphAtlas*)malloc(sizeof(_BLGlyphAtlas));
		memset(_gatlas, 0, sizeof(_BLGlyphAtlas));
		_gatlas->pTextures = blGenArray(FALSE);
		_gatlas->pGlyphs = blGenDict(FALSE);
		_gatlas->nCurTexture = INVALID_GUID;
		blDictInsert(_ft->pGlyphAtlas, _FontHeight, _gatlas);
	}
	FT_Set_Char_Size(_ft->sFtFace, _FontHeight * 64, 0, 72 * 64, 72);
	if (_Wrap)
		_BreakText(_ft, _str, (BLU32)(_Area->sRB.fX - _Area->sLT.fX), _Flag);
	BLF32 _w, _h;
	const BLUtf16* _str16 = blGenUtf16Str(_Text);
	BLU32 _str16len = blUtf16Length(_str16);
	_MeasureTextDim(_str16, _ft, _FontHeight, &_w, &_h, _Flag);
	BLVec2 _pt;
	_pt.fX = _Area->sLT.fX;
	_pt.fY = _Area->sLT.fY;
	if (_AlignmentV == BL_UA_VCENTER)
		_pt.fY = (_Area->sLT.fY + _Area->sRB.fY) * 0.5f - _h * 0.5f;
	else if (_AlignmentV == BL_UA_BOTTOM)
		_pt.fY = _Area->sRB.fY - _h;
	else
		_pt.fY = _Area->sLT.fY;
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	if (!_Wrap)
	{
		if (_AlignmentH == BL_UA_HCENTER)
			_pt.fX = (_Area->sLT.fX + _Area->sRB.fX) * 0.5f - _w * 0.5f;
		else if (_AlignmentH == BL_UA_RIGHT)
			_pt.fX = _Area->sRB.fX - _w;
		else
			_pt.fX = _Area->sLT.fX;
		for (BLU32 _idx = 0; _idx < _str16len; ++_idx)
		{
			BLUtf16 _c = _str16[_idx];
			if (_c == L'\r' || _c == L'\n')
				continue;
			_BLGlyph* _gi = _QueryGlyph(_ft, MAKEU32(_c, _Flag), _FontHeight);
			if (!_gi)
				continue;
			BLF32 _ltx = _pt.fX + _gi->sOffset.fX;
			BLF32 _lty = _pt.fY + _gi->sOffset.fY;
			BLF32 _rbx = _ltx + _gi->sRect.sRB.fX - _gi->sRect.sLT.fX;
			BLF32 _rby = _lty + _gi->sRect.sRB.fY - _gi->sRect.sLT.fY;
			BLF32 _texltx = _gi->sRect.sLT.fX / 256.f;
			BLF32 _texlty = _gi->sRect.sLT.fY / 256.f;
			BLF32 _texrbx = _gi->sRect.sRB.fX / 256.f;
			BLF32 _texrby = _gi->sRect.sRB.fY / 256.f;
			BLF32 _rgba[4];
			blDeColor4F(_Color, _rgba);
			BLF32 _vbo[] = {
				PIXEL_ALIGNED_INTERNAL(_ltx),
				PIXEL_ALIGNED_INTERNAL(_lty),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_rgba[3],
				_texltx,
				_texlty,
				PIXEL_ALIGNED_INTERNAL(_rbx),
				PIXEL_ALIGNED_INTERNAL(_lty),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_rgba[3],
				_texrbx,
				_texlty,
				PIXEL_ALIGNED_INTERNAL(_ltx),
				PIXEL_ALIGNED_INTERNAL(_rby),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_rgba[3],
				_texltx,
				_texrby,
				PIXEL_ALIGNED_INTERNAL(_rbx),
				PIXEL_ALIGNED_INTERNAL(_rby),
				_rgba[0],
				_rgba[1],
				_rgba[2],
				_rgba[3],
				_texrbx,
				_texrby
			};
			_pt.fX += _gi->nAdv;
			blTechSampler(_PrUIMem->nUITech, "Texture0", _gi->nTexture, 0);
			BLGuid _geo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"@#fontglyph#@"), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
			blDraw(_PrUIMem->nUITech, _geo, 1);
			blDeleteGeometryBuffer(_geo);
		}
	}
	else
	{
		BLU32 _ls = 0, _le = 0;
		BLUtf16* _line = (BLUtf16*)alloca((_str16len + 1) * sizeof(BLUtf16));
		while (_le < _str16len)
		{
			memset(_line, 0, (_str16len + 1)* sizeof(BLUtf16));
			_le = _str16len;
			for (BLU32 _idx = _ls; _idx < _str16len; ++_idx)
			{
				if (_line[_idx] == L'\n' && _idx != _str16len - 1)
				{
					_le = _idx;
					break;
				}
			}
			for (BLU32 _idx = 0; _idx < _le - _ls; ++_idx)
				_line[_idx] = _str16[_idx + _ls];
			if (_AlignmentH == BL_UA_HCENTER)
			{
				BLF32 _linew, _lineh;
				_MeasureTextDim(_line, _ft, _FontHeight, &_linew, &_lineh, _Flag);
				_pt.fX = (_Area->sLT.fX + _Area->sRB.fX) * 0.5f - _linew * 0.5f;
			}
			else if (_AlignmentH == BL_UA_RIGHT)
			{
				BLF32 _linew, _lineh;
				_MeasureTextDim(_line, _ft, _FontHeight, &_linew, &_lineh, _Flag);
				_pt.fX = _Area->sRB.fX - _linew;
			}
			else
				_pt.fX = _Area->sLT.fX;
			for (BLU32 _idx = 0; _idx < _le - _ls; ++_idx)
			{
				BLUtf16 _c = _line[_idx];
				if (_c == L'\r' || _c == L'\n')
					continue;
				_BLGlyph* _gi = _QueryGlyph(_ft, MAKEU32(_c, _Flag), _FontHeight);
				if (_gi->nTexture == INVALID_GUID)
					continue;
				BLF32 _ltx = _pt.fX + _gi->sOffset.fX;
				BLF32 _lty = _pt.fY + _gi->sOffset.fY;
				BLF32 _rbx = _ltx + _gi->sRect.sRB.fX - _gi->sRect.sLT.fX;
				BLF32 _rby = _lty + _gi->sRect.sRB.fY - _gi->sRect.sLT.fY;
				BLF32 _texltx = _gi->sRect.sLT.fX / 256.f;
				BLF32 _texlty = _gi->sRect.sLT.fY / 256.f;
				BLF32 _texrbx = _gi->sRect.sRB.fX / 256.f;
				BLF32 _texrby = _gi->sRect.sRB.fY / 256.f;
				BLF32 _rgba[4];
				blDeColor4F(_Color, _rgba);
				BLF32 _vbo[] = {
					PIXEL_ALIGNED_INTERNAL(_ltx),
					PIXEL_ALIGNED_INTERNAL(_lty),
					_rgba[0],
					_rgba[1],
					_rgba[2],
					_rgba[3],
					_texltx,
					_texlty,
					PIXEL_ALIGNED_INTERNAL(_rbx),
					PIXEL_ALIGNED_INTERNAL(_lty),
					_rgba[0],
					_rgba[1],
					_rgba[2],
					_rgba[3],
					_texrbx,
					_texlty,
					PIXEL_ALIGNED_INTERNAL(_ltx),
					PIXEL_ALIGNED_INTERNAL(_rby),
					_rgba[0],
					_rgba[1],
					_rgba[2],
					_rgba[3],
					_texltx,
					_texrby,
					PIXEL_ALIGNED_INTERNAL(_rbx),
					PIXEL_ALIGNED_INTERNAL(_rby),
					_rgba[0],
					_rgba[1],
					_rgba[2],
					_rgba[3],
					_texrbx,
					_texrby
				};
				_pt.fX += _gi->nAdv;
				blTechSampler(_PrUIMem->nUITech, "Texture0", _gi->nTexture, 0);
				BLGuid _geo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"@#fontglyph#@"), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
				blDraw(_PrUIMem->nUITech, _geo, 1);
				blDeleteGeometryBuffer(_geo);
			}
			_ls = _le + 1;
			_pt.fY += _FontHeight;
		}
	}
	blDeleteUtf16Str((BLUtf16*)_str16);
}
static BLVoid
_DrawPanel(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawLabel(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawButton(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sButton.nPixmapTex, 0);
	BLF32 _gray = 1.f;
	BLF32 _offsetx = 0.f, _offsety = 0.f;
	BLF32 _stencil = 1.f;
	BLRect _texcoord, _texcoord9;
	BLRect _texcoords, _texcoords9;
	BLBool _flipx = _Node->uExtension.sButton.bFlipX;
	BLBool _flipy = _Node->uExtension.sButton.bFlipY;
	switch (_Node->uExtension.sButton.nState)
	{
	case 0:
		if (_Node->uExtension.sButton.aDisableMap[0] == 0 || !strcmp(_Node->uExtension.sButton.aDisableMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sButton.sCommonTex;
			_texcoord9 = _Node->uExtension.sButton.sCommonTex9;
			_gray = -1.f;
		}
		else
		{
			_texcoord = _Node->uExtension.sButton.sDisableTex;
			_texcoord9 = _Node->uExtension.sButton.sDisableTex9;
			_gray = 1.f;
		}
		break;
	case 1:
		{
			_texcoord = _Node->uExtension.sButton.sCommonTex;
			_texcoord9 = _Node->uExtension.sButton.sCommonTex9;
		}
		break;
	case 2:
		if (_Node->uExtension.sButton.aHoveredMap[0] == 0 || !strcmp(_Node->uExtension.sButton.aHoveredMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sButton.sCommonTex;
			_texcoord9 = _Node->uExtension.sButton.sCommonTex9;
		}
		else
		{
			_texcoord = _Node->uExtension.sButton.sHoveredTex;
			_texcoord9 = _Node->uExtension.sButton.sHoveredTex9;
		}
		break;
	case 3:
		if (_Node->uExtension.sButton.aPressedMap[0] == 0 || !strcmp(_Node->uExtension.sButton.aPressedMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sButton.sCommonTex;
			_texcoord9 = _Node->uExtension.sButton.sCommonTex9;
			_offsetx = 3.f;
			_offsety = _Height / _Width * 3.f;
		}
		else
		{
			_texcoord = _Node->uExtension.sButton.sPressedTex;
			_texcoord9 = _Node->uExtension.sButton.sPressedTex9;
			_offsetx = 0.f;
			_offsety = 0.f;
		}
		break;
	default:
		_texcoord = _Node->uExtension.sButton.sCommonTex;
		_texcoord9 = _Node->uExtension.sButton.sCommonTex9;
		break;
	}
	if (_Node->uExtension.sButton.aStencilMap[0] && strcmp(_Node->uExtension.sButton.aStencilMap, "Nil"))
	{
		_stencil = -1.f;
		_texcoords.sLT.fX = _Node->uExtension.sButton.sStencilTex.sLT.fX / _Node->uExtension.sButton.nTexWidth + 0.001f;
		_texcoords.sLT.fY = _Node->uExtension.sButton.sStencilTex.sLT.fY / _Node->uExtension.sButton.nTexHeight + 0.001f;
		_texcoords.sRB.fX = _Node->uExtension.sButton.sStencilTex.sRB.fX / _Node->uExtension.sButton.nTexWidth - 0.001f;
		_texcoords.sRB.fY = _Node->uExtension.sButton.sStencilTex.sRB.fY / _Node->uExtension.sButton.nTexHeight - 0.001f;
		BLF32 _oriw = _Node->uExtension.sButton.sCommonTex.sRB.fX - _Node->uExtension.sButton.sCommonTex.sLT.fX;
		BLF32 _orih = _Node->uExtension.sButton.sCommonTex.sRB.fY - _Node->uExtension.sButton.sCommonTex.sLT.fY;
		BLF32 _dstw = _Node->uExtension.sButton.sStencilTex.sRB.fX - _Node->uExtension.sButton.sStencilTex.sLT.fX;
		BLF32 _dsth = _Node->uExtension.sButton.sStencilTex.sRB.fX - _Node->uExtension.sButton.sStencilTex.sLT.fX;
		BLF32 _ratioax = (_Node->uExtension.sButton.sCommonTex9.sLT.fX - _Node->uExtension.sButton.sCommonTex.sLT.fX) / _oriw;
		BLF32 _ratioay = (_Node->uExtension.sButton.sCommonTex9.sLT.fY - _Node->uExtension.sButton.sCommonTex.sLT.fY) / _orih;
		BLF32 _rationx = (_Node->uExtension.sButton.sCommonTex.sRB.fX - _Node->uExtension.sButton.sCommonTex9.sRB.fX) / _oriw;
		BLF32 _rationy = (_Node->uExtension.sButton.sCommonTex.sRB.fY - _Node->uExtension.sButton.sCommonTex9.sRB.fY) / _orih;
		_texcoords9.sLT.fX = (_Node->uExtension.sButton.sStencilTex.sLT.fX + _ratioax * _dstw) / _Node->uExtension.sButton.nTexWidth;
		_texcoords9.sLT.fY = (_Node->uExtension.sButton.sStencilTex.sLT.fY + _ratioay * _dsth) / _Node->uExtension.sButton.nTexHeight;
		_texcoords9.sRB.fX = (_Node->uExtension.sButton.sStencilTex.sRB.fX - _rationx * _dstw) / _Node->uExtension.sButton.nTexWidth;
		_texcoords9.sRB.fY = (_Node->uExtension.sButton.sStencilTex.sRB.fY - _rationy * _dsth) / _Node->uExtension.sButton.nTexHeight;
	}
	else
	{
		_texcoords.sLT.fX = 1.f;
		_texcoords.sLT.fY = 1.f;
		_texcoords.sRB.fX = 1.f;
		_texcoords.sRB.fY = 1.f;
		_texcoords9.sLT.fX = 1.f;
		_texcoords9.sLT.fY = 1.f;
		_texcoords9.sRB.fX = 1.f;
		_texcoords9.sRB.fY = 1.f;
	}
	if (_RectApproximate(&_texcoord, &_texcoord9))
	{
		if (_flipx)
		{
			BLF32 _tmp = _texcoord.sLT.fX;
			_texcoord.sLT.fX = _texcoord.sRB.fX;
			_texcoord.sRB.fX = _tmp;
			_tmp = _texcoords.sLT.fX;
			_texcoords.sLT.fX = _texcoords.sRB.fX;
			_texcoords.sRB.fX = _tmp;
		}
		if (_flipy)
		{
			BLF32 _tmp = _texcoord.sLT.fY;
			_texcoord.sLT.fY = _texcoord.sRB.fY;
			_texcoord.sRB.fY = _tmp;
			_tmp = _texcoords.sLT.fY;
			_texcoords.sLT.fY = _texcoords.sRB.fY;
			_texcoords.sRB.fY = _tmp;
		}
		BLF32 _vbo[] = {
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"@#widgetgeo#@"), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	else
	{
		BLF32 _marginax = _texcoord9.sLT.fX - _texcoord.sLT.fX;
		BLF32 _marginay = _texcoord9.sLT.fY - _texcoord.sLT.fY;
		BLF32 _marginnx = _texcoord.sRB.fX - _texcoord9.sRB.fX;
		BLF32 _marginny = _texcoord.sRB.fY - _texcoord9.sRB.fY;
		if (_flipx)
		{
			BLF32 _tmp = _texcoord.sLT.fX;
			_texcoord.sLT.fX = _texcoord.sRB.fX;
			_texcoord.sRB.fX = _tmp;
			_tmp = _texcoords.sLT.fX;
			_texcoords.sLT.fX = _texcoords.sRB.fX;
			_texcoords.sRB.fX = _tmp;
			_tmp = _texcoord9.sLT.fX;
			_texcoord9.sLT.fX = _texcoord9.sRB.fX;
			_texcoord9.sRB.fX = _tmp;
			_tmp = _texcoords9.sLT.fX;
			_texcoords9.sLT.fX = _texcoords9.sRB.fX;
			_texcoords9.sRB.fX = _tmp;
			_tmp = _marginax;
			_marginax = _marginnx;
			_marginnx = _tmp;
		}
		if (_flipy)
		{
			BLF32 _tmp = _texcoord.sLT.fY;
			_texcoord.sLT.fY = _texcoord.sRB.fY;
			_texcoord.sRB.fY = _tmp;
			_tmp = _texcoords.sLT.fY;
			_texcoords.sLT.fY = _texcoords.sRB.fY;
			_texcoords.sRB.fY = _tmp;
			_tmp = _texcoord9.sLT.fY;
			_texcoord9.sLT.fY = _texcoord9.sRB.fY;
			_texcoord9.sRB.fY = _tmp;
			_tmp = _texcoords9.sLT.fY;
			_texcoords9.sLT.fY = _texcoords9.sRB.fY;
			_texcoords9.sRB.fY = _tmp;
			_tmp = _marginay;
			_marginay = _marginny;
			_marginny = _tmp;
		}
		BLF32 _vob[] = {
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil, 
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sButton.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sButton.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"@#widgetgeo#@"), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	BLRect _area;
	_area.sLT.fX = _X - 0.5f * _Width;
	_area.sLT.fY = _Y - 0.5f * _Height;
	_area.sRB.fX = _X + 0.5f * _Width;
	_area.sRB.fY = _Y + 0.5f * _Height;
	BLU16 _flag = 0;
	if (_Node->uExtension.sButton.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sButton.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sButton.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sButton.bItalics)
		_flag |= 0xF000;
	_WriteText(_Node->uExtension.sButton.pText, _Node->uExtension.sButton.aFontSource, _Node->uExtension.sButton.nFontHeight, _Node->uExtension.sButton.eTxtAlignmentH, _Node->uExtension.sButton.eTxtAlignmentV, FALSE, &_area, &_area, (_Node->uExtension.sButton.nState == 0) ? _PrUIMem->nTextDisableColor : _Node->uExtension.sButton.nTxtColor, _flag, FALSE);
}
static BLVoid
_DrawCheck(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawSlider(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawText(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawProgress(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawDial(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawTable(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawPrimitive(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	if (_Node->uExtension.sPrimitive.nPathNum < 2)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blTechSampler(_PrUIMem->nUITech, "Texture0", _PrUIMem->nBlankTex, 0);
	//blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	BLF32* _vb;
	BLU32* _ib;
	BLU32 _idxcount;
	BLU32 _vtxcount;
	BLU32 _coltrans = _Node->uExtension.sPrimitive.nColor & 0x00FFFFFF;
	BLF32 _rgba[4];
	blDeColor4F(_Node->uExtension.sPrimitive.nColor, _rgba);
	BLF32 _rgbat[4];
	blDeColor4F(_coltrans, _rgbat);
	if (!_Node->uExtension.sPrimitive.bFill)
	{
		BLU32 _fillcount = 0;
		BLU32 _count = _Node->uExtension.sPrimitive.nPathNum;
		if (!_Node->uExtension.sPrimitive.bClosed)
			_count = _Node->uExtension.sPrimitive.nPathNum - 1;
		_idxcount = _count * 12;
		_vtxcount = _Node->uExtension.sPrimitive.nPathNum * 3;
		_vb = (BLF32*)alloca(_vtxcount * 8 * sizeof(BLF32));
		_ib = (BLU32*)alloca(_idxcount * sizeof(BLU32));
		BLVec2* _tempnormals = (BLVec2*)alloca(_Node->uExtension.sPrimitive.nPathNum * 3 * sizeof(BLVec2));
		BLVec2* _temppoints = _tempnormals + _Node->uExtension.sPrimitive.nPathNum;
		for (BLU32 _idx1 = 0; _idx1 < _count; ++_idx1)
		{
			BLU32 _idx2 = (_idx1 + 1) == _Node->uExtension.sPrimitive.nPathNum ? 0 : _idx1 + 1;
			BLVec2 _diff;
			_diff.fX = _Node->uExtension.sPrimitive.pXPath[_idx2] - _Node->uExtension.sPrimitive.pXPath[_idx1];
			_diff.fY = _Node->uExtension.sPrimitive.pYPath[_idx2] - _Node->uExtension.sPrimitive.pYPath[_idx1];
			BLF32 _invlen;
			BLF32 _d = _diff.fX * _diff.fX + _diff.fY * _diff.fY;
			if (_d > 0.0f)
				_invlen = 1.0f / sqrtf(_d);
			else
				_invlen = 1.f;
			_diff.fX *= _invlen;
			_diff.fY *= _invlen;
			_tempnormals[_idx1].fX = _diff.fY;
			_tempnormals[_idx1].fY = -_diff.fX;
		}
		if (!_Node->uExtension.sPrimitive.bClosed)
		{
			_tempnormals[_Node->uExtension.sPrimitive.nPathNum - 1].fX = _tempnormals[_Node->uExtension.sPrimitive.nPathNum - 2].fX;
			_tempnormals[_Node->uExtension.sPrimitive.nPathNum - 1].fY = _tempnormals[_Node->uExtension.sPrimitive.nPathNum - 2].fY;
			_temppoints[0].fX = _Node->uExtension.sPrimitive.pXPath[0] + _tempnormals[0].fX;
			_temppoints[0].fY = _Node->uExtension.sPrimitive.pYPath[0] + _tempnormals[0].fY;
			_temppoints[1].fX = _Node->uExtension.sPrimitive.pXPath[0] - _tempnormals[0].fX;
			_temppoints[1].fY = _Node->uExtension.sPrimitive.pYPath[0] - _tempnormals[0].fY;
			_temppoints[(_Node->uExtension.sPrimitive.nPathNum - 1) * 2 + 0].fX = _Node->uExtension.sPrimitive.pXPath[_Node->uExtension.sPrimitive.nPathNum - 1] + _tempnormals[_Node->uExtension.sPrimitive.nPathNum - 1].fX;
			_temppoints[(_Node->uExtension.sPrimitive.nPathNum - 1) * 2 + 0].fY = _Node->uExtension.sPrimitive.pYPath[_Node->uExtension.sPrimitive.nPathNum - 1] + _tempnormals[_Node->uExtension.sPrimitive.nPathNum - 1].fY;
			_temppoints[(_Node->uExtension.sPrimitive.nPathNum - 1) * 2 + 1].fX = _Node->uExtension.sPrimitive.pXPath[_Node->uExtension.sPrimitive.nPathNum - 1] - _tempnormals[_Node->uExtension.sPrimitive.nPathNum - 1].fX;
			_temppoints[(_Node->uExtension.sPrimitive.nPathNum - 1) * 2 + 1].fY = _Node->uExtension.sPrimitive.pYPath[_Node->uExtension.sPrimitive.nPathNum - 1] - _tempnormals[_Node->uExtension.sPrimitive.nPathNum - 1].fY;
		}
		BLU32 _idx1 = 0;
		for (BLU32 _i1 = 0; _i1 < _count; ++_i1)
		{
			BLU32 _i2 = (_i1 + 1) == _Node->uExtension.sPrimitive.nPathNum ? 0 : _i1 + 1;
			BLU32 _idx2 = (_i1 + 1) == _Node->uExtension.sPrimitive.nPathNum ? 0 : _idx1 + 3;
			BLVec2 _dm;
			_dm.fX = (_tempnormals[_i1].fX + _tempnormals[_i2].fX) * 0.5f;
			_dm.fY = (_tempnormals[_i1].fY + _tempnormals[_i2].fY) * 0.5f;
			BLF32 _dmr2 = _dm.fX * _dm.fX + _dm.fY * _dm.fY;
			if (_dmr2 > 0.000001f)
			{
				BLF32 _scale = 1.0f / _dmr2;
				if (_scale > 100.0f)
					_scale = 100.0f;
				_dm.fX *= _scale;
				_dm.fY *= _scale;
			}
			_temppoints[_i2 * 2 + 0].fX = _Node->uExtension.sPrimitive.pXPath[_i2] + _dm.fX;
			_temppoints[_i2 * 2 + 0].fY = _Node->uExtension.sPrimitive.pYPath[_i2] + _dm.fY;
			_temppoints[_i2 * 2 + 1].fX = _Node->uExtension.sPrimitive.pXPath[_i2] - _dm.fX;
			_temppoints[_i2 * 2 + 1].fY = _Node->uExtension.sPrimitive.pYPath[_i2] - _dm.fY;
			_ib[_fillcount++] = _idx2 + 0;
			_ib[_fillcount++] = _idx1 + 0;
			_ib[_fillcount++] = _idx1 + 2;
			_ib[_fillcount++] = _idx1 + 2;
			_ib[_fillcount++] = _idx2 + 2;
			_ib[_fillcount++] = _idx2 + 0;
			_ib[_fillcount++] = _idx2 + 1;
			_ib[_fillcount++] = _idx1 + 1;
			_ib[_fillcount++] = _idx1 + 0;
			_ib[_fillcount++] = _idx1 + 0;
			_ib[_fillcount++] = _idx2 + 0;
			_ib[_fillcount++] = _idx2 + 1;
			_idx1 = _idx2;
		}
		_fillcount = 0;
		for (BLU32 _idx = 0; _idx < _Node->uExtension.sPrimitive.nPathNum; ++_idx)
		{
			_vb[_fillcount++] = _Node->uExtension.sPrimitive.pXPath[_idx] + _X;
			_vb[_fillcount++] = _Node->uExtension.sPrimitive.pYPath[_idx] + _Y;
			_vb[_fillcount++] = _rgba[0];
			_vb[_fillcount++] = _rgba[1];
			_vb[_fillcount++] = _rgba[2];
			_vb[_fillcount++] = _rgba[3];
			_vb[_fillcount++] = 0.5f;
			_vb[_fillcount++] = 0.5f;
			_vb[_fillcount++] = _temppoints[_idx * 2 + 1].fX + _X;
			_vb[_fillcount++] = _temppoints[_idx * 2 + 1].fY + _Y;
			_vb[_fillcount++] = _rgbat[0];
			_vb[_fillcount++] = _rgbat[1];
			_vb[_fillcount++] = _rgbat[2];
			_vb[_fillcount++] = _rgbat[3];
			_vb[_fillcount++] = 0.5f;
			_vb[_fillcount++] = 0.5f;
			_vb[_fillcount++] = _temppoints[_idx * 2].fX + _X;
			_vb[_fillcount++] = _temppoints[_idx * 2].fY + _Y;
			_vb[_fillcount++] = _rgbat[0];
			_vb[_fillcount++] = _rgbat[1];
			_vb[_fillcount++] = _rgbat[2];
			_vb[_fillcount++] = _rgbat[3];
			_vb[_fillcount++] = 0.5f;
			_vb[_fillcount++] = 0.5f;
		}
	}
	else
	{
		BLU32 _fillcount = 0;
		_idxcount = (_Node->uExtension.sPrimitive.nPathNum - 2) * 3 + _Node->uExtension.sPrimitive.nPathNum * 6;
		_vtxcount = (_Node->uExtension.sPrimitive.nPathNum * 2);
		_vb = (BLF32*)alloca(_vtxcount * 8 * sizeof(BLF32));
		_ib = (BLU32*)alloca(_idxcount * sizeof(BLU32));
		BLU32 _vtxinneridx = 0;
		BLU32 _vtxouteridx = 1;
		for (BLU32 _idx = 2; _idx < _Node->uExtension.sPrimitive.nPathNum; ++_idx)
		{
			_ib[_fillcount++] = ((_vtxinneridx));
			_ib[_fillcount++] = ((_vtxinneridx + ((_idx - 1) << 1)));
			_ib[_fillcount++] = ((_vtxinneridx + (_idx << 1)));
		}
		BLVec2* _tempnormals = (BLVec2*)alloca(_Node->uExtension.sPrimitive.nPathNum * sizeof(BLVec2));
		for (BLU32 _idx0 = _Node->uExtension.sPrimitive.nPathNum - 1, _idx1 = 0; _idx1 < _Node->uExtension.sPrimitive.nPathNum; _idx0 = _idx1++)
		{
			BLVec2 _p0;
			_p0.fX = _Node->uExtension.sPrimitive.pXPath[_idx0];
			_p0.fY = _Node->uExtension.sPrimitive.pYPath[_idx0];
			BLVec2 _p1;
			_p1.fX = _Node->uExtension.sPrimitive.pXPath[_idx1];
			_p1.fY = _Node->uExtension.sPrimitive.pYPath[_idx1];
			BLVec2 _diff;
			_diff.fX = _p1.fX - _p0.fX;
			_diff.fY = _p1.fY - _p0.fY;
			BLF32 _invlen;
			BLF32 _d = _diff.fX * _diff.fX + _diff.fY * _diff.fY;
			if (_d > 0.0f)
				_invlen = 1.0f / sqrtf(_d);
			else
				_invlen = 1.f;
			_diff.fX *= _invlen;
			_diff.fY *= _invlen;
			_tempnormals[_idx0].fX = _diff.fY;
			_tempnormals[_idx0].fY = -_diff.fX;
		}
		BLU32 _fillcount2 = 0;
		for (BLU32 _idx0 = _Node->uExtension.sPrimitive.nPathNum - 1, _idx1 = 0; _idx1 < _Node->uExtension.sPrimitive.nPathNum; _idx0 = _idx1++)
		{
			BLVec2 _n0 = _tempnormals[_idx0];
			BLVec2 _n1 = _tempnormals[_idx1];
			BLVec2 _dm;
			_dm.fX = (_n0.fX + _n1.fX) * 0.5f;
			_dm.fY = (_n0.fY + _n1.fY) * 0.5f;
			BLF32 _dmr2 = _dm.fX * _dm.fX + _dm.fY * _dm.fY;
			if (_dmr2 > 0.000001f)
			{
				BLF32 _scale = 1.0f / _dmr2;
				if (_scale > 100.0f) 
					_scale = 100.0f;
				_dm.fX *= _scale;
				_dm.fY *= _scale;
			}
			_dm.fX *= 0.5f;
			_dm.fY *= 0.5f;
			_vb[_fillcount2++] = _Node->uExtension.sPrimitive.pXPath[_idx1] - _dm.fX + _X;
			_vb[_fillcount2++] = _Node->uExtension.sPrimitive.pYPath[_idx1] - _dm.fY + _Y;
			_vb[_fillcount2++] = _rgba[0];
			_vb[_fillcount2++] = _rgba[1];
			_vb[_fillcount2++] = _rgba[2];
			_vb[_fillcount2++] = _rgba[3];
			_vb[_fillcount2++] = 0.5f;
			_vb[_fillcount2++] = 0.5f;
			_vb[_fillcount2++] = _Node->uExtension.sPrimitive.pXPath[_idx1] + _dm.fX + _X;
			_vb[_fillcount2++] = _Node->uExtension.sPrimitive.pYPath[_idx1] + _dm.fY + _Y;
			_vb[_fillcount2++] = _rgbat[0];
			_vb[_fillcount2++] = _rgbat[1];
			_vb[_fillcount2++] = _rgbat[2];
			_vb[_fillcount2++] = _rgbat[3];
			_vb[_fillcount2++] = 0.5f;
			_vb[_fillcount2++] = 0.5f;
			_ib[_fillcount++] = (_vtxinneridx + (_idx1 << 1));
			_ib[_fillcount++] = (_vtxinneridx + (_idx0 << 1));
			_ib[_fillcount++] = (_vtxouteridx + (_idx0 << 1));
			_ib[_fillcount++] = (_vtxouteridx + (_idx0 << 1));
			_ib[_fillcount++] = (_vtxouteridx + (_idx1 << 1));
			_ib[_fillcount++] = (_vtxinneridx + (_idx1 << 1));
		}
	}
	BLGuid _geo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"@#widgetgeo#@"), BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _vb, _vtxcount * 8 * sizeof(BLF32), _ib, _idxcount * sizeof(BLU32), BL_IF_32);
	blDraw(_PrUIMem->nUITech, _geo, 1);
	blDeleteGeometryBuffer(_geo);
}
static BLVoid
_DrawWidget(_BLWidget* _Node, BLF32 _XPos, BLF32 _YPos)
{
	BLF32 _x, _y, _w, _h;
	if (_Node->pParent)
	{
		BLF32 _pw = _Node->pParent->sDimension.fX > 0.f ? _Node->pParent->sDimension.fX : _PrUIMem->nFboWidth;
		BLF32 _ph = _Node->pParent->sDimension.fY > 0.f ? _Node->pParent->sDimension.fY : _PrUIMem->nFboHeight;
		if (_Node->eReferenceH == BL_UA_LEFT && _Node->eReferenceV == BL_UA_TOP)
		{
			_x = _Node->sPosition.fX - 0.5f * _pw + _XPos;
			_y = _Node->sPosition.fY - 0.5f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_LEFT && _Node->eReferenceV == BL_UA_VCENTER)
		{
			_x = _Node->sPosition.fX - 0.5f * _pw + _XPos;
			_y = _Node->sPosition.fY + 0.0f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_LEFT && _Node->eReferenceV == BL_UA_BOTTOM)
		{
			_x = _Node->sPosition.fX - 0.5f * _pw + _XPos;
			_y = _Node->sPosition.fY + 0.5f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_HCENTER && _Node->eReferenceV == BL_UA_TOP)
		{
			_x = _Node->sPosition.fX + 0.0f * _pw + _XPos;
			_y = _Node->sPosition.fY - 0.5f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_HCENTER && _Node->eReferenceV == BL_UA_VCENTER)
		{
			_x = _Node->sPosition.fX + 0.0f * _pw + _XPos;
			_y = _Node->sPosition.fY + 0.0f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_HCENTER && _Node->eReferenceV == BL_UA_BOTTOM)
		{
			_x = _Node->sPosition.fX + 0.0f * _pw + _XPos;
			_y = _Node->sPosition.fY + 0.5f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_RIGHT && _Node->eReferenceV == BL_UA_TOP)
		{
			_x = _Node->sPosition.fX + 0.5f * _pw + _XPos;
			_y = _Node->sPosition.fY - 0.5f * _ph + _YPos;
		}
		else if (_Node->eReferenceH == BL_UA_RIGHT && _Node->eReferenceV == BL_UA_VCENTER)
		{
			_x = _Node->sPosition.fX + 0.5f * _pw + _XPos;
			_y = _Node->sPosition.fY + 0.0f * _ph + _YPos;
		}
		else
		{
			_x = _Node->sPosition.fX + 0.5f * _pw + _XPos;
			_y = _Node->sPosition.fY + 0.5f * _ph + _YPos;
		}
		if (_Node->ePolicy == BL_UP_FIXED)
		{
			_w = _Node->sDimension.fX;
			_h = _Node->sDimension.fY;
		}
		else if (_Node->ePolicy == BL_UP_HMatch)
		{
			_w = _pw;
			_h = _pw / _Node->fRatio;
		}
		else if (_Node->ePolicy == BL_UP_VMatch)
		{
			_w = _ph * _Node->fRatio;
			_h = _ph;
		}
		else
		{
			_w = _pw;
			_h = _ph;
		}
		switch (_Node->eType)
		{
		case BL_UT_PANEL:
			_DrawPanel(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_LABEL:
			_DrawLabel(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_BUTTON:
			_DrawButton(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_CHECK:
			_DrawCheck(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_SLIDER:
			_DrawSlider(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_TEXT:
			_DrawText(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_PROGRESS:
			_DrawProgress(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_DIAL:
			_DrawDial(_Node, _x, _y, _w, _h);
			break;
		case BL_UT_TABLE:
			_DrawTable(_Node, _x, _y, _w, _h);
			break;
		default:
			_DrawPrimitive(_Node, _x, _y, _w, _h);
			break;
		}
	}
	else
	{
		_x = _PrUIMem->nFboWidth * 0.5f;
		_y = _PrUIMem->nFboHeight * 0.5f;
	}
	FOREACH_ARRAY (_BLWidget*, _iter, _Node->pChildren)
	{
		_DrawWidget(_iter, _x, _y);
	}
}
static const BLVoid
_MouseSubscriber(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam)
{
	if (_Type == BL_ME_MOVE)
	{
		_BLWidget* _lasthovered = _PrUIMem->pHoveredWidget;
		BLU32 _oriw, _orih, _aw, _ah;
		BLF32 _rx, _ry;
		blGetWindowSize(&_oriw, &_orih, &_aw, &_ah, &_rx, &_ry);
		BLF32 _x = (BLF32)LOWU16(_UParam) * _rx;
		BLF32 _y = (BLF32)HIGU16(_UParam) * _ry;
		_PrUIMem->pHoveredWidget = _LocateWidget(_PrUIMem->pRoot, _x, _y, 0.f, 0.f);
		if (_PrUIMem->pHoveredWidget != _lasthovered)
		{
			if (_lasthovered && _lasthovered->bInteractive)
			{
				//	lasthovered->_sig_leave.emit();
				if (_lasthovered->eType == BL_UT_BUTTON)
				{
					if (_lasthovered->uExtension.sButton.nState)
					{
						_lasthovered->uExtension.sButton.nState = 1;
						_PrUIMem->bDirty = TRUE;
					}
				}
			}
			if (_PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget->bInteractive)
			{
				//m_hovered_widget->_sig_enter.emit();
				if (_PrUIMem->pHoveredWidget->eType == BL_UT_BUTTON)
				{
					if (_PrUIMem->pHoveredWidget->uExtension.sButton.nState)
					{
						_PrUIMem->pHoveredWidget->uExtension.sButton.nState = 2;
						_PrUIMem->bDirty = TRUE;
					}
				}
			}
		}
		if (_PrUIMem->pFocusWidget && _PrUIMem->pFocusWidget->bVisible && _PrUIMem->pFocusWidget->bInteractive)
		{
			_BLWidget* _wid = _PrUIMem->pFocusWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			switch (_wid->eType)
			{
			default:break;
			}
		}
		if (!_PrUIMem->pFocusWidget && _PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget->bVisible && _PrUIMem->pHoveredWidget->bInteractive)
		{
			_BLWidget* _wid = _PrUIMem->pHoveredWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			switch (_wid->eType)
			{
			default:break;
			}
		}
	}
	else if (_Type == BL_ME_LDOWN || _Type == BL_ME_RDOWN)
	{
		_BLWidget* _lasthovered = _PrUIMem->pHoveredWidget;
		BLU32 _oriw, _orih, _aw, _ah;
		BLF32 _rx, _ry;
		blGetWindowSize(&_oriw, &_orih, &_aw, &_ah, &_rx, &_ry);
		BLF32 _x = (BLF32)LOWU16(_UParam) * _rx;
		BLF32 _y = (BLF32)HIGU16(_UParam) * _ry;
		_PrUIMem->pHoveredWidget = _LocateWidget(_PrUIMem->pRoot, _x, _y, 0.f, 0.f);
		if (_PrUIMem->pHoveredWidget != _lasthovered)
		{
			if (_lasthovered && _lasthovered->bInteractive)
			{
				//	lasthovered->_sig_leave.emit();
				if (_lasthovered->eType == BL_UT_BUTTON)
				{
					if (_lasthovered->uExtension.sButton.nState)
					{
						_lasthovered->uExtension.sButton.nState = 1;
						_PrUIMem->bDirty = TRUE;
					}
				}
			}
			if (_PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget->bInteractive)
			{
				//m_hovered_widget->_sig_enter.emit();
				if (_PrUIMem->pHoveredWidget->eType == BL_UT_BUTTON)
				{
					if (_PrUIMem->pHoveredWidget->uExtension.sButton.nState)
					{
						_PrUIMem->pHoveredWidget->uExtension.sButton.nState = 2;
						_PrUIMem->bDirty = TRUE;
					}
				}
			}
		}
		if ((_PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget != _PrUIMem->pFocusWidget) || !_PrUIMem->pFocusWidget)
		{
			if (_PrUIMem->pFocusWidget != _PrUIMem->pHoveredWidget)
			{
				if (_PrUIMem->pHoveredWidget == _PrUIMem->pRoot)
					_PrUIMem->pHoveredWidget = NULL;
				if (_PrUIMem->pFocusWidget)
				{
					//lost focs;_PrUIMem->pFocusWidget
				}
				if (_PrUIMem->pHoveredWidget)
				{
					//gain focus;_PrUIMem->pHoveredWidget
				}
				_PrUIMem->pFocusWidget = _PrUIMem->pHoveredWidget;
			}
		}
		_BLWidget* _lastfocus = _PrUIMem->pFocusWidget;
		if (_PrUIMem->pFocusWidget && _PrUIMem->pFocusWidget->bVisible && _PrUIMem->pFocusWidget->bInteractive)
		{
			_BLWidget* _wid = _PrUIMem->pFocusWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			switch (_wid->eType)
			{
			case BL_UT_BUTTON:
				if (_wid->uExtension.sButton.nState)
				{
					_wid->uExtension.sButton.nState = 3;
					_PrUIMem->bDirty = TRUE;
				}
			default:break;
			}
		}
		if (!_lastfocus && _PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget->bVisible && _PrUIMem->pHoveredWidget->bInteractive)
		{
			_BLWidget* _wid = _PrUIMem->pHoveredWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			switch (_wid->eType)
			{
			case BL_UT_BUTTON:
				if (_wid->uExtension.sButton.nState)
				{
					_wid->uExtension.sButton.nState = 3;
					_PrUIMem->bDirty = TRUE;
				}
			default:break;
			}
		}
	}
	else if (_Type == BL_ME_LUP || _Type == BL_ME_RUP)
	{
		_BLWidget* _lastfocus = _PrUIMem->pFocusWidget;
		if (_PrUIMem->pFocusWidget && _PrUIMem->pFocusWidget->bVisible && _PrUIMem->pFocusWidget->bInteractive)
		{
			_BLWidget* _wid = _PrUIMem->pFocusWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			switch (_wid->eType)
			{
			case BL_UT_BUTTON:
				if (_wid->uExtension.sButton.nState)
				{
					_wid->uExtension.sButton.nState = 1;
					_PrUIMem->bDirty = TRUE;
				}
			default:break;
			}
		}
		if (!_lastfocus && _PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget->bVisible && _PrUIMem->pHoveredWidget->bInteractive)
		{
			_BLWidget* _wid = _PrUIMem->pHoveredWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			switch (_wid->eType)
			{
			case BL_UT_BUTTON:
				if (_wid->uExtension.sButton.nState)
				{
					_wid->uExtension.sButton.nState = 1;
					_PrUIMem->bDirty = TRUE;
				}
			default:break;
			}
		}
	}
}
static const BLVoid
_KeyboardSubscriber(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam)
{
	if (_Type == BL_ME_MOVE)
	{
	}
}
BLVoid
_UIInit()
{
    _PrUIMem = (_BLUIMember*)malloc(sizeof(_BLUIMember));
    _PrUIMem->pFonts = blGenArray(FALSE);
    memset(_PrUIMem->aDir, 0, sizeof(_PrUIMem->aDir));
    memset(_PrUIMem->aArchive, 0, sizeof(_PrUIMem->aArchive));
    FT_Init_FreeType(&_PrUIMem->sFtLibrary);
	_PrUIMem->pBasePlate = NULL;
    _PrUIMem->pRoot = (_BLWidget*)malloc(sizeof(_BLWidget));
    memset(_PrUIMem->pRoot, 0, sizeof(_BLWidget));
    _PrUIMem->pRoot->sDimension.fX = -1.f;
    _PrUIMem->pRoot->sDimension.fY = -1.f;
	_PrUIMem->pRoot->sPosition.fX = 0.f;
	_PrUIMem->pRoot->sPosition.fY = 0.f;
	_PrUIMem->pRoot->pParent = NULL;
	_PrUIMem->pRoot->eReferenceH = BL_UA_HCENTER;
	_PrUIMem->pRoot->eReferenceV = BL_UA_VCENTER;
	_PrUIMem->pRoot->pChildren = blGenArray(FALSE);
	_PrUIMem->pRoot->eType = BL_UT_PANEL;
	_PrUIMem->pRoot->uExtension.sPanel.nPixmapTex = INVALID_GUID;
	_PrUIMem->pRoot->nID = blGenGuid(_PrUIMem->pRoot, blHashUtf8((const BLUtf8*)"Root"));
	_PrUIMem->pHoveredWidget = NULL;
	_PrUIMem->pFocusWidget = NULL;
    _PrUIMem->pFonts = blGenArray(TRUE);
	_PrUIMem->nUITech = blGenTechnique("2D.bsl", NULL, FALSE, FALSE);
	BLU32 _blankdata[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
	_PrUIMem->nBlankTex = blGenTexture(blHashUtf8((const BLUtf8*)"#@blanktex#@"), BL_TT_2D, BL_TF_RGBA8, FALSE, TRUE, FALSE, 1, 1, 4, 4, 1, (BLU8*)_blankdata);
	BLU32 _width, _height;
	BLF32 _rx, _ry;
    blGetWindowSize(&_width, &_height, &_PrUIMem->nFboWidth, &_PrUIMem->nFboHeight, &_rx, &_ry);
	blSubscribeEvent(BL_ET_MOUSE, _MouseSubscriber);
	blSubscribeEvent(BL_ET_KEY, _KeyboardSubscriber);
}
BLVoid
_UIStep(BLU32 _Delta, BLBool _Baseplate)
{
	BLF32 _screensz[2] = { 2.f / (BLF32)_PrUIMem->nFboWidth, 2.f / (BLF32)_PrUIMem->nFboHeight };
	blTechUniform(_PrUIMem->nUITech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
	if (_Baseplate)
	{
		BLF32 _x, _y, _w, _h;
		_BLWidget* _node = _PrUIMem->pBasePlate;
        if (!_node)
            return;
		_x = _node->sPosition.fX;
		_y = _node->sPosition.fY;
		do
		{
			BLF32 _pw = _node->pParent->sDimension.fX > 0.f ? _node->pParent->sDimension.fX : _PrUIMem->nFboWidth;
			BLF32 _ph = _node->pParent->sDimension.fY > 0.f ? _node->pParent->sDimension.fY : _PrUIMem->nFboHeight;
			BLF32 _px = _node->pParent ? _node->pParent->sPosition.fX : 0.f;
			BLF32 _py = _node->pParent ? _node->pParent->sPosition.fY : 0.f;
			if (_node->eReferenceH == BL_UA_LEFT && _node->eReferenceV == BL_UA_TOP)
			{
				_x += 0.5f * _pw + _px;
				_y += 0.5f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_LEFT && _node->eReferenceV == BL_UA_VCENTER)
			{
				_x += 0.5f * _pw + _px;
				_y += 0.0f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_LEFT && _node->eReferenceV == BL_UA_BOTTOM)
			{
				_x += 0.5f * _pw + _px;
				_y += -0.5f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_HCENTER && _node->eReferenceV == BL_UA_TOP)
			{
				_x += 0.0f * _pw + _px;
				_y += 0.5f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_HCENTER && _node->eReferenceV == BL_UA_VCENTER)
			{
				_x += 0.0f * _pw + _px;
				_y += 0.0f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_HCENTER && _node->eReferenceV == BL_UA_BOTTOM)
			{
				_x += 0.0f * _pw + _px;
				_y += -0.5f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_RIGHT && _node->eReferenceV == BL_UA_TOP)
			{
				_x += 0.5f * _pw + _px;
				_y += 0.5f * _ph + _py;
			}
			else if (_node->eReferenceH == BL_UA_RIGHT && _node->eReferenceV == BL_UA_VCENTER)
			{
				_x += 0.5f * _pw + _px;
				_y += 0.0f * _ph + _py;
			}
			else
			{
				_x += 0.5f * _pw + _px;
				_y += -0.5f * _ph + _py;
			}
			_node = _node->pParent;
		} while (_node);
		BLF32 _pw = _PrUIMem->pBasePlate->pParent->sDimension.fX > 0.f ? _PrUIMem->pBasePlate->pParent->sDimension.fX : _PrUIMem->nFboWidth;
		BLF32 _ph = _PrUIMem->pBasePlate->pParent->sDimension.fY > 0.f ? _PrUIMem->pBasePlate->pParent->sDimension.fY : _PrUIMem->nFboHeight;
		if (_PrUIMem->pBasePlate->ePolicy == BL_UP_FIXED)
		{
			_w = _pw;
			_h = _ph;
		}
		else if (_PrUIMem->pBasePlate->ePolicy == BL_UP_HMatch)
		{
			_w = _pw;
			_h = _pw / _PrUIMem->pBasePlate->fRatio;
		}
		else if (_PrUIMem->pBasePlate->ePolicy == BL_UP_VMatch)
		{
			_w = _ph * _PrUIMem->pBasePlate->fRatio;
			_h = _ph;
		}
		else
		{
			_w = _pw;
			_h = _ph;
		}
	}
	else
	{
		//if (_PrUIMem->bDirty)
		{
			_DrawWidget(_PrUIMem->pRoot, 0.f, 0.f);
			_PrUIMem->bDirty = FALSE;
		}
		///draw quad
	}
}
BLVoid
_UIDestroy()
{
	blUnsubscribeEvent(BL_ET_KEY, _KeyboardSubscriber);
	blUnsubscribeEvent(BL_ET_MOUSE, _MouseSubscriber);
	blDeleteTexture(_PrUIMem->nBlankTex);
	blDeleteTechnique(_PrUIMem->nUITech);
	blDeleteUI(_PrUIMem->pRoot->nID);
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			FT_Done_Face(_iter->sFtFace);
			{
				FOREACH_DICT(_BLGlyphAtlas*, _iter2, _iter->pGlyphAtlas)
				{
					FOREACH_ARRAY(BLGuid*, _iter3, _iter2->pTextures)
					{
						blDeleteTexture(*_iter3);
						free(_iter3);
					}
					blDeleteArray(_iter2->pTextures);
					FOREACH_DICT(_BLGlyph*, _iter4, _iter2->pGlyphs)
					{
						free(_iter4);
					}
					blDeleteDict(_iter2->pGlyphs);
				}
			}
			blDeleteStream(_iter->nFontStream);
			free(_iter);
		}
		blDeleteArray(_PrUIMem->pFonts);
	}
	FT_Done_FreeType(_PrUIMem->sFtLibrary);
	free(_PrUIMem);
}
BLVoid
blUIWorkspace(IN BLAnsi* _Dictionary, IN BLAnsi* _Archive)
{
	strcpy(_PrUIMem->aDir, _Dictionary);
	if (_Archive)
		strcpy(_PrUIMem->aArchive, _Archive);
	BLAnsi _path[260] = { 0 };
	BLGuid _stream;
	if (_Archive)
	{
		strcpy(_path, _Dictionary);
		strcat(_path, "/ui.uws");
		_stream = blGenStream(_path, _Archive);
	}
	else
	{
		strcpy(_path, blWorkingDir(TRUE));
		strcat(_path, _Dictionary);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		strcat(_path, "\\ui.uws");
#else
		strcat(_path, "/ui.uws");
#endif
		_stream = blGenStream(_path, _Archive);
	}
	ezxml_t _doc = ezxml_parse_str(blStreamData(_stream), blStreamLength(_stream));
	ezxml_t _resolution = ezxml_child(_doc, "Resolution");
	ezxml_t _caretcolor = ezxml_child(_doc, "CaretColor");
	const BLAnsi* _car = ezxml_attr(_caretcolor, "Value");
	_PrUIMem->nCaretColor = (BLU32)strtoul(_car, NULL, 10);
	ezxml_t _selectrangecolor = ezxml_child(_doc, "SelectRangeColor");
	const BLAnsi* _sel = ezxml_attr(_selectrangecolor, "Value");
	_PrUIMem->nSelectRangeColor = (BLU32)strtoul(_sel, NULL, 10);
	ezxml_t _textdisablecolor = ezxml_child(_doc, "TextDisableColor");
	const BLAnsi* _txt = ezxml_attr(_textdisablecolor, "Value");
	_PrUIMem->nTextDisableColor = (BLU32)strtoul(_txt, NULL, 10);
	ezxml_free(_doc);
	blDeleteStream(_stream);
}
BLVoid
blUIFile(IN BLAnsi* _Filename)
{
    BLAnsi _path[260] = { 0 };
    if (_PrUIMem->aArchive[0] == 0)
    {
        strcpy(_path, blWorkingDir(TRUE));
        strcat(_path, _PrUIMem->aDir);
    }
    else
        strcpy(_path, _PrUIMem->aDir);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
    strcat(_path, "\\bui\\");
#else
    strcat(_path, "/bui/");
#endif
    strcat(_path, _Filename);
    BLGuid _layout = blGenStream(_path, _PrUIMem->aArchive[0] ? _PrUIMem->aArchive : NULL);
    ezxml_t _doc = ezxml_parse_str(blStreamData(_layout), blStreamLength(_layout));
    ezxml_t _element = ezxml_child(_doc, "Element");
    BLU32 _idx = 0;
    BLAnsi* _tmp;
    do {
        const BLAnsi* _rpt = ezxml_attr(_element, "ReferencePt");
        BLEnum _ha, _va;
        if (!strcmp(_rpt, "LT")) { _ha = BL_UA_LEFT; _va = BL_UA_TOP; }
        else if (!strcmp(_rpt, "LM")) { _ha = BL_UA_LEFT; _va = BL_UA_VCENTER; }
        else if (!strcmp(_rpt, "LB")) { _ha = BL_UA_LEFT; _va = BL_UA_BOTTOM; }
        else if (!strcmp(_rpt, "MT")) { _ha = BL_UA_HCENTER; _va = BL_UA_TOP; }
        else if (!strcmp(_rpt, "MM")) { _ha = BL_UA_HCENTER; _va = BL_UA_VCENTER; }
        else if (!strcmp(_rpt, "MB")) { _ha = BL_UA_HCENTER; _va = BL_UA_BOTTOM; }
        else if (!strcmp(_rpt, "RT")) { _ha = BL_UA_RIGHT; _va = BL_UA_TOP; }
        else if (!strcmp(_rpt, "RM")) { _ha = BL_UA_RIGHT; _va = BL_UA_VCENTER; }
        else { _ha = BL_UA_RIGHT; _va = BL_UA_BOTTOM; }
        const BLAnsi* _geo = ezxml_attr(_element, "Geometry");
        _idx = 0;
        BLS32 _geovar[4];
        _tmp = strtok((BLAnsi*)_geo, ",");
        while (_tmp)
        {
            _geovar[_idx] = (BLS32)strtol(_tmp, NULL, 10);
            _tmp = strtok(NULL, ",");
            _idx++;
        }
		_geovar[2] = _geovar[2] - _geovar[0];
		_geovar[3] = _geovar[3] - _geovar[1];
        const BLAnsi* _name = ezxml_attr(_element, "Name");
        BLEnum _typevar;
        const BLAnsi* _type = ezxml_attr(_element, "Type");
        const BLAnsi* _parent = ezxml_attr(_element, "Parent");
        BLU32 _parentvar = (BLU32)strtoul(_parent, NULL, 10);
        const BLAnsi* _policy = ezxml_attr(_element, "Policy");
        BLEnum _policyvar;
        if (!strcmp(_policy, "Fixed"))
            _policyvar = BL_UP_FIXED;
        else if (!strcmp(_policy, "V-Match"))
            _policyvar = BL_UP_VMatch;
        else if (!strcmp(_policy, "H-Match"))
            _policyvar = BL_UP_HMatch;
        else
            _policyvar = BL_UP_STRETCH;
        _element = ezxml_child(_element, "Base");
        const BLAnsi* _penetration = ezxml_attr(_element, "Penetration");
        BLBool _penetrationvar = strcmp(_penetration, "true") ? FALSE : TRUE;
        const BLAnsi* _cliped = ezxml_attr(_element, "Cliped");
        BLBool _clipedvar = strcmp(_cliped, "true") ? FALSE : TRUE;
        const BLAnsi* _abs = ezxml_attr(_element, "AbsScissor");
        BLBool _absvar = strcmp(_abs, "true") ? FALSE : TRUE;
        const BLAnsi* _tooltip = ezxml_attr(_element, "Tooltip");
        const BLAnsi* _minsize = ezxml_attr(_element, "MinSize");
        BLF32 _minsizevar[2];
        _tmp = strtok((BLAnsi*)_minsize, ",");
        _minsizevar[0] = (BLF32)strtod(_tmp, NULL);
        _minsizevar[1] = (BLF32)strtod(strtok(NULL, ","), NULL);
        const BLAnsi* _maxsize = ezxml_attr(_element, "MaxSize");
        BLF32 _maxsizevar[2];
        _tmp = strtok((BLAnsi*)_maxsize, ",");
        _maxsizevar[0] = (BLF32)strtod(_tmp, NULL);
        _maxsizevar[1] = (BLF32)strtod(strtok(NULL, ","), NULL);
        _element = _element->sibling;
        if (strcmp(ezxml_name(_element), "Ext"))
            continue;
        if (!strcmp(_type, "Panel"))
        {
            _typevar = BL_UT_PANEL;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _dragable = ezxml_attr(_element, "Dragable");
            BLBool _dragablevar = strcmp(_dragable, "true") ? FALSE : TRUE;
            const BLAnsi* _modal = ezxml_attr(_element, "Modal");
            BLBool _modalvar = strcmp(_modal, "true") ? FALSE : TRUE;
            const BLAnsi* _baseplate = ezxml_attr(_element, "BasePlate");
            BLBool _baseplatevar = strcmp(_baseplate, "true") ? FALSE : TRUE;
            const BLAnsi* _scrollable = ezxml_attr(_element, "Scrollable");
            BLBool _scrollablevar = strcmp(_scrollable, "true") ? FALSE : TRUE;
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
            blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_PANEL);
        }
        else if (!strcmp(_type, "Button"))
        {
            _typevar = BL_UT_BUTTON;
            const BLAnsi* _enable = ezxml_attr(_element, "Enable");
            BLBool _enablevar = strcmp(_enable, "true") ? FALSE : TRUE;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _text = ezxml_attr(_element, "Text");
            BLEnum _txtha, _txtva;
            const BLAnsi* _alignmenth = ezxml_attr(_element, "AlignmentH");
            if (!strcmp(_alignmenth, "Left"))
                _txtha = BL_UA_LEFT;
            else if (!strcmp(_alignmenth, "Right"))
                _txtha = BL_UA_RIGHT;
            else
                _txtha = BL_UA_HCENTER;
            const BLAnsi* _alignmentv = ezxml_attr(_element, "AlignmentV");
            if (!strcmp(_alignmentv, "Top"))
                _txtva = BL_UA_TOP;
            else if (!strcmp(_alignmentv, "Bottom"))
                _txtva = BL_UA_BOTTOM;
            else
                _txtva = BL_UA_VCENTER;
            const BLAnsi* _fontsrc = ezxml_attr(_element, "FontSrc");
            const BLAnsi* _fontsize = ezxml_attr(_element, "FontSize");
            BLU32 _fontsizevar = (BLU32)strtoul(_fontsize, NULL, 10);
            const BLAnsi* _fontoutline = ezxml_attr(_element, "FontOutline");
            BLBool _fontoutlinevar = strcmp(_fontoutline, "true") ? FALSE : TRUE;
            const BLAnsi* _fontbold = ezxml_attr(_element, "FontBold");
            BLBool _fontboldvar = strcmp(_fontbold, "true") ? FALSE : TRUE;
            const BLAnsi* _fontshadow = ezxml_attr(_element, "FontShadow");
            BLBool _fontshadowvar = strcmp(_fontshadow, "true") ? FALSE : TRUE;
            const BLAnsi* _fontitalics = ezxml_attr(_element, "FontItalics");
            BLBool _fontitalicsvar = strcmp(_fontitalics, "true") ? FALSE : TRUE;
            const BLAnsi* _textcolor = ezxml_attr(_element, "TextColor");
            BLU32 _textcolorvar = (BLU32)strtoul(_textcolor, NULL, 10);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _hoveredmap = ezxml_attr(_element, "HoveredMap");
            const BLAnsi* _hoveredtexcoord = ezxml_attr(_element, "HoveredTexcoord");
            BLF32 _hoveredtexcoordvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_hoveredtexcoord, ",");
            while (_tmp)
            {
                _hoveredtexcoordvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _pressedmap = ezxml_attr(_element, "PressedMap");
            const BLAnsi* _pressedtexcoord = ezxml_attr(_element, "PressedTexcoord");
            BLF32 _pressedtexcoordvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_pressedtexcoord, ",");
            while (_tmp)
            {
                _pressedtexcoordvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _disablemap = ezxml_attr(_element, "DisableMap");
            const BLAnsi* _disabletexcoord = ezxml_attr(_element, "DisableTexcoord");
            BLF32 _disabletexcoordvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_disabletexcoord, ",");
            while (_tmp)
            {
                _disabletexcoordvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
            BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_BUTTON);
            blUIReferencePoint(_widguid, _ha, _va);
            blUISizePolicy(_widguid, _policyvar);
            blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
            blUIScissor(_widguid, _clipedvar, _absvar);
            blUITooltip(_widguid, (const BLUtf8*)_tooltip);
            blUIPenetration(_widguid, _penetrationvar);
			blUIButtonFont(_widguid, _fontsrc, _fontsizevar, _fontoutlinevar, _fontboldvar, _fontshadowvar, _fontitalicsvar);
			blUIButtonFlip(_widguid, _flipxvar, _flipyvar);
			blUIButtonText(_widguid, (const BLUtf8*)_text, _textcolorvar, _txtha, _txtva);
            blUIButtonPixmap(_widguid, _pixmap);
            blUIButtonStencilMap(_widguid, _stencilmap);
            blUIButtonCommonMap(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
            blUIButtonHoverMap(_widguid, _hoveredmap, _hoveredtexcoordvar[0], _hoveredtexcoordvar[1], _hoveredtexcoordvar[2], _hoveredtexcoordvar[3]);
            blUIButtonPressMap(_widguid, _pressedmap, _pressedtexcoordvar[0], _pressedtexcoordvar[1], _pressedtexcoordvar[2], _pressedtexcoordvar[3]);
            blUIButtonDisableMap(_widguid, _disablemap, _disabletexcoordvar[0], _disabletexcoordvar[1], _disabletexcoordvar[2], _disabletexcoordvar[3]);
            blUIButtonEnable(_widguid, _enablevar);
        }
        else if (!strcmp(_type, "Label"))
        {
            _typevar = BL_UT_LABEL;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _text = ezxml_attr(_element, "Text");
            const BLAnsi* _padding = ezxml_attr(_element, "Padding");
            BLF32 _paddingvar[2];
            _tmp = strtok((BLAnsi*)_padding, ",");
            _paddingvar[0] = (BLF32)strtod(_tmp, NULL);
            _paddingvar[1] = (BLF32)strtod(strtok(NULL, ","), NULL);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Check"))
        {
            _typevar = BL_UT_CHECK;
            const BLAnsi* _enable = ezxml_attr(_element, "Enable");
            BLBool _enablevar = strcmp(_enable, "true") ? FALSE : TRUE;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _text = ezxml_attr(_element, "Text");
            BLEnum _txtha, _txtva;
            const BLAnsi* _alignmenth = ezxml_attr(_element, "AlignmentH");
            if (!strcmp(_alignmenth, "Left"))
                _txtha = BL_UA_LEFT;
            else if (!strcmp(_alignmenth, "Right"))
                _txtha = BL_UA_RIGHT;
            else
                _txtha = BL_UA_HCENTER;
            const BLAnsi* _alignmentv = ezxml_attr(_element, "AlignmentV");
            if (!strcmp(_alignmentv, "Top"))
                _txtva = BL_UA_LEFT;
            else if (!strcmp(_alignmentv, "Bottom"))
                _txtva = BL_UA_RIGHT;
            else
                _txtva = BL_UA_HCENTER;
            const BLAnsi* _fontsrc = ezxml_attr(_element, "FontSrc");
            const BLAnsi* _fontsize = ezxml_attr(_element, "FontSize");
            BLU32 _fontsizevar = (BLU32)strtoul(_fontsize, NULL, 10);
            const BLAnsi* _fontoutline = ezxml_attr(_element, "FontOutline");
            BLBool _fontoutlinevar = strcmp(_fontoutline, "true") ? FALSE : TRUE;
            const BLAnsi* _fontbold = ezxml_attr(_element, "FontBold");
            BLBool _fontboldvar = strcmp(_fontbold, "true") ? FALSE : TRUE;
            const BLAnsi* _fontshadow = ezxml_attr(_element, "FontShadow");
            BLBool _fontshadowvar = strcmp(_fontshadow, "true") ? FALSE : TRUE;
            const BLAnsi* _fontitalics = ezxml_attr(_element, "FontItalics");
            BLBool _fontitalicsvar = strcmp(_fontitalics, "true") ? FALSE : TRUE;
            const BLAnsi* _textcolor = ezxml_attr(_element, "TextColor");
            BLU32 _textcolorvar = (BLU32)strtoul(_textcolor, NULL, 10);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _checkedmap = ezxml_attr(_element, "CheckedMap");
            const BLAnsi* _checkedtexcoord = ezxml_attr(_element, "CheckedTexcoord");
            BLF32 _checkedtexcoordvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_checkedtexcoord, ",");
            while (_tmp)
            {
                _checkedtexcoordvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _disablemap = ezxml_attr(_element, "DisableMap");
            const BLAnsi* _disabletexcoord = ezxml_attr(_element, "DisableTexcoord");
            BLF32 _disabletexcoordvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_disabletexcoord, ",");
            while (_tmp)
            {
                _disabletexcoordvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Text"))
        {
            _typevar = BL_UT_TEXT;
            const BLAnsi* _enable = ezxml_attr(_element, "Enable");
            BLBool _enablevar = strcmp(_enable, "true") ? FALSE : TRUE;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _wordwrap = ezxml_attr(_element, "Wordwrap");
            BLBool _wordwrapvar = strcmp(_wordwrap, "true") ? FALSE : TRUE;
            const BLAnsi* _numeric = ezxml_attr(_element, "Numeric");
            BLBool _numericvar = strcmp(_numeric, "true") ? FALSE : TRUE;
            const BLAnsi* _password = ezxml_attr(_element, "Password");
            BLBool _passwordvar = strcmp(_password, "true") ? FALSE : TRUE;
            const BLAnsi* _autoscroll = ezxml_attr(_element, "Autoscroll");
            BLBool _autoscrollvar = strcmp(_autoscroll, "true") ? FALSE : TRUE;
            const BLAnsi* _multiline = ezxml_attr(_element, "Multiline");
            BLBool _multilinevar = strcmp(_multiline, "true") ? FALSE : TRUE;
            const BLAnsi* _maxlength = ezxml_attr(_element, "MaxLength");
            BLU32 _maxlengthvar = (BLU32)strtoul(_maxlength, NULL, 10);
            const BLAnsi* _numericrange = ezxml_attr(_element, "NumericRange");
            BLS32 _numericrangevar[2];
            _tmp = strtok((BLAnsi*)_numericrange, ",");
            _numericrangevar[0] = (BLS32)strtol(_tmp, NULL, 10);
            _numericrangevar[1] = (BLS32)strtol(strtok(NULL, ","), NULL, 10);
            const BLAnsi* _placeholder = ezxml_attr(_element, "Placeholder");
            BLEnum _txtha, _txtva;
            const BLAnsi* _alignmenth = ezxml_attr(_element, "AlignmentH");
            if (!strcmp(_alignmenth, "Left"))
                _txtha = BL_UA_LEFT;
            else if (!strcmp(_alignmenth, "Right"))
                _txtha = BL_UA_RIGHT;
            else
                _txtha = BL_UA_HCENTER;
            const BLAnsi* _alignmentv = ezxml_attr(_element, "AlignmentV");
            if (!strcmp(_alignmentv, "Top"))
                _txtva = BL_UA_LEFT;
            else if (!strcmp(_alignmentv, "Bottom"))
                _txtva = BL_UA_RIGHT;
            else
                _txtva = BL_UA_HCENTER;
            const BLAnsi* _fontsrc = ezxml_attr(_element, "FontSrc");
            const BLAnsi* _fontsize = ezxml_attr(_element, "FontSize");
            BLU32 _fontsizevar = (BLU32)strtoul(_fontsize, NULL, 10);
            const BLAnsi* _fontoutline = ezxml_attr(_element, "FontOutline");
            BLBool _fontoutlinevar = strcmp(_fontoutline, "true") ? FALSE : TRUE;
            const BLAnsi* _fontbold = ezxml_attr(_element, "FontBold");
            BLBool _fontboldvar = strcmp(_fontbold, "true") ? FALSE : TRUE;
            const BLAnsi* _fontshadow = ezxml_attr(_element, "FontShadow");
            BLBool _fontshadowvar = strcmp(_fontshadow, "true") ? FALSE : TRUE;
            const BLAnsi* _fontitalics = ezxml_attr(_element, "FontItalics");
            BLBool _fontitalicsvar = strcmp(_fontitalics, "true") ? FALSE : TRUE;
            const BLAnsi* _textcolor = ezxml_attr(_element, "TextColor");
            BLU32 _textcolorvar = (BLU32)strtoul(_textcolor, NULL, 10);
            const BLAnsi* _padding = ezxml_attr(_element, "Padding");
            BLF32 _paddingvar[2];
            _tmp = strtok((BLAnsi*)_padding, ",");
            _paddingvar[0] = (BLF32)strtod(_tmp, NULL);
            _paddingvar[1] = (BLF32)strtod(strtok(NULL, ","), NULL);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Progress"))
        {
            _typevar = BL_UT_PROGRESS;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _percent = ezxml_attr(_element, "Percent");
            BLU32 _percentvar = (BLU32)strtoul(_percent, NULL, 10);
            const BLAnsi* _border = ezxml_attr(_element, "Border");
            BLF32 _bordevarr[2];
            _tmp = strtok((BLAnsi*)_border, ",");
            _bordevarr[0] = (BLF32)strtod(_tmp, NULL);
            _bordevarr[1] = (BLF32)strtod(strtok(NULL, ","), NULL);
            const BLAnsi* _text = ezxml_attr(_element, "Text");
            const BLAnsi* _fontsrc = ezxml_attr(_element, "FontSrc");
            const BLAnsi* _fontsize = ezxml_attr(_element, "FontSize");
            BLU32 _fontsizevar = (BLU32)strtoul(_fontsize, NULL, 10);
            const BLAnsi* _fontoutline = ezxml_attr(_element, "FontOutline");
            BLBool _fontoutlinevar = strcmp(_fontoutline, "true") ? FALSE : TRUE;
            const BLAnsi* _fontbold = ezxml_attr(_element, "FontBold");
            BLBool _fontboldvar = strcmp(_fontbold, "true") ? FALSE : TRUE;
            const BLAnsi* _fontshadow = ezxml_attr(_element, "FontShadow");
            BLBool _fontshadowvar = strcmp(_fontshadow, "true") ? FALSE : TRUE;
            const BLAnsi* _fontitalics = ezxml_attr(_element, "FontItalics");
            BLBool _fontitalicsvar = strcmp(_fontitalics, "true") ? FALSE : TRUE;
            const BLAnsi* _textcolor = ezxml_attr(_element, "TextColor");
            BLU32 _textcolorvar = (BLU32)strtoul(_textcolor, NULL, 10);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _fillmap = ezxml_attr(_element, "FillMap");
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Slider"))
        {
            _typevar = BL_UT_SLIDER;
            const BLAnsi* _enable = ezxml_attr(_element, "Enable");
            BLBool _enablevar = strcmp(_enable, "true") ? FALSE : TRUE;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _orientation = ezxml_attr(_element, "Orientation");
            BLBool _orientationvar = strcmp(_orientation, "Horizontal") ? FALSE : TRUE;
            const BLAnsi* _sliderstep = ezxml_attr(_element, "SliderStep");
            BLU32 _sliderstepvar = (BLU32)strtoul(_sliderstep, NULL, 10);
            const BLAnsi* _sliderposition = ezxml_attr(_element, "SliderPosition");
            BLU32 _sliderpositionvar = (BLU32)strtoul(_sliderposition, NULL, 10);
            const BLAnsi* _slidersize = ezxml_attr(_element, "SliderSize");
            BLU32 _slidersizevar[2];
            _tmp = strtok((BLAnsi*)_slidersize, ",");
            _slidersizevar[0] = (BLU32)strtoul(_tmp, NULL, 10);
            _slidersizevar[1] = (BLU32)strtoul(strtok(NULL, ","), NULL, 10);
            const BLAnsi* _range = ezxml_attr(_element, "Range");
            BLF32 _rangevar[2];
            _tmp = strtok((BLAnsi*)_range, ",");
            _rangevar[0] = (BLF32)strtod(_tmp, NULL);
            _rangevar[1] = (BLF32)strtod(strtok(NULL, ","), NULL);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _slidercommonmap = ezxml_attr(_element, "SliderCommonMap");
            const BLAnsi* _slidersisablemap = ezxml_attr(_element, "SliderDisableMap");
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Table"))
        {
            _typevar = BL_UT_TABLE;
            const BLAnsi* _flipx = ezxml_attr(_element, "FlipX");
            BLBool _flipxvar = strcmp(_flipx, "true") ? FALSE : TRUE;
            const BLAnsi* _flipy = ezxml_attr(_element, "FlipY");
            BLBool _flipyvar = strcmp(_flipy, "true") ? FALSE : TRUE;
            const BLAnsi* _fontsrc = ezxml_attr(_element, "FontSrc");
            const BLAnsi* _fontsize = ezxml_attr(_element, "FontSize");
            BLU32 _fontsizevar = (BLU32)strtol(_fontsize, NULL, 10);
            const BLAnsi* _fontoutline = ezxml_attr(_element, "FontOutline");
            BLBool _fontoutlinevar = strcmp(_fontoutline, "true") ? FALSE : TRUE;
            const BLAnsi* _fontbold = ezxml_attr(_element, "FontBold");
            BLBool _fontboldvar = strcmp(_fontbold, "true") ? FALSE : TRUE;
            const BLAnsi* _fontshadow = ezxml_attr(_element, "FontShadow");
            BLBool _fontshadowvar = strcmp(_fontshadow, "true") ? FALSE : TRUE;
            const BLAnsi* _fontitalics = ezxml_attr(_element, "FontItalics");
            BLBool _fontitalicsvar = strcmp(_fontitalics, "true") ? FALSE : TRUE;
            const BLAnsi* _textcolor = ezxml_attr(_element, "TextColor");
            BLU32 _textcolorvar = (BLU32)strtoul(_textcolor, NULL, 10);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _commontex = ezxml_attr(_element, "CommonTexcoord");
            BLF32 _commontexvar[4];
            _idx = 0;
            _tmp = strtok((BLAnsi*)_commontex, ",");
            while (_tmp)
            {
                _commontexvar[_idx] = (BLF32)strtod(_tmp, NULL);
                _tmp = strtok(NULL, ",");
                _idx++;
            }
            const BLAnsi* _odditemmap = ezxml_attr(_element, "OddItemMap");
            const BLAnsi* _evenitemmap = ezxml_attr(_element, "EvenItemMap");
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Dial"))
        {
            _typevar = BL_UT_DIAL;
            const BLAnsi* _clockwise = ezxml_attr(_element, "Clockwise");
            BLBool _clockwisevar = strcmp(_clockwise, "true") ? FALSE : TRUE;
            const BLAnsi* _anglecut = ezxml_attr(_element, "AngleCut");
            BLBool _anglecutvar = strcmp(_anglecut, "true") ? FALSE : TRUE;
            const BLAnsi* _startangle = ezxml_attr(_element, "StartAngle");
            BLU32 _startanglevar = (BLU32)strtoul(_startangle, NULL, 0);
            const BLAnsi* _endangle = ezxml_attr(_element, "EndAngle");
            BLU32 _endanglevar = (BLU32)strtoul(_endangle, NULL, 0);
            const BLAnsi* _pixmap = ezxml_attr(_element, "Pixmap");
            const BLAnsi* _commonmap = ezxml_attr(_element, "CommonMap");
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
        }
        else if (!strcmp(_type, "Primitive"))
        {
            _typevar = BL_UT_PRIMITIVE;
            const BLAnsi* _fill = ezxml_attr(_element, "Fill");
            BLBool _fillvar = strcmp(_fill, "true") ? FALSE : TRUE;
            const BLAnsi* _closed = ezxml_attr(_element, "Closed");
            BLBool _closedvar = strcmp(_closed, "true") ? FALSE : TRUE;
            const BLAnsi* _color = ezxml_attr(_element, "Color");
            BLU32 _colorvar = (BLU32)strtoul(_color, NULL, 10);
            BLF32* _xpath = (BLF32*)alloca(2048 * sizeof(BLF32));
            BLF32* _ypath = (BLF32*)alloca(2048 * sizeof(BLF32));
            BLU32 _pathnum = 0;
            const BLAnsi* _path = ezxml_attr(_element, "Path");
            _idx = 0;
            _tmp = strtok((BLAnsi*)_path, ",");
            while (_tmp)
            {
                if (_idx % 2 == 0)
                    _xpath[_pathnum] = (BLF32)strtod(_tmp, NULL);
                else
                {
                    _ypath[_pathnum] = (BLF32)strtod(_tmp, NULL);
                    _pathnum++;
                }
                _tmp = strtok(NULL, ",");
                _idx++;
            }
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_PRIMITIVE);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUIPrimitiveClosed(_widguid, _closedvar);
			blUIPrimitiveColor(_widguid, _colorvar);
			blUIPrimitiveFill(_widguid, _fillvar);
			blUIPrimitivePath(_widguid, _xpath, _ypath, _pathnum);
        }
		_element = _element->ordered;
    } while (_element);
    ezxml_free(_doc);
    blDeleteStream(_layout);
}
BLGuid 
blGenUI(IN BLAnsi* _WidgetName, IN BLS32 _PosX, IN BLS32 _PosY, IN BLU32 _Width, IN BLU32 _Height, IN BLGuid _Parent, IN BLEnum _Type)
{
	_BLWidget* _widget = (_BLWidget*)malloc(sizeof(_BLWidget));
	_widget->sDimension.fX = (BLF32)_Width;
	_widget->sDimension.fY = (BLF32)_Height;
	_widget->pParent = blGuidAsPointer(_Parent);
	if (!_widget->pParent)
		_widget->pParent = _PrUIMem->pRoot;
	_widget->eType = _Type;
	_widget->pChildren = blGenArray(FALSE);
	_widget->sPosition.fX = (BLF32)_PosX;
	_widget->sPosition.fY = (BLF32)_PosY;
	_widget->eReferenceH = BL_UA_HCENTER;
	_widget->eReferenceV = BL_UA_VCENTER;
	_widget->ePolicy = BL_UP_FIXED;
	_widget->fRatio = (BLF32)_Width / (BLF32)_Height;
	_widget->nMaxWidth = 0;
	_widget->nMaxHeight = 0;
	_widget->nMinWidth = 1;
	_widget->nMinHeight = 1;
	_widget->pTooltip = NULL;
	_widget->bVisible = TRUE;
	_widget->bCliped = TRUE;
	_widget->bPenetration = TRUE;
	_widget->bAbsScissor = FALSE;
	_widget->bInteractive = TRUE;
    _widget->bValid = (_Type == BL_UT_PRIMITIVE) ? TRUE : FALSE;
	switch (_widget->eType)
	{
		case BL_UT_PANEL:
		{
			memset(_widget->uExtension.sPanel.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sPanel.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sPanel.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sPanel.bDragable = FALSE;
			_widget->uExtension.sPanel.bBasePlate = FALSE;
			_widget->uExtension.sPanel.bModal = FALSE;
			_widget->uExtension.sPanel.bScrollable = TRUE;
			_widget->uExtension.sPanel.bFlipX = FALSE;
			_widget->uExtension.sPanel.bFlipY = FALSE;
            _widget->uExtension.sPanel.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_LABEL:
		{
			_widget->uExtension.sLabel.pText = NULL;
			memset(_widget->uExtension.sLabel.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sLabel.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sLabel.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sLabel.bFlipX = FALSE;
            _widget->uExtension.sLabel.bFlipY = FALSE;
            _widget->uExtension.sLabel.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_BUTTON:
		{
			_widget->uExtension.sLabel.pText = NULL;
			memset(_widget->uExtension.sButton.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sButton.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sButton.aHoveredMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sButton.aPressedMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sButton.aDisableMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sButton.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sButton.eTxtAlignmentH = BL_UA_HCENTER;
			_widget->uExtension.sButton.eTxtAlignmentV = BL_UA_VCENTER;
			_widget->uExtension.sButton.nFontHeight = 0;
			_widget->uExtension.sButton.bOutline = FALSE;
			_widget->uExtension.sButton.bBold = FALSE;
			_widget->uExtension.sButton.bShadow = FALSE;
			_widget->uExtension.sButton.bItalics = FALSE;
			_widget->uExtension.sButton.nTxtColor = 0xFFFFFFFF;
			_widget->uExtension.sButton.bFlipX = FALSE;
			_widget->uExtension.sButton.bFlipY = FALSE;
            _widget->uExtension.sButton.nState = 1;
            _widget->uExtension.sButton.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_CHECK:
		{
			_widget->uExtension.sCheck.pText = NULL;
			memset(_widget->uExtension.sCheck.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sCheck.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sCheck.aPressedMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sCheck.aDisableMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sCheck.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sCheck.eTxtAlignmentH = BL_UA_HCENTER;
			_widget->uExtension.sCheck.eTxtAlignmentV = BL_UA_VCENTER;
			_widget->uExtension.sCheck.nFontHash = 0xFFFFFFFF;
			_widget->uExtension.sCheck.nFontHeight = 0;
			_widget->uExtension.sCheck.bOutline = FALSE;
			_widget->uExtension.sCheck.bBold = FALSE;
			_widget->uExtension.sCheck.bShadow = FALSE;
			_widget->uExtension.sCheck.bItalics = FALSE;
			_widget->uExtension.sCheck.nTxtColor = 0xFFFFFFFF;
			_widget->uExtension.sCheck.bFlipX = FALSE;
			_widget->uExtension.sCheck.bFlipY = FALSE;
            _widget->uExtension.sCheck.nState = 1;
            _widget->uExtension.sCheck.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_SLIDER:
		{
			memset(_widget->uExtension.sSlider.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aSliderCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aSliderDisableMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sSlider.bHorizontal = TRUE;
			_widget->uExtension.sSlider.bSliderDragged = FALSE;
			_widget->uExtension.sSlider.nMinValue = 0;
			_widget->uExtension.sSlider.nMaxValue = 100;
			_widget->uExtension.sSlider.nStep = 1;
			_widget->uExtension.sSlider.nSliderPosition = 0;
			_widget->uExtension.sSlider.sSliderSize.fX = 10.f;
			_widget->uExtension.sSlider.sSliderSize.fY = 10.f;
			_widget->uExtension.sSlider.bFlipX = FALSE;
			_widget->uExtension.sSlider.bFlipY = FALSE;
            _widget->uExtension.sSlider.nState = 1;
            _widget->uExtension.sSlider.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_TEXT:
		{
			_widget->uExtension.sText.pText = NULL;
			_widget->uExtension.sText.pPlaceholder = NULL;
			memset(_widget->uExtension.sText.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sText.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sText.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sText.nMaxLength = 999999;
			_widget->uExtension.sText.nMinValue = 0;
			_widget->uExtension.sText.nMaxValue = 999999;
			_widget->uExtension.sText.bSelecting = FALSE;
			_widget->uExtension.sText.bAutoscroll = TRUE;
			_widget->uExtension.sText.bWordwrap = FALSE;
			_widget->uExtension.sText.bMultiline = FALSE;
			_widget->uExtension.sText.bPassword = FALSE;
			_widget->uExtension.sText.bNumeric = FALSE;
			_widget->uExtension.sText.eTxtAlignmentH = BL_UA_HCENTER;
			_widget->uExtension.sText.eTxtAlignmentV = BL_UA_VCENTER;
			_widget->uExtension.sText.nFontHash = 0xFFFFFFFF;
			_widget->uExtension.sText.nFontHeight = 0;
			_widget->uExtension.sText.bOutline = FALSE;
			_widget->uExtension.sText.bBold = FALSE;
			_widget->uExtension.sText.bShadow = FALSE;
			_widget->uExtension.sText.bItalics = FALSE;
			_widget->uExtension.sText.nTxtColor = 0xFFFFFFFF;
			_widget->uExtension.sText.nScrollPosH = 0;
			_widget->uExtension.sText.nScrollPosV = 0;
			_widget->uExtension.sText.nCaretPos = 0;
			_widget->uExtension.sText.nSelectBegin = 0;
			_widget->uExtension.sText.nSelectEnd = 0;
			_widget->uExtension.sText.nPaddingX = 0;
			_widget->uExtension.sText.nPaddingY = 0;
			_widget->uExtension.sText.bFlipX = FALSE;
			_widget->uExtension.sText.bFlipY = FALSE;
            _widget->uExtension.sText.nState = 1;
            _widget->uExtension.sText.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_PROGRESS:
		{
			_widget->uExtension.sProgress.pText = NULL;
			memset(_widget->uExtension.sProgress.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aFillMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sProgress.nFontHash = 0xFFFFFFFF;
			_widget->uExtension.sProgress.nFontHeight = 0;
			_widget->uExtension.sProgress.bOutline = FALSE;
			_widget->uExtension.sProgress.bBold = FALSE;
			_widget->uExtension.sProgress.bShadow = FALSE;
			_widget->uExtension.sProgress.bItalics = FALSE;
			_widget->uExtension.sProgress.nTxtColor = 0xFFFFFFFF;
			_widget->uExtension.sProgress.bFlipX = FALSE;
			_widget->uExtension.sProgress.bFlipY = FALSE;
            _widget->uExtension.sProgress.nPercent = 0;
            _widget->uExtension.sProgress.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_DIAL:
		{
			memset(_widget->uExtension.sDial.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sDial.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sDial.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sDial.nStartAngle = 0;
			_widget->uExtension.sDial.nEndAngle = 270;
			_widget->uExtension.sDial.bAngleCut = FALSE;
            _widget->uExtension.sDial.bClockWise = FALSE;
            _widget->uExtension.sDial.nPixmapTex = INVALID_GUID;
		}
		break;
		case BL_UT_TABLE:
		{
			memset(_widget->uExtension.sTable.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aOddItemMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aEvenItemMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sTable.nFontHash = 0xFFFFFFFF;
			_widget->uExtension.sTable.nFontHeight = 0;
			_widget->uExtension.sTable.bOutline = FALSE;
			_widget->uExtension.sTable.bBold = FALSE;
			_widget->uExtension.sTable.bShadow = FALSE;
			_widget->uExtension.sTable.bItalics = FALSE;
			_widget->uExtension.sTable.nTxtColor = 0xFFFFFFFF;
			_widget->uExtension.sTable.bFlipX = FALSE;
            _widget->uExtension.sTable.bFlipY = FALSE;
            _widget->uExtension.sTable.nPixmapTex = INVALID_GUID;
		}
		break;
		default:
		{
			_widget->uExtension.sPrimitive.bFill = TRUE;
			_widget->uExtension.sPrimitive.bClosed = TRUE;
			_widget->uExtension.sPrimitive.nColor = 0x88888888;
			_widget->uExtension.sPrimitive.pXPath = NULL;
			_widget->uExtension.sPrimitive.pYPath = NULL;
			_widget->uExtension.sPrimitive.nPathNum = 0;
		}
		break;
	}
	BLU32 _hashname = blHashUtf8((const BLUtf8*)_WidgetName);
	_widget->nID = blGenGuid(_widget, _hashname);
	blArrayPushBack(_widget->pParent->pChildren, _widget);
	_PrUIMem->bDirty = TRUE;
	return _widget->nID;
}
BLVoid
blDeleteUI(IN BLGuid _ID)
{
    if (_ID == INVALID_GUID)
        return;
    _BLWidget* _widget = blGuidAsPointer(_ID);
    if (!_widget)
        return;
	_DiscardResource(_ID, _UnloadUI, _UIRelease);
    {
        FOREACH_ARRAY (_BLWidget*, _iter, _widget->pChildren)
        {
            _iter->pParent = NULL;
            blDeleteUI(_iter->nID);
        }
    }
    blDeleteArray(_widget->pChildren);
    free(_widget);
    blDeleteGuid(_ID);
    _PrUIMem->bDirty = TRUE;
}
BLGuid 
blUIQuery(IN BLAnsi* _WidgetName)
{
	return _QueryWidget(_PrUIMem->pRoot, blHashUtf8((const BLUtf8*)_WidgetName), TRUE)->nID;
}
BLVoid 
blUISize(IN BLGuid _ID, IN BLU32 _Width, IN BLU32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->sDimension.fX = (BLF32)_Width;
	_widget->sDimension.fY = (BLF32)_Height;
	_widget->fRatio = (BLF32)_Width / (BLF32)_Height;
	if (_widget->nMaxWidth)
		_widget->sDimension.fX = (BLF32)MIN_INTERNAL(_widget->nMaxWidth, (BLU32)_widget->sDimension.fX);
	if (_widget->nMaxHeight)
		_widget->sDimension.fY = (BLF32)MIN_INTERNAL(_widget->nMaxHeight, (BLU32)_widget->sDimension.fY);
	_widget->sDimension.fX = (BLF32)MAX_INTERNAL(MAX_INTERNAL(1, _widget->nMinWidth), (BLU32)_widget->sDimension.fX);
	_widget->sDimension.fY = (BLF32)MAX_INTERNAL(MAX_INTERNAL(1, _widget->nMinHeight), (BLU32)_widget->sDimension.fY);
}
BLVoid 
blUIGetSize(IN BLGuid _ID, OUT BLU32* _Width, OUT BLU32* _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_Width = (BLU32)_widget->sDimension.fX;
	*_Height = (BLU32)_widget->sDimension.fY;
}
BLVoid
blUIReferencePoint(IN BLGuid _ID, IN BLEnum _ReferenceH, IN BLEnum _ReferenceV)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->eReferenceH = _ReferenceH;
	_widget->eReferenceV = _ReferenceV;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetReferencePoint(IN BLGuid _ID, OUT BLEnum* _ReferenceH, OUT BLEnum* _ReferenceV)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_ReferenceH = _widget->eReferenceH;
	*_ReferenceV = _widget->eReferenceV;
}
BLVoid
blUISizePolicy(IN BLGuid _ID, IN BLEnum _Policy)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->ePolicy = _Policy;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetSizePolicy(IN BLGuid _ID, OUT BLEnum* _Policy)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_Policy = _widget->ePolicy;
}
BLVoid
blUISizeLimit(IN BLGuid _ID, IN BLU32 _MaxWidth, IN BLU32 _MaxHeight, IN BLU32 _MinWidth, IN BLU32 _MinHeight)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->nMinWidth = _MinWidth;
	_widget->nMinHeight = _MinHeight;
	_widget->nMaxWidth = _MaxWidth;
	_widget->nMaxHeight = _MaxHeight;
	if (_MaxWidth)
		_widget->sDimension.fX = (BLF32)MIN_INTERNAL(_MaxWidth, (BLU32)_widget->sDimension.fX);
	if (_MaxHeight)
		_widget->sDimension.fY = (BLF32)MIN_INTERNAL(_MaxHeight, (BLU32)_widget->sDimension.fY);
	_widget->sDimension.fX = (BLF32)MAX_INTERNAL(MAX_INTERNAL(1, _MinWidth), (BLU32)_widget->sDimension.fX);
	_widget->sDimension.fY = (BLF32)MAX_INTERNAL(MAX_INTERNAL(1, _MinHeight), (BLU32)_widget->sDimension.fY);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetSizeLimit(IN BLGuid _ID, OUT BLU32* _MaxWidth, OUT BLU32* _MaxHeight, OUT BLU32* _MinWidth, OUT BLU32* _MinHeight)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_MinWidth = _widget->nMinWidth;
	*_MinHeight = _widget->nMinHeight;
	*_MaxWidth = _widget->nMaxWidth;
	*_MaxHeight = _widget->nMaxHeight;
}
BLVoid
blUIScissor(IN BLGuid _ID, IN BLBool _Cliped, IN BLBool _AbsScissor)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->bCliped = _Cliped;
	_widget->bAbsScissor = _AbsScissor;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetScissor(IN BLGuid _ID, OUT BLBool* _Cliped, OUT BLBool* _AbsScissor)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_Cliped = _widget->bCliped;
	*_AbsScissor = _widget->bAbsScissor;
}
BLVoid
blUIInteractive(IN BLGuid _ID, IN BLBool _Interactive)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->bInteractive = _Interactive;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetInteractive(IN BLGuid _ID, OUT BLBool* _Interactive)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_Interactive = _widget->bInteractive;
}
BLVoid
blUIVisible(IN BLGuid _ID, IN BLBool _Visible)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->bVisible = _Visible;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetVisible(IN BLGuid _ID, OUT BLBool* _Visible)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_Visible = _widget->bVisible;
}
BLVoid
blUITooltip(IN BLGuid _ID, IN BLUtf8* _Tooltip)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	BLU32 _strlen = blUtf8Length(_Tooltip);
	if (!_strlen)
		return;
	_widget->pTooltip = malloc(_strlen + 1);
	memset(_widget->pTooltip, 0, _strlen + 1);
	strcpy((BLAnsi*)_widget->pTooltip, (const BLAnsi*)_Tooltip);
	_PrUIMem->bDirty = TRUE;
}
const BLUtf8* 
blUIGetTooltip(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return NULL;
	return _widget->pTooltip;
}
BLVoid
blUIPenetration(IN BLGuid _ID, IN BLBool _Penetration)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	_widget->bPenetration = _Penetration;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIGetPenetration(IN BLGuid _ID, OUT BLBool* _Penetration)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	*_Penetration = _widget->bPenetration;
}
BLVoid
blUIPanelPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	memset(_widget->uExtension.sPanel.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sPanel.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	memset(_widget->uExtension.sPanel.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sPanel.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	memset(_widget->uExtension.sPanel.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sPanel.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelDragable(IN BLGuid _ID, IN BLBool _Dragable)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	_widget->uExtension.sPanel.bDragable = _Dragable;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelBasePlate(IN BLGuid _ID, IN BLBool _BasePlate)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	_widget->uExtension.sPanel.bBasePlate = _BasePlate;
	_PrUIMem->pBasePlate = _widget;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelModal(IN BLGuid _ID, IN BLBool _Modal)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	_widget->uExtension.sPanel.bModal = _Modal;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelScrollable(IN BLGuid _ID, IN BLBool _Scrollable)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	_widget->uExtension.sPanel.bScrollable = _Scrollable;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPanelFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PANEL)
		return;
	_widget->uExtension.sPanel.bFlipX = _FlipX;
	_widget->uExtension.sPanel.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aPixmap, _Pixmap);
    BLAnsi _texfile[260] = { 0 };
    sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aCommonMap, _CommonMap);
	_widget->uExtension.sButton.sCommonTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sCommonTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sCommonTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sCommonTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sButton.sCommonTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sCommonTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sCommonTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sCommonTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonHoverMap(IN BLGuid _ID, IN BLAnsi* _HoveredMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aHoveredMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aHoveredMap, _HoveredMap);
	_widget->uExtension.sButton.sHoveredTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sHoveredTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sHoveredTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sHoveredTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sButton.sHoveredTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sHoveredTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sHoveredTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sHoveredTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonPressMap(IN BLGuid _ID, IN BLAnsi* _PressedMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aPressedMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aPressedMap, _PressedMap);
	_widget->uExtension.sButton.sPressedTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sPressedTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sPressedTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sPressedTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sButton.sPressedTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sPressedTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sPressedTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sPressedTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonDisableMap(IN BLGuid _ID, IN BLAnsi* _DisableMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aDisableMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aDisableMap, _DisableMap);
	_widget->uExtension.sButton.sDisableTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sDisableTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sDisableTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sDisableTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sButton.sDisableTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sButton.sDisableTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sButton.sDisableTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sButton.sDisableTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonText(IN BLGuid _ID, IN BLUtf8* _Text,	IN BLU32 _TxtColor,	IN BLEnum _TxtAlignmentH, IN BLEnum _TxtAlignmentV)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	BLU32 _strlen = blUtf8Length(_Text) + 1;
	if (_widget->uExtension.sButton.pText)
		free(_widget->uExtension.sButton.pText);
	_widget->uExtension.sButton.pText = (BLUtf8*)malloc(_strlen);
	memset(_widget->uExtension.sButton.pText, 0, _strlen);
	strcpy((BLAnsi*)_widget->uExtension.sButton.pText, (const BLAnsi*)_Text);
	_widget->uExtension.sButton.nTxtColor = _TxtColor;
	_widget->uExtension.sButton.eTxtAlignmentH = _TxtAlignmentH;
	_widget->uExtension.sButton.eTxtAlignmentV = _TxtAlignmentV;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aFontSource, 0, sizeof(_widget->uExtension.sButton.aFontSource));
	strcpy(_widget->uExtension.sButton.aFontSource, _Font);
	_widget->uExtension.sButton.nFontHeight = _FontHeight;
	if (_Outline)
	{
		_widget->uExtension.sButton.bOutline = _Outline;
		_widget->uExtension.sButton.bBold = FALSE;
		_widget->uExtension.sButton.bShadow = FALSE;
		_widget->uExtension.sButton.bItalics = _Italics;
	}
	else if (_Bold)
	{
		_widget->uExtension.sButton.bOutline = FALSE;
		_widget->uExtension.sButton.bBold = _Bold;
		_widget->uExtension.sButton.bShadow = FALSE;
		_widget->uExtension.sButton.bItalics = _Italics;
	}
	else if (_Shadow)
	{
		_widget->uExtension.sButton.bOutline = FALSE;
		_widget->uExtension.sButton.bBold = FALSE;
		_widget->uExtension.sButton.bShadow = _Shadow;
		_widget->uExtension.sButton.bItalics = _Italics;
	}
	else
	{
		_widget->uExtension.sButton.bOutline = FALSE;
		_widget->uExtension.sButton.bBold = FALSE;
		_widget->uExtension.sButton.bShadow = FALSE;
		_widget->uExtension.sButton.bItalics = _Italics;
	}
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	_widget->uExtension.sButton.bFlipX = _FlipX;
	_widget->uExtension.sButton.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonEnable(IN BLGuid _ID, IN BLBool _Enable)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	_widget->uExtension.sButton.nState = _Enable ? 1 : 0;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUILabelPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_LABEL)
		return;
	memset(_widget->uExtension.sLabel.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sLabel.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUILabelStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_LABEL)
		return;
	memset(_widget->uExtension.sLabel.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sLabel.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUILabelCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_LABEL)
		return;
	memset(_widget->uExtension.sLabel.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sLabel.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUILableText(IN BLGuid _ID, IN BLUtf8* _Text, IN BLU32 _TxtColor, IN BLEnum _TxtAlignmentH, IN BLEnum _TxtAlignmentV, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics, IN BLBool _Rich)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_LABEL)
		return;
	if (_Rich)
	{
		BLU32 _strlen = blUtf8Length(_Text) + 1;
		_widget->uExtension.sLabel.pText = (BLUtf8*)malloc(_strlen);
		memset(_widget->uExtension.sLabel.pText, 0, _strlen);
		strcpy((BLAnsi*)_widget->uExtension.sLabel.pText, (const BLAnsi*)_Text);
	}
	else
	{
		BLAnsi _align[4] = { 0 };
		if (_TxtAlignmentH == BL_UA_LEFT && _TxtAlignmentV == BL_UA_TOP)
			strcpy(_align, "LT");
		else if (_TxtAlignmentH == BL_UA_LEFT && _TxtAlignmentV == BL_UA_VCENTER)
			strcpy(_align, "LM");
		else if (_TxtAlignmentH == BL_UA_LEFT && _TxtAlignmentV == BL_UA_BOTTOM)
			strcpy(_align, "LB");
		else if (_TxtAlignmentH == BL_UA_HCENTER && _TxtAlignmentV == BL_UA_TOP)
			strcpy(_align, "MT");
		else if (_TxtAlignmentH == BL_UA_HCENTER && _TxtAlignmentV == BL_UA_VCENTER)
			strcpy(_align, "MM");
		else if (_TxtAlignmentH == BL_UA_HCENTER && _TxtAlignmentV == BL_UA_BOTTOM)
			strcpy(_align, "MB");
		else if (_TxtAlignmentH == BL_UA_RIGHT && _TxtAlignmentV == BL_UA_TOP)
			strcpy(_align, "RT");
		else if (_TxtAlignmentH == BL_UA_RIGHT && _TxtAlignmentV == BL_UA_VCENTER)
			strcpy(_align, "RM");
		else
			strcpy(_align, "RB");
		BLU16 _flag = 0;
		if (_Outline)
			_flag |= 0x000F;
		else if (_Bold)
			_flag |= 0x00F0;
		else if (_Shadow)
			_flag |= 0x0F00;
		if (_Italics)
			_flag |= 0xF000;
		BLAnsi _tmp[256] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		sprintf_s(_tmp, 256, "#font:%s:%d:%d##align:%s##color:%zu:-1#", _Font, _FontHeight, _flag, _align, _TxtColor);
#elif defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_ANDROID)
        sprintf(_tmp, "#font:%s:%d:%d##align:%s##color:%zu:-1#", _Font, _FontHeight, _flag, _align, _TxtColor);
#else
		sprintf(_tmp, "#font:%s:%d:%d##align:%s##color:%u:-1#", _Font, _FontHeight, _flag, _align, _TxtColor);
#endif
		BLU32 _strlen = blUtf8Length(_Text) + 1UL + (BLU32)strlen(_tmp);
		_widget->uExtension.sLabel.pText = (BLUtf8*)malloc(_strlen);
		memset(_widget->uExtension.sLabel.pText, 0, _strlen);
		strcpy((BLAnsi*)_widget->uExtension.sLabel.pText, _tmp);
		strcat((BLAnsi*)_widget->uExtension.sLabel.pText, (const BLAnsi*)_Text);
	}
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUILabelFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_LABEL)
		return;
	_widget->uExtension.sLabel.bFlipX = _FlipX;
	_widget->uExtension.sLabel.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sCheck.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sCheck.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sCheck.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckPressMap(IN BLGuid _ID, IN BLAnsi* _PressedMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aPressedMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sCheck.aPressedMap, _PressedMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckDisableMap(IN BLGuid _ID, IN BLAnsi* _DisableMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aDisableMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sCheck.aDisableMap, _DisableMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckText(IN BLGuid _ID, IN BLUtf8* _Text, IN BLU32 _TxtColor, IN BLEnum _TxtAlignmentH, IN BLEnum _TxtAlignmentV)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	BLU32 _strlen = blUtf8Length(_Text) + 1;
	_widget->uExtension.sCheck.pText = (BLUtf8*)malloc(_strlen);
	memset(_widget->uExtension.sCheck.pText, 0, _strlen);
	strcpy((BLAnsi*)_widget->uExtension.sCheck.pText, (const BLAnsi*)_Text);
	_widget->uExtension.sCheck.nTxtColor = _TxtColor;
	_widget->uExtension.sCheck.eTxtAlignmentH = _TxtAlignmentH;
	_widget->uExtension.sCheck.eTxtAlignmentV = _TxtAlignmentV;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	_widget->uExtension.sCheck.nFontHash = blHashUtf8((const BLUtf8*)_Font);
	_widget->uExtension.sCheck.nFontHeight = _FontHeight;
	_widget->uExtension.sCheck.bOutline = _Outline;
	_widget->uExtension.sCheck.bBold = _Bold;
	_widget->uExtension.sCheck.bShadow = _Shadow;
	_widget->uExtension.sCheck.bItalics = _Italics;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	_widget->uExtension.sCheck.bFlipX = _FlipX;
	_widget->uExtension.sCheck.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckEnable(IN BLGuid _ID, IN BLBool _Enable)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	_widget->uExtension.sCheck.nState = _Enable ? 1 : 0;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	memset(_widget->uExtension.sText.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sText.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	memset(_widget->uExtension.sText.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sText.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	memset(_widget->uExtension.sText.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sText.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextTypography(IN BLGuid _ID, IN BLBool _Autoscroll, IN BLBool _Wordwrap, IN BLBool _Multiline)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.bAutoscroll = _Autoscroll;
	_widget->uExtension.sText.bWordwrap = _Wordwrap;
	_widget->uExtension.sText.bMultiline = _Multiline;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextPassword(IN BLGuid _ID, IN BLBool _Password)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.bPassword = _Password;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextNumeric(IN BLGuid _ID, IN BLBool _Numeric, IN BLS32 _MinValue, IN BLS32 _MaxValue)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.bNumeric = _Numeric;
	_widget->uExtension.sText.nMinValue = _MinValue;
	_widget->uExtension.sText.nMaxValue = _MaxValue;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextPadding(IN BLGuid _ID, IN BLF32 _PaddingX, IN BLF32 _PaddingY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.nPaddingX = _PaddingX;
	_widget->uExtension.sText.nPaddingY = _PaddingY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextMaxLength(IN BLGuid _ID, IN BLU32 _MaxLength)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.nMaxLength = _MaxLength;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextPlaceholder(IN BLGuid _ID, IN BLUtf8*	_Placeholder, IN BLU32 _TxtColor, IN BLEnum _TxtAlignmentH, IN BLEnum _TxtAlignmentV)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	BLU32 _strlen = blUtf8Length(_Placeholder) + 1;
	_widget->uExtension.sText.pText = (BLUtf8*)malloc(_strlen);
	memset(_widget->uExtension.sText.pText, 0, _strlen);
	strcpy((BLAnsi*)_widget->uExtension.sText.pText, (const BLAnsi*)_Placeholder);
	_widget->uExtension.sText.nTxtColor = _TxtColor;
	_widget->uExtension.sText.eTxtAlignmentH = _TxtAlignmentH;
	_widget->uExtension.sText.eTxtAlignmentV = _TxtAlignmentV;
	_PrUIMem->bDirty = TRUE;
}
const BLUtf8* 
blUIGetTextText(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return NULL;
	if (_widget->eType != BL_UT_TEXT)
		return NULL;
	return _widget->uExtension.sText.pText;
}
BLVoid
blUITextFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.nFontHash = blHashUtf8((const BLUtf8*)_Font);
	_widget->uExtension.sText.nFontHeight = _FontHeight;
	_widget->uExtension.sText.bOutline = _Outline;
	_widget->uExtension.sText.bBold = _Bold;
	_widget->uExtension.sText.bShadow = _Shadow;
	_widget->uExtension.sText.bItalics = _Italics;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.bFlipX = _FlipX;
	_widget->uExtension.sText.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextEnable(IN BLGuid _ID, IN BLBool _Enable)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.nState = _Enable ? 1 : 0;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	memset(_widget->uExtension.sProgress.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sProgress.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	memset(_widget->uExtension.sProgress.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sProgress.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	memset(_widget->uExtension.sProgress.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sProgress.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressFillMap(IN BLGuid _ID, IN BLAnsi* _FillMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	memset(_widget->uExtension.sProgress.aFillMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sProgress.aFillMap, _FillMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressPercent(IN BLGuid _ID, IN BLU32 _Percent)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	_widget->uExtension.sProgress.nPercent = (BLU32)blScalarClamp((BLF32)_Percent, 0, 100);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressText(IN BLGuid _ID, IN BLUtf8* _Text, IN BLU32 _TxtColor)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	BLU32 _strlen = blUtf8Length(_Text) + 1;
	_widget->uExtension.sProgress.pText = (BLUtf8*)malloc(_strlen);
	memset(_widget->uExtension.sProgress.pText, 0, _strlen);
	strcpy((BLAnsi*)_widget->uExtension.sProgress.pText, (const BLAnsi*)_Text);
	_widget->uExtension.sProgress.nTxtColor = _TxtColor;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	_widget->uExtension.sProgress.nFontHash = blHashUtf8((const BLUtf8*)_Font);
	_widget->uExtension.sProgress.nFontHeight = _FontHeight;
	_widget->uExtension.sProgress.bOutline = _Outline;
	_widget->uExtension.sProgress.bBold = _Bold;
	_widget->uExtension.sProgress.bShadow = _Shadow;
	_widget->uExtension.sProgress.bItalics = _Italics;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIProgressFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	_widget->uExtension.sProgress.bFlipX = _FlipX;
	_widget->uExtension.sProgress.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	memset(_widget->uExtension.sSlider.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sSlider.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	memset(_widget->uExtension.sSlider.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sSlider.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	memset(_widget->uExtension.sSlider.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sSlider.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderCommonMap(IN BLGuid _ID, IN BLAnsi* _SliderCommonMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	memset(_widget->uExtension.sSlider.aSliderCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sSlider.aSliderCommonMap, _SliderCommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderDisableMap(IN BLGuid _ID, IN BLAnsi* _SliderDisableMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	memset(_widget->uExtension.sSlider.aSliderDisableMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sSlider.aSliderDisableMap, _SliderDisableMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderHorizontal(IN BLGuid _ID, IN BLBool _Horizontal)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.bHorizontal = _Horizontal;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderStep(IN BLGuid _ID, IN BLBool _Step)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.nStep = _Step;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderPos(IN BLGuid _ID, IN BLU32 _Pos)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.nSliderPosition = _Pos;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderSize(IN BLGuid _ID, IN BLU32 _Width, IN BLU32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.sSliderSize.fX = (BLF32)_Width;
	_widget->uExtension.sSlider.sSliderSize.fY = (BLF32)_Height;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderRange(IN BLGuid _ID, IN BLU32 _MinValue, IN BLU32 _MaxValue)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.nMinValue = _MinValue;
	_widget->uExtension.sSlider.nMaxValue = _MaxValue;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	_widget->uExtension.sSlider.bFlipX = _FlipX;
	_widget->uExtension.sSlider.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderEnable(IN BLGuid _ID, IN BLBool _Enable)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.nState = _Enable ? 1 : 0;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITablePixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	memset(_widget->uExtension.sTable.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sTable.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	memset(_widget->uExtension.sTable.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sTable.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	memset(_widget->uExtension.sTable.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sTable.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableOddItemMap(IN BLGuid _ID, IN BLAnsi* _OddItemMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	memset(_widget->uExtension.sTable.aOddItemMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sTable.aOddItemMap, _OddItemMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableEvenItemMap(IN BLGuid _ID, IN BLAnsi* _EvenItemMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	memset(_widget->uExtension.sTable.aEvenItemMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sTable.aEvenItemMap, _EvenItemMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableAddRow(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableAddColumn(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableText(IN BLGuid _ID, IN BLUtf8* _Text, IN BLU32 _TxtColor, IN BLU32 _Row, IN BLU32 _Column)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	_PrUIMem->bDirty = TRUE;
}
const BLUtf8* 
blUIGetTableText(IN BLGuid _ID, IN BLU32 _Row, IN BLU32 _Column)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return NULL;
	if (_widget->eType != BL_UT_TABLE)
		return NULL;
    return NULL;
}
BLVoid
blUITableFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	_widget->uExtension.sTable.nFontHash = blHashUtf8((const BLUtf8*)_Font);
	_widget->uExtension.sTable.nFontHeight = _FontHeight;
	_widget->uExtension.sTable.bOutline = _Outline;
	_widget->uExtension.sTable.bBold = _Bold;
	_widget->uExtension.sTable.bShadow = _Shadow;
	_widget->uExtension.sTable.bItalics = _Italics;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableFlip(IN BLGuid _ID, IN BLBool _FlipX, IN BLBool _FlipY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	_widget->uExtension.sTable.bFlipX = _FlipX;
	_widget->uExtension.sTable.bFlipY = _FlipY;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIDialPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	memset(_widget->uExtension.sDial.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sDial.aPixmap, _Pixmap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIDialStencilMap(IN BLGuid _ID, IN BLAnsi* _StencilMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	memset(_widget->uExtension.sDial.aStencilMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sDial.aStencilMap, _StencilMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIDialCommonMap(IN BLGuid _ID, IN BLAnsi* _CommonMap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	memset(_widget->uExtension.sDial.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sDial.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIDialAngle(IN BLGuid _ID, IN BLS32 _StartAngle, IN BLS32 _EndAngle)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	_widget->uExtension.sDial.nStartAngle = _StartAngle;
	_widget->uExtension.sDial.nEndAngle = _EndAngle;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIDialAngleCut(IN BLGuid _ID, IN BLBool _Cut)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	_widget->uExtension.sDial.bAngleCut = _Cut;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIDialClockwise(IN BLGuid _ID, IN BLBool _Clockwise)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	_widget->uExtension.sDial.bClockWise = _Clockwise;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPrimitiveFill(IN BLGuid _ID, IN BLBool _Fill)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PRIMITIVE)
		return;
	_widget->uExtension.sPrimitive.bFill = _Fill;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPrimitiveClosed(IN BLGuid _ID, IN BLBool _Closed)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PRIMITIVE)
		return;
	_widget->uExtension.sPrimitive.bClosed = _Closed;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIPrimitiveColor(IN BLGuid _ID, IN BLU32 _Color)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PRIMITIVE)
		return;
	_widget->uExtension.sPrimitive.nColor = _Color;
	_PrUIMem->bDirty = TRUE;
}
BLVoid 
blUIPrimitivePath(IN BLGuid _ID, IN BLF32* _XPath, IN BLF32* _YPath, IN BLU32 _PathNum)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PRIMITIVE)
		return;
	if (_widget->uExtension.sPrimitive.pXPath)
		free(_widget->uExtension.sPrimitive.pXPath);
	if (_widget->uExtension.sPrimitive.pYPath)
		free(_widget->uExtension.sPrimitive.pYPath);
	_widget->uExtension.sPrimitive.pXPath = (BLF32*)malloc(_PathNum * sizeof(BLF32));
	_widget->uExtension.sPrimitive.pYPath = (BLF32*)malloc(_PathNum * sizeof(BLF32));
	memcpy(_widget->uExtension.sPrimitive.pXPath, _XPath, _PathNum * sizeof(BLF32));
	memcpy(_widget->uExtension.sPrimitive.pYPath, _YPath, _PathNum * sizeof(BLF32));
	_widget->uExtension.sPrimitive.nPathNum = _PathNum;
	_PrUIMem->bDirty = TRUE;
}
