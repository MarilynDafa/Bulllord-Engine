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
#include "../headers/utils.h"
#include "internal/thread.h"
#include "internal/internal.h"
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
typedef struct _UtilsMember {
	_BLMemCache sMemCache;
	BLVoid* aPtrBuf[MEMCACHE_CAP_INTERNAL];
	blMutex* pMutex;
	_BLCtx sContext;
	BLAnsi aMd5Buf[64];
	BLUtf8 aU8Buf[32];
	BLUtf16 aU16Buf[32];
    BLU32 nUriCount;
}_BLUtilsMember;
static _BLUtilsMember* _PrUtilsMem = NULL;
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
	_index = (BLU32)((_PrUtilsMem->sContext.aCount[0] >> 3) & 0x3F);
	if ((_PrUtilsMem->sContext.aCount[0] += ((BLU32)_Inputlen << 3)) < ((BLU32)_Inputlen << 3))
		_PrUtilsMem->sContext.aCount[1]++;
	_PrUtilsMem->sContext.aCount[1] += ((BLU32)_Inputlen >> 29);
	_partlen = 64 - _index;
	if (_Inputlen >= _partlen)
	{
		memcpy(_PrUtilsMem->sContext.aBuffer + _index, _Input, _partlen);
		_MD5Transform(_PrUtilsMem->sContext.aState, _PrUtilsMem->sContext.aBuffer);
		for (_i = _partlen; _i + 63 < _Inputlen; _i += 64)
			_MD5Transform(_PrUtilsMem->sContext.aState, _Input + _i);
		_index = 0;
	}
	else
		_i = 0;
	memcpy(_PrUtilsMem->sContext.aBuffer + _index, _Input + _i, _Inputlen - _i);
}
BLVoid
_UtilsInit()
{
	BLU32 _idx;
	_PrUtilsMem = (_BLUtilsMember*)malloc(sizeof(_BLUtilsMember));
	memset(_PrUtilsMem->aU8Buf, 0, sizeof(_PrUtilsMem->aU8Buf));
	memset(_PrUtilsMem->aU16Buf, 0, sizeof(_PrUtilsMem->aU16Buf));
	memset(_PrUtilsMem->aPtrBuf, 0, sizeof(_PrUtilsMem->aPtrBuf));
	_PrUtilsMem->sMemCache.nUsed = 0;
	_PrUtilsMem->sMemCache.nMax = MEMCACHE_CAP_INTERNAL;
	_PrUtilsMem->sMemCache.pDense = (BLU32*)malloc(_PrUtilsMem->sMemCache.nMax * sizeof(BLU32));
	_PrUtilsMem->sMemCache.pSparse = (BLU32*)malloc(_PrUtilsMem->sMemCache.nMax * sizeof(BLU32));
	for (_idx = 0; _idx < _PrUtilsMem->sMemCache.nMax; ++_idx)
		_PrUtilsMem->sMemCache.pDense[_idx] = _idx;
	_PrUtilsMem->pMutex = blGenMutex();
    _PrUtilsMem->nUriCount = 0;
}
BLVoid
_UtilsStep(BLU32 _Delta)
{
}
BLVoid
_UtilsDestroy()
{
	blDeleteMutex(_PrUtilsMem->pMutex);
	free(_PrUtilsMem->sMemCache.pDense);
	free(_PrUtilsMem->sMemCache.pSparse);
	free(_PrUtilsMem);
}
BLU32
blUniqueUri()
{
    BLAnsi _buf[128] = { 0 };
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
    sprintf(_buf, "%s_%zu", "BulllordEngineUniqueUriCount", _PrUtilsMem->nUriCount);
#elif defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_ANDROID)
    sprintf(_buf, "%s_%zu", "BulllordEngineUniqueUriCount", _PrUtilsMem->nUriCount);
#else
    sprintf(_buf, "%s_%u", "BulllordEngineUniqueUriCount", _PrUtilsMem->nUriCount);
#endif
    return blHashUtf8((const BLUtf8*)_buf);
}
BLGuid
blGenGuid(IN BLVoid* _Ptr, IN BLU32 _Uri)
{
	blMutexLock(_PrUtilsMem->pMutex);
	BLU32 _inptr = 0xFFFFFFFF;
	if (_Ptr)
	{
		if (_PrUtilsMem->sMemCache.nUsed < _PrUtilsMem->sMemCache.nMax)
		{
			BLU32 _index = _PrUtilsMem->sMemCache.nUsed;
			++_PrUtilsMem->sMemCache.nUsed;
			_inptr = _PrUtilsMem->sMemCache.pDense[_index];
			_PrUtilsMem->sMemCache.pSparse[_inptr] = _index;
		}
        else
            blDebugOutput("GUID gen error");
		_PrUtilsMem->aPtrBuf[_inptr] = (BLVoid*)_Ptr;
	}
	BLGuid _gid;
	_gid = MAKEGUID_INTERNAL(_inptr, _Uri);
	blMutexUnlock(_PrUtilsMem->pMutex);
	return _gid;
}
BLVoid
blDeleteGuid(IN BLGuid _ID)
{
	blMutexLock(_PrUtilsMem->pMutex);
	BLU32 _inptr = PTRPART_INTERNAL(_ID);
	if (_inptr != 0xFFFFFFFF)
	{
		BLU32 _index = _PrUtilsMem->sMemCache.pSparse[_inptr];
		--_PrUtilsMem->sMemCache.nUsed;
		BLU32 _temp = _PrUtilsMem->sMemCache.pDense[_PrUtilsMem->sMemCache.nUsed];
		_PrUtilsMem->sMemCache.pDense[_PrUtilsMem->sMemCache.nUsed] = _inptr;
		_PrUtilsMem->sMemCache.pSparse[_temp] = _index;
		_PrUtilsMem->sMemCache.pDense[_index] = _temp;
		_PrUtilsMem->aPtrBuf[_inptr] = NULL;
	}
	blMutexUnlock(_PrUtilsMem->pMutex);
}
BLVoid*
blGuidAsPointer(BLGuid _ID)
{
	BLU32 _inptr = PTRPART_INTERNAL(_ID);
	if (_inptr >= MEMCACHE_CAP_INTERNAL)
        return NULL;
	else
		return _PrUtilsMem->aPtrBuf[_inptr];
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
	_PrUtilsMem->sContext.aCount[0] = _PrUtilsMem->sContext.aCount[1] = 0;
	_PrUtilsMem->sContext.aState[0] = 0x67452301;
	_PrUtilsMem->sContext.aState[1] = 0xefcdab89;
	_PrUtilsMem->sContext.aState[2] = 0x98badcfe;
	_PrUtilsMem->sContext.aState[3] = 0x10325476;
	_MD5Update((BLU8*)_Str, _len);
	for (_i = 0, _j = 0; _j < 8; _i++, _j += 4)
	{
		_bits[_j] = (BLU8)(_PrUtilsMem->sContext.aCount[_i] & 0xff);
		_bits[_j + 1] = (BLU8)((_PrUtilsMem->sContext.aCount[_i] >> 8) & 0xff);
		_bits[_j + 2] = (BLU8)((_PrUtilsMem->sContext.aCount[_i] >> 16) & 0xff);
		_bits[_j + 3] = (BLU8)((_PrUtilsMem->sContext.aCount[_i] >> 24) & 0xff);
	}
	_index = (BLU32)((_PrUtilsMem->sContext.aCount[0] >> 3) & 0x3f);
	_padlen = (_index < 56) ? (56 - _index) : (120 - _index);
	BLU8 _padding[64] = { 0 };
	_padding[0] = 0x80;
	_MD5Update(_padding, _padlen);
	_MD5Update(_bits, 8);
	BLU8 _digest[16];
	for (_i = 0, _j = 0; _j < 16; _i++, _j += 4)
	{
		_digest[_j] = (BLU8)(_PrUtilsMem->sContext.aState[_i] & 0xff);
		_digest[_j + 1] = (BLU8)((_PrUtilsMem->sContext.aState[_i] >> 8) & 0xff);
		_digest[_j + 2] = (BLU8)((_PrUtilsMem->sContext.aState[_i] >> 16) & 0xff);
		_digest[_j + 3] = (BLU8)((_PrUtilsMem->sContext.aState[_i] >> 24) & 0xff);
	}
	memset(&_PrUtilsMem->sContext, 0, sizeof(_BLCtx));
	memset(_PrUtilsMem->aMd5Buf, 0, sizeof(_PrUtilsMem->aMd5Buf));
	for (_i = 0; _i<16; _i++)
		sprintf(_PrUtilsMem->aMd5Buf, "%s%02x", _PrUtilsMem->aMd5Buf, _digest[_i]);
	return _PrUtilsMem->aMd5Buf;
}
BLU32
blHashUtf8(IN BLUtf8* _Str)
{
	BLS8* _data;
	BLS32 _rem;
	BLU32 _h , _tmp;
	const BLUtf8* _stmp = _Str;
	BLU32 _len = 0;
	while (*_stmp++)
		_len++;
	_data = (BLS8*)_Str;
	_h = _len;
	if (_len <= 0 || _data == NULL)
		return 0;
	_rem = _len & 3;
	_len >>= 2;
	for (; _len > 0; _len--)
	{
		_h += *((const BLU16*)_data);
		_tmp = (*((const BLU16*)(_data + 2)) << 11) ^ _h;
		_h = (_h << 16) ^ _tmp;
		_data += 2 * sizeof (BLU16);
		_h += _h >> 11;
	}
	switch (_rem)
	{
	case 3:
		_h += *((const BLU16*)_data);
		_h ^= _h << 16;
		_h ^= ((BLS8)_data[sizeof (BLU16)]) << 18;
		_h += _h >> 11;
		break;
	case 2:
		_h += *((const BLU16*)_data);
		_h ^= _h << 11;
		_h += _h >> 17;
		break;
	case 1:
		_h += (BLS8)*_data;
		_h ^= _h << 10;
		_h += _h >> 1;
		break;
    default:
        break;
	}
	_h ^= _h << 3;
	_h += _h >> 5;
	_h ^= _h << 4;
	_h += _h >> 17;
	_h ^= _h << 25;
	_h += _h >> 6;
	return _h;
}
BLU32
blHashUtf16(IN BLUtf16* _Str)
{
	BLS8* _data;
	BLS32 _rem;
	BLU32 _h , _tmp;
	const BLUtf16* _stmp = _Str;
	BLU32 _len = 0;
	while (*_stmp++)
		_len++;
	_len = 2 * _len;
	_data = (BLS8*)_Str;
	_h = _len;
	if (_len <= 0 || _data == NULL)
		return 0;
	_rem = _len & 3;
	_len >>= 2;
	for (; _len > 0; _len--)
	{
		_h += *((const BLU16*)_data);
		_tmp = (*((const BLU16*)(_data + 2)) << 11) ^ _h;
		_h = (_h << 16) ^ _tmp;
		_data += 2 * sizeof (BLU16);
		_h += _h >> 11;
	}
	switch (_rem)
	{
	case 3:
		_h += *((const BLU16*)_data);
		_h ^= _h << 16;
		_h ^= ((BLS8)_data[sizeof (BLU16)]) << 18;
		_h += _h >> 11;
		break;
	case 2:
		_h += *((const BLU16*)_data);
		_h ^= _h << 11;
		_h += _h >> 17;
		break;
	case 1:
		_h += (BLS8)*_data;
		_h ^= _h << 10;
		_h += _h >> 1;
		break;
    default:
        break;
	}
	_h ^= _h << 3;
	_h += _h >> 5;
	_h ^= _h << 4;
	_h += _h >> 17;
	_h ^= _h << 25;
	_h += _h >> 6;
	return _h;
}
BLU32
blUtf8Length(IN BLUtf8* _Str)
{
	const BLUtf8* _tmp = _Str;
	BLU32 _cnt = 0;
	while (*_tmp++)
		_cnt++;
	return _cnt;
}
BLU32
blUtf16Length(IN BLUtf16* _Str)
{
	const BLUtf16* _tmp = _Str;
	BLU32 _cnt = 0;
	while (*_tmp++)
		_cnt++;
	return _cnt;
}
BLBool
blUtf8Equal(IN BLUtf8* _Str1, IN BLUtf8* _Str2)
{
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
	return (BLS32)strtol((BLAnsi*)_Str, NULL, 0);
}
BLS32
blUtf16ToInteger(IN BLUtf16* _Str)
{
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
	return (BLU32)strtoul((BLAnsi*)_Str, NULL, 0);
}
BLU32
blUtf16ToUInteger(IN BLUtf16* _Str)
{
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
	return (BLF32)strtod((BLAnsi*)_Str, NULL);
}
BLF32
blUtf16ToFloat(IN BLUtf16* _Str)
{
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
	return (BLF32)strtod((BLAnsi*)_buf, NULL);
}
const BLUtf8*
blIntegerToUtf8(IN BLS32 _Var)
{
	memset(_PrUtilsMem->aU8Buf, 0, sizeof(_PrUtilsMem->aU8Buf));
	sprintf((BLAnsi*)_PrUtilsMem->aU8Buf, "%d", _Var);
	return _PrUtilsMem->aU8Buf;
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
	_dest = _PrUtilsMem->aU16Buf;
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
	return _PrUtilsMem->aU16Buf;
}
const BLUtf8*
blFloatToUtf8(IN BLF32 _Var)
{
	memset(_PrUtilsMem->aU8Buf, 0, sizeof(_PrUtilsMem->aU8Buf));
	sprintf((BLAnsi*)_PrUtilsMem->aU8Buf, "%f", _Var);
	return _PrUtilsMem->aU8Buf;
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
	_dest = _PrUtilsMem->aU16Buf;
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
	return _PrUtilsMem->aU16Buf;
}
const BLUtf16*
blGenUtf16Str(IN BLUtf8* _Str)
{
	BLUtf8 _tcp;
	BLU32 _strlen, _destcapacity, _idx;
	const BLUtf16* _ret = NULL;
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
	++_encsze;
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
	*_temp = 0;
	return _ret;
}
BLVoid
blDeleteUtf16Str(INOUT BLUtf16* _Str)
{
	free(_Str);
	_Str = NULL;
}
const BLUtf8*
blGenUtf8Str(IN BLUtf16* _Str)
{
	BLUtf8* _ret, *_rettmp;
	const BLUtf16* _tmp = _Str;
	BLU32 _idx, _destcapacity, _cnt = 0, _tmpcnt = 0, _buffsize = 0;
	while (*_tmp++)
		_cnt++;
	_tmpcnt = _cnt;
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
	_ret = (BLUtf8*)malloc(_buffsize*sizeof(BLUtf8));
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
	*_rettmp = 0;
	return _ret;
}
BLVoid
blDeleteUtf8Str(INOUT BLUtf8* _Str)
{
	free(_Str);
	_Str = NULL;
}
const BLUtf8*
blGetExtNameUtf8(IN BLUtf8* _Filename)
{
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
blGetExtNameUtf16(IN BLUtf16* _Filename)
{
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
            _out[_jdx] = _b64enc[_out[_jdx]];
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
	BLS64 _t[256] = { 0 };
	BLS64 _run = 0;
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
			*_op++ = _c;
	}
}

