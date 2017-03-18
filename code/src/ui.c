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
			BLAnsi aCheckedMap[128];
			BLAnsi aDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sCheckedTex9;
			BLRect sCheckedTex;
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
			BLAnsi aFontSource[32];
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLU32 nTxtColor;
			BLU32 nPlaceholderColor;
			BLU32 nMaxLength;
			BLS32 nMinValue;
			BLS32 nMaxValue;
			BLBool bSelecting;
			BLBool bAutoscroll;
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
			BLRect sCurRect;
			BLS32 aSplitPositions[2048];
			BLUtf16* pSplitText[2048];
			BLU32 nSplitSize;
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
			BLU32 nBorderX;
			BLU32 nBorderY;
			BLAnsi aFontSource[32];
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
			BLAnsi aDisableMap[128];
			BLAnsi aSliderCommonMap[128];
			BLAnsi aSliderDisableMap[128];
			BLAnsi aStencilMap[128];
			BLRect sCommonTex9;
			BLRect sCommonTex;
			BLRect sDisableTex9;
			BLRect sDisableTex;
			BLRect sSliderCommonTex;
			BLRect sSliderDisableTex;
			BLRect sStencilTex;
			BLBool bHorizontal;
			BLBool bSliderDragged;
			BLBool bTrayClick;
			BLBool bDragging;
			BLS32 nMinValue;
			BLS32 nMaxValue;
			BLS32 nStep;
			BLS32 nSliderPosition;
			BLS32 nDesiredPos;
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
			BLAnsi aFontSource[32];
			BLU32 nFontHeight;
			BLBool bOutline;
			BLBool bBold;
			BLBool bShadow;
			BLBool bItalics;
			BLBool bFlipX;
            BLBool bFlipY;
            BLGuid nPixmapTex;
			BLS32 aColumnWidth[32];
			BLU32 nColumnNum;
			BLUtf8** pCellText;
			BLUtf16** pCellBroken;
			BLU32* pCellColor;
			BLU32 nCellNum;
			BLS32 nScroll;
			BLU32 nRowHeight;
			BLU32 nTotalHeight;
			BLU32 nTotalWidth;
			BLU32 nSelectedCell;
			BLBool bSelecting;
			BLBool bDragging;
			BLF32 fDraggingPos;
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
			BLF32 fAngle;
			BLS32 nEndAngle;
			BLBool bAngleCut;
			BLBool bClockWise;
			BLBool bDragging;
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
	BLGuid nGlyphGeo;
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
_WidgetAbsArea(_BLWidget* _Node, BLRect* _Area)
{
	if (!_Node->pParent)
		return;
	else
	{
		_BLWidget* _tmp = _Node;
		BLF32 _x = _Node->sPosition.fX;
		BLF32 _y = _Node->sPosition.fY;
		BLF32 _pw;
		BLF32 _ph;
		while (_tmp != _PrUIMem->pRoot)
		{
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
			_tmp = _tmp->pParent;
		}
		_Area->sLT.fX = _x - _Node->sDimension.fX * 0.5f;
		_Area->sLT.fY = _y - _Node->sDimension.fY * 0.5f;
		_Area->sRB.fX = _x + _Node->sDimension.fX * 0.5f;
		_Area->sRB.fY = _y + _Node->sDimension.fY * 0.5f;
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
	{
		_glyphatlas = (_BLGlyphAtlas*)malloc(sizeof(_BLGlyphAtlas));
		memset(_glyphatlas, 0, sizeof(_BLGlyphAtlas));
		_glyphatlas->pTextures = blGenArray(FALSE);
		_glyphatlas->pGlyphs = blGenDict(FALSE);
		_glyphatlas->nCurTexture = INVALID_GUID;
		blDictInsert(_Font->pGlyphAtlas, _FontHeight, _glyphatlas);
		FT_Set_Char_Size(_Font->sFtFace, _FontHeight * 64, 0, 72 * 64, 72);
	}
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
static BLS32 
_CharIndex(_BLFont* _Font, BLU32 _FontHeight, const BLUtf16* _Text, BLS32 _X, BLU16 _Flag)
{
	BLS32 _ret = 0;
	BLU32 _txtlen = blUtf16Length(_Text);
	for (BLU32 _idx = 0; _idx < _txtlen; ++_idx)
	{
		if (_ret >= _X)
			return (_idx == 0) ? 0 : (_idx - 1);
		BLUtf16 _c = _Text[_idx];
		if (_c == L'\r' || _c == L'\n')
			continue;
		_BLGlyph* _gi = _QueryGlyph(_Font, MAKEU32(_c, _Flag), _FontHeight);
		if (!_gi)
			continue;
		_ret += _gi->nAdv;
	}
	if (_ret >= _X)
		return _txtlen - 1;
	else
		return -1;
}
static BLVoid
_MeasureTextDim(const BLUtf16* _Text, _BLFont* _Font, BLU32 _FontHeight, BLF32* _Width, BLF32* _Height, BLU16 _Flag)
{
	BLBool _outline = (_Flag & 0x000F) > 0;
	BLBool _bold = (_Flag & 0x00F0) > 0;
	BLBool _shadow = (_Flag & 0x0F00) > 0;
	BLBool _italics = (_Flag & 0xF000) > 0;
	if (!_Text)
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
		for (BLU32 _idx = _start; _idx < _end; ++_idx)
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
static BLUtf16*
_BreakText(_BLFont* _Font, BLU32 _FontHeight, BLUtf16* _Text, BLU32 _Width, BLU16 _Flag)
{
	BLU32 _lineleftw = _Width;
	BLU32 _s16len = blUtf16Length(_Text);
	BLU32 _newlen = _s16len;
	BLUtf16* _ret = (BLUtf16*)malloc((_s16len + 1) * sizeof(BLUtf16));
	for (BLU32 _jdx = 0; _jdx < _s16len; ++_jdx)
		_ret[_jdx] = _Text[_jdx];
	_ret[_s16len] = 0;
	for (BLU32 _idx = 0; _idx < _s16len; ++_idx)
	{
		BLUtf16 _ch = _Text[_idx];
		if (_ch == L'\n')
			_lineleftw = _Width;
		BLU32 _adv;
		if (_ch == L'\r' || _ch == L'\n')
			_adv = 0;
		else
		{
			_BLGlyph* _gi =_QueryGlyph(_Font, MAKEU32(_ch, _Flag), _FontHeight);
			if (!_gi)
			{
				free(_Text);
				return _ret;
			}
			_adv = _gi->nAdv;
		}
		if (_lineleftw < _adv)
		{
			if (_Width < _adv)
				break;
			_lineleftw = _Width;
			_newlen++;
			_ret = (BLUtf16*)realloc(_ret, (_newlen + 1) * sizeof(BLUtf16));
			memmove(_ret + _idx + 1, _ret + _idx, (_newlen - 1 - _idx));
			_ret[_idx] = L'\n';
			_ret[_newlen] = 0;
		}
		else
			_lineleftw -= _adv;
	}
	free(_Text);
	return _ret;
}
static BLS32
_TextLine(_BLWidget* _Node, BLS32 _Pos)
{
	if ( !_Node->uExtension.sText.bMultiline)
		return 0;
	BLS32 _idx = 0;
	while (_idx < (BLS32)_Node->uExtension.sText.nSplitSize)
	{
		if (_Node->uExtension.sText.aSplitPositions[_idx] > _Pos)
			return _idx - 1;
		++_idx;
	}
	return (BLS32)_Node->uExtension.sText.nSplitSize - 1;
}
static void 
_TextRect(_BLWidget* _Node, _BLFont* _Font, BLS32 _Line, BLRect* _Rect, BLU16 _Flag)
{
	if (_Line < 0)
		return;
	if (!_Font)
		return;
	BLVec2 _d;
	BLU32 _linecount = _Node->uExtension.sText.bMultiline ? _Node->uExtension.sText.nSplitSize : 1;
	if (_Node->uExtension.sText.bMultiline)
	{
		_MeasureTextDim(_Node->uExtension.sText.pSplitText[_Line], _Font, _Node->uExtension.sText.nFontHeight, &_d.fX, &_d.fY, _Flag);
		_d.fY = (_d.fY) ? _d.fY : _Node->uExtension.sText.nFontHeight;
	}
	else
	{
		if (_Node->uExtension.sText.bPassword)
		{
			const BLUtf16* _str16 = blGenUtf16Str(_Node->uExtension.sText.pText);
			BLU32 _str16len = blUtf16Length(_str16);
			BLUtf16* _starph = (BLUtf16*)alloca((_str16len + 1) * sizeof(BLUtf16));
			for (BLU32 _jdx = 0; _jdx < _str16len; ++_jdx)
				_starph[_jdx] = L'*';
			_starph[_str16len] = 0;
			_MeasureTextDim(_Node->uExtension.sText.bPassword ? _starph : _str16, _Font, _Node->uExtension.sText.nFontHeight, &_d.fX, &_d.fY, _Flag);
			_d.fY = (_d.fY) ? _d.fY : _Node->uExtension.sText.nFontHeight;
			blDeleteUtf16Str((BLUtf16*)_str16);
		}
		else
		{
			const BLUtf16* _str16 = blGenUtf16Str(_Node->uExtension.sText.pText);
			_MeasureTextDim(_str16, _Font, _Node->uExtension.sText.nFontHeight, &_d.fX, &_d.fY, _Flag);
			_d.fY = (_d.fY) ? _d.fY : _Node->uExtension.sText.nFontHeight;
			blDeleteUtf16Str((BLUtf16*)_str16);
		}
	}
	switch (_Node->uExtension.sText.eTxtAlignmentH)
	{
	case BL_UA_LEFT:
		_Node->uExtension.sText.sCurRect.sLT.fX = 0.f;
		_Node->uExtension.sText.sCurRect.sRB.fX = _d.fX;
		break;
	case BL_UA_HCENTER:
		_Node->uExtension.sText.sCurRect.sLT.fX = ((_Rect->sRB.fX - _Rect->sLT.fX) * 0.5f) - (_d.fX * 0.5f);
		_Node->uExtension.sText.sCurRect.sRB.fX = ((_Rect->sRB.fX - _Rect->sLT.fX) * 0.5f) + (_d.fX * 0.5f);
		break;
	case BL_UA_RIGHT:
		_Node->uExtension.sText.sCurRect.sLT.fX = _Rect->sRB.fX - _Rect->sLT.fX - _d.fX;
		_Node->uExtension.sText.sCurRect.sRB.fX = _Rect->sRB.fX - _Rect->sLT.fX;
		break;
	default:assert(0); break;
	}
	switch (_Node->uExtension.sText.eTxtAlignmentV)
	{
	case BL_UA_TOP:
		_Node->uExtension.sText.sCurRect.sLT.fY = _d.fY * _Line;
		break;
	case BL_UA_VCENTER:
		_Node->uExtension.sText.sCurRect.sLT.fY = ((_Rect->sRB.fY - _Rect->sLT.fY) * 0.5f) - (_linecount * _d.fY) * 0.5f + _d.fY * _Line;
		break;
	case BL_UA_BOTTOM:
		_Node->uExtension.sText.sCurRect.sLT.fY = _Rect->sRB.fY - _Rect->sLT.fY - _linecount * _d.fY + _d.fY * _Line;
		break;
	default:assert(0); break;
	}
	_Node->uExtension.sText.sCurRect.sLT.fX -= _Node->uExtension.sText.nScrollPosH;
	_Node->uExtension.sText.sCurRect.sRB.fX -= _Node->uExtension.sText.nScrollPosH;
	_Node->uExtension.sText.sCurRect.sLT.fY -= _Node->uExtension.sText.nScrollPosV;
	_Node->uExtension.sText.sCurRect.sRB.fY = _Node->uExtension.sText.sCurRect.sLT.fY + _d.fY;
	_Node->uExtension.sText.sCurRect.sLT.fX += _Rect->sLT.fX;
	_Node->uExtension.sText.sCurRect.sLT.fY += _Rect->sLT.fY;
	_Node->uExtension.sText.sCurRect.sRB.fX += _Rect->sLT.fX;
	_Node->uExtension.sText.sCurRect.sRB.fY += _Rect->sLT.fY;
}
static void 
_TextSplit(_BLWidget* _Node, _BLFont* _Font, BLU16 _Flag)
{
	if (!_Node->uExtension.sText.bMultiline)
		return;
	if (_Node->uExtension.sText.nSplitSize >= 2048)
		return;
	_Node->uExtension.sText.nSplitSize = 0;
	const BLUtf16* _text = blGenUtf16Str(_Node->uExtension.sText.pText);
	BLUtf16* _line = NULL;
	BLUtf16* _word = (BLUtf16*)alloca(128 * sizeof(BLUtf16));
	memset(_word, 0, 128 * sizeof(BLUtf16));
	BLUtf16* _whitespace = (BLUtf16*)alloca(128 * sizeof(BLUtf16));
	memset(_whitespace, 0, 128 * sizeof(BLUtf16));
	BLU32 _whiteidx = 0, _wordidx = 0;
	BLS32 _lastlinestart = 0;
	BLS32 _size = (BLS32)blUtf16Length(_text);
	BLS32 _length = 0;
	BLS32 _elwidth = (BLS32)(_Node->sDimension.fX - _Node->uExtension.sText.nPaddingX * 2.f) - 6;
	BLUtf16 _c;
	for (BLS32 _idx = 0; _idx < _size; ++_idx)
	{
		_c = _text[_idx];
		BLBool _linebreak = FALSE;
		if (_c == L'\r')
		{
			_linebreak = TRUE;
			_c = 0;
			if (_text[_idx + 1] == L'\n')
			{
				memmove((BLUtf16*)_text + _idx + 1, _text + _idx + 2, _size - _idx - 2);
				--_size;
				if (_Node->uExtension.sText.nCaretPos > _idx)
					--_Node->uExtension.sText.nCaretPos;
			}
		}
		else if (_c == L'\n')
		{
			_linebreak = TRUE;
			_c = 0;
		}
		if (!_Node->uExtension.sText.bMultiline)
			_linebreak = FALSE;
		if (_c == L' ' || _c == 0 || _idx == (_size - 1))
		{
			BLF32 _whitelen = 0.f, _wordlen = 0.f, _tmp = 0.f;
			_MeasureTextDim(_whitespace, _Font, _Node->uExtension.sText.nFontHeight, &_whitelen, &_tmp, _Flag);
			_MeasureTextDim(_word, _Font, _Node->uExtension.sText.nFontHeight, &_wordlen, &_tmp, _Flag);
			BLU32 _wslen = blUtf16Length(_whitespace);
			BLU32 _wdlen = blUtf16Length(_word);
			BLU32 _llen = blUtf16Length(_line);
			_line = (BLUtf16*)realloc(_line, (_llen + _wslen + _wdlen + 1) * sizeof(BLUtf16));
			BLU32 _jdx = 0;
			for (; _jdx < _wslen; ++_jdx)
				_line[_llen + _jdx] = _whitespace[_jdx];
			_jdx = 0;
			for (; _jdx < _wdlen; ++_jdx)
				_line[_llen + _wslen + _jdx] = _word[_jdx];
			_line[_llen + _wslen + _wdlen] = 0;
			_length += (BLS32)_whitelen + (BLS32)_wordlen;
			_wordidx = 0;
			_whiteidx = 0;
			memset(_word, 0, 128 * sizeof(BLUtf16));
			memset(_whitespace, 0, 128 * sizeof(BLUtf16));
			if (_c)
			{
				_whitespace[_whiteidx++] = _c;
				_whitespace[_whiteidx] = 0;
			}
			if (_linebreak)
			{
				BLU32 _wslen = blUtf16Length(_whitespace);
				BLU32 _wdlen = blUtf16Length(_word);
				BLU32 _llen = blUtf16Length(_line);
				_line = (BLUtf16*)realloc(_line, (_llen + _wslen + _wdlen + 1) * sizeof(BLUtf16));
				BLU32 _jdx = 0;
				for (; _jdx < _wslen; ++_jdx)
					_line[_llen + _jdx] = _whitespace[_jdx];
				_jdx = 0;
				for (; _jdx < _wdlen; ++_jdx)
					_line[_llen + _wslen + _jdx] = _word[_jdx];
				_line[_llen + _wslen + _wdlen] = 0;
				if (_Node->uExtension.sText.pSplitText[_Node->uExtension.sText.nSplitSize])
					free(_Node->uExtension.sText.pSplitText[_Node->uExtension.sText.nSplitSize]);
				_Node->uExtension.sText.pSplitText[_Node->uExtension.sText.nSplitSize] = _line;
				_Node->uExtension.sText.aSplitPositions[_Node->uExtension.sText.nSplitSize] = _lastlinestart;
				_Node->uExtension.sText.nSplitSize++;
				_lastlinestart = _idx + 1;
				_line = NULL;
				_wordidx = 0;
				_whiteidx = 0;
				memset(_word, 0, 128 * sizeof(BLUtf16));
				memset(_whitespace, 0, 128 * sizeof(BLUtf16));
				_length = 0;
			}
		}
		else
		{
			_word[_wordidx++] = _c;
			_word[_wordidx] = 0;
		}
	}
	BLU32 _wslen = blUtf16Length(_whitespace);
	BLU32 _wdlen = blUtf16Length(_word);
	BLU32 _llen = blUtf16Length(_line);
	_line = (BLUtf16*)realloc(_line, (_llen + _wslen + _wdlen + 1) * sizeof(BLUtf16));
	BLU32 _jdx = 0;
	for (; _jdx < _wslen; ++_jdx)
		_line[_llen + _jdx] = _whitespace[_jdx];
	_jdx = 0;
	for (; _jdx < _wdlen; ++_jdx)
		_line[_llen + _wslen + _jdx] = _word[_jdx];
	_line[_llen + _wslen + _wdlen] = 0;
	if (_Node->uExtension.sText.pSplitText[_Node->uExtension.sText.nSplitSize])
		free(_Node->uExtension.sText.pSplitText[_Node->uExtension.sText.nSplitSize]);
	_Node->uExtension.sText.pSplitText[_Node->uExtension.sText.nSplitSize] = _line;
	_Node->uExtension.sText.aSplitPositions[_Node->uExtension.sText.nSplitSize] = _lastlinestart;
	_Node->uExtension.sText.nSplitSize++;
	blDeleteUtf16Str((BLUtf16*)_text);
}
static BLS32 
_CaretPos(_BLWidget* _Node, BLVec2* _Pos)
{
	BLU32 _linecount = _Node->uExtension.sText.bMultiline ? _Node->uExtension.sText.nSplitSize : 1;
	BLUtf16* _curline = NULL;
	BLS32 _startpos = 0;
	BLF32 _x, _y;
	_x = _Pos->fX + 3;
	_y = _Pos->fY;
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_Node->uExtension.sText.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return 0;
	const BLUtf16* _text16 = blGenUtf16Str(_Node->uExtension.sText.pText);
	BLU16 _flag = 0;
	if (_Node->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sText.bItalics)
		_flag |= 0xF000;
	BLRect _framerect;
	_WidgetAbsArea(_Node, &_framerect);
	_framerect.sLT.fX += _Node->uExtension.sText.nPaddingX;
	_framerect.sLT.fY += _Node->uExtension.sText.nPaddingY;
	_framerect.sRB.fX -= _Node->uExtension.sText.nPaddingX;
	_framerect.sRB.fY -= _Node->uExtension.sText.nPaddingY;
	for (BLU32 _idx = 0; _idx < _linecount; ++_idx)
	{
		_TextRect(_Node, _ft, _idx, &_framerect, _flag);
		if (_idx == 0 && _y < _Node->uExtension.sText.sCurRect.sLT.fY)
			_y = _Node->uExtension.sText.sCurRect.sLT.fY;
		if (_idx == _linecount - 1 && _y > _Node->uExtension.sText.sCurRect.sRB.fY)
			_y = _Node->uExtension.sText.sCurRect.sRB.fY;
		if (_y >= _Node->uExtension.sText.sCurRect.sLT.fY && _y <= _Node->uExtension.sText.sCurRect.sRB.fY)
		{
			_curline = _Node->uExtension.sText.bMultiline ? _Node->uExtension.sText.pSplitText[_idx] : _text16;
			_startpos = _Node->uExtension.sText.bMultiline ? _Node->uExtension.sText.aSplitPositions[_idx] : 0;
			break;
		}
	}
	if (_x < _Node->uExtension.sText.sCurRect.sLT.fX)
		_x = _Node->uExtension.sText.sCurRect.sLT.fX;
	BLS32 _idx;
	BLU32 _s16len = blUtf16Length(_curline);
	if (!_s16len)
		_idx = -1;
	else
	{
		if (_Node->uExtension.sText.bPassword)
		{
			BLUtf16* _starph = (BLUtf16*)alloca((_s16len + 1) * sizeof(BLUtf16));
			memset(_starph, 0, (_s16len + 1) * sizeof(BLUtf16));
			for (BLU32 _jdx = 0; _jdx < _s16len; ++_jdx)
				_starph[_jdx] = L'*';
			_idx = _CharIndex(_ft, _Node->uExtension.sText.nFontHeight, _starph, (BLS32)(_x - _Node->uExtension.sText.sCurRect.sLT.fX), _flag);
		}
		else
			_idx = _CharIndex(_ft, _Node->uExtension.sText.nFontHeight, _curline, (BLS32)(_x - _Node->uExtension.sText.sCurRect.sLT.fX), _flag);
	}
	BLS32 _ret;
	if (_idx != -1)
		_ret = _idx + _startpos;
	else
		_ret = blUtf16Length(_curline) + _startpos;
	blDeleteUtf16Str((BLUtf16*)_text16);
	return _ret;
}
static void 
_TextScroll(_BLWidget* _Node)
{
	if (!_Node->uExtension.sText.bAutoscroll)
		return;
	BLS32 _curline = _TextLine(_Node, _Node->uExtension.sText.nCaretPos);
	if (_curline < 0)
		return;
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_Node->uExtension.sText.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return;
	BLU16 _flag = 0;
	if (_Node->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sText.bItalics)
		_flag |= 0xF000;
	BLRect _framerect;
	_WidgetAbsArea(_Node, &_framerect);
	_framerect.sLT.fX += _Node->uExtension.sText.nPaddingX;
	_framerect.sLT.fY += _Node->uExtension.sText.nPaddingY;
	_framerect.sRB.fX -= _Node->uExtension.sText.nPaddingX;
	_framerect.sRB.fY -= _Node->uExtension.sText.nPaddingY;
	_TextRect(_Node, _ft, _curline, &_framerect, _flag);
	BLU32 _caretwidth = 1;
	const BLUtf16* _s16 = blGenUtf16Str(_Node->uExtension.sText.pText);
	BLUtf16* _curtxt = _Node->uExtension.sText.bMultiline ? _Node->uExtension.sText.pSplitText[_curline] : _s16;
	BLS32 _cpos = _Node->uExtension.sText.bMultiline ? _Node->uExtension.sText.nCaretPos - _Node->uExtension.sText.aSplitPositions[_curline] : _Node->uExtension.sText.nCaretPos;
	BLF32 _cbegin, _cend, _txtw, _tmp;
	BLUtf16* _cursubstr = (BLUtf16*)alloca((1 + _cpos) * sizeof(BLUtf16));
	memset(_cursubstr, 0, (1 + _cpos) * sizeof(BLUtf16));
	for (BLS32 _jdx = 0; _jdx < _cpos; ++_jdx)
		_cursubstr[_jdx] = _curtxt[_jdx];
	_MeasureTextDim(_cursubstr, _ft, _Node->uExtension.sText.nFontHeight, &_cbegin, &_tmp, _flag);
	_cend = _cbegin + _caretwidth;
	_MeasureTextDim(_curtxt, _ft, _Node->uExtension.sText.nFontHeight, &_txtw, &_tmp, _flag);
	if (_txtw < _framerect.sRB.fX - _framerect.sLT.fX)
	{
		_Node->uExtension.sText.nScrollPosH = 0;
		_TextRect(_Node, _ft, _curline, &_framerect, _flag);
	}
	if (_Node->uExtension.sText.sCurRect.sLT.fX + _cbegin < _framerect.sLT.fX)
	{
		_Node->uExtension.sText.nScrollPosH -= (BLS32)(_framerect.sLT.fX - (_Node->uExtension.sText.sCurRect.sLT.fX + _cbegin));
		_TextRect(_Node, _ft, _curline, &_framerect, _flag);
	}
	else if (_Node->uExtension.sText.sCurRect.sLT.fX + _cend > _framerect.sRB.fX)
	{
		_Node->uExtension.sText.nScrollPosH += (BLS32)((_Node->uExtension.sText.sCurRect.sLT.fX + _cend) - _framerect.sRB.fX);
		_TextRect(_Node, _ft, _curline, &_framerect, _flag);
	}
	if (_Node->uExtension.sText.bMultiline)
	{
		BLF32 _lineheight;
		_MeasureTextDim(L"A", _ft, _Node->uExtension.sText.nFontHeight, &_tmp, &_lineheight, _flag);
		if (_lineheight >= _framerect.sRB.fY - _framerect.sLT.fY)
		{
			_Node->uExtension.sText.nScrollPosV = 0;
			_TextRect(_Node, _ft, _curline, &_framerect, _flag);
			BLS32 _unscrolledpos = (BLS32)_Node->uExtension.sText.sCurRect.sLT.fY;
			BLS32 _pivot = (BLS32)_framerect.sLT.fY;
			switch (_Node->uExtension.sText.eTxtAlignmentV)
			{
			case BL_UA_VCENTER:
				_pivot += (BLS32)((_framerect.sRB.fY - _framerect.sLT.fY) * 0.5f);
				_unscrolledpos += (BLS32)(_lineheight * 0.5f);
				break;
			case BL_UA_BOTTOM:
				_pivot += (BLS32)(_framerect.sRB.fY - _framerect.sLT.fY);
				_unscrolledpos += (BLS32)(_lineheight);
				break;
			default:break;
			}
			_Node->uExtension.sText.nScrollPosV = _unscrolledpos - _pivot;
			_TextRect(_Node, _ft, _curline, &_framerect, _flag);
		}
		else
		{
			_TextRect(_Node, _ft, 0, &_framerect, _flag);
			if (_Node->uExtension.sText.sCurRect.sLT.fY > _framerect.sLT.fY && _Node->uExtension.sText.eTxtAlignmentV != BL_UA_BOTTOM)
				_Node->uExtension.sText.nScrollPosV = 0;
			else if (_Node->uExtension.sText.eTxtAlignmentV != BL_UA_TOP)
			{
				BLU32 _lastline = (!_Node->uExtension.sText.nSplitSize) ? 0 : _Node->uExtension.sText.nSplitSize - 1;
				_TextRect(_Node, _ft, _lastline, &_framerect, _flag);
				if (_Node->uExtension.sText.sCurRect.sRB.fY < _framerect.sRB.fY)
					_Node->uExtension.sText.nScrollPosV -= (BLS32)(_framerect.sRB.fY - _Node->uExtension.sText.sCurRect.sRB.fY);
			}
			_TextRect(_Node, _ft, _curline, &_framerect, _flag);
			if (_Node->uExtension.sText.sCurRect.sLT.fY < _framerect.sLT.fY)
			{
				_Node->uExtension.sText.nScrollPosV -= (BLS32)(_framerect.sLT.fY - _Node->uExtension.sText.sCurRect.sLT.fY);
				_TextRect(_Node, _ft, _curline, &_framerect, _flag);
			}
			else if (_Node->uExtension.sText.sCurRect.sRB.fY > _framerect.sRB.fY)
			{
				_Node->uExtension.sText.nScrollPosV += (BLS32)(_Node->uExtension.sText.sCurRect.sRB.fY - _framerect.sRB.fY);
				_TextRect(_Node, _ft, _curline, &_framerect, _flag);
			}
		}
	}
	blDeleteUtf16Str((BLUtf16*)_s16);
}
static void
_TableSplit(_BLWidget* _Node, _BLFont* _Font, BLU32 _Flag, const BLUtf8* _Text, BLUtf16** _BrokenText, BLU32 _Width)
{
	if (!_Text)
		return;
	BLUtf16* _line = NULL;
	BLUtf16* _linedots = NULL;
	BLUtf16 _c[2];
	_c[1] = L'\0';
	BLU32 _maxlen = _Width - 4;
	BLF32 _txtw, _txth;
	_MeasureTextDim(L"...", _Font, _Node->uExtension.sTable.nFontHeight, &_txtw, &_txth, _Flag);
	BLU32 _dontsmaxlLen = (BLU32)(_Width - _txtw);
	BLU32 _pos = 0;
	BLU32 _idx;
	const BLUtf16* _txt = blGenUtf16Str(_Text);
	BLU32 _size = blUtf16Length(_txt);
	BLUtf16* _tmp = alloca(1024 * sizeof(BLUtf16));
	for (_idx = 0; _idx < _size; ++_idx)
	{
		_c[0] = _txt[_idx];
		if (_c[0] == L'\n')
			break;
		_MeasureTextDim(_c, _Font, _Node->uExtension.sTable.nFontHeight, &_txtw, &_txth, _Flag);
		_pos += (BLU32)_txtw;
		if (_pos > _maxlen)
			break;
		BLU32 _tmplen = blUtf16Length(_line);
		if (_line)
		{
			for (BLU32 _jdx = 0; _jdx < _tmplen; ++_jdx)
				_tmp[_jdx] = _line[_jdx];			
		}
		_tmp[_tmplen] = _c[0];
		_tmp[_tmplen + 1] = 0;
		_MeasureTextDim(_tmp, _Font, _Node->uExtension.sTable.nFontHeight, &_txtw, &_txth, _Flag);
		if (_txtw > _dontsmaxlLen)
		{
			BLU32 _linelen = blUtf16Length(_line);
			_linedots = (BLUtf16*)alloca((1 + _linelen) * sizeof(BLUtf16));
			for (BLU32 _jdx = 0; _jdx < _linelen; ++_jdx)
				_linedots[_jdx] = _line[_jdx];
			_linedots[_linelen] = 0;
		}
		_line = (BLUtf16*)realloc(_line, (_tmplen + 2) * sizeof(BLUtf16));
		_line[_tmplen] = _c[0];
		_line[_tmplen + 1] = 0;
	}
	if (_idx < _size)
	{
		if (*_BrokenText)
			free(*_BrokenText);
		BLU32 _linelen = blUtf16Length(_linedots);
		BLUtf16* _btmp = (BLUtf16*)malloc((_linelen + 4) * sizeof(BLUtf16));
		for (BLU32 _jdx = 0; _jdx < _linelen; ++_jdx)
			_btmp[_jdx] = _linedots[_jdx];
		_btmp[_linelen + 0] = L'.';
		_btmp[_linelen + 1] = L'.';
		_btmp[_linelen + 2] = L'.';
		_btmp[_linelen + 3] = 0;
		*_BrokenText = _btmp;
	}
	else
	{
		if (*_BrokenText)
			free(*_BrokenText);
		BLU32 _linelen = blUtf16Length(_line);
		BLUtf16* _btmp = (BLUtf16*)malloc((_linelen + 1) * sizeof(BLUtf16));
		for (BLU32 _jdx = 0; _jdx < _linelen; ++_jdx)
			_btmp[_jdx] = _line[_jdx];
		_btmp[_linelen] = 0;
		*_BrokenText = _btmp;
	}
	free(_line);
	blDeleteUtf16Str((BLUtf16*)_txt);
}
static void
_TableMake(_BLWidget* _Node)
{
	_Node->uExtension.sTable.nTotalHeight = 0;
	_Node->uExtension.sTable.nTotalWidth = 0;	
	if (_Node->uExtension.sTable.nColumnNum)
		_Node->uExtension.sTable.nTotalHeight = _Node->uExtension.sTable.nRowHeight * (BLU32)roundf((BLF32)_Node->uExtension.sTable.nCellNum / (BLF32)_Node->uExtension.sTable.nColumnNum);
	else
		_Node->uExtension.sTable.nTotalHeight = 0;
	for (BLU32 _idx = 0; _idx < _Node->uExtension.sTable.nColumnNum; ++_idx)
		_Node->uExtension.sTable.nTotalWidth += _Node->uExtension.sTable.aColumnWidth[_idx];
	if ((BLS32)_Node->uExtension.sTable.nTotalHeight >(BLS32)_Node->sDimension.fY)
		_Node->uExtension.sTable.nScroll = (BLS32)blScalarClamp((BLF32)_Node->uExtension.sTable.nScroll, 0.f, (BLF32)_Node->uExtension.sTable.nTotalHeight - _Node->sDimension.fY);
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
			if (!strcmp(_tag, _src->uExtension.sCheck.aCheckedMap))
			{
				_src->uExtension.sCheck.sCheckedTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sCheck.sCheckedTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sCheck.sCheckedTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sCheck.sCheckedTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sCheck.sCheckedTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sCheckedTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sCheck.sCheckedTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sCheck.sCheckedTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
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
			if (!strcmp(_tag, _src->uExtension.sSlider.aDisableMap))
			{
				_src->uExtension.sSlider.sDisableTex.sLT.fX = (BLF32)_ltx;
				_src->uExtension.sSlider.sDisableTex.sLT.fY = (BLF32)_lty;
				_src->uExtension.sSlider.sDisableTex.sRB.fX = (BLF32)_rbx;
				_src->uExtension.sSlider.sDisableTex.sRB.fY = (BLF32)_rby;
				_src->uExtension.sSlider.sDisableTex9.sLT.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sSlider.sDisableTex9.sLT.fY += (BLF32)(_lty + _rby) * 0.5f;
				_src->uExtension.sSlider.sDisableTex9.sRB.fX += (BLF32)(_ltx + _rbx) * 0.5f;
				_src->uExtension.sSlider.sDisableTex9.sRB.fY += (BLF32)(_lty + _rby) * 0.5f;
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
		{
			_src->uExtension.sCheck.eTexFormat = _format;
			_src->uExtension.sCheck.pTexData = _texdata;
			_src->uExtension.sCheck.nTexWidth = _width;
			_src->uExtension.sCheck.nTexHeight = _height;
			FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
			{
				if (_iter->nHashName == blHashUtf8((const BLUtf8*)(_src->uExtension.sCheck.aFontSource)))
				{
					_loadfont = FALSE;
					break;
				}
			}
			sprintf(_texfilettf, "%s/font/%s.ttf", _PrUIMem->aDir, _src->uExtension.sCheck.aFontSource);
			sprintf(_texfilettc, "%s/font/%s.ttc", _PrUIMem->aDir, _src->uExtension.sCheck.aFontSource);
		}
		break;
	case BL_UT_SLIDER:
		{
			_src->uExtension.sSlider.eTexFormat = _format;
			_src->uExtension.sSlider.pTexData = _texdata;
			_src->uExtension.sSlider.nTexWidth = _width;
			_src->uExtension.sSlider.nTexHeight = _height;
			_loadfont = FALSE;
		}
		break;
	case BL_UT_TEXT:
		{
			_src->uExtension.sText.eTexFormat = _format;
			_src->uExtension.sText.pTexData = _texdata;
			_src->uExtension.sText.nTexWidth = _width;
			_src->uExtension.sText.nTexHeight = _height;
			FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
			{
				if (_iter->nHashName == blHashUtf8((const BLUtf8*)(_src->uExtension.sText.aFontSource)))
				{
					_loadfont = FALSE;
					break;
				}
			}
			sprintf(_texfilettf, "%s/font/%s.ttf", _PrUIMem->aDir, _src->uExtension.sText.aFontSource);
			sprintf(_texfilettc, "%s/font/%s.ttc", _PrUIMem->aDir, _src->uExtension.sText.aFontSource);
		}
		break;
	case BL_UT_PROGRESS:
		{
			_src->uExtension.sProgress.eTexFormat = _format;
			_src->uExtension.sProgress.pTexData = _texdata;
			_src->uExtension.sProgress.nTexWidth = _width;
			_src->uExtension.sProgress.nTexHeight = _height;
			FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
			{
				if (_iter->nHashName == blHashUtf8((const BLUtf8*)(_src->uExtension.sProgress.aFontSource)))
				{
					_loadfont = FALSE;
					break;
				}
			}
			sprintf(_texfilettf, "%s/font/%s.ttf", _PrUIMem->aDir, _src->uExtension.sProgress.aFontSource);
			sprintf(_texfilettc, "%s/font/%s.ttc", _PrUIMem->aDir, _src->uExtension.sProgress.aFontSource);
		}
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
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)(_src->uExtension.sTable.aFontSource)))
			{
				_loadfont = FALSE;
				break;
			}
		}
		sprintf(_texfilettf, "%s/font/%s.ttf", _PrUIMem->aDir, _src->uExtension.sTable.aFontSource);
		sprintf(_texfilettc, "%s/font/%s.ttc", _PrUIMem->aDir, _src->uExtension.sTable.aFontSource);
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
		for (BLU32 _idx = 0; _idx < _widget->uExtension.sText.nSplitSize; ++_idx)
			free(_widget->uExtension.sText.pSplitText[_idx]);
	}
	break;
	case BL_UT_TABLE:
	{
		for (BLU32 _idx = 0; _idx < _widget->uExtension.sTable.nCellNum; ++_idx)
		{
			if (_widget->uExtension.sTable.pCellText[_idx])
				free(_widget->uExtension.sTable.pCellText[_idx]);
			if (_widget->uExtension.sTable.pCellBroken[_idx])
				free(_widget->uExtension.sTable.pCellBroken[_idx]);
		}
		if (_widget->uExtension.sTable.pCellColor)
			free(_widget->uExtension.sTable.pCellColor);
		if (_widget->uExtension.sTable.pCellText)
			free(_widget->uExtension.sTable.pCellText);
		if (_widget->uExtension.sTable.pCellBroken)
			free(_widget->uExtension.sTable.pCellBroken);
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
static BLVoid
_WriteText(const BLUtf16* _Text, const BLAnsi* _Font, BLU32 _FontHeight, BLEnum _AlignmentH, BLEnum _AlignmentV, BLBool _Multiline, BLRect* _Area, BLRect* _ClipArea, BLU32 _Color, BLU16 _Flag, BLBool _Pwd)
{
	BLU32 _txtlen = blUtf16Length(_Text);
	if (!_txtlen)
		return;
	_BLFont* _ft = NULL;
	BLUtf16* _str = (BLUtf16*)alloca((_txtlen + 1) * sizeof(BLUtf16));
	memset(_str, 0, (_txtlen + 1) * sizeof(BLUtf16));
	if (_Pwd)
	{
		for (BLU32 _idx = 0; _idx < _txtlen; ++_idx)
			_str[_idx] = L'*';
	}
	else
	{
		for (BLU32 _idx = 0; _idx < _txtlen; ++_idx)
			_str[_idx] = _Text[_idx];
	}
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
	BLF32 _w, _h;
	_MeasureTextDim(_str, _ft, _FontHeight, &_w, &_h, _Flag);
	BLVec2 _pt;
	_pt.fX = _Area->sLT.fX;
	_pt.fY = _Area->sLT.fY;
	if (_AlignmentV == BL_UA_VCENTER)
		_pt.fY = (_Area->sLT.fY + _Area->sRB.fY) * 0.5f - _h * 0.5f;
	else if (_AlignmentV == BL_UA_BOTTOM)
		_pt.fY = _Area->sRB.fY - _h;
	else
		_pt.fY = _Area->sLT.fY;
	if (!_Multiline)
	{
		if (_AlignmentH == BL_UA_HCENTER)
			_pt.fX = (_Area->sLT.fX + _Area->sRB.fX) * 0.5f - _w * 0.5f;
		else if (_AlignmentH == BL_UA_RIGHT)
			_pt.fX = _Area->sRB.fX - _w;
		else
			_pt.fX = _Area->sLT.fX;
		for (BLU32 _idx = 0; _idx < _txtlen; ++_idx)
		{
			BLUtf16 _c = _str[_idx];
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
			blGeometryBufferUpdate(_PrUIMem->nGlyphGeo, 0, (BLU8*)_vbo, sizeof(_vbo), 0, NULL, 0);
			blDraw(_PrUIMem->nUITech, _PrUIMem->nGlyphGeo, 1);
		}
	}
	else
	{
		BLU32 _ls = 0, _le = 0;
		BLUtf16* _line = (BLUtf16*)alloca((_txtlen + 1) * sizeof(BLUtf16));
		while (_le < _txtlen)
		{
			memset(_line, 0, (_txtlen + 1)* sizeof(BLUtf16));
			_le = _txtlen;
			for (BLU32 _idx = _ls; _idx < _txtlen; ++_idx)
			{
				if (_str[_idx] == L'\n')
				{
					_le = _idx;
					break;
				}
			}
			for (BLU32 _idx = 0; _idx < _le - _ls; ++_idx)
				_line[_idx] = _str[_idx + _ls];
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
				blGeometryBufferUpdate(_PrUIMem->nGlyphGeo, 0, (BLU8*)_vbo, sizeof(_vbo), 0, NULL, 0);
				blDraw(_PrUIMem->nUITech, _PrUIMem->nGlyphGeo, 1);
			}
			_ls = _le + 1;
			_pt.fY += _FontHeight;
		}
	}
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
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
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
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
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
	const BLUtf16* _str16 = blGenUtf16Str(_Node->uExtension.sButton.pText);
	BLU32 _txtcolor = (_Node->uExtension.sButton.nState == 0) ? _PrUIMem->nTextDisableColor : _Node->uExtension.sButton.nTxtColor;
	_WriteText(_str16, _Node->uExtension.sButton.aFontSource, _Node->uExtension.sButton.nFontHeight, _Node->uExtension.sButton.eTxtAlignmentH, _Node->uExtension.sButton.eTxtAlignmentV, FALSE, &_area, &_area, _txtcolor, _flag, FALSE);
	blDeleteUtf16Str((BLUtf16*)_str16);
}
static BLVoid
_DrawCheck(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sCheck.nPixmapTex, 0);
	BLF32 _gray = 1.f;
	BLF32 _offsetx = 0.f, _offsety = 0.f;
	BLF32 _stencil = 1.f;
	BLRect _texcoord, _texcoord9;
	BLRect _texcoords, _texcoords9;
	BLBool _flipx = _Node->uExtension.sCheck.bFlipX;
	BLBool _flipy = _Node->uExtension.sCheck.bFlipY;
	switch (_Node->uExtension.sCheck.nState)
	{
	case 0:
		if (_Node->uExtension.sCheck.aDisableMap[0] == 0 || !strcmp(_Node->uExtension.sCheck.aDisableMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sCheck.sCommonTex;
			_texcoord9 = _Node->uExtension.sCheck.sCommonTex9;
			_gray = -1.f;
		}
		else
		{
			_texcoord = _Node->uExtension.sCheck.sDisableTex;
			_texcoord9 = _Node->uExtension.sCheck.sDisableTex9;
			_gray = 1.f;
		}
		break;
	case 1:
	{
		_texcoord = _Node->uExtension.sCheck.sCommonTex;
		_texcoord9 = _Node->uExtension.sCheck.sCommonTex9;
	}
	break;
	case 2:
		if (_Node->uExtension.sCheck.aCheckedMap[0] == 0 || !strcmp(_Node->uExtension.sCheck.aCheckedMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sCheck.sCommonTex;
			_texcoord9 = _Node->uExtension.sCheck.sCommonTex9;
		}
		else
		{
			_texcoord = _Node->uExtension.sCheck.sCheckedTex;
			_texcoord9 = _Node->uExtension.sCheck.sCheckedTex9;
		}
		break;
	default:
		_texcoord = _Node->uExtension.sCheck.sCommonTex;
		_texcoord9 = _Node->uExtension.sCheck.sCommonTex9;
		break;
	}
	if (_Node->uExtension.sCheck.aStencilMap[0] && strcmp(_Node->uExtension.sCheck.aStencilMap, "Nil"))
	{
		_stencil = -1.f;
		_texcoords.sLT.fX = _Node->uExtension.sCheck.sStencilTex.sLT.fX / _Node->uExtension.sCheck.nTexWidth + 0.001f;
		_texcoords.sLT.fY = _Node->uExtension.sCheck.sStencilTex.sLT.fY / _Node->uExtension.sCheck.nTexHeight + 0.001f;
		_texcoords.sRB.fX = _Node->uExtension.sCheck.sStencilTex.sRB.fX / _Node->uExtension.sCheck.nTexWidth - 0.001f;
		_texcoords.sRB.fY = _Node->uExtension.sCheck.sStencilTex.sRB.fY / _Node->uExtension.sCheck.nTexHeight - 0.001f;
		BLF32 _oriw = _Node->uExtension.sCheck.sCommonTex.sRB.fX - _Node->uExtension.sCheck.sCommonTex.sLT.fX;
		BLF32 _orih = _Node->uExtension.sCheck.sCommonTex.sRB.fY - _Node->uExtension.sCheck.sCommonTex.sLT.fY;
		BLF32 _dstw = _Node->uExtension.sCheck.sStencilTex.sRB.fX - _Node->uExtension.sCheck.sStencilTex.sLT.fX;
		BLF32 _dsth = _Node->uExtension.sCheck.sStencilTex.sRB.fX - _Node->uExtension.sCheck.sStencilTex.sLT.fX;
		BLF32 _ratioax = (_Node->uExtension.sCheck.sCommonTex9.sLT.fX - _Node->uExtension.sCheck.sCommonTex.sLT.fX) / _oriw;
		BLF32 _ratioay = (_Node->uExtension.sCheck.sCommonTex9.sLT.fY - _Node->uExtension.sCheck.sCommonTex.sLT.fY) / _orih;
		BLF32 _rationx = (_Node->uExtension.sCheck.sCommonTex.sRB.fX - _Node->uExtension.sCheck.sCommonTex9.sRB.fX) / _oriw;
		BLF32 _rationy = (_Node->uExtension.sCheck.sCommonTex.sRB.fY - _Node->uExtension.sCheck.sCommonTex9.sRB.fY) / _orih;
		_texcoords9.sLT.fX = (_Node->uExtension.sCheck.sStencilTex.sLT.fX + _ratioax * _dstw) / _Node->uExtension.sCheck.nTexWidth;
		_texcoords9.sLT.fY = (_Node->uExtension.sCheck.sStencilTex.sLT.fY + _ratioay * _dsth) / _Node->uExtension.sCheck.nTexHeight;
		_texcoords9.sRB.fX = (_Node->uExtension.sCheck.sStencilTex.sRB.fX - _rationx * _dstw) / _Node->uExtension.sCheck.nTexWidth;
		_texcoords9.sRB.fY = (_Node->uExtension.sCheck.sStencilTex.sRB.fY - _rationy * _dsth) / _Node->uExtension.sCheck.nTexHeight;
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
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
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
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sCheck.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sCheck.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	BLRect _area;
	_area.sLT.fX = _X - 0.5f * _Width;
	_area.sLT.fY = _Y - 0.5f * _Height;
	_area.sRB.fX = _X + 0.5f * _Width;
	_area.sRB.fY = _Y + 0.5f * _Height;
	BLU16 _flag = 0;
	if (_Node->uExtension.sCheck.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sCheck.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sCheck.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sCheck.bItalics)
		_flag |= 0xF000;
	const BLUtf16* _str16 = blGenUtf16Str(_Node->uExtension.sButton.pText);
	BLU32 _txtcolor = (_Node->uExtension.sCheck.nState == 0) ? _PrUIMem->nTextDisableColor : _Node->uExtension.sCheck.nTxtColor;
	_WriteText(_str16, _Node->uExtension.sCheck.aFontSource, _Node->uExtension.sCheck.nFontHeight, _Node->uExtension.sCheck.eTxtAlignmentH, _Node->uExtension.sCheck.eTxtAlignmentV, FALSE, &_area, &_area, _txtcolor, _flag, FALSE);
	blDeleteUtf16Str((BLUtf16*)_str16);
}
static BLVoid
_DrawSlider(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sSlider.nPixmapTex, 0);
	BLF32 _gray = 1.f;
	BLF32 _offsetx = 0.f, _offsety = 0.f;
	BLF32 _stencil = 1.f;
	BLRect _texcoord, _texcoord9;
	BLRect _texcoords, _texcoords9;
	BLBool _flipx = _Node->uExtension.sSlider.bFlipX;
	BLBool _flipy = _Node->uExtension.sSlider.bFlipY;
	if (_Node->uExtension.sSlider.nState)
	{
		_texcoord = _Node->uExtension.sSlider.sCommonTex;
		_texcoord9 = _Node->uExtension.sSlider.sCommonTex9;
	}
	else
	{
		if (_Node->uExtension.sSlider.aDisableMap[0] == 0 || !strcmp(_Node->uExtension.sSlider.aDisableMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sSlider.sCommonTex;
			_texcoord9 = _Node->uExtension.sSlider.sCommonTex9;
			_gray = -1.f;
		}
		else
		{
			_texcoord = _Node->uExtension.sSlider.sDisableTex;
			_texcoord9 = _Node->uExtension.sSlider.sDisableTex9;
			_gray = 1.f;
		}
	}
	if (_Node->uExtension.sSlider.aStencilMap[0] && strcmp(_Node->uExtension.sSlider.aStencilMap, "Nil"))
	{
		_stencil = -1.f;
		_texcoords.sLT.fX = _Node->uExtension.sSlider.sStencilTex.sLT.fX / _Node->uExtension.sSlider.nTexWidth + 0.001f;
		_texcoords.sLT.fY = _Node->uExtension.sSlider.sStencilTex.sLT.fY / _Node->uExtension.sSlider.nTexHeight + 0.001f;
		_texcoords.sRB.fX = _Node->uExtension.sSlider.sStencilTex.sRB.fX / _Node->uExtension.sSlider.nTexWidth - 0.001f;
		_texcoords.sRB.fY = _Node->uExtension.sSlider.sStencilTex.sRB.fY / _Node->uExtension.sSlider.nTexHeight - 0.001f;
		BLF32 _oriw = _Node->uExtension.sSlider.sCommonTex.sRB.fX - _Node->uExtension.sSlider.sCommonTex.sLT.fX;
		BLF32 _orih = _Node->uExtension.sSlider.sCommonTex.sRB.fY - _Node->uExtension.sSlider.sCommonTex.sLT.fY;
		BLF32 _dstw = _Node->uExtension.sSlider.sStencilTex.sRB.fX - _Node->uExtension.sSlider.sStencilTex.sLT.fX;
		BLF32 _dsth = _Node->uExtension.sSlider.sStencilTex.sRB.fX - _Node->uExtension.sSlider.sStencilTex.sLT.fX;
		BLF32 _ratioax = (_Node->uExtension.sSlider.sCommonTex9.sLT.fX - _Node->uExtension.sSlider.sCommonTex.sLT.fX) / _oriw;
		BLF32 _ratioay = (_Node->uExtension.sSlider.sCommonTex9.sLT.fY - _Node->uExtension.sSlider.sCommonTex.sLT.fY) / _orih;
		BLF32 _rationx = (_Node->uExtension.sSlider.sCommonTex.sRB.fX - _Node->uExtension.sSlider.sCommonTex9.sRB.fX) / _oriw;
		BLF32 _rationy = (_Node->uExtension.sSlider.sCommonTex.sRB.fY - _Node->uExtension.sSlider.sCommonTex9.sRB.fY) / _orih;
		_texcoords9.sLT.fX = (_Node->uExtension.sSlider.sStencilTex.sLT.fX + _ratioax * _dstw) / _Node->uExtension.sSlider.nTexWidth;
		_texcoords9.sLT.fY = (_Node->uExtension.sSlider.sStencilTex.sLT.fY + _ratioay * _dsth) / _Node->uExtension.sSlider.nTexHeight;
		_texcoords9.sRB.fX = (_Node->uExtension.sSlider.sStencilTex.sRB.fX - _rationx * _dstw) / _Node->uExtension.sSlider.nTexWidth;
		_texcoords9.sRB.fY = (_Node->uExtension.sSlider.sStencilTex.sRB.fY - _rationy * _dsth) / _Node->uExtension.sSlider.nTexHeight;
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
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
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
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	if (_Node->uExtension.sSlider.nState)
	{
		_texcoord = _Node->uExtension.sSlider.sSliderCommonTex;
		_gray = 1.f;
	}
	else
	{
		if (_Node->uExtension.sSlider.aSliderDisableMap[0] == 0 || !strcmp(_Node->uExtension.sSlider.aSliderDisableMap, "Nil"))
		{
			_texcoord = _Node->uExtension.sSlider.sSliderCommonTex;
			_gray = -1.f;
		}
		else
		{
			_texcoord = _Node->uExtension.sSlider.sSliderDisableTex;
			_gray = 1.f;
		}
	}
	BLF32 _perc = (BLF32)_Node->uExtension.sSlider.nSliderPosition / (BLF32)(_Node->uExtension.sSlider.nMaxValue - _Node->uExtension.sSlider.nMinValue);
	BLF32 _sx, _sy;
	if (_Node->uExtension.sSlider.bHorizontal)
	{
		_sx = _X - _Width * 0.5f + _Node->uExtension.sSlider.sSliderSize.fX * 0.5f + (_Width - _Node->uExtension.sSlider.sSliderSize.fX) * _perc;
		_sy = _Y;
	}
	else
	{
		_sx = _X;
		_sy = _Y + _Height * 0.5f - _Node->uExtension.sSlider.sSliderSize.fY * 0.5f - (_Height - _Node->uExtension.sSlider.sSliderSize.fY) * _perc;
	}
	BLF32 _vbo[] = {
		PIXEL_ALIGNED_INTERNAL(_sx - _Node->uExtension.sSlider.sSliderSize.fX * 0.5f),
		PIXEL_ALIGNED_INTERNAL(_sy - _Node->uExtension.sSlider.sSliderSize.fY * 0.5f),
		1.f,
		1.f,
		1.f,
		1.f * _gray,
		_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
		_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
		PIXEL_ALIGNED_INTERNAL(_sx + _Node->uExtension.sSlider.sSliderSize.fX * 0.5f),
		PIXEL_ALIGNED_INTERNAL(_sy - _Node->uExtension.sSlider.sSliderSize.fY * 0.5f),
		1.f,
		1.f,
		1.f,
		1.f * _gray,
		_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
		_texcoord.sLT.fY / _Node->uExtension.sSlider.nTexHeight,
		PIXEL_ALIGNED_INTERNAL(_sx - _Node->uExtension.sSlider.sSliderSize.fX * 0.5f),
		PIXEL_ALIGNED_INTERNAL(_sy + _Node->uExtension.sSlider.sSliderSize.fY * 0.5f),
		1.f,
		1.f,
		1.f,
		1.f * _gray,
		_texcoord.sLT.fX / _Node->uExtension.sSlider.nTexWidth,
		_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight,
		PIXEL_ALIGNED_INTERNAL(_sx + _Node->uExtension.sSlider.sSliderSize.fX * 0.5f),
		PIXEL_ALIGNED_INTERNAL(_sy + _Node->uExtension.sSlider.sSliderSize.fY *0.5f),
		1.f,
		1.f,
		1.f,
		1.f * _gray,
		_texcoord.sRB.fX / _Node->uExtension.sSlider.nTexWidth,
		_texcoord.sRB.fY / _Node->uExtension.sSlider.nTexHeight
	};
	BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
	blDraw(_PrUIMem->nUITech, _geo, 1);
	blDeleteGeometryBuffer(_geo);
}
static BLVoid
_DrawText(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	BLF32 _gray = 1.f;
	BLF32 _offsetx = 0.f, _offsety = 0.f;
	BLF32 _stencil = 1.f;
	BLRect _texcoord, _texcoord9;
	BLRect _texcoords, _texcoords9;
	BLBool _flipx = _Node->uExtension.sText.bFlipX;
	BLBool _flipy = _Node->uExtension.sText.bFlipY;
	_texcoord = _Node->uExtension.sText.sCommonTex;
	_texcoord9 = _Node->uExtension.sText.sCommonTex9;
	if (_Node->uExtension.sText.aStencilMap[0] && strcmp(_Node->uExtension.sText.aStencilMap, "Nil"))
	{
		_stencil = -1.f;
		_texcoords.sLT.fX = _Node->uExtension.sText.sStencilTex.sLT.fX / _Node->uExtension.sText.nTexWidth + 0.001f;
		_texcoords.sLT.fY = _Node->uExtension.sText.sStencilTex.sLT.fY / _Node->uExtension.sText.nTexHeight + 0.001f;
		_texcoords.sRB.fX = _Node->uExtension.sText.sStencilTex.sRB.fX / _Node->uExtension.sText.nTexWidth - 0.001f;
		_texcoords.sRB.fY = _Node->uExtension.sText.sStencilTex.sRB.fY / _Node->uExtension.sText.nTexHeight - 0.001f;
		BLF32 _oriw = _Node->uExtension.sText.sCommonTex.sRB.fX - _Node->uExtension.sText.sCommonTex.sLT.fX;
		BLF32 _orih = _Node->uExtension.sText.sCommonTex.sRB.fY - _Node->uExtension.sText.sCommonTex.sLT.fY;
		BLF32 _dstw = _Node->uExtension.sText.sStencilTex.sRB.fX - _Node->uExtension.sText.sStencilTex.sLT.fX;
		BLF32 _dsth = _Node->uExtension.sText.sStencilTex.sRB.fX - _Node->uExtension.sText.sStencilTex.sLT.fX;
		BLF32 _ratioax = (_Node->uExtension.sText.sCommonTex9.sLT.fX - _Node->uExtension.sText.sCommonTex.sLT.fX) / _oriw;
		BLF32 _ratioay = (_Node->uExtension.sText.sCommonTex9.sLT.fY - _Node->uExtension.sText.sCommonTex.sLT.fY) / _orih;
		BLF32 _rationx = (_Node->uExtension.sText.sCommonTex.sRB.fX - _Node->uExtension.sText.sCommonTex9.sRB.fX) / _oriw;
		BLF32 _rationy = (_Node->uExtension.sText.sCommonTex.sRB.fY - _Node->uExtension.sText.sCommonTex9.sRB.fY) / _orih;
		_texcoords9.sLT.fX = (_Node->uExtension.sText.sStencilTex.sLT.fX + _ratioax * _dstw) / _Node->uExtension.sText.nTexWidth;
		_texcoords9.sLT.fY = (_Node->uExtension.sText.sStencilTex.sLT.fY + _ratioay * _dsth) / _Node->uExtension.sText.nTexHeight;
		_texcoords9.sRB.fX = (_Node->uExtension.sText.sStencilTex.sRB.fX - _rationx * _dstw) / _Node->uExtension.sText.nTexWidth;
		_texcoords9.sRB.fY = (_Node->uExtension.sText.sStencilTex.sRB.fY - _rationy * _dsth) / _Node->uExtension.sText.nTexHeight;
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
		blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sText.nPixmapTex, 0);
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
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	else
	{
		blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sText.nPixmapTex, 0);
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
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sText.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sText.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_Node->uExtension.sText.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return;
	BLU16 _flag = 0;
	if (_Node->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sText.bItalics)
		_flag |= 0xF000;
	const BLUtf16* _text = blGenUtf16Str(_Node->uExtension.sText.pText);
	BLRect _area;
	_area.sLT.fX = _X - 0.5f * _Width;
	_area.sLT.fY = _Y - 0.5f * _Height;
	_area.sRB.fX = _X + 0.5f * _Width;
	_area.sRB.fY = _Y + 0.5f * _Height;
	BLBool _focus = _PrUIMem->pFocusWidget == _Node;
	BLRect _framerect;
	_framerect.sLT.fX = _X - _Width * 0.5f + _Node->uExtension.sText.nPaddingX;
	_framerect.sLT.fY = _Y - _Height * 0.5f + _Node->uExtension.sText.nPaddingY;
	_framerect.sRB.fX = _X + _Width * 0.5f - _Node->uExtension.sText.nPaddingX;
	_framerect.sRB.fY = _Y + _Height * 0.5f - _Node->uExtension.sText.nPaddingY;
	if (!blUtf16Length(_text) && _PrUIMem->pFocusWidget != _Node)
	{
		_text = blGenUtf16Str(_Node->uExtension.sText.pPlaceholder);
		_WriteText(_text, _Node->uExtension.sText.aFontSource, _Node->uExtension.sText.nFontHeight, _Node->uExtension.sText.eTxtAlignmentH, _Node->uExtension.sText.eTxtAlignmentV, TRUE, &_framerect, &_area, _Node->uExtension.sText.nPlaceholderColor, _flag, FALSE);
		blDeleteUtf16Str((BLUtf16*)_text);
		return;
	}
	BLS32 _startpos = 0;
	BLUtf16* _curline = (BLUtf16*)_text;
	BLUtf16* _s1;
	BLUtf16* _s2;
	BLBool _ml = (!_Node->uExtension.sText.bPassword && _Node->uExtension.sText.bMultiline);
	BLS32 _realmbgn = _Node->uExtension.sText.nSelectBegin < _Node->uExtension.sText.nSelectEnd ? _Node->uExtension.sText.nSelectBegin : _Node->uExtension.sText.nSelectEnd;
	BLS32 _realmend = _Node->uExtension.sText.nSelectBegin < _Node->uExtension.sText.nSelectEnd ? _Node->uExtension.sText.nSelectEnd : _Node->uExtension.sText.nSelectBegin;
	BLS32 _hlinestart = _ml ? _TextLine(_Node, _realmbgn) : 0;
	BLS32 _hlinecount = _ml ? _TextLine(_Node, _realmend) - _hlinestart + 1 : 1;
	BLS32 _linecount = _ml ? _Node->uExtension.sText.nSplitSize : 1;
	for (BLS32 _idx = 0; _idx < _linecount; ++_idx)
	{
		_TextRect(_Node, _ft, _idx, &_framerect, _flag);
		_curline = _ml ? _Node->uExtension.sText.pSplitText[_idx] : _text;
		_startpos = _ml ? _Node->uExtension.sText.aSplitPositions[_idx] : 0;			
		BLU32 _txtcolor = (_Node->uExtension.sText.nState == 0) ? _PrUIMem->nTextDisableColor : _Node->uExtension.sText.nTxtColor;
		_WriteText(_curline, _Node->uExtension.sText.aFontSource, _Node->uExtension.sText.nFontHeight, _Node->uExtension.sText.eTxtAlignmentH, _Node->uExtension.sText.eTxtAlignmentV, TRUE, &_Node->uExtension.sText.sCurRect, &_area, _txtcolor, _flag, _Node->uExtension.sText.bPassword);
		if (_focus && _Node->uExtension.sText.nSelectBegin != _Node->uExtension.sText.nSelectEnd && _idx >= _hlinestart && _idx < _hlinestart + _hlinecount)
		{
			BLF32 _mbegin = 0.f, _mend = 0.f, _outy;
			BLU32 _curlen = (BLU32)blUtf16Length(_curline);
			BLS32 _linestartpos = 0, _lineendpos = _curlen;
			if (_idx == _hlinestart)
			{
				if (_Node->uExtension.sText.bPassword)
				{
					BLUtf16* _starph = (BLUtf16*)alloca((_realmbgn - _startpos + 1) * sizeof(BLUtf16));
					for (BLS32 _jdx = 0; _jdx < _realmbgn - _startpos; ++_jdx)
						_starph[_jdx] = L'*';
					_starph[_realmbgn - _startpos] = 0;
					_MeasureTextDim((const BLUtf16*)_starph, _ft, _Node->uExtension.sText.nFontHeight, &_mbegin, &_outy, _flag);
				}
				else
				{
					_s1 = (BLUtf16*)alloca((_realmbgn - _startpos + 1) * sizeof(BLUtf16));
					for (BLS32 _jdx = 0; _jdx < _realmbgn - _startpos; ++_jdx)
						_s1[_jdx] = _curline[_jdx];
					_s1[_realmbgn - _startpos] = 0;
					_MeasureTextDim((const BLUtf16*)_s1, _ft, _Node->uExtension.sText.nFontHeight, &_mbegin, &_outy, _flag);
				}
				_linestartpos = _realmbgn - _startpos;
			}
			if (_idx == _hlinestart + _hlinecount - 1)
			{
				if (_Node->uExtension.sText.bPassword)
				{
					BLUtf16* _starph = (BLUtf16*)alloca((_realmend - _startpos + 1) * sizeof(BLUtf16));
					for (BLS32 _jdx = 0; _jdx < _realmend - _startpos; ++_jdx)
						_starph[_jdx] = L'*';
					_starph[_realmend - _startpos] = 0;
					_MeasureTextDim((const BLUtf16*)_starph, _ft, _Node->uExtension.sText.nFontHeight, &_mend, &_outy, _flag);
				}
				else
				{
					_s2 = (BLUtf16*)alloca((_realmend - _startpos + 1) * sizeof(BLUtf16));
					for (BLS32 _jdx = 0; _jdx < _realmend - _startpos; ++_jdx)
						_s2[_jdx] = _curline[_jdx];
					_s2[_realmend - _startpos] = 0;
					_MeasureTextDim((const BLUtf16*)_s2, _ft, _Node->uExtension.sText.nFontHeight, &_mend, &_outy, _flag);
				}
				_lineendpos = _realmend - _startpos;
			}
			else
			{
				if (_Node->uExtension.sText.bPassword)
				{
					BLUtf16* _starph = (BLUtf16*)alloca((_curlen + 1) * sizeof(BLUtf16));
					for (BLS32 _jdx = 0; _jdx < (BLS32)_curlen; ++_jdx)
						_starph[_jdx] = L'*';
					_starph[_curlen] = 0;
					_MeasureTextDim((const BLUtf16*)_starph, _ft, _Node->uExtension.sText.nFontHeight, &_mend, &_outy, _flag);
				}
				else
					_MeasureTextDim((const BLUtf16*)_curline, _ft, _Node->uExtension.sText.nFontHeight, &_mend, &_outy, _flag);
			}
			_Node->uExtension.sText.sCurRect.sLT.fX += _mbegin;
			_Node->uExtension.sText.sCurRect.sRB.fX = _Node->uExtension.sText.sCurRect.sLT.fX + _mend - _mbegin;
			BLF32 _selrgba[4];
			blDeColor4F(_PrUIMem->nSelectRangeColor, _selrgba);
			blTechSampler(_PrUIMem->nUITech, "Texture0", _PrUIMem->nBlankTex, 0);
			BLF32 _vbo[] = {
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sLT.fX),
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sLT.fY),
				_selrgba[0],
				_selrgba[1],
				_selrgba[2],
				_selrgba[3],
				0.f,
				0.f,
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sRB.fX),
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sLT.fY),
				_selrgba[0],
				_selrgba[1],
				_selrgba[2],
				_selrgba[3],
				1.f,
				0.f,
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sLT.fX),
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sRB.fY),
				_selrgba[0],
				_selrgba[1],
				_selrgba[2],
				_selrgba[3],
				0.f,
				1.f,
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sRB.fX),
				PIXEL_ALIGNED_INTERNAL(_Node->uExtension.sText.sCurRect.sRB.fY),
				_selrgba[0],
				_selrgba[1],
				_selrgba[2],
				_selrgba[3],
				1.f,
				1.f
			};
			BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
			blDraw(_PrUIMem->nUITech, _geo, 1);
			blDeleteGeometryBuffer(_geo);
		}
	}
	blDeleteUtf16Str((BLUtf16*)_text);
}
static BLVoid
_DrawProgress(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sProgress.nPixmapTex, 0);
	BLF32 _gray = 1.f;
	BLF32 _offsetx = 0.f, _offsety = 0.f;
	BLF32 _stencil = 1.f;
	BLRect _texcoord, _texcoord9;
	BLRect _texcoords, _texcoords9;
	BLBool _flipx = _Node->uExtension.sProgress.bFlipX;
	BLBool _flipy = _Node->uExtension.sProgress.bFlipY;
	_texcoord = _Node->uExtension.sProgress.sCommonTex;
	_texcoord9 = _Node->uExtension.sProgress.sCommonTex9;
	if (_Node->uExtension.sProgress.aStencilMap[0] && strcmp(_Node->uExtension.sProgress.aStencilMap, "Nil"))
	{
		_stencil = -1.f;
		_texcoords.sLT.fX = _Node->uExtension.sProgress.sStencilTex.sLT.fX / _Node->uExtension.sProgress.nTexWidth + 0.001f;
		_texcoords.sLT.fY = _Node->uExtension.sProgress.sStencilTex.sLT.fY / _Node->uExtension.sProgress.nTexHeight + 0.001f;
		_texcoords.sRB.fX = _Node->uExtension.sProgress.sStencilTex.sRB.fX / _Node->uExtension.sProgress.nTexWidth - 0.001f;
		_texcoords.sRB.fY = _Node->uExtension.sProgress.sStencilTex.sRB.fY / _Node->uExtension.sProgress.nTexHeight - 0.001f;
		BLF32 _oriw = _Node->uExtension.sProgress.sCommonTex.sRB.fX - _Node->uExtension.sProgress.sCommonTex.sLT.fX;
		BLF32 _orih = _Node->uExtension.sProgress.sCommonTex.sRB.fY - _Node->uExtension.sProgress.sCommonTex.sLT.fY;
		BLF32 _dstw = _Node->uExtension.sProgress.sStencilTex.sRB.fX - _Node->uExtension.sProgress.sStencilTex.sLT.fX;
		BLF32 _dsth = _Node->uExtension.sProgress.sStencilTex.sRB.fX - _Node->uExtension.sProgress.sStencilTex.sLT.fX;
		BLF32 _ratioax = (_Node->uExtension.sProgress.sCommonTex9.sLT.fX - _Node->uExtension.sProgress.sCommonTex.sLT.fX) / _oriw;
		BLF32 _ratioay = (_Node->uExtension.sProgress.sCommonTex9.sLT.fY - _Node->uExtension.sProgress.sCommonTex.sLT.fY) / _orih;
		BLF32 _rationx = (_Node->uExtension.sProgress.sCommonTex.sRB.fX - _Node->uExtension.sProgress.sCommonTex9.sRB.fX) / _oriw;
		BLF32 _rationy = (_Node->uExtension.sProgress.sCommonTex.sRB.fY - _Node->uExtension.sProgress.sCommonTex9.sRB.fY) / _orih;
		_texcoords9.sLT.fX = (_Node->uExtension.sProgress.sStencilTex.sLT.fX + _ratioax * _dstw) / _Node->uExtension.sProgress.nTexWidth;
		_texcoords9.sLT.fY = (_Node->uExtension.sProgress.sStencilTex.sLT.fY + _ratioay * _dsth) / _Node->uExtension.sProgress.nTexHeight;
		_texcoords9.sRB.fX = (_Node->uExtension.sProgress.sStencilTex.sRB.fX - _rationx * _dstw) / _Node->uExtension.sProgress.nTexWidth;
		_texcoords9.sRB.fY = (_Node->uExtension.sProgress.sStencilTex.sRB.fY - _rationy * _dsth) / _Node->uExtension.sProgress.nTexHeight;
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
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
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
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	_offsetx = (BLF32)_Node->uExtension.sProgress.nBorderX;
	_offsety = (BLF32)_Node->uExtension.sProgress.nBorderY;
	BLF32 _perc = (BLF32)_Node->uExtension.sProgress.nPercent / 100.f;
	_texcoord = _Node->uExtension.sProgress.sFillTex;
	BLF32 _fillw;
	_fillw = _texcoord.sRB.fX - _texcoord.sLT.fX;
	_texcoord.sRB.fX = _texcoord.sLT.fX + _fillw * _perc;
	BLF32 _vbo[] = {
		PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
		PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
		_texcoords.sLT.fX,
		_texcoords.sLT.fY,
		1.f * _stencil,
		1.f * _gray,
		_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
		_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
		PIXEL_ALIGNED_INTERNAL(_X + _Width * (_perc - 0.5f) - _offsetx),
		PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
		_texcoords.sRB.fX,
		_texcoords.sLT.fY,
		1.f * _stencil,
		1.f * _gray,
		_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
		_texcoord.sLT.fY / _Node->uExtension.sProgress.nTexHeight,
		PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
		PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
		_texcoords.sLT.fX,
		_texcoords.sRB.fY,
		1.f * _stencil,
		1.f * _gray,
		_texcoord.sLT.fX / _Node->uExtension.sProgress.nTexWidth,
		_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight,
		PIXEL_ALIGNED_INTERNAL(_X + _Width * (_perc - 0.5f) - _offsetx),
		PIXEL_ALIGNED_INTERNAL(_Y + _Height *0.5f - _offsety),
		_texcoords.sRB.fX,
		_texcoords.sRB.fY,
		1.f * _stencil,
		1.f * _gray,
		_texcoord.sRB.fX / _Node->uExtension.sProgress.nTexWidth,
		_texcoord.sRB.fY / _Node->uExtension.sProgress.nTexHeight
	};
	BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
	blDraw(_PrUIMem->nUITech, _geo, 1);
	blDeleteGeometryBuffer(_geo);
	BLRect _area;
	_area.sLT.fX = _X - 0.5f * _Width;
	_area.sLT.fY = _Y - 0.5f * _Height;
	_area.sRB.fX = _X + 0.5f * _Width;
	_area.sRB.fY = _Y + 0.5f * _Height;
	BLU16 _flag = 0;
	if (_Node->uExtension.sProgress.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sProgress.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sProgress.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sProgress.bItalics)
		_flag |= 0xF000;
	const BLUtf16* _str16 = blGenUtf16Str(_Node->uExtension.sButton.pText);
	BLU32 _txtcolor = _Node->uExtension.sProgress.nTxtColor;
	_WriteText(_str16, _Node->uExtension.sProgress.aFontSource, _Node->uExtension.sProgress.nFontHeight, BL_UA_HCENTER, BL_UA_VCENTER, FALSE, &_area, &_area, _txtcolor, _flag, FALSE);
	blDeleteUtf16Str((BLUtf16*)_str16);
}
static BLVoid
_DrawDial(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sDial.nPixmapTex, 0);
	BLF32 _gray = 1.f;
	BLF32 _stencil = 1.f;
	if (_Node->uExtension.sDial.bAngleCut)
	{
		BLVec2 _pts[17], _tpts[18], _tspts[18];
		BLS32 _startx, _endx;
		BLS32 _theta = (BLS32)(atan2f(_Width, _Height) * 180.0f / PI_INTERNAL);
		if (_Node->uExtension.sDial.nStartAngle == 0)
			_startx = 0;
		else if (_Node->uExtension.sDial.nStartAngle > 0 && _Node->uExtension.sDial.nStartAngle < _theta)
			_startx = 1;
		else if (_Node->uExtension.sDial.nStartAngle == _theta)
			_startx = 2;
		else if (_Node->uExtension.sDial.nStartAngle > _theta && _Node->uExtension.sDial.nStartAngle < 90)
			_startx = 3;
		else if (_Node->uExtension.sDial.nStartAngle == 90)
			_startx = 4;
		else if (_Node->uExtension.sDial.nStartAngle > 90 && _Node->uExtension.sDial.nStartAngle < 180 - _theta)
			_startx = 5;
		else if (_Node->uExtension.sDial.nStartAngle == 180 - _theta)
			_startx = 6;
		else if (_Node->uExtension.sDial.nStartAngle > 180 - _theta && _Node->uExtension.sDial.nStartAngle < 180)
			_startx = 7;
		else if (_Node->uExtension.sDial.nStartAngle == 180)
			_startx = 8;
		else if (_Node->uExtension.sDial.nStartAngle > 180 && _Node->uExtension.sDial.nStartAngle < 180 + _theta)
			_startx = 9;
		else if (_Node->uExtension.sDial.nStartAngle == 180 + _theta)
			_startx = 10;
		else if (_Node->uExtension.sDial.nStartAngle > 180 + _theta && _Node->uExtension.sDial.nStartAngle < 270)
			_startx = 11;
		else if (_Node->uExtension.sDial.nStartAngle == 270)
			_startx = 12;
		else if (_Node->uExtension.sDial.nStartAngle > 270 && _Node->uExtension.sDial.nStartAngle < 360 - _theta)
			_startx = 13;
		else if (_Node->uExtension.sDial.nStartAngle == 360 - _theta)
			_startx = 14;
		else if (_Node->uExtension.sDial.nStartAngle > 360 - _theta && _Node->uExtension.sDial.nStartAngle < 360)
			_startx = 15;
		else
			_startx = 0;
		if (_Node->uExtension.sDial.nEndAngle == 0)
			_endx = 0;
		else if (_Node->uExtension.sDial.nEndAngle > 0 && _Node->uExtension.sDial.nEndAngle < _theta)
			_endx = 1;
		else if (_Node->uExtension.sDial.nEndAngle == _theta)
			_endx = 2;
		else if (_Node->uExtension.sDial.nEndAngle > _theta && _Node->uExtension.sDial.nEndAngle < 90)
			_endx = 3;
		else if (_Node->uExtension.sDial.nEndAngle == 90)
			_endx = 4;
		else if (_Node->uExtension.sDial.nEndAngle > 90 && _Node->uExtension.sDial.nEndAngle < 180 - _theta)
			_endx = 5;
		else if (_Node->uExtension.sDial.nEndAngle == 180 - _theta)
			_endx = 6;
		else if (_Node->uExtension.sDial.nEndAngle > 180 - _theta && _Node->uExtension.sDial.nEndAngle < 180)
			_endx = 7;
		else if (_Node->uExtension.sDial.nEndAngle == 180)
			_endx = 8;
		else if (_Node->uExtension.sDial.nEndAngle > 180 && _Node->uExtension.sDial.nEndAngle < 180 + _theta)
			_endx = 9;
		else if (_Node->uExtension.sDial.nEndAngle == 180 + _theta)
			_endx = 10;
		else if (_Node->uExtension.sDial.nEndAngle > 180 + _theta && _Node->uExtension.sDial.nEndAngle < 270)
			_endx = 11;
		else if (_Node->uExtension.sDial.nEndAngle == 270)
			_endx = 12;
		else if (_Node->uExtension.sDial.nEndAngle > 270 && _Node->uExtension.sDial.nEndAngle < 360 - _theta)
			_endx = 13;
		else if (_Node->uExtension.sDial.nEndAngle == 360 - _theta)
			_endx = 14;
		else if (_Node->uExtension.sDial.nEndAngle > 360 - _theta && _Node->uExtension.sDial.nEndAngle < 360)
			_endx = 15;
		else
			_endx = 0;
		BLS32 _vbidx[16];
		memset(_vbidx, -1, sizeof(_vbidx));
		if (_Node->uExtension.sDial.bClockWise)
		{
			_vbidx[0] = _startx; _endx = (_endx > _startx) ? _endx : _endx + 16;
		}
		else
		{
			_vbidx[0] = _endx; _startx = (_startx > _endx) ? _startx : _startx + 16;
		}
		BLU32 _idx = 1;
		for (; _idx < 16; ++_idx)
		{
			if (_Node->uExtension.sDial.bClockWise)
			{
				if (_startx % 2 == 1)
					_vbidx[_idx] = _startx - 1 + _idx * 2;
				else 
					_vbidx[_idx] = _startx + _idx * 2;
				if (_vbidx[_idx] >= _endx)
				{
					_vbidx[_idx] = _endx;
					if (_vbidx[_idx] >= 16)
						_vbidx[_idx] -= 16;
					break;
				}
				else if (_vbidx[_idx] >= 16)
					_vbidx[_idx] -= 16;
			}
			else
			{
				if (_endx % 2 == 1)
					_vbidx[_idx] = _endx - 1 + _idx * 2;
				else
					_vbidx[_idx] = _endx + _idx * 2;
				if (_vbidx[_idx] >= _startx)
				{
					_vbidx[_idx] = _startx;
					if (_vbidx[_idx] >= 16)
						_vbidx[_idx] -= 16;
					break;
				}
				else if (_vbidx[_idx] >= 16)
					_vbidx[_idx] -= 16;
			}
		}
		_startx = (_startx > 16) ? _startx - 16 : _startx;
		_endx = (_endx > 16) ? _endx - 16 : _endx;
		_pts[0].fX = _X;
		_pts[0].fY = _Y - _Height * 0.5f;
		_pts[2].fX = _X + _Width * 0.5f;
		_pts[2].fY = _Y - _Height * 0.5f;
		_pts[4].fX = _X + _Width * 0.5f;
		_pts[4].fY = _Y;
		_pts[6].fX = _X + _Width * 0.5f;
		_pts[6].fY = _Y + _Height * 0.5f;
		_pts[8].fX = _X;
		_pts[8].fY = _Y + _Height * 0.5f;
		_pts[10].fX = _X - _Width * 0.5f;
		_pts[10].fY = _Y + _Height * 0.5f;
		_pts[12].fX = _X - _Width * 0.5f;
		_pts[12].fY = _Y;
		_pts[14].fX = _X - _Width * 0.5f;
		_pts[14].fY = _Y - _Height * 0.5f;
		_tpts[0].fX = (_Node->uExtension.sDial.sCommonTex.sLT.fX + _Node->uExtension.sDial.sCommonTex.sRB.fX) * 0.5f / _Node->uExtension.sDial.nTexWidth;
		_tpts[0].fY = (_Node->uExtension.sDial.sCommonTex.sLT.fY) / _Node->uExtension.sDial.nTexHeight;
		_tpts[2].fX = (_Node->uExtension.sDial.sCommonTex.sRB.fX) / _Node->uExtension.sDial.nTexWidth;
		_tpts[2].fY = (_Node->uExtension.sDial.sCommonTex.sLT.fY) / _Node->uExtension.sDial.nTexHeight;
		_tpts[4].fX = (_Node->uExtension.sDial.sCommonTex.sRB.fX) / _Node->uExtension.sDial.nTexWidth;
		_tpts[4].fY = (_Node->uExtension.sDial.sCommonTex.sLT.fY + _Node->uExtension.sDial.sCommonTex.sRB.fY) * 0.5f / _Node->uExtension.sDial.nTexHeight;
		_tpts[6].fX = (_Node->uExtension.sDial.sCommonTex.sRB.fX) / _Node->uExtension.sDial.nTexWidth;
		_tpts[6].fY = (_Node->uExtension.sDial.sCommonTex.sRB.fY) / _Node->uExtension.sDial.nTexHeight;
		_tpts[8].fX = (_Node->uExtension.sDial.sCommonTex.sLT.fX + _Node->uExtension.sDial.sCommonTex.sRB.fX) * 0.5f / _Node->uExtension.sDial.nTexWidth;
		_tpts[8].fY = (_Node->uExtension.sDial.sCommonTex.sRB.fY) / _Node->uExtension.sDial.nTexHeight;
		_tpts[10].fX = (_Node->uExtension.sDial.sCommonTex.sLT.fX) / _Node->uExtension.sDial.nTexWidth;
		_tpts[10].fY = (_Node->uExtension.sDial.sCommonTex.sRB.fY) / _Node->uExtension.sDial.nTexHeight;
		_tpts[12].fX = (_Node->uExtension.sDial.sCommonTex.sLT.fX) / _Node->uExtension.sDial.nTexWidth;
		_tpts[12].fY = (_Node->uExtension.sDial.sCommonTex.sLT.fY + _Node->uExtension.sDial.sCommonTex.sRB.fY) * 0.5f / _Node->uExtension.sDial.nTexHeight;
		_tpts[14].fX = (_Node->uExtension.sDial.sCommonTex.sLT.fX) / _Node->uExtension.sDial.nTexWidth;
		_tpts[14].fY = (_Node->uExtension.sDial.sCommonTex.sLT.fY) / _Node->uExtension.sDial.nTexHeight;
		_tpts[17].fX = (_Node->uExtension.sDial.sCommonTex.sLT.fX + _Node->uExtension.sDial.sCommonTex.sRB.fX) * 0.5f / _Node->uExtension.sDial.nTexWidth;
		_tpts[17].fY = (_Node->uExtension.sDial.sCommonTex.sLT.fY + _Node->uExtension.sDial.sCommonTex.sRB.fY) * 0.5f / _Node->uExtension.sDial.nTexHeight;
		if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
		{
			_stencil = -1.f;
			_tspts[0].fX = (_Node->uExtension.sDial.sStencilTex.sLT.fX + _Node->uExtension.sDial.sStencilTex.sRB.fX) * 0.5f / _Node->uExtension.sDial.nTexWidth;
			_tspts[0].fY = (_Node->uExtension.sDial.sStencilTex.sLT.fY) / _Node->uExtension.sDial.nTexHeight;
			_tspts[2].fX = (_Node->uExtension.sDial.sStencilTex.sRB.fX) / _Node->uExtension.sDial.nTexWidth;
			_tspts[2].fY = (_Node->uExtension.sDial.sStencilTex.sLT.fY) / _Node->uExtension.sDial.nTexHeight;
			_tspts[4].fX = (_Node->uExtension.sDial.sStencilTex.sRB.fX) / _Node->uExtension.sDial.nTexWidth;
			_tspts[4].fY = (_Node->uExtension.sDial.sStencilTex.sLT.fY + _Node->uExtension.sDial.sStencilTex.sRB.fY) * 0.5f / _Node->uExtension.sDial.nTexHeight;
			_tspts[6].fX = (_Node->uExtension.sDial.sStencilTex.sRB.fX) / _Node->uExtension.sDial.nTexWidth;
			_tspts[6].fY = (_Node->uExtension.sDial.sStencilTex.sRB.fY) / _Node->uExtension.sDial.nTexHeight;
			_tspts[8].fX = (_Node->uExtension.sDial.sStencilTex.sLT.fX + _Node->uExtension.sDial.sStencilTex.sRB.fX) * 0.5f / _Node->uExtension.sDial.nTexWidth;
			_tspts[8].fY = (_Node->uExtension.sDial.sStencilTex.sRB.fY) / _Node->uExtension.sDial.nTexHeight;
			_tspts[10].fX = (_Node->uExtension.sDial.sStencilTex.sLT.fX) / _Node->uExtension.sDial.nTexWidth;
			_tspts[10].fY = (_Node->uExtension.sDial.sStencilTex.sRB.fY) / _Node->uExtension.sDial.nTexHeight;
			_tspts[12].fX = (_Node->uExtension.sDial.sStencilTex.sLT.fX) / _Node->uExtension.sDial.nTexWidth;
			_tspts[12].fY = (_Node->uExtension.sDial.sStencilTex.sLT.fY + _Node->uExtension.sDial.sStencilTex.sRB.fY) * 0.5f / _Node->uExtension.sDial.nTexHeight;
			_tspts[14].fX = (_Node->uExtension.sDial.sStencilTex.sLT.fX) / _Node->uExtension.sDial.nTexWidth;
			_tspts[14].fY = (_Node->uExtension.sDial.sStencilTex.sLT.fY) / _Node->uExtension.sDial.nTexHeight;
			_tspts[17].fX = (_Node->uExtension.sDial.sStencilTex.sLT.fX + _Node->uExtension.sDial.sStencilTex.sRB.fX) * 0.5f / _Node->uExtension.sDial.nTexWidth;
			_tspts[17].fY = (_Node->uExtension.sDial.sStencilTex.sLT.fY + _Node->uExtension.sDial.sStencilTex.sRB.fY) * 0.5f / _Node->uExtension.sDial.nTexHeight;
		}
		else
		{
			for (BLU32 _idx = 0; _idx < 18; ++_idx)
			{
				_tspts[_idx].fX = 1.f;
				_tspts[_idx].fY = 1.f;
			}
		}
		BLU32 _startback = ((_startx == _endx) ? (!_Node->uExtension.sDial.bClockWise ? 16 : _startx) : _startx);
		BLU32 _endback = ((_startx == _endx) ? (!_Node->uExtension.sDial.bClockWise ? _endx : 16) : _endx);
		BLF32 _tx = (_Node->uExtension.sDial.sCommonTex.sLT.fX + _Node->uExtension.sDial.sCommonTex.sRB.fX) * 0.5f;
		BLF32 _ty = (_Node->uExtension.sDial.sCommonTex.sLT.fY + _Node->uExtension.sDial.sCommonTex.sRB.fY) * 0.5f;
		BLF32 _tw = (_Node->uExtension.sDial.sCommonTex.sRB.fX - _Node->uExtension.sDial.sCommonTex.sLT.fX);
		BLF32 _th = (_Node->uExtension.sDial.sCommonTex.sRB.fY - _Node->uExtension.sDial.sCommonTex.sLT.fY);
		BLF32 _stx = (_Node->uExtension.sDial.sStencilTex.sLT.fX + _Node->uExtension.sDial.sStencilTex.sRB.fX) * 0.5f;
		BLF32 _sty = (_Node->uExtension.sDial.sStencilTex.sLT.fY + _Node->uExtension.sDial.sStencilTex.sRB.fY) * 0.5f;
		BLF32 _stw = (_Node->uExtension.sDial.sStencilTex.sRB.fX - _Node->uExtension.sDial.sStencilTex.sLT.fX);
		BLF32 _sth = (_Node->uExtension.sDial.sStencilTex.sRB.fY - _Node->uExtension.sDial.sStencilTex.sLT.fY);
		if (_startx == 1)
		{
			BLF32 _theta = _Node->uExtension.sDial.nStartAngle * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X + tanf(_theta) * _Height * 0.5f;
			_pts[_startback].fY = _Y - _Height * 0.5f;
			_tpts[_startback].fX = (_tx + tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty - _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx + tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty - _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 3)
		{
			BLF32 _theta = (90.f -_Node->uExtension.sDial.nStartAngle) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X + _Width * 0.5f;
			_pts[_startback].fY = _Y - tanf(_theta) * _Width * 0.5f;
			_tpts[_startback].fX = (_tx + _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty - tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx + _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty - tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 5)
		{
			BLF32 _theta = (_Node->uExtension.sDial.nStartAngle - 90.f) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X + _Width * 0.5f;
			_pts[_startback].fY = _Y + tanf(_theta) * _Width * 0.5f;
			_tpts[_startback].fX = (_tx + _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty + tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx + _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty - tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 7)
		{
			BLF32 _theta = (180.f - _Node->uExtension.sDial.nStartAngle) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X + tanf(_theta) * _Height * 0.5f;
			_pts[_startback].fY = _Y + _Height * 0.5f;
			_tpts[_startback].fX = (_tx + tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty + _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx + tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty + _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 9)
		{
			BLF32 _theta = (_Node->uExtension.sDial.nStartAngle - 180.f) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X - tanf(_theta) * _Height * 0.5f;
			_pts[_startback].fY = _Y + _Height * 0.5f;
			_tpts[_startback].fX = (_tx - tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty + _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx - tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty + _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 11)
		{
			BLF32 _theta = (270.f - _Node->uExtension.sDial.nStartAngle) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X - _Width * 0.5f;
			_pts[_startback].fY = _Y + tanf(_theta) * _Width * 0.5f;
			_tpts[_startback].fX = (_tx - _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty + tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx - _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty + tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 13)
		{
			BLF32 _theta = (_Node->uExtension.sDial.nStartAngle - 270.f) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X - _Width * 0.5f;
			_pts[_startback].fY = _Y - tanf(_theta) * _Width * 0.5f;
			_tpts[_startback].fX = (_tx - _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty - tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx - _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty - tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_startx == 15)
		{
			BLF32 _theta = (360.f - _Node->uExtension.sDial.nStartAngle) * PI_INTERNAL / 180.0f;
			_pts[_startback].fX = _X - tanf(_theta) * _Height * 0.5f;
			_pts[_startback].fY = _Y - _Height * 0.5f;
			_tpts[_startback].fX = (_tx - tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_startback].fY = (_ty - _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_startback].fX = (_stx - tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_startback].fY = (_sty - _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		if (_endx == 1)
		{
			BLF32 _theta = _Node->uExtension.sDial.nEndAngle * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X + tanf(_theta) * _Height * 0.5f;
			_pts[_endback].fY = _Y - _Height * 0.5f;
			_tpts[_endback].fX = (_tx + tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty - _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx + tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty - _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 3)
		{
			BLF32 _theta = (90.f - _Node->uExtension.sDial.nEndAngle) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X + _Width * 0.5f;
			_pts[_endback].fY = _Y - tanf(_theta) * _Width * 0.5f;
			_tpts[_endback].fX = (_tx + _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty - tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx + _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty - tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 5)
		{
			BLF32 _theta = (_Node->uExtension.sDial.nEndAngle - 90.f) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X + _Width * 0.5f;
			_pts[_endback].fY = _Y + tanf(_theta) * _Width * 0.5f;
			_tpts[_endback].fX = (_tx + _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty + tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx + _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty + tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 7)
		{
			BLF32 _theta = (180.f - _Node->uExtension.sDial.nEndAngle) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X + tanf(_theta) * _Height * 0.5f;
			_pts[_endback].fY = _Y + _Height * 0.5f;
			_tpts[_endback].fX = (_tx + tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty + _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx + tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty + _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 9)
		{
			BLF32 _theta = (_Node->uExtension.sDial.nEndAngle - 180.f) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X - tanf(_theta) * _Height * 0.5f;
			_pts[_endback].fY = _Y + _Height * 0.5f;
			_tpts[_endback].fX = (_tx - tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty + _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx - tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty + _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 11)
		{
			BLF32 _theta = (270.f - _Node->uExtension.sDial.nEndAngle) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X - _Width * 0.5f;
			_pts[_endback].fY = _Y + tanf(_theta) * _Width * 0.5f;
			_tpts[_endback].fX = (_tx - _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty + tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx - _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty + tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 13)
		{
			BLF32 _theta = (_Node->uExtension.sDial.nEndAngle - 270.f) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X - _Width * 0.5f;
			_pts[_endback].fY = _Y - tanf(_theta) * _Width * 0.5f;
			_tpts[_endback].fX = (_tx - _tw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty - tanf(_theta) * _tw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx - _stw * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty - tanf(_theta) * _stw * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		else if (_endx == 15)
		{
			BLF32 _theta = (360.f - _Node->uExtension.sDial.nEndAngle) * PI_INTERNAL / 180.0f;
			_pts[_endback].fX = _X - tanf(_theta) * _Height * 0.5f;
			_pts[_endback].fY = _Y - _Height * 0.5f;
			_tpts[_endback].fX = (_tx - tanf(_theta) * _th * 0.5f) / _Node->uExtension.sDial.nTexWidth;
			_tpts[_endback].fY = (_ty - _th * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
			{
				_tspts[_endback].fX = (_stx - tanf(_theta) * _sth * 0.5f) / _Node->uExtension.sDial.nTexWidth;
				_tspts[_endback].fY = (_sty - _sth * 0.5f) / _Node->uExtension.sDial.nTexHeight;
			}
		}
		BLF32* _vbo = (BLF32*)alloca(20 * 8 * sizeof(BLF32));
		BLU32 _vbcount = 0;
		_vbo[0] = _X;
		_vbo[1] = _Y;
		_vbo[2] = _tspts[17].fX;
		_vbo[3] = _tspts[17].fY;
		_vbo[4] = 1.f * _stencil;
		_vbo[5] = 1.f * _gray;
		_vbo[6] = _tpts[17].fX;
		_vbo[7] = _tpts[17].fY;
		do {
			if (_vbidx[_vbcount] == -1)
				break;
			if (_startx == _endx && _vbidx[_vbcount + 1] == -1)
			{
				_vbo[(1 + _vbcount) * 8 + 0] = PIXEL_ALIGNED_INTERNAL(_pts[16].fX);
				_vbo[(1 + _vbcount) * 8 + 1] = PIXEL_ALIGNED_INTERNAL(_pts[16].fY);
				_vbo[(1 + _vbcount) * 8 + 2] = _tspts[16].fX;
				_vbo[(1 + _vbcount) * 8 + 3] = _tspts[16].fY;
				_vbo[(1 + _vbcount) * 8 + 4] = 1.f * _stencil;
				_vbo[(1 + _vbcount) * 8 + 5] = 1.f * _gray;
				_vbo[(1 + _vbcount) * 8 + 6] = _tpts[16].fX;
				_vbo[(1 + _vbcount) * 8 + 7] = _tpts[16].fY;
			}
			else
			{
				_vbo[(1 + _vbcount) * 8 + 0] = PIXEL_ALIGNED_INTERNAL(_pts[_vbidx[_vbcount]].fX);
				_vbo[(1 + _vbcount) * 8 + 1] = PIXEL_ALIGNED_INTERNAL(_pts[_vbidx[_vbcount]].fY);
				_vbo[(1 + _vbcount) * 8 + 2] = _tspts[_vbidx[_vbcount]].fX;
				_vbo[(1 + _vbcount) * 8 + 3] = _tspts[_vbidx[_vbcount]].fY;
				_vbo[(1 + _vbcount) * 8 + 4] = 1.f * _stencil;
				_vbo[(1 + _vbcount) * 8 + 5] = 1.f * _gray;
				_vbo[(1 + _vbcount) * 8 + 6] = _tpts[_vbidx[_vbcount]].fX;
				_vbo[(1 + _vbcount) * 8 + 7] = _tpts[_vbidx[_vbcount]].fY;
			}
			++_vbcount;
		} while (1);
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLEFAN, TRUE, _semantic, _decls, 3, _vbo, (1 + _vbcount) * 8 * sizeof(BLF32), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	else
	{
		BLRect _texcoord;
		BLRect _texcoords;
		_texcoord = _Node->uExtension.sDial.sCommonTex;
		if (_Node->uExtension.sDial.aStencilMap[0] && strcmp(_Node->uExtension.sDial.aStencilMap, "Nil"))
		{
			_stencil = -1.f;
			_texcoords.sLT.fX = _Node->uExtension.sDial.sStencilTex.sLT.fX / _Node->uExtension.sDial.nTexWidth + 0.001f;
			_texcoords.sLT.fY = _Node->uExtension.sDial.sStencilTex.sLT.fY / _Node->uExtension.sDial.nTexHeight + 0.001f;
			_texcoords.sRB.fX = _Node->uExtension.sDial.sStencilTex.sRB.fX / _Node->uExtension.sDial.nTexWidth - 0.001f;
			_texcoords.sRB.fY = _Node->uExtension.sDial.sStencilTex.sRB.fY / _Node->uExtension.sDial.nTexHeight - 0.001f;
		}
		else
		{
			_texcoords.sLT.fX = 1.f;
			_texcoords.sLT.fY = 1.f;
			_texcoords.sRB.fX = 1.f;
			_texcoords.sRB.fY = 1.f;
		}
		BLF32 _rad = _Node->uExtension.sDial.bClockWise ? _Node->uExtension.sDial.fAngle * PI_INTERNAL / 180.0f : -_Node->uExtension.sDial.fAngle * PI_INTERNAL / 180.0f;
		BLF32 _ltx = ((_X - _Width * 0.5f) - _X) * cosf(_rad) - ((_Y - _Height * 0.5f) - _Y) * sinf(_rad) + _X;
		BLF32 _lty = ((_X - _Width * 0.5f) - _X) * sinf(_rad) + ((_Y - _Height * 0.5f) - _Y) * cosf(_rad) + _Y;
		BLF32 _rtx = ((_X + _Width * 0.5f) - _X) * cosf(_rad) - ((_Y - _Height * 0.5f) - _Y) * sinf(_rad) + _X;
		BLF32 _rty = ((_X + _Width * 0.5f) - _X) * sinf(_rad) + ((_Y - _Height * 0.5f) - _Y) * cosf(_rad) + _Y;
		BLF32 _lbx = ((_X - _Width * 0.5f) - _X) * cosf(_rad) - ((_Y + _Height * 0.5f) - _Y) * sinf(_rad) + _X;
		BLF32 _lby = ((_X - _Width * 0.5f) - _X) * sinf(_rad) + ((_Y + _Height * 0.5f) - _Y) * cosf(_rad) + _Y;
		BLF32 _rbx = ((_X + _Width * 0.5f) - _X) * cosf(_rad) - ((_Y + _Height * 0.5f) - _Y) * sinf(_rad) + _X;
		BLF32 _rby = ((_X + _Width * 0.5f) - _X) * sinf(_rad) + ((_Y + _Height * 0.5f) - _Y) * cosf(_rad) + _Y;
		BLF32 _vbo[] = {
			PIXEL_ALIGNED_INTERNAL(_ltx),
			PIXEL_ALIGNED_INTERNAL(_lty),
			_texcoords.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sDial.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sDial.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_rtx),
			PIXEL_ALIGNED_INTERNAL(_rty),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sDial.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sDial.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_lbx),
			PIXEL_ALIGNED_INTERNAL(_lby),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sDial.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sDial.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_rbx),
			PIXEL_ALIGNED_INTERNAL(_rby),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sDial.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sDial.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
}
static BLVoid
_DrawTable(_BLWidget* _Node, BLF32 _X, BLF32 _Y, BLF32 _Width, BLF32 _Height)
{
	if (!_Node->bValid || !_Node->bVisible)
		return;
	_TableMake(_Node);
	BLRect _scissorrect;
	_WidgetScissorRect(_Node, &_scissorrect);
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sTable.nPixmapTex, 0);
	BLF32 _gray = 1.f;
	BLF32 _offsetx = 0.f, _offsety = 0.f;
	BLF32 _stencil = 1.f;
	BLRect _texcoord, _texcoord9;
	BLRect _texcoords, _texcoords9;
	BLBool _flipx = _Node->uExtension.sTable.bFlipX;
	BLBool _flipy = _Node->uExtension.sTable.bFlipY;
	_texcoord = _Node->uExtension.sTable.sCommonTex;
	_texcoord9 = _Node->uExtension.sTable.sCommonTex9;
	if (_Node->uExtension.sTable.aStencilMap[0] && strcmp(_Node->uExtension.sTable.aStencilMap, "Nil"))
	{
		_stencil = -1.f;
		_texcoords.sLT.fX = _Node->uExtension.sTable.sStencilTex.sLT.fX / _Node->uExtension.sTable.nTexWidth + 0.001f;
		_texcoords.sLT.fY = _Node->uExtension.sTable.sStencilTex.sLT.fY / _Node->uExtension.sTable.nTexHeight + 0.001f;
		_texcoords.sRB.fX = _Node->uExtension.sTable.sStencilTex.sRB.fX / _Node->uExtension.sTable.nTexWidth - 0.001f;
		_texcoords.sRB.fY = _Node->uExtension.sTable.sStencilTex.sRB.fY / _Node->uExtension.sTable.nTexHeight - 0.001f;
		BLF32 _oriw = _Node->uExtension.sTable.sCommonTex.sRB.fX - _Node->uExtension.sTable.sCommonTex.sLT.fX;
		BLF32 _orih = _Node->uExtension.sTable.sCommonTex.sRB.fY - _Node->uExtension.sTable.sCommonTex.sLT.fY;
		BLF32 _dstw = _Node->uExtension.sTable.sStencilTex.sRB.fX - _Node->uExtension.sTable.sStencilTex.sLT.fX;
		BLF32 _dsth = _Node->uExtension.sTable.sStencilTex.sRB.fX - _Node->uExtension.sTable.sStencilTex.sLT.fX;
		BLF32 _ratioax = (_Node->uExtension.sTable.sCommonTex9.sLT.fX - _Node->uExtension.sTable.sCommonTex.sLT.fX) / _oriw;
		BLF32 _ratioay = (_Node->uExtension.sTable.sCommonTex9.sLT.fY - _Node->uExtension.sTable.sCommonTex.sLT.fY) / _orih;
		BLF32 _rationx = (_Node->uExtension.sTable.sCommonTex.sRB.fX - _Node->uExtension.sTable.sCommonTex9.sRB.fX) / _oriw;
		BLF32 _rationy = (_Node->uExtension.sTable.sCommonTex.sRB.fY - _Node->uExtension.sTable.sCommonTex9.sRB.fY) / _orih;
		_texcoords9.sLT.fX = (_Node->uExtension.sTable.sStencilTex.sLT.fX + _ratioax * _dstw) / _Node->uExtension.sTable.nTexWidth;
		_texcoords9.sLT.fY = (_Node->uExtension.sTable.sStencilTex.sLT.fY + _ratioay * _dsth) / _Node->uExtension.sTable.nTexHeight;
		_texcoords9.sRB.fX = (_Node->uExtension.sTable.sStencilTex.sRB.fX - _rationx * _dstw) / _Node->uExtension.sTable.nTexWidth;
		_texcoords9.sRB.fY = (_Node->uExtension.sTable.sStencilTex.sRB.fY - _rationy * _dsth) / _Node->uExtension.sTable.nTexHeight;
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
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
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
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _offsety),
			_texcoords.sRB.fX,
			_texcoords.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y - _Height * 0.5f + _marginay + _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sLT.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sLT.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sLT.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X - _Width * 0.5f + _marginax + _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sLT.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sLT.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords9.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _marginny - _offsety),
			_texcoords.sRB.fX,
			_texcoords9.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord9.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _marginnx - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords9.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord9.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
			PIXEL_ALIGNED_INTERNAL(_X + _Width * 0.5f - _offsetx),
			PIXEL_ALIGNED_INTERNAL(_Y + _Height * 0.5f - _offsety),
			_texcoords.sRB.fX,
			_texcoords.sRB.fY,
			1.f * _stencil,
			1.f * _gray,
			_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
			_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight
		};
		BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vob, sizeof(_vob), NULL, 0, BL_IF_INVALID);
		blDraw(_PrUIMem->nUITech, _geo, 1);
		blDeleteGeometryBuffer(_geo);
	}
	BLRect _scrolledclient = { 0 };
	_scrolledclient.sLT.fX = _X - _Width * 0.5f;
	_scrolledclient.sLT.fY = _Y - _Height * 0.5f + 1;
	_scrolledclient.sRB.fY = _Y - _Height * 0.5f + 1 + _Node->uExtension.sTable.nTotalHeight;
	_scrolledclient.sRB.fX = _X - _Width * 0.5f + _Node->uExtension.sTable.nTotalWidth;
	_scrolledclient.sLT.fY -= _Node->uExtension.sTable.nScroll;
	_scrolledclient.sRB.fY -= _Node->uExtension.sTable.nScroll;
	BLRect _rowr = { 0 };
	_rowr.sLT.fX = _scrolledclient.sLT.fX;
	_rowr.sLT.fY = _scrolledclient.sLT.fY;
	_rowr.sRB.fX = _scrolledclient.sRB.fX;
	_rowr.sRB.fY = _scrolledclient.sRB.fY;
	_rowr.sRB.fY = _rowr.sLT.fY + _Node->uExtension.sTable.nRowHeight;
	BLRect _clientc = { 0 };
	_clientc.sLT.fX = _X - _Width * 0.5f;
	_clientc.sLT.fY = _Y - _Height * 0.5f;
	_clientc.sRB.fX = _X + _Width * 0.5f;
	_clientc.sRB.fY = _Y + _Height * 0.5f;
	BLU32 _rownum = (BLU32)roundf((BLF32)_Node->uExtension.sTable.nCellNum / _Node->uExtension.sTable.nColumnNum);
	BLS32 _pos;
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_Node->uExtension.sTable.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return;
	BLU16 _flag = 0;
	if (_Node->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_Node->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_Node->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_Node->uExtension.sText.bItalics)
		_flag |= 0xF000; 
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_clientc.sLT.fX, (BLU32)_clientc.sLT.fY, (BLU32)(_clientc.sRB.fX - _clientc.sLT.fX), (BLU32)(_clientc.sRB.fY - _clientc.sLT.fY));
	for (BLU32 _idx = 0; _idx < _rownum; ++_idx)
	{
		if (_rowr.sRB.fY >= _clientc.sLT.fY - 10 && _rowr.sLT.fY <= _clientc.sRB.fY + 10)
		{
			BLBool _odd = TRUE;
			BLRect _rc;
			if (_idx % 2 == 1)
			{
				_rc.sLT.fX = _rowr.sLT.fX;
				_rc.sLT.fY = _rowr.sLT.fY;
				_rc.sRB.fX = _rowr.sRB.fX;
				_rc.sRB.fY = _rowr.sRB.fY;
				_rc.sRB.fX = _rc.sLT.fX + _Width;
			}
			if (_idx % 2 == 0)
			{
				_rc.sLT.fX = _rowr.sLT.fX;
				_rc.sLT.fY = _rowr.sLT.fY;
				_rc.sRB.fX = _rowr.sRB.fX;
				_rc.sRB.fY = _rowr.sRB.fY;
				_rc.sRB.fX = _rc.sLT.fX + _Width;
				_odd = FALSE;
			}
			_texcoord = _odd ? _Node->uExtension.sTable.sOddItemTex : _Node->uExtension.sTable.sEvenItemTex;
			if (_flipx)
			{
				BLF32 _tmp = _texcoord.sLT.fX;
				_texcoord.sLT.fX = _texcoord.sRB.fX;
				_texcoord.sRB.fX = _tmp;
			}
			if (_flipy)
			{
				BLF32 _tmp = _texcoord.sLT.fY;
				_texcoord.sLT.fY = _texcoord.sRB.fY;
				_texcoord.sRB.fY = _tmp;
			}
			BLF32 _vbo[] = {
				PIXEL_ALIGNED_INTERNAL(_rc.sLT.fX + _offsetx),
				PIXEL_ALIGNED_INTERNAL(_rc.sLT.fY + _offsety),
				1.f,
				1.f,
				1.f * _stencil,
				1.f * _gray,
				_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
				_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
				PIXEL_ALIGNED_INTERNAL(_rc.sRB.fX - _offsetx),
				PIXEL_ALIGNED_INTERNAL(_rc.sLT.fY + _offsety),
				1.f,
				1.f,
				1.f * _stencil,
				1.f * _gray,
				_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
				_texcoord.sLT.fY / _Node->uExtension.sTable.nTexHeight,
				PIXEL_ALIGNED_INTERNAL(_rc.sLT.fX + _offsetx),
				PIXEL_ALIGNED_INTERNAL(_rc.sRB.fY - _offsety),
				1.f,
				1.f,
				1.f * _stencil,
				1.f * _gray,
				_texcoord.sLT.fX / _Node->uExtension.sTable.nTexWidth,
				_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight,
				PIXEL_ALIGNED_INTERNAL(_rc.sRB.fX + _offsetx),
				PIXEL_ALIGNED_INTERNAL(_rc.sRB.fY - _offsety),
				1.f,
				1.f,
				1.f * _stencil,
				1.f * _gray,
				_texcoord.sRB.fX / _Node->uExtension.sTable.nTexWidth,
				_texcoord.sRB.fY / _Node->uExtension.sTable.nTexHeight
			};
			blTechSampler(_PrUIMem->nUITech, "Texture0", _Node->uExtension.sTable.nPixmapTex, 0);
			BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
			blDraw(_PrUIMem->nUITech, _geo, 1);
			blDeleteGeometryBuffer(_geo);
			BLRect _textr;
			_textr.sLT.fX = _rowr.sLT.fX;
			_textr.sLT.fY = _rowr.sLT.fY;
			_textr.sRB.fX = _rowr.sRB.fX;
			_textr.sRB.fY = _rowr.sRB.fY;
			_pos = (BLS32)_rowr.sLT.fX;
			if (_idx < _Node->uExtension.sTable.nCellNum / _Node->uExtension.sTable.nColumnNum)
			{
				for (BLU32 _jdx = 0; _jdx < _Node->uExtension.sTable.nColumnNum; ++_jdx)
				{
					_textr.sLT.fX = (BLF32)_pos;
					_textr.sRB.fX = (BLF32)_pos + _Node->uExtension.sTable.aColumnWidth[_jdx];
					BLU32 _cellidx = _jdx + _idx * _Node->uExtension.sTable.nColumnNum;
					if (_cellidx < _Node->uExtension.sTable.nCellNum)
					{
						if (_Node->uExtension.sTable.pCellText[_cellidx] && !strncmp(_Node->uExtension.sTable.pCellText[_cellidx], "#image#", 7))
						{
						}
						else
						{
							if (!_Node->uExtension.sTable.pCellBroken[_cellidx])
								_TableSplit(_Node, _ft, _flag, _Node->uExtension.sTable.pCellText[_cellidx], &_Node->uExtension.sTable.pCellBroken[_cellidx], _Node->uExtension.sTable.aColumnWidth[_jdx]);
							_WriteText(_Node->uExtension.sTable.pCellBroken[_cellidx], _Node->uExtension.sTable.aFontSource, _Node->uExtension.sTable.nFontHeight, BL_UA_HCENTER, BL_UA_VCENTER, FALSE, &_textr, &_clientc, _Node->uExtension.sTable.pCellColor[_cellidx], _flag, FALSE);
						}
					}
					_pos += _Node->uExtension.sTable.aColumnWidth[_jdx];
				}
			}
		}
		_rowr.sLT.fY += _Node->uExtension.sTable.nRowHeight;
		_rowr.sRB.fY += _Node->uExtension.sTable.nRowHeight;
	}
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
	blRasterState(BL_CM_CW, 0, 0.f, TRUE, (BLU32)_scissorrect.sLT.fX, (BLU32)_scissorrect.sLT.fY, (BLU32)(_scissorrect.sRB.fX - _scissorrect.sLT.fX), (BLU32)(_scissorrect.sRB.fY - _scissorrect.sLT.fY));
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
	BLGuid _geo = blGenGeometryBuffer(0xFFFFFFFF, BL_PT_TRIANGLES, TRUE, _semantic, _decls, 3, _vb, _vtxcount * 8 * sizeof(BLF32), _ib, _idxcount * sizeof(BLU32), BL_IF_32);
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
		BLF32 _x = (BLF32)LOWU16(_UParam) / _rx;		
		BLF32 _y = (BLF32)HIGU16(_UParam) / _ry;
		_x = (_x > 10 * _oriw) ? 0.f : _x;
		_y = (_y > 10 * _orih) ? 0.f : _y;
		_PrUIMem->pHoveredWidget = _LocateWidget(_PrUIMem->pRoot, _x, _y, 0.f, 0.f);
		if (_PrUIMem->pHoveredWidget != _lasthovered)
		{
			if (_lasthovered && _lasthovered->bInteractive)
			{
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
			case BL_UT_SLIDER:
				if (_wid->uExtension.sSlider.nState && _wid->uExtension.sSlider.bDragging)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLF32 _w, _p;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (_wid->uExtension.sSlider.bHorizontal)
					{
						_w = _area.sRB.fX - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX;
						_p = _x - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
					}
					else
					{
						_w = _area.sRB.fY - _area.sLT.fY - _wid->uExtension.sSlider.sSliderSize.fY;
						_p = -_y + _area.sRB.fY - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
					}
					BLS32 _newp = (BLS32)(_p / _w * (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue)) + _wid->uExtension.sSlider.nMinValue;
					BLS32 _oldp = _wid->uExtension.sSlider.nSliderPosition;
					if (!_wid->uExtension.sSlider.bSliderDragged)
					{
						if (blRectContains(&_drawarea, &_pos))
						{
							BLRect _thumb;
							BLF32 _perc = (BLF32)_wid->uExtension.sSlider.nSliderPosition / (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue);
							if (_wid->uExtension.sSlider.bHorizontal)
							{
								_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - (_area.sRB.fX - _area.sLT.fX) * 0.5f + ((_area.sRB.fX - _area.sLT.fX) - _wid->uExtension.sSlider.sSliderSize.fX) * _perc;
								_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
								_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
								_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
							}
							else
							{
								_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
								_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f + (_area.sRB.fY - _area.sLT.fY) * 0.5f - ((_area.sRB.fY - _area.sLT.fY) - _wid->uExtension.sSlider.sSliderSize.fY) * _perc - _wid->uExtension.sSlider.sSliderSize.fY;
								_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
								_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
							}
							_wid->uExtension.sSlider.bSliderDragged = blRectContains(&_thumb, &_pos);
							_wid->uExtension.sSlider.bTrayClick = !_wid->uExtension.sSlider.bSliderDragged;
						}
						if (_wid->uExtension.sSlider.bSliderDragged)
							blUISliderSliderPos(_wid->nID, _newp);
						else
						{
							_wid->uExtension.sSlider.bTrayClick = FALSE;
							break;
						}
						_wid->uExtension.sSlider.nDesiredPos = _newp;
					}
					else
						blUISliderSliderPos(_wid->nID, _newp);
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_DIAL:
				if (!_wid->uExtension.sDial.bAngleCut && _wid->uExtension.sDial.bDragging)
				{
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					BLF32 _stheta = _wid->uExtension.sDial.nStartAngle * PI_INTERNAL / 180.0f, _etheta = _wid->uExtension.sDial.nEndAngle * PI_INTERNAL / 180.0f;
					BLVec2 _vs, _ve, _vp;
					_vs.fX = sinf(_stheta);
					_vs.fY = -cosf(_stheta);
					_ve.fX = sinf(_etheta);
					_ve.fY = -cosf(_etheta);
					_vp.fX = _x - (_drawarea.sLT.fX + _drawarea.sRB.fX) * 0.5f;
					_vp.fY = _y - (_drawarea.sLT.fY + _drawarea.sRB.fY) * 0.5f;
					BLF32 _anglerange = atan2f(_vs.fX * _ve.fY - _ve.fX * _vs.fY, _vs.fX * _ve.fX + _vs.fY * _ve.fY) * (180.f / PI_INTERNAL);
					_anglerange = (blVec2CrossProduct(&_vs, &_ve) > 0.f) ? _anglerange : _anglerange + 360;
					_anglerange = _wid->uExtension.sDial.bClockWise ? _anglerange : 360 - _anglerange;
					BLF32 _anglecross = atan2f(_vs.fX * _vp.fY - _vp.fX * _vs.fY, _vs.fX * _vp.fX + _vs.fY * _vp.fY) * (180.f / PI_INTERNAL);
					_anglecross = (blVec2CrossProduct(&_vs, &_vp) > 0.f) ? _anglecross : _anglecross + 360;
					_anglecross = _wid->uExtension.sDial.bClockWise ? _anglecross : 360 - _anglecross;
					if (_wid->uExtension.sDial.bClockWise)
					{
						if (_anglecross > 360 - (360 - _anglerange) * 0.5f)
							_anglecross = 0.f;
						else if ((_anglecross < 360 - (360 - _anglerange) * 0.5f) && _anglecross > _anglerange)
							_anglecross = _anglerange;
						_anglecross = _anglecross + _wid->uExtension.sDial.nStartAngle;
					}
					else
					{
						if (_anglecross > _anglerange + (360 - _anglerange) * 0.5f)
							_anglecross = 0.f;
						else if ((_anglecross < _anglerange + (360 - _anglerange) * 0.5f) && _anglecross > _anglerange)
							_anglecross = _anglerange;
						_anglecross = 360 - _wid->uExtension.sDial.nStartAngle + _anglecross;
					}
					blUIDialAngle(_wid->nID, _anglecross);					
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TABLE:
				if (_wid->uExtension.sTable.bSelecting || _wid->uExtension.sTable.bDragging)
				{
					_wid->uExtension.sTable.bDragging = TRUE;
					_wid->uExtension.sTable.bSelecting = FALSE;
					_wid->uExtension.sTable.nScroll = _wid->uExtension.sTable.nScroll - (BLS32)(_y - _wid->uExtension.sTable.fDraggingPos);
					_wid->uExtension.sTable.fDraggingPos = _y;
					if ((BLS32)_wid->uExtension.sTable.nTotalHeight >(BLS32)_wid->sDimension.fY)
						_wid->uExtension.sTable.nScroll = (BLS32)blScalarClamp((BLF32)_wid->uExtension.sTable.nScroll, 0.f, (BLF32)_wid->uExtension.sTable.nTotalHeight - _wid->sDimension.fY);
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TEXT:
				if (_wid->uExtension.sText.bSelecting && _wid->uExtension.sText.nState)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
					_wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
					_TextScroll(_wid);
					_PrUIMem->bDirty = TRUE;
				}
				break;
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
			case BL_UT_SLIDER:
				if (_wid->uExtension.sSlider.nState && _wid->uExtension.sSlider.bDragging)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLF32 _w, _p;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (_wid->uExtension.sSlider.bHorizontal)
					{
						_w = _area.sRB.fX - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX;
						_p = _x - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
					}
					else
					{
						_w = _area.sRB.fY - _area.sLT.fY - _wid->uExtension.sSlider.sSliderSize.fY;
						_p = -_y + _area.sRB.fY - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
					}
					BLS32 _newp = (BLS32)(_p / _w * (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue)) + _wid->uExtension.sSlider.nMinValue;
					BLS32 _oldp = _wid->uExtension.sSlider.nSliderPosition;
					if (!_wid->uExtension.sSlider.bSliderDragged)
					{
						if (blRectContains(&_drawarea, &_pos))
						{
							BLRect _thumb;
							BLF32 _perc = (BLF32)_wid->uExtension.sSlider.nSliderPosition / (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue);
							if (_wid->uExtension.sSlider.bHorizontal)
							{
								_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - (_area.sRB.fX - _area.sLT.fX) * 0.5f + ((_area.sRB.fX - _area.sLT.fX) - _wid->uExtension.sSlider.sSliderSize.fX) * _perc;
								_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
								_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
								_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
							}
							else
							{
								_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
								_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f + (_area.sRB.fY - _area.sLT.fY) * 0.5f - ((_area.sRB.fY - _area.sLT.fY) - _wid->uExtension.sSlider.sSliderSize.fY) * _perc - _wid->uExtension.sSlider.sSliderSize.fY;
								_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
								_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
							}
							_wid->uExtension.sSlider.bSliderDragged = blRectContains(&_thumb, &_pos);
							_wid->uExtension.sSlider.bTrayClick = !_wid->uExtension.sSlider.bSliderDragged;
						}
						if (_wid->uExtension.sSlider.bSliderDragged)
							blUISliderSliderPos(_wid->nID, _newp);
						else
						{
							_wid->uExtension.sSlider.bTrayClick = FALSE;
							break;
						}
						_wid->uExtension.sSlider.nDesiredPos = _newp;
					}
					else
						blUISliderSliderPos(_wid->nID, _newp);
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_DIAL:
				if (!_wid->uExtension.sDial.bAngleCut && _wid->uExtension.sDial.bDragging)
				{
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					BLF32 _stheta = _wid->uExtension.sDial.nStartAngle * PI_INTERNAL / 180.0f, _etheta = _wid->uExtension.sDial.nEndAngle * PI_INTERNAL / 180.0f;
					BLVec2 _vs, _ve, _vp;
					_vs.fX = sinf(_stheta);
					_vs.fY = -cosf(_stheta);
					_ve.fX = sinf(_etheta);
					_ve.fY = -cosf(_etheta);
					_vp.fX = _x - (_drawarea.sLT.fX + _drawarea.sRB.fX) * 0.5f;
					_vp.fY = _y - (_drawarea.sLT.fY + _drawarea.sRB.fY) * 0.5f;
					BLF32 _anglerange = atan2f(_vs.fX * _ve.fY - _ve.fX * _vs.fY, _vs.fX * _ve.fX + _vs.fY * _ve.fY) * (180.f / PI_INTERNAL);
					_anglerange = (blVec2CrossProduct(&_vs, &_ve) > 0.f) ? _anglerange : _anglerange + 360;
					_anglerange = _wid->uExtension.sDial.bClockWise ? _anglerange : 360 - _anglerange;
					BLF32 _anglecross = atan2f(_vs.fX * _vp.fY - _vp.fX * _vs.fY, _vs.fX * _vp.fX + _vs.fY * _vp.fY) * (180.f / PI_INTERNAL);
					_anglecross = (blVec2CrossProduct(&_vs, &_vp) > 0.f) ? _anglecross : _anglecross + 360;
					_anglecross = _wid->uExtension.sDial.bClockWise ? _anglecross : 360 - _anglecross;
					if (_wid->uExtension.sDial.bClockWise)
					{
						if (_anglecross > 360 - (360 - _anglerange) * 0.5f)
							_anglecross = 0.f;
						else if ((_anglecross < 360 - (360 - _anglerange) * 0.5f) && _anglecross > _anglerange)
							_anglecross = _anglerange;
						_anglecross = _anglecross + _wid->uExtension.sDial.nStartAngle;
					}
					else
					{
						if (_anglecross > _anglerange + (360 - _anglerange) * 0.5f)
							_anglecross = 0.f;
						else if ((_anglecross < _anglerange + (360 - _anglerange) * 0.5f) && _anglecross > _anglerange)
							_anglecross = _anglerange;
						_anglecross = 360 - _wid->uExtension.sDial.nStartAngle + _anglecross;
					}
					blUIDialAngle(_wid->nID, _anglecross);
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TABLE:
				if (_wid->uExtension.sTable.bSelecting || _wid->uExtension.sTable.bDragging)
				{
					_wid->uExtension.sTable.bDragging = TRUE;
					_wid->uExtension.sTable.bSelecting = FALSE;
					_wid->uExtension.sTable.nScroll = _wid->uExtension.sTable.nScroll - (BLS32)(_y - _wid->uExtension.sTable.fDraggingPos);
					_wid->uExtension.sTable.fDraggingPos = _y;
					if ((BLS32)_wid->uExtension.sTable.nTotalHeight >(BLS32)_wid->sDimension.fY)
						_wid->uExtension.sTable.nScroll = (BLS32)blScalarClamp((BLF32)_wid->uExtension.sTable.nScroll, 0.f, (BLF32)_wid->uExtension.sTable.nTotalHeight - _wid->sDimension.fY);
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TEXT:
				if (_wid->uExtension.sText.bSelecting && _wid->uExtension.sText.nState)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
					_wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
					_TextScroll(_wid);
					_PrUIMem->bDirty = TRUE;
				}
				break;
			default:break;
			}
		}
	}
	else if (_Type == BL_ME_LDOWN || _Type == BL_ME_RDOWN)
	{
		BLU32 _oriw, _orih, _aw, _ah;
		BLF32 _rx, _ry;
		blGetWindowSize(&_oriw, &_orih, &_aw, &_ah, &_rx, &_ry);
		BLF32 _x = (BLF32)LOWU16(_UParam) / _rx;
		BLF32 _y = (BLF32)HIGU16(_UParam) / _ry;
		_x = (_x > 10 * _oriw) ? 0.f : _x;
		_y = (_y > 10 * _orih) ? 0.f : _y;
		_BLWidget* _lasthovered = _PrUIMem->pHoveredWidget;
		_PrUIMem->pHoveredWidget = _LocateWidget(_PrUIMem->pRoot, _x, _y, 0.f, 0.f);
		if (_PrUIMem->pHoveredWidget != _lasthovered)
		{
			if (_lasthovered && _lasthovered->bInteractive)
			{
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
					if (_PrUIMem->pFocusWidget->eType == BL_UT_SLIDER)
						_PrUIMem->pFocusWidget->uExtension.sSlider.bDragging = FALSE;
					else if (_PrUIMem->pFocusWidget->eType == BL_UT_DIAL)
						_PrUIMem->pFocusWidget->uExtension.sDial.bDragging = FALSE;
					else if (_PrUIMem->pFocusWidget->eType == BL_UT_TEXT)
						blDetachIME();
					else if (_PrUIMem->pFocusWidget->eType == BL_UT_TABLE)
						_PrUIMem->pFocusWidget->uExtension.sTable.bDragging = FALSE;
					//lost focs;_PrUIMem->pFocusWidget
					//Panel
				}
				if (_PrUIMem->pHoveredWidget)
				{
					if (_PrUIMem->pHoveredWidget->eType == BL_UT_TEXT)
						blAttachIME(_x, _y + _PrUIMem->pHoveredWidget->sDimension.fY * 0.5f);
					//gain focus;_PrUIMem->pHoveredWidget
					//Panel
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
				break;
			case BL_UT_SLIDER:
				if (_wid->uExtension.sSlider.nState)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (blRectContains(&_drawarea, &_pos))
					{
						_wid->uExtension.sSlider.bDragging = TRUE;
						BLRect _thumb;
						BLF32 _perc = (BLF32)_wid->uExtension.sSlider.nSliderPosition / (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue);
						if (_wid->uExtension.sSlider.bHorizontal)
						{
							_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - (_area.sRB.fX - _area.sLT.fX) * 0.5f + ((_area.sRB.fX - _area.sLT.fX) - _wid->uExtension.sSlider.sSliderSize.fX) * _perc;
							_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
							_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
							_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
						}
						else
						{
							_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
							_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f + (_area.sRB.fY - _area.sLT.fY) * 0.5f - ((_area.sRB.fY - _area.sLT.fY) - _wid->uExtension.sSlider.sSliderSize.fY) * _perc - _wid->uExtension.sSlider.sSliderSize.fY;
							_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
							_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
						}
						_wid->uExtension.sSlider.bSliderDragged = blRectContains(&_thumb, &_pos);
						_wid->uExtension.sSlider.bTrayClick = !_wid->uExtension.sSlider.bSliderDragged;
						BLF32 _w, _p;
						if (_wid->uExtension.sSlider.bHorizontal)
						{
							_w = _area.sRB.fX - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX;
							_p = _x - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
						}
						else
						{
							_w = _area.sRB.fY - _area.sLT.fY - _wid->uExtension.sSlider.sSliderSize.fY;
							_p = - _y + _area.sRB.fY - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
						}
						_wid->uExtension.sSlider.nDesiredPos = (BLS32)(_p / _w * (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue)) + _wid->uExtension.sSlider.nMinValue;
					}
				}
				break;
			case BL_UT_DIAL:
				if (!_wid->uExtension.sDial.bAngleCut)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (blRectContains(&_drawarea, &_pos))
					{
						_wid->uExtension.sDial.bDragging = TRUE;
					}
				}
				break;
			case BL_UT_TABLE:
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (blRectContains(&_drawarea, &_pos))
					{
						_wid->uExtension.sTable.bDragging = FALSE;
						_wid->uExtension.sTable.bSelecting = TRUE;
						_wid->uExtension.sTable.fDraggingPos = _y;
					}
				}
				break;
			case BL_UT_TEXT:
				if (_wid->uExtension.sText.nState == 1)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					if (_PrUIMem->pFocusWidget != _wid)
					{
						_wid->uExtension.sText.bSelecting = TRUE;
						_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
						_wid->uExtension.sText.nSelectBegin = _wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
						_TextScroll(_wid);
						_PrUIMem->bDirty = TRUE;
					}
					else
					{
						BLRect _area;
						BLRect _sarea;
						_WidgetAbsArea(_wid, &_area);
						_WidgetScissorRect(_wid, &_sarea);
						BLRect _drawarea = blRectIntersects(&_area, &_sarea);
						if (blRectContains(&_drawarea, &_pos))
						{
							_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
							BLS32 _newbegin = _wid->uExtension.sText.nSelectBegin;
							if (!_wid->uExtension.sText.bSelecting)
								_newbegin = _wid->uExtension.sText.nCaretPos;
							_wid->uExtension.sText.bSelecting = TRUE;
							_wid->uExtension.sText.nSelectBegin = _newbegin;
							_wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
							_TextScroll(_wid);
							_PrUIMem->bDirty = TRUE;
						}
					}
				}
				break;
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
				break;
			case BL_UT_SLIDER:
				break;
				if (_wid->uExtension.sSlider.nState)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (blRectContains(&_drawarea, &_pos))
					{
						_wid->uExtension.sSlider.bDragging = TRUE;
						BLRect _thumb;
						BLF32 _perc = (BLF32)_wid->uExtension.sSlider.nSliderPosition / (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue);
						if (_wid->uExtension.sSlider.bHorizontal)
						{
							_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - (_area.sRB.fX - _area.sLT.fX) * 0.5f + ((_area.sRB.fX - _area.sLT.fX) - _wid->uExtension.sSlider.sSliderSize.fX) * _perc;
							_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
							_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
							_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
						}
						else
						{
							_thumb.sLT.fX = (_area.sLT.fX + _area.sRB.fX) * 0.5f - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
							_thumb.sLT.fY = (_area.sLT.fY + _area.sRB.fY) * 0.5f + (_area.sRB.fY - _area.sLT.fY) * 0.5f - ((_area.sRB.fY - _area.sLT.fY) - _wid->uExtension.sSlider.sSliderSize.fY) * _perc - _wid->uExtension.sSlider.sSliderSize.fY;
							_thumb.sRB.fX = _thumb.sLT.fX + _wid->uExtension.sSlider.sSliderSize.fX;
							_thumb.sRB.fY = _thumb.sLT.fY + _wid->uExtension.sSlider.sSliderSize.fY;
						}
						_wid->uExtension.sSlider.bSliderDragged = blRectContains(&_thumb, &_pos);
						_wid->uExtension.sSlider.bTrayClick = !_wid->uExtension.sSlider.bSliderDragged;
						BLF32 _w, _p;
						if (_wid->uExtension.sSlider.bHorizontal)
						{
							_w = _area.sRB.fX - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX;
							_p = _x - _area.sLT.fX - _wid->uExtension.sSlider.sSliderSize.fX * 0.5f;
						}
						else
						{
							_w = _area.sRB.fY - _area.sLT.fY - _wid->uExtension.sSlider.sSliderSize.fY;
							_p = -_y + _area.sRB.fY - _wid->uExtension.sSlider.sSliderSize.fY * 0.5f;
						}
						_wid->uExtension.sSlider.nDesiredPos = (BLS32)(_p / _w * (BLF32)(_wid->uExtension.sSlider.nMaxValue - _wid->uExtension.sSlider.nMinValue)) + _wid->uExtension.sSlider.nMinValue;
					}
				}
				break;
			case BL_UT_DIAL:
				if (!_wid->uExtension.sDial.bAngleCut)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (blRectContains(&_drawarea, &_pos))
					{
						_wid->uExtension.sDial.bDragging = TRUE;
					}
				}
				break;
			case BL_UT_TABLE:
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					if (blRectContains(&_drawarea, &_pos))
					{
						_wid->uExtension.sTable.bDragging = FALSE;
						_wid->uExtension.sTable.bSelecting = TRUE;
						_wid->uExtension.sTable.fDraggingPos = _y;
					}
				}
				break;
			case BL_UT_TEXT:
				if (_wid->uExtension.sText.nState == 1)
				{
					BLVec2 _pos;
					_pos.fX = _x;
					_pos.fY = _y;
					if (_PrUIMem->pFocusWidget != _wid)
					{
						_wid->uExtension.sText.bSelecting = TRUE;
						_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
						_wid->uExtension.sText.nSelectBegin = _wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
						_TextScroll(_wid);
						_PrUIMem->bDirty = TRUE;
					}
					else
					{
						BLRect _area;
						BLRect _sarea;
						_WidgetAbsArea(_wid, &_area);
						_WidgetScissorRect(_wid, &_sarea);
						BLRect _drawarea = blRectIntersects(&_area, &_sarea);
						if (blRectContains(&_drawarea, &_pos))
						{
							_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
							BLS32 _newbegin = _wid->uExtension.sText.nSelectBegin;
							if (!_wid->uExtension.sText.bSelecting)
								_newbegin = _wid->uExtension.sText.nCaretPos;
							_wid->uExtension.sText.bSelecting = TRUE;
							_wid->uExtension.sText.nSelectBegin = _newbegin;
							_wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
							_TextScroll(_wid);
							_PrUIMem->bDirty = TRUE;
						}
					}
				}
				break;
			default:break;
			}
		}
	}
	else if (_Type == BL_ME_LUP || _Type == BL_ME_RUP)
	{
		BLU32 _oriw, _orih, _aw, _ah;
		BLF32 _rx, _ry;
		blGetWindowSize(&_oriw, &_orih, &_aw, &_ah, &_rx, &_ry);
		BLF32 _x = (BLF32)LOWU16(_UParam) / _rx;
		BLF32 _y = (BLF32)HIGU16(_UParam) / _ry;
		_x = (_x > 10 * _oriw) ? 0.f : _x;
		_y = (_y > 10 * _orih) ? 0.f : _y;
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
				break;
			case BL_UT_CHECK:
				if (_wid->uExtension.sCheck.nState)
				{
					_wid->uExtension.sCheck.nState = 3 - _wid->uExtension.sCheck.nState;
					_PrUIMem->bDirty = TRUE;
				}
			case BL_UT_SLIDER:
				if (_wid->uExtension.sSlider.nState)
				{
					if (_wid->uExtension.sSlider.bTrayClick)
					{
						if (_wid->uExtension.sSlider.nDesiredPos >= _wid->uExtension.sSlider.nSliderPosition + _wid->uExtension.sSlider.nStep)
							blUISliderSliderPos(_wid->nID, _wid->uExtension.sSlider.nSliderPosition + _wid->uExtension.sSlider.nStep);
						else if (_wid->uExtension.sSlider.nDesiredPos <= _wid->uExtension.sSlider.nSliderPosition - _wid->uExtension.sSlider.nStep)
							blUISliderSliderPos(_wid->nID, _wid->uExtension.sSlider.nSliderPosition - _wid->uExtension.sSlider.nStep);
						else if (_wid->uExtension.sSlider.nDesiredPos >= _wid->uExtension.sSlider.nSliderPosition - _wid->uExtension.sSlider.nStep && _wid->uExtension.sSlider.nDesiredPos <= _wid->uExtension.sSlider.nSliderPosition + _wid->uExtension.sSlider.nStep)
							blUISliderSliderPos(_wid->nID, _wid->uExtension.sSlider.nDesiredPos);
					}
					_wid->uExtension.sSlider.bDragging = FALSE;
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_DIAL:
				if (!_wid->uExtension.sDial.bAngleCut && _wid->uExtension.sDial.bDragging)
				{
					_wid->uExtension.sDial.bDragging = FALSE;
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TABLE:
				if (_wid->uExtension.sTable.bDragging)
				{
					_wid->uExtension.sTable.bDragging = FALSE;
					_wid->uExtension.sTable.bSelecting = FALSE;
					_PrUIMem->bDirty = TRUE;
				}
				else if (_wid->uExtension.sTable.bSelecting)
				{
					_wid->uExtension.sTable.bDragging = FALSE;
					_wid->uExtension.sTable.bSelecting = FALSE;
					BLU32 _oldsel = _wid->uExtension.sTable.nSelectedCell;
					BLS32 _selrow, _selcol = 0;
					BLRect _area;
					BLRect _sarea;
					_WidgetAbsArea(_wid, &_area);
					_WidgetScissorRect(_wid, &_sarea);
					BLRect _drawarea = blRectIntersects(&_area, &_sarea);
					BLU32 _rownum = (BLU32)roundf((BLF32)_wid->uExtension.sTable.nCellNum / _wid->uExtension.sTable.nColumnNum);
					if (_y < _drawarea.sLT.fY)
						break;
					if (_wid->uExtension.sTable.nRowHeight)
						_selrow = (BLS32)((_y - _drawarea.sLT.fY) + _wid->uExtension.sTable.nScroll) / _wid->uExtension.sTable.nRowHeight;
					if (_selrow >= (BLS32)_rownum)
						_selrow = _rownum - 1;
					else if (_selrow < 0)
						_selrow = 0;
					BLF32 _edge = _drawarea.sLT.fX;
					for (BLU32 _idx = 0; _idx < _wid->uExtension.sTable.nColumnNum; ++_idx)
					{
						if (_x > _edge && _x < _edge + _wid->uExtension.sTable.aColumnWidth[_idx])
						{
							_selcol = _idx;
							break;
						}
						else
							_edge += _wid->uExtension.sTable.aColumnWidth[_idx];
					}
					_wid->uExtension.sTable.nSelectedCell = _selrow * _wid->uExtension.sTable.nColumnNum + _selcol;
					if (_wid->uExtension.sTable.nSelectedCell != _oldsel)
					{
					}
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TEXT:
				if (_wid->uExtension.sText.nState == 1)
				{
					if (_PrUIMem->pFocusWidget == _wid)
					{
						BLVec2 _pos;
						_pos.fX = _x;
						_pos.fY = _y;
						_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
						if (_wid->uExtension.sText.bSelecting)
							_wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
						_wid->uExtension.sText.bSelecting = FALSE;
						_TextScroll(_wid);
						_PrUIMem->bDirty = TRUE;
					}
				}
				break;
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
				break;
			case BL_UT_CHECK:
				if (_wid->uExtension.sCheck.nState)
				{
					_wid->uExtension.sCheck.nState = 3 - _wid->uExtension.sCheck.nState;
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_SLIDER:
				if (_wid->uExtension.sSlider.nState)
				{
					if (_wid->uExtension.sSlider.bTrayClick)
					{
						if (_wid->uExtension.sSlider.nDesiredPos >= _wid->uExtension.sSlider.nSliderPosition + _wid->uExtension.sSlider.nStep)
							blUISliderSliderPos(_wid->nID, _wid->uExtension.sSlider.nSliderPosition + _wid->uExtension.sSlider.nStep);
						else if (_wid->uExtension.sSlider.nDesiredPos <= _wid->uExtension.sSlider.nSliderPosition - _wid->uExtension.sSlider.nStep)
							blUISliderSliderPos(_wid->nID, _wid->uExtension.sSlider.nSliderPosition - _wid->uExtension.sSlider.nStep);
						else if (_wid->uExtension.sSlider.nDesiredPos >= _wid->uExtension.sSlider.nSliderPosition - _wid->uExtension.sSlider.nStep && _wid->uExtension.sSlider.nDesiredPos <= _wid->uExtension.sSlider.nSliderPosition + _wid->uExtension.sSlider.nStep)
							blUISliderSliderPos(_wid->nID, _wid->uExtension.sSlider.nDesiredPos);
					}
					_wid->uExtension.sSlider.bDragging = FALSE;
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_DIAL:
				if (!_wid->uExtension.sDial.bAngleCut && _wid->uExtension.sDial.bDragging)
				{
					_wid->uExtension.sDial.bDragging = FALSE;
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TABLE:
				if (_wid->uExtension.sTable.bDragging)
				{
					_wid->uExtension.sTable.bDragging = FALSE;
					_PrUIMem->bDirty = TRUE;
				}
				break;
			case BL_UT_TEXT:
				if (_wid->uExtension.sText.nState == 1)
				{
					if (_PrUIMem->pFocusWidget == _wid)
					{
						BLVec2 _pos;
						_pos.fX = _x;
						_pos.fY = _y;
						_wid->uExtension.sText.nCaretPos = _CaretPos(_wid, &_pos);
						if (_wid->uExtension.sText.bSelecting)
							_wid->uExtension.sText.nSelectEnd = _wid->uExtension.sText.nCaretPos;
						_wid->uExtension.sText.bSelecting = FALSE;
						_TextScroll(_wid);
						_PrUIMem->bDirty = TRUE;
					}
				}
				break;
			default:break;
			}
		}
	}
	else
	{
		BLBool _up = (BLBool)HIGU16(_UParam);
		BLS32 _val = LOWU16(_UParam);
		_BLWidget* _lastfocus = _PrUIMem->pFocusWidget;
		if (_PrUIMem->pFocusWidget && _PrUIMem->pFocusWidget->bVisible)
		{
			_BLWidget* _wid = _PrUIMem->pFocusWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			if (_wid->eType == BL_UT_TEXT)
			{
				BLS32 _newmarkbegin = _wid->uExtension.sText.nSelectBegin;
				BLS32 _newmarkend = _wid->uExtension.sText.nSelectEnd;
				if (_up)
				{
					if (_wid->uExtension.sText.bMultiline)
					{
						BLS32 _lineno = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
						if (_lineno > 0)
						{
							BLS32 _cp = _wid->uExtension.sText.nCaretPos - _wid->uExtension.sText.aSplitPositions[_lineno];
							if (((BLU32)_lineno >= _wid->uExtension.sText.nSplitSize) && (BLS32)blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1]) < _cp)
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno - 1] + MAX_INTERNAL((BLU32)1, blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1])) - 1;
							else
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno - 1] + _cp;
						}
						_newmarkbegin = 0;
						_newmarkend = 0;
					}
				}
				else
				{
					if (_wid->uExtension.sText.bMultiline)
					{
						BLS32 _lineno = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
						if (_lineno < (BLS32)_wid->uExtension.sText.nSplitSize - 1)
						{
							BLS32 _cp = _wid->uExtension.sText.nCaretPos - _wid->uExtension.sText.aSplitPositions[_lineno];
							if (((BLU32)_lineno >= _wid->uExtension.sText.nSplitSize) && (BLS32)blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1]) < _cp)
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno + 1] + MAX_INTERNAL((BLU32)1, blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1])) - 1;
							else
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno + 1] + _cp;
						}
						_newmarkbegin = 0;
						_newmarkend = 0;
					}
				}
				_wid->uExtension.sText.nSelectBegin = _newmarkbegin;
				_wid->uExtension.sText.nSelectEnd = _newmarkend;
				_TextScroll(_wid);
				_PrUIMem->bDirty = TRUE;
			}
			else if (_wid->eType == BL_UT_TABLE)
			{
				_wid->uExtension.sTable.nScroll = _wid->uExtension.sTable.nScroll + 10 * _val * (_up ? 1 : -1);
				if ((BLS32)_wid->uExtension.sTable.nTotalHeight >(BLS32)_wid->sDimension.fY)
					_wid->uExtension.sTable.nScroll = (BLS32)blScalarClamp((BLF32)_wid->uExtension.sTable.nScroll, 0.f, (BLF32)_wid->uExtension.sTable.nTotalHeight - _wid->sDimension.fY);
				_PrUIMem->bDirty = TRUE;
			}
		}
		if (!_lastfocus && _PrUIMem->pHoveredWidget && _PrUIMem->pHoveredWidget->bVisible)
		{
			_BLWidget* _wid = _PrUIMem->pHoveredWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			if (_wid->eType == BL_UT_TEXT)
			{
				BLS32 _newmarkbegin = _wid->uExtension.sText.nSelectBegin;
				BLS32 _newmarkend = _wid->uExtension.sText.nSelectEnd;
				if (_up)
				{
					if (_wid->uExtension.sText.bMultiline)
					{
						BLS32 _lineno = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
						if (_lineno > 0)
						{
							BLS32 _cp = _wid->uExtension.sText.nCaretPos - _wid->uExtension.sText.aSplitPositions[_lineno];
							if (((BLU32)_lineno >= _wid->uExtension.sText.nSplitSize) && (BLS32)blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1]) < _cp)
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno - 1] + MAX_INTERNAL((BLU32)1, blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1])) - 1;
							else
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno - 1] + _cp;
						}
						_newmarkbegin = 0;
						_newmarkend = 0;
					}
				}
				else
				{
					if (_wid->uExtension.sText.bMultiline)
					{
						BLS32 _lineno = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
						if (_lineno < (BLS32)_wid->uExtension.sText.nSplitSize - 1)
						{
							BLS32 _cp = _wid->uExtension.sText.nCaretPos - _wid->uExtension.sText.aSplitPositions[_lineno];
							if (((BLU32)_lineno >= _wid->uExtension.sText.nSplitSize) && (BLS32)blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1]) < _cp)
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno + 1] + MAX_INTERNAL((BLU32)1, blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1])) - 1;
							else
								_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno + 1] + _cp;
						}
						_newmarkbegin = 0;
						_newmarkend = 0;
					}
				}
				_wid->uExtension.sText.nSelectBegin = _newmarkbegin;
				_wid->uExtension.sText.nSelectEnd = _newmarkend;
				_TextScroll(_wid);
				_PrUIMem->bDirty = TRUE;
			}
			else if (_wid->eType == BL_UT_TABLE)
			{
				_wid->uExtension.sTable.nScroll = _wid->uExtension.sTable.nScroll + 10 * _val * (_up ? 1 : -1);
				if ((BLS32)_wid->uExtension.sTable.nTotalHeight >(BLS32)_wid->sDimension.fY)
					_wid->uExtension.sTable.nScroll = (BLS32)blScalarClamp((BLF32)_wid->uExtension.sTable.nScroll, 0.f, (BLF32)_wid->uExtension.sTable.nTotalHeight - _wid->sDimension.fY);
				_PrUIMem->bDirty = TRUE;
			}
		}
	}
}
static const BLVoid
_KeyboardSubscriber(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam)
{
	if (_Type == BL_ET_KEY)
	{
		if (_PrUIMem->pFocusWidget && _PrUIMem->pFocusWidget->bVisible)
		{
			_BLWidget* _wid = _PrUIMem->pFocusWidget;
			while (_wid->bPenetration)
				_wid = _wid->pParent;
			if (BL_UT_TEXT == _wid->eType && _wid->uExtension.sText.nState)
			{
				_BLFont* _ft = NULL;
				{
					FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
					{
						if (_iter->nHashName == blHashUtf8((const BLUtf8*)_wid->uExtension.sText.aFontSource))
						{
							_ft = _iter;
							break;
						}
					}
				}
				if (!_ft)
					return;
				BLU16 _flag = 0;
				if (_wid->uExtension.sText.bOutline)
					_flag |= 0x000F;
				if (_wid->uExtension.sText.bBold)
					_flag |= 0x00F0;
				if (_wid->uExtension.sText.bShadow)
					_flag |= 0x0F00;
				if (_wid->uExtension.sText.bItalics)
					_flag |= 0xF000;
				if (_PParam)
				{
					BLAnsi _old[256] = { 0 };
					if (_wid->uExtension.sText.pText)
						strcpy(_old, _wid->uExtension.sText.pText);
					BLUtf8* _str = _PParam;
					const BLUtf16* _s16 = blGenUtf16Str(_str);
					BLU32 _s16len = blUtf16Length(_s16);					
					for (BLU32 _idx = 0; _idx < _s16len; ++_idx)
					{
						BLUtf16 _ch = _s16[_idx];
						if (_ch <= 255)
						{
							if (_ch != 13)
							{
								if (_ch >= 0 && _ch <= 31)
									continue;
								else if (_ch == 255)
									continue;
							}
						}
						if (_ch)
						{
							if (_s16len < _wid->uExtension.sText.nMaxLength)
							{
								BLUtf16* _s;
								if (_wid->uExtension.sText.nSelectBegin != _wid->uExtension.sText.nSelectEnd)
								{
									const BLUtf16* _tmp = blGenUtf16Str(_wid->uExtension.sText.pText);
									BLU32 _tmplen = blUtf16Length(_tmp);
									BLS32 _realmbgn = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd;
									BLS32 _realmend = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectEnd : _wid->uExtension.sText.nSelectBegin;
									_s = (BLUtf16*)alloca((2 + _tmplen + _realmbgn - _realmend) * sizeof(BLUtf16));
									BLS32 _jdx = 0;
									for (; _jdx < _realmbgn; ++_jdx)
										_s[_jdx] = _tmp[_jdx];
									_s[_jdx++] = _ch;
									for (BLU32 _kdx = 0; _kdx < _tmplen - _realmend; ++_kdx, ++_jdx)
										_s[_jdx] = _tmp[_realmend + _kdx];
									_s[_jdx] = 0;
									if (_wid->uExtension.sText.pText)
										free(_wid->uExtension.sText.pText);
									_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str(_s);
									_wid->uExtension.sText.nCaretPos = _realmbgn + 1;
								}
								else
								{
									const BLUtf16* _tmp = blGenUtf16Str(_wid->uExtension.sText.pText);
									BLU32 _tmplen = blUtf16Length(_tmp);
									_s = (BLUtf16*)alloca((2 + _tmplen) * sizeof(BLUtf16));
									BLS32 _jdx = 0;
									for (; _jdx < _wid->uExtension.sText.nCaretPos; ++_jdx)
										_s[_jdx] = _tmp[_jdx];
									_s[_jdx++] = _ch;
									for (BLU32 _kdx = 0; _kdx < _tmplen - _wid->uExtension.sText.nCaretPos; ++_kdx, ++_jdx)
										_s[_jdx] = _tmp[_wid->uExtension.sText.nCaretPos + _kdx];
									_s[_jdx] = 0;
									if (_wid->uExtension.sText.pText)
										free(_wid->uExtension.sText.pText);
									_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str(_s);
									++_wid->uExtension.sText.nCaretPos;
									blDeleteUtf16Str((BLUtf16*)_tmp);
								}
								_wid->uExtension.sText.nSelectBegin = _wid->uExtension.sText.nSelectEnd = 0;
							}
						}
						_TextSplit(_wid, _ft, _flag);
						_TextScroll(_wid);
						if (_wid->uExtension.sText.bNumeric)
						{
							BLBool _numeric = TRUE;
							BLAnsi _tmp[256] = "";
							const BLAnsi* _ptr = _wid->uExtension.sText.pText;
							sscanf(_ptr, " %[.,0-9]", _tmp);
							if (strcmp(_ptr, _tmp))
								_numeric = FALSE;
							BLAnsi* _p;
							if (_p = strstr(_tmp, "."))
							{
								_p = strstr(_p + 1, ".");
								if (_p)
									_numeric = FALSE;
							}
							if (!_numeric)
							{
								if (_wid->uExtension.sText.pText)
								{
									free(_wid->uExtension.sText.pText);
									_wid->uExtension.sText.pText = NULL;
								}
								if (_old[0])
								{
									_wid->uExtension.sText.pText = malloc(strlen(_old) + 1);
									memset(_wid->uExtension.sText.pText, 0, strlen(_old) + 1);
									strcpy(_wid->uExtension.sText.pText, _old);
								}
								_wid->uExtension.sText.nCaretPos--;
								break;
							}
							BLF32 _val = (BLF32)strtod(_wid->uExtension.sText.pText, NULL);
							BLF32 _valnew = blScalarClamp(_val, (BLF32)_wid->uExtension.sText.nMinValue, (BLF32)_wid->uExtension.sText.nMaxValue);
							if ((BLS32)_val != (BLS32)_valnew)
							{
								_val = _valnew;
								if (_wid->uExtension.sText.pText)
									free(_wid->uExtension.sText.pText);
								memset(_tmp, 0, sizeof(_tmp));
								sprintf(_tmp, "%d", (BLS32)_val);
								_wid->uExtension.sText.pText = malloc(strlen(_tmp) + 1);
								memset(_wid->uExtension.sText.pText, 0, strlen(_tmp) + 1);
								strcpy(_wid->uExtension.sText.pText, _tmp);
								_wid->uExtension.sText.nCaretPos--;
							}
						}
					}
					blDeleteUtf16Str((BLUtf16*)_s16);
				}
				else
				{
					BLBool _textchanged = FALSE;
					BLS32 _newbegin = _wid->uExtension.sText.nSelectBegin;
					BLS32 _newend = _wid->uExtension.sText.nSelectEnd;
					switch (HIGU16(_UParam))
					{
					case BL_KC_RETURN:
						if (!LOWU16(_UParam))
						{
							if (!_wid->uExtension.sText.bMultiline)
							{
							}
						}
						break;
					case BL_KC_DELETE:
						if (!LOWU16(_UParam))
						{
							BLUtf16* _s;
							if (_wid->uExtension.sText.nSelectBegin != _wid->uExtension.sText.nSelectEnd)
							{
								BLUtf16* _tmp = (BLUtf16*)blGenUtf16Str(_wid->uExtension.sText.pText);
								BLU32 _tmplen = blUtf16Length(_tmp);
								if (!_tmplen)
									break;
								BLS32 _realmbgn = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd;
								BLS32 _realmend = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectEnd : _wid->uExtension.sText.nSelectBegin;
								_s = (BLUtf16*)alloca((_tmplen - _realmend + _realmbgn + 1) * sizeof(BLUtf16));
								BLS32 _jdx = 0;
								for (; _jdx < _realmbgn; ++_jdx)
									_s[_jdx] = _tmp[_jdx];
								for (BLU32 _kdx = 0; _kdx < _tmplen - _realmend; ++_kdx, ++_jdx)
									_s[_jdx] = _tmp[_realmend + _kdx];
								_s[_tmplen - _realmend + _realmbgn] = 0;
								if (_wid->uExtension.sText.pText)
									free(_wid->uExtension.sText.pText);
								_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str(_s);
								_wid->uExtension.sText.nCaretPos = _realmbgn;
								blDeleteUtf16Str(_tmp);
								if (_wid->uExtension.sText.nCaretPos > (BLS32)_tmplen - _realmend + _realmbgn)
									_wid->uExtension.sText.nCaretPos = _tmplen - _realmend + _realmbgn;
							}
							else
							{
								BLUtf16* _tmp = (BLUtf16*)blGenUtf16Str(_wid->uExtension.sText.pText);
								BLU32 _tmplen = blUtf16Length(_tmp);
								if (!_tmplen || _tmplen == _wid->uExtension.sText.nCaretPos)
									break;
								_s = (BLUtf16*)alloca((_tmplen) * sizeof(BLUtf16));
								BLS32 _jdx = 0;
								for (; _jdx < _wid->uExtension.sText.nCaretPos; ++_jdx)
									_s[_jdx] = _tmp[_jdx];
								for (BLU32 _kdx = 0; _kdx < _tmplen - _wid->uExtension.sText.nCaretPos - 1; ++_kdx, ++_jdx)
									_s[_jdx] = _tmp[_wid->uExtension.sText.nCaretPos + 1 + _kdx];
								_s[_tmplen - 1] = 0;
								if (_wid->uExtension.sText.pText)
									free(_wid->uExtension.sText.pText);
								_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str(_s);
								blDeleteUtf16Str(_tmp);
								if (_wid->uExtension.sText.nCaretPos >(BLS32)_tmplen - 1)
									_wid->uExtension.sText.nCaretPos = _tmplen - 1;
							}
							_newbegin = 0;
							_newend = 0;
							_textchanged = TRUE;
						}
						break;
					case BL_KC_BACKSPACE:
						if (!LOWU16(_UParam))
						{
							BLUtf16* _s;
							if (_wid->uExtension.sText.nSelectBegin != _wid->uExtension.sText.nSelectEnd)
							{
								BLUtf16* _s16 = (BLUtf16*)blGenUtf16Str(_wid->uExtension.sText.pText);
								BLU32 _s16len = blUtf16Length(_s16);
								if (!_s16len)
									break;
								BLS32 _realmbgn = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd;
								BLS32 _realmend = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectEnd : _wid->uExtension.sText.nSelectBegin;
								_s = (BLUtf16*)alloca((_s16len - _realmend + _realmbgn + 1) * sizeof(BLUtf16));
								for (BLS32 _jdx = 0; _jdx < _realmbgn; ++_jdx)
									_s[_jdx] = _s16[_jdx];
								for (BLU32 _jdx = 0; _jdx < _s16len - _realmend; ++_jdx)
									_s[_realmbgn + _jdx] = _s16[_realmend + _jdx];
								_s[_s16len - _realmend + _realmbgn] = 0;
								_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str((const BLUtf16*)_s);
								_wid->uExtension.sText.nCaretPos = _realmbgn;
								blDeleteUtf16Str(_s16);
							}
							else
							{
								BLUtf16* _s16 = (BLUtf16*)blGenUtf16Str(_wid->uExtension.sText.pText);
								BLU32 _s16len = blUtf16Length(_s16);
								if (!_s16len)
									break;
								if (_wid->uExtension.sText.nCaretPos > 0)
								{
									_s = (BLUtf16*)alloca(_s16len * sizeof(BLUtf16));
									for (BLS32 _jdx = 0; _jdx < _wid->uExtension.sText.nCaretPos - 1; ++_jdx)
										_s[_jdx] = _s16[_jdx];
									for (BLU32 _jdx = 0; _jdx < _s16len - _wid->uExtension.sText.nCaretPos; ++_jdx)
										_s[_jdx + _wid->uExtension.sText.nCaretPos - 1] = _s16[_wid->uExtension.sText.nCaretPos + _jdx];
									_s[_s16len - 1] = 0;
								}
								else
								{
									_s = (BLUtf16*)alloca((_s16len - _wid->uExtension.sText.nCaretPos + 1) * sizeof(BLUtf16));
									for (BLU32 _jdx = 0; _jdx < _s16len - _wid->uExtension.sText.nCaretPos; ++_jdx)
										_s[_jdx] = _s16[_wid->uExtension.sText.nCaretPos + _jdx];
									_s[_s16len - _wid->uExtension.sText.nCaretPos] = 0;
								}
								_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str((const BLUtf16*)_s);
								--_wid->uExtension.sText.nCaretPos;
								blDeleteUtf16Str(_s16);
							}
							if (_wid->uExtension.sText.nCaretPos < 0)
								_wid->uExtension.sText.nCaretPos = 0;
							_newbegin = 0;
							_newend = 0;
							_textchanged = TRUE;
						}
						break;
					case BL_KC_LEFT:
						if (!LOWU16(_UParam))
						{
							_newbegin = 0;
							_newend = 0;
							const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
							if (_wid->uExtension.sText.nCaretPos > 0)
								_wid->uExtension.sText.nCaretPos--;
							blDeleteUtf16Str((BLUtf16*)_s16);
						}
						break;
					case BL_KC_RIGHT:
						if (!LOWU16(_UParam))
						{
							_newbegin = 0;
							_newend = 0;
							const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
							if (blUtf16Length(_s16) > (BLU32)_wid->uExtension.sText.nCaretPos)
								_wid->uExtension.sText.nCaretPos++;
							blDeleteUtf16Str((BLUtf16*)_s16);
						}
						break;
					case BL_KC_UP:
						if (!LOWU16(_UParam))
						{
							if (_wid->uExtension.sText.bMultiline)
							{
								BLS32 _lineno = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
								BLS32 _mb = (_wid->uExtension.sText.nSelectBegin == _wid->uExtension.sText.nSelectEnd) ? _wid->uExtension.sText.nCaretPos : (_wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd);
								if (_lineno > 0)
								{
									BLS32 _cp = _wid->uExtension.sText.nCaretPos - _wid->uExtension.sText.aSplitPositions[_lineno];
									BLU32 _bslen = blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno - 1]);
									if ((BLS32)_bslen < _cp)
										_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno - 1] + MAX_INTERNAL((BLU32)1, _bslen) - 1;
									else
										_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno - 1] + _cp;
								}
								_newbegin = 0;
								_newend = 0;
							}
						}
						break;
					case BL_KC_DOWN:
						if (!LOWU16(_UParam))
						{
							if (_wid->uExtension.sText.bMultiline)
							{
								BLS32 _lineno = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
								BLS32 _mb = (_wid->uExtension.sText.nSelectBegin == _wid->uExtension.sText.nSelectEnd) ? _wid->uExtension.sText.nCaretPos : (_wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd);
								if (_lineno < (BLS32)_wid->uExtension.sText.nSplitSize - 1)
								{
									BLS32 _cp = _wid->uExtension.sText.nCaretPos - _wid->uExtension.sText.aSplitPositions[_lineno];
									BLU32 _bslen = blUtf16Length(_wid->uExtension.sText.pSplitText[_lineno + 1]);
									if ((BLS32)_bslen < _cp)
										_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno + 1] + MAX_INTERNAL((BLU32)1, _bslen) - 1;
									else
										_wid->uExtension.sText.nCaretPos = _wid->uExtension.sText.aSplitPositions[_lineno + 1] + _cp;
								}
								_newbegin = 0;
								_newend = 0;
							}
						}
						break;
					case BL_KC_HOME:
						if (!LOWU16(_UParam))
						{
							BLS32 _p = 0;
							if (_wid->uExtension.sText.bMultiline)
							{
								_p = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
								_p = _wid->uExtension.sText.aSplitPositions[_p];
							}
							_newbegin = 0;
							_newend = 0;
							_wid->uExtension.sText.nCaretPos = _p;
						}
						break;
					case BL_KC_END:
						if (!LOWU16(_UParam))
						{
							const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
							BLS32 _p = blUtf16Length(_s16);
							if (_wid->uExtension.sText.bMultiline)
							{
								_p = _TextLine(_wid, _wid->uExtension.sText.nCaretPos);
								_p = _wid->uExtension.sText.aSplitPositions[_p] + (BLS32)blUtf16Length(_wid->uExtension.sText.pSplitText[_p]);
								if (_p > 0 && (_s16[_p - 1] == L'\r' || _s16[_p - 1] == L'\n'))
									_p -= 1;
							}
							_newbegin = 0;
							_newend = 0;
							_wid->uExtension.sText.nCaretPos = _p;
							blDeleteUtf16Str((BLUtf16*)_s16);
						}
						break;
					case BL_KC_SELECT:
						if (LOWU16(_UParam))
						{
							const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
							_newbegin = 0;
							_newend = blUtf16Length(_s16);
							blDeleteUtf16Str((BLUtf16*)_s16);
						}
						break;
					case BL_KC_COPY:
						if (LOWU16(_UParam))
						{
							if (!_wid->uExtension.sText.bPassword && (_wid->uExtension.sText.nSelectBegin != _wid->uExtension.sText.nSelectEnd))
							{
								BLS32 _realmbgn = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd;
								BLS32 _realmend = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectEnd : _wid->uExtension.sText.nSelectBegin;
								const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
								BLUtf16* _s = (BLUtf16*)alloca((_realmend - _realmbgn + 1) * sizeof(BLUtf16));
								for (BLS32 _jdx = 0; _jdx < _realmend - _realmbgn; ++_jdx)
									_s[_jdx] = _s16[_jdx + _realmbgn];
								_s[_realmend - _realmbgn] = 0;
								const BLUtf8* _clipboard = blGenUtf8Str((const BLUtf16*)_s);
								blSetClipboard(_clipboard);
								blDeleteUtf8Str((BLUtf8*)_clipboard);
								blDeleteUtf16Str((BLUtf16*)_s16);
							}
						}
						break;
					case BL_KC_CUT:
						if (LOWU16(_UParam))
						{
							if (!_wid->uExtension.sText.bPassword && (_wid->uExtension.sText.nSelectBegin != _wid->uExtension.sText.nSelectEnd))
							{
								BLS32 _realmbgn = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd;
								BLS32 _realmend = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectEnd : _wid->uExtension.sText.nSelectBegin;
								const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
								BLU32 _s16len = blUtf16Length(_s16);
								BLUtf16* _s = (BLUtf16*)alloca((_realmend - _realmbgn + 1) * sizeof(BLUtf16));
								BLS32 _jdx = 0;
								for (_jdx = 0; _jdx < _realmend - _realmbgn; ++_jdx)
									_s[_jdx] = _s16[_jdx + _realmbgn];
								_s[_realmend - _realmbgn] = 0;
								const BLUtf8* _clipboard = blGenUtf8Str((const BLUtf16*)_s);
								blSetClipboard(_clipboard);
								blDeleteUtf8Str((BLUtf8*)_clipboard);
								BLUtf16* _sn = (BLUtf16*)alloca((_s16len - _realmend + _realmbgn + 1) * sizeof(BLUtf16));
								_jdx = 0;
								for (_jdx = 0; _jdx < _realmbgn; ++_jdx)
									_sn[_jdx] = _s16[_jdx];
								for (BLU32 _kdx = 0; _kdx < _s16len - _realmend; ++_kdx, ++_jdx)
									_sn[_jdx] = _s16[_kdx + _realmend];
								_sn[_s16len - _realmend + _realmbgn] = 0;
								if (_wid->uExtension.sText.pText)
									free(_wid->uExtension.sText.pText);
								_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str(_sn);
								_wid->uExtension.sText.nCaretPos = _realmbgn;
								_newbegin = 0;
								_newend = 0;
								_textchanged = TRUE;
								blDeleteUtf16Str((BLUtf16*)_s16);
							}
						}
						break;
					case BL_KC_PASTE:
						if (LOWU16(_UParam))
						{
							BLS32 _realmbgn = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectBegin : _wid->uExtension.sText.nSelectEnd;
							BLS32 _realmend = _wid->uExtension.sText.nSelectBegin < _wid->uExtension.sText.nSelectEnd ? _wid->uExtension.sText.nSelectEnd : _wid->uExtension.sText.nSelectBegin;
							const BLUtf8* _clipboard = blGetClipboard();
							if (_clipboard)
							{
								const BLUtf16* _clipboard16 = blGenUtf16Str(_clipboard);
								const BLUtf16* _s16 = blGenUtf16Str(_wid->uExtension.sText.pText);
								if (_wid->uExtension.sText.nSelectBegin == _wid->uExtension.sText.nSelectEnd)
								{
									BLU32 _c16len = blUtf16Length(_clipboard16);
									BLU32 _s16len = blUtf16Length(_s16);
									BLUtf16* _s = (BLUtf16*)alloca((_c16len + _s16len + 1) * sizeof(BLUtf16));
									BLS32 _jdx = 0;
									for (_jdx = 0; _jdx < _wid->uExtension.sText.nCaretPos; ++_jdx)
										_s[_jdx] = _s16[_jdx];
									for (BLU32 _kdx = 0; _kdx < _c16len; ++_kdx, ++_jdx)
										_s[_jdx] = _clipboard16[_kdx];
									for (BLU32 _kdx = 0; _kdx < _s16len - _wid->uExtension.sText.nCaretPos; ++_kdx, ++_jdx)
										_s[_jdx] = _s16[_wid->uExtension.sText.nCaretPos + _kdx];
									_s[_c16len + _s16len] = 0;
									if (blUtf16Length(_s) <= _wid->uExtension.sText.nMaxLength)
									{
										if (_wid->uExtension.sText.pText)
											free(_wid->uExtension.sText.pText);
										_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str((const BLUtf16*)_s);
										_wid->uExtension.sText.nCaretPos += _c16len;
									}
								}
								else
								{
									BLU32 _c16len = blUtf16Length(_clipboard16);
									BLU32 _s16len = blUtf16Length(_s16);
									BLUtf16* _s = (BLUtf16*)alloca((_s16len + _c16len + _realmbgn - _realmend + 1) * sizeof(BLUtf16));
									BLS32 _jdx = 0;
									for (_jdx = 0; _jdx < _realmbgn; ++_jdx)
										_s[_jdx] = _s16[_jdx];
									for (BLU32 _kdx = 0; _kdx < _c16len; ++_kdx, ++_jdx)
										_s[_jdx] = _clipboard16[_kdx];
									for (BLU32 _kdx = 0; _kdx < _s16len - _realmend; ++_kdx, ++_jdx)
										_s[_jdx] = _s16[_realmend + _kdx];
									_s[_s16len + _c16len + _realmbgn - _realmend] = 0;
									if (blUtf16Length(_s) <= _wid->uExtension.sText.nMaxLength)
									{
										if (_wid->uExtension.sText.pText)
											free(_wid->uExtension.sText.pText);
										_wid->uExtension.sText.pText = (BLUtf8*)blGenUtf8Str((const BLUtf16*)_s);
										_wid->uExtension.sText.nCaretPos = _realmbgn + _c16len;
									}
								}
								blDeleteUtf16Str((BLUtf16*)_s16);
								blDeleteUtf16Str((BLUtf16*)_clipboard16);
							}
							_newbegin = 0;
							_newend = 0;
							_textchanged = TRUE;
						}
						break;
					default: break;
					}
					_wid->uExtension.sText.nSelectBegin = _newbegin;
					_wid->uExtension.sText.nSelectEnd = _newend;
					if (_textchanged)
						_TextSplit(_wid, _ft, _flag);
					_TextScroll(_wid);
				}
			}
			_PrUIMem->bDirty = TRUE;
		}
	}
}
BLVoid
_UIInit()
{
    _PrUIMem = (_BLUIMember*)malloc(sizeof(_BLUIMember));
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
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	_PrUIMem->nGlyphGeo = blGenGeometryBuffer(blHashUtf8((const BLUtf8*)"#@glyphgeo#@"), BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, NULL, sizeof(float) * 32, NULL, 0, BL_IF_INVALID);
	BLU32 _blankdata[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
	_PrUIMem->nBlankTex = blGenTexture(blHashUtf8((const BLUtf8*)"#@blanktex#@"), BL_TT_2D, BL_TF_RGBA8, FALSE, TRUE, FALSE, 1, 1, 2, 2, 1, (BLU8*)_blankdata);
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
	blDeleteGeometryBuffer(_PrUIMem->nGlyphGeo);
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
					free(_iter2);
				}
			}
			blDeleteDict(_iter->pGlyphAtlas);
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
		if (INVALID_GUID == _stream)
		{
			blDebugOutput("UI workspace open fail");
			return;
		}
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
		if (INVALID_GUID == _stream)
		{
			blDebugOutput("UI workspace open fail");
			return;
		}
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
	if (_layout == INVALID_GUID)
	{
		blDebugOutput("load ui layout failed>%s", _Filename);
		return;
	}
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
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_CHECK);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUICheckFont(_widguid, _fontsrc, _fontsizevar, _fontoutlinevar, _fontboldvar, _fontshadowvar, _fontitalicsvar);
			blUICheckFlip(_widguid, _flipxvar, _flipyvar);
			blUICheckText(_widguid, (const BLUtf8*)_text, _textcolorvar, _txtha, _txtva);
			blUICheckPixmap(_widguid, _pixmap);
			blUICheckStencilMap(_widguid, _stencilmap);
			blUICheckCommonMap(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
			blUICheckCheckedMap(_widguid, _checkedmap, _checkedtexcoordvar[0], _checkedtexcoordvar[1], _checkedtexcoordvar[2], _checkedtexcoordvar[3]);
			blUICheckDisableMap(_widguid, _disablemap, _disabletexcoordvar[0], _disabletexcoordvar[1], _disabletexcoordvar[2], _disabletexcoordvar[3]);
			blUICheckEnable(_widguid, _enablevar);
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
			const BLAnsi* _phcolor = ezxml_attr(_element, "PlaceholderColor");
			BLU32 _phcolorvar = (BLU32)strtoul(_phcolor, NULL, 10);			
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
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_TEXT);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUITextFont(_widguid, _fontsrc, _fontsizevar, _fontoutlinevar, _fontboldvar, _fontshadowvar, _fontitalicsvar);
			blUITextFlip(_widguid, _flipxvar, _flipyvar);
			blUITextPlaceholder(_widguid, (const BLUtf8*)_placeholder, _textcolorvar, _phcolorvar, _txtha, _txtva);
			blUITextPixmap(_widguid, _pixmap);
			blUITextStencilMap(_widguid, _stencilmap);
			blUITextCommonMap(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
			blUITextTypography(_widguid, _autoscrollvar, _multilinevar);
			blUITextPassword(_widguid, _passwordvar);
			blUITextNumeric(_widguid, _numericvar, _numericrangevar[0], _numericrangevar[1]);
			blUITextPadding(_widguid, _paddingvar[0], _paddingvar[1]);
			blUITextMaxLength(_widguid, _maxlengthvar);
			blUITextEnable(_widguid, _enablevar);
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
            BLU32 _bordevarr[2];
            _tmp = strtok((BLAnsi*)_border, ",");
            _bordevarr[0] = (BLU32)strtoul(_tmp, NULL, 10);
            _bordevarr[1] = (BLU32)strtoul(strtok(NULL, ","), NULL, 10);
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
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_PROGRESS);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUIProgressFont(_widguid, _fontsrc, _fontsizevar, _fontoutlinevar, _fontboldvar, _fontshadowvar, _fontitalicsvar);
			blUIProgressFlip(_widguid, _flipxvar, _flipyvar);
			blUIProgressText(_widguid, (const BLUtf8*)_text, _textcolorvar);
			blUIProgressPixmap(_widguid, _pixmap);
			blUIProgressStencilMap(_widguid, _stencilmap);
			blUIProgressCommonMap(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
			blUIProgressFillMap(_widguid, _fillmap);
			blUIProgressPercent(_widguid, _percentvar);
			blUIProgressBorder(_widguid, _bordevarr[0], _bordevarr[1]);
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
            BLU32 _rangevar[2];
            _tmp = strtok((BLAnsi*)_range, ",");
            _rangevar[0] = (BLU32)strtoul(_tmp, NULL, 10);
            _rangevar[1] = (BLU32)strtoul(strtok(NULL, ","), NULL, 10);
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
            const BLAnsi* _slidercommonmap = ezxml_attr(_element, "SliderCommonMap");
            const BLAnsi* _slidersisablemap = ezxml_attr(_element, "SliderDisableMap");
            const BLAnsi* _stencilmap = ezxml_attr(_element, "StencilMap");
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_SLIDER);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUISliderFlip(_widguid, _flipxvar, _flipyvar);
			blUISliderPixmap(_widguid, _pixmap);
			blUISliderStencilMap(_widguid, _stencilmap);
			blUISliderCommonMap(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
			blUISliderDisableMap(_widguid, _disablemap, _disabletexcoordvar[0], _disabletexcoordvar[1], _disabletexcoordvar[2], _disabletexcoordvar[3]);
			blUISliderSliderCommonMap(_widguid, _slidercommonmap);
			blUISliderSliderDisableMap(_widguid, _slidersisablemap);
			blUISliderHorizontal(_widguid, _orientationvar);
			blUISliderSliderStep(_widguid, _sliderstepvar);
			blUISliderSliderPos(_widguid, _sliderpositionvar);
			blUISliderSliderSize(_widguid, _slidersizevar[0], _slidersizevar[1]);
			blUISliderSliderRange(_widguid, _rangevar[0], _rangevar[1]);
			blUISliderEnable(_widguid, _enablevar);
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
			const BLAnsi* _rowheight = ezxml_attr(_element, "RowHeight");
			BLU32 _rowheightvar = (BLU32)strtoul(_rowheight, NULL, 10);
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_TABLE);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUITablePixmap(_widguid, _pixmap);
			blUITableStencilMap(_widguid, _stencilmap);
			blUITableCommonMap(_widguid, _commonmap, _commontexvar[0], _commontexvar[1], _commontexvar[2], _commontexvar[3]);
			blUITableOddItemMap(_widguid, _odditemmap);
			blUITableEvenItemMap(_widguid, _evenitemmap);
			blUITableFont(_widguid, _fontsrc, _fontsizevar, _fontoutlinevar, _fontboldvar, _fontshadowvar, _fontitalicsvar);
			blUITableFlip(_widguid, _flipxvar, _flipyvar);
			blUITableRowHeight(_widguid, _rowheightvar);
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
			BLGuid _widguid = blGenUI(_name, _geovar[0], _geovar[1], _geovar[2], _geovar[3], _QueryWidget(_PrUIMem->pRoot, _parentvar, TRUE)->nID, BL_UT_DIAL);
			blUIReferencePoint(_widguid, _ha, _va);
			blUISizePolicy(_widguid, _policyvar);
			blUISizeLimit(_widguid, (BLU32)_maxsizevar[0], (BLU32)_maxsizevar[1], (BLU32)_minsizevar[0], (BLU32)_minsizevar[1]);
			blUIScissor(_widguid, _clipedvar, _absvar);
			blUITooltip(_widguid, (const BLUtf8*)_tooltip);
			blUIPenetration(_widguid, _penetrationvar);
			blUIDialPixmap(_widguid, _pixmap);
			blUIDialStencilMap(_widguid, _stencilmap);
			blUIDialCommonMap(_widguid, _commonmap);
			blUIDialClockwise(_widguid, _clockwisevar);
			blUIDialAngleCut(_widguid, _anglecutvar);
			blUIDialAngleRange(_widguid, _startanglevar, _endanglevar);
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
			memset(_widget->uExtension.sCheck.aCheckedMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sCheck.aDisableMap, 0, sizeof(BLAnsi) * 128);
			memset(_widget->uExtension.sCheck.aStencilMap, 0, sizeof(BLAnsi) * 128);
			_widget->uExtension.sCheck.eTxtAlignmentH = BL_UA_HCENTER;
			_widget->uExtension.sCheck.eTxtAlignmentV = BL_UA_VCENTER;
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
			memset(_widget->uExtension.sSlider.aDisableMap, 0, sizeof(BLAnsi) * 128);
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
			_widget->uExtension.sSlider.bDragging = FALSE;
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
			memset(_widget->uExtension.sText.pSplitText, 0, sizeof(BLUtf16*) * 2048);
			_widget->uExtension.sText.nMaxLength = 999999;
			_widget->uExtension.sText.nMinValue = 0;
			_widget->uExtension.sText.nMaxValue = 999999;
			_widget->uExtension.sText.bSelecting = FALSE;
			_widget->uExtension.sText.bAutoscroll = TRUE;
			_widget->uExtension.sText.bMultiline = FALSE;
			_widget->uExtension.sText.bPassword = FALSE;
			_widget->uExtension.sText.bNumeric = FALSE;
			_widget->uExtension.sText.eTxtAlignmentH = BL_UA_HCENTER;
			_widget->uExtension.sText.eTxtAlignmentV = BL_UA_VCENTER;
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
			_widget->uExtension.sText.nSplitSize = 0;
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
			_widget->uExtension.sProgress.nBorderX = 0;
			_widget->uExtension.sProgress.nBorderY = 0;
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
			_widget->uExtension.sDial.fAngle = 0.f;
			_widget->uExtension.sDial.bDragging = FALSE;
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
			_widget->uExtension.sTable.nFontHeight = 0;
			_widget->uExtension.sTable.bOutline = FALSE;
			_widget->uExtension.sTable.bBold = FALSE;
			_widget->uExtension.sTable.bShadow = FALSE;
			_widget->uExtension.sTable.bItalics = FALSE;
			_widget->uExtension.sTable.bFlipX = FALSE;
            _widget->uExtension.sTable.bFlipY = FALSE;
			_widget->uExtension.sTable.pCellText = NULL;
			_widget->uExtension.sTable.pCellBroken = NULL;
			_widget->uExtension.sTable.pCellColor = NULL;
			_widget->uExtension.sTable.nColumnNum = 0;
			_widget->uExtension.sTable.nCellNum = 0;
			_widget->uExtension.sTable.nScroll = 0;
			_widget->uExtension.sTable.nRowHeight = 14;
			_widget->uExtension.sTable.nTotalHeight = 0;
			_widget->uExtension.sTable.nTotalWidth = 0;
			_widget->uExtension.sTable.nSelectedCell = -1;
			_widget->uExtension.sTable.bSelecting = FALSE;
			_widget->uExtension.sTable.bDragging = FALSE;
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
	BLAnsi _texfile[260] = { 0 };
	sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
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
	_widget->uExtension.sCheck.sCommonTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sCheck.sCommonTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sCheck.sCommonTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sCheck.sCommonTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sCheck.sCommonTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sCheck.sCommonTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sCheck.sCommonTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sCheck.sCommonTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUICheckCheckedMap(IN BLGuid _ID, IN BLAnsi* _CheckedMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aCheckedMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sCheck.aCheckedMap, _CheckedMap);
	_widget->uExtension.sCheck.sCheckedTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sCheck.sCheckedTex9.sRB.fY = _CenterY + _Height * 0.5f;
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
	_widget->uExtension.sCheck.sDisableTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sCheck.sDisableTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sCheck.sDisableTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sCheck.sDisableTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sCheck.sDisableTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sCheck.sDisableTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sCheck.sDisableTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sCheck.sDisableTex9.sRB.fY = _CenterY + _Height * 0.5f;
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
	if (_widget->uExtension.sCheck.pText)
		free(_widget->uExtension.sCheck.pText);
	_widget->uExtension.sCheck.pText = (BLUtf8*)malloc(_strlen);
	memset(_widget->uExtension.sCheck.pText, 0, _strlen);
	strcpy((BLAnsi*)_widget->uExtension.sCheck.pText, (const BLAnsi*)_Text);
	_widget->uExtension.sCheck.nTxtColor = _TxtColor;
	_widget->uExtension.sCheck.eTxtAlignmentH = _TxtAlignmentH;
	_widget->uExtension.sCheck.eTxtAlignmentV = _TxtAlignmentV;
}
BLVoid
blUICheckFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_CHECK)
		return;
	memset(_widget->uExtension.sCheck.aFontSource, 0, sizeof(_widget->uExtension.sCheck.aFontSource));
	strcpy(_widget->uExtension.sCheck.aFontSource, _Font);
	_widget->uExtension.sCheck.nFontHeight = _FontHeight;
	if (_Outline)
	{
		_widget->uExtension.sCheck.bOutline = _Outline;
		_widget->uExtension.sCheck.bBold = FALSE;
		_widget->uExtension.sCheck.bShadow = FALSE;
		_widget->uExtension.sCheck.bItalics = _Italics;
	}
	else if (_Bold)
	{
		_widget->uExtension.sCheck.bOutline = FALSE;
		_widget->uExtension.sCheck.bBold = _Bold;
		_widget->uExtension.sCheck.bShadow = FALSE;
		_widget->uExtension.sCheck.bItalics = _Italics;
	}
	else if (_Shadow)
	{
		_widget->uExtension.sCheck.bOutline = FALSE;
		_widget->uExtension.sCheck.bBold = FALSE;
		_widget->uExtension.sCheck.bShadow = _Shadow;
		_widget->uExtension.sCheck.bItalics = _Italics;
	}
	else
	{
		_widget->uExtension.sCheck.bOutline = FALSE;
		_widget->uExtension.sCheck.bBold = FALSE;
		_widget->uExtension.sCheck.bShadow = FALSE;
		_widget->uExtension.sCheck.bItalics = _Italics;
	}
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
BLBool 
blUIGetCheckState(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return FALSE;
	if (_widget->eType != BL_UT_CHECK)
		return FALSE;
	return (_widget->uExtension.sCheck.nState == 2) ? TRUE : FALSE;
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
	BLAnsi _texfile[260] = { 0 };
	sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
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
	_widget->uExtension.sText.sCommonTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sText.sCommonTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sText.sCommonTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sText.sCommonTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sText.sCommonTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sText.sCommonTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sText.sCommonTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sText.sCommonTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextTypography(IN BLGuid _ID, IN BLBool _Autoscroll, IN BLBool _Multiline)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	_widget->uExtension.sText.bAutoscroll = _Autoscroll;
	_widget->uExtension.sText.bMultiline = _Multiline;
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_widget->uExtension.sText.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return;
	BLU16 _flag = 0;
	if (_widget->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_widget->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_widget->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_widget->uExtension.sText.bItalics)
		_flag |= 0xF000;
	_TextSplit(_widget, _ft, _flag);
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
	if (_widget->uExtension.sText.pText)
	{
		BLS32 _value = strtol((const BLAnsi*)_widget->uExtension.sText.pText, NULL, 10);
		_value = (BLS32)blScalarClamp((BLF32)_value, (BLF32)_MinValue, (BLF32)_MaxValue);
		BLAnsi _buf[32] = { 0 };
		sprintf(_buf, "%d", _value);
		_widget->uExtension.sText.pText = (BLUtf8*)realloc(_widget->uExtension.sText.pText, strlen(_buf) + 1);
		memset(_widget->uExtension.sText.pText, 0, strlen(_buf) + 1);
		strcpy(_widget->uExtension.sText.pText, _buf);
	}
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
blUITextPlaceholder(IN BLGuid _ID, IN BLUtf8*	_Placeholder, IN BLU32 _TxtColor, IN BLU32 _PlaceholderColor, IN BLEnum _TxtAlignmentH, IN BLEnum _TxtAlignmentV)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	BLU32 _strlen = blUtf8Length(_Placeholder) + 1;
	if (_widget->uExtension.sText.pPlaceholder)
		free(_widget->uExtension.sText.pPlaceholder);
	_widget->uExtension.sText.pPlaceholder = (BLUtf8*)malloc(_strlen);
	memset(_widget->uExtension.sText.pPlaceholder, 0, _strlen);
	strcpy((BLAnsi*)_widget->uExtension.sText.pPlaceholder, (const BLAnsi*)_Placeholder);
	_widget->uExtension.sText.nTxtColor = _TxtColor;
	_widget->uExtension.sText.nPlaceholderColor = _PlaceholderColor;
	_widget->uExtension.sText.eTxtAlignmentH = _TxtAlignmentH;
	_widget->uExtension.sText.eTxtAlignmentV = _TxtAlignmentV;
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITextFont(IN BLGuid _ID, IN BLAnsi* _Font, IN BLU32 _FontHeight, IN BLBool _Outline, IN BLBool _Bold, IN BLBool _Shadow, IN BLBool _Italics)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TEXT)
		return;
	memset(_widget->uExtension.sText.aFontSource, 0, sizeof(_widget->uExtension.sText.aFontSource));
	strcpy(_widget->uExtension.sText.aFontSource, _Font);
	_widget->uExtension.sText.nFontHeight = _FontHeight;
	if (_Outline)
	{
		_widget->uExtension.sText.bOutline = _Outline;
		_widget->uExtension.sText.bBold = FALSE;
		_widget->uExtension.sText.bShadow = FALSE;
		_widget->uExtension.sText.bItalics = _Italics;
	}
	else if (_Bold)
	{
		_widget->uExtension.sText.bOutline = FALSE;
		_widget->uExtension.sText.bBold = _Bold;
		_widget->uExtension.sText.bShadow = FALSE;
		_widget->uExtension.sText.bItalics = _Italics;
	}
	else if (_Shadow)
	{
		_widget->uExtension.sText.bOutline = FALSE;
		_widget->uExtension.sText.bBold = FALSE;
		_widget->uExtension.sText.bShadow = _Shadow;
		_widget->uExtension.sText.bItalics = _Italics;
	}
	else
	{
		_widget->uExtension.sText.bOutline = FALSE;
		_widget->uExtension.sText.bBold = FALSE;
		_widget->uExtension.sText.bShadow = FALSE;
		_widget->uExtension.sText.bItalics = _Italics;
	}
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
blUIProgressPixmap(IN BLGuid _ID, IN BLAnsi* _Pixmap)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	memset(_widget->uExtension.sProgress.aPixmap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sProgress.aPixmap, _Pixmap);
	BLAnsi _texfile[260] = { 0 };
	sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
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
	_widget->uExtension.sProgress.sCommonTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sProgress.sCommonTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sProgress.sCommonTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sProgress.sCommonTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sProgress.sCommonTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sProgress.sCommonTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sProgress.sCommonTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sProgress.sCommonTex9.sRB.fY = _CenterY + _Height * 0.5f;
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
	_widget->uExtension.sProgress.nPercent = (BLU32)blScalarClamp((BLF32)_Percent, 0.f, 100.f);
	_PrUIMem->bDirty = TRUE;
}
BLVoid 
blUIProgressBorder(IN BLGuid _ID, IN BLU32 _BorderX, IN BLU32 _BorderY)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_PROGRESS)
		return;
	_widget->uExtension.sProgress.nBorderX = _BorderX;
	_widget->uExtension.sProgress.nBorderY = _BorderY;
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
	if (_widget->uExtension.sProgress.pText)
		free(_widget->uExtension.sProgress.pText);
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
	memset(_widget->uExtension.sProgress.aFontSource, 0, sizeof(_widget->uExtension.sProgress.aFontSource));
	strcpy(_widget->uExtension.sProgress.aFontSource, _Font);
	_widget->uExtension.sProgress.nFontHeight = _FontHeight;
	if (_Outline)
	{
		_widget->uExtension.sProgress.bOutline = _Outline;
		_widget->uExtension.sProgress.bBold = FALSE;
		_widget->uExtension.sProgress.bShadow = FALSE;
		_widget->uExtension.sProgress.bItalics = _Italics;
	}
	else if (_Bold)
	{
		_widget->uExtension.sProgress.bOutline = FALSE;
		_widget->uExtension.sProgress.bBold = _Bold;
		_widget->uExtension.sProgress.bShadow = FALSE;
		_widget->uExtension.sProgress.bItalics = _Italics;
	}
	else if (_Shadow)
	{
		_widget->uExtension.sProgress.bOutline = FALSE;
		_widget->uExtension.sProgress.bBold = FALSE;
		_widget->uExtension.sProgress.bShadow = _Shadow;
		_widget->uExtension.sProgress.bItalics = _Italics;
	}
	else
	{
		_widget->uExtension.sProgress.bOutline = FALSE;
		_widget->uExtension.sProgress.bBold = FALSE;
		_widget->uExtension.sProgress.bShadow = FALSE;
		_widget->uExtension.sProgress.bItalics = _Italics;
	}
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
BLF32 
blUIGetProgressPercent(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return 0.f;
	if (_widget->eType != BL_UT_PROGRESS)
		return 0.f;
	return (BLF32)_widget->uExtension.sProgress.nPercent / 100.f;
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
	BLAnsi _texfile[260] = { 0 };
	sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
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
	_widget->uExtension.sSlider.sCommonTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sSlider.sCommonTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sSlider.sCommonTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sSlider.sCommonTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sSlider.sCommonTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sSlider.sCommonTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sSlider.sCommonTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sSlider.sCommonTex9.sRB.fY = _CenterY + _Height * 0.5f;
	_PrUIMem->bDirty = TRUE;
}
BLVoid 
blUISliderDisableMap(IN BLGuid _ID, IN BLAnsi* _DisableMap, IN BLF32 _CenterX, IN BLF32 _CenterY, IN BLF32 _Width, IN BLF32 _Height)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	memset(_widget->uExtension.sSlider.aDisableMap, 0, sizeof(BLAnsi) * 128);
	strcpy(_widget->uExtension.sSlider.aDisableMap, _DisableMap);
	_widget->uExtension.sSlider.sDisableTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sSlider.sDisableTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sSlider.sDisableTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sSlider.sDisableTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sSlider.sDisableTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sSlider.sDisableTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sSlider.sDisableTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sSlider.sDisableTex9.sRB.fY = _CenterY + _Height * 0.5f;
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
blUISliderSliderStep(IN BLGuid _ID, IN BLU32 _Step)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.nStep = MIN_INTERNAL(_Step, (BLU32)(_widget->uExtension.sSlider.nMaxValue - _widget->uExtension.sSlider.nMinValue));
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUISliderSliderPos(IN BLGuid _ID, IN BLS32 _Pos)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_SLIDER)
		return;
	_widget->uExtension.sSlider.nSliderPosition = (BLS32)blScalarClamp((BLF32)_Pos, (BLF32)_widget->uExtension.sSlider.nMinValue, (BLF32)_widget->uExtension.sSlider.nMaxValue);
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
	_widget->uExtension.sSlider.nSliderPosition = (BLS32)blScalarClamp((BLF32)_widget->uExtension.sSlider.nSliderPosition, (BLF32)_widget->uExtension.sSlider.nMinValue, (BLF32)_widget->uExtension.sSlider.nMaxValue);
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
BLU32
blUIGetSliderPos(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return 0;
	if (_widget->eType != BL_UT_SLIDER)
		return 0;
	return _widget->uExtension.sSlider.nSliderPosition;
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
	BLAnsi _texfile[260] = { 0 };
	sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
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
	_widget->uExtension.sTable.sCommonTex.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sTable.sCommonTex.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sTable.sCommonTex.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sTable.sCommonTex.sRB.fY = _CenterY + _Height * 0.5f;
	_widget->uExtension.sTable.sCommonTex9.sLT.fX = _CenterX - _Width * 0.5f;
	_widget->uExtension.sTable.sCommonTex9.sLT.fY = _CenterY - _Height * 0.5f;
	_widget->uExtension.sTable.sCommonTex9.sRB.fX = _CenterX + _Width * 0.5f;
	_widget->uExtension.sTable.sCommonTex9.sRB.fY = _CenterY + _Height * 0.5f;
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
blUITableMaking(IN BLGuid _ID, IN BLU32 _Row, IN BLU32 _Column)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	if (!_Column || !_Row)
		return;
	for (BLU32 _idx = 0; _idx < _widget->uExtension.sTable.nCellNum; ++_idx)
	{
		if (_widget->uExtension.sTable.pCellText[_idx])
			free(_widget->uExtension.sTable.pCellText[_idx]);
		if (_widget->uExtension.sTable.pCellBroken[_idx])
			free(_widget->uExtension.sTable.pCellBroken[_idx]);
	}
	_widget->uExtension.sTable.nColumnNum = _Column;
	_widget->uExtension.sTable.nCellNum = _Column * _Row;
	_widget->uExtension.sTable.pCellText = (BLUtf8**)realloc(_widget->uExtension.sTable.pCellText, _widget->uExtension.sTable.nCellNum * sizeof(BLUtf8*));
	_widget->uExtension.sTable.pCellBroken = (BLUtf16**)realloc(_widget->uExtension.sTable.pCellBroken, _widget->uExtension.sTable.nCellNum * sizeof(BLUtf16*));
	_widget->uExtension.sTable.pCellColor = (BLU32*)realloc(_widget->uExtension.sTable.pCellColor, _widget->uExtension.sTable.nCellNum * sizeof(BLU32));
	for (BLU32 _idx = 0; _idx < _widget->uExtension.sTable.nCellNum; ++_idx)
	{
		_widget->uExtension.sTable.pCellText[_idx] = NULL;
		_widget->uExtension.sTable.pCellBroken[_idx] = NULL;
		_widget->uExtension.sTable.pCellColor[_idx] = 0xFFFFFFFF;
	}
	for (BLU32 _idx = 0; _idx < _widget->uExtension.sTable.nColumnNum; ++_idx)
		_widget->uExtension.sTable.aColumnWidth[_idx] = (BLS32)(_widget->sDimension.fX / _Column);
	_TableMake(_widget);
	_PrUIMem->bDirty = TRUE;
}
BLVoid 
blUITableRowHeight(IN BLGuid _ID, IN BLU32 _RowHeight)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	_widget->uExtension.sTable.nRowHeight = _RowHeight;
	_TableMake(_widget);
	_PrUIMem->bDirty = TRUE;
}
BLVoid 
blUITableColumnWidth(IN BLGuid _ID, IN BLU32 _Column, IN BLU32 _Width)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	if (_Column >= _widget->uExtension.sTable.nColumnNum)
		return;
	if (_widget->uExtension.sTable.nColumnNum == 1)
		return;
	if (_Column == _widget->uExtension.sTable.nColumnNum - 1)
	{
		BLS32 _total = _widget->uExtension.sTable.aColumnWidth[_Column] + _widget->uExtension.sTable.aColumnWidth[_Column - 1];
		_widget->uExtension.sTable.aColumnWidth[_Column] = _Width;
		_widget->uExtension.sTable.aColumnWidth[_Column - 1] = _total - _Width;
	}
	else
	{
		BLS32 _total = _widget->uExtension.sTable.aColumnWidth[_Column] + _widget->uExtension.sTable.aColumnWidth[_Column + 1];
		_widget->uExtension.sTable.aColumnWidth[_Column] = _Width;
		_widget->uExtension.sTable.aColumnWidth[_Column + 1] = _total - _Width;
	}
	BLU32 _rownum = (BLU32)roundf((BLF32)_widget->uExtension.sTable.nCellNum / _widget->uExtension.sTable.nColumnNum);
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_widget->uExtension.sTable.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	if (!_ft)
		return;
	BLU16 _flag = 0;
	if (_widget->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_widget->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_widget->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_widget->uExtension.sText.bItalics)
		_flag |= 0xF000;
	for (BLU32 _idx = 0; _idx < _rownum; ++_idx)
	{
		BLU32 _cellidx = _Column + _idx * _widget->uExtension.sTable.nColumnNum;
		if (_cellidx < _widget->uExtension.sTable.nCellNum)
			_TableSplit(_widget, _ft, _flag, _widget->uExtension.sTable.pCellText[_cellidx], &_widget->uExtension.sTable.pCellBroken[_cellidx], _Width);
	}
	_TableMake(_widget);
	_PrUIMem->bDirty = TRUE;
}
BLVoid
blUITableCell(IN BLGuid _ID, IN BLUtf8* _Text, IN BLU32 _TxtColor, IN BLU32 _Row, IN BLU32 _Column)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_TABLE)
		return;
	if (_Column >= 32)
		return;
	_BLFont* _ft = NULL;
	{
		FOREACH_ARRAY(_BLFont*, _iter, _PrUIMem->pFonts)
		{
			if (_iter->nHashName == blHashUtf8((const BLUtf8*)_widget->uExtension.sTable.aFontSource))
			{
				_ft = _iter;
				break;
			}
		}
	}
	BLU16 _flag = 0;
	if (_widget->uExtension.sText.bOutline)
		_flag |= 0x000F;
	if (_widget->uExtension.sText.bBold)
		_flag |= 0x00F0;
	if (_widget->uExtension.sText.bShadow)
		_flag |= 0x0F00;
	if (_widget->uExtension.sText.bItalics)
		_flag |= 0xF000;
	BLU32 _cellidx = _Column + _Row * _widget->uExtension.sTable.nColumnNum;
	if (_cellidx < _widget->uExtension.sTable.nCellNum)
	{
		if (_widget->uExtension.sTable.pCellText[_cellidx])
			free(_widget->uExtension.sTable.pCellText[_cellidx]);
		BLU32 _txtlen = blUtf8Length(_Text);
		_widget->uExtension.sTable.pCellText[_cellidx] = (BLUtf8*)malloc((_txtlen + 1) * sizeof(BLUtf8*));
		strcpy((BLAnsi*)_widget->uExtension.sTable.pCellText[_cellidx], (const BLAnsi*)_Text);
		_widget->uExtension.sTable.pCellText[_cellidx][_txtlen] = 0;
		_widget->uExtension.sTable.pCellBroken[_cellidx] = NULL;
		_widget->uExtension.sTable.pCellColor[_cellidx] = _TxtColor;
		if (_ft)
			_TableSplit(_widget, _ft, _flag, _widget->uExtension.sTable.pCellText[_cellidx], &_widget->uExtension.sTable.pCellBroken[_cellidx], _widget->uExtension.sTable.aColumnWidth[_Column]);
	}
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
	BLU32 _cellidx = _Column + _Row * _widget->uExtension.sTable.nColumnNum;
	if (_cellidx < _widget->uExtension.sTable.nCellNum)
		return _widget->uExtension.sTable.pCellText[_cellidx];
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
	memset(_widget->uExtension.sTable.aFontSource, 0, sizeof(_widget->uExtension.sTable.aFontSource));
	strcpy(_widget->uExtension.sTable.aFontSource, _Font);
	_widget->uExtension.sTable.nFontHeight = _FontHeight;
	if (_Outline)
	{
		_widget->uExtension.sTable.bOutline = _Outline;
		_widget->uExtension.sTable.bBold = FALSE;
		_widget->uExtension.sTable.bShadow = FALSE;
		_widget->uExtension.sTable.bItalics = _Italics;
	}
	else if (_Bold)
	{
		_widget->uExtension.sTable.bOutline = FALSE;
		_widget->uExtension.sTable.bBold = _Bold;
		_widget->uExtension.sTable.bShadow = FALSE;
		_widget->uExtension.sTable.bItalics = _Italics;
	}
	else if (_Shadow)
	{
		_widget->uExtension.sTable.bOutline = FALSE;
		_widget->uExtension.sTable.bBold = FALSE;
		_widget->uExtension.sTable.bShadow = _Shadow;
		_widget->uExtension.sTable.bItalics = _Italics;
	}
	else
	{
		_widget->uExtension.sTable.bOutline = FALSE;
		_widget->uExtension.sTable.bBold = FALSE;
		_widget->uExtension.sTable.bShadow = FALSE;
		_widget->uExtension.sTable.bItalics = _Italics;
	}
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
	BLAnsi _texfile[260] = { 0 };
	sprintf(_texfile, "%s/pixmap/%s.bmg", _PrUIMem->aDir, _Pixmap);
	_FetchResource(_texfile, (_PrUIMem->aArchive[0] == 0) ? NULL : _PrUIMem->aArchive, &_widget, _widget->nID, _LoadUI, _UISetup, TRUE);
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
blUIDialAngle(IN BLGuid _ID, IN BLF32 _Angle)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return;
	if (_widget->eType != BL_UT_DIAL)
		return;
	_widget->uExtension.sDial.fAngle = (BLS32)_Angle % 360 + (_Angle - (BLS32)_Angle);
	_PrUIMem->bDirty = TRUE;
}
BLVoid 
blUIDialAngleRange(IN BLGuid _ID, IN BLS32 _StartAngle, IN BLS32 _EndAngle)
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
BLF32 
blUIGetDialAngle(IN BLGuid _ID)
{
	_BLWidget* _widget = (_BLWidget*)blGuidAsPointer(_ID);
	if (!_widget)
		return 0.f;
	if (_widget->eType != BL_UT_DIAL)
		return 0.f;
	return _widget->uExtension.sDial.fAngle;
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
