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
#include "internal/thread.h"
blMutex*
blGenMutex()
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	blMutex* _ret = (blMutex*)malloc(sizeof(blMutex));
	InitializeCriticalSectionEx(&(_ret->sHandle), 0, 0);
	_ret->sCond = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	_ret->waiters = 0;
	return _ret;
#elif defined(BL_PLATFORM_WEB)
	return NULL;
#else
	blMutex* _ret = (blMutex*)malloc(sizeof(blMutex));
	pthread_mutex_init(&(_ret->sHandle), NULL);
	pthread_cond_init(&(_ret->sCond), NULL);
	_ret->waiters = 0;
	return _ret;
#endif
}
BLVoid
blDeleteMutex(INOUT blMutex* _Mu)
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (!_Mu)
		return;
	DeleteCriticalSection(&(_Mu->sHandle));
	CloseHandle(_Mu->sCond);
	free(_Mu);
	_Mu = NULL;
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	if (!_Mu)
		return;
	pthread_mutex_destroy(&(_Mu->sHandle));
	pthread_cond_destroy(&(_Mu->sCond));
	free(_Mu);
	_Mu = NULL;
#endif
}
BLVoid
blMutexLock(INOUT blMutex* _Mu)
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	EnterCriticalSection(&(_Mu->sHandle));
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	pthread_mutex_lock(&(_Mu->sHandle));
#endif
}
BLVoid
blMutexUnlock(INOUT blMutex* _Mu)
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	LeaveCriticalSection(&(_Mu->sHandle));
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	pthread_mutex_unlock(&(_Mu->sHandle));
#endif
}
BLVoid
blMutexWait(INOUT blMutex* _Mu)
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	InterlockedIncrement(&_Mu->waiters);
	LeaveCriticalSection(&(_Mu->sHandle));
	WaitForSingleObjectEx(_Mu->sCond, INFINITE, TRUE);
	InterlockedDecrement(&_Mu->waiters);
	EnterCriticalSection(&(_Mu->sHandle));
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	pthread_mutex_lock(&(_Mu->sHandle));
	pthread_cond_wait(&_Mu->sCond, &_Mu->sHandle);
	pthread_mutex_unlock(&(_Mu->sHandle));
#endif
}
BLVoid
blMutexNotify(INOUT blMutex* _Mu)
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (_Mu->waiters > 0)
		SetEvent(_Mu->sCond);
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	pthread_mutex_lock(&(_Mu->sHandle));
	pthread_cond_broadcast(&(_Mu->sCond));
	pthread_mutex_unlock(&(_Mu->sHandle));
#endif
}
BLThread*
blGenThread(IN blThreadFunc _Func, IN blWakeupFunc _Wake, IN BLVoid* _Userdata)
{
#if defined(BL_PLATFORM_WEB)
	return NULL;
#else
	BLThread* _ret = (BLThread*)malloc(sizeof(BLThread));
	_ret->bRunning = FALSE;
	_ret->sHandle = 0;
	_ret->pThreadFunc = _Func;
	_ret->pWakeupFunc = _Wake;
	_ret->pUserdata = _Userdata;
	return _ret;
#endif
}
BLVoid
blThreadRun(INOUT BLThread* _Tr)
{
	_Tr->bRunning = TRUE;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	DWORD _tid;
	_Tr->sHandle = CreateThread(NULL, 0, _Tr->pThreadFunc, (LPVOID)_Tr->pUserdata, 0, &_tid);
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	if (pthread_create(&_Tr->sHandle, NULL, _Tr->pThreadFunc, (BLVoid*)_Tr->pUserdata) != 0)
		_Tr->sHandle = 0;
#endif
}
BLVoid
blDeleteThread(INOUT BLThread* _Tr)
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (!_Tr)
		return;
	_Tr->bRunning = FALSE;
	if (_Tr->pWakeupFunc)
		_Tr->pWakeupFunc(_Tr);
	WaitForSingleObjectEx(_Tr->sHandle, INFINITE, TRUE);
	CloseHandle(_Tr->sHandle);
	_Tr->sHandle = 0;
	free(_Tr);
	_Tr = NULL;
#elif defined(BL_PLATFORM_WEB)
	return;
#else
	if (!_Tr)
		return;
	_Tr->bRunning = FALSE;
	if (_Tr->pWakeupFunc)
		_Tr->pWakeupFunc(_Tr);
	pthread_join(_Tr->sHandle, NULL);
	_Tr->sHandle = 0;
	free(_Tr);
	_Tr = NULL;
#endif	
}
BLVoid
blYield()
{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	Sleep(1);
#else
	struct timespec _ts = { 0, 1 };
	nanosleep(&_ts, NULL);
#endif
}
