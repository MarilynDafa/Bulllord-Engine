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
#include "../headers/network.h"
#include "../headers/util.h"
#include "../headers/system.h"
#include "internal/list.h"
#include "internal/array.h"
#include "internal/internal.h"
#include "../externals/fastlz/fastlz.h"
#include "../externals/duktape/duktape.h"
#include "../externals/mbedtls/rsa.h"
#include "../externals/mbedtls/md.h"
#include "../externals/mbedtls/pk.h"
#include "../externals/mbedtls/entropy.h"
#include "../externals/mbedtls/ctr_drbg.h"
#ifdef EMSCRIPTEN
#	include <emscripten/fetch.h>
#endif
#pragma pack(1)
typedef struct _NetMsg {
	BLU32 nID;
	BLVoid* pBuf;
	BLU32 nLength;
	BLEnum eNetType;
}_BLNetMsg;
typedef struct _HttpJob {
	BLBool bOver;
	BLSocket sSocket;
	BLThread* pThread;
	_BLNetMsg* pMsg;
}_BLHttpJob;
typedef struct  _HttpSect {
	BLAnsi* pHost;
	BLAnsi* pPath;
	BLAnsi* pRemoteName;
	BLAnsi* pLocalName;
	BLU16 nPort;
	BLU32 nStart;
	BLU32 nEnd;
	BLU32 nState;
}_BLHttpSect;
#pragma pack()
typedef struct _NetworkMember {
	DUK_CONTEXT* pDukContext;
	BLAnsi aHostTCP[64];
	BLU16 nPortTCP;
	BLAnsi aHostUDP[64];
	BLU16 nPortUDP;
	BLAnsi aHostHTTP[64];
	BLU16 nPortHTTP;
	BLList* pCriList;
	BLList* pNorList;
	BLList* pRevList;
	BLList* pHttpJobArray;
	BLArray* pDownList;
	BLArray* pLocalList;
	BLU32 nFinish[64];
	BLThread* pConnThread;
	BLThread* pSendThread;
	BLThread* pRecvThread;
	BLThread* pDownMain;
	BLU32 _PrCurDownHash;
	volatile BLU32 nCurDownTotal;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	volatile LONG nCurDownSize;
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
	volatile atomic_int nCurDownSize;
#elif defined(BL_PLATFORM_LINUX)
	volatile int32_t nCurDownSize;
#else
	volatile int32_t nCurDownSize;
#endif
	BLBool bClientIpv6;
    BLBool bTcpServerIpv6;
    BLBool bUdpServerIpv6;
    BLBool bHttpServerIpv6;
	BLBool bConnected;
	BLSocket sTcpSocket;
	BLSocket sUdpSocket;
}_BLNetworkMember;
static _BLNetworkMember* _PrNetworkMem = NULL;
static BLVoid
_FillAddr(const BLAnsi* _Host, BLU16 _Port, struct sockaddr_in* _Out, struct sockaddr_in6* _Out6, BLEnum _Type)
{
    if (_Out6 && !_Out)
    {
        if (!_PrNetworkMem->bTcpServerIpv6 && _Type == BL_NT_TCP)
        {
            struct addrinfo _hints;
            struct addrinfo* _res;
            memset(&_hints, 0, sizeof(_hints));
            _hints.ai_family = PF_UNSPEC;
            _hints.ai_socktype = SOCK_STREAM;
#if defined(WIN32)
			_hints.ai_flags = AI_NUMERICHOST;
#elif defined(EMSCRIPTEN)
			_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
            _hints.ai_flags = AI_DEFAULT;
#endif
            BLS32 _rlt = getaddrinfo("ipv4only.arpa", NULL, &_hints, &_res);
            if (_rlt == 0)
            {
                for (struct addrinfo* _temp = _res; _temp; _temp = _temp->ai_next)
                {
                    if(_temp->ai_family == AF_INET6)
                    {
                        memcpy(_Out6, (struct sockaddr_in6*)_temp->ai_addr, sizeof(struct sockaddr_in6));
#if defined(WIN32)
						unsigned long _ipv4;
						inet_pton(AF_INET, _Host, (void*)&_ipv4);
						memcpy(_Out6->sin6_addr.s6_addr + 12, &_ipv4, sizeof(unsigned long));
#else
                        in_addr_t _ipv4 = inet_addr(_Host);
                        memcpy(_Out6->sin6_addr.s6_addr + 12, &_ipv4, sizeof(in_addr_t));
#endif
                        _Out6->sin6_port = htons(_Port);
                        break;
                    }
                }
            }
            freeaddrinfo(_res);
        }
        else if (_PrNetworkMem->bTcpServerIpv6 && _Type == BL_NT_TCP)
        {
            _Out6->sin6_family = PF_INET6;
            inet_pton(PF_INET6, _Host, (BLVoid*)&_Out6->sin6_addr);
            _Out6->sin6_port = htons(_Port);
        }
        else if (!_PrNetworkMem->bUdpServerIpv6 && _Type == BL_NT_UDP)
        {
            struct addrinfo _hints;
            struct addrinfo* _res;
            memset(&_hints, 0, sizeof(_hints));
            _hints.ai_family = PF_UNSPEC;
            _hints.ai_socktype = SOCK_DGRAM;
#if defined(WIN32)
			_hints.ai_flags = AI_NUMERICHOST;
#elif defined(EMSCRIPTEN)
			_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
			_hints.ai_flags = AI_DEFAULT;
#endif
            BLS32 _rlt = getaddrinfo("ipv4only.arpa", NULL, &_hints, &_res);
            if (_rlt == 0)
            {
                for (struct addrinfo* _temp = _res; _temp; _temp = _temp->ai_next)
                {
                    if(_temp->ai_family == AF_INET6)
                    {
                        memcpy(_Out6, (struct sockaddr_in6*)_temp->ai_addr, sizeof(struct sockaddr_in6));
#if defined(WIN32)
						unsigned long _ipv4;
						inet_pton(AF_INET, _Host, (void*)&_ipv4);
#else
						in_addr_t _ipv4 = inet_addr(_Host);
						memcpy(_Out6->sin6_addr.s6_addr + 12, &_ipv4, sizeof(in_addr_t));
#endif
                        _Out6->sin6_port = htons(_Port);
                        break;
                    }
                }
            }
            freeaddrinfo(_res);
        }
        else if (_PrNetworkMem->bUdpServerIpv6 && _Type == BL_NT_UDP)
        {
            _Out6->sin6_family = PF_INET6;
            inet_pton(PF_INET6, _Host, (BLVoid*)&_Out6->sin6_addr);
            _Out6->sin6_port = htons(_Port);
        }
        else if (!_PrNetworkMem->bHttpServerIpv6 && _Type == BL_NT_HTTP)
        {
            struct addrinfo _hints;
            struct addrinfo* _res;
            memset(&_hints, 0, sizeof(_hints));
            _hints.ai_family = PF_UNSPEC;
            _hints.ai_socktype = SOCK_STREAM;
#if defined(WIN32)
			_hints.ai_flags = AI_NUMERICHOST;
#elif defined(EMSCRIPTEN)
			_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
			_hints.ai_flags = AI_DEFAULT;
#endif
            BLS32 _rlt = getaddrinfo("ipv4only.arpa", NULL, &_hints, &_res);
            if (_rlt == 0)
            {
                for (struct addrinfo* _temp = _res; _temp; _temp = _temp->ai_next)
                {
                    if(_temp->ai_family == AF_INET6)
                    {
                        memcpy(_Out6, (struct sockaddr_in6*)_temp->ai_addr, sizeof(struct sockaddr_in6));
#if defined(WIN32)
						unsigned long _ipv4;
						inet_pton(AF_INET, _Host, (void*)&_ipv4);
#else
						in_addr_t _ipv4 = inet_addr(_Host);
						memcpy(_Out6->sin6_addr.s6_addr + 12, &_ipv4, sizeof(in_addr_t));
#endif
                        _Out6->sin6_port = htons(_Port);
                        break;
                    }
                }
            }
            freeaddrinfo(_res);
        }
        else if (_PrNetworkMem->bHttpServerIpv6 && _Type == BL_NT_HTTP)
        {
            _Out6->sin6_family = PF_INET6;
            inet_pton(PF_INET6, _Host, (BLVoid*)&_Out6->sin6_addr);
            _Out6->sin6_port = htons(_Port);
        }
    }
    else if (!_Out6 && _Out)
    {
        if (!_PrNetworkMem->bTcpServerIpv6 && _Type == BL_NT_TCP)
        {
            inet_pton(PF_INET, _Host, (BLVoid*)&_Out->sin_addr);
            _Out->sin_family = PF_INET;
            _Out->sin_port = htons(_Port);
        }
        else if (_PrNetworkMem->bTcpServerIpv6 && _Type == BL_NT_TCP)
        {
        }
        else if (!_PrNetworkMem->bUdpServerIpv6 && _Type == BL_NT_UDP)
        {
            inet_pton(PF_INET, _Host, (BLVoid*)&_Out->sin_addr);
            _Out->sin_family = PF_INET;
            _Out->sin_port = htons(_Port);
        }
        else if (_PrNetworkMem->bUdpServerIpv6 && _Type == BL_NT_UDP)
        {
        }
        else if (!_PrNetworkMem->bHttpServerIpv6 && _Type == BL_NT_HTTP)
        {
            inet_pton(PF_INET, _Host, (BLVoid*)&_Out->sin_addr);
            _Out->sin_family = PF_INET;
            _Out->sin_port = htons(_Port);
        }
        else if (_PrNetworkMem->bHttpServerIpv6 && _Type == BL_NT_HTTP)
        {
        }
    }
}
static BLS32
_GetError()
{
#ifdef BL_PLATFORM_WIN32
	switch (WSAGetLastError())
	{
	case WSAEWOULDBLOCK:
	case WSAENOBUFS:return 0xEA;
	case WSAECONNABORTED:
	case WSAECONNRESET:
	case WSAETIMEDOUT:
	case WSAENETRESET:
	case WSAENOTCONN:return 0xDEAD;
	default:return 0xDEAD;
	}
#else
	if ((errno == EAGAIN) || (errno == EINPROGRESS))
		return 0xEA;
	switch (errno)
	{
	case EINPROGRESS:
	case EWOULDBLOCK:return 0xEA;
	case ECONNABORTED:
	case ECONNRESET:
	case ETIMEDOUT:
	case ENETRESET:
	case ENOTCONN:return 0xDEAD;
	default:return 0xDEAD;
	}
#endif
}
static BLBool
_Select(BLSocket _Sok, BLBool _Recv)
{
	struct timeval _time;
	fd_set _selector;
	FD_ZERO(&_selector);
	FD_SET(_Sok, &_selector);
	_time.tv_sec = 0;
	_time.tv_usec = 150 * 1000;
	if (_Recv)
		return select(0, &_selector, 0, 0, &_time) >= 0;
	else
		return select(0, 0, &_selector, 0, &_time) >= 0;
}
static BLBool
_Send(BLSocket _Sock, _BLNetMsg* _Msg, const BLAnsi* _Header)
{
	if (_Msg->eNetType == BL_NT_UDP)
	{
	}
	else
	{
		BLS32 _nreturn = 0;
		if (_Header)
		{
			BLS32 _sz = (BLS32)strlen(_Header);
			while (_sz > 0)
			{
				_nreturn = (BLS32)send(_Sock, _Header, _sz, 0);
				if (-1 == _nreturn)
				{
					if (_GetError() == 0xEA)
						_Select(_Sock, FALSE);
					else
						return FALSE;
				}
				else
				{
					_Header += _nreturn;
					_sz -= _nreturn;
				}
			}
		}
		BLAnsi* _header = (BLAnsi*)&_Msg->nLength;
		BLS32 _headsz = sizeof(BLU32);
		while (_headsz > 0)
		{
			_nreturn = (BLS32)send(_Sock, _header, _headsz, 0);
			if (-1 == _nreturn)
			{
				if (_GetError() == 0xEA)
					_Select(_Sock, FALSE);
				else
					return FALSE;
			}
			else
			{
				_header += _nreturn;
				_headsz -= _nreturn;
			}
		}
		_header = (BLAnsi*)&_Msg->nID;
		_headsz = sizeof(BLU32);
		while (_headsz > 0)
		{
			_nreturn = (BLS32)send(_Sock, _header, _headsz, 0);
			if (-1 == _nreturn)
			{
				if (_GetError() == 0xEA)
					_Select(_Sock, FALSE);
				else
					return FALSE;
			}
			else
			{
				_header += _nreturn;
				_headsz -= _nreturn;
			}
		}
		_headsz = _Msg->nLength;
		BLU8* _data = (BLU8*)_Msg->pBuf;
		while (_headsz > 0)
		{
			if (!_data)
				break;
			_nreturn = (BLS32)send(_Sock, (BLAnsi*)_data, _headsz, 0);
			if (-1 == _nreturn)
			{
				if (_GetError() == 0xEA)
					_Select(_Sock, FALSE);
				else
					return FALSE;
			}
			else
			{
				_data += _nreturn;
				_headsz -= _nreturn;
			}
		}
	}
	return TRUE;
}
static BLAnsi*
_Recv(BLSocket _Sock, BLU32* _Msgsz)
{
	BLU32 _tempsz = 0;
	BLS32 _nresult = 0;
	BLAnsi* _tempptr = NULL;
	_tempsz = sizeof(BLU32);
	_tempptr = (BLAnsi*)_Msgsz;
	while (_tempsz > 0)
	{
		if (_Select(_Sock, TRUE))
		{
			_nresult = (BLS32)recv(_Sock, _tempptr, _tempsz, 0);
			if (0 == _nresult)
				return NULL;
			if (-1 == _nresult)
				return NULL;
			else
			{
				_tempsz -= _nresult;
				_tempptr += _nresult;
			}
		}
	}
	if (-1 != _nresult)
	{
		BLAnsi* _bufin = (BLAnsi*)malloc(*_Msgsz + *_Msgsz / 64 + 64);
		_tempsz = sizeof(BLU32) + *_Msgsz;
		_tempptr = _bufin;
		while (_tempsz > 0)
		{
			if (_Select(_Sock, TRUE))
			{
				_nresult = (BLS32)recv(_Sock, _tempptr, _tempsz, 0);
				if (0 == _nresult)
				{
					free(_bufin);
					return NULL;
				}
				if (-1 == _nresult)
				{
					free(_bufin);
					return NULL;
				}
				else
				{
					_tempsz -= _nresult;
					_tempptr += _nresult;
				}
			}
		}
		return _bufin;
	}
	return NULL;
}
#if defined(BL_PLATFORM_WEB)
static BLVoid
_OnWGetLoaded(BLU32 _Dummy, BLVoid* _User, BLVoid* _Data, BLU32 _DataSz)
{
	const BLAnsi* _url = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList);
	const BLAnsi* _local = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pLocalList);
	for (BLU32 _idx = 0; _idx < 64; ++_idx)
	{
		if (_PrNetworkMem->nFinish[_idx] == 0)
		{
			_PrNetworkMem->nFinish[_idx] = _PrNetworkMem->_PrCurDownHash;
			break;
		}
	}
	BLS32 _error;
	emscripten_idb_store("/emscriptenfs", _local, _Data, _DataSz, &_error);
	free(_url);
	free(_local);
	blArrayPopFront(_PrNetworkMem->pDownList);
	blArrayPopFront(_PrNetworkMem->pLocalList);
	if (_PrNetworkMem->pDownList->nSize)
		blDownload();
}
static BLVoid
_OnWGetError(BLU32 _Dummy, BLVoid* _User, BLS32 _Error, const BLAnsi* _Stats)
{
	if (_PrNetworkMem->pDownList->nSize)
	{
		const BLAnsi* _url = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList);
		const BLAnsi* _local = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pLocalList);
		free(_url);
		free(_local);
		blArrayPopFront(_PrNetworkMem->pDownList);
		blArrayPopFront(_PrNetworkMem->pLocalList);
		if (_PrNetworkMem->pDownList->nSize)
			blDownload();
	}
}
static BLVoid
_OnWGetProg(BLU32 _Dummy, BLVoid* _User, BLS32 _Down, BLS32 _Total)
{
	_PrNetworkMem->nCurDownSize = _Down;
	_PrNetworkMem->nCurDownTotal = _Total;
}
static BLVoid
_OnWebSocketError(BLS32 _Fd, BLS32 _Err, const BLAnsi* _Msg, BLVoid* _UserData)
{
	if (_PrNetworkMem->sTcpSocket == _Fd)
		_PrNetworkMem->bConnected = FALSE;
}
static BLVoid 
_OnWebSocketMsg(BLS32 _Fd, BLVoid* _UserData)
{
	BLAnsi* _bufin = NULL;
	BLU32 _msgsz = 0;
	_bufin = _Recv(_PrNetworkMem->sTcpSocket, &_msgsz);
	if (_bufin)
	{
		BLU32 _osz = *(BLU32*)_bufin;
		if (_osz == _msgsz)
		{
			BLAnsi* _rp = _bufin + sizeof(unsigned int);
			BLU32 _packsz = 0;
			while (_rp < _bufin + sizeof(unsigned int) + _msgsz)
			{
				BLU32* _rc = (BLU32*)_rp;
				_BLNetMsg* _ele = (_BLNetMsg*)malloc(sizeof(_BLNetMsg));
				_ele->nID = *_rc;
				_rc++;
				_packsz = _ele->nLength = *_rc;
				_rc++;
				_ele->pBuf = malloc(_ele->nLength);
				memcpy(_ele->pBuf, (BLU8*)_rc, _ele->nLength);
				blMutexLock(_PrNetworkMem->pRevList->pMutex);
				blListPushBack(_PrNetworkMem->pRevList, _ele);
				blMutexUnlock(_PrNetworkMem->pRevList->pMutex);
				_rp += _packsz;
			}
		}
		else
		{
			BLU8* _buflzo = (BLU8*)malloc(_osz);
			BLU32 _sz = fastlz_decompress((BLU8*)_bufin + sizeof(unsigned int), _msgsz, _buflzo, _osz);
			if (_sz)
			{
				BLAnsi* _rp = (BLAnsi*)_buflzo;
				BLU32 _packsz = 0;
				while (_rp < (BLAnsi*)_buflzo + _osz)
				{
					BLU32* _rc = (BLU32*)_rp;
					_BLNetMsg* _ele = (_BLNetMsg*)malloc(sizeof(_BLNetMsg));
					_ele->nID = *_rc;
					_rc++;
					_packsz = _ele->nLength = *_rc;
					_rc++;
					_ele->pBuf = malloc(_ele->nLength);
					memcpy(_ele->pBuf, (BLU8*)_rc, _ele->nLength);
					blMutexLock(_PrNetworkMem->pRevList->pMutex);
					blListPushBack(_PrNetworkMem->pRevList, _ele);
					blMutexUnlock(_PrNetworkMem->pRevList->pMutex);
					_rp += _packsz;
				}
				free(_buflzo);
			}
		}
		free(_bufin);
	}
}
#else
static BLU32
_HttpDownloadRequest(BLSocket _Sock, const BLAnsi* _Host, const BLAnsi* _Addr, const BLAnsi* _Filename, BLU32 _Pos, BLU32 _End, BLAnsi _Redirect[1024])
{
	BLBool _responsed = FALSE, _endresponse = FALSE;
	BLU32 _responsesz = 0, _k, _idx = 0, _sz, _nreturn;
	BLAnsi _filesz[32] = { 0 };
	BLAnsi* _tmp;
	BLAnsi* _temp = (BLAnsi*)malloc(512 * sizeof(BLAnsi));
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (_End == 0xFFFFFFFF)
		sprintf_s(_temp, 512, "HEAD %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt)\r\n\r\n", _Addr, _Filename, _Host);
	else
		sprintf_s(_temp, 512, "GET %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt)\r\nConnection: close\r\nRange: bytes=%lu-%lu\r\n\r\n", _Addr, _Filename, _Host, (unsigned long)_Pos, (unsigned long)_End);
#elif defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_ANDROID) || defined(BL_PLATFORM_WEB)
	if (_End == 0xFFFFFFFF)
		sprintf(_temp, "HEAD %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt)\r\n\r\n", _Addr, _Filename, _Host);
	else
		sprintf(_temp, "GET %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt)\r\nConnection: close\r\nRange: bytes=%zu-%zu\r\n\r\n", _Addr, _Filename, _Host, _Pos, _End);
#else
	if (_End == 0xFFFFFFFF)
		sprintf(_temp, "HEAD %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt)\r\n\r\n", _Addr, _Filename, _Host);
	else
		sprintf(_temp, "GET %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt)\r\nConnection: close\r\nRange: bytes=%zu-%zu\r\n\r\n", _Addr, _Filename, _Host, (unsigned long)_Pos, (unsigned long)_End);
#endif
	_sz = (BLU32)strlen(_temp);
	_tmp = _temp;
	while (_sz > 0)
	{
		_nreturn = (BLS32)send(_Sock, _tmp, _sz, 0);
		if (0xFFFFFFFF == _nreturn)
		{
			if (_GetError() == 0xEA)
				_Select(_Sock, FALSE);
			else
			{
				free(_temp);
				return -1;
			}
		}
		else
		{
			_tmp += _nreturn;
			_sz -= _nreturn;
		}
	}
	memset(_temp, 0, 512 * sizeof(BLAnsi));
	if (!_responsed)
	{
		BLAnsi _c = 0;
		while (!_endresponse && _idx < 513)
		{
			if (!_GbSystemRunning)
				return -1;
			if (0 == recv(_Sock, &_c, 1, 0))
			{
				free(_temp);
				return -1;
			}
			_temp[_idx++] = _c;
			if (_idx >= 4)
			{
				if (_temp[_idx - 4] == '\r' && _temp[_idx - 3] == '\n' && _temp[_idx - 2] == '\r' && _temp[_idx - 1] == '\n')
					_endresponse = TRUE;
			}
		}
		if (_idx > 512)
		{
			_responsed = FALSE;
		}
		else
		{
			_temp[_idx] = 0;
			_responsesz = _idx;
			_responsed = TRUE;
		}
	}
	if (!_responsesz)
	{
		free(_temp);
		return -1;
	}
	if (_temp[9] == '4' && _temp[10] == '0' && _temp[11] == '4')
	{
		free(_temp);
		return -1;
	}
	if (_temp[9] == '3' && _temp[10] == '0' && _temp[11] == '2')
	{
		for (_idx = 0; _idx < strlen(_temp); ++_idx)
		{
			if (_temp[_idx] == 'h')
			{
				if (0 == strncmp(_temp + _idx, "http", 4))
				{
					if (_Redirect)
					{
						for (BLU32 _i = 0; ; ++_i)
						{
							if (_temp[_idx] == '\r' && _temp[1 + _idx] == '\n')
								break;
							_Redirect[_i] = _temp[_idx];
							++_idx;
						}
						free(_temp);
						return 0;
					}
					else
					{
						free(_temp);
						return -1;
					}
				}
			}
		}
	}
	for (_idx = 0; _idx < strlen(_temp); ++_idx)
	{
		if (_temp[_idx] == 'C')
		{
			if (0 == strncmp(_temp + _idx, "Content-Length", 14))
			{
				break;
			}
		}
	}
	_idx += 14;
	_k = _idx;
	for (; _idx <strlen(_temp); ++_idx)
	{
		if (_idx >= 512)
		{
			free(_temp);
			return -1;
		}
		if (_temp[_idx] == '\r' && _temp[1 + _idx] == '\n')
			break;
	}
	if ((BLS32)_idx - (BLS32)_k - 1 > 0)
	{
		strncpy(_filesz, _temp + _k + 1, _idx - _k - 1);
		free(_temp);
		return (BLU32)atoll(_filesz);
	}
	else
	{
		free(_temp);
		return -1;
	}
}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_DownloadWorkerThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_DownloadWorkerThreadFunc(BLVoid* _Userdata)
#endif
{
	struct sockaddr_in _sin;
	struct sockaddr_in6 _sin6;
	struct addrinfo _hint, *_servaddr, *_iter;
	_BLHttpSect* _sect = (_BLHttpSect*)_Userdata;
	BLSocket _httpsok;
	BLU32 _filesz, _sectsz, _len, _sumlen;
	BLBool _fileexist;
	BLAnsi* _buffer = (BLAnsi*)malloc(8192 * sizeof(BLAnsi));
	BLS32 _nodelay = 1, _reuse = 1;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	HANDLE _fp;
#else
	FILE* _fp;
#endif
reconnect:
	_httpsok = socket(_PrNetworkMem->bClientIpv6 ? PF_INET6 : PF_INET, SOCK_STREAM, 0);
	memset(&_hint, 0, sizeof(_hint));
	_hint.ai_family = PF_UNSPEC;
	_hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(_sect->pHost, "http", &_hint, &_servaddr))
        goto failed;
    if (_PrNetworkMem->bClientIpv6)
    {
        for (_iter = _servaddr; _iter != NULL; _iter = _iter->ai_next)
        {
            if (_iter->ai_family == AF_INET6)
                _sin6 = *(struct sockaddr_in6*)_iter->ai_addr;
        }
    }
    else
        _sin = *(struct sockaddr_in*)_servaddr->ai_addr;
	freeaddrinfo(_servaddr);
	if (!_PrNetworkMem->bClientIpv6)
	{
		_sin.sin_port = htons(_sect->nPort);
		if (connect(_httpsok, (struct sockaddr*)&_sin, sizeof(_sin)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (BLU32 _n = 0; _n < 64; ++_n)
				{
					if (_Select(_httpsok, FALSE))
						goto success;
				}
			}
			goto failed;
		}
	}
	else
	{
		_sin6.sin6_port = htons(_sect->nPort);
		if (connect(_httpsok, (struct sockaddr*)&_sin6, sizeof(_sin6)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (BLU32 _n = 0; _n < 64; ++_n)
				{
					if (_Select(_httpsok, FALSE))
						goto success;
				}
			}
			goto failed;
		}
    }
    _nodelay = 1, _reuse = 1;
    setsockopt(_httpsok, IPPROTO_TCP, TCP_NODELAY, (BLAnsi*)&_nodelay, sizeof(_nodelay));
    setsockopt(_httpsok, SOL_SOCKET, SO_REUSEADDR, (BLAnsi*)&_reuse, sizeof(_reuse));
    struct linger _lin;
    _lin.l_linger = 0;
    _lin.l_onoff = 1;
    setsockopt(_httpsok, SOL_SOCKET, SO_LINGER, (BLAnsi*)&_lin, sizeof(_lin));
	_filesz = 0;
	_sectsz = (_sect->nEnd) - (_sect->nStart);
	_fileexist = FALSE;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
	WCHAR _wfilename[260] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, _sect->pLocalName, -1, _wfilename, sizeof(_wfilename));
	_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
	_fp = CreateFileA(_sect->pLocalName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#else
	_fp = fopen(_sect->pLocalName, "rb");
#endif
	if (FILE_INVALID_INTERNAL(_fp))
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		LARGE_INTEGER _li;
		GetFileSizeEx(_fp, &_li);
		_filesz = _li.LowPart;
		CloseHandle(_fp);
#else
		fseek(_fp, 0, SEEK_END);
		_filesz = (BLU32)ftell(_fp);
		fseek(_fp, 0, SEEK_SET);
		fclose(_fp);
#endif
		_fileexist = TRUE;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		InterlockedExchangeAdd(&_PrNetworkMem->nCurDownSize, (LONG)_filesz);
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
		atomic_fetch_add(&_PrNetworkMem->nCurDownSize, _filesz);
#elif defined(BL_PLATFORM_LINUX)
		__sync_fetch_and_add(&_PrNetworkMem->nCurDownSize, _filesz);
#elif defined(BL_PLATFORM_ANDROID)
		{
			int32_t _prev, _tmp, _status;
			__asm__ __volatile__("dmb" : : : "memory");
			do {
				__asm__ __volatile__("ldrex %0, [%4]\n"
					"add %1, %0, %5\n"
					"strex %2, %1, [%4]"
					: "=&r" (_prev), "=&r" (_tmp),
					"=&r" (_status), "+m" (*&_PrNetworkMem->nCurDownSize)
					: "r" (&_PrNetworkMem->nCurDownSize), "Ir" (_filesz)
					: "cc");
			} while (__builtin_expect(_status != 0, 0));
		}
#elif defined(BL_PLATFORM_WEB)
		__sync_fetch_and_add(&_PrNetworkMem->nCurDownSize, _filesz);
#else
#error	unsupport platform
#endif
	}
	if (_sectsz == _filesz)
		goto success;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
	if (_fileexist)
		_fp = CreateFile2(_wfilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
	else
		_fp = CreateFile2(_wfilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS, NULL);
#else
	if (_fileexist)
		_fp = CreateFileA(_sect->pLocalName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		_fp = CreateFileA(_sect->pLocalName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#else
	if (_fileexist)
		_fp = fopen(_sect->pLocalName, "r+b");
	else
		_fp = fopen(_sect->pLocalName, "w+b");
#endif
	_HttpDownloadRequest(_httpsok, _sect->pHost, _sect->pPath, _sect->pRemoteName, _sect->nStart + _filesz, _sect->nEnd, NULL);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	LARGE_INTEGER _li;
	GetFileSizeEx(_fp, &_li);
	assert(_li.LowPart == _filesz);
	_li.LowPart = _li.HighPart = 0;
	SetFilePointerEx(_fp, _li, NULL, FILE_END);
#else
	fseek(_fp, 0, SEEK_END);
	assert((BLU32)ftell(_fp) == _filesz);
#endif
	_sumlen = 0;
	memset(_buffer, 0, 8192 * sizeof(BLAnsi));
	while (1)
	{
		if (!_GbSystemRunning)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			CloseHandle(_fp);
#else
			fclose(_fp);
#endif
			goto failed;
		}
		if (_sumlen >= _sectsz - _filesz)
			break;
		memset(_buffer, 0, 8192 * sizeof(BLAnsi));
		_len = (BLU32)recv(_httpsok, _buffer, 8192 * sizeof(BLAnsi), 0);
		if (0xFFFFFFFF == _len)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			CloseHandle(_fp);
#else
			fclose(_fp);
#endif
			goto failed;
		}
		if (_len == 0)
		{
			blYield();
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			CloseHandle(_fp);
			closesocket(_httpsok);
#else
			fclose(_fp);
			close(_httpsok);
#endif
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			InterlockedExchangeAdd(&_PrNetworkMem->nCurDownSize, -(LONG)(_filesz));
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
            atomic_fetch_sub(&_PrNetworkMem->nCurDownSize, _filesz);
#elif defined(BL_PLATFORM_LINUX)
			__sync_fetch_and_sub(&_PrNetworkMem->nCurDownSize, _filesz);
#elif defined(BL_PLATFORM_ANDROID)
			{
				int32_t _prev, _tmp, _status;
				__asm__ __volatile__("dmb" : : : "memory");
				do {
					__asm__ __volatile__("ldrex %0, [%4]\n"
						"add %1, %0, %5\n"
						"strex %2, %1, [%4]"
						: "=&r" (_prev), "=&r" (_tmp),
						"=&r" (_status), "+m" (*&_PrNetworkMem->nCurDownSize)
						: "r" (&_PrNetworkMem->nCurDownSize), "Ir" (-_filesz)
						: "cc");
				} while (__builtin_expect(_status != 0, 0));
			}
#elif defined(BL_PLATFORM_WEB)
			__sync_fetch_and_sub(&_PrNetworkMem->nCurDownSize, _filesz);
#else
#error		unsupport platform
#endif
			goto reconnect;
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		if (_filesz + _sumlen + _len >= _sectsz)
			WriteFile(_fp, _buffer, _sectsz - _filesz - _sumlen, NULL, NULL);
		else
			WriteFile(_fp, _buffer, _len, NULL, NULL);
		FlushFileBuffers(_fp);
#else
		if (_filesz + _sumlen + _len >= _sectsz)
			fwrite(_buffer, _sectsz - _filesz - _sumlen, 1, _fp);
		else
			fwrite(_buffer, _len, 1, _fp);
		fflush(_fp);
#endif
		_sumlen += _len;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		InterlockedExchangeAdd(&_PrNetworkMem->nCurDownSize, (LONG)_len);
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
        atomic_fetch_add(&_PrNetworkMem->nCurDownSize, _len);
#elif defined(BL_PLATFORM_LINUX)
		__sync_fetch_and_add(&_PrNetworkMem->nCurDownSize, _len);
#elif defined(BL_PLATFORM_ANDROID)
		{
			int32_t _prev, _tmp, _status;
			__asm__ __volatile__("dmb" : : : "memory");
			do {
				__asm__ __volatile__("ldrex %0, [%4]\n"
					"add %1, %0, %5\n"
					"strex %2, %1, [%4]"
					: "=&r" (_prev), "=&r" (_tmp),
					"=&r" (_status), "+m" (*&_PrNetworkMem->nCurDownSize)
					: "r" (&_PrNetworkMem->nCurDownSize), "Ir" (_len)
					: "cc");
			} while (__builtin_expect(_status != 0, 0));
		}
#elif defined(BL_PLATFORM_WEB)
		__sync_fetch_and_add(&_PrNetworkMem->nCurDownSize, _len);
#else
#error	unsupport platform
#endif
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	CloseHandle(_fp);
#else
	fclose(_fp);
#endif
success:
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	closesocket(_httpsok);
#else
	close(_httpsok);
#endif
	_sect->nState = -1;
	free(_buffer);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
failed:
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	closesocket(_httpsok);
#else
	close(_httpsok);
#endif
	_sect->nState = 1;
	free(_buffer);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_NetDownMainThread(BLVoid* _Userdata)
#else
static BLVoid*
_NetDownMainThread(BLVoid* _Userdata)
#endif
{
	while (_PrNetworkMem->pDownList->nSize > 0)
	{
		blMutexLock(_PrNetworkMem->pDownList->pMutex);
		blMutexLock(_PrNetworkMem->pLocalList->pMutex);
		BLU16 _port = 80;
		BLS32 _idx = 0;
		BLU32 _n = 0, _filesz;
		struct sockaddr_in _sin;
		struct sockaddr_in6 _sin6;
		BLAnsi _host[1024] = { 0 };
		BLAnsi _path[1024] = { 0 };
		BLAnsi _file[1024] = { 0 };
		BLAnsi _writefile[260] = { 0 };
		BLAnsi _redirect[1024] = { 0 };
		BLSocket _httpsok = INVALID_SOCKET_INTERNAL;
		BLS32 _nodelay = 1, _reuse = 1;
		BLU32 _sectsz;
	redirect:
		_port = 80;
		_idx = 0;
		_n = 0;
		memset(_host, 0, sizeof(_host));
		memset(_path, 0, sizeof(_path));
		memset(_file, 0, sizeof(_file));
		memset(_writefile, 0, sizeof(_writefile));
		const BLAnsi* _url = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList);
		const BLAnsi* _localfile = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pLocalList);
		if (strncmp(_url, "http://", 7) != 0)
			goto failed;
		for (_idx = 7; _idx < (BLS32)strlen(_url) - 7; ++_idx)
		{
			if (_url[_idx] == '/')
				break;
			else
				_host[_idx - 7] = _url[_idx];
		}
		for (_idx = 0; _idx < (BLS32)strlen(_host); ++_idx)
		{
			if (_host[_idx] == ':')
			{
				_port = atoi(_host + _idx + 1);
				_host[_idx] = 0;
				break;
			}
		}
		for (_idx = (BLS32)strlen(_url) - 1; _idx >= 0; --_idx)
		{
			if (_url[_idx] == '/')
				break;
		}
		strncpy(_path, _url, _idx + 1);
		strncpy(_file, _url + _idx + 1, strlen(_url) - _idx - 1);
		strcpy(_writefile, _localfile);
		strcat(_writefile, _file);
		_httpsok = socket(_PrNetworkMem->bClientIpv6 ? PF_INET6 : PF_INET, SOCK_STREAM, 0);
		if (_PrNetworkMem->bClientIpv6)
			_FillAddr(_host, _port, NULL, &_sin6, BL_NT_HTTP);
		else
			_FillAddr(_host, _port, &_sin, NULL, BL_NT_HTTP);
		if (!_PrNetworkMem->bClientIpv6)
		{
			if (connect(_httpsok, (struct sockaddr*)&_sin, sizeof(_sin)) < 0)
			{
				if (0xEA == _GetError())
				{
					for (_n = 0; _n < 64; ++_n)
					{
						if (_Select(_httpsok, FALSE))
							goto success;
					}
				}
				goto failed;
			}
		}
		else
		{
			if (connect(_httpsok, (struct sockaddr*)&_sin6, sizeof(_sin6)) < 0)
			{
				if (0xEA == _GetError())
				{
					for (_n = 0; _n < 64; ++_n)
					{
						if (_Select(_httpsok, FALSE))
							goto success;
					}
				}
				goto failed;
			}
		}
		_nodelay = 1;
		_reuse = 1;
		setsockopt(_httpsok, IPPROTO_TCP, TCP_NODELAY, (BLAnsi*)&_nodelay, sizeof(_nodelay));
		setsockopt(_httpsok, SOL_SOCKET, SO_REUSEADDR, (BLAnsi*)&_reuse, sizeof(_reuse));
		struct linger _lin;
		_lin.l_linger = 0;
		_lin.l_onoff = 1;
		setsockopt(_httpsok, SOL_SOCKET, SO_LINGER, (BLAnsi*)&_lin, sizeof(_lin));
	success:
		_filesz = _HttpDownloadRequest(_httpsok, _host, _path, _file, 0, -1, _redirect);
		if ((BLU32)-1 == _filesz)
			goto failed;
		else if (0 == _filesz)
		{
			BLAnsi* _retmp = (BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList);
			free(_retmp);
			blArrayPopFront(_PrNetworkMem->pDownList);
			_retmp = (BLAnsi*)malloc(strlen(_redirect) + 1);
			strcpy(_retmp, _redirect);
			blArrayPushFront(_PrNetworkMem->pDownList, _retmp);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			closesocket(_httpsok);
#else
			close(_httpsok);
#endif
			goto redirect;
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		closesocket(_httpsok);
#else
		close(_httpsok);
#endif
		_httpsok = 0;
		_BLHttpSect _sects[3];
		_sectsz = _filesz / 3;
		for (_idx = 0; _idx < 3; ++_idx)
		{
			_sects[_idx].pHost = (BLAnsi*)malloc(strlen(_host) + 1);
			strcpy(_sects[_idx].pHost, _host);
			_sects[_idx].pHost[strlen(_host)] = 0;
			_sects[_idx].nPort = _port;
			_sects[_idx].pPath = (BLAnsi*)malloc(strlen(_path) + 1);
			strcpy(_sects[_idx].pPath, _path);
			_sects[_idx].pPath[strlen(_path)] = 0;
			_sects[_idx].pRemoteName = (BLAnsi*)malloc(strlen(_file) + 1);
			strcpy(_sects[_idx].pRemoteName, _file);
			_sects[_idx].pRemoteName[strlen(_file)] = 0;
			_sects[_idx].pLocalName = (BLAnsi*)malloc(strlen(_localfile) + strlen(_file) + 3);
			sprintf(_sects[_idx].pLocalName, "%s%s_%d", _localfile, _file, _idx);
			if (_idx < 2)
			{
				_sects[_idx].nStart = _idx * _sectsz;
				_sects[_idx].nEnd = (_idx + 1) * _sectsz;
			}
			else
			{
				_sects[_idx].nStart = _idx * _sectsz;
				_sects[_idx].nEnd = _filesz;
			}
			_sects[_idx].nState = 0;
		}
		_PrNetworkMem->nCurDownTotal = _filesz;
		_PrNetworkMem->nCurDownSize = 0;
		_PrNetworkMem->_PrCurDownHash = blHashString((const BLUtf8*)_url);
		BLThread* _workthread[3];
		for (_idx = 0; _idx < 3; ++_idx)
		{
			_workthread[_idx] = blGenThread(_DownloadWorkerThreadFunc, NULL, &_sects[_idx]);
			blThreadRun(_workthread[_idx]);
		}
		while (TRUE)
		{
			if (_sects[0].nState == 0xFFFFFFFF && _sects[1].nState == 0xFFFFFFFF && _sects[2].nState == 0xFFFFFFFF)
			{
				blDeleteThread(_workthread[0]);
				blDeleteThread(_workthread[1]);
				blDeleteThread(_workthread[2]);
				break;
			}
			else if (_sects[0].nState == 1 || _sects[1].nState == 1 || _sects[2].nState == 1)
			{
				blDeleteThread(_workthread[0]);
				blDeleteThread(_workthread[1]);
				blDeleteThread(_workthread[2]);
				for (_idx = 0; _idx < 3; ++_idx)
				{
					free(_sects[_idx].pHost);
					free(_sects[_idx].pPath);
					free(_sects[_idx].pRemoteName);
					free(_sects[_idx].pLocalName);
				}
				goto failed;
			}
			else
				blYield();
		}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		HANDLE _fp, _fpread;
#ifdef WINAPI_FAMILY
		WCHAR _wfilename[260] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, _writefile, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS, NULL);
#else
		_fp = CreateFileA(_writefile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		for (_idx = 0; _idx < 3; ++_idx)
		{
			BLU32 _pos = _sects[_idx].nStart;
			LARGE_INTEGER _li;
			_li.HighPart = 0;
			_li.LowPart = _pos;
			SetFilePointerEx(_fp, _li, NULL, FILE_BEGIN);
#ifdef WINAPI_FAMILY
			memset(_wfilename, 0, sizeof(_wfilename));
			MultiByteToWideChar(CP_UTF8, 0, _sects[_idx].pLocalName, -1, _wfilename, sizeof(_wfilename));
			_fpread = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
			_fpread = CreateFileA(_sects[_idx].pLocalName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
			BLU8 _c;
			do
			{
				ReadFile(_fpread, &_c, sizeof(BLU8), NULL, NULL);
				WriteFile(_fp, &_c, sizeof(BLU8), NULL, NULL);
				_pos++;
				if (_pos == _sects[_idx].nEnd)
					break;
			} while (1);
			CloseHandle(_fpread);
		}
		LARGE_INTEGER _li;
		GetFileSizeEx(_fp, &_li);
		if (_li.LowPart != _PrNetworkMem->nCurDownTotal)
			DeleteFileA(_writefile);
		else
		{
			for (_idx = 0; _idx < 3; ++_idx)
				DeleteFileA(_sects[_idx].pLocalName);
		}
		CloseHandle(_fp);
#else
		FILE* _fp, *_fpread;
		_fp = fopen(_writefile, "wb");
		for (_idx = 0; _idx < 3; ++_idx)
		{
			BLU32 _pos = _sects[_idx].nStart;
			fseek(_fp, _pos, SEEK_SET);
			_fpread = fopen(_sects[_idx].pLocalName, "rb");
			BLS32 _c;
			while ((_c = fgetc(_fpread)) != EOF)
			{
				fputc(_c, _fp);
				_pos++;
				if (_pos == _sects[_idx].nEnd)
					break;
			}
			fclose(_fpread);
		}
		if ((BLU32)ftell(_fp) != _PrNetworkMem->nCurDownTotal)
			remove(_writefile);
		else
		{
			for (_idx = 0; _idx < 3; ++_idx)
				remove(_sects[_idx].pLocalName);
		}
		fclose(_fp);
#endif
		for (_idx = 0; _idx < 3; ++_idx)
		{
			free(_sects[_idx].pHost);
			free(_sects[_idx].pPath);
			free(_sects[_idx].pRemoteName);
			free(_sects[_idx].pLocalName);
		}
	failed:
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		if (_httpsok != INVALID_SOCKET_INTERNAL)
			closesocket(_httpsok);
#else
		if (_httpsok != INVALID_SOCKET_INTERNAL)
			close(_httpsok);
#endif
		BLAnsi _filetmp[260] = { 0 };
		for (_idx = (BLS32)strlen((const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList)) - 1; _idx >= 0; --_idx)
		{
			if (((BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList))[_idx] == '/')
			{
				strcpy(_filetmp, (BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList) + _idx + 1);
				break;
			}
		}
		BLAnsi _pathtmp[260] = { 0 };
		strcpy(_pathtmp, (BLAnsi*)blArrayFrontElement(_PrNetworkMem->pLocalList));
		strcat(_pathtmp, _filetmp);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		WIN32_FILE_ATTRIBUTE_DATA _wfad;
		if (GetFileAttributesExA(_pathtmp, GetFileExInfoStandard, &_wfad))
#else
		if (access(_pathtmp, 0) != -1)
#endif
		{
			for (_idx = 0; _idx < 64; ++_idx)
			{
				if (_PrNetworkMem->nFinish[_idx] == 0)
				{
					_PrNetworkMem->nFinish[_idx] = _PrNetworkMem->_PrCurDownHash;
					break;
				}
			}
		}
		BLAnsi* _tmp;
		_tmp = (BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList);
		free(_tmp);
		blArrayPopFront(_PrNetworkMem->pDownList);
		_tmp = (BLAnsi*)blArrayFrontElement(_PrNetworkMem->pLocalList);
		free(_tmp);
		blArrayPopFront(_PrNetworkMem->pLocalList);
		blMutexUnlock(_PrNetworkMem->pLocalList->pMutex);
		blMutexUnlock(_PrNetworkMem->pDownList->pMutex);
		_PrNetworkMem->nCurDownTotal = 0;
		_PrNetworkMem->nCurDownSize = 0;
	}
	_PrNetworkMem->_PrCurDownHash = -1;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
}
#endif
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_NetSocketSendThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_NetSocketSendThreadFunc(BLVoid* _Userdata)
#endif
{
	while (!_PrNetworkMem->pSendThread)
		blYield();
	do
	{
		if (_PrNetworkMem->bConnected)
		{
			while (_PrNetworkMem->pCriList->nSize)
			{
				blMutexLock(_PrNetworkMem->pCriList->pMutex);
				_BLNetMsg* _msg = (_BLNetMsg*)blListFrontElement(_PrNetworkMem->pCriList);
				if (!_Send((_msg->eNetType == BL_NT_UDP) ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, _msg, NULL))
					_PrNetworkMem->bConnected = FALSE;
				blListPopFront(_PrNetworkMem->pCriList);
				free(_msg);
				blMutexUnlock(_PrNetworkMem->pCriList->pMutex);
			}
			if (_PrNetworkMem->pNorList->nSize)
			{
				blMutexLock(_PrNetworkMem->pNorList->pMutex);
				_BLNetMsg* _msg = (_BLNetMsg*)blListFrontElement(_PrNetworkMem->pNorList);
				if (!_Send((_msg->eNetType == BL_NT_UDP) ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, _msg, NULL))
					_PrNetworkMem->bConnected = FALSE;
				blListPopFront(_PrNetworkMem->pNorList);
				free(_msg);
				blMutexUnlock(_PrNetworkMem->pNorList->pMutex);
			}
		}
		blYield();
	} while (_PrNetworkMem->pSendThread->bRunning);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xDEAD;
#else
	return (BLVoid*)0xDEAD;
#endif
}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_NetSocketRecvThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_NetSocketRecvThreadFunc(BLVoid* _Userdata)
#endif
{
	while (!_PrNetworkMem->pRecvThread)
		blYield();
	do
	{
		BLAnsi* _bufin = NULL;
		BLU32 _msgsz = 0;
		_bufin = _Recv(_PrNetworkMem->sTcpSocket, &_msgsz);
		if (_bufin)
		{
			BLU8* _buflzo = (BLU8*)alloca(_msgsz * 4);
			BLU32 _sz = fastlz_decompress((BLU8*)_bufin, _msgsz, _buflzo, _msgsz * 4);
			if (_sz)
			{
				BLAnsi* _rp = (BLAnsi*)_buflzo;
				while (_rp < (BLAnsi*)_buflzo + _sz)
				{
					BLU32* _rc = (BLU32*)_rp;
					_BLNetMsg* _ele = (_BLNetMsg*)malloc(sizeof(_BLNetMsg));
					_ele->nID = *_rc;
					_rc++;
					_ele->nLength = *_rc;
					_rc++;
					_ele->pBuf = malloc(_ele->nLength);
					memcpy(_ele->pBuf, (BLU8*)_rc, _ele->nLength);
					blMutexLock(_PrNetworkMem->pRevList->pMutex);
					blListPushBack(_PrNetworkMem->pRevList, _ele);
					blMutexUnlock(_PrNetworkMem->pRevList->pMutex);
					_rp += _ele->nLength + 2 * sizeof(BLU32);
				}
			}
			free(_bufin);
		}
		else
			_PrNetworkMem->bConnected = FALSE;
		blYield();
	} while (_PrNetworkMem->pRecvThread->bRunning);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_NetSocketConnThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_NetSocketConnThreadFunc(BLVoid* _Userdata)
#endif
{
	BLU32 _n;
	if (_PrNetworkMem->bClientIpv6)
    {
        blYield();
        struct sockaddr_in6 _address;
        _FillAddr(_Userdata ? _PrNetworkMem->aHostUDP : _PrNetworkMem->aHostTCP, _Userdata ? _PrNetworkMem->nPortUDP : _PrNetworkMem->nPortTCP, NULL, &_address, _Userdata ? BL_NT_UDP : BL_NT_TCP);
		if (connect(_Userdata ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, (struct sockaddr*)(&_address), sizeof(_address)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (_n = 0; _n < 64; ++_n)
				{
					if (_Select(_Userdata ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, FALSE))
						goto beginthread;
				}
			}
			goto end;
		}
	}
	else
    {
        blYield();
		struct sockaddr_in _address;
        _FillAddr(_Userdata ? _PrNetworkMem->aHostUDP : _PrNetworkMem->aHostTCP, _Userdata ? _PrNetworkMem->nPortUDP : _PrNetworkMem->nPortTCP, &_address, NULL, _Userdata ? BL_NT_UDP : BL_NT_TCP);
        if (connect(_Userdata ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, (struct sockaddr*)&(_address), sizeof(_address)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (_n = 0; _n < 64; ++_n)
				{
					if (_Select(_Userdata ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, FALSE))
						goto beginthread;
				}
			}
			goto end;
		}
	}
beginthread:
#if defined(BL_PLATFORM_WEB)
	_PrNetworkMem->pSendThread = (BLVoid*)0xFFFFFFFF;
	_PrNetworkMem->pRecvThread = (BLVoid*)0xFFFFFFFF;
#else
	_PrNetworkMem->pSendThread = blGenThread(_NetSocketSendThreadFunc, NULL, NULL);
	_PrNetworkMem->pRecvThread = blGenThread(_NetSocketRecvThreadFunc, NULL, NULL);
	blThreadRun(_PrNetworkMem->pSendThread);
	blThreadRun(_PrNetworkMem->pRecvThread);
#endif
	_PrNetworkMem->bConnected = TRUE;
end:
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_NetHTTPWorkThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_NetHTTPWorkThreadFunc(BLVoid* _Userdata)
#endif
{
	blMutexWait(_PrNetworkMem->pHttpJobArray->pMutex);
	BLU32 _n;
	BLAnsi _httpheader[1024];
	BLAnsi* _bufin = NULL;
	_BLHttpJob* _job = (_BLHttpJob*)_Userdata;
	_BLNetMsg* _msg = _job->pMsg;
	BLU32 _msgsz = 0;
	if (_PrNetworkMem->bClientIpv6)
	{
		struct sockaddr_in6 _address;
		blYield();
		_FillAddr(_PrNetworkMem->aHostHTTP, _PrNetworkMem->nPortHTTP, NULL, &_address, BL_NT_HTTP);
		if (connect(((_BLHttpJob*)_Userdata)->sSocket, (struct sockaddr*)&(_address), sizeof(_address)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (_n = 0; _n < 64; ++_n)
				{
					if (_Select(((_BLHttpJob*)_Userdata)->sSocket, FALSE))
						goto beginwork;
				}
			}
			free(((_BLHttpJob*)_Userdata)->pMsg);
			goto end;
		}
	}
	else
	{
		struct sockaddr_in _address;
		blYield();
		_FillAddr(_PrNetworkMem->aHostHTTP, _PrNetworkMem->nPortHTTP, &_address, NULL, BL_NT_HTTP);
		if (connect(((_BLHttpJob*)_Userdata)->sSocket, (struct sockaddr*)&(_address), sizeof(_address)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (_n = 0; _n < 64; ++_n)
				{
					if (_Select(((_BLHttpJob*)_Userdata)->sSocket, FALSE))
						goto beginwork;
				}
			}
			free(((_BLHttpJob*)_Userdata)->pMsg);
			goto end;
		}
	}
beginwork:
	sprintf(_httpheader, "POST / HTTP/1.1\r\nHost: %s:%d\r\nAccept: */*\r\nConnection: close\r\nContent-Length: %zu\r\n\r\n", _PrNetworkMem->aHostHTTP, _PrNetworkMem->nPortHTTP, _msg->nLength + 2 * sizeof(BLU32));
	_Send(((_BLHttpJob*)_Userdata)->sSocket, _msg, _httpheader);
	blYield();
#if !defined(BL_PLATFORM_WEB)
	_bufin = _Recv(((_BLHttpJob*)_Userdata)->sSocket, &_msgsz);
	if (_bufin)
	{
		BLU8* _buflzo = (BLU8*)alloca(_msgsz * 4);
		BLU32 _sz = fastlz_decompress((BLU8*)_bufin, _msgsz, _buflzo, _msgsz * 4);
		if (_sz)
		{
			BLAnsi* _rp = (BLAnsi*)_buflzo;
			while (_rp < (BLAnsi*)_buflzo + _sz)
			{
				BLU32* _rc = (BLU32*)_rp;
				_BLNetMsg* _ele = (_BLNetMsg*)malloc(sizeof(_BLNetMsg));
				_ele->nID = *_rc;
				_rc++;
				_ele->nLength = *_rc;
				_rc++;
				_ele->pBuf = malloc(_ele->nLength);
				memcpy(_ele->pBuf, (BLU8*)_rc, _ele->nLength);
				blMutexLock(_PrNetworkMem->pRevList->pMutex);
				blListPushBack(_PrNetworkMem->pRevList, _ele);
				blMutexUnlock(_PrNetworkMem->pRevList->pMutex);
				_rp += _ele->nLength + 2 * sizeof(BLU32);
			}
			}
		free(_bufin);
		}
#endif
end:
	((_BLHttpJob*)_Userdata)->bOver = TRUE;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	return 0xdead;
#else
	return (BLVoid*)0xdead;
#endif
}
BLVoid
_NetworkInit(DUK_CONTEXT* _DKC)
{
	_PrNetworkMem = (_BLNetworkMember*)malloc(sizeof(_BLNetworkMember));
	memset(_PrNetworkMem->aHostTCP, 0, sizeof(_PrNetworkMem->aHostTCP));
	_PrNetworkMem->nPortTCP = 0;
	memset(_PrNetworkMem->aHostUDP, 0, sizeof(_PrNetworkMem->aHostUDP));
	_PrNetworkMem->nPortUDP = 0;
	memset(_PrNetworkMem->aHostHTTP, 0, sizeof(_PrNetworkMem->aHostHTTP));
	_PrNetworkMem->nPortHTTP = 0;
	_PrNetworkMem->pDukContext = _DKC;
	_PrNetworkMem->pCriList = NULL;
	_PrNetworkMem->pNorList = NULL;
	_PrNetworkMem->pRevList = NULL;
	_PrNetworkMem->pHttpJobArray = NULL;
	_PrNetworkMem->pDownList = NULL;
	_PrNetworkMem->pLocalList = NULL;
	memset(_PrNetworkMem->nFinish, 0, sizeof(_PrNetworkMem->nFinish));
	_PrNetworkMem->pConnThread = NULL;
	_PrNetworkMem->pSendThread = NULL;
	_PrNetworkMem->pRecvThread = NULL;
	_PrNetworkMem->pDownMain = NULL;
	_PrNetworkMem->_PrCurDownHash = -1;
	_PrNetworkMem->nCurDownTotal = 0;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	_PrNetworkMem->nCurDownSize = 0;
#elif defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
	_PrNetworkMem->nCurDownSize = 0;
#elif defined(BL_PLATFORM_LINUX)
	_PrNetworkMem->nCurDownSize = 0;
#else
	_PrNetworkMem->nCurDownSize = 0;
#endif
    _PrNetworkMem->bClientIpv6 = FALSE;
    _PrNetworkMem->bTcpServerIpv6 = FALSE;
    _PrNetworkMem->bUdpServerIpv6 = FALSE;
    _PrNetworkMem->bHttpServerIpv6 = FALSE;
	_PrNetworkMem->bConnected = FALSE;
	_PrNetworkMem->sTcpSocket = INVALID_SOCKET_INTERNAL;
	_PrNetworkMem->sUdpSocket = INVALID_SOCKET_INTERNAL;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	WSADATA ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
#endif
	_PrNetworkMem->pCriList = blGenList(TRUE);
	_PrNetworkMem->pNorList = blGenList(TRUE);
	_PrNetworkMem->pRevList = blGenList(TRUE);
	_PrNetworkMem->pHttpJobArray = blGenList(TRUE);
	_PrNetworkMem->pDownList = blGenArray(TRUE);
	_PrNetworkMem->pLocalList = blGenArray(TRUE);
	struct addrinfo* _result = NULL;
	struct addrinfo* _curr;
	BLS32 _ret = getaddrinfo("www.bing.com", NULL, NULL, &_result);
	if (_ret == 0)
	{
		for (_curr = _result; _curr != NULL; _curr = _curr->ai_next)
		{
			switch (_curr->ai_family)
			{
			case AF_INET6:
				_PrNetworkMem->bClientIpv6 = TRUE;
				break;
			default:
				break;
			}
		}
	}
	freeaddrinfo(_result);
#if defined(BL_PLATFORM_WEB)
	emscripten_set_socket_message_callback(NULL, _OnWebSocketMsg);
	emscripten_set_socket_error_callback(NULL, _OnWebSocketError);
#endif
	blDebugOutput("Network initialize successfully");
}
BLVoid
_NetworkDestroy()
{
	if (_PrNetworkMem->pConnThread)
	{
		blDeleteThread(_PrNetworkMem->pConnThread);
		_PrNetworkMem->pConnThread = NULL;
	}
	if (_PrNetworkMem->pSendThread)
	{
		blDeleteThread(_PrNetworkMem->pSendThread);
		_PrNetworkMem->pSendThread = NULL;
	}
	if (_PrNetworkMem->pRecvThread)
	{
		blDeleteThread(_PrNetworkMem->pRecvThread);
		_PrNetworkMem->pRecvThread = NULL;
	}
	if (_PrNetworkMem->pDownMain)
	{
		blDeleteThread(_PrNetworkMem->pDownMain);
		_PrNetworkMem->pDownMain = NULL;
	}
	{
		FOREACH_LIST(_BLNetMsg*, _citer, _PrNetworkMem->pCriList)
		{
			free(_citer->pBuf);
			free(_citer);
		}
	}
	{
		FOREACH_LIST(_BLNetMsg*, _niter, _PrNetworkMem->pNorList)
		{
			free(_niter->pBuf);
			free(_niter);
		}
	}
	{
		FOREACH_LIST(_BLNetMsg*, _riter, _PrNetworkMem->pRevList)
		{
			free(_riter->pBuf);
			free(_riter);
		}
	}
	{
		FOREACH_ARRAY(BLAnsi*, _riter, _PrNetworkMem->pDownList)
		{
			free(_riter);
		}
	}
	{
		FOREACH_ARRAY(BLAnsi*, _riter, _PrNetworkMem->pLocalList)
		{
			free(_riter);
		}
	}
	blDeleteList(_PrNetworkMem->pCriList);
	blDeleteList(_PrNetworkMem->pNorList);
	blDeleteList(_PrNetworkMem->pRevList);
	blDeleteArray(_PrNetworkMem->pDownList);
	blDeleteArray(_PrNetworkMem->pLocalList);
	if (_PrNetworkMem->sTcpSocket != INVALID_SOCKET_INTERNAL)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		shutdown(_PrNetworkMem->sTcpSocket, SD_SEND);
		closesocket(_PrNetworkMem->sTcpSocket);
		_PrNetworkMem->sTcpSocket = INVALID_SOCKET_INTERNAL;
#else
		shutdown(_PrNetworkMem->sTcpSocket, SHUT_WR);
		close(_PrNetworkMem->sTcpSocket);
		_PrNetworkMem->sTcpSocket = INVALID_SOCKET_INTERNAL;
#endif
	}
	if (_PrNetworkMem->sUdpSocket != INVALID_SOCKET_INTERNAL)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		shutdown(_PrNetworkMem->sUdpSocket, SD_SEND);
		closesocket(_PrNetworkMem->sUdpSocket);
		_PrNetworkMem->sUdpSocket = INVALID_SOCKET_INTERNAL;
#else
		shutdown(_PrNetworkMem->sUdpSocket, SHUT_WR);
		close(_PrNetworkMem->sUdpSocket);
		_PrNetworkMem->sUdpSocket = INVALID_SOCKET_INTERNAL;
#endif
	}
	FOREACH_LIST(_BLHttpJob*, _iter, _PrNetworkMem->pHttpJobArray)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		shutdown(_iter->sSocket, SD_SEND);
		closesocket(_iter->sSocket);
#else
		shutdown(_iter->sSocket, SHUT_WR);
		close(_iter->sSocket);
#endif
		blDeleteThread(_iter->pThread);
	}
	if (_PrNetworkMem->pHttpJobArray)
	{
		blDeleteList(_PrNetworkMem->pHttpJobArray);
		_PrNetworkMem->pHttpJobArray = NULL;
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	WSACleanup();
#endif
	blDebugOutput("Network shutdown");
	free(_PrNetworkMem);
}
BLVoid
_NetworkStep(BLU32 _Delta)
{
	if (_PrNetworkMem->pSendThread && _PrNetworkMem->pRecvThread)
	{
		if (_PrNetworkMem->pConnThread)
		{
			blDeleteThread(_PrNetworkMem->pConnThread);
			_PrNetworkMem->pConnThread = NULL;
			blInvokeEvent(BL_ET_NET, 0xFFFFFFFF, 0xFFFFFFFF, NULL, INVALID_GUID);
		}
		if (!_PrNetworkMem->bConnected)
		{
			blDeleteThread(_PrNetworkMem->pSendThread);
			_PrNetworkMem->pSendThread = NULL;
			blDeleteThread(_PrNetworkMem->pRecvThread);
			_PrNetworkMem->pRecvThread = NULL;
			blInvokeEvent(BL_ET_NET, 0, 0, NULL, INVALID_GUID);
		}
		blMutexLock(_PrNetworkMem->pRevList->pMutex);
		{
			FOREACH_LIST(_BLNetMsg*, _iter, _PrNetworkMem->pRevList)
			{
				BLVoid* _buf = malloc(_iter->nLength);
				memcpy(_buf, _iter->pBuf, _iter->nLength);
				blInvokeEvent(BL_ET_NET, _iter->nID, _iter->nLength, _buf, INVALID_GUID);
				free(_iter->pBuf);
				free(_iter);
			}
		}
		blClearList(_PrNetworkMem->pRevList);
		blMutexUnlock(_PrNetworkMem->pRevList->pMutex);
	}
	blMutexLock(_PrNetworkMem->pRevList->pMutex);
	{
		FOREACH_LIST(_BLNetMsg*, _iter, _PrNetworkMem->pRevList)
		{
			BLVoid* _buf = malloc(_iter->nLength);
			memcpy(_buf, _iter->pBuf, _iter->nLength);
			blInvokeEvent(BL_ET_NET, _iter->nID, _iter->nLength, _buf, INVALID_GUID);
			free(_iter->pBuf);
			free(_iter);
		}
	}
	blClearList(_PrNetworkMem->pRevList);
	blMutexUnlock(_PrNetworkMem->pRevList->pMutex);
	blMutexLock(_PrNetworkMem->pHttpJobArray->pMutex);
	{
		FOREACH_LIST(_BLHttpJob*, _iter, _PrNetworkMem->pHttpJobArray)
		{
			if (_iter->bOver)
			{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
				shutdown(_iter->sSocket, SD_SEND);
				closesocket(_iter->sSocket);
#else
				shutdown(_iter->sSocket, SHUT_WR);
				close(_iter->sSocket);
#endif
				blDeleteThread(_iter->pThread);
				blListErase(_PrNetworkMem->pHttpJobArray, _iterator_iter);
			}
		}
	}
	blMutexUnlock(_PrNetworkMem->pHttpJobArray->pMutex);
#if defined(BL_PLATFORM_WEB)
	if (_PrNetworkMem->bConnected)
	{
		while (_PrNetworkMem->pCriList->nSize)
		{
			blMutexLock(_PrNetworkMem->pCriList->pMutex);
			_BLNetMsg* _msg = (_BLNetMsg*)blListFrontElement(_PrNetworkMem->pCriList);
			if (!_Send((_msg->eNetType == BL_NT_UDP) ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, _msg, NULL))
				_PrNetworkMem->bConnected = FALSE;
			blListPopFront(_PrNetworkMem->pCriList);
			free(_msg);
			blMutexUnlock(_PrNetworkMem->pCriList->pMutex);
		}
		if (_PrNetworkMem->pNorList->nSize)
		{
			blMutexLock(_PrNetworkMem->pNorList->pMutex);
			_BLNetMsg* _msg = (_BLNetMsg*)blListFrontElement(_PrNetworkMem->pNorList);
			if (!_Send((_msg->eNetType == BL_NT_UDP) ? _PrNetworkMem->sUdpSocket : _PrNetworkMem->sTcpSocket, _msg, NULL))
				_PrNetworkMem->bConnected = FALSE;
			blListPopFront(_PrNetworkMem->pNorList);
			free(_msg);
			blMutexUnlock(_PrNetworkMem->pNorList->pMutex);
		}
	}
#endif
}
BLVoid
blConnect(IN BLAnsi* _Host, IN BLU16 _Port, IN BLEnum _Type)
{
	if (_Type == BL_NT_UDP)
	{
        struct addrinfo _hints;
        memset(&_hints, 0, sizeof(_hints));
        _hints.ai_family = PF_UNSPEC;
        _hints.ai_socktype = SOCK_DGRAM;
#if defined(WIN32)
		_hints.ai_flags = AI_NUMERICHOST;
#elif defined(EMSCRIPTEN)
		_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
		_hints.ai_flags = AI_DEFAULT;
#endif
        struct addrinfo* _answer;
        getaddrinfo(_Host, NULL, &_hints, &_answer);
        for (struct addrinfo* _curr = _answer; _curr != NULL; _curr = _curr->ai_next)
        {
            switch (_curr->ai_family)
            {
                case AF_UNSPEC:
                    break;
                case AF_INET:
                    break;
                case AF_INET6:
                    _PrNetworkMem->bUdpServerIpv6 = TRUE;
                    break;
            }
        }
        freeaddrinfo(_answer);
		memset(_PrNetworkMem->aHostUDP, 0, sizeof(_PrNetworkMem->aHostUDP));
		strcpy(_PrNetworkMem->aHostUDP, _Host);
		_PrNetworkMem->nPortUDP = _Port;
		_PrNetworkMem->sUdpSocket = socket(_PrNetworkMem->bClientIpv6 ? PF_INET6 : PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	else if (_Type == BL_NT_TCP)
	{
        struct addrinfo _hints;
        memset(&_hints, 0, sizeof(_hints));
        _hints.ai_family = PF_UNSPEC;
        _hints.ai_socktype = SOCK_STREAM;
#if defined(WIN32)
		_hints.ai_flags = AI_NUMERICHOST;
#elif defined(EMSCRIPTEN)
		_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
		_hints.ai_flags = AI_DEFAULT;
#endif
        struct addrinfo* _answer;
        getaddrinfo(_Host, NULL, &_hints, &_answer);
        for (struct addrinfo* _curr = _answer; _curr != NULL; _curr = _curr->ai_next)
        {
            switch (_curr->ai_family)
            {
                case AF_UNSPEC:
                    break;
                case AF_INET:
                    break;
                case AF_INET6:
                    _PrNetworkMem->bTcpServerIpv6 = TRUE;
                    break;
            }
        }
        freeaddrinfo(_answer);
		memset(_PrNetworkMem->aHostTCP, 0, sizeof(_PrNetworkMem->aHostTCP));
		strcpy(_PrNetworkMem->aHostTCP, _Host);
		_PrNetworkMem->nPortTCP = _Port;
		BLS32 _nodelay = 1, _reuse = 1;
		struct linger _lin;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		u_long _nonblocking = 1;
#endif
		_PrNetworkMem->sTcpSocket = socket(_PrNetworkMem->bClientIpv6 ? PF_INET6 : PF_INET, SOCK_STREAM, IPPROTO_TCP);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		ioctlsocket(_PrNetworkMem->sTcpSocket, FIONBIO, &_nonblocking);
#else
		fcntl(_PrNetworkMem->sTcpSocket, F_SETFL, fcntl(_PrNetworkMem->sTcpSocket, F_GETFL) | O_NONBLOCK);
#endif
		setsockopt(_PrNetworkMem->sTcpSocket, IPPROTO_TCP, TCP_NODELAY, (BLAnsi*)&_nodelay, sizeof(_nodelay));
		setsockopt(_PrNetworkMem->sTcpSocket, SOL_SOCKET, SO_REUSEADDR, (BLAnsi*)&_reuse, sizeof(_reuse));
		_lin.l_linger = 0;
		_lin.l_onoff = 1;
		setsockopt(_PrNetworkMem->sTcpSocket, SOL_SOCKET, SO_LINGER, (BLAnsi*)&_lin, sizeof(_lin));
#if defined(BL_PLATFORM_WEB)
		_PrNetworkMem->pConnThread = (BLVoid*)0xFFFFFFFF;
		_NetSocketConnThreadFunc(NULL);
#else
		_PrNetworkMem->pConnThread = blGenThread(_NetSocketConnThreadFunc, NULL, NULL);
		blThreadRun(_PrNetworkMem->pConnThread);
#endif
	}
	else
	{
        struct addrinfo _hints;
        memset(&_hints, 0, sizeof(_hints));
        _hints.ai_family = PF_UNSPEC;
        _hints.ai_socktype = SOCK_STREAM;
#if defined(WIN32)
		_hints.ai_flags = AI_NUMERICHOST;
#elif defined(EMSCRIPTEN)
		_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
#else
		_hints.ai_flags = AI_DEFAULT;
#endif
        struct addrinfo* _answer;
        getaddrinfo(_Host, NULL, &_hints, &_answer);
        for (struct addrinfo* _curr = _answer; _curr != NULL; _curr = _curr->ai_next)
        {
            switch (_curr->ai_family)
            {
                case AF_UNSPEC:
                    break;
                case AF_INET:
                    break;
                case AF_INET6:
                    _PrNetworkMem->bHttpServerIpv6 = TRUE;
                    break;
            }
        }
        freeaddrinfo(_answer);
		memset(_PrNetworkMem->aHostHTTP, 0, sizeof(_PrNetworkMem->aHostHTTP));
		strcpy(_PrNetworkMem->aHostHTTP, _Host);
		_PrNetworkMem->nPortHTTP = _Port;
	}
}
BLVoid
blDisconnect()
{
	{
		FOREACH_LIST(_BLNetMsg*, _citer, _PrNetworkMem->pCriList)
		{
			free(_citer->pBuf);
			free(_citer);
		}
	}
	{
		FOREACH_LIST(_BLNetMsg*, _niter, _PrNetworkMem->pNorList)
		{
			free(_niter->pBuf);
			free(_niter);
		}
	}
	{
		FOREACH_LIST(_BLNetMsg*, _riter, _PrNetworkMem->pRevList)
		{
			free(_riter->pBuf);
			free(_riter);
		}
	}
	if (_PrNetworkMem->pSendThread)
	{
		blDeleteThread(_PrNetworkMem->pSendThread);
		_PrNetworkMem->pSendThread = NULL;
	}
	if (_PrNetworkMem->pRecvThread)
	{
		blDeleteThread(_PrNetworkMem->pRecvThread);
		_PrNetworkMem->pRecvThread = NULL;
	}
	if (_PrNetworkMem->pConnThread)
	{
		blDeleteThread(_PrNetworkMem->pConnThread);
		_PrNetworkMem->pConnThread = NULL;
	}
	if (_PrNetworkMem->sTcpSocket != INVALID_SOCKET_INTERNAL)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		shutdown(_PrNetworkMem->sTcpSocket, SD_SEND);
		closesocket(_PrNetworkMem->sTcpSocket);
		_PrNetworkMem->sTcpSocket = INVALID_SOCKET_INTERNAL;
#else
		shutdown(_PrNetworkMem->sTcpSocket, SHUT_WR);
		close(_PrNetworkMem->sTcpSocket);
		_PrNetworkMem->sTcpSocket = INVALID_SOCKET_INTERNAL;
#endif
	}
	if (_PrNetworkMem->sUdpSocket != INVALID_SOCKET_INTERNAL)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		shutdown(_PrNetworkMem->sUdpSocket, SD_SEND);
		closesocket(_PrNetworkMem->sUdpSocket);
		_PrNetworkMem->sUdpSocket = INVALID_SOCKET_INTERNAL;
#else
		shutdown(_PrNetworkMem->sUdpSocket, SHUT_WR);
		close(_PrNetworkMem->sUdpSocket);
		_PrNetworkMem->sUdpSocket = INVALID_SOCKET_INTERNAL;
#endif
	}
}
BLVoid 
blSendNetMsg(IN BLU32 _ID, IN BLAnsi* _JsonData, IN BLBool _Critical, IN BLBool _Overwrite, IN BLEnum _Nettype)
{
	_BLNetMsg* _msg = (_BLNetMsg*)malloc(sizeof(_BLNetMsg));
	_msg->nID = _ID;
	_msg->eNetType = _Nettype;
	_msg->nLength = (BLU32)strlen(_JsonData);
	BLU8* _stream;
	BLU32 _sz;
	_stream = (BLU8*)alloca(((_msg->nLength + (BLU32)(_msg->nLength * 0.06)) > 66) ? (_msg->nLength + (BLU32)(_msg->nLength * 0.06)) : 66);
	_sz = fastlz_compress(_JsonData, _msg->nLength, _stream);
	_msg->pBuf = malloc(_sz);
	memcpy(_msg->pBuf, _stream, _sz);
	_msg->nLength = _sz;
	if (_Critical || _Nettype == BL_NT_HTTP)
	{
		if (_Overwrite && _Nettype != BL_NT_HTTP)
		{
			blMutexLock(_PrNetworkMem->pCriList->pMutex);
			{
				FOREACH_LIST(_BLNetMsg*, _citer, _PrNetworkMem->pCriList)
				{
					if (_citer->nID == _msg->nID)
					{
						free(_citer->pBuf);
						free(_citer);
						blListErase(_PrNetworkMem->pCriList, _iterator_citer);
						break;
					}
				}
			}
			blMutexUnlock(_PrNetworkMem->pCriList->pMutex);
		}		
		if (_Nettype != BL_NT_HTTP)
			blListPushBack(_PrNetworkMem->pCriList, _msg);
	}
	else
	{
		if (_Overwrite)
		{
			blMutexLock(_PrNetworkMem->pNorList->pMutex);
			{
				FOREACH_LIST(_BLNetMsg*, _niter, _PrNetworkMem->pNorList)
				{
					if (_niter->nID == _msg->nID)
					{
						free(_niter->pBuf);
						free(_niter);
						blListErase(_PrNetworkMem->pNorList, _iterator_niter);
						break;
					}
				}
			}
			blMutexUnlock(_PrNetworkMem->pNorList->pMutex);
		}
		blListPushBack(_PrNetworkMem->pNorList, _msg);
	}
	if (_Nettype == BL_NT_HTTP)
	{
		BLS32 _nodelay = 1, _reuse = 1;
		struct linger _lin;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		u_long _nonblocking = 1;
#endif
		_BLHttpJob* _job = (_BLHttpJob*)malloc(sizeof(_BLHttpJob));
		_job->sSocket = socket(_PrNetworkMem->bClientIpv6 ? PF_INET6 : PF_INET, SOCK_STREAM, IPPROTO_TCP);
		_job->bOver = FALSE;
		_job->pMsg = _msg;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		ioctlsocket(_job->sSocket, FIONBIO, &_nonblocking);
#else
		fcntl(_job->sSocket, F_SETFL, fcntl(_job->sSocket, F_GETFL) | O_NONBLOCK);
#endif
		setsockopt(_job->sSocket, IPPROTO_TCP, TCP_NODELAY, (BLAnsi*)&_nodelay, sizeof(_nodelay));
		setsockopt(_job->sSocket, SOL_SOCKET, SO_REUSEADDR, (BLAnsi*)&_reuse, sizeof(_reuse));
		_lin.l_linger = 0;
		_lin.l_onoff = 1;
		setsockopt(_job->sSocket, SOL_SOCKET, SO_LINGER, (BLAnsi*)&_lin, sizeof(_lin));
		blMutexLock(_PrNetworkMem->pCriList->pMutex);
		blListPushBack(_PrNetworkMem->pHttpJobArray, _job);
		blMutexUnlock(_PrNetworkMem->pCriList->pMutex);
#if defined(BL_PLATFORM_WEB)
		_NetHTTPWorkThreadFunc(_job);
#else
		_job->pThread = blGenThread(_NetHTTPWorkThreadFunc, NULL, _job);
		blThreadRun(_job->pThread);
#endif
	}
}
BLBool 
blRSASign(IN BLAnsi* _Input, IN BLAnsi* _PrivateKey, IN BLU32 _Version, OUT BLAnsi _Output[1025])
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_key(&_ctx, (const BLU8*)_PrivateKey, strlen(_PrivateKey) + 1, 0, 0))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_sign";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _hash[32] = { 0 };
	mbedtls_md_type_t _type = _Version ? MBEDTLS_MD_SHA256 : MBEDTLS_MD_SHA1;
	if (mbedtls_md(mbedtls_md_info_from_type(_type), (const BLU8*)_Input, strlen(_Input), _hash))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _buf[1024] = { 0 };
	size_t _olen;
	if (mbedtls_pk_sign(&_ctx, _type, _hash, 0, _buf, &_olen, mbedtls_ctr_drbg_random, &_ctrdrbg))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	memset(_Output, 0, 1025 * sizeof(BLAnsi));
	const BLAnsi* _base64 = blGenBase64Encoder(_buf, (BLU32)_olen);
	strcpy(_Output, _base64);
	blDeleteBase64Encoder((BLAnsi*)_base64);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	return TRUE;
}
BLBool 
blRSAVerify(IN BLAnsi* _Input, IN BLAnsi* _PublicKey, IN BLU32 _Version)
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_public_key(&_ctx, (const BLU8*)_PublicKey, strlen(_PublicKey) + 1))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_verify";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _hash[32] = { 0 };
	mbedtls_md_type_t _type = _Version ? MBEDTLS_MD_SHA256 : MBEDTLS_MD_SHA1;
	if (mbedtls_md(mbedtls_md_info_from_type(_type), (const BLU8*)_Input, strlen(_Input), _hash))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU32 _bufsz;
	const BLU8* _buf = blGenBase64Decoder(_Input, &_bufsz);
	if (mbedtls_pk_verify(&_ctx, _type, _hash, 0, _buf, _bufsz))
	{
		blDeleteBase64Decoder((BLU8*)_buf);
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	blDeleteBase64Decoder((BLU8*)_buf);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	return TRUE;
}
BLBool 
blRSAEncrypt(IN BLAnsi* _Input, IN BLAnsi* _PublicKey, OUT BLAnsi _Output[1025])
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_public_key(&_ctx, (const BLU8*)_PublicKey, strlen(_PublicKey) + 1))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_encrypt";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU8 _buf[1024] = { 0 };
	size_t _olen;
	if (mbedtls_pk_encrypt(&_ctx, (const BLU8*)_Input, strlen(_Input), _buf, &_olen, sizeof(_buf), mbedtls_ctr_drbg_random, &_ctrdrbg))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	memset(_Output, 0, sizeof(BLAnsi) * 1025);
	const BLAnsi* _base64 = blGenBase64Encoder(_buf, (BLU32)_olen);
	strcpy(_Output, _base64);
	blDeleteBase64Encoder((BLAnsi*)_base64);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	return TRUE;
}
BLBool 
blRSADecrypt(IN BLAnsi* _Input, IN BLAnsi* _PrivateKey, OUT BLAnsi _Output[1025])
{
	mbedtls_pk_context _ctx;
	mbedtls_pk_init(&_ctx);
	if (mbedtls_pk_parse_key(&_ctx, (const BLU8*)_PrivateKey, strlen(_PrivateKey) + 1, 0, 0))
	{
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	mbedtls_entropy_context _entropy;
	mbedtls_entropy_init(&_entropy);
	mbedtls_ctr_drbg_context _ctrdrbg;
	mbedtls_ctr_drbg_init(&_ctrdrbg);
	const BLAnsi* _pers = "bulllord_pk_decrypt";
	if (mbedtls_ctr_drbg_seed(&_ctrdrbg, mbedtls_entropy_func, &_entropy, (const BLU8*)_pers, strlen(_pers)))
	{
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	BLU32 _bufsz;
	const BLU8* _buf = blGenBase64Decoder(_Input, &_bufsz);
	BLAnsi _result[1024] = { 0 };
	size_t _olen;
	if (mbedtls_pk_decrypt(&_ctx, _buf, _bufsz, (BLU8*)_result, &_olen, sizeof(_result), mbedtls_ctr_drbg_random, &_ctrdrbg))
	{
		blDeleteBase64Decoder((BLU8*)_buf);
		mbedtls_ctr_drbg_free(&_ctrdrbg);
		mbedtls_entropy_free(&_entropy);
		mbedtls_pk_free(&_ctx);
		return FALSE;
	}
	blDeleteBase64Decoder((BLU8*)_buf);
	mbedtls_ctr_drbg_free(&_ctrdrbg);
	mbedtls_entropy_free(&_entropy);
	mbedtls_pk_free(&_ctx);
	memset(_Output, 0, 1025 * sizeof(BLAnsi));
	memcpy(_Output, _result, _olen);
	return TRUE;
}
BLBool 
blHTTPRequest(IN BLAnsi* _Url, IN BLAnsi* _Param, IN BLBool _Get, OUT BLAnsi _Response[1025])
{
	if (strlen(_Url) >= 1024)
		return FALSE;
	if (strncmp(_Url, "http://", 7) != 0)
		return FALSE;
	BLBool _chunked;
	BLBool _responsed = FALSE, _endresponse = FALSE;
	BLS32 _idx = 0;
	BLU32 _port = 80, _nreturn, _sz, _responsesz = 0;
	struct sockaddr_in _sin;
	struct sockaddr_in6 _sin6;
	struct addrinfo _hint, *_servaddr, *_iter;
	BLAnsi _host[1024] = { 0 };
	BLAnsi* _tmp;
	BLSocket _sok = socket(_PrNetworkMem->bClientIpv6 ? PF_INET6 : PF_INET, SOCK_STREAM, 0);
	memset(&_hint, 0, sizeof(_hint));
	_hint.ai_family = PF_UNSPEC;
	_hint.ai_socktype = SOCK_STREAM;
	for (_idx = 7; _idx < (BLS32)strlen(_Url) - 7; ++_idx)
	{
		if (_Url[_idx] == '/')
			break;
		else
			_host[_idx - 7] = _Url[_idx];
	}
	for (_idx = 0; _idx < (BLS32)strlen(_Url); ++_idx)
	{
		if (_host[_idx] == ':')
		{
			_port = atoi(_host + _idx + 1);
			_host[_idx] = 0;
			break;
		}
	}
	for (_idx = (BLS32)strlen(_Url) - 1; _idx >= 0; --_idx)
	{
		if (_Url[_idx] == '/')
			break;
	}
	if (getaddrinfo(_host, "http", &_hint, &_servaddr))
		return FALSE;
    if (_PrNetworkMem->bClientIpv6)
    {
        for (_iter = _servaddr; _iter != NULL; _iter = _iter->ai_next)
        {
            if (_iter->ai_family == PF_INET6)
                _sin6 = *(struct sockaddr_in6*)_iter->ai_addr;
        }
    }
    else
        _sin = *(struct sockaddr_in*)_servaddr->ai_addr;
	freeaddrinfo(_servaddr);
	if (!_PrNetworkMem->bClientIpv6)
	{
		_sin.sin_port = htons(_port);
		if (connect(_sok, (struct sockaddr*)&_sin, sizeof(_sin)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (_idx = 0; _idx < 64; ++_idx)
				{
					if (_Select(_sok, FALSE))
						goto success;
				}
			}
			goto failed;
		}
	}
	else
	{
		_sin6.sin6_port = htons(_port);
		if (connect(_sok, (struct sockaddr*)&_sin6, sizeof(_sin6)) < 0)
		{
			if (0xEA == _GetError())
			{
				for (_idx = 0; _idx < 64; ++_idx)
				{
					if (_Select(_sok, FALSE))
						goto success;
				}
			}
			goto failed;
		}
	}
success:
	memset(_Response, 0, 1025 * sizeof(BLAnsi));	
	if (_Get)
		sprintf(_Response, "GET %s?%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", _Url, _Param, _host);
	else
		sprintf(_Response, "POST %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %zu\r\n\r\n%s", _Url, _host, strlen(_Url), _Param);
	_sz = (BLU32)strlen(_Response);
	_tmp = _Response;
	while (_sz > 0)
	{
		_nreturn = (BLS32)send(_sok, _tmp, _sz, 0);
		if (0xFFFFFFFF == _nreturn)
		{
			if (_GetError() == 0xEA)
				_Select(_sok, FALSE);
			else
				goto failed;
		}
		else
		{
			_tmp += _nreturn;
			_sz -= _nreturn;
		}
	}
	memset(_Response, 0, sizeof(BLAnsi) * 1025);
	_idx = 0;
	if (!_responsed)
	{
		BLAnsi _c = 0;
		while (!_endresponse && _idx < 1025)
		{
			if (!_GbSystemRunning)
				goto failed;
			if (_Select(_sok, TRUE))
			{
				if (0 == recv(_sok, &_c, 1, 0))
					goto failed;
				_Response[_idx++] = _c;
				if (_idx >= 4)
				{
					if (_Response[_idx - 4] == '\r' && _Response[_idx - 3] == '\n' && _Response[_idx - 2] == '\r' && _Response[_idx - 1] == '\n')
						_endresponse = TRUE;
				}
			}
		}
		_Response[_idx] = 0;
		_responsesz = _idx;
		_responsed = TRUE;
	}
	if (!_responsed)
		goto failed;
	if (!_responsesz)
		goto failed;
	if (_Response[9] != '2' || _Response[10] != '0' || _Response[11] != '0')
		goto failed;
	_responsesz = -1;
	_chunked = TRUE;
	for (_idx = 0; _idx < (BLS32)strlen(_Response); ++_idx)
	{
		if (_Response[_idx] == 'C')
		{
			if (0 == strncmp(_Response + _idx, "Content-Length", 14))
			{
				_chunked = FALSE;
				break;
			}
		}
	}
	if (!_chunked)
	{
		_idx += 14;
		BLAnsi _filesz[32];
		BLS32 _k = _idx;
		for (; _idx < (BLS32)strlen(_Response); ++_idx)
		{
			if (_idx >= 1024)
				goto failed;
			if (_Response[_idx] == '\r' && _Response[1 + _idx] == '\n')
				break;
		}
		if ((BLS32)_idx - (BLS32)_k - 1 > 0)
		{
			strncpy(_filesz, _Response + _k + 1, _idx - _k - 1);
			_responsesz = atoi(_filesz);
		}
		else
			goto failed;
	}
	if (!_chunked && _responsesz >= 1024)
		goto failed;
	memset(_Response, 0, sizeof(BLAnsi) * 1025);
	if (!_chunked)
	{
		_idx = 0;
		BLAnsi _c = 0;
		while (_idx < (BLS32)_responsesz)
		{
			if (!_GbSystemRunning)
				goto failed;
			if (_Select(_sok, TRUE))
			{
				if (0 == recv(_sok, &_c, 1, 0))
					break;
				_Response[_idx++] = _c;
			}
		}
		_Response[_idx] = 0;
	}
	else
	{
		BLAnsi _c = 0;
		BLAnsi _nbtmp[32] = { 0 };
		BLS32 _chunksz;
		_idx = 0;
		while (1)
		{
			if (!_GbSystemRunning)
				goto failed;
			if (_Select(_sok, TRUE))
			{
				if (0 == recv(_sok, &_c, 1, 0))
					break;
				if (_c == '\n')
					break;
				else if (_c == '\r')
					continue;
				else
					_nbtmp[_idx++] = _c;
			}
		}
		_chunksz = atoi(_nbtmp);
		_idx = 0;
		while (_idx < (BLS32)_chunksz)
		{
			if (!_GbSystemRunning)
				goto failed;
			if (_Select(_sok, TRUE))
			{
				if (0 == recv(_sok, &_c, 1, 0))
					break;
				_Response[_idx++] = _c;
			}
		}
		_Response[_idx] = 0;
	}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (_sok)
		closesocket(_sok);
#else
	if (_sok)
		close(_sok);
#endif
	return TRUE;
failed:
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
	if (_sok)
		closesocket(_sok);
#else
	if (_sok)
		close(_sok);
#endif
	return FALSE;
}
BLBool
blAddDownloadList(IN BLAnsi* _Host, IN BLAnsi* _Localpath, OUT BLU32* _Taskid)
{
	if (_PrNetworkMem->pDownList->nSize >= 64)
		return FALSE;
	*_Taskid = -1;
	BLU32 _idx;
	BLU32 _sz = (BLU32)strlen(_Host);
	BLAnsi* _remoteurl = (BLAnsi*)malloc(_sz + 1);
	strcpy(_remoteurl, _Host);
	_remoteurl[_sz] = 0;
	_sz = (BLU32)strlen(_Localpath);
	BLAnsi* _localpath = (BLAnsi*)malloc(260);
	memset(_localpath, 0, sizeof(BLAnsi) * 260);
	strcpy(_localpath, blUserFolderDir());
	strcat(_localpath, _Localpath);
	for (_idx = 0; _idx < strlen(_localpath); ++_idx)
	{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
		if (_localpath[_idx] == '/')
			_localpath[_idx] = '\\';
		if (_Localpath[_idx] == '\\')
		{
			BLAnsi _tmppath[256] = { 0 };
			strncpy(_tmppath, _localpath, _idx);
			CreateDirectoryA(_tmppath, NULL);
		}
#else
		if (_localpath[_idx] == '\\')
			_localpath[_idx] = '/';
		if (_localpath[_idx] == '/')
		{
			BLAnsi _tmppath[256] = { 0 };
			strncpy(_tmppath, _localpath, _idx);
			mkdir(_tmppath, 0755);
		}
#endif
	}
	FOREACH_ARRAY(BLAnsi*, _iter, _PrNetworkMem->pDownList)
	{
		if (strcmp(_iter, _Host) == 0)
		{
			free(_remoteurl);
			free(_localpath);
			return FALSE;
		}
	}
	for (_idx = (BLS32)strlen(_Host) - 1; _idx >= 1; --_idx)
	{
		if (_Host[_idx] == '/')
			break;
	}
	BLAnsi _tmp[260] = { 0 };
	strcpy(_tmp, _localpath);
	strncat(_tmp + strlen(_tmp), _Host + _idx + 1, strlen(_Host) - _idx - 1);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#ifdef WINAPI_FAMILY
	WCHAR _wfilename[260] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, _tmp, -1, _wfilename, sizeof(_wfilename));
	HANDLE _fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
	HANDLE _fp = CreateFileA(_tmp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
	if (FILE_INVALID_INTERNAL(_fp))
	{
		LARGE_INTEGER _li;
		GetFileSizeEx(_fp, &_li);
		BLU32 _filesz = _li.LowPart;
		CloseHandle(_fp);
		BLU32 _shouldsz = 0;
		BLU32 _tmplen = (BLU32)strlen(_tmp);
		_tmp[_tmplen] = '_';
		_tmp[_tmplen + 1] = '1';
#ifdef WINAPI_FAMILY
		memset(_wfilename, 0, sizeof(_wfilename));
		MultiByteToWideChar(CP_UTF8, 0, _tmp, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		_fp = CreateFileA(_tmp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		if (FILE_INVALID_INTERNAL(_fp))
		{
			GetFileSizeEx(_fp, &_li);
			_shouldsz += _li.LowPart;
			CloseHandle(_fp);
		}
		_tmp[_tmplen + 1] = '2';
#ifdef WINAPI_FAMILY
		memset(_wfilename, 0, sizeof(_wfilename));
		MultiByteToWideChar(CP_UTF8, 0, _tmp, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		_fp = CreateFileA(_tmp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		if (_fp)
		{
			GetFileSizeEx(_fp, &_li);
			_shouldsz += _li.LowPart;
			CloseHandle(_fp);
		}
		_tmp[_tmplen + 1] = '3';
#ifdef WINAPI_FAMILY
		memset(_wfilename, 0, sizeof(_wfilename));
		MultiByteToWideChar(CP_UTF8, 0, _tmp, -1, _wfilename, sizeof(_wfilename));
		_fp = CreateFile2(_wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, NULL);
#else
		_fp = CreateFileA(_tmp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
		if (_fp)
		{
			GetFileSizeEx(_fp, &_li);
			_shouldsz += _li.LowPart;
			CloseHandle(_fp);
        }
        if ((_shouldsz == _filesz) || _filesz)
		{
			_tmp[_tmplen + 1] = '1';
			DeleteFileA(_tmp);
			_tmp[_tmplen + 1] = '2';
			DeleteFileA(_tmp);
			_tmp[_tmplen + 1] = '3';
			DeleteFileA(_tmp);
			free(_remoteurl);
			free(_localpath);
			return FALSE;
		}
		else
		{
			_tmp[_tmplen] = 0;
			_tmp[_tmplen + 1] = 0;
			DeleteFileA(_tmp);
		}
	}
#elif defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_ANDROID) || defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_IOS)
	FILE* _fp = fopen(_tmp, "rb");
	if (FILE_INVALID_INTERNAL(_fp))
	{
		fseek(_fp, 0, SEEK_END);
		BLU32 _filesz = (BLU32)ftell(_fp);
		fclose(_fp);
		BLU32 _shouldsz = 0;
		BLU32 _tmplen = (BLU32)strlen(_tmp);
		_tmp[_tmplen] = '_';
		_tmp[_tmplen + 1] = '1';
		_fp = fopen(_tmp, "rb");
		if (_fp)
		{
			fseek(_fp, 0, SEEK_END);
			_shouldsz += ftell(_fp);
			fclose(_fp);
		}
		_tmp[_tmplen + 1] = '2';
		_fp = fopen(_tmp, "rb");
		if (_fp)
		{
			fseek(_fp, 0, SEEK_END);
			_shouldsz += ftell(_fp);
			fclose(_fp);
		}
		_tmp[_tmplen + 1] = '3';
		_fp = fopen(_tmp, "rb");
		if (_fp)
		{
			fseek(_fp, 0, SEEK_END);
			_shouldsz += ftell(_fp);
			fclose(_fp);
		}
		if ((_shouldsz == _filesz) || _filesz)
		{
			_tmp[_tmplen + 1] = '1';
			remove(_tmp);
			_tmp[_tmplen + 1] = '2';
			remove(_tmp);
			_tmp[_tmplen + 1] = '3';
			remove(_tmp);
			free(_remoteurl);
			free(_localpath);
			return FALSE;
		}
		else
		{
			_tmp[_tmplen] = 0;
			_tmp[_tmplen + 1] = 0;
			remove(_tmp);
		}
	}
#endif
	blArrayPushBack(_PrNetworkMem->pDownList, _remoteurl);
	blArrayPushBack(_PrNetworkMem->pLocalList, _localpath);
	*_Taskid = blHashString((const BLUtf8*)_Host);
	return TRUE;
}
BLVoid
blDownload()
{
#if defined(BL_PLATFORM_WEB)
	const BLAnsi* _url = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pDownList);
	const BLAnsi* _local = (const BLAnsi*)blArrayFrontElement(_PrNetworkMem->pLocalList);
	_PrNetworkMem->_PrCurDownHash = blHashString((const BLUtf8*)_url);
	_PrNetworkMem->nCurDownSize = 0;
	_PrNetworkMem->nCurDownTotal = 0;
	emscripten_async_wget2_data(_url, "GET", NULL, NULL, 1, _OnWGetLoaded, _OnWGetError, _OnWGetProg);
#else
	_PrNetworkMem->pDownMain = blGenThread(_NetDownMainThread, NULL, NULL);
	blThreadRun(_PrNetworkMem->pDownMain);
#endif
}
BLVoid
blProgressQuery(OUT BLU32* _Curtask, OUT BLU32* _Progress, OUT BLU32 _Finish[64])
{
	*_Curtask = _PrNetworkMem->_PrCurDownHash;
	*_Progress = _PrNetworkMem->nCurDownSize * 100 / _PrNetworkMem->nCurDownTotal;
	for (BLU32 _idx = 0; _idx < 64; ++_idx)
		_Finish[_idx] = _PrNetworkMem->nFinish[_idx];
}
