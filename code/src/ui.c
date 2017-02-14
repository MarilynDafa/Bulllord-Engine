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
#include FT_STROKER_H
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
	BLDictionary* pAtlasDict;
	FT_Face sFtFace;
}_BLFont;
typedef struct _PixmapSheet{
    BLF32 fLTX;
    BLF32 fLTY;
    BLF32 fRBX;
    BLF32 fRBY;
}_BLPixmapSheet;
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
			BLRect aCommonTex[9];
			BLBool bDragable;
			BLBool bBasePlate;
			BLBool bModal;
			BLBool bScrollable;
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
            BLDictionary* pPixmapSheetDct;
		}sPanel;
		struct _Button {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aHoveredMap[128];
			BLAnsi aPressedMap[128];
			BLAnsi aDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect aCommonTex[9];
			BLRect aHoveredTex[9];
			BLRect aPressedTex[9];
			BLRect aDisableTex[9];
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
            BLDictionary* pPixmapSheetDct;
		}sButton;
		struct _Label {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLRect aCommonTex[9];
			BLAnsi aStencilMap[128];
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
            BLDictionary* pPixmapSheetDct;
		}sLabel;
		struct _Check {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aPressedMap[128];
			BLAnsi aDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect aCommonTex[9];
			BLRect aPressedTex[9];
			BLRect aDisableTex[9];
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
            BLDictionary* pPixmapSheetDct;
		}sCheck;
		struct _Text {
			BLUtf8* pText;
			BLUtf8* pPlaceholder;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLRect aCommonTex[9];
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
            BLDictionary* pPixmapSheetDct;
		}sText;
		struct _Progress {
			BLUtf8* pText;
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLAnsi aFillMap[128];
			BLRect aCommonTex[9];
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
            BLDictionary* pPixmapSheetDct;
		}sProgress;
		struct _Slider {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aSliderCommonMap[128];
			BLAnsi aSliderDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect aCommonTex[9];
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
            BLDictionary* pPixmapSheetDct;
		}sSlider;
		struct _Table {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLAnsi aOddItemMap[128];
			BLAnsi aEvenItemMap[128];
			BLRect aCommonTex[9];
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
            BLDictionary* pPixmapSheetDct;
		}sTable;
		struct _Dial {
			BLAnsi aPixmap[128];
			BLAnsi aCommonMap[128];
			BLAnsi aStencilMap[128];
			BLS32 nStartAngle;
			BLS32 nEndAngle;
			BLBool bAngleCut;
			BLBool bClockWise;
            BLGuid nPixmapTex;
            BLDictionary* pPixmapSheetDct;
		}sDial;
		struct _Primitive {
			BLBool bFill;
			BLBool bClosed;
			BLU32 nColor;
			BLF32* pXPath;
			BLF32* pYPath;
		}sPrimitive;
	} uExtension;
}_BLWidget;
typedef struct _UIMember {
	BLArray* pFonts;
	_BLWidget* pRoot;
	_BLWidget* pBasePlate;
	BLArray* pPixmaps;
	BLAnsi aDir[260];
	BLAnsi aArchive[260];
	FT_Library sFtLibrary;
	BLU32 nFboWidth;
	BLU32 nFboHeight;
	BLU32 nCaretColor;
	BLU32 nSelectRangeColor;
	BLU32 nTextDisableColor;
	BLBool bDirty;
}_BLUIMember;
static _BLUIMember* _PrUIMem = NULL;

static _BLFont*
_FontFace(const BLAnsi* _Filename, const BLAnsi* _Archive)
{
	BLGuid _font = blGenStream(_Filename, _Archive);
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
		return NULL;
	}
	BLU32 _endi = 0, _starti = 0;
	for (BLS32 _idx = (BLS32)strlen(_Filename); _idx > 0; --_idx)
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
	_ret->pAtlasDict = blGenDict(FALSE);
	blDeleteStream(_font);
	return _ret;
}
static _BLGlyph*
_QueryGlyph(_BLFont* _Font, BLU32 _Char, BLU32 _FontHeight)
{
	_BLGlyphAtlas* _glyphatlas = blDictElement(_Font->pAtlasDict, _FontHeight);
	if (!_glyphatlas)
		return NULL;
	_BLGlyph* _glyph = blDictElement(_glyphatlas->pGlyphs, _Char);
	if (!_glyph)
	{
		_glyph = (_BLGlyph*)malloc(sizeof(_BLGlyph));
		memset(_glyph, 0, sizeof(_BLGlyph));
		_glyph->nAdv = 0;
		_glyph->bValid = FALSE;
		blDictInsert(_Font->pAtlasDict, _Char, _glyph);
		if (_glyphatlas->nCurTexture == INVALID_GUID)
		{
			_glyphatlas->nCurTexture = blGenTexture(blUniqueUri(), BL_TT_2D, BL_TF_RG8, FALSE, FALSE, FALSE, 1, 1, 256, 256, 1, NULL);
			blTextureFilter(_glyphatlas->nCurTexture, BL_TF_NEAREST, BL_TF_NEAREST, BL_TW_CLAMP, BL_TW_CLAMP, FALSE);
			BLGuid* _curt = (BLGuid*)malloc(sizeof(BLGuid));
			*_curt = _glyphatlas->nCurTexture;
			blArrayPushBack(_glyphatlas->pTextures, _curt);
			_glyphatlas->nRecordX = 2;
			_glyphatlas->nRecordY = 2;
			_glyphatlas->nYB = 2;
		}
		BLBool _outline = LOWU16(_Char) & 0x000F;
		BLBool _bold = LOWU16(_Char) & 0x00F0;
		BLBool _shadow = LOWU16(_Char) & 0x0F00;
		BLBool _italics = LOWU16(_Char) & 0xF000;
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
		_glyphw += 2;
		_glyphh += 2;
		BLU32 _xnext = _glyphatlas->nRecordX + _glyphw;
		if (_xnext > 256)
		{
			_glyphatlas->nRecordX = 2;
			_xnext = _glyphatlas->nRecordX + _glyphw;
			_glyphatlas->nRecordY = _glyphatlas->nYB;
		}
		BLU32 _ybot = _glyphatlas->nRecordY + _glyphh + (_shadow ? 1 : 0);
		while (_ybot > 256)
		{
			_glyphatlas->nCurTexture = blGenTexture(blUniqueUri(), BL_TT_2D, BL_TF_RG8, FALSE, FALSE, FALSE, 1, 1, 256, 256, 1, NULL);
			blTextureFilter(_glyphatlas->nCurTexture, BL_TF_NEAREST, BL_TF_NEAREST, BL_TW_CLAMP, BL_TW_CLAMP, FALSE);
			BLGuid* _curt = (BLGuid*)malloc(sizeof(BLGuid));
			*_curt = _glyphatlas->nCurTexture;
			blArrayPushBack(_glyphatlas->pTextures, _curt);
			_glyphatlas->nRecordX = 2;
			_glyphatlas->nRecordY = 2;
			_glyphatlas->nYB = 2;
			_xnext = _glyphatlas->nRecordX + _glyphw;
			if (_xnext > 256)
			{
				_glyphatlas->nRecordX = 2;
				_xnext = _glyphatlas->nRecordX + _glyphw;
				_glyphatlas->nRecordY = _glyphatlas->nYB;
			}
			_ybot = _glyphatlas->nRecordY + _glyphh;
		}
		_glyph->sRect.sLT.fX = (BLF32)_glyphatlas->nRecordX;
		_glyph->sRect.sLT.fY = (BLF32)_glyphatlas->nRecordY;
		_glyph->sOffset.fX = (BLF32)_Font->sFtFace->glyph->bitmap_left;
		_glyph->sOffset.fY = _FontHeight * 0.75f + 1 - (_Font->sFtFace->glyph->metrics.horiBearingY >> 6);
		_glyph->sRect.sRB.fX = (BLF32)_glyphatlas->nRecordX + _glyphw - 2;
		_glyph->sRect.sRB.fY = (BLF32)_glyphatlas->nRecordY + _glyphh - 2;
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
		BLU16* _buf = (BLU16*)malloc(_imgw * _imgh * sizeof(BLU16));
		memset(_buf, 0, _imgw * _imgh * sizeof(BLU16));
		FT_Bitmap* _bitmap = &_Font->sFtFace->glyph->bitmap;
		if (_outline)
		{
			BLU16 _dsttmp, _srctmp;
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
					_srctmp = *_src;
					*_dstlt++ += 0x00; *_dstt++ += 0x00; *_dstrt++ += 0x00; *_dstl++ += 0x00; *_dstr++ += 0x00; *_dstlb++ += 0x00; *_dstb++ += 0x00; *_dstrb++ += 0x00;
                    BLU32 _dst32 = *_dst;
                    BLU32 _dstlt32 = *_dstlt;
                    BLU32 _dstt32 = *_dstt;
                    BLU32 _dstrt32 = *_dstrt;
                    BLU32 _dstl32 = *_dstl;
                    BLU32 _dstr32 = *_dstr;
                    BLU32 _dstlb32 = *_dstlb;
                    BLU32 _dstb32 = *_dstb;
                    BLU32 _dstrb32 = *_dstrb;
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dst32 + (0xFF) * (_srctmp / 255.f));
					*_dstlt++ = (BLU8)MIN_INTERNAL(_dstlt32 + (*_src)*0.5f, 255);
					*_dstt++ = (BLU8)MIN_INTERNAL(_dstt32 + (*_src)*0.5f, 255);
					*_dstrt++ = (BLU8)MIN_INTERNAL(_dstrt32 + (*_src)*0.5f, 255);
					*_dstl++ = (BLU8)MIN_INTERNAL(_dstl32 + (*_src)*0.5f, 255);
					*_dstr++ = (BLU8)MIN_INTERNAL(_dstr32 + (*_src)*0.5f, 255);
					*_dstlb++ = (BLU8)MIN_INTERNAL(_dstlb32 + (*_src)*0.5f, 255);
					*_dstb++ = (BLU8)MIN_INTERNAL(_dstb32 + (*_src)*0.5f, 255);
					*_dstrb++ = (BLU8)MIN_INTERNAL(_dstrb32 + (*_src)*0.5f, 255);
					_dsttmp = *(_dst);
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dsttmp + _srctmp);
					_src++;
				}
			}
			_tmpbuflt += _imgw;
			_tmpbuft += _imgw;
			_tmpbufl += _imgw;
			_tmpbuf += _imgw;
			_tmpbufr += _imgw;
			_tmpbufrt += _imgw;
			_tmpbuflb += _imgw;
			_tmpbufb += _imgw;
			_tmpbufrb += _imgw;
		}
		else if (_bold)
		{
			BLU16 _dsttmp, _srctmp;
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
					_srctmp = *_src;
					*_dst2++ = 0xFF; *_dst3++ = 0xFF; *_dst4++ = 0xFF;
                    BLU32 _dst32 = *_dst;
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dst32 + (0xFF) * (_srctmp / 255.f));
					*_dst2++ = (BLU8)MIN_INTERNAL((*_src)*0.35f, 255);
					*_dst3++ = (BLU8)MIN_INTERNAL((*_src)*0.35f, 255);
					*_dst4++ = (BLU8)MIN_INTERNAL((*_src)*0.35f, 255);
					_dsttmp = *(_dst);
					*_dst++ = (BLU8)MIN_INTERNAL(255, _dsttmp + _srctmp);
					_src++;
				}
			}
			_tmpbuf += _imgw;
			_tmpbuf2 += _imgw;
			_tmpbuf3 += _imgw;
			_tmpbuf4 += _imgw;
		}
		else if (_shadow)
		{
			BLU16 _dsttmp, _srctmp;
			BLU16* _tmpbuf = _buf;
			BLU16* _tmpbuf2 = _buf + _bitmap->pitch;
			for (BLU32 _idx = 0; _idx < _bitmap->rows; ++_idx)
			{
				BLU8* _src = _bitmap->buffer + _idx * _bitmap->pitch;
				BLU8* _dst = (BLU8*)(_tmpbuf);
				BLU8* _dst2 = (BLU8*)(_tmpbuf2);
				for (BLU32 _jdx = 0; _jdx < _bitmap->width; ++_jdx)
				{
					_srctmp = *_src;
					*_dst2++ = 0x40;
					*_dst++ = (BLU8)MIN_INTERNAL(255, 0x40 + (0xFF) * (_srctmp / 255.0));
					*_dst2++ = (BLU8)(0.5f * (*_src++)); _dsttmp = *_src;
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
		blUpdateTexture(_glyphatlas->nCurTexture, 0, 0, BL_CTF_IGNORE, _glyphatlas->nRecordX, _glyphatlas->nRecordY, 0, _imgw, _imgh, 1, _buf);
		free(_buf);
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
_MeasureTextDim(const BLUtf8* _Text, _BLFont* _Font, BLU32 _FontHeight, BLF32* _Width, BLF32* _Height, BLBool _Outline, BLBool _Bold, BLBool _Shadow, BLBool _Italics)
{
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
	const BLUtf16* _str16 = blGenUtf16Str(_Text);
	BLU32 _str16len = blUtf16Length(_str16);
	BLU32 _start = 0, _end = 0;
	while (_end < _str16len)
	{
		if (_start != 0)
			*_Height += _linespace;
		_end = blUtf16Length(_str16);
		for (BLU32 _idx = 0; _idx < _end; ++_idx)
		{
			if (_str16[_idx] == L'\n')
			{
				_end = _idx;
				break;
			}
		}
		BLUtf16* _tmp16 = (BLUtf16*)alloca((_end - _start + 1) * sizeof(BLUtf16));
		for (BLU32 _idx = _start; _idx < _end - _start; ++_idx)
			_tmp16[_idx - _start] = _str16[_idx];
		_tmp16[_end - _start] = 0;
		_start = _end + 1;
		BLU32 _adv = 0;
		BLU16 _flag = 0;
		if (_Italics)
			_flag |= 0xF000;
		if (_Outline)
			_flag |= 0x000F;
		else if (_Bold)
			_flag |= 0x00F0;
		else if (_Shadow)
			_flag |= 0x0F00;
		for (BLU32 _idx = 0; _idx < _end - _start; ++_idx)
		{
			BLUtf16 _char = _tmp16[_idx];
			if (_char == L'\r' || _char == L'\n')
				continue;
			_BLGlyph* _glyph = _QueryGlyph(_Font, MAKEU32(_char, _flag), _FontHeight);
			if (!_glyph)
				continue;
			_adv += _glyph->nAdv;
		}
		if (_adv >= *_Width)
			*_Width = (BLF32)_adv;
		_Height += _lineheight;
	}
	if (_Outline)
	{
		_Width += 2;
		_Height += 2;
	}
	else if (_Bold)
	{
		_Width += 1;
		_Height += 1;
	}
	else if (_Shadow)
	{
		_Height += 1;
	}
	blDeleteUtf16Str((BLUtf16*)_str16);
}
static BLVoid
_ActiveFontHeight(_BLFont* _Font, BLU32 _Height)
{
	_BLGlyphAtlas* _glyphatlas = blDictElement(_Font->pAtlasDict, _Height);
	if (!_glyphatlas)
	{
		_glyphatlas = (_BLGlyphAtlas*)malloc(sizeof(_BLGlyphAtlas));
		memset(_glyphatlas, 0, sizeof(_BLGlyphAtlas));
		_glyphatlas->pGlyphs = blGenDict(FALSE);
		_glyphatlas->pTextures = blGenArray(FALSE);
		_glyphatlas->nCurTexture = INVALID_GUID;
		blDictInsert(_Font->pAtlasDict, _Height, _glyphatlas);
	}
	FT_Set_Char_Size(_Font->sFtFace, _Height * 64, 0, 72 * 64, 72);
}
static _BLWidget*
_QueryWidget(_BLWidget* _Node, BLU32 _HashName, BLBool _SearchChildren_)
{
	_BLWidget* _ret = _PrUIMem->pRoot;
	FOREACH_ARRAY(_BLWidget*, _iter, _Node->pChildren)
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
static BLVoid
_FrontWidget(_BLWidget* _Node)
{
	if (!_Node->pParent)
		return;
	BLU32 _idx = 0;
	FOREACH_ARRAY(_BLWidget*, _iter, _Node->pParent->pChildren)
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
_DrawPanel(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawLabel(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawButton(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawCheck(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawSlider(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawText(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawProgress(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawDial(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawTable(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
}
static BLVoid
_DrawPrimitive(BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
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
			_DrawPanel(_x, _y, _w, _h);
			break;
		case BL_UT_LABEL:
			_DrawLabel(_x, _y, _w, _h);
			break;
		case BL_UT_BUTTON:
			_DrawButton(_x, _y, _w, _h);
			break;
		case BL_UT_CHECK:
			_DrawCheck(_x, _y, _w, _h);
			break;
		case BL_UT_SLIDER:
			_DrawSlider(_x, _y, _w, _h);
			break;
		case BL_UT_TEXT:
			_DrawText(_x, _y, _w, _h);
			break;
		case BL_UT_PROGRESS:
			_DrawProgress(_x, _y, _w, _h);
			break;
		case BL_UT_DIAL:
			_DrawDial(_x, _y, _w, _h);
			break;
		case BL_UT_TABLE:
			_DrawTable(_x, _y, _w, _h);
			break;
		default:
			_DrawPrimitive(_x, _y, _w, _h);
			break;
		}
	}
	else
	{
		_x = _PrUIMem->nFboWidth * 0.5f;
		_y = _PrUIMem->nFboHeight * 0.5f;
	}
	FOREACH_ARRAY(_BLWidget*, _iter, _Node->pChildren)
	{
		_DrawWidget(_iter, _x, _y);
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
    _PrUIMem->pRoot = (_BLWidget*)malloc(sizeof(_BLWidget));
    memset(_PrUIMem->pRoot, 0, sizeof(_BLWidget));
    _PrUIMem->pRoot->sDimension.fX = -1.f;
    _PrUIMem->pRoot->sDimension.fY = -1.f;
	_PrUIMem->pRoot->sPosition.fX = 0.f;
	_PrUIMem->pRoot->sPosition.fY = 0.f;
	_PrUIMem->pRoot->pParent = NULL;
    _PrUIMem->pBasePlate = NULL;
	_PrUIMem->pRoot->eReferenceH = BL_UA_HCENTER;
	_PrUIMem->pRoot->eReferenceV = BL_UA_VCENTER;
	_PrUIMem->pRoot->pChildren = blGenArray(FALSE);
	_PrUIMem->pRoot->eType = BL_UT_PANEL;
	_PrUIMem->pRoot->nID = blGenGuid(_PrUIMem->pRoot, blHashUtf8((const BLUtf8*)"Root"));
    _PrUIMem->pPixmaps = blGenArray(FALSE);
    blWindowSize(&_PrUIMem->nFboWidth, &_PrUIMem->nFboHeight);
}
BLVoid
_UIStep(BLU32 _Delta, BLBool _Baseplate)
{
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
		if (_PrUIMem->bDirty)
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
    FT_Done_FreeType(_PrUIMem->sFtLibrary);
	blDeleteUI(_PrUIMem->pRoot->nID);
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			FT_Done_Face(_iter->sFtFace);
			{
				FOREACH_DICT(_BLGlyphAtlas*, _iter2, _iter->pAtlasDict)
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
			free(_iter);
		}
		blDeleteArray(_PrUIMem->pFonts);
	}
	{
		FOREACH_ARRAY(BLGuid*, _iter, _PrUIMem->pPixmaps)
		{
			blDeleteTexture(*_iter);
			free(_iter);
		}
		blDeleteArray(_PrUIMem->pPixmaps);
	}
	free(_PrUIMem);
}
BLVoid
blUIWorkspace(IN BLAnsi* _Dictionary, IN BLAnsi* _Archive, IN BLBool _UseDesignRes)
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
	blWindowSize(&_PrUIMem->nFboWidth, &_PrUIMem->nFboHeight);
	if (_UseDesignRes)
	{
		const BLAnsi* _res = ezxml_attr(_resolution, "Value");
		BLU32 _split = (BLU32)(strchr(_res, ',') - _res);
		BLAnsi _tmp[32] = { 0 };
		strncpy(_tmp, _res, _split);
		BLS32 _w = (BLS32)strtol(_tmp, NULL, 10);
		memset(_tmp, 0, sizeof(_tmp));
		strncpy(_tmp, _res + _split + 1, strlen(_res) - _split);
		BLS32 _h = (BLS32)strtol(_tmp, NULL, 10);
		BLF32 _ratioorg = (BLF32)_PrUIMem->nFboWidth / (BLF32)_PrUIMem->nFboHeight;
		BLF32 _ratiodeg = (BLF32)_w / (BLF32)_h;
		if (_ratiodeg >= _ratioorg)
		{
			_PrUIMem->nFboHeight = _h;
			_PrUIMem->nFboWidth = (BLU32)(_ratioorg * _h);
		}
		else
		{
			_PrUIMem->nFboHeight = (BLU32)(_w / _ratioorg);
			_PrUIMem->nFboWidth = _w;
		}
	}
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
blUIQueryResolution(OUT BLU32* _Width, OUT BLU32* _Height)
{
	*_Width = _PrUIMem->nFboWidth;
	*_Height = _PrUIMem->nFboHeight;
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
            blUIButtonPixmap(_widguid, _pixmap);
            blUIButtonStencil(_widguid, _stencilmap);
            blUIButtonCommon(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
            blUIButtonHover(_widguid, _hoveredmap, _hoveredtexcoordvar[0], _hoveredtexcoordvar[1], _hoveredtexcoordvar[2], _hoveredtexcoordvar[3]);
            blUIButtonPress(_widguid, _pressedmap, _pressedtexcoordvar[0], _pressedtexcoordvar[1], _pressedtexcoordvar[2], _pressedtexcoordvar[3]);
            blUIButtonDisable(_widguid, _disablemap, _disabletexcoordvar[0], _disabletexcoordvar[1], _disabletexcoordvar[2], _disabletexcoordvar[3]);
            blUIButtonFont(_widguid, _fontsrc, _fontsizevar, _fontoutlinevar, _fontboldvar, _fontshadowvar, _fontitalicsvar);
            blUIButtonFlip(_widguid, _flipxvar, _flipyvar);
            blUIButtonText(_widguid, (const BLUtf8*)_text, _textcolorvar, _txtha, _txtva);
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
        }
        if (_element->sibling)
            _element = _element->sibling;
        else
        {
            do {
                _element = _element->parent;
                if (!_element)
                    break;
            } while (!_element->next);
            _element = _element ? _element->next : NULL;
        }
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
	_widget->fRatio = _widget->sDimension.fX / _widget->sDimension.fY;
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
    _widget->bValid = FALSE;
	switch (_widget->eType)
	{
		case BL_UT_PANEL:
		{
			memset(_widget->uExtension.sPanel.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sPanel.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sPanel.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sPanel.aCommonTex, 0, sizeof(BLRect) * 9);
			_widget->uExtension.sPanel.bDragable = FALSE;
			_widget->uExtension.sPanel.bBasePlate = FALSE;
			_widget->uExtension.sPanel.bModal = FALSE;
			_widget->uExtension.sPanel.bScrollable = TRUE;
			_widget->uExtension.sPanel.bFlipX = FALSE;
			_widget->uExtension.sPanel.bFlipY = FALSE;
            _widget->uExtension.sPanel.nPixmapTex = INVALID_GUID;
            _widget->uExtension.sPanel.pPixmapSheetDct = blGenDict(FALSE);
		}
		break;
		case BL_UT_LABEL:
		{
			_widget->uExtension.sLabel.pText = NULL;
			memset(_widget->uExtension.sLabel.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sLabel.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sLabel.aCommonTex, 0, sizeof(BLRect) * 9);
			memset(_widget->uExtension.sLabel.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sLabel.bFlipX = FALSE;
            _widget->uExtension.sLabel.bFlipY = FALSE;
            _widget->uExtension.sLabel.nPixmapTex = INVALID_GUID;
            _widget->uExtension.sLabel.pPixmapSheetDct = blGenDict(FALSE);
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
			memset(_widget->uExtension.sButton.aCommonTex, 0, sizeof(BLRect) * 9);
			memset(_widget->uExtension.sButton.aHoveredTex, 0, sizeof(BLRect) * 9);
			memset(_widget->uExtension.sButton.aPressedTex, 0, sizeof(BLRect) * 9);
			memset(_widget->uExtension.sButton.aDisableTex, 0, sizeof(BLRect) * 9);
			_widget->uExtension.sButton.eTxtAlignmentH = BL_UA_HCENTER;
			_widget->uExtension.sButton.eTxtAlignmentV = BL_UA_VCENTER;
			_widget->uExtension.sButton.nFontHash = 0xFFFFFFFF;
			_widget->uExtension.sButton.nFontHeight = 0;
			_widget->uExtension.sButton.bOutline = FALSE;
			_widget->uExtension.sButton.bBold = FALSE;
			_widget->uExtension.sButton.bShadow = FALSE;
			_widget->uExtension.sButton.bItalics = FALSE;
			_widget->uExtension.sButton.nTxtColor = 0xFFFFFFFF;
			_widget->uExtension.sButton.bFlipX = FALSE;
			_widget->uExtension.sButton.bFlipY = FALSE;
            _widget->uExtension.sButton.nState = 0;
            _widget->uExtension.sButton.nPixmapTex = INVALID_GUID;
            _widget->uExtension.sButton.pPixmapSheetDct = blGenDict(FALSE);
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
			memset(_widget->uExtension.sCheck.aCommonTex, 0, sizeof(BLRect) * 9);
			memset(_widget->uExtension.sCheck.aPressedTex, 0, sizeof(BLRect) * 9);
			memset(_widget->uExtension.sCheck.aDisableTex, 0, sizeof(BLRect) * 9);
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
            _widget->uExtension.sCheck.nState = 0;
            _widget->uExtension.sCheck.nPixmapTex = INVALID_GUID;
            _widget->uExtension.sCheck.pPixmapSheetDct = blGenDict(FALSE);
		}
		break;
		case BL_UT_SLIDER:
		{
			memset(_widget->uExtension.sSlider.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aSliderCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aSliderDisableMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sSlider.aCommonTex, 0, sizeof(BLRect) * 9);
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
            _widget->uExtension.sSlider.nState = 0;
            _widget->uExtension.sSlider.nPixmapTex = INVALID_GUID;
            _widget->uExtension.sSlider.pPixmapSheetDct = blGenDict(FALSE);
		}
		break;
		case BL_UT_TEXT:
		{
			_widget->uExtension.sText.pText = NULL;
			_widget->uExtension.sText.pPlaceholder = NULL;
			memset(_widget->uExtension.sText.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sText.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sText.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sText.aCommonTex, 0, sizeof(BLRect) * 9);
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
            _widget->uExtension.sText.nState = 0;
            _widget->uExtension.sText.nPixmapTex = INVALID_GUID;
            _widget->uExtension.sText.pPixmapSheetDct = blGenDict(FALSE);
		}
		break;
		case BL_UT_PROGRESS:
		{
			_widget->uExtension.sProgress.pText = NULL;
			memset(_widget->uExtension.sProgress.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aFillMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sProgress.aCommonTex, 0, sizeof(BLRect) * 9);
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
            _widget->uExtension.sProgress.pPixmapSheetDct = blGenDict(FALSE);
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
            _widget->uExtension.sDial.pPixmapSheetDct = blGenDict(FALSE);
		}
		break;
		case BL_UT_TABLE:
		{
			memset(_widget->uExtension.sTable.aPixmap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aCommonMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aStencilMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aOddItemMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aEvenItemMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sTable.aCommonTex, 0, sizeof(BLRect) * 9);
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
            _widget->uExtension.sTable.pPixmapSheetDct = blGenDict(FALSE);
		}
		break;
		default:
		{
			_widget->uExtension.sPrimitive.bFill = TRUE;
			_widget->uExtension.sPrimitive.bClosed = TRUE;
			_widget->uExtension.sPrimitive.nColor = 0x88888888;
			_widget->uExtension.sPrimitive.pXPath = NULL;
			_widget->uExtension.sPrimitive.pYPath = NULL;
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
        case BL_UT_PANEL:
        {
            if (_widget->uExtension.sPanel.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sPanel.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sPanel.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_LABEL:
        {
            if (_widget->uExtension.sLabel.pText)
                free(_widget->uExtension.sLabel.pText);
            if (_widget->uExtension.sLabel.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sLabel.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sLabel.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_BUTTON:
        {
            if (_widget->uExtension.sButton.pText)
                free(_widget->uExtension.sButton.pText);
            if (_widget->uExtension.sButton.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sButton.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sButton.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_CHECK:
        {
            if (_widget->uExtension.sCheck.pText)
                free(_widget->uExtension.sCheck.pText);
            if (_widget->uExtension.sCheck.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sCheck.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sCheck.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_SLIDER:
        {
            if (_widget->uExtension.sSlider.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sSlider.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sSlider.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_TEXT:
        {
            if (_widget->uExtension.sText.pText)
                free(_widget->uExtension.sText.pText);
            if (_widget->uExtension.sText.pPlaceholder)
                free(_widget->uExtension.sText.pPlaceholder);
            if (_widget->uExtension.sText.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sText.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sText.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_PROGRESS:
        {
            if (_widget->uExtension.sProgress.pText)
                free(_widget->uExtension.sProgress.pText);
            if (_widget->uExtension.sProgress.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sProgress.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sProgress.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_DIAL:
        {
            if (_widget->uExtension.sDial.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sDial.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sDial.pPixmapSheetDct);
            }
        }
        break;
        case BL_UT_TABLE:
        {
            if (_widget->uExtension.sTable.pPixmapSheetDct)
            {
                FOREACH_DICT(_BLPixmapSheet*, _iter, _widget->uExtension.sTable.pPixmapSheetDct)
                {
                    free(_iter);
                }
                blDeleteDict(_widget->uExtension.sTable.pPixmapSheetDct);
            }
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
    {
        FOREACH_ARRAY(_BLWidget*, _iter, _widget->pChildren)
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
blUIPanelStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUIPanelCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
BLGuid
_LoadBmg(IN BLAnsi* _Pixmap, BLU32 _Hash)
{
    BLAnsi _texfile[260] = { 0 };
    memset(_texfile, 0, sizeof(_texfile));
    BLAnsi _path[260] = { 0 };
    if (_PrUIMem->aArchive[0] == 0)
    {
        strcpy(_path, blWorkingDir(TRUE));
        strcat(_path, _PrUIMem->aDir);
    }
    else
        strcpy(_path, _PrUIMem->aDir);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
    strcat(_path, "\\pixmap\\");
#else
    strcat(_path, "/pixmap/");
#endif
    strcat(_path, _Pixmap);
    strcat(_path, ".bmg");
    BLGuid _tex = blGenStream(_path, _PrUIMem->aArchive[0] ? _PrUIMem->aArchive : NULL);
    return 0;
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
    BLU32 _hash = blHashUtf8((const BLUtf8*)_texfile);
    _widget->uExtension.sButton.nPixmapTex = blGainTexture(_hash);
    if (INVALID_GUID == _widget->uExtension.sButton.nPixmapTex)
        _widget->uExtension.sButton.nPixmapTex = _LoadBmg(_Pixmap, _hash);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUIButtonCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aCommonMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aCommonMap, _CommonMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonHover(IN BLGuid _ID, IN BLAnsi* _HoveredMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aHoveredMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aHoveredMap, _HoveredMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonPress(IN BLGuid _ID, IN BLAnsi* _PressedMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aPressedMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aPressedMap, _PressedMap);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUIButtonDisable(IN BLGuid _ID, IN BLAnsi* _DisableMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_BUTTON)
		return;
	memset(_widget->uExtension.sButton.aDisableMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sButton.aDisableMap, _DisableMap);
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
	_widget->uExtension.sButton.nFontHash = blHashUtf8((const BLUtf8*)_Font);
	_widget->uExtension.sButton.nFontHeight = _FontHeight;
	_widget->uExtension.sButton.bOutline = _Outline;
	_widget->uExtension.sButton.bBold = _Bold;
	_widget->uExtension.sButton.bShadow = _Shadow;
	_widget->uExtension.sButton.bItalics = _Italics;
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
blUILabelStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUILabelCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUICheckStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUICheckCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUICheckPress(IN BLGuid _ID, IN BLAnsi* _PressedMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUICheckDisable(IN BLGuid _ID, IN BLAnsi* _DisableMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUITextStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUITextCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUIProgressStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUIProgressCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUIProgressFill(IN BLGuid _ID, IN BLAnsi* _FillMap)
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
blUISliderStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUISliderCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUISliderSliderCommon(IN BLGuid _ID, IN BLAnsi* _SliderCommonMap)
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
blUISliderSliderDisable(IN BLGuid _ID, IN BLAnsi* _SliderDisableMap)
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
blUITableStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUITableCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
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
blUITableOddItem(IN BLGuid _ID, IN BLAnsi* _OddItemMap)
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
blUITableEvenItem(IN BLGuid _ID, IN BLAnsi* _EvenItemMap)
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
blUIDialStencil(IN BLGuid _ID, IN BLAnsi* _StencilMap)
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
blUIDialCommon(IN BLGuid _ID, IN BLAnsi* _CommonMap)
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
	_PrUIMem->bDirty = TRUE;
}
