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
#include "../headers/audio.h"
#include "../headers/streamio.h"
#include "../headers/utils.h"
#include "../headers/system.h"
#include "internal/mathematics.h"
#include "internal/list.h"
#include "internal/thread.h"
#include "internal/internal.h"
#include "../externals/ogg/include/ogg/ogg.h"
#include "../externals/opus/include/opus.h"
#include "../externals/opus/include/opus_multistream.h"
#include "../externals/duktape/duktape.h"
#if defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_OSX)
#   define BL_USE_AL_API
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(BL_PLATFORM_LINUX)
#   define BL_USE_AL_API
#include <AL/al.h>
#include <AL/alc.h>
#elif defined(BL_PLATFORM_ANDROID)
#   define BL_USE_SL_API
#include <SLES/OpenSLES.h>
#elif defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#   define BL_USE_COREAUDIO_API
#	if(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#		include <xaudio2.h>
#		include <x3daudio.h>
#		pragma comment(lib,"xaudio2.lib")
#	else
#		include <xaudio2.h>
#		include <x3daudio.h>
#		pragma comment(lib,"x3daudio.lib")
#	endif
#endif
#ifdef __cplusplus
extern "C" {
extern BLBool _FetchResource(const BLAnsi*, BLVoid**, BLGuid, BLBool(*)(BLVoid*, const BLAnsi*),BLBool(*)(BLVoid*), BLBool);
extern BLBool _DiscardResource(BLGuid, BLBool(*)(BLVoid*), BLBool(*)(BLVoid*));
#endif
#ifdef __cplusplus
}
#endif
typedef struct _ShapeState {
	BLF32* pABuf;
	BLF32* pBBuf;
	BLS32 nFS;
	BLS32 nMute;
}_BLShapeState;
typedef struct _AudioDevice{
#if defined(BL_USE_AL_API)
    ALCcontext* pALContext;
    ALCdevice* pALDevice;
#elif defined(BL_USE_SL_API)
    SLEngineItf pDevice;
    SLObjectItf pDeviceObj;
    SLObjectItf pMixObj;
#elif defined(BL_USE_COREAUDIO_API)
	IXAudio2MasteringVoice* pCAContext;
	IXAudio2* pCADevice;
	X3DAUDIO_HANDLE pCAInstance;
#endif
    BLThread* pThread;
    BLF32 fMusicVolume;
    BLF32 fSystemVolume;
    BLF32 fEnvVolume;
}_BLAudioDevice;
typedef struct _AudioSource{
	OpusMSDecoder* pDecoder;
	ogg_sync_state sSyncState;
	ogg_stream_state sStreamState;
	ogg_page sPage;
	_BLShapeState sShape;
	ogg_int64_t nPageGranule;
	ogg_int64_t nLinkOut;
	ogg_int32_t nSerialno;
	BLS32 nPreSkip;
	BLS32 nInitPreSkip;
	BLS32 nGranOffset;
	BLS32 nChannels;
	BLF32* pFPData;
	BLBool bHasOpusStream;
	BLBool bHasTagsPacket;
	BLGuid nStream;
	BLU32 nOffset;
    BLBool bLoop;
	BLBool bValid;
	BLBool b3d;
	BLU8* pSoundBufA;
	BLU8* pSoundBufB;
	BLU8* pSoundBufC;
	BLVec3 sPos;
    BLGuid nID;
    BLU32 nBufTurn;
#if defined(BL_USE_AL_API)
    ALuint aBuffers[3];
    ALuint nSource;
#elif defined(BL_USE_SL_API)
    SLObjectItf pSource;
    SLBufferQueueItf pBufferFunc;
#elif defined(BL_USE_COREAUDIO_API)
	IXAudio2SourceVoice* pSource;
#endif
}_BLAudioSource;
typedef struct _AudioMember {
	duk_context* pDukContext;
	_BLAudioDevice pAudioDev;
	_BLAudioSource* pBackMusic;
	BLList* pSounds;
	blMutex* pMusicMutex;
	BLVec3 sListenerUp;
	BLVec3 sListenerDir;
	BLVec3 sListenerPos;
	BLVec3 sListenerLastPos;
	BLU32 nRNGSeed;
	BLU32 nPCMChannels;
	BLU32 nPCMSampleRate;
	BLU8* aPCMBuf[8];
	BLU32 nPCMBufTurn;
	BLU32 nPCMFill;
#if defined(BL_USE_AL_API)
	ALuint pPCMStream;
#elif defined(BL_USE_SL_API)
	SLObjectItf pPCMStream;
#elif defined(BL_USE_COREAUDIO_API)
	IXAudio2SourceVoice* pPCMStream;
#endif
}_BLAudioMember;
static _BLAudioMember* _PrAudioMem = NULL;

static opus_int64
_BufferWrite(_BLAudioSource* _Src, BLF32* _Pcm, BLS32 _Channels, BLS32 _FrameSize, BLS32* _Skip, _BLShapeState* _Shapemem, opus_int64 _Maxout, BLU32 _Offset, BLS32 _Turn)
{
	opus_int64 _sampout = 0;
	BLS32 _tmpskip;
	BLU32 _outlen;
	BLS16* _out;
	BLF32* _output;
	const BLF32 _gains[3] = { 32768.f - 15.f,32768.f - 15.f,32768.f - 3.f };
	const BLF32 _fcoef[3][8] =
	{
		{ 2.2374f, -.7339f, -.1251f, -.6033f, 0.9030f, .0116f, -.5853f, -.2571f },
		{ 2.2061f, -.4706f, -.2534f, -.6214f, 1.0587f, .0676f, -.6054f, -.2738f },
		{ 1.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f,0.0000f, 0.0000f, 0.0000f },
	};
	BLS32 _rate = _Shapemem->nFS == 44100 ? 1 : (_Shapemem->nFS == 48000 ? 0 : 2);
	BLF32 _gain = _gains[_rate];
	BLF32* _bbuf;
	BLF32* _abuf;
	BLS32 _mute = _Shapemem->nMute;
	_bbuf = _Shapemem->pBBuf;
	_abuf = _Shapemem->pABuf;
	if (_mute > 64)
		memset(_abuf, 0, sizeof(BLF32) * _Channels * 4);
	_out = (BLS16*)alloca(960 * 6 * _Channels * sizeof(BLS16));
	_Maxout = _Maxout < 0 ? 0 : _Maxout;
	do
	{
		if (_Skip)
		{
			_tmpskip = (*_Skip > _FrameSize) ? _FrameSize : *_Skip;
			*_Skip -= _tmpskip;
		}
		else
			_tmpskip = 0;
		_output = _Pcm + _Channels * _tmpskip;
		_outlen = _FrameSize - _tmpskip;
		_FrameSize = 0;
		for (BLU32 _idx = 0; _idx < _outlen; ++_idx)
		{
			BLS32 _pos = _idx * _Channels;
			BLS32 _silent = 1;
			for (BLS32 _c = 0; _c < _Channels; ++_c)
			{
				BLS32 _si;
				BLF32 _r, _s, _err = 0.f;
				_silent &= (BLS32)_output[_pos + _c] == 0;
				_s = _output[_pos + _c] * _gain;
				for (BLS32 _jdx = 0; _jdx < 4; ++_jdx)
					_err += _fcoef[_rate][_jdx] * _bbuf[_c * 4 + _jdx] - _fcoef[_rate][_jdx + 4] * _abuf[_c * 4 + _jdx];
				memmove(&_abuf[_c * 4 + 1], &_abuf[_c * 4], sizeof(BLF32) * 3);
				memmove(&_bbuf[_c * 4 + 1], &_bbuf[_c * 4], sizeof(BLF32) * 3);
				_abuf[_c * 4] = _err;
				_s = _s - _err;
				_PrAudioMem->nRNGSeed = (_PrAudioMem->nRNGSeed * 96314165) + 907633515;
				_r = (BLF32)_PrAudioMem->nRNGSeed * (1 / (BLF32)UINT_MAX);
				_PrAudioMem->nRNGSeed = (_PrAudioMem->nRNGSeed * 96314165) + 907633515;
				_r -= (BLF32)_PrAudioMem->nRNGSeed * (1 / (BLF32)UINT_MAX);
				if (_mute > 16)
					_r = 0;
				_out[_pos + _c] = _si = (BLS32)(floorf(.5f + fmaxf(-32768, fminf(_s + _r, 32767))));
				_bbuf[_c * 4] = (_mute > 16) ? 0 : fmaxf(-1.5f, fminf(_si - _s, 1.5f));
			}
			_mute++;
			if (!_silent)
				_mute = 0;
		}
		_Shapemem->nMute = MIN_INTERNAL(_mute, 960);
		if (_Maxout > 0)
		{
			if (_Turn == 0)
				memcpy(_Src->pSoundBufA + _sampout + _Offset, (BLU8*)_out, sizeof(BLS16) * _Channels * (_outlen < (BLU32)_Maxout ? _outlen : (BLU32)_Maxout));
			else if (_Turn == 1)
				memcpy(_Src->pSoundBufB + _sampout + _Offset, (BLU8*)_out, sizeof(BLS16) * _Channels * (_outlen < (BLU32)_Maxout ? _outlen : (BLU32)_Maxout));
			else
				memcpy(_Src->pSoundBufC + _sampout + _Offset, (BLU8*)_out, sizeof(BLS16) * _Channels * (_outlen < (BLU32)_Maxout ? _outlen : (BLU32)_Maxout));
			_sampout += sizeof(BLS16) * _Channels * (_outlen < (BLU32)_Maxout ? _outlen : (BLU32)_Maxout);
            _Maxout -= sizeof(BLS16) * _Channels * (_outlen < (BLU32)_Maxout ? _outlen : (BLU32)_Maxout);
		}
	} while (_FrameSize > 0 && _Maxout > 0);
	return _sampout;
}
static BLBool
_MakeStream(_BLAudioSource* _Src, BLU32 _Bufidx, BLU32* _TotalRead)
{
	BLBool _ret = TRUE;
	*_TotalRead = 0;
	ogg_packet _packet;
	BLU32 _fragsize = 0;
	BLBool _eof = FALSE;
refill:
	while (ogg_sync_pageout(&_Src->sSyncState, &_Src->sPage) == 1)
	{
		ogg_stream_pagein(&_Src->sStreamState, &_Src->sPage);
		_Src->nPageGranule = ogg_page_granulepos(&_Src->sPage);
		while (ogg_stream_packetout(&_Src->sStreamState, &_packet) == 1)
		{
			if (!_Src->bHasOpusStream || _Src->sStreamState.serialno != _Src->nSerialno)
				break;
			BLF32 _losspercent = -1.f;
			BLBool _lost = FALSE;
			BLS32 _framesz;
			opus_int64 _maxout, _outsamp;
			if (_losspercent > 0.f && 100.f * ((BLF32)rand()) / RAND_MAX < _losspercent)
				_lost = TRUE;
			if (_packet.e_o_s && _Src->sStreamState.serialno == _Src->nSerialno)
				_eof = TRUE;
			if (!_lost)
				_framesz = (BLS32)opus_multistream_decode_float(_Src->pDecoder, (BLU8*)_packet.packet, (opus_int32)_packet.bytes, _Src->pFPData, 5760, 0);
			else
			{
				opus_int32 _lostsize;
				_lostsize = 5760;
				if (_packet.bytes > 0)
				{
					opus_int32 _spp;
					_spp = (opus_int32)opus_packet_get_nb_frames(_packet.packet, (opus_int32)_packet.bytes);
					if (_spp > 0)
					{
						_spp *= opus_packet_get_samples_per_frame(_packet.packet, 48000);
						if (_spp > 0)
							_lostsize = _spp;
					}
				}
				_framesz = opus_multistream_decode_float(_Src->pDecoder, NULL, 0, _Src->pFPData, _lostsize, 0);
			}
			assert(_framesz >= 0);
			_maxout = ((_Src->nPageGranule - _Src->nGranOffset) * _Src->sShape.nFS / 48000) - _Src->nLinkOut;
			_outsamp = _BufferWrite(_Src, _Src->pFPData, _Src->nChannels, _framesz, &_Src->nPreSkip, &_Src->sShape, 0 > _maxout ? 0 : _maxout, _fragsize, _Bufidx);
			_Src->nLinkOut += _outsamp / (sizeof(BLS16) * _Src->nChannels);
			_fragsize += (BLU32)_outsamp;
		}
		if (_eof)
		{
			if (_Src->bLoop)
			{
				BLS32 _packetcount = 0;
				_Src->bHasOpusStream = TRUE;
				_Src->bHasTagsPacket = FALSE;
				_Src->nLinkOut = 0;
				ogg_sync_clear(&_Src->sSyncState);
				blStreamSeek(_Src->nStream, 0);
				ogg_sync_init(&_Src->sSyncState);
				BLAnsi* _tmpdata = ogg_sync_buffer(&_Src->sSyncState, 256);
				ogg_sync_wrote(&_Src->sSyncState, blStreamRead(_Src->nStream, 256, _tmpdata));
				while (ogg_sync_pageout(&_Src->sSyncState, &_Src->sPage) == 1)
				{
					ogg_stream_pagein(&_Src->sStreamState, &_Src->sPage);
					_Src->nPageGranule = ogg_page_granulepos(&_Src->sPage);
					ogg_stream_packetout(&_Src->sStreamState, &_packet);
					if (_packetcount == 0)
					{
						ogg_stream_packetout(&_Src->sStreamState, &_packet);
						_Src->nGranOffset = _Src->nInitPreSkip;
					}
					else if (_packetcount == 1)
					{
						_Src->bHasTagsPacket = TRUE;
						ogg_stream_packetout(&_Src->sStreamState, &_packet);
					}
					_packetcount++;
				}
			}
			else
			{
				_Src->bHasOpusStream = FALSE;
				_ret = FALSE;
			}
		}
	}
	BLAnsi* _tmpdata = ogg_sync_buffer(&_Src->sSyncState, 256);
	ogg_sync_wrote(&_Src->sSyncState, blStreamRead(_Src->nStream, 256, _tmpdata));
	if (!_ret)
		blInvokeEvent(BL_ET_SYSTEM, BL_SE_AUDIOOVER, 0, NULL, _Src->nID);
	*_TotalRead = _fragsize;
    if (!*_TotalRead)
        goto refill;
    return _ret;
}
static BLBool
_LoadAudio(BLVoid* _Src, const BLAnsi* _Filename)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	_src->nStream = blGenStream(_Filename);
	if (_src->nStream == INVALID_GUID)
		return FALSE;
	_src->nOffset = 0;
	_src->nSerialno = 0;
	_src->nPageGranule = 0;
	_src->nLinkOut = 0;
	_src->nPreSkip = 0;
	_src->nGranOffset = 0;
	_src->pFPData = NULL;
	_src->bHasOpusStream = FALSE;
	_src->bHasTagsPacket = FALSE;
	_src->pDecoder = NULL;
	_src->sShape.pABuf = NULL;
	_src->sShape.pBBuf = NULL;
	_src->sShape.nMute = 960;
	_src->sShape.nFS = 0;
	BLS32 _err, _packetcount = 0;
	ogg_packet _packet;
	_err = ogg_sync_init(&_src->sSyncState);
	memset(&_src->sStreamState, 0, sizeof(_src->sStreamState));
	assert(_err == OPUS_OK);
	BLAnsi* _tmpdata = ogg_sync_buffer(&_src->sSyncState, 4096);
	ogg_sync_wrote(&_src->sSyncState, blStreamRead(_src->nStream, 4096, _tmpdata));
	while (ogg_sync_pageout(&_src->sSyncState, &_src->sPage) == 1)
	{
		if (!_src->sStreamState.body_data)
			ogg_stream_init(&_src->sStreamState, ogg_page_serialno(&_src->sPage));
		if (ogg_page_serialno(&_src->sPage) != _src->sStreamState.serialno)
			ogg_stream_reset_serialno(&_src->sStreamState, ogg_page_serialno(&_src->sPage));
		ogg_stream_pagein(&_src->sStreamState, &_src->sPage);
		_src->nPageGranule = ogg_page_granulepos(&_src->sPage);
		while (ogg_stream_packetout(&_src->sStreamState, &_packet) == 1)
		{
			if (_packet.b_o_s && _packet.bytes >= 8 && !memcmp(_packet.packet, "OpusHead", 8))
			{
				if (_src->bHasOpusStream && _src->bHasTagsPacket)
				{
					_src->bHasOpusStream = FALSE;
					blDebugOutput("Opus ended without EOS and a new stream began");
					return FALSE;
				}
				if (!_src->bHasOpusStream)
				{
					if (_packetcount > 0 && _src->nSerialno == _src->sStreamState.serialno)
					{
						blDebugOutput("Apparent chaining without changing serial number");
						return FALSE;
					}
					_src->nSerialno = (ogg_int32_t)_src->sStreamState.serialno;
					_src->bHasOpusStream = TRUE;
					_src->bHasTagsPacket = FALSE;
					_src->nLinkOut = 0;
					_packetcount = 0;
				}
			}
			if (!_src->bHasOpusStream || _src->sStreamState.serialno != _src->nSerialno)
				break;
			if (_packetcount == 0)
			{
				BLS32 _mappingfamily, _streams = 0, _rate = 0;
				BLS32 _version;
				ogg_uint32_t _inputsamplerate;
				BLS32 _gain;
				BLS32 _nbcoupled;
				BLU8 _streammap[255];
				BLAnsi _str[9];
				BLU8 _ch;
				ogg_uint16_t _shortval;
				BLS32 _pos = 0;
				_str[8] = 0;
				assert(_packet.bytes >= 19);
				memcpy(_str, _packet.packet + _pos, 8);
				_pos += 8;
				assert(memcmp(_str, "OpusHead", 8) == 0);
				memcpy(&_ch, _packet.packet + _pos, 1);
				_pos += 1;
				_version = _ch;
				assert((_version & 240) == 0);
				memcpy(&_ch, _packet.packet + _pos, 1);
				_pos += 1;
				_src->nChannels = _ch;
				assert(_src->nChannels != 0);
				memcpy(&_shortval, _packet.packet + _pos, 2);
				_pos += 2;
				_src->nInitPreSkip = _src->nPreSkip = _shortval;
				memcpy(&_inputsamplerate, _packet.packet + _pos, 4);
				_pos += 4;
				memcpy(&_shortval, _packet.packet + _pos, 2);
				_pos += 2;
				_gain = _shortval;
				memcpy(&_ch, _packet.packet + _pos, 1);
				_pos += 1;
				_mappingfamily = _ch;
				if (_mappingfamily != 0)
				{
					memcpy(&_ch, _packet.packet + _pos, 1);
					_pos += 1;
					assert(_ch >= 1);
					_streams = _ch;
					memcpy(&_ch, _packet.packet + _pos, 1);
					_pos += 1;
					assert(_ch <= _streams && (_ch + _streams) <= 255);
					_nbcoupled = _ch;
					for (BLS32 _idx = 0; _idx < _src->nChannels; ++_idx)
					{
						memcpy(&_streammap[_idx], _packet.packet + _pos, 1);
						_pos += 1;
						assert(_streammap[_idx] <= (_streams + _nbcoupled) || _streammap[_idx] != 255);
					}
				}
				else
				{
					assert(_src->nChannels <= 2);
					_streams = 1;
					_nbcoupled = _src->nChannels > 1;
					_streammap[0] = 0;
					_streammap[1] = 1;
				}
				assert(!(_version == 0 || _version == 1) || _pos == _packet.bytes);
				if (!_rate)
					_rate = _inputsamplerate;
				if (_rate == 0)
					_rate = 48000;
				if (_rate < 8000 || _rate > 192000)
					_rate = 48000;
				_src->pDecoder = opus_multistream_decoder_create(_rate, _src->nChannels, _streams, _nbcoupled, _streammap, &_err);
				if (_err != OPUS_OK || !_src->pDecoder)
					_src->pDecoder = NULL;
				if (_gain != 0 && _src->pDecoder)
				{
					BLS32 _gainadj = _gain;
					_err = opus_multistream_decoder_ctl(_src->pDecoder, OPUS_SET_GAIN(_gainadj));
					if (_err != OPUS_OK)
						_src->pDecoder = NULL;
				}
				if (!_src->pDecoder)
				{
					blDebugOutput("Opus decoder create failed");
					return FALSE;
				}
				if (ogg_stream_packetout(&_src->sStreamState, &_packet) != 0 || _src->sPage.header[_src->sPage.header_len - 1] == 255)
				{
					blDebugOutput("Extra packets on initial header page. Invalid stream.");
					return FALSE;
				}
				_src->nGranOffset = _src->nPreSkip;
				if (!_src->sShape.pABuf)
				{
					_src->sShape.pABuf = (BLF32*)calloc(_src->nChannels, sizeof(BLF32) * 4);
					_src->sShape.pBBuf = (BLF32*)calloc(_src->nChannels, sizeof(BLF32) * 4);
					_src->sShape.nFS = _rate;
				}
				if (!_src->pFPData)
					_src->pFPData = (BLF32*)malloc(_src->nChannels * sizeof(BLF32) * 960 * 6);
			}
			else if (_packetcount == 1)
			{
				_src->bHasTagsPacket = TRUE;
				if (ogg_stream_packetout(&_src->sStreamState, &_packet) != 0 || _src->sPage.header[_src->sPage.header_len - 1] == 255)
				{
					blDebugOutput("Extra packets on initial tags page. Invalid stream.");
					return FALSE;
				}
                break;
			}
			_packetcount++;
		}
	}
	return TRUE;
}
static BLBool
_UnloadAudio(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	if (!_src->b3d && _src->bLoop)
	{
		blMutexLock(_PrAudioMem->pMusicMutex);
		opus_multistream_decoder_destroy(_src->pDecoder);
		blDeleteStream(_src->nStream);
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	}
	else
	{
		blMutexLock(_PrAudioMem->pSounds->pMutex);
		opus_multistream_decoder_destroy(_src->pDecoder);
		blDeleteStream(_src->nStream);
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	}
	if (_src->sStreamState.body_data)
		ogg_stream_clear(&_src->sStreamState);
	ogg_sync_clear(&_src->sSyncState);
	if (_src->sShape.pABuf)
		free(_src->sShape.pABuf);
	if (_src->sShape.pBBuf)
		free(_src->sShape.pBBuf);
	if (_src->pFPData)
		free(_src->pFPData);
	return TRUE;
}
#if defined(BL_USE_AL_API)
static BLBool
_ALSoundSetup(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
    ALsizei _queuesz = 0;
	_src->pSoundBufA = (BLU8*)malloc(4 * 3840 * 10);
	_src->pSoundBufB = (BLU8*)malloc(4 * 3840 * 10);
	_src->pSoundBufC = (BLU8*)malloc(4 * 3840 * 10);
    memset(_src->pSoundBufA, 0, 4 * 3840 * 10);
    memset(_src->pSoundBufB, 0, 4 * 3840 * 10);
    memset(_src->pSoundBufC, 0, 4 * 3840 * 10);
    alGetError();
    alGenSources(1, &_src->nSource);
    alGenBuffers(3, _src->aBuffers);
	if (!_src->b3d)
        alSourcei(_src->nSource, AL_SOURCE_RELATIVE, TRUE);
    else
    {
        alSourcei(_src->nSource, AL_SOURCE_RELATIVE, FALSE);
        alSourcef(_src->nSource, AL_ROLLOFF_FACTOR, 1.0f);
    }
	_src->nBufTurn = 0;
	if (!_src->b3d && _src->bLoop)
		alSourcef(_src->nSource, AL_GAIN, _PrAudioMem->pAudioDev.fMusicVolume);
	else
	{
		if (!_src->b3d)
			alSourcef(_src->nSource, AL_GAIN, _PrAudioMem->pAudioDev.fSystemVolume);
		else
			alSourcef(_src->nSource, AL_GAIN, _PrAudioMem->pAudioDev.fEnvVolume);
    }
    alSourcei(_src->nSource, AL_BUFFER, 0);
    for (BLU32 _i = 0; _i < 3; ++_i)
    {
        BLU32 _tmpsz;
        if (_MakeStream(_src, _i, &_tmpsz))
        {
            switch (_i)
            {
                case 0:
                    alBufferData(_src->aBuffers[0], (_src->nChannels == 2) ? 0x1103 : 0x1101, _src->pSoundBufA, _tmpsz, 48000);
                    break;
                case 1:
                    alBufferData(_src->aBuffers[1], (_src->nChannels == 2) ? 0x1103 : 0x1101, _src->pSoundBufB, _tmpsz, 48000);
                    break;
                case 2:
                    alBufferData(_src->aBuffers[2], (_src->nChannels == 2) ? 0x1103 : 0x1101, _src->pSoundBufC, _tmpsz, 48000);
                    break;
                default:
                    break;
            }
            ++_queuesz;
        }
    }
    alSourceQueueBuffers(_src->nSource, _queuesz, _src->aBuffers);
	alSourcePlay(_src->nSource);
	if (_src->b3d || !_src->bLoop)
		blListPushBack(_PrAudioMem->pSounds, _src);
	else
		_PrAudioMem->pBackMusic = _src;
	_src->bValid = TRUE;
	return TRUE;
}
static BLBool
_ALSoundRelease(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	if (!_src->b3d && _src->bLoop)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
    alSourceStop(_src->nSource);
    BLS32 _queued = 0;
    alGetSourcei(_src->nSource, AL_BUFFERS_QUEUED, &_queued);
    while (_queued--)
    {
        ALuint _buffer;
        alSourceUnqueueBuffers(_src->nSource, 1, &_buffer);
    }
    alDeleteSources(1, &_src->nSource);
    alDeleteBuffers(3, _src->aBuffers);
	free(_src->pSoundBufA);
	free(_src->pSoundBufB);
	free(_src->pSoundBufC);
	if (!_src->b3d && _src->bLoop)
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	else
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	return TRUE;
}
static BLBool
_ALUpdate(_BLAudioSource* _Src)
{
    if (!_Src || !_Src->bValid)
        return TRUE;
    ALint _processed = 0;
    ALenum _state = 0;
    alGetSourcei(_Src->nSource, AL_SOURCE_STATE, &_state);
    if (_state == AL_PLAYING)
    {
        alGetSourcei(_Src->nSource, AL_BUFFERS_PROCESSED, &_processed);
        while (_processed--)
        {
			BLU32 _totalread;
            ALuint _buffer;
            alSourceUnqueueBuffers(_Src->nSource, 1, &_buffer);
            if (_Src->aBuffers[0] == _buffer)
            {
                if (_MakeStream(_Src, 0, &_totalread))
					alBufferData(_Src->aBuffers[0], (_Src->nChannels == 2) ? 0x1103 : 0x1101, _Src->pSoundBufA, _totalread, 48000);
                alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
            else if (_Src->aBuffers[1] == _buffer)
            {
                if (_MakeStream(_Src, 1, &_totalread))
					alBufferData(_Src->aBuffers[1], (_Src->nChannels == 2) ? 0x1103 : 0x1101, _Src->pSoundBufB, _totalread, 48000);
                alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
            else if (_Src->aBuffers[2] == _buffer)
            {
                if (_MakeStream(_Src, 2, &_totalread))
					alBufferData(_Src->aBuffers[2], (_Src->nChannels == 2) ? 0x1103 : 0x1101, _Src->pSoundBufC, _totalread, 48000);
                alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
        }
    }
    alGetSourcei(_Src->nSource, AL_SOURCE_STATE, &_state);
    if (_state == AL_STOPPED && !_Src->bLoop)
        return TRUE;
    else
        return FALSE;
}
#elif defined(BL_USE_SL_API)
static BLBool
_SLSoundSetup(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	_src->pSoundBufA = (BLU8*)malloc(4 * 3840 * 10);
	_src->pSoundBufB = (BLU8*)malloc(4 * 3840 * 10);
	_src->pSoundBufC = (BLU8*)malloc(4 * 3840 * 10);
    memset(_src->pSoundBufA, 0, 4 * 3840 * 10);
    memset(_src->pSoundBufB, 0, 4 * 3840 * 10);
    memset(_src->pSoundBufC, 0, 4 * 3840 * 10);
	_src->nBufTurn = 0;
    SLDataLocator_BufferQueue _bufq = {SL_DATALOCATOR_BUFFERQUEUE, 3};
    SLDataFormat_PCM _pcm;
    _pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    _pcm.formatType = SL_DATAFORMAT_PCM;
	if (!_src->b3d && _src->bLoop)
	{
		_pcm.numChannels = 2;
        _pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
		_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
		_pcm.samplesPerSec = 48000;
	}
	else
	{
		_pcm.numChannels = 2;
        _pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
		_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
		_pcm.samplesPerSec = 48000;
	}
	SLresult _result;
    SLDataSource _data = {&_bufq, &_pcm};
    SLDataLocator_OutputMix _outmix = {SL_DATALOCATOR_OUTPUTMIX, _PrAudioMem->pAudioDev.pMixObj};
    SLDataSink _snk = {&_outmix, NULL};
    const SLInterfaceID _ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean _req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	_result = (*_PrAudioMem->pAudioDev.pDevice)->CreateAudioPlayer(_PrAudioMem->pAudioDev.pDevice, &_src->pSource, &_data, &_snk, 3, _ids, _req);
	assert(_result == SL_RESULT_SUCCESS);
	_result = (*_src->pSource)->Realize(_src->pSource, SL_BOOLEAN_FALSE);
	assert(_result == SL_RESULT_SUCCESS);
    SLVolumeItf _volfunc;
	_result = (*_src->pSource)->GetInterface(_src->pSource, SL_IID_VOLUME, &_volfunc);
	assert(_result == SL_RESULT_SUCCESS);
	if (!_src->b3d && _src->bLoop)
        (*_volfunc)->SetVolumeLevel(_volfunc, _PrAudioMem->pAudioDev.fMusicVolume);
	else
	{
		if (!_src->b3d)
            (*_volfunc)->SetVolumeLevel(_volfunc, _PrAudioMem->pAudioDev.fSystemVolume);
		else
            (*_volfunc)->SetVolumeLevel(_volfunc, _PrAudioMem->pAudioDev.fEnvVolume);
    }
    SLPlayItf _playfunc;
	_result = (*_src->pSource)->GetInterface(_src->pSource, SL_IID_PLAY, &_playfunc);
	assert(_result == SL_RESULT_SUCCESS);
	_result = (*_src->pSource)->GetInterface(_src->pSource, SL_IID_BUFFERQUEUE, &_src->pBufferFunc);
	assert(_result == SL_RESULT_SUCCESS);
	_result = (*_playfunc)->SetPlayState(_playfunc, SL_PLAYSTATE_PLAYING);
	assert(_result == SL_RESULT_SUCCESS);
	if (!_src->b3d && _src->bLoop)
		blListPushBack(_PrAudioMem->pSounds, _src);
	else
		_PrAudioMem->pBackMusic = _src;
	_src->bValid = TRUE;
	return TRUE;
}
static BLBool
_SLSoundRelease(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	if (!_src->b3d && _src->bLoop)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	_src->bValid = FALSE;
	if (_src->pSource)
		(*_src->pSource)->Destroy(_src->pSource);
	free(_src->pSoundBufA);
	free(_src->pSoundBufB);
	free(_src->pSoundBufC);
	if (!_src->b3d && _src->bLoop)
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	else
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	return TRUE;
}
static BLBool
_SLUpdate(_BLAudioSource* _Src)
{
    if (!_Src || !_Src->bValid)
        return TRUE;
    SLBufferQueueState _processed;
    SLuint32 _state = 0;
    SLPlayItf _playfunc;
    (*_Src->pSource)->GetInterface(_Src->pSource, SL_IID_PLAY, &_playfunc);
    (*_playfunc)->GetPlayState(_playfunc, &_state);
    if (_state == SL_PLAYSTATE_PLAYING)
    {
		(*_Src->pBufferFunc)->GetState(_Src->pBufferFunc, &_processed);
		SLint32 _proce = 3 - _processed.count;
        while (_proce--)
        {
			BLU32 _totalread;
            assert(_proce < 3);
            _MakeStream(_Src, (2 - _proce + _processed.playIndex) % 3, &_totalread);
            switch((2 - _proce + _processed.playIndex) % 3)
            {
            case 0:
                (*_Src->pBufferFunc)->Enqueue(_Src->pBufferFunc, _Src->pSoundBufA, _totalread);
                break;
            case 1:
                (*_Src->pBufferFunc)->Enqueue(_Src->pBufferFunc, _Src->pSoundBufB, _totalread);
                break;
            default:
                (*_Src->pBufferFunc)->Enqueue(_Src->pBufferFunc, _Src->pSoundBufC, _totalread);
                break;
            }
        }
    }
    (*_playfunc)->GetPlayState(_playfunc, &_state);
    if (_state == SL_PLAYSTATE_STOPPED && !_Src->bLoop)
        return TRUE;
    else
        return FALSE;
}
#elif defined(BL_USE_COREAUDIO_API)
static BLBool
_CASoundSetup(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	_src->pSoundBufA = (BLU8*)malloc(4 * 3840 * 10);
	_src->pSoundBufB = (BLU8*)malloc(4 * 3840 * 10);
	_src->pSoundBufC = (BLU8*)malloc(4 * 3840 * 10);
	memset(_src->pSoundBufA, 0, 4 * 3840 * 10);
	memset(_src->pSoundBufB, 0, 4 * 3840 * 10);
	memset(_src->pSoundBufC, 0, 4 * 3840 * 10);
	WAVEFORMATEX _pwfx;
	_pwfx.wFormatTag = WAVE_FORMAT_PCM;
	_pwfx.nChannels = _src->nChannels;
	_pwfx.wBitsPerSample = 16;
	_pwfx.nSamplesPerSec = 48000;
	_pwfx.nBlockAlign = _pwfx.nChannels*_pwfx.wBitsPerSample / 8;
	_pwfx.nAvgBytesPerSec = _pwfx.nBlockAlign * _pwfx.nSamplesPerSec;
	_pwfx.cbSize = 0;
	_PrAudioMem->pAudioDev.pCADevice->CreateSourceVoice(&_src->pSource, &_pwfx);
	if (!_src->b3d && _src->bLoop)
		_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fMusicVolume);
	else
	{
		if (!_src->b3d)
			_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fSystemVolume);
		else
			_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fEnvVolume);
	}
	_src->nBufTurn = 0;
	_src->pSource->Start(0);
	if (_src->b3d || !_src->bLoop)
		blListPushBack(_PrAudioMem->pSounds, _src);
	else
		_PrAudioMem->pBackMusic = _src;
	_src->bValid = TRUE;
	return TRUE;
}
static BLBool
_CASoundRelease(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	if (!_src->b3d && _src->bLoop)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	_src->bValid = FALSE;
	_src->pSource->Stop(0);
	_src->pSource->DestroyVoice();
	free(_src->pSoundBufA);
	free(_src->pSoundBufB);
	free(_src->pSoundBufC);
	if (!_src->b3d && _src->bLoop)
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	else
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	return TRUE;
}
static BLBool
_CAUpdate(_BLAudioSource* _Src)
{
	if (!_Src || !_Src->bValid)
		return TRUE;
	BLBool _ret = TRUE;
	XAUDIO2_VOICE_STATE _state;
	for (;; )
	{
		_Src->pSource->GetState(&_state);
		if (_state.BuffersQueued < 2)
		{
			BLU32 _totalread;
			_ret = _MakeStream(_Src, _Src->nBufTurn, &_totalread);
			if (_totalread)
			{
				XAUDIO2_BUFFER _buf = { 0 };
				_buf.AudioBytes = _totalread;
				if (_Src->nBufTurn == 0)
					_buf.pAudioData = _Src->pSoundBufA;
				else if (_Src->nBufTurn == 1)
					_buf.pAudioData = _Src->pSoundBufB;
				else
					_buf.pAudioData = _Src->pSoundBufC;
				if (!_ret)
					_buf.Flags = XAUDIO2_END_OF_STREAM;
				_Src->pSource->SubmitSourceBuffer(&_buf);
				_Src->nBufTurn++;
				_Src->nBufTurn %= 3;
			}
			if (!_ret)
				return FALSE;
		}
		else
			return TRUE;
	}
	return TRUE;
}
#endif
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
static DWORD __stdcall
_AudioThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_AudioThreadFunc(BLVoid* _Userdata)
#endif
{
    while (!_GbSystemRunning)
        blYield();
    do
    {
		if (_GbSystemRunning == 1 && !_GbTVMode)
		{
			blMutexLock(_PrAudioMem->pMusicMutex);
#if defined(BL_USE_AL_API)
			_ALUpdate(_PrAudioMem->pBackMusic);
#elif defined(BL_USE_SL_API)
			_SLUpdate(_PrAudioMem->pBackMusic);
#elif defined(BL_USE_COREAUDIO_API)
			_CAUpdate(_PrAudioMem->pBackMusic);
#endif
			blMutexUnlock(_PrAudioMem->pMusicMutex);
			blMutexLock(_PrAudioMem->pSounds->pMutex);
			FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
			{
#if defined(BL_USE_AL_API)
				if (_ALUpdate(_iter))
#elif defined(BL_USE_SL_API)
				if (_SLUpdate(_iter))
#elif defined(BL_USE_COREAUDIO_API)
				if (_CAUpdate(_iter))
#endif
				{
					blListErase(_PrAudioMem->pSounds, _iterator_iter);
#if defined(BL_USE_AL_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _ALSoundRelease);
#elif defined(BL_USE_SL_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _SLSoundRelease);
#elif defined(BL_USE_COREAUDIO_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _CASoundRelease);
#endif
					free(_iterator_iter);
					blDeleteGuid(_iter->nID);
					break;
				}
			}
			blMutexUnlock(_PrAudioMem->pSounds->pMutex);
			blYield();
		}
    } while (_GbSystemRunning && _PrAudioMem->pAudioDev.pThread->bRunning);
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
    return 0xDEAD;
#else
    return (BLVoid*)0xDEAD;
#endif
}
#if defined(BL_USE_AL_API)
static BLVoid
_ALInit()
{
	_PrAudioMem->pAudioDev.fMusicVolume = _PrAudioMem->pAudioDev.fSystemVolume = _PrAudioMem->pAudioDev.fEnvVolume = 0.7f;
	_PrAudioMem->pAudioDev.pALDevice = alcOpenDevice(0);
    assert(_PrAudioMem->pAudioDev.pALDevice);
	_PrAudioMem->pAudioDev.pALContext = alcCreateContext(_PrAudioMem->pAudioDev.pALDevice, 0);
    assert(_PrAudioMem->pAudioDev.pALContext);
    if (!alcMakeContextCurrent(_PrAudioMem->pAudioDev.pALContext))
    {
        assert(0);
        alcDestroyContext(_PrAudioMem->pAudioDev.pALContext);
        alcCloseDevice(_PrAudioMem->pAudioDev.pALDevice);
		blDebugOutput("Audio initialize failed");
		return;
    }
    _PrAudioMem->pSounds = blGenList(TRUE);
    _PrAudioMem->pMusicMutex = blGenMutex();
    _PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
	blThreadRun(_PrAudioMem->pAudioDev.pThread);
	blDebugOutput("Audio initialize successfully");
}
static BLVoid
_ALDestroy()
{
    blDeleteThread(_PrAudioMem->pAudioDev.pThread);
    blDeleteMutex(_PrAudioMem->pMusicMutex);
    blDeleteList(_PrAudioMem->pSounds);
    alcMakeContextCurrent(0);
    alcDestroyContext(_PrAudioMem->pAudioDev.pALContext);
	_PrAudioMem->pAudioDev.pALContext = NULL;
    alcCloseDevice(_PrAudioMem->pAudioDev.pALDevice);
	_PrAudioMem->pAudioDev.pALDevice = NULL;
}
#elif defined(BL_USE_SL_API)
static BLVoid
_SLInit()
{
	_PrAudioMem->pAudioDev.fMusicVolume = _PrAudioMem->pAudioDev.fSystemVolume = _PrAudioMem->pAudioDev.fEnvVolume = 0.7f;
    SLresult _res;
    _res = slCreateEngine(&_PrAudioMem->pAudioDev.pDeviceObj, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == _res);
    _res = (*_PrAudioMem->pAudioDev.pDeviceObj)->Realize(_PrAudioMem->pAudioDev.pDeviceObj, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == _res);
    _res = (*_PrAudioMem->pAudioDev.pDeviceObj)->GetInterface(_PrAudioMem->pAudioDev.pDeviceObj, SL_IID_ENGINE, &_PrAudioMem->pAudioDev.pDevice);
    assert(SL_RESULT_SUCCESS == _res);
    _res = (*_PrAudioMem->pAudioDev.pDevice)->CreateOutputMix(_PrAudioMem->pAudioDev.pDevice, &_PrAudioMem->pAudioDev.pMixObj, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == _res);
    _res = (*_PrAudioMem->pAudioDev.pMixObj)->Realize(_PrAudioMem->pAudioDev.pMixObj, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == _res);
	_PrAudioMem->pSounds = blGenList(TRUE);
	_PrAudioMem->pMusicMutex = blGenMutex();
	_PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
	blThreadRun(_PrAudioMem->pAudioDev.pThread);
	blDebugOutput("Audio initialize successfully");
}
static BLVoid
_SLDestroy()
{
	blDeleteThread(_PrAudioMem->pAudioDev.pThread);
	blDeleteMutex(_PrAudioMem->pMusicMutex);
	blDeleteList(_PrAudioMem->pSounds);
    (*_PrAudioMem->pAudioDev.pMixObj)->Destroy(_PrAudioMem->pAudioDev.pMixObj);
    (*_PrAudioMem->pAudioDev.pDeviceObj)->Destroy(_PrAudioMem->pAudioDev.pDeviceObj);
	_PrAudioMem->pAudioDev.pMixObj = NULL;
	_PrAudioMem->pAudioDev.pDevice = NULL;
	_PrAudioMem->pAudioDev.pDeviceObj = NULL;
}
#elif defined(BL_USE_COREAUDIO_API)
static BLVoid
_CAInit()
{
	_PrAudioMem->pAudioDev.fMusicVolume = _PrAudioMem->pAudioDev.fSystemVolume = _PrAudioMem->pAudioDev.fEnvVolume = 0.7f;
	HRESULT _hr;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(_hr = XAudio2Create(&_PrAudioMem->pAudioDev.pCADevice)))
	{
		assert(0);
		_PrAudioMem->pAudioDev.pCADevice = NULL;
		CoUninitialize();
		blDebugOutput("Audio initialize failed");
		return;
	}
	if (FAILED(_hr = _PrAudioMem->pAudioDev.pCADevice->CreateMasteringVoice(&_PrAudioMem->pAudioDev.pCAContext)))
	{
		assert(0);
		_PrAudioMem->pAudioDev.pCADevice->Release();
		_PrAudioMem->pAudioDev.pCADevice = NULL;
		CoUninitialize();
		blDebugOutput("Audio initialize failed");
		return;
	}
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
	DWORD _mask;
	_PrAudioMem->pAudioDev.pCAContext->GetChannelMask(&_mask);
	X3DAudioInitialize(_mask, X3DAUDIO_SPEED_OF_SOUND, _PrAudioMem->pAudioDev.pCAInstance);
#else
	XAUDIO2_DEVICE_DETAILS _details;
	_PrAudioMem->pAudioDev.pCADevice->GetDeviceDetails( 0, &_details );
	X3DAudioInitialize(_details.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, _PrAudioMem->pAudioDev.pCAInstance);
#endif
	_PrAudioMem->pSounds = blGenList(TRUE);
	_PrAudioMem->pMusicMutex = blGenMutex();
	_PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
	blThreadRun(_PrAudioMem->pAudioDev.pThread);
	blDebugOutput("Audio initialize successfully");
}
static BLVoid
_CADestroy()
{
	blDeleteThread(_PrAudioMem->pAudioDev.pThread);
	blDeleteMutex(_PrAudioMem->pMusicMutex);
	blDeleteList(_PrAudioMem->pSounds);
	_PrAudioMem->pAudioDev.pCADevice->StopEngine();
	_PrAudioMem->pAudioDev.pCAContext->DestroyVoice();
	_PrAudioMem->pAudioDev.pCADevice->Release();
	CoUninitialize();
	_PrAudioMem->pAudioDev.pCAContext = NULL;
	_PrAudioMem->pAudioDev.pCADevice = NULL;
}
#endif
BLVoid
_AudioInit(duk_context* _DKC)
{
	_PrAudioMem = (_BLAudioMember*)malloc(sizeof(_BLAudioMember));
	_PrAudioMem->pDukContext = _DKC;
	_PrAudioMem->pBackMusic = NULL;
	_PrAudioMem->pSounds = NULL;
	_PrAudioMem->pMusicMutex = NULL;
	_PrAudioMem->nRNGSeed = 22222;
	_PrAudioMem->nPCMChannels = -1;
	_PrAudioMem->nPCMSampleRate = -1;
	_PrAudioMem->nPCMBufTurn = 0;
	_PrAudioMem->nPCMFill = 0;
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
		_PrAudioMem->aPCMBuf[_idx] = NULL;
#if defined(BL_USE_AL_API)
    _ALInit();
	_PrAudioMem->pPCMStream = -1;
#elif defined(BL_USE_SL_API)
    _SLInit();
#elif defined(BL_USE_COREAUDIO_API)
    _CAInit();
	_PrAudioMem->pPCMStream = NULL;
#endif
}
BLVoid
_AudioStep(BLU32 _Delta)
{
	blMutexLock(_PrAudioMem->pSounds->pMutex);
	FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
	{
		if (_iter->bLoop && _iter->b3d)
		{
#if defined(BL_USE_AL_API)
			alListener3f(AL_POSITION, _PrAudioMem->sListenerPos.fX, _PrAudioMem->sListenerPos.fY, _PrAudioMem->sListenerPos.fZ);
			BLF32 _orient[6] = { _PrAudioMem->sListenerDir.fX, _PrAudioMem->sListenerDir.fY, _PrAudioMem->sListenerDir.fZ, _PrAudioMem->sListenerUp.fX, _PrAudioMem->sListenerUp.fY, _PrAudioMem->sListenerUp.fZ };
			alListenerfv(AL_ORIENTATION, _orient);
			alListener3f(AL_VELOCITY, (_PrAudioMem->sListenerPos.fX - _PrAudioMem->sListenerLastPos.fX) / _Delta, (_PrAudioMem->sListenerPos.fY - _PrAudioMem->sListenerLastPos.fY) / _Delta, (_PrAudioMem->sListenerPos.fZ - _PrAudioMem->sListenerLastPos.fZ) / _Delta);
			alSource3f(_iter->nSource, AL_POSITION, _iter->sPos.fX, _iter->sPos.fY, _iter->sPos.fZ);
			alSource3f(_iter->nSource, AL_DIRECTION, 0.f, 0.f, 0.f);
			alSource3f(_iter->nSource, AL_VELOCITY, 0.f, 0.f, 0.f);
#elif defined(BL_USE_SL_API)
#elif defined(BL_USE_COREAUDIO_API)
			BLU32 _channels;
#if (_WIN32_WINNT >= 0x0602)
			XAUDIO2_VOICE_DETAILS _details;
			_PrAudioMem->pAudioDev.pCAContext->GetVoiceDetails(&_details);
			_channels = _details.InputChannels;
#else
			XAUDIO2_DEVICE_DETAILS _details;
			_PrAudioMem->pAudioDev.pCADevice->GetDeviceDetails(0, &_details);
			_channels = _details.OutputFormat.Format.nChannels;
#endif
			X3DAUDIO_LISTENER _listener;
			_listener.Position.x = _PrAudioMem->sListenerPos.fX;
			_listener.Position.y = _PrAudioMem->sListenerPos.fY;
			_listener.Position.z = _PrAudioMem->sListenerPos.fZ;
			_listener.OrientFront.x = _PrAudioMem->sListenerDir.fX;
			_listener.OrientFront.y = _PrAudioMem->sListenerDir.fY;
			_listener.OrientFront.z = _PrAudioMem->sListenerDir.fZ;
			_listener.OrientTop.x = _PrAudioMem->sListenerUp.fX;
			_listener.OrientTop.y = _PrAudioMem->sListenerUp.fY;
			_listener.OrientTop.z = _PrAudioMem->sListenerDir.fZ;
			_listener.Velocity.x = (_PrAudioMem->sListenerPos.fX - _PrAudioMem->sListenerLastPos.fX) / _Delta;
			_listener.Velocity.y = (_PrAudioMem->sListenerPos.fY - _PrAudioMem->sListenerLastPos.fY) / _Delta;
			_listener.Velocity.z = (_PrAudioMem->sListenerPos.fZ - _PrAudioMem->sListenerLastPos.fZ) / _Delta;
			_listener.pCone = NULL;
			X3DAUDIO_EMITTER _emitter;
			_emitter.pCone = NULL;
			_emitter.Position.x = _iter->sPos.fX;
			_emitter.Position.y = _iter->sPos.fY;
			_emitter.Position.z = _iter->sPos.fZ;
			_emitter.OrientFront.x = 0.f;
			_emitter.OrientFront.y = 0.f;
			_emitter.OrientTop.x = 0.f;
			_emitter.OrientTop.y = 0.f;
			_emitter.OrientTop.z = 0.f;
			_emitter.ChannelCount = 1;
			_emitter.ChannelRadius = 0.f;
			_emitter.pChannelAzimuths = NULL;
			_emitter.pVolumeCurve = NULL;
			_emitter.pLFECurve = NULL;
			_emitter.pLPFDirectCurve = NULL;
			_emitter.pLPFReverbCurve = NULL;
			_emitter.pReverbCurve = NULL;
			_emitter.CurveDistanceScaler = 14.f;
			_emitter.DopplerScaler = 0.f;
			_emitter.Velocity.x = 0.f;
			_emitter.Velocity.y = 0.f;
			_emitter.Velocity.z = 0.f;
			X3DAUDIO_DSP_SETTINGS _dsp;
			_dsp.SrcChannelCount = 1;
			_dsp.DstChannelCount = _channels;
			FLOAT32 _coefficients[8];
			_dsp.pMatrixCoefficients = _coefficients;
			X3DAudioCalculate(_PrAudioMem->pAudioDev.pCAInstance, &_listener, &_emitter, X3DAUDIO_CALCULATE_MATRIX, &_dsp);
			_iter->pSource->SetOutputMatrix(_PrAudioMem->pAudioDev.pCAContext, 1, _channels, _coefficients);
#endif
			_PrAudioMem->sListenerLastPos = _PrAudioMem->sListenerPos;
		}
    }
    blMutexUnlock(_PrAudioMem->pSounds->pMutex);
}
BLVoid
_AudioDestroy()
{
#if defined(BL_USE_AL_API)
	if (_PrAudioMem->pBackMusic)
		_DiscardResource(_PrAudioMem->pBackMusic->nID, _UnloadAudio, _ALSoundRelease);
    _ALDestroy();
#elif defined(BL_USE_SL_API)
	if (_PrAudioMem->pBackMusic)
		_DiscardResource(_PrAudioMem->pBackMusic->nID, _UnloadAudio, _SLSoundRelease);
    _SLDestroy();
#elif defined(BL_USE_COREAUDIO_API)
	if (_PrAudioMem->pBackMusic)
		_DiscardResource(_PrAudioMem->pBackMusic->nID, _UnloadAudio, _CASoundRelease);
    _CADestroy();
#endif
	if (_PrAudioMem->pBackMusic)
	{
		blDeleteGuid(_PrAudioMem->pBackMusic->nID);
		free(_PrAudioMem->pBackMusic);
		_PrAudioMem->pBackMusic = NULL;
	}
	blDebugOutput("Audio shutdown");
	free(_PrAudioMem);
}
BLVoid
blMusicVolume(IN BLF32 _Vol)
{
    if (!_PrAudioMem->pBackMusic)
        return;
    _PrAudioMem->pAudioDev.fMusicVolume = _Vol;
    blMutexLock(_PrAudioMem->pMusicMutex);
#if defined(BL_USE_AL_API)
    alSourcef(_PrAudioMem->pBackMusic->nSource, AL_GAIN, _Vol);
#elif defined(BL_USE_SL_API)
    SLVolumeItf _func;
    (*_PrAudioMem->pBackMusic->pSource)->GetInterface(_PrAudioMem->pBackMusic->pSource, SL_IID_VOLUME, &_func);
    (*_func)->SetVolumeLevel(_func, _Vol);
#elif defined(BL_USE_COREAUDIO_API)
	_PrAudioMem->pBackMusic->pSource->SetVolume(_Vol);
#endif
    blMutexUnlock(_PrAudioMem->pMusicMutex);
}
BLVoid
blEnvironmentVolume(IN BLF32 _Vol)
{
	_PrAudioMem->pAudioDev.fEnvVolume = _Vol;
    blMutexLock(_PrAudioMem->pSounds->pMutex);
    FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
    {
		if (_iter->bLoop)
		{
#if defined(BL_USE_AL_API)
			alSourcef(_iter->nSource, AL_GAIN, _Vol);
#elif defined(BL_USE_SL_API)
            SLVolumeItf _func;
            (*_iter->pSource)->GetInterface(_iter->pSource, SL_IID_VOLUME, &_func);
            (*_func)->SetVolumeLevel(_func, _Vol);
#elif defined(BL_USE_COREAUDIO_API)
			_iter->pSource->SetVolume(_Vol);
#endif
        }
    }
    blMutexUnlock(_PrAudioMem->pSounds->pMutex);
}
BLVoid
blSystemVolume(IN BLF32 _Vol)
{
	_PrAudioMem->pAudioDev.fSystemVolume = _Vol;
    blMutexLock(_PrAudioMem->pSounds->pMutex);
    FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
    {
		if (!_iter->bLoop)
        {
#if defined(BL_USE_AL_API)
			alSourcef(_iter->nSource, AL_GAIN, _Vol);
#elif defined(BL_USE_SL_API)
            SLVolumeItf _func;
            (*_iter->pSource)->GetInterface(_iter->pSource, SL_IID_VOLUME, &_func);
            (*_func)->SetVolumeLevel(_func, _Vol);
#elif defined(BL_USE_COREAUDIO_API)
			_iter->pSource->SetVolume(_Vol);
#endif
        }
    }
    blMutexUnlock(_PrAudioMem->pSounds->pMutex);
}
BLVoid
blVolumeQuery(OUT BLF32* _Music, OUT BLF32* _System, OUT BLF32* _Env)
{
	*_Music = _PrAudioMem->pAudioDev.fMusicVolume;
	*_System = _PrAudioMem->pAudioDev.fSystemVolume;
	*_Env = _PrAudioMem->pAudioDev.fEnvVolume;
}
BLVoid
blListenerPos(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_PrAudioMem->sListenerPos.fX = _Xpos;
	_PrAudioMem->sListenerPos.fY = _Ypos;
	_PrAudioMem->sListenerPos.fZ = _Zpos;
}
BLVoid
blListenerUp(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_PrAudioMem->sListenerUp.fX = _Xpos;
	_PrAudioMem->sListenerUp.fY = _Ypos;
	_PrAudioMem->sListenerUp.fZ = _Zpos;
}
BLVoid
blListenerDir(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_PrAudioMem->sListenerDir.fX = _Xpos;
	_PrAudioMem->sListenerDir.fY = _Ypos;
	_PrAudioMem->sListenerDir.fZ = _Zpos;
}
BLVoid
blEmitterPos(IN BLGuid _ID, IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_BLAudioSource* _sound = (_BLAudioSource*)blGuidAsPointer(_ID);
	if (!_sound)
			return;
	blMutexLock(_PrAudioMem->pMusicMutex);
	_sound->sPos.fX = _Xpos;
	_sound->sPos.fY = _Ypos;
	_sound->sPos.fZ = _Zpos;
	blMutexUnlock(_PrAudioMem->pSounds->pMutex);
}
BLGuid
blGenAudio(IN BLAnsi* _Filename, IN BLBool _Loop, IN BLBool _3D, IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_BLAudioSource* _sound = (_BLAudioSource*)malloc(sizeof(_BLAudioSource));
	_sound->bLoop = _Loop;
	_sound->b3d = _3D;
	_sound->sPos.fX = _Xpos;
	_sound->sPos.fY = _Ypos;
	_sound->sPos.fZ = _Zpos;
	_sound->nID = blGenGuid(_sound, blHashUtf8((const BLUtf8*)_Filename));
	_sound->bValid = FALSE;
#if defined(BL_USE_AL_API)
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _ALSoundSetup, TRUE);
#elif defined(BL_USE_SL_API)
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _SLSoundSetup, TRUE);
#elif defined(BL_USE_COREAUDIO_API)
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _CASoundSetup, TRUE);
#endif
	return _sound->nID;
}
BLVoid
blDeleteAudio(IN BLGuid _ID)
{
	if (_ID == INVALID_GUID)
		return;
	else if(_ID == ASSOCIATED_GUID)
	{
		blMutexLock(_PrAudioMem->pSounds->pMutex);
		FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
		{
#if defined(BL_USE_AL_API)
			_DiscardResource(_iter->nID, _UnloadAudio, _ALSoundRelease);
#elif defined(BL_USE_SL_API)
			_DiscardResource(_iter->nID, _UnloadAudio, _SLSoundRelease);
#elif defined(BL_USE_COREAUDIO_API)
			_DiscardResource(_iter->nID, _UnloadAudio, _CASoundRelease);
#endif
			blDeleteGuid(_iter->nID);
		}
		blClearList(_PrAudioMem->pSounds);
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	}
	else
	{
		if (_ID == _PrAudioMem->pBackMusic->nID)
        {
            blMutexLock(_PrAudioMem->pMusicMutex);
#if defined(BL_USE_AL_API)
			_DiscardResource(_ID, _UnloadAudio, _ALSoundRelease);
#elif defined(BL_USE_SL_API)
			_DiscardResource(_ID, _UnloadAudio, _SLSoundRelease);
#elif defined(BL_USE_COREAUDIO_API)
			_DiscardResource(_ID, _UnloadAudio, _CASoundRelease);
#endif
			free(_PrAudioMem->pBackMusic);
			_PrAudioMem->pBackMusic = NULL;
            blDeleteGuid(_ID);
            blMutexUnlock(_PrAudioMem->pMusicMutex);
            return;
		}
		blMutexLock(_PrAudioMem->pSounds->pMutex);
		FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
		{
			if (_iter->nID == _ID)
			{
				blListErase(_PrAudioMem->pSounds, _iterator_iter);
#if defined(BL_USE_AL_API)
				_DiscardResource(_ID, _UnloadAudio, _ALSoundRelease);
#elif defined(BL_USE_SL_API)
				_DiscardResource(_ID, _UnloadAudio, _SLSoundRelease);
#elif defined(BL_USE_COREAUDIO_API)
				_DiscardResource(_ID, _UnloadAudio, _CASoundRelease);
#endif
				free(_iterator_iter);
				blDeleteGuid(_ID);
				break;
			}
		}
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	}
}
BLVoid
blPCMStreamParam(IN BLU32 _Channels, IN BLU32 _SamplesPerSec)
{
	if (_Channels == 0xFFFFFFFF && _SamplesPerSec == 0xFFFFFFFF)
	{
		_GbTVMode = FALSE;
#if defined(BL_USE_AL_API)
		if (_PrAudioMem->pPCMStream != 0xFFFFFFFF)
		{
            alSourceStop(_PrAudioMem->pPCMStream);
            ALint _numprocessed;
            alGetSourcei(_PrAudioMem->pPCMStream, AL_BUFFERS_PROCESSED, &_numprocessed);
            while (_numprocessed--)
            {
                ALuint _buff;
                alSourceUnqueueBuffers(_PrAudioMem->pPCMStream, 1, &_buff);
                alDeleteBuffers(1, &_buff);
            }
            alSourceStop(_PrAudioMem->pPCMStream);
            alDeleteSources(1, &_PrAudioMem->pPCMStream);
            _PrAudioMem->pPCMStream = -1;
		}
#elif defined(BL_USE_SL_API)
		if ((*_PrAudioMem->pPCMStream))
			(*_PrAudioMem->pPCMStream)->Destroy(_PrAudioMem->pPCMStream);
#elif defined(BL_USE_COREAUDIO_API)
		if (_PrAudioMem->pPCMStream)
		{
			_PrAudioMem->pPCMStream->Stop(0);
			_PrAudioMem->pPCMStream->DestroyVoice();
		}
#endif
		for (BLU32 _idx = 0; _idx < 8; ++_idx)
		{
			if (_PrAudioMem->aPCMBuf[_idx])
			{
				free(_PrAudioMem->aPCMBuf[_idx]);
				_PrAudioMem->aPCMBuf[_idx] = NULL;
			}
		}
	}
	else
	{
		_GbTVMode = TRUE;
		if (_PrAudioMem->nPCMChannels != _Channels && _PrAudioMem->nPCMSampleRate != _SamplesPerSec)
		{
            _PrAudioMem->nPCMChannels = _Channels;
            _PrAudioMem->nPCMSampleRate = _SamplesPerSec;
#if defined(BL_USE_AL_API)
            if (_PrAudioMem->pPCMStream != 0xFFFFFFFF)
            {
                alSourceStop(_PrAudioMem->pPCMStream);
                ALint _numprocessed;
                alGetSourcei(_PrAudioMem->pPCMStream, AL_BUFFERS_PROCESSED, &_numprocessed);
                while (_numprocessed--)
                {
                    ALuint _buff;
                    alSourceUnqueueBuffers(_PrAudioMem->pPCMStream, 1, &_buff);
                    alDeleteBuffers(1, &_buff);
                }
                alSourceStop(_PrAudioMem->pPCMStream);
                alDeleteSources(1, &_PrAudioMem->pPCMStream);
            }
            alGenSources(1, &_PrAudioMem->pPCMStream);
            alSourcei(_PrAudioMem->pPCMStream, AL_SOURCE_RELATIVE, TRUE);
            alSourcef(_PrAudioMem->pPCMStream, AL_GAIN, 1.f);
            assert(alGetError() == AL_NO_ERROR);
#elif defined(BL_USE_SL_API)
			if ((*_PrAudioMem->pPCMStream))
				(*_PrAudioMem->pPCMStream)->Destroy(_PrAudioMem->pPCMStream);
#elif defined(BL_USE_COREAUDIO_API)
			if (_PrAudioMem->pPCMStream)
			{
				_PrAudioMem->pPCMStream->Stop(0);
				_PrAudioMem->pPCMStream->DestroyVoice();
			}
			WAVEFORMATEX _pwfx;
			_pwfx.wFormatTag = WAVE_FORMAT_PCM;
			_pwfx.nChannels = _Channels;
			_pwfx.wBitsPerSample = 16;
			_pwfx.nSamplesPerSec = _SamplesPerSec;
			_pwfx.nBlockAlign = _pwfx.nChannels*_pwfx.wBitsPerSample / 8;
			_pwfx.nAvgBytesPerSec = _pwfx.nBlockAlign * _pwfx.nSamplesPerSec;
			_pwfx.cbSize = 0;
			HRESULT _rsz = _PrAudioMem->pAudioDev.pCADevice->CreateSourceVoice(&_PrAudioMem->pPCMStream, &_pwfx);
			assert(SUCCEEDED(_rsz));
			_PrAudioMem->pPCMStream->Start(0);
#endif
		}
	}
}
BLVoid
blPCMStreamData(IN BLS16* _PCM, IN BLU32 _Length)
{
	if (_PrAudioMem->nPCMFill >= 65536)
	{
#if defined(BL_USE_COREAUDIO_API)
		XAUDIO2_VOICE_STATE _state;
		_PrAudioMem->pPCMStream->GetState(&_state);
		XAUDIO2_BUFFER _buf = { 0 };
		_buf.AudioBytes = _PrAudioMem->nPCMFill;
		_buf.pAudioData = _PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8];
		_PrAudioMem->pPCMStream->SubmitSourceBuffer(&_buf);
#elif defined(BL_USE_AL_API)
        ALint _processed = 0;
        ALenum _state = 0;
        ALuint _buffer = 0;
        alGenBuffers(1, &_buffer);
        alBufferData(_buffer, (_PrAudioMem->nPCMChannels == 2) ? 0x1103 : 0x1101, _PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8], _PrAudioMem->nPCMFill, _PrAudioMem->nPCMSampleRate);
        alSourceQueueBuffers(_PrAudioMem->pPCMStream, 1, &_buffer);
        alGetSourcei(_PrAudioMem->pPCMStream, AL_SOURCE_STATE, &_state);
        if (_state != AL_PLAYING)
            alSourcePlay(_PrAudioMem->pPCMStream);
        alGetSourcei(_PrAudioMem->pPCMStream, AL_BUFFERS_PROCESSED, &_processed);
        while (_processed--)
        {
            ALuint _buff;
            alSourceUnqueueBuffers(_PrAudioMem->pPCMStream, 1, &_buff);
            alDeleteBuffers(1, &_buff);
        }
        assert(alGetError() == AL_NO_ERROR);
#elif defined(BL_USE_SL_API)
#else
#endif
		_PrAudioMem->nPCMFill = 0;
		_PrAudioMem->nPCMBufTurn++;
		_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] = (BLU8*)malloc(80000);
		memcpy(_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] + _PrAudioMem->nPCMFill, _PCM, _Length);
		_PrAudioMem->nPCMFill += _Length;
	}
	else if (!_PrAudioMem->nPCMFill && _Length)
	{
		_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] = (BLU8*)malloc(80000);
		memcpy(_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] + _PrAudioMem->nPCMFill, _PCM, _Length);
		_PrAudioMem->nPCMFill += _Length;
	}
	else if (_PrAudioMem->nPCMFill && _Length)
	{
		memcpy(_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] + _PrAudioMem->nPCMFill, _PCM, _Length);
		_PrAudioMem->nPCMFill += _Length;
	}
}
