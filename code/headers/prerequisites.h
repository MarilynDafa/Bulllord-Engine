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
#ifndef __prerequisiters_h_
#define __prerequisiters_h_
#if defined(EMSCRIPTEN)
#	define BL_PLATFORM_WEB	0
#elif defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#	if defined(WINAPI_FAMILY)
#		if WINAPI_FAMILY == WINAPI_FAMILY_APP|| WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#			define BL_PLATFORM_UWP      2
#		endif
#	else
#		define BL_PLATFORM_WIN32    1
#	endif
#elif defined(__APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define BL_PLATFORM_IOS  6
#   else
#       define BL_PLATFORM_OSX  5
#   endif
#elif defined(ANDROID)
#	define BL_PLATFORM_ANDROID  4
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define BL_PLATFORM_LINUX    3
#else
#	error unsupport platform
#endif
#if defined(DEBUG) | defined(_DEBUG)
#	define BL_DEBUG_MODE 1 
#else
#	define BL_DEBUG_MODE 0
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <setjmp.h>
#if defined(BL_PLATFORM_WIN32)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	pragma warning(disable: 4996)
#	pragma warning(disable: 4264)
#	pragma warning(disable: 4091)
#	include <windows.h>
#	include <process.h>
#	include <WinSock2.h>
#	include <Ws2tcpip.h>
#	include <shellapi.h>
#	include <intrin.h>
#	include <direct.h>
#	include <ShlObj.h>
#	include <time.h>
#	include <io.h>
#	include <assert.h>
#	if defined(VLD_FORCE_ENABLE)
#		include <vld.h>
#	endif
#   define BL_GL_BACKEND
#   define BL_VK_BACKEND
#elif defined(BL_PLATFORM_OSX)
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#	include <unistd.h>
#   include <netdb.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/socket.h>
#   include <sys/param.h>
#   include <sys/time.h>
#   include <sys/stat.h>
#   include <sys/sysctl.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#   include <mach/mach_time.h>
#   include <stdatomic.h>
#   include <CoreFoundation/CoreFoundation.h>
#   define BL_GL_BACKEND
#   define BL_MTL_BACKEND
#elif defined(BL_PLATFORM_LINUX)
#	include <unistd.h>
#   include <stdarg.h>
#   include <ctype.h>
#   include <fcntl.h>
#   include <locale.h>
#	include <pthread.h>
#   include <assert.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#   include <sys/stat.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#   include <sys/sysctl.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#   include <X11/Xatom.h>
#   include <errno.h>
#   define BL_GL_BACKEND
#   define BL_VK_BACKEND
#elif defined(BL_PLATFORM_IOS)
#	include <unistd.h>
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#   include <errno.h>
#   include <fcntl.h>
#   include <ctype.h>
#   include <stdarg.h>
#   include <assert.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#   include <sys/stat.h>
#   include <sys/sysctl.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#   include <mach/mach_time.h>
#   include <stdatomic.h>
#   define BL_GL_BACKEND
#   define BL_MTL_BACKEND
#elif defined(BL_PLATFORM_ANDROID)
#	include <unistd.h>
#   include <errno.h>
#   include <fcntl.h>
#   include <ctype.h>
#	include <math.h>
#   include <assert.h>
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/stat.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#   include <android/log.h>
#	include <android/native_activity.h>
#   define BL_GL_BACKEND
#   define BL_VK_BACKEND
#elif defined(BL_PLATFORM_UWP)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	pragma warning(disable: 4005)
#	include <windows.h>
#	include <WinSock2.h>
#	include <Ws2tcpip.h>
#	include <shellapi.h>
#	include <intrin.h>
#	include <direct.h>
#	include <ShlObj.h>
#	include <time.h>
#	include <io.h>
#	include <assert.h>
#	include <malloc.h>
#   define BL_DX_BACKEND
#elif defined(BL_PLATFORM_WEB)
#	include <unistd.h>
#   include <stdarg.h>
#   include <ctype.h>
#   include <fcntl.h>
#   include <locale.h>
#	include <pthread.h>
#   include <assert.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#   include <sys/stat.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#   include <errno.h>
#	include <emscripten/emscripten.h>
#	include <emscripten/threading.h>
#	include <GLFW/glfw3.h>
#   define BL_GL_BACKEND
#else
#	error "what's the fucking platform"
#endif

typedef signed char BLS8;
typedef unsigned char BLU8;
typedef signed short BLS16;
typedef unsigned short BLU16;
typedef signed int BLS32;
typedef unsigned int BLU32;
typedef float BLF32;
typedef double BLF64;
typedef char BLAnsi;
typedef unsigned char BLUtf8;
typedef unsigned short BLUtf16;
typedef unsigned char BLBool;
typedef unsigned int BLEnum;
typedef void BLVoid;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
typedef __int64 BLS64;
typedef unsigned __int64 BLU64;
typedef SOCKET BLSocket;
typedef  unsigned __int64 BLGuid;
#else
typedef long long BLS64;
typedef unsigned long long BLU64;
typedef BLS32 BLSocket;
typedef unsigned long long BLGuid;
#endif

/* BLEngine SDK Version*/
#define BL_VERSION_MAJOR 1
#define BL_VERSION_MINOR 0
#define BL_VERSION_REVISION 0
#define BL_SDK_VERSION "1.0.0"

/* Symbol Export*/
#if defined BL_API
#	undef BL_API
#endif
#if defined(WIN32) || defined(WIN64)
#	if defined(VLD_FORCE_ENABLE)
#		define BL_API extern
#	else
#		ifdef BL_EXPORT
#			define BL_API __declspec(dllexport)
#		else
#			define BL_API __declspec(dllimport)
#		endif
#	endif
#elif  defined(EMSCRIPTEN)
#	ifdef BL_EXPORT
#		define BL_API EMSCRIPTEN_KEEPALIVE 
#	else
#		define BL_API
#	endif
#else
#	ifdef BL_EXPORT
#		define BL_API __attribute__ ((visibility("default")))
#	else
#		define BL_API
#	endif
#endif

/**
 * Uniform Entry Point
 */
#if defined(BL_PLATFORM_WIN32)
#	ifdef _DEBUG
#		define BL_MAINBEGIN(argc, argv) int main(int argc, char** argv) {
#	else
#		define BL_MAINBEGIN(argc, argv) int _stdcall WinMain(HINSTANCE inst, HINSTANCE hinst, LPSTR argv, int argc) {
#	endif
#		define BL_MAINEND return 0; }
#elif defined(BL_PLATFORM_UWP)
#		define BL_MAINBEGIN(argc, argv) [Platform::MTAThread] int main(Platform::Array<Platform::String^>^ argc) {
#		define BL_MAINEND return 0; }
#elif defined(BL_PLATFORM_OSX)
#       define BL_MAINBEGIN(argc, argv) int main(int argc, const char* argv[]) {
#		define BL_MAINEND return 0; }
#elif defined(BL_PLATFORM_IOS)
#       define BL_MAINBEGIN(argc, argv) int main(int argc, const char* argv[]) {
#		define BL_MAINEND return 0; }
#elif defined(BL_PLATFORM_LINUX)
#       define BL_MAINBEGIN(argc, argv) int main(int argc, const char* argv[]) {
#		define BL_MAINEND return 0; }
#elif defined(BL_PLATFORM_ANDROID)		
#       define BL_MAINBEGIN(argc, argv) void ANativeActivity_onCreate(ANativeActivity* _Activity, void* _State, size_t _StateSize) {blSystemPrepare(_Activity, _State, _StateSize);
#		define BL_MAINEND  }
#elif defined(BL_PLATFORM_WEB)
#       define BL_MAINBEGIN(argc, argv) int main(int argc, const char* argv[]) {
#		define BL_MAINEND return 0; }
#else
#	    error "what's the fucking platform"
#endif

/**
 * Boolean Macro
 */
#ifdef TRUE
#	undef TRUE
#endif
#define TRUE 1  ///< TRUE Value
#ifdef FALSE
#	undef FALSE
#endif
#define FALSE 0 ///< FALSE Value

/**
 * Parameter Description Macro
 */
#ifdef IN
#	undef IN
#endif
#define IN const    ///<In Qualifier
#if defined OUT
#	undef OUT
#endif
#define OUT         ///<Out Qualifier
#if defined INOUT
#	undef INOUT
#endif
#define INOUT       ///<InOut Qualifier

/**
 * GUID Macro
 */
#if defined INVALID_GUID
#   undef INVALID_GUID
#endif
#define INVALID_GUID 0xFFFFFFFFFFFFFFFF         ///<Invalid Guid
#if defined ASSOCIATED_GUID
#	undef ASSOCIATED_GUID
#endif
#define ASSOCIATED_GUID 0xFFFFFFFF00000000      ///<Associated Guid

/**
 * Event Util Macro
 */
#define LOWU16(val) ((BLU16)(((BLU32)(val)) & 0xFFFF))      ///<Get Low 16bit part from a 32bit unsigned int
#define HIGU16(val) ((BLU16)((((BLU32)(val)) >> 16) & 0xFFFF))     ///<Get High 16bit part from a 32bit unsigned int
#define MAKEU32(high, low) ((BLU32)(((BLU16)(((BLU32)(low)) & 0xFFFF)) | ((BLU32)((BLU16)(((BLU32)(high)) & 0xFFFF))) << 16))       ///<Combine two short into a 32bit unsigned int

 /* _EVENT_TYPE */
#define BL_ET_SYSTEM	0
#define BL_ET_NET		1
#define BL_ET_MOUSE		2
#define BL_ET_KEY		3
#define BL_ET_UI		4
#define BL_ET_SPRITE	5
#define BL_ET_COUNT		6

 /* _SYSTEM_EVENT */
#define BL_SE_TIMER			1
#define BL_SE_RESOLUTION    2
#define BL_SE_PRELOAD		3

/* _MOUSE_EVENT */
#define BL_ME_LDOWN		0
#define BL_ME_RDOWN		1
#define BL_ME_LUP		2
#define BL_ME_RUP		3
#define BL_ME_MOVE		4
#define BL_ME_WHEEL		5

/* _KEYCODE */
#define BL_KC_UNKNOWN			0
#define BL_KC_A					4
#define BL_KC_B					5
#define BL_KC_C					6
#define BL_KC_D					7
#define BL_KC_E					8
#define BL_KC_F					9
#define BL_KC_G					10
#define BL_KC_H					11
#define BL_KC_I					12
#define BL_KC_J					13
#define BL_KC_K					14
#define BL_KC_L					15
#define BL_KC_M					16
#define BL_KC_N					17
#define BL_KC_O					18
#define BL_KC_P					19
#define BL_KC_Q					20
#define BL_KC_R					21
#define BL_KC_S					22
#define BL_KC_T					23
#define BL_KC_U					24
#define BL_KC_V					25
#define BL_KC_W					26
#define BL_KC_X					27
#define BL_KC_Y					28
#define BL_KC_Z					29
#define BL_KC_1					30
#define BL_KC_2					31
#define BL_KC_3					32
#define BL_KC_4					33
#define BL_KC_5					34
#define BL_KC_6					35
#define BL_KC_7					36
#define BL_KC_8					37
#define BL_KC_9					38
#define BL_KC_0					39
#define BL_KC_RETURN			40
#define BL_KC_ESCAPE			41
#define BL_KC_BACKSPACE			42
#define BL_KC_TAB				43
#define BL_KC_SPACE				44
#define BL_KC_MINUS				45
#define BL_KC_EQUALS			46
#define BL_KC_LEFTBRACKET		47
#define BL_KC_RIGHTBRACKET		48
#define BL_KC_BACKSLASH			49
#define BL_KC_NONUSHASH			50
#define BL_KC_SEMICOLON			51
#define BL_KC_APOSTROPHE		52
#define BL_KC_GRAVE				53
#define BL_KC_COMMA				54
#define BL_KC_PERIOD			55
#define BL_KC_SLASH				56
#define BL_KC_CAPSLOCK			57
#define BL_KC_F1				58
#define BL_KC_F2				59
#define BL_KC_F3				60
#define BL_KC_F4				61
#define BL_KC_F5				62
#define BL_KC_F6				63
#define BL_KC_F7				64
#define BL_KC_F8				65
#define BL_KC_F9				66
#define BL_KC_F10				67
#define BL_KC_F11				68
#define BL_KC_F12				69
#define BL_KC_PRINTSCREEN		70
#define BL_KC_SCROLLLOCK		71
#define BL_KC_PAUSE				72
#define BL_KC_INSERT			73
#define BL_KC_HOME				74
#define BL_KC_PAGEUP			75
#define BL_KC_DELETE			76
#define BL_KC_END				77
#define BL_KC_PAGEDOWN			78
#define BL_KC_RIGHT				79
#define BL_KC_LEFT				80
#define BL_KC_DOWN				81
#define BL_KC_UP				82
#define BL_KC_NUMLOCKCLEAR		83
#define BL_KC_KP_DIVIDE			84
#define BL_KC_KP_MULTIPLY		85
#define BL_KC_KP_MINUS			86
#define BL_KC_KP_PLUS			87
#define BL_KC_KP_ENTER			88
#define BL_KC_KP_1				89
#define BL_KC_KP_2				90
#define BL_KC_KP_3				91
#define BL_KC_KP_4				92
#define BL_KC_KP_5				93
#define BL_KC_KP_6				94
#define BL_KC_KP_7				95
#define BL_KC_KP_8				96
#define BL_KC_KP_9				97
#define BL_KC_KP_0				98
#define BL_KC_KP_PERIOD			99
#define BL_KC_NONUSBACKSLASH	100
#define BL_KC_APPLICATION		101
#define BL_KC_POWER				102
#define BL_KC_KP_EQUALS			103
#define BL_KC_F13				104
#define BL_KC_F14				105
#define BL_KC_F15				106
#define BL_KC_F16				107
#define BL_KC_F17				108
#define BL_KC_F18				109
#define BL_KC_F19				110
#define BL_KC_F20				111
#define BL_KC_F21				112
#define BL_KC_F22				113
#define BL_KC_F23				114
#define BL_KC_F24				115
#define BL_KC_EXECUTE			116
#define BL_KC_HELP				117
#define BL_KC_MENU				118
#define BL_KC_SELECT			119
#define BL_KC_STOP				120
#define BL_KC_AGAIN				121
#define BL_KC_UNDO				122
#define BL_KC_CUT				123
#define BL_KC_COPY				124
#define BL_KC_PASTE				125
#define BL_KC_FIND				126
#define BL_KC_MUTE				127
#define BL_KC_VOLUMEUP			128
#define BL_KC_VOLUMEDOWN		129
#define BL_KC_KP_COMMA			133
#define BL_KC_KP_EQUALSAS400	134
#define BL_KC_INTERNATIONAL1	135
#define BL_KC_INTERNATIONAL2	136
#define BL_KC_INTERNATIONAL3	137
#define BL_KC_INTERNATIONAL4	138
#define BL_KC_INTERNATIONAL5	139
#define BL_KC_INTERNATIONAL6	140
#define BL_KC_INTERNATIONAL7	141
#define BL_KC_INTERNATIONAL8	142
#define BL_KC_INTERNATIONAL9	143
#define BL_KC_LANG1				144
#define BL_KC_LANG2				145
#define BL_KC_LANG3				146
#define BL_KC_LANG4				147
#define BL_KC_LANG5				148
#define BL_KC_LANG6				149
#define BL_KC_LANG7				150
#define BL_KC_LANG8				151
#define BL_KC_LANG9				152
#define BL_KC_ALTERASE			153
#define BL_KC_SYSREQ			154
#define BL_KC_CANCEL			155
#define BL_KC_CLEAR				156
#define BL_KC_PRIOR				157
#define BL_KC_RETURN2			158
#define BL_KC_SEPARATOR			159
#define BL_KC_OUT				160
#define BL_KC_OPER				161
#define BL_KC_CLEARAGAIN		162
#define BL_KC_CRSEL				163
#define BL_KC_EXSEL				164
#define BL_KC_KP_00				176
#define BL_KC_KP_000			177
#define BL_KC_THOUSANDSSEPARATOR 178
#define BL_KC_DECIMALSEPARATOR	179
#define BL_KC_CURRENCYUNIT		180
#define BL_KC_CURRENCYSUBUNIT	181
#define BL_KC_KP_LEFTPAREN		182
#define BL_KC_KP_RIGHTPAREN		183
#define BL_KC_KP_LEFTBRACE		184
#define BL_KC_KP_RIGHTBRACE		185
#define BL_KC_KP_TAB			186
#define BL_KC_KP_BACKSPACE		187
#define BL_KC_KP_A				188
#define BL_KC_KP_B				189
#define BL_KC_KP_C				190
#define BL_KC_KP_D				191
#define BL_KC_KP_E				192
#define BL_KC_KP_F				193
#define BL_KC_KP_XOR			194
#define BL_KC_KP_POWER			195
#define BL_KC_KP_PERCENT		196
#define BL_KC_KP_LESS			197
#define BL_KC_KP_GREATER		198
#define BL_KC_KP_AMPERSAND		199
#define BL_KC_KP_DBLAMPERSAND	200
#define BL_KC_KP_VERTICALBAR	201
#define BL_KC_KP_DBLVERTICALBAR	202
#define BL_KC_KP_COLON			203
#define BL_KC_KP_HASH			204
#define BL_KC_KP_SPACE			205
#define BL_KC_KP_AT				206
#define BL_KC_KP_EXCLAM			207
#define BL_KC_KP_MEMSTORE		208
#define BL_KC_KP_MEMRECALL		209
#define BL_KC_KP_MEMCLEAR		210
#define BL_KC_KP_MEMADD			211
#define BL_KC_KP_MEMSUBTRACT	212
#define BL_KC_KP_MEMMULTIPLY	213
#define BL_KC_KP_MEMDIVIDE		214
#define BL_KC_KP_PLUSMINUS		215
#define BL_KC_KP_CLEAR			216
#define BL_KC_KP_CLEARENTRY		217
#define BL_KC_KP_BINARY			218
#define BL_KC_KP_OCTAL			219
#define BL_KC_KP_DECIMAL		220
#define BL_KC_KP_HEXADECIMAL	221
#define BL_KC_LCTRL				224
#define BL_KC_LSHIFT			225
#define BL_KC_LALT				226
#define BL_KC_LGUI				227
#define BL_KC_RCTRL				228
#define BL_KC_RSHIFT			229
#define BL_KC_RALT				230
#define BL_KC_RGUI				231
#define BL_KC_MODE				257
#define BL_KC_AUDIONEXT			258
#define BL_KC_AUDIOPREV			259
#define BL_KC_AUDIOSTOP			260
#define BL_KC_AUDIOPLAY			261
#define BL_KC_AUDIOMUTE			262
#define BL_KC_MEDIASELECT		263
#define BL_KC_WWW				264
#define BL_KC_MAIL				265
#define BL_KC_CALCULATOR		266
#define BL_KC_COMPUTER			267
#define BL_KC_AC_SEARCH			268
#define BL_KC_AC_HOME			269
#define BL_KC_AC_BACK			270
#define BL_KC_AC_FORWARD		271
#define BL_KC_AC_STOP			272
#define BL_KC_AC_REFRESH		273
#define BL_KC_AC_BOOKMARKS		274
#define BL_KC_BRIGHTNESSDOWN	275
#define BL_KC_BRIGHTNESSUP		276
#define BL_KC_DISPLAYSWITCH		277
#define BL_KC_KBDILLUMTOGGLE	278
#define BL_KC_KBDILLUMDOWN		279
#define BL_KC_KBDILLUMUP		280
#define BL_KC_EJECT				281
#define BL_KC_SLEEP				282
#define BL_KC_APP1				283
#define BL_KC_APP2				284
#define BL_KC_EXIT              285

/* _RENDER_API_TYPE */
#define BL_GL_API				0 //linux & OSX & Win32 & Android
#define BL_DX_API				1 //UWP
#define BL_METAL_API			2 //New OSX & IOS
#define BL_VULKAN_API			3 //Android Win32

/* _NETWORK_TYPE */
#define BL_NT_HTTP		0
#define BL_NT_TCP		1
#define BL_NT_UDP		2

/* _RENDER_QUALITY */
#define BL_RQ_ULP		0
#define BL_RQ_NORMAL	1
#define BL_RQ_EXTREME	2

/* _CULL_MODE */
#define BL_CM_NONE      0
#define BL_CM_CW        1
#define BL_CM_CCW       2

/* _COMPARISON_FUNC */
#define BL_CF_NEVER           0
#define BL_CF_LESS            1
#define BL_CF_EQUAL           2
#define BL_CF_LESSEQUAL       3
#define BL_CF_GREATER         4
#define BL_CF_NOTEQUAL        5
#define BL_CF_GREATEREQUAL    6
#define BL_CF_ALWAYS          7

/* _STENCIL_OPERATION */
#define BL_SO_KEEP            0
#define BL_SO_ZERO            1
#define BL_SO_REPLACE         2
#define BL_SO_INCRSAT         3
#define BL_SO_DECRSAT         4
#define BL_SO_INVERT          5
#define BL_SO_INCR            6
#define BL_SO_DECR            7

/* _BLEND_FACTORS */
#define BL_BF_ZERO              0
#define BL_BF_ONE               1
#define BL_BF_SRCCOLOR          2
#define BL_BF_INVSRCCOLOR       3
#define BL_BF_SRCALPHA          4
#define BL_BF_INVSRCALPHA       5
#define BL_BF_DESTCOLOR         6
#define BL_BF_INVDESTCOLOR      7
#define BL_BF_DESTALPHA         8
#define BL_BF_INVDESTALPHA      9
#define BL_BF_SRCALPHASAT       10
#define BL_BF_FACTOR            11
#define BL_BF_INVFACTOR         12

/* _BLEND_OPERATION */
#define BL_BO_ADD               0
#define BL_BO_SUBTRACT          1
#define BL_BO_REVSUBTRACT       2
#define BL_BO_MIN               3
#define BL_BO_MAX               4

/* _INDEXBUFFER_FORMAT*/
#define BL_IF_INVALID       0
#define BL_IF_16            1
#define BL_IF_32            2

/* _TEXTURE_TYPE */
#define BL_TT_1D			0
#define BL_TT_2D			1
#define BL_TT_3D			2
#define BL_TT_CUBE			3
#define BL_TT_ARRAY1D       4
#define BL_TT_ARRAY2D       5
#define BL_TT_ARRAYCUBE     6

/* _TEXTURE_FORMAT */
#define BL_TF_ASTC      1
#define BL_TF_UNKNOWN	2
#define BL_TF_R8		3
#define BL_TF_R8I		4
#define BL_TF_R8U		5
#define BL_TF_R8S		6
#define BL_TF_R32I		7
#define BL_TF_R32U		8
#define BL_TF_R32F		9	//depth
#define BL_TF_RG8		10
#define BL_TF_RG8I		11
#define BL_TF_RG8U		12
#define BL_TF_RG8S		13
#define BL_TF_RG32I		14
#define BL_TF_RG32U		15
#define BL_TF_RG32F		16	//Noise
#define BL_TF_RGB8		17
#define BL_TF_RGB8I		18
#define BL_TF_RGB8U		19
#define BL_TF_RGB8S		20
#define BL_TF_R11G11B10F	21	//Lighting accumulate
#define BL_TF_RGB9E5F	22	//hdr
#define BL_TF_BGRA8		23
#define BL_TF_RGBA8		24
#define BL_TF_RGBA8I	25
#define BL_TF_RGBA8U	26
#define BL_TF_RGBA8S	27
#define BL_TF_RGBA32I	28
#define BL_TF_RGBA32U	29
#define BL_TF_RGBA32F	30
#define BL_TF_RGB10A2	31	//Lighting accumulate
#define BL_TF_DS_UNKNOWN	32
#define BL_TF_D16		33
#define BL_TF_D32		34
#define BL_TF_D24S8		35
#define BL_TF_COUNT		36

/* _CUBE_TEXTURE_FACE */
#define BL_CTF_IGNORE		-1
#define BL_CTF_POSITIVE_X   0
#define BL_CTF_NEGATIVE_X   1
#define BL_CTF_POSITIVE_Y   2
#define BL_CTF_NEGATIVE_Y   3
#define BL_CTF_POSITIVE_Z   4
#define BL_CTF_NEGATIVE_Z   5

/* _TEXTURE_FILTER */
#define BL_TF_NEAREST       0
#define BL_TF_LINEAR        1

/* _TEXTURE_WRAPPING */
#define BL_TW_REPEAT        0
#define BL_TW_CLAMP         1
#define BL_TW_MIRROR        2

/* _TEXTURE_SWIZZLE*/
#define BL_TS_RED			0
#define BL_TS_GREEN			1
#define BL_TS_BLUE			2
#define BL_TS_ALPHA			3
#define BL_TS_ZERO			4
#define BL_TS_ONE			5

/* _PRIMITIVE_TOPOLOGY */
#define BL_PT_POINTS        0
#define BL_PT_LINES         1
#define BL_PI_LINELOOP      2
#define BL_PT_LINESTRIP     3
#define BL_PT_TRIANGLES     4
#define BL_PT_TRIANGLESTRIP 5
#define BL_PT_TRIANGLEFAN   6

/* _SEMANTIC_LAYOUT */
#define BL_SL_INVALID       -1
#define BL_SL_POSITION		0
#define BL_SL_NORMAL		1
#define BL_SL_TANGENT		2
#define BL_SL_BITANGENT		3
#define BL_SL_COLOR0		4
#define BL_SL_COLOR1		5
#define BL_SL_INDICES		6
#define BL_SL_WEIGHT		7
#define BL_SL_TEXCOORD0		8
#define BL_SL_TEXCOORD1		9
#define BL_SL_TEXCOORD2		10
#define BL_SL_TEXCOORD3		11
#define BL_SL_TEXCOORD4		12
#define BL_SL_TEXCOORD5		13
#define BL_SL_INSTANCE1     14
#define BL_SL_INSTANCE2     15

/* _VERTEX_DECL */
#define BL_VD_INVALID       -1
#define BL_VD_BOOLX1        0
#define BL_VD_BOOLX2        1
#define BL_VD_BOOLX3        2
#define BL_VD_BOOLX4        3
#define BL_VD_INTX1         4
#define BL_VD_INTX2         5
#define BL_VD_INTX3         6
#define BL_VD_INTX4         7
#define BL_VD_FLOATX1       8
#define BL_VD_FLOATX2       9
#define BL_VD_FLOATX3       10
#define BL_VD_FLOATX4       11

/* _UNIFORM_BUFFER*/
#define BL_UB_S32X1         0
#define BL_UB_S32X2         1
#define BL_UB_S32X3         2
#define BL_UB_S32X4         3
#define BL_UB_F32X1         4
#define BL_UB_F32X2         5
#define BL_UB_F32X3         6
#define BL_UB_F32X4         7
#define BL_UB_MAT2          8
#define BL_UB_MAT3          9
#define BL_UB_MAT4          10
#define BL_UB_MAT2X3        11
#define BL_UB_MAT3X2        12
#define BL_UB_MAT2X4        13
#define BL_UB_MAT4X2        14
#define BL_UB_MAT3X4        15
#define BL_UB_MAT4X3        16
#define BL_UB_SAMPLER       17

/* _UI_TYPE */
#define BL_UT_PANEL			0x0000
#define BL_UT_LABEL			0x0001
#define BL_UT_BUTTON		0x0002
#define BL_UT_CHECK			0x0003
#define BL_UT_SLIDER		0x0004
#define BL_UT_TEXT			0x0005
#define BL_UT_PROGRESS		0x0006
#define BL_UT_DIAL			0x0007
#define BL_UT_TABLE			0x0008
#define BL_UT_PRIMITIVE		0x0009
#define BL_UT_INVALID		0xFFFF

/* _UI_POLICY */
#define BL_UP_FIXED			0x0000
#define BL_UP_HMatch		0x0001
#define BL_UP_VMatch		0x0002
#define BL_UP_STRETCH		0x0003

/* _UI_ALIGNMENT */
#define BL_UA_LEFT			0x0000
#define BL_UA_RIGHT			0x0001
#define BL_UA_HCENTER		0x0002
#define BL_UA_TOP			0x0003
#define BL_UA_BOTTOM		0x0004
#define BL_UA_VCENTER		0x0005

/* IME Type */
#define BL_IT_TEXT			0
#define BL_IT_NUMERIC		1
#define BL_IT_PASSWORD		2

#endif
