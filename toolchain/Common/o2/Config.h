/****************************************************************************
 Copyright (c) 2013-2016 Marilyn Dafa
  Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef CONFIG_H_INCLUDE
#define CONFIG_H_INCLUDE

//!check platform
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#	if defined(WINAPI_FAMILY)
#		include <winapifamily.h>
#		if WINAPI_FAMILY == WINAPI_FAMILY_APP|| WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#			if WINAPI_FAMILY == WINAPI_FAMILY_APP
#				define O2D_PLATFORM_WARM
#				define O2D_USE_DX_API
#			endif
#			if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#				define O2D_PLATFORM_WPHONE
#				define O2D_USE_DX_API
#			endif
#		else
#			define O2D_PLATFORM_WINDOWS
#			define O2D_USE_DX_API
#		endif
#	else
#		define O2D_PLATFORM_WINDOWS
#		define O2D_USE_GL_API
#	endif
#elif defined(__APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define O2D_PLATFORM_IOS
#       define O2D_USE_GLES_API
#   else
#       define O2D_PLATFORM_OSX
#       define O2D_USE_GL_API
#   endif
#elif defined(ANDROID)
#	define O2D_PLATFORM_ANDROID
#	define O2D_USE_GLES_API
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define O2D_PLATFORM_LINUX
#	define O2D_USE_GL_API
#else	
#	error unsupport platform
#endif

//!check compiler
#if defined(_MSC_VER)
#	if (_MSC_VER >= 1600)
#		define O2D_COMPILER_MSVC
#	else
#		error need high compiler version
#	endif
#elif defined(__clang__)
#       define O2D_COMPILER_CLANG
#elif defined(__GNUC__)
#	if (__GNUC__  > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2))
#		define O2D_COMPILER_GCC
#	else
#		error need high compiler version
#	endif
#else
#		error unsupprt compiler
#endif

//!check cpu
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#	define O2D_ARCHITECTURE_64
#else
#	define O2D_ARCHITECTURE_32
#endif

//!check debug
#if defined(DEBUG) | defined(_DEBUG)
#	define O2D_DEBUG_MODE
#else
#	define O2D_RELEASE_MODE
#endif

#endif