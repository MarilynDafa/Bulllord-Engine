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
#ifndef __ui_h_
#define __ui_h_
#include "prerequisites.h"
#include "streamio.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLVoid blUIWorkspace(
	IN BLAnsi* _Dictionary,
	IN BLAnsi* _Archive);

BL_API BLVoid blUIFile(
	IN BLAnsi* _Filename);

BL_API BLGuid blGenUI(
	IN BLAnsi* _WidgetName,
	IN BLS32 _PosX,
	IN BLS32 _PosY,
	IN BLU32 _Width,
	IN BLU32 _Height,
	IN BLGuid _Parent,
	IN BLEnum _Type);

BL_API BLVoid blDeleteUI(
	IN BLGuid _ID);

BL_API BLGuid blUIQuery(
	IN BLAnsi* _WidgetName);

BL_API BLVoid blUISize(
	IN BLGuid _ID,
	IN BLU32 _Width,
	IN BLU32 _Height);

BL_API BLVoid blUIGetSize(
	IN BLGuid _ID,
	OUT BLU32* _Width,
	OUT BLU32* _Height);

BL_API BLVoid blUIReferencePoint(
	IN BLGuid _ID,
	IN BLEnum _ReferenceH,
	IN BLEnum _ReferenceV);

BL_API BLVoid blUIGetReferencePoint(
	IN BLGuid _ID,
	OUT BLEnum* _ReferenceH,
	OUT BLEnum* _ReferenceV);

BL_API BLVoid blUISizePolicy(
	IN BLGuid _ID,
	IN BLEnum _Policy);

BL_API BLVoid blUIGetSizePolicy(
	IN BLGuid _ID,
	OUT BLEnum* _Policy);

BL_API BLVoid blUISizeLimit(
	IN BLGuid _ID,
	IN BLU32 _MaxWidth,
	IN BLU32 _MaxHeight,
	IN BLU32 _MinWidth,
	IN BLU32 _MinHeight);

BL_API BLVoid blUIGetSizeLimit(
	IN BLGuid _ID,
	OUT BLU32* _MaxWidth,
	OUT BLU32* _MaxHeight,
	OUT BLU32* _MinWidth,
	OUT BLU32* _MinHeight);

BL_API BLVoid blUIScissor(
	IN BLGuid _ID,
	IN BLBool _Cliped, 
	IN BLBool _AbsScissor);

BL_API BLVoid blUIGetScissor(
	IN BLGuid _ID,
	OUT BLBool* _Cliped,
	OUT BLBool* _AbsScissor);

BL_API BLVoid blUIInteractive(
	IN BLGuid _ID,
	IN BLBool _Interactive);

BL_API BLVoid blUIGetInteractive(
	IN BLGuid _ID,
	OUT BLBool* _Interactive);

BL_API BLVoid blUIVisible(
	IN BLGuid _ID,
	IN BLBool _Visible);

BL_API BLVoid blUIGetVisible(
	IN BLGuid _ID,
	OUT BLBool* _Visible);

BL_API BLVoid blUITooltip(
	IN BLGuid _ID,
	IN BLUtf8* _Tooltip);

BL_API const BLUtf8* blUIGetTooltip(
	IN BLGuid _ID);

BL_API BLVoid blUIPenetration(
	IN BLGuid _ID,
	IN BLBool _Penetration);

BL_API BLVoid blUIGetPenetration(
	IN BLGuid _ID,
	OUT BLBool* _Penetration);

BL_API BLVoid blUIPanelPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUIPanelStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUIPanelCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUIPanelDragable(
	IN BLGuid _ID,
	IN BLBool _Dragable);

BL_API BLVoid blUIPanelBasePlate(
	IN BLGuid _ID,
	IN BLBool _BasePlate);

BL_API BLVoid blUIPanelModal(
	IN BLGuid _ID,
	IN BLBool _Modal);

BL_API BLVoid blUIPanelScrollable(
	IN BLGuid _ID,
	IN BLBool _Scrollable);

BL_API BLVoid blUIPanelFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUIButtonPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUIButtonStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUIButtonCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUIButtonHoverMap(
	IN BLGuid _ID,
	IN BLAnsi* _HoveredMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUIButtonPressMap(
	IN BLGuid _ID,
	IN BLAnsi* _PressedMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUIButtonDisableMap(
	IN BLGuid _ID,
	IN BLAnsi* _DisableMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUIButtonText(
	IN BLGuid _ID,
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor,
	IN BLEnum _TxtAlignmentH,
	IN BLEnum _TxtAlignmentV);

BL_API BLVoid blUIButtonFont(
	IN BLGuid _ID,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics);

BL_API BLVoid blUIButtonFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUIButtonEnable(
	IN BLGuid _ID,
	IN BLBool _Enable);

BL_API BLVoid blUILabelPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUILabelStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUILabelCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUILableText(
	IN BLGuid _ID,
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor,
	IN BLEnum _TxtAlignmentH,
	IN BLEnum _TxtAlignmentV,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics,
	IN BLBool _Rich);

BL_API BLVoid blUILabelFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUICheckPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUICheckStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUICheckCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUICheckCheckedMap(
	IN BLGuid _ID,
	IN BLAnsi* _CheckedMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUICheckDisableMap(
	IN BLGuid _ID,
	IN BLAnsi* _DisableMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUICheckText(
	IN BLGuid _ID,
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor,
	IN BLEnum _TxtAlignmentH,
	IN BLEnum _TxtAlignmentV);

BL_API BLVoid blUICheckFont(
	IN BLGuid _ID,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics);

BL_API BLVoid blUICheckFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUICheckEnable(
	IN BLGuid _ID,
	IN BLBool _Enable);

BL_API BLBool blUIGetCheckState(
	IN BLGuid _ID);

BL_API BLVoid blUITextPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUITextStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUITextCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUITextTypography(
	IN BLGuid _ID,
	IN BLBool _Autoscroll,
	IN BLBool _Wordwrap,
	IN BLBool _Multiline);

BL_API BLVoid blUITextPassword(
	IN BLGuid _ID,
	IN BLBool _Password);

BL_API BLVoid blUITextNumeric(
	IN BLGuid _ID,
	IN BLBool _Numeric,
	IN BLS32 _MinValue,
	IN BLS32 _MaxValue);

BL_API BLVoid blUITextPadding(
	IN BLGuid _ID,
	IN BLF32 _PaddingX,
	IN BLF32 _PaddingY);

BL_API BLVoid blUITextMaxLength(
	IN BLGuid _ID,
	IN BLU32 _MaxLength);

BL_API BLVoid blUITextPlaceholder(
	IN BLGuid _ID,
	IN BLUtf8* _Placeholder,
	IN BLU32 _TxtColor,
	IN BLEnum _TxtAlignmentH,
	IN BLEnum _TxtAlignmentV);

BL_API const BLUtf8* blUIGetTextText(
	IN BLGuid _ID);

BL_API BLVoid blUITextFont(
	IN BLGuid _ID,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics);

BL_API BLVoid blUITextFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUITextEnable(
	IN BLGuid _ID,
	IN BLBool _Enable);

BL_API BLVoid blUIProgressPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUIProgressStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUIProgressCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUIProgressFillMap(
	IN BLGuid _ID,
	IN BLAnsi* _FillMap);

BL_API BLVoid blUIProgressPercent(
	IN BLGuid _ID,
	IN BLU32 _Percent);

BL_API BLVoid blUIProgressBorder(
	IN BLGuid _ID,
	IN BLU32 _BorderX,
	IN BLU32 _BorderY);

BL_API BLVoid blUIProgressText(
	IN BLGuid _ID,
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor);

BL_API BLVoid blUIProgressFont(
	IN BLGuid _ID,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics);

BL_API BLVoid blUIProgressFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLF32 blUIGetProgressPercent(
	IN BLGuid _ID);

BL_API BLVoid blUISliderPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUISliderStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUISliderCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUISliderDisableMap(
	IN BLGuid _ID,
	IN BLAnsi* _DisableMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUISliderSliderCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _SliderCommonMap);

BL_API BLVoid blUISliderSliderDisableMap(
	IN BLGuid _ID,
	IN BLAnsi* _SliderDisableMap);

BL_API BLVoid blUISliderHorizontal(
	IN BLGuid _ID,
	IN BLBool _Horizontal);

BL_API BLVoid blUISliderSliderStep(
	IN BLGuid _ID,
	IN BLU32 _Step);

BL_API BLVoid blUISliderSliderPos(
	IN BLGuid _ID,
	IN BLS32 _Pos);

BL_API BLVoid blUISliderSliderSize(
	IN BLGuid _ID,
	IN BLU32 _Width,
	IN BLU32 _Height);

BL_API BLVoid blUISliderSliderRange(
	IN BLGuid _ID,
	IN BLU32 _MinValue,
	IN BLU32 _MaxValue);

BL_API BLVoid blUISliderFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUISliderEnable(
	IN BLGuid _ID,
	IN BLBool _Enable);

BL_API BLU32 blUIGetSliderPos(
	IN BLGuid _ID);

BL_API BLVoid blUITablePixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUITableStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUITableCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap,
	IN BLF32 _CenterX,
	IN BLF32 _CenterY,
	IN BLF32 _Width,
	IN BLF32 _Height);

BL_API BLVoid blUITableOddItemMap(
	IN BLGuid _ID,
	IN BLAnsi* _OddItemMap);

BL_API BLVoid blUITableEvenItemMap(
	IN BLGuid _ID,
	IN BLAnsi* _EvenItemMap);

BL_API BLVoid blUITableAddRow(
	IN BLGuid _ID);

BL_API BLVoid blUITableAddColumn(
	IN BLGuid _ID);

BL_API BLVoid blUITableText(
	IN BLGuid _ID,
	IN BLUtf8* _Text,
	IN BLU32 _TxtColor,
	IN BLU32 _Row,
	IN BLU32 _Column);

BL_API const BLUtf8* blUIGetTableText(
	IN BLGuid _ID,
	IN BLU32 _Row,
	IN BLU32 _Column);

BL_API BLVoid blUITableFont(
	IN BLGuid _ID,
	IN BLAnsi* _Font,
	IN BLU32 _FontHeight,
	IN BLBool _Outline,
	IN BLBool _Bold,
	IN BLBool _Shadow,
	IN BLBool _Italics);

BL_API BLVoid blUITableFlip(
	IN BLGuid _ID,
	IN BLBool _FlipX,
	IN BLBool _FlipY);

BL_API BLVoid blUIDialPixmap(
	IN BLGuid _ID,
	IN BLAnsi* _Pixmap);

BL_API BLVoid blUIDialStencilMap(
	IN BLGuid _ID,
	IN BLAnsi* _StencilMap);

BL_API BLVoid blUIDialCommonMap(
	IN BLGuid _ID,
	IN BLAnsi* _CommonMap);

BL_API BLVoid blUIDialAngle(
	IN BLGuid _ID,
	IN BLS32 _StartAngle,
	IN BLS32 _EndAngle);

BL_API BLVoid blUIDialAngleCut(
	IN BLGuid _ID,
	IN BLBool _Cut);

BL_API BLVoid blUIDialClockwise(
	IN BLGuid _ID,
	IN BLBool _Clockwise);

BL_API BLVoid blUIPrimitiveFill(
	IN BLGuid _ID,
	IN BLBool _Fill);

BL_API BLVoid blUIPrimitiveClosed(
	IN BLGuid _ID,
	IN BLBool _Closed);

BL_API BLVoid blUIPrimitiveColor(
	IN BLGuid _ID,
	IN BLU32 _Color);

BL_API BLVoid blUIPrimitivePath(
	IN BLGuid _ID,
	IN BLF32* _XPath,
	IN BLF32* _YPath,
	IN BLU32 _PathNum);
#ifdef __cplusplus
}
#endif
#endif/*__ui_h_*/
