#include "mpgconfig.h"

#include <inttypes.h>
#include <string.h>
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "a52.h"
#include "audio_out.h"
#include "audio_out_internal.h"
    
#ifdef __cplusplus
}
#endif

#include "../../headers/utils.h"
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	if(_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#		include <xaudio2.h>
#		include <x3daudio.h>
#	else
#		include <xaudio2.h>
#		include <x3daudio.h>
#	endif
#elif defined(ANDROID)
#include <SLES/OpenSLES.h>
#elif defined(__APPLE_CC__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

typedef struct null_instance_s {
    ao_instance_t ao;
    int channels;
    int sample_rate;
    int set_params;
    unsigned char* pSoundBufC;
    unsigned char* pSoundBufD;
    unsigned char* pSoundBufE;
    int nFill;
    int flags;
    int nBufTurn;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
    IXAudio2SourceVoice* pSource;
#elif defined(ANDROID)
    SLObjectItf pSource;
    SLBufferQueueItf pBufferFunc;
#else
    ALuint aBuffers[3];
    ALuint nSource;
    int nFormat;
#endif
} null_instance_t;


/*
 XAudio
 _Pr1 : IXAudio2*
 OpenSLES
 _Pr1 : SLEngineItf
 _Pr2 : SLOutputMixItf
 */

void _GetDeviceDetial(void** _Pr1, void** _Pr2);

static int null_setup (ao_instance_t * _instance, int sample_rate, int * flags,
                       level_t * level, sample_t * bias)
{
    null_instance_t * instance = (null_instance_t *) _instance;
    
    instance->sample_rate = sample_rate;
    *flags = instance->flags;
    *level = CONVERT_LEVEL;
    *bias = CONVERT_BIAS;
    
    return 0;
}

static int wav_channels(int flags, uint32_t * speaker_flags)
{
    static const uint16_t speaker_tbl[] = {
        3, 4, 3, 7, 0x103, 0x107, 0x33, 0x37, 4, 4, 3
    };
    static const uint8_t nfchans_tbl[] = {
        2, 1, 2, 3, 3, 4, 4, 5, 1, 1, 2
    };
    int chans;
    
    *speaker_flags = speaker_tbl[flags & A52_CHANNEL_MASK];
    chans = nfchans_tbl[flags & A52_CHANNEL_MASK];
    
    if (flags & A52_LFE) {
        *speaker_flags |= 8;	/* WAVE_SPEAKER_LOW_FREQUENCY */
        chans++;
    }
    
    return chans;
}
static inline int16_t _convert(int32_t i)
{
#ifdef LIBA52_FIXED
    i >>= 15;
#else
    i -= 0x43c00000;
#endif
    return (i > 32767) ? 32767 : ((i < -32768) ? -32768 : i);
}

static void _convert2s16_2(convert_t * _f, int16_t * s16)
{
    int i;
    int32_t * f = (int32_t *)_f;
    
    for (i = 0; i < 256; i++) {
        s16[2 * i] = _convert(f[i]);
        s16[2 * i + 1] = _convert(f[i + 256]);
    }
}

static int null_play (ao_instance_t * instance, int flags, sample_t * _samples)
{
    null_instance_t * inst = (null_instance_t *)instance;
    int chans;
    uint32_t speaker_flags;
    convert_t * samples = _samples;
    chans = wav_channels(flags, &speaker_flags);
    int16_t* int16_samples = (int16_t*)alloca(256 * sizeof(int16_t) * chans);
    _convert2s16_2(samples, int16_samples);
    int _totalread = 256 * sizeof(int16_t) * chans;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(ANDROID)
    int switchbuf = 0;
    if (inst->nFill >= 65536)
    {
        switchbuf = 1;
        if (switchbuf)
        {
            inst->nFill = 0;
            inst->nBufTurn++;
        }
        if (inst->nBufTurn % 3 == 0)
            memcpy(inst->pSoundBufC , int16_samples, _totalread);
        else if (inst->nBufTurn % 3 == 1)
            memcpy(inst->pSoundBufD, int16_samples, _totalread);
        else
            memcpy(inst->pSoundBufE , int16_samples, _totalread);
        inst->nFill += _totalread;
    }
    else
    {
        if (inst->nBufTurn % 3 == 0)
            memcpy(inst->pSoundBufC + inst->nFill, int16_samples, _totalread);
        else if (inst->nBufTurn % 3 == 1)
            memcpy(inst->pSoundBufD + inst->nFill, int16_samples, _totalread);
        else
            memcpy(inst->pSoundBufE + inst->nFill, int16_samples, _totalread);
        inst->nFill += _totalread;
    }
    void* _Pr1, *_Pr2;
    _GetDeviceDetial(&_Pr1, &_Pr2);
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
    HRESULT rsz;
    if (inst->set_params)
    {
        WAVEFORMATEX _pwfx;
        _pwfx.wFormatTag = WAVE_FORMAT_PCM;
        _pwfx.nChannels = chans;
        _pwfx.wBitsPerSample = 16;
        _pwfx.nSamplesPerSec = inst->sample_rate;
        _pwfx.nBlockAlign = _pwfx.nChannels*_pwfx.wBitsPerSample / 8;
        _pwfx.nAvgBytesPerSec = _pwfx.nBlockAlign * _pwfx.nSamplesPerSec;
        _pwfx.cbSize = 0;
        IXAudio2* pDev = (IXAudio2*)_Pr1;
        rsz = pDev->CreateSourceVoice(&inst->pSource, &_pwfx);
        assert(SUCCEEDED(rsz));
        inst->pSource->Start(0);
        inst->flags = flags;
        inst->set_params = 0;
    }
    if (inst->nFill >= 65536 )
    {
        XAUDIO2_VOICE_STATE _state;
        for (;; )
        {
            inst->pSource->GetState(&_state);
            if (_state.BuffersQueued < 2)
            {
                XAUDIO2_BUFFER _buf = { 0 };
                _buf.AudioBytes = inst->nFill;
                if (inst->nBufTurn % 3 == 0)
                    _buf.pAudioData = inst->pSoundBufC;
                else if (inst->nBufTurn % 3 == 1)
                    _buf.pAudioData = inst->pSoundBufD;
                else
                    _buf.pAudioData = inst->pSoundBufE;
                rsz = inst->pSource->SubmitSourceBuffer(&_buf);
                assert(SUCCEEDED(rsz));
                break;
            }
        }
    }
#elif defined(ANDROID)
    if (inst->set_params)
    {
        SLDataLocator_BufferQueue _bufq = { SL_DATALOCATOR_BUFFERQUEUE, 3 };
        SLDataFormat_PCM _pcm;
        _pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
        _pcm.formatType = SL_DATAFORMAT_PCM;
        if (chans == 1)
        {
            _pcm.numChannels = 1;
            _pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
        }
        else
        {
            _pcm.numChannels = 2;
            _pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        }
        _pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        _pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        _pcm.samplesPerSec = inst->sample_rate * 1000;
        SLresult _result;
        SLDataSource _data = { &_bufq, &_pcm };
        SLEngineItf pDevice = (SLEngineItf)_Pr1;
        SLObjectItf pMixObj = (SLObjectItf)_Pr2;
        SLDataLocator_OutputMix _outmix = { SL_DATALOCATOR_OUTPUTMIX, pMixObj };
        SLDataSink _snk = { &_outmix, NULL };
        const SLInterfaceID _ids[3] = { SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME };
        const SLboolean _req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        _result = (*pDevice)->CreateAudioPlayer(pDevice, &inst->pSource, &_data, &_snk, 3, _ids, _req);
        assert(_result == SL_RESULT_SUCCESS);
        _result = (*inst->pSource)->Realize(inst->pSource, SL_BOOLEAN_FALSE);
        assert(_result == SL_RESULT_SUCCESS);
        SLPlayItf _playfunc;
        _result = (*inst->pSource)->GetInterface(inst->pSource, SL_IID_PLAY, &_playfunc);
        assert(_result == SL_RESULT_SUCCESS);
        _result = (*inst->pSource)->GetInterface(inst->pSource, SL_IID_BUFFERQUEUE, &inst->pBufferFunc);
        assert(_result == SL_RESULT_SUCCESS);
        _result = (*_playfunc)->SetPlayState(_playfunc, SL_PLAYSTATE_PLAYING);
        assert(_result == SL_RESULT_SUCCESS);
        inst->flags = flags;
        inst->set_params = 0;
    }
    if (inst->nFill >= 65536)
    {
        SLBufferQueueState _processed;
        (*inst->pBufferFunc)->GetState(inst->pBufferFunc, &_processed);
        for (;; )
        {
            int _Proc = 0;
            SLint32 _proce = 3 - _processed.count;
            while (_proce--)
            {
                _Proc = 1;
                assert(_proce < 3);
                switch ((2 - _proce + _processed.playIndex) % 3)
                {
                    case 0:
                        (*inst->pBufferFunc)->Enqueue(inst->pBufferFunc, inst->pSoundBufC, inst->nFill);
                        break;
                    case 1:
                        (*inst->pBufferFunc)->Enqueue(inst->pBufferFunc, inst->pSoundBufD, inst->nFill);
                        break;
                    default:
                        (*inst->pBufferFunc)->Enqueue(inst->pBufferFunc, inst->pSoundBufE, inst->nFill);
                        break;
                }
            }
            if(_Proc)
                break;
            else
                (*inst->pBufferFunc)->GetState(inst->pBufferFunc, &_processed);
        }
    }
#else
    {
        if (inst->set_params == 1)
        {
            alGetError();
            alGenSources(1, &inst->nSource);
            alSourcei(inst->nSource, AL_SOURCE_RELATIVE, 1);
            if (chans == 1)
                inst->nFormat = 0x1101;
            else
                inst->nFormat = 0x1103;
            inst->flags = flags;
            inst->set_params = 0;
        }
        ALuint bufferID = 0;
        alGenBuffers(1, &bufferID);
        alBufferData(bufferID, inst->nFormat, int16_samples, _totalread,inst->sample_rate);
        ALint stateVaue = 0;
        ALint _numprocessed, _numqueued;
        alGetSourcei(inst->nSource, AL_BUFFERS_PROCESSED, &_numprocessed);
        alGetSourcei(inst->nSource, AL_BUFFERS_QUEUED, &_numqueued);
        alGetSourcei(inst->nSource, AL_SOURCE_STATE, &stateVaue);
 
        alSourceQueueBuffers(inst->nSource, 1, &bufferID);
        if (stateVaue != AL_PLAYING)
        {
            alSourcei(inst->nSource, AL_BUFFER, bufferID);
            alSourcePlay(inst->nSource);
        }
        while(_numprocessed --)
        {
            ALuint buff;
            alSourceUnqueueBuffers(inst->nSource, 1, &buff);
            alDeleteBuffers(1, &buff);
        }
        if (_numqueued > 256)
        {
            usleep(1000 * _numqueued);
        }
        alGetSourcei(inst->nSource, AL_BUFFERS_QUEUED, &_numqueued);
    }
#endif
    return 0;
}

static void null_close (ao_instance_t * instance)
{
    null_instance_t * inst = (null_instance_t *)instance;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
    if (inst->pSource)
    {
        inst->pSource->Stop(0);
        inst->pSource->DestroyVoice();
    }
#elif defined(ANDROID)
    if (inst->pSource)
        (*inst->pSource)->Destroy(inst->pSource);
#else
    ALint _numprocessed, _numqueued;
    alGetSourcei(inst->nSource, AL_BUFFERS_PROCESSED, &_numprocessed);
    alGetSourcei(inst->nSource, AL_BUFFERS_QUEUED, &_numqueued);
    usleep(1000000);
    while(_numprocessed --)
    {
        ALuint buff;
        alSourceUnqueueBuffers(inst->nSource, 1, &buff);
        alDeleteBuffers(1, &buff);
    }
    alSourceStop(inst->nSource);
    alDeleteSources(1, &inst->nSource);
#endif
    free(inst->pSoundBufC);
    free(inst->pSoundBufD);
    free(inst->pSoundBufE);
}

static null_instance_t instance = {{null_setup, null_play, null_close}, 0};

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__APPLE_CC__)
extern "C" {
    ao_instance_t * ao_null_open (void)
    {
        instance.sample_rate = 0;
        instance.flags = A52_STEREO;
        instance.channels = 2;
        instance.set_params = 1;
        instance.pSoundBufC = (unsigned char*)malloc(65536);
        instance.pSoundBufD = (unsigned char*)malloc(65536);
        instance.pSoundBufE = (unsigned char*)malloc(65536);
        memset(instance.pSoundBufC, 0, 65536);
        memset(instance.pSoundBufD, 0, 65536);
        memset(instance.pSoundBufE, 0, 65536);
        instance.nFill = 0;
        instance.nBufTurn = 0;
        return (ao_instance_t *) &instance;
    }
    
    ao_instance_t * ao_null4_open (void)
    {
        instance.channels = A52_2F2R;
        
        return (ao_instance_t *) &instance;
    }
    
    ao_instance_t * ao_null6_open (void)
    {
        instance.channels = A52_3F2R | A52_LFE;
        
        return (ao_instance_t *) &instance;
    }
}
#else
ao_instance_t * ao_null_open (void)
{
    instance.sample_rate = 0;
    instance.flags = A52_STEREO;
    instance.channels = 2;
    instance.set_params = 1;
    instance.pSoundBufC = (unsigned char*)malloc(65536);
    instance.pSoundBufD = (unsigned char*)malloc(65536);
    instance.pSoundBufE = (unsigned char*)malloc(65536);
    memset(instance.pSoundBufC, 0, 65536);
    memset(instance.pSoundBufD, 0, 65536);
    memset(instance.pSoundBufE, 0, 65536);
    instance.nFill = 0;
    instance.nBufTurn = 0;
    return (ao_instance_t *) &instance;
}

ao_instance_t * ao_null4_open (void)
{
    instance.channels = A52_2F2R;
    
    return (ao_instance_t *) &instance;
}

ao_instance_t * ao_null6_open (void)
{
    instance.channels = A52_3F2R | A52_LFE;
    
    return (ao_instance_t *) &instance;
}
#endif