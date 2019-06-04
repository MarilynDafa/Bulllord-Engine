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
#ifndef __thread_h_
#define __thread_h_
#include "../../headers/prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Mutex{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	CRITICAL_SECTION sHandle;
	HANDLE sCond;
#else
	pthread_mutex_t sHandle;
	pthread_cond_t sCond;
#endif
	long waiters;
}blMutex;

#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	typedef DWORD(__stdcall *blThreadFunc)(BLVoid*);
	typedef BLVoid (*blWakeupFunc)(BLVoid*);
#else
	typedef BLVoid*(*blThreadFunc)(BLVoid*);
	typedef BLVoid(*blWakeupFunc)(BLVoid*);
#endif

typedef struct _Thread{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	HANDLE sHandle;
#else
	pthread_t sHandle;
#endif
	blThreadFunc pThreadFunc;
	blWakeupFunc pWakeupFunc;
	const BLVoid* pUserdata;
	BLBool bRunning;
}BLThread;

blMutex* blGenMutex(void);

BLVoid blDeleteMutex(
	INOUT blMutex* _Mu);

BLVoid blMutexLock(
	INOUT blMutex* _Mu);

BLVoid blMutexUnlock(
	INOUT blMutex* _Mu);

BLVoid blMutexWait(
	INOUT blMutex* _Mu);

BLVoid blMutexNotify(
	INOUT blMutex* _Mu);

BLThread* blGenThread(
	IN blThreadFunc _Func, 
	IN blWakeupFunc _Wake, 
	IN BLVoid* _Userdata);

BLVoid blThreadRun(
	INOUT BLThread* _Tr);

BLVoid blDeleteThread(
	INOUT BLThread* _Tr);

BLVoid blYield(void);

#ifdef __cplusplus
}
#endif

#endif/* __thread_h_ */
