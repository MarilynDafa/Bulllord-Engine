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
#include "../headers/streamio.h"
#include "../headers/system.h"
#include "../headers/util.h"
#include "../externals/miniz/miniz.h"
#include "internal/dictionary.h"
#include "internal/array.h"
#include "internal/list.h"
#include "internal/thread.h"
#include "internal/internal.h"
#if defined BL_PLATFORM_ANDROID
#	include <android/asset_manager.h>
#elif defined(BL_PLATFORM_WEB)
#	include <emscripten/fetch.h>
#endif
#include "../externals/duktape/duktape.h"
#pragma pack(1)
typedef struct _BpkFileHeader{
    BLU32 nFourCC;
    BLU32 nVersion;
    BLU32 nFileNum;
    BLU32 nEntryOffset;
    BLU32 nContextOffset;
}_BLBpkFileHeader;
typedef struct _BpkFileEntry{
    BLU32 nHashname;
    BLU32 nOffset;
    BLU32 nLength;
    BLAnsi aMd5[32];
	BLAnsi* pPatch;
}_BLBpkFileEntry;
typedef struct _PatchHeader{
    BLU32 nVersion;
    BLU32 nDestVersion;
    BLU32 nAddListOffset;
    BLU32 nAddListNum;
    BLU32 nModListOffset;
    BLU32 nModListNum;
    BLU32 nDelListOffset;
    BLU32 nDelListNum;
}_BLPatchHeader;
typedef struct _PatchEntry{
    BLU32 nHashname;
    BLU32 nOffset;
    BLU32 nComSize;
    BLAnsi aMd5[32];
}_BLPatchEntry;
#pragma pack()
typedef struct _BpkArchive{
    BLDictionary* pFiles;
    BLAnsi* pPath;
    BLAnsi* pArchive;
    BLU32 nVersion;
    BLU32 nOffset;
}_BLBpkArchive;
typedef struct _ResNode {
	BLVoid* pRes;
	BLAnsi aFilename[260];
	BLBool(*fLoad)(BLVoid*, const BLAnsi*);
	BLBool(*fSetup)(BLVoid*);
	BLGuid nGuid;
}_BLResNode;
typedef struct _Stream {
	BLVoid* pBuffer;
	BLU32 nLen;
	BLU32 nID;
	BLU8* pPos;
	BLU8* pEnd;
}_BLStream;
typedef struct _StreamIOMember {
	DUK_CONTEXT* pDukContext;
	BLVoid* pAndroidAM;
	BLArray* pArchives;
	BLS32 nSqlRef;
	BLBool bSqlFinish;
	BLThread* pLoadThread;
	BLList* pLoadingQueue;
	BLList* pSetupQueue;
	BLList* pPreloadQueue;
	BLU32 nPreloadNum;
}_BLStreamIOMember;
static _BLStreamIOMember* _PrStreamIOMem = NULL;
extern BLBool _FontFace(const BLAnsi* _Filename);
#if defined(BL_PLATFORM_WEB)
BLVoid
_WriteSuccess(emscripten_fetch_t* _Fetch)
{
	emscripten_fetch_close(_Fetch);
}
BLVoid _WriteFailure(emscripten_fetch_t* _Fetch)
{
	emscripten_fetch_close(_Fetch);
}
static BLVoid 
_OnWGetLoaded(const BLAnsi* _Filename)
{
	FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pLoadingQueue)
	{
		if (!strcmp(_iter->aFilename, _Filename + 1))
		{
			BLBool _ret = _iter->fLoad(_iter->pRes, _iter->aFilename);
			if (_ret)
			{
				blListPushBack(_PrStreamIOMem->pSetupQueue, _iter);
				_iterator_iter = blListErase(_PrStreamIOMem->pLoadingQueue, _iterator_iter);
			}
			else
			{
				_iterator_iter = blListErase(_PrStreamIOMem->pLoadingQueue, _iterator_iter);
				free(_iter);
			}			
		}
	}
}
static BLVoid
_OnWGetError(const BLAnsi* _Filename)
{
	blDebugOutput("%s wget error", _Filename);
	FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pLoadingQueue)
	{
		if (!strcmp(_iter->aFilename, _Filename + 1))
		{			
			blListErase(_PrStreamIOMem->pLoadingQueue, _iterator_iter);
			free(_iter);
			break;
		}
	}
}
#endif
BLBool
_FetchResource(const BLAnsi* _Filename, BLVoid** _Res, BLGuid _ID, BLBool(*_Load)(BLVoid*, const BLAnsi*), BLBool(*_Setup)(BLVoid*), BLBool _Async)
{
	_BLResNode* _node = NULL;
	{
		blMutexLock(_PrStreamIOMem->pLoadingQueue->pMutex);
		{
			FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pLoadingQueue)
			{
				if (_iter->nGuid == _ID)
				{
					blMutexUnlock(_PrStreamIOMem->pLoadingQueue->pMutex);
					return FALSE;
				}
			}
		}
		blMutexUnlock(_PrStreamIOMem->pLoadingQueue->pMutex);
		blMutexLock(_PrStreamIOMem->pSetupQueue->pMutex);
		{
			FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pSetupQueue)
			{
				if (_iter->nGuid == _ID)
				{
					blMutexUnlock(_PrStreamIOMem->pSetupQueue->pMutex);
					return FALSE;
				}
			}
		}
		blMutexUnlock(_PrStreamIOMem->pSetupQueue->pMutex);
		if (!_Async)
		{
			_Load(*_Res, _Filename);
			_Setup(*_Res);
			return TRUE;
		}
		else
		{
#if defined(BL_PLATFORM_WEB)
			BLAnsi _tmp[260] = { 0 };
			strcpy(_tmp, blWorkingDir());
			strcat(_tmp, _Filename);
			if (access(_tmp, 0) == -1)
			{
				BLBool _needwget = TRUE;
				FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pLoadingQueue)
				{
					if (!strcmp(_iter->aFilename, _Filename))
					{
						_needwget = FALSE;
						break;
					}	
				}
				if (_needwget)
					emscripten_async_wget(_tmp, _tmp, _OnWGetLoaded, _OnWGetError);
			}
#endif
			_node = (_BLResNode*)malloc(sizeof(_BLResNode));
			memset(_node->aFilename, 0, sizeof(_node->aFilename));
			strcpy(_node->aFilename, _Filename);
            _node->pRes = _Res ? *_Res : NULL;
			_node->fSetup = _Setup;
			_node->fLoad = _Load;
			_node->nGuid = _ID;
			blMutexLock(_PrStreamIOMem->pLoadingQueue->pMutex);
			blListPushBack(_PrStreamIOMem->pLoadingQueue, _node);
			blMutexUnlock(_PrStreamIOMem->pLoadingQueue->pMutex);
			return FALSE;
		}
	}
}
BLBool
_DiscardResource(BLGuid _ID, BLBool(*_Unload)(BLVoid*), BLBool(*_Release)(BLVoid*))
{
	blMutexLock(_PrStreamIOMem->pLoadingQueue->pMutex);
	{
		FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pLoadingQueue)
		{
			if (_iter->nGuid == _ID)
			{
				free(_iter);
				blListErase(_PrStreamIOMem->pLoadingQueue, _iterator_iter);
				blMutexUnlock(_PrStreamIOMem->pLoadingQueue->pMutex);
				return TRUE;
			}
		}
	}
	blMutexUnlock(_PrStreamIOMem->pLoadingQueue->pMutex);
	blMutexLock(_PrStreamIOMem->pSetupQueue->pMutex);
	{
		FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pSetupQueue)
		{
			if (_iter->nGuid == _ID)
			{
				_Unload(_iter->pRes);
				free(_iter);
				blListErase(_PrStreamIOMem->pSetupQueue, _iterator_iter);
				blMutexUnlock(_PrStreamIOMem->pSetupQueue->pMutex);
				return TRUE;
			}
		}
	}
	blMutexUnlock(_PrStreamIOMem->pSetupQueue->pMutex);
	BLVoid* _node = blGuidAsPointer(_ID);
	if (_node)
	{
		_Release(_node);
		_Unload(_node);
	}
	return TRUE;
}
static BLVoid
_PreloadFile(const BLAnsi* _Filename)
{
	if (blUtf8Equal(blFileSuffixUtf8((const BLUtf8*)_Filename), (const BLUtf8*)"ttf"))
		_FontFace(_Filename);
	else if (blUtf8Equal(blFileSuffixUtf8((const BLUtf8*)_Filename), (const BLUtf8*)"ttc"))
		_FontFace(_Filename);
	else if (blUtf8Equal(blFileSuffixUtf8((const BLUtf8*)_Filename), (const BLUtf8*)"fnt"))
		_FontFace(_Filename);
}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_LoadThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_LoadThreadFunc(BLVoid* _Userdata)
#endif
{
	do
	{
		while (_PrStreamIOMem->pLoadingQueue->nSize)
		{
			blMutexLock(_PrStreamIOMem->pLoadingQueue->pMutex);
			_BLResNode* _res = (_BLResNode*)blListFrontElement(_PrStreamIOMem->pLoadingQueue);
			BLBool _ret = _res->fLoad(_res->pRes, _res->aFilename);
			blListPopFront(_PrStreamIOMem->pLoadingQueue);
			blMutexUnlock(_PrStreamIOMem->pLoadingQueue->pMutex);
			if (_ret)
			{
				blMutexLock(_PrStreamIOMem->pSetupQueue->pMutex);
				blListPushBack(_PrStreamIOMem->pSetupQueue, _res);
				blMutexUnlock(_PrStreamIOMem->pSetupQueue->pMutex);
			}
			else
				free(_res);
			blYield();
		}
		blYield();
	} while (_PrStreamIOMem->pLoadThread->bRunning);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
}
BLVoid
_StreamIOInit(DUK_CONTEXT* _DKC, BLVoid* _AssetMgr)
{
	_PrStreamIOMem = (_BLStreamIOMember*)malloc(sizeof(_BLStreamIOMember));
	_PrStreamIOMem->pDukContext = _DKC;
	_PrStreamIOMem->pAndroidAM = _AssetMgr;
	_PrStreamIOMem->pArchives = blGenArray(FALSE);
	_PrStreamIOMem->pLoadingQueue = blGenList(TRUE);
	_PrStreamIOMem->pSetupQueue = blGenList(TRUE);
	_PrStreamIOMem->pPreloadQueue = blGenList(TRUE);
	_PrStreamIOMem->nPreloadNum = 0;
#if !defined(BL_PLATFORM_WEB)
	_PrStreamIOMem->pLoadThread = blGenThread(_LoadThreadFunc, NULL, NULL);
	blThreadRun(_PrStreamIOMem->pLoadThread);
#endif
	blArchiveRegist("localdata0079.bpk", "localdata0079");
	blArchiveRegist("remotedata0084.bpk", "remotedata0084");
	BLAnsi _path[260];
	strcpy(_path, blUserFolderDir());
	BLAnsi** _pats = NULL;
	BLU32 _patnum = 0;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	WIN32_FIND_DATA _filedata;
	HANDLE _filelist;
	strcat(_path, "*.pat");
	WCHAR _wpath[260] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wpath, sizeof(_wpath));
	_filelist = FindFirstFileW(_wpath, &_filedata);
	do
	{
		if (_filedata.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			_patnum++;
			_pats = (BLAnsi**)realloc(_pats, _patnum * sizeof(BLAnsi*));
			_pats[_patnum - 1] = (BLAnsi*)alloca(wcslen(_filedata.cFileName) + 1);
			BLUtf8 _tmp[1024] = { 0 };
			WideCharToMultiByte(CP_UTF8, 0, _filedata.cFileName, -1, (LPSTR)_tmp, 1024, NULL, NULL);
			strcpy(_pats[_patnum - 1], (const BLAnsi*)_tmp);
			_pats[_patnum - 1][wcslen(_filedata.cFileName)] = 0;
		}
	} while (FindNextFile(_filelist, &_filedata));
#else
#endif
	for (BLU32 _idx = 0; _idx < _patnum; ++_idx)
	{
		for (BLU32 _jdx = 0; _idx + _jdx < _patnum - 1; ++_jdx)
		{
			if (blNatCompare(_pats[_jdx], _pats[_jdx + 1]) > 0)
			{
				BLAnsi* _temp = _pats[_jdx];
				_pats[_jdx] = _pats[_jdx + 1];
				_pats[_jdx + 1] = _temp;
			}
		}
	}
	for (BLU32 _idx = 0; _idx < _patnum; ++_idx)
	{
		memset(_path, 0, sizeof(_path));
		strcpy(_path, blUserFolderDir());
		strcat(_path, _pats[_idx]);
		if (!strncmp(_pats[_idx], "local", 5))
			blArchivePatch(_path, "localdata0079");
		else if (!strncmp(_pats[_idx], "remot", 5))
			blArchivePatch(_path, "remotedata0084");
	}
	if (_pats)
		free(_pats);
	blDebugOutput("IO initialize successfully");
}
BLVoid
_StreamIOStep(BLU32 _Delta)
{
	if (_PrStreamIOMem->pPreloadQueue->nSize)
	{
		BLAnsi* _node = (BLAnsi*)blListFrontElement(_PrStreamIOMem->pPreloadQueue);
#if defined(BL_PLATFORM_WEB)
		BLAnsi _absfile[260] = { 0 };
		strcpy(_absfile, blWorkingDir());
		strcat(_absfile, _node);
		if (access(_absfile, 0) == -1)
			emscripten_wget(_absfile, _absfile);
#endif
		_PreloadFile(_node);
		blInvokeEvent(BL_ET_SYSTEM, BL_SE_PRELOAD, (_PrStreamIOMem->nPreloadNum - _PrStreamIOMem->pPreloadQueue->nSize - 1) * 100 / _PrStreamIOMem->nPreloadNum, _node, INVALID_GUID);
		blListPopFront(_PrStreamIOMem->pPreloadQueue);
		free(_node);
	}
	_BLResNode* _res;
	blMutexLock(_PrStreamIOMem->pSetupQueue->pMutex);
	if (_PrStreamIOMem->pSetupQueue->nSize)
	{
		_res = (_BLResNode*)blListFrontElement(_PrStreamIOMem->pSetupQueue);
		_res->fSetup(_res->pRes);
		blListPopFront(_PrStreamIOMem->pSetupQueue);
		free(_res);
	}
	blMutexUnlock(_PrStreamIOMem->pSetupQueue->pMutex);
}
BLVoid
_StreamIODestroy()
{
	{
		FOREACH_ARRAY(_BLBpkArchive*, _iter, _PrStreamIOMem->pArchives)
		{
			FOREACH_DICT(_BLBpkFileEntry*, _iter2, _iter->pFiles)
			{
				if (_iter2->pPatch)
					free(_iter2->pPatch);
				free(_iter2);
			}
			free(_iter->pArchive);
			free(_iter->pPath);
			blDeleteDict(_iter->pFiles);
			free(_iter);
		}
	}
	blDeleteThread(_PrStreamIOMem->pLoadThread);
	{
		FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pLoadingQueue)
		{
			free(_iter);
		}
	}
	{
		FOREACH_LIST(_BLResNode*, _iter, _PrStreamIOMem->pSetupQueue)
		{
			free(_iter);
		}
	}
	blDeleteList(_PrStreamIOMem->pPreloadQueue);
	blDeleteList(_PrStreamIOMem->pSetupQueue);
	blDeleteList(_PrStreamIOMem->pLoadingQueue);
	blDeleteArray(_PrStreamIOMem->pArchives);
	blDebugOutput("IO shutdown");
	free(_PrStreamIOMem);
}
BLGuid
blGenStream(IN BLAnsi* _Filename)
{
	if (_Filename)
	{
		BLU32 _id, _i;
		BLAnsi _tmpname[260];
		_BLStream* _ret = NULL;
		_BLBpkFileEntry* _file = NULL;
		FOREACH_ARRAY(_BLBpkArchive*, _iter, _PrStreamIOMem->pArchives)
		{
			memset(_tmpname, 0, sizeof(_tmpname));
			strcpy(_tmpname, _Filename);
			BLU32 _tmplen = (BLU32)strlen(_tmpname);
			for (_i = 0; _i < _tmplen; ++_i)
			{
				if (_tmpname[_i] == '\\')
					_tmpname[_i] = '/';
			}
			_id = blHashString((const BLUtf8*)_tmpname);
			_file = (_BLBpkFileEntry*)blDictElement(_iter->pFiles, _id);
			if (_file)
			{
				mz_ulong _sz;
				BLS32 _uncmpsz;
				BLU8* _cdata;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
				WCHAR _wfilename[260] = { 0 };
				MultiByteToWideChar(CP_UTF8, 0, _file->pPatch ? _file->pPatch : _iter->pPath, -1, _wfilename, sizeof(_wfilename));
				HANDLE _fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
				HANDLE _fp = CreateFileA(_file->pPatch ? _file->pPatch : _iter->pPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
				LARGE_INTEGER _li;
				_li.LowPart = _file->nOffset;
				_li.HighPart = 0;
				SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
				ReadFile(_fp, &_uncmpsz, sizeof(BLU32), NULL, NULL);
				_cdata = (BLU8*)malloc(_file->nLength);
				ReadFile(_fp, _cdata, _file->nLength, NULL, NULL);
				CloseHandle(_fp);
#elif defined(BL_PLATFORM_ANDROID)
				AAsset* _fp = AAssetManager_open(_PrStreamIOMem->pAndroidAM, _file->pPatch ? _file->pPatch : _iter->pPath, AASSET_MODE_UNKNOWN);
				assert(_fp != NULL);
				AAsset_seek(_fp, _file->nOffset, SEEK_SET);
				AAsset_read(_fp, &_uncmpsz, sizeof(BLU32) * 1);
				_cdata = (BLU8*)malloc(_file->nLength);
				AAsset_read(_fp, _cdata, sizeof(BLU8) * _file->nLength);
				AAsset_close(_fp);
#else
				FILE* _fp = fopen(_file->pPatch ? _file->pPatch : _iter->pPath, "rb");
				assert(_fp != NULL);
				fseek(_fp, _file->nOffset, SEEK_SET);
				fread(&_uncmpsz, sizeof(BLU32), 1, _fp);
				_cdata = (BLU8*)malloc(_file->nLength);
				fread(_cdata, sizeof(BLU8), _file->nLength, _fp);
				fclose(_fp);
#endif
				_ret = (_BLStream*)malloc(sizeof(_BLStream));
				_ret->pBuffer = malloc(_uncmpsz);
				_ret->nLen = _uncmpsz;
				_ret->pPos = (BLU8*)_ret->pBuffer;
				_ret->pEnd = _ret->pPos + _ret->nLen;
				_sz = (mz_ulong)_uncmpsz;
				mz_uncompress((BLU8*)_ret->pBuffer, &_sz, _cdata, _file->nLength);
				free(_cdata);
				return blGenGuid(_ret, _id);
			}
		}
		BLAnsi _path[260] = { 0 };
		strcpy(_path, blWorkingDir());
		strcat(_path, _Filename);
		BLU32 _tmplen = (BLU32)strlen(_path);
		for (_i = 0; _i < _tmplen; ++_i)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			if (_path[_i] == '/')
				_path[_i] = '\\';
#else
			if (_path[_i] == '\\')
				_path[_i] = '/';
#endif
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
		WCHAR _wfilename[260] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
		HANDLE _fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		HANDLE _fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		if (FILE_INVALID_INTERNAL(_fp))
		{
			BLU32 _datasz;
			LARGE_INTEGER _li;
			GetFileSizeEx(_fp, &_li);
			_datasz = _li.LowPart;
			_ret = (_BLStream*)malloc(sizeof(_BLStream));
			_ret->pBuffer = malloc(_datasz);
			ReadFile(_fp, _ret->pBuffer, _datasz, NULL, NULL);
			_ret->nLen = _datasz;
			_ret->pPos = (BLU8*)_ret->pBuffer;
			_ret->pEnd = _ret->pPos + _ret->nLen;
			CloseHandle(_fp);
			return blGenGuid(_ret, blHashString(_path));
		}
		else
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			strcat(_path, _Filename);
			BLU32 _tmplen = (BLU32)strlen(_path);
			for (_i = 0; _i < _tmplen; ++_i)
			{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
				if (_path[_i] == '/')
					_path[_i] = '\\';
#else
				if (_path[_i] == '\\')
					_path[_i] = '/';
#endif
			}
#ifdef WINAPI_FAMILY
			memset(_wfilename, 0, sizeof(_wfilename));
			MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
			_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
			_fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
			if (FILE_INVALID_INTERNAL(_fp))
			{
				BLU32 _datasz;
				LARGE_INTEGER _li;
				GetFileSizeEx(_fp, &_li);
				_datasz = _li.LowPart;
				_ret = (_BLStream*)malloc(sizeof(_BLStream));
				_ret->pBuffer = malloc(_datasz);
				ReadFile(_fp, _ret->pBuffer, _datasz, NULL, NULL);
				_ret->nLen = _datasz;
				_ret->pPos = (BLU8*)_ret->pBuffer;
				_ret->pEnd = _ret->pPos + _ret->nLen;
				CloseHandle(_fp);
				return blGenGuid(_ret, blHashString(_path));
			}
			else
				return INVALID_GUID;
		}
#elif defined(BL_PLATFORM_ANDROID)
		AAsset* _fp = AAssetManager_open(_PrStreamIOMem->pAndroidAM, _path, AASSET_MODE_UNKNOWN);
		if (FILE_INVALID_INTERNAL(_fp))
		{
			BLU32 _datasz;
			_datasz = (BLU32)AAsset_getLength(_fp);
			_ret = (_BLStream*)malloc(sizeof(_BLStream));
			_ret->pBuffer = malloc(_datasz);
			AAsset_read(_fp, _ret->pBuffer, _datasz);
			_ret->nLen = _datasz;
			_ret->pPos = (BLU8*)_ret->pBuffer;
			_ret->pEnd = _ret->pPos + _ret->nLen;
			AAsset_close(_fp);
			return blGenGuid(_ret, blHashString((const BLUtf8*)_path));
		}
		else
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			strcat(_path, _Filename);
			BLU32 _tmplen = (BLU32)strlen(_path);
			for (_i = 0; _i < _tmplen; ++_i)
			{
				if (_path[_i] == '\\')
					_path[_i] = '/';
			}
			FILE* _fp2 = fopen(_path, "rb");
			if (FILE_INVALID_INTERNAL(_fp2))
			{
				BLU32 _datasz;
				fseek(_fp2, 0, SEEK_END);
				_datasz = (BLU32)ftell(_fp2);
				fseek(_fp2, 0, SEEK_SET);
				_ret = (_BLStream*)malloc(sizeof(_BLStream));
				_ret->pBuffer = malloc(_datasz);
				fread(_ret->pBuffer, sizeof(BLU8), _datasz, _fp2);
				_ret->nLen = _datasz;
				_ret->pPos = (BLU8*)_ret->pBuffer;
				_ret->pEnd = _ret->pPos + _ret->nLen;
				fclose(_fp2);
				return blGenGuid(_ret, blHashString((const BLUtf8*)_path));
			}
			else
				return INVALID_GUID;
		}
#elif defined(BL_PLATFORM_WEB)
		FILE* _fp = fopen(_path, "rb");
		if (FILE_INVALID_INTERNAL(_fp))
		{
			BLU32 _datasz;
			fseek(_fp, 0, SEEK_END);
			_datasz = (BLU32)ftell(_fp);
			fseek(_fp, 0, SEEK_SET);
			_ret = (_BLStream*)malloc(sizeof(_BLStream));
			_ret->pBuffer = malloc(_datasz);
			fread(_ret->pBuffer, sizeof(BLU8), _datasz, _fp);
			_ret->nLen = _datasz;
			_ret->pPos = (BLU8*)_ret->pBuffer;
			_ret->pEnd = _ret->pPos + _ret->nLen;
			fclose(_fp);
			return blGenGuid(_ret, blHashString((const BLUtf8*)_path));
		}
		else
		{
			emscripten_wget(_path, _path);
			FILE* _fp = fopen(_path, "rb");
			if (FILE_INVALID_INTERNAL(_fp))
			{
				BLU32 _datasz;
				fseek(_fp, 0, SEEK_END);
				_datasz = (BLU32)ftell(_fp);
				fseek(_fp, 0, SEEK_SET);
				_ret = (_BLStream*)malloc(sizeof(_BLStream));
				_ret->pBuffer = malloc(_datasz);
				fread(_ret->pBuffer, sizeof(BLU8), _datasz, _fp);
				_ret->nLen = _datasz;
				_ret->pPos = (BLU8*)_ret->pBuffer;
				_ret->pEnd = _ret->pPos + _ret->nLen;
				fclose(_fp);
				return blGenGuid(_ret, blHashString((const BLUtf8*)_path));
			}
			else
				return INVALID_GUID;
		}
#else
		FILE* _fp = fopen(_path, "rb");
		if (FILE_INVALID_INTERNAL(_fp))
		{
			BLU32 _datasz;
			fseek(_fp, 0, SEEK_END);
			_datasz = (BLU32)ftell(_fp);
			fseek(_fp, 0, SEEK_SET);
			_ret = (_BLStream*)malloc(sizeof(_BLStream));
			_ret->pBuffer = malloc(_datasz);
			fread(_ret->pBuffer, sizeof(BLU8), _datasz, _fp);
			_ret->nLen = _datasz;
			_ret->pPos = (BLU8*)_ret->pBuffer;
			_ret->pEnd = _ret->pPos + _ret->nLen;
			fclose(_fp);
			return blGenGuid(_ret, blHashString((const BLUtf8*)_path));
		}
		else
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			strcat(_path, _Filename);
			_tmplen = (BLU32)strlen(_path);
			for (_i = 0; _i < _tmplen; ++_i)
			{
				if (_path[_i] == '\\')
					_path[_i] = '/';
			}
			_fp = fopen(_path, "rb");
			if (FILE_INVALID_INTERNAL(_fp))
			{
				BLU32 _datasz;
				fseek(_fp, 0, SEEK_END);
				_datasz = (BLU32)ftell(_fp);
				fseek(_fp, 0, SEEK_SET);
				_ret = (_BLStream*)malloc(sizeof(_BLStream));
				_ret->pBuffer = malloc(_datasz);
				fread(_ret->pBuffer, sizeof(BLU8), _datasz, _fp);
				_ret->nLen = _datasz;
				_ret->pPos = (BLU8*)_ret->pBuffer;
				_ret->pEnd = _ret->pPos + _ret->nLen;
				fclose(_fp);
				return blGenGuid(_ret, blHashString((const BLUtf8*)_path));
			}
			else
				return INVALID_GUID;
		}
#endif
		return INVALID_GUID;
	}
	else
	{
		_BLStream* _ret = NULL;
		_ret = (_BLStream*)malloc(sizeof(_BLStream));
		_ret->pBuffer = NULL;
		_ret->nLen = 0;
		_ret->pPos = (BLU8*)_ret->pBuffer;
		_ret->pEnd = _ret->pPos + _ret->nLen;
		return blGenGuid(_ret, blUniqueUri());
	}
}
BLBool
blDeleteStream(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream->pBuffer)
		free(_stream->pBuffer);
	free(_stream);
	blDeleteGuid(_ID);
	return TRUE;
}
BLVoid*
blStreamData(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return NULL;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
		return _stream->pBuffer;
	else
	{
		blDebugOutput("Invalid Operation blStreamTell");
		return NULL;
	}
}
BLU32
blStreamTell(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return -1;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
		return (BLU32)(_stream->pPos - (BLU8*)_stream->pBuffer);
	else
	{
		blDebugOutput("Invalid Operation blStreamTell");
		return 0;
	}
}
BLU32
blStreamLength(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return -1;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
		return (BLU32)(_stream->nLen);
	else
	{
		blDebugOutput("Invalid Operation blStreamLength");
		return 0;
	}
}
BLBool
blStreamEos(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return FALSE;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
		return (_stream->pPos == _stream->pEnd);
	else
	{
		blDebugOutput("Invalid Operation blStreamEos");
		return TRUE;
	}
}
BLU32
blStreamRead(IN BLGuid _ID, IN BLU32 _Count, OUT BLVoid* _Buf)
{
	if (_ID == INVALID_GUID)
		return 0;
	BLU32 _cnt;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
	{
		_cnt = _Count;
		if (_stream->pPos + _cnt > _stream->pEnd)
			_cnt = (BLU32)(_stream->pEnd - _stream->pPos);
		if (_cnt == 0)
			return 0;
		memcpy(_Buf, _stream->pPos, _cnt);
		_stream->pPos += _cnt;
		return _cnt;
	}
	else
	{
		blDebugOutput("Invalid Operation blStreamRead");
		return 0;
	}
}
BLU32
blStreamWrite(IN BLGuid _ID, IN BLU32 _Count, IN BLVoid* _Buf)
{
	if (_ID == INVALID_GUID)
		return 0;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
	{
		_stream->pBuffer = (BLVoid*)realloc(_stream->pBuffer, _stream->nLen + _Count);
		_stream->pPos = _stream->pBuffer;
		_stream->nLen += _Count;
		memcpy(_stream->pPos, _Buf, _Count);
		_stream->pEnd = (BLU8*)_stream->pBuffer + _stream->nLen;
		return _Count;
	}
	else
	{
		blDebugOutput("Invalid Operation blStreamWrite");
		return 0;
	}
}
BLVoid
blStreamSeek(IN BLGuid _ID, IN BLU32 _Pos)
{
	if (_ID == INVALID_GUID)
		return;
	_BLStream* _stream = (_BLStream*)blGuidAsPointer(_ID);
	if (_stream)
	{
		if (_stream->pPos + _Pos > _stream->pEnd)
			_stream->pPos = _stream->pEnd;
		if (_stream->pPos + _Pos < (BLU8*)_stream->pBuffer)
			_stream->pPos = (BLU8*)_stream->pBuffer;
		_stream->pPos = (BLU8*)_stream->pBuffer + _Pos;
	}
	else
		blDebugOutput("Invalid Operation blStreamSeek");
}
BLBool
blFileExist(IN BLAnsi* _AbsFilename)
{
	BLU32 _i;
	BLAnsi _tmpname[260];
	strcpy(_tmpname, _AbsFilename);
	for (_i = 0; _i < strlen(_tmpname); ++_i)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		if (_tmpname[_i] == '/')
			_tmpname[_i] = '\\';
#else
		if (_tmpname[_i] == '\\')
			_tmpname[_i] = '/';
#endif
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	WIN32_FILE_ATTRIBUTE_DATA _wfad;
	if (GetFileAttributesExA(_tmpname, GetFileExInfoStandard, &_wfad))
		return TRUE;
	else
		return FALSE;
#else
	if (access(_tmpname, 0) != -1)
		return TRUE;
	else
		return FALSE;
#endif
}
BLBool
blFileWrite(IN BLAnsi* _Filename, IN BLU32 _Count, IN BLU8* _Data)
{
	BLU32 _i;
	BLAnsi _tmpname[260] = { 0 };
    BLAnsi _path[260] = { 0 };
    strcpy(_tmpname , _Filename);
    for (_i = 0 ; _i < strlen(_tmpname) ; ++_i)
    {
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
        if(_tmpname[_i] == '/')
            _tmpname[_i] = '\\';
#else
        if(_tmpname[_i] == '\\')
            _tmpname[_i] = '/';
#endif
    }
    strcpy(_path , blUserFolderDir());
    strcat(_path , _tmpname);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
	WCHAR _wfilename[260] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
	HANDLE _fp = CreateFile2(_wfilename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS, NULL);
#else
	HANDLE _fp = CreateFileA(_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
	if (FILE_INVALID_INTERNAL(_fp))
	{
		WriteFile(_fp, _Data, _Count, NULL, NULL);
		CloseHandle(_fp);
		return TRUE;
	}
#elif defined(BL_PLATFORM_WEB)
	emscripten_fetch_attr_t _attr;
	emscripten_fetch_attr_init(&_attr);
	strcpy(_attr.requestMethod, "EM_IDB_STORE");
	_attr.attributes = EMSCRIPTEN_FETCH_REPLACE | EMSCRIPTEN_FETCH_PERSIST_FILE;
	_attr.requestData = _Data;
	_attr.requestDataSize = _Count;
	_attr.onsuccess = _WriteSuccess;
	_attr.onerror = _WriteFailure;
	emscripten_fetch(&_attr, _path);
	return TRUE;
#else
	FILE* _fp = fopen(_path , "wb");
    if (FILE_INVALID_INTERNAL(_fp))
    {
        fwrite(_Data , sizeof(BLU8) , _Count, _fp);
        fclose(_fp);
        return TRUE;
    }
#endif
    return FALSE;
}
BLBool
blFileDelete(IN BLAnsi* _Filename)
{
	BLU32 _i;
	BLAnsi _tmpname[260];
	strcpy(_tmpname, _Filename);
	for (_i = 0; _i < strlen(_tmpname); ++_i)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		if (_tmpname[_i] == '/')
			_tmpname[_i] = '\\';
#else
		if (_tmpname[_i] == '\\')
			_tmpname[_i] = '/';
#endif
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (DeleteFileA(_tmpname))
		return FALSE;
	else
		return TRUE;
#elif defined(BL_PLATFORM_WEB)
	emscripten_fetch_attr_t _attr;
	emscripten_fetch_attr_init(&_attr);
	strcpy(_attr.requestMethod, "EM_IDB_DELETE");
	_attr.onsuccess = _WriteSuccess;
	_attr.onerror = _WriteFailure;
	emscripten_fetch(&_attr, _tmpname);
	return TRUE;
#else
	if (remove(_tmpname))
		return FALSE;
	else
		return TRUE;
#endif
}
BLBool
blArchiveRegist(IN BLAnsi* _Filename, IN BLAnsi* _Archive)
{
	BLU32 _i;
    BLAnsi _tmpname[260];
    BLAnsi _path[260] = {0};
    _BLBpkFileHeader _header;
	_BLBpkArchive* _ret;
    strcpy(_tmpname , _Filename);
	strcpy(_path, blWorkingDir());
    strcat(_path, _tmpname);
	for (_i = 0; _i < strlen(_path); ++_i)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		if (_path[_i] == '/')
			_path[_i] = '\\';
#else
		if (_path[_i] == '\\')
			_path[_i] = '/';
#endif
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
	WCHAR _wfilename[260] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
	HANDLE _fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
	HANDLE _fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#elif defined(BL_PLATFORM_ANDROID)
	BLBool _inworkdir = TRUE;
	BLVoid* _fp = AAssetManager_open(_PrStreamIOMem->pAndroidAM, _path, AASSET_MODE_UNKNOWN);
#else
    FILE* _fp = fopen(_path , "rb");
#endif
    if (!FILE_INVALID_INTERNAL(_fp))
    {
        memset(_path , 0 , sizeof(_path));
        strcpy(_path , blUserFolderDir());
        strcat(_path , _tmpname);
		for (_i = 0; _i < strlen(_path); ++_i)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			if (_path[_i] == '/')
				_path[_i] = '\\';
#else
			if (_path[_i] == '\\')
				_path[_i] = '/';
#endif
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
		memset(_wfilename, 0, sizeof(_wfilename));
		MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
		HANDLE _fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		HANDLE _fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#elif defined(BL_PLATFORM_ANDROID)
		_inworkdir = FALSE;
		_fp = fopen(_path, "rb");
#else
        _fp = fopen(_path , "rb");
#endif
        if(!FILE_INVALID_INTERNAL(_fp))
            return FALSE;
    }
	_ret = (_BLBpkArchive*)malloc(sizeof(_BLBpkArchive));
	_ret->pFiles = blGenDict(FALSE);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	ReadFile(_fp, &_header, sizeof(_BLBpkFileHeader), NULL, NULL);
	LARGE_INTEGER _li;
	_li.LowPart = _header.nEntryOffset;
	_li.HighPart = 0;
	SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
	for (_i = 0; _i < _header.nFileNum; ++_i)
	{
		_BLBpkFileEntry* entry = (_BLBpkFileEntry*)malloc(sizeof(_BLBpkFileEntry));
		ReadFile(_fp, entry, sizeof(_BLBpkFileEntry) - sizeof(BLAnsi*), NULL, NULL);
		entry->pPatch = NULL;
		blDictInsert(_ret->pFiles, entry->nHashname, entry);
	}
	_ret->nVersion = _header.nVersion;
	_ret->nOffset = _header.nEntryOffset;
	_ret->pPath = (BLAnsi*)malloc(strlen(_path) + 1);
	strcpy(_ret->pPath, _path);
	_ret->pPath[strlen(_path)] = 0;
	_ret->pArchive = (BLAnsi*)malloc(strlen(_Archive) + 1);
	strcpy(_ret->pArchive, _Archive);
	_ret->pArchive[strlen(_Archive)] = 0;
	blArrayPushBack(_PrStreamIOMem->pArchives, _ret);
	CloseHandle(_fp);
#elif defined(BL_PLATFORM_ANDROID)
	if (_inworkdir)
	{
		AAsset_read(_fp, &_header, sizeof(_BLBpkFileHeader) * 1);
		AAsset_seek(_fp, _header.nEntryOffset, SEEK_SET);
		for (_i = 0; _i < _header.nFileNum; ++_i)
		{
			_BLBpkFileEntry* entry = (_BLBpkFileEntry*)malloc(sizeof(_BLBpkFileEntry));
			AAsset_read(_fp, entry, sizeof(_BLBpkFileEntry) * 1 - sizeof(BLAnsi*));
			entry->pPatch = NULL;
			blDictInsert(_ret->pFiles, entry->nHashname, entry);
		}
		_ret->nVersion = _header.nVersion;
		_ret->nOffset = _header.nEntryOffset;
		_ret->pPath = (BLAnsi*)malloc(strlen(_path) + 1);
		strcpy(_ret->pPath, _path);
		_ret->pPath[strlen(_path)] = 0;
		_ret->pArchive = (BLAnsi*)malloc(strlen(_Archive) + 1);
		strcpy(_ret->pArchive, _Archive);
		_ret->pArchive[strlen(_Archive)] = 0;
		blArrayPushBack(_PrStreamIOMem->pArchives, _ret);
		AAsset_close(_fp);
	}
	else
	{
		fread(&_header, sizeof(_BLBpkFileHeader), 1, _fp);
		fseek(_fp, _header.nEntryOffset, SEEK_SET);
		for (_i = 0; _i < _header.nFileNum; ++_i)
		{
			_BLBpkFileEntry* entry = (_BLBpkFileEntry*)malloc(sizeof(_BLBpkFileEntry));
			fread(entry, sizeof(_BLBpkFileEntry) - sizeof(BLAnsi*), 1, _fp);
			entry->pPatch = NULL;
			blDictInsert(_ret->pFiles, entry->nHashname, entry);
		}
		_ret->nVersion = _header.nVersion;
		_ret->nOffset = _header.nEntryOffset;
		_ret->pPath = (BLAnsi*)malloc(strlen(_path) + 1);
		strcpy(_ret->pPath, _path);
		_ret->pPath[strlen(_path)] = 0;
		_ret->pArchive = (BLAnsi*)malloc(strlen(_Archive) + 1);
		strcpy(_ret->pArchive, _Archive);
		_ret->pArchive[strlen(_Archive)] = 0;
		blArrayPushBack(_PrStreamIOMem->pArchives, _ret);
		fclose(_fp);
	}
#else
	fread(&_header, sizeof(_BLBpkFileHeader), 1, _fp);
	fseek(_fp, _header.nEntryOffset, SEEK_SET);
	for (_i = 0; _i < _header.nFileNum; ++_i)
	{
		_BLBpkFileEntry* entry = (_BLBpkFileEntry*)malloc(sizeof(_BLBpkFileEntry));
		fread(entry, sizeof(_BLBpkFileEntry) - sizeof(BLAnsi*), 1, _fp);
		entry->pPatch = NULL;
		blDictInsert(_ret->pFiles, entry->nHashname, entry);
	}
	_ret->nVersion = _header.nVersion;
	_ret->nOffset = _header.nEntryOffset;
	_ret->pPath = (BLAnsi*)malloc(strlen(_path) + 1);
	strcpy(_ret->pPath, _path);
	_ret->pPath[strlen(_path)] = 0;
	_ret->pArchive = (BLAnsi*)malloc(strlen(_Archive) + 1);
	strcpy(_ret->pArchive, _Archive);
	_ret->pArchive[strlen(_Archive)] = 0;
	blArrayPushBack(_PrStreamIOMem->pArchives, _ret);
	fclose(_fp);
#endif
	return TRUE;
}
BLBool
blArchivePatch(IN BLAnsi* _Filename, IN BLAnsi* _Archive)
{
	BLU32 _aidx = 0 , _i;
    BLAnsi _tmpname[260];
    BLAnsi _path[260] = {0};
    _BLPatchHeader _header;
	_BLBpkArchive* _iter = NULL;
	if (!_PrStreamIOMem->pArchives->nSize)
		return FALSE;
    {
		FOREACH_ARRAY(_BLBpkArchive*, _biter, _PrStreamIOMem->pArchives)
		{
			if (!strcmp(_Archive, _biter->pArchive))
			{
				_iter = _biter;
				break;
			}
			_aidx++;
		}
	}
    if (!_iter)
		return FALSE;
    strcpy(_tmpname , _Filename);
    for (_i = 0 ; _i < strlen(_tmpname) ; ++_i)
    {
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
        if(_tmpname[_i] == '/')
            _tmpname[_i] = '\\';
#else
        if(_tmpname[_i] == '\\')
            _tmpname[_i] = '/';
#endif
    }
    strcpy(_path , blUserFolderDir());
    strcat(_path , _tmpname);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
	WCHAR _wfilename[260] = { 0 };
	memset(_wfilename, 0, sizeof(_wfilename));
	MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
	HANDLE _fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
	HANDLE _fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#elif defined(BL_PLATFORM_ANDROID)
	BLBool _inworkdir = FALSE;
	BLVoid* _fp = fopen(_path, "rb");
#else
    FILE* _fp = fopen(_path , "rb");
#endif
	if (!FILE_INVALID_INTERNAL(_fp))
	{
		memset(_path, 0, sizeof(_path));
		strcpy(_path, blWorkingDir());
		strcat(_path, _tmpname);
		for (_i = 0; _i < strlen(_path); ++_i)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			if (_path[_i] == '/')
				_path[_i] = '\\';
#else
			if (_path[_i] == '\\')
				_path[_i] = '/';
#endif
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
		memset(_wfilename, 0, sizeof(_wfilename));
		MultiByteToWideChar(CP_UTF8, 0, _path, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		_fp = CreateFileA(_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#elif defined(BL_PLATFORM_ANDROID)
		_inworkdir = TRUE;
		_fp = AAssetManager_open(_PrStreamIOMem->pAndroidAM, _path, AASSET_MODE_UNKNOWN);
#else
		_fp = fopen(_path, "rb");
#endif
		if (!FILE_INVALID_INTERNAL(_fp))
			return FALSE;
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	ReadFile(_fp, &_header, sizeof(_header), NULL, NULL);
#elif defined(BL_PLATFORM_ANDROID)
	if (_inworkdir)
		AAsset_read(_fp, &_header, sizeof(_header) * 1);
	else
		fread(&_header, sizeof(_header), 1, _fp);
#else
	fread(&_header, sizeof(_header), 1, _fp);
#endif
	if (_header.nVersion != _iter->nVersion)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		CloseHandle(_fp);
#elif defined(BL_PLATFORM_ANDROID)
		if (_inworkdir)
			AAsset_close(_fp);
		else
			fclose(_fp);
#else
		fclose(_fp);
#endif
		return FALSE;
	}
    if (_header.nAddListNum)
    {
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		LARGE_INTEGER _li;
		_li.LowPart = _header.nAddListOffset;
		_li.HighPart = 0;
		SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
#elif defined(BL_PLATFORM_ANDROID)
		if (_inworkdir)
			AAsset_seek(_fp, _header.nAddListOffset, SEEK_SET);
		else
			fseek(_fp, _header.nAddListOffset, SEEK_SET);
#else
        fseek(_fp , _header.nAddListOffset , SEEK_SET);
#endif
        for (_i = 0 ; _i < _header.nAddListNum ; ++_i)
        {
			_BLBpkFileEntry* _fentry;
            _BLPatchEntry _entry;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			ReadFile(_fp, &_entry, sizeof(_entry), NULL, NULL);
#elif defined(BL_PLATFORM_ANDROID)
			if (_inworkdir)
				AAsset_read(_fp, &_entry, sizeof(_entry) * 1);
			else
				fread(&_entry, sizeof(_entry), 1, _fp);
#else
            fread(&_entry, sizeof(_entry), 1 , _fp);
#endif
			_fentry = (_BLBpkFileEntry*)malloc(sizeof(_BLBpkFileEntry));
            _fentry->nHashname = _entry.nHashname;
            _fentry->nLength = _entry.nComSize;
            _fentry->nOffset = _entry.nOffset;
            strncpy(_fentry->aMd5 , _entry.aMd5 , 32);
			_fentry->pPatch = (BLAnsi*)malloc(strlen(_path) + 1);
			strcpy(_fentry->pPatch, _path);
			_fentry->pPatch[strlen(_path)] = 0;
            blDictInsert(_iter->pFiles, _fentry->nHashname , _fentry);
        }
    }
    if (_header.nModListNum)
    {
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		LARGE_INTEGER _li;
		_li.LowPart = _header.nModListOffset;
		_li.HighPart = 0;
		SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
#elif defined(BL_PLATFORM_ANDROID)
		if (_inworkdir)
			AAsset_seek(_fp, _header.nModListOffset, SEEK_SET);
		else
			fseek(_fp, _header.nModListOffset, SEEK_SET);
#else
        fseek(_fp , _header.nModListOffset , SEEK_SET);
#endif
        for (_i = 0 ; _i < _header.nModListNum ; ++_i)
        {
			_BLBpkFileEntry* _fentry;
			_BLPatchEntry _entry;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			ReadFile(_fp, &_entry, sizeof(_entry), NULL, NULL);
#elif defined(BL_PLATFORM_ANDROID)
			if (_inworkdir)
				AAsset_read(_fp, &_entry, sizeof(_entry) * 1);
			else
				fread(&_entry, sizeof(_entry), 1, _fp);
#else
            fread(&_entry, sizeof(_entry), 1, _fp);
#endif
			_fentry = (_BLBpkFileEntry*)blDictElement(_iter->pFiles, _entry.nHashname);
			_fentry->nHashname = _entry.nHashname;
			_fentry->nLength = _entry.nComSize;
			_fentry->nOffset = _entry.nOffset;
			strncpy(_fentry->aMd5, _entry.aMd5, 32);
			_fentry->pPatch = (BLAnsi*)malloc(strlen(_path) + 1);
			strcpy(_fentry->pPatch, _path);
			_fentry->pPatch[strlen(_path)] = 0;
        }
    }
    if (_header.nDelListNum)
    {
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		LARGE_INTEGER _li;
		_li.LowPart = _header.nDelListOffset;
		_li.HighPart = 0;
		SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
#elif defined(BL_PLATFORM_ANDROID)
		if (_inworkdir)
			AAsset_seek(_fp, _header.nDelListOffset, SEEK_SET);
		else
			fseek(_fp, _header.nDelListOffset, SEEK_SET);
#else
        fseek(_fp, _header.nDelListOffset, SEEK_SET);
#endif
        for (_i = 0 ; _i < _header.nDelListNum ; ++_i)
        {
            _BLPatchEntry _entry;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			ReadFile(_fp, &_entry, sizeof(_entry), NULL, NULL);
#elif defined(BL_PLATFORM_ANDROID)
			if (_inworkdir)
				AAsset_read(_fp, &_entry, sizeof(_entry) * 1);
			else
				fread(&_entry, sizeof(_entry), 1, _fp);
#else
            fread(&_entry, sizeof(_entry), 1, _fp);
#endif
            blDictErase(_iter->pFiles, _entry.nHashname);
        }
    }
    _iter->nVersion = _header.nDestVersion;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	CloseHandle(_fp);
#elif defined(BL_PLATFORM_ANDROID)
	if (_inworkdir)
		AAsset_close(_fp);
	else
		fclose(_fp);
#else
    fclose(_fp);
#endif
	return TRUE;
}
BLBool
blArchiveQuery(IN BLAnsi* _Archive, OUT BLU32* _Version)
{
    FOREACH_ARRAY(_BLBpkArchive*, _iter, _PrStreamIOMem->pArchives)
    {
		if (!strcmp(_Archive, _iter->pArchive))
		{
			*_Version = _iter->nVersion;
			return TRUE;
		}
    }
	_Version = NULL;
    return FALSE;
}
BLVoid 
blPreload(IN BLAnsi* _Filenames, IN BLBool _Flush)
{
	FOREACH_LIST(BLAnsi*, _iter, _PrStreamIOMem->pPreloadQueue)
	{
		free(_iter);
		blListErase(_PrStreamIOMem->pPreloadQueue, _iterator_iter);
	}
	if (_Filenames)
	{
		BLAnsi* _tmp;
		BLAnsi* _tag = (BLAnsi*)alloca(strlen(_Filenames) + 1);
		memset(_tag, 0, strlen(_Filenames) + 1);
		strcpy(_tag, _Filenames);
		_tmp = strtok((BLAnsi*)_tag, ",");
		while (_tmp)
		{
			if (_Flush)
				_PreloadFile(_tmp);
			else
			{
				BLAnsi* _node = (BLAnsi*)malloc(strlen(_tmp) + 1);
				memset(_node, 0, strlen(_tmp) + 1);
				strcpy(_node, _tmp);
				blListPushBack(_PrStreamIOMem->pPreloadQueue, _node);
			}
			_tmp = strtok(NULL, ",");
		}
		if (!_Flush)
			_PrStreamIOMem->nPreloadNum = _PrStreamIOMem->pPreloadQueue->nSize;
		else
			blInvokeEvent(BL_ET_SYSTEM, BL_SE_PRELOAD, 100, _Filenames, INVALID_GUID);
	}
}

