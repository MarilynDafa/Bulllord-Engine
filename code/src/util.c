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
#include "../headers/util.h"
#include "internal/thread.h"
#include "internal/internal.h"
#include "../externals/duktape/duktape.h"
typedef struct _Ctx{
	BLU32 aState[4];
	BLU32 aCount[2];
	BLU8 aBuffer[64];
}_BLCtx;
typedef struct _MemCache {
	BLU32 nUsed;
	BLU32 nMax;
	BLU32* pDense;
	BLU32* pSparse;
}_BLMemCache;
typedef struct _UtilMember {
	DUK_CONTEXT* pDukContext;
	_BLMemCache sMemCache;
	BLVoid* aPtrBuf[MEMCACHE_CAP_INTERNAL];
	blMutex* pMutex;
	_BLCtx sContext;
	BLAnsi aMd5Buf[64];
	BLUtf8 aU8Buf[32];
	BLUtf16 aU16Buf[32];
	BLAnsi aGuidBuf[32];
    BLU32 nUriCount;
}_BLUtilMember;
static _BLUtilMember* _PrUtilMem = NULL;
static BLVoid
_MD5FF(BLU32* _Aa, BLU32 _Bb, BLU32 _Cc, BLU32 _Dd, BLU32 _Xx, BLU32 _Ss, BLU32 _AC)
{
	*_Aa += (((_Bb)& (_Cc)) | ((~_Bb) & (_Dd))) + (_Xx)+(BLU32)(_AC);
	*_Aa = (((*_Aa) << (_Ss)) | ((*_Aa) >> (32 - (_Ss))));
	*_Aa += _Bb;
}
static BLVoid
_MD5GG(BLU32* _Aa, BLU32 _Bb, BLU32 _Cc, BLU32 _Dd, BLU32 _Xx, BLU32 _Ss, BLU32 _AC)
{
	*_Aa += (((_Bb)& (_Dd)) | ((_Cc)& (~_Dd))) + (_Xx)+(BLU32)(_AC);
	*_Aa = (((*_Aa) << (_Ss)) | ((*_Aa) >> (32 - (_Ss))));
	*_Aa += _Bb;
}
static BLVoid
_MD5HH(BLU32* _Aa, BLU32 _Bb, BLU32 _Cc, BLU32 _Dd, BLU32 _Xx, BLU32 _Ss, BLU32 _AC)
{
	*_Aa += ((_Bb) ^ (_Cc) ^ (_Dd)) + (_Xx) + (BLU32)(_AC);
	*_Aa = (((*_Aa) << (_Ss)) | ((*_Aa) >> (32 - (_Ss))));
	*_Aa += _Bb;
}
static BLVoid
_MD5II(BLU32* _Aa, BLU32 _Bb, BLU32 _Cc, BLU32 _Dd, BLU32 _Xx, BLU32 _Ss, BLU32 _AC)
{
	*_Aa += ((_Cc) ^ ((_Bb) | (~_Dd))) + (_Xx) + (BLU32)(_AC);
	*_Aa = (((*_Aa) << (_Ss)) | ((*_Aa) >> (32 - (_Ss))));
	*_Aa += _Bb;
}
static BLVoid
_MD5Transform(BLU32 _State[4], BLU8 _Block[64])
{
	BLU32 _a = _State[0], _b = _State[1], _c = _State[2], _d = _State[3];
	BLU32 _x[16];
	BLU32 _i, _j;
	for (_i = 0, _j = 0; _j < 64; _i++, _j += 4)
		_x[_i] = ((BLU32)_Block[_j]) | (((BLU32)_Block[_j + 1]) << 8) | (((BLU32)_Block[_j + 2]) << 16) | (((BLU32)_Block[_j + 3]) << 24);
	_MD5FF(&_a, _b, _c, _d, _x[0], 7, 0xd76aa478);
	_MD5FF(&_d, _a, _b, _c, _x[1], 12, 0xe8c7b756);
	_MD5FF(&_c, _d, _a, _b, _x[2], 17, 0x242070db);
	_MD5FF(&_b, _c, _d, _a, _x[3], 22, 0xc1bdceee);
	_MD5FF(&_a, _b, _c, _d, _x[4], 7, 0xf57c0faf);
	_MD5FF(&_d, _a, _b, _c, _x[5], 12, 0x4787c62a);
	_MD5FF(&_c, _d, _a, _b, _x[6], 17, 0xa8304613);
	_MD5FF(&_b, _c, _d, _a, _x[7], 22, 0xfd469501);
	_MD5FF(&_a, _b, _c, _d, _x[8], 7, 0x698098d8);
	_MD5FF(&_d, _a, _b, _c, _x[9], 12, 0x8b44f7af);
	_MD5FF(&_c, _d, _a, _b, _x[10], 17, 0xffff5bb1);
	_MD5FF(&_b, _c, _d, _a, _x[11], 22, 0x895cd7be);
	_MD5FF(&_a, _b, _c, _d, _x[12], 7, 0x6b901122);
	_MD5FF(&_d, _a, _b, _c, _x[13], 12, 0xfd987193);
	_MD5FF(&_c, _d, _a, _b, _x[14], 17, 0xa679438e);
	_MD5FF(&_b, _c, _d, _a, _x[15], 22, 0x49b40821);
	_MD5GG(&_a, _b, _c, _d, _x[1], 5, 0xf61e2562);
	_MD5GG(&_d, _a, _b, _c, _x[6], 9, 0xc040b340);
	_MD5GG(&_c, _d, _a, _b, _x[11], 14, 0x265e5a51);
	_MD5GG(&_b, _c, _d, _a, _x[0], 20, 0xe9b6c7aa);
	_MD5GG(&_a, _b, _c, _d, _x[5], 5, 0xd62f105d);
	_MD5GG(&_d, _a, _b, _c, _x[10], 9, 0x2441453);
	_MD5GG(&_c, _d, _a, _b, _x[15], 14, 0xd8a1e681);
	_MD5GG(&_b, _c, _d, _a, _x[4], 20, 0xe7d3fbc8);
	_MD5GG(&_a, _b, _c, _d, _x[9], 5, 0x21e1cde6);
	_MD5GG(&_d, _a, _b, _c, _x[14], 9, 0xc33707d6);
	_MD5GG(&_c, _d, _a, _b, _x[3], 14, 0xf4d50d87);
	_MD5GG(&_b, _c, _d, _a, _x[8], 20, 0x455a14ed);
	_MD5GG(&_a, _b, _c, _d, _x[13], 5, 0xa9e3e905);
	_MD5GG(&_d, _a, _b, _c, _x[2], 9, 0xfcefa3f8);
	_MD5GG(&_c, _d, _a, _b, _x[7], 14, 0x676f02d9);
	_MD5GG(&_b, _c, _d, _a, _x[12], 20, 0x8d2a4c8a);
	_MD5HH(&_a, _b, _c, _d, _x[5], 4, 0xfffa3942);
	_MD5HH(&_d, _a, _b, _c, _x[8], 11, 0x8771f681);
	_MD5HH(&_c, _d, _a, _b, _x[11], 16, 0x6d9d6122);
	_MD5HH(&_b, _c, _d, _a, _x[14], 23, 0xfde5380c);
	_MD5HH(&_a, _b, _c, _d, _x[1], 4, 0xa4beea44);
	_MD5HH(&_d, _a, _b, _c, _x[4], 11, 0x4bdecfa9);
	_MD5HH(&_c, _d, _a, _b, _x[7], 16, 0xf6bb4b60);
	_MD5HH(&_b, _c, _d, _a, _x[10], 23, 0xbebfbc70);
	_MD5HH(&_a, _b, _c, _d, _x[13], 4, 0x289b7ec6);
	_MD5HH(&_d, _a, _b, _c, _x[0], 11, 0xeaa127fa);
	_MD5HH(&_c, _d, _a, _b, _x[3], 16, 0xd4ef3085);
	_MD5HH(&_b, _c, _d, _a, _x[6], 23, 0x4881d05);
	_MD5HH(&_a, _b, _c, _d, _x[9], 4, 0xd9d4d039);
	_MD5HH(&_d, _a, _b, _c, _x[12], 11, 0xe6db99e5);
	_MD5HH(&_c, _d, _a, _b, _x[15], 16, 0x1fa27cf8);
	_MD5HH(&_b, _c, _d, _a, _x[2], 23, 0xc4ac5665);
	_MD5II(&_a, _b, _c, _d, _x[0], 6, 0xf4292244);
	_MD5II(&_d, _a, _b, _c, _x[7], 10, 0x432aff97);
	_MD5II(&_c, _d, _a, _b, _x[14], 15, 0xab9423a7);
	_MD5II(&_b, _c, _d, _a, _x[5], 21, 0xfc93a039);
	_MD5II(&_a, _b, _c, _d, _x[12], 6, 0x655b59c3);
	_MD5II(&_d, _a, _b, _c, _x[3], 10, 0x8f0ccc92);
	_MD5II(&_c, _d, _a, _b, _x[10], 15, 0xffeff47d);
	_MD5II(&_b, _c, _d, _a, _x[1], 21, 0x85845dd1);
	_MD5II(&_a, _b, _c, _d, _x[8], 6, 0x6fa87e4f);
	_MD5II(&_d, _a, _b, _c, _x[15], 10, 0xfe2ce6e0);
	_MD5II(&_c, _d, _a, _b, _x[6], 15, 0xa3014314);
	_MD5II(&_b, _c, _d, _a, _x[13], 21, 0x4e0811a1);
	_MD5II(&_a, _b, _c, _d, _x[4], 6, 0xf7537e82);
	_MD5II(&_d, _a, _b, _c, _x[11], 10, 0xbd3af235);
	_MD5II(&_c, _d, _a, _b, _x[2], 15, 0x2ad7d2bb);
	_MD5II(&_b, _c, _d, _a, _x[9], 21, 0xeb86d391);
	_State[0] += _a;
	_State[1] += _b;
	_State[2] += _c;
	_State[3] += _d;
	memset(_x, 0, sizeof(_x));
}
static BLVoid
_MD5Update(BLU8* _Input, BLU32 _Inputlen)
{
	BLU32 _i = 0;
	BLU32 _index = 0;
	BLU32 _partlen = 0;
	_index = (BLU32)((_PrUtilMem->sContext.aCount[0] >> 3) & 0x3F);
	if ((_PrUtilMem->sContext.aCount[0] += ((BLU32)_Inputlen << 3)) < ((BLU32)_Inputlen << 3))
		_PrUtilMem->sContext.aCount[1]++;
	_PrUtilMem->sContext.aCount[1] += ((BLU32)_Inputlen >> 29);
	_partlen = 64 - _index;
	if (_Inputlen >= _partlen)
	{
		memcpy(_PrUtilMem->sContext.aBuffer + _index, _Input, _partlen);
		_MD5Transform(_PrUtilMem->sContext.aState, _PrUtilMem->sContext.aBuffer);
		for (_i = _partlen; _i + 63 < _Inputlen; _i += 64)
			_MD5Transform(_PrUtilMem->sContext.aState, _Input + _i);
		_index = 0;
	}
	else
		_i = 0;
	memcpy(_PrUtilMem->sContext.aBuffer + _index, _Input + _i, _Inputlen - _i);
}
BLVoid
_UtilsInit(DUK_CONTEXT* _DKC)
{
	BLU32 _idx;
	_PrUtilMem = (_BLUtilMember*)malloc(sizeof(_BLUtilMember));
	memset(_PrUtilMem->aU8Buf, 0, sizeof(_PrUtilMem->aU8Buf));
	memset(_PrUtilMem->aU16Buf, 0, sizeof(_PrUtilMem->aU16Buf));
	memset(_PrUtilMem->aPtrBuf, 0, sizeof(_PrUtilMem->aPtrBuf));
	_PrUtilMem->pDukContext = _DKC;
	_PrUtilMem->sMemCache.nUsed = 0;
	_PrUtilMem->sMemCache.nMax = MEMCACHE_CAP_INTERNAL;
	_PrUtilMem->sMemCache.pDense = (BLU32*)malloc(_PrUtilMem->sMemCache.nMax * sizeof(BLU32));
	_PrUtilMem->sMemCache.pSparse = (BLU32*)malloc(_PrUtilMem->sMemCache.nMax * sizeof(BLU32));
	for (_idx = 0; _idx < _PrUtilMem->sMemCache.nMax; ++_idx)
		_PrUtilMem->sMemCache.pDense[_idx] = _idx;
	_PrUtilMem->pMutex = blGenMutex();
    _PrUtilMem->nUriCount = 0;
}
BLVoid
_UtilsStep(BLU32 _Delta)
{
}
BLVoid
_UtilsDestroy()
{
	assert(_PrUtilMem->sMemCache.nUsed == 0);
	blDeleteMutex(_PrUtilMem->pMutex);
	free(_PrUtilMem->sMemCache.pDense);
	free(_PrUtilMem->sMemCache.pSparse);
	free(_PrUtilMem);
}
BLU32
blUniqueUri()
{
    BLAnsi _buf[128] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
    sprintf(_buf, "%s_%I32u", "BulllordEngineUniqueUriCount", _PrUtilMem->nUriCount);
#elif defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_ANDROID)
    sprintf(_buf, "%s_%zu", "BulllordEngineUniqueUriCount", _PrUtilMem->nUriCount);
#else
    sprintf(_buf, "%s_%u", "BulllordEngineUniqueUriCount", _PrUtilMem->nUriCount);
#endif
    return blHashString((const BLUtf8*)_buf);
}
BLGuid
blGenGuid(IN BLVoid* _Ptr, IN BLU32 _Uri)
{
	blMutexLock(_PrUtilMem->pMutex);
	BLU32 _inptr = 0xFFFFFFFF;
	if (_Ptr)
	{
		if (_PrUtilMem->sMemCache.nUsed < _PrUtilMem->sMemCache.nMax)
		{
			BLU32 _index = _PrUtilMem->sMemCache.nUsed;
			++_PrUtilMem->sMemCache.nUsed;
			_inptr = _PrUtilMem->sMemCache.pDense[_index];
			_PrUtilMem->sMemCache.pSparse[_inptr] = _index;
		}
        else
            blDebugOutput("GUID gen error");
		_PrUtilMem->aPtrBuf[_inptr] = (BLVoid*)_Ptr;
	}
	BLGuid _gid;
	_gid = MAKEGUID_INTERNAL(_inptr, _Uri);
	blMutexUnlock(_PrUtilMem->pMutex);
	return _gid;
}
BLVoid
blDeleteGuid(IN BLGuid _ID)
{
	blMutexLock(_PrUtilMem->pMutex);
	BLU32 _inptr = PTRPART_INTERNAL(_ID);
	if (_inptr != 0xFFFFFFFF)
	{
		BLU32 _index = _PrUtilMem->sMemCache.pSparse[_inptr];
		--_PrUtilMem->sMemCache.nUsed;
		BLU32 _temp = _PrUtilMem->sMemCache.pDense[_PrUtilMem->sMemCache.nUsed];
		_PrUtilMem->sMemCache.pDense[_PrUtilMem->sMemCache.nUsed] = _inptr;
		_PrUtilMem->sMemCache.pSparse[_temp] = _index;
		_PrUtilMem->sMemCache.pDense[_index] = _temp;
		_PrUtilMem->aPtrBuf[_inptr] = NULL;
	}
	blMutexUnlock(_PrUtilMem->pMutex);
}
BLVoid*
blGuidAsPointer(IN BLGuid _ID)
{
	BLU32 _inptr = PTRPART_INTERNAL(_ID);
	if (_inptr >= MEMCACHE_CAP_INTERNAL)
        return NULL;
	else
		return _PrUtilMem->aPtrBuf[_inptr];
}
const BLAnsi*
blGuidAsString(IN BLGuid _ID)
{
	memset(_PrUtilMem->aGuidBuf, 0, sizeof(_PrUtilMem->aGuidBuf));
	sprintf((BLAnsi*)_PrUtilMem->aGuidBuf, "%llu", _ID);
	return _PrUtilMem->aGuidBuf;
}
BLGuid
blStringAsGuid(IN BLAnsi* _String)
{
	return strtoull(_String, NULL, 10);
}
BLVoid
blDebugOutput(IN BLAnsi* _Format, ...)
{
#if defined(_DEBUG) || defined(DEBUG)
	BLAnsi _szbuf[512] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	WCHAR _wszbuf[512] = { 0 };
#endif
	va_list _ap;
	va_start(_ap, _Format);
	vsnprintf(_szbuf, 512, _Format, _ap);
	va_end(_ap);
#if defined(BL_PLATFORM_WIN32)
	MultiByteToWideChar(CP_UTF8, 0, _szbuf, -1, _wszbuf, sizeof(_wszbuf));
	OutputDebugStringW(L"BULLLORD DEBUG INFORMATION > ");
	OutputDebugStringW(_wszbuf);
	OutputDebugStringW(L"\n");
#elif defined(BL_PLATFORM_UWP)
	MultiByteToWideChar(CP_UTF8, 0, _szbuf, -1, _wszbuf, sizeof(_wszbuf));
	OutputDebugString(L"BULLLORD DEBUG INFORMATION > ");
	OutputDebugString(_wszbuf);
	OutputDebugString(L"\n");
#elif defined(BL_PLATFORM_LINUX)
	printf("BULLLORD DEBUG INFORMATION > %s\n", _szbuf);
#elif defined(BL_PLATFORM_ANDROID)
	__android_log_print(ANDROID_LOG_INFO, "", "BULLLORD DEBUG INFORMATION > %s", _szbuf);
#elif defined(BL_PLATFORM_OSX)
    printf("BULLLORD DEBUG INFORMATION > %s\n", _szbuf);
#elif defined(BL_PLATFORM_IOS)
	printf("BULLLORD DEBUG INFORMATION > %s\n", _szbuf);
#elif defined(BL_PLATFORM_WEB)
	printf("BULLLORD DEBUG INFORMATION > %s\n", _szbuf);
#endif
#endif
}
const BLAnsi*
blMD5String(IN BLAnsi* _Str)
{
	BLU32 _i, _j;
	BLU8 _bits[8];
	BLU32 _index = 0;
	BLU32 _padlen;
	BLU32 _len = (BLU32)strlen(_Str);
	_PrUtilMem->sContext.aCount[0] = _PrUtilMem->sContext.aCount[1] = 0;
	_PrUtilMem->sContext.aState[0] = 0x67452301;
	_PrUtilMem->sContext.aState[1] = 0xefcdab89;
	_PrUtilMem->sContext.aState[2] = 0x98badcfe;
	_PrUtilMem->sContext.aState[3] = 0x10325476;
	_MD5Update((BLU8*)_Str, _len);
	for (_i = 0, _j = 0; _j < 8; _i++, _j += 4)
	{
		_bits[_j] = (BLU8)(_PrUtilMem->sContext.aCount[_i] & 0xff);
		_bits[_j + 1] = (BLU8)((_PrUtilMem->sContext.aCount[_i] >> 8) & 0xff);
		_bits[_j + 2] = (BLU8)((_PrUtilMem->sContext.aCount[_i] >> 16) & 0xff);
		_bits[_j + 3] = (BLU8)((_PrUtilMem->sContext.aCount[_i] >> 24) & 0xff);
	}
	_index = (BLU32)((_PrUtilMem->sContext.aCount[0] >> 3) & 0x3f);
	_padlen = (_index < 56) ? (56 - _index) : (120 - _index);
	BLU8 _padding[64] = { 0 };
	_padding[0] = 0x80;
	_MD5Update(_padding, _padlen);
	_MD5Update(_bits, 8);
	BLU8 _digest[16];
	for (_i = 0, _j = 0; _j < 16; _i++, _j += 4)
	{
		_digest[_j] = (BLU8)(_PrUtilMem->sContext.aState[_i] & 0xff);
		_digest[_j + 1] = (BLU8)((_PrUtilMem->sContext.aState[_i] >> 8) & 0xff);
		_digest[_j + 2] = (BLU8)((_PrUtilMem->sContext.aState[_i] >> 16) & 0xff);
		_digest[_j + 3] = (BLU8)((_PrUtilMem->sContext.aState[_i] >> 24) & 0xff);
	}
	memset(&_PrUtilMem->sContext, 0, sizeof(_BLCtx));
	memset(_PrUtilMem->aMd5Buf, 0, sizeof(_PrUtilMem->aMd5Buf));
	for (_i = 0; _i<16; _i++)
		sprintf(_PrUtilMem->aMd5Buf, "%s%02x", _PrUtilMem->aMd5Buf, _digest[_i]);
	return _PrUtilMem->aMd5Buf;
}
BLS32
blNatCompare(IN BLAnsi* _StrA, IN BLAnsi* _StrB)
{
	BLS32 _ai, _bi, _fractional, _result;
	BLAnsi _ca, _cb;
	_ai = _bi = 0;
	while (1)
	{
		_ca = _StrA[_ai];
		_cb = _StrB[_bi];
		while (isspace(_ca))
			_ca = _StrA[++_ai];
		while (isspace(_cb))
			_cb = _StrB[++_bi];
		if (isdigit(_ca) && isdigit(_cb))
		{
			_fractional = (_ca == '0' || _cb == '0');
			if (_fractional)
			{
				BLAnsi* _tmpa = (BLAnsi*)_StrA + _ai;
				BLAnsi* _tmpb = (BLAnsi*)_StrB + _bi;
				for (;; _tmpa++, _tmpb++)
				{
					if (!isdigit(*_tmpa) && !isdigit(*_tmpb))
					{
						_result = 0;
						break;
					}
					if (!isdigit(*_tmpa))
					{
						_result = -1;
						break;
					}
					if (!isdigit(*_tmpb))
					{
						_result = +1;
						break;
					}
					if (*_tmpa < *_tmpb)
					{
						_result = -1;
						break;
					}
					if (*_tmpa > *_tmpb)
					{
						_result = +1;
						break;
					}
				}
				if (_result)
					return _result;
			}
			else
			{
				BLS32 _bias = 0;
				BLAnsi* _tmpa = (BLAnsi*)_StrA + _ai;
				BLAnsi* _tmpb = (BLAnsi*)_StrB + _bi;
				for (;; _tmpa++, _tmpb++)
				{
					if (!isdigit(*_tmpa) && !isdigit(*_tmpb))
					{
						_result = _bias;
						break;
					}
					if (!isdigit(*_tmpa))
					{
						_result = -1;
						break;
					}
					if (!isdigit(*_tmpb))
					{
						_result = +1;
						break;
					}
					if (*_tmpa < *_tmpb)
					{
						if (!_bias)
							_bias = -1;
					}
					else if (*_tmpa > *_tmpb)
					{
						if (!_bias)
							_bias = +1;
					}
					else if (!*_tmpa && !*_tmpb)
					{
						_result = _bias;
						break;
					}
				}
				if (_result)
					return _result;
			}
		}
		if (!_ca && !_cb)
			return 0;
		if (_ca < _cb)
			return -1;
		if (_ca > _cb)
			return +1;
		++_ai; ++_bi;
	}
	return 0;
}
BLU32
blHashString(IN BLUtf8* _Str)
{
	if (!_Str)
		return 0;
	const BLUtf8* _tmp = _Str;
	BLU32 _cnt = 0;
	while (*_tmp++)
		_cnt++;
	BLU32 _crc32val = 0;
	for (BLU32 _i = 0; _i < _cnt; ++_i)
		_crc32val = CRCCODE_INTERNAL[(_crc32val ^ _Str[_i]) & 0xFF] ^ (_crc32val >> 8);
	return _crc32val;
}
BLU32
blUtf8Length(IN BLUtf8* _Str)
{
	if (!_Str)
		return 0;
	const BLUtf8* _tmp = _Str;
	BLU32 _cnt = 0;
	while (*_tmp++)
		_cnt++;
	return _cnt;
}
BLU32
blUtf16Length(IN BLUtf16* _Str)
{
	if (!_Str)
		return 0;
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	while (*_tmp++)
		_cnt++;
	return _cnt;
}
BLBool
blUtf8Equal(IN BLUtf8* _Str1, IN BLUtf8* _Str2)
{
	if (!_Str1 || !_Str2)
		return FALSE;
	BLUtf8* _src = (BLUtf8*)_Str1;
	BLUtf8* _dst = (BLUtf8*)_Str2;
	BLS32 _ret = 0;
	while (!(_ret = (BLS32)(*_src - *_dst)) && *_dst)
		++_src, ++_dst;
	if (_ret < 0)
		return FALSE;
	else if (_ret > 0)
		return FALSE;
	return TRUE;
}
BLBool
blUtf16Equal(IN BLUtf16* _Str1, IN BLUtf16* _Str2)
{
	if (!_Str1 || !_Str2)
		return FALSE;
	BLUtf16* _src = (BLUtf16*)_Str1;
	BLUtf16* _dst = (BLUtf16*)_Str2;
	BLS32 _ret = 0;
	while (!(_ret = (BLS32)(*_src - *_dst)) && *_dst)
		++_src, ++_dst;
	if (_ret < 0)
		return FALSE;
	else if (_ret > 0)
		return FALSE;
	return TRUE;
}
BLVoid
blSwitchLowerUtf8(INOUT BLUtf8* _Str)
{
	if (!_Str)
		return;
	const BLUtf8* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _i = 0;
	while (*_tmp++)
		_cnt++;
	for (_i = 0; _i < _cnt; ++_i)
		_Str[_i] = tolower(_Str[_i]);
}
BLVoid
blSwitchLowerUtf16(INOUT BLUtf16* _Str)
{
	if (!_Str)
		return;
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _i = 0;
	while (*_tmp++)
		_cnt++;
	for (_i = 0; _i < _cnt; ++_i)
		_Str[_i] = tolower(_Str[_i]);
}
BLVoid
blSwitchUpperUtf8(INOUT BLUtf8* _Str)
{
	if (!_Str)
		return;
	const BLUtf8* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _i = 0;
	while (*_tmp++)
		_cnt++;
	for (_i = 0; _i < _cnt; ++_i)
		_Str[_i] = toupper(_Str[_i]);
}
BLVoid
blSwitchUpperUtf16(INOUT BLUtf16* _Str)
{
	if (!_Str)
		return;
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _i = 0;
	while (*_tmp++)
		_cnt++;
	for (_i = 0; _i < _cnt; ++_i)
		_Str[_i] = toupper(_Str[_i]);
}
BLS32
blUtf8ToInteger(IN BLUtf8* _Str)
{
	if (!_Str)
		return 0;
	return (BLS32)strtol((BLAnsi*)_Str, NULL, 0);
}
BLS32
blUtf16ToInteger(IN BLUtf16* _Str)
{
	if (!_Str)
		return 0;
	BLUtf8 _buf[32] = {0};
	BLUtf8* _dest = _buf;
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _destcapacity = 32;
	BLU32 _idx;
	while (*_tmp++)
		_cnt++;
	for (_idx = 0; _idx < _cnt; ++_idx)
	{
		BLUtf16 _cp = _Str[_idx];
		BLU32 _ensz;
		if (_cp < 0x80)
			_ensz = 1;
		else if (_cp < 0x0800)
			_ensz = 2;
		else if (_cp < (BLUtf16)0x10000)
			_ensz = 3;
		else
			_ensz = 4;
		if (_destcapacity < _ensz)
			break;
		if (_cp < 0x80)
		{
			*_dest++ = (char)_cp;
			--_destcapacity;
		}
		else if (_cp < 0x0800)
		{
			*_dest++ = (char)((_cp >> 6) | 0xC0);
			*_dest++ = (char)((_cp & 0x3F) | 0x80);
			_destcapacity -= 2;
		}
		else if (_cp < (BLUtf16)0x10000)
		{
			*_dest++ = (char)((_cp >> 12) | 0xE0);
			*_dest++ = (char)(((_cp >> 6) & 0x3F) | 0x80);
			*_dest++ = (char)((_cp & 0x3F) | 0x80);
			_destcapacity -= 3;
		}
		else
		{
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			_destcapacity -= 4;
		}
	}
	return (BLS32)strtol((BLAnsi*)_buf, NULL, 0);
}
BLU32
blUtf8ToUInteger(IN BLUtf8* _Str)
{
	if (!_Str)
		return 0;
	return (BLU32)strtoul((BLAnsi*)_Str, NULL, 0);
}
BLU32
blUtf16ToUInteger(IN BLUtf16* _Str)
{
	if (!_Str)
		return 0;
	BLUtf8 _buf[32] = { 0 };
	BLUtf8* _dest = _buf;
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _destcapacity = 32;
	BLU32 _idx;
	while (*_tmp++)
		_cnt++;
	for (_idx = 0; _idx < _cnt; ++_idx)
	{
		BLUtf16 _cp = _Str[_idx];
		BLU32 _ensz;
		if (_cp < 0x80)
			_ensz = 1;
		else if (_cp < 0x0800)
			_ensz = 2;
		else if (_cp < (BLUtf16)0x10000)
			_ensz = 3;
		else
			_ensz = 4;
		if (_destcapacity < _ensz)
			break;
		if (_cp < 0x80)
		{
			*_dest++ = (char)_cp;
			--_destcapacity;
		}
		else if (_cp < 0x0800)
		{
			*_dest++ = (char)((_cp >> 6) | 0xC0);
			*_dest++ = (char)((_cp & 0x3F) | 0x80);
			_destcapacity -= 2;
		}
		else if (_cp < (BLUtf16)0x10000)
		{
			*_dest++ = (char)((_cp >> 12) | 0xE0);
			*_dest++ = (char)(((_cp >> 6) & 0x3F) | 0x80);
			*_dest++ = (char)((_cp & 0x3F) | 0x80);
			_destcapacity -= 3;
		}
		else
		{
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			_destcapacity -= 4;
		}
	}
	return (BLU32)strtoul((BLAnsi*)_buf, NULL, 0);
}
BLF32
blUtf8ToFloat(IN BLUtf8* _Str)
{
	if (!_Str)
		return 0.f;
	return (BLF32)strtod((BLAnsi*)_Str, NULL);
}
BLF32
blUtf16ToFloat(IN BLUtf16* _Str)
{
	if (!_Str)
		return 0.f;
	BLUtf8 _buf[32] = { 0 };
	BLUtf8* _dest = _buf;
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	BLU32 _destcapacity = 32;
	BLU32 _idx;
	while (*_tmp++)
		_cnt++;
	for (_idx = 0; _idx < _cnt; ++_idx)
	{
		BLUtf16 _cp = _Str[_idx];
		BLU32 _ensz;
		if (_cp < 0x80)
			_ensz = 1;
		else if (_cp < 0x0800)
			_ensz = 2;
		else
			_ensz = 3;
		if (_destcapacity < _ensz)
			break;
		if (_cp < 0x80)
		{
			*_dest++ = (char)_cp;
			--_destcapacity;
		}
		else if (_cp < 0x0800)
		{
			*_dest++ = (char)((_cp >> 6) | 0xC0);
			*_dest++ = (char)((_cp & 0x3F) | 0x80);
			_destcapacity -= 2;
		}
		else if (_cp < (BLUtf16)0x10000)
		{
			*_dest++ = (char)((_cp >> 12) | 0xE0);
			*_dest++ = (char)(((_cp >> 6) & 0x3F) | 0x80);
			*_dest++ = (char)((_cp & 0x3F) | 0x80);
			_destcapacity -= 3;
		}
		else
		{
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			*_dest++ = (char)'?';
			_destcapacity -= 4;
		}
	}
	return (BLF32)strtod((BLAnsi*)_buf, NULL);
}
const BLUtf8*
blIntegerToUtf8(IN BLS32 _Var)
{
	memset(_PrUtilMem->aU8Buf, 0, sizeof(_PrUtilMem->aU8Buf));
	sprintf((BLAnsi*)_PrUtilMem->aU8Buf, "%d", _Var);
	return _PrUtilMem->aU8Buf;
}
const BLUtf16*
blIntegerToUtf16(IN BLS32 _Var)
{
	const BLUtf8* _tmp;
	BLUtf16* _dest;
	BLU32 _idx, _cnt = 0;
	BLU32 destcapacity = 32;
	BLUtf8 _u8buf[32] = { 0 };
	sprintf((BLAnsi*)_u8buf, "%d", _Var);
	_tmp = _u8buf;
	_dest = _PrUtilMem->aU16Buf;
	while (*_tmp++)
		_cnt++;
	for (_idx = 0; ((_idx < _cnt) && (destcapacity > 0));)
	{
		BLUtf16 _cp;
		BLUtf8 _cu = _u8buf[_idx++];
		if (_cu < 0x80)
			_cp = (BLUtf16)(_cu);
		else if (_cu < 0xE0)
		{
			_cp = ((_cu & 0x1F) << 6);
			_cp |= (_u8buf[_idx++] & 0x3F);
		}
		else if (_cu < 0xF0)
		{
			_cp = ((_cu & 0x0F) << 12);
			_cp |= ((_u8buf[_idx++] & 0x3F) << 6);
			_cp |= (_u8buf[_idx++] & 0x3F);
		}
		else
		{
			_cp = L'?';
		}
		*_dest++ = _cp;
		--destcapacity;
	}
	return _PrUtilMem->aU16Buf;
}
const BLUtf8*
blFloatToUtf8(IN BLF32 _Var)
{
	memset(_PrUtilMem->aU8Buf, 0, sizeof(_PrUtilMem->aU8Buf));
	sprintf((BLAnsi*)_PrUtilMem->aU8Buf, "%f", _Var);
	return _PrUtilMem->aU8Buf;
}
const BLUtf16*
blFloatToUtf16(IN BLF32 _Var)
{
	const BLUtf8* _tmp;
	BLUtf16* _dest;
	BLU32 _idx, _cnt = 0;
	BLU32 destcapacity = 32;
	BLUtf8 _u8buf[32] = { 0 };
	sprintf((BLAnsi*)_u8buf, "%f", _Var);
	_tmp = _u8buf;
	_dest = _PrUtilMem->aU16Buf;
	while (*_tmp++)
		_cnt++;
	for (_idx = 0; ((_idx < _cnt) && (destcapacity > 0));)
	{
		BLUtf16 _cp;
		BLUtf8 _cu = _u8buf[_idx++];
		if (_cu < 0x80)
			_cp = (BLUtf16)(_cu);
		else if (_cu < 0xE0)
		{
			_cp = ((_cu & 0x1F) << 6);
			_cp |= (_u8buf[_idx++] & 0x3F);
		}
		else if (_cu < 0xF0)
		{
			_cp = ((_cu & 0x0F) << 12);
			_cp |= ((_u8buf[_idx++] & 0x3F) << 6);
			_cp |= (_u8buf[_idx++] & 0x3F);
		}
		else
		{
			_cp = L'?';
		}
		*_dest++ = _cp;
		--destcapacity;
	}
	return _PrUtilMem->aU16Buf;
}
const BLUtf16*
blGenUtf16Str(IN BLUtf8* _Str)
{
	if (!_Str)
		return NULL;
	BLUtf8 _tcp;
	BLU32 _strlen, _destcapacity, _idx;
	BLUtf16* _ret = NULL;
	const BLUtf8* _tmp = _Str;
	const BLUtf8* _buf = _Str;
	BLUtf16* _temp;
	BLU32 _cnt = 0;
	BLU32 _encsze = 0;
	while (*_tmp++)
		_cnt++;
	_strlen = _cnt;
	while(_cnt--)
	{
		_tcp = *_buf++;
		++_encsze;
		if(_tcp < 0x80)
		{   ;	}
		else if(_tcp < 0xE0)
		{	--_cnt;	++_buf;	}
		else if(_tcp < 0xF0)
		{	_cnt -= 2;	_buf += 2;	}
		else
		{   _cnt -= 2;	_buf += 3;	}
	}
	_ret = _temp = (BLUtf16*)malloc((1 + _encsze) * sizeof(BLUtf16));
	_destcapacity = _encsze;
	for(_idx = 0; ((_idx < _strlen) && (_destcapacity > 0));)
	{
		BLUtf16 _cp;
		BLUtf8 _cu = _Str[_idx++];
		if(_cu < 0x80)
			_cp = (BLUtf16)(_cu);
		else if(_cu < 0xE0)
		{
			_cp = ((_cu & 0x1F) << 6);
			_cp |= (_Str[_idx++] & 0x3F);
		}
		else if(_cu < 0xF0)
		{
			_cp = ((_cu & 0x0F) << 12);
			_cp |= ((_Str[_idx++] & 0x3F) << 6);
			_cp |= (_Str[_idx++] & 0x3F);
		}
		else
		{
			_cp = L'?';
		}
		*_temp++ = _cp;
		--_destcapacity;
	}
	_ret[_encsze] = 0;
	return _ret;
}
BLVoid
blDeleteUtf16Str(INOUT BLUtf16* _Str)
{
	if (!_Str)
		return;
	free(_Str);
	_Str = NULL;
}
const BLUtf8*
blGenUtf8Str(IN BLUtf16* _Str)
{
	if (!_Str)
		return NULL;
	BLUtf8* _ret, *_rettmp;
	const BLUtf16* _tmp = _Str;
	BLU32 _idx, _destcapacity, _cnt = 0, _tmpcnt = 0, _buffsize = 0;
	while (*_tmp++)
		_cnt++;
	_tmpcnt = _cnt;
	_tmp = _Str;
	while(_tmpcnt--)
	{
		if(*_tmp < 0x80)
			_buffsize += 1;
		else if(*_tmp < 0x0800)
			_buffsize += 2;
		else
			_buffsize += 3;
		_tmp++;
	}
	_ret = (BLUtf8*)malloc((1 + _buffsize)*sizeof(BLUtf8));
	_rettmp = (BLUtf8*)_ret;
	_destcapacity = _buffsize;
	for(_idx = 0; _idx < _cnt; ++_idx)
	{
		BLUtf16 _cp = _Str[_idx];
		BLU32 _tmpesz;
		if(_cp < 0x80)
			_tmpesz = 1;
		else if(_cp < 0x0800)
			_tmpesz = 2;
		else
			_tmpesz = 3;
		if(_destcapacity < _tmpesz)
			break;
		if(_cp < 0x80)
		{
			*_rettmp++ = (BLUtf8)_cp;
			--_destcapacity;
		}
		else if(_cp < 0x0800)
		{
			*_rettmp++ = (BLUtf8)((_cp >> 6) | 0xC0);
			*_rettmp++ = (BLUtf8)((_cp & 0x3F) | 0x80);
			_destcapacity -= 2;
		}
		else
		{
			*_rettmp++ = (BLUtf8)((_cp >> 12) | 0xE0);
			*_rettmp++ = (BLUtf8)(((_cp >> 6) & 0x3F) | 0x80);
			*_rettmp++ = (BLUtf8)((_cp & 0x3F) | 0x80);
			_destcapacity -= 3;
		}
	}
	_ret[_buffsize] = 0;
	return _ret;
}
BLVoid
blDeleteUtf8Str(INOUT BLUtf8* _Str)
{
	if (!_Str)
		return;
	free(_Str);
	_Str = NULL;
}
const BLUtf8*
blFileSuffixUtf8(IN BLUtf8* _Filename)
{
	if (!_Filename)
		return NULL;
	const BLUtf8* _tmp = _Filename;
	BLU32 _len = 0;
	BLS32 _i;
	BLBool _periodfound = FALSE;
	BLUtf8* _ext;
	while (*_tmp++)
		_len++;
	_ext = (BLUtf8*)_Filename;
	_ext += _len;
	for (_i = _len; _i >= 0; _i--)
	{
		if (*_ext == '.')
		{
			_periodfound = TRUE;
			break;
		}
		_ext--;
	}
	if (!_periodfound)
		return NULL;
	return _ext + 1;
}
const BLUtf16*
blFileSuffixUtf16(IN BLUtf16* _Filename)
{
	if (!_Filename)
		return NULL;
	BLS32 _i;
	const BLUtf16* _tmp = _Filename;
	BLU32 _len = 0;
	BLBool _periodfound = FALSE;
	BLUtf16* _ext;
	while (*_tmp++)
		_len++;
	_ext = (BLUtf16*)_Filename;
	_ext += _len;
	for (_i = _len; _i >= 0; _i--)
	{
		if (*_ext == L'.')
		{
			_periodfound = TRUE;
			break;
		}
		_ext--;
	}
	if (!_periodfound)
		return NULL;
	return _ext + 1;
}
const BLAnsi*
blGenBase64Encoder(IN BLU8* _Blob, IN BLU32 _Size)
{
    const BLAnsi _b64enc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789" "+/";
    BLU32 _idx, _jdx, _mlen, _rpos, _dif;
    BLU32 _frame = 0;
    BLAnsi _out[5];
    BLAnsi* _res;
    _mlen = 4 * _Size / 3 + 1;
    if (_Size % 3)
        _mlen += 4;
    _res = (BLAnsi*)malloc(_mlen);
    if (!_res)
        return NULL;
    _res[_mlen - 1] = '\0';
    _out[4] = '\0';
    for (_idx = 0; _idx < _Size; _idx += 3)
    {
        _dif = (_Size - _idx) / 3 ? 3 : (_Size - _idx) % 3;
        for (_jdx = 0; _jdx < _dif; ++_jdx)
            memcpy(((BLAnsi*)&_frame) + 2 - _jdx, _Blob + _idx + _jdx, 1);
        for (_jdx = 0; _jdx < _dif + 1; ++_jdx)
        {
            _out[_jdx] = (BLAnsi)((_frame & 0xFC0000) >> 18);
            _out[_jdx] = _b64enc[(BLS32)_out[_jdx]];
            _frame = _frame << 6;
        }
        if (_dif == 1)
            _out[2] = _out [3] = '=';
        else if (_dif == 2)
            _out [3] = '=';
        _rpos = (_idx / 3) * 4;
        strcpy(_res + _rpos, _out);
    }
    return _res;
}
BLVoid
blDeleteBase64Encoder(INOUT BLAnsi* _String)
{
	free(_String);
}
const BLU8*
blGenBase64Decoder(IN BLAnsi* _String, OUT BLU32* _Size)
{
    BLU8* _res, _v;
    BLU32 _jdx, _idx;
    BLU32 _in = 0;
    BLU32 _srclen = (BLU32)(strlen(_String));
    if (_srclen % 4)
        return NULL;
    *_Size = (_srclen / 4) * 3;
    _res = (BLU8*)malloc(*_Size);
    if (!_res)
        return NULL;
    for (_idx = 0; _idx < _srclen; _idx += 4)
    {
        _in = 0;
        for (_jdx = 0; _jdx < 4; ++_jdx)
        {
            BLAnsi _c = _String[_idx + _jdx];
            if (_c >= 'A' && _c <= 'Z')
                _v = _c - 'A';
            else if (_c >= 'a' && _c <= 'z')
                _v = _c - 'a' + 26;
            else if (_c >= '0' && _c <= '9')
                _v = _c - '0' + 52;
            else if (_c == '+')
                _v = 62;
            else if (_c == '/')
                _v = 63;
            else if (_c == '=')
                _v = 0;
            else
                _v = 0xFF;
            if (_v == 0xFF)
            {
                free(_res);
                return NULL;
            }
            _in = _in << 6;
            _in += _v;
        }
        for (_jdx = 0; _jdx < 3; ++_jdx)
            memcpy(_res + (_idx / 4) * 3 + _jdx, ((BLU8*)&_in) + 2 - _jdx, 1);
    }
    if (_String[_srclen-1] == '=')
        (*_Size)--;
    if (_String[_srclen-2] == '=')
        (*_Size)--;
    return _res;
}
BLVoid
blDeleteBase64Decoder(INOUT BLU8* _Blob)
{
	free(_Blob);
}
BLU32
blColor4B(IN BLU8 _Rr, IN BLU8 _Gg, IN BLU8 _Bb, IN BLU8 _Aa)
{
    return (_Aa << 24)+(_Bb << 16)+(_Gg << 8)+(_Rr << 0);
}
BLU32
blColor4F(IN BLF32 _Rr, IN BLF32 _Gg, IN BLF32 _Bb, IN BLF32 _Aa)
{
    BLU8 _r = (BLU8)(_Rr * 255 + 0.5f);
    BLU8 _g = (BLU8)(_Gg * 255 + 0.5f);
    BLU8 _b = (BLU8)(_Bb * 255 + 0.5f);
    BLU8 _a = (BLU8)(_Aa * 255 + 0.5f);
    return (_a << 24)+(_b << 16)+(_g << 8)+(_r << 0);
}
BLVoid
blDeColor4B(IN BLU32 _Clr, OUT BLU8 _Clrb[4])
{
    _Clrb[3] = (BLU8)(_Clr >> 24);
    _Clrb[2] = (BLU8)(_Clr >> 16);
    _Clrb[1] = (BLU8)(_Clr >>  8);
    _Clrb[0] = (BLU8)(_Clr >>  0);
}
BLVoid
blDeColor4F(IN BLU32 _Clr, OUT BLF32 _Clrf[4])
{
    BLF32 _inv = 1.0f / 255.0f;
	BLU8 _a = (BLU8)(_Clr >> 24);
	BLU8 _b = (BLU8)(_Clr >> 16);
	BLU8 _g = (BLU8)(_Clr >> 8);
	BLU8 _r = (BLU8)(_Clr >> 0);
	_Clrf[3] = _inv * _a;
	_Clrf[2] = _inv * _b;
	_Clrf[1] = _inv * _g;
	_Clrf[0] = _inv * _r;
}
BLVoid
blRLEDecode(IN BLU8* _Src, IN BLU32 _Dstlen, INOUT BLU8* _Dst)
{
	BLU8 *_ip = (BLU8*)_Src, *_op = _Dst;
	BLU32 _idx;
	BLS32 _c, _pc = -1;
	BLS32 _t[256] = { 0 };
	BLS32 _run = 0;
	for (_idx = 0; _idx < 32; ++_idx)
	{
		BLU32 _jdx;
		_c = *_ip++;
		for (_jdx = 0; _jdx < 8; ++_jdx)
			_t[_idx * 8 + _jdx] = (_c >> _jdx) & 1;
	}
	while (_op < _Dst + _Dstlen)
	{
		_c = *_ip++;
		if (_t[_c])
		{
			for (_run = 0; (_pc = *_ip++) == 255; _run += 255);
			_run += _pc + 1;
			for (; _run > 0; --_run)
				*_op++ = _c;
		}
		else
			*(_op++) = _c;
	}
}

