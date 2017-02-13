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
#include "../headers/gpu.h"
#include "../headers/system.h"
#include "internal/dictionary.h"
#include "internal/array.h"
#include "internal/mathematics.h"
#include "internal/internal.h"
#if defined BL_PLATFORM_OSX
#   include <AppKit/NSOpenGL.h>
#   include <AppKit/NSApplication.h>
#elif defined BL_PLATFORM_ANDROID
#	include <EGL/egl.h>
#	include <EGL/eglext.h>
#	include <android/native_window.h>
#endif
typedef struct _HardwareCaps{
	BLEnum eApiType;
	BLU64 nFuncSupported;
	BLU16 nMaxTextureSize;
	BLU8  nMaxFBAttachments;
    BLBool bCSSupport;
    BLBool bGSSupport;
    BLBool bAnisotropy;
    BLBool bTessellationSupport;
    BLBool bFloatRTSupport;
	BLU16 aTexFormats[BL_TF_COUNT];
}_BLHardwareCaps;
typedef struct _UniformBuffer{
	struct _UniformInfo {
		union {
			BLS32 aS32Var[4];
			BLU32 aU32Var[4];
			BLF32 aF32Var[4];
			BLF32 aMatVar[16];
			void* pAddr;
		} uData;
		BLEnum eType;
		BLU16 nHandle;
	};
	BLDictionary* pUniforms;
}_BLUniformBuffer;
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_LINUX) || defined(BL_PLATFORM_OSX)
typedef struct _TextureBuffer {
	GLuint nID;
	GLuint nRBO;
	BLU8* pData;
	BLU32 nFlags;
	BLU32 nWidth;
	BLU32 nHeight;
	BLU32 nDepth;
	BLU32 nLayer;
	BLU8 nType;
	BLU8 nTextureFormat;
	BLU8 nNumMips;
}_BLTextureBufferGL;
#elif defined(BL_PLATFORM_UWP)
typedef struct _CommandQueue {
	struct _CommandList {
		ID3D12GraphicsCommandList* pCommandList;
		ID3D12CommandAllocator* pCommandAllocator;
		HANDLE pEvent;
	};
	struct _RingBuffer {
		BLU32 nSize;
		BLU32 nCurrent;
		BLU32 nWrite;
		BLU32 nRead;
	};
	_CommandList aCommandList[256];
	BLArray* pRelease[256];
	_RingBuffer sControl;
	ID3D12CommandQueue* pCommandQueue;
	ID3D12Fence* pFence;
	BLU64 nCurrentFence;
	BLU64 nCompletedFence;
}_BLCommandQueueDX;
typedef struct _GenericBuffer {
	D3D12_SHADER_RESOURCE_VIEW_DESC  sSrvd;
	D3D12_UNORDERED_ACCESS_VIEW_DESC sUavd;
	D3D12_GPU_VIRTUAL_ADDRESS nGpuVA;
	D3D12_RESOURCE_STATES eState;
	ID3D12Resource* pPtr;
	BLU32 nSize;
	BLU16 nFags;
	BLU16 nDecl;
	BLBool bDynamic;
}_BLGenericBufferDX;
typedef struct _ScratchBuffer {
	ID3D12DescriptorHeap* pHeap;
	ID3D12Resource* pUpload;
	D3D12_GPU_VIRTUAL_ADDRESS nGpuVA;
	D3D12_CPU_DESCRIPTOR_HANDLE sCpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE sGpuHandle;
	BLU8* pData;
	BLU32 nIncrementSize;
	BLU32 nPos;
}_BLScrathBufferDX;
typedef struct _SamplerBuffer {
	ID3D12DescriptorHeap* pHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE sCpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE sGpuHandle;
	BLU32 nIncrementSize;
	BLU16 nDescriptorsPerBlock;
}_BLSamplerBufferDX;
typedef struct _TextureBuffer {
	D3D12_SHADER_RESOURCE_VIEW_DESC  sSrvd;
	D3D12_UNORDERED_ACCESS_VIEW_DESC sUavd;
	ID3D12Resource* pPtr;
	D3D12_RESOURCE_STATES eState;
	BLU8* pData;
	BLU32 nFlags;
	BLU32 nWidth;
	BLU32 nHeight;
	BLU32 nDepth;
	BLU32 nLayer;
	BLU8 nType;
	BLU8 nTextureFormat;
	BLU8 nNumMips;
}_BLTextureBufferDX;
typedef struct _BatchBuffer {
	struct _DrawVertexIndirectCommand {
		D3D12_GPU_VIRTUAL_ADDRESS nCbv;
		D3D12_VERTEX_BUFFER_VIEW aVbv[2];
		D3D12_DRAW_ARGUMENTS sDraw;
	};
	struct _DrawIndexedIndirectCommand {
		D3D12_GPU_VIRTUAL_ADDRESS nCbv;
		D3D12_VERTEX_BUFFER_VIEW aVbv[2];
		D3D12_INDEX_BUFFER_VIEW sIbv;
		D3D12_DRAW_INDEXED_ARGUMENTS sDrawIndexed;
	};
	struct _Stats {
		BLU32 nImmediate[2];
		BLU32 nIndirect[2];
	};
	ID3D12CommandSignature* aCommandSignature[2];
	_DrawIndexedIndirectCommand aCurrent;
	_BLGenericBufferDX aIndirect[32];
	_Stats sStats;
	void* nCmds[2];
	BLU32 aNum[2];
	BLU32 nCurrIndirect;
	BLU32 nMaxDrawPerBatch;
	BLU32 nMinIndirect;
	BLU32 nFlushPerBatch;
}_BLBatchBufferDX;
typedef struct _FrameBuffer {
	struct _Attachment {
		BLU16 nHandle;
		BLU16 nMip;
		BLU16 nLayer;
	};
	IDXGISwapChain* pSwapChain;
	BLU16 aTexture[8];
	_Attachment aAttachment[8];
	BLU16 nDepth;
	BLU32 nWidth;
	BLU32 nHeight;
	BLU16 nDenseIdx;
	BLU8 nNum;
	BLU8 nNumTh;
}_BLFrameBufferDX;
typedef struct _ShaderBuffer {
	struct _Buffer {
		BLU32 nSize;
		BLU32 nPos;
		BLU8 aBuffer[8];
	} *sConstBuffer;
	struct _Handle {
		BLU32 nLoc;
		BLU16 nCount;
		BLU8 nType;
	}aPredefined[UNIFORM_COUNT_INTERNAL];
	void* pCode;
	BLU32 nHash;
	BLU16 nAttrMask[SHADERATR_COUNT_INTERNAL];
	BLU16 nUniforms;
	BLU16 nSize;
	BLU8 nPredefined;
}_BLShaderBufferDX;
#elif defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_ANDROID)
#endif
typedef struct _GpuMember {
	_BLHardwareCaps sHardwareCaps;
	BLF32 fPresentElapsed;
	BLBool bVsync;
	BLU16 nCurFramebufferHandle;
	BLU16 nBackBufferIdx;
#if defined(BL_PLATFORM_WIN32)
	HGLRC sGLRC;
#elif defined(BL_PLATFORM_UWP)
	IDXGIFactory4* pDxgiFactory;
	ID3D12Device* pDevice;
	IDXGIAdapter3* pAdapter;
	IDXGISwapChain1* pSwapChain;
	ID3D12DescriptorHeap* pRtvHeap;
	ID3D12DescriptorHeap* pDsvHeap;
	ID3D12Resource* aBackColor[3];
	ID3D12Resource* pDepthStencil;
	ID3D12RootSignature* pRootSignature;
	ID3D12GraphicsCommandList* pCommandList;
	_BLGenericBufferDX aVertexBuffers[4096];
	_BLGenericBufferDX aIndexBuffers[4096];
	_BLShaderBufferDX aShaderBuffers[512];
	_BLScrathBufferDX aScratchBuffer[3];
	_BLFrameBufferDX aFrameBuffer[128];
	_BLSamplerBufferDX sSamplerBuffer;
	_BLCommandQueueDX sCmdQueue;
	_BLUniformBuffer sUniformBuffer;
	_BLBatchBufferDX sBatchBuffer;
	BLDictionary* pPipelineState;
	BLDictionary* pTextureDict;
	BLU64 aBackBufferFence[3];
#elif defined(BL_PLATFORM_OSX)
    NSOpenGLContext* pGLC;
#elif defined(BL_PLATFORM_IOS)
#elif defined(BL_PLATFORM_LINUX)
    GLXContext pContext;
    BLBool bCtxError;
#elif defined(BL_PLATFORM_ANDROID)
	EGLDisplay pEglDisplay;
	EGLContext pEglContext;
	EGLSurface pEglSurface;
	EGLConfig pEglConfig;
#endif
}_BLGpuMember;
static _BLGpuMember* _PrGpuMem = NULL;

#if defined(BL_PLATFORM_WIN32)
static BLBool
_IsTextureFormatValid(BLEnum _Fmt, BLBool _Srgb)
{
	const GLenum _glinternalfmt[] = {
		GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_ATC_RGB_AMD, GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, GL_ZERO, GL_R8, GL_R8I, GL_R8UI, GL_R8_SNORM,
		GL_R16, GL_R16I, GL_R16UI, GL_R16F, GL_R16_SNORM, GL_R32I, GL_R32UI, GL_R32F, GL_RG8, GL_RG8I, GL_RG8UI, GL_RG8_SNORM, GL_RG16, GL_RG16I, GL_RG16UI, GL_RG16F, GL_RG16_SNORM,
		GL_RG32I, GL_RG32UI, GL_RG32F, GL_RGB8, GL_RGB8I, GL_RGB8UI, GL_RGB8_SNORM, GL_RGB9_E5, GL_RGBA8, GL_RGBA8, GL_RGBA8I, GL_RGBA8UI, GL_RGBA8_SNORM, GL_RGBA16, GL_RGBA16I,
		GL_RGBA16UI, GL_RGBA16F, GL_RGBA16_SNORM, GL_RGBA32I, GL_RGBA32UI, GL_RGBA32F, GL_RGB10_A2, GL_R11F_G11F_B10F, GL_ZERO, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH24_STENCIL8,
		GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F, GL_STENCIL_INDEX8
	};
	const GLenum _glsrgbfmt[] = {
		GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_COMPRESSED_SRGB8_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO,
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_SRGB8, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_SRGB8_ALPHA8, GL_SRGB8_ALPHA8,
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO,
		GL_ZERO, GL_ZERO, GL_ZERO
	};
	const GLenum _glfmt[] = {
		GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_ZERO, GL_RED, GL_RED_INTEGER,
		GL_RED_INTEGER, GL_RED, GL_RED, GL_RED_INTEGER, GL_RED_INTEGER, GL_RED, GL_RED, GL_RED_INTEGER, GL_RED_INTEGER, GL_RED, GL_RG, GL_RG_INTEGER, GL_RG_INTEGER, GL_RG, GL_RG, GL_RG_INTEGER,
		GL_RG_INTEGER, GL_RG, GL_RG, GL_RG_INTEGER, GL_RG_INTEGER, GL_RG, GL_RGB, GL_RGB_INTEGER, GL_RGB_INTEGER, GL_RGB, GL_RGB, GL_BGRA, GL_RGBA, GL_RGBA_INTEGER, GL_RGBA_INTEGER,
		GL_RGBA, GL_RGBA, GL_RGBA_INTEGER, GL_RGBA_INTEGER, GL_RGBA, GL_RGBA, GL_RGBA_INTEGER, GL_RGBA_INTEGER, GL_RGBA, GL_RGBA, GL_RGB, GL_ZERO, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
		GL_DEPTH_STENCIL, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_STENCIL_INDEX
	};
	const GLenum _gltype[] = {
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT,
		GL_SHORT, GL_UNSIGNED_SHORT, GL_HALF_FLOAT, GL_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT,
		GL_UNSIGNED_SHORT, GL_FLOAT, GL_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_INT_5_9_9_9_REV, GL_UNSIGNED_BYTE,
		GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_SHORT, GL_HALF_FLOAT, GL_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT,
		GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_ZERO, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_UNSIGNED_INT_24_8, GL_UNSIGNED_INT, GL_FLOAT, GL_FLOAT, GL_FLOAT,GL_UNSIGNED_BYTE
	};
	GLenum _internalfmt = _Srgb ? _glsrgbfmt[_Fmt] : _glinternalfmt[_Fmt];
	if (GL_ZERO == _internalfmt)
		return FALSE;
	GLuint _id;
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	GLsizei _size = (16 * 16 * BLOCK_INFO_BPP_INTERNAL[_Fmt]) / 8;
	void* _ptr;
	union { void* ptr; BLU32 addr; } _un;
	_un.ptr = alloca(_size + 16);
	BLU32 unaligned = _un.addr;
	BLU32 mask = 15;
	BLU32 aligned = (((unaligned)+(mask)) & ((~0)&(~(mask))));
	_un.addr = aligned;
	_ptr = _un.ptr;
	memset(_ptr, 1, _size);
	if (_Fmt < BL_TF_UNKNOWN)
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, _glinternalfmt[_Fmt], 16, 16, GL_ZERO, _size, _ptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, _glinternalfmt[_Fmt], 16, 16, 0, _glfmt[_Fmt], _gltype[_Fmt], _ptr);
	glDeleteTextures(1, &_id);
	if (glGetError())
		return FALSE;
	else
		return TRUE;
}

static BLBool
_IsFramebufferFormatValid(BLEnum _Fmt, BLBool _Srgb)
{
	const GLenum _glinternalfmt[] = {
		GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ETC1_RGB8_OES, GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_ATC_RGB_AMD, GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, GL_ZERO, GL_R8, GL_R8I, GL_R8UI, GL_R8_SNORM,
		GL_R16, GL_R16I, GL_R16UI, GL_R16F, GL_R16_SNORM, GL_R32I, GL_R32UI, GL_R32F, GL_RG8, GL_RG8I, GL_RG8UI, GL_RG8_SNORM, GL_RG16, GL_RG16I, GL_RG16UI, GL_RG16F, GL_RG16_SNORM,
		GL_RG32I, GL_RG32UI, GL_RG32F, GL_RGB8, GL_RGB8I, GL_RGB8UI, GL_RGB8_SNORM, GL_RGB9_E5, GL_RGBA8, GL_RGBA8, GL_RGBA8I, GL_RGBA8UI, GL_RGBA8_SNORM, GL_RGBA16, GL_RGBA16I,
		GL_RGBA16UI, GL_RGBA16F, GL_RGBA16_SNORM, GL_RGBA32I, GL_RGBA32UI, GL_RGBA32F, GL_RGB10_A2, GL_R11F_G11F_B10F, GL_ZERO, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH24_STENCIL8,
		GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F, GL_STENCIL_INDEX8
	};
	const GLenum _glsrgbfmt[] = {
		GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, GL_COMPRESSED_SRGB8_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO,
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_SRGB8, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_SRGB8_ALPHA8, GL_SRGB8_ALPHA8,
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO,
		GL_ZERO, GL_ZERO, GL_ZERO
	};
	const GLenum _glfmt[] = {
		GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ETC1_RGB8_OES, GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_ZERO, GL_RED, GL_RED_INTEGER,
		GL_RED_INTEGER, GL_RED, GL_RED, GL_RED_INTEGER, GL_RED_INTEGER, GL_RED, GL_RED, GL_RED, GL_RED_INTEGER, GL_RED, GL_RG, GL_RG, GL_RG, GL_RG, GL_RG, GL_RG, GL_RG, GL_RG, GL_RG,
		GL_RG, GL_RED_INTEGER, GL_RG, GL_RGB, GL_RGB, GL_RGB, GL_RGB, GL_RGB, GL_BGRA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA_INTEGER,
		GL_RGBA, GL_RGBA, GL_RGB, GL_ZERO, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_STENCIL_INDEX
	};
	const GLenum _gltype[] = {
		GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT,
		GL_SHORT, GL_UNSIGNED_SHORT, GL_HALF_FLOAT, GL_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT,
		GL_UNSIGNED_SHORT, GL_FLOAT, GL_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_INT_5_9_9_9_REV, GL_UNSIGNED_BYTE,
		GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_SHORT, GL_HALF_FLOAT, GL_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT,
		GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_ZERO, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_UNSIGNED_INT_24_8, GL_UNSIGNED_INT, GL_FLOAT, GL_FLOAT, GL_FLOAT,GL_UNSIGNED_BYTE
	};
	GLenum _internalfmt = _Srgb ? _glsrgbfmt[_Fmt] : _glinternalfmt[_Fmt];
	if (GL_ZERO == _internalfmt)
		return FALSE;
	GLuint _fbo;
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	GLuint _id;
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	GLsizei _size = (16 * 16 * BLOCK_INFO_BPP_INTERNAL[_Fmt]) / 8;
	void* _ptr;
	union { void* ptr; BLU32 addr; } _un;
	_un.ptr = alloca(_size + 16);
	BLU32 unaligned = _un.addr;
	BLU32 mask = 15;
	BLU32 aligned = (((unaligned)+(mask)) & ((~0)&(~(mask))));
	_un.addr = aligned;
	_ptr = _un.ptr;
	if (_Fmt < BL_TF_UNKNOWN)
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, _glinternalfmt[_Fmt], 16, 16, GL_ZERO, _size, _ptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, _glinternalfmt[_Fmt], 16, 16, 0, _glfmt[_Fmt], _gltype[_Fmt], _ptr);
	GLenum _attachment;
	if (_Fmt > BL_TF_DEPTH_UNKNOWN && _Fmt < BL_TF_COUNT)
	{
		if (_Fmt == BL_TF_D16 || _Fmt == BL_TF_D24 || _Fmt == BL_TF_D32 || _Fmt == BL_TF_D16F || _Fmt == BL_TF_D24F || _Fmt == BL_TF_D32F)
			_attachment = GL_DEPTH_ATTACHMENT;
		else if (_Fmt == BL_TF_D0S8)
			_attachment = GL_STENCIL_ATTACHMENT;
		else
			_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
	}
	else
		_attachment = GL_COLOR_ATTACHMENT0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, _id, 0);
	glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &_fbo);
	glDeleteTextures(1, &_id);
	return GL_FRAMEBUFFER_COMPLETE == glGetError();
}

void
_GpuIntervention(HWND _Hwnd, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.nFuncSupported = 0;
	BLBool _vkinited = FALSE;
	BLBool _gpuok = FALSE;
	if (!_vkinited)
	{
	}
	if (!_vkinited)
	{
		BLS32 _glversion;
		_PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
		PIXELFORMATDESCRIPTOR _pfd;
		memset(&_pfd, 0, sizeof(_pfd));
		_pfd.nSize = sizeof(_pfd);
		_pfd.nVersion = 1;
		_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		_pfd.iPixelType = PFD_TYPE_RGBA;
		_pfd.cColorBits = 32;
		_pfd.cDepthBits = 24;
		_pfd.cStencilBits = 8;
		_pfd.iLayerType = PFD_MAIN_PLANE;
		BLS32 _pf = ChoosePixelFormat(GetDC(_Hwnd), &_pfd);
		assert(_pf != 0);
		SetPixelFormat(GetDC(_Hwnd), _pf, &_pfd);
		_PrGpuMem->sGLRC = wglCreateContext(GetDC(_Hwnd));
		wglMakeCurrent(GetDC(_Hwnd), _PrGpuMem->sGLRC);
		HMODULE _lib = LoadLibraryW(L"opengl32.dll");
		GL_LOAD_INTERNAL(_lib);
		GL_SYMBOL_INTERNAL(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB, _lib);
		GL_SYMBOL_INTERNAL(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT, _lib);
		FreeLibrary(_lib);
		BLS32 _versions[5][2] =
		{
			{ 4 , 5 },
			{ 4 , 4 },
			{ 4 , 3 },
			{ 4 , 2 },
			{ 4 , 1 }
		};
		BLS32 _attribus[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 5,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};
		for (BLS32 _idx = 0; _idx < 5 ; ++_idx)
		{
			_attribus[1] = _versions[_idx][0];
			_attribus[3] = _versions[_idx][1];
			HGLRC _hwgl = wglCreateContextAttribsARB(GetDC(_Hwnd), 0, _attribus);
			if (_hwgl)
			{
				_gpuok = TRUE;
				blDebugOutput("Opengl %d.%d boost", _versions[_idx][0], _versions[_idx][1]);
				_glversion = _versions[_idx][0] * 10 + _versions[_idx][1];
				wglMakeCurrent(GetDC(_Hwnd), 0);
				wglDeleteContext(_PrGpuMem->sGLRC);
				_PrGpuMem->sGLRC = _hwgl;
				wglMakeCurrent(GetDC(_Hwnd), _PrGpuMem->sGLRC);
				break;
			}
		}
		if (_gpuok && _PrGpuMem->bVsync)
			wglSwapIntervalEXT(1);
		else if (_gpuok && !_PrGpuMem->bVsync)
			wglSwapIntervalEXT(0);
		else
		{
			blDebugOutput("bulllord needs opengl 4.1 or above");
			return;
		}
		const GLenum _glinternalfmt[] = {
			GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
			GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_ATC_RGB_AMD, GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, GL_ZERO, GL_R8, GL_R8I, GL_R8UI, GL_R8_SNORM,
			GL_R16, GL_R16I, GL_R16UI, GL_R16F, GL_R16_SNORM, GL_R32I, GL_R32UI, GL_R32F, GL_RG8, GL_RG8I, GL_RG8UI, GL_RG8_SNORM, GL_RG16, GL_RG16I, GL_RG16UI, GL_RG16F, GL_RG16_SNORM,
			GL_RG32I, GL_RG32UI, GL_RG32F, GL_RGB8, GL_RGB8I, GL_RGB8UI, GL_RGB8_SNORM, GL_RGB9_E5, GL_RGBA8, GL_RGBA8, GL_RGBA8I, GL_RGBA8UI, GL_RGBA8_SNORM, GL_RGBA16, GL_RGBA16I,
			GL_RGBA16UI, GL_RGBA16F, GL_RGBA16_SNORM, GL_RGBA32I, GL_RGBA32UI, GL_RGBA32F, GL_RGB10_A2, GL_R11F_G11F_B10F, GL_ZERO, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH24_STENCIL8,
			GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT32F, GL_STENCIL_INDEX8
		};
		GLint _numcmpformats = 0;
		GL_CHECK_INTERNAL(glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &_numcmpformats));
		GLint* _cmpformat = NULL;
		BLBool _texsupport[BL_TF_COUNT] = { FALSE };
		if (0 < _numcmpformats)
		{
			_numcmpformats = _numcmpformats > 256 ? 256 : _numcmpformats;
			_cmpformat = (GLint*)malloc(sizeof(GLint)*_numcmpformats);
			GL_CHECK_INTERNAL(glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, _cmpformat));
			for (BLS32 _i = 0; _i < _numcmpformats; ++_i)
			{
				GLint _internalfmt = _cmpformat[_i];
				BLU32 _fmt = BL_TF_UNKNOWN;
				for (BLU32 _j = 0; _j < _fmt; ++_j)
				{
					if (_glinternalfmt[_j] == (GLenum)_internalfmt)
					{
						_texsupport[_j] = TRUE;
						_fmt = _j;
					}
				}
			}
		}
		GLint _tmp = 0;
		GL_CHECK_INTERNAL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_tmp));
		_PrGpuMem->sHardwareCaps.nMaxTextureSize = _tmp;
		GL_CHECK_INTERNAL(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &_tmp));
		_PrGpuMem->sHardwareCaps.nMaxFBAttachments = MIN_INTERNAL(_tmp, 8);
		_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_TEXTURE2D_ARRAY_INTERNAL;
		_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_TEXTURECUBE_ARRAY_INTERNAL;
		_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_DRAW_INDIRECT_INTERNAL;
		_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_PROGRAM_PIPELINE_INTERNAL;
		if (_glversion >= 43)
		{
			_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_COMPUTE_SHADER_INTERNAL;
			_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_DRAW_MULTIINDIRECT_INTERNAL;
		}
		for (BLU32 _i =  0; _i < BL_TF_COUNT; ++_i)
		{
			if (BL_TF_UNKNOWN != _i &&  BL_TF_DEPTH_UNKNOWN != _i)
				_texsupport[_i] = _IsTextureFormatValid(_i, FALSE);
		}
		GLuint _vao;
		glGenVertexArrays(1, &_vao);
		glDeleteVertexArrays(1, &_vao);
		if (!glGetError())
			_PrGpuMem->sHardwareCaps.nFuncSupported |= CAP_VAO_INTERNAL;
		for (BLU32 _i = 0; _i < BL_TF_COUNT; ++_i)
		{
			BLU16 _supported = BL_TS_NONE;
			_supported |= _texsupport[_i] ? BL_TS_2D | BL_TS_3D | BL_TS_CUBE : BL_TS_NONE;
			_supported |= _IsTextureFormatValid(_i, TRUE) ? BL_TS_2D_SRGB | BL_TS_3D_SRGB | BL_TS_CUBE_SRGB : BL_TS_NONE;
			_supported |= (_PrGpuMem->sHardwareCaps.nFuncSupported & CAP_COMPUTE_SHADER_INTERNAL) && _IsTextureFormatValid(_i, FALSE) ? BL_TS_IMAGE : BL_TS_NONE;
			_supported |= _IsFramebufferFormatValid(_i, FALSE) ? BL_TS_FRAMEBUFFER : BL_TS_NONE;
			_PrGpuMem->sHardwareCaps.aTexFormats[_i] = _supported;
		}
		free(_cmpformat);
	}
}
void
_GpuSwapBuffer(BLBool _Discard)
{
}
void
_GpuAnitIntervention(HWND _Hwnd)
{
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		wglMakeCurrent(GetDC(_Hwnd), 0);
		wglDeleteContext(_PrGpuMem->sGLRC);
	}
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_UWP)
static ID3D12GraphicsCommandList*
_AllocCmdList()
{
	do
	{
		BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nRead - _PrGpuMem->sCmdQueue.sControl.nWrite + _PrGpuMem->sCmdQueue.sControl.nSize;
		BLU32 _dist = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nRead - (BLS32)_PrGpuMem->sCmdQueue.sControl.nWrite) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nRead - _PrGpuMem->sCmdQueue.sControl.nWrite) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nRead - (BLS32)_PrGpuMem->sCmdQueue.sControl.nWrite) >> 31)) - 1;
		BLU32 _maxsz = ((_PrGpuMem->sCmdQueue.sControl.nSize - 1) & (((BLS32)_dist) >> 31)) | (_dist & ~(((BLS32)_dist) >> 31));
		BLU32 _sizenosign = 1 & 0x7fffffff;
		BLU32 _test = _sizenosign - _maxsz;
		BLU32 _size = ((1) & (((BLS32)_test) >> 31)) | (_maxsz & ~(((BLS32)_test) >> 31));
		BLU32 _advance = _PrGpuMem->sCmdQueue.sControl.nWrite + _size;
		BLU32 _write = _advance % _PrGpuMem->sCmdQueue.sControl.nSize;
		_PrGpuMem->sCmdQueue.sControl.nWrite = _write;
		if (_size == 0)
		{
			_BLCommandQueueDX::_CommandList& _cmdlist = _PrGpuMem->sCmdQueue.aCommandList[_PrGpuMem->sCmdQueue.sControl.nRead];
			if (WAIT_OBJECT_0 == WaitForSingleObject(_cmdlist.pEvent, INFINITE))
			{
				CloseHandle(_cmdlist.pEvent);
				_cmdlist.pEvent = NULL;
				_PrGpuMem->sCmdQueue.nCompletedFence = _PrGpuMem->sCmdQueue.pFence->GetCompletedValue();
				_PrGpuMem->sCmdQueue.pCommandQueue->Wait(_PrGpuMem->sCmdQueue.pFence, _PrGpuMem->sCmdQueue.nCompletedFence);
				FOREACH_ARRAY(ID3D12Resource*, _iter, _PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead])
				{
					_iter->Release();
				}
				blClearArray(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead]);
				BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
				_maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
				_sizenosign = 1 & 0x7fffffff;
				_test = _sizenosign - _maxsz;
				_size = ((1) & (((BLS32)_test) >> 31)) | (_maxsz & ~(((BLS32)_test) >> 31));
				_advance = _PrGpuMem->sCmdQueue.sControl.nRead + _size;
				BLU32 _read = _advance % _PrGpuMem->sCmdQueue.sControl.nSize;
				_PrGpuMem->sCmdQueue.sControl.nRead = _read;
			}
		}
		else
			break;
	} while (1);
	_BLCommandQueueDX::_CommandList& _cmdlist = _PrGpuMem->sCmdQueue.aCommandList[_PrGpuMem->sCmdQueue.sControl.nCurrent];
	DX_CHECK_INTERNAL(_cmdlist.pCommandAllocator->Reset());
	DX_CHECK_INTERNAL(_cmdlist.pCommandList->Reset(_cmdlist.pCommandAllocator, NULL));
	return _cmdlist.pCommandList;
}
static BLU64
_KickCmdList()
{
	_BLCommandQueueDX::_CommandList& _cmdlist = _PrGpuMem->sCmdQueue.aCommandList[_PrGpuMem->sCmdQueue.sControl.nCurrent];
	DX_CHECK_INTERNAL(_cmdlist.pCommandList->Close());
	ID3D12CommandList* _dx12cmdlist[] = { _cmdlist.pCommandList };
	_PrGpuMem->sCmdQueue.pCommandQueue->ExecuteCommandLists(sizeof(_dx12cmdlist) / sizeof(_dx12cmdlist[0]), _dx12cmdlist);
	_cmdlist.pEvent = CreateEventExW(NULL, NULL, 0, EVENT_ALL_ACCESS);
	BLU64 _fence = _PrGpuMem->sCmdQueue.nCurrentFence++;
	_PrGpuMem->sCmdQueue.pCommandQueue->Signal(_PrGpuMem->sCmdQueue.pFence, _fence);
	_PrGpuMem->sCmdQueue.pFence->SetEventOnCompletion(_fence, _cmdlist.pEvent);
	BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nWrite - _PrGpuMem->sCmdQueue.sControl.nCurrent + _PrGpuMem->sCmdQueue.sControl.nSize;
	BLU32 _maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nWrite - (BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nWrite - _PrGpuMem->sCmdQueue.sControl.nCurrent) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nWrite - (BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent) >> 31));
	BLU32 _sizenosign = 1 & 0x7fffffff;
	BLU32 _test = _sizenosign - _maxsz;
	BLU32 _size = ((1) & (((BLS32)_test) >> 31)) | (_maxsz & ~(((BLS32)_test) >> 31));
	BLU32 _advance = _PrGpuMem->sCmdQueue.sControl.nCurrent + _size;
	BLU32 _current = _advance % _PrGpuMem->sCmdQueue.sControl.nSize;
	_PrGpuMem->sCmdQueue.sControl.nCurrent = _current;
	return _fence;
}
static void
_UpdateBuffer(_BLGenericBufferDX* _Buf, ID3D12GraphicsCommandList* _CommandList, BLU32 _Offset, BLU32 _Size, void* _Data)
{
	D3D12_RESOURCE_DESC _desc;
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	_desc.Alignment = 0;
	_desc.Width = _Size;
	_desc.Height = 1;
	_desc.DepthOrArraySize = 1;
	_desc.MipLevels = 1;
	_desc.Format = DXGI_FORMAT_UNKNOWN;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ID3D12Resource* _staging;
	D3D12_HEAP_PROPERTIES _hp = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_hp, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_ID3D12Resource, (void**)&_staging));
	BLU8* _data;
	DX_CHECK_INTERNAL(_staging->Map(0, NULL, (void**)&_data));
	memcpy(_data, _Data, _Size);
	_staging->Unmap(0, NULL);
	D3D12_RESOURCE_STATES _state = D3D12_RESOURCE_STATE_COPY_DEST;
	if (_Buf->eState != D3D12_RESOURCE_STATE_COPY_DEST)
	{
		_state = _Buf->eState;
		D3D12_RESOURCE_BARRIER _barrier;
		_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		_barrier.Transition.pResource = const_cast<ID3D12Resource*>(_Buf->pPtr);
		_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		_barrier.Transition.StateBefore = _Buf->eState;
		_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		_PrGpuMem->pCommandList->ResourceBarrier(1, &_barrier);
		_Buf->eState = D3D12_RESOURCE_STATE_COPY_DEST;
	}
	_CommandList->CopyBufferRegion(_Buf->pPtr, _Offset, _staging, 0, _Size);
	if (_Buf->eState != _state)
	{
		D3D12_RESOURCE_BARRIER _barrier;
		_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		_barrier.Transition.pResource = (ID3D12Resource*)(_Buf->pPtr);
		_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		_barrier.Transition.StateBefore = _Buf->eState;
		_barrier.Transition.StateAfter = _state;
		_PrGpuMem->pCommandList->ResourceBarrier(1, &_barrier);
		_Buf->eState = _state;
	}
	blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _staging);
}
static void
_CreateBuffer(_BLGenericBufferDX* _Buf, BLU32 _Size, void* _Data, BLU16 _Flags, BLBool _Vertex, BLU32 _Stride)
{
	_Buf->nSize = _Size;
	_Buf->nFags = _Flags;
	BLBool _needuav = 0 != (_Flags & (BUFFER_COMPUTE_WRITE_INTERNAL | BUFFER_DRAW_INDIRECT_INTERNAL));
	BLBool _drawindirect = 0 != (_Flags & BUFFER_DRAW_INDIRECT_INTERNAL);
	_Buf->bDynamic = (NULL == _Data || _needuav);
	DXGI_FORMAT _format;
	BLU32 _stride;
	BLU32 _flags = _needuav	? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;
	if (_drawindirect)
	{
		_format = DXGI_FORMAT_R32G32B32A32_UINT;
		_stride = 16;
	}
	else
	{
		BLU32 _uavformat = (_Flags & BUFFER_COMPUTE_FORMAT_MASK_INTERNAL) >> BUFFER_COMPUTE_FORMAT_SHIFT_INTERNAL;
		if (0 == _uavformat)
		{
			if (_Vertex)
			{
				_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				_stride = 16;
			}
			else
			{
				if (0 == (_Flags & BUFFER_INDEX32_INTERNAL))
				{
					_format = DXGI_FORMAT_R16_UINT;
					_stride = 2;
				}
				else
				{
					_format = DXGI_FORMAT_R32_UINT;
					_stride = 4;
				}
			}
		}
		else
		{
			BLU32 _sub = (((_Flags & BUFFER_COMPUTE_TYPE_MASK_INTERNAL) >> BUFFER_COMPUTE_TYPE_SHIFT_INTERNAL) - 1);
			BLU32 _le = -(_sub <= (BLU32)((_Flags & BUFFER_COMPUTE_TYPE_MASK_INTERNAL) >> BUFFER_COMPUTE_TYPE_SHIFT_INTERNAL));
			BLU32 _uavtype = _sub & _le;
			const DXGI_FORMAT _auavformat[][3] =
			{
				DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R8_SINT,           DXGI_FORMAT_R8_UINT,            DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R8G8_SINT,         DXGI_FORMAT_R8G8_UINT,          DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R8G8B8A8_SINT,     DXGI_FORMAT_R8G8B8A8_UINT,      DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R16_SINT,          DXGI_FORMAT_R16_UINT,           DXGI_FORMAT_R16_FLOAT ,
				DXGI_FORMAT_R16G16_SINT,       DXGI_FORMAT_R16G16_UINT,        DXGI_FORMAT_R16G16_FLOAT,
				DXGI_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_UINT,  DXGI_FORMAT_R16G16B16A16_FLOAT,
				DXGI_FORMAT_R32_SINT,          DXGI_FORMAT_R32_UINT,           DXGI_FORMAT_R32_FLOAT,
				DXGI_FORMAT_R32G32_SINT,       DXGI_FORMAT_R32G32_UINT,        DXGI_FORMAT_R32G32_FLOAT,
				DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_UINT,  DXGI_FORMAT_R32G32B32A32_FLOAT
			};
			const BLU32 _auavstride[] =	{ 0, 1,	2, 4, 2, 4, 8, 4, 8, 16 };
			_format = _auavformat[_uavformat][_uavtype];
			_stride = _auavstride[_uavformat];
		}
	}
	_stride = (0 == _Stride ? _stride : _Stride);
	_Buf->sSrvd.Format = _format;
	_Buf->sSrvd.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	_Buf->sSrvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	_Buf->sSrvd.Buffer.FirstElement = 0;
	_Buf->sSrvd.Buffer.NumElements = _Buf->nSize / _stride;
	_Buf->sSrvd.Buffer.StructureByteStride = 0;
	_Buf->sSrvd.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	_Buf->sUavd.Format = _format;
	_Buf->sUavd.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	_Buf->sUavd.Buffer.FirstElement = 0;
	_Buf->sUavd.Buffer.NumElements = _Buf->nSize / _stride;
	_Buf->sUavd.Buffer.StructureByteStride = 0;
	_Buf->sUavd.Buffer.CounterOffsetInBytes = 0;
	_Buf->sUavd.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	D3D12_RESOURCE_DESC _desc;
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	_desc.Alignment = 0;
	_desc.Width = _Size;
	_desc.Height = 1;
	_desc.DepthOrArraySize = 1;
	_desc.MipLevels = 1;
	_desc.Format = DXGI_FORMAT_UNKNOWN;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	_desc.Flags = D3D12_RESOURCE_FLAGS(_flags);
	D3D12_HEAP_PROPERTIES _hp = { D3D12_HEAP_TYPE_DEFAULT,  D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_hp, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_ID3D12Resource, (void**)&_Buf->pPtr));
	_Buf->nGpuVA = _Buf->pPtr->GetGPUVirtualAddress();
	if (_Buf->eState != (_drawindirect ? D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT : D3D12_RESOURCE_STATE_GENERIC_READ))
	{
		D3D12_RESOURCE_BARRIER _barrier;
		_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		_barrier.Transition.pResource = (ID3D12Resource*)(_Buf->pPtr);
		_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		_barrier.Transition.StateBefore = _Buf->eState;
		_barrier.Transition.StateAfter = (_drawindirect ? D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT : D3D12_RESOURCE_STATE_GENERIC_READ);
		_PrGpuMem->pCommandList->ResourceBarrier(1, &_barrier);
		_Buf->eState = (_drawindirect ? D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT : D3D12_RESOURCE_STATE_GENERIC_READ);
	}
	if (!_Buf->bDynamic)
		_UpdateBuffer(_Buf, _PrGpuMem->pCommandList, 0, _Size, _Data);
}

static void
_DestroyBuffer(_BLGenericBufferDX* _Buf)
{
	if (_Buf->pPtr)
	{
		blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _Buf->pPtr);
		_Buf->bDynamic = FALSE;
	}
}

void
_GpuIntervention(Windows::UI::Core::CoreWindow^ _Hwnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	BLBool _gpuok = FALSE;
	BLU32 _idx = 0;
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.eApiType = BL_DX_API;
	_PrGpuMem->sHardwareCaps.nFuncSupported = CAP_COMPUTE_SHADER_INTERNAL;
	_PrGpuMem->sCmdQueue.nCurrentFence = 0;
	_PrGpuMem->sCmdQueue.nCompletedFence = 0;
	_PrGpuMem->sCmdQueue.sControl.nCurrent = 0;
	_PrGpuMem->sCmdQueue.sControl.nRead = 0;
	_PrGpuMem->sCmdQueue.sControl.nWrite = 0;
	_PrGpuMem->sCmdQueue.sControl.nSize = 256;
	for (_idx = 0; _idx < 256 ; ++_idx)
		_PrGpuMem->sCmdQueue.pRelease[_idx] = blGenArray(FALSE);
	for (_idx = 0; _idx < 128; ++_idx)
	{
		_PrGpuMem->aFrameBuffer[_idx].pSwapChain = NULL;
		_PrGpuMem->aFrameBuffer[_idx].nWidth = 0;
		_PrGpuMem->aFrameBuffer[_idx].nHeight = 0;
		_PrGpuMem->aFrameBuffer[_idx].nDenseIdx = 0xFFFF;
		_PrGpuMem->aFrameBuffer[_idx].nNum = 0;
		_PrGpuMem->aFrameBuffer[_idx].nNumTh = 0;
		for (BLU32 _k = 0; _k < 8 ; ++_k)
			_PrGpuMem->aFrameBuffer[_idx].aTexture[_k] = 0xFFFF;
	}
	_PrGpuMem->sSamplerBuffer.nDescriptorsPerBlock = 1;
	_PrGpuMem->sBatchBuffer.nCurrIndirect = 0;
	_PrGpuMem->sBatchBuffer.nMaxDrawPerBatch = 0;
	_PrGpuMem->sBatchBuffer.nMinIndirect = 0;
	_PrGpuMem->sBatchBuffer.nFlushPerBatch = 0;
	memset(_PrGpuMem->sBatchBuffer.aNum, 0, sizeof(_PrGpuMem->sBatchBuffer.aNum));
	for (_idx = 0; _idx < 32 ; ++_idx)
	{
		_PrGpuMem->sBatchBuffer.aIndirect[_idx].nDecl = -1;
		_PrGpuMem->sBatchBuffer.aIndirect[_idx].pPtr = NULL;
		_PrGpuMem->sBatchBuffer.aIndirect[_idx].eState = D3D12_RESOURCE_STATE_COMMON;
		_PrGpuMem->sBatchBuffer.aIndirect[_idx].nSize = 0;
		_PrGpuMem->sBatchBuffer.aIndirect[_idx].nFags = 0;
		_PrGpuMem->sBatchBuffer.aIndirect[_idx].bDynamic = FALSE;
	}
	for (_idx = 0; _idx < 512; ++_idx)
	{
		_PrGpuMem->aShaderBuffers[_idx].pCode = NULL;
		_PrGpuMem->aShaderBuffers[_idx].sConstBuffer = NULL;
		_PrGpuMem->aShaderBuffers[_idx].nHash = 0;
		_PrGpuMem->aShaderBuffers[_idx].nUniforms = 0;
		_PrGpuMem->aShaderBuffers[_idx].nPredefined = 0;
	}
	for (_idx = 0; _idx < 4096; ++_idx)
	{
		_PrGpuMem->aVertexBuffers[_idx].nDecl = -1;
		_PrGpuMem->aVertexBuffers[_idx].pPtr = NULL;
		_PrGpuMem->aVertexBuffers[_idx].eState = D3D12_RESOURCE_STATE_COMMON;
		_PrGpuMem->aVertexBuffers[_idx].nSize = 0;
		_PrGpuMem->aVertexBuffers[_idx].nFags = 0;
		_PrGpuMem->aVertexBuffers[_idx].bDynamic = FALSE;
		_PrGpuMem->aIndexBuffers[_idx].nDecl = -1;
		_PrGpuMem->aIndexBuffers[_idx].pPtr = NULL;
		_PrGpuMem->aIndexBuffers[_idx].eState = D3D12_RESOURCE_STATE_COMMON;
		_PrGpuMem->aIndexBuffers[_idx].nSize = 0;
		_PrGpuMem->aIndexBuffers[_idx].nFags = 0;
		_PrGpuMem->aIndexBuffers[_idx].bDynamic = FALSE;
	}
	_PrGpuMem->pTextureDict = blGenDict(FALSE);
	_PrGpuMem->sUniformBuffer.pUniforms = blGenDict(FALSE);
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->pPipelineState = blGenDict(FALSE);
	DX_CHECK_INTERNAL(CreateDXGIFactory2(0, IID_PPV_ARGS(&(_PrGpuMem->pDxgiFactory))));
	IDXGIAdapter3* _tmpadapter;
	for (_idx = 0; DXGI_ERROR_NOT_FOUND != _PrGpuMem->pDxgiFactory->EnumAdapters(_idx, (IDXGIAdapter**)(&_tmpadapter)); ++_idx)
	{
		DXGI_ADAPTER_DESC2 _desc;
		DX_CHECK_INTERNAL(_tmpadapter->GetDesc2(&_desc));
		if (_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		else
		{
			_PrGpuMem->pAdapter = _tmpadapter;
			_tmpadapter = NULL;
			break;
		}
	}
#if defined(_DEBUG) && !defined(BL_PLATFORM_UWP)
	ID3D12Debug* _debug;
	DX_CHECK_INTERNAL(D3D12GetDebugInterface(IID_ID3D12Debug, (void**)&_debug));
	_debug->EnableDebugLayer();
#endif
	D3D_FEATURE_LEVEL _featurelevel[4] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	for (_idx = 0; _idx < 4; ++_idx)
	{
		if (SUCCEEDED(D3D12CreateDevice(_PrGpuMem->pAdapter, _featurelevel[_idx], IID_ID3D12Device, (void**)&_PrGpuMem->pDevice)))
		{
			_gpuok = TRUE;
			break;
		}
	}
	if (!_gpuok)
	{
		DX_CHECK_INTERNAL(_PrGpuMem->pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&_PrGpuMem->pAdapter)));
		if (_PrGpuMem->pAdapter)
			if (SUCCEEDED(D3D12CreateDevice(_PrGpuMem->pAdapter, D3D_FEATURE_LEVEL_11_0, IID_ID3D12Device, (void**)&_PrGpuMem->pDevice)))
				_gpuok = TRUE;
		if (!_gpuok)
		{
			blDebugOutput("bulllord needs directx feature level 11.0 or above");
			return;
		}
	}	
	for (_idx = 0; DXGI_ERROR_NOT_FOUND != _PrGpuMem->pDxgiFactory->EnumAdapters(_idx, (IDXGIAdapter**)(&_tmpadapter)); ++_idx)
	{
		DXGI_ADAPTER_DESC2 _dsec;
		_tmpadapter->GetDesc2(&_dsec);
		if (_dsec.AdapterLuid.LowPart == _PrGpuMem->pDevice->GetAdapterLuid().LowPart  &&  _dsec.AdapterLuid.HighPart == _PrGpuMem->pDevice->GetAdapterLuid().HighPart)
		{
			_PrGpuMem->pAdapter = _tmpadapter;
			_tmpadapter = NULL;
			break;
		}
	}
	D3D12_COMMAND_QUEUE_DESC _queuedesc;
	_queuedesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	_queuedesc.Priority = 0;
	_queuedesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	_queuedesc.NodeMask = 1;
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandQueue(&_queuedesc, IID_ID3D12CommandQueue, (void**)&_PrGpuMem->sCmdQueue.pCommandQueue));
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, (void**)&_PrGpuMem->sCmdQueue.pFence));
	for (_idx = 0; _idx < 256; ++_idx)
	{
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_ID3D12CommandAllocator, (void**)&_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandAllocator));
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandAllocator, NULL, IID_ID3D12GraphicsCommandList, (void**)&_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandList));
		DX_CHECK_INTERNAL(_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandList->Close());
	}
	DXGI_SWAP_CHAIN_DESC1 _scd;
	_scd.Width = _Width;
	_scd.Height = _Height;
	_scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_scd.Stereo = FALSE;
	_scd.SampleDesc.Count = 1;
	_scd.SampleDesc.Quality = 0;
	_scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_scd.BufferCount = 3;
	_scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	_scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
	_scd.Scaling = DXGI_SCALING_STRETCH;
#else
	_scd.Scaling = DXGI_SCALING_NONE;
#endif
	_scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	DX_CHECK_INTERNAL(_PrGpuMem->pDxgiFactory->CreateSwapChainForCoreWindow(_PrGpuMem->sCmdQueue.pCommandQueue, (IUnknown*)(_Hwnd), &_scd, NULL, &_PrGpuMem->pSwapChain));
#ifdef _DEBUG
	ID3D12InfoQueue* _infoqueue;
	if (SUCCEEDED(_PrGpuMem->pDevice->QueryInterface(IID_ID3D12InfoQueue, (void**)&_infoqueue)))
	{
		_infoqueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		_infoqueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		_infoqueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		D3D12_INFO_QUEUE_FILTER _infofilter;
		memset(&_infofilter, 0, sizeof(_infofilter));
		D3D12_MESSAGE_CATEGORY _catlist[] =	{ D3D12_MESSAGE_CATEGORY_STATE_CREATION, D3D12_MESSAGE_CATEGORY_EXECUTION, };
		_infofilter.DenyList.NumCategories = 2;
		_infofilter.DenyList.pCategoryList = _catlist;
		_infoqueue->PushStorageFilter(&_infofilter);
		_infoqueue->Release();
	}
#endif
	D3D12_DESCRIPTOR_HEAP_DESC _rtvdescheap;
	_rtvdescheap.NumDescriptors = 3 + 8*128;
	_rtvdescheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	_rtvdescheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	_rtvdescheap.NodeMask = 1;
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_rtvdescheap, IID_ID3D12DescriptorHeap, (void**)&_PrGpuMem->pRtvHeap));
	D3D12_DESCRIPTOR_HEAP_DESC _dsvdescheap;
	_dsvdescheap.NumDescriptors = 1	+ 128;
	_dsvdescheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	_dsvdescheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	_dsvdescheap.NodeMask = 1;
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_dsvdescheap, IID_ID3D12DescriptorHeap, (void**)&_PrGpuMem->pDsvHeap));
	for (_idx = 0; _idx < 3; ++_idx)
	{
		BLU32 _maxdesc = (4 << 10) + 128 + ((64 << 10) - 1);
		_PrGpuMem->aScratchBuffer[_idx].nIncrementSize = _PrGpuMem->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_DESCRIPTOR_HEAP_DESC _desc;
		_desc.NumDescriptors = _maxdesc;
		_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		_desc.NodeMask = 1;
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_desc, IID_ID3D12DescriptorHeap, (void**)&_PrGpuMem->aScratchBuffer[_idx].pHeap));
		D3D12_HEAP_PROPERTIES _hp = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
		D3D12_RESOURCE_DESC _resdesc;
		_resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		_resdesc.Alignment = 0;
		_resdesc.Width = _maxdesc * 1024;
		_resdesc.Height = 1;
		_resdesc.DepthOrArraySize = 1;
		_resdesc.MipLevels = 1;
		_resdesc.Format = DXGI_FORMAT_UNKNOWN;
		_resdesc.SampleDesc.Count = 1;
		_resdesc.SampleDesc.Quality = 0;
		_resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		_resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_hp, D3D12_HEAP_FLAG_NONE, &_resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_ID3D12Resource, (void**)&_PrGpuMem->aScratchBuffer[_idx].pUpload));
		_PrGpuMem->aScratchBuffer[_idx].nGpuVA = _PrGpuMem->aScratchBuffer[_idx].pUpload->GetGPUVirtualAddress();
		_PrGpuMem->aScratchBuffer[_idx].pUpload->Map(0, NULL, (void**)&_PrGpuMem->aScratchBuffer[_idx].pData);
		_PrGpuMem->aScratchBuffer[_idx].nPos = 0;
		_PrGpuMem->aScratchBuffer[_idx].sCpuHandle = _PrGpuMem->aScratchBuffer[_idx].pHeap->GetCPUDescriptorHandleForHeapStart();
		_PrGpuMem->aScratchBuffer[_idx].sGpuHandle = _PrGpuMem->aScratchBuffer[_idx].pHeap->GetGPUDescriptorHandleForHeapStart();
	}
	_PrGpuMem->sSamplerBuffer.nDescriptorsPerBlock = 16;
	_PrGpuMem->sSamplerBuffer.nIncrementSize = _PrGpuMem->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	D3D12_DESCRIPTOR_HEAP_DESC _samplerdesc;
	_samplerdesc.NumDescriptors = 1024;
	_samplerdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	_samplerdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	_samplerdesc.NodeMask = 1;
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_samplerdesc, IID_ID3D12DescriptorHeap, (void**)&_PrGpuMem->sSamplerBuffer.pHeap));
	_PrGpuMem->sSamplerBuffer.sCpuHandle = _PrGpuMem->sSamplerBuffer.pHeap->GetCPUDescriptorHandleForHeapStart();
	_PrGpuMem->sSamplerBuffer.sGpuHandle = _PrGpuMem->sSamplerBuffer.pHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_DESCRIPTOR_RANGE _descrange[] =
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,     16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,     1,                                0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV,     16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
	};
	D3D12_ROOT_PARAMETER _rootparam[] =
	{
		{ D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ { 1, &_descrange[0] } }, D3D12_SHADER_VISIBILITY_ALL },
		{ D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ { 1, &_descrange[1] } }, D3D12_SHADER_VISIBILITY_ALL },
		{ D3D12_ROOT_PARAMETER_TYPE_CBV,{ { 0, 0 } }, D3D12_SHADER_VISIBILITY_ALL },
		{ D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ { 1, &_descrange[3] } }, D3D12_SHADER_VISIBILITY_ALL },
	};
	_rootparam[2].Descriptor.RegisterSpace = 0;
	_rootparam[2].Descriptor.ShaderRegister = 0;
	D3D12_ROOT_SIGNATURE_DESC _signature;
	_signature.NumParameters = 4;
	_signature.pParameters = _rootparam;
	_signature.NumStaticSamplers = 0;
	_signature.pStaticSamplers = NULL;
	_signature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ID3DBlob* _out;
	ID3DBlob* _error;
	DX_CHECK_INTERNAL(D3D12SerializeRootSignature(&_signature, D3D_ROOT_SIGNATURE_VERSION_1, &_out, &_error));
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateRootSignature(0, _out->GetBufferPointer(), _out->GetBufferSize(), IID_ID3D12RootSignature, (void**)&_PrGpuMem->pRootSignature));
	for (_idx = 0; _idx < UNIFORM_COUNT_INTERNAL; ++_idx)
	{
		struct _BLUniformBuffer::_UniformInfo* _ele = (struct _BLUniformBuffer::_UniformInfo*)malloc(sizeof(struct _BLUniformBuffer::_UniformInfo));
		_ele->eType = _idx;
		_ele->nHandle = -1;
		blDictInsert(_PrGpuMem->sUniformBuffer.pUniforms, _idx, _ele);
	}
	_PrGpuMem->sHardwareCaps.nMaxTextureSize = 16384;
	_PrGpuMem->sHardwareCaps.nMaxFBAttachments = 8;
	DXGI_FORMAT _orifmt[] = {
		DXGI_FORMAT_BC1_UNORM, DXGI_FORMAT_BC3_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_SINT, DXGI_FORMAT_R8_UINT, DXGI_FORMAT_R8_SNORM, DXGI_FORMAT_R16_UNORM,
		DXGI_FORMAT_R16_SINT, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16_SNORM, DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_R8G8_SINT, DXGI_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8_SNORM, DXGI_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16_SINT, DXGI_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16_FLOAT,
		DXGI_FORMAT_R16G16_SNORM, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R10G10B10A2_UNORM,
		DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS,
		DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS
	};
	DXGI_FORMAT _srvfmt[] = {
		DXGI_FORMAT_BC1_UNORM, DXGI_FORMAT_BC3_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_SINT, DXGI_FORMAT_R8_UINT, DXGI_FORMAT_R8_SNORM, DXGI_FORMAT_R16_UNORM,
		DXGI_FORMAT_R16_SINT, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16_SNORM, DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_R8G8_SINT, DXGI_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8_SNORM, DXGI_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16_SINT, DXGI_FORMAT_R16G16_UINT,
		DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16_SNORM, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_SINT,
		DXGI_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_R11G11B10_FLOAT,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	};
	DXGI_FORMAT _dsvfmt[] = {
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D32_FLOAT,
		DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT
	};
	DXGI_FORMAT _srgbfmt[] = {
		DXGI_FORMAT_BC1_UNORM_SRGB, DXGI_FORMAT_BC3_UNORM_SRGB, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN
	};
	for (_idx = 0; _idx < BL_TF_COUNT; ++_idx)
	{
		BLU16 _support = 0;
		DXGI_FORMAT _fmt = (_idx > BL_TF_DEPTH_UNKNOWN && _idx < BL_TF_COUNT) ? _dsvfmt[_idx] : _orifmt[_idx];
		DXGI_FORMAT _sfmt = _srgbfmt[_idx];
		if (DXGI_FORMAT_UNKNOWN != _fmt)
		{
			D3D12_FEATURE_DATA_FORMAT_SUPPORT _data;
			_data.Format = _fmt;
			if (SUCCEEDED(_PrGpuMem->pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &_data, sizeof(_data))))
			{
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_TEXTURE2D)) ? BL_TS_2D : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_TEXTURE3D)) ? BL_TS_3D : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_TEXTURECUBE)) ? BL_TS_CUBE : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_BUFFER | D3D12_FORMAT_SUPPORT1_IA_VERTEX_BUFFER | D3D12_FORMAT_SUPPORT1_IA_INDEX_BUFFER)) ? BL_TS_VERTEX : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_SHADER_LOAD)) ? BL_TS_IMAGE : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_RENDER_TARGET | D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)) ? BL_TS_FRAMEBUFFER : BL_TS_NONE;
				if (0 != (_support & BL_TS_IMAGE))
				{
					_support &= ~BL_TS_IMAGE;
					_data.Format = _orifmt[_idx];
					DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &_data, sizeof(_data)));
					_support |= 0 != (_data.Support2 & (0 | D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD | D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE)) ? BL_TS_IMAGE : BL_TS_NONE;
				}
			}
		}
		if (DXGI_FORMAT_UNKNOWN != _sfmt)
		{
			D3D12_FEATURE_DATA_FORMAT_SUPPORT _data;
			_data.Format = _sfmt;
			if (SUCCEEDED(_PrGpuMem->pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &_data, sizeof(_data))))
			{
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_TEXTURE2D)) ? BL_TS_2D_SRGB : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_TEXTURE3D)) ? BL_TS_3D_SRGB : BL_TS_NONE;
				_support |= 0 != (_data.Support1 & (0 | D3D12_FORMAT_SUPPORT1_TEXTURECUBE)) ? BL_TS_CUBE_SRGB : BL_TS_NONE;
			}
		}
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = _support;
	}
	memset(_PrGpuMem->aBackBufferFence, 0, sizeof(_PrGpuMem->aBackBufferFence));
	BLU32 _rtvdesc = _PrGpuMem->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (_idx = 0; _idx < _scd.BufferCount; ++_idx)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE _handle = _PrGpuMem->pRtvHeap->GetCPUDescriptorHandleForHeapStart();
		_handle.ptr += _idx * _rtvdesc;
		DX_CHECK_INTERNAL(_PrGpuMem->pSwapChain->GetBuffer(_idx, IID_ID3D12Resource, (void**)&_PrGpuMem->aBackColor[_idx]));
		_PrGpuMem->pDevice->CreateRenderTargetView(_PrGpuMem->aBackColor[_idx], NULL, _handle);
	}
	D3D12_RESOURCE_DESC _backdsdesc;
	_backdsdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_backdsdesc.Alignment = 0;
	_backdsdesc.Width = _Width;
	_backdsdesc.Height = _Height;
	_backdsdesc.DepthOrArraySize = 1;
	_backdsdesc.MipLevels = 0;
	_backdsdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	_backdsdesc.SampleDesc.Count = 1;
	_backdsdesc.SampleDesc.Quality = 0;
	_backdsdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	_backdsdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE _clearclr;
	_clearclr.Format = _backdsdesc.Format;
	_clearclr.DepthStencil.Depth = 1.0f;
	_clearclr.DepthStencil.Stencil = 0;
	D3D12_HEAP_PROPERTIES _dsvhp = { D3D12_HEAP_TYPE_DEFAULT,  D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_dsvhp, D3D12_HEAP_FLAG_NONE, &_backdsdesc, D3D12_RESOURCE_STATE_COMMON, &_clearclr, IID_ID3D12Resource, (void**)&_PrGpuMem->pDepthStencil));
	D3D12_DEPTH_STENCIL_VIEW_DESC _dsvdesc;
	memset(&_dsvdesc, 0, sizeof(_dsvdesc));
	_dsvdesc.Format = _backdsdesc.Format;
	_dsvdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	_dsvdesc.Flags = D3D12_DSV_FLAGS(0);
	_PrGpuMem->pDevice->CreateDepthStencilView(_PrGpuMem->pDepthStencil, &_dsvdesc, _PrGpuMem->pDsvHeap->GetCPUDescriptorHandleForHeapStart());
	_PrGpuMem->pCommandList = _AllocCmdList();
	_PrGpuMem->sBatchBuffer.nMaxDrawPerBatch = 4 << 10;
	_PrGpuMem->sBatchBuffer.nFlushPerBatch = 4 << 10;
	_PrGpuMem->sBatchBuffer.nMinIndirect = 64;
	D3D12_INDIRECT_ARGUMENT_DESC _drawvertexarg[] =
	{
		{ D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW,{ { 2 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,{ { 0 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,{ { 1 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_DRAW,{ { 0 } } }
	};
	D3D12_COMMAND_SIGNATURE_DESC _drawvertexcmdsig ={ sizeof(_BLBatchBufferDX::_DrawVertexIndirectCommand), 4,_drawvertexarg, 1};
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandSignature(&_drawvertexcmdsig, _PrGpuMem->pRootSignature, IID_ID3D12CommandSignature, (void**)&_PrGpuMem->sBatchBuffer.aCommandSignature[0]));
	D3D12_INDIRECT_ARGUMENT_DESC _drawindexedarg[] =
	{
		{ D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW,{ { 2 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,{ { 0 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,{ { 1 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW,{ { 0 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED,{ { 0 } } },
	};
	D3D12_COMMAND_SIGNATURE_DESC _drawindexedcmdsig ={ sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand), 5, _drawindexedarg, 1};
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandSignature(&_drawindexedcmdsig, _PrGpuMem->pRootSignature, IID_ID3D12CommandSignature, (void**)&_PrGpuMem->sBatchBuffer.aCommandSignature[1]));
	_PrGpuMem->sBatchBuffer.nCmds[0] = malloc((4 << 10) * sizeof(_BLBatchBufferDX::_DrawVertexIndirectCommand));
	_PrGpuMem->sBatchBuffer.nCmds[1] = malloc((4 << 10) * sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand));
	for (_idx = 0; _idx < 32; ++_idx)
		_CreateBuffer(&_PrGpuMem->sBatchBuffer.aIndirect[_idx], (4 << 10) * sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand), NULL, BUFFER_DRAW_INDIRECT_INTERNAL, FALSE, sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand));
}
void
_GpuSwapBuffer(BLBool _Discard)
{
	if (_Discard)
	{
	}
	else
	{

		if (_PrGpuMem->pSwapChain)
		{
			HRESULT _hr = 0;
			BLF32 _start = blSystemSecTime();
			for (BLU32 _idx = 0; _idx < 128; ++_idx)
			{
				if (_PrGpuMem->aFrameBuffer[_idx].pSwapChain)
					_hr = _PrGpuMem->aFrameBuffer[_idx].pSwapChain->Present(_PrGpuMem->bVsync ? 1 : 0, 0);
			}
			if (SUCCEEDED(_hr))
			{
				do
				{
					BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
					BLU32 _maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
					if (_maxsz)
					{
						_BLCommandQueueDX::_CommandList& _cmdlist = _PrGpuMem->sCmdQueue.aCommandList[_PrGpuMem->sCmdQueue.sControl.nRead];
						if (WAIT_OBJECT_0 == WaitForSingleObject(_cmdlist.pEvent, INFINITE))
						{
							CloseHandle(_cmdlist.pEvent);
							_cmdlist.pEvent = NULL;
							_PrGpuMem->sCmdQueue.nCompletedFence = _PrGpuMem->sCmdQueue.pFence->GetCompletedValue();
							_PrGpuMem->sCmdQueue.pCommandQueue->Wait(_PrGpuMem->sCmdQueue.pFence, _PrGpuMem->sCmdQueue.nCompletedFence);
							FOREACH_ARRAY(ID3D12Resource*, _iter, _PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead])
							{
								_iter->Release();
							}
							blClearArray(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead]);
							BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
							_maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
							BLU32 _sizenosign = 1 & 0x7fffffff;
							BLU32 _test = _sizenosign - _maxsz;
							BLU32 _size = ((1) & (((BLS32)_test) >> 31)) | (_maxsz & ~(((BLS32)_test) >> 31));
							BLU32 _advance = _PrGpuMem->sCmdQueue.sControl.nRead + _size;
							BLU32 _read = _advance % _PrGpuMem->sCmdQueue.sControl.nSize;
							_PrGpuMem->sCmdQueue.sControl.nRead = _read;
						}
					}
					if (_PrGpuMem->aBackBufferFence[(_PrGpuMem->nBackBufferIdx - 1) % 3U] <= _PrGpuMem->sCmdQueue.nCompletedFence)
						return;
				} while (1);
				_hr = _PrGpuMem->pSwapChain->Present(_PrGpuMem->bVsync ? 1 : 0, 0);
			}
			_PrGpuMem->fPresentElapsed = blSystemSecTime() - _start;
			if ((_hr == DXGI_ERROR_DEVICE_REMOVED) || (_hr == DXGI_ERROR_DEVICE_HUNG) || (_hr == DXGI_ERROR_DEVICE_RESET) || (_hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR) || (_hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE))
				blDebugOutput("Directx Device Lost");
		}
	}
}
void
_GpuAnitIntervention()
{
	BLU32 _idx = 0, _num;
	free(_PrGpuMem->sBatchBuffer.nCmds[0]);
	free(_PrGpuMem->sBatchBuffer.nCmds[1]);
	_PrGpuMem->sBatchBuffer.aCommandSignature[0]->Release();
	_PrGpuMem->sBatchBuffer.aCommandSignature[1]->Release();
	for (_idx = 0; _idx < 32; ++_idx)
	{
		if (NULL != _PrGpuMem->sBatchBuffer.aIndirect[_idx].pPtr)
		{
			blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _PrGpuMem->sBatchBuffer.aIndirect[_idx].pPtr);
			_PrGpuMem->sBatchBuffer.aIndirect[_idx].bDynamic = FALSE;
		}
	}
	_KickCmdList();
	do
	{
		BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
		BLU32 _maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
		if (_maxsz)
		{
			_BLCommandQueueDX::_CommandList& _cmdlist = _PrGpuMem->sCmdQueue.aCommandList[_PrGpuMem->sCmdQueue.sControl.nRead];
			if (WAIT_OBJECT_0 == WaitForSingleObject(_cmdlist.pEvent, INFINITE))
			{
				CloseHandle(_cmdlist.pEvent);
				_cmdlist.pEvent = NULL;
				_PrGpuMem->sCmdQueue.nCompletedFence = _PrGpuMem->sCmdQueue.pFence->GetCompletedValue();
				_PrGpuMem->sCmdQueue.pCommandQueue->Wait(_PrGpuMem->sCmdQueue.pFence, _PrGpuMem->sCmdQueue.nCompletedFence);
				FOREACH_ARRAY(ID3D12Resource*, _iter, _PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead])
				{
					_iter->Release();
				}
				blClearArray(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead]);
				BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
				_maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
				BLU32 _sizenosign = 1 & 0x7fffffff;
				BLU32 _test = _sizenosign - _maxsz;
				BLU32 _size = ((1) & (((BLS32)_test) >> 31)) | (_maxsz & ~(((BLS32)_test) >> 31));
				BLU32 _advance = _PrGpuMem->sCmdQueue.sControl.nRead + _size;
				BLU32 _read = _advance % _PrGpuMem->sCmdQueue.sControl.nSize;
				_PrGpuMem->sCmdQueue.sControl.nRead = _read;
			}
		}
		else
			break;
	} while (1);
	for (_idx = 0, _num = 3U; _idx < _num; ++_idx)
		_PrGpuMem->aBackColor[_idx]->Release();
	_PrGpuMem->pDepthStencil->Release();
	_PrGpuMem->sSamplerBuffer.pHeap->Release();
	for (_idx = 0; _idx < 3U; ++_idx)
	{
		_PrGpuMem->aScratchBuffer[_idx].pUpload->Unmap(0, NULL);
		_PrGpuMem->aScratchBuffer[_idx].pUpload->Release();
		_PrGpuMem->aScratchBuffer[_idx].pHeap->Release();
	}
	{
		FOREACH_DICT(ID3D12PipelineState*, _iter, _PrGpuMem->pPipelineState)
		{
			_iter->AddRef();
			BLU32 _ref = _iter->Release();
			assert(_ref == 1);
			_iter->Release();
		}
	}
	blDeleteDict(_PrGpuMem->pPipelineState);
	for (_idx = 0; _idx < 512; ++_idx)
	{
		if (NULL != _PrGpuMem->aShaderBuffers[_idx].sConstBuffer)
			free(_PrGpuMem->aShaderBuffers[_idx].sConstBuffer);
		_PrGpuMem->aShaderBuffers[_idx].nPredefined = 0;
		if (_PrGpuMem->aShaderBuffers[_idx].pCode)
		{
			_PrGpuMem->aShaderBuffers[_idx].pCode = NULL;
			_PrGpuMem->aShaderBuffers[_idx].nHash = 0;
		}
	}
	for (_idx = 0; _idx < 4096; ++_idx)
	{
		if (NULL != _PrGpuMem->aIndexBuffers[_idx].pPtr)
		{
			blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _PrGpuMem->aIndexBuffers[_idx].pPtr);
			_PrGpuMem->aIndexBuffers[_idx].bDynamic = FALSE;
		}
		if (NULL != _PrGpuMem->aVertexBuffers[_idx].pPtr)
		{
			blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _PrGpuMem->aVertexBuffers[_idx].pPtr);
			_PrGpuMem->aVertexBuffers[_idx].bDynamic = FALSE;
		}
	}
	{
		FOREACH_DICT(_BLTextureBufferDX*, _iter, _PrGpuMem->pTextureDict)
		{
			if (NULL != _iter->pPtr)
				blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _iter->pPtr);
		}
	}
	_PrGpuMem->pRtvHeap->Release();
	_PrGpuMem->pDsvHeap->Release();
	_PrGpuMem->pRootSignature->Release();
	_PrGpuMem->pSwapChain->Release();
	do
	{
		BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
		BLU32 _maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
		if (_maxsz)
		{
			_BLCommandQueueDX::_CommandList& _cmdlist = _PrGpuMem->sCmdQueue.aCommandList[_PrGpuMem->sCmdQueue.sControl.nRead];
			if (WAIT_OBJECT_0 == WaitForSingleObject(_cmdlist.pEvent, INFINITE))
			{
				CloseHandle(_cmdlist.pEvent);
				_cmdlist.pEvent = NULL;
				_PrGpuMem->sCmdQueue.nCompletedFence = _PrGpuMem->sCmdQueue.pFence->GetCompletedValue();
				_PrGpuMem->sCmdQueue.pCommandQueue->Wait(_PrGpuMem->sCmdQueue.pFence, _PrGpuMem->sCmdQueue.nCompletedFence);
				{
					FOREACH_ARRAY(ID3D12Resource*, _iter, _PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead])
					{
						_iter->Release();
					}
				}
				blClearArray(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nRead]);
				BLU32 _le = _PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead + _PrGpuMem->sCmdQueue.sControl.nSize;
				_maxsz = ((_le) & (((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31)) | ((_PrGpuMem->sCmdQueue.sControl.nCurrent - _PrGpuMem->sCmdQueue.sControl.nRead) & ~(((BLS32)_PrGpuMem->sCmdQueue.sControl.nCurrent - (BLS32)_PrGpuMem->sCmdQueue.sControl.nRead) >> 31));
				BLU32 _sizenosign = 1 & 0x7fffffff;
				BLU32 _test = _sizenosign - _maxsz;
				BLU32 _size = ((1) & (((BLS32)_test) >> 31)) | (_maxsz & ~(((BLS32)_test) >> 31));
				BLU32 _advance = _PrGpuMem->sCmdQueue.sControl.nRead + _size;
				BLU32 _read = _advance % _PrGpuMem->sCmdQueue.sControl.nSize;
				_PrGpuMem->sCmdQueue.sControl.nRead = _read;
			}
		}
		else
			break;
	} while (1);
	_PrGpuMem->sCmdQueue.pFence->Release();
	for (_idx = 0; _idx < 256; ++_idx)
	{
		_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandAllocator->Release();
		_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandList->Release();
	}
	_PrGpuMem->sCmdQueue.pCommandQueue->Release();
	_PrGpuMem->pDevice->Release();
	_PrGpuMem->pAdapter->Release();
	_PrGpuMem->pDxgiFactory->Release();
	{
		FOREACH_DICT(struct _BLUniformBuffer::_UniformInfo*, _iter, _PrGpuMem->sUniformBuffer.pUniforms)
		{
			free(_iter);
		}
	}
	blDeleteDict(_PrGpuMem->pTextureDict);
	blDeleteDict(_PrGpuMem->sUniformBuffer.pUniforms);
	for (_idx = 0; _idx < 256; ++_idx)
		blDeleteArray(_PrGpuMem->sCmdQueue.pRelease[_idx]);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_LINUX)
static BLS32
_CtxErrorHandler(Display*, XErrorEvent*)
{
    _PrGpuMem->bCtxError = TRUE;
    return 0;
}
void
_GpuIntervention(Display* _Display, Window _Window, GLXFBConfig _Config, void* _Lib, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.nFuncSupported = 0;
	_PrGpuMem->bCtxError = FALSE;
	BLBool _vkinited = FALSE;
	BLBool _gpuok = FALSE;
	if (!_vkinited)
	{
		BLS32 _glversion;
		_PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
		GL_LOAD_INTERNAL(_Lib);
        glxSwapBuffersARB = (PFNGLXSWAPBUFFERSPROC)glXGetProcAddress((const GLubyte*)"glxSwapBuffersARB");
        PFNGLXMAKECONTEXTCURRENTPROC glXMakeContextCurrent = (PFNGLXMAKECONTEXTCURRENTPROC)glXGetProcAddress((const GLubyte*)"glXMakeContextCurrent");
        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
        PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
        PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
		BLS32 _versions[5][2] =
		{
			{ 4 , 5 },
			{ 4 , 4 },
			{ 4 , 3 },
			{ 4 , 2 },
			{ 4 , 1 }
		};
		BLS32 _attribus[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
			GLX_CONTEXT_MINOR_VERSION_ARB, 5,
			None
		};
        BLS32(*_err)(Display*, XErrorEvent*) = XSetErrorHandler(&_CtxErrorHandler);
		for (BLS32 _idx = 0; _idx < 5 ; ++_idx)
		{
			_attribus[1] = _versions[_idx][0];
			_attribus[3] = _versions[_idx][1];
			_PrGpuMem->pContext = glXCreateContextAttribsARB(_Display, _Config, NULL, true, _attribus);
			XSync(_Display, False);
            if (!_PrGpuMem->bCtxError && _PrGpuMem->pContext)
			{
				_gpuok = TRUE;
				blDebugOutput("Opengl %d.%d boost", _versions[_idx][0], _versions[_idx][1]);
				_glversion = _versions[_idx][0] * 10 + _versions[_idx][1];
				_gpuok = glXMakeContextCurrent(_Display, _Window, _Window, _PrGpuMem->pContext);
                if (glXSwapIntervalEXT)
                    glXSwapIntervalEXT(_Display , _Window , _Vsync ? 1 : 0);
                else if(glXSwapIntervalSGI)
                    glXSwapIntervalSGI(_Vsync ? 1: 0);
				break;
			}
		}
        XSetErrorHandler(_err);
    }
}
void
_GpuSwapBuffer(BLBool _Discard)
{
}
void
_GpuAnitIntervention()
{
}
#elif defined(BL_PLATFORM_ANDROID)
void
_GpuIntervention(ANativeWindow* _Wnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.nFuncSupported = 0;
	_PrGpuMem->pEglConfig = _PrGpuMem->pEglContext = _PrGpuMem->pEglDisplay = _PrGpuMem->pEglSurface = 0;
	BLBool _vkinited = FALSE;
	BLBool _gpuok = FALSE;
	if (_vkinited)
	{
		_PrGpuMem->sHardwareCaps.eApiType = BL_VULKAN_API;
	}
	else
	{
		_PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
		EGLint _configattrs[] =
		{
			EGL_SAMPLE_BUFFERS,     0,
			EGL_SAMPLES,            0,
			EGL_DEPTH_SIZE,         16,
			EGL_RED_SIZE,           8,
			EGL_GREEN_SIZE,         8,
			EGL_BLUE_SIZE,          8,
			EGL_ALPHA_SIZE,         8,
			EGL_STENCIL_SIZE,       8,
			EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE,	0x0040,
			EGL_NONE
		};
		EGLint _configcount;
		const EGLint _contextattrs[] = { 0x3098, 3, 0x30FB, 0, 0x30FC, 0, EGL_NONE };
		const EGLint _surfaceattrs[] = { EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL_NONE };
		if (_PrGpuMem->pEglDisplay == EGL_NO_DISPLAY && _PrGpuMem->pEglContext == EGL_NO_CONTEXT)
		{
			_PrGpuMem->pEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if (_PrGpuMem->pEglDisplay == EGL_NO_DISPLAY)
			{
				blDebugOutput("egl initialize failed");
				return;
			}
			if (eglInitialize(_PrGpuMem->pEglDisplay, NULL, NULL) != EGL_TRUE)
			{
				blDebugOutput("egl initialize failed");
				return;
			}
			if (eglChooseConfig(_PrGpuMem->pEglDisplay, _configattrs, &_PrGpuMem->pEglConfig, 1, &_configcount) != EGL_TRUE || _configcount <= 0)
			{
				blDebugOutput("egl no valid config");
				return;
			}
			_PrGpuMem->pEglContext = eglCreateContext(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglConfig, EGL_NO_CONTEXT, _contextattrs);
			if (_PrGpuMem->pEglContext == EGL_NO_CONTEXT)
			{
				blDebugOutput("egl no valid context");
				return;
			}
		}
		EGLint _format;
		eglGetConfigAttrib(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglConfig, EGL_NATIVE_VISUAL_ID, &_format);
		ANativeWindow_setBuffersGeometry(_Wnd, _Width, _Height, _format);
		_PrGpuMem->pEglSurface = eglCreateWindowSurface(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglConfig, _Wnd, _surfaceattrs);
		if (_PrGpuMem->pEglSurface == EGL_NO_SURFACE)
		{
			blDebugOutput("egl no valid surface");
			return;
		}
		if (eglMakeCurrent(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglSurface, _PrGpuMem->pEglSurface, _PrGpuMem->pEglContext) != EGL_TRUE)
		{
			blDebugOutput("egl can not make current");
			return;
		}
		eglSwapInterval(_PrGpuMem->pEglDisplay, _Vsync ? 1 : 0);
	}
}
void 
_GpuSwapBuffer(BLBool)
{
}
void 
_GpuAnitIntervention()
{
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		eglMakeCurrent(_PrGpuMem->pEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroySurface(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglSurface);
		eglDestroyContext(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglContext);
		eglTerminate(_PrGpuMem->pEglDisplay);
	}
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_OSX)
void
_GpuIntervention(NSView* _View, BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->fPresentElapsed = 0.f;
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->nCurFramebufferHandle = 0xFFFF;
    _PrGpuMem->nBackBufferIdx = 0;
    _PrGpuMem->sHardwareCaps.nFuncSupported = 0;
    _PrGpuMem->pGLC = nil;
    _PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
    if (BL_GL_API == _PrGpuMem->sHardwareCaps.eApiType)
    {
        CGDisplayCount _numdisplays;
        CGDirectDisplayID* _displays, _maindis;
        if (kCGErrorSuccess != CGGetOnlineDisplayList(0, NULL, &_numdisplays))
        {
            blDebugOutput("OSX display not found");
            return;
        }
        _displays = (CGDirectDisplayID*)alloca(sizeof(CGDirectDisplayID) * _numdisplays);
        if (kCGErrorSuccess != CGGetOnlineDisplayList(_numdisplays, _displays, &_numdisplays))
        {
            blDebugOutput("OSX display not found");
            return;
        }
        for (BLU32 _i = 0; _i < _numdisplays; ++_i)
        {
            if (CGDisplayIsMain(_displays[_i]))
            {
                _maindis = _displays[_i];
                break;
            }
        }
        CFURLRef _dir = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("/System/Library/Frameworks/OpenGL.framework"), kCFURLPOSIXPathStyle, TRUE);
        CFBundleRef _lib = CFBundleCreate(kCFAllocatorDefault, _dir);
        GL_LOAD_INTERNAL(_lib);
        CFRelease(_lib);
        CFRelease(_dir);
        NSOpenGLPixelFormatAttribute _attr[32] = {0};
        _attr[0] = NSOpenGLPFAOpenGLProfile;
        _attr[1] = NSOpenGLProfileVersion4_1Core;
        _attr[2] = NSOpenGLPFAColorSize;
        _attr[3] = 24;
        _attr[4] = NSOpenGLPFAAlphaSize;
        _attr[5] = 8;
        _attr[6] = NSOpenGLPFADepthSize;
        _attr[7] = 24;
        _attr[8] = NSOpenGLPFAStencilSize;
        _attr[9] = 8;
        _attr[10] = NSOpenGLPFADoubleBuffer;
        _attr[11] = 1;
        _attr[12] = NSOpenGLPFAAccelerated;
        _attr[13] = 1;
        _attr[14] = NSOpenGLPFANoRecovery;
        _attr[15] = 1;
        _attr[16] = NSOpenGLPFAScreenMask;
        _attr[17] = CGDisplayIDToOpenGLDisplayMask(_maindis);
        NSOpenGLPixelFormat* _fmt;
        _fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:_attr];
        if (_fmt == nil)
        {
            blDebugOutput("bulllord can not choose valid pixel format");
            return;
        }
        _PrGpuMem->pGLC = [[NSOpenGLContext alloc] initWithFormat:_fmt shareContext:nil];
        [_fmt release];
        if (_PrGpuMem->pGLC == nil)
        {
            blDebugOutput("Failed creating OpenGL context");
            return;
        }
        [_PrGpuMem->pGLC setView:_View];
        [_PrGpuMem->pGLC update];
        [_PrGpuMem->pGLC makeCurrentContext];
        GLint _sync = _Vsync ? 1 : 0;
        [_PrGpuMem->pGLC setValues:&_sync forParameter:NSOpenGLCPSwapInterval];
    }
}
void
_GpuSwapBuffer(BLBool _Discard)
{
    if (BL_GL_API == _PrGpuMem->sHardwareCaps.eApiType)
    {
        if (_Discard == 1)
        {
            
        }
        else if (_Discard == 2)
        {
            GLint _sync = 0;
            [_PrGpuMem->pGLC setValues:&_sync forParameter:NSOpenGLCPSwapInterval];
        }
        else if (_Discard == 3)
        {
            GLint _sync = 1;
            [_PrGpuMem->pGLC setValues:&_sync forParameter:NSOpenGLCPSwapInterval];
        }
        else
            [_PrGpuMem->pGLC flushBuffer];
    }
}
void
_GpuAnitIntervention()
{
    if (BL_GL_API == _PrGpuMem->sHardwareCaps.eApiType)
    {
        [NSOpenGLContext clearCurrentContext];
        [_PrGpuMem->pGLC update];
        [_PrGpuMem->pGLC release];
    }
    free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_IOS)
void
_GpuIntervention()
{
}
void
_GpuSwapBuffer(BLBool _Discard)
{
}
void
_GpuAnitIntervention()
{
}
#else
#   "error what's the fucking platform"
#endif
void
blQueryHardwareCaps(OUT BLEnum* _Api, OUT BLU32* _MaxTexSize, OUT BLU32* _MaxFramebuffer, OUT BLU16 _TexSupport[BL_TF_COUNT])
{
	*_Api = _PrGpuMem->sHardwareCaps.eApiType;
	*_MaxTexSize = _PrGpuMem->sHardwareCaps.nMaxTextureSize;
	*_MaxFramebuffer = _PrGpuMem->sHardwareCaps.nMaxFBAttachments;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_TexSupport[_idx] = _PrGpuMem->sHardwareCaps.aTexFormats[_idx];
}
void
blRasterState(IN BLEnum _FillMode, IN BLEnum _CullMode, IN BLS32 _DepthBias, IN BLF32 _DepthBiasClamp, IN BLF32 _SlopeScaledDepthBias, IN BLBool _DepthClip, IN BLBool _Scissor)
{
}
void
blDepthStencilState(IN BLBool _Depth, IN BLBool _Mask, IN BLEnum _DepthCompFunc, IN BLBool _Stencil, IN BLU8 _StencilReadMask, IN BLU8 _StencilWriteMask, IN BLEnum _FrontStencilFailOp, IN BLEnum _FrontStencilDepthFailOp, IN BLEnum _FrontStencilPassOp, IN BLEnum _FrontStencilCompFunc, IN BLEnum _BackStencilFailOp, IN BLEnum _BackStencilDepthFailOp, IN BLEnum _BackStencilPassOp, IN BLEnum _BackStencilCompFunc)
{
}
void
blBlendState(IN BLBool _AlphaToCoverage, IN BLBool _IndependentBlend, IN BLBool _Blend, IN BLEnum _SrcBlendFactor, IN BLEnum _DestBlendFactor, IN BLEnum _SrcBlendAlphaFactor, IN BLEnum _DestBlendAlphaFactor, IN BLEnum _BlendOp, IN BLEnum _BlendOpAlpha, IN BLBool _RBitWrite, IN BLBool _GBitWrite, IN BLBool _BBitWrite, IN BLBool _ABitWrite)
{
}
BLGuid
blGenFramebuffer()
{
    return INVALID_GUID;
}
void
blDeleteFramebuffer(IN BLGuid _FBO)
{
}
void
blClearFramebuffer(IN BLGuid _FBO, IN BLBool _ColorBit, IN BLBool _DepthBit, IN BLBool _StencilBit, IN BLU32 _Color, IN BLF32 _Depth, IN BLS32 _Stencil)
{
}
void
blBindFramebuffer(IN BLGuid _FBO)
{
}
BLU32
blAttachFramebuffer(IN BLGuid _FBO, IN BLGuid _Tex, IN BLS32 _Level, IN BLEnum _CFace, IN BLS32 _ZOffset)
{
    return -1;
}
void
blDetachFramebuffer(IN BLGuid _FBO, IN BLU32 _Index)
{
}
BLGuid
blGenUniformbuffer(IN BLU32 _BufferSz)
{
    return INVALID_GUID;
}
void
blDeleteUniformbuffer(IN BLGuid _UBO)
{
}
void
blUpdateUniformbuffer(IN BLGuid _UBO, IN BLU32 _Offset, IN BLU8* const _Data, IN BLU32 _DataSz)
{
}
BLGuid
blGenTexture(IN BLEnum _Type, IN BLEnum _Format, IN BLU32 _Array, IN BLU32 _Mipmap, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, IN BLU8* _Data)
{
    return INVALID_GUID;
}
void
blDeleteTexture(IN BLGuid _Tex)
{
}
void
blTextureFilter(IN BLGuid _Tex, IN BLEnum _MinFilter, IN BLEnum _MagFilter, IN BLBool _Anisotropy)
{
}
BLBool
blUploadTexture(IN BLGuid _Tex, IN BLU32 _Layer, IN BLU32 _Level, IN BLEnum _Face, IN BLU32 _XOffset, IN BLU32 _YOffset, IN BLU32 _ZOffset, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, IN void* const _Data, IN BLU32 _DataSz)
{
    return TRUE;
}
BLBool blDownloadTexture(IN BLGuid _Tex, IN BLU32 _Layer, IN BLU32 _Level, IN BLEnum _Face, IN BLU32 _XOffset, IN BLU32 _YOffset, IN BLU32 _ZOffset, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, OUT void* _Data, IN BLU32 _DataSz)
{
    return TRUE;
}
void blQueryTexture(IN BLGuid _Tex, OUT BLEnum* _Type, OUT BLEnum* _Format, OUT BLU32* _Array, OUT BLU32* _Mipmap, OUT BLU32* _Width, OUT BLU32* _Height, OUT BLU32* _Depth, OUT BLU32* _Size)
{
}
BLGuid
blGenGeometryBuffer(IN BLEnum _Topology, IN BLBool _Dynamic, IN void* const _VBO, IN BLU32 _VBSz, IN BLU32 _VBStride, IN void* const _IBO, IN BLU32 _IBSz)
{
    return INVALID_GUID;
}
void
blDeleteGeometryBuffer(IN BLGuid _GBO)
{
}
void
blReloadGeometryBuffer(IN BLGuid _GBO, IN BLU32 _VBOffset, IN BLU8* const _VBO, IN BLU32 _VBSz, IN BLU32 _IBOffset, IN BLU8* const _IBO, IN BLU32 _IBSz)
{
}
BLGuid
blGenTechnique(IN BLAnsi* const _Filename)
{
    return INVALID_GUID;
}
void
blDeleteTechnique(IN BLGuid _Tech)
{
}
void
blDraw(IN BLGuid _Tech, IN BLGuid _GBO)
{
}
