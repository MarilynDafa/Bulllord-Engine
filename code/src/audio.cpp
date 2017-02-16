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
#include "../externals/mpaudec/mpaudec.h"
#include "internal/mathematics.h"
#include "internal/list.h"
#include "internal/thread.h"
#include "internal/internal.h"
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
extern BLBool _FetchResource(const BLAnsi*, const BLAnsi*, BLVoid**, BLGuid, BLBool(*)(BLVoid*, const BLAnsi*, const BLAnsi*),BLBool(*)(BLVoid*), BLBool);
extern BLBool _DiscardResource(BLGuid, BLBool(*)(BLVoid*), BLBool(*)(BLVoid*));
#endif
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
#endif
    BLThread* pThread;
    BLF32 fMusicVolume;
    BLF32 fSystemVolume;
    BLF32 fEnvVolume;
}_BLAudioDevice;
typedef struct _AudioSource{
    union{
		struct _Mp3 {
			BLGuid nStreamMp3;
			MPAuDecContext* pContext;
			BLU32 nChannels;
			BLS32 nFrequency;
			BLS32 nBPS;
			BLS32 nSize;
			BLS32 nRead;
			BLU32 nOffset;
			BLU8 aPacket[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
		}sMp3;
		struct _Wav {
			BLGuid nStreamWav;
			BLU32 nChannels;
			BLS32 nOffset;
			BLS32 nFrequency;
			BLS32 nBPS;
		}sWav;
    } uData;
    BLBool bLoop;
    BLBool bCompressed;
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
	_BLAudioDevice pAudioDev;
	_BLAudioSource* pBackMusic;
	BLList* pSounds;
	blMutex* pMusicMutex;
	blMutex* pListenerMutex;
	BLVec3 sListenerUp;
	BLVec3 sListenerDir;
	BLVec3 sListenerPos;
	BLVec3 sListenerLastPos;
}_BLAudioMember;
static _BLAudioMember* _PrAudioMem = NULL;

static BLS32
_Format(_BLAudioSource* _Src)
{
	if (_Src->bCompressed)
	{
		if (_Src->uData.sMp3.nChannels == 1 && _Src->uData.sMp3.nBPS == 8)
			return 0x1100;
		else if (_Src->uData.sMp3.nChannels == 1 && _Src->uData.sMp3.nBPS == 16)
			return 0x1101;
		else if (_Src->uData.sMp3.nChannels == 2 && _Src->uData.sMp3.nBPS == 8)
			return 0x1102;
		else
			return 0x1103;
	}
	else
	{
		if (_Src->uData.sWav.nChannels == 1 && _Src->uData.sWav.nBPS == 8)
			return 0x1100;
		else if (_Src->uData.sWav.nChannels == 1 && _Src->uData.sWav.nBPS == 16)
			return 0x1101;
		else if (_Src->uData.sWav.nChannels == 2 && _Src->uData.sWav.nBPS == 8)
			return 0x1102;
		else
			return 0x1103;
	}
}
static BLBool
_MakeStream(_BLAudioSource* _Src, BLU32 _Bufidx)
{
    BLU32 _totalread = 0;
	BLU32 _format;
	BLU32 _samplerate;
	BLBool _ret = TRUE;
    if (_Src->bCompressed)
    {
		_format = _Format(_Src);
        _samplerate = _Src->uData.sMp3.pContext->sample_rate;
        while (_totalread < 65536)
        {
			BLU32 _actualread = 0;
			if (_Src->uData.sMp3.nSize <= _Src->uData.sMp3.nRead)
			{
				_Src->uData.sMp3.nSize = 0;
				_Src->uData.sMp3.nRead = 0;
				memset(_Src->uData.sMp3.aPacket, 0, sizeof(_Src->uData.sMp3.aPacket));
				BLU8 _inputbuf[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
				BLU32 _oldpos = blStreamTell(_Src->uData.sMp3.nStreamMp3);
				BLU32 _inputsz = blStreamRead(_Src->uData.sMp3.nStreamMp3, _Src->uData.sMp3.pContext->frame_size > MPAUDEC_MAX_AUDIO_FRAME_SIZE ? MPAUDEC_MAX_AUDIO_FRAME_SIZE : _Src->uData.sMp3.pContext->frame_size, _inputbuf);
				if (_inputsz)
					blStreamSeek(_Src->uData.sMp3.nStreamMp3, _oldpos + mpaudec_decode_frame(_Src->uData.sMp3.pContext, _Src->uData.sMp3.aPacket, &_Src->uData.sMp3.nSize, _inputbuf, _inputsz));
			}
			if (_Src->uData.sMp3.nSize > _Src->uData.sMp3.nRead)
			{
				BLU32 _amountleft = _Src->uData.sMp3.nSize - _Src->uData.sMp3.nRead;
				if (_amountleft < 65536 - _totalread)
				{
					if (_Bufidx == 0)
						memcpy(_Src->pSoundBufA + _totalread, _Src->uData.sMp3.aPacket + _Src->uData.sMp3.nRead, _amountleft);
					else if (_Bufidx == 1)
						memcpy(_Src->pSoundBufB + _totalread, _Src->uData.sMp3.aPacket + _Src->uData.sMp3.nRead, _amountleft);
					else
						memcpy(_Src->pSoundBufC + _totalread, _Src->uData.sMp3.aPacket + _Src->uData.sMp3.nRead, _amountleft);
					_Src->uData.sMp3.nRead += _amountleft;
					_actualread = _amountleft;
				}
				else
				{
					if (_Bufidx == 0)
						memcpy(_Src->pSoundBufA + _totalread, _Src->uData.sMp3.aPacket + _Src->uData.sMp3.nRead, 65536 - _totalread);
					else if (_Bufidx == 1)
						memcpy(_Src->pSoundBufB + _totalread, _Src->uData.sMp3.aPacket + _Src->uData.sMp3.nRead, 65536 - _totalread);
					else
						memcpy(_Src->pSoundBufC + _totalread, _Src->uData.sMp3.aPacket + _Src->uData.sMp3.nRead, 65536 - _totalread);
					_Src->uData.sMp3.nRead += 65536 - _totalread;
					_actualread = 65536 - _totalread;
				}
			}
			if (_actualread > 0)
				_totalread += _actualread;
            else
            {
                if (_Src->bLoop)
                    blStreamSeek(_Src->uData.sMp3.nStreamMp3, _Src->uData.sMp3.nOffset);
				else
                    _ret = FALSE;
				break;
            }
        }
    }
    else
    {
		_format = _Format(_Src);
        _samplerate = _Src->uData.sWav.nFrequency;
        while (_totalread < 65536)
        {
			BLU32 _actualread;
			if (_Bufidx == 0)
				_actualread = blStreamRead(_Src->uData.sWav.nStreamWav, 65536 - _totalread, _Src->pSoundBufA + _totalread);
			else if (_Bufidx == 1)
				_actualread = blStreamRead(_Src->uData.sWav.nStreamWav, 65536 - _totalread, _Src->pSoundBufB + _totalread);
			else
				_actualread = blStreamRead(_Src->uData.sWav.nStreamWav, 65536 - _totalread, _Src->pSoundBufC + _totalread);
            if (_actualread > 0)
                _totalread += _actualread;
            else
            {
				if (_Src->bLoop)
					blStreamSeek(_Src->uData.sWav.nStreamWav, _Src->uData.sWav.nOffset);
                else
                    _ret = FALSE;
				break;
            }
        }
    }
#if defined(BL_USE_AL_API)
	if (_Bufidx == 0)
		alBufferData(_Src->aBuffers[_Bufidx], _format, _Src->pSoundBufA, _totalread, _samplerate);
	else if (_Bufidx == 1)
		alBufferData(_Src->aBuffers[_Bufidx], _format, _Src->pSoundBufB, _totalread, _samplerate);
	else
		alBufferData(_Src->aBuffers[_Bufidx], _format, _Src->pSoundBufC, _totalread, _samplerate);
#elif defined(BL_USE_SL_API)
#elif defined(BL_USE_COREAUDIO_API)
	XAUDIO2_BUFFER _buf = { 0 };
	_buf.AudioBytes = _totalread;
	if (_Bufidx == 0)
		_buf.pAudioData = _Src->pSoundBufA;
	else if (_Bufidx == 1)
		_buf.pAudioData = _Src->pSoundBufB;
	else
		_buf.pAudioData = _Src->pSoundBufC;
	if (!_ret)
		_buf.Flags = XAUDIO2_END_OF_STREAM;
	_Src->pSource->SubmitSourceBuffer(&_buf);
#endif
    return _ret;
}
static BLBool
_LoadAudio(BLVoid* _Src, const BLAnsi* _Filename, const BLAnsi* _Archive)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	_src->bCompressed = blUtf8Equal(blGetExtNameUtf8((BLUtf8*)_Filename), (const BLUtf8*)"mp3");
	if (_Archive)
	{
		if (_src->bCompressed)
		{
			_src->uData.sMp3.nStreamMp3 = blGenStream(_Filename, _Archive);
			if (_src->uData.sMp3.nStreamMp3 == INVALID_GUID)
				return FALSE;
		}
		else
		{
			_src->uData.sWav.nStreamWav = blGenStream(_Filename, _Archive);
			if (_src->uData.sWav.nStreamWav == INVALID_GUID)
				return FALSE;
		}
	}
	else
	{
		BLAnsi _tmpname[260];
		strcpy(_tmpname, _Filename);
		for (BLU32 _i = 0; _i < strlen(_tmpname); ++_i)
		{
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			if (_tmpname[_i] == '/')
				_tmpname[_i] = '\\';
#else
			if (_tmpname[_i] == '\\')
				_tmpname[_i] = '/';
#endif
		}
		BLAnsi _path[260] = { 0 };
		strcpy(_path, blWorkingDir(TRUE));
		strcat(_path, _tmpname);
		BLBool _finded = FALSE;
		if (_src->bCompressed)
		{
			_src->uData.sMp3.nStreamMp3 = blGenStream(_path, NULL);
			_finded = (_src->uData.sMp3.nStreamMp3 != INVALID_GUID) ? TRUE : FALSE;
		}
		else
		{
			_src->uData.sWav.nStreamWav = blGenStream(_path, NULL);
			_finded = (_src->uData.sWav.nStreamWav != INVALID_GUID) ? TRUE : FALSE;
		}
		if (!_finded)
		{
			memset(_path, 0, sizeof(_path));
			strcpy(_path, blUserFolderDir());
			strcat(_path, _tmpname);
			if (_src->bCompressed)
			{
				_src->uData.sMp3.nStreamMp3 = blGenStream(_path, NULL);
				_finded = (_src->uData.sMp3.nStreamMp3 != INVALID_GUID) ? TRUE : FALSE;
			}
			else
			{
				_src->uData.sWav.nStreamWav = blGenStream(_path, NULL);
				_finded = (_src->uData.sWav.nStreamWav != INVALID_GUID) ? TRUE : FALSE;
			}
		}
		if (!_finded)
			return FALSE;
	}
	if (!_src->bCompressed)
	{
		BLAnsi _ident[4];
		BLS32 _tempint32 = 0;
		BLS16 _tempint16 = 0;
		BLU32 _startoffset = 0;
		blStreamRead(_src->uData.sWav.nStreamWav, 4, _ident);
		if (strncmp(_ident, "RIFF", 4) == 0)
		{
			blStreamRead(_src->uData.sWav.nStreamWav, 4, &_tempint32);
			if (_tempint32 >= 44)
			{
				blStreamRead(_src->uData.sWav.nStreamWav, 4, _ident);
				if (strncmp(_ident, "WAVE", 4) == 0)
				{
					_startoffset = blStreamTell(_src->uData.sWav.nStreamWav);
					do {
						blStreamRead(_src->uData.sWav.nStreamWav, 4, _ident);
					} while ((strncmp(_ident, "fmt ", 4) != 0) && !blStreamEos(_src->uData.sWav.nStreamWav));
					if (blStreamTell(_src->uData.sWav.nStreamWav) < (blStreamLength(_src->uData.sWav.nStreamWav) - 16))
					{
						blStreamRead(_src->uData.sWav.nStreamWav, 4, &_tempint32);
						if (_tempint32 >= 16)
						{
							blStreamRead(_src->uData.sWav.nStreamWav, 2, &_tempint16);
							if (_tempint16 == 1)
							{
								blStreamRead(_src->uData.sWav.nStreamWav, 2, &_tempint16);
								_src->uData.sWav.nChannels = _tempint16;
								if (_src->uData.sWav.nChannels == 1 || _src->uData.sWav.nChannels == 2)
								{
									blStreamRead(_src->uData.sWav.nStreamWav, 4, &_tempint32);
									_src->uData.sWav.nFrequency = _tempint32;
									blStreamRead(_src->uData.sWav.nStreamWav, 4, &_tempint32);
									blStreamRead(_src->uData.sWav.nStreamWav, 2, &_tempint16);
									blStreamRead(_src->uData.sWav.nStreamWav, 2, &_tempint16);
									_src->uData.sWav.nBPS = _tempint16;
									if (_src->uData.sWav.nBPS == 8 || _src->uData.sWav.nBPS == 16)
									{
										blStreamSeek(_src->uData.sWav.nStreamWav, _startoffset);
										do {
											blStreamRead(_src->uData.sWav.nStreamWav, 4, _ident);
										} while ((strncmp(_ident, "data", 4) != 0) && (!blStreamEos(_src->uData.sWav.nStreamWav)));
										if (!blStreamEos(_src->uData.sWav.nStreamWav))
										{
											blStreamRead(_src->uData.sWav.nStreamWav, 4, &_tempint32);
											_src->uData.sWav.nOffset = blStreamTell(_src->uData.sWav.nStreamWav);
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
	else
	{
		_src->uData.sMp3.pContext = (MPAuDecContext*)malloc(sizeof(struct MPAuDecContext));
		if (mpaudec_init(_src->uData.sMp3.pContext) < 0)
		{
			free(_src->uData.sMp3.pContext);
			_src->uData.sMp3.pContext = NULL;
			assert(0);
			return FALSE;
		}
		BLS8 _idv3header[10];
		BLS32 _amountread = blStreamRead(_src->uData.sMp3.nStreamMp3, 10, &_idv3header);
		if (_amountread == 10 && _idv3header[0] == 'I' && _idv3header[1] == 'D' && _idv3header[2] == '3')
		{
			BLS32 _size = 0;
			_size = (_idv3header[6] & 0x7f) << (3 * 7);
			_size |= (_idv3header[7] & 0x7f) << (2 * 7);
			_size |= (_idv3header[8] & 0x7f) << (1 * 7);
			_size |= (_idv3header[9] & 0x7f);
			_size += 10;
			_src->uData.sMp3.nOffset = _size;
			blStreamSeek(_src->uData.sMp3.nStreamMp3, _src->uData.sMp3.nOffset);
		}
		else
		{
			assert(0);
			return FALSE;
		}
		_src->uData.sMp3.pContext->parse_only = 1;
		BLU8 _tmpbuf[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
		BLS32 _outputsz = 0;
		BLU8 _inputbuf[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
		BLS32 _inputsz = blStreamRead(_src->uData.sMp3.nStreamMp3, 4096, _inputbuf);
		BLS32 _rv = mpaudec_decode_frame(_src->uData.sMp3.pContext, _tmpbuf, &_outputsz, _inputbuf, _inputsz);
		assert(_rv >= 0);
		_src->uData.sMp3.nChannels = _src->uData.sMp3.pContext->channels;
		_src->uData.sMp3.nFrequency = _src->uData.sMp3.pContext->sample_rate;
		_src->uData.sMp3.nBPS = 16;
		_src->uData.sMp3.pContext->parse_only = 0;
		blStreamSeek(_src->uData.sMp3.nStreamMp3, _src->uData.sMp3.nOffset);
		_src->uData.sMp3.nRead = _src->uData.sMp3.nSize = 0;
	}
	return TRUE;
}
static BLBool
_UnloadAudio(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
	BLBool _com = _src->bCompressed;
	if (_com)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	if (_src->bCompressed)
	{
		mpaudec_clear(_src->uData.sMp3.pContext);
		free(_src->uData.sMp3.pContext);
		blDeleteStream(_src->uData.sMp3.nStreamMp3);
		_PrAudioMem->pBackMusic = NULL;
	}
	else
		blDeleteStream(_src->uData.sWav.nStreamWav);
	if (_com)
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	else
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
	return TRUE;
}
#if defined(BL_USE_AL_API)
static BLBool
_ALSoundSetup(BLVoid* _Src)
{
	_BLAudioSource* _src = (_BLAudioSource*)_Src;
    ALsizei _queuesz = 0;
	_src->pSoundBufA = (BLU8*)malloc(65536);
	_src->pSoundBufB = (BLU8*)malloc(65536);
	_src->pSoundBufC = (BLU8*)malloc(65536);
    memset(_src->pSoundBufA, 0, 65536 * sizeof(BLU8));
    memset(_src->pSoundBufB, 0, 65536 * sizeof(BLU8));
    memset(_src->pSoundBufC, 0, 65536 * sizeof(BLU8));
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
	if (_src->bCompressed)
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
        if (_MakeStream(_src, _i))
            ++_queuesz;
    }
    alSourceQueueBuffers(_src->nSource, _queuesz, _src->aBuffers);
	alSourcePlay(_src->nSource);
	if (!_src->bCompressed)
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
	BLBool _com = _src->bCompressed;
	if (_com)
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
	if (_com)
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
            ALuint _buffer;
            alSourceUnqueueBuffers(_Src->nSource, 1, &_buffer);
            if (_Src->aBuffers[0] == _buffer)
            {
                if (_MakeStream(_Src, 0))
                    alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
            else if (_Src->aBuffers[1] == _buffer)
            {
                if (_MakeStream(_Src, 1))
                    alSourceQueueBuffers(_Src->nSource, 1, &_buffer);
            }
            else if (_Src->aBuffers[2] == _buffer)
            {
                if (_MakeStream(_Src, 2))
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
	_src->pSoundBufA = (BLU8*)malloc(65536);
	_src->pSoundBufB = (BLU8*)malloc(65536);
	_src->pSoundBufC = (BLU8*)malloc(65536);
    memset(_src->pSoundBufA, 0, 65536 * sizeof(BLU8));
    memset(_src->pSoundBufB, 0, 65536 * sizeof(BLU8));
    memset(_src->pSoundBufC, 0, 65536 * sizeof(BLU8));
	_src->nBufTurn = 0;
    SLDataLocator_BufferQueue _bufq = {SL_DATALOCATOR_BUFFERQUEUE, 3};
    SLDataFormat_PCM _pcm; 
    _pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    _pcm.formatType = SL_DATAFORMAT_PCM;
    if (_src->bCompressed)
	{
		if (_src->uData.pMp3->nChannels == 1)
		{
			_pcm.numChannels = 1;
			_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
        }
		else
		{
			_pcm.numChannels = 2;
            _pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;

        }
		if (_src->uData.pMp3->nBPS == 8)
        {
            _pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_8;
            _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_8;
        }
		else
		{
            _pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
            _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
		}
		_pcm.samplesPerSec = _src->uData.pMp3->nFrequency * 1000;
	}
	else
	{
		if (_src->uData.pWav->nChannels == 1)
		{
			_pcm.numChannels = 1;
			_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
        }
		else
		{
			_pcm.numChannels = 2;
            _pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;

        }
		if (_src->uData.pWav->nBPS == 8)
        {
            _pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_8;
            _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_8;
        }
		else
		{
            _pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
            _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
		}
		_pcm.samplesPerSec = _src->uData.pWav->nFrequency * 1000;
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
	if (_src->bCompressed)
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
	if (!_src->bCompressed)
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
	BLBool _com = _src->bCompressed;
	if (_com)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	_src->bValid = FALSE;
	if (_src->pSource)
		(*_src->pSource)->Destroy(_src->pSource);
	free(_src->pSoundBufA);
	free(_src->pSoundBufB);
	free(_src->pSoundBufC);
	if (_com)
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
            assert(_proce < 3);
            _MakeStream(_Src, (2 - _proce + _processed.playIndex) % 3);
            switch((2 - _proce + _processed.playIndex) % 3)
            {
            case 0:
                (*_Src->pBufferFunc)->Enqueue(_Src->pBufferFunc, _Src->pSoundBufA, 65536);
                break;
            case 1:
                (*_Src->pBufferFunc)->Enqueue(_Src->pBufferFunc, _Src->pSoundBufB, 65536);
                break;
            default:
                (*_Src->pBufferFunc)->Enqueue(_Src->pBufferFunc, _Src->pSoundBufC, 65536);
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
	_src->pSoundBufA = (BLU8*)malloc(65536 * sizeof(BLU8));
	_src->pSoundBufB = (BLU8*)malloc(65536 * sizeof(BLU8));
	_src->pSoundBufC = (BLU8*)malloc(65536 * sizeof(BLU8));
	memset(_src->pSoundBufA, 0, 65536 * sizeof(BLU8));
	memset(_src->pSoundBufB, 0, 65536 * sizeof(BLU8));
	memset(_src->pSoundBufC, 0, 65536 * sizeof(BLU8));
	WAVEFORMATEX _pwfx;
	if (_src->bCompressed)
	{
		_pwfx.wFormatTag = WAVE_FORMAT_PCM;
		_pwfx.nChannels = _src->uData.sMp3.nChannels;
		_pwfx.wBitsPerSample = _src->uData.sMp3.nBPS;
		_pwfx.nSamplesPerSec = _src->uData.sMp3.nFrequency;
		_pwfx.nBlockAlign = _pwfx.nChannels*_pwfx.wBitsPerSample / 8;
		_pwfx.nAvgBytesPerSec = _pwfx.nBlockAlign * _pwfx.nSamplesPerSec;
		_pwfx.cbSize = 0;
		_PrAudioMem->pAudioDev.pCADevice->CreateSourceVoice(&_src->pSource, &_pwfx);
		_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fMusicVolume);
	}
	else
	{
		_pwfx.wFormatTag = WAVE_FORMAT_PCM;
		_pwfx.nChannels = _src->uData.sWav.nChannels;
		_pwfx.wBitsPerSample = _src->uData.sWav.nBPS;
		_pwfx.nSamplesPerSec = _src->uData.sWav.nFrequency;
		_pwfx.nBlockAlign = _pwfx.nChannels*_pwfx.wBitsPerSample / 8;
		_pwfx.nAvgBytesPerSec = _pwfx.nBlockAlign * _pwfx.nSamplesPerSec;
		_pwfx.cbSize = 0;
		_PrAudioMem->pAudioDev.pCADevice->CreateSourceVoice(&_src->pSource, &_pwfx);
		if (!_src->b3d)
			_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fSystemVolume);
		else
			_src->pSource->SetVolume(_PrAudioMem->pAudioDev.fEnvVolume);
	}
	_src->nBufTurn = 0;
	_src->pSource->Start(0);
	if (!_src->bCompressed)
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
	BLBool _com = _src->bCompressed;
	if (_com)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	_src->bValid = FALSE;
	_src->pSource->Stop(0);
	_src->pSource->DestroyVoice();
	free(_src->pSoundBufA);
	free(_src->pSoundBufB);
	free(_src->pSoundBufC);
	if (_com)
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
			_ret = _MakeStream(_Src, _Src->nBufTurn);
			_Src->nBufTurn++;
			_Src->nBufTurn %= 3;
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
		if (_GbSystemRunning == 1 && !_GbVideoPlaying)
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
				if (!_CAUpdate(_iter))
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
    if(!alcMakeContextCurrent(_PrAudioMem->pAudioDev.pALContext))
    {
        assert(0);
        alcDestroyContext(_PrAudioMem->pAudioDev.pALContext);
        alcCloseDevice(_PrAudioMem->pAudioDev.pALDevice);
		blDebugOutput("Audio initialize failed");
		return;
    }
    _PrAudioMem->pSounds = blGenList(TRUE);
    _PrAudioMem->pMusicMutex = blGenMutex();
    _PrAudioMem->pListenerMutex = blGenMutex();
    _PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
	blThreadRun(_PrAudioMem->pAudioDev.pThread);
	blDebugOutput("Audio initialize successfully");
}
static BLVoid
_ALDestroy()
{
    blDeleteThread(_PrAudioMem->pAudioDev.pThread);
    blDeleteMutex(_PrAudioMem->pMusicMutex);
    blDeleteMutex(_PrAudioMem->pListenerMutex);
    blDeleteList(_PrAudioMem->pSounds);
	mpaudec_cleanup();
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
	_PrAudioMem->pListenerMutex = blGenMutex();
	_PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
	blThreadRun(_PrAudioMem->pAudioDev.pThread);
	blDebugOutput("Audio initialize successfully");
}
static BLVoid
_SLDestroy()
{
	blDeleteThread(_PrAudioMem->pAudioDev.pThread);
	blDeleteMutex(_PrAudioMem->pMusicMutex);
	blDeleteMutex(_PrAudioMem->pListenerMutex);
	blDeleteList(_PrAudioMem->pSounds);
    (*_PrAudioMem->pAudioDev.pMixObj)->Destroy(_PrAudioMem->pAudioDev.pMixObj);
    (*_PrAudioMem->pAudioDev.pDeviceObj)->Destroy(_PrAudioMem->pAudioDev.pDeviceObj);
	_PrAudioMem->pAudioDev.pMixObj = NULL;
	_PrAudioMem->pAudioDev.pDevice = NULL;
	_PrAudioMem->pAudioDev.pDeviceObj = NULL;
	mpaudec_cleanup();
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
	_PrAudioMem->pListenerMutex = blGenMutex();
	_PrAudioMem->pAudioDev.pThread = blGenThread(_AudioThreadFunc, NULL, NULL);
	blThreadRun(_PrAudioMem->pAudioDev.pThread);
	blDebugOutput("Audio initialize successfully");
}
static BLVoid
_CADestroy()
{
	blDeleteThread(_PrAudioMem->pAudioDev.pThread);
	blDeleteMutex(_PrAudioMem->pMusicMutex);
	blDeleteMutex(_PrAudioMem->pListenerMutex);
	blDeleteList(_PrAudioMem->pSounds);
	mpaudec_cleanup();
	_PrAudioMem->pAudioDev.pCADevice->StopEngine();
	_PrAudioMem->pAudioDev.pCAContext->DestroyVoice();
	_PrAudioMem->pAudioDev.pCADevice->Release();
	CoUninitialize();
	_PrAudioMem->pAudioDev.pCAContext = NULL;
	_PrAudioMem->pAudioDev.pCADevice = NULL;
}
#endif
BLVoid
_AudioInit()
{
	_PrAudioMem = (_BLAudioMember*)malloc(sizeof(_BLAudioMember));
	_PrAudioMem->pBackMusic = NULL;
	_PrAudioMem->pSounds = NULL;
	_PrAudioMem->pMusicMutex = NULL;
	_PrAudioMem->pListenerMutex = NULL;
#if defined(BL_USE_AL_API)
    _ALInit();
#elif defined(BL_USE_SL_API)
    _SLInit();
#elif defined(BL_USE_COREAUDIO_API)
    _CAInit();
#endif
	mpaudec_open();
}
BLVoid
_AudioStep(BLU32 _Delta)
{
	blMutexLock(_PrAudioMem->pSounds->pMutex);
	FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
	{
		if (_iter->bLoop && !_iter->bCompressed)
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
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
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
    _ALDestroy();
#elif defined(BL_USE_SL_API)
    _SLDestroy();
#elif defined(BL_USE_COREAUDIO_API)
    _CADestroy();
#endif
	blDebugOutput("Audio shutdown");
	free(_PrAudioMem);
}
BLVoid
blUseMusicVolume(IN BLF32 _Vol)
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
blUseEnvironmentVolume(IN BLF32 _Vol)
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
blUseSystemVolume(IN BLF32 _Vol)
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
blQueryAudioVolume(OUT BLF32* _Music, OUT BLF32* _System, OUT BLF32* _Env)
{
	*_Music = _PrAudioMem->pAudioDev.fMusicVolume;
	*_System = _PrAudioMem->pAudioDev.fSystemVolume;
	*_Env = _PrAudioMem->pAudioDev.fEnvVolume;
}
BLVoid
blSetListenerPos(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_PrAudioMem->sListenerPos.fX = _Xpos;
	_PrAudioMem->sListenerPos.fY = _Ypos;
	_PrAudioMem->sListenerPos.fZ = _Zpos;
}
BLVoid
blSetListenerUp(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_PrAudioMem->sListenerUp.fX = _Xpos;
	_PrAudioMem->sListenerUp.fY = _Ypos;
	_PrAudioMem->sListenerUp.fZ = _Zpos;
}
BLVoid
blSetListenerDir(IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	_PrAudioMem->sListenerDir.fX = _Xpos;
	_PrAudioMem->sListenerDir.fY = _Ypos;
	_PrAudioMem->sListenerDir.fZ = _Zpos;
}
BLVoid
blSetEmitterPos(IN BLGuid _ID, IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	FOREACH_LIST(_BLAudioSource*, _iter, _PrAudioMem->pSounds)
	{
		if (_iter->nID == _ID)
		{
			_iter->sPos.fX = _Xpos;
			_iter->sPos.fY = _Ypos;
			_iter->sPos.fZ = _Zpos;
			return;
		}
	}
}
BLGuid
blGenAudio(IN BLAnsi* _Filename, IN BLAnsi* _Archive, IN BLBool _Loop, IN BLBool _3D, IN BLF32 _Xpos, IN BLF32 _Ypos, IN BLF32 _Zpos)
{
	BLBool _com = blUtf8Equal(blGetExtNameUtf8((BLUtf8*)_Filename) , (const BLUtf8*)"mp3");
	if (_com)
		blMutexLock(_PrAudioMem->pMusicMutex);
	else
		blMutexLock(_PrAudioMem->pSounds->pMutex);
	_BLAudioSource* _sound = (_BLAudioSource*)malloc(sizeof(_BLAudioSource));
	_sound->bCompressed = _com;
	_sound->bLoop = _Loop;
	_sound->b3d = _3D;
	_sound->sPos.fX = _Xpos;
	_sound->sPos.fY = _Ypos;
	_sound->sPos.fZ = _Zpos;
	_sound->nID = blGenGuid(_sound, blHashUtf8((const BLUtf8*)_Filename));
	_sound->bValid = FALSE;
	if (_com)
		blMutexUnlock(_PrAudioMem->pMusicMutex);
	else
		blMutexUnlock(_PrAudioMem->pSounds->pMutex);
#if defined(BL_USE_AL_API)
	_FetchResource(_Filename, _Archive, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _ALSoundSetup, TRUE);
#elif defined(BL_USE_SL_API)
	_FetchResource(_Filename, _Archive, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _SLSoundSetup, TRUE);
#elif defined(BL_USE_COREAUDIO_API)
	_FetchResource(_Filename, _Archive, (BLVoid**)&_sound, _sound->nID, _LoadAudio, _CASoundSetup, TRUE);
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
				blListErase(_PrAudioMem->pSounds, _iterator_iter);;
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
