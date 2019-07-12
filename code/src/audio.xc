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
#include "../headers/util.h"
#include "../headers/system.h"
#include "internal/mathematics.h"
#include "internal/list.h"
#include "internal/thread.h"
#include "internal/internal.h"
#include "../externals/mpaudec/mpaudec.h"
#include "../externals/duktape/duktape.h"
#if defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_OSX)
#   define BL_USE_AL_API
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(BL_PLATFORM_LINUX)
#   define BL_USE_AL_API
#include <AL/al.h>
#include <AL/alc.h>
#elif defined(BL_PLATFORM_WEB)
#	define BL_USE_WEB_API
#include <emscripten/emscripten.h>
#elif defined(BL_PLATFORM_ANDROID)
#   define BL_USE_SL_API
#include <SLES/OpenSLES.h>
#elif defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
#   define BL_USE_COREAUDIO_API
#	if(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#		include <xaudio2.h>
#		include <x3daudio.h>
#	else
#		include <xaudio2.h>
#		include <x3daudio.h>
#	endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern BLBool _FetchResource(const BLAnsi*, BLVoid**, BLGuid, BLBool(*)(BLVoid*, const BLAnsi*),BLBool(*)(BLVoid*), BLBool);
extern BLBool _DiscardResource(BLGuid, BLBool(*)(BLVoid*), BLBool(*)(BLVoid*));
#ifdef __cplusplus
}
#endif
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
#elif defined(BL_USE_WEB_API)
    BLU8* pBuffer;
#endif
    BLThread* pThread;
    BLF32 fMusicVolume;
    BLF32 fSystemVolume;
    BLF32 fEnvVolume;
}_BLAudioDevice;
#if defined(BL_USE_WEB_API)
typedef struct _AudioRingNode{
    BLU32 nHead;
    BLU32 nTail;
    BLU32 nNum;
    BLU32 aQueue[1024];
} _BLAudioRingNode;

#define _SAUDIO_DEFAULT_SAMPLE_RATE (44100)
#define _SAUDIO_DEFAULT_BUFFER_FRAMES (2048)
#define _SAUDIO_DEFAULT_PACKET_FRAMES (128)
#define _SAUDIO_DEFAULT_BUFFER_CHANNEL (1)
#define _SAUDIO_DEFAULT_NUM_PACKETS ((_SAUDIO_DEFAULT_BUFFER_FRAMES/_SAUDIO_DEFAULT_PACKET_FRAMES)*4)
#endif
typedef struct _AudioSource{
    BLBool bLoop;
	BLBool bValid;
	BLBool b3d;
	BLVec3 sPos;
    BLGuid nID;
#if defined(BL_USE_AL_API)
	BLS32 nOffset;
	BLS32 nFrequency;
	BLU32 nChannels;
	BLS32 nBPS;
	MPAuDecContext* pMp3Context;
	BLS32 nMp3Size;
	BLS32 nMp3Read;
	BLU8 aMp3Packet[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
	BLGuid nStream;
	BLU8* pSoundBufA;
	BLU8* pSoundBufB;
	BLU8* pSoundBufC;
    ALuint aBuffers[3];
    ALuint nSource;
    BLU32 nBufTurn;
#elif defined(BL_USE_SL_API)
	BLS32 nOffset;
	BLS32 nFrequency;
	BLU32 nChannels;
	BLS32 nBPS;
	MPAuDecContext* pMp3Context;
	BLS32 nMp3Size;
	BLS32 nMp3Read;
	BLU8 aMp3Packet[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
	BLGuid nStream;
	BLU8* pSoundBufA;
	BLU8* pSoundBufB;
	BLU8* pSoundBufC;
    SLObjectItf pSource;
    SLBufferQueueItf pBufferFunc;
    BLU32 nBufTurn;
#elif defined(BL_USE_COREAUDIO_API)
	BLS32 nOffset;
	BLS32 nFrequency;
	BLU32 nChannels;
	BLS32 nBPS;
	MPAuDecContext* pMp3Context;
	BLS32 nMp3Size;
	BLS32 nMp3Read;
	BLU8 aMp3Packet[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
	BLGuid nStream;
	BLU8* pSoundBufA;
	BLU8* pSoundBufB;
	BLU8* pSoundBufC;
	IXAudio2SourceVoice* pSource;
    BLU32 nBufTurn;
#elif defined(BL_USE_WEB_API)
	BLS32 nOffset;
	BLS32 nFrequency;
	BLU32 nChannels;
	BLS32 nBPS;
	MPAuDecContext* pMp3Context;
	BLS32 nMp3Size;
	BLS32 nMp3Read;
	BLU8 aMp3Packet[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
	BLGuid nStream;
#endif
}_BLAudioSource;
typedef struct _AudioMember {
	DUK_CONTEXT* pDukContext;
	_BLAudioDevice pAudioDev;
	_BLAudioSource* pBackMusic;
	BLList* pSounds;
	blMutex* pMusicMutex;
	BLVec3 sListenerUp;
	BLVec3 sListenerDir;
	BLVec3 sListenerPos;
	BLVec3 sListenerLastPos;
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
#elif defined(BL_USE_WEB_API)
	_BLAudioRingNode sReadQueue;
	_BLAudioRingNode sWriteQueue;
	BLS32 nCurPacket;
	BLS32 nCurOffset;
	BLU8* pBackend;
	BLU8* pBasePtr;
#endif
}_BLAudioMember;
static _BLAudioMember* _PrAudioMem = NULL;
static BLBool
_MakeStream(_BLAudioSource* _Src, BLU32 _Bufidx, BLU32* _TotalRead)
{
	BLU32 _totalread = 0;
	BLU32 _samplerate;
	BLBool _ret = TRUE;
#if !defined(BL_USE_WEB_API)
	if (_Src->pMp3Context)
	{
		_samplerate = _Src->pMp3Context->sample_rate;
		while (_totalread < 65536)
		{
			BLU32 _actualread = 0;
			if (_Src->nMp3Size <= _Src->nMp3Read)
			{
				_Src->nMp3Size = 0;
				_Src->nMp3Read = 0;
				memset(_Src->aMp3Packet, 0, sizeof(_Src->aMp3Packet));
				BLU8 _inputbuf[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
				BLU32 _oldpos = blStreamTell(_Src->nStream);
				BLU32 _inputsz = blStreamRead(_Src->nStream, _Src->pMp3Context->frame_size > MPAUDEC_MAX_AUDIO_FRAME_SIZE ? MPAUDEC_MAX_AUDIO_FRAME_SIZE : _Src->pMp3Context->frame_size, _inputbuf);
				if (_inputsz)
					blStreamSeek(_Src->nStream, _oldpos + mpaudec_decode_frame(_Src->pMp3Context, _Src->aMp3Packet, &_Src->nMp3Size, _inputbuf, _inputsz));
			}
			if (_Src->nMp3Size > _Src->nMp3Read)
			{
				BLU32 _amountleft = _Src->nMp3Size - _Src->nMp3Read;
				if (_amountleft < 65536 - _totalread)
				{
					if (_Bufidx == 0)
						memcpy(_Src->pSoundBufA + _totalread, _Src->aMp3Packet + _Src->nMp3Read, _amountleft);
					else if (_Bufidx == 1)
						memcpy(_Src->pSoundBufB + _totalread, _Src->aMp3Packet + _Src->nMp3Read, _amountleft);
					else
						memcpy(_Src->pSoundBufC + _totalread, _Src->aMp3Packet + _Src->nMp3Read, _amountleft);
					_Src->nMp3Read += _amountleft;
					_actualread = _amountleft;
				}
				else
				{
					if (_Bufidx == 0)
						memcpy(_Src->pSoundBufA + _totalread, _Src->aMp3Packet + _Src->nMp3Read, 65536 - _totalread);
					else if (_Bufidx == 1)
						memcpy(_Src->pSoundBufB + _totalread, _Src->aMp3Packet + _Src->nMp3Read, 65536 - _totalread);
					else
						memcpy(_Src->pSoundBufC + _totalread, _Src->aMp3Packet + _Src->nMp3Read, 65536 - _totalread);
					_Src->nMp3Read += 65536 - _totalread;
					_actualread = 65536 - _totalread;
				}
			}
			if (_actualread > 0)
				_totalread += _actualread;
			else
			{
				if (_Src->bLoop)
					blStreamSeek(_Src->nStream, _Src->nOffset);
				else
					_ret = FALSE;
				break;
			}
		}
	}
	else
	{
		_samplerate = _Src->nFrequency;
		while (_totalread < 65536)
		{
			BLU32 _actualread;
			if (_Bufidx == 0)
				_actualread = blStreamRead(_Src->nStream, 65536 - _totalread, _Src->pSoundBufA + _totalread);
			else if (_Bufidx == 1)
				_actualread = blStreamRead(_Src->nStream, 65536 - _totalread, _Src->pSoundBufB + _totalread);
			else
				_actualread = blStreamRead(_Src->nStream, 65536 - _totalread, _Src->pSoundBufC + _totalread);
			if (_actualread > 0)
				_totalread += _actualread;
			else
			{
				if (_Src->bLoop)
					blStreamSeek(_Src->nStream, _Src->nOffset);
				else
					_ret = FALSE;
				break;
			}
		}
	}
	*_TotalRead = _totalread;
#endif
    return _ret;
}
static BLBool
_LoadAudio(BLVoid* _Src, const BLAnsi* _Filename)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	_src->nStream = blGenStream(_Filename);
	if (_src->nStream == INVALID_GUID)
		return FALSE;
	if (blUtf8Equal(blFileSuffixUtf8((BLUtf8*)_Filename), (const BLUtf8*)"mp3"))
	{
		_src->pMp3Context = (MPAuDecContext*)malloc(sizeof(struct MPAuDecContext));
		if (mpaudec_init(_src->pMp3Context) < 0)
		{
			free(_src->pMp3Context);
			_src->pMp3Context = NULL;
			return FALSE;
		}
		BLS8 _idv3header[10];
		BLS32 _amountread = blStreamRead(_src->nStream, 10, &_idv3header);
		if (_amountread == 10 && _idv3header[0] == 'I' && _idv3header[1] == 'D' && _idv3header[2] == '3')
		{
			BLS32 _size = 0;
			_size = (_idv3header[6] & 0x7f) << (3 * 7);
			_size |= (_idv3header[7] & 0x7f) << (2 * 7);
			_size |= (_idv3header[8] & 0x7f) << (1 * 7);
			_size |= (_idv3header[9] & 0x7f);
			_size += 10;
			_src->nOffset = _size;
			blStreamSeek(_src->nStream, _src->nOffset);
		}
		else
			return FALSE;
		_src->pMp3Context->parse_only = 1;
		BLU8 _tmpbuf[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
		BLS32 _outputsz = 0;
		BLU8 _inputbuf[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
		BLS32 _inputsz = blStreamRead(_src->nStream, 4096, _inputbuf);
		BLS32 _rv = mpaudec_decode_frame(_src->pMp3Context, _tmpbuf, &_outputsz, _inputbuf, _inputsz);
		assert(_rv >= 0);
		_src->nChannels = _src->pMp3Context->channels;
		_src->nFrequency = _src->pMp3Context->sample_rate;
		_src->nBPS = 16;
		_src->pMp3Context->parse_only = 0;
		blStreamSeek(_src->nStream, _src->nOffset);
		_src->nMp3Read = _src->nMp3Size = 0;
	}
	else
	{
		BLAnsi _ident[4];
		BLS32 _tempint32 = 0;
		BLS16 _tempint16 = 0;
		BLU32 _startoffset = 0;
		blStreamRead(_src->nStream, 4, _ident);
		if (strncmp(_ident, "RIFF", 4) == 0)
		{
			blStreamRead(_src->nStream, 4, &_tempint32);
			if (_tempint32 >= 44)
			{
				blStreamRead(_src->nStream, 4, _ident);
				if (strncmp(_ident, "WAVE", 4) == 0)
				{
					_startoffset = blStreamTell(_src->nStream);
					do {
						blStreamRead(_src->nStream, 4, _ident);
					} while ((strncmp(_ident, "fmt ", 4) != 0) && !blStreamEos(_src->nStream));
					if (blStreamTell(_src->nStream) < (blStreamLength(_src->nStream) - 16))
					{
						blStreamRead(_src->nStream, 4, &_tempint32);
						if (_tempint32 >= 16)
						{
							blStreamRead(_src->nStream, 2, &_tempint16);
							if (_tempint16 == 1)
							{
								blStreamRead(_src->nStream, 2, &_tempint16);
								_src->nChannels = _tempint16;
								if (_src->nChannels == 1 || _src->nChannels == 2)
								{
									blStreamRead(_src->nStream, 4, &_tempint32);
									_src->nFrequency = _tempint32;
									blStreamRead(_src->nStream, 4, &_tempint32);
									blStreamRead(_src->nStream, 2, &_tempint16);
									blStreamRead(_src->nStream, 2, &_tempint16);
									_src->nBPS = _tempint16;
									if (_src->nBPS == 8 || _src->nBPS == 16)
									{
										blStreamSeek(_src->nStream, _startoffset);
										do {
											blStreamRead(_src->nStream, 4, _ident);
										} while ((strncmp(_ident, "data", 4) != 0) && (!blStreamEos(_src->nStream)));
										if (!blStreamEos(_src->nStream))
										{
											blStreamRead(_src->nStream, 4, &_tempint32);
											_src->nOffset = blStreamTell(_src->nStream);
										}
									}
									else
										return FALSE;
								}
								else
									return FALSE;
							}
							else
								return FALSE;
						}
					}
				}
				else
				{
					assert(0);
					return FALSE;
				}
			}
			else
			{
				assert(0);
				return FALSE;
			}
		}
		else
		{
			assert(0);
			return FALSE;
		}
	}	
	return TRUE;
}
static BLBool
_UnloadAudio(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;	
	blMutexLock(_PrAudioMem->pMusicMutex);
	blMutexLock(_PrAudioMem->pSounds->pMutex);
	if (_src->pMp3Context)
	{
		mpaudec_clear(_src->pMp3Context);
		free(_src->pMp3Context);
	}
	blDeleteStream(_src->nStream);
	if (!_src->b3d && _src->bLoop)
		_PrAudioMem->pBackMusic = NULL;
	blMutexUnlock(_PrAudioMem->pMusicMutex);
	blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	return TRUE;
}
#if defined(BL_USE_AL_API)
static BLBool
_ALSoundSetup(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
    ALsizei _queuesz = 0;
	_src->pSoundBufA = (BLU8*)calloc(1, 65536);
	_src->pSoundBufB = (BLU8*)calloc(1, 65536);
	_src->pSoundBufC = (BLU8*)calloc(1, 65536);
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
	ALint _alfmt = 0;
	if (_src->nChannels == 1 && _src->nBPS == 8)
		_alfmt = 0x1100;
	else if (_src->nChannels == 1 && _src->nBPS == 16)
		_alfmt = 0x1101;
	else if (_src->nChannels == 2 && _src->nBPS == 8)
		_alfmt = 0x1102;
	else
		_alfmt = 0x1103;
    for (BLU32 _i = 0; _i < 3; ++_i)
    {
        BLU32 _tmpsz;
        if (_MakeStream(_src, _i, &_tmpsz))
        {
            switch (_i)
            {
                case 0:
                    alBufferData(_src->aBuffers[0], _alfmt, _src->pSoundBufA, _tmpsz, _src->nFrequency);
                    break;
                case 1:
                    alBufferData(_src->aBuffers[1], _alfmt, _src->pSoundBufB, _tmpsz, _src->nFrequency);
                    break;
                case 2:
                    alBufferData(_src->aBuffers[2], _alfmt, _src->pSoundBufC, _tmpsz, _src->nFrequency);
                    break;
                default:
                    break;
            }
            ++_queuesz;
        }
    }
    alSourceQueueBuffers(_src->nSource, _queuesz, _src->aBuffers);
	alSourcePlay(_src->nSource);
	if (!_src->b3d && _src->bLoop)
		_PrAudioMem->pBackMusic = _src;
	else
		blListPushBack(_PrAudioMem->pSounds, _src);
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
	ALint _alfmt = 0;
	if (_Src->nChannels == 1 && _Src->nBPS == 8)
		_alfmt = 0x1100;
	else if (_Src->nChannels == 1 && _Src->nBPS == 16)
		_alfmt = 0x1101;
	else if (_Src->nChannels == 2 && _Src->nBPS == 8)
		_alfmt = 0x1102;
	else
		_alfmt = 0x1103;
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
					alBufferData(_Src->aBuffers[0], _alfmt, _Src->pSoundBufA, _totalread, _Src->nFrequency);
                alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
            else if (_Src->aBuffers[1] == _buffer)
            {
                if (_MakeStream(_Src, 1, &_totalread))
					alBufferData(_Src->aBuffers[1], _alfmt, _Src->pSoundBufB, _totalread, _Src->nFrequency);
                alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
            else if (_Src->aBuffers[2] == _buffer)
            {
                if (_MakeStream(_Src, 2, &_totalread))
					alBufferData(_Src->aBuffers[2], _alfmt, _Src->pSoundBufC, _totalread, _Src->nFrequency);
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
	_src->pSoundBufA = (BLU8*)calloc(1, 65536);
	_src->pSoundBufB = (BLU8*)calloc(1, 65536);
	_src->pSoundBufC = (BLU8*)calloc(1, 65536);
	_src->nBufTurn = 0;
    SLDataLocator_BufferQueue _bufq = {SL_DATALOCATOR_BUFFERQUEUE, 3};
    SLDataFormat_PCM _pcm;
    _pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    _pcm.formatType = SL_DATAFORMAT_PCM;
	_pcm.numChannels = _src->nChannels;
    _pcm.channelMask = (2 == _src->nChannels) ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER;
	_pcm.bitsPerSample = (_src->nBPS == 8) ? SL_PCMSAMPLEFORMAT_FIXED_8 : SL_PCMSAMPLEFORMAT_FIXED_16;
    _pcm.containerSize = (_src->nBPS == 8) ? SL_PCMSAMPLEFORMAT_FIXED_8 : SL_PCMSAMPLEFORMAT_FIXED_16;
	_pcm.samplesPerSec = _src->nFrequency * 1000;
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
	_src->pSoundBufA = (BLU8*)calloc(1, 65536);
	_src->pSoundBufB = (BLU8*)calloc(1, 65536);
	_src->pSoundBufC = (BLU8*)calloc(1, 65536);
	WAVEFORMATEX _pwfx;
	_pwfx.wFormatTag = WAVE_FORMAT_PCM;
	_pwfx.nChannels = _src->nChannels;
	_pwfx.wBitsPerSample = _src->nBPS;
	_pwfx.nSamplesPerSec = _src->nFrequency;
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
	if (!_src->b3d && _src->bLoop)
		_PrAudioMem->pBackMusic = _src;
	else
		blListPushBack(_PrAudioMem->pSounds, _src);
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
#elif defined(BL_USE_WEB_API)
static BLBool
_WEBSoundSetup(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	if (!_src->b3d && _src->bLoop)
	{
		//_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fMusicVolume);
	}
	else
	{
		//if (!_src->b3d)
		//	_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fSystemVolume);
		//else
		//	_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fEnvVolume);
	}
	if (!_src->b3d && _src->bLoop)
		_PrAudioMem->pBackMusic = _src;
	else
		blListPushBack(_PrAudioMem->pSounds, _src);
	_src->bValid = TRUE;
	return TRUE;
}
static BLBool
_WEBSoundRelease(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	if (!_src->b3d && _src->bLoop)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	_src->bValid = FALSE;
	if (!_src->b3d && _src->bLoop)
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	else
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	return TRUE;
}
#define BUF_SIZE (32)
static BLBool
_WEBUpdate(_BLAudioSource* _Src)
{
	float buf[BUF_SIZE];
	int buf_pos = 0;
	uint32_t count = 0;

	BLS32 _count;
	if (_PrAudioMem->sWriteQueue.nHead >= _PrAudioMem->sWriteQueue.nTail)
		_count = _PrAudioMem->sWriteQueue.nHead - _PrAudioMem->sWriteQueue.nTail;
	else
		_count = (_PrAudioMem->sWriteQueue.nHead + _PrAudioMem->sWriteQueue.nNum) - _PrAudioMem->sWriteQueue.nTail;
	BLS32 _bytes = (_count * _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL);
	if (_PrAudioMem->nCurPacket != -1)
		_bytes += _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL - _PrAudioMem->nCurOffset;
	BLS32 _numframes = _bytes / (sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL);
	for (BLS32 _i = 0; _i < _numframes; _i++) 
	{
		buf[buf_pos++] = (count++ & (1 << 3)) ? 0.5f : -0.5f;
		if (buf_pos == BUF_SIZE) 
		{
			buf_pos = 0;

			const BLS32 _bytes = BUF_SIZE * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL;
			const BLU8* _ptr = (const BLU8*)buf;
			BLS32 _allcopy = _bytes;
			while (_allcopy > 0)
			{
				if (_PrAudioMem->nCurPacket == -1)
				{
					if (_PrAudioMem->sWriteQueue.nHead != _PrAudioMem->sWriteQueue.nTail)
					{
						BLS32 _val = _PrAudioMem->sWriteQueue.aQueue[_PrAudioMem->sWriteQueue.nTail];
						BLS32 _i = _PrAudioMem->sWriteQueue.nTail + 1;
						_PrAudioMem->sWriteQueue.nTail = (BLU16)(_i % _PrAudioMem->sWriteQueue.nNum);
						_PrAudioMem->nCurPacket = _val;
					}
				}
				if (_PrAudioMem->nCurPacket != -1)
				{
					BLS32 _tocopy = _allcopy;
					const BLS32 _maxcopy = _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL - _PrAudioMem->nCurOffset;
					if (_tocopy > _maxcopy)
						_tocopy = _maxcopy;
					BLU8* _dst = _PrAudioMem->pBasePtr + _PrAudioMem->nCurPacket * _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL + _PrAudioMem->nCurOffset;
					memcpy(_dst, _ptr, _tocopy);
					_ptr += _tocopy;
					_PrAudioMem->nCurOffset += _tocopy;
					_allcopy -= _tocopy;
				}
				else
				{
					BLS32 _copied = _bytes - _allcopy;
					return _copied;
				}
				if (_PrAudioMem->nCurOffset == _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL)
				{
					_PrAudioMem->sReadQueue.aQueue[_PrAudioMem->sReadQueue.nHead] = _PrAudioMem->nCurPacket;
					BLS32 _i = _PrAudioMem->sReadQueue.nHead + 1;
					_PrAudioMem->sReadQueue.nHead = (BLU16)(_i % _PrAudioMem->sReadQueue.nNum);
					_PrAudioMem->nCurPacket = -1;
					_PrAudioMem->nCurOffset = 0;
				}
			}
		}
	}
	return TRUE;
	if (!_Src || !_Src->bValid)
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
#elif defined(BL_USE_WEB_API)
			_WEBUpdate(_PrAudioMem->pBackMusic);
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
#elif defined(BL_USE_WEB_API)
				if (_WEBUpdate(_iter))
#endif
				{
					blListErase(_PrAudioMem->pSounds, _iterator_iter);
#if defined(BL_USE_AL_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _ALSoundRelease);
#elif defined(BL_USE_SL_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _SLSoundRelease);
#elif defined(BL_USE_COREAUDIO_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _CASoundRelease);
#elif defined(BL_USE_WEB_API)
					_DiscardResource(_iter->nID, _UnloadAudio, _WEBSoundRelease);
#endif
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
#elif defined(BL_USE_WEB_API)
EMSCRIPTEN_KEEPALIVE BLS32
_EMSCPull(BLS32 _Frames)
{
	if (_Frames == _SAUDIO_DEFAULT_BUFFER_FRAMES)
	{
		const BLS32 _bytes = _Frames * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL;
		BLS32 _copied = 0;
		const BLS32 _needed = _bytes / (_SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL);
		BLU8* _dst = _PrAudioMem->pBackend;
		BLS32 _count;
		if (_PrAudioMem->sReadQueue.nHead >= _PrAudioMem->sReadQueue.nTail)
			_count = _PrAudioMem->sReadQueue.nHead - _PrAudioMem->sReadQueue.nTail;
		else
			_count = (_PrAudioMem->sReadQueue.nHead + _PrAudioMem->sReadQueue.nNum) - _PrAudioMem->sReadQueue.nTail;
		if (_count >= _needed)
		{
			for (BLS32 _idx = 0; _idx < _needed; _idx++)
			{
				BLS32 _index = _PrAudioMem->sReadQueue.aQueue[_PrAudioMem->sReadQueue.nTail];
				BLS32 _i = _PrAudioMem->sReadQueue.nTail + 1;
				_PrAudioMem->sReadQueue.nTail = (BLU16)(_i % _PrAudioMem->sReadQueue.nNum);
				_PrAudioMem->sWriteQueue.aQueue[_PrAudioMem->sWriteQueue.nHead] = _index;
				_i = _PrAudioMem->sWriteQueue.nHead + 1;
				_PrAudioMem->sWriteQueue.nHead = (BLU16)(_i % _PrAudioMem->sWriteQueue.nNum);
				const BLU8* _src = _PrAudioMem->pBasePtr + _index * _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL;
				memcpy(_dst, _src, _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL);
				_dst += _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL;
				_copied += _SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL;
			}
		}
		if (0 == _copied)
			memset(_PrAudioMem->pBackend, 0, _bytes);
		return (BLS32)_PrAudioMem->pBackend;
	}
	else
		return 0;
}
EM_JS(BLS32, _AudioJSInit, (BLS32 _SampleRate, BLS32 _Channels, BLS32 _BufferSz), {
	Module._saudio_context = null;
	Module._saudio_node = null;
	if (typeof AudioContext !== 'undefined')
	{
		Module._saudio_context = new AudioContext({
			sampleRate: _SampleRate,
			latencyHint : 'interactive',
		});
	}
	else if (typeof webkitAudioContext !== 'undefined')
	{
		Module._saudio_context = new webkitAudioContext({
			sampleRate: _SampleRate,
			latencyHint : 'interactive',
		});
	}
	else
	{
		Module._saudio_context = null;
	}
	if (Module._saudio_context)
	{
		Module._saudio_node = Module._saudio_context.createScriptProcessor(_BufferSz, 0, _Channels);
		Module._saudio_node.onaudioprocess = function pump_audio(event)
		{
			var _numframes = event.outputBuffer.length;
			var _ptr = __EMSCPull(_numframes);
			if (_ptr)
			{
				var _channels = event.outputBuffer.numberOfChannels;
				for (var _chn = 0; _chn < _channels; _chn++)
				{
					var _chan = event.outputBuffer.getChannelData(_chn);
					for (var _i = 0; _i < _numframes; _i++)
					{
						_chan[_i] = HEAPF32[(_ptr >> 2) + ((_channels * _i) + _chn)];
					}
				}
			}
		};
		Module._saudio_node.connect(Module._saudio_context.destination);
		var resume_webaudio = function()
		{
			if (Module._saudio_context)
			{
				if (Module._saudio_context.state === 'suspended')
				{
					Module._saudio_context.resume();
				}
			}
		};
		document.addEventListener('click', resume_webaudio, {once:true});
		document.addEventListener('touchstart', resume_webaudio, {once:true });
		document.addEventListener('keydown', resume_webaudio, {once:true });
		return 1;
	}
	else {
		return 0;
	}
});
EM_JS(BLS32, _AudioJSBufferFrames, (void), {
	if (Module._saudio_node) {
		return Module._saudio_node.bufferSize;
	}
	else {
		return 0;
	}
});
static BLVoid
_WEBInit()
{
	if (_AudioJSInit(_SAUDIO_DEFAULT_SAMPLE_RATE, _SAUDIO_DEFAULT_BUFFER_CHANNEL, _SAUDIO_DEFAULT_BUFFER_FRAMES))
	{
		_PrAudioMem->pBackend = (BLU8*)malloc(_SAUDIO_DEFAULT_BUFFER_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL);
		_PrAudioMem->pBasePtr = (BLU8*)malloc(_SAUDIO_DEFAULT_PACKET_FRAMES * sizeof(BLF32) * _SAUDIO_DEFAULT_BUFFER_CHANNEL * _SAUDIO_DEFAULT_NUM_PACKETS);
		_PrAudioMem->nCurPacket = -1;
		_PrAudioMem->nCurOffset = 0;
		_PrAudioMem->sReadQueue.nHead = 0;
		_PrAudioMem->sReadQueue.nTail = 0;
		_PrAudioMem->sReadQueue.nNum = _SAUDIO_DEFAULT_NUM_PACKETS + 1;
		_PrAudioMem->sWriteQueue.nHead = 0;
		_PrAudioMem->sWriteQueue.nTail = 0;
		_PrAudioMem->sWriteQueue.nNum = _SAUDIO_DEFAULT_NUM_PACKETS + 1;
		for (BLS32 _i = 0; _i < _SAUDIO_DEFAULT_NUM_PACKETS; _i++)
		{
			_PrAudioMem->sWriteQueue.aQueue[_PrAudioMem->sWriteQueue.nHead] = _i;
			_PrAudioMem->sWriteQueue.nHead = (_PrAudioMem->sWriteQueue.nHead + 1 % _PrAudioMem->sWriteQueue.nNum);
		}
		_PrAudioMem->pAudioDev.fMusicVolume = _PrAudioMem->pAudioDev.fSystemVolume = _PrAudioMem->pAudioDev.fEnvVolume = 0.7f;
		_PrAudioMem->pSounds = blGenList(TRUE);
		_PrAudioMem->pMusicMutex = blGenMutex();
		_PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
		blThreadRun(_PrAudioMem->pAudioDev.pThread);
		blDebugOutput("Audio initialize successfully");
    }
}
static BLVoid
_WEBDestroy()
{	
    blDeleteThread(_PrAudioMem->pAudioDev.pThread);
    blDeleteMutex(_PrAudioMem->pMusicMutex);
    blDeleteList(_PrAudioMem->pSounds);
    free(_PrAudioMem->pBasePtr);
    free(_PrAudioMem->pBackend);
}
#endif
BLVoid
_AudioInit(DUK_CONTEXT* _DKC)
{
	_PrAudioMem = (_BLAudioMember*)malloc(sizeof(_BLAudioMember));
	_PrAudioMem->pDukContext = _DKC;
	_PrAudioMem->pBackMusic = NULL;
	_PrAudioMem->pSounds = NULL;
	_PrAudioMem->pMusicMutex = NULL;
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
	_PrAudioMem->pPCMStream = NULL;
#elif defined(BL_USE_COREAUDIO_API)
    _CAInit();
	_PrAudioMem->pPCMStream = NULL;
#elif defined(BL_USE_WEB_API)
	_WEBInit();
#endif
}
BLVoid
_AudioStep(BLU32 _Delta)
{
	_WEBUpdate(NULL);
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
#elif defined(BL_USE_WEB_API)
	if (_PrAudioMem->pBackMusic)
		_DiscardResource(_PrAudioMem->pBackMusic->nID, _UnloadAudio, _WEBSoundRelease);
    _WEBDestroy();
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
#elif defined(BL_USE_WEB_API)
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
#elif defined(BL_USE_WEB_API)
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
#elif defined(BL_USE_WEB_API)
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
	_sound->nID = blGenGuid(_sound, blHashString((const BLUtf8*)_Filename));
	_sound->bValid = FALSE;
#if defined(BL_USE_AL_API)
	_sound->pMp3Context = NULL;
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _ALSoundSetup, TRUE);
#elif defined(BL_USE_SL_API)
	_sound->pMp3Context = NULL;
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _SLSoundSetup, TRUE);
#elif defined(BL_USE_COREAUDIO_API)
	_sound->pMp3Context = NULL;
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _CASoundSetup, TRUE);
#elif defined(BL_USE_WEB_API)
	_sound->pMp3Context = NULL;
	_FetchResource(_Filename, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _WEBSoundSetup, TRUE);
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
#elif defined(BL_USE_WEB_API)
			_DiscardResource(_iter->nID, _UnloadAudio, _WEBSoundRelease);
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
#elif defined(BL_USE_WEB_API)
			_DiscardResource(_ID, _UnloadAudio, _WEBSoundRelease);
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
#elif defined(BL_USE_WEB_API)
				_DiscardResource(_ID, _UnloadAudio, _WEBSoundRelease);
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
#if defined(BL_USE_WEB_API)
	return;
#endif
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
		if (_PrAudioMem->pPCMStream)
			(*_PrAudioMem->pPCMStream)->Destroy(_PrAudioMem->pPCMStream);
#elif defined(BL_USE_COREAUDIO_API)
		if (_PrAudioMem->pPCMStream)
		{
			_PrAudioMem->pPCMStream->Stop(0);
			_PrAudioMem->pPCMStream->DestroyVoice();
		}
#elif defined(BL_USE_WEB_API)
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
			for (BLU32 _idx = 0; _idx < 8; ++_idx)
				_PrAudioMem->aPCMBuf[_idx] = (BLU8*)malloc(80000);
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
			if (_PrAudioMem->pPCMStream)
				(*_PrAudioMem->pPCMStream)->Destroy(_PrAudioMem->pPCMStream);
			SLDataLocator_BufferQueue _bufq = { SL_DATALOCATOR_BUFFERQUEUE, 3 };
			SLDataFormat_PCM _pcm;
			_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
			_pcm.formatType = SL_DATAFORMAT_PCM;
			_pcm.numChannels = _Channels;
			_pcm.channelMask = (2 == _Channels) ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER;
			_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
			_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
			_pcm.samplesPerSec = _SamplesPerSec * 1000;
			SLresult _result;
			SLDataSource _data = { &_bufq, &_pcm };
			SLDataLocator_OutputMix _outmix = { SL_DATALOCATOR_OUTPUTMIX, _PrAudioMem->pAudioDev.pMixObj };
			SLDataSink _snk = { &_outmix, NULL };
			const SLInterfaceID _ids[3] = { SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME };
			const SLboolean _req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
			_result = (*_PrAudioMem->pAudioDev.pDevice)->CreateAudioPlayer(_PrAudioMem->pAudioDev.pDevice, &_PrAudioMem->pPCMStream, &_data, &_snk, 3, _ids, _req);
			assert(_result == SL_RESULT_SUCCESS);
			_result = (*_PrAudioMem->pPCMStream)->Realize(_PrAudioMem->pPCMStream, SL_BOOLEAN_FALSE);
			assert(_result == SL_RESULT_SUCCESS);
			SLPlayItf _playfunc;
			_result = (*_PrAudioMem->pPCMStream)->GetInterface(_PrAudioMem->pPCMStream, SL_IID_PLAY, &_playfunc);
			assert(_result == SL_RESULT_SUCCESS);
			_result = (*_playfunc)->SetPlayState(_playfunc, SL_PLAYSTATE_PLAYING);
			assert(_result == SL_RESULT_SUCCESS);
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
#elif defined(BL_USE_WEB_API)
#endif
		}
	}
}
BLVoid
blPCMStreamData(IN BLS16* _PCM, IN BLU32 _Length)
{
#if defined(BL_USE_WEB_API)
	return;
#endif
	if (_PrAudioMem->nPCMFill >= 65536)
	{
#if defined(BL_USE_COREAUDIO_API)
		XAUDIO2_BUFFER _buf = { 0 };
		_buf.AudioBytes = _PrAudioMem->nPCMFill;
		_buf.pAudioData = _PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8];
		_PrAudioMem->pPCMStream->SubmitSourceBuffer(&_buf);
		XAUDIO2_VOICE_STATE _state;
		do {
			_PrAudioMem->pPCMStream->GetState(&_state);
			if (_state.BuffersQueued > 6)
				blTickDelay(10);
		} while (_state.BuffersQueued > 6);
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
		ALint _queued;
		do {
			alGetSourcei(_PrAudioMem->pPCMStream, AL_BUFFERS_QUEUED, &_queued);
			if (_queued > 6)
				blTickDelay(10);
		} while (_queued > 6);
		assert(alGetError() == AL_NO_ERROR);
#elif defined(BL_USE_SL_API)
		SLBufferQueueItf _bufferfunc;
		(*_PrAudioMem->pPCMStream)->GetInterface(_PrAudioMem->pPCMStream, SL_IID_BUFFERQUEUE, &_bufferfunc);
		(*_bufferfunc)->Enqueue(_bufferfunc, _PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8], _PrAudioMem->nPCMFill);
		SLBufferQueueState _queued;
		do {
			(*_bufferfunc)->GetState(_bufferfunc, &_queued);
			if (_queued.count > 6)
				blTickDelay(10);
		} while (_queued.count > 6);
#elif defined(BL_USE_WEB_API)
#endif
		_PrAudioMem->nPCMFill = 0;
		_PrAudioMem->nPCMBufTurn++;
		memcpy(_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] + _PrAudioMem->nPCMFill, _PCM, _Length);
		_PrAudioMem->nPCMFill += _Length;
	}
	else if (_Length)
	{
		memcpy(_PrAudioMem->aPCMBuf[_PrAudioMem->nPCMBufTurn % 8] + _PrAudioMem->nPCMFill, _PCM, _Length);
		_PrAudioMem->nPCMFill += _Length;
	}
}
