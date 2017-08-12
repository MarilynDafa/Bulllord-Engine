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
#include "../headers/system.h"
#include "../headers/audio.h"
#include "../headers/utils.h"
#include "../headers/streamio.h"
#include "../headers/gpu.h"
#include "internal/internal.h"
#include "internal/dictionary.h"
#include "../externals/duktape/duktape.h"
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
#   include <ppl.h>
#   include <ppltasks.h>
#elif defined BL_PLATFORM_OSX
#   import <Cocoa/Cocoa.h>
#elif defined BL_PLATFORM_IOS
#   import <UIKit/UIKit.h>
#elif defined BL_PLATFORM_LINUX
#   include <X11/cursorfont.h>
#elif defined BL_PLATFORM_ANDROID
#	include <jni.h>
#	include <android/configuration.h>
#	include <android/looper.h>
#   include <android/asset_manager.h>
#	include <android/window.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
	extern BLVoid _StreamIOInit(duk_context*, BLVoid*);
	extern BLVoid _StreamIOStep(BLU32);
    extern BLVoid _StreamIODestroy();
    extern BLVoid _NetworkInit(duk_context*);
    extern BLVoid _NetworkStep(BLU32);
	extern BLVoid _NetworkDestroy();
	extern BLVoid _UtilsInit(duk_context*);
	extern BLVoid _UtilsStep(BLU32);
	extern BLVoid _UtilsDestroy();
    extern BLVoid _SpriteInit(duk_context*);
    extern BLVoid _SpriteStep(BLU32, BLBool);
    extern BLVoid _SpriteDestroy();
	extern BLVoid _UIInit(duk_context*, BLBool);
	extern BLVoid _UIStep(BLU32, BLBool);
	extern BLVoid _UIDestroy();
	extern BLBool _UseCustomCursor();
#ifdef __cplusplus
}
#endif
extern BLVoid _AudioInit(duk_context*);
extern BLVoid _AudioStep(BLU32);
extern BLVoid _AudioDestroy();
extern BLVoid _SystemInit();
extern BLVoid _SystemStep();
extern BLVoid _SystemDestroy();
#if defined(BL_PLATFORM_WIN32)
extern BLVoid _GpuIntervention(duk_context*, HWND, BLU32, BLU32, BLBool);
extern BLVoid _GpuSwapBuffer();
extern BLVoid _GpuAnitIntervention(HWND);
#elif defined(BL_PLATFORM_UWP)
extern BLVoid _GpuIntervention(duk_context*, Windows::UI::Core::CoreWindow^, BLU32, BLU32, BLBool);
extern BLVoid _GpuSwapBuffer();
extern BLVoid _GpuAnitIntervention();
#elif defined(BL_PLATFORM_OSX)
extern BLVoid _GpuIntervention(duk_context*, NSView*, BLU32, BLU32, BLBool);
extern BLVoid _GpuSwapBuffer();
extern BLVoid _GpuAnitIntervention();
#elif defined(BL_PLATFORM_IOS)
extern BLVoid _GpuIntervention(duk_context*, BLBool);
extern BLVoid _GpuSwapBuffer();
extern BLVoid _GpuAnitIntervention();
#elif defined(BL_PLATFORM_LINUX)
extern BLVoid _GpuIntervention(duk_context*, Display*, Window, BLU32, BLU32, GLXFBConfig, BLVoid*, BLBool);
extern BLVoid _GpuSwapBuffer();
extern BLVoid _GpuAnitIntervention();
#elif defined(BL_PLATFORM_ANDROID)
extern BLVoid _GpuIntervention(duk_context*, ANativeWindow*, BLU32, BLU32, BLBool, BLBool);
extern BLVoid _GpuSwapBuffer();
extern BLVoid _GpuAnitIntervention();
#else
#	"error what's the fucking platform"
#endif
typedef struct _BoostParam {
	BLUtf8 pAppName[128];
	BLU32 nScreenWidth;
	BLU32 nScreenHeight;
	BLU32 nDesignWidth;
	BLU32 nDesignHeight;
	BLBool bUseDesignRes;
	BLBool bFullscreen;
	BLBool bProfiler;
	BLEnum eQuality;
	BLS32 nHandle;
	const BLVoid(*pBeginFunc)(BLVoid);
	const BLVoid(*pStepFunc)(BLU32);
	const BLVoid(*pEndFunc)(BLVoid);
}_BLBoostParam;
typedef struct _Event {
	union
	{
		struct _NetEvent {
			BLU32 nID;
			BLVoid* pBuf;
			BLU32 nLength;
		} sNet;
		struct _UiEvent {
			BLS32 nSParam;
			BLU32 nUParam;
			BLVoid* pPParam;
			BLGuid nID;
		} sUI;
		struct _MouseEvent {
			BLEnum eEvent;
			BLS32 nX;
			BLS32 nY;
			BLS32 nWheel;
		} sMouse;
		struct _KeyEvent {
			BLEnum eCode;
			BLUtf8* pString;
			BLBool bPressed;
		} sKey;
		struct _SysEvent {
			BLS32 nSParam;
			BLU32 nUParam;
			BLVoid* pPParam;
		} sSys;
		struct _SpriteEvent {
			BLS32 nSParam;
			BLU32 nUParam;
			BLGuid nID;
		} sSprite;
	} uEvent;
	BLEnum eType;
} _BLEvent;
typedef struct _Timer {
	BLS32 nId;
	BLF32 fElapse;
	BLU32 nLastTime;
}_BLTimer;
typedef struct _Plugins {
	BLU32 nHash;
#if defined(BL_PLATFORM_WIN32)
	HMODULE pHandle;
#elif defined(BL_PLATFORM_UWP)
	HINSTANCE pHandle;
#else
	BLVoid* pHandle;
#endif
}_BLPlugin;
typedef struct _SystemMember {
	duk_context* pDukContext;
	_BLBoostParam sBoostParam;
	const BLBool(*pSubscriber[BL_ET_COUNT][128])(BLEnum, BLU32, BLS32, BLVoid*, BLGuid);
	const BLVoid(*pBeginFunc)(BLVoid);
	const BLVoid(*pStepFunc)(BLU32);
	const BLVoid(*pEndFunc)(BLVoid);
	BLVoid* aFuncPtr[BL_ET_COUNT + 3];
	_BLEvent* pEvents;
	_BLTimer aTimers[8];
	_BLPlugin aPlugins[64];
	BLAnsi aWorkDir[260];
	BLAnsi aUserDir[260];
	BLAnsi aEncryptkey[260];
	BLU8 aClipboard[1024];
	BLU32 nEventsSz;
	BLU32 nEventIdx;
	BLU32 nSysTime;
	BLS32 nOrientation;
#if defined(BL_PLATFORM_WIN32)
	HWND nHwnd;
	HIMC nIMC;
	BLBool bCtrlPressed;
#elif defined(BL_PLATFORM_UWP)
	Windows::UI::Text::Core::CoreTextEditContext^ pCTEcxt;
	Windows::Devices::Sensors::Accelerometer^ pSensor;
	Windows::Foundation::Point sIMEpos;
	BLBool bCtrlPressed;
#elif defined(BL_PLATFORM_LINUX)
    Display* pDisplay;
    Window nWindow;
    Cursor nCursor;
	Cursor nNilCursor;
    Atom nDelwin;
    Atom nProtocols;
    Atom nPing;
    Atom nBorder;
    Atom nFullScreen;
    Atom nWMState;
    Colormap nColormap;
    KeyCode nLastCode;
    XIM pIME;
    XIC pIC;
    BLVoid* pLib;
    BLU32 nMouseX;
    BLU32 nMouseY;
    BLBool bCtrlPressed;
    BLBool bEWMHSupported;
#elif defined(BL_PLATFORM_ANDROID)
	ANativeActivity* pActivity;
	AConfiguration* pConfig;
	ALooper* pLooper;
	ANativeWindow* pWindow;
	AInputQueue* pInputQueue;
	ANativeWindow* pPendingWindow;
	AInputQueue* pPendingInputQueue;
	JNINativeMethod sNativeMethod;
	pthread_mutex_t sMutex;
	pthread_cond_t sCond;
	pthread_t nThread;
	jobject pBLJava;
    BLVoid* pSavedState;
	BLS32 nActivityState;
	BLS32 nStateSaved;
	BLU32 nSavedStateSize;
	BLS32 nMsgRead;
	BLS32 nMsgWrite;
	BLBool bAvtivityFocus;
	BLBool bBackendState;
#elif defined(BL_PLATFORM_OSX)
	NSAutoreleasePool* pPool;
    NSWindow* pWindow;
    NSPoint sIMEpos;
    NSTextView* pTICcxt;
    NSResponder<NSWindowDelegate>* pDelegate;
    BLBool bCtrlPressed;
#elif defined(BL_PLATFORM_IOS)
	NSAutoreleasePool* pPool;
    UIWindow* pWindow;
    UITextField* pTICcxt;
    UIView* pCtlView;
    BLS32 nKeyboardHeight;
    BLU32 nRetinaScale;
#endif
}_BLSystemMember;
BLBool _GbSystemRunning = FALSE;
BLBool _GbTVMode = FALSE;
BLEnum _GbRenderQuality = BL_RQ_ULP;
static _BLSystemMember* _PrSystemMem = NULL;
#if defined(BL_PLATFORM_ANDROID)
static _BLBoostParam* _PrAndroidGlue = NULL;
#endif

static const void
_JSBeginFunc()
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_COUNT]);
	duk_call(_PrSystemMem->pDukContext, 0);
}
static duk_ret_t
_BeginFuncBridge(duk_context* _DKC)
{
	DUK_REGISTER_CALLBACK(_PrSystemMem->pBeginFunc, _JSBeginFunc, _PrSystemMem->aFuncPtr[BL_ET_COUNT]);
}
static const void
_JSStepFunc(BLU32 _Delta)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_COUNT + 1]);
	duk_push_uint(_PrSystemMem->pDukContext, _Delta);
	duk_call(_PrSystemMem->pDukContext, 1);
}
static duk_ret_t
_StepFuncBridge(duk_context* _DKC)
{
	DUK_REGISTER_CALLBACK(_PrSystemMem->pStepFunc, _JSStepFunc, _PrSystemMem->aFuncPtr[BL_ET_COUNT + 1]);
}
static const void
_JSEndFunc()
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_COUNT + 2]);
	duk_call(_PrSystemMem->pDukContext, 0);
}
static duk_ret_t
_EndFuncBridge(duk_context* _DKC)
{
	DUK_REGISTER_CALLBACK(_PrSystemMem->pEndFunc, _JSEndFunc, _PrSystemMem->aFuncPtr[BL_ET_COUNT + 2]);
}
static const BLBool
_SystemEventFunc(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_SYSTEM]);
	duk_push_uint(_PrSystemMem->pDukContext, _Type);
	duk_push_uint(_PrSystemMem->pDukContext, _UParam);
	duk_push_int(_PrSystemMem->pDukContext, _SParam);
	duk_push_pointer(_PrSystemMem->pDukContext, _PParam);
	duk_push_string(_PrSystemMem->pDukContext, blGuidAsString(_ID));
	duk_call(_PrSystemMem->pDukContext, 5);
	return TRUE;
}
static duk_ret_t
_SystemEventBridge(duk_context* _DKC)
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[BL_ET_SYSTEM][_idx])
		_idx++;
	DUK_REGISTER_CALLBACK(_PrSystemMem->pSubscriber[BL_ET_SYSTEM][_idx], _SystemEventFunc, _PrSystemMem->aFuncPtr[BL_ET_SYSTEM]);
}
static const BLBool
_NetEventFunc(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_NET]);
	duk_push_uint(_PrSystemMem->pDukContext, _Type);
	duk_push_uint(_PrSystemMem->pDukContext, _UParam);
	duk_push_int(_PrSystemMem->pDukContext, _SParam);
	duk_push_pointer(_PrSystemMem->pDukContext, _PParam);
	duk_push_string(_PrSystemMem->pDukContext, blGuidAsString(_ID));
	duk_call(_PrSystemMem->pDukContext, 5);
	return TRUE;
}
static duk_ret_t
_NetEventBridge(duk_context* _DKC)
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[BL_ET_NET][_idx])
		_idx++;
	DUK_REGISTER_CALLBACK(_PrSystemMem->pSubscriber[BL_ET_NET][_idx], _NetEventFunc, _PrSystemMem->aFuncPtr[BL_ET_NET]);
}
static const BLBool
_MouseEventFunc(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_MOUSE]);
	duk_push_uint(_PrSystemMem->pDukContext, _Type);
	duk_push_uint(_PrSystemMem->pDukContext, _UParam);
	duk_push_int(_PrSystemMem->pDukContext, _SParam);
	duk_push_pointer(_PrSystemMem->pDukContext, _PParam);
	duk_push_string(_PrSystemMem->pDukContext, blGuidAsString(_ID));
	duk_call(_PrSystemMem->pDukContext, 5);
	return TRUE;
}
static duk_ret_t
_MouseEventBridge(duk_context* _DKC)
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[BL_ET_MOUSE][_idx])
		_idx++;
	DUK_REGISTER_CALLBACK(_PrSystemMem->pSubscriber[BL_ET_MOUSE][_idx], _MouseEventFunc, _PrSystemMem->aFuncPtr[BL_ET_MOUSE]);
}
static const BLBool
_KeyEventFunc(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_KEY]);
	duk_push_uint(_PrSystemMem->pDukContext, _Type);
	duk_push_uint(_PrSystemMem->pDukContext, _UParam);
	duk_push_int(_PrSystemMem->pDukContext, _SParam);
	duk_push_pointer(_PrSystemMem->pDukContext, _PParam);
	duk_push_string(_PrSystemMem->pDukContext, blGuidAsString(_ID));
	duk_call(_PrSystemMem->pDukContext, 5);
	return TRUE;
}
static duk_ret_t
_KeyEventBridge(duk_context* _DKC)
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[BL_ET_KEY][_idx])
		_idx++;
	DUK_REGISTER_CALLBACK(_PrSystemMem->pSubscriber[BL_ET_KEY][_idx], _KeyEventFunc, _PrSystemMem->aFuncPtr[BL_ET_KEY]);
}
static const BLBool
_UIEventFunc(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_UI]);
	duk_push_uint(_PrSystemMem->pDukContext, _Type);
	duk_push_uint(_PrSystemMem->pDukContext, _UParam);
	duk_push_int(_PrSystemMem->pDukContext, _SParam);
	duk_push_pointer(_PrSystemMem->pDukContext, _PParam);
	duk_push_string(_PrSystemMem->pDukContext, blGuidAsString(_ID));
	duk_call(_PrSystemMem->pDukContext, 5);
	return TRUE;
}
static duk_ret_t
_UIEventBridge(duk_context* _DKC)
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[BL_ET_UI][_idx])
		_idx++;
	DUK_REGISTER_CALLBACK(_PrSystemMem->pSubscriber[BL_ET_UI][_idx], _UIEventFunc, _PrSystemMem->aFuncPtr[BL_ET_UI]);
}
static const BLBool
_SpriteEventFunc(BLEnum _Type, BLU32 _UParam, BLS32 _SParam, BLVoid* _PParam, BLGuid _ID)
{
	duk_push_heapptr(_PrSystemMem->pDukContext, _PrSystemMem->aFuncPtr[BL_ET_SPRITE]);
	duk_push_uint(_PrSystemMem->pDukContext, _Type);
	duk_push_uint(_PrSystemMem->pDukContext, _UParam);
	duk_push_int(_PrSystemMem->pDukContext, _SParam);
	duk_push_pointer(_PrSystemMem->pDukContext, _PParam);
	duk_push_string(_PrSystemMem->pDukContext, blGuidAsString(_ID));
	duk_call(_PrSystemMem->pDukContext, 5);
	return TRUE;
}
static duk_ret_t
_SpriteEventBridge(duk_context* _DKC)
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[BL_ET_SPRITE][_idx])
		_idx++;
	DUK_REGISTER_CALLBACK(_PrSystemMem->pSubscriber[BL_ET_SPRITE][_idx], _SpriteEventFunc, _PrSystemMem->aFuncPtr[BL_ET_SPRITE]);
}
static duk_ret_t
_JSExport(duk_context* _DKC)
{
	JS_FUNCTION_REG("blSubscribeBeginEvent", _BeginFuncBridge);
	JS_FUNCTION_REG("blSubscribeStepEvent", _StepFuncBridge);
	JS_FUNCTION_REG("blSubscribeEndEvent", _EndFuncBridge);
	JS_FUNCTION_REG("blSubscribeSystemEvent", _SystemEventBridge);
	JS_FUNCTION_REG("blSubscribeNetEvent", _NetEventBridge);
	JS_FUNCTION_REG("blSubscribeMouseEvent", _MouseEventBridge);
	JS_FUNCTION_REG("blSubscribeKeyEvent", _KeyEventBridge);
	JS_FUNCTION_REG("blSubscribeUIEvent", _UIEventBridge);
	JS_FUNCTION_REG("blSubscribeSpriteEvent", _SpriteEventBridge);
	return 1;
}
#if defined(BL_PLATFORM_WIN32)
LRESULT CALLBACK
_WndProc(HWND _Hwnd, UINT _Msg, WPARAM _Wparam, LPARAM _Lparam)
{
    switch (_Msg)
	{
	case WM_ACTIVATE:
	{
		if (WA_INACTIVE == LOWORD(_Wparam))
			_GbSystemRunning = 2;
		else
			_GbSystemRunning = 1;
		if (_UseCustomCursor())
			ShowCursor(FALSE);
		RECT _rc;
		GetWindowRect(_PrSystemMem->nHwnd, &_rc);
		InvalidateRect(_PrSystemMem->nHwnd, &_rc, FALSE);
		UpdateWindow(_PrSystemMem->nHwnd);
	}
	break;
	case WM_ERASEBKGND:
		return 1;
	case WM_ENTERSIZEMOVE:
		_GbSystemRunning = 2;
		break;
	case WM_EXITSIZEMOVE:
	{
		RECT _rc;
		GetClientRect(_Hwnd, &_rc);
		POINT _lt;
		_lt.x = _rc.left;
		_lt.y = _rc.top;
		ClientToScreen(_Hwnd, &_lt);
		_GbSystemRunning = 1;
	}
	break;
	case WM_SIZE:
	{
		if ((SIZE_MAXHIDE == _Wparam) || (SIZE_MINIMIZED == _Wparam))
			_GbSystemRunning = 2;
		else
			_GbSystemRunning = 1;
		RECT _rc;
		GetClientRect(_PrSystemMem->nHwnd, &_rc);
		_PrSystemMem->sBoostParam.nScreenWidth = _rc.right - _rc.left;
		_PrSystemMem->sBoostParam.nScreenHeight = _rc.bottom - _rc.top;
		GetWindowRect(_PrSystemMem->nHwnd, &_rc);
		InvalidateRect(_PrSystemMem->nHwnd, &_rc, FALSE);
		UpdateWindow(_PrSystemMem->nHwnd);
		blInvokeEvent(BL_ET_SYSTEM, BL_SE_RESOLUTION, 0, NULL, INVALID_GUID);
	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* _info = (MINMAXINFO*)(_Lparam);
		_info->ptMaxTrackSize.x = 50000;
		_info->ptMaxTrackSize.y = 50000;
	}
	break;
    case WM_DESTROY:
    case WM_CLOSE:
    {
		blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
        SetCursor(LoadCursorW(NULL, IDC_ARROW));
        ReleaseCapture();
        PostQuitMessage(0);
        _GbSystemRunning = FALSE;
	}
	break;
	case WM_MOUSELEAVE:
		if (!IsIconic(_Hwnd))
		{
			SetWindowLongPtrW(_Hwnd, GWLP_USERDATA, 0);
			SetCursor(LoadCursorW(NULL, IDC_ARROW));
		}
		break;
	case WM_MOUSEMOVE:
    {
		if (GetWindowLongPtrW(_Hwnd, GWLP_USERDATA) == 0)
		{
			SetWindowLongPtrW(_Hwnd, GWLP_USERDATA, 1);
			if (_UseCustomCursor())
				ShowCursor(FALSE);
			else
				SetCursor(LoadCursorW(NULL, IDC_ARROW));
		}
		blInvokeEvent(BL_ET_MOUSE, (BLU32)_Lparam, BL_ME_MOVE, NULL, INVALID_GUID);
		TRACKMOUSEEVENT _tme;
		_tme.cbSize = sizeof(_tme);
		_tme.dwFlags = TME_LEAVE;
		_tme.dwHoverTime = HOVER_DEFAULT;
		_tme.hwndTrack = _Hwnd;
		TrackMouseEvent(&_tme);
	}
	break;
    case WM_MOUSEWHEEL:
    {
		BLS16 _val= (BLS16)((BLS16)HIWORD(_Wparam) / 120.f);
		if (_val > 0)
			blInvokeEvent(BL_ET_MOUSE, MAKEU32(1, _val), BL_ME_WHEEL, NULL, INVALID_GUID);
		else
			blInvokeEvent(BL_ET_MOUSE, MAKEU32(0, -_val), BL_ME_WHEEL, NULL, INVALID_GUID);
	}
	break;
    case WM_LBUTTONDOWN:
	{
		blInvokeEvent(BL_ET_MOUSE, (BLU32)_Lparam, BL_ME_LDOWN, NULL, INVALID_GUID);
        SetCapture(_Hwnd);
	}
	break;
    case WM_LBUTTONUP:
	{
		blInvokeEvent(BL_ET_MOUSE, (BLU32)_Lparam, BL_ME_LUP, NULL, INVALID_GUID);
		ReleaseCapture();
	}
	break;
    case WM_RBUTTONDOWN:
    {
		blInvokeEvent(BL_ET_MOUSE, (BLU32)_Lparam, BL_ME_RDOWN, NULL, INVALID_GUID);
        SetCapture(_Hwnd);
	}
	break;
    case WM_RBUTTONUP:
	{
		blInvokeEvent(BL_ET_MOUSE, (BLU32)_Lparam, BL_ME_RUP, NULL, INVALID_GUID);
		ReleaseCapture();
	}
	break;
    case WM_KEYDOWN:
    {
		BLEnum _code;
		BLBool _extended = ((_Lparam & (1 << 24)) != 0);
		BLS32 _wscancode = (_Lparam >> 16) & 0xFF;
		if (_wscancode == 0 || _wscancode == 0x45)
		{
			switch (_Wparam)
			{
			case VK_CLEAR: _code = BL_KC_CLEAR; break;
			case VK_MODECHANGE: _code = BL_KC_MODE; break;
			case VK_SELECT: _code = BL_KC_SELECT; break;
			case VK_EXECUTE: _code = BL_KC_EXECUTE; break;
			case VK_HELP: _code = BL_KC_HELP; break;
			case VK_PAUSE: _code = BL_KC_PAUSE; break;
			case VK_NUMLOCK: _code = BL_KC_NUMLOCKCLEAR; break;
			case VK_F13: _code = BL_KC_F13; break;
			case VK_F14: _code = BL_KC_F14; break;
			case VK_F15: _code = BL_KC_F15; break;
			case VK_F16: _code = BL_KC_F16; break;
			case VK_F17: _code = BL_KC_F17; break;
			case VK_F18: _code = BL_KC_F18; break;
			case VK_F19: _code = BL_KC_F19; break;
			case VK_F20: _code = BL_KC_F20; break;
			case VK_F21: _code = BL_KC_F21; break;
			case VK_F22: _code = BL_KC_F22; break;
			case VK_F23: _code = BL_KC_F23; break;
			case VK_F24: _code = BL_KC_F24; break;
			case VK_OEM_NEC_EQUAL: _code = BL_KC_KP_EQUALS; break;
			case VK_BROWSER_BACK: _code = BL_KC_AC_BACK; break;
			case VK_BROWSER_FORWARD: _code = BL_KC_AC_FORWARD; break;
			case VK_BROWSER_REFRESH: _code = BL_KC_AC_REFRESH; break;
			case VK_BROWSER_STOP: _code = BL_KC_AC_STOP; break;
			case VK_BROWSER_SEARCH: _code = BL_KC_AC_SEARCH; break;
			case VK_BROWSER_FAVORITES: _code = BL_KC_AC_BOOKMARKS; break;
			case VK_BROWSER_HOME: _code = BL_KC_AC_HOME; break;
			case VK_VOLUME_MUTE: _code = BL_KC_AUDIOMUTE; break;
			case VK_VOLUME_DOWN: _code = BL_KC_VOLUMEDOWN; break;
			case VK_VOLUME_UP: _code = BL_KC_VOLUMEUP; break;
			case VK_MEDIA_NEXT_TRACK: _code = BL_KC_AUDIONEXT; break;
			case VK_MEDIA_PREV_TRACK: _code = BL_KC_AUDIOPREV; break;
			case VK_MEDIA_STOP: _code = BL_KC_AUDIOSTOP; break;
			case VK_MEDIA_PLAY_PAUSE: _code = BL_KC_AUDIOPLAY; break;
			case VK_LAUNCH_MAIL: _code = BL_KC_MAIL; break;
			case VK_LAUNCH_MEDIA_SELECT: _code = BL_KC_MEDIASELECT; break;
			case VK_OEM_102: _code = BL_KC_NONUSBACKSLASH; break;
			case VK_ATTN: _code = BL_KC_SYSREQ; break;
			case VK_CRSEL: _code = BL_KC_CRSEL; break;
			case VK_EXSEL: _code = BL_KC_EXSEL; break;
			case VK_OEM_CLEAR: _code = BL_KC_CLEAR; break;
			case VK_LAUNCH_APP1: _code = BL_KC_APP1; break;
			case VK_LAUNCH_APP2: _code = BL_KC_APP2; break;
			default: _code = BL_KC_UNKNOWN; break;
			}
		}
		if (_wscancode > 127)
			_code = BL_KC_UNKNOWN;
		_code = SCANCODE_INTERNAL[_wscancode];
		if (!_extended)
		{
			switch (_code)
			{
			case BL_KC_HOME:_code = BL_KC_KP_7; break;
			case BL_KC_UP:_code = BL_KC_KP_8; break;
			case BL_KC_PAGEUP:_code = BL_KC_KP_9; break;
			case BL_KC_LEFT:_code = BL_KC_KP_4; break;
			case BL_KC_RIGHT:_code = BL_KC_KP_6; break;
			case BL_KC_END:_code = BL_KC_KP_1; break;
			case BL_KC_DOWN:_code = BL_KC_KP_2; break;
			case BL_KC_PAGEDOWN:_code = BL_KC_KP_3; break;
			case BL_KC_INSERT:_code = BL_KC_KP_0; break;
			case BL_KC_DELETE:_code = BL_KC_KP_PERIOD; break;
			case BL_KC_PRINTSCREEN:_code = BL_KC_KP_MULTIPLY; break;
			default:break;
			}
		}
		else
		{
			switch (_code)
			{
			case BL_KC_RETURN:_code = BL_KC_KP_ENTER; break;
			case BL_KC_LALT:_code = BL_KC_RALT; break;
			case BL_KC_LCTRL:_code = BL_KC_RCTRL; break;
			case BL_KC_SLASH:_code = BL_KC_KP_DIVIDE; break;
			case BL_KC_CAPSLOCK:_code = BL_KC_KP_PLUS; break;
			default:break;
			}
		}
		if (_code == BL_KC_LCTRL)
			_PrSystemMem->bCtrlPressed = TRUE;
		if (_PrSystemMem->bCtrlPressed)
		{
			switch (_code)
			{
			case BL_KC_A:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_SELECT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_C:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_COPY, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_X:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_CUT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_V:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_PASTE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			default:
				blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			}
		}
		else
			blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
	}
	break;
	case WM_KEYUP:
	{
		BLEnum _code;
		BLBool _extended = ((_Lparam & (1 << 24)) != 0);
		BLS32 _wscancode = (_Lparam >> 16) & 0xFF;
		if (_wscancode == 0 || _wscancode == 0x45)
		{
			switch (_Wparam)
			{
			case VK_CLEAR: _code = BL_KC_CLEAR; break;
			case VK_MODECHANGE: _code = BL_KC_MODE; break;
			case VK_SELECT: _code = BL_KC_SELECT; break;
			case VK_EXECUTE: _code = BL_KC_EXECUTE; break;
			case VK_HELP: _code = BL_KC_HELP; break;
			case VK_PAUSE: _code = BL_KC_PAUSE; break;
			case VK_NUMLOCK: _code = BL_KC_NUMLOCKCLEAR; break;
			case VK_F13: _code = BL_KC_F13; break;
			case VK_F14: _code = BL_KC_F14; break;
			case VK_F15: _code = BL_KC_F15; break;
			case VK_F16: _code = BL_KC_F16; break;
			case VK_F17: _code = BL_KC_F17; break;
			case VK_F18: _code = BL_KC_F18; break;
			case VK_F19: _code = BL_KC_F19; break;
			case VK_F20: _code = BL_KC_F20; break;
			case VK_F21: _code = BL_KC_F21; break;
			case VK_F22: _code = BL_KC_F22; break;
			case VK_F23: _code = BL_KC_F23; break;
			case VK_F24: _code = BL_KC_F24; break;
			case VK_OEM_NEC_EQUAL: _code = BL_KC_KP_EQUALS; break;
			case VK_BROWSER_BACK: _code = BL_KC_AC_BACK; break;
			case VK_BROWSER_FORWARD: _code = BL_KC_AC_FORWARD; break;
			case VK_BROWSER_REFRESH: _code = BL_KC_AC_REFRESH; break;
			case VK_BROWSER_STOP: _code = BL_KC_AC_STOP; break;
			case VK_BROWSER_SEARCH: _code = BL_KC_AC_SEARCH; break;
			case VK_BROWSER_FAVORITES: _code = BL_KC_AC_BOOKMARKS; break;
			case VK_BROWSER_HOME: _code = BL_KC_AC_HOME; break;
			case VK_VOLUME_MUTE: _code = BL_KC_AUDIOMUTE; break;
			case VK_VOLUME_DOWN: _code = BL_KC_VOLUMEDOWN; break;
			case VK_VOLUME_UP: _code = BL_KC_VOLUMEUP; break;
			case VK_MEDIA_NEXT_TRACK: _code = BL_KC_AUDIONEXT; break;
			case VK_MEDIA_PREV_TRACK: _code = BL_KC_AUDIOPREV; break;
			case VK_MEDIA_STOP: _code = BL_KC_AUDIOSTOP; break;
			case VK_MEDIA_PLAY_PAUSE: _code = BL_KC_AUDIOPLAY; break;
			case VK_LAUNCH_MAIL: _code = BL_KC_MAIL; break;
			case VK_LAUNCH_MEDIA_SELECT: _code = BL_KC_MEDIASELECT; break;
			case VK_OEM_102: _code = BL_KC_NONUSBACKSLASH; break;
			case VK_ATTN: _code = BL_KC_SYSREQ; break;
			case VK_CRSEL: _code = BL_KC_CRSEL; break;
			case VK_EXSEL: _code = BL_KC_EXSEL; break;
			case VK_OEM_CLEAR: _code = BL_KC_CLEAR; break;
			case VK_LAUNCH_APP1: _code = BL_KC_APP1; break;
			case VK_LAUNCH_APP2: _code = BL_KC_APP2; break;
			default: _code = BL_KC_UNKNOWN; break;
			}
		}
		if (_wscancode > 127)
			_code = BL_KC_UNKNOWN;
		_code = SCANCODE_INTERNAL[_wscancode];
		if (!_extended)
		{
			switch (_code)
			{
			case BL_KC_HOME:_code = BL_KC_KP_7; break;
			case BL_KC_UP:_code = BL_KC_KP_8; break;
			case BL_KC_PAGEUP:_code = BL_KC_KP_9; break;
			case BL_KC_LEFT:_code = BL_KC_KP_4; break;
			case BL_KC_RIGHT:_code = BL_KC_KP_6; break;
			case BL_KC_END:_code = BL_KC_KP_1; break;
			case BL_KC_DOWN:_code = BL_KC_KP_2; break;
			case BL_KC_PAGEDOWN:_code = BL_KC_KP_3; break;
			case BL_KC_INSERT:_code = BL_KC_KP_0; break;
			case BL_KC_DELETE:_code = BL_KC_KP_PERIOD; break;
			case BL_KC_PRINTSCREEN:_code = BL_KC_KP_MULTIPLY; break;
			default:break;
			}
		}
		else
		{
			switch (_code)
			{
			case BL_KC_RETURN:_code = BL_KC_KP_ENTER; break;
			case BL_KC_LALT:_code = BL_KC_RALT; break;
			case BL_KC_LCTRL:_code = BL_KC_RCTRL; break;
			case BL_KC_SLASH:_code = BL_KC_KP_DIVIDE; break;
			case BL_KC_CAPSLOCK:_code = BL_KC_KP_PLUS; break;
			default:break;
			}
		}
		if (_code == BL_KC_LCTRL)
			_PrSystemMem->bCtrlPressed = FALSE;
		blInvokeEvent(BL_ET_KEY, MAKEU32(_code, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
	}
	break;
	case WM_CHAR:
	case WM_IME_CHAR:
	{
		BLUtf8 _text[5];
		UINT32 _codepoint = (UINT32)_Wparam;
		if (_codepoint <= 0x7F)
		{
			_text[0] = (BLUtf8)_codepoint;
			_text[1] = '\0';
		}
		else if (_codepoint <= 0x7FF)
		{
			_text[0] = 0xC0 | (BLUtf8)((_codepoint >> 6) & 0x1F);
			_text[1] = 0x80 | (BLUtf8)(_codepoint & 0x3F);
			_text[2] = '\0';
		}
		else if (_codepoint <= 0xFFFF)
		{
			_text[0] = 0xE0 | (BLUtf8)((_codepoint >> 12) & 0x0F);
			_text[1] = 0x80 | (BLUtf8)((_codepoint >> 6) & 0x3F);
			_text[2] = 0x80 | (BLUtf8)(_codepoint & 0x3F);
			_text[3] = '\0';
		}
		else if (_codepoint <= 0x10FFFF)
		{
			_text[0] = 0xF0 | (BLUtf8)((_codepoint >> 18) & 0x0F);
			_text[1] = 0x80 | (BLUtf8)((_codepoint >> 12) & 0x3F);
			_text[2] = 0x80 | (BLUtf8)((_codepoint >> 6) & 0x3F);
			_text[3] = 0x80 | (BLUtf8)(_codepoint & 0x3F);
			_text[4] = '\0';
		}
		else
			break;
		blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, _text, INVALID_GUID);
    }
	break;
	default:
		return DefWindowProcW(_Hwnd, _Msg, _Wparam, _Lparam);
	}
	return 0;
}
BLVoid
_EnterFullscreen()
{
    DEVMODEW _settings;
    ZeroMemory(&_settings, sizeof(_settings));
    _settings.dmSize = sizeof(_settings);
    _settings.dmPelsWidth = GetDeviceCaps(GetDC(_PrSystemMem->nHwnd), HORZRES);
    _settings.dmPelsHeight = GetDeviceCaps(GetDC(_PrSystemMem->nHwnd), VERTRES);
    _settings.dmBitsPerPel = GetDeviceCaps(GetDC(_PrSystemMem->nHwnd), BITSPIXEL);
    _settings.dmDisplayFrequency = ::GetDeviceCaps(::GetDC(_PrSystemMem->nHwnd), VREFRESH);
    _settings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
    if (ChangeDisplaySettingsW(&_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        return;
    SetWindowLongW(_PrSystemMem->nHwnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    SetWindowLongW(_PrSystemMem->nHwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
	SetWindowPos(_PrSystemMem->nHwnd, 0, 0, 0, _settings.dmPelsWidth, _settings.dmPelsHeight, SWP_SHOWWINDOW | SWP_NOZORDER);
	ShowWindow(_PrSystemMem->nHwnd, SW_MAXIMIZE);
	SetWindowLongPtrW(_PrSystemMem->nHwnd, GWLP_USERDATA, 0);
}
BLVoid
_ExitFullscreen(BLU32 _Width, BLU32 _Height)
{
    if (ChangeDisplaySettingsW(NULL, CDS_RESET) != DISP_CHANGE_SUCCESSFUL)
        return;
    SetWindowLongPtrW(_PrSystemMem->nHwnd, GWL_EXSTYLE, WS_EX_LEFT);
    SetWindowLongPtrW(_PrSystemMem->nHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    HDC _sdc = GetDC(NULL);
    BLS32 _left = (GetDeviceCaps(_sdc, HORZRES) - _Width) / 2;
    BLS32 _top = (GetDeviceCaps(_sdc, VERTRES) - _Height) / 2;
    BLS32 _width = _Width;
    BLS32 _height = _Height;
    ReleaseDC(NULL, _sdc);
    DWORD _style = WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
    RECT _rectangle = { 0, 0, (LONG)_Width, (LONG)_Height };
    AdjustWindowRect(&_rectangle, _style, FALSE);
    _width = _rectangle.right - _rectangle.left;
    _height = _rectangle.bottom - _rectangle.top;
    SetWindowPos(_PrSystemMem->nHwnd, NULL, _left, _top, _width, _height, SWP_SHOWWINDOW | SWP_NOZORDER);
	ShowWindow(_PrSystemMem->nHwnd, SW_RESTORE);
	SetWindowLongPtrW(_PrSystemMem->nHwnd, GWLP_USERDATA, 0);
}
BLVoid
_ShowWindow()
{
    const BLUtf16* _title = blGenUtf16Str(_PrSystemMem->sBoostParam.pAppName);
    WNDCLASSW _wc;
    _wc.style = 0;
    _wc.lpfnWndProc = _WndProc;
    _wc.cbClsExtra = 0;
    _wc.cbWndExtra = 0;
    _wc.hInstance = GetModuleHandleW(NULL);
    _wc.hIcon = NULL;
    _wc.hCursor = 0;
    _wc.hbrBackground = 0;
    _wc.lpszMenuName = NULL;
    _wc.lpszClassName = L"bulllord";
    RegisterClassW(&_wc);
    HDC _sdc = GetDC(NULL);
    BLS32 _left = (GetDeviceCaps(_sdc, HORZRES) - _PrSystemMem->sBoostParam.nScreenWidth) / 2;
    BLS32 _top = (GetDeviceCaps(_sdc, VERTRES) - _PrSystemMem->sBoostParam.nScreenHeight) / 2;
    BLS32 _width = _PrSystemMem->sBoostParam.nScreenWidth;
    BLS32 _height = _PrSystemMem->sBoostParam.nScreenHeight;
    ReleaseDC(NULL, _sdc);
    DWORD _style = WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
    RECT _rectangle = { 0, 0, _width, _height };
    AdjustWindowRect(&_rectangle, _style, FALSE);
    _width = _rectangle.right - _rectangle.left;
    _height = _rectangle.bottom - _rectangle.top;
	_PrSystemMem->nHwnd = CreateWindowW(L"bulllord", (LPCWSTR)_title, _style, _left, _top, _width, _height, NULL, NULL, GetModuleHandle(NULL), NULL);
    SetWindowLongPtrW(_PrSystemMem->nHwnd, GWL_EXSTYLE, WS_EX_LEFT);
    SetWindowLongPtrW(_PrSystemMem->nHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    SetWindowPos(_PrSystemMem->nHwnd, NULL, 0, 0, _width, _height, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER);
    if (_PrSystemMem->sBoostParam.bFullscreen)
        _EnterFullscreen();
	ShowWindow(_PrSystemMem->nHwnd, SW_NORMAL);
	UpdateWindow(_PrSystemMem->nHwnd);
	SetWindowLongPtrW(_PrSystemMem->nHwnd, GWLP_USERDATA, 0);
    blDeleteUtf16Str((BLUtf16*)_title);
	_GpuIntervention(_PrSystemMem->pDukContext, _PrSystemMem->nHwnd, _width, _height, !_PrSystemMem->sBoostParam.bProfiler);
	_PrSystemMem->nIMC = ImmGetContext(_PrSystemMem->nHwnd);
	if (_PrSystemMem->nIMC)
	{
		ImmAssociateContext(_PrSystemMem->nHwnd, NULL);
		ImmReleaseContext(_PrSystemMem->nHwnd, _PrSystemMem->nIMC);
		SetFocus(_PrSystemMem->nHwnd);
	}
}
BLVoid
_PollMsg()
{
    BOOL _gotmsg;
    MSG _msg;
    if (_GbSystemRunning == 1)
        _gotmsg = ::PeekMessage(&_msg, 0, 0, 0, PM_REMOVE) ? TRUE : FALSE;
    else
        _gotmsg = ::GetMessage(&_msg, 0, 0, 0) ? TRUE : FALSE;
    if (_gotmsg)
    {
        ::TranslateMessage(&_msg);
        ::DispatchMessage(&_msg);
    }
}
BLVoid
_CloseWindow()
{
	_GpuAnitIntervention(_PrSystemMem->nHwnd);
	ReleaseDC(_PrSystemMem->nHwnd, GetDC(_PrSystemMem->nHwnd));
	ChangeDisplaySettings(NULL, 0);
	ShowCursor(TRUE);
	DestroyWindow(_PrSystemMem->nHwnd);
}
#elif defined(BL_PLATFORM_UWP)
ref class UWPView sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
    virtual BLVoid Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ _Appview)
	{
		_Appview->Activated += ref new Windows::Foundation::TypedEventHandler<Windows::ApplicationModel::Core::CoreApplicationView^, Windows::ApplicationModel::Activation::IActivatedEventArgs^>(this, &UWPView::OnActivated);
		Windows::ApplicationModel::Core::CoreApplication::Suspending += ref new Windows::Foundation::EventHandler<Windows::ApplicationModel::SuspendingEventArgs^>(this, &UWPView::OnSuspending);
		Windows::ApplicationModel::Core::CoreApplication::Resuming += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &UWPView::OnResuming);
    }
	virtual BLVoid Uninitialize() {}
    virtual BLVoid SetWindow(Windows::UI::Core::CoreWindow^ _Window)
    {
		if (_PrSystemMem->nOrientation == SCREEN_PORTRAIT_INTERNAL)
			Windows::Graphics::Display::DisplayInformation::AutoRotationPreferences = Windows::Graphics::Display::DisplayOrientations::Portrait;
		else
			Windows::Graphics::Display::DisplayInformation::AutoRotationPreferences = Windows::Graphics::Display::DisplayOrientations::Landscape | Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped;
		_Window->SizeChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::WindowSizeChangedEventArgs^>(this, &UWPView::OnWindowSizeChanged);
		_Window->VisibilityChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::VisibilityChangedEventArgs^>(this, &UWPView::OnVisibilityChanged);
		_Window->Closed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::CoreWindowEventArgs^>(this, &UWPView::OnWindowClosed);
		_Window->PointerPressed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &UWPView::OnPointerPressed);
		_Window->PointerReleased += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &UWPView::OnPointerReleased);
		_Window->PointerMoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &UWPView::OnPointerMoved);
		_Window->PointerEntered += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &UWPView::OnPointerEntered);
		_Window->PointerExited += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &UWPView::OnPointerExited);
		_Window->PointerWheelChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &UWPView::OnPointerWheelChanged);
		_Window->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(this, &UWPView::OnKeyDown);
		_Window->KeyUp += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(this, &UWPView::OnKeyUp);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
		HardwareButtons::BackPressed += ref new EventHandler<BackPressedEventArgs^>(this, &UWPView::OnBackButtonPressed);
#endif
		Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->OrientationChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation^, Object^>(this, &UWPView::OnOrientationChanged);
		_PrSystemMem->pCTEcxt = Windows::UI::Text::Core::CoreTextServicesManager::GetForCurrentView()->CreateEditContext();
		_PrSystemMem->pCTEcxt->CompositionStarted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs^>(this, &UWPView::OnCompositionStarted);
		_PrSystemMem->pCTEcxt->CompositionCompleted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs^>(this, &UWPView::OnCompositionCompleted);
		_PrSystemMem->pCTEcxt->LayoutRequested += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs^>(this, &UWPView::OnLayoutRequested);
		_PrSystemMem->pCTEcxt->FocusRemoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Platform::Object^>(this, &UWPView::OnFocusRemoved);
		_PrSystemMem->pCTEcxt->TextUpdating += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs^>(this, &UWPView::OnTextUpdating);
		_PrSystemMem->pCTEcxt->FormatUpdating += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs^>(this, &UWPView::OnFormatUpdating);
		_PrSystemMem->pCTEcxt->SelectionRequested += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs^>(this, &UWPView::OnSelectionRequested);
		_PrSystemMem->pCTEcxt->SelectionUpdating += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs^>(this, &UWPView::OnSelectionUpdating);
		_PrSystemMem->pCTEcxt->TextRequested += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext^, Windows::UI::Text::Core::CoreTextTextRequestedEventArgs^>(this, &UWPView::OnTextRequested);
		_PrSystemMem->sBoostParam.nScreenWidth = ((BLS32)(0.5f + (((BLF32)(_Window->Bounds.Width) * (BLF32)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi) / 96.f)));
		_PrSystemMem->sBoostParam.nScreenHeight = ((BLS32)(0.5f + (((BLF32)(_Window->Bounds.Height) * (BLF32)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi) / 96.f)));
		_cachestr = L"";
		_initcursor = 100;
	}
	virtual BLVoid Load(Platform::String^)
	{
		if (!_GbSystemRunning)
		{
			Windows::UI::Core::CoreWindow ^ _window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
			_GpuIntervention(_PrSystemMem->pDukContext, _window,
				((BLS32)(0.5f + (((BLF32)(_window->Bounds.Width) * (BLF32)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi) / 96.f))),
				((BLS32)(0.5f + (((BLF32)(_window->Bounds.Height) * (BLF32)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi) / 96.f))),
				!_PrSystemMem->sBoostParam.bProfiler);
			_GbSystemRunning = TRUE;
			_PrSystemMem->pBeginFunc();
		}
	}
    virtual BLVoid Run()
	{
		do {
			if (_GbSystemRunning == 1)
				Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
			else
				Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
			_SystemStep();
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
			if (_initcursor >= 0 && _UseCustomCursor())
			{
				Windows::UI::Core::CoreWindow::GetForCurrentThread()->PointerCursor = nullptr;
				_initcursor--;
			}
#endif
		} while (_GbSystemRunning);
		_SystemDestroy();
	}
protected:
	BLVoid OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ _View, Windows::ApplicationModel::Activation::IActivatedEventArgs^ _Args)
	{
		if (_Args->Kind == Windows::ApplicationModel::Activation::ActivationKind::Launch)
		{
			if (static_cast<Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^>(_Args)->PrelaunchActivated)
			{
				Windows::ApplicationModel::Core::CoreApplication::Exit();
				return;
			}
		}
		Windows::UI::Core::CoreWindow::GetForCurrentThread()->Activate();
	}
	BLVoid OnSuspending(Platform::Object^ _Sender, Windows::ApplicationModel::SuspendingEventArgs^ _Args)
	{
		if (_GbSystemRunning == 2)
		{
			blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
			_GbSystemRunning = FALSE;
			_SystemDestroy();
		}
		Windows::ApplicationModel::SuspendingDeferral^ _deferral = _Args->SuspendingOperation->GetDeferral();
		concurrency::create_task([this, _deferral]()
		{
			_deferral->Complete();
		});
	}
	BLVoid OnResuming(Platform::Object^ _Sender, Platform::Object^ _Args)
	{
	}
	BLVoid OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::WindowSizeChangedEventArgs^ _Args)
	{
		_PrSystemMem->sBoostParam.nScreenWidth = ((BLS32)(0.5f + (((BLF32)(_Sender->Bounds.Width) * (BLF32)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi) / 96.f)));
		_PrSystemMem->sBoostParam.nScreenHeight = ((BLS32)(0.5f + (((BLF32)(_Sender->Bounds.Height) * (BLF32)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi) / 96.f)));
		blInvokeEvent(BL_ET_SYSTEM, BL_SE_RESOLUTION, 0, NULL, INVALID_GUID);
	}
	BLVoid OnVisibilityChanged(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::VisibilityChangedEventArgs^ _Args)
	{
		_GbSystemRunning = _Args->Visible ? 1 : 2;
	}
	BLVoid OnWindowClosed(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::CoreWindowEventArgs^ _Args)
	{
		_GbSystemRunning = FALSE;
	}
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
	BLVoid OnBackButtonPressed(Platform::Object^ _Sender, Windows::Phone::UI::Input::BackPressedEventArgs^ _Args)
	{
		blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_ESCAPE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
	}
#endif
	BLVoid OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ _Sender, Platform::Object^ _Args)
	{
	}
	BLVoid OnPointerPressed(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::PointerEventArgs^ _Args)
	{
		_leftmouse = _Args->CurrentPoint->Properties->IsLeftButtonPressed;
		blInvokeEvent(BL_ET_MOUSE, MAKEU32(_Args->CurrentPoint->Position.Y, _Args->CurrentPoint->Position.X), _leftmouse ? BL_ME_LDOWN : BL_ME_RDOWN, NULL, INVALID_GUID);
	}
	BLVoid OnPointerReleased(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::PointerEventArgs^ _Args)
	{
		blInvokeEvent(BL_ET_MOUSE, MAKEU32(_Args->CurrentPoint->Position.Y, _Args->CurrentPoint->Position.X), _leftmouse ? BL_ME_LUP : BL_ME_RUP, NULL, INVALID_GUID);
	}
	BLVoid OnPointerMoved(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::PointerEventArgs^ _Args)
	{
		blInvokeEvent(BL_ET_MOUSE, MAKEU32(_Args->CurrentPoint->Position.Y, _Args->CurrentPoint->Position.X), BL_ME_MOVE, NULL, INVALID_GUID);
	}
	BLVoid OnPointerEntered(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::PointerEventArgs^ _Args)
	{
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
		if (_UseCustomCursor())
			_Sender->PointerCursor = nullptr;
		else
			_Sender->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
#endif
	}
	BLVoid OnPointerExited(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::PointerEventArgs^ _Args)
	{
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
		_Sender->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
#endif
	}
	BLVoid OnPointerWheelChanged(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::PointerEventArgs^ _Args)
	{
		BLS32 _val = (BLS32)((BLF32)_Args->CurrentPoint->Properties->MouseWheelDelta / WHEEL_DELTA);
		if (_val > 0)
			blInvokeEvent(BL_ET_MOUSE, MAKEU32(1, _val), BL_ME_WHEEL, NULL, INVALID_GUID);
		else
			blInvokeEvent(BL_ET_MOUSE, MAKEU32(0, -_val), BL_ME_WHEEL, NULL, INVALID_GUID);
	}
	BLVoid OnKeyDown(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::KeyEventArgs^ _Args)
	{
		BLBool _shift = FALSE;
		BLEnum _scancode = BL_KC_UNKNOWN;
		if (_Args->KeyStatus.ScanCode < (sizeof(SCANCODE_INTERNAL) / sizeof(SCANCODE_INTERNAL[0])))
		{
			switch ((BLS32)_Args->VirtualKey)
			{
			case 16:
				switch (SCANCODE_INTERNAL[_Args->KeyStatus.ScanCode])
				{
				case BL_KC_LSHIFT:
				case BL_KC_RSHIFT:
					_shift = TRUE;
					_scancode = SCANCODE_INTERNAL[_Args->KeyStatus.ScanCode];
					break;
				}
				break;
			default:
				break;
			}
		}
		if (!_shift)
		{
			if ((BLS32)_Args->VirtualKey < (sizeof(OFFICIALCODE_INTERNAL) / sizeof(OFFICIALCODE_INTERNAL[0])))
				_scancode = OFFICIALCODE_INTERNAL[(BLS32)_Args->VirtualKey];
			if (_scancode == BL_KC_UNKNOWN)
			{
				switch ((BLS32)_Args->VirtualKey)
				{
				case 173: _scancode = BL_KC_MUTE; break;
				case 174: _scancode = BL_KC_VOLUMEDOWN; break;
				case 175: _scancode = BL_KC_VOLUMEUP; break;
				case 176: _scancode = BL_KC_AUDIONEXT; break;
				case 177: _scancode = BL_KC_AUDIOPREV; break;
				case 179: _scancode = BL_KC_AUDIOPLAY; break;
				case 180: _scancode = BL_KC_MAIL; break;
				case 181: _scancode = BL_KC_MEDIASELECT; break;
				case 183: _scancode = BL_KC_CALCULATOR; break;
				case 186: _scancode = BL_KC_SEMICOLON; break;
				case 187: _scancode = BL_KC_EQUALS; break;
				case 188: _scancode = BL_KC_COMMA; break;
				case 189: _scancode = BL_KC_MINUS; break;
				case 190: _scancode = BL_KC_PERIOD; break;
				case 191: _scancode = BL_KC_SLASH; break;
				case 192: _scancode = BL_KC_GRAVE; break;
				case 219: _scancode = BL_KC_LEFTBRACKET; break;
				case 220: _scancode = BL_KC_BACKSLASH; break;
				case 221: _scancode = BL_KC_RIGHTBRACKET; break;
				case 222: _scancode = BL_KC_APOSTROPHE; break;
				default: _scancode = BL_KC_UNKNOWN; break;
				}
			}
			if (_scancode == BL_KC_UNKNOWN)
			{
				if (_Args->KeyStatus.ScanCode < (sizeof(SCANCODE_INTERNAL) / sizeof(SCANCODE_INTERNAL[0])))
					_scancode = SCANCODE_INTERNAL[_Args->KeyStatus.ScanCode];
			}
		}
		if (_scancode == BL_KC_LCTRL)
			_PrSystemMem->bCtrlPressed = TRUE;
		if (_PrSystemMem->bCtrlPressed)
		{
			switch (_scancode)
			{
			case BL_KC_A:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_SELECT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_C:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_COPY, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_X:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_CUT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_V:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_PASTE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			default:
				blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			}
		}
		else
			blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
	}
	BLVoid OnKeyUp(Windows::UI::Core::CoreWindow^ _Sender, Windows::UI::Core::KeyEventArgs^ _Args)
	{
		BLBool _shift = FALSE;
		BLEnum _scancode = BL_KC_UNKNOWN;
		if (_Args->KeyStatus.ScanCode < (sizeof(SCANCODE_INTERNAL) / sizeof(SCANCODE_INTERNAL[0])))
		{
			switch ((BLS32)_Args->VirtualKey)
			{
			case 16:
				switch (SCANCODE_INTERNAL[_Args->KeyStatus.ScanCode])
				{
				case BL_KC_LSHIFT:
				case BL_KC_RSHIFT:
					_shift = TRUE;
					_scancode = SCANCODE_INTERNAL[_Args->KeyStatus.ScanCode];
					break;
				}
				break;
			default:
				break;
			}
		}
		if (!_shift)
		{
			if ((BLS32)_Args->VirtualKey < (sizeof(OFFICIALCODE_INTERNAL) / sizeof(OFFICIALCODE_INTERNAL[0])))
				_scancode = OFFICIALCODE_INTERNAL[(BLS32)_Args->VirtualKey];
			if (_scancode == BL_KC_UNKNOWN)
			{
				switch ((BLS32)_Args->VirtualKey)
				{
				case 173: _scancode = BL_KC_MUTE; break;
				case 174: _scancode = BL_KC_VOLUMEDOWN; break;
				case 175: _scancode = BL_KC_VOLUMEUP; break;
				case 176: _scancode = BL_KC_AUDIONEXT; break;
				case 177: _scancode = BL_KC_AUDIOPREV; break;
				case 179: _scancode = BL_KC_AUDIOPLAY; break;
				case 180: _scancode = BL_KC_MAIL; break;
				case 181: _scancode = BL_KC_MEDIASELECT; break;
				case 183: _scancode = BL_KC_CALCULATOR; break;
				case 186: _scancode = BL_KC_SEMICOLON; break;
				case 187: _scancode = BL_KC_EQUALS; break;
				case 188: _scancode = BL_KC_COMMA; break;
				case 189: _scancode = BL_KC_MINUS; break;
				case 190: _scancode = BL_KC_PERIOD; break;
				case 191: _scancode = BL_KC_SLASH; break;
				case 192: _scancode = BL_KC_GRAVE; break;
				case 219: _scancode = BL_KC_LEFTBRACKET; break;
				case 220: _scancode = BL_KC_BACKSLASH; break;
				case 221: _scancode = BL_KC_RIGHTBRACKET; break;
				case 222: _scancode = BL_KC_APOSTROPHE; break;
				default: _scancode = BL_KC_UNKNOWN; break;
				}
			}
			if (_scancode == BL_KC_UNKNOWN)
			{
				if (_Args->KeyStatus.ScanCode < (sizeof(SCANCODE_INTERNAL) / sizeof(SCANCODE_INTERNAL[0])))
					_scancode = SCANCODE_INTERNAL[_Args->KeyStatus.ScanCode];
			}
		}
		if (_scancode == BL_KC_LCTRL)
			_PrSystemMem->bCtrlPressed = FALSE;
		blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
	}
	BLVoid OnLayoutRequested(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs^ _Args)
	{
		Windows::Foundation::Rect _r;
		_r.X = _PrSystemMem->sIMEpos.X + Windows::UI::Core::CoreWindow::GetForCurrentThread()->Bounds.Left;
		_r.Y = _PrSystemMem->sIMEpos.Y + Windows::UI::Core::CoreWindow::GetForCurrentThread()->Bounds.Top;
		_r.Height = 18;
		_r.Width = 250;
		_Args->Request->LayoutBounds->TextBounds = _r;
		_Args->Request->LayoutBounds->ControlBounds = _r;
	}
	BLVoid OnTextUpdating(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs^ _Args)
	{
		_cachestr = _Args->Text;
	}
	BLVoid OnFormatUpdating(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs^ _Args)
	{
	}
	BLVoid OnCompositionStarted(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs^ _Args)
	{
	}
	BLVoid OnCompositionCompleted(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs^ _Args)
	{
		BLUtf8 _tmp[1024] = { 0 };
		WideCharToMultiByte(CP_UTF8, 0, _cachestr->Data(), -1, (LPSTR)_tmp, 1024, NULL, NULL);
		blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, _tmp, INVALID_GUID);
		_cachestr = L"";
	}
	BLVoid OnSelectionRequested(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs^ _Args){}
	BLVoid OnTextRequested(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextTextRequestedEventArgs^ _Args)
	{
		_Args->Request->Text = ref new Platform::String(_cachestr->Data());
	}
	BLVoid OnSelectionUpdating(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs^ _Args){}
	BLVoid OnFocusRemoved(Windows::UI::Text::Core::CoreTextEditContext^ _Sender, Platform::Object^ _Args)
	{
		_cachestr = L"";
	}
private:
	BLS32 _initcursor;
	BLBool _leftmouse;
	Platform::String^ _cachestr;
};
ref class UWPSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
		return ref new UWPView();
	}
};
BLVoid
_EnterFullscreen()
{
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
	Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TryEnterFullScreenMode();
#endif
}
BLVoid
_ExitFullscreen(BLU32 _Width, BLU32 _Height)
{
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
	Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->ExitFullScreenMode();
	Windows::Foundation::Size _sz{ (BLF32)_Width, (BLF32)_Height };
	Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TryResizeView(_sz);
#endif
}
BLVoid
_ShowWindow()
{
	auto _window = ref new UWPSource();
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
    _PrSystemMem->sBoostParam.bFullscreen = TRUE;
#endif
	if (_PrSystemMem->sBoostParam.bFullscreen)
		Windows::UI::ViewManagement::ApplicationView::PreferredLaunchWindowingMode = Windows::UI::ViewManagement::ApplicationViewWindowingMode::FullScreen;
	else
	{
		Windows::UI::ViewManagement::ApplicationView::PreferredLaunchViewSize = Windows::Foundation::Size{ (BLF32)_PrSystemMem->sBoostParam.nScreenWidth, (BLF32)_PrSystemMem->sBoostParam.nScreenHeight };
		Windows::UI::ViewManagement::ApplicationView::PreferredLaunchWindowingMode = Windows::UI::ViewManagement::ApplicationViewWindowingMode::PreferredLaunchViewSize;
	}
	if (_PrSystemMem->sBoostParam.nScreenWidth > _PrSystemMem->sBoostParam.nScreenHeight)
		_PrSystemMem->nOrientation = SCREEN_LANDSCAPE_INTERNAL;
	else
		_PrSystemMem->nOrientation = SCREEN_PORTRAIT_INTERNAL;
	Windows::ApplicationModel::Core::CoreApplication::Run(_window);
}
BLVoid
_PollMsg()
{
}
BLVoid
_CloseWindow()
{
	_GpuAnitIntervention();
}
#elif defined(BL_PLATFORM_LINUX)
static BLVoid
_WndProc(XEvent* _Event)
{
    BLBool _filtered = FALSE;
    if (_PrSystemMem->pIME)
        _filtered = XFilterEvent(_Event, None);
    switch (_Event->type)
    {
    case KeyPress:
    {
        KeyCode _code = _Event->xkey.keycode;
        BLS32 _mincode, _maxcode;
        XDisplayKeycodes(_PrSystemMem->pDisplay, &_mincode, &_maxcode);
        BLEnum _scancode = SCANCODE_INTERNAL[_code - _mincode];
		if (_scancode == BL_KC_LCTRL)
			_PrSystemMem->bCtrlPressed = TRUE;
		if (_PrSystemMem->bCtrlPressed)
		{
			switch (_scancode)
			{
			case BL_KC_A:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_SELECT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_C:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_COPY, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_X:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_CUT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			case BL_KC_V:
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_PASTE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			default:
				blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				break;
			}
		}
		else
        {
            if (_PrSystemMem->nLastCode != _code || _PrSystemMem->nLastCode != _Event->xkey.time)
            {
                if (_code)
                    blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
            }
            _PrSystemMem->nLastCode = _code;
            if (!_filtered && _PrSystemMem->pIC)
            {
                BLS32 _count;
                Status _status;
                BLAnsi* _temp = (BLAnsi*)alloca(101);
                memset(_temp, 0, 101);
                _count = Xutf8LookupString(_PrSystemMem->pIC, &_Event->xkey, _temp, 100, NULL, &_status);
                if (_status == XBufferOverflow)
                {
                    _temp = (BLAnsi*)alloca(_count + 1);
                    _count = Xutf8LookupString(_PrSystemMem->pIC, &_Event->xkey, _temp, _count, NULL, &_status);
                }
                if (_status == XLookupChars || _status == XLookupBoth)
                    blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, _temp, INVALID_GUID);
            }
            else
                blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
        }
    }
    break;
    case KeyRelease:
    {
        KeyCode _code = _Event->xkey.keycode;
        BLS32 _mincode, _maxcode;
        XDisplayKeycodes(_PrSystemMem->pDisplay, &_mincode, &_maxcode);
        BLEnum _scancode = SCANCODE_INTERNAL[_code - _mincode];
		if (_scancode == BL_KC_LCTRL)
			_PrSystemMem->bCtrlPressed = FALSE;
		blInvokeEvent(BL_ET_KEY, MAKEU32(_scancode, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
    }
    break;
    case ButtonPress:
    {
        if (_Event->xbutton.button == Button1)
        	blInvokeEvent(BL_ET_MOUSE, MAKEU32(_PrSystemMem->nMouseY, _PrSystemMem->nMouseX), BL_ME_LDOWN, NULL, INVALID_GUID);
        else if (_Event->xbutton.button == Button3)
        	blInvokeEvent(BL_ET_MOUSE, MAKEU32(_PrSystemMem->nMouseY, _PrSystemMem->nMouseX), BL_ME_RDOWN, NULL, INVALID_GUID);
        else if (_Event->xbutton.button == Button4)
            blInvokeEvent(BL_ET_MOUSE, MAKEU32(1, 1), BL_ME_WHEEL, NULL, INVALID_GUID);
        else if (_Event->xbutton.button == Button5)
            blInvokeEvent(BL_ET_MOUSE, MAKEU32(0, 1), BL_ME_WHEEL, NULL, INVALID_GUID);
    }
    break;
    case ButtonRelease:
    {
        if (_Event->xbutton.button == Button1)
        	blInvokeEvent(BL_ET_MOUSE, MAKEU32(_PrSystemMem->nMouseY, _PrSystemMem->nMouseX), BL_ME_LUP, NULL, INVALID_GUID);
        else if(_Event->xbutton.button == Button3)
        	blInvokeEvent(BL_ET_MOUSE, MAKEU32(_PrSystemMem->nMouseY, _PrSystemMem->nMouseX), BL_ME_RUP, NULL, INVALID_GUID);
    }
    break;
    case MotionNotify:
    {
        _PrSystemMem->nMouseX = _Event->xmotion.x;
        _PrSystemMem->nMouseY = _Event->xmotion.y;
        blInvokeEvent(BL_ET_MOUSE, MAKEU32(_PrSystemMem->nMouseY, _PrSystemMem->nMouseX), BL_ME_MOVE, NULL, INVALID_GUID);
    }
    break;
    case EnterNotify:
    {
		if (_UseCustomCursor())
			XDefineCursor(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nNilCursor);
		else
			XDefineCursor(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nCursor);
    }
    break;
    case LeaveNotify:
    {
        XDefineCursor(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nCursor);
    }
    break;
    case Expose:
    {
        if (_Event->xexpose.count != 0)
            break;
    }
    break;
    case ConfigureNotify:
    {
        _PrSystemMem->sBoostParam.nScreenWidth = _Event->xconfigure.width;
        _PrSystemMem->sBoostParam.nScreenHeight = _Event->xconfigure.height;
		blInvokeEvent(BL_ET_SYSTEM, BL_SE_RESOLUTION, 0, NULL, INVALID_GUID);
    }
    break;
    case ClientMessage:
    {
        if (_filtered)
            return;
        if (_Event->xclient.message_type == None)
            return;
        if (_Event->xclient.message_type == _PrSystemMem->nProtocols)
        {
            const Atom _protocol = _Event->xclient.data.l[0];
            if (_protocol == None)
                return;
            if (_protocol == _PrSystemMem->nDelwin)
            {
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
                _GbSystemRunning = FALSE;
            }
            else if (_protocol == _PrSystemMem->nPing)
            {
                XEvent _reply = *_Event;
                _reply.xclient.window = RootWindow(_PrSystemMem->pDisplay, DefaultScreen(_PrSystemMem->pDisplay));
                XSendEvent(_PrSystemMem->pDisplay, RootWindow(_PrSystemMem->pDisplay, DefaultScreen(_PrSystemMem->pDisplay)), False, SubstructureNotifyMask | SubstructureRedirectMask, &_reply);
            }
        }
    }
    break;
    case FocusIn:
    {
        if (_Event->xfocus.mode == NotifyGrab || _Event->xfocus.mode == NotifyUngrab)
            return;
        if (_PrSystemMem->pIC)
            XSetICFocus(_PrSystemMem->pIC);
		_GbSystemRunning = 1;
    }
    break;
    case FocusOut:
    {
        if (_Event->xfocus.mode == NotifyGrab || _Event->xfocus.mode == NotifyUngrab)
            return;
        if (_PrSystemMem->pIC)
            XUnsetICFocus(_PrSystemMem->pIC);
		_GbSystemRunning = 2;
    }
    break;
    case DestroyNotify:
		blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
    break;
    default: break;
    }
}
BLVoid
_EnterFullscreen()
{
    XEvent _xev;
    _xev.xclient.type = ClientMessage;
    _xev.xclient.serial = 0;
    _xev.xclient.send_event = True;
    _xev.xclient.window = _PrSystemMem->nWindow;
    _xev.xclient.message_type = _PrSystemMem->nWMState;
    _xev.xclient.format = 32;
    _xev.xclient.data.l[0] = 1;
    _xev.xclient.data.l[1] = _PrSystemMem->nFullScreen;
    _xev.xclient.data.l[2] = 0;
    XSendEvent(_PrSystemMem->pDisplay, DefaultRootWindow(_PrSystemMem->pDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &_xev);
    XFlush(_PrSystemMem->pDisplay);
}
BLVoid
_ExitFullscreen(BLU32 _Width, BLU32 _Height)
{
    XEvent _xev;
    _xev.xclient.type = ClientMessage;
    _xev.xclient.serial = 0;
    _xev.xclient.send_event = True;
    _xev.xclient.window = _PrSystemMem->nWindow;
    _xev.xclient.message_type = _PrSystemMem->nWMState;
    _xev.xclient.format = 32;
    _xev.xclient.data.l[0] = 0;
    _xev.xclient.data.l[1] = _PrSystemMem->nFullScreen;
    _xev.xclient.data.l[2] = 0;
    if (_PrSystemMem->nBorder != None)
    {
        struct
        {
            unsigned long x1;
            unsigned long x2;
            unsigned long x3;
            long x4;
            unsigned long x5;
        } _bhints = {
            (1L << 1), 0, 1, 0, 0
        };
        XChangeProperty(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nBorder, _PrSystemMem->nBorder, 32, PropModeReplace, (BLU8*)&_bhints, sizeof(_bhints) / sizeof(long));
    }
    XSendEvent(_PrSystemMem->pDisplay, DefaultRootWindow(_PrSystemMem->pDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &_xev);
    XResizeWindow(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _Width, _Height);
    XFlush(_PrSystemMem->pDisplay);
}
BLVoid
_ShowWindow()
{
    if (setlocale(LC_ALL, "") == NULL)
        setlocale(LC_ALL, "C");
    XInitThreads();
    _PrSystemMem->pDisplay = XOpenDisplay(NULL);
    Atom _wmcheck = XInternAtom(_PrSystemMem->pDisplay, "_NET_SUPPORTING_WM_CHECK", True);
    Atom _netsupported = XInternAtom(_PrSystemMem->pDisplay, "_NET_SUPPORTED", True);
    _PrSystemMem->bEWMHSupported = _wmcheck && _netsupported;
    Atom _actualtype;
    BLS32 _actualformat;
    unsigned long _numitems;
    unsigned long _numbytes;
    BLU8* _data;
    BLS32 _result = XGetWindowProperty(_PrSystemMem->pDisplay, DefaultRootWindow(_PrSystemMem->pDisplay), _wmcheck, 0, 1, False, XA_WINDOW, &_actualtype, &_actualformat, &_numitems, &_numbytes, &_data);
    if (_result != Success || _actualtype != XA_WINDOW || _numitems != 1)
    {
        if(_result == Success)
            XFree(_data);
        _PrSystemMem->bEWMHSupported = FALSE;
    }
    else
    {
        Window _root = *(Window*)(_data);
        XFree(_data);
        if (!_root)
            _PrSystemMem->bEWMHSupported = FALSE;
        else
        {
            _result = XGetWindowProperty(_PrSystemMem->pDisplay, _root, _wmcheck, 0, 1, False, XA_WINDOW, &_actualtype, &_actualformat, &_numitems, &_numbytes, &_data);
            if (_result != Success || _actualtype != XA_WINDOW || _numitems != 1)
            {
                if(_result == Success)
                    XFree(_data);
                _PrSystemMem->bEWMHSupported = FALSE;
            }
            else
            {
                Window _child = *(Window*)(_data);
                XFree(_data);
                if (!_child)
                    _PrSystemMem->bEWMHSupported = FALSE;
                else if (_root != _child)
                    _PrSystemMem->bEWMHSupported = FALSE;
                else
                    _PrSystemMem->bEWMHSupported = TRUE;
            }
        }
    }
    _PrSystemMem->nProtocols = XInternAtom(_PrSystemMem->pDisplay, "WM_PROTOCOLS", False);
    _PrSystemMem->nBorder = XInternAtom(_PrSystemMem->pDisplay, "_MOTIF_WM_HINTS", True);
    _PrSystemMem->nDelwin = XInternAtom(_PrSystemMem->pDisplay, "WM_DELETE_WINDOW", False);
    _PrSystemMem->nFullScreen = XInternAtom(_PrSystemMem->pDisplay, "_NET_WM_STATE_FULLSCREEN", False);
    _PrSystemMem->nWMState = XInternAtom(_PrSystemMem->pDisplay, "_NET_WM_STATE", False);
    _PrSystemMem->nPing = None;
    Atom _wmpid = None;
    Atom _compositor = XInternAtom(_PrSystemMem->pDisplay, "_NET_WM_BYPASS_COMPOSITOR", False);
	BLS32 _visualattr[] = {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        8 , 8,
        9 , 8,
        10 , 8,
        11 , 8,
        12, 24,
        13, 8,
        5 , True,
        None
    };
    BLS32 _fbcount;
    _PrSystemMem->pLib = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
    glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)dlsym(_PrSystemMem->pLib, "glXGetProcAddress");
    glXGetProcAddressARB = (PFNGLXGETPROCADDRESSARBPROC)glXGetProcAddress((const GLubyte*)"glXGetProcAddressARB");
    PFNGLXCHOOSEFBCONFIGPROC glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddress((const GLubyte*)"glXChooseFBConfig");
    PFNGLXGETVISUALFROMFBCONFIGPROC glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)glXGetProcAddress((const GLubyte*)"glXGetVisualFromFBConfig");
    PFNGLXGETFBCONFIGATTRIBPROC glXGetFBConfigAttrib = (PFNGLXGETFBCONFIGATTRIBPROC)glXGetProcAddress((const GLubyte*)"glXGetFBConfigAttrib");
    GLXFBConfig* _fbc = glXChooseFBConfig(_PrSystemMem->pDisplay, DefaultScreen(_PrSystemMem->pDisplay), _visualattr, &_fbcount);
    assert(_fbc);
    BLS32 _bestfbc = -1, _worstfbc = -1, _bestnumsamp = -1, _worstnumsamp = 999;
    for (BLS32 _idx = 0; _idx < _fbcount; ++_idx)
    {
        XVisualInfo* _vi = glXGetVisualFromFBConfig(_PrSystemMem->pDisplay, _fbc[_idx]);
        if (_vi)
        {
            BLS32 _sampbuf, _samples;
            glXGetFBConfigAttrib(_PrSystemMem->pDisplay, _fbc[_idx], GLX_SAMPLE_BUFFERS , &_sampbuf);
            glXGetFBConfigAttrib(_PrSystemMem->pDisplay, _fbc[_idx], GLX_SAMPLES , &_samples);
            if (_bestfbc < 0 || (_sampbuf && _samples > _bestnumsamp))
                _bestfbc = _idx, _bestnumsamp = _samples;
            if (_worstfbc < 0 || !_sampbuf || _samples < _worstnumsamp)
                _worstfbc = _idx, _worstnumsamp = _samples;
        }
        XFree(_vi);
    }
    GLXFBConfig _bestfbconfig = _fbc[_bestfbc];
    XFree(_fbc);
    XVisualInfo* _vi = glXGetVisualFromFBConfig(_PrSystemMem->pDisplay, _bestfbconfig);
    XSetWindowAttributes _swa;
    _swa.colormap = _PrSystemMem->nColormap = XCreateColormap(_PrSystemMem->pDisplay, RootWindow(_PrSystemMem->pDisplay, _vi->screen), _vi->visual, AllocNone);
    _swa.background_pixmap = None;
    _swa.border_pixel = 0;
    _swa.event_mask = 6520959;
    _swa.override_redirect = (_PrSystemMem->sBoostParam.bFullscreen && !_PrSystemMem->bEWMHSupported) ? True : False;
    BLU32 _width = _PrSystemMem->sBoostParam.nScreenWidth;
    BLU32 _height = _PrSystemMem->sBoostParam.nScreenHeight;
    BLS32 _x = (DisplayWidth(_PrSystemMem->pDisplay, XDefaultScreen(_PrSystemMem->pDisplay))) - _width;
    BLS32 _y = (DisplayHeight(_PrSystemMem->pDisplay, XDefaultScreen(_PrSystemMem->pDisplay))) - _height;
    _PrSystemMem->nWindow = XCreateWindow(_PrSystemMem->pDisplay, RootWindow(_PrSystemMem->pDisplay, _vi->screen), _x/2, _y/2, _width, _height, 0, _vi->depth, InputOutput, _vi->visual, (CWEventMask | CWOverrideRedirect | CWColormap), &_swa);
    assert(_PrSystemMem->nWindow);
    XFree(_vi);
    XSelectInput(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, (FocusChangeMask | EnterWindowMask | LeaveWindowMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | PropertyChangeMask | StructureNotifyMask | KeymapStateMask));
    if (_PrSystemMem->bEWMHSupported)
    {
        _PrSystemMem->nPing = XInternAtom(_PrSystemMem->pDisplay, "_NET_WM_PING", False);
        _wmpid = XInternAtom(_PrSystemMem->pDisplay, "_NET_WM_PID", False);
        if (_PrSystemMem->nPing && _wmpid)
        {
            const pid_t _pid = getpid();
            XChangeProperty(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _wmpid, XA_CARDINAL, 32, PropModeReplace, (const BLU8*)&_pid, 1);
            XChangeProperty(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nProtocols, XA_ATOM, 32, PropModeReplace, (const BLU8*)&_wmpid, 1);
        }
    }
    XWMHints* _hints = XAllocWMHints();
    _hints->flags = StateHint;
    _hints->initial_state = NormalState;
    XSetWMHints(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _hints);
    XFree(_hints);
    if (_PrSystemMem->sBoostParam.bFullscreen && _PrSystemMem->nBorder != None)
    {
        struct
        {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
            long inputmode;
            unsigned long state;
        } _bhints;
        _bhints.flags = (1 << 0) | (1 << 1);
        _bhints.decorations = (1 << 6) | (1 << 2);
        _bhints.functions = (1 << 4) | (1 << 1) | (1 << 5);
        _bhints.inputmode = 0;
        _bhints.state = 0;
        XChangeProperty(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nBorder, _PrSystemMem->nBorder, 32, PropModeReplace, (const BLU8*)&_bhints, 5);
    }
    if (XSupportsLocale())
    {
        if (XSetLocaleModifiers("") != NULL)
        {
            _PrSystemMem->pIME = XOpenIM(_PrSystemMem->pDisplay, 0, NULL, NULL);
            if (_PrSystemMem->pIME)
            {
                BLU32 _idx;
                BLBool _found = FALSE;
                XIMStyles* _styles = NULL;
                if (XGetIMValues(_PrSystemMem->pIME, XNQueryInputStyle, &_styles, NULL) != NULL)
                    return;
                for (_idx = 0;  _idx < _styles->count_styles;  ++_idx)
                {
                    if (_styles->supported_styles[_idx] == (XIMPreeditNothing | XIMStatusNothing))
                    {
                        _found = TRUE;
                        break;
                    }
                }
                XFree(_styles);
                if (!_found)
                {
                    XCloseIM(_PrSystemMem->pIME);
                    _PrSystemMem->pIME = NULL;
                }
            }
        }
    }
    BLS32 _comp = 1;
    XChangeProperty(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _compositor, XA_CARDINAL, 32, PropModeReplace, (BLU8*)&_comp, 1);
    XSetWMProtocols(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, &_PrSystemMem->nDelwin, 1);
    XStoreName(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, (const BLAnsi*)_PrSystemMem->sBoostParam.pAppName);
    XMapWindow(_PrSystemMem->pDisplay, _PrSystemMem->nWindow);
	XMoveWindow(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _x / 2, _y / 2);
    if (_PrSystemMem->sBoostParam.bFullscreen)
        _EnterFullscreen();
    _PrSystemMem->nCursor = XCreateFontCursor(_PrSystemMem->pDisplay, XC_left_ptr);
	Pixmap _ncursorpixmap = XCreatePixmap(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, 1, 1, 1);
	GC _graphicsctx = XCreateGC(_PrSystemMem->pDisplay, _ncursorpixmap, 0, NULL);
	XDrawPoint(_PrSystemMem->pDisplay, _ncursorpixmap, _graphicsctx, 0, 0);
	XFreeGC(_PrSystemMem->pDisplay, _graphicsctx);
	XColor _alphacolor;
	_alphacolor.flags = DoRed | DoGreen | DoBlue;
	_alphacolor.red = _alphacolor.blue = _alphacolor.green = 0;
	_PrSystemMem->nNilCursor = XCreatePixmapCursor(_PrSystemMem->pDisplay, _ncursorpixmap, _ncursorpixmap, &_alphacolor, &_alphacolor, 0, 0);
	XFreePixmap(_PrSystemMem->pDisplay, _ncursorpixmap);
    _GpuIntervention(_PrSystemMem->pDukContext, _PrSystemMem->pDisplay, _PrSystemMem->nWindow, _width, _height, _bestfbconfig, _PrSystemMem->pLib, !_PrSystemMem->sBoostParam.bProfiler);
    XFlush(_PrSystemMem->pDisplay);
}
BLVoid
_PollMsg()
{
    BLS32 _count = XPending(_PrSystemMem->pDisplay);
    while (_count--)
    {
        XEvent _event;
        XNextEvent(_PrSystemMem->pDisplay, &_event);
        _WndProc(&_event);
    }
    XFlush(_PrSystemMem->pDisplay);
}
BLVoid
_CloseWindow()
{
    XLockDisplay(_PrSystemMem->pDisplay);
    XUnmapWindow(_PrSystemMem->pDisplay, _PrSystemMem->nWindow);
    XDestroyWindow(_PrSystemMem->pDisplay, _PrSystemMem->nWindow);
    if (_PrSystemMem->pIME)
        XCloseIM(_PrSystemMem->pIME);
    XFreeColormap(_PrSystemMem->pDisplay, _PrSystemMem->nColormap);
	_GpuAnitIntervention();
    XUnlockDisplay(_PrSystemMem->pDisplay);
	XCloseDisplay(_PrSystemMem->pDisplay);
	dlclose(_PrSystemMem->pLib);
}
#elif defined(BL_PLATFORM_ANDROID)
extern "C" {
	JNIEXPORT BLVoid JNICALL _TextChanged(JNIEnv* _Env, jclass, jstring _Text)
	{
		const BLUtf8* _utf8str = (const BLUtf8*)_Env->GetStringUTFChars(_Text, NULL);
		blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, _utf8str, INVALID_GUID);
		_Env->ReleaseStringUTFChars(_Text, (const BLAnsi*)_utf8str);
	}
}
static BLVoid
_WndProc(BLS32 _Msg, BLVoid* _Userdata)
{
	int8_t _cmd = _Msg;
	switch (_Msg)
	{
	case 10:
	case 13:
	case 14:
		pthread_mutex_lock(&_PrSystemMem->sMutex);
		write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
		while (_PrSystemMem->nActivityState != _cmd)
			pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
		pthread_mutex_unlock(&_PrSystemMem->sMutex);
		break;
	case 11:
		{
			pthread_mutex_lock(&_PrSystemMem->sMutex);
			write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
			while (_PrSystemMem->nActivityState != _cmd)
				pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
			pthread_mutex_unlock(&_PrSystemMem->sMutex);
			JNIEnv* _env = _PrSystemMem->pActivity->env;
			jclass _vercls = _env->FindClass("android/os/Build$VERSION");
			jfieldID _idfield = _env->GetStaticFieldID(_vercls, "SDK_INT", "I");
			jint _sdkver = _env->GetStaticIntField(_vercls, _idfield);
			ANativeActivity_setWindowFlags(_PrSystemMem->pActivity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
			jclass _acticls = _env->GetObjectClass(_PrSystemMem->pActivity->clazz);
			jmethodID _getmid = _env->GetMethodID(_acticls, "getWindow", "()Landroid/view/Window;");
			jobject _wndobj = _env->CallObjectMethod(_PrSystemMem->pActivity->clazz, _getmid);
			jclass _wndcls = _env->FindClass("android/view/Window");
			jmethodID _decormid = _env->GetMethodID(_wndcls, "getDecorView", "()Landroid/view/View;");
			jobject _decorobj = _env->CallObjectMethod(_wndobj, _decormid);
			jclass _viewcls = _env->FindClass("android/view/View");
			jint _flags = 0;
			if (_sdkver >= 14)
			{
				jfieldID _profilefield = _env->GetStaticFieldID(_viewcls, "SYSTEM_UI_FLAG_LOW_PROFILE", "I");
				jint _profile = _env->GetStaticIntField(_viewcls, _profilefield);
				_flags |= _profile;
			}
			if (_sdkver >= 16)
			{
				jfieldID _fullscreenfield = _env->GetStaticFieldID(_viewcls, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
				jint _fullscreen = _env->GetStaticIntField(_viewcls, _fullscreenfield);
				_flags |= _fullscreen;
			}
			if (_sdkver >= 19)
			{
				jfieldID _immersivefield = _env->GetStaticFieldID(_viewcls, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");
				jint _immersive = _env->GetStaticIntField(_viewcls, _immersivefield);
				_flags |= _immersive;
			}
			jmethodID _sysmid = _env->GetMethodID(_viewcls, "setSystemUiVisibility", "(I)V");
			_env->CallVoidMethod(_decorobj, _sysmid, _flags);
			_env->DeleteLocalRef(_vercls);
			_env->DeleteLocalRef(_acticls);
			_env->DeleteLocalRef(_wndobj);
			_env->DeleteLocalRef(_wndcls);
			_env->DeleteLocalRef(_decorobj);
			_env->DeleteLocalRef(_viewcls);
		}
		break;
	case 6:
		_PrSystemMem->bAvtivityFocus = TRUE;
		write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
		break;
	case 7:
		_PrSystemMem->bAvtivityFocus = FALSE;
		write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
		break;
	case 8:
	case 9:
	case 12:
		write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
		break;
	case 21:
	case 22:
		{
			pthread_mutex_lock(&_PrSystemMem->sMutex);
			if (_PrSystemMem->pPendingWindow != NULL)
			{
				_cmd = 2;
				write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
			}
			_PrSystemMem->pPendingWindow = (ANativeWindow*)_Userdata;
			if (_Userdata != NULL)
			{
				_cmd = 1;
				write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
			}
			while (_PrSystemMem->pWindow != _PrSystemMem->pPendingWindow)
				pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
			pthread_mutex_unlock(&_PrSystemMem->sMutex);
		}
		break;
	case 23:
	case 24:
		pthread_mutex_lock(&_PrSystemMem->sMutex);
		_PrSystemMem->pPendingInputQueue = (AInputQueue*)_Userdata;
		_cmd = 0;
		write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
		while (_PrSystemMem->pInputQueue != _PrSystemMem->pPendingInputQueue)
			pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
		pthread_mutex_unlock(&_PrSystemMem->sMutex);
		break;
	default:
		break;
	}
}
BLVoid
_ShowWindow()
{
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	if (_PrSystemMem->sBoostParam.nScreenWidth > _PrSystemMem->sBoostParam.nScreenHeight)
		_PrSystemMem->nOrientation = SCREEN_LANDSCAPE_INTERNAL;
	else
		_PrSystemMem->nOrientation = SCREEN_PORTRAIT_INTERNAL;
	JNIEnv* _env = _PrSystemMem->pActivity->env;
	_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
	jobject _actiobj = _PrSystemMem->pActivity->clazz;
	jclass _acticls = _env->GetObjectClass(_actiobj);
	jmethodID _orimid = _env->GetMethodID(_acticls, "setRequestedOrientation", "(I)V");
	if (_PrSystemMem->sBoostParam.nScreenWidth > _PrSystemMem->sBoostParam.nScreenHeight)
		_env->CallVoidMethod(_actiobj, _orimid, _PrSystemMem->nOrientation | 8);
	else
		_env->CallVoidMethod(_actiobj, _orimid, _PrSystemMem->nOrientation);
	jclass _metcls = _env->FindClass("android/util/DisplayMetrics");
	jmethodID _initmid = _env->GetMethodID(_metcls, "<init>", "()V");
	jobject _metobj = _env->NewObject(_metcls, _initmid);
	jmethodID _getmgrmid = _env->GetMethodID(_acticls, "getWindowManager", "()Landroid/view/WindowManager;");
	jobject _mgrobj = _env->CallObjectMethod(_actiobj, _getmgrmid);
	jclass _mgrcls = _env->FindClass("android/view/WindowManager");
	jmethodID _getdismid = _env->GetMethodID(_mgrcls, "getDefaultDisplay", "()Landroid/view/Display;");
	jobject _disobj = _env->CallObjectMethod(_mgrobj, _getdismid);
	jclass _discls = _env->FindClass("android/view/Display");
	jmethodID _getmetmid = _env->GetMethodID(_discls, "getMetrics", "(Landroid/util/DisplayMetrics;)V");
	_env->CallVoidMethod(_disobj, _getmetmid, _metobj);
	jfieldID _wfiled = _env->GetFieldID(_metcls, "widthPixels", "I");
	jfieldID _hfield = _env->GetFieldID(_metcls, "heightPixels", "I");
	jint _w = _env->GetIntField(_metobj, _wfiled);
	jint _h = _env->GetIntField(_metobj, _hfield);
	BLF32 _maxv = MAX_INTERNAL(_w, _h);
	BLF32 _minv = MIN_INTERNAL(_w, _h);
	if (_PrSystemMem->nOrientation == SCREEN_LANDSCAPE_INTERNAL)
	{
		_PrSystemMem->sBoostParam.nScreenWidth = _maxv;
		_PrSystemMem->sBoostParam.nScreenHeight = _minv;
	}
	else
	{
		_PrSystemMem->sBoostParam.nScreenWidth = _minv;
		_PrSystemMem->sBoostParam.nScreenHeight = _maxv;
	}
	jclass _cxtcls = _env->FindClass("android/content/Context");
	jmethodID _getmid = _env->GetMethodID(_cxtcls, "getDir", "(Ljava/lang/String;I)Ljava/io/File;");
	jstring _dexdir = _env->NewStringUTF("dex");
	jstring _outdexdir = _env->NewStringUTF("outdex");
	jobject _dirobj = _env->CallObjectMethod(_PrSystemMem->pActivity->clazz, _getmid, _dexdir, 0);
	jobject _outdirobj = _env->CallObjectMethod(_PrSystemMem->pActivity->clazz, _getmid, _outdexdir, 0);
	_env->DeleteLocalRef(_dexdir);
	_env->DeleteLocalRef(_outdexdir);
	jclass _filecls = _env->FindClass("java/io/File");
	jstring _dexfile = _env->NewStringUTF("org.bulllord.util.jar");
	jmethodID _filectormid = _env->GetMethodID(_filecls, "<init>", "(Ljava/io/File;Ljava/lang/String;)V");
	jmethodID _clsloadctormid = _env->GetMethodID(_cxtcls, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject _clsloadobj = _env->CallObjectMethod(_PrSystemMem->pActivity->clazz, _clsloadctormid);
	jobject _dexfileobj = _env->NewObject(_filecls, _filectormid, _dirobj, _dexfile);
	_env->DeleteLocalRef(_dexfile);
	jmethodID _existmid = _env->GetMethodID(_filecls, "exists", "()Z");
	jboolean _exist = _env->CallBooleanMethod(_dexfileobj, _existmid);
	jmethodID _pathmid = _env->GetMethodID(_filecls, "getAbsolutePath", "()Ljava/lang/String;");
	jstring _cstrobj = (jstring)_env->CallObjectMethod(_dirobj, _pathmid);
	jstring _ostrobj = (jstring)_env->CallObjectMethod(_outdirobj, _pathmid);
	const BLUtf8* _utfdir = (const BLUtf8*)_env->GetStringUTFChars(_cstrobj, NULL);
	BLAnsi _tmp[260] = { 0 };
	strcpy(_tmp, (const BLAnsi*)_utfdir);
	strcat(_tmp, "/org.bulllord.util.jar");
	_env->ReleaseStringUTFChars(_cstrobj, (const BLAnsi*)_utfdir);
	jstring _dexst = _env->NewStringUTF(_tmp);
	if (!_exist)
	{
		FILE* _fp = fopen(_tmp, "wb");
		BLU32 _bufsz;
		const BLU8* _buf = blGenBase64Decoder(DEXCODE_INTERNAL, &_bufsz);
		fwrite(_buf, 1, _bufsz, _fp);
		fclose(_fp);
		blDeleteBase64Decoder((BLU8*)_buf);
	}
	jclass _dalvikcls = _env->FindClass("dalvik/system/DexClassLoader");
	jmethodID _dvctormid = _env->GetMethodID(_dalvikcls, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
	jobject _dexloaderobj = _env->NewObject(_dalvikcls, _dvctormid, _dexst, _ostrobj, NULL, _clsloadobj);
	jmethodID _loadmid = _env->GetMethodID(_dalvikcls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring _devfile = _env->NewStringUTF("org.bulllord.util.BLInput");
	jclass _ret = (jclass)_env->CallObjectMethod(_dexloaderobj, _loadmid, _devfile);
	_env->DeleteLocalRef(_cxtcls);
	_env->DeleteLocalRef(_dirobj);
	_env->DeleteLocalRef(_outdirobj);
	_env->DeleteLocalRef(_filecls);
	_env->DeleteLocalRef(_dexfileobj);
	_env->DeleteLocalRef(_clsloadobj);
	_env->DeleteLocalRef(_dalvikcls);
	_env->DeleteLocalRef(_dexst);
	_env->DeleteLocalRef(_devfile);
	_env->DeleteLocalRef(_dexloaderobj);
	_env->RegisterNatives(_ret, &_PrSystemMem->sNativeMethod, 1);
	jmethodID _ctxctormid = _env->GetMethodID(_ret, "<init>", "(Landroid/content/Context;)V");
	_PrSystemMem->pBLJava = _env->NewObject(_ret, _ctxctormid, _PrSystemMem->pActivity->clazz);
	_PrSystemMem->pBLJava = _env->NewGlobalRef(_PrSystemMem->pBLJava);
	jmethodID _hideviewmid = _env->GetStaticMethodID(_ret, "addTextEdit", "()V");
	_env->CallStaticVoidMethod(_ret, _hideviewmid);
	_env->DeleteLocalRef(_acticls);
	_env->DeleteLocalRef(_metcls);
	_env->DeleteLocalRef(_disobj);
	_env->DeleteLocalRef(_mgrobj);
	_env->DeleteLocalRef(_mgrcls);
	_env->DeleteLocalRef(_metobj);
	_env->DeleteLocalRef(_discls);
	_env->DeleteLocalRef(_ret);
	_PrSystemMem->pActivity->vm->DetachCurrentThread();
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
	while (!_PrSystemMem->pWindow);
	_GpuIntervention(_PrSystemMem->pDukContext, _PrSystemMem->pWindow, _PrSystemMem->sBoostParam.nScreenWidth, _PrSystemMem->sBoostParam.nScreenHeight, !_PrSystemMem->sBoostParam.bProfiler, FALSE);
}
BLVoid
_PollMsg()
{
	BLS32 _ident;
	BLS32 _events;
	while ((_ident = ALooper_pollAll(0, NULL, &_events, NULL)) >= 0)
	{
		if (1 == _ident)
		{
			int8_t _cmd;
			read(_PrSystemMem->nMsgRead, &_cmd, sizeof(_cmd));
			if (12 == _cmd)
			{
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				if (_PrSystemMem->pSavedState != NULL)
				{
					free(_PrSystemMem->pSavedState);
					_PrSystemMem->pSavedState = NULL;
					_PrSystemMem->nSavedStateSize = 0;
				}
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
			}
			switch (_cmd)
			{
			case 0:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				if (_PrSystemMem->pInputQueue != NULL)
					AInputQueue_detachLooper(_PrSystemMem->pInputQueue);
				_PrSystemMem->pInputQueue = _PrSystemMem->pPendingInputQueue;
				if (_PrSystemMem->pInputQueue != NULL)
					AInputQueue_attachLooper(_PrSystemMem->pInputQueue, _PrSystemMem->pLooper, 2, NULL, NULL);
				pthread_cond_broadcast(&_PrSystemMem->sCond);
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				break;
			case 1:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				_PrSystemMem->pWindow = _PrSystemMem->pPendingWindow;
				pthread_cond_broadcast(&_PrSystemMem->sCond);
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				_PrSystemMem->bBackendState = TRUE;
				break;
			case 2:
				pthread_cond_broadcast(&_PrSystemMem->sCond);
				break;
			case 11:
			case 10:
			case 13:
			case 14:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				_PrSystemMem->nActivityState = _cmd;
				pthread_cond_broadcast(&_PrSystemMem->sCond);
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				break;
			case 8:
				AConfiguration_fromAssetManager(_PrSystemMem->pConfig, _PrSystemMem->pActivity->assetManager);
				break;
			case 15:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				_GbSystemRunning = FALSE;
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				break;
			}
			switch (_cmd)
			{
			case 2:
				_GbSystemRunning = 2;
				break;
			default:
				break;
			}
			switch (_cmd)
			{
			case 2:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				_PrSystemMem->pWindow = NULL;
				pthread_cond_broadcast(&_PrSystemMem->sCond);
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				break;
			case 12:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				_PrSystemMem->nStateSaved = 1;
				pthread_cond_broadcast(&_PrSystemMem->sCond);
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				break;
			case 11:
				pthread_mutex_lock(&_PrSystemMem->sMutex);
				if (_PrSystemMem->pSavedState != NULL)
				{
					free(_PrSystemMem->pSavedState);
					_PrSystemMem->pSavedState = NULL;
					_PrSystemMem->nSavedStateSize = 0;
				}
				pthread_mutex_unlock(&_PrSystemMem->sMutex);
				break;
			}
		}
		else
		{
			AInputEvent* _event = NULL;
			while (AInputQueue_getEvent(_PrSystemMem->pInputQueue, &_event) >= 0)
			{
				if (AInputQueue_preDispatchEvent(_PrSystemMem->pInputQueue, _event))
					continue;
                if (AINPUT_EVENT_TYPE_MOTION == AInputEvent_getType(_event))
                {
                    BLS32 _action = AMotionEvent_getAction(_event);
                    switch (_action & AMOTION_EVENT_ACTION_MASK)
                    {
                    case AMOTION_EVENT_ACTION_MOVE:
                        {
                            BLS32 _x = AMotionEvent_getX(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            BLS32 _y = AMotionEvent_getY(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_MOVE, NULL, INVALID_GUID);
                            AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
                        }
                        break;
                    case AMOTION_EVENT_ACTION_DOWN:
                        {
                            BLS32 _x = AMotionEvent_getX(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            BLS32 _y = AMotionEvent_getY(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_LDOWN, NULL, INVALID_GUID);
                            AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
                        }
                        break;
                    case AMOTION_EVENT_ACTION_POINTER_DOWN:
                        {
                            BLS32 _x = AMotionEvent_getX(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            BLS32 _y = AMotionEvent_getY(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_RDOWN, NULL, INVALID_GUID);
                            AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
                        }
                        break;
                    case AMOTION_EVENT_ACTION_UP:
                        {
                            BLS32 _x = AMotionEvent_getX(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            BLS32 _y = AMotionEvent_getY(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_LUP, NULL, INVALID_GUID);
                            AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
                        }
                        break;
                    case AMOTION_EVENT_ACTION_POINTER_UP:
                        {
                            BLS32 _x = AMotionEvent_getX(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            BLS32 _y = AMotionEvent_getY(_event, _action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_RUP, NULL, INVALID_GUID);
                            AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
                        }
                        break;
                    default:
                        AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 0);
                        break;
                    }
                }
                else if (AINPUT_EVENT_TYPE_KEY == AInputEvent_getType(_event))
                {
					BLS32 _action = AKeyEvent_getAction(_event);
					BLS32 _key = AKeyEvent_getKeyCode(_event);
					if ((_action == AKEY_EVENT_ACTION_DOWN || _action == AKEY_EVENT_ACTION_UP || _action == AKEY_EVENT_ACTION_MULTIPLE) && _key != AKEYCODE_VOLUME_UP && _key != AKEYCODE_VOLUME_DOWN)
					{
						BLEnum _code = SCANCODE_INTERNAL[_key];
						switch (_action)
						{
						case AKEY_EVENT_ACTION_DOWN:
							if (_code == BL_KC_BACKSPACE || _code == BL_KC_RETURN)
								blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
							else if(_code == BL_KC_AC_BACK)
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_ESCAPE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
							AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
							break;
						case AKEY_EVENT_ACTION_UP:
							if (_code == BL_KC_BACKSPACE || _code == BL_KC_RETURN)
								blInvokeEvent(BL_ET_KEY, MAKEU32(_code, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
							else if (_code == BL_KC_AC_BACK)
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_ESCAPE, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
							else
							{
								JNIEnv* _env = _PrSystemMem->pActivity->env;
								_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
								jlong _downtim = AKeyEvent_getDownTime(_event);
								jlong _eventtim = AKeyEvent_getEventTime(_event);
								jint _repeat = AKeyEvent_getRepeatCount(_event);
								jint _meta = AKeyEvent_getMetaState(_event);
								jint _device = AInputEvent_getDeviceId(_event);
								jint _scancode = AKeyEvent_getScanCode(_event);
								jint _flags = AKeyEvent_getFlags(_event);
								jint _source = AInputEvent_getSource(_event);
								jclass _kecls = _env->FindClass("android/view/KeyEvent");
								jmethodID _keconmid = _env->GetMethodID(_kecls, "<init>", "(JJIIIIIIII)V");
								jobject _keobj = _env->NewObject(_kecls, _keconmid, _downtim, _eventtim, _action, _key, _repeat, _meta, _device, _scancode, _flags, _source);
								jmethodID _unicodemid = _env->GetMethodID(_kecls, "getUnicodeChar", "(I)I");
								jint _unicode = _env->CallIntMethod(_keobj, _unicodemid, _meta);
								_env->DeleteLocalRef(_kecls);
								_env->DeleteLocalRef(_keobj);
								_PrSystemMem->pActivity->vm->DetachCurrentThread();
								BLUtf16 _tmp[2] = { _unicode, 0 };
								const BLUtf8* _utf8str = blGenUtf8Str(_tmp);
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, _utf8str, INVALID_GUID);
								blDeleteUtf8Str((BLUtf8*)_utf8str);
							}
							AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 1);
							break;
						default:
							AInputQueue_finishEvent(_PrSystemMem->pInputQueue, _event, 0);
							break;
						}
					}
                }
			}
		}
	}
}
BLVoid
_CloseWindow()
{
	JNIEnv* _env = _PrSystemMem->pActivity->env;
	_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
	_env->DeleteGlobalRef(_PrSystemMem->pBLJava);
	_PrSystemMem->pActivity->vm->DetachCurrentThread();
	_GpuAnitIntervention();
}
static BLVoid
_Start(ANativeActivity* _Activity) { _WndProc(10, NULL); }
static BLVoid
_Destroy(ANativeActivity* _Activity)
{
	int8_t _cmd = 15;
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	write(_PrSystemMem->nMsgWrite, &_cmd, sizeof(_cmd));
	if (_GbSystemRunning)
		pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
	close(_PrSystemMem->nMsgRead);
	close(_PrSystemMem->nMsgWrite);
	pthread_cond_destroy(&_PrSystemMem->sCond);
	pthread_mutex_destroy(&_PrSystemMem->sMutex);
	free(_PrSystemMem);
	_PrSystemMem = NULL;
}
static BLVoid
_Resume(ANativeActivity* _Activity){ _WndProc(11, NULL); _GbSystemRunning = 1; }
static BLVoid
_Pause(ANativeActivity* _Activity) { _WndProc(13, NULL); _GbSystemRunning = 2; }
static BLVoid
_Stop(ANativeActivity* _Activity) { _WndProc(14, NULL);}
static BLVoid
_LowMemory(ANativeActivity* _Activity) { _WndProc(9, NULL); }
static BLVoid
_WindowFocusChanged(ANativeActivity* _Activity, BLS32 _Focused) { _WndProc(_Focused ? 6 : 7, NULL); _GbSystemRunning = _Focused ? 1 : 2; }
static BLVoid
_ConfigurationChanged(ANativeActivity* _Activity) { _WndProc(8, NULL); }
static BLVoid
_NativeWindowCreated(ANativeActivity* _Activity, ANativeWindow* _Window) { _WndProc(21, _Window); }
static BLVoid
_NativeWindowDestroyed(ANativeActivity* _Activity, ANativeWindow* _Window) { _WndProc(22, NULL); }
static BLVoid
_InputQueueCreated(ANativeActivity* _Activity, AInputQueue* _Queue) { _WndProc(23, _Queue); }
static BLVoid
_InputQueueDestroyed(ANativeActivity* _Activity, AInputQueue* _Queue) { _WndProc(24, NULL); }
static BLVoid*
_SaveInstanceState(ANativeActivity* _Activity, size_t* _Len)
{
	BLVoid* _saved = NULL;
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	_PrSystemMem->nStateSaved = 0;
	_WndProc(12, NULL);
	while (!_PrSystemMem->nStateSaved)
		pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
	if (_PrSystemMem->pSavedState != NULL)
	{
		_saved = _PrSystemMem->pSavedState;
		*_Len = _PrSystemMem->nSavedStateSize;
		_PrSystemMem->pSavedState = NULL;
		_PrSystemMem->nSavedStateSize = 0;
	}
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
	return _saved;
}
static BLVoid*
_AppEntry(BLVoid* _Param)
{
	_PrSystemMem->pConfig = AConfiguration_new();
	AConfiguration_setOrientation(_PrSystemMem->pConfig, ACONFIGURATION_ORIENTATION);
	AConfiguration_setKeyboard(_PrSystemMem->pConfig, ACONFIGURATION_KEYBOARD);
	AConfiguration_setKeysHidden(_PrSystemMem->pConfig, ACONFIGURATION_KEYBOARD_HIDDEN);
	AConfiguration_setScreenSize(_PrSystemMem->pConfig, ACONFIGURATION_SCREEN_SIZE);
	AConfiguration_fromAssetManager(_PrSystemMem->pConfig, _PrSystemMem->pActivity->assetManager);
	ALooper* _looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
	ALooper_addFd(_looper, _PrSystemMem->nMsgRead, 1, ALOOPER_EVENT_INPUT, NULL, NULL);
	_PrSystemMem->pLooper = _looper;
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	_GbSystemRunning = TRUE;
	pthread_cond_broadcast(&_PrSystemMem->sCond);
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
	while (1)
	{
		ANativeWindow* _wnd;
		pthread_mutex_lock(&_PrSystemMem->sMutex);
		_wnd = _PrSystemMem->pWindow;
		pthread_mutex_unlock(&_PrSystemMem->sMutex);
		if (!_wnd)
			_PollMsg();
		else
			break;
	}
	blSystemRun(NULL, 84, 84, 84, 84, 1, 1, 1, 1, NULL, NULL, NULL);
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	if (_PrSystemMem->pSavedState != NULL)
	{
		free(_PrSystemMem->pSavedState);
		_PrSystemMem->pSavedState = NULL;
		_PrSystemMem->nSavedStateSize = 0;
	}
	if (_PrSystemMem->pInputQueue != NULL)
		AInputQueue_detachLooper(_PrSystemMem->pInputQueue);
	AConfiguration_delete(_PrSystemMem->pConfig);
	pthread_cond_broadcast(&_PrSystemMem->sCond);
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
	return NULL;
}
#elif defined(BL_PLATFORM_OSX)
@interface OSXDelegate : NSResponder <NSWindowDelegate> @end
@implementation OSXDelegate
- (BLVoid)flagsChanged:(NSEvent *)_Event{ }
- (BLVoid)keyDown:(NSEvent *)_Event{ }
- (BLVoid)keyUp:(NSEvent *)_Event{ }
- (BLVoid)doCommandBySelector:(SEL)_Selector{ }
- (BOOL)windowShouldClose:(id)_Sender{_GbSystemRunning = FALSE; return YES; }
- (BLVoid)windowDidMiniaturize:(NSNotification*)_Notification{ _GbSystemRunning = 2; }
- (BLVoid)windowDidDeminiaturize:(NSNotification*)_Notification{ _GbSystemRunning = 1; }
- (BLVoid)windowWillMove:(NSNotification*)_Notification{ _GbSystemRunning = 2; }
- (BLVoid)windowDidMove:(NSNotification*)_Notification{ _GbSystemRunning = 1; }
- (BLVoid)windowDidResize:(NSNotification *)_Notification
{
    NSRect _rect = [_PrSystemMem->pWindow contentRectForFrameRect:[_PrSystemMem->pWindow frame]];
    _PrSystemMem->sBoostParam.nScreenWidth = _rect.size.width;
    _PrSystemMem->sBoostParam.nScreenHeight = _rect.size.height;
    NSView* _view = [_PrSystemMem->pWindow contentView];
    NSTrackingArea* _area = [[_view trackingAreas] firstObject];
    [_view removeTrackingArea: _area];
    NSTrackingAreaOptions _options = NSTrackingMouseEnteredAndExited|NSTrackingActiveAlways|NSTrackingAssumeInside;
    _area = [[NSTrackingArea alloc] initWithRect:_view.bounds options:_options owner:_view userInfo:nil];
    [_view addTrackingArea:_area];
	blInvokeEvent(BL_ET_SYSTEM, BL_SE_RESOLUTION, 0, NULL, INVALID_GUID);
}
- (BLVoid)close
{
	blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
    _GbSystemRunning = FALSE;
    [_PrSystemMem->pWindow setDelegate:nil];
    if ([_PrSystemMem->pWindow nextResponder] == self)
        [_PrSystemMem->pWindow setNextResponder:nil];
    if ([[_PrSystemMem->pWindow contentView] nextResponder] == self)
        [[_PrSystemMem->pWindow contentView] setNextResponder:nil];
}
@end
@interface OSXWindow : NSWindow
- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;
@end
@implementation OSXWindow
- (BOOL)canBecomeKeyWindow { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }
@end
@interface OSXTextInput : NSTextView @end
@implementation OSXTextInput
- (BLVoid)doCommandBySelector:(SEL)_Selector{}
- (BOOL)canBecomeFirstResponder{ return YES; }
- (NSInteger)conversationIdentifier{ return (NSInteger) self; }
- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)_R actualRange:(NSRangePointer)_A{ return nil; }
- (NSUInteger)characterIndexForPoint:(NSPoint)_Point { return 0; }
- (NSArray *)validAttributesForMarkedText{ return [NSArray array]; }
- (BLVoid)insertText:(id)_String replacementRange:(NSRange)_ReplacementRange
{
    const BLUtf8* _str;
    if ([_String isKindOfClass: [NSAttributedString class]])
        _str = (const BLUtf8*)[[_String string] UTF8String];
    else
        _str = (const BLUtf8*)[_String UTF8String];
    blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, _str, INVALID_GUID);
}
- (BLVoid)insertText:(id)_String
{
    [self insertText:_String replacementRange:NSMakeRange(0, 0)];
}
- (NSRect)firstRectForCharacterRange:(NSRange)_Range actualRange:(NSRangePointer)_ActualRange;
{
    NSRect _rect;
    NSPoint _origin = [_PrSystemMem->pWindow frame].origin;
    NSSize _size = [_PrSystemMem->pWindow frame].size;
    _rect.origin.x = _origin.x + _PrSystemMem->sIMEpos.x;
    _rect.origin.y = _size.height + _origin.y - _PrSystemMem->sIMEpos.y - 20;
    _rect.size.width = 200;
    _rect.size.height = 20;
    return _rect;
}
@end
BLVoid
_EnterFullscreen()
{
    NSRect _rect;
    _rect.origin.x = 0;
    _rect.origin.y = 0;
    _rect.size.width = CGDisplayPixelsWide(kCGDirectMainDisplay);
    _rect.size.height = CGDisplayPixelsHigh(kCGDirectMainDisplay);
    [NSMenu setMenuBarVisible:NO];
    [_PrSystemMem->pWindow setStyleMask:NSBorderlessWindowMask];
    [_PrSystemMem->pWindow setFrame:[_PrSystemMem->pWindow frameRectForContentRect:_rect] display:NO];
}
BLVoid
_ExitFullscreen(BLU32 _Width, BLU32 _Height)
{
    NSRect _rect;
    _rect.origin.x = (CGDisplayPixelsWide(kCGDirectMainDisplay) - _Width)/2;
    _rect.origin.y = (CGDisplayPixelsHigh(kCGDirectMainDisplay) - _Height)/2;
    _rect.size.width = _Width;
    _rect.size.height = _Height;
    [NSMenu setMenuBarVisible:YES];
    [_PrSystemMem->pWindow setStyleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask];
    [_PrSystemMem->pWindow setFrame:[_PrSystemMem->pWindow frameRectForContentRect:_rect] display:YES];
    [_PrSystemMem->pWindow setTitle:[NSString stringWithUTF8String : (const BLAnsi*)_PrSystemMem->sBoostParam.pAppName]];
}
BLVoid
_ShowWindow()
{
    _PrSystemMem->pPool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    NSString* _appname = [NSString stringWithUTF8String : (const BLAnsi*)_PrSystemMem->sBoostParam.pAppName];
    NSMenu* _bar = [[NSMenu alloc] init];
    [NSApp setMainMenu:_bar];
    NSMenuItem* _item = [_bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* _menu = [[NSMenu alloc] init];
    [_item setSubmenu:_menu];
    [_menu addItemWithTitle:[NSString stringWithFormat:@"About %@", _appname] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    [_menu addItem:[NSMenuItem separatorItem]];
    NSMenu* _services = [[NSMenu alloc] init];
    [NSApp setServicesMenu:_services];
    [[_menu addItemWithTitle:@"Services" action:NULL keyEquivalent:@""] setSubmenu:_services];
    [_services release];
    [_menu addItem:[NSMenuItem separatorItem]];
    [_menu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", _appname] action:@selector(hide:) keyEquivalent:@"h"];
    [[_menu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"]
     setKeyEquivalentModifierMask:NSAlternateKeyMask | NSCommandKeyMask];
    [_menu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
    [_menu addItem:[NSMenuItem separatorItem]];
    [_menu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", _appname] action:@selector(terminate:) keyEquivalent:@"q"];
    NSMenuItem* _windowitem = [_bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    [_bar release];
    NSMenu* _winmenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [NSApp setWindowsMenu:_winmenu];
    [_windowitem setSubmenu:_winmenu];
    [_winmenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    [_winmenu addItemWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];
    [_winmenu addItem:[NSMenuItem separatorItem]];
    [_winmenu addItemWithTitle:@"Bring All to Front" action:@selector(arrangeInFront:) keyEquivalent:@""];
    [_winmenu addItem:[NSMenuItem separatorItem]];
    [[_winmenu addItemWithTitle:@"Enter Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"]
     setKeyEquivalentModifierMask:NSControlKeyMask | NSCommandKeyMask];
    SEL _selector = NSSelectorFromString(@"setAppleMenu:");
    [NSApp performSelector:_selector withObject:_menu];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp finishLaunching];
    NSRect _rect;
    BLU32 _style = 0;
    if (_PrSystemMem->sBoostParam.bFullscreen)
    {
        _style = NSBorderlessWindowMask;
        _rect.origin.x = 0;
        _rect.origin.y = 0;
        _rect.size.width = CGDisplayPixelsWide(kCGDirectMainDisplay);
        _rect.size.height = CGDisplayPixelsHigh(kCGDirectMainDisplay);
        [NSMenu setMenuBarVisible:NO];
    }
    else
    {
        _style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
        _rect.origin.x = (CGDisplayPixelsWide(kCGDirectMainDisplay) - _PrSystemMem->sBoostParam.nScreenWidth)/2;
        _rect.origin.y = (CGDisplayPixelsHigh(kCGDirectMainDisplay) - _PrSystemMem->sBoostParam.nScreenHeight)/2;
        _rect.size.width = _PrSystemMem->sBoostParam.nScreenWidth;
        _rect.size.height = _PrSystemMem->sBoostParam.nScreenHeight;
        [NSMenu setMenuBarVisible:YES];
    }
    _PrSystemMem->pWindow = [[OSXWindow alloc] initWithContentRect:_rect styleMask:_style backing:NSBackingStoreBuffered defer:NO screen:[NSScreen screens][0]];
    [_PrSystemMem->pWindow setTitle:_appname];
    [_PrSystemMem->pWindow setBackgroundColor:[NSColor blackColor]];
    [_PrSystemMem->pWindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    NSView* _view = [_PrSystemMem->pWindow contentView];
    _PrSystemMem->pDelegate = [[OSXDelegate alloc] init];
    [_PrSystemMem->pWindow setDelegate:_PrSystemMem->pDelegate];
    [_PrSystemMem->pWindow setNextResponder:_PrSystemMem->pDelegate];
    [_PrSystemMem->pWindow setAcceptsMouseMovedEvents:YES];
    [_PrSystemMem->pWindow setOneShot:NO];
    [_PrSystemMem->pWindow makeKeyAndOrderFront:nil];
    [_PrSystemMem->pWindow center];
    [_PrSystemMem->pWindow setAutodisplay:YES];
    [_PrSystemMem->pWindow setReleasedWhenClosed:NO];
    NSTrackingAreaOptions _options = NSTrackingMouseEnteredAndExited|NSTrackingActiveAlways|NSTrackingAssumeInside;
    NSTrackingArea* _area = [[NSTrackingArea alloc] initWithRect:_view.bounds options:_options owner:_view userInfo:nil];
    [_view addTrackingArea:_area];
    _GpuIntervention(_PrSystemMem->pDukContext, _view, _rect.size.width, _rect.size.height, !_PrSystemMem->sBoostParam.bProfiler);
#   ifndef DEBUG
    if (_PrSystemMem->sBoostParam.bFullscreen)
        [_PrSystemMem->pWindow setLevel:CGShieldingWindowLevel()];
    else
        [_PrSystemMem->pWindow setLevel:kCGNormalWindowLevel];
#   endif
}
BLVoid
_PollMsg()
{
    @autoreleasepool
    {
        while (1)
        {
            NSEvent* _event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES ];
            if (_event == nil)
                break;
            switch ([_event type])
            {
				case NSMouseEntered:
				{
					if (_UseCustomCursor())
						[NSCursor hide];
				}
                break;
				case NSMouseExited:
				{
					[NSCursor unhide];
				}
                break;
				case NSLeftMouseDown:
				{
					if ([_event window])
					{
						if (NSPointInRect([_event locationInWindow], [[[_event window] contentView] frame]))
						{
							NSPoint _pos = [NSEvent mouseLocation];
							NSPoint _origin = [_PrSystemMem->pWindow frame].origin;
							BLU32 _x = _pos.x - _origin.x;
							BLU32 _y = _PrSystemMem->sBoostParam.nScreenHeight - _pos.y + _origin.y;
							blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_LDOWN, NULL, INVALID_GUID);
						}
					}
				}
                break;
				case NSRightMouseDown:
				{
					if ([_event window])
					{
						if (NSPointInRect([_event locationInWindow], [[[_event window] contentView] frame]))
						{
							NSPoint _pos = [NSEvent mouseLocation];
							NSPoint _origin = [_PrSystemMem->pWindow frame].origin;
							BLU32 _x = _pos.x - _origin.x;
							BLU32 _y = _PrSystemMem->sBoostParam.nScreenHeight - _pos.y + _origin.y;
							blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_RDOWN, NULL, INVALID_GUID);
						}
					}
				}
                break;
				case NSLeftMouseUp:
				{
					if ([_event window])
					{
						if (NSPointInRect([_event locationInWindow], [[[_event window] contentView] frame]))
						{
							NSPoint _pos = [NSEvent mouseLocation];
							NSPoint _origin = [_PrSystemMem->pWindow frame].origin;
							BLU32 _x = _pos.x - _origin.x;
							BLU32 _y = _PrSystemMem->sBoostParam.nScreenHeight - _pos.y + _origin.y;
							blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_LUP, NULL, INVALID_GUID);
						}
					}
				}
                break;
				case NSRightMouseUp:
				{
					if ([_event window])
					{
						if (NSPointInRect([_event locationInWindow], [[[_event window] contentView] frame]))
						{
							NSPoint _pos = [NSEvent mouseLocation];
							NSPoint _origin = [_PrSystemMem->pWindow frame].origin;
							BLU32 _x = _pos.x - _origin.x;
							BLU32 _y = _PrSystemMem->sBoostParam.nScreenHeight - _pos.y + _origin.y;
							blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_RUP, NULL, INVALID_GUID);
						}
					}
				}
                break;
				case NSMouseMoved:
				{
					if ([_event window])
					{
						if (NSPointInRect([_event locationInWindow], [[[_event window] contentView] frame]))
						{
							NSPoint _pos = [NSEvent mouseLocation];
							NSPoint _origin = [_PrSystemMem->pWindow frame].origin;
							BLU32 _x = _pos.x - _origin.x;
							BLU32 _y = _PrSystemMem->sBoostParam.nScreenHeight - _pos.y + _origin.y;
							blInvokeEvent(BL_ET_MOUSE, MAKEU32(_y, _x), BL_ME_MOVE, NULL, INVALID_GUID);
						}
					}
				}
                break;
				case NSScrollWheel:
				{
					BLF32 _delta;
					if (floor(NSAppKitVersionNumber) >= NSAppKitVersionNumber10_7)
					{
						_delta = [_event scrollingDeltaY];
						if ([_event hasPreciseScrollingDeltas])
							_delta *= 0.1f;
					}
					else
						_delta = [_event deltaY];
					_delta *= 10;
					if (_delta < 0)
						blInvokeEvent(BL_ET_MOUSE, MAKEU32(1, (BLS32)-_delta), BL_ME_WHEEL, NULL, INVALID_GUID);
					else
						blInvokeEvent(BL_ET_MOUSE, MAKEU32(0, (BLS32)_delta), BL_ME_WHEEL, NULL, INVALID_GUID);
				}
                break;
				case NSKeyDown:
				{
					BLEnum _code = SCANCODE_INTERNAL[[_event keyCode]];
					if (_PrSystemMem->bCtrlPressed)
					{
						switch (_code)
						{
							case BL_KC_A:
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_SELECT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
								break;
							case BL_KC_C:
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_COPY, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
								break;
							case BL_KC_X:
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_CUT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
								break;
							case BL_KC_V:
								blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_PASTE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
								break;
							default:
								blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
								break;
						}
					}
					else
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
				}
                break;
				case NSKeyUp:
				{
					blInvokeEvent(BL_ET_KEY, MAKEU32(SCANCODE_INTERNAL[[_event keyCode]], FALSE), BL_ET_KEY, NULL, INVALID_GUID);
				}
                break;
				case NSFlagsChanged:
				{
					BLEnum _code = SCANCODE_INTERNAL[[_event keyCode]];
					NSEventModifierFlags _flags = [_event modifierFlags];
					if (_code == BL_KC_CAPSLOCK)
					{
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
						break;
					}
					if (_flags & NSFunctionKeyMask)
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					else if (_flags & NSAlternateKeyMask)
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					else if (_flags & NSControlKeyMask)
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					else if (_flags & NSShiftKeyMask)
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					else if (_flags & NSNumericPadKeyMask)
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					else if (_flags & NSHelpKeyMask)
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					else if (_flags & NSCommandKeyMask)
					{
						_PrSystemMem->bCtrlPressed = TRUE;
						blInvokeEvent(BL_ET_KEY, MAKEU32(_code, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
					}
					else
					{
						if (!(_flags & NSAlphaShiftKeyMask))
						{
							if (_code == BL_KC_LGUI || _code == BL_KC_RGUI)
								_PrSystemMem->bCtrlPressed = FALSE;
							blInvokeEvent(BL_ET_KEY, MAKEU32(_code, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
						}
						else
							blInvokeEvent(BL_ET_KEY, MAKEU32(_code, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
					}
				}
                break;
            default:
                break;
            }
            [NSApp sendEvent:_event];
        }
    }
}
BLVoid
_CloseWindow()
{
    _GpuAnitIntervention();
    [_PrSystemMem->pTICcxt release];
    [NSMenu setMenuBarVisible:YES];
    [_PrSystemMem->pWindow release];
    [_PrSystemMem->pPool release];
}
#elif defined(BL_PLATFORM_IOS)
@interface IOSWindow : UIWindow
- (BLVoid)layoutSubviews;
@end
@implementation IOSWindow
- (BLVoid)layoutSubviews
{
    self.frame = self.screen.bounds;
    [super layoutSubviews];
}
@end
@interface IOSView : UIView
- (instancetype)initWithFrame:(CGRect)_Frame;
- (CGPoint)touchLocation:(UITouch*)_Touch shouldNormalize:(BOOL)_Normalize;
- (BLVoid)touchesBegan:(NSSet*)_Touches withEvent:(UIEvent*)_Event;
- (BLVoid)touchesEnded:(NSSet*)_Touches withEvent:(UIEvent*)_Event;
- (BLVoid)touchesMoved:(NSSet*)_Touches withEvent:(UIEvent*)_Event;
@end
@implementation IOSView{
    UITouch* _FFDown;
}
- (instancetype)initWithFrame:(CGRect)_Frame
{
    if ((self = [super initWithFrame:_Frame]))
    {
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        self.autoresizesSubviews = YES;
        self.multipleTouchEnabled = YES;
    }
    return self;
}
- (CGPoint)touchLocation:(UITouch*)_Touch shouldNormalize:(BOOL)_Normalize
{
    CGPoint _point = [_Touch locationInView:self];
    if (_Normalize)
    {
        CGRect _bounds = self.bounds;
        _point.x /= _bounds.size.width;
        _point.y /= _bounds.size.height;
    }
    return _point;
}
- (BLVoid)touchesBegan:(NSSet*)_Touches withEvent:(UIEvent*)_Event
{
    for (UITouch* _touch in _Touches)
    {
        if (!_FFDown)
            _FFDown = _touch;
        if (_FFDown == _touch)
        {
            CGPoint _location = [self touchLocation:_touch shouldNormalize:NO];
            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_location.y * _PrSystemMem->nRetinaScale, _location.x * _PrSystemMem->nRetinaScale), BL_ME_LDOWN, NULL, INVALID_GUID);
        }
        else
        {
            CGPoint _location = [self touchLocation:_touch shouldNormalize:YES];
            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_location.y * _PrSystemMem->nRetinaScale, _location.x * _PrSystemMem->nRetinaScale), BL_ME_RDOWN, NULL, INVALID_GUID);
        }
    }
}
- (BLVoid)touchesEnded:(NSSet*)_Touches withEvent:(UIEvent*)_Event
{
    for (UITouch* _touch in _Touches)
    {
        if (_touch == _FFDown)
        {
            CGPoint _location = [self touchLocation:_touch shouldNormalize:NO];
            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_location.y * _PrSystemMem->nRetinaScale, _location.x * _PrSystemMem->nRetinaScale), BL_ME_LUP, NULL, INVALID_GUID);
            _FFDown = nil;
        }
        else
        {
            CGPoint _location = [self touchLocation:_touch shouldNormalize:YES];
            blInvokeEvent(BL_ET_MOUSE, MAKEU32(_location.y * _PrSystemMem->nRetinaScale, _location.x * _PrSystemMem->nRetinaScale), BL_ME_RUP, NULL, INVALID_GUID);
        }
    }
}
- (BLVoid)touchesCancelled:(NSSet*)_Touches withEvent:(UIEvent*)_Event
{
    [self touchesEnded:_Touches withEvent:_Event];
}
- (BLVoid)touchesMoved:(NSSet*)_Touches withEvent:(UIEvent*)_Event
{
    for (UITouch* _touch in _Touches)
    {
        CGPoint _location = [self touchLocation:_touch shouldNormalize:NO];
        blInvokeEvent(BL_ET_MOUSE, MAKEU32(_location.y * _PrSystemMem->nRetinaScale, _location.x * _PrSystemMem->nRetinaScale), BL_ME_MOVE, NULL, INVALID_GUID);
    }
}
@end
@interface IOSLaunchController : UIViewController
- (instancetype)init;
- (BLVoid)loadView;
- (NSUInteger)supportedInterfaceOrientations;
@end
@implementation IOSLaunchController
- (BLVoid)loadView{}
- (BOOL)shouldAutorotate{ return NO; }
- (NSUInteger)supportedInterfaceOrientations{ return UIInterfaceOrientationMaskAll; }
- (instancetype)init
{
    if (!(self = [super initWithNibName:nil bundle:nil]))
        return nil;
    NSBundle* _bundle = [NSBundle mainBundle];
    NSString* _screenname = [_bundle objectForInfoDictionaryKey:@"UILaunchStoryboardName"];
    BOOL _atleast8 = [[UIDevice currentDevice].systemVersion doubleValue] >= (8.0);
    if (_screenname && _atleast8)
    {
        @try
        {
            self.view = [_bundle loadNibNamed:_screenname owner:self options:nil][0];
        }
        @catch (NSException* _exception)
        {
            return nil;
        }
    }
    if (!self.view)
    {
        NSArray* _launches = [_bundle objectForInfoDictionaryKey:@"UILaunchImages"];
        UIInterfaceOrientation _curorient = [UIApplication sharedApplication].statusBarOrientation;
        NSString* _imagename = nil;
        UIImage* _image = nil;
        BLU32 _screenw = (BLU32)([UIScreen mainScreen].bounds.size.width + 0.5);
        BLU32 _screenh = (BLU32)([UIScreen mainScreen].bounds.size.height + 0.5);
        if (_screenw > _screenh)
        {
            BLU32 _width = _screenw;
            _screenw = _screenh;
            _screenh = _width;
        }
        if (_launches)
        {
            for (NSDictionary* _dict in _launches)
            {
                UIInterfaceOrientationMask _orientmask = UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown;
                NSString* _minversion = _dict[@"UILaunchImageMinimumOSVersion"];
                NSString* _sizestring = _dict[@"UILaunchImageSize"];
                NSString* _orientstring = _dict[@"UILaunchImageOrientation"];
                if (_minversion && [[UIDevice currentDevice].systemVersion doubleValue] < _minversion.doubleValue)
                    continue;
                if (_sizestring)
                {
                    CGSize _size = CGSizeFromString(_sizestring);
                    if ((BLU32)(_size.width + 0.5) != _screenw || (BLU32)(_size.height + 0.5) != _screenh)
                        continue;
                }
                if (_orientstring)
                {
                    if ([_orientstring isEqualToString:@"PortraitUpsideDown"])
                        _orientmask = UIInterfaceOrientationMaskPortraitUpsideDown;
                    else if ([_orientstring isEqualToString:@"Landscape"])
                        _orientmask = UIInterfaceOrientationMaskLandscape;
                    else if ([_orientstring isEqualToString:@"LandscapeLeft"])
                        _orientmask = UIInterfaceOrientationMaskLandscapeLeft;
                    else if ([_orientstring isEqualToString:@"LandscapeRight"])
                        _orientmask = UIInterfaceOrientationMaskLandscapeRight;
                }
                if ((_orientmask & (1 << _curorient)) == 0)
                    continue;
                _imagename = _dict[@"UILaunchImageName"];
            }
            if (_imagename)
                _image = [UIImage imageNamed:_imagename];
        }
        else
        {
            _imagename = [_bundle objectForInfoDictionaryKey:@"UILaunchImageFile"];
            if (!_image)
                _imagename = @"Default";
            UIInterfaceOrientation _curorient = [UIApplication sharedApplication].statusBarOrientation;
            UIUserInterfaceIdiom _idiom = [UIDevice currentDevice].userInterfaceIdiom;
            if (_idiom == UIUserInterfaceIdiomPhone && _screenh == 568)
                _image = [UIImage imageNamed:[NSString stringWithFormat:@"%@-568h", _imagename]];
            else if (_idiom == UIUserInterfaceIdiomPad)
            {
                if (UIInterfaceOrientationIsLandscape(_curorient))
                {
                    if (_curorient == UIInterfaceOrientationLandscapeLeft)
                        _image = [UIImage imageNamed:[NSString stringWithFormat:@"%@-LandscapeLeft", _imagename]];
                    else
                        _image = [UIImage imageNamed:[NSString stringWithFormat:@"%@-LandscapeRight", _imagename]];
                    if (!_image)
                        _image = [UIImage imageNamed:[NSString stringWithFormat:@"%@-Landscape", _imagename]];
                }
                else
                {
                    if (_curorient == UIInterfaceOrientationPortraitUpsideDown)
                        _image = [UIImage imageNamed:[NSString stringWithFormat:@"%@-PortraitUpsideDown", _imagename]];
                    if (!_image)
                        _image = [UIImage imageNamed:[NSString stringWithFormat:@"%@-Portrait", _imagename]];
                }
            }
            if (!_image)
                _image = [UIImage imageNamed:_imagename];
        }
        if (_image)
        {
            UIImageView* _imgview = [[UIImageView alloc] initWithFrame:[UIScreen mainScreen].bounds];
            UIImageOrientation _imageorient = UIImageOrientationUp;
            if (UIInterfaceOrientationIsLandscape(_curorient))
            {
                if (_atleast8 && _image.size.width < _image.size.height)
                {
                    if (_curorient == UIInterfaceOrientationLandscapeLeft)
                        _imageorient = UIImageOrientationRight;
                    else if (_curorient == UIInterfaceOrientationLandscapeRight)
                        _imageorient = UIImageOrientationLeft;
                }
                else if (!_atleast8 && _image.size.width > _image.size.height)
                {
                    if (_curorient == UIInterfaceOrientationLandscapeLeft)
                        _imageorient = UIImageOrientationLeft;
                    else if (_curorient == UIInterfaceOrientationLandscapeRight)
                        _imageorient = UIImageOrientationRight;
                }
            }
            _imgview.image = [[UIImage alloc] initWithCGImage:_image.CGImage scale:_image.scale orientation:_imageorient];
            self.view = _imgview;
        }
    }
    return self;
}
@end
@interface IOSController : UIViewController <UITextFieldDelegate>
- (instancetype)initWithNil;
- (BLVoid)loadView;
- (BLVoid)viewDidLayoutSubviews;
- (NSUInteger)supportedInterfaceOrientations;
- (BOOL)prefersStatusBarHidden;
- (BLVoid)initKeyboard;
- (BLVoid)deinitKeyboard;
- (BLVoid)keyboardWillShow:(NSNotification*)_Notification;
- (BLVoid)keyboardWillHide:(NSNotification*)_Notification;
- (BLVoid)updateKeyboard;
@end
@implementation IOSController{
    dispatch_source_t _timer;
}
- (instancetype)initWithNil
{
    [super initWithNibName:nil bundle:nil];
    [self initKeyboard];
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0);
    _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
    dispatch_source_set_timer(_timer, DISPATCH_TIME_NOW, 0.0001 * NSEC_PER_SEC, 0.0001 * NSEC_PER_SEC);
    dispatch_source_set_event_handler(_timer, ^{
        _SystemStep();
    });
    dispatch_resume(_timer);
    return self;
}
- (BLVoid)dealloc
{
    dispatch_source_cancel(_timer);
    [self deinitKeyboard];
    [super dealloc];
}
- (BLVoid)loadView{}
- (BOOL)prefersStatusBarHidden{ return YES; }
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)_Orientation
{
    return ([self supportedInterfaceOrientations] & (1 << _Orientation)) != 0;
}
- (NSUInteger)supportedInterfaceOrientations
{
    NSUInteger _mask = 0;
    if ([[UIDevice currentDevice].systemVersion doubleValue] >= 7.0)
    {
        if (_PrSystemMem->nOrientation == SCREEN_LANDSCAPE_INTERNAL)
            _mask |= UIInterfaceOrientationMaskLandscape;
        else
            _mask |= (UIInterfaceOrientationMaskPortrait);
    }
    else
    {
        if (_PrSystemMem->nOrientation == SCREEN_LANDSCAPE_INTERNAL)
            _mask |= UIInterfaceOrientationMaskLandscape;
        else
            _mask |= (UIInterfaceOrientationMaskPortrait);
    }
    return _mask;
}
- (BLVoid)viewDidLayoutSubviews
{
    const CGSize _size = self.view.bounds.size;
    _PrSystemMem->sBoostParam.nScreenWidth = _size.width * 2;
    _PrSystemMem->sBoostParam.nScreenHeight = _size.height * 2;
}
- (BLVoid)doLoop:(CADisplayLink*)_Sender
{
    _SystemStep();
}
- (BLVoid)initKeyboard
{
    _PrSystemMem->pTICcxt = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
    _PrSystemMem->pTICcxt.delegate = self;
    _PrSystemMem->pTICcxt.text = @" ";
    _PrSystemMem->pTICcxt.autocapitalizationType = UITextAutocapitalizationTypeNone;
    _PrSystemMem->pTICcxt.autocorrectionType = UITextAutocorrectionTypeNo;
    _PrSystemMem->pTICcxt.enablesReturnKeyAutomatically = NO;
    _PrSystemMem->pTICcxt.keyboardAppearance = UIKeyboardAppearanceDefault;
    _PrSystemMem->pTICcxt.keyboardType = UIKeyboardTypeDefault;
    _PrSystemMem->pTICcxt.returnKeyType = UIReturnKeyDefault;
    _PrSystemMem->pTICcxt.secureTextEntry = NO;
    _PrSystemMem->pTICcxt.hidden = YES;
    NSNotificationCenter* _center = [NSNotificationCenter defaultCenter];
    [_center addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [_center addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
}
- (BLVoid)deinitKeyboard
{
    NSNotificationCenter* _center = [NSNotificationCenter defaultCenter];
    [_center removeObserver:self name:UIKeyboardWillShowNotification object:nil];
    [_center removeObserver:self name:UIKeyboardWillHideNotification object:nil];
}
- (BLVoid)setView:(UIView*)_View
{
    [super setView:_View];
    [_View addSubview:_PrSystemMem->pTICcxt];
}
- (BLVoid)keyboardWillShow:(NSNotification*)_Notification
{
    CGRect _kbrect = [[_Notification userInfo][UIKeyboardFrameBeginUserInfoKey] CGRectValue];
    _kbrect = [self.view convertRect:_kbrect fromView:nil];
    [self setKeyboardHeight:(int)_kbrect.size.height];
}
- (BLVoid)keyboardWillHide:(NSNotification *)_Notification
{
    [self setKeyboardHeight:0];
}
- (BLVoid)updateKeyboard
{
    CGAffineTransform _t = self.view.transform;
    CGPoint _offset = CGPointMake(0.0, 0.0);
    CGRect _frame = self.view.window.screen.bounds;
    if (_PrSystemMem->nKeyboardHeight)
    {
        int _rectbottom = 0;
        int _keybottom = self.view.bounds.size.height - _PrSystemMem->nKeyboardHeight;
        if (_keybottom < _rectbottom)
            _offset.y = _keybottom - _rectbottom;
    }
    _t.tx = 0.0;
    _t.ty = 0.0;
    _offset = CGPointApplyAffineTransform(_offset, _t);
    _frame.origin.x += _offset.x;
    _frame.origin.y += _offset.y;
    self.view.frame = _frame;
}
- (BLVoid)setKeyboardHeight:(int)_Height
{
    _PrSystemMem->nKeyboardHeight = _Height;
    [self updateKeyboard];
}
- (BOOL)textField:(UITextField*)_TextField shouldChangeCharactersInRange:(NSRange)_Range replacementString:(NSString*)_String
{
    NSUInteger _len = _String.length;
    if (_len == 0)
    {
        blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_BACKSPACE, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
        blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_BACKSPACE, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
    }
    else
        blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_UNKNOWN, FALSE), BL_ET_KEY, [_String UTF8String], INVALID_GUID);
    return NO;
}
- (BOOL)textFieldShouldReturn:(UITextField*)_TextField
{
    blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_RETURN, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
    blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_RETURN, FALSE), BL_ET_KEY, NULL, INVALID_GUID);
    [_PrSystemMem->pTICcxt resignFirstResponder];
    return YES;
}
@end
@interface IOSDelegate : UIResponder <UIApplicationDelegate>
+ (NSString*)getAppDelegateClassName;
@end
@implementation IOSDelegate{
    UIWindow* _launch;
}
+ (NSString*)getAppDelegateClassName
{
    return @"IOSDelegate";
}
- (BLVoid)hideLaunchScreen
{
    _launch = nil;
    [UIView animateWithDuration:0.0 animations:^
    {
        _launch.alpha = 0.0;
    } completion:^(BOOL _Finished) {
        _launch.hidden = YES;
    }];
}
- (BLVoid)postFinishLaunch
{
    [self performSelector:@selector(hideLaunchScreen) withObject:nil afterDelay:0.0];
    CGRect _frame = [[UIScreen mainScreen] bounds];
    BLF32 _maxv = MAX_INTERNAL(_frame.size.width, _frame.size.height);
    BLF32 _minv = MIN_INTERNAL(_frame.size.width, _frame.size.height);
    if (_PrSystemMem->nOrientation == SCREEN_LANDSCAPE_INTERNAL)
    {
        _frame.size.width = _maxv;
        _frame.size.height = _minv;
    }
    else
    {
        _frame.size.width = _minv;
        _frame.size.height = _maxv;
    }
    _PrSystemMem->sBoostParam.bFullscreen = TRUE;
    _PrSystemMem->sBoostParam.nScreenWidth = _frame.size.width * _PrSystemMem->nRetinaScale;
    _PrSystemMem->sBoostParam.nScreenHeight = _frame.size.height * _PrSystemMem->nRetinaScale;
    _PrSystemMem->pWindow = [[IOSWindow alloc] initWithFrame:_frame];
    IOSController* _controller = [[IOSController alloc] initWithNil];
    _PrSystemMem->pCtlView = [[IOSView alloc] initWithFrame:_frame];
    [_controller.view removeFromSuperview];
    [_controller setView:_PrSystemMem->pCtlView];
    _PrSystemMem->pWindow.rootViewController = nil;
    _PrSystemMem->pWindow.rootViewController = _controller;
    [_PrSystemMem->pWindow layoutIfNeeded];
    [_PrSystemMem->pWindow makeKeyAndVisible];
    _GbSystemRunning = TRUE;
    _PrSystemMem->pBeginFunc();
    if (_launch)
    {
        _launch.hidden = YES;
        _launch = nil;
    }
}
- (BOOL)application:(UIApplication*)_App didFinishLaunchingWithOptions:(NSDictionary*)_Opt
{
    [UIApplication sharedApplication].statusBarHidden = YES;
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    id _data = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSHighResolutionCapable"];
    if(_data && [_data boolValue])
        _PrSystemMem->nRetinaScale = [[UIScreen mainScreen] scale];
    else
        _PrSystemMem->nRetinaScale = 1;
    UIViewController* _launchcontroller = [[IOSLaunchController alloc] init];
    if (_launchcontroller.view)
    {
        _launch = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
        _launch.windowLevel = UIWindowLevelNormal + 1.0;
        _launch.hidden = NO;
        _launch.rootViewController = _launchcontroller;
    }
    [[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];
    [self performSelector:@selector(postFinishLaunch) withObject:nil afterDelay:0.0];
    return YES;
}
- (BLVoid)applicationDidBecomeActive:(UIApplication*)_App { _GbSystemRunning = 1; }
- (BLVoid)applicationDidEnterBackground:(UIApplication*)_App { _GbSystemRunning = 2; }
- (BLVoid)applicationWillTerminate:(UIApplication*)_App
{
	blInvokeEvent(BL_ET_KEY, MAKEU32(BL_KC_EXIT, TRUE), BL_ET_KEY, NULL, INVALID_GUID);
    _GbSystemRunning = FALSE;
    _SystemDestroy();
}
@end
BLVoid
_ShowWindow()
{
	if (_PrSystemMem->sBoostParam.nScreenWidth > _PrSystemMem->sBoostParam.nScreenHeight)
		_PrSystemMem->nOrientation = SCREEN_LANDSCAPE_INTERNAL;
	else
		_PrSystemMem->nOrientation = SCREEN_PORTRAIT_INTERNAL;
    _PrSystemMem->pPool = [[NSAutoreleasePool alloc] init];
    UIApplicationMain(0, nil, nil, [IOSDelegate getAppDelegateClassName]);
}
BLVoid
_PollMsg()
{
    SInt32 _result;
    do {
        _result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.000001, TRUE);
    } while (_result == kCFRunLoopRunHandledSource);
}
BLVoid
_CloseWindow()
{
    [_PrSystemMem->pCtlView release];
    [_PrSystemMem->pTICcxt release];
    [_PrSystemMem->pWindow release];
    [_PrSystemMem->pPool release];
}
#endif
static BLVoid
_PollEvent()
{
#ifdef BL_PLATFORM_ANDROID
	BLS32 _busy;
	_busy = pthread_mutex_trylock(&_PrSystemMem->sMutex);
#endif
    for (BLU32 _idx = 0; _idx < _PrSystemMem->nEventIdx; ++_idx)
    {
        switch (_PrSystemMem->pEvents[_idx].eType)
        {
            case BL_ET_NET:
            {
				if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][0])
				{
					BLU32 _fidx = 0;
					while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
					{
						if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](BL_ET_NET, _PrSystemMem->pEvents[_idx].uEvent.sNet.nID, _PrSystemMem->pEvents[_idx].uEvent.sNet.nLength, _PrSystemMem->pEvents[_idx].uEvent.sNet.pBuf, INVALID_GUID))
							break;
						++_fidx;
					}
				}
                if (_PrSystemMem->pEvents[_idx].uEvent.sNet.pBuf)
                    free((BLVoid*)_PrSystemMem->pEvents[_idx].uEvent.sNet.pBuf);
            } break;
            case BL_ET_UI:
            {
				if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][0])
				{
					BLU32 _fidx = 0;
					while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
					{
						if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](BL_ET_UI, _PrSystemMem->pEvents[_idx].uEvent.sUI.nUParam, _PrSystemMem->pEvents[_idx].uEvent.sUI.nSParam, _PrSystemMem->pEvents[_idx].uEvent.sUI.pPParam, _PrSystemMem->pEvents[_idx].uEvent.sUI.nID))
							break;
						_fidx++;
					}
				}
            } break;
            case BL_ET_MOUSE:
			{
				if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][0])
				{
					if (BL_ME_WHEEL == _PrSystemMem->pEvents[_idx].uEvent.sMouse.eEvent)
					{
						BLU32 _fidx = 0;
						while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
						{
							if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](BL_ME_WHEEL, _PrSystemMem->pEvents[_idx].uEvent.sMouse.nWheel, _PrSystemMem->pEvents[_idx].uEvent.sMouse.eEvent, NULL, INVALID_GUID))
								break;
							_fidx++;
						}
					}
					else
					{
						BLU32 _fidx = 0;
						while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
						{
							if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](_PrSystemMem->pEvents[_idx].uEvent.sMouse.eEvent, MAKEU32(_PrSystemMem->pEvents[_idx].uEvent.sMouse.nY, _PrSystemMem->pEvents[_idx].uEvent.sMouse.nX), _PrSystemMem->pEvents[_idx].uEvent.sMouse.eEvent, NULL, INVALID_GUID))
								break;
							_fidx++;
						}
					}
				}
            } break;
            case BL_ET_KEY:
			{
				if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][0])
				{
					BLU32 _fidx = 0;
					while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
					{
						if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](BL_ET_KEY, MAKEU32(_PrSystemMem->pEvents[_idx].uEvent.sKey.eCode, _PrSystemMem->pEvents[_idx].uEvent.sKey.bPressed), 0, _PrSystemMem->pEvents[_idx].uEvent.sKey.pString, INVALID_GUID))
							break;
						_fidx++;
					}
				}
                if (_PrSystemMem->pEvents[_idx].uEvent.sKey.pString)
                    free((BLVoid*)_PrSystemMem->pEvents[_idx].uEvent.sKey.pString);
            } break;
            case BL_ET_SYSTEM:
			{
				if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][0])
				{
					BLU32 _fidx = 0;
					while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
					{
						if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](BL_ET_SYSTEM, _PrSystemMem->pEvents[_idx].uEvent.sSys.nUParam, _PrSystemMem->pEvents[_idx].uEvent.sSys.nSParam, _PrSystemMem->pEvents[_idx].uEvent.sSys.pPParam, INVALID_GUID))
							break;
						_fidx++;
					}
				}
            } break;
			case BL_ET_SPRITE:
			{
				if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][0])
				{
					BLU32 _fidx = 0;
					while (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx])
					{
						if (_PrSystemMem->pSubscriber[_PrSystemMem->pEvents[_idx].eType][_fidx](BL_ET_SPRITE, _PrSystemMem->pEvents[_idx].uEvent.sSprite.nUParam, _PrSystemMem->pEvents[_idx].uEvent.sSprite.nSParam, NULL, _PrSystemMem->pEvents[_idx].uEvent.sSprite.nID))
							break;
						_fidx++;
					}
				}
			} break;
            default:break;
        }
    }
    _PrSystemMem->nEventIdx = 0;
    _PrSystemMem->nEventsSz = 0;
#ifdef BL_PLATFORM_ANDROID
	if (!_busy)
		pthread_mutex_unlock(&_PrSystemMem->sMutex);
#endif
}
BLVoid
_SystemInit()
{
	_PrSystemMem->nSysTime = blTickCounts();
	_UtilsInit(_PrSystemMem->pDukContext);
#ifdef BL_PLATFORM_ANDROID
	_StreamIOInit(_PrSystemMem->pDukContext, _PrSystemMem->pActivity->assetManager);
#else
	_StreamIOInit(_PrSystemMem->pDukContext, NULL);
#endif
	if (_PrSystemMem->pDukContext)
	{
		BLAnsi _tmpname[260] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		strcpy_s(_tmpname, 260, blWorkingDir());
		strcat_s(_tmpname, 260, "main.js");
#else
		strcpy(_tmpname, blWorkingDir());
		strcat(_tmpname, "main.js");
#endif
		duk_push_c_function(_PrSystemMem->pDukContext, _JSExport, 0);
		duk_call(_PrSystemMem->pDukContext, 0);
		duk_put_global_string(_PrSystemMem->pDukContext, "bulllord");
		BLGuid _stream = blGenStream(_tmpname);
		BLAnsi* _data = (BLAnsi*)malloc(blStreamLength(_stream) + 1);
		blStreamRead(_stream, blStreamLength(_stream), _data);
		_data[blStreamLength(_stream)] = 0;
		duk_peval_string(_PrSystemMem->pDukContext, _data);
		duk_get_global_string(_PrSystemMem->pDukContext, "APPNAME");
		strcpy((BLAnsi*)_PrSystemMem->sBoostParam.pAppName, (const BLAnsi*)duk_to_string(_PrSystemMem->pDukContext, -1));
		duk_pop(_PrSystemMem->pDukContext);
		duk_get_global_string(_PrSystemMem->pDukContext, "USE_DESIGN_RES");
		_PrSystemMem->sBoostParam.bUseDesignRes = duk_to_boolean(_PrSystemMem->pDukContext, -1);
		duk_pop(_PrSystemMem->pDukContext);
		duk_get_global_string(_PrSystemMem->pDukContext, "DESIGN_WIDTH");
		_PrSystemMem->sBoostParam.nDesignWidth = duk_to_int(_PrSystemMem->pDukContext, -1);
		duk_pop(_PrSystemMem->pDukContext);
		duk_get_global_string(_PrSystemMem->pDukContext, "DESIGN_HEIGHT");
		_PrSystemMem->sBoostParam.nDesignHeight = duk_to_int(_PrSystemMem->pDukContext, -1);
		duk_pop(_PrSystemMem->pDukContext);
		duk_get_global_string(_PrSystemMem->pDukContext, "PROFILER");
		_PrSystemMem->sBoostParam.bProfiler = duk_to_boolean(_PrSystemMem->pDukContext, -1);
		duk_pop(_PrSystemMem->pDukContext);
		if (_PrSystemMem->sBoostParam.nScreenWidth + _PrSystemMem->sBoostParam.nScreenHeight == 0)
		{
			duk_get_global_string(_PrSystemMem->pDukContext, "SCREEN_WIDTH_DEFAULT");
			_PrSystemMem->sBoostParam.nScreenWidth = duk_to_int(_PrSystemMem->pDukContext, -1);
			duk_pop(_PrSystemMem->pDukContext);
			duk_get_global_string(_PrSystemMem->pDukContext, "SCREEN_HEIGHT_DEFAULT");
			_PrSystemMem->sBoostParam.nScreenHeight = duk_to_int(_PrSystemMem->pDukContext, -1);
			duk_pop(_PrSystemMem->pDukContext);
			duk_get_global_string(_PrSystemMem->pDukContext, "FULLSCREEN_DEFAULT");
			_PrSystemMem->sBoostParam.bFullscreen = duk_to_boolean(_PrSystemMem->pDukContext, -1);
			duk_pop(_PrSystemMem->pDukContext);
			duk_get_global_string(_PrSystemMem->pDukContext, "QUALITY_DEFAULT");
			_PrSystemMem->sBoostParam.eQuality = duk_to_uint(_PrSystemMem->pDukContext, -1);
			duk_pop(_PrSystemMem->pDukContext);
		}
		blDeleteStream(_stream);
	}
	_ShowWindow();
    _AudioInit(_PrSystemMem->pDukContext);
    _UIInit(_PrSystemMem->pDukContext, _PrSystemMem->sBoostParam.bProfiler);
    _SpriteInit(_PrSystemMem->pDukContext);
    _NetworkInit(_PrSystemMem->pDukContext);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_ANDROID)
	_GbSystemRunning = TRUE;
	_PrSystemMem->pBeginFunc();
#endif
}
BLVoid
_SystemStep()
{
	blFrameBufferClear(TRUE, TRUE, TRUE);
    BLU32 _now = blTickCounts();
    BLU32 _delta = _now - _PrSystemMem->nSysTime;
    _PrSystemMem->nSysTime = _now;
	for (BLU32 _idx = 0 ; _idx < 8; ++_idx)
	{
		if (_PrSystemMem->aTimers[_idx].nId != -1)
		{
			if (_PrSystemMem->nSysTime - _PrSystemMem->aTimers[_idx].nLastTime >= (BLU32)_PrSystemMem->aTimers[_idx].fElapse * 1000)
			{
				blInvokeEvent(BL_ET_SYSTEM, BL_SE_TIMER, _PrSystemMem->aTimers[_idx].nId, NULL, INVALID_GUID);
				_PrSystemMem->aTimers[_idx].nLastTime = _PrSystemMem->nSysTime;
			}
		}
	}
	_PollMsg();
	_PollEvent();
#if defined(BL_PLATFORM_ANDROID)
	if (_PrSystemMem->bBackendState)
	{
		_GpuIntervention(_PrSystemMem->pDukContext, _PrSystemMem->pWindow, _PrSystemMem->sBoostParam.nScreenWidth, _PrSystemMem->sBoostParam.nScreenHeight, !_PrSystemMem->sBoostParam.bProfiler, TRUE);
		_PrSystemMem->bBackendState = FALSE;
	}
#endif
	_UtilsStep(_delta);
	_AudioStep(_delta);
	_NetworkStep(_delta);
	_UIStep(_delta, TRUE);
    _SpriteStep(_delta, FALSE);
	_UIStep(_delta, FALSE);
    _SpriteStep(_delta, TRUE);
	_StreamIOStep(_delta);
    _PrSystemMem->pStepFunc(_delta);
	if (_GbSystemRunning == 1)
		_GpuSwapBuffer();
}
BLVoid
_SystemDestroy()
{
	if (_GbTVMode)
		_GbTVMode = FALSE;
	BLAnsi _tmp[256] = { 0 };
	sprintf(_tmp, "%d", _PrSystemMem->sBoostParam.nScreenWidth);
	blEnvVariable((const BLUtf8*)"SCREEN_WIDTH", (BLUtf8*)_tmp);
	memset(_tmp, 0, sizeof(_tmp));
	sprintf(_tmp, "%d", _PrSystemMem->sBoostParam.nScreenHeight);
	blEnvVariable((const BLUtf8*)"SCREEN_HEIGHT", (BLUtf8*)_tmp);
	memset(_tmp, 0, sizeof(_tmp));
	if (_PrSystemMem->sBoostParam.bFullscreen)
		strcpy(_tmp, "true");
	else
		strcpy(_tmp, "false");
	blEnvVariable((const BLUtf8*)"FULLSCREEN", (BLUtf8*)_tmp);
	memset(_tmp, 0, sizeof(_tmp));
	sprintf(_tmp, "%d", _PrSystemMem->sBoostParam.eQuality);
	blEnvVariable((const BLUtf8*)"QUALITY", (BLUtf8*)_tmp);
	_PrSystemMem->pEndFunc();
    _NetworkDestroy();
    _SpriteDestroy();
	_UIDestroy();
    _AudioDestroy();
	_StreamIODestroy();
    _CloseWindow();
	_UtilsDestroy();
    if (_PrSystemMem->pEvents)
        free(_PrSystemMem->pEvents);
	if (_PrSystemMem->pDukContext)
		duk_destroy_heap(_PrSystemMem->pDukContext);
#if defined(BL_PLATFORM_UWP)
	delete _PrSystemMem;
	_PrSystemMem = NULL;
#else
	free(_PrSystemMem);
	_PrSystemMem = NULL;
#endif
}
BLEnum
blPlatformIdentity()
{
#if defined(BL_PLATFORM_WIN32)
    return BL_PLATFORM_WIN32;
#elif defined(BL_PLATFORM_UWP)
    return BL_PLATFORM_UWP;
#elif defined(BL_PLATFORM_LINUX)
    return BL_PLATFORM_LINUX;
#elif defined(BL_PLATFORM_ANDROID)
    return BL_PLATFORM_ANDROID;
#elif defined(BL_PLATFORM_OSX)
    return BL_PLATFORM_OSX;
#elif defined(BL_PLATFORM_IOS)
    return BL_PLATFORM_IOS;
#else
    return -1;
#endif
}
BLU32
blTickCounts()
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	static LARGE_INTEGER _litime = { 0 };
	if (_litime.QuadPart == 0)
		QueryPerformanceFrequency(&_litime);
	LARGE_INTEGER _lifreq;
	QueryPerformanceCounter(&_lifreq);
	return (BLU32)((_lifreq.QuadPart) * 1000 / _litime.QuadPart);
#elif defined(BL_PLATFORM_LINUX)
    struct timeval _val;
    gettimeofday(&_val, NULL);
    return (BLU32)(_val.tv_sec * 1000 + _val.tv_usec / 1000.0f);
#elif defined(BL_PLATFORM_ANDROID)
    struct timeval _val;
    gettimeofday(&_val, NULL);
    return (BLU32)(_val.tv_sec * 1000 + _val.tv_usec / 1000.0f);
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
    static mach_timebase_info_data_t _frequency = { 0 , 0 };
    if (_frequency.denom == 0)
        mach_timebase_info(&_frequency);
    BLU64 _now = mach_absolute_time();
    return (BLU32)((((_now) * _frequency.numer) / _frequency.denom) / 1000000);
#endif
    return 0;
}
BLVoid
blTickDelay(IN BLU32 _Ms)
{
	if (_Ms == 0)
		return;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	Sleep(_Ms);
#elif defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_ANDROID)
	struct timespec _res;
    _res.tv_sec = _Ms / 1000;
    _res.tv_nsec = (_Ms * 1000000) % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &_res, NULL);
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
    static BLF64 _timebase = -1.0;
    if (_timebase < 0)
	{
		mach_timebase_info_data_t _frequency = { 0 , 0 };
		mach_timebase_info(&_frequency);
		_timebase = (BLF64)_frequency.numer / (BLF64)_frequency.denom * 1e-9;
	}
    mach_wait_until(_Ms * 1000 / 1e6 / _timebase + mach_absolute_time());
#endif
}
const BLAnsi*
blUserFolderDir()
{
#if defined(BL_PLATFORM_WIN32)
    if (!_PrSystemMem->aUserDir[0])
    {
        BLAnsi _fullpath[260 + 1];
        GetModuleFileNameA(NULL, _fullpath, 260 + 1);
        BLAnsi* _basename = strrchr(_fullpath, '\\');
        if (_basename)
        {
            BLAnsi _appdatapath[260 + 1];
            if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, _appdatapath)))
            {
				memset(_PrSystemMem->aUserDir, 0, sizeof(_PrSystemMem->aUserDir));
                strcpy_s(_PrSystemMem->aUserDir, 260, _appdatapath);
				strcat_s(_PrSystemMem->aUserDir, 260, _basename);
                for (BLU32 _i = (BLU32)strlen(_PrSystemMem->aUserDir); _i >= 0; --_i)
                {
                    if (_PrSystemMem->aUserDir[_i] != '.')
						_PrSystemMem->aUserDir[_i] = 0;
                    else
                    {
						_PrSystemMem->aUserDir[_i] = 0;
                        break;
                    }
                }
                strcat_s(_PrSystemMem->aUserDir, 260, "\\");
                if (SUCCEEDED(SHCreateDirectoryExA(NULL, _PrSystemMem->aUserDir, NULL)))
                    return _PrSystemMem->aUserDir;
            }
        }
        strcpy_s(_PrSystemMem->aUserDir, 260, _fullpath);
        for (BLU32 _i = (BLU32)strlen(_fullpath); _i >= 0; --_i)
        {
            if (_PrSystemMem->aUserDir[_i] != '\\')
				_PrSystemMem->aUserDir[_i] = 0;
            else
                break;
        }
    }
    return _PrSystemMem->aUserDir;
#elif defined(BL_PLATFORM_UWP)
    if (!_PrSystemMem->aUserDir[0])
    {
        auto _localfolderpath = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
        Platform::String^ _path = _localfolderpath;
        const wchar_t* _wstr = _path->Data();
        BLU32 _wsz = (BLU32)wcslen(_wstr);
        BLU32 _y;
        for (_y = 0; _y < _wsz; ++_y)
			_PrSystemMem->aUserDir[_y] = (BLAnsi)_wstr[_y];
        strcat(_PrSystemMem->aUserDir, "\\");
    }
    return _PrSystemMem->aUserDir;
#elif defined(BL_PLATFORM_LINUX)
    if (!_PrSystemMem->aUserDir[0])
    {
        const BLAnsi* _xdgpath = getenv("XDG_DATA_HOME");
        if (!_xdgpath)
        {
            strcpy(_PrSystemMem->aUserDir, getenv("HOME"));
            strcat(_PrSystemMem->aUserDir, "/.local/share/");
        }
        else
        {
            strcpy(_PrSystemMem->aUserDir, _xdgpath);
            strcat(_PrSystemMem->aUserDir, "/");
        }
        strcat(_PrSystemMem->aUserDir, (const BLAnsi*)_PrSystemMem->sBoostParam.pAppName);
        mkdir(_PrSystemMem->aUserDir, 0755);
		strcat(_PrSystemMem->aUserDir, "/");
    }
    return _PrSystemMem->aUserDir;
#elif defined(BL_PLATFORM_ANDROID)
	if (!_PrSystemMem->aUserDir[0])
	{
		strcpy(_PrSystemMem->aUserDir, _PrSystemMem->pActivity->externalDataPath);
		mkdir(_PrSystemMem->aUserDir, 0755);
		strcat(_PrSystemMem->aUserDir, "/");
	}
	return _PrSystemMem->aUserDir;
#elif defined(BL_PLATFORM_OSX)
    if (!_PrSystemMem->aUserDir[0])
    {
        NSArray* _paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* _directory = [_paths objectAtIndex : 0];
        const BLAnsi* _strtmp = [_directory UTF8String];
        strcpy(_PrSystemMem->aUserDir, _strtmp);
        strcat(_PrSystemMem->aUserDir, "/");
        strcat(_PrSystemMem->aUserDir, (const BLAnsi*)_PrSystemMem->sBoostParam.pAppName);
        mkdir(_PrSystemMem->aUserDir, 0755);
		strcat(_PrSystemMem->aUserDir, "/");
    }
    return _PrSystemMem->aUserDir;
#elif defined(BL_PLATFORM_IOS)
    if (!_PrSystemMem->aUserDir[0])
    {
        NSArray* _paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* _directory = [_paths objectAtIndex : 0];
        const BLAnsi* _strtmp = [_directory UTF8String];
        strcpy(_PrSystemMem->aUserDir, _strtmp);
        strcat(_PrSystemMem->aUserDir, "/");
        strcat(_PrSystemMem->aUserDir, (const BLAnsi*)_PrSystemMem->sBoostParam.pAppName);
        mkdir(_PrSystemMem->aUserDir, 0755);
		strcat(_PrSystemMem->aUserDir, "/");
    }
    return _PrSystemMem->aUserDir;
#endif
}
const BLAnsi*
blWorkingDir()
{
#if defined(BL_PLATFORM_WIN32)
    if (!_PrSystemMem->aWorkDir[0])
    {
		DWORD(WINAPI* _modulemid)(HANDLE, HMODULE, LPWSTR, DWORD);
		DWORD _buflen = 128;
		WCHAR* _path = NULL;
		HMODULE _psapi = LoadLibraryW(L"psapi.dll");
		DWORD _len = 0;
		INT _idx;
		if (!_psapi)
			return NULL;
		_modulemid = (DWORD(WINAPI*)(HANDLE, HMODULE, LPWSTR, DWORD))GetProcAddress(_psapi, "GetModuleFileNameExW");
		if (!_modulemid)
		{
			FreeLibrary(_psapi);
			return NULL;
		}
		while (TRUE)
		{
			BLVoid* _ptr = realloc(_path, _buflen * sizeof(WCHAR));
			if (!_ptr)
			{
				free(_path);
				FreeLibrary(_psapi);
				return NULL;
			}
			_path = (WCHAR*)_ptr;
			_len = _modulemid(GetCurrentProcess(), NULL, _path, _buflen);
			if (_len != _buflen)
				break;
			_buflen *= 2;
		}
		FreeLibrary(_psapi);
		if (_len == 0)
			return NULL;
		for (_idx = _len - 1; _idx > 0; _idx--)
		{
			if (_path[_idx] == '\\')
				break;
		}
		assert(_idx > 0);
		_path[_idx + 1] = '\0';
		const BLAnsi* _tmp = (const BLAnsi*)blGenUtf8Str((const BLUtf16*)_path);
		strcpy_s(_PrSystemMem->aWorkDir, 260, _tmp);
		blDeleteUtf8Str((BLUtf8*)_tmp);
		free(_path);
    }
#elif defined(BL_PLATFORM_UWP)
    if (!_PrSystemMem->aWorkDir[0])
    {
        Windows::ApplicationModel::Package^ _package = Windows::ApplicationModel::Package::Current;
        Platform::String^ _path = _package->InstalledLocation->Path;
        const WCHAR* _wstr = _path->Data();
        BLU32 _wsz = (BLU32)wcslen(_wstr);
        BLU32 _y;
        for (_y = 0; _y < _wsz; ++_y)
			_PrSystemMem->aWorkDir[_y] = (BLAnsi)_wstr[_y];
        strcat(_PrSystemMem->aWorkDir, "\\Assets\\");
    }
#elif defined(BL_PLATFORM_LINUX)
    if (!_PrSystemMem->aWorkDir[0])
    {
        BLAnsi _fullpath[256] = { 0 };
        size_t _length = readlink("/proc/self/exe", _fullpath, sizeof(_fullpath) - 1);
        _fullpath[_length] = 0;
        for (BLS32 _i = strlen(_fullpath); _i >= 0; --_i)
        {
            if (_fullpath[_i] != '/')
                _fullpath[_i] = 0;
            else
                break;
        }
        strcpy(_PrSystemMem->aWorkDir, _fullpath);
    }
#elif defined(BL_PLATFORM_ANDROID)
#elif defined(BL_PLATFORM_OSX)
    if (!_PrSystemMem->aWorkDir[0])
    {
        NSString* _path = [[NSBundle mainBundle] resourcePath];
        const BLAnsi* _strtmp = [_path UTF8String];
        strcpy(_PrSystemMem->aWorkDir, _strtmp);
        strcat(_PrSystemMem->aWorkDir, "/");
    }
#elif defined(BL_PLATFORM_IOS)
    if (!_PrSystemMem->aWorkDir[0])
    {
        NSString* _path = [[NSBundle mainBundle] resourcePath];
        const BLAnsi* _strtmp = [_path UTF8String];
        strcpy(_PrSystemMem->aWorkDir, _strtmp);
        strcat(_PrSystemMem->aWorkDir, "/");
    }
#endif
	return _PrSystemMem->aWorkDir;
}
BLBool
blClipboardCopy(IN BLUtf8* _Text)
{
	if (!_Text)
		return FALSE;
	BLU32 _length = blUtf8Length(_Text);
	if (_length + 1 > 1024)
		return FALSE;
#if defined(BL_PLATFORM_WIN32)
	if (OpenClipboard(_PrSystemMem->nHwnd))
	{
		BLU32 _idx;
		HANDLE _hmem;
		WCHAR* _wtext;
		BLS32 _len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_Text, -1, NULL, 0);
		if ((1 + _len) * sizeof(WCHAR) > 1024)
		{
			CloseClipboard();
			return FALSE;
		}
		memset(_PrSystemMem->aClipboard, 0, 1024);
		::MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_Text, -1, (LPWSTR)_PrSystemMem->aClipboard, _len);
		_wtext = (WCHAR*)_PrSystemMem->aClipboard;
		_hmem = GlobalAlloc(GMEM_MOVEABLE, (_len+1)*sizeof(WCHAR));
		if (_hmem)
		{
			WCHAR* _dst = (WCHAR*)GlobalLock(_hmem);
			if (_dst)
			{
				for (_idx = 0; _wtext[_idx]; ++_idx)
				{
					if (_wtext[_idx] == '\n' && (_idx == 0 || _wtext[_idx-1] != '\r'))
						*_dst++ = '\r';
					*_dst++ = _wtext[_idx];
				}
				*_dst = 0;
				GlobalUnlock(_hmem);
			}
			EmptyClipboard();
			if (!SetClipboardData(CF_UNICODETEXT, _hmem))
			{
				CloseClipboard();
				return FALSE;
			}
		}
		CloseClipboard();
	}
	return TRUE;
#elif defined(BL_PLATFORM_UWP)
	WCHAR* _wtext;
	BLS32 _len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_Text, -1, NULL, 0);
	if ((1 + _len) * sizeof(WCHAR) > 1024)
		return FALSE;
	memset(_PrSystemMem->aClipboard, 0, 1024);
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_Text, -1, (LPWSTR)_PrSystemMem->aClipboard, _len);
	_wtext = (WCHAR*)_PrSystemMem->aClipboard;
	Windows::ApplicationModel::DataTransfer::DataPackage^ _datapackage = ref new Windows::ApplicationModel::DataTransfer::DataPackage();
	_datapackage->SetText(ref new Platform::String(_wtext));
	Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(_datapackage);
	return TRUE;
#elif defined(BL_PLATFORM_OSX)
	@autoreleasepool
	{
		NSPasteboard* _pasteboard;
		NSString* _format;
		if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_5)
			_format = NSPasteboardTypeString;
		else
			_format = NSStringPboardType;
		_pasteboard = [NSPasteboard generalPasteboard];
		[_pasteboard declareTypes:[NSArray arrayWithObject:_format] owner:nil];
		[_pasteboard setString:[NSString stringWithUTF8String:(const BLAnsi*)_Text] forType:_format];
	}
	return TRUE;
#else
    memset(_PrSystemMem->aClipboard, 0, 1024);
    memcpy(_PrSystemMem->aClipboard, _Text, blUtf8Length(_Text));
    return TRUE;
#endif
}
const BLUtf8*
blClipboardPaste()
{
#if defined(BL_PLATFORM_WIN32)
	memset(_PrSystemMem->aClipboard, 0, 1024);
	if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(_PrSystemMem->nHwnd))
	{
		HANDLE _hmem;
		WCHAR* _wstr;
		_hmem = GetClipboardData(CF_UNICODETEXT);
		if (_hmem)
		{
			_wstr = (WCHAR*)GlobalLock(_hmem);
			WideCharToMultiByte(CP_UTF8, 0, _wstr, -1, (CHAR*)_PrSystemMem->aClipboard, 1024, NULL, NULL);
			GlobalUnlock(_hmem);
		}
		else
		{
			CloseClipboard();
			return NULL;
		}
		CloseClipboard();
	}
#elif defined(BL_PLATFORM_UWP)
	Windows::ApplicationModel::DataTransfer::DataPackageView^ _con = Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
	if (_con->Contains(Windows::ApplicationModel::DataTransfer::StandardDataFormats::Text))
	{
		Platform::String^ _str;
		auto _task = _con->GetTextAsync();
		Platform::Exception^ _exception = nullptr;
		HANDLE _handle = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
		_task->Completed = ref new Windows::Foundation::AsyncOperationCompletedHandler<Platform::String^>([&]
		(Windows::Foundation::IAsyncOperation<Platform::String^>^ _AsyncInfo, Windows::Foundation::AsyncStatus _AsyncStatus)
		{
			try
			{
				_str = _AsyncInfo->GetResults();
			}
			catch (Platform::Exception^ _E)	{ _exception = _E; }
			SetEvent(_handle);
		});
		WaitForSingleObjectEx(_handle, INFINITE, FALSE);
		if (_exception)
			return NULL;
		memset(_PrSystemMem->aClipboard, 0, 1024);
		WideCharToMultiByte(CP_UTF8, 0, _str->Data(), -1, (CHAR*)_PrSystemMem->aClipboard, 1024, NULL, NULL);
	}
	else
		return NULL;
#elif defined(BL_PLATFORM_OSX)
	memset(_PrSystemMem->aClipboard, 0, 1024);
	@autoreleasepool
	{
		NSPasteboard* _pasteboard;
		NSString* _format;
		if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_5)
			_format = NSPasteboardTypeString;
		else
			_format = NSStringPboardType;
		NSString* _available;
		_pasteboard = [NSPasteboard generalPasteboard];
		_available = [_pasteboard availableTypeFromArray: [NSArray arrayWithObject:_format]];
		if ([_available isEqualToString:_format])
		{
			NSString* _string;
			_string = [_pasteboard stringForType:_format];
			if (_string != nil)
				memcpy(_PrSystemMem->aClipboard, [_string UTF8String], strlen([_string UTF8String]));
		}
	}
#endif
	return _PrSystemMem->aClipboard;
}
BLBool
blQuitEvent()
{
	BLBool _quit = FALSE;
	_PollMsg();
#ifdef BL_PLATFORM_ANDROID
	BLS32 _busy;
	_busy = pthread_mutex_trylock(&_PrSystemMem->sMutex);
#endif
    for (BLU32 _idx = 0; _idx < _PrSystemMem->nEventIdx; ++_idx)
    {
        switch (_PrSystemMem->pEvents[_idx].eType)
        {
			case BL_ET_NET:
			{
				if (_PrSystemMem->pEvents[_idx].uEvent.sNet.pBuf)
					free((BLVoid*)_PrSystemMem->pEvents[_idx].uEvent.sNet.pBuf);
			} break;
            case BL_ET_MOUSE:
			{
				if (BL_ME_LDOWN == _PrSystemMem->pEvents[_idx].uEvent.sMouse.eEvent)
					_quit = TRUE;
            } break;
            case BL_ET_KEY:
			{
				if (_PrSystemMem->pEvents[_idx].uEvent.sKey.eCode == BL_KC_ESCAPE || _PrSystemMem->pEvents[_idx].uEvent.sKey.eCode == BL_KC_EXIT)
					_quit = TRUE;
                if (_PrSystemMem->pEvents[_idx].uEvent.sKey.pString)
                    free((BLVoid*)_PrSystemMem->pEvents[_idx].uEvent.sKey.pString);
            } break;
            default:break;
        }
    }
    _PrSystemMem->nEventIdx = 0;
    _PrSystemMem->nEventsSz = 0;
#ifdef BL_PLATFORM_ANDROID
	if (!_busy)
		pthread_mutex_unlock(&_PrSystemMem->sMutex);
#endif
	return _quit;
}
BLBool
blEnvVariable(IN BLUtf8* _Section, INOUT BLUtf8 _Value[256])
{
	BLBool _set = (_Value[0] == 0) ? FALSE : TRUE;
	BLAnsi _path[260] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	HANDLE _fp;
    WIN32_FILE_ATTRIBUTE_DATA _wfad;
    strcpy_s(_path, 260, blUserFolderDir());
    strcat_s(_path, 260, "EnvStringOptions");
	BLBool _fileexist = GetFileAttributesExA(_path, GetFileExInfoStandard, &_wfad) ? TRUE : FALSE;
#else
    FILE* _fp;
    strcpy(_path, blUserFolderDir());
    strcat(_path, "EnvStringOptions");
	BLBool _fileexist = (access(_path, 0) != -1) ? TRUE : FALSE;
#endif
	BLDictionary* _tmpdic = NULL;
	BLUtf8* _tmpstr = NULL;
	BLU32 _vallen = blUtf8Length(_Value);
	BLU32 _id = blHashUtf8(_Section);
	if (!_set && !_fileexist)
		return FALSE;
	else if (_set && !_fileexist)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
		WCHAR _wfilename[260] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS, NULL);
#else
		_fp = CreateFileA(_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#else
		_fp = fopen(_path, "wb");
#endif
		_tmpdic = blGenDict(FALSE);
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	else
	{
#ifdef WINAPI_FAMILY
		WCHAR _wfilename[260] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		_fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		LARGE_INTEGER _li;
		GetFileSizeEx(_fp, &_li);
		if (_li.LowPart != 0)
		{
			_li.LowPart = 0;
			_li.HighPart = 0;
			SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
			_tmpdic = blGenDict(FALSE);
			do
			{
				BLU32 _sid, _strsz;
				if (!ReadFile(_fp, &_sid, sizeof(BLU32), NULL, NULL))
					break;
				if (_sid == 0xFFFFFFFF)
					break;
				if (!ReadFile(_fp, &_strsz, sizeof(BLU32), NULL, NULL))
					break;
				_tmpstr = (BLUtf8*)malloc(_strsz + 1);
				if (!ReadFile(_fp, _tmpstr, sizeof(BLUtf8) * _strsz, NULL, NULL))
				{
					free(_tmpstr);
					break;
				}
				_tmpstr[_strsz] = 0;
				blDictInsert(_tmpdic, _sid, _tmpstr);
			} while (1);
		}
		else
			_tmpdic = blGenDict(FALSE);
	}
	CloseHandle(_fp);
#else
	else
	{
		_fp = fopen(_path, "rb");
		fseek(_fp, 0, SEEK_END);
		if (ftell(_fp) != 0)
		{
			fseek(_fp, 0, SEEK_SET);
			_tmpdic = blGenDict(FALSE);
			do
			{
				BLU32 _strsz, _sid;
				if (!fread(&_sid, sizeof(BLU32), 1, _fp))
					break;
				if (_sid == 0xFFFFFFFF)
					break;
				if (!fread(&_strsz, sizeof(BLU32), 1, _fp))
					break;
				_tmpstr = (BLUtf8*)malloc(_strsz + 1);
				if (!fread(_tmpstr, sizeof(BLUtf8), _strsz, _fp))
				{
					free(_tmpstr);
					break;
				}
				_tmpstr[_strsz] = 0;
				blDictInsert(_tmpdic, _sid, _tmpstr);
			} while (1);
		}
		else
			_tmpdic = blGenDict(FALSE);
	}
	fclose(_fp);
#endif
	if (!_set)
	{
		memset(_Value, 0, sizeof(BLUtf8) * 256);
		BLUtf8* _str = (BLUtf8*)blDictElement(_tmpdic, _id);
		if (_str)
			memcpy(_Value, _str, strlen((const BLAnsi*)_str));
		else
		{
			FOREACH_DICT(BLUtf8*, _iter, _tmpdic)
			{
				free(_iter);
			}
			blDeleteDict(_tmpdic);
			return FALSE;
		}
	}
	else
	{
		BLUtf8* _str = (BLUtf8*)blDictElement(_tmpdic, _id);
		BLU32 _sz;
		if (_str)
		{
			free(_str);
			blDictErase(_tmpdic, _id);
			_tmpstr = (BLUtf8*)malloc(_vallen + 1);
			memcpy(_tmpstr, _Value, _vallen);
			_tmpstr[_vallen] = 0;
			blDictInsert(_tmpdic, _id, _tmpstr);
		}
		else
		{
			_tmpstr = (BLUtf8*)malloc(_vallen + 1);
			memcpy(_tmpstr, _Value, _vallen);
			_tmpstr[_vallen] = 0;
			blDictInsert(_tmpdic, _id, _tmpstr);
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
		WCHAR _wfilename[260] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS, NULL);
#else
		_fp = CreateFileA(_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		FOREACH_DICT(BLUtf8*, _iter, _tmpdic)
		{
			WriteFile(_fp, &_iteratorkey, sizeof(BLU32), NULL, NULL);
			_sz = (BLU32)strlen((const BLAnsi*)_iter);
			WriteFile(_fp, &_sz, sizeof(BLU32), NULL, NULL);
			WriteFile(_fp, _iter, _sz, NULL, NULL);
		}
		_sz = 0xFFFFFFFF;
		WriteFile(_fp, &_sz, sizeof(BLU32), NULL, NULL);
		CloseHandle(_fp);
#else
		_fp = fopen(_path, "wb");
		FOREACH_DICT(BLUtf8*, _iter, _tmpdic)
		{
			fwrite(&_iteratorkey, sizeof(BLU32), 1, _fp);
			_sz = (BLU32)strlen((const BLAnsi*)_iter);
			fwrite(&_sz, sizeof(BLU32), 1, _fp);
			fwrite(_iter, sizeof(BLUtf8), _sz, _fp);
		}
		_sz = 0xFFFFFFFF;
		fwrite(&_sz, sizeof(BLU32), 1, _fp);
		fclose(_fp);
#endif
	}
	FOREACH_DICT(BLUtf8*, _iter, _tmpdic)
	{
		free(_iter);
	}
	blDeleteDict(_tmpdic);
	return TRUE;
}
BLVoid
blOpenURL(IN BLUtf8* _Url)
{
	BLUtf8* _absurl;
	BLBool _malloc = FALSE;
	if (strncmp((const BLAnsi*)_Url, "http://", 7) == 0)
		_absurl = (BLUtf8*)_Url;
	else
	{
		_malloc = TRUE;
		_absurl = (BLUtf8*)malloc(strlen((const BLAnsi*)_Url) + 8);
		memset(_absurl, 0, strlen((const BLAnsi*)_Url) + 8);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		sprintf_s((BLAnsi*)_absurl, strlen((const BLAnsi*)_Url) + 8, "http://%s", _Url);
#else
		sprintf((BLAnsi*)_absurl, "http://%s", _Url);
#endif
	}
#if defined(BL_PLATFORM_WIN32)
    WCHAR _wurl[260] = { 0 };
    BLS32 _len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_absurl, -1, NULL, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_absurl, -1, _wurl, _len);
    ShellExecuteW(NULL, L"open", _wurl, NULL, NULL, SW_SHOWNORMAL);
#elif defined(BL_PLATFORM_UWP)
	WCHAR _wurl[260] = { 0 };
	BLS32 _len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_absurl, -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)_absurl, -1, _wurl, _len);
	Windows::Foundation::Uri^ _uri = ref new Windows::Foundation::Uri(ref new Platform::String(_wurl));
	Windows::System::Launcher::LaunchUriAsync(_uri);
#elif defined(BL_PLATFORM_LINUX)
    BLAnsi _cmd[512] = { 0 };
    strcpy(_cmd, "xdg-open ");
    strcat(_cmd, (const BLAnsi*)_absurl);
    system(_cmd);
#elif defined(BL_PLATFORM_ANDROID)
	JNIEnv* _env = _PrSystemMem->pActivity->env;
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
	jclass _activitycls = _env->FindClass("android/app/Activity");
	jmethodID _startmid = _env->GetMethodID(_activitycls, "startActivity", "(Landroid/content/Intent;)V");
	jclass _intentcls = _env->FindClass("android/content/Intent");
	jstring _actionpar = _env->NewStringUTF("android.intent.action.VIEW");
	jmethodID _intentctormid = _env->GetMethodID(_intentcls, "<init>", "(Ljava/lang/String;)V");
	jobject _intentobject = _env->NewObject(_intentcls, _intentctormid, _actionpar);
	_env->DeleteLocalRef(_actionpar);
	jmethodID _setmid = _env->GetMethodID(_intentcls, "setData", "(Landroid/net/Uri;)Landroid/content/Intent;");
	jclass _uricls = _env->FindClass("android/net/Uri");
	jmethodID _uriparsemid = _env->GetStaticMethodID(_uricls, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
	jstring _urlpar = _env->NewStringUTF((const BLAnsi*)_absurl);
	jobject _uriobject = _env->CallStaticObjectMethod(_uricls, _uriparsemid, _urlpar);
	jobject _newintentobj = _env->CallObjectMethod(_intentobject, _setmid, _uriobject);
	_env->DeleteLocalRef(_activitycls);
	_env->DeleteLocalRef(_intentcls);
	_env->DeleteLocalRef(_uricls);
	_env->DeleteLocalRef(_urlpar);
	_env->CallVoidMethod(_PrSystemMem->pActivity->clazz, _startmid, _newintentobj);
	_env->DeleteLocalRef(_uriobject);
	_env->DeleteLocalRef(_intentobject);
	_PrSystemMem->pActivity->vm->DetachCurrentThread();
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
#elif defined(BL_PLATFORM_OSX)
    NSString* _str = [NSString stringWithCString : (const BLAnsi*)_absurl encoding : NSUTF8StringEncoding];
    NSURL* _url = [NSURL URLWithString : _str];
    [[NSWorkspace sharedWorkspace] openURL:_url];
#elif defined(BL_PLATFORM_IOS)
    NSString* _str = [NSString stringWithCString : (const BLAnsi*)_absurl encoding : NSUTF8StringEncoding];
    NSURL* _url = [NSURL URLWithString : _str];
    [[UIApplication sharedApplication] openURL:_url];
#endif
	if (_malloc)
		free(_absurl);
}
BLBool
blOpenPlugin(IN BLAnsi* _Basename)
{
	BLU32 _hashname = blHashUtf8((const BLUtf8*)_Basename);
	BLU32 _idx = 0;
	for (; _idx < 64; ++_idx)
	{
		if (_PrSystemMem->aPlugins[_idx].nHash == 0)
			break;
		else if (_PrSystemMem->aPlugins[_idx].nHash == _hashname)
			return TRUE;
	}
	if (_idx >= 64)
		return FALSE;
	_PrSystemMem->aPlugins[_idx].nHash = _hashname;
    BLAnsi _path[260] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
    strcpy_s(_path, 260, blWorkingDir());
    strcat_s(_path, 260, "lib");
    strcat_s(_path, 260, _Basename);
    strcat_s(_path, 260, "Plugin.dll");
#elif defined(BL_PLATFORM_LINUX)
    strcpy(_path, blWorkingDir());
    strcat(_path, "lib");
    strcat(_path, _Basename);
    strcat(_path, "Plugin.so");
#elif defined(BL_PLATFORM_ANDROID)
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	JNIEnv* _env = _PrSystemMem->pActivity->env;
	_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
	jmethodID _applicationnfomid = _env->GetMethodID(_env->GetObjectClass(_PrSystemMem->pActivity->clazz), "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
	jobject _appinfo = _env->CallObjectMethod(_PrSystemMem->pActivity->clazz, _applicationnfomid);
	jfieldID _fieldid = _env->GetFieldID(_env->GetObjectClass(_appinfo), "nativeLibraryDir", "Ljava/lang/String;");
	jstring _nativelibrarydir = (jstring)_env->GetObjectField(_appinfo, _fieldid);
	const BLAnsi* _str = _env->GetStringUTFChars(_nativelibrarydir, NULL);
	strcpy(_path, _str);
	_env->ReleaseStringUTFChars(_nativelibrarydir, _str);
	_env->DeleteLocalRef(_appinfo);
	_PrSystemMem->pActivity->vm->DetachCurrentThread();
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
	strcat(_path, "/lib");
	strcat(_path, _Basename);
	strcat(_path, "Plugin.so");
#else
    strcpy(_path, blWorkingDir());
    strcat(_path, "lib");
    strcat(_path, _Basename);
    strcat(_path, "Plugin.dylib");
#endif
#if defined(BL_PLATFORM_WIN32)
    WCHAR _wfilename[260] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
    _PrSystemMem->aPlugins[_idx].pHandle = LoadLibraryW(_wfilename);
#elif defined(BL_PLATFORM_UWP)
    WCHAR _wfilename[260] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
	_PrSystemMem->aPlugins[_idx].pHandle = LoadPackagedLibrary(_wfilename, 0);
#elif defined(BL_PLATFORM_LINUX)
	_PrSystemMem->aPlugins[_idx].pHandle = dlopen(_path, RTLD_LAZY | RTLD_GLOBAL);
#elif defined(BL_PLATFORM_ANDROID)
	_PrSystemMem->aPlugins[_idx].pHandle = dlopen(_path, RTLD_LAZY | RTLD_GLOBAL);
#elif defined(BL_PLATFORM_OSX)
	_PrSystemMem->aPlugins[_idx].pHandle = dlopen(_path, RTLD_LAZY | RTLD_GLOBAL);
#elif defined(BL_PLATFORM_IOS)
	_PrSystemMem->aPlugins[_idx].pHandle = dlopen(_path, RTLD_LAZY | RTLD_GLOBAL);
#endif
	memset(_path, 0, sizeof(_path));
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	strcpy_s(_path, 260, "bl");
	strcat_s(_path, 260, _Basename);
	strcat_s(_path, 260, "OpenEXT");
#else
	strcpy(_path, "bl");
	strcat(_path, _Basename);
	strcat(_path, "OpenEXT");
#endif
	BLVoid(*_open)(BLVoid);
	_open = (BLVoid(*)(BLVoid))blPluginProcAddress(_Basename, _path);
	_open();
    return TRUE;
}
BLBool
blClosePlugin(IN BLAnsi* _Basename)
{
	BLU32 _hashname = blHashUtf8((const BLUtf8*)_Basename);
	BLU32 _idx = 0;
	for (; _idx < 64; ++_idx)
	{
		if (_PrSystemMem->aPlugins[_idx].nHash == _hashname)
			break;
	}
	if (_idx >= 64)
		return FALSE;
	BLAnsi _path[260] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	strcpy_s(_path, 260, "bl");
	strcat_s(_path, 260, _Basename);
	strcat_s(_path, 260, "CloseEXT");
#else
	strcpy(_path, "bl");
	strcat(_path, _Basename);
	strcat(_path, "CloseEXT");
#endif
	BLVoid(*_close)(BLVoid);
	_close = (BLVoid(*)(BLVoid))blPluginProcAddress(_Basename, _path);
	_close();
#if defined(BL_PLATFORM_WIN32)
    FreeLibrary(_PrSystemMem->aPlugins[_idx].pHandle);
#elif defined(BL_PLATFORM_UWP)
    FreeLibrary(_PrSystemMem->aPlugins[_idx].pHandle);
#elif defined(BL_PLATFORM_LINUX)
    dlclose(_PrSystemMem->aPlugins[_idx].pHandle);
#elif defined(BL_PLATFORM_ANDROID)
    dlclose(_PrSystemMem->aPlugins[_idx].pHandle);
#elif defined(BL_PLATFORM_OSX)
    dlclose(_PrSystemMem->aPlugins[_idx].pHandle);
#elif defined(BL_PLATFORM_IOS)
    dlclose(_PrSystemMem->aPlugins[_idx].pHandle);
#endif
	_PrSystemMem->aPlugins[_idx].nHash = 0;
    return TRUE;
}
BLVoid*
blPluginProcAddress(IN BLAnsi* _Basename, IN BLAnsi* _Function)
{
	BLU32 _hashname = blHashUtf8((const BLUtf8*)_Basename);
	BLU32 _idx = 0;
	for (; _idx < 64; ++_idx)
	{
		if (_PrSystemMem->aPlugins[_idx].nHash == _hashname)
			break;
	}
	if (_idx >= 64)
		return NULL;
    BLVoid* _ret;
#if defined(BL_PLATFORM_WIN32)
    _ret = GetProcAddress(_PrSystemMem->aPlugins[_idx].pHandle, _Function);
#elif defined(BL_PLATFORM_UWP)
    _ret = GetProcAddress(_PrSystemMem->aPlugins[_idx].pHandle, _Function);
#elif defined(BL_PLATFORM_LINUX)
    _ret = dlsym(_PrSystemMem->aPlugins[_idx].pHandle, _Function);
#elif defined(BL_PLATFORM_ANDROID)
	_ret = dlsym(_PrSystemMem->aPlugins[_idx].pHandle, _Function);
#elif defined(BL_PLATFORM_OSX)
	_ret = dlsym(_PrSystemMem->aPlugins[_idx].pHandle, _Function);
#elif defined(BL_PLATFORM_IOS)
	_ret = dlsym(_PrSystemMem->aPlugins[_idx].pHandle, _Function);
#endif
    return _ret;
}
BLVoid
blAttachIME(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLEnum _Type)
{
	BLF32 _x = _Xpos, _y = _Ypos;
	if (_PrSystemMem->sBoostParam.bUseDesignRes)
	{
		BLF32 _dwidth = (BLF32)_PrSystemMem->sBoostParam.nDesignWidth;
		BLF32 _dheight = (BLF32)_PrSystemMem->sBoostParam.nDesignHeight;
		BLF32 _ratioorg = (BLF32)(_PrSystemMem->sBoostParam.nScreenWidth) / (BLF32)(_PrSystemMem->sBoostParam.nScreenHeight);
		BLF32 _ratiodeg = (BLF32)(_dwidth) / (BLF32)(_dheight);
		BLU32 _actualw, _actualh;
		if (_ratiodeg >= _ratioorg)
		{
			_actualh = (BLU32)_dheight;
			_actualw = (BLU32)(_ratioorg * _dheight);
		}
		else
		{
			_actualh = (BLU32)(_dwidth / _ratioorg);
			_actualw = (BLU32)_dwidth;
		}
		_x *= (BLF32)(_PrSystemMem->sBoostParam.nScreenWidth) / (BLF32)(_actualw);
		_y *= (BLF32)(_PrSystemMem->sBoostParam.nScreenHeight) / (BLF32)(_actualh);
	}
#if defined(BL_PLATFORM_WIN32)
	if (_PrSystemMem->nIMC && _Type == KEYBOARD_TEXT_INTERNAL)
	{
		COMPOSITIONFORM _com;
		ImmAssociateContext(_PrSystemMem->nHwnd, _PrSystemMem->nIMC);
		ImmGetCompositionWindow(_PrSystemMem->nIMC, &_com);
		_com.dwStyle = CFS_POINT;
		_com.ptCurrentPos.x = (LONG)_x;
		_com.ptCurrentPos.y = (LONG)_y;
		ImmSetCompositionWindow(_PrSystemMem->nIMC, &_com);
	}
#elif defined(BL_PLATFORM_UWP)
#	if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
	_PrSystemMem->sIMEpos.X = _x;
	_PrSystemMem->sIMEpos.Y = _y;
	_PrSystemMem->pCTEcxt->NotifyFocusEnter();
	_PrSystemMem->pCTEcxt->NotifyLayoutChanged();
#	else
	if (_Type == KEYBOARD_TEXT_INTERNAL)
	{
		_PrSystemMem->sIMEpos.X = _x;
		_PrSystemMem->sIMEpos.Y = _y;
		_PrSystemMem->pCTEcxt->NotifyFocusEnter();
		_PrSystemMem->pCTEcxt->NotifyLayoutChanged();
	}
#	endif
#elif defined(BL_PLATFORM_LINUX)
    if (_PrSystemMem->pIME)
        _PrSystemMem->pIC = XCreateIC(_PrSystemMem->pIME, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, _PrSystemMem->nWindow, XNFocusWindow, _PrSystemMem->nWindow, NULL);
    if (_PrSystemMem->pIC)
    {
        XVaNestedList _attr;
        XPoint _spot;
        _spot.x = _x;
        _spot.y = _y;
        _attr = XVaCreateNestedList(0, XNSpotLocation, &_spot, NULL);
        XSetICValues(_PrSystemMem->pIC, XNPreeditAttributes, _attr, NULL);
        XFree(_attr);
        BLAnsi* _contents = Xutf8ResetIC(_PrSystemMem->pIC);
        if (_contents)
            XFree(_contents);
    }
#elif defined(BL_PLATFORM_OSX)
    if (!_PrSystemMem->pTICcxt)
    {
        _PrSystemMem->pTICcxt = [[OSXTextInput alloc] initWithFrame: NSMakeRect(0, 0, 0, 0)];
        _PrSystemMem->sIMEpos.x = _x;
        _PrSystemMem->sIMEpos.y = _y;
    }
    if (![[_PrSystemMem->pTICcxt superview] isEqual: [_PrSystemMem->pWindow contentView]])
    {
        [_PrSystemMem->pTICcxt removeFromSuperview];
        [[_PrSystemMem->pWindow contentView] addSubview: _PrSystemMem->pTICcxt];
        [_PrSystemMem->pWindow setNextResponder:nil];
        [_PrSystemMem->pWindow makeFirstResponder: _PrSystemMem->pTICcxt];
    }
#elif defined(BL_PLATFORM_IOS)
    [_PrSystemMem->pTICcxt becomeFirstResponder];
#elif defined(BL_PLATFORM_ANDROID)
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	while (!_PrSystemMem->bAvtivityFocus);
	JNIEnv* _env = _PrSystemMem->pActivity->env;
	_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
	jclass _blcls = _env->GetObjectClass(_PrSystemMem->pBLJava);
	jmethodID _showviewmid = _env->GetStaticMethodID(_blcls, "showTextInput", "()V");
	_env->CallStaticVoidMethod(_blcls, _showviewmid);
	_env->DeleteLocalRef(_blcls);
	_PrSystemMem->pActivity->vm->DetachCurrentThread();
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
#endif
}
BLVoid
blDetachIME(IN BLEnum _Type)
{
#if defined(BL_PLATFORM_WIN32)
	if (_PrSystemMem->nIMC && _Type == KEYBOARD_TEXT_INTERNAL)
	{
		ImmAssociateContext(_PrSystemMem->nHwnd, NULL);
		ImmReleaseContext(_PrSystemMem->nHwnd, _PrSystemMem->nIMC);
		SetFocus(_PrSystemMem->nHwnd);
	}
#elif defined(BL_PLATFORM_UWP)
#	if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
	_PrSystemMem->pCTEcxt->NotifyFocusLeave();
#	else
	if (_Type == KEYBOARD_TEXT_INTERNAL)
		_PrSystemMem->pCTEcxt->NotifyFocusLeave();
#	endif
#elif defined(BL_PLATFORM_LINUX)
    if (_PrSystemMem->pIME)
    {
        BLAnsi* _contents = Xutf8ResetIC(_PrSystemMem->pIC);
        if (_contents)
            XFree(_contents);
        if (_PrSystemMem->pIME)
            XDestroyIC(_PrSystemMem->pIC);
    }
#elif defined(BL_PLATFORM_OSX)
    if (_PrSystemMem->pTICcxt)
    {
        [_PrSystemMem->pWindow setNextResponder:_PrSystemMem->pDelegate];
        [_PrSystemMem->pTICcxt removeFromSuperview];
        [_PrSystemMem->pTICcxt release];
        _PrSystemMem->pTICcxt = nil;
    }
#elif defined(BL_PLATFORM_IOS)
    [_PrSystemMem->pTICcxt resignFirstResponder];
#elif defined(BL_PLATFORM_ANDROID)
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	JNIEnv* _env = _PrSystemMem->pActivity->env;
	_PrSystemMem->pActivity->vm->AttachCurrentThread(&_env, NULL);
	jclass _blcls = _env->GetObjectClass(_PrSystemMem->pBLJava);
	jmethodID _hideviewmid = _env->GetStaticMethodID(_blcls, "hideTextInput", "()V");
	_env->CallStaticVoidMethod(_blcls, _hideviewmid);
	_env->DeleteLocalRef(_blcls);
	_PrSystemMem->pActivity->vm->DetachCurrentThread();
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
#endif
}
BLVoid
blCursorVisiblity(IN BLBool _Show)
{
#if defined(BL_PLATFORM_WIN32)
	ShowCursor(_Show);
#elif defined(BL_PLATFORM_LINUX)
	if (_Show)
		XDefineCursor(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nCursor);
	else
		XDefineCursor(_PrSystemMem->pDisplay, _PrSystemMem->nWindow, _PrSystemMem->nNilCursor);
#elif defined(BL_PLATFORM_OSX)
	if (_Show)
		[NSCursor unhide];
	else
		[NSCursor hide];
#elif defined(BL_PLATFORM_UWP)
#	if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
	if (_Show)
		Windows::UI::Core::CoreWindow::GetForCurrentThread()->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
	else
		Windows::UI::Core::CoreWindow::GetForCurrentThread()->PointerCursor = nullptr;
#	endif
#else
#endif
}
BLVoid
blSubscribeEvent(IN BLEnum _Type, IN BLBool(*_Subscriber)(BLEnum, BLU32, BLS32, BLVoid*, BLGuid))
{
	BLU32 _idx = 0;
	while (_PrSystemMem->pSubscriber[_Type][_idx])
		_idx++;
	_PrSystemMem->pSubscriber[_Type][_idx] = _Subscriber;
}
BLVoid
blInvokeEvent(IN BLEnum _Type, IN BLU32 _Uparam, IN BLS32 _Sparam, IN BLVoid* _Pparam, IN BLGuid _ID)
{
#ifdef BL_PLATFORM_ANDROID
	BLS32 _busy;
	_busy = pthread_mutex_trylock(&_PrSystemMem->sMutex);
#endif
	BLEnum _etype = _Type;
    if (_PrSystemMem->nEventIdx >= _PrSystemMem->nEventsSz)
    {
		_PrSystemMem->nEventsSz = 2 * _PrSystemMem->nEventIdx + 2;
		_PrSystemMem->pEvents = (_BLEvent*)realloc(_PrSystemMem->pEvents, _PrSystemMem->nEventsSz * sizeof(_BLEvent));
    }
    if (_etype == BL_ET_NET)
    {
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].eType = _Type;
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sNet.nID = _Uparam;
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sNet.pBuf = (BLVoid*)_Pparam;
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sNet.nLength = _Sparam;
    }
    else if (_etype == BL_ET_UI)
    {
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].eType = _etype;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sUI.nSParam = _Sparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sUI.nUParam = _Uparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sUI.pPParam = (BLVoid*)_Pparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sUI.nID = _ID;
    }
    else if (_etype == BL_ET_MOUSE)
    {
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].eType = _etype;
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sMouse.nX = LOWU16(_Uparam);
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sMouse.nY = HIGU16(_Uparam);
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sMouse.eEvent = _Sparam;
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sMouse.nWheel = (_Sparam == BL_ME_WHEEL) ? (BLS32)_Uparam : 0;
    }
    else if (_etype == BL_ET_KEY)
    {
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].eType = _etype;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sKey.eCode = HIGU16(_Uparam);
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sKey.bPressed = (BLBool)LOWU16(_Uparam);
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sKey.pString = NULL;
        if (_Pparam)
        {
            const BLUtf8* _tmp = (const BLUtf8*)_Pparam;
            BLU32 _cnt = 0;
            while (*_tmp++)
                _cnt++;
			_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sKey.pString = (BLUtf8*)malloc((1 + _cnt)*sizeof(BLUtf8));
            memcpy(_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sKey.pString, _Pparam, _cnt*sizeof(BLUtf8));
			_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sKey.pString[_cnt] = 0;
        }
    }
	else if (_etype == BL_ET_SPRITE)
	{
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].eType = _etype;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sSprite.nSParam = _Sparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sSprite.nUParam = _Uparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sSprite.nID = _ID;
	}
    else
    {
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].eType = _etype;
        _PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sSys.nSParam = _Sparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sSys.nUParam = _Uparam;
		_PrSystemMem->pEvents[_PrSystemMem->nEventIdx].uEvent.sSys.pPParam = (BLVoid*)_Pparam;
    }
	_PrSystemMem->nEventIdx++;
#ifdef BL_PLATFORM_ANDROID
	if (!_busy)
		pthread_mutex_unlock(&_PrSystemMem->sMutex);
#endif
}
BLVoid
blDateTime(OUT BLS32* _Year, OUT BLS32* _Month, OUT BLS32* _Day, OUT BLS32* _Hour, OUT BLS32* _Minute, OUT BLS32* _Second, OUT BLS32* _Wday, OUT BLS32* _Yday, OUT BLS32* _Dst)
{
	time_t _timer;
	struct tm* _tblock;
	_timer = time(NULL);
	_tblock = localtime(&_timer);
	*_Year = _tblock->tm_year + 1900;
	*_Month = _tblock->tm_mon + 1;
	*_Day = _tblock->tm_mday;
	*_Hour = _tblock->tm_hour;
	*_Minute = _tblock->tm_min;
	*_Second = _tblock->tm_sec;
	*_Wday = _tblock->tm_wday;
	*_Yday = _tblock->tm_yday;
	*_Dst = _tblock->tm_isdst;
}
BLBool
blTimer(IN BLS32 _PositiveID, IN BLF32 _Elapse)
{
	BLU32 _idx = 0;
	BLBool _full = TRUE;
	for (_idx = 0 ; _idx < 8 ; ++_idx)
	{
		if (_PrSystemMem->aTimers[_idx].nId == -1)
		{
			_full = FALSE;
			break;
		}
	}
	if (_full)
		return FALSE;
	_PrSystemMem->aTimers[_idx].nId = _PositiveID;
	_PrSystemMem->aTimers[_idx].fElapse = _Elapse;
	_PrSystemMem->aTimers[_idx].nLastTime = blTickCounts();
	return TRUE;
}
BLVoid
blWindowQuery(OUT BLU32* _Width, OUT BLU32* _Height, OUT BLU32* _ActualWidth, OUT BLU32* _ActualHeight, OUT BLF32* _RatioX, OUT BLF32* _RatioY)
{
	*_Width = _PrSystemMem->sBoostParam.nScreenWidth;
	*_Height = _PrSystemMem->sBoostParam.nScreenHeight;
	if (_PrSystemMem->sBoostParam.bUseDesignRes)
	{
		BLF32 _dwidth = (BLF32)_PrSystemMem->sBoostParam.nDesignWidth;
		BLF32 _dheight = (BLF32)_PrSystemMem->sBoostParam.nDesignHeight;
		BLF32 _ratioorg = (BLF32)(*_Width) / (BLF32)(*_Height);
		BLF32 _ratiodeg = (BLF32)(_dwidth) / (BLF32)(_dheight);
		if (_ratiodeg >= _ratioorg)
		{
			*_ActualHeight = (BLU32)_dheight;
			*_ActualWidth = (BLU32)(_ratioorg * _dheight);
		}
		else
		{
			*_ActualHeight = (BLU32)(_dwidth / _ratioorg);
			*_ActualWidth = (BLU32)_dwidth;
		}
		*_RatioX = (BLF32)(*_Width) / (BLF32)(*_ActualWidth);
		*_RatioY = (BLF32)(*_Height) / (BLF32)(*_ActualHeight);
	}
	else
	{
		*_ActualWidth = *_Width;
		*_ActualHeight = *_Height;
		*_RatioX = 1.f;
		*_RatioY = 1.f;
	}
}
BLVoid
blWindowResize(IN BLU32 _Width, IN BLU32 _Height, IN BLBool _Fullscreen)
{
#if defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_UWP)
    if (_Fullscreen)
        _EnterFullscreen();
    else
        _ExitFullscreen(_Width, _Height);
	BLAnsi _tmp[256] = { 0 };
	sprintf(_tmp, "%d", _PrSystemMem->sBoostParam.nScreenWidth);
	blEnvVariable((const BLUtf8*)"SCREEN_WIDTH", (BLUtf8*)_tmp);
	memset(_tmp, 0, sizeof(_tmp));
	sprintf(_tmp, "%d", _PrSystemMem->sBoostParam.nScreenHeight);
	blEnvVariable((const BLUtf8*)"SCREEN_HEIGHT", (BLUtf8*)_tmp);
	memset(_tmp, 0, sizeof(_tmp));
	if (_PrSystemMem->sBoostParam.bFullscreen)
		strcpy(_tmp, "true");
	else
		strcpy(_tmp, "false");
	blEnvVariable((const BLUtf8*)"FULLSCREEN", (BLUtf8*)_tmp);
	memset(_tmp, 0, sizeof(_tmp));
	sprintf(_tmp, "%d", _PrSystemMem->sBoostParam.eQuality);
	blEnvVariable((const BLUtf8*)"QUALITY", (BLUtf8*)_tmp);
#endif
}
BLVoid
blSystemPrepare(IN BLVoid* _Activity, IN BLVoid* _State, IN BLU32 _StateSize)
{
#if defined(BL_PLATFORM_ANDROID)
	ANativeActivity* _activity = (ANativeActivity*)_Activity;
	_activity->callbacks->onDestroy = _Destroy;
	_activity->callbacks->onStart = _Start;
	_activity->callbacks->onResume = _Resume;
	_activity->callbacks->onSaveInstanceState = _SaveInstanceState;
	_activity->callbacks->onPause = _Pause;
	_activity->callbacks->onStop = _Stop;
	_activity->callbacks->onConfigurationChanged = _ConfigurationChanged;
	_activity->callbacks->onLowMemory = _LowMemory;
	_activity->callbacks->onWindowFocusChanged = _WindowFocusChanged;
	_activity->callbacks->onNativeWindowCreated = _NativeWindowCreated;
	_activity->callbacks->onNativeWindowDestroyed = _NativeWindowDestroyed;
	_activity->callbacks->onInputQueueCreated = _InputQueueCreated;
	_activity->callbacks->onInputQueueDestroyed = _InputQueueDestroyed;
	_PrSystemMem = (_BLSystemMember*)malloc(sizeof(_BLSystemMember));
	_PrSystemMem->pActivity = _activity;
	_PrSystemMem->pBLJava = NULL;
	_PrSystemMem->pWindow = NULL;
	_PrSystemMem->pSavedState = NULL;
	_PrSystemMem->pInputQueue = NULL;
	pthread_mutex_init(&_PrSystemMem->sMutex, NULL);
	pthread_cond_init(&_PrSystemMem->sCond, NULL);
	if (_State != NULL)
	{
		_PrSystemMem->pSavedState = malloc(_StateSize);
		_PrSystemMem->nSavedStateSize = _StateSize;
		memcpy(_PrSystemMem->pSavedState, _State, _StateSize);
	}
	BLS32 _msgpipe[2];
	pipe(_msgpipe);
	_PrSystemMem->nMsgRead = _msgpipe[0];
	_PrSystemMem->nMsgWrite = _msgpipe[1];
	_PrSystemMem->sNativeMethod.name = "textChanged";
	_PrSystemMem->sNativeMethod.signature = "(Ljava/lang/String;)V";
	_PrSystemMem->sNativeMethod.fnPtr = (BLVoid*)_TextChanged;
	pthread_attr_t _attr;
	pthread_attr_init(&_attr);
	pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&_PrSystemMem->nThread, &_attr, _AppEntry, NULL);
	pthread_mutex_lock(&_PrSystemMem->sMutex);
	while (!_GbSystemRunning)
		pthread_cond_wait(&_PrSystemMem->sCond, &_PrSystemMem->sMutex);
	pthread_mutex_unlock(&_PrSystemMem->sMutex);
#else
#endif
}
BLVoid
blSystemRun(IN BLAnsi* _Appname, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _DesignWidth, IN BLU32 _DesignHeight, IN BLBool _UseDesignRes, IN BLBool _Fullscreen, IN BLBool _Profiler, IN BLEnum _Quality, IN BLVoid(*_Begin)(BLVoid), IN BLVoid(*_Step)(BLU32), IN BLVoid(*_End)(BLVoid))
{
#if defined(BL_PLATFORM_UWP)
	_PrSystemMem = new _BLSystemMember;
#elif defined(BL_PLATFORM_ANDROID)
	if (!_PrAndroidGlue)
	{
		_PrAndroidGlue = (_BLBoostParam*)malloc(sizeof(_BLBoostParam));
		memset(_PrAndroidGlue->pAppName, 0, sizeof(_PrAndroidGlue->pAppName));
		strcpy((BLAnsi*)_PrAndroidGlue->pAppName, _Appname);
		_PrAndroidGlue->nScreenWidth = _Width;
		_PrAndroidGlue->nScreenHeight = _Height;
		_PrAndroidGlue->nDesignWidth = _DesignWidth;
		_PrAndroidGlue->nDesignHeight = _DesignHeight;
		_PrAndroidGlue->bUseDesignRes = _UseDesignRes;
		_PrAndroidGlue->bFullscreen = _Fullscreen;
		_PrAndroidGlue->bProfiler = _Profiler;
		_PrAndroidGlue->eQuality = _Quality;
		_PrAndroidGlue->pBeginFunc = _Begin;
		_PrAndroidGlue->pStepFunc = _Step;
		_PrAndroidGlue->pEndFunc = _End;
		return;
	}
#else
	_PrSystemMem = (_BLSystemMember*)malloc(sizeof(_BLSystemMember));
#endif
	memset(_PrSystemMem->pSubscriber, 0, sizeof(_PrSystemMem->pSubscriber));
	_PrSystemMem->pBeginFunc = NULL;
	_PrSystemMem->pStepFunc = NULL;
	_PrSystemMem->pEndFunc = NULL;
	_PrSystemMem->pEvents = NULL;
	_PrSystemMem->pDukContext = NULL;
	memset(_PrSystemMem->aWorkDir, 0, sizeof(_PrSystemMem->aWorkDir));
	memset(_PrSystemMem->aUserDir, 0, sizeof(_PrSystemMem->aUserDir));
	memset(_PrSystemMem->aEncryptkey, 0, sizeof(_PrSystemMem->aEncryptkey));
	_PrSystemMem->nEventsSz = 0;
	_PrSystemMem->nEventIdx = 0;
	_PrSystemMem->nSysTime = 0;
	_PrSystemMem->nOrientation = SCREEN_LANDSCAPE_INTERNAL;
	for (BLU32 _idx = 0; _idx < 64; ++_idx)
		_PrSystemMem->aPlugins[_idx].nHash = 0;
#if defined(BL_PLATFORM_WIN32)
	_PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_UWP)
	_PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_LINUX)
    _PrSystemMem->pDisplay = NULL;
    _PrSystemMem->pIME = NULL;
    _PrSystemMem->pIC = NULL;
    _PrSystemMem->pLib = NULL;
    _PrSystemMem->bCtrlPressed = FALSE;
	_PrSystemMem->bEWMHSupported = FALSE;
#elif defined(BL_PLATFORM_ANDROID)
    _PrSystemMem->bAvtivityFocus = FALSE;
    _PrSystemMem->bBackendState = 0;
#elif defined(BL_PLATFORM_OSX)
	_PrSystemMem->pPool = nil;
	_PrSystemMem->pWindow = nil;
    _PrSystemMem->pTICcxt = nil;
    _PrSystemMem->pDelegate = nil;
    _PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_IOS)
	_PrSystemMem->pPool = nil;
    _PrSystemMem->pWindow = nil;
    _PrSystemMem->pTICcxt = nil;
    _PrSystemMem->nKeyboardHeight = 0;
    _PrSystemMem->pCtlView = nil;
#endif
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
		_PrSystemMem->aTimers[_idx].nId = -1;
	memset(_PrSystemMem->sBoostParam.pAppName, 0, sizeof(_PrSystemMem->sBoostParam.pAppName));
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	strcpy_s((BLAnsi*)_PrSystemMem->sBoostParam.pAppName, 128, _Appname);
#elif defined(BL_PLATFORM_ANDROID)
	strcpy((BLAnsi*)_PrSystemMem->sBoostParam.pAppName, (const BLAnsi*)_PrAndroidGlue->pAppName);
#else
	strcpy((BLAnsi*)_PrSystemMem->sBoostParam.pAppName, _Appname);
#endif
	if (_Width + _Height == 0)
	{
		BLAnsi _tmp[256] = { 0 };
		if (blEnvVariable((const BLUtf8*)"SCREEN_WIDTH", (BLUtf8*)_tmp))
			_PrSystemMem->sBoostParam.nScreenWidth = (BLU32)strtoul(_tmp, NULL, 10);
		else
			_PrSystemMem->sBoostParam.nScreenWidth = 0;
		memset(_tmp, 0, sizeof(_tmp));
		if (blEnvVariable((const BLUtf8*)"SCREEN_HEIGHT", (BLUtf8*)_tmp))
			_PrSystemMem->sBoostParam.nScreenHeight = (BLU32)strtoul(_tmp, NULL, 10);
		else
			_PrSystemMem->sBoostParam.nScreenHeight = 0;
		memset(_tmp, 0, sizeof(_tmp));
		if (blEnvVariable((const BLUtf8*)"FULLSCREEN", (BLUtf8*)_tmp))
			_PrSystemMem->sBoostParam.bFullscreen = strcmp(_tmp, "true") ? FALSE : TRUE;
		else
			_PrSystemMem->sBoostParam.bFullscreen = FALSE;
		memset(_tmp, 0, sizeof(_tmp));
		if (blEnvVariable((const BLUtf8*)"QUALITY", (BLUtf8*)_tmp))
			_PrSystemMem->sBoostParam.eQuality = (BLU32)strtoul(_tmp, NULL, 10);
		else
			_PrSystemMem->sBoostParam.eQuality = BL_RQ_NORMAL;
	}
	else
	{
#if defined(BL_PLATFORM_ANDROID)
		_PrSystemMem->sBoostParam.nScreenWidth = _PrAndroidGlue->nScreenWidth;
		_PrSystemMem->sBoostParam.nScreenHeight = _PrAndroidGlue->nScreenHeight;
		_PrSystemMem->sBoostParam.bFullscreen = _PrAndroidGlue->bFullscreen;
		_PrSystemMem->sBoostParam.eQuality = _GbRenderQuality = _PrAndroidGlue->eQuality;
#else
		_PrSystemMem->sBoostParam.nScreenWidth = _Width;
		_PrSystemMem->sBoostParam.nScreenHeight = _Height;
		_PrSystemMem->sBoostParam.bFullscreen = _Fullscreen;
		_PrSystemMem->sBoostParam.eQuality = _GbRenderQuality = _Quality;
#endif
	}
#if defined(BL_PLATFORM_ANDROID)
	_PrSystemMem->sBoostParam.nDesignWidth = _PrAndroidGlue->nDesignWidth;
	_PrSystemMem->sBoostParam.nDesignHeight = _PrAndroidGlue->nDesignHeight;
	_PrSystemMem->sBoostParam.bUseDesignRes = _PrAndroidGlue->bUseDesignRes;
	_PrSystemMem->sBoostParam.bProfiler = _PrAndroidGlue->bProfiler;
    _PrSystemMem->pBeginFunc = _PrAndroidGlue->pBeginFunc;
    _PrSystemMem->pStepFunc = _PrAndroidGlue->pStepFunc;
    _PrSystemMem->pEndFunc = _PrAndroidGlue->pEndFunc;
	free(_PrAndroidGlue);
	_PrAndroidGlue = NULL;
#else
	_PrSystemMem->sBoostParam.nDesignWidth = _DesignWidth;
	_PrSystemMem->sBoostParam.nDesignHeight = _DesignHeight;
	_PrSystemMem->sBoostParam.bUseDesignRes = _UseDesignRes;
	_PrSystemMem->sBoostParam.bProfiler = _Profiler;
    _PrSystemMem->pBeginFunc = _Begin;
    _PrSystemMem->pStepFunc = _Step;
    _PrSystemMem->pEndFunc = _End;
#endif
    _SystemInit();
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_ANDROID)
    do {
        _SystemStep();
    } while (_GbSystemRunning);
    _SystemDestroy();
#endif
}
BLVoid
blSystemEmbedRun(IN BLS32 _Handle, IN BLVoid(*_Begin)(BLVoid), IN BLVoid(*_Step)(BLU32), IN BLVoid(*_End)(BLVoid))
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_OSX)
	_PrSystemMem = (_BLSystemMember*)malloc(sizeof(_BLSystemMember));
	memset(_PrSystemMem->pSubscriber, 0, sizeof(_PrSystemMem->pSubscriber));
	_PrSystemMem->pBeginFunc = NULL;
	_PrSystemMem->pStepFunc = NULL;
	_PrSystemMem->pEndFunc = NULL;
	_PrSystemMem->pEvents = NULL;
	memset(_PrSystemMem->aWorkDir, 0, sizeof(_PrSystemMem->aWorkDir));
	memset(_PrSystemMem->aUserDir, 0, sizeof(_PrSystemMem->aUserDir));
	memset(_PrSystemMem->aEncryptkey, 0, sizeof(_PrSystemMem->aEncryptkey));
	_PrSystemMem->nEventsSz = 0;
	_PrSystemMem->nEventIdx = 0;
	_PrSystemMem->nSysTime = 0;
	_PrSystemMem->nOrientation = SCREEN_LANDSCAPE_INTERNAL;
	for (BLU32 _idx = 0; _idx < 64; ++_idx)
		_PrSystemMem->aPlugins[_idx].nHash = 0;
#if defined(BL_PLATFORM_WIN32)
    _PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_LINUX)
    _PrSystemMem->pDisplay = NULL;
    _PrSystemMem->pIME = NULL;
    _PrSystemMem->pIC = NULL;
    _PrSystemMem->pLib = NULL;
    _PrSystemMem->bCtrlPressed = FALSE;
	_PrSystemMem->bEWMHSupported = FALSE;
#elif defined(BL_PLATFORM_OSX)
    _PrSystemMem->pPool = nil;
    _PrSystemMem->pWindow = nil;
    _PrSystemMem->pDelegate = nil;
    _PrSystemMem->pTICcxt = nil;
    _PrSystemMem->bCtrlPressed = FALSE;
#endif
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
		_PrSystemMem->aTimers[_idx].nId = -1;
	_PrSystemMem->pBeginFunc = _Begin;
	_PrSystemMem->pStepFunc = _Step;
	_PrSystemMem->pEndFunc = _End;
	_SystemInit();
	do {
		_SystemStep();
	} while (_GbSystemRunning);
	_SystemDestroy();
#endif
}
BLVoid
blSystemScriptRun(IN BLAnsi* _Encryptkey)
{
#if defined(BL_PLATFORM_UWP)
	_PrSystemMem = new _BLSystemMember;
#elif defined(BL_PLATFORM_ANDROID)
#else
	_PrSystemMem = (_BLSystemMember*)malloc(sizeof(_BLSystemMember));
#endif
	memset(_PrSystemMem->pSubscriber, 0, sizeof(_PrSystemMem->pSubscriber));
	_PrSystemMem->pBeginFunc = NULL;
	_PrSystemMem->pStepFunc = NULL;
	_PrSystemMem->pEndFunc = NULL;
	_PrSystemMem->pEvents = NULL;
	_PrSystemMem->pDukContext = duk_create_heap_default();
	memset(_PrSystemMem->aWorkDir, 0, sizeof(_PrSystemMem->aWorkDir));
	memset(_PrSystemMem->aUserDir, 0, sizeof(_PrSystemMem->aUserDir));
	memset(_PrSystemMem->aEncryptkey, 0, sizeof(_PrSystemMem->aEncryptkey));
	strcpy(_PrSystemMem->aEncryptkey, _Encryptkey);
	_PrSystemMem->nEventsSz = 0;
	_PrSystemMem->nEventIdx = 0;
	_PrSystemMem->nSysTime = 0;
	_PrSystemMem->nOrientation = SCREEN_LANDSCAPE_INTERNAL;
	for (BLU32 _idx = 0; _idx < 64; ++_idx)
		_PrSystemMem->aPlugins[_idx].nHash = 0;
	BLAnsi _tmp[256] = { 0 };
	if (blEnvVariable((const BLUtf8*)"SCREEN_WIDTH", (BLUtf8*)_tmp))
		_PrSystemMem->sBoostParam.nScreenWidth = (BLU32)strtoul(_tmp, NULL, 10);
	else
		_PrSystemMem->sBoostParam.nScreenWidth = 0;
	memset(_tmp, 0, sizeof(_tmp));
	if (blEnvVariable((const BLUtf8*)"SCREEN_HEIGHT", (BLUtf8*)_tmp))
		_PrSystemMem->sBoostParam.nScreenHeight = (BLU32)strtoul(_tmp, NULL, 10);
	else
		_PrSystemMem->sBoostParam.nScreenHeight = 0;
	memset(_tmp, 0, sizeof(_tmp));
	if (blEnvVariable((const BLUtf8*)"FULLSCREEN", (BLUtf8*)_tmp))
		_PrSystemMem->sBoostParam.bFullscreen = strcmp(_tmp, "true") ? FALSE : TRUE;
	else
		_PrSystemMem->sBoostParam.bFullscreen = FALSE;
	memset(_tmp, 0, sizeof(_tmp));
	if (blEnvVariable((const BLUtf8*)"QUALITY", (BLUtf8*)_tmp))
		_PrSystemMem->sBoostParam.eQuality = (BLU32)strtoul(_tmp, NULL, 10);
	else
		_PrSystemMem->sBoostParam.eQuality = BL_RQ_NORMAL;
#if defined(BL_PLATFORM_WIN32)
	_PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_UWP)
	_PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_LINUX)
	_PrSystemMem->pDisplay = NULL;
	_PrSystemMem->pIME = NULL;
	_PrSystemMem->pIC = NULL;
	_PrSystemMem->pLib = NULL;
	_PrSystemMem->bCtrlPressed = FALSE;
	_PrSystemMem->bEWMHSupported = FALSE;
#elif defined(BL_PLATFORM_ANDROID)
	_PrSystemMem->bAvtivityFocus = FALSE;
	_PrSystemMem->bBackendState = 0;
#elif defined(BL_PLATFORM_OSX)
	_PrSystemMem->pPool = nil;
	_PrSystemMem->pWindow = nil;
	_PrSystemMem->pTICcxt = nil;
	_PrSystemMem->pDelegate = nil;
	_PrSystemMem->bCtrlPressed = FALSE;
#elif defined(BL_PLATFORM_IOS)
	_PrSystemMem->pPool = nil;
	_PrSystemMem->pWindow = nil;
	_PrSystemMem->pTICcxt = nil;
	_PrSystemMem->nKeyboardHeight = 0;
	_PrSystemMem->pCtlView = nil;
#endif
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
		_PrSystemMem->aTimers[_idx].nId = -1;
	memset(_PrSystemMem->sBoostParam.pAppName, 0, sizeof(_PrSystemMem->sBoostParam.pAppName));
	_SystemInit();
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_ANDROID)
	do {
		_SystemStep();
	} while (_GbSystemRunning);
	_SystemDestroy();
#endif
}
