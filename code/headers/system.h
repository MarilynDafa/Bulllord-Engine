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
#ifndef __system_h_
#define __system_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
	BL_ET_NET 
		_Uparam -> MsgID
		_Sparam -> MsgLength
		_Pparam -> MsgBuf
	BL_ET_MOUSE
		_Sparam -> _MOUSE_EVENT
		LOWU16(_Uparam) -> X
		HIGU16(_Uparam) -> Y
		_Uparam -> wheel
	BL_ET_KEY
		HIGU16(_Uparam) KeyCode
		LOWU16(_Uparam) Pressed
		_Pparam String
	BL_ET_USER
		_Uparam ..
		_Sparam ..
		_Pparam ..
//ui event
//			_Type		_Uparam		_Sparam			_Pparam		 _ID
//slider	BL_ET_UI	sliderpos	BL_UT_SLIDER	NULL		id		value changed
//button	BL_ET_UI	0			BL_UT_BUTTON	NULL		id		button click
//check		BL_ET_UI	1 or 0		BL_UT_CHECK		NULL		id		1 checked, 0 unchecked
//label		BL_ET_UI	linkid		BL_UT_LABEL		NULL		id		label click linkid
//text		BL_ET_UI	keycode		BL_UT_TEXT		NULL		id		text changed
//dial		BL_ET_UI	angle		BL_UT_DIAL		NULL		id		dial value changed
//ui		BL_ET_UI	0xFFFFFFFF	type			NULL		id		ui action end
//sprite event
//			_Type			_Uparam									_Sparam													_Pparam		_ID
//			BL_ET_SPRITE	LOWU16(_Uparam)->X HIGU16(_Uparam)->Y	BL_ME_LDOWN or BL_ME_RDOWN or BL_ME_LUP or BL_ME_RUP	NULL		id	sprite click
//			BL_ET_SPRITE	0xFFFFFFFF								0														NULL		id	sprite action end
*/
    
BL_API BLEnum blPlatformIdentity();
    
BL_API BLU32 blSystemTicks();

BL_API const BLAnsi* blUserFolderDir();

BL_API const BLAnsi* blWorkingDir(
	IN BLBool _Content);

BL_API BLBool blSetClipboard(
	IN BLUtf8* _Text);

BL_API const BLUtf8* blGetClipboard();

BL_API BLBool blEnvString(
	IN BLUtf8* _Section,
	INOUT BLUtf8 _Value[256]);

BL_API BLVoid blOpenURL(
	IN BLUtf8* _Url);
    
BL_API BLBool blOpenPlugin(
    IN BLAnsi* _Basename);
    
BL_API BLBool blClosePlugin(
	IN BLAnsi* _Basename);
    
BL_API BLVoid* blGetPluginProcAddress(
	IN BLAnsi* _Basename,
    IN BLAnsi* _Function);

/**
	Video Format: MPEG2 PS Stream
	Audio Format: AAC 16BIT STEREO
*/
BL_API BLBool blVideoOperation(
	IN BLAnsi* _Filename,
	IN BLAnsi* _Archive,
	IN BLBool _Play);

BL_API BLVoid blAttachIME(
	IN BLF32 _Xpos,
	IN BLF32 _Ypos);

BL_API BLVoid blDetachIME();

BL_API BLVoid blSubscribeEvent(
	IN BLEnum _Type, 
	IN BLBool(*_Subscriber)(BLEnum, BLU32, BLS32, BLVoid*, BLGuid));

BL_API BLVoid blUnsubscribeEvent(
	IN BLEnum _Type,
	IN BLBool(*_Subscriber)(BLEnum, BLU32, BLS32, BLVoid*, BLGuid));

BL_API BLVoid blInvokeEvent(
	IN BLEnum _Type, 
	IN BLU32 _Uparam, 
	IN BLS32 _Sparam, 
	IN BLVoid* _Pparam,
	IN BLGuid _ID);

BL_API BLVoid blSystemDateTime(
	OUT BLS32* _Year,
	OUT BLS32* _Month,
	OUT BLS32* _Day,
	OUT BLS32* _Hour,
	OUT BLS32* _Minute,
	OUT BLS32* _Second,
	OUT BLS32* _Wday,
	OUT BLS32* _Yday,
	OUT BLS32* _Dst);

BL_API BLBool blSystemTimer(
	IN BLS32 _PositiveID,
	IN BLF32 _Elapse);
    
BL_API BLVoid blGetWindowSize(
	OUT BLU32* _Width, 
	OUT BLU32* _Height,
	OUT BLU32* _ActualWidth,
	OUT BLU32* _ActualHeight,
	OUT BLF32* _RatioX,
	OUT BLF32* _RatioY);

BL_API BLVoid blWindowSize(
	IN BLU32 _Width, 
	IN BLU32 _Height, 
	IN BLBool _Fullscreen);
    
BL_API BLVoid blSystemRun(
	IN BLAnsi* _Appname, 
	IN BLU32 _Width,
	IN BLU32 _Height,
	IN BLU32 _DesignWidth,
	IN BLU32 _DesignHeight,
	IN BLBool _UseDesignRes,
	IN BLBool _Fullscreen,
	IN BLBool _Profiler,
	IN BLEnum _Quality,
	IN BLVoid(*_Begin)(BLVoid),
	IN BLVoid(*_Step)(BLU32),
	IN BLVoid(*_End)(BLVoid));

BL_API BLVoid blSystemEmbedRun(
	IN BLS32 _Handle,
	IN BLVoid(*_Begin)(BLVoid),
	IN BLVoid(*_Step)(BLU32),
	IN BLVoid(*_End)(BLVoid));

#ifdef __cplusplus
}
#endif

#endif/* __system_h_ */
