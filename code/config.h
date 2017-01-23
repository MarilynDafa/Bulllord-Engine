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
#ifndef __config_h_
#define __config_h_

//BLEngine SDK Version
#define BL_VERSION_MAJOR 1
#define BL_VERSION_MINOR 0
#define BL_VERSION_REVISION 0
#define BL_SDK_VERSION "1.0.0"

//BL_PLATFORM_WIN32
//BL_PLATFORM_UWP
//BL_PLATFORM_LINUX
//BL_PLATFORM_ANDROID
//BL_PLATFORM_OSX
//BL_PLATFORM_IOS
//BL_PLATFORM_WEB
//BL_PLATFORM_CONSOLE
//BL_PLATFORM_TV

#if defined(__EMSCRIPTEN__)
#	define BL_PLATFORM_WEB
#elif defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#	if defined(WINAPI_FAMILY)
#		if WINAPI_FAMILY == WINAPI_FAMILY_APP|| WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#			define BL_PLATFORM_UWP
#		endif
#	else
#		define BL_PLATFORM_WIN32
#	endif
#elif defined(__APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define BL_PLATFORM_IOS
#   else
#       define BL_PLATFORM_OSX
#   endif
#elif defined(ANDROID)
#	define BL_PLATFORM_ANDROID
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define BL_PLATFORM_LINUX
#else	
#	error unsupport platform
#endif

#if defined(DEBUG) | defined(_DEBUG)
#	define BL_DEBUG_MODE
#else
#	define BL_RELEASE_MODE
#endif

#endif/* __config_h_ */
