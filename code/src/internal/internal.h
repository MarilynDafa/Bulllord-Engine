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
#ifndef __internal_h_
#define __internal_h_
#include "../../headers/prerequisites.h"
#if defined(BL_PLATFORM_WIN32)
#include "../gl/glcorearb.h"
#include "../gl/wglext.h"
#elif defined(BL_PLATFORM_LINUX)
#include "../gl/glcorearb.h"
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef struct __GLXcontextRec *GLXContext;
#include "../gl/glxext.h"
#elif defined(BL_PLATFORM_OSX)
#include "../gl/glcorearb.h"
#include <CoreGraphics/CoreGraphics.h>
#elif defined(BL_PLATFORM_IOS)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif defined(BL_PLATFORM_ANDROID)
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>
#ifndef GL_GLEXT_PROTOTYPES
#   define GL_GLEXT_PROTOTYPES 1
#endif
#elif defined(BL_PLATFORM_UWP)
#include <d3d12.h>
#include <dxgi1_5.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
/* Math */
#define EPSILON_INTERNAL 0.00001f
#define PI_INTERNAL 3.141592653589793f
#define MIN_INTERNAL(x,y) ((x)<(y)?(x):(y))
#define MAX_INTERNAL(x,y) ((x)>(y)?(x):(y))
/* UI */
#define PIXEL_ALIGNED_INTERNAL(pixel) ((BLF32)(BLS32)((pixel) + ((pixel)>0.0f?0.5f:-0.5f)))
/* File */
#if defined(_WIN64) || defined(WIN64)
#	define FILE_INVALID_INTERNAL(fp) ((fp) && (fp) != (BLVoid*)0x00000000FFFFFFFFULL)
#else
#	define FILE_INVALID_INTERNAL(fp) ((fp) && (fp) != (BLVoid*)0xFFFFFFFF)
#endif
#define FOURCC_INTERNAL(ch0, ch1, ch2, ch3) ((ch0<<0) + (ch1<<8) + (ch2<<16) + (ch3<<24))
/* GUID */
#define URIPART_INTERNAL(val) ((BLU32)(((BLU64)(val)) & 0xFFFFFFFF))
#define PTRPART_INTERNAL(val) ((BLU32)((((BLU64)(val)) >> 32) & 0xFFFFFFFF))
#define MAKEGUID_INTERNAL(sys, user) ((BLU64)(((BLU32)(((BLU64)(user)) & 0xFFFFFFFF)) | ((BLU64)((BLU32)(((BLU64)(sys)) & 0xFFFFFFFF))) << 32))
/*Managed Resource Config*/
#define IB_CAP_INTERNAL				4 << 11
#define VB_CAP_INTERNAL				4 << 11
#define UB_CAP_INTERNAL				64
#define VD_CAP_INTERNAL				64
#define DIB_CAP_INTERNAL			4 << 11
#define DVB_CAP_INTERNAL			4 << 11
#define SHADER_CAP_INTERNAL			128
#define TECH_CAP_INTERNAL			128
#define FBO_CAP_INTERNAL			128
#define SAMPLER_CAP_INTERNAL		1024
#define TEX_CAP_INTERNAL			4096
#define OBJ_CAP_INTERNAL			0x1FFFF
#define MEMCACHE_CAP_INTERNAL		(IB_CAP_INTERNAL) + (VB_CAP_INTERNAL) + (UB_CAP_INTERNAL) + (VD_CAP_INTERNAL) + (DIB_CAP_INTERNAL) + (DVB_CAP_INTERNAL) + (SHADER_CAP_INTERNAL) + (TECH_CAP_INTERNAL) + (FBO_CAP_INTERNAL) + (SAMPLER_CAP_INTERNAL) + (TEX_CAP_INTERNAL) + (OBJ_CAP_INTERNAL)
/* Socket */
#define INVALID_SOCKET_INTERNAL         ~0
/* Screen Orientation*/
#define SCREEN_LANDSCAPE_INTERNAL		0
#define SCREEN_PORTRAIT_INTERNAL		1
/* Sprite Action Type*/
#define SPACTION_MOVE_INTERNAL			0
#define SPACTION_ROTATE_INTERNAL		1
#define SPACTION_SCALE_INTERNAL			2
#define SPACTION_ALPHA_INTERNAL			3
#define SPACTION_DEAD_INTERNAL			4
/* UI Action Type */
#define UIACTION_UV_INTERNAL			0
#define UIACTION_MOVE_INTERNAL			1
#define UIACTION_ROTATE_INTERNAL		2
#define UIACTION_SCALE_INTERNAL			4
#define UIACTION_ALPHA_INTERNAL			5
#define UIACTION_DEAD_INTERNAL			6
/* JS Bind */
#define JS_FUNCTION_INTERNAL(func) static BLS32 _##func##JS(duk_context* _DKC)
/* Default Font*/
extern const BLAnsi* DEFAULTFONT_INTERNAL;
extern const BLAnsi* DEFAULTFONTIMG_INTERNAL;
/* Platform Scancode*/
#if defined(BL_PLATFORM_WIN32)
    static const BLEnum SCANCODE_INTERNAL[] =
    {
        BL_KC_UNKNOWN,
        BL_KC_ESCAPE,
        BL_KC_1,
        BL_KC_2,
        BL_KC_3,
        BL_KC_4,
        BL_KC_5,
        BL_KC_6,
        BL_KC_7,
        BL_KC_8,
        BL_KC_9,
        BL_KC_0,
        BL_KC_MINUS,
        BL_KC_EQUALS,
        BL_KC_BACKSPACE,
        BL_KC_TAB,
        BL_KC_Q,
        BL_KC_W,
        BL_KC_E,
        BL_KC_R,
        BL_KC_T,
        BL_KC_Y,
        BL_KC_U,
        BL_KC_I,
        BL_KC_O,
        BL_KC_P,
        BL_KC_LEFTBRACKET,
        BL_KC_RIGHTBRACKET,
        BL_KC_RETURN,
        BL_KC_LCTRL,
        BL_KC_A,
        BL_KC_S,
        BL_KC_D,
        BL_KC_F,
        BL_KC_G,
        BL_KC_H,
        BL_KC_J,
        BL_KC_K,
        BL_KC_L,
        BL_KC_SEMICOLON,
        BL_KC_APOSTROPHE,
        BL_KC_GRAVE,
        BL_KC_LSHIFT,
        BL_KC_BACKSLASH,
        BL_KC_Z,
        BL_KC_X,
        BL_KC_C,
        BL_KC_V,
        BL_KC_B,
        BL_KC_N,
        BL_KC_M,
        BL_KC_COMMA,
        BL_KC_PERIOD,
        BL_KC_SLASH,
        BL_KC_RSHIFT,
        BL_KC_PRINTSCREEN,
        BL_KC_LALT,
        BL_KC_SPACE,
        BL_KC_CAPSLOCK,
        BL_KC_F1,
        BL_KC_F2,
        BL_KC_F3,
        BL_KC_F4,
        BL_KC_F5,
        BL_KC_F6,
        BL_KC_F7,
        BL_KC_F8,
        BL_KC_F9,
        BL_KC_F10,
        BL_KC_NUMLOCKCLEAR,
        BL_KC_SCROLLLOCK,
        BL_KC_HOME,
        BL_KC_UP,
        BL_KC_PAGEUP,
        BL_KC_KP_MINUS,
        BL_KC_LEFT,
        BL_KC_KP_5,
        BL_KC_RIGHT,
        BL_KC_KP_PLUS,
        BL_KC_END,
        BL_KC_DOWN,
        BL_KC_PAGEDOWN,
        BL_KC_INSERT,
        BL_KC_DELETE,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_NONUSBACKSLASH,
        BL_KC_F11,
        BL_KC_F12,
        BL_KC_PAUSE,
        BL_KC_UNKNOWN,
        BL_KC_LGUI,
        BL_KC_RGUI,
        BL_KC_APPLICATION,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_F13,
        BL_KC_F14,
        BL_KC_F15,
        BL_KC_F16,
        BL_KC_F17,
        BL_KC_F18,
        BL_KC_F19,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL2,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL1,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL4,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL5,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL3,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN
    };
#elif defined(BL_PLATFORM_UWP)
    static const BLEnum SCANCODE_INTERNAL[] =
    {
        BL_KC_UNKNOWN,
        BL_KC_ESCAPE,
        BL_KC_1,
        BL_KC_2,
        BL_KC_3,
        BL_KC_4,
        BL_KC_5,
        BL_KC_6,
        BL_KC_7,
        BL_KC_8,
        BL_KC_9,
        BL_KC_0,
        BL_KC_MINUS,
        BL_KC_EQUALS,
        BL_KC_BACKSPACE,
        BL_KC_TAB,
        BL_KC_Q,
        BL_KC_W,
        BL_KC_E,
        BL_KC_R,
        BL_KC_T,
        BL_KC_Y,
        BL_KC_U,
        BL_KC_I,
        BL_KC_O,
        BL_KC_P,
        BL_KC_LEFTBRACKET,
        BL_KC_RIGHTBRACKET,
        BL_KC_RETURN,
        BL_KC_LCTRL,
        BL_KC_A,
        BL_KC_S,
        BL_KC_D,
        BL_KC_F,
        BL_KC_G,
        BL_KC_H,
        BL_KC_J,
        BL_KC_K,
        BL_KC_L,
        BL_KC_SEMICOLON,
        BL_KC_APOSTROPHE,
        BL_KC_GRAVE,
        BL_KC_LSHIFT,
        BL_KC_BACKSLASH,
        BL_KC_Z,
        BL_KC_X,
        BL_KC_C,
        BL_KC_V,
        BL_KC_B,
        BL_KC_N,
        BL_KC_M,
        BL_KC_COMMA,
        BL_KC_PERIOD,
        BL_KC_SLASH,
        BL_KC_RSHIFT,
        BL_KC_PRINTSCREEN,
        BL_KC_LALT,
        BL_KC_SPACE,
        BL_KC_CAPSLOCK,
        BL_KC_F1,
        BL_KC_F2,
        BL_KC_F3,
        BL_KC_F4,
        BL_KC_F5,
        BL_KC_F6,
        BL_KC_F7,
        BL_KC_F8,
        BL_KC_F9,
        BL_KC_F10,
        BL_KC_NUMLOCKCLEAR,
        BL_KC_SCROLLLOCK,
        BL_KC_HOME,
        BL_KC_UP,
        BL_KC_PAGEUP,
        BL_KC_KP_MINUS,
        BL_KC_LEFT,
        BL_KC_KP_5,
        BL_KC_RIGHT,
        BL_KC_KP_PLUS,
        BL_KC_END,
        BL_KC_DOWN,
        BL_KC_PAGEDOWN,
        BL_KC_INSERT,
        BL_KC_DELETE,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_NONUSBACKSLASH,
        BL_KC_F11,
        BL_KC_F12,
        BL_KC_PAUSE,
        BL_KC_UNKNOWN,
        BL_KC_LGUI,
        BL_KC_RGUI,
        BL_KC_APPLICATION,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_F13,
        BL_KC_F14,
        BL_KC_F15,
        BL_KC_F16,
        BL_KC_F17,
        BL_KC_F18,
        BL_KC_F19,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL2,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL1,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL4,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL5,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL3,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN
    };
    static const BLEnum OFFICIALCODE_INTERNAL[] =
    {
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_CANCEL,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_BACKSPACE,
        BL_KC_TAB,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_CLEAR,
        BL_KC_RETURN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_LSHIFT,
        BL_KC_LCTRL,
        BL_KC_MENU,
        BL_KC_PAUSE,
        BL_KC_CAPSLOCK,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_ESCAPE,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_SPACE,
        BL_KC_PAGEUP,
        BL_KC_PAGEDOWN,
        BL_KC_END,
        BL_KC_HOME,
        BL_KC_LEFT,
        BL_KC_UP,
        BL_KC_RIGHT,
        BL_KC_DOWN,
        BL_KC_SELECT,
        BL_KC_UNKNOWN,
        BL_KC_EXECUTE,
        BL_KC_UNKNOWN,
        BL_KC_INSERT,
        BL_KC_DELETE,
        BL_KC_HELP,
        BL_KC_0,
        BL_KC_1,
        BL_KC_2,
        BL_KC_3,
        BL_KC_4,
        BL_KC_5,
        BL_KC_6,
        BL_KC_7,
        BL_KC_8,
        BL_KC_9,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_A,
        BL_KC_B,
        BL_KC_C,
        BL_KC_D,
        BL_KC_E,
        BL_KC_F,
        BL_KC_G,
        BL_KC_H,
        BL_KC_I,
        BL_KC_J,
        BL_KC_K,
        BL_KC_L,
        BL_KC_M,
        BL_KC_N,
        BL_KC_O,
        BL_KC_P,
        BL_KC_Q,
        BL_KC_R,
        BL_KC_S,
        BL_KC_T,
        BL_KC_U,
        BL_KC_V,
        BL_KC_W,
        BL_KC_X,
        BL_KC_Y,
        BL_KC_Z,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_APPLICATION,
        BL_KC_UNKNOWN,
        BL_KC_SLEEP,
        BL_KC_KP_0,
        BL_KC_KP_1,
        BL_KC_KP_2,
        BL_KC_KP_3,
        BL_KC_KP_4,
        BL_KC_KP_5,
        BL_KC_KP_6,
        BL_KC_KP_7,
        BL_KC_KP_8,
        BL_KC_KP_9,
        BL_KC_KP_MULTIPLY,
        BL_KC_KP_PLUS,
        BL_KC_UNKNOWN,
        BL_KC_KP_MINUS,
        BL_KC_UNKNOWN,
        BL_KC_KP_DIVIDE,
        BL_KC_F1,
        BL_KC_F2,
        BL_KC_F3,
        BL_KC_F4,
        BL_KC_F5,
        BL_KC_F6,
        BL_KC_F7,
        BL_KC_F8,
        BL_KC_F9,
        BL_KC_F10,
        BL_KC_F11,
        BL_KC_F12,
        BL_KC_F13,
        BL_KC_F14,
        BL_KC_F15,
        BL_KC_F16,
        BL_KC_F17,
        BL_KC_F18,
        BL_KC_F19,
        BL_KC_F20,
        BL_KC_F21,
        BL_KC_F22,
        BL_KC_F23,
        BL_KC_F24,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_NUMLOCKCLEAR,
        BL_KC_SCROLLLOCK,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_LSHIFT,
        BL_KC_RSHIFT,
        BL_KC_LCTRL,
        BL_KC_RCTRL,
        BL_KC_MENU,
        BL_KC_MENU,
        BL_KC_AC_BACK,
        BL_KC_AC_FORWARD,
        BL_KC_AC_REFRESH,
        BL_KC_AC_STOP,
        BL_KC_AC_SEARCH,
        BL_KC_AC_BOOKMARKS,
        BL_KC_AC_HOME
    };
#elif defined(BL_PLATFORM_LINUX)
    static BLEnum const SCANCODE_INTERNAL[] = {
        BL_KC_UNKNOWN,
        BL_KC_ESCAPE,
        BL_KC_1,
        BL_KC_2,
        BL_KC_3,
        BL_KC_4,
        BL_KC_5,
        BL_KC_6,
        BL_KC_7,
        BL_KC_8,
        BL_KC_9,
        BL_KC_0,
        BL_KC_MINUS,
        BL_KC_EQUALS,
        BL_KC_BACKSPACE,
        BL_KC_TAB,
        BL_KC_Q,
        BL_KC_W,
        BL_KC_E,
        BL_KC_R,
        BL_KC_T,
        BL_KC_Y,
        BL_KC_U,
        BL_KC_I,
        BL_KC_O,
        BL_KC_P,
        BL_KC_LEFTBRACKET,
        BL_KC_RIGHTBRACKET,
        BL_KC_RETURN,
        BL_KC_LCTRL,
        BL_KC_A,
        BL_KC_S,
        BL_KC_D,
        BL_KC_F,
        BL_KC_G,
        BL_KC_H,
        BL_KC_J,
        BL_KC_K,
        BL_KC_L,
        BL_KC_SEMICOLON,
        BL_KC_APOSTROPHE,
        BL_KC_GRAVE,
        BL_KC_LSHIFT,
        BL_KC_BACKSLASH,
        BL_KC_Z,
        BL_KC_X,
        BL_KC_C,
        BL_KC_V,
        BL_KC_B,
        BL_KC_N,
        BL_KC_M,
        BL_KC_COMMA,
        BL_KC_PERIOD,
        BL_KC_SLASH,
        BL_KC_RSHIFT,
        BL_KC_KP_MULTIPLY,
        BL_KC_LALT,
        BL_KC_SPACE,
        BL_KC_CAPSLOCK,
        BL_KC_F1,
        BL_KC_F2,
        BL_KC_F3,
        BL_KC_F4,
        BL_KC_F5,
        BL_KC_F6,
        BL_KC_F7,
        BL_KC_F8,
        BL_KC_F9,
        BL_KC_F10,
        BL_KC_NUMLOCKCLEAR,
        BL_KC_SCROLLLOCK,
        BL_KC_KP_7,
        BL_KC_KP_8,
        BL_KC_KP_9,
        BL_KC_KP_MINUS,
        BL_KC_KP_4,
        BL_KC_KP_5,
        BL_KC_KP_6,
        BL_KC_KP_PLUS,
        BL_KC_KP_1,
        BL_KC_KP_2,
        BL_KC_KP_3,
        BL_KC_KP_0,
        BL_KC_KP_PERIOD,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_NONUSBACKSLASH,
        BL_KC_F11,
        BL_KC_F12,
        BL_KC_INTERNATIONAL1,
        BL_KC_LANG3,
        BL_KC_LANG4,
        BL_KC_INTERNATIONAL4,
        BL_KC_INTERNATIONAL2,
        BL_KC_INTERNATIONAL5,
        BL_KC_INTERNATIONAL5,
        BL_KC_KP_ENTER,
        BL_KC_RCTRL,
        BL_KC_KP_DIVIDE,
        BL_KC_SYSREQ,
        BL_KC_RALT,
        BL_KC_UNKNOWN,
        BL_KC_HOME,
        BL_KC_UP,
        BL_KC_PAGEUP,
        BL_KC_LEFT,
        BL_KC_RIGHT,
        BL_KC_END,
        BL_KC_DOWN,
        BL_KC_PAGEDOWN,
        BL_KC_INSERT,
        BL_KC_DELETE,
        BL_KC_UNKNOWN,
        BL_KC_MUTE,
        BL_KC_VOLUMEDOWN,
        BL_KC_VOLUMEUP,
        BL_KC_POWER,
        BL_KC_KP_EQUALS,
        BL_KC_KP_PLUSMINUS,
        BL_KC_PAUSE,
        BL_KC_UNKNOWN,
        BL_KC_KP_COMMA,
        BL_KC_LANG1,
        BL_KC_LANG2,
        BL_KC_INTERNATIONAL3,
        BL_KC_LGUI,
        BL_KC_RGUI,
        BL_KC_UNKNOWN,
        BL_KC_STOP,
        BL_KC_AGAIN,
        BL_KC_UNKNOWN,
        BL_KC_UNDO,
        BL_KC_UNKNOWN,
        BL_KC_COPY,
        BL_KC_UNKNOWN,
        BL_KC_PASTE,
        BL_KC_FIND,
        BL_KC_CUT,
        BL_KC_HELP,
        BL_KC_MENU,
        BL_KC_CALCULATOR,
        BL_KC_UNKNOWN,
        BL_KC_SLEEP,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_MAIL,
        BL_KC_AC_BOOKMARKS,
        BL_KC_COMPUTER,
        BL_KC_AC_BACK,
        BL_KC_AC_FORWARD,
        BL_KC_UNKNOWN,
        BL_KC_EJECT,
        BL_KC_UNKNOWN,
        BL_KC_AUDIONEXT,
        BL_KC_AUDIOPLAY,
        BL_KC_AUDIOPREV,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_AC_HOME,
        BL_KC_AC_REFRESH,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_KP_LEFTPAREN,
        BL_KC_KP_RIGHTPAREN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_F13,
        BL_KC_F14,
        BL_KC_F15,
        BL_KC_F16,
        BL_KC_F17,
        BL_KC_F18,
        BL_KC_F19,
        BL_KC_F20,
        BL_KC_F21,
        BL_KC_F22,
        BL_KC_F23,
        BL_KC_F24,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_AC_SEARCH,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_ALTERASE,
        BL_KC_CANCEL,
        BL_KC_BRIGHTNESSDOWN,
        BL_KC_BRIGHTNESSUP,
        BL_KC_UNKNOWN,
        BL_KC_DISPLAYSWITCH,
        BL_KC_KBDILLUMTOGGLE,
        BL_KC_KBDILLUMDOWN,
        BL_KC_KBDILLUMUP,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN
    };
#elif defined(BL_PLATFORM_ANDROID)
    static BLEnum const SCANCODE_INTERNAL[] = {
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_AC_HOME,
        BL_KC_AC_BACK,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_0,
        BL_KC_1,
        BL_KC_2,
        BL_KC_3,
        BL_KC_4,
        BL_KC_5,
        BL_KC_6,
        BL_KC_7,
        BL_KC_8,
        BL_KC_9,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UP,
        BL_KC_DOWN,
        BL_KC_LEFT,
        BL_KC_RIGHT,
        BL_KC_SELECT,
        BL_KC_VOLUMEUP,
        BL_KC_VOLUMEDOWN,
        BL_KC_POWER,
        BL_KC_UNKNOWN,
        BL_KC_CLEAR,
        BL_KC_A,
        BL_KC_B,
        BL_KC_C,
        BL_KC_D,
        BL_KC_E,
        BL_KC_F,
        BL_KC_G,
        BL_KC_H,
        BL_KC_I,
        BL_KC_J,
        BL_KC_K,
        BL_KC_L,
        BL_KC_M,
        BL_KC_N,
        BL_KC_O,
        BL_KC_P,
        BL_KC_Q,
        BL_KC_R,
        BL_KC_S,
        BL_KC_T,
        BL_KC_U,
        BL_KC_V,
        BL_KC_W,
        BL_KC_X,
        BL_KC_Y,
        BL_KC_Z,
        BL_KC_COMMA,
        BL_KC_PERIOD,
        BL_KC_LALT,
        BL_KC_RALT,
        BL_KC_LSHIFT,
        BL_KC_RSHIFT,
        BL_KC_TAB,
        BL_KC_SPACE,
        BL_KC_UNKNOWN,
        BL_KC_WWW,
        BL_KC_MAIL,
        BL_KC_RETURN,
        BL_KC_BACKSPACE,
        BL_KC_GRAVE,
        BL_KC_MINUS,
        BL_KC_EQUALS,
        BL_KC_LEFTBRACKET,
        BL_KC_RIGHTBRACKET,
        BL_KC_BACKSLASH,
        BL_KC_SEMICOLON,
        BL_KC_APOSTROPHE,
        BL_KC_SLASH,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_MENU,
        BL_KC_UNKNOWN,
        BL_KC_AC_SEARCH,
        BL_KC_AUDIOPLAY,
        BL_KC_AUDIOSTOP,
        BL_KC_AUDIONEXT,
        BL_KC_AUDIOPREV,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_MUTE,
        BL_KC_PAGEUP,
        BL_KC_PAGEDOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_ESCAPE,
        BL_KC_DELETE,
        BL_KC_LCTRL,
        BL_KC_RCTRL,
        BL_KC_CAPSLOCK,
        BL_KC_SCROLLLOCK,
        BL_KC_LGUI,
        BL_KC_RGUI,
        BL_KC_UNKNOWN,
        BL_KC_PRINTSCREEN,
        BL_KC_PAUSE,
        BL_KC_HOME,
        BL_KC_END,
        BL_KC_INSERT,
        BL_KC_AC_FORWARD,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_EJECT,
        BL_KC_UNKNOWN,
        BL_KC_F1,
        BL_KC_F2,
        BL_KC_F3,
        BL_KC_F4,
        BL_KC_F5,
        BL_KC_F6,
        BL_KC_F7,
        BL_KC_F8,
        BL_KC_F9,
        BL_KC_F10,
        BL_KC_F11,
        BL_KC_F12,
        BL_KC_UNKNOWN,
        BL_KC_KP_0,
        BL_KC_KP_1,
        BL_KC_KP_2,
        BL_KC_KP_3,
        BL_KC_KP_4,
        BL_KC_KP_5,
        BL_KC_KP_6,
        BL_KC_KP_7,
        BL_KC_KP_8,
        BL_KC_KP_9,
        BL_KC_KP_DIVIDE,
        BL_KC_KP_MULTIPLY,
        BL_KC_KP_MINUS,
        BL_KC_KP_PLUS,
        BL_KC_KP_PERIOD,
        BL_KC_KP_COMMA,
        BL_KC_KP_ENTER,
        BL_KC_KP_EQUALS,
        BL_KC_KP_LEFTPAREN,
        BL_KC_KP_RIGHTPAREN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_AC_BOOKMARKS,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_CALCULATOR,
        BL_KC_LANG5,
        BL_KC_UNKNOWN,
        BL_KC_INTERNATIONAL5,
        BL_KC_INTERNATIONAL4,
        BL_KC_LANG3,
        BL_KC_INTERNATIONAL3,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_BRIGHTNESSDOWN,
        BL_KC_BRIGHTNESSUP,
        BL_KC_UNKNOWN,
        BL_KC_SLEEP,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_HELP,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
        BL_KC_UNKNOWN,
    };
    extern const BLAnsi* DEXCODE_INTERNAL;
#elif defined(BL_PLATFORM_OSX)
    static const BLEnum SCANCODE_INTERNAL[] =
    {
        BL_KC_A,
        BL_KC_S,
        BL_KC_D,
        BL_KC_F,
        BL_KC_H,
        BL_KC_G,
        BL_KC_Z,
        BL_KC_X,
        BL_KC_C,
        BL_KC_V,
        BL_KC_NONUSBACKSLASH,
        BL_KC_B,
        BL_KC_Q,
        BL_KC_W,
        BL_KC_E,
        BL_KC_R,
        BL_KC_Y,
        BL_KC_T,
        BL_KC_1,
        BL_KC_2,
        BL_KC_3,
        BL_KC_4,
        BL_KC_6,
        BL_KC_5,
        BL_KC_EQUALS,
        BL_KC_9,
        BL_KC_7,
        BL_KC_MINUS,
        BL_KC_8,
        BL_KC_0,
        BL_KC_RIGHTBRACKET,
        BL_KC_O,
        BL_KC_U,
        BL_KC_LEFTBRACKET,
        BL_KC_I,
        BL_KC_P,
        BL_KC_RETURN,
        BL_KC_L,
        BL_KC_J,
        BL_KC_APOSTROPHE,
        BL_KC_K,
        BL_KC_SEMICOLON,
        BL_KC_BACKSLASH,
        BL_KC_COMMA,
        BL_KC_SLASH,
        BL_KC_N,
        BL_KC_M,
        BL_KC_PERIOD,
        BL_KC_TAB,
        BL_KC_SPACE,
        BL_KC_GRAVE,
        BL_KC_BACKSPACE,
        BL_KC_KP_ENTER,
        BL_KC_ESCAPE,
        BL_KC_RGUI,
        BL_KC_LGUI,
        BL_KC_LSHIFT,
        BL_KC_CAPSLOCK,
        BL_KC_LALT,
        BL_KC_LCTRL,
        BL_KC_RSHIFT,
        BL_KC_RALT,
        BL_KC_RCTRL,
        BL_KC_RGUI,
        BL_KC_F17,
        BL_KC_KP_PERIOD,
        BL_KC_UNKNOWN,
        BL_KC_KP_MULTIPLY,
        BL_KC_UNKNOWN,
        BL_KC_KP_PLUS,
        BL_KC_UNKNOWN,
        BL_KC_NUMLOCKCLEAR,
        BL_KC_VOLUMEUP,
        BL_KC_VOLUMEDOWN,
        BL_KC_MUTE,
        BL_KC_KP_DIVIDE,
        BL_KC_KP_ENTER,
        BL_KC_UNKNOWN,
        BL_KC_KP_MINUS,
        BL_KC_F18,
        BL_KC_F19,
        BL_KC_KP_EQUALS,
        BL_KC_KP_0,
        BL_KC_KP_1,
        BL_KC_KP_2,
        BL_KC_KP_3,
        BL_KC_KP_4,
        BL_KC_KP_5,
        BL_KC_KP_6,
        BL_KC_KP_7,
        BL_KC_UNKNOWN,
        BL_KC_KP_8,
        BL_KC_KP_9,
        BL_KC_INTERNATIONAL3,
        BL_KC_INTERNATIONAL1,
        BL_KC_KP_COMMA,
        BL_KC_F5,
        BL_KC_F6,
        BL_KC_F7,
        BL_KC_F3,
        BL_KC_F8,
        BL_KC_F9,
        BL_KC_LANG2,
        BL_KC_F11,
        BL_KC_LANG1,
        BL_KC_PRINTSCREEN,
        BL_KC_F16,
        BL_KC_SCROLLLOCK,
        BL_KC_UNKNOWN,
        BL_KC_F10,
        BL_KC_APPLICATION,
        BL_KC_F12,
        BL_KC_UNKNOWN,
        BL_KC_PAUSE,
        BL_KC_INSERT,
        BL_KC_HOME,
        BL_KC_PAGEUP,
        BL_KC_DELETE,
        BL_KC_F4,
        BL_KC_END,
        BL_KC_F2,
        BL_KC_PAGEDOWN,
        BL_KC_F1,
        BL_KC_LEFT,
        BL_KC_RIGHT,
        BL_KC_DOWN,
        BL_KC_UP,
        BL_KC_POWER
    };
#endif
#if (defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_OSX))
#define GL_PROTO_INTERNAL(type, func) extern type func;
    GL_PROTO_INTERNAL(PFNGLCULLFACEPROC, glCullFace)
    GL_PROTO_INTERNAL(PFNGLFRONTFACEPROC, glFrontFace)
    GL_PROTO_INTERNAL(PFNGLHINTPROC, glHint)
    GL_PROTO_INTERNAL(PFNGLLINEWIDTHPROC, glLineWidth)
    GL_PROTO_INTERNAL(PFNGLPOINTSIZEPROC, glPointSize)
    GL_PROTO_INTERNAL(PFNGLPOLYGONMODEPROC, glPolygonMode)
    GL_PROTO_INTERNAL(PFNGLSCISSORPROC, glScissor)
    GL_PROTO_INTERNAL(PFNGLTEXPARAMETERFPROC, glTexParameterf)
    GL_PROTO_INTERNAL(PFNGLTEXPARAMETERFVPROC, glTexParameterfv)
    GL_PROTO_INTERNAL(PFNGLTEXPARAMETERIPROC, glTexParameteri)
    GL_PROTO_INTERNAL(PFNGLTEXPARAMETERIVPROC, glTexParameteriv)
    GL_PROTO_INTERNAL(PFNGLTEXIMAGE1DPROC, glTexImage1D)
    GL_PROTO_INTERNAL(PFNGLTEXIMAGE2DPROC, glTexImage2D)
    GL_PROTO_INTERNAL(PFNGLDRAWBUFFERPROC, glDrawBuffer)
    GL_PROTO_INTERNAL(PFNGLCLEARPROC, glClear)
    GL_PROTO_INTERNAL(PFNGLCLEARCOLORPROC, glClearColor)
    GL_PROTO_INTERNAL(PFNGLCLEARSTENCILPROC, glClearStencil)
    GL_PROTO_INTERNAL(PFNGLCLEARDEPTHPROC, glClearDepth)
    GL_PROTO_INTERNAL(PFNGLSTENCILMASKPROC, glStencilMask)
    GL_PROTO_INTERNAL(PFNGLCOLORMASKPROC, glColorMask)
    GL_PROTO_INTERNAL(PFNGLDEPTHMASKPROC, glDepthMask)
    GL_PROTO_INTERNAL(PFNGLDISABLEPROC, glDisable)
    GL_PROTO_INTERNAL(PFNGLENABLEPROC, glEnable)
    GL_PROTO_INTERNAL(PFNGLFINISHPROC, glFinish)
    GL_PROTO_INTERNAL(PFNGLFLUSHPROC, glFlush)
    GL_PROTO_INTERNAL(PFNGLBLENDFUNCPROC, glBlendFunc)
    GL_PROTO_INTERNAL(PFNGLLOGICOPPROC, glLogicOp)
    GL_PROTO_INTERNAL(PFNGLSTENCILFUNCPROC, glStencilFunc)
    GL_PROTO_INTERNAL(PFNGLSTENCILOPPROC, glStencilOp)
    GL_PROTO_INTERNAL(PFNGLDEPTHFUNCPROC, glDepthFunc)
    GL_PROTO_INTERNAL(PFNGLPIXELSTOREFPROC, glPixelStoref)
    GL_PROTO_INTERNAL(PFNGLPIXELSTOREIPROC, glPixelStorei)
    GL_PROTO_INTERNAL(PFNGLREADBUFFERPROC, glReadBuffer)
    GL_PROTO_INTERNAL(PFNGLREADPIXELSPROC, glReadPixels)
    GL_PROTO_INTERNAL(PFNGLGETBOOLEANVPROC, glGetBooleanv)
    GL_PROTO_INTERNAL(PFNGLGETDOUBLEVPROC, glGetDoublev)
    GL_PROTO_INTERNAL(PFNGLGETERRORPROC, glGetError)
    GL_PROTO_INTERNAL(PFNGLGETFLOATVPROC, glGetFloatv)
    GL_PROTO_INTERNAL(PFNGLGETINTEGERVPROC, glGetIntegerv)
    GL_PROTO_INTERNAL(PFNGLGETSTRINGPROC, glGetString)
    GL_PROTO_INTERNAL(PFNGLGETTEXIMAGEPROC, glGetTexImage)
    GL_PROTO_INTERNAL(PFNGLGETTEXPARAMETERFVPROC, glGetTexParameterfv)
    GL_PROTO_INTERNAL(PFNGLGETTEXPARAMETERIVPROC, glGetTexParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETTEXLEVELPARAMETERFVPROC, glGetTexLevelParameterfv)
    GL_PROTO_INTERNAL(PFNGLGETTEXLEVELPARAMETERIVPROC, glGetTexLevelParameteriv)
    GL_PROTO_INTERNAL(PFNGLISENABLEDPROC, glIsEnabled)
    GL_PROTO_INTERNAL(PFNGLDEPTHRANGEPROC, glDepthRange)
    GL_PROTO_INTERNAL(PFNGLVIEWPORTPROC, glViewport)
    GL_PROTO_INTERNAL(PFNGLDRAWARRAYSPROC, glDrawArrays)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSPROC, glDrawElements)
    GL_PROTO_INTERNAL(PFNGLGETPOINTERVPROC, glGetPointerv)
    GL_PROTO_INTERNAL(PFNGLPOLYGONOFFSETPROC, glPolygonOffset)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXIMAGE1DPROC, glCopyTexImage1D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXIMAGE2DPROC, glCopyTexImage2D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXSUBIMAGE1DPROC, glCopyTexSubImage1D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXSUBIMAGE2DPROC, glCopyTexSubImage2D)
    GL_PROTO_INTERNAL(PFNGLTEXSUBIMAGE1DPROC, glTexSubImage1D)
    GL_PROTO_INTERNAL(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D)
    GL_PROTO_INTERNAL(PFNGLBINDTEXTUREPROC, glBindTexture)
    GL_PROTO_INTERNAL(PFNGLDELETETEXTURESPROC, glDeleteTextures)
    GL_PROTO_INTERNAL(PFNGLGENTEXTURESPROC, glGenTextures)
    GL_PROTO_INTERNAL(PFNGLISTEXTUREPROC, glIsTexture)
    GL_PROTO_INTERNAL(PFNGLDRAWRANGEELEMENTSPROC, glDrawRangeElements)
    GL_PROTO_INTERNAL(PFNGLTEXIMAGE3DPROC, glTexImage3D)
    GL_PROTO_INTERNAL(PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXSUBIMAGE3DPROC, glCopyTexSubImage3D)
    GL_PROTO_INTERNAL(PFNGLACTIVETEXTUREPROC, glActiveTexture)
    GL_PROTO_INTERNAL(PFNGLSAMPLECOVERAGEPROC, glSampleCoverage)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXIMAGE3DPROC, glCompressedTexImage3D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXIMAGE1DPROC, glCompressedTexImage1D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC, glCompressedTexSubImage3D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC, glCompressedTexSubImage2D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC, glCompressedTexSubImage1D)
    GL_PROTO_INTERNAL(PFNGLGETCOMPRESSEDTEXIMAGEPROC, glGetCompressedTexImage)
    GL_PROTO_INTERNAL(PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate)
    GL_PROTO_INTERNAL(PFNGLMULTIDRAWARRAYSPROC, glMultiDrawArrays)
    GL_PROTO_INTERNAL(PFNGLMULTIDRAWELEMENTSPROC, glMultiDrawElements)
    GL_PROTO_INTERNAL(PFNGLPOINTPARAMETERFPROC, glPointParameterf)
    GL_PROTO_INTERNAL(PFNGLPOINTPARAMETERFVPROC, glPointParameterfv)
    GL_PROTO_INTERNAL(PFNGLPOINTPARAMETERIPROC, glPointParameteri)
    GL_PROTO_INTERNAL(PFNGLPOINTPARAMETERIVPROC, glPointParameteriv)
    GL_PROTO_INTERNAL(PFNGLBLENDCOLORPROC, glBlendColor)
    GL_PROTO_INTERNAL(PFNGLBLENDEQUATIONPROC, glBlendEquation)
    GL_PROTO_INTERNAL(PFNGLGENQUERIESPROC, glGenQueries)
    GL_PROTO_INTERNAL(PFNGLDELETEQUERIESPROC, glDeleteQueries)
    GL_PROTO_INTERNAL(PFNGLISQUERYPROC, glIsQuery)
    GL_PROTO_INTERNAL(PFNGLBEGINQUERYPROC, glBeginQuery)
    GL_PROTO_INTERNAL(PFNGLENDQUERYPROC, glEndQuery)
    GL_PROTO_INTERNAL(PFNGLGETQUERYIVPROC, glGetQueryiv)
    GL_PROTO_INTERNAL(PFNGLGETQUERYOBJECTIVPROC, glGetQueryObjectiv)
    GL_PROTO_INTERNAL(PFNGLGETQUERYOBJECTUIVPROC, glGetQueryObjectuiv)
    GL_PROTO_INTERNAL(PFNGLBINDBUFFERPROC, glBindBuffer)
    GL_PROTO_INTERNAL(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)
    GL_PROTO_INTERNAL(PFNGLGENBUFFERSPROC, glGenBuffers)
    GL_PROTO_INTERNAL(PFNGLISBUFFERPROC, glIsBuffer)
    GL_PROTO_INTERNAL(PFNGLBUFFERDATAPROC, glBufferData)
    GL_PROTO_INTERNAL(PFNGLBUFFERSUBDATAPROC, glBufferSubData)
    GL_PROTO_INTERNAL(PFNGLGETBUFFERSUBDATAPROC, glGetBufferSubData)
    GL_PROTO_INTERNAL(PFNGLMAPBUFFERPROC, glMapBuffer)
    GL_PROTO_INTERNAL(PFNGLUNMAPBUFFERPROC, glUnmapBuffer)
    GL_PROTO_INTERNAL(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETBUFFERPOINTERVPROC, glGetBufferPointerv)
    GL_PROTO_INTERNAL(PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate)
    GL_PROTO_INTERNAL(PFNGLDRAWBUFFERSPROC, glDrawBuffers)
    GL_PROTO_INTERNAL(PFNGLSTENCILOPSEPARATEPROC, glStencilOpSeparate)
    GL_PROTO_INTERNAL(PFNGLSTENCILFUNCSEPARATEPROC, glStencilFuncSeparate)
    GL_PROTO_INTERNAL(PFNGLSTENCILMASKSEPARATEPROC, glStencilMaskSeparate)
    GL_PROTO_INTERNAL(PFNGLATTACHSHADERPROC, glAttachShader)
    GL_PROTO_INTERNAL(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation)
    GL_PROTO_INTERNAL(PFNGLCOMPILESHADERPROC, glCompileShader)
    GL_PROTO_INTERNAL(PFNGLCREATEPROGRAMPROC, glCreateProgram)
    GL_PROTO_INTERNAL(PFNGLCREATESHADERPROC, glCreateShader)
    GL_PROTO_INTERNAL(PFNGLDELETEPROGRAMPROC, glDeleteProgram)
    GL_PROTO_INTERNAL(PFNGLDELETESHADERPROC, glDeleteShader)
    GL_PROTO_INTERNAL(PFNGLDETACHSHADERPROC, glDetachShader)
    GL_PROTO_INTERNAL(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray)
    GL_PROTO_INTERNAL(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform)
    GL_PROTO_INTERNAL(PFNGLGETATTACHEDSHADERSPROC, glGetAttachedShaders)
    GL_PROTO_INTERNAL(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMIVPROC, glGetProgramiv)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)
    GL_PROTO_INTERNAL(PFNGLGETSHADERIVPROC, glGetShaderiv)
    GL_PROTO_INTERNAL(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)
    GL_PROTO_INTERNAL(PFNGLGETSHADERSOURCEPROC, glGetShaderSource)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMFVPROC, glGetUniformfv)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMIVPROC, glGetUniformiv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBDVPROC, glGetVertexAttribdv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBFVPROC, glGetVertexAttribfv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv)
    GL_PROTO_INTERNAL(PFNGLISPROGRAMPROC, glIsProgram)
    GL_PROTO_INTERNAL(PFNGLISSHADERPROC, glIsShader)
    GL_PROTO_INTERNAL(PFNGLLINKPROGRAMPROC, glLinkProgram)
    GL_PROTO_INTERNAL(PFNGLSHADERSOURCEPROC, glShaderSource)
    GL_PROTO_INTERNAL(PFNGLUSEPROGRAMPROC, glUseProgram)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1FPROC, glUniform1f)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2FPROC, glUniform2f)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3FPROC, glUniform3f)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4FPROC, glUniform4f)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1IPROC, glUniform1i)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2IPROC, glUniform2i)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3IPROC, glUniform3i)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4IPROC, glUniform4i)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1FVPROC, glUniform1fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2FVPROC, glUniform2fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3FVPROC, glUniform3fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4FVPROC, glUniform4fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1IVPROC, glUniform1iv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2IVPROC, glUniform2iv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3IVPROC, glUniform3iv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4IVPROC, glUniform4iv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)
    GL_PROTO_INTERNAL(PFNGLVALIDATEPROGRAMPROC, glValidateProgram)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB1DPROC, glVertexAttrib1d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB1DVPROC, glVertexAttrib1dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB1FPROC, glVertexAttrib1f)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB1FVPROC, glVertexAttrib1fv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB1SPROC, glVertexAttrib1s)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB1SVPROC, glVertexAttrib1sv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB2DPROC, glVertexAttrib2d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB2DVPROC, glVertexAttrib2dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB2FPROC, glVertexAttrib2f)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB2FVPROC, glVertexAttrib2fv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB2SPROC, glVertexAttrib2s)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB2SVPROC, glVertexAttrib2sv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB3DPROC, glVertexAttrib3d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB3DVPROC, glVertexAttrib3dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB3FPROC, glVertexAttrib3f)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB3FVPROC, glVertexAttrib3fv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB3SPROC, glVertexAttrib3s)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB3SVPROC, glVertexAttrib3sv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NBVPROC, glVertexAttrib4Nbv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NIVPROC, glVertexAttrib4Niv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NSVPROC, glVertexAttrib4Nsv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NUBPROC, glVertexAttrib4Nub)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NUBVPROC, glVertexAttrib4Nubv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NUIVPROC, glVertexAttrib4Nuiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4NUSVPROC, glVertexAttrib4Nusv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4BVPROC, glVertexAttrib4bv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4DPROC, glVertexAttrib4d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4DVPROC, glVertexAttrib4dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4FPROC, glVertexAttrib4f)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4FVPROC, glVertexAttrib4fv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4IVPROC, glVertexAttrib4iv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4SPROC, glVertexAttrib4s)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4SVPROC, glVertexAttrib4sv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4UBVPROC, glVertexAttrib4ubv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4UIVPROC, glVertexAttrib4uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIB4USVPROC, glVertexAttrib4usv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX2X3FVPROC, glUniformMatrix2x3fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX3X2FVPROC, glUniformMatrix3x2fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX2X4FVPROC, glUniformMatrix2x4fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX4X2FVPROC, glUniformMatrix4x2fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX3X4FVPROC, glUniformMatrix3x4fv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX4X3FVPROC, glUniformMatrix4x3fv)
    GL_PROTO_INTERNAL(PFNGLCOLORMASKIPROC, glColorMaski)
    GL_PROTO_INTERNAL(PFNGLGETBOOLEANI_VPROC, glGetBooleani_v)
    GL_PROTO_INTERNAL(PFNGLGETINTEGERI_VPROC, glGetIntegeri_v)
    GL_PROTO_INTERNAL(PFNGLENABLEIPROC, glEnablei)
    GL_PROTO_INTERNAL(PFNGLDISABLEIPROC, glDisablei)
    GL_PROTO_INTERNAL(PFNGLISENABLEDIPROC, glIsEnabledi)
    GL_PROTO_INTERNAL(PFNGLBEGINTRANSFORMFEEDBACKPROC, glBeginTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLENDTRANSFORMFEEDBACKPROC, glEndTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLBINDBUFFERRANGEPROC, glBindBufferRange)
    GL_PROTO_INTERNAL(PFNGLBINDBUFFERBASEPROC, glBindBufferBase)
    GL_PROTO_INTERNAL(PFNGLTRANSFORMFEEDBACKVARYINGSPROC, glTransformFeedbackVaryings)
    GL_PROTO_INTERNAL(PFNGLGETTRANSFORMFEEDBACKVARYINGPROC, glGetTransformFeedbackVarying)
    GL_PROTO_INTERNAL(PFNGLCLAMPCOLORPROC, glClampColor)
    GL_PROTO_INTERNAL(PFNGLBEGINCONDITIONALRENDERPROC, glBeginConditionalRender)
    GL_PROTO_INTERNAL(PFNGLENDCONDITIONALRENDERPROC, glEndConditionalRender)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBIIVPROC, glGetVertexAttribIiv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBIUIVPROC, glGetVertexAttribIuiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI1IPROC, glVertexAttribI1i)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI2IPROC, glVertexAttribI2i)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI3IPROC, glVertexAttribI3i)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4IPROC, glVertexAttribI4i)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI1UIPROC, glVertexAttribI1ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI2UIPROC, glVertexAttribI2ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI3UIPROC, glVertexAttribI3ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4UIPROC, glVertexAttribI4ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI1IVPROC, glVertexAttribI1iv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI2IVPROC, glVertexAttribI2iv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI3IVPROC, glVertexAttribI3iv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4IVPROC, glVertexAttribI4iv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI1UIVPROC, glVertexAttribI1uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI2UIVPROC, glVertexAttribI2uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI3UIVPROC, glVertexAttribI3uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4UIVPROC, glVertexAttribI4uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4BVPROC, glVertexAttribI4bv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4SVPROC, glVertexAttribI4sv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4UBVPROC, glVertexAttribI4ubv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBI4USVPROC, glVertexAttribI4usv)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMUIVPROC, glGetUniformuiv)
    GL_PROTO_INTERNAL(PFNGLBINDFRAGDATALOCATIONPROC, glBindFragDataLocation)
    GL_PROTO_INTERNAL(PFNGLGETFRAGDATALOCATIONPROC, glGetFragDataLocation)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1UIPROC, glUniform1ui)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2UIPROC, glUniform2ui)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3UIPROC, glUniform3ui)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4UIPROC, glUniform4ui)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1UIVPROC, glUniform1uiv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2UIVPROC, glUniform2uiv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3UIVPROC, glUniform3uiv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4UIVPROC, glUniform4uiv)
    GL_PROTO_INTERNAL(PFNGLTEXPARAMETERIIVPROC, glTexParameterIiv)
    GL_PROTO_INTERNAL(PFNGLTEXPARAMETERIUIVPROC, glTexParameterIuiv)
    GL_PROTO_INTERNAL(PFNGLGETTEXPARAMETERIIVPROC, glGetTexParameterIiv)
    GL_PROTO_INTERNAL(PFNGLGETTEXPARAMETERIUIVPROC, glGetTexParameterIuiv)
    GL_PROTO_INTERNAL(PFNGLCLEARBUFFERIVPROC, glClearBufferiv)
    GL_PROTO_INTERNAL(PFNGLCLEARBUFFERUIVPROC, glClearBufferuiv)
    GL_PROTO_INTERNAL(PFNGLCLEARBUFFERFVPROC, glClearBufferfv)
    GL_PROTO_INTERNAL(PFNGLCLEARBUFFERFIPROC, glClearBufferfi)
    GL_PROTO_INTERNAL(PFNGLGETSTRINGIPROC, glGetStringi)
    GL_PROTO_INTERNAL(PFNGLISRENDERBUFFERPROC, glIsRenderbuffer)
    GL_PROTO_INTERNAL(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer)
    GL_PROTO_INTERNAL(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers)
    GL_PROTO_INTERNAL(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers)
    GL_PROTO_INTERNAL(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage)
    GL_PROTO_INTERNAL(PFNGLGETRENDERBUFFERPARAMETERIVPROC, glGetRenderbufferParameteriv)
    GL_PROTO_INTERNAL(PFNGLISFRAMEBUFFERPROC, glIsFramebuffer)
    GL_PROTO_INTERNAL(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer)
    GL_PROTO_INTERNAL(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers)
    GL_PROTO_INTERNAL(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers)
    GL_PROTO_INTERNAL(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERTEXTURE1DPROC, glFramebufferTexture1D)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERTEXTURE3DPROC, glFramebufferTexture3D)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer)
    GL_PROTO_INTERNAL(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetFramebufferAttachmentParameteriv)
    GL_PROTO_INTERNAL(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap)
    GL_PROTO_INTERNAL(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer)
    GL_PROTO_INTERNAL(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC, glRenderbufferStorageMultisample)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERTEXTURELAYERPROC, glFramebufferTextureLayer)
    GL_PROTO_INTERNAL(PFNGLMAPBUFFERRANGEPROC, glMapBufferRange)
    GL_PROTO_INTERNAL(PFNGLFLUSHMAPPEDBUFFERRANGEPROC, glFlushMappedBufferRange)
    GL_PROTO_INTERNAL(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)
    GL_PROTO_INTERNAL(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)
    GL_PROTO_INTERNAL(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)
    GL_PROTO_INTERNAL(PFNGLISVERTEXARRAYPROC, glIsVertexArray)
    GL_PROTO_INTERNAL(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced)
    GL_PROTO_INTERNAL(PFNGLTEXBUFFERPROC, glTexBuffer)
    GL_PROTO_INTERNAL(PFNGLPRIMITIVERESTARTINDEXPROC, glPrimitiveRestartIndex)
    GL_PROTO_INTERNAL(PFNGLCOPYBUFFERSUBDATAPROC, glCopyBufferSubData)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMINDICESPROC, glGetUniformIndices)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEUNIFORMSIVPROC, glGetActiveUniformsiv)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEUNIFORMNAMEPROC, glGetActiveUniformName)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEUNIFORMBLOCKIVPROC, glGetActiveUniformBlockiv)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC, glGetActiveUniformBlockName)
    GL_PROTO_INTERNAL(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSBASEVERTEXPROC, glDrawElementsBaseVertex)
    GL_PROTO_INTERNAL(PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC, glDrawRangeElementsBaseVertex)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC, glDrawElementsInstancedBaseVertex)
    GL_PROTO_INTERNAL(PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC, glMultiDrawElementsBaseVertex)
    GL_PROTO_INTERNAL(PFNGLPROVOKINGVERTEXPROC, glProvokingVertex)
    GL_PROTO_INTERNAL(PFNGLFENCESYNCPROC, glFenceSync)
    GL_PROTO_INTERNAL(PFNGLISSYNCPROC, glIsSync)
    GL_PROTO_INTERNAL(PFNGLDELETESYNCPROC, glDeleteSync)
    GL_PROTO_INTERNAL(PFNGLCLIENTWAITSYNCPROC, glClientWaitSync)
    GL_PROTO_INTERNAL(PFNGLWAITSYNCPROC, glWaitSync)
    GL_PROTO_INTERNAL(PFNGLGETINTEGER64VPROC, glGetInteger64v)
    GL_PROTO_INTERNAL(PFNGLGETSYNCIVPROC, glGetSynciv)
    GL_PROTO_INTERNAL(PFNGLGETINTEGER64I_VPROC, glGetInteger64i_v)
    GL_PROTO_INTERNAL(PFNGLGETBUFFERPARAMETERI64VPROC, glGetBufferParameteri64v)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERTEXTUREPROC, glFramebufferTexture)
    GL_PROTO_INTERNAL(PFNGLTEXIMAGE2DMULTISAMPLEPROC, glTexImage2DMultisample)
    GL_PROTO_INTERNAL(PFNGLTEXIMAGE3DMULTISAMPLEPROC, glTexImage3DMultisample)
    GL_PROTO_INTERNAL(PFNGLGETMULTISAMPLEFVPROC, glGetMultisamplefv)
    GL_PROTO_INTERNAL(PFNGLSAMPLEMASKIPROC, glSampleMaski)
    GL_PROTO_INTERNAL(PFNGLBINDFRAGDATALOCATIONINDEXEDPROC, glBindFragDataLocationIndexed)
    GL_PROTO_INTERNAL(PFNGLGETFRAGDATAINDEXPROC, glGetFragDataIndex)
    GL_PROTO_INTERNAL(PFNGLGENSAMPLERSPROC, glGenSamplers)
    GL_PROTO_INTERNAL(PFNGLDELETESAMPLERSPROC, glDeleteSamplers)
    GL_PROTO_INTERNAL(PFNGLISSAMPLERPROC, glIsSampler)
    GL_PROTO_INTERNAL(PFNGLBINDSAMPLERPROC, glBindSampler)
    GL_PROTO_INTERNAL(PFNGLSAMPLERPARAMETERIPROC, glSamplerParameteri)
    GL_PROTO_INTERNAL(PFNGLSAMPLERPARAMETERIVPROC, glSamplerParameteriv)
    GL_PROTO_INTERNAL(PFNGLSAMPLERPARAMETERFPROC, glSamplerParameterf)
    GL_PROTO_INTERNAL(PFNGLSAMPLERPARAMETERFVPROC, glSamplerParameterfv)
    GL_PROTO_INTERNAL(PFNGLSAMPLERPARAMETERIIVPROC, glSamplerParameterIiv)
    GL_PROTO_INTERNAL(PFNGLSAMPLERPARAMETERIUIVPROC, glSamplerParameterIuiv)
    GL_PROTO_INTERNAL(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETSAMPLERPARAMETERIIVPROC, glGetSamplerParameterIiv)
    GL_PROTO_INTERNAL(PFNGLGETSAMPLERPARAMETERFVPROC, glGetSamplerParameterfv)
    GL_PROTO_INTERNAL(PFNGLGETSAMPLERPARAMETERIUIVPROC, glGetSamplerParameterIuiv)
    GL_PROTO_INTERNAL(PFNGLQUERYCOUNTERPROC, glQueryCounter)
    GL_PROTO_INTERNAL(PFNGLGETQUERYOBJECTI64VPROC, glGetQueryObjecti64v)
    GL_PROTO_INTERNAL(PFNGLGETQUERYOBJECTUI64VPROC, glGetQueryObjectui64v)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP1UIPROC, glVertexAttribP1ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP1UIVPROC, glVertexAttribP1uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP2UIPROC, glVertexAttribP2ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP2UIVPROC, glVertexAttribP2uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP3UIPROC, glVertexAttribP3ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP3UIVPROC, glVertexAttribP3uiv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP4UIPROC, glVertexAttribP4ui)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBP4UIVPROC, glVertexAttribP4uiv)
    //GL 4.0
    GL_PROTO_INTERNAL(PFNGLMINSAMPLESHADINGPROC, glMinSampleShading)
    GL_PROTO_INTERNAL(PFNGLBLENDEQUATIONIPROC, glBlendEquationi)
    GL_PROTO_INTERNAL(PFNGLBLENDEQUATIONSEPARATEIPROC, glBlendEquationSeparatei)
    GL_PROTO_INTERNAL(PFNGLBLENDFUNCIPROC, glBlendFunci)
    GL_PROTO_INTERNAL(PFNGLBLENDFUNCSEPARATEIPROC, glBlendFuncSeparatei)
    GL_PROTO_INTERNAL(PFNGLDRAWARRAYSINDIRECTPROC, glDrawArraysIndirect)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSINDIRECTPROC, glDrawElementsIndirect)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1DPROC, glUniform1d)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2DPROC, glUniform2d)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3DPROC, glUniform3d)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4DPROC, glUniform4d)
    GL_PROTO_INTERNAL(PFNGLUNIFORM1DVPROC, glUniform1dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM2DVPROC, glUniform2dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM3DVPROC, glUniform3dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORM4DVPROC, glUniform4dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX2DVPROC, glUniformMatrix2dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX3DVPROC, glUniformMatrix3dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX4DVPROC, glUniformMatrix4dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX2X3DVPROC, glUniformMatrix2x3dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX2X4DVPROC, glUniformMatrix2x4dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX3X2DVPROC, glUniformMatrix3x2dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX3X4DVPROC, glUniformMatrix3x4dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX4X2DVPROC, glUniformMatrix4x2dv)
    GL_PROTO_INTERNAL(PFNGLUNIFORMMATRIX4X3DVPROC, glUniformMatrix4x3dv)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMDVPROC, glGetUniformdv)
    GL_PROTO_INTERNAL(PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC, glGetSubroutineUniformLocation)
    GL_PROTO_INTERNAL(PFNGLGETSUBROUTINEINDEXPROC, glGetSubroutineIndex)
    GL_PROTO_INTERNAL(PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC, glGetActiveSubroutineUniformiv)
    GL_PROTO_INTERNAL(PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC, glGetActiveSubroutineUniformName)
    GL_PROTO_INTERNAL(PFNGLGETACTIVESUBROUTINENAMEPROC, glGetActiveSubroutineName)
    GL_PROTO_INTERNAL(PFNGLUNIFORMSUBROUTINESUIVPROC, glUniformSubroutinesuiv)
    GL_PROTO_INTERNAL(PFNGLGETUNIFORMSUBROUTINEUIVPROC, glGetUniformSubroutineuiv)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMSTAGEIVPROC, glGetProgramStageiv)
    GL_PROTO_INTERNAL(PFNGLPATCHPARAMETERIPROC, glPatchParameteri)
    GL_PROTO_INTERNAL(PFNGLPATCHPARAMETERFVPROC, glPatchParameterfv)
    GL_PROTO_INTERNAL(PFNGLBINDTRANSFORMFEEDBACKPROC, glBindTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLDELETETRANSFORMFEEDBACKSPROC, glDeleteTransformFeedbacks)
    GL_PROTO_INTERNAL(PFNGLGENTRANSFORMFEEDBACKSPROC, glGenTransformFeedbacks)
    GL_PROTO_INTERNAL(PFNGLISTRANSFORMFEEDBACKPROC, glIsTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLPAUSETRANSFORMFEEDBACKPROC, glPauseTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLRESUMETRANSFORMFEEDBACKPROC, glResumeTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKPROC, glDrawTransformFeedback)
    GL_PROTO_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC, glDrawTransformFeedbackStream)
    GL_PROTO_INTERNAL(PFNGLBEGINQUERYINDEXEDPROC, glBeginQueryIndexed)
    GL_PROTO_INTERNAL(PFNGLENDQUERYINDEXEDPROC, glEndQueryIndexed)
    GL_PROTO_INTERNAL(PFNGLGETQUERYINDEXEDIVPROC, glGetQueryIndexediv)
    //GL 4.1
    GL_PROTO_INTERNAL(PFNGLRELEASESHADERCOMPILERPROC, glReleaseShaderCompiler)
    GL_PROTO_INTERNAL(PFNGLSHADERBINARYPROC, glShaderBinary)
    GL_PROTO_INTERNAL(PFNGLGETSHADERPRECISIONFORMATPROC, glGetShaderPrecisionFormat)
    GL_PROTO_INTERNAL(PFNGLDEPTHRANGEFPROC, glDepthRangef)
    GL_PROTO_INTERNAL(PFNGLCLEARDEPTHFPROC, glClearDepthf)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMBINARYPROC, glGetProgramBinary)
    GL_PROTO_INTERNAL(PFNGLPROGRAMBINARYPROC, glProgramBinary)
    GL_PROTO_INTERNAL(PFNGLPROGRAMPARAMETERIPROC, glProgramParameteri)
    GL_PROTO_INTERNAL(PFNGLUSEPROGRAMSTAGESPROC, glUseProgramStages)
    GL_PROTO_INTERNAL(PFNGLACTIVESHADERPROGRAMPROC, glActiveShaderProgram)
    GL_PROTO_INTERNAL(PFNGLCREATESHADERPROGRAMVPROC, glCreateShaderProgramv)
    GL_PROTO_INTERNAL(PFNGLBINDPROGRAMPIPELINEPROC, glBindProgramPipeline)
    GL_PROTO_INTERNAL(PFNGLDELETEPROGRAMPIPELINESPROC, glDeleteProgramPipelines)
    GL_PROTO_INTERNAL(PFNGLGENPROGRAMPIPELINESPROC, glGenProgramPipelines)
    GL_PROTO_INTERNAL(PFNGLISPROGRAMPIPELINEPROC, glIsProgramPipeline)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMPIPELINEIVPROC, glGetProgramPipelineiv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1IPROC, glProgramUniform1i)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1IVPROC, glProgramUniform1iv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1FPROC, glProgramUniform1f)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1FVPROC, glProgramUniform1fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1DPROC, glProgramUniform1d)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1DVPROC, glProgramUniform1dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1UIPROC, glProgramUniform1ui)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM1UIVPROC, glProgramUniform1uiv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2IPROC, glProgramUniform2i)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2IVPROC, glProgramUniform2iv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2FPROC, glProgramUniform2f)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2FVPROC, glProgramUniform2fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2DPROC, glProgramUniform2d)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2DVPROC, glProgramUniform2dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2UIPROC, glProgramUniform2ui)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM2UIVPROC, glProgramUniform2uiv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3IPROC, glProgramUniform3i)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3IVPROC, glProgramUniform3iv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3FPROC, glProgramUniform3f)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3FVPROC, glProgramUniform3fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3DPROC, glProgramUniform3d)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3DVPROC, glProgramUniform3dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3UIPROC, glProgramUniform3ui)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM3UIVPROC, glProgramUniform3uiv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4IPROC, glProgramUniform4i)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4IVPROC, glProgramUniform4iv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4FPROC, glProgramUniform4f)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4FVPROC, glProgramUniform4fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4DPROC, glProgramUniform4d)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4DVPROC, glProgramUniform4dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4UIPROC, glProgramUniform4ui)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORM4UIVPROC, glProgramUniform4uiv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2FVPROC, glProgramUniformMatrix2fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3FVPROC, glProgramUniformMatrix3fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4FVPROC, glProgramUniformMatrix4fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2DVPROC, glProgramUniformMatrix2dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3DVPROC, glProgramUniformMatrix3dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4DVPROC, glProgramUniformMatrix4dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC, glProgramUniformMatrix2x3fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC, glProgramUniformMatrix3x2fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC, glProgramUniformMatrix2x4fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC, glProgramUniformMatrix4x2fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC, glProgramUniformMatrix3x4fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC, glProgramUniformMatrix4x3fv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC, glProgramUniformMatrix2x3dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC, glProgramUniformMatrix3x2dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC, glProgramUniformMatrix2x4dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC, glProgramUniformMatrix4x2dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC, glProgramUniformMatrix3x4dv)
    GL_PROTO_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC, glProgramUniformMatrix4x3dv)
    GL_PROTO_INTERNAL(PFNGLVALIDATEPROGRAMPIPELINEPROC, glValidateProgramPipeline)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMPIPELINEINFOLOGPROC, glGetProgramPipelineInfoLog)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL1DPROC, glVertexAttribL1d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL2DPROC, glVertexAttribL2d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL3DPROC, glVertexAttribL3d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL4DPROC, glVertexAttribL4d)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL1DVPROC, glVertexAttribL1dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL2DVPROC, glVertexAttribL2dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL3DVPROC, glVertexAttribL3dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBL4DVPROC, glVertexAttribL4dv)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBLPOINTERPROC, glVertexAttribLPointer)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXATTRIBLDVPROC, glGetVertexAttribLdv)
    GL_PROTO_INTERNAL(PFNGLVIEWPORTARRAYVPROC, glViewportArrayv)
    GL_PROTO_INTERNAL(PFNGLVIEWPORTINDEXEDFPROC, glViewportIndexedf)
    GL_PROTO_INTERNAL(PFNGLVIEWPORTINDEXEDFVPROC, glViewportIndexedfv)
    GL_PROTO_INTERNAL(PFNGLSCISSORARRAYVPROC, glScissorArrayv)
    GL_PROTO_INTERNAL(PFNGLSCISSORINDEXEDPROC, glScissorIndexed)
    GL_PROTO_INTERNAL(PFNGLSCISSORINDEXEDVPROC, glScissorIndexedv)
    GL_PROTO_INTERNAL(PFNGLDEPTHRANGEARRAYVPROC, glDepthRangeArrayv)
    GL_PROTO_INTERNAL(PFNGLDEPTHRANGEINDEXEDPROC, glDepthRangeIndexed)
    GL_PROTO_INTERNAL(PFNGLGETFLOATI_VPROC, glGetFloati_v)
    GL_PROTO_INTERNAL(PFNGLGETDOUBLEI_VPROC, glGetDoublei_v)
    //GL 4.2
    GL_PROTO_INTERNAL(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC, glDrawArraysInstancedBaseInstance)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC, glDrawElementsInstancedBaseInstance)
    GL_PROTO_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC, glDrawElementsInstancedBaseVertexBaseInstance)
    GL_PROTO_INTERNAL(PFNGLGETINTERNALFORMATIVPROC, glGetInternalformativ)
    GL_PROTO_INTERNAL(PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC, glGetActiveAtomicCounterBufferiv)
    GL_PROTO_INTERNAL(PFNGLBINDIMAGETEXTUREPROC, glBindImageTexture)
    GL_PROTO_INTERNAL(PFNGLMEMORYBARRIERPROC, glMemoryBarrier)
    GL_PROTO_INTERNAL(PFNGLTEXSTORAGE1DPROC, glTexStorage1D)
    GL_PROTO_INTERNAL(PFNGLTEXSTORAGE2DPROC, glTexStorage2D)
    GL_PROTO_INTERNAL(PFNGLTEXSTORAGE3DPROC, glTexStorage3D)
    GL_PROTO_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC, glDrawTransformFeedbackInstanced)
    GL_PROTO_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC, glDrawTransformFeedbackStreamInstanced)
    //GL 4.3
    GL_PROTO_INTERNAL(PFNGLCLEARBUFFERDATAPROC, glClearBufferData)
    GL_PROTO_INTERNAL(PFNGLCLEARBUFFERSUBDATAPROC, glClearBufferSubData)
    GL_PROTO_INTERNAL(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute)
    GL_PROTO_INTERNAL(PFNGLDISPATCHCOMPUTEINDIRECTPROC, glDispatchComputeIndirect)
    GL_PROTO_INTERNAL(PFNGLCOPYIMAGESUBDATAPROC, glCopyImageSubData)
    GL_PROTO_INTERNAL(PFNGLFRAMEBUFFERPARAMETERIPROC, glFramebufferParameteri)
    GL_PROTO_INTERNAL(PFNGLGETFRAMEBUFFERPARAMETERIVPROC, glGetFramebufferParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETINTERNALFORMATI64VPROC, glGetInternalformati64v)
    GL_PROTO_INTERNAL(PFNGLINVALIDATETEXSUBIMAGEPROC, glInvalidateTexSubImage)
    GL_PROTO_INTERNAL(PFNGLINVALIDATETEXIMAGEPROC, glInvalidateTexImage)
    GL_PROTO_INTERNAL(PFNGLINVALIDATEBUFFERSUBDATAPROC, glInvalidateBufferSubData)
    GL_PROTO_INTERNAL(PFNGLINVALIDATEBUFFERDATAPROC, glInvalidateBufferData)
    GL_PROTO_INTERNAL(PFNGLINVALIDATEFRAMEBUFFERPROC, glInvalidateFramebuffer)
    GL_PROTO_INTERNAL(PFNGLINVALIDATESUBFRAMEBUFFERPROC, glInvalidateSubFramebuffer)
    GL_PROTO_INTERNAL(PFNGLMULTIDRAWARRAYSINDIRECTPROC, glMultiDrawArraysIndirect)
    GL_PROTO_INTERNAL(PFNGLMULTIDRAWELEMENTSINDIRECTPROC, glMultiDrawElementsIndirect)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMINTERFACEIVPROC, glGetProgramInterfaceiv)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMRESOURCEINDEXPROC, glGetProgramResourceIndex)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMRESOURCENAMEPROC, glGetProgramResourceName)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMRESOURCEIVPROC, glGetProgramResourceiv)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMRESOURCELOCATIONPROC, glGetProgramResourceLocation)
    GL_PROTO_INTERNAL(PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC, glGetProgramResourceLocationIndex)
    GL_PROTO_INTERNAL(PFNGLSHADERSTORAGEBLOCKBINDINGPROC, glShaderStorageBlockBinding)
    GL_PROTO_INTERNAL(PFNGLTEXBUFFERRANGEPROC, glTexBufferRange)
    GL_PROTO_INTERNAL(PFNGLTEXSTORAGE2DMULTISAMPLEPROC, glTexStorage2DMultisample)
    GL_PROTO_INTERNAL(PFNGLTEXSTORAGE3DMULTISAMPLEPROC, glTexStorage3DMultisample)
    GL_PROTO_INTERNAL(PFNGLTEXTUREVIEWPROC, glTextureView)
    GL_PROTO_INTERNAL(PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBIFORMATPROC, glVertexAttribIFormat)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBLFORMATPROC, glVertexAttribLFormat)
    GL_PROTO_INTERNAL(PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding)
    GL_PROTO_INTERNAL(PFNGLVERTEXBINDINGDIVISORPROC, glVertexBindingDivisor)
    GL_PROTO_INTERNAL(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl)
    GL_PROTO_INTERNAL(PFNGLDEBUGMESSAGEINSERTPROC, glDebugMessageInsert)
    GL_PROTO_INTERNAL(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback)
    GL_PROTO_INTERNAL(PFNGLGETDEBUGMESSAGELOGPROC, glGetDebugMessageLog)
    GL_PROTO_INTERNAL(PFNGLPUSHDEBUGGROUPPROC, glPushDebugGroup)
    GL_PROTO_INTERNAL(PFNGLPOPDEBUGGROUPPROC, glPopDebugGroup)
    GL_PROTO_INTERNAL(PFNGLOBJECTLABELPROC, glObjectLabel)
    GL_PROTO_INTERNAL(PFNGLGETOBJECTLABELPROC, glGetObjectLabel)
    GL_PROTO_INTERNAL(PFNGLOBJECTPTRLABELPROC, glObjectPtrLabel)
    GL_PROTO_INTERNAL(PFNGLGETOBJECTPTRLABELPROC, glGetObjectPtrLabel)
    //GL 4.4
    GL_PROTO_INTERNAL(PFNGLBUFFERSTORAGEPROC, glBufferStorage)
    GL_PROTO_INTERNAL(PFNGLCLEARTEXIMAGEPROC, glClearTexImage)
    GL_PROTO_INTERNAL(PFNGLCLEARTEXSUBIMAGEPROC, glClearTexSubImage)
    GL_PROTO_INTERNAL(PFNGLBINDBUFFERSBASEPROC, glBindBuffersBase)
    GL_PROTO_INTERNAL(PFNGLBINDBUFFERSRANGEPROC, glBindBuffersRange)
    GL_PROTO_INTERNAL(PFNGLBINDTEXTURESPROC, glBindTextures)
    GL_PROTO_INTERNAL(PFNGLBINDSAMPLERSPROC, glBindSamplers)
    GL_PROTO_INTERNAL(PFNGLBINDIMAGETEXTURESPROC, glBindImageTextures)
    GL_PROTO_INTERNAL(PFNGLBINDVERTEXBUFFERSPROC, glBindVertexBuffers)
    //GL 4.5
    GL_PROTO_INTERNAL(PFNGLCLIPCONTROLPROC, glClipControl)
    GL_PROTO_INTERNAL(PFNGLCREATETRANSFORMFEEDBACKSPROC, glCreateTransformFeedbacks)
    GL_PROTO_INTERNAL(PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC, glTransformFeedbackBufferBase)
    GL_PROTO_INTERNAL(PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC, glTransformFeedbackBufferRange)
    GL_PROTO_INTERNAL(PFNGLGETTRANSFORMFEEDBACKIVPROC, glGetTransformFeedbackiv)
    GL_PROTO_INTERNAL(PFNGLGETTRANSFORMFEEDBACKI_VPROC, glGetTransformFeedbacki_v)
    GL_PROTO_INTERNAL(PFNGLGETTRANSFORMFEEDBACKI64_VPROC, glGetTransformFeedbacki64_v)
    GL_PROTO_INTERNAL(PFNGLCREATEBUFFERSPROC, glCreateBuffers)
    GL_PROTO_INTERNAL(PFNGLNAMEDBUFFERSTORAGEPROC, glNamedBufferStorage)
    GL_PROTO_INTERNAL(PFNGLNAMEDBUFFERDATAPROC, glNamedBufferData)
    GL_PROTO_INTERNAL(PFNGLNAMEDBUFFERSUBDATAPROC, glNamedBufferSubData)
    GL_PROTO_INTERNAL(PFNGLCOPYNAMEDBUFFERSUBDATAPROC, glCopyNamedBufferSubData)
    GL_PROTO_INTERNAL(PFNGLCLEARNAMEDBUFFERDATAPROC, glClearNamedBufferData)
    GL_PROTO_INTERNAL(PFNGLCLEARNAMEDBUFFERSUBDATAPROC, glClearNamedBufferSubData)
    GL_PROTO_INTERNAL(PFNGLMAPNAMEDBUFFERPROC, glMapNamedBuffer)
    GL_PROTO_INTERNAL(PFNGLMAPNAMEDBUFFERRANGEPROC, glMapNamedBufferRange)
    GL_PROTO_INTERNAL(PFNGLUNMAPNAMEDBUFFERPROC, glUnmapNamedBuffer)
    GL_PROTO_INTERNAL(PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC, glFlushMappedNamedBufferRange)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDBUFFERPARAMETERIVPROC, glGetNamedBufferParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDBUFFERPARAMETERI64VPROC, glGetNamedBufferParameteri64v)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDBUFFERPOINTERVPROC, glGetNamedBufferPointerv)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDBUFFERSUBDATAPROC, glGetNamedBufferSubData)
    GL_PROTO_INTERNAL(PFNGLCREATEFRAMEBUFFERSPROC, glCreateFramebuffers)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC, glNamedFramebufferRenderbuffer)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC, glNamedFramebufferParameteri)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC, glNamedFramebufferTexture)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC, glNamedFramebufferTextureLayer)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC, glNamedFramebufferDrawBuffer)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC, glNamedFramebufferDrawBuffers)
    GL_PROTO_INTERNAL(PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC, glNamedFramebufferReadBuffer)
    GL_PROTO_INTERNAL(PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC, glInvalidateNamedFramebufferData)
    GL_PROTO_INTERNAL(PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC, glInvalidateNamedFramebufferSubData)
    GL_PROTO_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERIVPROC, glClearNamedFramebufferiv)
    GL_PROTO_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC, glClearNamedFramebufferuiv)
    GL_PROTO_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERFVPROC, glClearNamedFramebufferfv)
    GL_PROTO_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERFIPROC, glClearNamedFramebufferfi)
    GL_PROTO_INTERNAL(PFNGLBLITNAMEDFRAMEBUFFERPROC, glBlitNamedFramebuffer)
    GL_PROTO_INTERNAL(PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC, glCheckNamedFramebufferStatus)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC, glGetNamedFramebufferParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetNamedFramebufferAttachmentParameteriv)
    GL_PROTO_INTERNAL(PFNGLCREATERENDERBUFFERSPROC, glCreateRenderbuffers)
    GL_PROTO_INTERNAL(PFNGLNAMEDRENDERBUFFERSTORAGEPROC, glNamedRenderbufferStorage)
    GL_PROTO_INTERNAL(PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC, glNamedRenderbufferStorageMultisample)
    GL_PROTO_INTERNAL(PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC, glGetNamedRenderbufferParameteriv)
    GL_PROTO_INTERNAL(PFNGLCREATETEXTURESPROC, glCreateTextures)
    GL_PROTO_INTERNAL(PFNGLTEXTUREBUFFERPROC, glTextureBuffer)
    GL_PROTO_INTERNAL(PFNGLTEXTUREBUFFERRANGEPROC, glTextureBufferRange)
    GL_PROTO_INTERNAL(PFNGLTEXTURESTORAGE1DPROC, glTextureStorage1D)
    GL_PROTO_INTERNAL(PFNGLTEXTURESTORAGE2DPROC, glTextureStorage2D)
    GL_PROTO_INTERNAL(PFNGLTEXTURESTORAGE3DPROC, glTextureStorage3D)
    GL_PROTO_INTERNAL(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC, glTextureStorage2DMultisample)
    GL_PROTO_INTERNAL(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC, glTextureStorage3DMultisample)
    GL_PROTO_INTERNAL(PFNGLTEXTURESUBIMAGE1DPROC, glTextureSubImage1D)
    GL_PROTO_INTERNAL(PFNGLTEXTURESUBIMAGE2DPROC, glTextureSubImage2D)
    GL_PROTO_INTERNAL(PFNGLTEXTURESUBIMAGE3DPROC, glTextureSubImage3D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC, glCompressedTextureSubImage1D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC, glCompressedTextureSubImage2D)
    GL_PROTO_INTERNAL(PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC, glCompressedTextureSubImage3D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXTURESUBIMAGE1DPROC, glCopyTextureSubImage1D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXTURESUBIMAGE2DPROC, glCopyTextureSubImage2D)
    GL_PROTO_INTERNAL(PFNGLCOPYTEXTURESUBIMAGE3DPROC, glCopyTextureSubImage3D)
    GL_PROTO_INTERNAL(PFNGLTEXTUREPARAMETERFPROC, glTextureParameterf)
    GL_PROTO_INTERNAL(PFNGLTEXTUREPARAMETERFVPROC, glTextureParameterfv)
    GL_PROTO_INTERNAL(PFNGLTEXTUREPARAMETERIPROC, glTextureParameteri)
    GL_PROTO_INTERNAL(PFNGLTEXTUREPARAMETERIIVPROC, glTextureParameterIiv)
    GL_PROTO_INTERNAL(PFNGLTEXTUREPARAMETERIUIVPROC, glTextureParameterIuiv)
    GL_PROTO_INTERNAL(PFNGLTEXTUREPARAMETERIVPROC, glTextureParameteriv)
    GL_PROTO_INTERNAL(PFNGLGENERATETEXTUREMIPMAPPROC, glGenerateTextureMipmap)
    GL_PROTO_INTERNAL(PFNGLBINDTEXTUREUNITPROC, glBindTextureUnit)
    GL_PROTO_INTERNAL(PFNGLGETTEXTUREIMAGEPROC, glGetTextureImage)
    GL_PROTO_INTERNAL(PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC, glGetCompressedTextureImage)
    GL_PROTO_INTERNAL(PFNGLGETTEXTURELEVELPARAMETERFVPROC, glGetTextureLevelParameterfv)
    GL_PROTO_INTERNAL(PFNGLGETTEXTURELEVELPARAMETERIVPROC, glGetTextureLevelParameteriv)
    GL_PROTO_INTERNAL(PFNGLGETTEXTUREPARAMETERFVPROC, glGetTextureParameterfv)
    GL_PROTO_INTERNAL(PFNGLGETTEXTUREPARAMETERIIVPROC, glGetTextureParameterIiv)
    GL_PROTO_INTERNAL(PFNGLGETTEXTUREPARAMETERIUIVPROC, glGetTextureParameterIuiv)
    GL_PROTO_INTERNAL(PFNGLGETTEXTUREPARAMETERIVPROC, glGetTextureParameteriv)
    GL_PROTO_INTERNAL(PFNGLCREATEVERTEXARRAYSPROC, glCreateVertexArrays)
    GL_PROTO_INTERNAL(PFNGLDISABLEVERTEXARRAYATTRIBPROC, glDisableVertexArrayAttrib)
    GL_PROTO_INTERNAL(PFNGLENABLEVERTEXARRAYATTRIBPROC, glEnableVertexArrayAttrib)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYELEMENTBUFFERPROC, glVertexArrayElementBuffer)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYVERTEXBUFFERPROC, glVertexArrayVertexBuffer)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYVERTEXBUFFERSPROC, glVertexArrayVertexBuffers)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYATTRIBBINDINGPROC, glVertexArrayAttribBinding)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYATTRIBFORMATPROC, glVertexArrayAttribFormat)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYATTRIBIFORMATPROC, glVertexArrayAttribIFormat)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYATTRIBLFORMATPROC, glVertexArrayAttribLFormat)
    GL_PROTO_INTERNAL(PFNGLVERTEXARRAYBINDINGDIVISORPROC, glVertexArrayBindingDivisor)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXARRAYIVPROC, glGetVertexArrayiv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXARRAYINDEXEDIVPROC, glGetVertexArrayIndexediv)
    GL_PROTO_INTERNAL(PFNGLGETVERTEXARRAYINDEXED64IVPROC, glGetVertexArrayIndexed64iv)
    GL_PROTO_INTERNAL(PFNGLCREATESAMPLERSPROC, glCreateSamplers)
    GL_PROTO_INTERNAL(PFNGLCREATEPROGRAMPIPELINESPROC, glCreateProgramPipelines)
    GL_PROTO_INTERNAL(PFNGLCREATEQUERIESPROC, glCreateQueries)
    GL_PROTO_INTERNAL(PFNGLGETQUERYBUFFEROBJECTI64VPROC, glGetQueryBufferObjecti64v)
    GL_PROTO_INTERNAL(PFNGLGETQUERYBUFFEROBJECTIVPROC, glGetQueryBufferObjectiv)
    GL_PROTO_INTERNAL(PFNGLGETQUERYBUFFEROBJECTUI64VPROC, glGetQueryBufferObjectui64v)
    GL_PROTO_INTERNAL(PFNGLGETQUERYBUFFEROBJECTUIVPROC, glGetQueryBufferObjectuiv)
    GL_PROTO_INTERNAL(PFNGLMEMORYBARRIERBYREGIONPROC, glMemoryBarrierByRegion)
    GL_PROTO_INTERNAL(PFNGLGETTEXTURESUBIMAGEPROC, glGetTextureSubImage)
    GL_PROTO_INTERNAL(PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC, glGetCompressedTextureSubImage)
    GL_PROTO_INTERNAL(PFNGLGETGRAPHICSRESETSTATUSPROC, glGetGraphicsResetStatus)
    GL_PROTO_INTERNAL(PFNGLGETNCOMPRESSEDTEXIMAGEPROC, glGetnCompressedTexImage)
    GL_PROTO_INTERNAL(PFNGLGETNTEXIMAGEPROC, glGetnTexImage)
    GL_PROTO_INTERNAL(PFNGLGETNUNIFORMDVPROC, glGetnUniformdv)
    GL_PROTO_INTERNAL(PFNGLGETNUNIFORMFVPROC, glGetnUniformfv)
    GL_PROTO_INTERNAL(PFNGLGETNUNIFORMIVPROC, glGetnUniformiv)
    GL_PROTO_INTERNAL(PFNGLGETNUNIFORMUIVPROC, glGetnUniformuiv)
    GL_PROTO_INTERNAL(PFNGLREADNPIXELSPROC, glReadnPixels)
    GL_PROTO_INTERNAL(PFNGLTEXTUREBARRIERPROC, glTextureBarrier)
#   if defined(BL_PLATFORM_WIN32)
    GL_PROTO_INTERNAL(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB)
    GL_PROTO_INTERNAL(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT)
#	define GL_SYMBOL_INTERNAL(type, func, lib) { func = (type)wglGetProcAddress(#func); if(!func) func = (type)GetProcAddress(lib, #func); }
#   elif defined(BL_PLATFORM_LINUX)
    typedef BLVoid (APIENTRYP PFNGLXSWAPBUFFERSPROC) (Display*, GLXDrawable);
    GL_PROTO_INTERNAL(PFNGLXGETPROCADDRESSPROC, glXGetProcAddress)
    GL_PROTO_INTERNAL(PFNGLXGETPROCADDRESSARBPROC, glXGetProcAddressARB)
    GL_PROTO_INTERNAL(PFNGLXSWAPBUFFERSPROC, glxSwapBuffersARB)
#	define GL_SYMBOL_INTERNAL(type, func, lib) { func = (type)dlsym(lib, #func); }
#   elif defined(BL_PLATFORM_OSX)
#	define GL_SYMBOL_INTERNAL(type, func, lib) { CFStringRef _proc = CFStringCreateWithCString(kCFAllocatorDefault, #func, kCFStringEncodingASCII); func = (type)CFBundleGetFunctionPointerForName(lib, _proc); CFRelease(_proc); }
#   endif
#	define GL_LOAD_INTERNAL(lib)																											\
{																																			\
GL_SYMBOL_INTERNAL(PFNGLCULLFACEPROC, glCullFace, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLFRONTFACEPROC, glFrontFace, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLHINTPROC, glHint, lib)																								\
GL_SYMBOL_INTERNAL(PFNGLLINEWIDTHPROC, glLineWidth, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLPOINTSIZEPROC, glPointSize, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLPOLYGONMODEPROC, glPolygonMode, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLSCISSORPROC, glScissor, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLTEXPARAMETERFPROC, glTexParameterf, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXPARAMETERFVPROC, glTexParameterfv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXPARAMETERIPROC, glTexParameteri, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXPARAMETERIVPROC, glTexParameteriv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXIMAGE1DPROC, glTexImage1D, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLTEXIMAGE2DPROC, glTexImage2D, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDRAWBUFFERPROC, glDrawBuffer, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLCLEARPROC, glClear, lib)																							\
GL_SYMBOL_INTERNAL(PFNGLCLEARCOLORPROC, glClearColor, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLCLEARSTENCILPROC, glClearStencil, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLCLEARDEPTHPROC, glClearDepth, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLSTENCILMASKPROC, glStencilMask, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLCOLORMASKPROC, glColorMask, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDEPTHMASKPROC, glDepthMask, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDISABLEPROC, glDisable, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLENABLEPROC, glEnable, lib)																							\
GL_SYMBOL_INTERNAL(PFNGLFINISHPROC, glFinish, lib)																							\
GL_SYMBOL_INTERNAL(PFNGLFLUSHPROC, glFlush, lib)																							\
GL_SYMBOL_INTERNAL(PFNGLBLENDFUNCPROC, glBlendFunc, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLLOGICOPPROC, glLogicOp, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLSTENCILFUNCPROC, glStencilFunc, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLSTENCILOPPROC, glStencilOp, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDEPTHFUNCPROC, glDepthFunc, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLPIXELSTOREFPROC, glPixelStoref, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLPIXELSTOREIPROC, glPixelStorei, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLREADBUFFERPROC, glReadBuffer, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLREADPIXELSPROC, glReadPixels, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETBOOLEANVPROC, glGetBooleanv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETDOUBLEVPROC, glGetDoublev, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETERRORPROC, glGetError, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLGETFLOATVPROC, glGetFloatv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETINTEGERVPROC, glGetIntegerv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETSTRINGPROC, glGetString, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETTEXIMAGEPROC, glGetTexImage, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETTEXPARAMETERFVPROC, glGetTexParameterfv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETTEXPARAMETERIVPROC, glGetTexParameteriv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETTEXLEVELPARAMETERFVPROC, glGetTexLevelParameterfv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETTEXLEVELPARAMETERIVPROC, glGetTexLevelParameteriv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLISENABLEDPROC, glIsEnabled, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDEPTHRANGEPROC, glDepthRange, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLVIEWPORTPROC, glViewport, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLDRAWARRAYSPROC, glDrawArrays, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSPROC, glDrawElements, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETPOINTERVPROC, glGetPointerv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLPOLYGONOFFSETPROC, glPolygonOffset, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXIMAGE1DPROC, glCopyTexImage1D, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXIMAGE2DPROC, glCopyTexImage2D, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXSUBIMAGE1DPROC, glCopyTexSubImage1D, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXSUBIMAGE2DPROC, glCopyTexSubImage2D, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXSUBIMAGE1DPROC, glTexSubImage1D, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLBINDTEXTUREPROC, glBindTexture, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDELETETEXTURESPROC, glDeleteTextures, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGENTEXTURESPROC, glGenTextures, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLISTEXTUREPROC, glIsTexture, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDRAWRANGEELEMENTSPROC, glDrawRangeElements, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXIMAGE3DPROC, glTexImage3D, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXSUBIMAGE3DPROC, glCopyTexSubImage3D, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLACTIVETEXTUREPROC, glActiveTexture, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLSAMPLECOVERAGEPROC, glSampleCoverage, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXIMAGE3DPROC, glCompressedTexImage3D, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXIMAGE1DPROC, glCompressedTexImage1D, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC, glCompressedTexSubImage3D, lib)														\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC, glCompressedTexSubImage2D, lib)														\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC, glCompressedTexSubImage1D, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETCOMPRESSEDTEXIMAGEPROC, glGetCompressedTexImage, lib)															\
GL_SYMBOL_INTERNAL(PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLMULTIDRAWARRAYSPROC, glMultiDrawArrays, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLMULTIDRAWELEMENTSPROC, glMultiDrawElements, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPOINTPARAMETERFPROC, glPointParameterf, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPOINTPARAMETERFVPROC, glPointParameterfv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPOINTPARAMETERIPROC, glPointParameteri, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPOINTPARAMETERIVPROC, glPointParameteriv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBLENDCOLORPROC, glBlendColor, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLBLENDEQUATIONPROC, glBlendEquation, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGENQUERIESPROC, glGenQueries, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDELETEQUERIESPROC, glDeleteQueries, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLISQUERYPROC, glIsQuery, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLBEGINQUERYPROC, glBeginQuery, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLENDQUERYPROC, glEndQuery, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYIVPROC, glGetQueryiv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYOBJECTIVPROC, glGetQueryObjectiv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYOBJECTUIVPROC, glGetQueryObjectuiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLBINDBUFFERPROC, glBindBuffer, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLDELETEBUFFERSPROC, glDeleteBuffers, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGENBUFFERSPROC, glGenBuffers, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLISBUFFERPROC, glIsBuffer, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLBUFFERDATAPROC, glBufferData, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLBUFFERSUBDATAPROC, glBufferSubData, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETBUFFERSUBDATAPROC, glGetBufferSubData, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLMAPBUFFERPROC, glMapBuffer, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNMAPBUFFERPROC, glUnmapBuffer, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETBUFFERPOINTERVPROC, glGetBufferPointerv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate, lib)															\
GL_SYMBOL_INTERNAL(PFNGLDRAWBUFFERSPROC, glDrawBuffers, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLSTENCILOPSEPARATEPROC, glStencilOpSeparate, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLSTENCILFUNCSEPARATEPROC, glStencilFuncSeparate, lib)																\
GL_SYMBOL_INTERNAL(PFNGLSTENCILMASKSEPARATEPROC, glStencilMaskSeparate, lib)																\
GL_SYMBOL_INTERNAL(PFNGLATTACHSHADERPROC, glAttachShader, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLCOMPILESHADERPROC, glCompileShader, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCREATEPROGRAMPROC, glCreateProgram, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCREATESHADERPROC, glCreateShader, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDELETEPROGRAMPROC, glDeleteProgram, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLDELETESHADERPROC, glDeleteShader, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDETACHSHADERPROC, glDetachShader, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray, lib)														\
GL_SYMBOL_INTERNAL(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETATTACHEDSHADERSPROC, glGetAttachedShaders, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMIVPROC, glGetProgramiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETSHADERIVPROC, glGetShaderiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETSHADERSOURCEPROC, glGetShaderSource, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMFVPROC, glGetUniformfv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMIVPROC, glGetUniformiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBDVPROC, glGetVertexAttribdv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBFVPROC, glGetVertexAttribfv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLISPROGRAMPROC, glIsProgram, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLISSHADERPROC, glIsShader, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLLINKPROGRAMPROC, glLinkProgram, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLSHADERSOURCEPROC, glShaderSource, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLUSEPROGRAMPROC, glUseProgram, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1FPROC, glUniform1f, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2FPROC, glUniform2f, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3FPROC, glUniform3f, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4FPROC, glUniform4f, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1IPROC, glUniform1i, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2IPROC, glUniform2i, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3IPROC, glUniform3i, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4IPROC, glUniform4i, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1FVPROC, glUniform1fv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2FVPROC, glUniform2fv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3FVPROC, glUniform3fv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4FVPROC, glUniform4fv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1IVPROC, glUniform1iv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2IVPROC, glUniform2iv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3IVPROC, glUniform3iv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4IVPROC, glUniform4iv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVALIDATEPROGRAMPROC, glValidateProgram, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB1DPROC, glVertexAttrib1d, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB1DVPROC, glVertexAttrib1dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB1FPROC, glVertexAttrib1f, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB1FVPROC, glVertexAttrib1fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB1SPROC, glVertexAttrib1s, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB1SVPROC, glVertexAttrib1sv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB2DPROC, glVertexAttrib2d, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB2DVPROC, glVertexAttrib2dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB2FPROC, glVertexAttrib2f, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB2FVPROC, glVertexAttrib2fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB2SPROC, glVertexAttrib2s, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB2SVPROC, glVertexAttrib2sv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB3DPROC, glVertexAttrib3d, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB3DVPROC, glVertexAttrib3dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB3FPROC, glVertexAttrib3f, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB3FVPROC, glVertexAttrib3fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB3SPROC, glVertexAttrib3s, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB3SVPROC, glVertexAttrib3sv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NBVPROC, glVertexAttrib4Nbv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NIVPROC, glVertexAttrib4Niv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NSVPROC, glVertexAttrib4Nsv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NUBPROC, glVertexAttrib4Nub, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NUBVPROC, glVertexAttrib4Nubv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NUIVPROC, glVertexAttrib4Nuiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4NUSVPROC, glVertexAttrib4Nusv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4BVPROC, glVertexAttrib4bv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4DPROC, glVertexAttrib4d, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4DVPROC, glVertexAttrib4dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4FPROC, glVertexAttrib4f, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4FVPROC, glVertexAttrib4fv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4IVPROC, glVertexAttrib4iv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4SPROC, glVertexAttrib4s, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4SVPROC, glVertexAttrib4sv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4UBVPROC, glVertexAttrib4ubv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4UIVPROC, glVertexAttrib4uiv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIB4USVPROC, glVertexAttrib4usv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer, lib)																\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX2X3FVPROC, glUniformMatrix2x3fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX3X2FVPROC, glUniformMatrix3x2fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX2X4FVPROC, glUniformMatrix2x4fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX4X2FVPROC, glUniformMatrix4x2fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX3X4FVPROC, glUniformMatrix3x4fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX4X3FVPROC, glUniformMatrix4x3fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLCOLORMASKIPROC, glColorMaski, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETBOOLEANI_VPROC, glGetBooleani_v, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETINTEGERI_VPROC, glGetIntegeri_v, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLENABLEIPROC, glEnablei, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLDISABLEIPROC, glDisablei, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLISENABLEDIPROC, glIsEnabledi, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLBEGINTRANSFORMFEEDBACKPROC, glBeginTransformFeedback, lib)															\
GL_SYMBOL_INTERNAL(PFNGLENDTRANSFORMFEEDBACKPROC, glEndTransformFeedback, lib)																\
GL_SYMBOL_INTERNAL(PFNGLBINDBUFFERRANGEPROC, glBindBufferRange, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBINDBUFFERBASEPROC, glBindBufferBase, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTRANSFORMFEEDBACKVARYINGSPROC, glTransformFeedbackVaryings, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETTRANSFORMFEEDBACKVARYINGPROC, glGetTransformFeedbackVarying, lib)												\
GL_SYMBOL_INTERNAL(PFNGLCLAMPCOLORPROC, glClampColor, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLBEGINCONDITIONALRENDERPROC, glBeginConditionalRender, lib)															\
GL_SYMBOL_INTERNAL(PFNGLENDCONDITIONALRENDERPROC, glEndConditionalRender, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBIIVPROC, glGetVertexAttribIiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBIUIVPROC, glGetVertexAttribIuiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI1IPROC, glVertexAttribI1i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI2IPROC, glVertexAttribI2i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI3IPROC, glVertexAttribI3i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4IPROC, glVertexAttribI4i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI1UIPROC, glVertexAttribI1ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI2UIPROC, glVertexAttribI2ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI3UIPROC, glVertexAttribI3ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4UIPROC, glVertexAttribI4ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI1IVPROC, glVertexAttribI1iv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI2IVPROC, glVertexAttribI2iv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI3IVPROC, glVertexAttribI3iv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4IVPROC, glVertexAttribI4iv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI1UIVPROC, glVertexAttribI1uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI2UIVPROC, glVertexAttribI2uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI3UIVPROC, glVertexAttribI3uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4UIVPROC, glVertexAttribI4uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4BVPROC, glVertexAttribI4bv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4SVPROC, glVertexAttribI4sv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4UBVPROC, glVertexAttribI4ubv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBI4USVPROC, glVertexAttribI4usv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMUIVPROC, glGetUniformuiv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLBINDFRAGDATALOCATIONPROC, glBindFragDataLocation, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETFRAGDATALOCATIONPROC, glGetFragDataLocation, lib)																\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1UIPROC, glUniform1ui, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2UIPROC, glUniform2ui, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3UIPROC, glUniform3ui, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4UIPROC, glUniform4ui, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1UIVPROC, glUniform1uiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2UIVPROC, glUniform2uiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3UIVPROC, glUniform3uiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4UIVPROC, glUniform4uiv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLTEXPARAMETERIIVPROC, glTexParameterIiv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLTEXPARAMETERIUIVPROC, glTexParameterIuiv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETTEXPARAMETERIIVPROC, glGetTexParameterIiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETTEXPARAMETERIUIVPROC, glGetTexParameterIuiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCLEARBUFFERIVPROC, glClearBufferiv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCLEARBUFFERUIVPROC, glClearBufferuiv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCLEARBUFFERFVPROC, glClearBufferfv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCLEARBUFFERFIPROC, glClearBufferfi, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETSTRINGIPROC, glGetStringi, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLISRENDERBUFFERPROC, glIsRenderbuffer, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETRENDERBUFFERPARAMETERIVPROC, glGetRenderbufferParameteriv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLISFRAMEBUFFERPROC, glIsFramebuffer, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus, lib)															\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERTEXTURE1DPROC, glFramebufferTexture1D, lib)																\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D, lib)																\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERTEXTURE3DPROC, glFramebufferTexture3D, lib)																\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetFramebufferAttachmentParameteriv, lib)								\
GL_SYMBOL_INTERNAL(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC, glRenderbufferStorageMultisample, lib)											\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERTEXTURELAYERPROC, glFramebufferTextureLayer, lib)														\
GL_SYMBOL_INTERNAL(PFNGLMAPBUFFERRANGEPROC, glMapBufferRange, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLFLUSHMAPPEDBUFFERRANGEPROC, glFlushMappedBufferRange, lib)															\
GL_SYMBOL_INTERNAL(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLISVERTEXARRAYPROC, glIsVertexArray, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced, lib)																\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced, lib)															\
GL_SYMBOL_INTERNAL(PFNGLTEXBUFFERPROC, glTexBuffer, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLPRIMITIVERESTARTINDEXPROC, glPrimitiveRestartIndex, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCOPYBUFFERSUBDATAPROC, glCopyBufferSubData, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMINDICESPROC, glGetUniformIndices, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEUNIFORMSIVPROC, glGetActiveUniformsiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEUNIFORMNAMEPROC, glGetActiveUniformName, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEUNIFORMBLOCKIVPROC, glGetActiveUniformBlockiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC, glGetActiveUniformBlockName, lib)													\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding, lib)																\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSBASEVERTEXPROC, glDrawElementsBaseVertex, lib)															\
GL_SYMBOL_INTERNAL(PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC, glDrawRangeElementsBaseVertex, lib)												\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC, glDrawElementsInstancedBaseVertex, lib)										\
GL_SYMBOL_INTERNAL(PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC, glMultiDrawElementsBaseVertex, lib)												\
GL_SYMBOL_INTERNAL(PFNGLPROVOKINGVERTEXPROC, glProvokingVertex, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLFENCESYNCPROC, glFenceSync, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLISSYNCPROC, glIsSync, lib)																							\
GL_SYMBOL_INTERNAL(PFNGLDELETESYNCPROC, glDeleteSync, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLCLIENTWAITSYNCPROC, glClientWaitSync, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLWAITSYNCPROC, glWaitSync, lib)																						\
GL_SYMBOL_INTERNAL(PFNGLGETINTEGER64VPROC, glGetInteger64v, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETSYNCIVPROC, glGetSynciv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLGETINTEGER64I_VPROC, glGetInteger64i_v, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETBUFFERPARAMETERI64VPROC, glGetBufferParameteri64v, lib)															\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERTEXTUREPROC, glFramebufferTexture, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXIMAGE2DMULTISAMPLEPROC, glTexImage2DMultisample, lib)															\
GL_SYMBOL_INTERNAL(PFNGLTEXIMAGE3DMULTISAMPLEPROC, glTexImage3DMultisample, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETMULTISAMPLEFVPROC, glGetMultisamplefv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLSAMPLEMASKIPROC, glSampleMaski, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLBINDFRAGDATALOCATIONINDEXEDPROC, glBindFragDataLocationIndexed, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETFRAGDATAINDEXPROC, glGetFragDataIndex, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGENSAMPLERSPROC, glGenSamplers, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDELETESAMPLERSPROC, glDeleteSamplers, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLISSAMPLERPROC, glIsSampler, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLBINDSAMPLERPROC, glBindSampler, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLSAMPLERPARAMETERIPROC, glSamplerParameteri, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLSAMPLERPARAMETERIVPROC, glSamplerParameteriv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLSAMPLERPARAMETERFPROC, glSamplerParameterf, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLSAMPLERPARAMETERFVPROC, glSamplerParameterfv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLSAMPLERPARAMETERIIVPROC, glSamplerParameterIiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLSAMPLERPARAMETERIUIVPROC, glSamplerParameterIuiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETSAMPLERPARAMETERIIVPROC, glGetSamplerParameterIiv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETSAMPLERPARAMETERFVPROC, glGetSamplerParameterfv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETSAMPLERPARAMETERIUIVPROC, glGetSamplerParameterIuiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLQUERYCOUNTERPROC, glQueryCounter, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYOBJECTI64VPROC, glGetQueryObjecti64v, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYOBJECTUI64VPROC, glGetQueryObjectui64v, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP1UIPROC, glVertexAttribP1ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP1UIVPROC, glVertexAttribP1uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP2UIPROC, glVertexAttribP2ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP2UIVPROC, glVertexAttribP2uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP3UIPROC, glVertexAttribP3ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP3UIVPROC, glVertexAttribP3uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP4UIPROC, glVertexAttribP4ui, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBP4UIVPROC, glVertexAttribP4uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLMINSAMPLESHADINGPROC, glMinSampleShading, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBLENDEQUATIONIPROC, glBlendEquationi, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLBLENDEQUATIONSEPARATEIPROC, glBlendEquationSeparatei, lib)															\
GL_SYMBOL_INTERNAL(PFNGLBLENDFUNCIPROC, glBlendFunci, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLBLENDFUNCSEPARATEIPROC, glBlendFuncSeparatei, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLDRAWARRAYSINDIRECTPROC, glDrawArraysIndirect, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSINDIRECTPROC, glDrawElementsIndirect, lib)																\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1DPROC, glUniform1d, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2DPROC, glUniform2d, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3DPROC, glUniform3d, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4DPROC, glUniform4d, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM1DVPROC, glUniform1dv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM2DVPROC, glUniform2dv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM3DVPROC, glUniform3dv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORM4DVPROC, glUniform4dv, lib)																					\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX2DVPROC, glUniformMatrix2dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX3DVPROC, glUniformMatrix3dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX4DVPROC, glUniformMatrix4dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX2X3DVPROC, glUniformMatrix2x3dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX2X4DVPROC, glUniformMatrix2x4dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX3X2DVPROC, glUniformMatrix3x2dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX3X4DVPROC, glUniformMatrix3x4dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX4X2DVPROC, glUniformMatrix4x2dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMMATRIX4X3DVPROC, glUniformMatrix4x3dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMDVPROC, glGetUniformdv, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC, glGetSubroutineUniformLocation, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETSUBROUTINEINDEXPROC, glGetSubroutineIndex, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC, glGetActiveSubroutineUniformiv, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC, glGetActiveSubroutineUniformName, lib)											\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVESUBROUTINENAMEPROC, glGetActiveSubroutineName, lib)														\
GL_SYMBOL_INTERNAL(PFNGLUNIFORMSUBROUTINESUIVPROC, glUniformSubroutinesuiv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETUNIFORMSUBROUTINEUIVPROC, glGetUniformSubroutineuiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMSTAGEIVPROC, glGetProgramStageiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPATCHPARAMETERIPROC, glPatchParameteri, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPATCHPARAMETERFVPROC, glPatchParameterfv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBINDTRANSFORMFEEDBACKPROC, glBindTransformFeedback, lib)															\
GL_SYMBOL_INTERNAL(PFNGLDELETETRANSFORMFEEDBACKSPROC, glDeleteTransformFeedbacks, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGENTRANSFORMFEEDBACKSPROC, glGenTransformFeedbacks, lib)															\
GL_SYMBOL_INTERNAL(PFNGLISTRANSFORMFEEDBACKPROC, glIsTransformFeedback, lib)																\
GL_SYMBOL_INTERNAL(PFNGLPAUSETRANSFORMFEEDBACKPROC, glPauseTransformFeedback, lib)															\
GL_SYMBOL_INTERNAL(PFNGLRESUMETRANSFORMFEEDBACKPROC, glResumeTransformFeedback, lib)														\
GL_SYMBOL_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKPROC, glDrawTransformFeedback, lib)															\
GL_SYMBOL_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC, glDrawTransformFeedbackStream, lib)												\
GL_SYMBOL_INTERNAL(PFNGLBEGINQUERYINDEXEDPROC, glBeginQueryIndexed, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLENDQUERYINDEXEDPROC, glEndQueryIndexed, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYINDEXEDIVPROC, glGetQueryIndexediv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLRELEASESHADERCOMPILERPROC, glReleaseShaderCompiler, lib)															\
GL_SYMBOL_INTERNAL(PFNGLSHADERBINARYPROC, glShaderBinary, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETSHADERPRECISIONFORMATPROC, glGetShaderPrecisionFormat, lib)														\
GL_SYMBOL_INTERNAL(PFNGLDEPTHRANGEFPROC, glDepthRangef, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLCLEARDEPTHFPROC, glClearDepthf, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMBINARYPROC, glGetProgramBinary, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMBINARYPROC, glProgramBinary, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMPARAMETERIPROC, glProgramParameteri, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLUSEPROGRAMSTAGESPROC, glUseProgramStages, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLACTIVESHADERPROGRAMPROC, glActiveShaderProgram, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCREATESHADERPROGRAMVPROC, glCreateShaderProgramv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLBINDPROGRAMPIPELINEPROC, glBindProgramPipeline, lib)																\
GL_SYMBOL_INTERNAL(PFNGLDELETEPROGRAMPIPELINESPROC, glDeleteProgramPipelines, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGENPROGRAMPIPELINESPROC, glGenProgramPipelines, lib)																\
GL_SYMBOL_INTERNAL(PFNGLISPROGRAMPIPELINEPROC, glIsProgramPipeline, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMPIPELINEIVPROC, glGetProgramPipelineiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1IPROC, glProgramUniform1i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1IVPROC, glProgramUniform1iv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1FPROC, glProgramUniform1f, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1FVPROC, glProgramUniform1fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1DPROC, glProgramUniform1d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1DVPROC, glProgramUniform1dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1UIPROC, glProgramUniform1ui, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM1UIVPROC, glProgramUniform1uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2IPROC, glProgramUniform2i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2IVPROC, glProgramUniform2iv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2FPROC, glProgramUniform2f, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2FVPROC, glProgramUniform2fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2DPROC, glProgramUniform2d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2DVPROC, glProgramUniform2dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2UIPROC, glProgramUniform2ui, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM2UIVPROC, glProgramUniform2uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3IPROC, glProgramUniform3i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3IVPROC, glProgramUniform3iv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3FPROC, glProgramUniform3f, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3FVPROC, glProgramUniform3fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3DPROC, glProgramUniform3d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3DVPROC, glProgramUniform3dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3UIPROC, glProgramUniform3ui, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM3UIVPROC, glProgramUniform3uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4IPROC, glProgramUniform4i, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4IVPROC, glProgramUniform4iv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4FPROC, glProgramUniform4f, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4FVPROC, glProgramUniform4fv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4DPROC, glProgramUniform4d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4DVPROC, glProgramUniform4dv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4UIPROC, glProgramUniform4ui, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORM4UIVPROC, glProgramUniform4uiv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2FVPROC, glProgramUniformMatrix2fv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3FVPROC, glProgramUniformMatrix3fv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4FVPROC, glProgramUniformMatrix4fv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2DVPROC, glProgramUniformMatrix2dv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3DVPROC, glProgramUniformMatrix3dv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4DVPROC, glProgramUniformMatrix4dv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC, glProgramUniformMatrix2x3fv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC, glProgramUniformMatrix3x2fv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC, glProgramUniformMatrix2x4fv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC, glProgramUniformMatrix4x2fv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC, glProgramUniformMatrix3x4fv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC, glProgramUniformMatrix4x3fv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC, glProgramUniformMatrix2x3dv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC, glProgramUniformMatrix3x2dv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC, glProgramUniformMatrix2x4dv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC, glProgramUniformMatrix4x2dv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC, glProgramUniformMatrix3x4dv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC, glProgramUniformMatrix4x3dv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLVALIDATEPROGRAMPIPELINEPROC, glValidateProgramPipeline, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMPIPELINEINFOLOGPROC, glGetProgramPipelineInfoLog, lib)													\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL1DPROC, glVertexAttribL1d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL2DPROC, glVertexAttribL2d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL3DPROC, glVertexAttribL3d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL4DPROC, glVertexAttribL4d, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL1DVPROC, glVertexAttribL1dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL2DVPROC, glVertexAttribL2dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL3DVPROC, glVertexAttribL3dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBL4DVPROC, glVertexAttribL4dv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBLPOINTERPROC, glVertexAttribLPointer, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXATTRIBLDVPROC, glGetVertexAttribLdv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVIEWPORTARRAYVPROC, glViewportArrayv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLVIEWPORTINDEXEDFPROC, glViewportIndexedf, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVIEWPORTINDEXEDFVPROC, glViewportIndexedfv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLSCISSORARRAYVPROC, glScissorArrayv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLSCISSORINDEXEDPROC, glScissorIndexed, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLSCISSORINDEXEDVPROC, glScissorIndexedv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLDEPTHRANGEARRAYVPROC, glDepthRangeArrayv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLDEPTHRANGEINDEXEDPROC, glDepthRangeIndexed, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETFLOATI_VPROC, glGetFloati_v, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETDOUBLEI_VPROC, glGetDoublei_v, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC, glDrawArraysInstancedBaseInstance, lib)										\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC, glDrawElementsInstancedBaseInstance, lib)									\
GL_SYMBOL_INTERNAL(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC, glDrawElementsInstancedBaseVertexBaseInstance, lib)				\
GL_SYMBOL_INTERNAL(PFNGLGETINTERNALFORMATIVPROC, glGetInternalformativ, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC, glGetActiveAtomicCounterBufferiv, lib)											\
GL_SYMBOL_INTERNAL(PFNGLBINDIMAGETEXTUREPROC, glBindImageTexture, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLMEMORYBARRIERPROC, glMemoryBarrier, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXSTORAGE1DPROC, glTexStorage1D, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLTEXSTORAGE2DPROC, glTexStorage2D, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLTEXSTORAGE3DPROC, glTexStorage3D, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC, glDrawTransformFeedbackInstanced, lib)											\
GL_SYMBOL_INTERNAL(PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC, glDrawTransformFeedbackStreamInstanced, lib)								\
GL_SYMBOL_INTERNAL(PFNGLCLEARBUFFERDATAPROC, glClearBufferData, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLCLEARBUFFERSUBDATAPROC, glClearBufferSubData, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLDISPATCHCOMPUTEINDIRECTPROC, glDispatchComputeIndirect, lib)														\
GL_SYMBOL_INTERNAL(PFNGLCOPYIMAGESUBDATAPROC, glCopyImageSubData, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLFRAMEBUFFERPARAMETERIPROC, glFramebufferParameteri, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETFRAMEBUFFERPARAMETERIVPROC, glGetFramebufferParameteriv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETINTERNALFORMATI64VPROC, glGetInternalformati64v, lib)															\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATETEXSUBIMAGEPROC, glInvalidateTexSubImage, lib)															\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATETEXIMAGEPROC, glInvalidateTexImage, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATEBUFFERSUBDATAPROC, glInvalidateBufferSubData, lib)														\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATEBUFFERDATAPROC, glInvalidateBufferData, lib)																\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATEFRAMEBUFFERPROC, glInvalidateFramebuffer, lib)															\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATESUBFRAMEBUFFERPROC, glInvalidateSubFramebuffer, lib)														\
GL_SYMBOL_INTERNAL(PFNGLMULTIDRAWARRAYSINDIRECTPROC, glMultiDrawArraysIndirect, lib)														\
GL_SYMBOL_INTERNAL(PFNGLMULTIDRAWELEMENTSINDIRECTPROC, glMultiDrawElementsIndirect, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMINTERFACEIVPROC, glGetProgramInterfaceiv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMRESOURCEINDEXPROC, glGetProgramResourceIndex, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMRESOURCENAMEPROC, glGetProgramResourceName, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMRESOURCEIVPROC, glGetProgramResourceiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMRESOURCELOCATIONPROC, glGetProgramResourceLocation, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC, glGetProgramResourceLocationIndex, lib)										\
GL_SYMBOL_INTERNAL(PFNGLSHADERSTORAGEBLOCKBINDINGPROC, glShaderStorageBlockBinding, lib)													\
GL_SYMBOL_INTERNAL(PFNGLTEXBUFFERRANGEPROC, glTexBufferRange, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXSTORAGE2DMULTISAMPLEPROC, glTexStorage2DMultisample, lib)														\
GL_SYMBOL_INTERNAL(PFNGLTEXSTORAGE3DMULTISAMPLEPROC, glTexStorage3DMultisample, lib)														\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREVIEWPROC, glTextureView, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBIFORMATPROC, glVertexAttribIFormat, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBLFORMATPROC, glVertexAttribLFormat, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding, lib)																\
GL_SYMBOL_INTERNAL(PFNGLVERTEXBINDINGDIVISORPROC, glVertexBindingDivisor, lib)																\
GL_SYMBOL_INTERNAL(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl, lib)																\
GL_SYMBOL_INTERNAL(PFNGLDEBUGMESSAGEINSERTPROC, glDebugMessageInsert, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback, lib)																\
GL_SYMBOL_INTERNAL(PFNGLGETDEBUGMESSAGELOGPROC, glGetDebugMessageLog, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLPUSHDEBUGGROUPPROC, glPushDebugGroup, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLPOPDEBUGGROUPPROC, glPopDebugGroup, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLOBJECTLABELPROC, glObjectLabel, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETOBJECTLABELPROC, glGetObjectLabel, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLOBJECTPTRLABELPROC, glObjectPtrLabel, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETOBJECTPTRLABELPROC, glGetObjectPtrLabel, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLBUFFERSTORAGEPROC, glBufferStorage, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCLEARTEXIMAGEPROC, glClearTexImage, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCLEARTEXSUBIMAGEPROC, glClearTexSubImage, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBINDBUFFERSBASEPROC, glBindBuffersBase, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBINDBUFFERSRANGEPROC, glBindBuffersRange, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLBINDTEXTURESPROC, glBindTextures, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLBINDSAMPLERSPROC, glBindSamplers, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLBINDIMAGETEXTURESPROC, glBindImageTextures, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLBINDVERTEXBUFFERSPROC, glBindVertexBuffers, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLCLIPCONTROLPROC, glClipControl, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLCREATETRANSFORMFEEDBACKSPROC, glCreateTransformFeedbacks, lib)														\
GL_SYMBOL_INTERNAL(PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC, glTransformFeedbackBufferBase, lib)												\
GL_SYMBOL_INTERNAL(PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC, glTransformFeedbackBufferRange, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETTRANSFORMFEEDBACKIVPROC, glGetTransformFeedbackiv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETTRANSFORMFEEDBACKI_VPROC, glGetTransformFeedbacki_v, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETTRANSFORMFEEDBACKI64_VPROC, glGetTransformFeedbacki64_v, lib)													\
GL_SYMBOL_INTERNAL(PFNGLCREATEBUFFERSPROC, glCreateBuffers, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLNAMEDBUFFERSTORAGEPROC, glNamedBufferStorage, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLNAMEDBUFFERDATAPROC, glNamedBufferData, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLNAMEDBUFFERSUBDATAPROC, glNamedBufferSubData, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLCOPYNAMEDBUFFERSUBDATAPROC, glCopyNamedBufferSubData, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCLEARNAMEDBUFFERDATAPROC, glClearNamedBufferData, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCLEARNAMEDBUFFERSUBDATAPROC, glClearNamedBufferSubData, lib)														\
GL_SYMBOL_INTERNAL(PFNGLMAPNAMEDBUFFERPROC, glMapNamedBuffer, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLMAPNAMEDBUFFERRANGEPROC, glMapNamedBufferRange, lib)																\
GL_SYMBOL_INTERNAL(PFNGLUNMAPNAMEDBUFFERPROC, glUnmapNamedBuffer, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC, glFlushMappedNamedBufferRange, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDBUFFERPARAMETERIVPROC, glGetNamedBufferParameteriv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDBUFFERPARAMETERI64VPROC, glGetNamedBufferParameteri64v, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDBUFFERPOINTERVPROC, glGetNamedBufferPointerv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDBUFFERSUBDATAPROC, glGetNamedBufferSubData, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCREATEFRAMEBUFFERSPROC, glCreateFramebuffers, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC, glNamedFramebufferRenderbuffer, lib)												\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC, glNamedFramebufferParameteri, lib)													\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC, glNamedFramebufferTexture, lib)														\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC, glNamedFramebufferTextureLayer, lib)												\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC, glNamedFramebufferDrawBuffer, lib)													\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC, glNamedFramebufferDrawBuffers, lib)												\
GL_SYMBOL_INTERNAL(PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC, glNamedFramebufferReadBuffer, lib)													\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC, glInvalidateNamedFramebufferData, lib)											\
GL_SYMBOL_INTERNAL(PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC, glInvalidateNamedFramebufferSubData, lib)									\
GL_SYMBOL_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERIVPROC, glClearNamedFramebufferiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC, glClearNamedFramebufferuiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERFVPROC, glClearNamedFramebufferfv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLCLEARNAMEDFRAMEBUFFERFIPROC, glClearNamedFramebufferfi, lib)														\
GL_SYMBOL_INTERNAL(PFNGLBLITNAMEDFRAMEBUFFERPROC, glBlitNamedFramebuffer, lib)																\
GL_SYMBOL_INTERNAL(PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC, glCheckNamedFramebufferStatus, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC, glGetNamedFramebufferParameteriv, lib)											\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetNamedFramebufferAttachmentParameteriv, lib)						\
GL_SYMBOL_INTERNAL(PFNGLCREATERENDERBUFFERSPROC, glCreateRenderbuffers, lib)																\
GL_SYMBOL_INTERNAL(PFNGLNAMEDRENDERBUFFERSTORAGEPROC, glNamedRenderbufferStorage, lib)														\
GL_SYMBOL_INTERNAL(PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC, glNamedRenderbufferStorageMultisample, lib)								\
GL_SYMBOL_INTERNAL(PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC, glGetNamedRenderbufferParameteriv, lib)										\
GL_SYMBOL_INTERNAL(PFNGLCREATETEXTURESPROC, glCreateTextures, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREBUFFERPROC, glTextureBuffer, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREBUFFERRANGEPROC, glTextureBufferRange, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESTORAGE1DPROC, glTextureStorage1D, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESTORAGE2DPROC, glTextureStorage2D, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESTORAGE3DPROC, glTextureStorage3D, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC, glTextureStorage2DMultisample, lib)												\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC, glTextureStorage3DMultisample, lib)												\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESUBIMAGE1DPROC, glTextureSubImage1D, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESUBIMAGE2DPROC, glTextureSubImage2D, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXTURESUBIMAGE3DPROC, glTextureSubImage3D, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC, glCompressedTextureSubImage1D, lib)												\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC, glCompressedTextureSubImage2D, lib)												\
GL_SYMBOL_INTERNAL(PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC, glCompressedTextureSubImage3D, lib)												\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXTURESUBIMAGE1DPROC, glCopyTextureSubImage1D, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXTURESUBIMAGE2DPROC, glCopyTextureSubImage2D, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCOPYTEXTURESUBIMAGE3DPROC, glCopyTextureSubImage3D, lib)															\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREPARAMETERFPROC, glTextureParameterf, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREPARAMETERFVPROC, glTextureParameterfv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREPARAMETERIPROC, glTextureParameteri, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREPARAMETERIIVPROC, glTextureParameterIiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREPARAMETERIUIVPROC, glTextureParameterIuiv, lib)																\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREPARAMETERIVPROC, glTextureParameteriv, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGENERATETEXTUREMIPMAPPROC, glGenerateTextureMipmap, lib)															\
GL_SYMBOL_INTERNAL(PFNGLBINDTEXTUREUNITPROC, glBindTextureUnit, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTUREIMAGEPROC, glGetTextureImage, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC, glGetCompressedTextureImage, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTURELEVELPARAMETERFVPROC, glGetTextureLevelParameterfv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTURELEVELPARAMETERIVPROC, glGetTextureLevelParameteriv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTUREPARAMETERFVPROC, glGetTextureParameterfv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTUREPARAMETERIIVPROC, glGetTextureParameterIiv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTUREPARAMETERIUIVPROC, glGetTextureParameterIuiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTUREPARAMETERIVPROC, glGetTextureParameteriv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCREATEVERTEXARRAYSPROC, glCreateVertexArrays, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLDISABLEVERTEXARRAYATTRIBPROC, glDisableVertexArrayAttrib, lib)														\
GL_SYMBOL_INTERNAL(PFNGLENABLEVERTEXARRAYATTRIBPROC, glEnableVertexArrayAttrib, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYELEMENTBUFFERPROC, glVertexArrayElementBuffer, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYVERTEXBUFFERPROC, glVertexArrayVertexBuffer, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYVERTEXBUFFERSPROC, glVertexArrayVertexBuffers, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYATTRIBBINDINGPROC, glVertexArrayAttribBinding, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYATTRIBFORMATPROC, glVertexArrayAttribFormat, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYATTRIBIFORMATPROC, glVertexArrayAttribIFormat, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYATTRIBLFORMATPROC, glVertexArrayAttribLFormat, lib)														\
GL_SYMBOL_INTERNAL(PFNGLVERTEXARRAYBINDINGDIVISORPROC, glVertexArrayBindingDivisor, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXARRAYIVPROC, glGetVertexArrayiv, lib)																		\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXARRAYINDEXEDIVPROC, glGetVertexArrayIndexediv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETVERTEXARRAYINDEXED64IVPROC, glGetVertexArrayIndexed64iv, lib)													\
GL_SYMBOL_INTERNAL(PFNGLCREATESAMPLERSPROC, glCreateSamplers, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLCREATEPROGRAMPIPELINESPROC, glCreateProgramPipelines, lib)															\
GL_SYMBOL_INTERNAL(PFNGLCREATEQUERIESPROC, glCreateQueries, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYBUFFEROBJECTI64VPROC, glGetQueryBufferObjecti64v, lib)														\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYBUFFEROBJECTIVPROC, glGetQueryBufferObjectiv, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYBUFFEROBJECTUI64VPROC, glGetQueryBufferObjectui64v, lib)													\
GL_SYMBOL_INTERNAL(PFNGLGETQUERYBUFFEROBJECTUIVPROC, glGetQueryBufferObjectuiv, lib)														\
GL_SYMBOL_INTERNAL(PFNGLMEMORYBARRIERBYREGIONPROC, glMemoryBarrierByRegion, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETTEXTURESUBIMAGEPROC, glGetTextureSubImage, lib)																	\
GL_SYMBOL_INTERNAL(PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC, glGetCompressedTextureSubImage, lib)												\
GL_SYMBOL_INTERNAL(PFNGLGETGRAPHICSRESETSTATUSPROC, glGetGraphicsResetStatus, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETNCOMPRESSEDTEXIMAGEPROC, glGetnCompressedTexImage, lib)															\
GL_SYMBOL_INTERNAL(PFNGLGETNTEXIMAGEPROC, glGetnTexImage, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLGETNUNIFORMDVPROC, glGetnUniformdv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETNUNIFORMFVPROC, glGetnUniformfv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETNUNIFORMIVPROC, glGetnUniformiv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLGETNUNIFORMUIVPROC, glGetnUniformuiv, lib)																			\
GL_SYMBOL_INTERNAL(PFNGLREADNPIXELSPROC, glReadnPixels, lib)																				\
GL_SYMBOL_INTERNAL(PFNGLTEXTUREBARRIERPROC, glTextureBarrier, lib)																			\
}
#endif
#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#	define GL_COMPRESSED_RGB_S3TC_DXT1_EXT          0x83F0
#endif
#ifndef GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
#	define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT         0x8C4C
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#	define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT         0x83F1
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
#	define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT   0x8C4D
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#	define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT         0x83F3
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
#	define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT   0x8C4F
#endif
#ifndef GL_COMPRESSED_RGB8_ETC2
#	define GL_COMPRESSED_RGB8_ETC2                  0x9274
#endif
#ifndef GL_COMPRESSED_SRGB8_ETC2
#   define GL_COMPRESSED_SRGB8_ETC2                 0x9275
#endif
#ifndef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
#	define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2	0x9276
#endif
#ifndef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
#   define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#endif
#ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
#	define GL_COMPRESSED_RGBA8_ETC2_EAC                 0x9278
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
#   define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          0x9279
#endif
#ifndef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
#   define GL_COMPRESSED_RGBA_ASTC_4x4_KHR              0x93B0
#endif
#ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
#   define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR      0x93B0
#endif
#ifndef GL_COMPRESSED_RG_RGTC2
#   define GL_COMPRESSED_RG_RGTC2                       0x8DBD
#endif
#ifndef GL_COMPRESSED_RG11_EAC
#   define GL_COMPRESSED_RG11_EAC                       0x9272
#endif
#ifndef GL_TEXTURE_CUBE_MAP_ARRAY
#   define GL_TEXTURE_CUBE_MAP_ARRAY                    0x9009
#endif
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#	define GL_TEXTURE_MAX_ANISOTROPY_EXT                0x84FE
#endif
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#	define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT            0x84FF
#endif
#ifndef GL_GEOMETRY_SHADER
#   define GL_GEOMETRY_SHADER                           0x8DD9
#endif
#if defined(BL_PLATFORM_UWP)
#	if _DEBUG
#		define DX_CHECK_INTERNAL(call) { HRESULT _hr = call; if (FAILED(_hr)) throw Platform::Exception::CreateException(_hr); }
#	else
#		define DX_CHECK_INTERNAL(call) call;
#	endif
#else
#	if _DEBUG
#		define GL_CHECK_INTERNAL(call) {call; if (glGetError()) assert(0); }
#		define VK_CHECK_INTERNAL(call)
#	else
#		define GL_CHECK_INTERNAL(call) call;
#		define VK_CHECK_INTERNAL(call) call;
#	endif
#	if defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_OSX)
#		if _DEBUG
#			define MT_CHECK_INTERNAL(call) call;
#		else
#			define MT_CHECK_INTERNAL(call) call;
#		endif
#	endif
#endif
    extern BLBool _GbSystemRunning;
    extern BLBool _GbVideoPlaying;
    extern BLEnum _GbRenderQuality;
#ifdef __cplusplus
}
#endif

#endif/* __internal_h_ */
