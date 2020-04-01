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
#if !defined(EMSCRIPTEN)
#	include "yuv2rgb.h"
#	include "vorbis/codec.h"
#	include "vpx/vpx_decoder.h"
#	include "vpx/vp8dx.h"
#	include "webm/mkvparser/mkvparser.h"
#	include "opus/include/opus.h"
#	include "audio.h"
#	include "streamio.h"
#	include "util.h"
#	include "system.h"
#	include "gpu.h"
#endif
#include "videoentry.h"
#if !defined(BL_PLATFORM_WEB)
class MkvReader : public mkvparser::IMkvReader
{
public:
	MkvReader(const BLAnsi* _FilePath)
	{
		_file = blStreamGen(_FilePath);
	}
	~MkvReader()
	{
		if (_file != INVALID_GUID)
			blStreamDelete(_file);
	}
	virtual int Read(long long _Pos, long _Len, unsigned char* _Buf)
	{
		if (_file == INVALID_GUID)
			return -1;
		blStreamSeek(_file, (BLU32)_Pos);
		BLU32 _size = blStreamRead(_file, (BLU32)_Len, _Buf);
		if (_size < BLU32(_Len))
			return -1;
		return 0;
	}
	virtual int Length(long long* _Total, long long* _Available)
	{
		if (_file == INVALID_GUID)
			return -1;
		if (_Total)
			*_Total = blStreamLength(_file);
		if (_Available)
			*_Available = blStreamLength(_file);
		return 0;
	}
private:
	BLGuid _file;
};
typedef struct _VorbisDecoderExt{
	vorbis_info sInfo;
	vorbis_dsp_state sDspState;
	vorbis_block sBlock;
	ogg_packet sPacket;
	BLBool bHasDSPState;
	BLBool bHasBlock;
}_BLVorbisDecoderExt;
typedef struct _OpusVorbisDecoderExt {
	_BLVorbisDecoderExt* pVorbis;
	OpusDecoder* pOpus;
	BLU32 nNumSamples;
	BLU32 nChannels;
}_BLOpusVorbisDecoderExt;
typedef struct _YUVImage {
	BLU32 nWidth;
	BLU32 nHeight;
	BLU32 nChromaShiftW;
	BLU32 nChromaShiftH;
	BLU8* aPlanes[3];
	BLS32 aLinesize[3];
}_BLYUVImageExt;
typedef struct _RGBFrameList {
	struct _ListNode {
		BLU8* pData;
		BLF64 fTime;
		struct _ListNode* pNext;
		struct _ListNode* pPrev;
	} *pFirst, *pLast;
	BLU32 nSize;
}_BLRGBFrameList;
typedef struct _VPXDecoderExt {
	vpx_codec_ctx* pCtx;
	BLVoid* pIter;
	BLS32 nDelay;
}_BLVPXDecoderExt;
typedef struct _WebMFrameExt {
	BLU32 nBufferSize;
	BLU32 nBufferCapacity;
	BLU8* pBuffer;
	BLF64 fTime;
	BLBool bKey;
}_BLWebMFrameExt;
typedef struct _WebMDemuxerExt {
	mkvparser::IMkvReader* pReader;
	mkvparser::Segment* pSegment;
	const mkvparser::Cluster* pCluster;
	const mkvparser::Block* pBlock;
	const mkvparser::BlockEntry* pBlockEntry;
	const mkvparser::VideoTrack* pVideoTrack;
	const mkvparser::AudioTrack* pAudioTrack;
	BLS32 nBlockFrameIndex;
	BLU8 nVideoCodec;
	BLU8 nAudioCodec;
	BLBool bOpen;
	BLBool bEos;
}_BlWebMDemuxerExt;
typedef struct _VideoMemberExt {
	_BLOpusVorbisDecoderExt* pAudioDecodec;
	_BLVPXDecoderExt* pVideoDecodec;
	_BLRGBFrameList* pImgDataCache;
#if defined(WIN32)
	CRITICAL_SECTION sHandle;
	HANDLE sThread;
#else
	pthread_mutex_t sHandle;
	pthread_t sThread;
#endif
	BLU32 nVideoWidth;
	BLU32 nVideoHeight;
	BLBool bAudioSetParams;
	BLBool bVideoSetParams;
	BLBool bVideoOver;
	BLGuid nTex;
	BLGuid nGeo;
	BLGuid nTech;
	BLF64 fTime;
    BLBool bClickInterrupt;
    BLEnum eKeyInterrupt;
}_BLVideoMemberExt;
static _BLVideoMemberExt* _PrVideoMem = NULL;
static _BLOpusVorbisDecoderExt*
_AudioCodecOpen(_BlWebMDemuxerExt* _Demuxer)
{
	_BLOpusVorbisDecoderExt* _ret = (_BLOpusVorbisDecoderExt*)malloc(sizeof(_BLOpusVorbisDecoderExt));
	_ret->pVorbis = NULL;
	_ret->pOpus = NULL;
	_ret->nNumSamples = 0;
	switch (_Demuxer->nAudioCodec)
	{
	case 1:
	{
		_ret->nChannels = (BLU32)_Demuxer->pAudioTrack->GetChannels();
		size_t _extradatasz = 0;
		const BLU8* _extradata = _Demuxer->pAudioTrack->GetCodecPrivate(_extradatasz);
		if (_extradatasz < 3 || !_extradata || _extradata[0] != 2)
			return NULL;
		BLU32 _headersz[3] = { 0 };
		BLU32 _offset = 1;
		for (BLU32 _idx = 0; _idx < 2; ++_idx)
		{
			for (;;)
			{
				if (_offset >= _extradatasz)
					return NULL;
				_headersz[_idx] += _extradata[_offset];
				if (_extradata[_offset++] < 0xFF)
					break;
			}
		}
		_headersz[2] = (BLU32)_extradatasz - (_headersz[0] + _headersz[1] + _offset);
		if (_headersz[0] + _headersz[1] + _headersz[2] + _offset != _extradatasz)
			return NULL;
		ogg_packet _op[3];
		memset(_op, 0, sizeof(_op));
		_op[0].packet = (BLU8*)_extradata + _offset;
		_op[0].bytes = _headersz[0];
		_op[0].b_o_s = 1;
		_op[1].packet = (BLU8*)_extradata + _offset + _headersz[0];
		_op[1].bytes = _headersz[1];
		_op[2].packet = (BLU8*)_extradata + _offset + _headersz[0] + _headersz[1];
		_op[2].bytes = _headersz[2];
		_ret->pVorbis = (_BLVorbisDecoderExt*)malloc(sizeof(_BLVorbisDecoderExt));
		_ret->pVorbis->bHasDSPState = _ret->pVorbis->bHasBlock = FALSE;
		vorbis_info_init(&_ret->pVorbis->sInfo);
		vorbis_comment _vc;
		vorbis_comment_init(&_vc);
		for (BLU32 _idx = 0; _idx < 3; ++_idx)
		{
			if (vorbis_synthesis_headerin(&_ret->pVorbis->sInfo, &_vc, &_op[_idx]))
			{
				vorbis_comment_clear(&_vc);
				return NULL;
			}
		}
		vorbis_comment_clear(&_vc);
		if (vorbis_synthesis_init(&_ret->pVorbis->sDspState, &_ret->pVorbis->sInfo))
			return NULL;
		_ret->pVorbis->bHasDSPState = TRUE;
		if (_ret->pVorbis->sInfo.channels != (BLS32)_ret->nChannels || (BLS32)_ret->pVorbis->sInfo.rate != (BLS32)_Demuxer->pAudioTrack->GetSamplingRate())
			return NULL;
		if (vorbis_block_init(&_ret->pVorbis->sDspState, &_ret->pVorbis->sBlock))
			return NULL;
		_ret->pVorbis->bHasBlock = TRUE;
		memset(&_ret->pVorbis->sPacket, 0, sizeof(_ret->pVorbis->sPacket));
		_ret->nNumSamples = 4096 / _ret->nChannels;
		return _ret;
	}
	case 2:
	{
		_ret->nChannels = (BLU32)_Demuxer->pAudioTrack->GetChannels();
		BLS32 _err = 0;
		_ret->pOpus = opus_decoder_create((opus_int32)_Demuxer->pAudioTrack->GetSamplingRate(), _ret->nChannels, &_err);
		if (!_err)
		{
			_ret->nNumSamples = (BLU32)(_Demuxer->pAudioTrack->GetSamplingRate() * 0.06 + 0.5);
			return _ret;
		}
		return NULL;
	}
	default:
		break;
	}
	if (_ret->pVorbis)
	{
		if (_ret->pVorbis->bHasBlock)
			vorbis_block_clear(&_ret->pVorbis->sBlock);
		if (_ret->pVorbis->bHasDSPState)
			vorbis_dsp_clear(&_ret->pVorbis->sDspState);
		vorbis_info_clear(&_ret->pVorbis->sInfo);
		free(_ret->pVorbis);
	}
	if (_ret->pOpus)
		opus_decoder_destroy(_ret->pOpus);
	free(_ret);
	return NULL;
}
static BLVoid
_AudioCodecClose(_BLOpusVorbisDecoderExt* _Codec)
{
	if (!_Codec)
		return;
	if (_Codec->pVorbis)
	{
		if (_Codec->pVorbis->bHasBlock)
			vorbis_block_clear(&_Codec->pVorbis->sBlock);
		if (_Codec->pVorbis->bHasDSPState)
			vorbis_dsp_clear(&_Codec->pVorbis->sDspState);
		vorbis_info_clear(&_Codec->pVorbis->sInfo);
		free(_Codec->pVorbis);
	}
	if (_Codec->pOpus)
		opus_decoder_destroy(_Codec->pOpus);
}
static BLBool
_PCMS16Data(_BLWebMFrameExt* _Frame, BLS16* _Buffer, BLS32* _NumOutSamples)
{
	if (_PrVideoMem->pAudioDecodec->pVorbis)
	{
		_PrVideoMem->pAudioDecodec->pVorbis->sPacket.packet = _Frame->pBuffer;
		_PrVideoMem->pAudioDecodec->pVorbis->sPacket.bytes = _Frame->nBufferSize;
		if (vorbis_synthesis(&_PrVideoMem->pAudioDecodec->pVorbis->sBlock, &_PrVideoMem->pAudioDecodec->pVorbis->sPacket))
			return FALSE;
		if (vorbis_synthesis_blockin(&_PrVideoMem->pAudioDecodec->pVorbis->sDspState, &_PrVideoMem->pAudioDecodec->pVorbis->sBlock))
			return FALSE;
		BLS32 _maxsamples = _PrVideoMem->pAudioDecodec->nNumSamples;
		BLS32 _samplescount = 0;
		BLS32 _count = 0;
		BLF32** _pcm;
		while ((_samplescount = vorbis_synthesis_pcmout(&_PrVideoMem->pAudioDecodec->pVorbis->sDspState, &_pcm)))
		{
			BLS32 _convert = _samplescount <= _maxsamples ? _samplescount : _maxsamples;
			for (BLU32 _c = 0; _c < _PrVideoMem->pAudioDecodec->nChannels; ++_c)
			{
				BLF32* _samples = _pcm[_c];
				for (BLU32 _idx = 0, _jdx = _c; _idx < (BLU32)_convert; ++_idx, _jdx += _PrVideoMem->pAudioDecodec->nChannels)
				{
					BLS32 _sample = (BLS32)(_samples[_idx] * 32767.0f);
					if (_sample > 32767)
						_sample = 32767;
					else if (_sample < -32768)
						_sample = -32768;
					_Buffer[_count + _jdx] = _sample;
				}
			}
			vorbis_synthesis_read(&_PrVideoMem->pAudioDecodec->pVorbis->sDspState, _convert);
			_count += _convert;
		}
		*_NumOutSamples = _count;
		return TRUE;
	}
	else if (_PrVideoMem->pAudioDecodec->pOpus)
	{
		BLS32 _samples = opus_decode(_PrVideoMem->pAudioDecodec->pOpus, _Frame->pBuffer, _Frame->nBufferSize, _Buffer, _PrVideoMem->pAudioDecodec->nNumSamples, 0);
		if (_samples >= 0)
		{
			*_NumOutSamples = _samples;
			return TRUE;
		}
	}
	return FALSE;
}
static _BLVPXDecoderExt*
_VideoCodecOpen(_BlWebMDemuxerExt* _Demuxer)
{
	_BLVPXDecoderExt* _ret = (_BLVPXDecoderExt*)malloc(sizeof(_BLVPXDecoderExt));
	_ret->pCtx = NULL;
	_ret->pIter = NULL;
	_ret->nDelay = 0;
	const vpx_codec_dec_cfg_t _codeccfg = {8, 0, 0};
	vpx_codec_iface_t* _codeciface = NULL;
	switch (_Demuxer->nVideoCodec)
	{
	case 1:
		_codeciface = vpx_codec_vp8_dx();
		break;
	case 2:
		_codeciface = vpx_codec_vp9_dx();
		_ret->nDelay = 8 - 1;
		break;
	default:
		free(_ret);
		return NULL;
	}
	_ret->pCtx = (vpx_codec_ctx_t*)malloc(sizeof(vpx_codec_ctx_t));
	if (vpx_codec_dec_init(_ret->pCtx, _codeciface, &_codeccfg, _ret->nDelay > 0 ? VPX_CODEC_USE_FRAME_THREADING : 0))
	{
		free(_ret->pCtx);
		_ret->pCtx = NULL;
		free(_ret);
		return NULL;
	}
	return _ret;
}
static BLVoid
_VideoCodecClose(_BLVPXDecoderExt* _Codec)
{
	if (!_Codec)
		return;
	if (_Codec->pCtx)
	{
		vpx_codec_destroy(_Codec->pCtx);
		free(_Codec->pCtx);
	}
	free(_Codec);
}
static BLBool
_FrameData(_BLWebMFrameExt* _Frame)
{
	_PrVideoMem->pVideoDecodec->pIter = NULL;
	return VPX_CODEC_OK == vpx_codec_decode(_PrVideoMem->pVideoDecodec->pCtx, _Frame->pBuffer, _Frame->nBufferSize, NULL, 0);
}
static BLBool
_YUVImage(_BLYUVImageExt* _Image)
{
	if (vpx_image_t* _img = vpx_codec_get_frame(_PrVideoMem->pVideoDecodec->pCtx, (vpx_codec_iter_t*)&_PrVideoMem->pVideoDecodec->pIter))
	{
		if ((_img->fmt & VPX_IMG_FMT_PLANAR) && !(_img->fmt & (VPX_IMG_FMT_HAS_ALPHA | VPX_IMG_FMT_HIGHBITDEPTH)))
		{
			if (_img->stride[0] && _img->stride[1] && _img->stride[2])
			{
				BLS32 _uplane = !!(_img->fmt & VPX_IMG_FMT_UV_FLIP) + 1;
				BLS32 _vplane = !(_img->fmt & VPX_IMG_FMT_UV_FLIP) + 1;
				_Image->nWidth = _img->d_w;
				_Image->nHeight = _img->d_h;
				_Image->nChromaShiftW = _img->x_chroma_shift;
				_Image->nChromaShiftH = _img->y_chroma_shift;
				_Image->aPlanes[0] = _img->planes[0];
				_Image->aPlanes[1] = _img->planes[_uplane];
				_Image->aPlanes[2] = _img->planes[_vplane];
				_Image->aLinesize[0] = _img->stride[0];
				_Image->aLinesize[1] = _img->stride[_uplane];
				_Image->aLinesize[2] = _img->stride[_vplane];
				return TRUE;
			}
		}
		else
			return FALSE;
	}
	return FALSE;
}
static BLVoid
_Demuxer(_BlWebMDemuxerExt* _Demuxer, mkvparser::IMkvReader* _Reader, BLS32 _VideoTrack, BLS32 _AudioTrack)
{
	_Demuxer->pReader = _Reader;
	_Demuxer->pSegment = NULL;
	_Demuxer->pCluster = NULL;
	_Demuxer->pBlock = NULL;
	_Demuxer->pBlockEntry = NULL;
	_Demuxer->nBlockFrameIndex = 0;
	_Demuxer->pVideoTrack = NULL;
	_Demuxer->nVideoCodec = 0;
	_Demuxer->pAudioTrack = NULL;
	_Demuxer->nAudioCodec = 0;
	_Demuxer->bOpen = FALSE;
	_Demuxer->bEos = FALSE;
	BLS64 _pos = 0;
	if (mkvparser::EBMLHeader().Parse(_Demuxer->pReader, _pos))
		return;
	if (mkvparser::Segment::CreateInstance(_Demuxer->pReader, _pos, _Demuxer->pSegment))
		return;
	if (_Demuxer->pSegment->Load() < 0)
		return;
	const mkvparser::Tracks* _tracks = _Demuxer->pSegment->GetTracks();
	const unsigned long _trackscount = _tracks->GetTracksCount();
	BLS32 _currvideotrack = -1, _curraudiotrack = -1;
	for (BLU32 _idx = 0; _idx < _trackscount; ++_idx)
	{
		const mkvparser::Track* _track = _tracks->GetTrackByIndex(_idx);
		if (const BLAnsi* _codecid = _track->GetCodecId())
		{
			if ((!_Demuxer->pVideoTrack || _currvideotrack != _VideoTrack) && _track->GetType() == mkvparser::Track::kVideo)
			{
				if (!strcmp(_codecid, "V_VP8"))
					_Demuxer->nVideoCodec = 1;
				else if (!strcmp(_codecid, "V_VP9"))
					_Demuxer->nVideoCodec = 2;
				if (_Demuxer->nVideoCodec)
					_Demuxer->pVideoTrack = (mkvparser::VideoTrack*)(_track);
				++_currvideotrack;
			}
			if ((!_Demuxer->pAudioTrack || _curraudiotrack != _AudioTrack) && _track->GetType() == mkvparser::Track::kAudio)
			{
				if (!strcmp(_codecid, "A_VORBIS"))
					_Demuxer->nAudioCodec = 1;
				else if (!strcmp(_codecid, "A_OPUS"))
					_Demuxer->nAudioCodec = 2;
				if (_Demuxer->nAudioCodec)
					_Demuxer->pAudioTrack = (mkvparser::AudioTrack*)(_track);
				++_curraudiotrack;
			}
		}
	}
	if (!_Demuxer->pVideoTrack && !_Demuxer->pAudioTrack)
		return;
	_Demuxer->bOpen = TRUE;
}
static BLBool
_NotSupportedTrackNumber(_BlWebMDemuxerExt* _Demuxer, long _VideoTrackNumber, long _AudioTrackNumber)
{
	const long _tracknumber = (long)_Demuxer->pBlock->GetTrackNumber();
	return (_tracknumber != _VideoTrackNumber && _tracknumber != _AudioTrackNumber);
}
static BLBool
_ReadFrame(_BlWebMDemuxerExt* _Demuxer, _BLWebMFrameExt* _VideoFrame, _BLWebMFrameExt* _AudioFrame)
{
	const long _videotracknumber = (_VideoFrame && _Demuxer->pVideoTrack) ? _Demuxer->pVideoTrack->GetNumber() : 0;
	const long _audiotracknumber = (_AudioFrame && _Demuxer->pAudioTrack) ? _Demuxer->pAudioTrack->GetNumber() : 0;
	BLBool _blockentryeos = FALSE;
	if (_VideoFrame)
		_VideoFrame->nBufferSize = 0;
	if (_AudioFrame)
		_AudioFrame->nBufferSize = 0;
	if (_videotracknumber == 0 && _audiotracknumber == 0)
		return FALSE;
	if (_Demuxer->bEos)
		return FALSE;
	if (!_Demuxer->pCluster)
		_Demuxer->pCluster = (mkvparser::Cluster*)_Demuxer->pSegment->GetFirst();
	do
	{
		BLBool _getnewblock = FALSE;
		long _status = 0;
		if (!_Demuxer->pBlockEntry && !_blockentryeos)
		{
			_status = _Demuxer->pCluster->GetFirst(_Demuxer->pBlockEntry);
			_getnewblock = TRUE;
		}
		else if (_blockentryeos || _Demuxer->pBlockEntry->EOS())
		{
			_Demuxer->pCluster = _Demuxer->pSegment->GetNext(_Demuxer->pCluster);
			if (!_Demuxer->pCluster || _Demuxer->pCluster->EOS())
			{
				_Demuxer->bEos = TRUE;
				return FALSE;
			}
			_status = _Demuxer->pCluster->GetFirst(_Demuxer->pBlockEntry);
			_blockentryeos = FALSE;
			_getnewblock = TRUE;
		}
		else if (!_Demuxer->pBlock || _Demuxer->nBlockFrameIndex == _Demuxer->pBlock->GetFrameCount() || _NotSupportedTrackNumber(_Demuxer, _videotracknumber, _audiotracknumber))
		{
			_status = _Demuxer->pCluster->GetNext(_Demuxer->pBlockEntry, _Demuxer->pBlockEntry);
			if (!_Demuxer->pBlockEntry || _Demuxer->pBlockEntry->EOS())
			{
				_blockentryeos = TRUE;
				continue;
			}
			_getnewblock = TRUE;
		}
		if (_status || !_Demuxer->pBlockEntry)
			return FALSE;
		if (_getnewblock)
		{
			_Demuxer->pBlock = _Demuxer->pBlockEntry->GetBlock();
			_Demuxer->nBlockFrameIndex = 0;
		}
	} while (_blockentryeos || _NotSupportedTrackNumber(_Demuxer, _videotracknumber, _audiotracknumber));
	_BLWebMFrameExt* _frame = NULL;
	const long _tracknumber = (long)_Demuxer->pBlock->GetTrackNumber();
	if (_tracknumber == _videotracknumber)
		_frame = _VideoFrame;
	else if (_tracknumber == _audiotracknumber)
		_frame = _AudioFrame;
	else
	{
		assert(_tracknumber == _videotracknumber || _tracknumber == _audiotracknumber);
		return FALSE;
	}
	const mkvparser::Block::Frame& _blockframe = _Demuxer->pBlock->GetFrame(_Demuxer->nBlockFrameIndex++);
	if ((BLU32)_blockframe.len > _frame->nBufferCapacity)
	{
		BLU8* _newbuff = (BLU8*)realloc(_frame->pBuffer, _frame->nBufferCapacity = (BLU32)_blockframe.len);
		if (_newbuff)
			_frame->pBuffer = _newbuff;
		else
			return FALSE;
	}
	_frame->nBufferSize = (BLU32)_blockframe.len;
	_frame->fTime = _Demuxer->pBlock->GetTime(_Demuxer->pCluster) / 1e9;
	_frame->bKey = _Demuxer->pBlock->IsKey();
	return !_blockframe.Read(_Demuxer->pReader, _frame->pBuffer);
}
static BLBool
_RenderAudioFrame(_BLWebMFrameExt* _AudioFrame, _BlWebMDemuxerExt* _Demuxer, BLS16* _Pcm)
{
	if ((_PrVideoMem->pAudioDecodec->pOpus || _PrVideoMem->pAudioDecodec->pVorbis) && (_AudioFrame->nBufferSize > 0))
	{
		if (_PrVideoMem->bAudioSetParams)
		{
			blPCMStreamParam((BLU32)_Demuxer->pAudioTrack->GetChannels(), (BLU32)_Demuxer->pAudioTrack->GetSamplingRate());
			_PrVideoMem->bAudioSetParams = FALSE;
		}
		BLS32 _numoutsamples;
		if (_PCMS16Data(_AudioFrame, _Pcm, &_numoutsamples))
			blPCMStreamData(_Pcm, _numoutsamples * (BLU32)_Demuxer->pAudioTrack->GetChannels() * sizeof(BLS16));
	}
	return TRUE;
}
static BLBool
_RenderVideoFrame(_BLWebMFrameExt* _VideoFrame, _BlWebMDemuxerExt* _Demuxer)
{
	if (_PrVideoMem->pVideoDecodec->pCtx && (_VideoFrame->nBufferSize > 0))
	{
		_BLYUVImageExt _image;
		if (!_FrameData(_VideoFrame))
		{
			blDebugOutput("Video decode error\n");
			return FALSE;
		}
		if (_PrVideoMem->bVideoSetParams)
		{
			_PrVideoMem->nVideoWidth = (BLU32)_Demuxer->pVideoTrack->GetWidth();
			_PrVideoMem->nVideoHeight = (BLU32)_Demuxer->pVideoTrack->GetHeight();
			_PrVideoMem->bVideoSetParams = FALSE;
		}
		while (_YUVImage(&_image))
		{
			BLU8* _imgdata = (BLU8*)malloc((BLU32)(_Demuxer->pVideoTrack->GetWidth() * _Demuxer->pVideoTrack->GetHeight() * sizeof(BLU32)));
			if (_image.nChromaShiftW == 1 && _image.nChromaShiftH == 1)
				yuv420_2_rgb8888(_imgdata, _image.aPlanes[0], _image.aPlanes[2], _image.aPlanes[1], _image.nWidth, _image.nHeight, _image.aLinesize[0], _image.aLinesize[1], _image.nWidth << 2, 0);
			else if (_image.nChromaShiftW == 1 && _image.nChromaShiftH == 0)
				yuv422_2_rgb8888(_imgdata, _image.aPlanes[0], _image.aPlanes[2], _image.aPlanes[1], _image.nWidth, _image.nHeight, _image.aLinesize[0], _image.aLinesize[1], _image.nWidth << 2, 0);
			else if (_image.nChromaShiftW == 0 && _image.nChromaShiftH == 0)
				yuv444_2_rgb8888(_imgdata, _image.aPlanes[0], _image.aPlanes[2], _image.aPlanes[1], _image.nWidth, _image.nHeight, _image.aLinesize[0], _image.aLinesize[1], _image.nWidth << 2, 0);
			else
			{
				blDebugOutput("Invalid YUV Format");
				return FALSE;
			}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			EnterCriticalSection(&(_PrVideoMem->sHandle));
#else
			pthread_mutex_lock(&(_PrVideoMem->sHandle));
#endif
			_BLRGBFrameList::_ListNode* _node;
			_node = (_BLRGBFrameList::_ListNode*)malloc(sizeof(_BLRGBFrameList::_ListNode));
			_node->pData = _imgdata;
			_node->fTime = _VideoFrame->fTime;
			_node->pPrev = _PrVideoMem->pImgDataCache->pLast;
			_node->pNext = NULL;
			if (_node->pPrev != NULL)
				_node->pPrev->pNext = _node;
			_PrVideoMem->pImgDataCache->pLast = _node;
			if (_PrVideoMem->pImgDataCache->pFirst == NULL)
				_PrVideoMem->pImgDataCache->pFirst = _node;
			_PrVideoMem->pImgDataCache->nSize++;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
			LeaveCriticalSection(&(_PrVideoMem->sHandle));
#else
			pthread_mutex_unlock(&(_PrVideoMem->sHandle));
#endif
		}
	}
	return TRUE;
}
#if defined(WIN32)
static DWORD __stdcall
_DecodeThreadFunc(BLVoid* _Userdata)
#else
static BLVoid*
_DecodeThreadFunc(BLVoid* _Userdata)
#endif
{
	BLAnsi* _filename = (BLAnsi*)_Userdata;
	_BlWebMDemuxerExt _demuxer;
	MkvReader* _mkv = new MkvReader(_filename);
	_Demuxer(&_demuxer, _mkv, 0, 0);
	BLF64 _lasttime = 0.0;
	if (_demuxer.bOpen)
	{
		_PrVideoMem->pVideoDecodec = _VideoCodecOpen(&_demuxer);
		_PrVideoMem->pAudioDecodec = _AudioCodecOpen(&_demuxer);
		_BLWebMFrameExt _videoframe, _audioframe;
		_videoframe.nBufferSize = _audioframe.nBufferSize = 0;
		_videoframe.nBufferCapacity = _audioframe.nBufferCapacity = 0;
		_videoframe.pBuffer = _audioframe.pBuffer = NULL;
		_videoframe.fTime = _audioframe.fTime = 0.0;
		_videoframe.bKey = _audioframe.bKey = FALSE;		
		BLS16* _pcm = (_PrVideoMem->pAudioDecodec->pOpus || _PrVideoMem->pAudioDecodec->pVorbis) ? (BLS16*)malloc((BLU32)(_PrVideoMem->pAudioDecodec->nNumSamples * _demuxer.pAudioTrack->GetChannels() * sizeof(BLS16))) : NULL;
		do
		{
			if (!_ReadFrame(&_demuxer, &_videoframe, &_audioframe))
				break;
			if (!_RenderAudioFrame(&_audioframe, &_demuxer, _pcm))
				break;
			if (!_RenderVideoFrame(&_videoframe, &_demuxer))
				break;
			BLF64 _mintime = _audioframe.fTime < _videoframe.fTime ? _audioframe.fTime : _videoframe.fTime;
			blSysTickDelay((BLU32)((_mintime - _lasttime) * 500));
			_lasttime = _mintime;
		} while (!blSysPeekEvent(_PrVideoMem->bClickInterrupt, _PrVideoMem->eKeyInterrupt));
		free(_pcm);
		_PrVideoMem->fTime = 0.0;
		if (_videoframe.pBuffer)
			free(_videoframe.pBuffer);
		if (_audioframe.pBuffer)
			free(_audioframe.pBuffer);
	}
	delete _mkv;
	_PrVideoMem->bVideoOver = TRUE;
#if defined(WIN32)
	return 0xDEAD;
#else
	return (BLVoid*)0xDEAD;
#endif
}
#endif
BLVoid
blVideoOpenEXT(IN BLAnsi* _Version, ...)
{
#if !defined(EMSCRIPTEN)
	_PrVideoMem = (_BLVideoMemberExt*)malloc(sizeof(_BLVideoMemberExt));
	_PrVideoMem->bAudioSetParams = TRUE;
	_PrVideoMem->bVideoSetParams = TRUE;
	_PrVideoMem->nTex = INVALID_GUID;
	_PrVideoMem->nGeo = INVALID_GUID;
	_PrVideoMem->fTime = 0.0;
	_PrVideoMem->pAudioDecodec = NULL;
	_PrVideoMem->pVideoDecodec = NULL;
	_PrVideoMem->nVideoWidth = 0;
	_PrVideoMem->nVideoHeight = 0;
	_PrVideoMem->bVideoOver = FALSE;
	_PrVideoMem->pImgDataCache = (_BLRGBFrameList*)malloc(sizeof(_BLRGBFrameList));
	_PrVideoMem->pImgDataCache->nSize = 0;
	_PrVideoMem->pImgDataCache->pFirst = _PrVideoMem->pImgDataCache->pLast = NULL;
    _PrVideoMem->bClickInterrupt = FALSE;
    _PrVideoMem->eKeyInterrupt = BL_KC_UNKNOWN;
	BLEnum _semantic[] = { BL_SL_POSITION, BL_SL_COLOR0, BL_SL_TEXCOORD0 };
	BLEnum _decls[] = { BL_VD_FLOATX2, BL_VD_FLOATX4, BL_VD_FLOATX2 };
	_PrVideoMem->nTech = blTechniqueGain(blHashString((const BLUtf8*)"shaders/2D.bsl"));
	BLF32 _vbo[] = {
		0.f,
		0.f,
		1.f,
		1.f,
		1.f,
		1.f,
		0.f,
		0.f,
		1.f,
		0.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		0.f,
		0.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		0.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f
	};
	_PrVideoMem->nGeo = blGeometryBufferGen(0xFFFFFFFF, BL_PT_TRIANGLESTRIP, TRUE, _semantic, _decls, 3, _vbo, sizeof(_vbo), NULL, 0, BL_IF_INVALID);
	blGpuVSync(FALSE);
	blSysCursorVisiblity(FALSE);
#if defined(WIN32)
	InitializeCriticalSectionEx(&(_PrVideoMem->sHandle), 0, 0);
#else
	pthread_mutex_init(&(_PrVideoMem->sHandle), NULL);
#endif
#endif
}
BLVoid
blVideoCloseEXT()
{
#if !defined(EMSCRIPTEN)
#if defined(WIN32)
	DeleteCriticalSection(&(_PrVideoMem->sHandle));
#else
	pthread_mutex_destroy(&(_PrVideoMem->sHandle));
#endif
	blGpuVSync(TRUE);
	blSysCursorVisiblity(TRUE);
	blPCMStreamParam(-1, -1);
	_AudioCodecClose(_PrVideoMem->pAudioDecodec);
	_VideoCodecClose(_PrVideoMem->pVideoDecodec);
	if (INVALID_GUID != _PrVideoMem->nTex)
		blTextureDelete(_PrVideoMem->nTex);
	if (INVALID_GUID != _PrVideoMem->nGeo)
		blGeometryBufferDelete(_PrVideoMem->nGeo);
	blTechniqueDelete(_PrVideoMem->nTech);
	free(_PrVideoMem->pImgDataCache);
#endif
}
BLBool
blVideoPlayEXT(IN BLAnsi* _Filename, IN BLBool _LClickInterrupt, IN BLEnum _KeyInterrupt)
{
#if defined(EMSCRIPTEN)
	EM_ASM_ARGS({
		var _url = UTF8ToString($0);
		var _click = UTF8ToString($1);
		var _key = UTF8ToString($2);
		var _glcanvas = document.querySelector('canvas');				
		var _video = document.createElement('video');
		var _absx = _glcanvas.getBoundingClientRect().left + document.body.scrollLeft; 
		var _absy = _glcanvas.getBoundingClientRect().top + document.body.scrollTop; 
		var _width = _glcanvas.getBoundingClientRect().right - _glcanvas.getBoundingClientRect().left;
		var _height = _glcanvas.getBoundingClientRect().bottom - _glcanvas.getBoundingClientRect().top;
		_video.setAttribute('width', _width + 'px');
		_video.setAttribute('height', '100%');
		_video.setAttribute('autoplay', 'yes');
		_video.setAttribute('src', _url);
		_video.setAttribute('type', 'video/webm');
		_video.style.position = 'fixed';
		_video.style.left = _absx + 'px';
		_video.style.top = '0px';
		var _videobg = document.createElement('videobackground');	
		_videobg.setAttribute('id', 'videobackground');	
        _videobg.style.background = '#000000';
		_videobg.style.position = 'fixed';
		_videobg.style.left = _absx + 'px';
		_videobg.style.top = _absy + 'px';
        _videobg.style.width = _width + 'px';
        _videobg.style.height = _height + 'px';
		_videobg.appendChild(_video);
		document.body.appendChild(_videobg);
		_video.focus();
		_video.addEventListener('ended', function () {			
			var _glcanvas = document.querySelector('canvas');
			var _videobg = document.querySelector('videobackground');
			if (_videobg != null)
				document.body.removeChild(_videobg);
			_glcanvas.focus();
        }, true);
		if (_click)
		{
			_video.addEventListener('click', function() {
				var _glcanvas = document.querySelector('canvas');
				var _videobg = document.querySelector('videobackground');
				if (_videobg != null)
					document.body.removeChild(_videobg);
				_glcanvas.focus();
			}, false);
		}
		if (_key)
		{
			_video.addEventListener('keydown', function() {
				var _glcanvas = document.querySelector('canvas');
				var _videobg = document.querySelector('videobackground');
				if (_videobg != null)
					document.body.removeChild(_videobg);
				_glcanvas.focus();
			}, false);
		}
	},_Filename);
#else
	BLF64 _framedelta = 0.0;
	BLU32 _width, _height, _dwidth, _dheight;
	BLF32 _rx, _ry;
	blSysWindowQuery(&_width, &_height, &_dwidth, &_dheight, &_rx, &_ry);
	BLF32 _screensz[2] = { 2.f / (BLF32)_width, 2.f / (BLF32)_height };
	blTechniqueUniform(_PrVideoMem->nTech, BL_UB_F32X2, "ScreenDim", _screensz, sizeof(_screensz));
	blGpuRasterState(BL_CM_CW, 0, 0.f, FALSE, 0, 0, 0, 0, FALSE);
#if defined(WIN32)
	DWORD _tid;
	_PrVideoMem->sThread = CreateThread(NULL, 0, _DecodeThreadFunc, (LPVOID)_Filename, 0, &_tid);
#else
	pthread_create(&_PrVideoMem->sThread, NULL, _DecodeThreadFunc, (BLVoid*)_Filename);
#endif
	while (_PrVideoMem->bVideoSetParams || _PrVideoMem->bAudioSetParams)
		blSysTickDelay(5);
	if (_PrVideoMem->nVideoWidth == 0 || _PrVideoMem->nVideoHeight == 0)
		return FALSE;
    _PrVideoMem->bClickInterrupt = _LClickInterrupt;
    _PrVideoMem->eKeyInterrupt = _KeyInterrupt;
	_PrVideoMem->fTime = 0.0;
	_PrVideoMem->nTex = blTextureGen(0xFFFFFFFF, BL_TT_2D, BL_TF_RGBA8, FALSE, FALSE, TRUE, 1, 1, (BLU32)_PrVideoMem->nVideoWidth, (BLU32)_PrVideoMem->nVideoHeight, 1, NULL);
	blTextureFilter(_PrVideoMem->nTex, BL_TF_LINEAR, BL_TF_LINEAR, BL_TW_CLAMP, BL_TW_CLAMP, FALSE);
	blTechniqueSampler(_PrVideoMem->nTech, "Texture0", _PrVideoMem->nTex, 0);
	BLF32 _videoratio = (BLF32)_PrVideoMem->nVideoWidth / (BLF32)_PrVideoMem->nVideoHeight;
	BLF32 _vbo[] = {
		0.f,
		(_height - _width / _videoratio) * 0.5f,
		1.f,
		1.f,
		1.f,
		1.f,
		0.f,
		0.f,
		(BLF32)_width,
		(_height - _width / _videoratio) * 0.5f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		0.f,
		0.f,
		(_height - _width / _videoratio) * 0.5f + _width / _videoratio,
		1.f,
		1.f,
		1.f,
		1.f,
		0.f,
		1.f,
		(BLF32)_width,
		(_height - _width / _videoratio) * 0.5f + _width / _videoratio,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f,
		1.f
	};
	blGeometryBufferUpdate(_PrVideoMem->nGeo, 0, (BLU8*)_vbo, sizeof(_vbo), 0, NULL, 0);
	do {
		while (!_PrVideoMem->pImgDataCache->nSize)
		{
			if (_PrVideoMem->bVideoOver)
				goto videoend;
			blSysTickDelay(10);
		}
		blFrameBufferClear(TRUE, TRUE, FALSE);
		blTextureUpdate(_PrVideoMem->nTex, 0, 0, BL_CTF_IGNORE, 0, 0, 0, _PrVideoMem->nVideoWidth, _PrVideoMem->nVideoHeight, 1, _PrVideoMem->pImgDataCache->pFirst->pData);
		blTechniqueDraw(_PrVideoMem->nTech, _PrVideoMem->nGeo, 1);
		blFlush();
		_framedelta = _PrVideoMem->pImgDataCache->pFirst->fTime - _PrVideoMem->fTime;
		blSysTickDelay((BLU32)(_framedelta * 1000));
		_PrVideoMem->fTime = _PrVideoMem->pImgDataCache->pFirst->fTime;
#if defined(WIN32)
		EnterCriticalSection(&(_PrVideoMem->sHandle));
#else
		pthread_mutex_lock(&(_PrVideoMem->sHandle));
#endif
		_BLRGBFrameList::_ListNode* _node;
		_node = _PrVideoMem->pImgDataCache->pFirst;		
		_PrVideoMem->pImgDataCache->pFirst = _node->pNext;
		if (_node == _PrVideoMem->pImgDataCache->pLast)
			_PrVideoMem->pImgDataCache->pLast = _node->pPrev;
		else
			_node->pNext->pPrev = _node->pPrev;
		_PrVideoMem->pImgDataCache->nSize--;
		free(_node->pData);
		free(_node);
#if defined(WIN32)
		LeaveCriticalSection(&(_PrVideoMem->sHandle));
#else
		pthread_mutex_unlock(&(_PrVideoMem->sHandle));
#endif
	} while (!_PrVideoMem->bVideoOver);
videoend:
#if defined(WIN32)
	WaitForSingleObjectEx(_PrVideoMem->sThread, INFINITE, TRUE);
	CloseHandle(_PrVideoMem->sThread);
#else
	pthread_join(_PrVideoMem->sThread, NULL);
#endif	
	for (struct _BLRGBFrameList::_ListNode* _iter = _PrVideoMem->pImgDataCache->pFirst; _iter; _iter = _iter->pNext)	
		free(_iter->pData);
#endif
	return TRUE;
}
