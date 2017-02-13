/**************************************************************************************/
//   Copyright (C) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef PREREQUISITES_H_INCLUDED
#define PREREQUISITES_H_INCLUDED
#include "config.h"
#include <math.h>
#include <locale.h>
#include <errno.h>
#include <memory.h>
#include <stdarg.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>
#include <string.h>
#include <time.h>
#include <new>
#if defined(O2D_PLATFORM_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#	include <process.h>
#	include <WinSock2.h>
#	include <shellapi.h>
#	include <intrin.h>
#	include <eh.h>
#	include <direct.h>
#	include <io.h>
#elif defined(O2D_PLATFORM_OSX)
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
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#   include <mach/mach_time.h>
#   include <CoreFoundation/CoreFoundation.h>
#elif defined(O2D_PLATFORM_LINUX)
#	include <unistd.h>
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#elif defined(O2D_PLATFORM_IOS)
#	include <unistd.h>
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#   include <mach/mach_time.h>
#elif defined(O2D_PLATFORM_ANDROID)
#	include <unistd.h>
#	include <pthread.h>
#	include <signal.h>
#	include <sched.h>
#	include <dlfcn.h>
#   include <netdb.h>
#	include <sys/types.h>
#	include <sys/ioctl.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <arpa/inet.h>
#	include <android_native_app_glue.h>
#elif defined(O2D_PLATFORM_WPHONE)
#	include <WinSock2.h>
#else
#	error fucking platform
#endif
#ifdef O2D_COMPILER_MSVC
#	define O2D_API
#elif defined O2D_COMPILER_GCC
#	define O2D_API __attribute__ ((visibility ("default")))
#elif defined O2D_COMPILER_CLANG
#	define O2D_API __attribute__ ((visibility ("default")))
#else
#error unknow compiler
#endif
	/////////////////////////////////////////////////////////////////////
	typedef signed char s8;
	typedef unsigned char u8;
	typedef signed short s16;
	typedef unsigned short u16;
	typedef signed int s32;
	typedef unsigned int u32;
	typedef float f32;
	typedef double f64;
	typedef char ansi;
	typedef unsigned char utf8;
	typedef wchar_t utf16;
#if defined(O2D_COMPILER_MSVC)
	typedef __int64 s64;
	typedef unsigned __int64 u64;
#else
	typedef long long s64;
	typedef unsigned long long u64;
#endif
/////////////////////////////////////////////////////////////////////
enum e_logging_level
{
	LL_SYSTEM ,
	LL_INFORMATION,
	LL_WARNING,
	LL_ERROR
};
enum e_console_text_color
{
	CTC_RED,
	CTC_SILVER,
	CTC_GREEN,
	CTC_BLUE
};	
enum e_clear_mask
{
	CM_COLOR = 1<<0, 
	CM_DEPTH = 1<<1, 
	CM_STENCIL = 1<<2
};
enum e_plane_side
{
	PS_NO_SIDE,
	PS_POSITIVE_SIDE,
	PS_NEGATIVE_SIDE,
	PS_BOTH_SIDE
};
enum e_index_format
{
	IF_16UI , 
	IF_32UI
};
enum e_frustum_plane
{
	FP_NEAR = 0 , 
	FP_FAR = 1 , 
	FP_LEFT = 2 , 
	FP_RIGHT = 3 ,
	FP_TOP = 4 ,
	FP_BOTTOM = 5 , 
	FP_NUM = 6
};
enum e_visibility_state
{
	VS_NONE , 
	VS_FULL , 
	VS_PARTIAL
};
enum e_socket_status
{
	SS_DONE , 
	SS_NOT_READY , 
	SS_DISCONNECTED , 
	SS_ERROR
};
enum e_socket_type
{
	ST_UDP , 
	ST_TCP
};
enum e_network_priority
{
	NP_CRITICAL , 
	NP_NORMAL ,
	NP_INSIGNIFICANT
};
enum e_http_state
{
	HS_IDLE , 
	HS_REQ_STARTED , 
	HS_REQ_SENT
};
enum e_http_struct
{
	HS_STATUSLINE,
	HS_HEADERS,
	HS_BODY,
	HS_CHUNKLEN,
	HS_CHUNKEND,
	HS_TRAILERS,
	HS_COMPLETE
};
enum e_console_result
{
	CR_VOID_CMD , 
	CR_PRINT_HELP , 
	CR_CLEAN_SCREEN , 
	CR_INVALID_CMD , 
	CR_COMMAND_FAIL , 
	CR_COMMAND_SUC
};
enum e_resource_state
{
	RS_UNSET , 
	RS_LOADED , 
	RS_SETUPED
};
enum e_resource_type
{
	RT_INVALID , 
	RT_SOUND , 
	RT_TEXTURE , 
	RT_SHADER ,
	RT_UI , 
	RT_SPRITE
};
enum e_buildin_res_id
{
	BRI_UI_GLFX = 0, 
	BRI_SPRITE_GLFX , 
	BRI_OUTLINE_GLFX , 
	BRI_BLANK_GLFX , 
	BRI_GRAY_GLFX , 
	BRI_ATMO_GLFX , 
	BRI_BLINK_GLFX , 
	BRI_METEORO_GLFX , 
	BRI_RAIN_TEX , 
	BRI_SNOW_TEX
};
enum e_brush_type
{
	BT_ROAD = 0,
	BT_OBSTACLE = 1,
	BT_WATER = 2,
	BT_MUD = 3
};
enum e_render_state
{
	RS_DEPTH_ENABLE = 0, 
	RS_DEPTH_FUNC , 
	RS_BLEND_ENABLE , 
	RS_BLEND_OP , 
	RS_BLEND_OP_ALPHA , 
	RS_SRC_FACTOR , 
	RS_DST_FACTOR , 
	RS_SRC_ALPHA_FACTOR , 
	RS_DST_ALPHA_FACTOR , 
	RS_NUM
};
enum e_vertex_element_type
{
	VET_FLOAT1 = 0,
	VET_FLOAT2 = 1,
	VET_FLOAT3 = 2,
	VET_FLOAT4 = 3,
	VET_INTEGER1 = 4,
	VET_INTEGER2 = 5,
	VET_INTEGER3 = 6,
	VET_INTEGER4 = 7,
};
enum e_uniform_type
{
	UT_INTEGER1 , 
	UT_INTEGER1_ARRAY , 
	UT_INTEGER2 , 
	UT_INTEGER2_ARRAY , 
	UT_INTEGER3 , 
	UT_INTEGER3_ARRAY , 
	UT_INTEGER4 , 
	UT_INTEGER4_ARRAY , 
	UT_FLOAT1 , 
	UT_FLOAT1_ARRAY , 
	UT_FLOAT2 , 
	UT_FLOAT2_ARRAY , 
	UT_FLOAT3 , 
	UT_FLOAT3_ARRAY , 
	UT_FLOAT4 , 
	UT_FLOAT4_ARRAY , 
	UT_MATRIX2X2 , 
	UT_MATRIX2X2_ARRAY ,
	UT_MATRIX3X3, 
	UT_MATRIX3X3_ARRAY , 
	UT_MATRIX4X4 , 
	UT_MATRIX4X4_ARRAY
};	
enum e_vertex_element_semantic
{
	VES_POSITION = 1, 
	VES_DIFFUSE = 2,
	VES_TEXTURE_COORD = 3
};
enum e_sprite_animation_type
{
	SAT_NONE,
	SAT_LOOP,
	SAT_ONESHOT
};
enum e_widget_type
{
	WT_WIDGET ,
	WT_LABEL , 
	WT_BUTTON , 
	WT_RADIO , 
	WT_CARTOONBOX , 
	WT_EDITBOX , 
	WT_MENU , 
	WT_PROGRESSBAR , 
	WT_SLIDER , 
	WT_SCROLLBOX , 
	WT_DRAGDROPSLOT ,
	WT_TABCTL ,
	WT_LISTBOX , 
	WT_TABLEBOX , 
	WT_DIALOG , 
	WT_RICHBOX , 
	WT_TREEBOX
};
enum e_text_alignment
{
	TA_H_LEFT = 1<<0,
	TA_H_RIGHT = 1<<1,
	TA_H_CENTER = 1<<2,
	TA_V_TOP = 1<<3,  
	TA_V_BOTTOM = 1<<4,
	TA_V_CENTER = 1<<5 
};
enum e_tooltip_state
{
	TS_SHOW , 
	TS_HIDE , 
	TS_FADEIN
};
enum e_uniform_ui_color
{
	UUC_EDIT_HIGHLIGHT = 0 , 
	UUC_EDIT_CURSOR = 1 ,
	UUC_MENU_SEPARATOR = 2 ,
	UUC_MENU_SELECTING = 3 , 
	UUC_TABLE_SEPARATOR = 4 ,
	UUC_TABLE_SELECTING = 5 ,
	UUC_TABLE_HIGHLIGHT = 6 , 
	UUC_LIST_SELECTING = 7 , 
	UUC_LIST_HIGHLIGHT = 8 , 
	UUC_TREE_LINE = 9 ,
	UUC_TREE_SELECTING = 10 ,
	UUC_TXT_DISABLE_CLR = 11 ,
	SDT_COUNT = 12
};
enum e_particle_emitter
{
	PE_RECTANGLE , 
	PE_CIRCLE , 
	PE_RING , 
	PE_POINT
};
enum e_emitter_param
{
	EP_VALUE1 =0, 
	EP_VALUE2 ,
	EP_NUM
};
enum e_input_event
{
	IE_NOTHING = 1<<0 ,
	IE_TOUCH_FINISH = 1<<1 , 
	IE_TOUCH_MOVE = 1<<2 ,
	IE_TOUCH_START = 1<<3 ,
	IE_TOUCH_DOUBLE = 1<<4 , 
	IE_PAN = 1<<5 ,
	IE_CHAR = 1<<6
};
enum e_ui_chunk
{
	UC_LT =0, 
	UC_T , 
	UC_RT , 
	UC_L , 
	UC_MID , 
	UC_R , 
	UC_LB , 
	UC_B , 
	UC_RB , 
	UC_NUM
};
enum e_compare_function
{
	CF_ALWAYS_FAIL,
	CF_ALWAYS_PASS,
	CF_LESS,
	CF_LESS_EQUAL,
	CF_EQUAL,
	CF_NOT_EQUAL,
	CF_GREATER_EQUAL,
	CF_GREATER
};
enum e_blend_factor
{
	BF_ONE , 
	BF_ZERO , 
	BF_DEST_COLOR , 
	BF_SRC_COLOR , 
	BF_ONE_MINUS_DEST_COLOR , 
	BF_ONE_MINUS_SRC_COLOR , 
	BF_DEST_ALPHA , 
	BF_SRC_ALPHA , 
	BF_ONE_MINUS_DEST_ALPHA , 
	BF_ONE_MINUS_SRC_ALPHA
};
enum e_blend_operation
{
	BO_ADD ,
	BO_SUBTRACT ,
	BO_REVERSE_SUBTRACT
};
enum e_filter_options
{
	FO_POINT ,
	FO_LINEAR ,
	FO_TRILINEAR
};
enum e_render_operation
{
	RO_POINTS , 
	RO_LINES , 
	RO_LINE_LOOP , 
	RO_LINE_STRIP ,
	RO_TRIANGLES , 
	RO_TRIANGLE_STRIP ,
	RO_TRIANGLE_FAN 
};
enum e_buffer_usage
{
	BU_STATIC_DRAW , 
	BU_STREAM_DRAW , 
	BU_DYNAMIC_DRAW
};
enum e_texture_support
{
	TS_RGBA = 1<<0 , 
	TS_S3TC = 1<<1 ,
	TS_ATITC = 1<<2 , 
	TS_PVRTC = 1<<3 , 
	TS_ETC1 = 1<<4
};
enum e_uiskin_chunk_id
{
	UCID_HEADER = 0xDEAD , 
	UCID_DESC = 0x5770 ,
	UCID_FRAGMENT = 0x6480 ,
	UCID_TEXTURE = 0x6481
};
enum e_font_chunk_id
{
	FCID_HEADER = 0xDEAD , 
	FCID_SOURCE_PRO = 0x8745 , 
	FCID_ANTI_PRO = 0x8746 , 
	FCID_OUTLINE_PRO = 0x8747 ,
	FCID_CODERANGE = 0x8748 , 
	FCID_FNT_IMAGE = 0x8949 
};
enum e_function_keycode
{
	FC_BACKSPACE = 0x08 ,
	FC_COPY = 0x03 , 
	FC_CUT = 0x18 , 
	FC_PASTE = 0x16 , 
	FC_RETURN = 0x0D
};
enum e_quadtree_quadrant
{
	QQ_NE = 0 ,
	QQ_NW = 1 ,
	QQ_SW = 2 ,
	QQ_SE = 3
};
enum e_meteorological_type
{
	MT_SUNNY , 
	MT_RAIN , 
	MT_SNOW
};
/////////////////////////////////////////////////////////////////////
static const s32 MAX_SEARCH_PER_STEP = 16;
static const s32 MAX_COLOR_ATTACHMENTS = 16;
static const s32 GT_NET_BLOCK_TIME = 1000*150;
static const s32 MAX_RAND = 0x7fff;
static const u32 NPOS = (u32)(-1);
static const u32 OUTLINE_TEX_SIZE = 128U;
static const u32 MAX_VATTRIB_SIZE = 16;
static const u32 MAX_SAFE_U32_VALUE = 0xffffffff / 10 - 10;
static const u32 FILENAME_MAX_LENGTH = 512;
static const u32 CRMASK_CR6TRUE = 0x00000080;
static const u32 CRMASK_CR6FALSE = 0x00000020;
static const u32 GL_INVALID_BUFFER = 0xFFFFFFFF;
static const u32 NUM_AUDIO_CACHE_BUFFERS = 2;
static const u32 FONT_TEXTURE_SIZE = 256;
static const u32 SOUND_BUFFER_SIZE = 65536;
static const u32 NT_RETRY_TIMES = 20;
static const u32 INTER_GLYPH_PAD_SPACE = 2;
static const u32 INVALID_NODE_INDEX = -1;
static const u32 MAX_CACHE_CP = 32;
static const f32 EPSILON = 0.00001f;
static const f32 PI = 3.141592653589793f;
static const f32 R_PI = 0.318309886183791f;
static const f32 HALF_PI = 1.570796326794897f;
static const f32 ONEFOURTH_PI = 0.7853981633974483f;
static const f32 TWO_PI = 6.283185307179586f;
static const f32 R_TWO_PI = 0.159154943091895f;	
static const f32 DEG_2_RAD = PI / 180.0f;
static const f32 RAD_2_DEG = 180.0f / PI;	
static const f32 MAX_FLOAT = 3.402823466e+38F;
static const f32 FT_POS_COEF = 1.f/64.f;
static const f32 FT_PT_SCALE = 72.f / 96.f;
static const f32 QUADLAYER_VIS_SCALE = 1.2f;
static const s64 TICKS_FROM_1601_TO_1900 = 94354848000000000LL;

#define LOWU16(val) ((u16)(((u32)(val)) & 0xFFFF))      ///<Get Low 16bit part from a 32bit unsigned int
#define HIGU16(val) ((u16)((((u32)(val)) >> 16) & 0xFFFF))     ///<Get High 16bit part from a 32bit unsigned int
#define MAKEU32(high, low) ((u32)(((u16)(((u32)(low)) & 0xFFFF)) | ((u32)((u16)(((u32)(high)) & 0xFFFF))) << 16))       ///<Combine two short into a 32bit unsigned int

#endif 
