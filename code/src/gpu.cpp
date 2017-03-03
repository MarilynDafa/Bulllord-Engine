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
#include "../headers/streamio.h"
#include "internal/dictionary.h"
#include "internal/array.h"
#include "internal/mathematics.h"
#include "internal/internal.h"
#include "../externals/xml/ezxml.h"
#if defined BL_PLATFORM_OSX
#   include <AppKit/NSOpenGL.h>
#   include <AppKit/NSApplication.h>
#elif defined BL_PLATFORM_ANDROID
#	include <EGL/egl.h>
#	include <EGL/eglext.h>
#	include <android/native_window.h>
#endif
typedef struct _GpuRes{
    void* pRes;
    BLS32 nRefCount;
}_BLGpuRes;
typedef struct _HardwareCaps{
	BLEnum eApiType;
    BLU32 nApiVersion;
	BLU16 nMaxTextureSize;
	BLU8  nMaxFBAttachments;
    BLBool bCSSupport;
    BLBool bGSSupport;
    BLBool bAnisotropy;
    BLBool bTessellationSupport;
    BLBool bFloatRTSupport;
    BLF32 fMaxAnisotropy;
	BLBool aTexFormats[BL_TF_COUNT];
}_BLHardwareCaps;
typedef struct _PipelineState{
    BLEnum eCullMode;
    BLS32 nDepthBias;
    BLF32 fSlopeScaledDepthBias;
    BLBool bScissor;
    BLU32 nScissorX;
    BLU32 nScissorY;
    BLU32 nScissorW;
    BLU32 nScissorH;
    BLBool bDepth;
    BLBool bDepthMask;
    BLEnum eDepthCompFunc;
    BLBool bStencil;
    BLU8 nStencilReadMask;
    BLU8 nStencilWriteMask;
    BLEnum eFrontStencilFailOp;
    BLEnum eFrontStencilDepthFailOp;
    BLEnum eFrontStencilPassOp;
    BLEnum eFrontStencilCompFunc;
    BLEnum eBackStencilFailOp;
    BLEnum eBackStencilDepthFailOp;
    BLEnum eBackStencilPassOp;
    BLEnum eBackStencilCompFunc;
    BLBool bAlphaToCoverage;
    BLBool bBlend;
    BLEnum eSrcBlendFactor;
    BLEnum eDestBlendFactor;
    BLEnum eSrcBlendAlphaFactor;
    BLEnum eDestBlendAlphaFactor;
    BLEnum eBlendOp;
    BLEnum eBlendOpAlpha;
    BLU8 aBlendFactor[4];
    BLBool bRasterStateDirty;
    BLBool bDSStateDirty;
    BLBool bBlendStateDirty;
    union {
#if defined(BL_GL_BACKEND)
        struct _GL {
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VK {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTL{
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DX{
        } sDX;
#endif
    } uData;
}_BLPipelineState;
typedef struct _TextureBuffer {
    BLEnum eTarget;
    BLEnum eFormat;
    BLGuid nID;
    BLU32 nWidth;
    BLU32 nHeight;
    BLU32 nDepth;
    BLU32 nLayer;
    BLU8 nNumMips;
    BLU32 nBpp;
    union {
#if defined(BL_GL_BACKEND)
        struct _GL {
            GLuint nHandle;
            GLuint nRTHandle;
            GLenum eInternalFmt;
            GLenum eType;
            GLenum eFormat;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VK {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTL{
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DX{
        } sDX;
#endif
    } uData;
}_BLTextureBuffer;
typedef struct _FrameBuffer {
    struct _Attachment{
        _BLTextureBuffer* pAttachments;
        BLU32 nMip;
        BLU32 nFace;
    } aAttachments[8];
    _BLTextureBuffer* pDSAttachment;
    BLU32 nAttachmentNum;
    BLU32 nWidth;
    BLU32 nHeight;
    union {
#if defined(BL_GL_BACKEND)
        struct _GL {
            GLuint nHandle;
            GLuint nResolveHandle;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VK {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTL{
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DX{
        } sDX;
#endif
    } uData;
}_BLFrameBuffer;
typedef struct _GeometryBuffer {
    BLGuid nID;
    BLEnum eVBTopology;
    BLEnum eIBFormat;
    BLU32 nVertexNum;
    BLU32 nVBSize;
    BLU32 nIBSize;
    BLBool bDynamic;
    BLEnum aInsSem[4];
    union {
#if defined(BL_GL_BACKEND)
        struct _GL {
            GLuint nVBHandle;
            GLuint nIBHandle;
            GLuint nVAHandle;
            GLuint nInsHandle[4];
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VK {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTL{
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DX{
        } sDX;
#endif
    } uData;
}_BLGeometryBuffer;
typedef struct _Technique{
	BLGuid nID;
    struct _UniformVar{
        BLAnsi aName[128];
        BLEnum eType;
        BLU32 nCount;
        BLVoid* pVar;
    } aUniformVars[16];
    struct _SamplerVar{
        BLAnsi aName[128];
        BLEnum eType;
        BLU32 nUnit;
        BLVoid* pTex;
    } aSamplerVars[16];
    union {
#if defined(BL_GL_BACKEND)
        struct _GL {
            GLuint nHandle;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VK {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTL{
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DX{
        } sDX;
#endif
    } uData;
}_BLTechnique;
#if defined(BL_GL_BACKEND)
#elif defined(BL_DX_BACKEND)
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
	BLVoid* aCmds[2];
	BLU32 aNum[2];
	BLU32 nCurrIndirect;
	BLU32 nMaxDrawPerBatch;
	BLU32 nMinIndirect;
	BLU32 nFlushPerBatch;
}_BLBatchBufferDX;
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
	BLVoid* pCode;
	BLU32 nHash;
	BLU16 nAttrMask[SHADERATR_COUNT_INTERNAL];
	BLU16 nUniforms;
	BLU16 nSize;
	BLU8 nPredefined;
}_BLShaderBufferDX;
#endif
typedef struct _GpuMember {
	_BLHardwareCaps sHardwareCaps;
    _BLPipelineState sPipelineState;
	BLF32 fPresentElapsed;
	BLBool bVsync;
	BLU16 nCurFramebufferHandle;
	BLU16 nBackBufferIdx;
    BLDictionary* pTextureCache;
    BLDictionary* pBufferCache;
	BLDictionary* pTechCache;
#if defined(BL_PLATFORM_WIN32)
	HGLRC sGLRC;
	HDC sGLHDC;
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
static BLU32
_TextureSize(BLEnum _BLFmt, BLU32 _Width, BLU32 _Height, BLU32 _Depth)
{
    switch (_BLFmt)
    {
        case BL_TF_BC1:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 8;
        case BL_TF_BC1A1:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 8;
        case BL_TF_BC3:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 16;
        case BL_TF_ETC2:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 8;
        case BL_TF_ETC2A1:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 8;
        case BL_TF_ETC2A:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 16;
        case BL_TF_ASTC:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 16;
        case BL_TF_BC5:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 16;
        case BL_TF_ETC2RG:
            return ((_Width + 3) / 4) * ((_Height + 3) / 4) * _Depth * 16;
        case BL_TF_R8:
            return _Width * _Height * _Depth * 1;
        case BL_TF_R8I:
            return _Width * _Height * _Depth * 1;
        case BL_TF_R8U:
            return _Width * _Height * _Depth * 1;
        case BL_TF_R8S:
            return _Width * _Height * _Depth * 1;
        case BL_TF_R32I:
            return _Width * _Height * _Depth * 4;
        case BL_TF_R32U:
            return _Width * _Height * _Depth * 4;
        case BL_TF_R32F:
            return _Width * _Height * _Depth * 4;
        case BL_TF_RG8:
            return _Width * _Height * _Depth * 2;
        case BL_TF_RG8I:
            return _Width * _Height * _Depth * 2;
        case BL_TF_RG8U:
            return _Width * _Height * _Depth * 2;
        case BL_TF_RG8S:
            return _Width * _Height * _Depth * 2;
        case BL_TF_RG32I:
            return _Width * _Height * _Depth * 8;
        case BL_TF_RG32U:
            return _Width * _Height * _Depth * 8;
        case BL_TF_RG32F:
            return _Width * _Height * _Depth * 8;
        case BL_TF_RGB8:
            return _Width * _Height * _Depth * 3;
        case BL_TF_RGB8I:
            return _Width * _Height * _Depth * 3;
        case BL_TF_RGB8U:
            return _Width * _Height * _Depth * 3;
        case BL_TF_RGB8S:
            return _Width * _Height * _Depth * 3;
        case BL_TF_RGB9E5F:
            return _Width * _Height * _Depth * 4;
        case BL_TF_R11G11B10F:
            return _Width * _Height * _Depth * 4;
        case BL_TF_BGRA8:
            return _Width * _Height * _Depth * 4;
        case BL_TF_RGBA8:
            return _Width * _Height * _Depth * 4;
        case BL_TF_RGBA8I:
            return _Width * _Height * _Depth * 4;
        case BL_TF_RGBA8U:
            return _Width * _Height * _Depth * 4;
        case BL_TF_RGBA8S:
            return _Width * _Height * _Depth * 4;
        case BL_TF_RGBA32I:
            return _Width * _Height * _Depth * 16;
        case BL_TF_RGBA32U:
            return _Width * _Height * _Depth * 16;
        case BL_TF_RGBA32F:
            return _Width * _Height * _Depth * 16;
        case BL_TF_RGB10A2:
            return _Width * _Height * _Depth * 4;
        case BL_TF_D16:
            return _Width * _Height * _Depth * 2;
        case BL_TF_D24:
            return _Width * _Height * _Depth * 3;
        case BL_TF_D32:
            return _Width * _Height * _Depth * 4;
        case BL_TF_D24S8:
            return _Width * _Height * _Depth * 4;
        default:
            return 0;
    }
}
#if defined(BL_GL_BACKEND)
static BLVoid
_PipelineStateDefaultGL(BLU32 _Width, BLU32 _Height)
{
	GL_CHECK_INTERNAL(glClearColor(0.f, 0.f, 0.f, 0.f));
    GL_CHECK_INTERNAL(glEnable(GL_CULL_FACE));
    GL_CHECK_INTERNAL(glCullFace(GL_BACK));
    GL_CHECK_INTERNAL(glFrontFace(GL_CW));
    _PrGpuMem->sPipelineState.eCullMode = BL_CM_CW;
    GL_CHECK_INTERNAL(glDisable(GL_POLYGON_OFFSET_FILL));
    GL_CHECK_INTERNAL(glPolygonOffset(0.f, 0.f));
    _PrGpuMem->sPipelineState.nDepthBias = 0;
    _PrGpuMem->sPipelineState.fSlopeScaledDepthBias = 0.f;
    GL_CHECK_INTERNAL(glEnable(GL_SCISSOR_TEST));
	_PrGpuMem->sPipelineState.nScissorX = 0;
	_PrGpuMem->sPipelineState.nScissorY = 0;
	_PrGpuMem->sPipelineState.nScissorW = _Width;
	_PrGpuMem->sPipelineState.nScissorH = _Height;
    GL_CHECK_INTERNAL(glScissor(0, 0, _Width, _Height));
    _PrGpuMem->sPipelineState.bScissor = TRUE;
    GL_CHECK_INTERNAL(glEnable(GL_DEPTH_TEST));
    _PrGpuMem->sPipelineState.bDepth = TRUE;
    GL_CHECK_INTERNAL(glDepthFunc(GL_LESS));
    _PrGpuMem->sPipelineState.eDepthCompFunc = BL_CF_LESS;
    GL_CHECK_INTERNAL(glDepthMask(GL_TRUE));
    _PrGpuMem->sPipelineState.bDepthMask = TRUE;
    GL_CHECK_INTERNAL(glDisable(GL_STENCIL_TEST));
    _PrGpuMem->sPipelineState.bStencil = FALSE;
    _PrGpuMem->sPipelineState.nStencilReadMask = _PrGpuMem->sPipelineState.nStencilWriteMask = 0xFF;
    GL_CHECK_INTERNAL(glStencilMask(_PrGpuMem->sPipelineState.nStencilWriteMask));
    GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
    GL_CHECK_INTERNAL(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP));
    _PrGpuMem->sPipelineState.eFrontStencilFailOp = BL_SO_KEEP;
    _PrGpuMem->sPipelineState.eFrontStencilDepthFailOp = BL_SO_KEEP;
    _PrGpuMem->sPipelineState.eFrontStencilPassOp = BL_SO_KEEP;
    GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
    GL_CHECK_INTERNAL(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP));
    _PrGpuMem->sPipelineState.eBackStencilFailOp = BL_SO_KEEP;
    _PrGpuMem->sPipelineState.eBackStencilDepthFailOp = BL_SO_KEEP;
    _PrGpuMem->sPipelineState.eBackStencilPassOp = BL_SO_KEEP;
    GL_CHECK_INTERNAL(glDisable(GL_BLEND));
    _PrGpuMem->sPipelineState.bBlend = FALSE;
    GL_CHECK_INTERNAL(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    _PrGpuMem->sPipelineState.bAlphaToCoverage = FALSE;
    GL_CHECK_INTERNAL(glBlendEquationSeparate(GL_FUNC_ADD , GL_FUNC_ADD));
    GL_CHECK_INTERNAL(glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO));
    GL_CHECK_INTERNAL(glBlendColor(0.f, 0.f, 0.f, 0.f));
    memset(_PrGpuMem->sPipelineState.aBlendFactor, 0 , 4 * sizeof(BLU8));
    _PrGpuMem->sPipelineState.eBlendOp = BL_BO_ADD;
    _PrGpuMem->sPipelineState.eBlendOpAlpha = BL_BO_ADD;
    _PrGpuMem->sPipelineState.eSrcBlendFactor = BL_BF_ONE;
    _PrGpuMem->sPipelineState.eSrcBlendAlphaFactor = BL_BF_ONE;
    _PrGpuMem->sPipelineState.eDestBlendFactor = BL_BF_ZERO;
    _PrGpuMem->sPipelineState.eDestBlendAlphaFactor = BL_BF_ZERO;
    _PrGpuMem->sPipelineState.bRasterStateDirty = FALSE;
    _PrGpuMem->sPipelineState.bDSStateDirty = FALSE;
    _PrGpuMem->sPipelineState.bBlendStateDirty = FALSE;
}
static BLVoid
_PipelineStateRefreshGL()
{
    if (_PrGpuMem->sPipelineState.bRasterStateDirty)
    {
        switch (_PrGpuMem->sPipelineState.eCullMode)
        {
            case BL_CM_NONE:
            {
                GL_CHECK_INTERNAL(glDisable(GL_CULL_FACE));
            }
            break;
            case BL_CM_CCW:
            {
                GL_CHECK_INTERNAL(glEnable(GL_CULL_FACE));
                GL_CHECK_INTERNAL(glCullFace(GL_BACK));
                GL_CHECK_INTERNAL(glFrontFace(GL_CCW));
            }
            break;
            default:
            {
                GL_CHECK_INTERNAL(glEnable(GL_CULL_FACE));
                GL_CHECK_INTERNAL(glCullFace(GL_BACK));
                GL_CHECK_INTERNAL(glFrontFace(GL_CW));
            }
            break;
        }
        if (!blScalarApproximate(_PrGpuMem->sPipelineState.fSlopeScaledDepthBias, 0.f) || _PrGpuMem->sPipelineState.nDepthBias)
        {
            GL_CHECK_INTERNAL(glEnable(GL_POLYGON_OFFSET_FILL));
            GL_CHECK_INTERNAL(glPolygonOffset(0.f, 0.f));
        }
        else
        {
            GL_CHECK_INTERNAL(glDisable(GL_POLYGON_OFFSET_FILL));
        }
		BLU32 _screenw, _screenh;
		blWindowSize(&_screenw, &_screenh);
		BLS32 _scissorx = (BLS32)_PrGpuMem->sPipelineState.nScissorX;
		BLS32 _scissory = (BLS32)_screenh - _PrGpuMem->sPipelineState.nScissorH - _PrGpuMem->sPipelineState.nScissorY;
		BLS32 _scissorw = (BLS32)_PrGpuMem->sPipelineState.nScissorW;
		BLS32 _scissorh = (BLS32)_PrGpuMem->sPipelineState.nScissorH;
        GL_CHECK_INTERNAL(glScissor(_scissorx, _scissory, _scissorw, _scissorh));
        _PrGpuMem->sPipelineState.bRasterStateDirty = FALSE;
    }
    if (_PrGpuMem->sPipelineState.bDSStateDirty)
    {
        if (_PrGpuMem->sPipelineState.bDepth)
        {
            GL_CHECK_INTERNAL(glEnable(GL_DEPTH_TEST));
        }
        else
        {
            GL_CHECK_INTERNAL(glDisable(GL_DEPTH_TEST));
        }
        switch (_PrGpuMem->sPipelineState.eDepthCompFunc)
        {
            case BL_CF_NEVER:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_NEVER));
            }
            break;
            case BL_CF_LESS:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_LESS));
            }
            break;
            case BL_CF_EQUAL:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_EQUAL));
            }
            break;
            case BL_CF_LESSEQUAL:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_LEQUAL));
            }
            break;
            case BL_CF_GREATER:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_GREATER));
            }
            break;
            case BL_CF_NOTEQUAL:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_NOTEQUAL));
            }
            break;
            case BL_CF_GREATEREQUAL:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_GEQUAL));
            }
            break;
            default:
            {
                GL_CHECK_INTERNAL(glDepthFunc(GL_ALWAYS));
            }
            break;
        }
        if (_PrGpuMem->sPipelineState.bDepthMask)
        {
            GL_CHECK_INTERNAL(glDepthMask(GL_TRUE));
        }
        else
        {
            GL_CHECK_INTERNAL(glDepthMask(GL_FALSE));
        }
        if (_PrGpuMem->sPipelineState.bStencil)
        {
            GL_CHECK_INTERNAL(glEnable(GL_STENCIL_TEST));
        }
        else
        {
            GL_CHECK_INTERNAL(glDisable(GL_STENCIL_TEST));
        }
        GL_CHECK_INTERNAL(glStencilMask(_PrGpuMem->sPipelineState.nStencilWriteMask));
        switch (_PrGpuMem->sPipelineState.eFrontStencilCompFunc)
        {
            case BL_CF_NEVER:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_NEVER, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_LESS:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_LESS, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_EQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_EQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_LESSEQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_LEQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_GREATER:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_GREATER, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_NOTEQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_NOTEQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_GREATEREQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_GEQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            default:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
        }
        switch (_PrGpuMem->sPipelineState.eBackStencilCompFunc)
        {
            case BL_CF_NEVER:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_NEVER, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_LESS:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_LESS, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_EQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_EQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_LESSEQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_LEQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_GREATER:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_GREATER, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_NOTEQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_NOTEQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            case BL_CF_GREATEREQUAL:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_GEQUAL, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
            default:
            {
                GL_CHECK_INTERNAL(glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, _PrGpuMem->sPipelineState.nStencilReadMask));
            }
            break;
        }
        GLenum _sfailf, _zfailf, _zpassf;
        switch (_PrGpuMem->sPipelineState.eFrontStencilFailOp)
        {
            case BL_SO_KEEP: _sfailf = GL_KEEP; break;
            case BL_SO_ZERO: _sfailf = GL_ZERO; break;
            case BL_SO_REPLACE: _sfailf = GL_REPLACE; break;
            case BL_SO_INCRSAT: _sfailf = GL_INCR_WRAP; break;
            case BL_SO_DECRSAT: _sfailf = GL_DECR_WRAP; break;
            case BL_SO_INVERT: _sfailf = GL_INVERT; break;
            case BL_SO_INCR: _sfailf = GL_INCR; break;
            default: _sfailf = GL_DECR; break;
        }
        switch (_PrGpuMem->sPipelineState.eFrontStencilDepthFailOp)
        {
            case BL_SO_KEEP: _zfailf = GL_KEEP; break;
            case BL_SO_ZERO: _zfailf = GL_ZERO; break;
            case BL_SO_REPLACE: _zfailf = GL_REPLACE; break;
            case BL_SO_INCRSAT: _zfailf = GL_INCR_WRAP; break;
            case BL_SO_DECRSAT: _zfailf = GL_DECR_WRAP; break;
            case BL_SO_INVERT: _zfailf = GL_INVERT; break;
            case BL_SO_INCR: _zfailf = GL_INCR; break;
            default: _zfailf = GL_DECR; break;
        }
        switch (_PrGpuMem->sPipelineState.eFrontStencilPassOp)
        {
            case BL_SO_KEEP: _zpassf = GL_KEEP; break;
            case BL_SO_ZERO: _zpassf = GL_ZERO; break;
            case BL_SO_REPLACE: _zpassf = GL_REPLACE; break;
            case BL_SO_INCRSAT: _zpassf = GL_INCR_WRAP; break;
            case BL_SO_DECRSAT: _zpassf = GL_DECR_WRAP; break;
            case BL_SO_INVERT: _zpassf = GL_INVERT; break;
            case BL_SO_INCR: _zpassf = GL_INCR; break;
            default: _zpassf = GL_DECR; break;
        }
        GL_CHECK_INTERNAL(glStencilOpSeparate(GL_FRONT, _sfailf, _zfailf, _zpassf));
        GLenum _sfailb, _zfailb, _zpassb;
        switch (_PrGpuMem->sPipelineState.eBackStencilFailOp)
        {
            case BL_SO_KEEP: _sfailb = GL_KEEP; break;
            case BL_SO_ZERO: _sfailb = GL_ZERO; break;
            case BL_SO_REPLACE: _sfailb = GL_REPLACE; break;
            case BL_SO_INCRSAT: _sfailb = GL_INCR_WRAP; break;
            case BL_SO_DECRSAT: _sfailb = GL_DECR_WRAP; break;
            case BL_SO_INVERT: _sfailb = GL_INVERT; break;
            case BL_SO_INCR: _sfailb = GL_INCR; break;
            default: _sfailb = GL_DECR; break;
        }
        switch (_PrGpuMem->sPipelineState.eBackStencilDepthFailOp)
        {
            case BL_SO_KEEP: _zfailb = GL_KEEP; break;
            case BL_SO_ZERO: _zfailb = GL_ZERO; break;
            case BL_SO_REPLACE: _zfailb = GL_REPLACE; break;
            case BL_SO_INCRSAT: _zfailb = GL_INCR_WRAP; break;
            case BL_SO_DECRSAT: _zfailb = GL_DECR_WRAP; break;
            case BL_SO_INVERT: _zfailb = GL_INVERT; break;
            case BL_SO_INCR: _zfailb = GL_INCR; break;
            default: _zfailb = GL_DECR; break;
        }
        switch (_PrGpuMem->sPipelineState.eBackStencilPassOp)
        {
            case BL_SO_KEEP: _zpassb = GL_KEEP; break;
            case BL_SO_ZERO: _zpassb = GL_ZERO; break;
            case BL_SO_REPLACE: _zpassb = GL_REPLACE; break;
            case BL_SO_INCRSAT: _zpassb = GL_INCR_WRAP; break;
            case BL_SO_DECRSAT: _zpassb = GL_DECR_WRAP; break;
            case BL_SO_INVERT: _zpassb = GL_INVERT; break;
            case BL_SO_INCR: _zpassb = GL_INCR; break;
            default: _zpassb = GL_DECR; break;
        }
        GL_CHECK_INTERNAL(glStencilOpSeparate(GL_BACK, _sfailb, _zfailb, _zpassb));
        _PrGpuMem->sPipelineState.bDSStateDirty = FALSE;
    }
    if (_PrGpuMem->sPipelineState.bBlendStateDirty)
    {
        if (_PrGpuMem->sPipelineState.bBlend)
        {
            GL_CHECK_INTERNAL(glEnable(GL_BLEND));
        }
        else
        {
            GL_CHECK_INTERNAL(glDisable(GL_BLEND));
        }
        if (_PrGpuMem->sPipelineState.bAlphaToCoverage)
        {
            GL_CHECK_INTERNAL(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
        }
        else
        {
            GL_CHECK_INTERNAL(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
        }
        GLenum _es, _eas;
        switch (_PrGpuMem->sPipelineState.eBlendOp)
        {
            case BL_BO_ADD: _es = GL_FUNC_ADD; break;
            case BL_BO_SUBTRACT: _es = GL_FUNC_SUBTRACT; break;
            case BL_BO_REVSUBTRACT: _es = GL_FUNC_REVERSE_SUBTRACT; break;
            case BL_BO_MIN: _es = GL_MIN; break;
            default: _es = GL_MAX; break;
        }
        switch (_PrGpuMem->sPipelineState.eBlendOpAlpha)
        {
            case BL_BO_ADD: _eas = GL_FUNC_ADD; break;
            case BL_BO_SUBTRACT: _eas = GL_FUNC_SUBTRACT; break;
            case BL_BO_REVSUBTRACT: _eas = GL_FUNC_REVERSE_SUBTRACT; break;
            case BL_BO_MIN: _eas = GL_MIN; break;
            default: _eas = GL_MAX; break;
        }
        GLenum _srgb, _drgb, _srgba, _drgba;
        switch (_PrGpuMem->sPipelineState.eSrcBlendFactor)
        {
            case BL_BF_ZERO: _srgb = GL_ZERO; break;
            case BL_BF_ONE: _srgb = GL_ONE; break;
            case BL_BF_SRCCOLOR: _srgb = GL_SRC_COLOR; break;
            case BL_BF_INVSRCCOLOR: _srgb = GL_ONE_MINUS_SRC_COLOR; break;
            case BL_BF_SRCALPHA: _srgb = GL_SRC_ALPHA; break;
            case BL_BF_INVSRCALPHA: _srgb = GL_ONE_MINUS_SRC_ALPHA; break;
            case BL_BF_DESTCOLOR: _srgb = GL_DST_COLOR; break;
            case BL_BF_INVDESTCOLOR: _srgb = GL_ONE_MINUS_DST_COLOR; break;
            case BL_BF_DESTALPHA: _srgb = GL_DST_ALPHA; break;
            case BL_BF_INVDESTALPHA: _srgb = GL_ONE_MINUS_DST_ALPHA; break;
            case BL_BF_SRCALPHASAT: _srgb = GL_SRC_ALPHA_SATURATE; break;
            case BL_BF_FACTOR: _srgb = GL_CONSTANT_COLOR; break;
            default: _srgb = GL_ONE_MINUS_CONSTANT_COLOR; break;
        }
        switch (_PrGpuMem->sPipelineState.eDestBlendFactor)
        {
            case BL_BF_ZERO: _drgb = GL_ZERO; break;
            case BL_BF_ONE: _drgb = GL_ONE; break;
            case BL_BF_SRCCOLOR: _drgb = GL_SRC_COLOR; break;
            case BL_BF_INVSRCCOLOR: _drgb = GL_ONE_MINUS_SRC_COLOR; break;
            case BL_BF_SRCALPHA: _drgb = GL_SRC_ALPHA; break;
            case BL_BF_INVSRCALPHA: _drgb = GL_ONE_MINUS_SRC_ALPHA; break;
            case BL_BF_DESTCOLOR: _drgb = GL_DST_COLOR; break;
            case BL_BF_INVDESTCOLOR: _drgb = GL_ONE_MINUS_DST_COLOR; break;
            case BL_BF_DESTALPHA: _drgb = GL_DST_ALPHA; break;
            case BL_BF_INVDESTALPHA: _drgb = GL_ONE_MINUS_DST_ALPHA; break;
            case BL_BF_SRCALPHASAT: _drgb = GL_SRC_ALPHA_SATURATE; break;
            case BL_BF_FACTOR: _drgb = GL_CONSTANT_COLOR; break;
            default: _drgb = GL_ONE_MINUS_CONSTANT_COLOR; break;
        }
        switch (_PrGpuMem->sPipelineState.eSrcBlendAlphaFactor)
        {
            case BL_BF_ZERO: _srgba = GL_ZERO; break;
            case BL_BF_ONE: _srgba = GL_ONE; break;
            case BL_BF_SRCCOLOR: _srgba = GL_SRC_COLOR; break;
            case BL_BF_INVSRCCOLOR: _srgba = GL_ONE_MINUS_SRC_COLOR; break;
            case BL_BF_SRCALPHA: _srgba = GL_SRC_ALPHA; break;
            case BL_BF_INVSRCALPHA: _srgba = GL_ONE_MINUS_SRC_ALPHA; break;
            case BL_BF_DESTCOLOR: _srgba = GL_DST_COLOR; break;
            case BL_BF_INVDESTCOLOR: _srgba = GL_ONE_MINUS_DST_COLOR; break;
            case BL_BF_DESTALPHA: _srgba = GL_DST_ALPHA; break;
            case BL_BF_INVDESTALPHA: _srgba = GL_ONE_MINUS_DST_ALPHA; break;
            case BL_BF_SRCALPHASAT: _srgba = GL_SRC_ALPHA_SATURATE; break;
            case BL_BF_FACTOR: _srgba = GL_CONSTANT_COLOR; break;
            default: _srgba = GL_ONE_MINUS_CONSTANT_COLOR; break;
        }
        switch (_PrGpuMem->sPipelineState.eDestBlendAlphaFactor)
        {
            case BL_BF_ZERO: _drgba = GL_ZERO; break;
            case BL_BF_ONE: _drgba = GL_ONE; break;
            case BL_BF_SRCCOLOR: _drgba = GL_SRC_COLOR; break;
            case BL_BF_INVSRCCOLOR: _drgba = GL_ONE_MINUS_SRC_COLOR; break;
            case BL_BF_SRCALPHA: _drgba = GL_SRC_ALPHA; break;
            case BL_BF_INVSRCALPHA: _drgba = GL_ONE_MINUS_SRC_ALPHA; break;
            case BL_BF_DESTCOLOR: _drgba = GL_DST_COLOR; break;
            case BL_BF_INVDESTCOLOR: _drgba = GL_ONE_MINUS_DST_COLOR; break;
            case BL_BF_DESTALPHA: _drgba = GL_DST_ALPHA; break;
            case BL_BF_INVDESTALPHA: _drgba = GL_ONE_MINUS_DST_ALPHA; break;
            case BL_BF_SRCALPHASAT: _drgba = GL_SRC_ALPHA_SATURATE; break;
            case BL_BF_FACTOR: _drgba = GL_CONSTANT_COLOR; break;
            default: _drgba = GL_ONE_MINUS_CONSTANT_COLOR; break;
        }
        GL_CHECK_INTERNAL(glBlendFuncSeparate(_srgb, _drgb, _srgba, _drgba));
        GL_CHECK_INTERNAL(glBlendColor((GLfloat)_PrGpuMem->sPipelineState.aBlendFactor[0] / 255.f,
                                       (GLfloat)_PrGpuMem->sPipelineState.aBlendFactor[1] / 255.f,
                                       (GLfloat)_PrGpuMem->sPipelineState.aBlendFactor[2] / 255.f,
                                       (GLfloat)_PrGpuMem->sPipelineState.aBlendFactor[3] / 255.f));
        _PrGpuMem->sPipelineState.bBlendStateDirty = FALSE;
    }
}
static BLBool
_TextureFormatValidGL(GLenum _InternalFmt, GLenum _Fmt, GLenum _Type, BLU32 _Bpp)
{
    BLBool _compressed = FALSE;
    if (_InternalFmt == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
        _InternalFmt == GL_COMPRESSED_SRGB_S3TC_DXT1_EXT ||
        _InternalFmt == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
        _InternalFmt == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT ||
        _InternalFmt == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ||
        _InternalFmt == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT ||
        _InternalFmt == GL_COMPRESSED_RGB8_ETC2 ||
        _InternalFmt == GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 ||
        _InternalFmt == GL_COMPRESSED_RGBA8_ETC2_EAC ||
        _InternalFmt == GL_COMPRESSED_SRGB8_ETC2 ||
        _InternalFmt == GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 ||
        _InternalFmt == GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC ||
        _InternalFmt == GL_COMPRESSED_RGBA_ASTC_4x4_KHR ||
        _InternalFmt == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR ||
        _InternalFmt == GL_COMPRESSED_RG_RGTC2 ||
        _InternalFmt == GL_COMPRESSED_RG11_EAC)
        _compressed = TRUE;
	GLuint _id;
	GL_CHECK_INTERNAL(glGenTextures(1, &_id));
	GL_CHECK_INTERNAL(glBindTexture(GL_TEXTURE_2D, _id));
	GLsizei _size = (16 * 16 * _Bpp) / 8;
	BLVoid* _ptr;
	union {	BLVoid* ptr; BLU32 addr; } _un;
	_un.ptr = alloca(_size + 16);
	BLU32 _unalignedvar = _un.addr;
	BLU32 _mask = 15;
	BLU32 _aligned = (((_unalignedvar) + (_mask)) & ((~0) & (~(_mask))));
	_un.addr = _aligned;
	_ptr = _un.ptr;
	memset(_ptr, 1, _size);
	if (_compressed)
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, _InternalFmt, 16, 16, 0, _size, _ptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, _InternalFmt, 16, 16, 0, _Fmt, _Type, _ptr);
	glDeleteTextures(1, &_id);
	if (glGetError())
		return FALSE;
	else
		return TRUE;
}
static BLVoid
_FillTextureFormatGL(BLEnum _BLFmt, GLenum* _IFmt, GLenum* _Fmt, GLenum* _Type, GLenum* _SrgbFmt, GLenum* _RtFmt)
{
    switch (_BLFmt)
    {
        case BL_TF_BC1:
            *_IFmt = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            *_SrgbFmt = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
            *_Fmt = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_BC1A1:
            *_IFmt = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            *_SrgbFmt = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
            *_Fmt = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_BC3:
            *_IFmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            *_SrgbFmt = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
            *_Fmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_ETC2:
            *_IFmt = GL_COMPRESSED_RGB8_ETC2;
            *_SrgbFmt = GL_COMPRESSED_SRGB8_ETC2;
            *_Fmt = GL_COMPRESSED_RGB8_ETC2;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_ETC2A1:
            *_IFmt = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            *_SrgbFmt = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            *_Fmt = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_ETC2A:
            *_IFmt = GL_COMPRESSED_RGBA8_ETC2_EAC;
            *_SrgbFmt = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
            *_Fmt = GL_COMPRESSED_RGBA8_ETC2_EAC;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_ASTC:
            *_IFmt = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
            *_SrgbFmt = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
            *_Fmt = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_BC5:
            *_IFmt = GL_COMPRESSED_RG_RGTC2;
            *_SrgbFmt = GL_COMPRESSED_RG_RGTC2;
            *_Fmt = GL_COMPRESSED_RG_RGTC2;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_ETC2RG:
            *_IFmt = GL_COMPRESSED_RG11_EAC;
            *_SrgbFmt = GL_COMPRESSED_RG11_EAC;
            *_Fmt = GL_COMPRESSED_RG11_EAC;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
        case BL_TF_R8:
            *_IFmt = GL_R8;
            *_SrgbFmt = GL_R8;
            *_Fmt = GL_RED;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_R8;
            break;
        case BL_TF_R8I:
            *_IFmt = GL_R8I;
            *_SrgbFmt = GL_R8I;
            *_Fmt = GL_RED;
            *_Type = GL_BYTE;
            *_RtFmt = GL_R8I;
            break;
        case BL_TF_R8U:
            *_IFmt = GL_R8UI;
            *_SrgbFmt = GL_R8UI;
            *_Fmt = GL_RED;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_R8UI;
            break;
        case BL_TF_R8S:
            *_IFmt = GL_R8_SNORM;
            *_SrgbFmt = GL_R8_SNORM;
            *_Fmt = GL_RED;
            *_Type = GL_BYTE;
            *_RtFmt = GL_R8_SNORM;
            break;
        case BL_TF_R32I:
            *_IFmt = GL_R32I;
            *_SrgbFmt = GL_R32I;
            *_Fmt = GL_RED;
            *_Type = GL_INT;
            *_RtFmt = GL_R32I;
            break;
        case BL_TF_R32U:
            *_IFmt = GL_R32UI;
            *_SrgbFmt = GL_R32UI;
            *_Fmt = GL_RED;
            *_Type = GL_UNSIGNED_INT;
            *_RtFmt = GL_R32UI;
            break;
        case BL_TF_R32F:
            *_IFmt = GL_R32F;
            *_SrgbFmt = GL_R32F;
            *_Fmt = GL_RED;
            *_Type = GL_FLOAT;
            *_RtFmt = GL_R32F;
            break;
        case BL_TF_RG8:
            *_IFmt = GL_RG8;
            *_SrgbFmt = GL_RG8;
            *_Fmt = GL_RG;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RG8;
            break;
        case BL_TF_RG8I:
            *_IFmt = GL_RG8I;
            *_SrgbFmt = GL_RG8I;
            *_Fmt = GL_RG;
            *_Type = GL_BYTE;
            *_RtFmt = GL_RG8I;
            break;
        case BL_TF_RG8U:
            *_IFmt = GL_RG8UI;
            *_SrgbFmt = GL_RG8UI;
            *_Fmt = GL_RG;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RG8UI;
            break;
        case BL_TF_RG8S:
            *_IFmt = GL_RG8_SNORM;
            *_SrgbFmt = GL_RG8_SNORM;
            *_Fmt = GL_RG;
            *_Type = GL_BYTE;
            *_RtFmt = GL_RG8_SNORM;
            break;
        case BL_TF_RG32I:
            *_IFmt = GL_RG32I;
            *_SrgbFmt = GL_RG32I;
            *_Fmt = GL_RG;
            *_Type = GL_INT;
            *_RtFmt = GL_RG32I;
            break;
        case BL_TF_RG32U:
            *_IFmt = GL_RG32UI;
            *_SrgbFmt = GL_RG32UI;
            *_Fmt = GL_RG;
            *_Type = GL_UNSIGNED_INT;
            *_RtFmt = GL_RG32UI;
            break;
        case BL_TF_RG32F:
            *_IFmt = GL_RG32F;
            *_SrgbFmt = GL_RG32F;
            *_Fmt = GL_RG;
            *_Type = GL_FLOAT;
            *_RtFmt = GL_RG32F;
            break;
        case BL_TF_RGB8:
            *_IFmt = GL_RGB8;
            *_SrgbFmt = GL_SRGB8;
            *_Fmt = GL_RGB;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RGB8;
            break;
        case BL_TF_RGB8I:
            *_IFmt = GL_RGB8I;
            *_SrgbFmt = GL_RGB8I;
            *_Fmt = GL_RGB;
            *_Type = GL_BYTE;
            *_RtFmt = GL_RGB8I;
            break;
        case BL_TF_RGB8U:
            *_IFmt = GL_RGB8UI;
            *_SrgbFmt = GL_RGB8UI;
            *_Fmt = GL_RGB;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RGB8UI;
            break;
        case BL_TF_RGB8S:
            *_IFmt = GL_RGB8_SNORM;
            *_SrgbFmt = GL_RGB8_SNORM;
            *_Fmt = GL_RGB;
            *_Type = GL_BYTE;
            *_RtFmt = GL_RGB8_SNORM;
            break;
        case BL_TF_RGB9E5F:
            *_IFmt = GL_RGB9_E5;
            *_SrgbFmt = GL_RGB9_E5;
            *_Fmt = GL_RGB;
            *_Type = GL_UNSIGNED_INT_5_9_9_9_REV;
            *_RtFmt = GL_RGB9_E5;
            break;
        case BL_TF_R11G11B10F:
            *_IFmt = GL_R11F_G11F_B10F;
            *_SrgbFmt = GL_R11F_G11F_B10F;
            *_Fmt = GL_RGBA;
            *_Type = GL_UNSIGNED_INT_10F_11F_11F_REV;
            *_RtFmt = GL_R11F_G11F_B10F;
            break;
        case BL_TF_BGRA8:
            *_IFmt = GL_RGBA8;
            *_SrgbFmt = GL_SRGB8_ALPHA8;
            *_Fmt = GL_BGRA;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RGBA8;
            break;
        case BL_TF_RGBA8:
            *_IFmt = GL_RGBA8;
            *_SrgbFmt = GL_SRGB8_ALPHA8;
            *_Fmt = GL_RGBA;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RGBA8;
            break;
        case BL_TF_RGBA8I:
            *_IFmt = GL_RGBA8I;
            *_SrgbFmt = GL_RGBA8I;
            *_Fmt = GL_RGBA;
            *_Type = GL_BYTE;
            *_RtFmt = GL_RGBA8I;
            break;
        case BL_TF_RGBA8U:
            *_IFmt = GL_RGBA8UI;
            *_SrgbFmt = GL_RGBA8UI;
            *_Fmt = GL_RGBA;
            *_Type = GL_UNSIGNED_BYTE;
            *_RtFmt = GL_RGBA8UI;
            break;
        case BL_TF_RGBA8S:
            *_IFmt = GL_RGBA8_SNORM;
            *_SrgbFmt = GL_RGBA8_SNORM;
            *_Fmt = GL_RGBA;
            *_Type = GL_BYTE;
            *_RtFmt = GL_RGBA8_SNORM;
            break;
        case BL_TF_RGBA32I:
            *_IFmt = GL_RGBA32I;
            *_SrgbFmt = GL_RGBA32I;
            *_Fmt = GL_RGBA;
            *_Type = GL_INT;
            *_RtFmt = GL_RGBA32I;
            break;
        case BL_TF_RGBA32U:
            *_IFmt = GL_RGBA32UI;
            *_SrgbFmt = GL_RGBA32UI;
            *_Fmt = GL_RGBA;
            *_Type = GL_UNSIGNED_INT;
            *_RtFmt = GL_RGBA32UI;
            break;
        case BL_TF_RGBA32F:
            *_IFmt = GL_RGBA32F;
            *_SrgbFmt = GL_RGBA32F;
            *_Fmt = GL_RGBA;
            *_Type = GL_FLOAT;
            *_RtFmt = GL_RGBA32F;
            break;
        case BL_TF_RGB10A2:
            *_IFmt = GL_RGB10_A2;
            *_SrgbFmt = GL_RGB10_A2;
            *_Fmt = GL_RGBA;
            *_Type = GL_UNSIGNED_INT_2_10_10_10_REV;
            *_RtFmt = GL_RGB10_A2;
            break;
        case BL_TF_D16:
            *_IFmt = GL_DEPTH_COMPONENT16;
            *_SrgbFmt = GL_DEPTH_COMPONENT16;
            *_Fmt = GL_DEPTH_COMPONENT;
            *_Type = GL_UNSIGNED_SHORT;
            *_RtFmt = GL_DEPTH_COMPONENT16;
            break;
        case BL_TF_D24:
            *_IFmt = GL_DEPTH_COMPONENT24;
            *_SrgbFmt = GL_DEPTH_COMPONENT24;
            *_Fmt = GL_DEPTH_COMPONENT;
            *_Type = GL_UNSIGNED_INT;
            *_RtFmt = GL_DEPTH_COMPONENT24;
            break;
        case BL_TF_D32:
            *_IFmt = GL_DEPTH_COMPONENT32F;
            *_SrgbFmt = GL_DEPTH_COMPONENT32F;
            *_Fmt = GL_DEPTH_COMPONENT;
            *_Type = GL_FLOAT;
            *_RtFmt = GL_DEPTH_COMPONENT32F;
            break;
        case BL_TF_D24S8:
            *_IFmt = GL_DEPTH24_STENCIL8;
            *_SrgbFmt = GL_DEPTH24_STENCIL8;
            *_Fmt = GL_DEPTH_STENCIL;
            *_Type = GL_UNSIGNED_INT_24_8;
            *_RtFmt = GL_DEPTH24_STENCIL8;
            break;
        default:
            *_IFmt = GL_ZERO;
            *_SrgbFmt = GL_ZERO;
            *_Fmt = GL_ZERO;
            *_Type = GL_ZERO;
            *_RtFmt = GL_ZERO;
            break;
    }
}
#elif defined(BL_DX_BACKEND)
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
static BLVoid
_UpdateBuffer(_BLGenericBufferDX* _Buf, ID3D12GraphicsCommandList* _CommandList, BLU32 _Offset, BLU32 _Size, BLVoid* _Data)
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
    DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_hp, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_ID3D12Resource, (BLVoid**)&_staging));
    BLU8* _data;
    DX_CHECK_INTERNAL(_staging->Map(0, NULL, (BLVoid**)&_data));
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
static BLVoid
_CreateBuffer(_BLGenericBufferDX* _Buf, BLU32 _Size, BLVoid* _Data, BLU16 _Flags, BLBool _Vertex, BLU32 _Stride)
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
    DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_hp, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_ID3D12Resource, (BLVoid**)&_Buf->pPtr));
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
static BLVoid
_DestroyBuffer(_BLGenericBufferDX* _Buf)
{
    if (_Buf->pPtr)
    {
        blArrayPushBack(_PrGpuMem->sCmdQueue.pRelease[_PrGpuMem->sCmdQueue.sControl.nCurrent], _Buf->pPtr);
        _Buf->bDynamic = FALSE;
    }
}
#elif defined(BL_VK_BACKEND)
#else
#endif
#if defined(BL_PLATFORM_WIN32)
BLVoid
_GpuIntervention(HWND _Hwnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
	_PrGpuMem->sHardwareCaps.bGSSupport = TRUE;
	_PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
	_PrGpuMem->sHardwareCaps.bTessellationSupport = TRUE;
	_PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
	_PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
	_PrGpuMem->pTextureCache = blGenDict(TRUE);
	_PrGpuMem->pBufferCache = blGenDict(TRUE);
	_PrGpuMem->pTechCache = blGenDict(TRUE);
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	BLBool _vkinited = FALSE;
	BLBool _gpuok = FALSE;
	if (_vkinited)
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_VULKAN_API;
	}
	else
	{
		_PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
		_PrGpuMem->sGLHDC = GetDC(_Hwnd);
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
		BLS32 _pf = ChoosePixelFormat(_PrGpuMem->sGLHDC, &_pfd);
		assert(_pf != 0);
		SetPixelFormat(_PrGpuMem->sGLHDC, _pf, &_pfd);
		_PrGpuMem->sGLRC = wglCreateContext(_PrGpuMem->sGLHDC);
		wglMakeCurrent(_PrGpuMem->sGLHDC, _PrGpuMem->sGLRC);
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
			HGLRC _hwgl = wglCreateContextAttribsARB(_PrGpuMem->sGLHDC, 0, _attribus);
			if (_hwgl)
			{
				_gpuok = TRUE;
				wglMakeCurrent(_PrGpuMem->sGLHDC, 0);
				wglDeleteContext(_PrGpuMem->sGLRC);
				_PrGpuMem->sGLRC = _hwgl;
				wglMakeCurrent(_PrGpuMem->sGLHDC, _PrGpuMem->sGLRC);
				_PrGpuMem->sHardwareCaps.nApiVersion = _attribus[1] * 100 + _attribus[3] * 10;
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
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1] = _TextureFormatValidGL(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1A1] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC3] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2] = _TextureFormatValidGL(GL_COMPRESSED_RGB8_ETC2, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ETC2, GL_ZERO, GL_ZERO, 4);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A1] = _TextureFormatValidGL(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A] = _TextureFormatValidGL(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC5] = _TextureFormatValidGL(GL_COMPRESSED_RG_RGTC2, GL_ZERO, GL_ZERO, 8);
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2RG] = _TextureFormatValidGL(GL_COMPRESSED_RG11_EAC, GL_ZERO, GL_ZERO, 8);
		GLint _extensions = 0;
		GL_CHECK_INTERNAL(glGetIntegerv(GL_NUM_EXTENSIONS, &_extensions));
		for (GLint _idx = 0; _idx < _extensions; ++_idx)
		{
			const BLAnsi* _name = (const BLAnsi*)glGetStringi(GL_EXTENSIONS, _idx);
			if (!strcmp(_name, "GL_EXT_texture_filter_anisotropic"))
			{
				GL_CHECK_INTERNAL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_PrGpuMem->sHardwareCaps.fMaxAnisotropy));
			}
		}
		_PipelineStateDefaultGL(_Width, _Height);
	}
}
BLVoid
_GpuSwapBuffer()
{
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		::SwapBuffers(_PrGpuMem->sGLHDC);
	}
}
BLVoid
_GpuAnitIntervention(HWND _Hwnd)
{
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		wglMakeCurrent(_PrGpuMem->sGLHDC, 0);
		wglDeleteContext(_PrGpuMem->sGLRC);
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_UWP)
BLVoid
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
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#   if defined(_DEBUG)
	ID3D12Debug* _debug;
	DX_CHECK_INTERNAL(D3D12GetDebugInterface(IID_ID3D12Debug, (BLVoid**)&_debug));
	_debug->EnableDebugLayer();
#   endif
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
		if (SUCCEEDED(D3D12CreateDevice(_PrGpuMem->pAdapter, _featurelevel[_idx], IID_ID3D12Device, (BLVoid**)&_PrGpuMem->pDevice)))
		{
			_gpuok = TRUE;
			break;
		}
	}
	if (!_gpuok)
	{
		DX_CHECK_INTERNAL(_PrGpuMem->pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&_PrGpuMem->pAdapter)));
		if (_PrGpuMem->pAdapter)
			if (SUCCEEDED(D3D12CreateDevice(_PrGpuMem->pAdapter, D3D_FEATURE_LEVEL_11_0, IID_ID3D12Device, (BLVoid**)&_PrGpuMem->pDevice)))
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
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandQueue(&_queuedesc, IID_ID3D12CommandQueue, (BLVoid**)&_PrGpuMem->sCmdQueue.pCommandQueue));
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, (BLVoid**)&_PrGpuMem->sCmdQueue.pFence));
	for (_idx = 0; _idx < 256; ++_idx)
	{
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_ID3D12CommandAllocator, (BLVoid**)&_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandAllocator));
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandAllocator, NULL, IID_ID3D12GraphicsCommandList, (BLVoid**)&_PrGpuMem->sCmdQueue.aCommandList[_idx].pCommandList));
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
	if (SUCCEEDED(_PrGpuMem->pDevice->QueryInterface(IID_ID3D12InfoQueue, (BLVoid**)&_infoqueue)))
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
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_rtvdescheap, IID_ID3D12DescriptorHeap, (BLVoid**)&_PrGpuMem->pRtvHeap));
	D3D12_DESCRIPTOR_HEAP_DESC _dsvdescheap;
	_dsvdescheap.NumDescriptors = 1	+ 128;
	_dsvdescheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	_dsvdescheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	_dsvdescheap.NodeMask = 1;
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_dsvdescheap, IID_ID3D12DescriptorHeap, (BLVoid**)&_PrGpuMem->pDsvHeap));
	for (_idx = 0; _idx < 3; ++_idx)
	{
		BLU32 _maxdesc = (4 << 10) + 128 + ((64 << 10) - 1);
		_PrGpuMem->aScratchBuffer[_idx].nIncrementSize = _PrGpuMem->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_DESCRIPTOR_HEAP_DESC _desc;
		_desc.NumDescriptors = _maxdesc;
		_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		_desc.NodeMask = 1;
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_desc, IID_ID3D12DescriptorHeap, (BLVoid**)&_PrGpuMem->aScratchBuffer[_idx].pHeap));
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
		DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_hp, D3D12_HEAP_FLAG_NONE, &_resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_ID3D12Resource, (BLVoid**)&_PrGpuMem->aScratchBuffer[_idx].pUpload));
		_PrGpuMem->aScratchBuffer[_idx].nGpuVA = _PrGpuMem->aScratchBuffer[_idx].pUpload->GetGPUVirtualAddress();
		_PrGpuMem->aScratchBuffer[_idx].pUpload->Map(0, NULL, (BLVoid**)&_PrGpuMem->aScratchBuffer[_idx].pData);
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
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateDescriptorHeap(&_samplerdesc, IID_ID3D12DescriptorHeap, (BLVoid**)&_PrGpuMem->sSamplerBuffer.pHeap));
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
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateRootSignature(0, _out->GetBufferPointer(), _out->GetBufferSize(), IID_ID3D12RootSignature, (BLVoid**)&_PrGpuMem->pRootSignature));
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
		DX_CHECK_INTERNAL(_PrGpuMem->pSwapChain->GetBuffer(_idx, IID_ID3D12Resource, (BLVoid**)&_PrGpuMem->aBackColor[_idx]));
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
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommittedResource(&_dsvhp, D3D12_HEAP_FLAG_NONE, &_backdsdesc, D3D12_RESOURCE_STATE_COMMON, &_clearclr, IID_ID3D12Resource, (BLVoid**)&_PrGpuMem->pDepthStencil));
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
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandSignature(&_drawvertexcmdsig, _PrGpuMem->pRootSignature, IID_ID3D12CommandSignature, (BLVoid**)&_PrGpuMem->sBatchBuffer.aCommandSignature[0]));
	D3D12_INDIRECT_ARGUMENT_DESC _drawindexedarg[] =
	{
		{ D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW,{ { 2 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,{ { 0 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW,{ { 1 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW,{ { 0 } } },
		{ D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED,{ { 0 } } },
	};
	D3D12_COMMAND_SIGNATURE_DESC _drawindexedcmdsig ={ sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand), 5, _drawindexedarg, 1};
	DX_CHECK_INTERNAL(_PrGpuMem->pDevice->CreateCommandSignature(&_drawindexedcmdsig, _PrGpuMem->pRootSignature, IID_ID3D12CommandSignature, (BLVoid**)&_PrGpuMem->sBatchBuffer.aCommandSignature[1]));
	_PrGpuMem->sBatchBuffer.nCmds[0] = malloc((4 << 10) * sizeof(_BLBatchBufferDX::_DrawVertexIndirectCommand));
	_PrGpuMem->sBatchBuffer.nCmds[1] = malloc((4 << 10) * sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand));
	for (_idx = 0; _idx < 32; ++_idx)
		_CreateBuffer(&_PrGpuMem->sBatchBuffer.aIndirect[_idx], (4 << 10) * sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand), NULL, BUFFER_DRAW_INDIRECT_INTERNAL, FALSE, sizeof(_BLBatchBufferDX::_DrawIndexedIndirectCommand));
}
BLVoid
_GpuSwapBuffer()
{
	if (_Overdrive)
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
BLVoid
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
		FOREACH_DICT (_BLTextureBufferDX*, _iter, _PrGpuMem->pTextureDict)
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
		FOREACH_DICT (struct _BLUniformBuffer::_UniformInfo*, _iter, _PrGpuMem->sUniformBuffer.pUniforms)
		{
			free(_iter);
		}
	}
	blDeleteDict(_PrGpuMem->pTextureDict);
	blDeleteDict(_PrGpuMem->sUniformBuffer.pUniforms);
	for (_idx = 0; _idx < 256; ++_idx)
		blDeleteArray(_PrGpuMem->sCmdQueue.pRelease[_idx]);
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_LINUX)
static BLS32
_CtxErrorHandler(Display*, XErrorEvent*)
{
    _PrGpuMem->bCtxError = TRUE;
    return 0;
}
BLVoid
_GpuIntervention(Display* _Display, Window _Window, GLXFBConfig _Config, BLVoid* _Lib, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
    _PrGpuMem->sHardwareCaps.nFuncSupported = 0;
    for (BLU32 _idx = 0 ; _idx < BL_TF_COUNT ; ++_idx)
        _PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	_PrGpuMem->bCtxError = FALSE;
	BLBool _vkinited = FALSE;
    BLBool _gpuok = FALSE;
    if (_vkinited)
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_VULKAN_API;
    }
    else
    {
        BLS32 _glversion;
        _PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
        GL_LOAD_INTERNAL(_lib);
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1] = _TextureFormatValid(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) & _TextureFormatValid(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC3] = _TextureFormatValid(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8) & _TextureFormatValid(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2] = _TextureFormatValid(GL_COMPRESSED_RGB8_ETC2, GL_ZERO, GL_ZERO, 4) & _TextureFormatValid(GL_COMPRESSED_SRGB8_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A1] = _TextureFormatValid(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4) & _TextureFormatValid(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A] = _TextureFormatValid(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8) & _TextureFormatValid(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValid(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) & _TextureFormatValid(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC5] = _TextureFormatValid(GL_COMPRESSED_RG_RGTC2, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2RG] = _TextureFormatValid(GL_COMPRESSED_RG11_EAC, GL_ZERO, GL_ZERO, 8);
    }
}
BLVoid
_GpuSwapBuffer()
{
}
BLVoid
_GpuAnitIntervention()
{
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
}
#elif defined(BL_PLATFORM_ANDROID)
BLVoid
_GpuIntervention(ANativeWindow* _Wnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
    _PrGpuMem->sHardwareCaps.nFuncSupported = 0;
    for (BLU32 _idx = 0 ; _idx < BL_TF_COUNT ; ++_idx)
        _PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1] = _TextureFormatValid(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) & _TextureFormatValid(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC3] = _TextureFormatValid(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8) & _TextureFormatValid(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2] = _TextureFormatValid(GL_COMPRESSED_RGB8_ETC2, GL_ZERO, GL_ZERO, 4) & _TextureFormatValid(GL_COMPRESSED_SRGB8_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A1] = _TextureFormatValid(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4) & _TextureFormatValid(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A] = _TextureFormatValid(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8) & _TextureFormatValid(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValid(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) & _TextureFormatValid(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC5] = _TextureFormatValid(GL_COMPRESSED_RG_RGTC2, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2RG] = _TextureFormatValid(GL_COMPRESSED_RG11_EAC, GL_ZERO, GL_ZERO, 8);
    }
}
BLVoid
_GpuSwapBuffer()
{
}
BLVoid
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
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_OSX)
BLVoid
_GpuIntervention(NSView* _View, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->fPresentElapsed = 0.f;
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->nCurFramebufferHandle = 0xFFFF;
    _PrGpuMem->nBackBufferIdx = 0;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTessellationSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
    _PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
    _PrGpuMem->pTextureCache = blGenDict(TRUE);
    _PrGpuMem->pBufferCache = blGenDict(TRUE);
    _PrGpuMem->pTechCache = blGenDict(TRUE);
    for (BLU32 _idx = 0 ; _idx < BL_TF_COUNT ; ++_idx)
        _PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
    _PrGpuMem->pGLC = nil;
    BLBool _vkinited = FALSE;
    if (_vkinited)
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_VULKAN_API;
    }
    else
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
        NSOpenGLPixelFormatAttribute _attr[32] = { 0 };
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1] = _TextureFormatValidGL(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1A1] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC3] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2] = _TextureFormatValidGL(GL_COMPRESSED_RGB8_ETC2, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A1] = _TextureFormatValidGL(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A] = _TextureFormatValidGL(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC5] = _TextureFormatValidGL(GL_COMPRESSED_RG_RGTC2, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2RG] = _TextureFormatValidGL(GL_COMPRESSED_RG11_EAC, GL_ZERO, GL_ZERO, 8);
        GLint _extensions = 0;
        GL_CHECK_INTERNAL(glGetIntegerv(GL_NUM_EXTENSIONS, &_extensions));
        for (GLint _idx = 0; _idx < _extensions; ++_idx)
        {
            const BLAnsi* _name = (const BLAnsi*)glGetStringi(GL_EXTENSIONS, _idx);
            if (!strcmp(_name, "GL_EXT_texture_filter_anisotropic"))
            {
                GL_CHECK_INTERNAL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_PrGpuMem->sHardwareCaps.fMaxAnisotropy));
            }
        }
        _PipelineStateDefaultGL(_Width, _Height);
        _PrGpuMem->sHardwareCaps.nApiVersion = 410;
    }
}
BLVoid
_GpuSwapBuffer()
{
    if (BL_GL_API == _PrGpuMem->sHardwareCaps.eApiType)
    {
		[_PrGpuMem->pGLC flushBuffer];
    }
}
BLVoid
_GpuAnitIntervention()
{
    if (BL_GL_API == _PrGpuMem->sHardwareCaps.eApiType)
    {
        [NSOpenGLContext clearCurrentContext];
        [_PrGpuMem->pGLC update];
        [_PrGpuMem->pGLC release];
    }
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
    blDeleteDict(_PrGpuMem->pTextureCache);
    blDeleteDict(_PrGpuMem->pBufferCache);
    free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_IOS)
BLVoid
_GpuIntervention(BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->fPresentElapsed = 0.f;
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->nCurFramebufferHandle = 0xFFFF;
    _PrGpuMem->nBackBufferIdx = 0;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTessellationSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
    _PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
    _PrGpuMem->pTextureCache = blGenDict(TRUE);
    _PrGpuMem->pBufferCache = blGenDict(TRUE);
}
BLVoid
_GpuSwapBuffer()
{
}
BLVoid
_GpuAnitIntervention()
{
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT (_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
}
#else
#   "error what's the fucking platform"
#endif
BLVoid 
blVSync(IN BLBool _On)
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)malloc(sizeof(_BLFrameBuffer));
	memset(_fbo, 0, sizeof(_BLFrameBuffer));
	_fbo->nAttachmentNum = 0;
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
#	if defined(BL_PLATFORM_WIN32)
		wglSwapIntervalEXT(_On);
#	elif defined(BL_PLATFORM_OSX)
		GLint _sync = _On;
		[_PrGpuMem->pGLC setValues : &_sync forParameter : NSOpenGLCPSwapInterval];
#	elif defined(BL_PLATFORM_LINUX)
#	elif defined(BL_PLATFORM_IOS)
#	elif defined(BL_PLATFORM_ANDROID)
#	endif
	}
#elif defined(BL_MTL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
	{
	}
#elif defined(BL_VK_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
#elif defined(BL_DX_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
	{
	}
#endif
}
BLVoid
blQueryHardwareCaps(OUT BLEnum* _Api, OUT BLU32* _MaxTexSize, OUT BLU32* _MaxFramebuffer, OUT BLBool _TexSupport[BL_TF_COUNT])
{
	*_Api = _PrGpuMem->sHardwareCaps.eApiType;
	*_MaxTexSize = _PrGpuMem->sHardwareCaps.nMaxTextureSize;
	*_MaxFramebuffer = _PrGpuMem->sHardwareCaps.nMaxFBAttachments;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_TexSupport[_idx] = _PrGpuMem->sHardwareCaps.aTexFormats[_idx];
}
BLVoid
blRasterState(IN BLEnum _CullMode, IN BLS32 _DepthBias, IN BLF32 _SlopeScaledDepthBias, IN BLBool _Scissor, IN BLU32 _X, IN BLU32 _Y, IN BLU32 _W, IN BLU32 _H)
{
    if (_PrGpuMem->sPipelineState.eCullMode != _CullMode ||
        _PrGpuMem->sPipelineState.nDepthBias != _DepthBias ||
        !blScalarApproximate(_PrGpuMem->sPipelineState.fSlopeScaledDepthBias, _SlopeScaledDepthBias) ||
        _PrGpuMem->sPipelineState.bScissor != _Scissor ||
        _PrGpuMem->sPipelineState.nScissorX != _X ||
        _PrGpuMem->sPipelineState.nScissorY != _Y ||
        _PrGpuMem->sPipelineState.nScissorW != _W ||
        _PrGpuMem->sPipelineState.nScissorH != _H)
    {
        _PrGpuMem->sPipelineState.eCullMode = _CullMode;
        _PrGpuMem->sPipelineState.nDepthBias = _DepthBias;
        _PrGpuMem->sPipelineState.fSlopeScaledDepthBias = _SlopeScaledDepthBias;
        _PrGpuMem->sPipelineState.bScissor = _Scissor;
        _PrGpuMem->sPipelineState.nScissorX = _X;
		_PrGpuMem->sPipelineState.nScissorY = _Y;
		_PrGpuMem->sPipelineState.nScissorW = _W;
		_PrGpuMem->sPipelineState.nScissorH = _H;
        _PrGpuMem->sPipelineState.bRasterStateDirty = TRUE;
    }
}
BLVoid
blDepthStencilState(IN BLBool _Depth, IN BLBool _Mask, IN BLEnum _DepthCompFunc, IN BLBool _Stencil, IN BLU8 _StencilReadMask, IN BLU8 _StencilWriteMask, IN BLEnum _FrontStencilFailOp, IN BLEnum _FrontStencilDepthFailOp, IN BLEnum _FrontStencilPassOp, IN BLEnum _FrontStencilCompFunc, IN BLEnum _BackStencilFailOp, IN BLEnum _BackStencilDepthFailOp, IN BLEnum _BackStencilPassOp, IN BLEnum _BackStencilCompFunc)
{
    if (_PrGpuMem->sPipelineState.bDepth != _Depth ||
        _PrGpuMem->sPipelineState.bDepthMask != _Mask ||
        _PrGpuMem->sPipelineState.eDepthCompFunc != _DepthCompFunc ||
        _PrGpuMem->sPipelineState.bStencil != _Stencil ||
        _PrGpuMem->sPipelineState.nStencilReadMask != _StencilReadMask ||
        _PrGpuMem->sPipelineState.nStencilWriteMask != _StencilWriteMask ||
        _PrGpuMem->sPipelineState.eFrontStencilFailOp != _FrontStencilFailOp ||
        _PrGpuMem->sPipelineState.eFrontStencilDepthFailOp != _FrontStencilDepthFailOp ||
        _PrGpuMem->sPipelineState.eFrontStencilPassOp != _FrontStencilPassOp ||
        _PrGpuMem->sPipelineState.eFrontStencilCompFunc != _FrontStencilCompFunc ||
        _PrGpuMem->sPipelineState.eBackStencilFailOp != _BackStencilFailOp ||
        _PrGpuMem->sPipelineState.eBackStencilDepthFailOp != _BackStencilDepthFailOp ||
        _PrGpuMem->sPipelineState.eBackStencilPassOp != _BackStencilPassOp ||
        _PrGpuMem->sPipelineState.eBackStencilCompFunc != _BackStencilCompFunc)
    {
        _PrGpuMem->sPipelineState.bDepth = _Depth;
        _PrGpuMem->sPipelineState.bDepthMask = _Mask;
        _PrGpuMem->sPipelineState.eDepthCompFunc = _DepthCompFunc;
        _PrGpuMem->sPipelineState.bStencil = _Stencil;
        _PrGpuMem->sPipelineState.nStencilReadMask = _StencilReadMask;
        _PrGpuMem->sPipelineState.nStencilWriteMask = _StencilWriteMask;
        _PrGpuMem->sPipelineState.eFrontStencilFailOp = _FrontStencilFailOp;
        _PrGpuMem->sPipelineState.eFrontStencilDepthFailOp = _FrontStencilDepthFailOp;
        _PrGpuMem->sPipelineState.eFrontStencilPassOp = _FrontStencilPassOp;
        _PrGpuMem->sPipelineState.eFrontStencilCompFunc = _FrontStencilCompFunc;
        _PrGpuMem->sPipelineState.eBackStencilFailOp = _BackStencilFailOp;
        _PrGpuMem->sPipelineState.eBackStencilDepthFailOp = _BackStencilDepthFailOp;
        _PrGpuMem->sPipelineState.eBackStencilPassOp = _BackStencilPassOp;
        _PrGpuMem->sPipelineState.eBackStencilCompFunc = _BackStencilCompFunc;
        _PrGpuMem->sPipelineState.bDSStateDirty = TRUE;
    }
}
BLVoid
blBlendState(IN BLBool _AlphaToCoverage, IN BLBool _Blend, IN BLEnum _SrcBlendFactor, IN BLEnum _DestBlendFactor, IN BLEnum _SrcBlendAlphaFactor, IN BLEnum _DestBlendAlphaFactor, IN BLEnum _BlendOp, IN BLEnum _BlendOpAlpha, IN BLU8 _BlendFactor[4])
{
    if (_PrGpuMem->sPipelineState.bAlphaToCoverage != _AlphaToCoverage ||
        _PrGpuMem->sPipelineState.bBlend != _Blend ||
        _PrGpuMem->sPipelineState.eSrcBlendFactor != _SrcBlendFactor ||
        _PrGpuMem->sPipelineState.eDestBlendFactor != _DestBlendFactor ||
        _PrGpuMem->sPipelineState.eSrcBlendAlphaFactor != _SrcBlendAlphaFactor ||
        _PrGpuMem->sPipelineState.eDestBlendAlphaFactor != _DestBlendAlphaFactor ||
        _PrGpuMem->sPipelineState.eBlendOp != _BlendOp ||
        _PrGpuMem->sPipelineState.eBlendOpAlpha != _BlendOpAlpha ||
        _PrGpuMem->sPipelineState.aBlendFactor[0] != _BlendFactor[0] ||
        _PrGpuMem->sPipelineState.aBlendFactor[1] != _BlendFactor[1] ||
        _PrGpuMem->sPipelineState.aBlendFactor[2] != _BlendFactor[2] ||
        _PrGpuMem->sPipelineState.aBlendFactor[3] != _BlendFactor[3])
    {
        _PrGpuMem->sPipelineState.bAlphaToCoverage = _AlphaToCoverage;
        _PrGpuMem->sPipelineState.bBlend = _Blend;
        _PrGpuMem->sPipelineState.eSrcBlendFactor = _SrcBlendFactor;
        _PrGpuMem->sPipelineState.eDestBlendFactor = _DestBlendFactor;
        _PrGpuMem->sPipelineState.eSrcBlendAlphaFactor = _SrcBlendAlphaFactor;
        _PrGpuMem->sPipelineState.eDestBlendAlphaFactor = _DestBlendAlphaFactor;
        _PrGpuMem->sPipelineState.eBlendOp = _BlendOp;
        _PrGpuMem->sPipelineState.eBlendOpAlpha = _BlendOpAlpha;
        _PrGpuMem->sPipelineState.aBlendFactor[0] = _BlendFactor[0];
        _PrGpuMem->sPipelineState.aBlendFactor[1] = _BlendFactor[1];
        _PrGpuMem->sPipelineState.aBlendFactor[2] = _BlendFactor[2];
        _PrGpuMem->sPipelineState.aBlendFactor[3] = _BlendFactor[3];
        _PrGpuMem->sPipelineState.bBlendStateDirty = TRUE;
    }
}
BLGuid
blGenFrameBuffer()
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)malloc(sizeof(_BLFrameBuffer));
    memset(_fbo, 0, sizeof(_BLFrameBuffer));
    _fbo->nAttachmentNum = 0;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glGenFramebuffers(1, &_fbo->uData.sGL.nHandle));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
    return blGenGuid(_fbo, 0xFFFFFFFF);
}
BLVoid
blDeleteFrameBuffer(IN BLGuid _FBO)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
    if (!_fbo)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glDeleteFramebuffers(1, &_fbo->uData.sGL.nHandle));
        if (_fbo->uData.sGL.nResolveHandle)
            GL_CHECK_INTERNAL(glDeleteFramebuffers(1, &_fbo->uData.sGL.nResolveHandle));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
    free(_fbo);
    blDeleteGuid(_FBO);
}
BLVoid
blFrameBufferClear(IN BLGuid _FBO, IN BLBool _ColorBit, IN BLBool _DepthBit, IN BLBool _StencilBit)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        if (_fbo)
        {
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
        }
        else
        {
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
        BLU32 _clearbuf = 0;
        if (_ColorBit)
            _clearbuf |= GL_COLOR_BUFFER_BIT;
        if (_DepthBit)
            _clearbuf |= GL_DEPTH_BUFFER_BIT;
        if (_StencilBit)
            _clearbuf |= GL_STENCIL_BUFFER_BIT;
        if(_clearbuf != 0)
            GL_CHECK_INTERNAL(glClear(_clearbuf));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blFrameBufferBind(IN BLGuid _FBO)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
    if (!_fbo)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blFrameBufferResolve(IN BLGuid _FBO)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
    if (!_fbo)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        if (_fbo->uData.sGL.nResolveHandle)
        {
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
            GL_CHECK_INTERNAL(glReadBuffer(GL_COLOR_ATTACHMENT0));
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo->uData.sGL.nResolveHandle));
            GL_CHECK_INTERNAL(glBlitFramebuffer(0, 0, _fbo->nWidth, _fbo->nHeight, 0, 0, _fbo->nWidth, _fbo->nHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
            GL_CHECK_INTERNAL(glReadBuffer(GL_NONE) );
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
        if (_fbo->nAttachmentNum)
        {
            for (BLU32 _idx = 0; _idx < _fbo->nAttachmentNum; ++_idx)
            {
                _BLTextureBuffer* _tex = _fbo->aAttachments[_idx].pAttachments;
                if (_tex->nNumMips > 1)
                {
                    GLenum _target = GL_TEXTURE_2D;
                    switch (_tex->eTarget)
                    {
                        case BL_TT_1D: _target = GL_TEXTURE_2D; break;
                        case BL_TT_2D: _target = GL_TEXTURE_2D; break;
                        case BL_TT_3D: _target = GL_TEXTURE_3D; break;
                        case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
                        case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
                        case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
                        case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
                        default:break;
                    }
                    GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                    GL_CHECK_INTERNAL(glGenerateMipmap(_target));
                    GL_CHECK_INTERNAL(glBindTexture(_target, 0));
                }
            }
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blFrameBufferDiscard(IN BLGuid _FBO, IN BLBool _DiscardColor, IN BLBool _DiscardDepth, IN BLBool _DiscardStencil)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
    if (!_fbo)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GLenum _buffers[10];
        BLU32 _idx = 0;
        if (_DiscardColor)
        {
            for (BLU32 _jdx = 0; _jdx < _fbo->nAttachmentNum; ++_jdx)
                _buffers[_idx++] = GL_COLOR_ATTACHMENT0 + _jdx;
        }
        if (_DiscardDepth && _DiscardStencil)
            _buffers[_idx++] = GL_DEPTH_STENCIL_ATTACHMENT;
        else if (_DiscardDepth && !_DiscardStencil)
            _buffers[_idx++] = GL_DEPTH_ATTACHMENT;
        else if (!_DiscardDepth && _DiscardStencil)
            _buffers[_idx++] = GL_STENCIL_ATTACHMENT;
        GL_CHECK_INTERNAL(glInvalidateFramebuffer(GL_FRAMEBUFFER, _idx, _buffers));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLU32
blFrameBufferAttach(IN BLGuid _FBO, IN BLGuid _Tex, IN BLS32 _Level, IN BLEnum _CFace)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
    if (!_fbo)
        return -1;
    BLEnum _type;
    BLEnum _format;
    BLU32 _array;
    BLU32 _mipmap;
    BLU32 _width;
    BLU32 _height;
    BLU32 _depth;
    BLU32 _size;
    blTextureQuery(_Tex, &_type, &_format, &_array, &_mipmap, &_width, &_height, &_depth, &_size);
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
        if (0 == _fbo->nAttachmentNum)
        {
            _fbo->nWidth  = MAX_INTERNAL(_width  >> _Level, 1);
            _fbo->nHeight = MAX_INTERNAL(_height >> _Level, 1);
        }
        GLenum _attachment;
        if (_format < BL_TF_DS_UNKNOWN)
        {
            _attachment = GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentNum;
            _fbo->aAttachments[_fbo->nAttachmentNum].pAttachments = _tex;
            _fbo->aAttachments[_fbo->nAttachmentNum].nFace = _CFace;
            _fbo->aAttachments[_fbo->nAttachmentNum].nMip = _Level;
            _fbo->nAttachmentNum++;
        }
        else if (_format == BL_TF_D24S8)
        {
            _attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            _fbo->pDSAttachment = _tex;
        }
        else
        {
            _attachment = GL_DEPTH_ATTACHMENT;
            _fbo->pDSAttachment = _tex;
        }
        if (_tex->uData.sGL.nRTHandle)
        {
            GL_CHECK_INTERNAL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachment, GL_RENDERBUFFER, _tex->uData.sGL.nRTHandle));
        }
        else
        {
            GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _CFace : GL_TEXTURE_2D;
            if (_tex->eTarget == BL_TT_2D || _tex->eTarget == BL_TT_CUBE)
            {
                GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, _target, _tex->uData.sGL.nHandle, _Level));
            }
        }
        if (_tex->uData.sGL.nRTHandle && _tex->uData.sGL.nHandle)
        {
            GL_CHECK_INTERNAL(glGenFramebuffers(1, &_tex->uData.sGL.nRTHandle));
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _tex->uData.sGL.nRTHandle));
            for (BLU32 _idx = 0; _idx < _fbo->nAttachmentNum; ++_idx)
            {
                _tex = _fbo->aAttachments[_idx].pAttachments;
                _attachment = GL_COLOR_ATTACHMENT0 + _idx;
                if (_tex->eFormat < BL_TF_DS_UNKNOWN)
                {
                    GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _fbo->aAttachments[_idx].nFace : GL_TEXTURE_2D;
                    GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, _target, _tex->uData.sGL.nHandle, _fbo->aAttachments[_idx].nMip));
                }
            }
        }
        assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
    return _fbo->nAttachmentNum;
}
BLVoid
blFrameBufferDetach(IN BLGuid _FBO, IN BLU32 _Index, IN BLBool _DepthStencil)
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
    if (!_fbo)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        if (_fbo->pDSAttachment && _DepthStencil)
        {
            GLenum _attachment;
            if (_fbo->pDSAttachment->eTarget == BL_TF_D24S8)
                _attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            else
                _attachment = GL_DEPTH_ATTACHMENT;
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
            GL_CHECK_INTERNAL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachment, GL_RENDERBUFFER, 0));
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nResolveHandle));
            GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, 0, 0));
            _fbo->pDSAttachment = NULL;
        }
        GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
        _BLTextureBuffer* _tex = _fbo->aAttachments[_Index].pAttachments;
        if (_fbo->uData.sGL.nResolveHandle)
        {
            GL_CHECK_INTERNAL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _Index, GL_RENDERBUFFER, 0));
        }
        else
        {
            GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _fbo->aAttachments[_Index].nFace : GL_TEXTURE_2D;
            GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _Index, _target, 0, _fbo->aAttachments[_Index].nMip));
        }
        if (_fbo->uData.sGL.nResolveHandle)
        {
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nResolveHandle));
            GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _fbo->aAttachments[_Index].nFace : GL_TEXTURE_2D;
            GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _Index, _target, 0, _fbo->aAttachments[_Index].nMip));
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLGuid
blGenTexture(IN BLU32 _Hash, IN BLEnum _Target, IN BLEnum _Format, IN BLBool _Srgb, IN BLBool _Immutable, IN BLBool _RenderTarget, IN BLU32 _Layer, IN BLU32 _Mipmap, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, IN BLU8* _Data)
{
    _BLTextureBuffer* _tex = NULL;
    BLBool _cache = FALSE;
    if (_Immutable)
    {
        blMutexLock(_PrGpuMem->pTextureCache->pMutex);
        _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTextureCache, _Hash);
        _tex = _res ? (_BLTextureBuffer*)_res->pRes : NULL;
        if (_res)
            _res->nRefCount++;
        blMutexUnlock(_PrGpuMem->pTextureCache->pMutex);
    }
	if (!_tex)
	{
		_tex = (_BLTextureBuffer*)malloc(sizeof(_BLTextureBuffer));
		memset(_tex, 0, sizeof(_BLTextureBuffer));
		_cache = _Immutable;
	}
	else
		return _tex->nID;
    _tex->nWidth = _Width;
    _tex->nHeight = _Height;
    _tex->nDepth = _Depth;
    _tex->nLayer = _Layer;
    _tex->nNumMips = _Mipmap;
    _tex->eTarget = _Target;
    _tex->eFormat = _Format;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GLenum _ifmt, _fmt, _type, _sfmt, _rtfmt;
        _FillTextureFormatGL(_Format, &_ifmt, &_fmt, &_type, &_sfmt, &_rtfmt);
        GL_CHECK_INTERNAL(glGenTextures(1, &_tex->uData.sGL.nHandle));
        GLenum _target;
        switch (_Target)
        {
            case BL_TT_1D: _target = GL_TEXTURE_2D; break;
            case BL_TT_2D: _target = GL_TEXTURE_2D; break;
            case BL_TT_3D: _target = GL_TEXTURE_3D; break;
            case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
            case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
            case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
            case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
            default:break;
        }
        GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
        if (_Immutable)
        {
            switch (_Target)
            {
            case BL_TT_1D:
                GL_CHECK_INTERNAL(glTexStorage2D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height));
                break;
            case BL_TT_2D:
                GL_CHECK_INTERNAL(glTexStorage2D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height));
                break;
            case BL_TT_3D:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height, _Depth));
                break;
            case BL_TT_CUBE:
                GL_CHECK_INTERNAL(glTexStorage2D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height));
                break;
            case BL_TT_ARRAY1D:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height, _Layer));
                break;
            case BL_TT_ARRAY2D:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height, _Layer));
                break;
            case BL_TT_ARRAYCUBE:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, _Srgb ? _ifmt : _sfmt, _Width, _Height, _Layer));
                break;
            default:break;
            }
        }
        if (_RenderTarget)
        {
            GL_CHECK_INTERNAL(glGenRenderbuffers(1, &_tex->uData.sGL.nRTHandle));
            GL_CHECK_INTERNAL(glBindRenderbuffer(GL_RENDERBUFFER, _tex->uData.sGL.nRTHandle));
            GL_CHECK_INTERNAL(glRenderbufferStorage(GL_RENDERBUFFER, _rtfmt, _Width, _Height));
            GL_CHECK_INTERNAL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
        }
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAX_LEVEL, _Mipmap - 1));
        if (_Data)
        {
            BLU32 _faces = (_target == GL_TEXTURE_CUBE_MAP) || (_target == GL_TEXTURE_CUBE_MAP_ARRAY) ? 6:1;
            for (BLU32 _aidx = 0; _aidx < _Layer; ++ _aidx)
            {
                BLU32 _w = _Width;
                BLU32 _h = _Height;
                BLU32 _d = _Depth;
                for(BLU32 _face = 0; _face < _faces; ++_face)
                {
                    for (BLU32 _level = 0; _level < _Mipmap; ++_level)
                    {
                        GLsizei _imagesz = _TextureSize(_Format, _w, _h, _d);
                        switch(_target)
                        {
                            case GL_TEXTURE_2D:
                            {
                                if(_Format > BL_TF_UNKNOWN)
                                {
                                    if (_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glTexSubImage2D(_target, _level, 0, 0, _w, _h, _fmt, _type, _Data + _aidx * _Mipmap + _level));
                                    }
                                    else
                                    {
                                        GL_CHECK_INTERNAL(glTexImage2D(_target, _level, _ifmt, _w, _h, 0, _fmt, _type, _Data + _aidx * _Mipmap + _level));
                                    }
                                }
                                else
                                {
                                    if (_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexSubImage2D(_target, _level, 0, 0, _w, _h, _ifmt, _imagesz, _Data + _aidx * _Mipmap + _level));
                                    }
                                    else
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexImage2D(_target, _level, _ifmt, _w, _h, 0, _imagesz, _Data + _aidx * _Mipmap + _level));
                                    }
                                }
                            }
                            break;
                            case GL_TEXTURE_2D_ARRAY:
                            {
                                if(_Format > BL_TF_UNKNOWN)
                                {
                                    if (0 == _aidx && !_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glTexImage3D(_target, _level, _ifmt, _w, _h, _Layer, 0, _fmt, _type, 0));
                                    }
                                    GL_CHECK_INTERNAL(glTexSubImage3D(_target, _level, 0, 0, _aidx, _w, _h, 1, _fmt, _type, _Data + _aidx * _Mipmap + _level));
                                }
                                else
                                {
                                    if (0 == _aidx && !_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexImage3D(_target, _level, _ifmt, _w, _h, _Layer, 0, _imagesz*_Layer, 0));
                                    }
                                    GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _level, 0, 0, _aidx, _w, _h, 1, _ifmt , _imagesz, _Data + _aidx * _Mipmap + _level));
                                }
                            }
                            break;
                            case GL_TEXTURE_CUBE_MAP:
                            {
                                if(_Format > BL_TF_UNKNOWN)
                                {
                                    if (_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0 , 0, _w, _h, _fmt, _type, _Data + _aidx * _Mipmap + _level));
                                    }
                                    else
                                    {
                                        GL_CHECK_INTERNAL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, _ifmt, _w, _h, 0, _fmt, _type, _Data + _aidx * _Mipmap + _level));
                                    }
                                }
                                else
                                {
                                    if (_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _w, _h, _ifmt, _imagesz, _Data + _aidx * _Mipmap + _level));
                                    }
                                    else
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, _ifmt, _w, _h, 0, _imagesz, _Data + _aidx * _Mipmap + _level));
                                    }
                                }
                            }
                            break;
                            case GL_TEXTURE_CUBE_MAP_ARRAY:
                            {
                                if(_Format > BL_TF_UNKNOWN)
                                {
                                    if (0 == _aidx)
                                    {
                                        GL_CHECK_INTERNAL(glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, _ifmt, _w, _h, _Layer, 0, _fmt, _type, 0));
                                    }
                                    GL_CHECK_INTERNAL(glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _aidx, _w, _h, 1, _fmt, _type, _Data + _aidx * _Mipmap + _level));
                                }
                                else
                                {
                                    if (0 == _aidx)
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, _ifmt, _w, _h, _Layer, 0, _imagesz * _Layer, 0));
                                    }
                                    GL_CHECK_INTERNAL(glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _aidx, _w, _h, 1, _ifmt, _imagesz, _Data + _aidx * _Layer + _level));
                                }
                            }
                            break;
                            case GL_TEXTURE_3D:
                            {
                                if(_Format > BL_TF_UNKNOWN)
                                {
                                    if (_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glTexSubImage3D(_target, _level, 0, 0, 0, _w, _h, _d, _fmt, _type, _Data + _level));
                                    }
                                    else
                                    {
                                        GL_CHECK_INTERNAL(glTexImage3D(_target, _level, _ifmt, _w, _h, _d, 0, _fmt, _type, _Data + _level));
                                    }
                                }
                                else
                                {
                                    if (_Immutable)
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _level, 0, 0, 0, _w, _h, _d, _ifmt, _imagesz, _Data + _level));
                                    }
                                    else
                                    {
                                        GL_CHECK_INTERNAL(glCompressedTexImage3D(_target, _level, _ifmt, _w, _h, _d, 0, _imagesz, _Data + _level));
                                    }
                                }
                            }
                            break;
                            default:assert(0);break;
                        }
                        _w = MAX_INTERNAL(1U, _w / 2);
                        _h = MAX_INTERNAL(1U, _h / 2);
                        _d = MAX_INTERNAL(1U, _d / 2);
                    }
                }
            }
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
    if (_cache)
    {
        blMutexLock(_PrGpuMem->pTextureCache->pMutex);
		_BLGpuRes* _res = (_BLGpuRes*)malloc(sizeof(_BLGpuRes));
		_res->nRefCount = 1;
		_res->pRes = _tex;
        blDictInsert(_PrGpuMem->pTextureCache, _Hash, _res);
        blMutexUnlock(_PrGpuMem->pTextureCache->pMutex);
    }
    _tex->nID = blGenGuid(_tex, _Hash);
    return _tex->nID;
}
BLVoid
blDeleteTexture(IN BLGuid _Tex)
{
    BLBool _discard = FALSE;
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
	if (!_tex)
		return;
    blMutexLock(_PrGpuMem->pTextureCache->pMutex);
    _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTextureCache, URIPART_INTERNAL(_Tex));
    if (_res)
    {
        _res->nRefCount--;
        if (_res->nRefCount <= 0)
        {
            _discard = TRUE;
            free(_res);
            blDictErase(_PrGpuMem->pTextureCache, URIPART_INTERNAL(_Tex));
        }
    }
    else
        _discard = TRUE;
    blMutexUnlock(_PrGpuMem->pTextureCache->pMutex);
    if (!_discard)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GLenum _target;
        switch (_tex->eTarget)
        {
            case BL_TT_1D: _target = GL_TEXTURE_2D; break;
            case BL_TT_2D: _target = GL_TEXTURE_2D; break;
            case BL_TT_3D: _target = GL_TEXTURE_3D; break;
            case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
            case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
            case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
            case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
            default:break;
        }
        GL_CHECK_INTERNAL(glBindTexture(_target, 0));
        GL_CHECK_INTERNAL(glDeleteTextures(1, &_tex->uData.sGL.nHandle));
        if (_tex->uData.sGL.nRTHandle)
            GL_CHECK_INTERNAL(glDeleteRenderbuffers(1, &_tex->uData.sGL.nRTHandle));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
    free(_tex);
    blDeleteGuid(_Tex);
}
BLGuid
blGainTexture(IN BLU32 _Hash)
{
    blMutexLock(_PrGpuMem->pTextureCache->pMutex);
    _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTextureCache, _Hash);
    blMutexUnlock(_PrGpuMem->pTextureCache->pMutex);
    if (_res)
    {
        _res->nRefCount++;
        return ((_BLTextureBuffer*)_res->pRes)->nID;
    }
    else
        return INVALID_GUID;
}
BLVoid
blTextureFilter(IN BLGuid _Tex, IN BLEnum _MinFilter, IN BLEnum _MagFilter, IN BLEnum _WrapS, IN BLEnum _WrapT, IN BLBool _Anisotropy)
{
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
	if (!_tex)
		return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GLenum _target;
        switch (_tex->eTarget)
        {
            case BL_TT_1D: _target = GL_TEXTURE_2D; break;
            case BL_TT_2D: _target = GL_TEXTURE_2D; break;
            case BL_TT_3D: _target = GL_TEXTURE_3D; break;
            case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
            case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
            case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
            case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
            default:break;
        }
        GLenum _minfilter;
        if (BL_TF_NEAREST == _MinFilter)
            _minfilter = GL_NEAREST;
        else
            _minfilter = GL_LINEAR;
        GLenum _magfilter;
        if (BL_TF_NEAREST == _MagFilter)
            _magfilter = GL_NEAREST;
        else
            _magfilter = GL_LINEAR;
        GLenum _wraps, _wrapt;
        switch (_WrapS)
        {
            case BL_TW_REPEAT:
                _wraps = GL_REPEAT;
                break;
            case BL_TW_CLAMP:
                _wraps = GL_CLAMP_TO_EDGE;
                break;
            case BL_TW_MIRROR:
                _wraps = GL_MIRRORED_REPEAT;
                break;
            default:break;
        }
        switch (_WrapT)
        {
            case BL_TW_REPEAT:
                _wrapt = GL_REPEAT;
                break;
            case BL_TW_CLAMP:
                _wrapt = GL_CLAMP_TO_EDGE;
                break;
            case BL_TW_MIRROR:
                _wrapt = GL_MIRRORED_REPEAT;
                break;
            default:break;
        }
        GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wraps));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrapt));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _magfilter));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _minfilter));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAX_LEVEL, _tex->nNumMips - 1));
        if (_Anisotropy && _PrGpuMem->sHardwareCaps.fMaxAnisotropy >= 1.f)
        {
            GL_CHECK_INTERNAL(glTexParameterf(_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, _PrGpuMem->sHardwareCaps.fMaxAnisotropy));
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blTextureUpdate(IN BLGuid _Tex, IN BLU32 _Layer, IN BLU32 _Level, IN BLEnum _Face, IN BLU32 _XOffset, IN BLU32 _YOffset, IN BLU32 _ZOffset, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, IN BLVoid* _Data)
{
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
	if (!_tex)
		return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GLenum _ifmt, _fmt, _type, _sfmt, _rtfmt;
        _FillTextureFormatGL(_tex->eFormat, &_ifmt, &_fmt, &_type, &_sfmt, &_rtfmt);
        GLenum _target;
        switch (_tex->eTarget)
        {
            case BL_TT_1D:
                _target = GL_TEXTURE_2D;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage2D(_target, _Level, _XOffset, _YOffset, _Width, _Height, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage2D(_target, _Level, _XOffset, _YOffset, _Width, _Height, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }
                break;
            case BL_TT_2D:
                _target = GL_TEXTURE_2D;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage2D(_target, _Level, _XOffset, _YOffset, _Width, _Height, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage2D(_target, _Level, _XOffset, _YOffset, _Width, _Height, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }
                break;
            case BL_TT_3D:
                _target = GL_TEXTURE_3D;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage3D(_target, _Level, _XOffset, _YOffset, _ZOffset, _Width, _Height, _Depth, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _Level, _XOffset, _YOffset, _ZOffset, _Width, _Height, _Depth, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }
                break;
            case BL_TT_CUBE:
                _target = GL_TEXTURE_CUBE_MAP;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _Face, _Level, _XOffset, _YOffset, _Width, _Height, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _Face, _Level, _XOffset, _YOffset, _Width, _Height, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }

                break;
            case BL_TT_ARRAY1D:
                _target = GL_TEXTURE_2D_ARRAY;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage3D(_target, _Level, _XOffset, _YOffset, _Layer, _Width, _Height, _Depth, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _Level, _XOffset, _YOffset, _Layer, _Width, _Height, _Depth, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }
                break;
            case BL_TT_ARRAY2D:
                _target = GL_TEXTURE_2D_ARRAY;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage3D(_target, _Level, _XOffset, _YOffset, _Layer, _Width, _Height, _Depth, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _Level, _XOffset, _YOffset, _Layer, _Width, _Height, _Depth, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }
                break;
            case BL_TT_ARRAYCUBE:
                _target = GL_TEXTURE_CUBE_MAP_ARRAY;
                GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
                GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                if(_tex->eFormat > BL_TF_UNKNOWN)
                {
                    GL_CHECK_INTERNAL(glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _Face, _Level, _XOffset, _YOffset, _Layer, _Width, _Height, _Layer, _fmt, _type, _Data));
                }
                else
                {
                    GL_CHECK_INTERNAL(glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _Face, _Level, _XOffset, _YOffset, _Layer, _Width, _Height, _Layer, _ifmt, _TextureSize(_tex->eFormat, _Width, _Height, 1), _Data));
                }
                break;
            default:break;
        }
        GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blTextureQuery(IN BLGuid _Tex, OUT BLEnum* _Target, OUT BLEnum* _Format, OUT BLU32* _Layer, OUT BLU32* _Mipmap, OUT BLU32* _Width, OUT BLU32* _Height, OUT BLU32* _Depth, OUT BLU32* _Size)
{
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
	if (!_tex)
		return;
    *_Target = _tex->eTarget;
    *_Format = _tex->eFormat;
    *_Layer = _tex->nLayer;
    *_Mipmap = _tex->nNumMips;
    *_Width = _tex->nWidth;
    *_Height = _tex->nHeight;
    *_Depth = _tex->nDepth;
    *_Size = _TextureSize(_tex->eFormat, _tex->nWidth, _tex->nHeight, _tex->nDepth);
}
BLGuid
blGenGeometryBuffer(IN BLU32 _Hash, IN BLEnum _Topology, IN BLBool _Dynamic, IN BLEnum* _Semantic, IN BLEnum* _Decl, IN BLU32 _DeclNum,IN BLVoid* _VBO, IN BLU32 _VBSz, IN BLVoid* _IBO, IN BLU32 _IBSz, IN BLEnum _IBFmt)
{
    _BLGeometryBuffer* _geo = NULL;
    BLBool _cache = FALSE;
    if (!_Dynamic)
    {
        blMutexLock(_PrGpuMem->pBufferCache->pMutex);
        _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pBufferCache, _Hash);
        _geo = _res ? (_BLGeometryBuffer*)_res->pRes : NULL;
        if (_res)
            _res->nRefCount++;
        blMutexUnlock(_PrGpuMem->pBufferCache->pMutex);
    }
	if (!_geo)
	{
		_geo = (_BLGeometryBuffer*)malloc(sizeof(_BLGeometryBuffer));
		memset(_geo, 0, sizeof(_BLGeometryBuffer));
		_cache = !_Dynamic;
	}
	else
		return _geo->nID;
    _geo->eVBTopology = _Topology;
    _geo->eIBFormat = _IBFmt;
    _geo->bDynamic = _Dynamic;
    _geo->nVBSize = _VBSz;
    _geo->nIBSize = _IBSz;
    _geo->aInsSem[0] = _geo->aInsSem[1] = _geo->aInsSem[2] = _geo->aInsSem[3] = BL_SL_INVALID;
    BLU32 _stride = 0;
    for (BLU32 _idx = 0; _idx < _DeclNum; ++_idx)
    {
        switch(_Decl[_idx])
        {
            case BL_VD_BOOLX1: _stride += 1 * 1; break;
            case BL_VD_BOOLX2: _stride += 1 * 2; break;
            case BL_VD_BOOLX3: _stride += 1 * 3; break;
            case BL_VD_BOOLX4: _stride += 1 * 4; break;
            case BL_VD_INTX1: _stride += 4 * 1; break;
            case BL_VD_INTX2: _stride += 4 * 2; break;
            case BL_VD_INTX3: _stride += 4 * 3; break;
            case BL_VD_INTX4: _stride += 4 * 4; break;
            case BL_VD_FLOATX1: _stride += 4 * 1; break;
            case BL_VD_FLOATX2: _stride += 4 * 2; break;
            case BL_VD_FLOATX3: _stride += 4 * 3; break;
            case BL_VD_FLOATX4: _stride += 4 * 4; break;
            default:assert(0);break;
        }
    }
    _geo->nVertexNum = _VBSz / _stride;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        _geo->uData.sGL.nIBHandle = -1;
        _geo->uData.sGL.nInsHandle[0] = _geo->uData.sGL.nInsHandle[1] = _geo->uData.sGL.nInsHandle[2] = _geo->uData.sGL.nInsHandle[3] = -1;
		GL_CHECK_INTERNAL(glGenVertexArrays(1, &_geo->uData.sGL.nVAHandle));
		GL_CHECK_INTERNAL(glBindVertexArray(_geo->uData.sGL.nVAHandle));
        GL_CHECK_INTERNAL(glGenBuffers(1, &_geo->uData.sGL.nVBHandle));
        GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nVBHandle));
        GL_CHECK_INTERNAL(glBufferData(GL_ARRAY_BUFFER, _VBSz, _VBO, _Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        if (_IBFmt != BL_IF_INVALID)
        {
            GL_CHECK_INTERNAL(glGenBuffers(1, &_geo->uData.sGL.nIBHandle));
            GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geo->uData.sGL.nIBHandle));
            GL_CHECK_INTERNAL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IBSz, _IBO, _Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        }
        BLU8* _offset = NULL;
        BLU32 _used[16] = { 0 };
        for (BLU32 _idx = 0; _idx < _DeclNum; ++_idx)
        {
            BLU32 _gtsize;
            GLint _numele;
            GLenum _gtype;
            switch(_Decl[_idx])
            {
                case BL_VD_BOOLX1: _numele = 1; _gtype = GL_BYTE; _gtsize = 1 * 1; break;
                case BL_VD_BOOLX2: _numele = 2; _gtype = GL_BYTE; _gtsize = 2 * 1; break;
                case BL_VD_BOOLX3: _numele = 3; _gtype = GL_BYTE; _gtsize = 3 * 1; break;
                case BL_VD_BOOLX4: _numele = 4; _gtype = GL_BYTE; _gtsize = 4 * 1; break;
                case BL_VD_INTX1: _numele = 1; _gtype = GL_INT; _gtsize = 1 * 4; break;
                case BL_VD_INTX2: _numele = 2; _gtype = GL_INT; _gtsize = 2 * 4; break;
                case BL_VD_INTX3: _numele = 3; _gtype = GL_INT; _gtsize = 3 * 4; break;
                case BL_VD_INTX4: _numele = 4; _gtype = GL_INT; _gtsize = 4 * 4; break;
                case BL_VD_FLOATX1: _numele = 1; _gtype = GL_FLOAT; _gtsize = 1 * 4; break;
                case BL_VD_FLOATX2: _numele = 2; _gtype = GL_FLOAT; _gtsize = 2 * 4; break;
                case BL_VD_FLOATX3: _numele = 3; _gtype = GL_FLOAT; _gtsize = 3 * 4; break;
                case BL_VD_FLOATX4: _numele = 4; _gtype = GL_FLOAT; _gtsize = 4 * 4; break;
                default: assert(0); break;
            }
            GL_CHECK_INTERNAL(glEnableVertexAttribArray(_Semantic[_idx]));
			GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nVBHandle));
            GL_CHECK_INTERNAL(glVertexAttribPointer(_Semantic[_idx], _numele, _gtype, GL_FALSE, _stride, _offset));
            _used[_Semantic[_idx]] = 1;
            _offset += _gtsize;
        }
        for (BLU32 _idx = 0; _idx < 16; ++_idx)
        {
            if (!_used[_idx])
            {
                GL_CHECK_INTERNAL(glDisableVertexAttribArray(_idx));
            }
        }
		GL_CHECK_INTERNAL(glBindVertexArray(0));
		GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		if (_IBFmt != BL_IF_INVALID)
			GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
    if (_cache)
    {
        blMutexLock(_PrGpuMem->pBufferCache->pMutex);
        _BLGpuRes* _res = (_BLGpuRes*)malloc(sizeof(_BLGpuRes));
        _res->nRefCount = 1;
        _res->pRes = _geo;
        blDictInsert(_PrGpuMem->pBufferCache, _Hash, _res);
        blMutexUnlock(_PrGpuMem->pBufferCache->pMutex);
    }
    _geo->nID = blGenGuid(_geo, _Hash);
    return _geo->nID;
}
BLVoid
blDeleteGeometryBuffer(IN BLGuid _GBO)
{
    BLBool _discard = FALSE;
    _BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
	if (!_geo)
		return;
    blMutexLock(_PrGpuMem->pBufferCache->pMutex);
    _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pBufferCache, URIPART_INTERNAL(_geo->nID));
    if (_res)
    {
        _res->nRefCount--;
        if (_res->nRefCount <= 0)
        {
            _discard = TRUE;
			free(_res);
            blDictErase(_PrGpuMem->pBufferCache, URIPART_INTERNAL(_geo->nID));
        }
    }
    else
        _discard = TRUE;
    blMutexUnlock(_PrGpuMem->pBufferCache->pMutex);
    if (!_discard)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glDeleteVertexArrays(1, &_geo->uData.sGL.nVAHandle));
        GL_CHECK_INTERNAL(glDeleteBuffers(1, &_geo->uData.sGL.nVBHandle));
        if (_geo->uData.sGL.nIBHandle != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glDeleteBuffers(1, &_geo->uData.sGL.nIBHandle));
        }
        if (_geo->uData.sGL.nInsHandle[0] != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glDeleteBuffers(1, &_geo->uData.sGL.nInsHandle[0]));
        }
        if (_geo->uData.sGL.nInsHandle[1] != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glDeleteBuffers(1, &_geo->uData.sGL.nInsHandle[1]));
        }
        if (_geo->uData.sGL.nInsHandle[2] != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glDeleteBuffers(1, &_geo->uData.sGL.nInsHandle[2]));
        }
        if (_geo->uData.sGL.nInsHandle[3] != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glDeleteBuffers(1, &_geo->uData.sGL.nInsHandle[3]));
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
	free(_geo);
}
BLGuid
blGainGeometryBuffer(IN BLU32 _Hash)
{
    blMutexLock(_PrGpuMem->pBufferCache->pMutex);
    _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pBufferCache, _Hash);
    blMutexUnlock(_PrGpuMem->pBufferCache->pMutex);
    if (_res)
    {
        _res->nRefCount++;
        return ((_BLGeometryBuffer*)_res->pRes)->nID;
    }
    else
        return INVALID_GUID;
}
BLVoid
blGeometryBufferUpdate(IN BLGuid _GBO, IN BLU32 _VBOffset, IN BLU8* _VBO, IN BLU32 _VBSz, IN BLU32 _IBOffset, IN BLU8* _IBO, IN BLU32 _IBSz)
{
    _BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
	if (!_geo)
		return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nVBHandle));
        GL_CHECK_INTERNAL(glBufferSubData(GL_ARRAY_BUFFER, _VBOffset, _VBSz, _VBO));
        GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        if (_geo->uData.sGL.nIBHandle != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geo->uData.sGL.nIBHandle));
            GL_CHECK_INTERNAL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _IBOffset, _IBSz, _IBO));
            GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blGeometryBufferInstance(IN BLGuid _GBO, IN BLEnum* _Semantic, IN BLEnum* _Decl, IN BLU32 _DeclNum, IN BLU32 _Instance)
{
    if (_DeclNum > 4)
        return;
    _BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
	if (!_geo)
		return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glBindVertexArray(_geo->uData.sGL.nVAHandle));
        for (BLU32 _idx = 0; _idx < _DeclNum; ++_idx)
        {
            GL_CHECK_INTERNAL(glGenBuffers(1, &_geo->uData.sGL.nInsHandle[_idx]));
            BLU32 _vbsz = 0, _numele = 0;
            GLenum _type;
            switch(_Decl[_idx])
            {
                case BL_VD_BOOLX1: _numele = 1; _type = GL_BYTE; _vbsz = 1 * 1; break;
                case BL_VD_BOOLX2: _numele = 2; _type = GL_BYTE; _vbsz = 2 * 1; break;
                case BL_VD_BOOLX3: _numele = 3; _type = GL_BYTE; _vbsz = 3 * 1; break;
                case BL_VD_BOOLX4: _numele = 4; _type = GL_BYTE; _vbsz = 4 * 1; break;
                case BL_VD_INTX1: _numele = 1; _type = GL_INT; _vbsz = 1 * 4; break;
                case BL_VD_INTX2: _numele = 2; _type = GL_INT; _vbsz = 2 * 4; break;
                case BL_VD_INTX3: _numele = 3; _type = GL_INT; _vbsz = 3 * 4; break;
                case BL_VD_INTX4: _numele = 4; _type = GL_INT; _vbsz = 4 * 4; break;
                case BL_VD_FLOATX1: _numele = 1; _type = GL_FLOAT; _vbsz = 1 * 4; break;
                case BL_VD_FLOATX2: _numele = 2; _type = GL_FLOAT; _vbsz = 2 * 4; break;
                case BL_VD_FLOATX3: _numele = 3; _type = GL_FLOAT; _vbsz = 3 * 4; break;
                case BL_VD_FLOATX4: _numele = 4; _type = GL_FLOAT; _vbsz = 4 * 4; break;
                default: assert(0); break;
            }
            _vbsz *= _Instance;
            GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nInsHandle[_idx]));
            GL_CHECK_INTERNAL(glBufferData(GL_ARRAY_BUFFER, _vbsz, NULL, GL_DYNAMIC_DRAW));
            GL_CHECK_INTERNAL(glEnableVertexAttribArray(_Semantic[_idx]));
            GL_CHECK_INTERNAL(glVertexAttribPointer(_Semantic[_idx], _numele, GL_FLOAT, GL_FALSE, 0, NULL));
            GL_CHECK_INTERNAL(glVertexAttribDivisor(_Semantic[_idx], 1));
            _geo->aInsSem[_idx] = _Semantic[_idx];
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLVoid
blGeometryInstanceUpdate(IN BLGuid _GBO, IN BLEnum _Semantic, IN BLVoid* _Buffer, IN BLU32 _BufferSz)
{
    _BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
	if (!_geo)
		return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        BLU32 _idx = 0;
        for (; _idx < 4; ++_idx)
        {
            if (_geo->aInsSem[_idx] == _Semantic)
                break;
        }
        GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nInsHandle[_idx]));
        GL_CHECK_INTERNAL(glBufferSubData(GL_ARRAY_BUFFER, 0, _BufferSz, _Buffer));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
BLGuid
blGenTechnique(IN BLAnsi* _Filename, IN BLAnsi* _Archive, IN BLBool _ForceCompile, IN BLBool _ContentDir)
{
	BLBool _cache = FALSE;
	_BLTechnique* _tech;
	blMutexLock(_PrGpuMem->pTechCache->pMutex);
	_BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTechCache, blHashUtf8((const BLUtf8*)_Filename));
	_tech = _res ? (_BLTechnique*)_res->pRes : NULL;
	if (_res)
		_res->nRefCount++;
	blMutexUnlock(_PrGpuMem->pTechCache->pMutex);
	if (!_tech)
	{
		_tech = (_BLTechnique*)malloc(sizeof(_BLTechnique));
		_cache = TRUE;
	}
	if (!_cache)
		return _tech->nID;
    for (BLU32 _idx = 0; _idx < 16; ++_idx)
    {
        memset(_tech->aUniformVars[_idx].aName, 0, 128 * sizeof(BLAnsi));
        _tech->aUniformVars[_idx].pVar = NULL;
        memset(_tech->aSamplerVars[_idx].aName, 0, 128 * sizeof(BLAnsi));
        _tech->aSamplerVars[_idx].pTex = NULL;
    }
    BLU32 _hash = blHashUtf8((const BLUtf8*)_Filename);
    BLGuid _stream = INVALID_GUID;
    BLAnsi _path[260] = { 0 };
    BLBool _findbinary = FALSE;
    ezxml_t _doc = NULL;
    const BLAnsi* _vert = NULL, *_frag = NULL, *_geom = NULL, *_tesc = NULL, *_tess = NULL, *_comp = NULL;
    if (!_ForceCompile)
    {
        memset(_path, 0, 260 * sizeof(BLAnsi));
        strcpy(_path, blUserFolderDir());
        strcat(_path, _Filename);
        _stream = blGenStream(_path, NULL);
        _findbinary = (_stream == INVALID_GUID) ? FALSE : TRUE;
    }
    if (!_findbinary)
    {
        if (_Archive)
            _stream = blGenStream(_Filename, _Archive);
        else
        {
            BLAnsi _tmpname[260];
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_UWP)
            strcpy_s(_tmpname, 260, (const BLAnsi*)_Filename);
            strcpy_s(_path, 260, blWorkingDir(_ContentDir));
            strcat_s(_path, 260, _tmpname);
#else
            strcpy(_tmpname, (const BLAnsi*)_Filename);
            strcpy(_path, blWorkingDir(_ContentDir));
            strcat(_path, _tmpname);
#endif
            _stream = blGenStream(_path, NULL);
        }
        _doc = ezxml_parse_str((BLAnsi*)blStreamData(_stream), blStreamLength(_stream));
        const BLAnsi* _tag = NULL;
        switch (_PrGpuMem->sHardwareCaps.eApiType)
        {
            case BL_GL_API: _tag = "GL"; break;
            case BL_DX_API: _tag = "DX"; break;
            case BL_METAL_API: _tag = "MTL"; break;
            case BL_VULKAN_API: _tag = "VK"; break;
        }
        ezxml_t _element = ezxml_child(_doc, _tag);
        _element = ezxml_child(_element, "Vert");
        do {
            if (!strcmp(ezxml_name(_element), "Vert"))
                _vert = ezxml_txt(_element);
            else if (!strcmp(ezxml_name(_element), "Frag"))
                _frag = ezxml_txt(_element);
            else if (!strcmp(ezxml_name(_element), "Geom"))
                _geom = ezxml_txt(_element);
            else if (!strcmp(ezxml_name(_element), "Tesc"))
                _tesc = ezxml_txt(_element);
            else if (!strcmp(ezxml_name(_element), "Tess"))
                _tess = ezxml_txt(_element);
            else if (!strcmp(ezxml_name(_element), "Comp"))
                _comp = ezxml_txt(_element);
            _element = _element->sibling;
        } while (_element);
    }
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        BLAnsi _glslver[64] = { 0 };
        sprintf(_glslver, "#version %d core\n", _PrGpuMem->sHardwareCaps.nApiVersion);
        if (!_findbinary)
        {
            GLint _compiled = 0;
            GLint _linked = 0;
            GLuint _vs = 0, _fs = 0, _gs = 0;
            _tech->uData.sGL.nHandle = glCreateProgram();
            if (_vert)
            {
                BLAnsi* _tmp = (BLAnsi*)alloca(strlen(_glslver) + strlen(_vert) + 1);
                strcpy(_tmp, _glslver);
                strcat(_tmp, _vert);
                _tmp[strlen(_glslver) + strlen(_vert)] = 0;
                _vs = glCreateShader(GL_VERTEX_SHADER);
                GL_CHECK_INTERNAL(glShaderSource(_vs, 1, &_tmp, NULL));
                GL_CHECK_INTERNAL(glCompileShader(_vs));
                GL_CHECK_INTERNAL(glGetShaderiv(_vs, GL_COMPILE_STATUS, &_compiled));
                if (!_compiled)
                {
                    GLint _len = 0;
                    GL_CHECK_INTERNAL(glGetShaderiv(_vs, GL_INFO_LOG_LENGTH, &_len));
                    if (_len)
                    {
                        BLAnsi* _info = (BLAnsi*)alloca((_len+1)*sizeof(BLAnsi));
                        GL_CHECK_INTERNAL(glGetShaderInfoLog(_vs, _len, &_len, _info));
                        blDebugOutput(_info);
                    }
                }
                else
                {
                    GL_CHECK_INTERNAL(glAttachShader(_tech->uData.sGL.nHandle, _vs));
                }
            }
            if (_frag)
            {
                BLAnsi* _tmp = (BLAnsi*)alloca(strlen(_glslver) + strlen(_frag) + 1);
                strcpy(_tmp, _glslver);
                strcat(_tmp, _frag);
                _tmp[strlen(_glslver) + strlen(_frag)] = 0;
                _fs = glCreateShader(GL_FRAGMENT_SHADER);
                GL_CHECK_INTERNAL(glShaderSource(_fs, 1, &_tmp, NULL));
                GL_CHECK_INTERNAL(glCompileShader(_fs));
                GL_CHECK_INTERNAL(glGetShaderiv(_fs, GL_COMPILE_STATUS, &_compiled));
                if (!_compiled)
                {
                    GLint _len = 0;
                    GL_CHECK_INTERNAL(glGetShaderiv(_fs, GL_INFO_LOG_LENGTH, &_len));
                    if (_len)
                    {
                        BLAnsi* _info = (BLAnsi*)alloca((_len+1)*sizeof(BLAnsi));
                        GL_CHECK_INTERNAL(glGetShaderInfoLog(_fs, _len, &_len, _info));
                        blDebugOutput(_info);
                    }
                }
                else
                {
                    GL_CHECK_INTERNAL(glAttachShader(_tech->uData.sGL.nHandle, _fs));
                }
            }
            if (_geom && _PrGpuMem->sHardwareCaps.bGSSupport)
            {
                _gs = glCreateShader(GL_GEOMETRY_SHADER);
                GL_CHECK_INTERNAL(glShaderSource(_gs, 1, &_geom, NULL));
                GL_CHECK_INTERNAL(glCompileShader(_gs));
                GL_CHECK_INTERNAL(glGetShaderiv(_gs, GL_COMPILE_STATUS, &_compiled));
                if (!_compiled)
                {
                    GLint _len = 0;
                    GL_CHECK_INTERNAL(glGetShaderiv(_gs, GL_INFO_LOG_LENGTH, &_len));
                    if (_len)
                    {
                        BLAnsi* _info = (BLAnsi*)alloca((_len+1)*sizeof(BLAnsi));
                        GL_CHECK_INTERNAL(glGetShaderInfoLog(_gs, _len, &_len, _info));
                        blDebugOutput(_info);
                    }
                }
                else
                {
                    GL_CHECK_INTERNAL(glAttachShader(_tech->uData.sGL.nHandle, _gs));
                }
            }
            GL_CHECK_INTERNAL(glProgramParameteri(_tech->uData.sGL.nHandle, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE));
            GL_CHECK_INTERNAL(glLinkProgram(_tech->uData.sGL.nHandle));
            GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_LINK_STATUS, &_linked));
            if (!_linked)
            {
                GLint _len = 0;
                GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_INFO_LOG_LENGTH, &_len));
                if (_len > 0)
                {
                    BLAnsi* _info = (BLAnsi*)alloca((_len+1)*sizeof(BLAnsi));
                    GL_CHECK_INTERNAL(glGetProgramInfoLog(_tech->uData.sGL.nHandle, _len, &_len, _info));
                    blDebugOutput(_info);
                }
            }
            GLint _len = 0;
            GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_PROGRAM_BINARY_LENGTH, &_len));
            if (_len)
            {
                BLU8* _binary = (BLU8*)malloc(_len * sizeof(BLU8) + sizeof(GLenum));
                GL_CHECK_INTERNAL(glGetProgramBinary(_tech->uData.sGL.nHandle, _len, NULL, (GLenum*)_binary, _binary + sizeof(GLenum)));
                blWriteFile(_Filename, _len * sizeof(BLU8) + sizeof(GLenum), _binary);
            }
            if (_vs)
            {
                GL_CHECK_INTERNAL(glDeleteShader(_vs));
            }
            if (_fs)
            {
                GL_CHECK_INTERNAL(glDeleteShader(_fs));
            }
            if (_gs)
            {
                GL_CHECK_INTERNAL(glDeleteShader(_gs));
            }
        }
        else
        {
            GLint _linked = 0;
            _tech->uData.sGL.nHandle = glCreateProgram();
            GLenum _format = 0;
            blStreamRead(_stream, sizeof(GLenum), &_format);
            BLU32 _len = blStreamLength(_stream) - sizeof(GLenum);
            GL_CHECK_INTERNAL(glProgramBinary(_tech->uData.sGL.nHandle, _format, (BLU8*)blStreamData(_stream) + sizeof(GLenum), _len));
            GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_LINK_STATUS, &_linked));
            if (!_linked)
            {
                GLint _len = 0;
                GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_INFO_LOG_LENGTH, &_len));
                if (_len > 0)
                {
                    BLAnsi* _info = (BLAnsi*)alloca((_len+1)*sizeof(BLAnsi));
                    GL_CHECK_INTERNAL(glGetProgramInfoLog(_tech->uData.sGL.nHandle, _len, &_len, _info));
                    blDebugOutput(_info);
                }
            }
        }
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
	if (!_findbinary)
		ezxml_free(_doc);
	if (_cache)
	{
		blMutexLock(_PrGpuMem->pTechCache->pMutex);
		_BLGpuRes* _res = (_BLGpuRes*)malloc(sizeof(_BLGpuRes));
		_res->nRefCount = 1;
		_res->pRes = _tech;
		blDictInsert(_PrGpuMem->pTechCache, _hash, _res);
		blMutexUnlock(_PrGpuMem->pTechCache->pMutex);
	}
    blDeleteStream(_stream);
	_tech->nID = blGenGuid(_tech, _hash);
	return _tech->nID;
}
BLVoid
blDeleteTechnique(IN BLGuid _Tech)
{
	BLBool _discard = FALSE;
    _BLTechnique* _tech = (_BLTechnique*)blGuidAsPointer(_Tech);	
	blMutexLock(_PrGpuMem->pTechCache->pMutex);
	_BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTechCache, URIPART_INTERNAL(_tech->nID));
	if (_res)
	{
		_res->nRefCount--;
		if (_res->nRefCount <= 0)
		{
			_discard = TRUE;
			free(_res);
			blDictErase(_PrGpuMem->pTechCache, URIPART_INTERNAL(_tech->nID));
		}
	}
	else
		_discard = TRUE;
	blMutexUnlock(_PrGpuMem->pTechCache->pMutex);
	if (!_discard)
		return;
    for (BLU32 _idx = 0; _idx < 16; ++_idx)
    {
        if (_tech->aUniformVars[_idx].pVar)
            free(_tech->aUniformVars[_idx].pVar);
    }
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        GL_CHECK_INTERNAL(glDeleteProgram(_tech->uData.sGL.nHandle));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
	free(_tech);
    blDeleteGuid(_Tech);
}
BLVoid
blTechUniform(IN BLGuid _Tech, IN BLEnum _Type, IN BLAnsi* _Name, IN BLVoid* _Data, IN BLU32 _DataSz)
{
    _BLTechnique* _tech = (_BLTechnique*)blGuidAsPointer(_Tech);
    if (!_tech)
        return;
    BLU32 _idx = 0;
    for (_idx = 0; _idx < 16; ++_idx)
    {
        if (_tech->aUniformVars[_idx].aName[0] == 0)
        {
            strcpy(_tech->aUniformVars[_idx].aName, _Name);
            _tech->aUniformVars[_idx].eType = _Type;
            _tech->aUniformVars[_idx].pVar = malloc(_DataSz);
            memcpy(_tech->aUniformVars[_idx].pVar, _Data, _DataSz);
            break;
        }
        else if (!strcmp(_tech->aUniformVars[_idx].aName, _Name))
        {
            _tech->aUniformVars[_idx].eType = _Type;
            _tech->aUniformVars[_idx].pVar = realloc(_tech->aUniformVars[_idx].pVar, _DataSz);
            memcpy(_tech->aUniformVars[_idx].pVar, _Data, _DataSz);
            break;
        }
    }
    switch (_Type)
    {
        case BL_UB_MAT2:
        case BL_UB_MAT3:
        case BL_UB_MAT4:
        case BL_UB_MAT2X3:
        case BL_UB_MAT3X2:
        case BL_UB_MAT2X4:
        case BL_UB_MAT4X2:
        case BL_UB_MAT3X4:
        case BL_UB_MAT4X3:
            _tech->aUniformVars[_idx].nCount = 1;
            break;
        case BL_UB_S32X1:
        case BL_UB_F32X1:
            _tech->aUniformVars[_idx].nCount = _DataSz / 4;
            break;
        case BL_UB_S32X2:
        case BL_UB_F32X2:
            _tech->aUniformVars[_idx].nCount = _DataSz / 8;
            break;
        case BL_UB_S32X3:
        case BL_UB_F32X3:
            _tech->aUniformVars[_idx].nCount = _DataSz / 12;
            break;
        case BL_UB_S32X4:
        case BL_UB_F32X4:
            _tech->aUniformVars[_idx].nCount = _DataSz / 16;
            break;
        default: _tech->aUniformVars[_idx].nCount = 0; break;
    }
}
BLVoid
blTechSampler(IN BLGuid _Tech, IN BLAnsi* _Name, IN BLGuid _Tex, IN BLU32 _Unit)
{
    _BLTechnique* _tech = (_BLTechnique*)blGuidAsPointer(_Tech);
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
    if (!_tech || !_tex)
        return;
    BLU32 _idx = 0;
    for (_idx = 0; _idx < 16; ++_idx)
    {
        if (_tech->aSamplerVars[_idx].aName[0] == 0)
        {
            strcpy(_tech->aSamplerVars[_idx].aName, _Name);
            _tech->aSamplerVars[_idx].eType = BL_UB_SAMPLER;
            _tech->aSamplerVars[_idx].nUnit = _Unit;
            _tech->aSamplerVars[_idx].pTex = _tex;
            break;
        }
        else if (!strcmp(_tech->aSamplerVars[_idx].aName, _Name))
        {
            _tech->aSamplerVars[_idx].eType = BL_UB_SAMPLER;
            _tech->aSamplerVars[_idx].nUnit = _Unit;
            _tech->aSamplerVars[_idx].pTex = _tex;
            break;
        }
    }
}
BLVoid
blDraw(IN BLGuid _Tech, IN BLGuid _GBO, IN BLU32 _Instance)
{
	_PipelineStateRefreshGL();
	_BLTechnique* _tech = (_BLTechnique*)blGuidAsPointer(_Tech);
	_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
		GL_CHECK_INTERNAL(glUseProgram(_tech->uData.sGL.nHandle));
		for (BLU32 _idx = 0; _idx < 16; ++_idx)
		{
			if (_tech->aUniformVars[_idx].aName[0])
			{
				GLint _handle = glGetUniformLocation(_tech->uData.sGL.nHandle, _tech->aUniformVars[_idx].aName);
				void* _data = _tech->aUniformVars[_idx].pVar;
				BLU32 _count = _tech->aUniformVars[_idx].nCount;
				switch (_tech->aUniformVars[_idx].eType)
				{
				case BL_UB_S32X1: GL_CHECK_INTERNAL(glUniform1iv(_handle, _count, (GLint*)_data)); break;
				case BL_UB_S32X2: GL_CHECK_INTERNAL(glUniform2iv(_handle, _count, (GLint*)_data)); break;
				case BL_UB_S32X3: GL_CHECK_INTERNAL(glUniform3iv(_handle, _count, (GLint*)_data)); break;
				case BL_UB_S32X4: GL_CHECK_INTERNAL(glUniform4iv(_handle, _count, (GLint*)_data)); break;
				case BL_UB_F32X1: GL_CHECK_INTERNAL(glUniform1fv(_handle, _count, (GLfloat*)_data)); break;
				case BL_UB_F32X2: GL_CHECK_INTERNAL(glUniform2fv(_handle, _count, (GLfloat*)_data)); break;
				case BL_UB_F32X3: GL_CHECK_INTERNAL(glUniform3fv(_handle, _count, (GLfloat*)_data)); break;
				case BL_UB_F32X4: GL_CHECK_INTERNAL(glUniform4fv(_handle, _count, (GLfloat*)_data)); break;
				case BL_UB_MAT2: GL_CHECK_INTERNAL(glUniformMatrix2fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT3: GL_CHECK_INTERNAL(glUniformMatrix3fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT4: GL_CHECK_INTERNAL(glUniformMatrix4fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT2X3: GL_CHECK_INTERNAL(glUniformMatrix2x3fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT3X2: GL_CHECK_INTERNAL(glUniformMatrix3x2fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT2X4: GL_CHECK_INTERNAL(glUniformMatrix2x4fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT4X2: GL_CHECK_INTERNAL(glUniformMatrix4x2fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT3X4: GL_CHECK_INTERNAL(glUniformMatrix3x4fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				case BL_UB_MAT4X3: GL_CHECK_INTERNAL(glUniformMatrix4x3fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
				default:assert(0); break;
				}
			}
		}
		for (BLU32 _idx = 0; _idx < 16; ++_idx)
		{
			if (_tech->aSamplerVars[_idx].aName[0])
			{
				GLint _handle = glGetUniformLocation(_tech->uData.sGL.nHandle, _tech->aSamplerVars[_idx].aName);
				GL_CHECK_INTERNAL(glActiveTexture(GL_TEXTURE0 + _tech->aSamplerVars[_idx].nUnit));
				_BLTextureBuffer* _tex = (_BLTextureBuffer*)_tech->aSamplerVars[_idx].pTex;
				GLenum _target = GL_TEXTURE_2D;
				switch (_tex->eTarget)
				{
				case BL_TT_1D: _target = GL_TEXTURE_2D;	break;
				case BL_TT_2D: _target = GL_TEXTURE_2D;	break;
				case BL_TT_3D: _target = GL_TEXTURE_3D;	break;
				case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
				case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
				case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
				case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
				default:break;
				}
				GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
				GL_CHECK_INTERNAL(glUniform1i(_handle, _tech->aSamplerVars[_idx].nUnit));
			}
		}
		GLenum _prim;
		switch (_geo->eVBTopology)
		{
		case BL_PT_POINTS: _prim = GL_POINTS; break;
		case BL_PT_LINES: _prim = GL_LINES; break;
		case BL_PI_LINELOOP: _prim = GL_LINE_LOOP; break;
		case BL_PT_LINESTRIP: _prim = GL_LINE_STRIP; break;
		case BL_PT_TRIANGLES: _prim = GL_TRIANGLES; break;
		case BL_PT_TRIANGLESTRIP: _prim = GL_TRIANGLE_STRIP; break;
		case BL_PT_TRIANGLEFAN: _prim = GL_TRIANGLE_FAN; break;
		default: _prim = GL_TRIANGLES; break;
		}
		if (_geo->uData.sGL.nIBHandle != 0xFFFFFFFF)
		{
			GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geo->uData.sGL.nIBHandle));
			GL_CHECK_INTERNAL(glBindVertexArray(_geo->uData.sGL.nVAHandle));
			if (1 == _Instance)
			{
				GL_CHECK_INTERNAL(glDrawElements(_prim, (_geo->eIBFormat == BL_IF_32) ? (_geo->nIBSize >> 2) : (_geo->nIBSize >> 1), (_geo->eIBFormat == BL_IF_32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, NULL));
			}
			else
			{
				GL_CHECK_INTERNAL(glDrawElementsInstanced(_prim, (_geo->eIBFormat == BL_IF_32) ? (_geo->nIBSize >> 2) : (_geo->nIBSize >> 1), (_geo->eIBFormat == BL_IF_32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, NULL, _Instance));
			}
			GL_CHECK_INTERNAL(glBindVertexArray(0));
		}
		else
		{
			GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nVBHandle));
			GL_CHECK_INTERNAL(glBindVertexArray(_geo->uData.sGL.nVAHandle));
			if (1 == _Instance)
			{
				GL_CHECK_INTERNAL(glDrawArrays(_prim, 0, _geo->nVertexNum));
			}
			else
			{
				GL_CHECK_INTERNAL(glDrawArraysInstanced(_prim, 0, _geo->nVertexNum, _Instance));
			}
			GL_CHECK_INTERNAL(glBindVertexArray(0));
		}
		GL_CHECK_INTERNAL(glUseProgram(0));
	}
#elif defined(BL_MTL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
	{
	}
#elif defined(BL_VK_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
#elif defined(BL_DX_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
	{
	}
#endif
}
BLVoid
blDraw2(IN BLGuid _Tech, IN BLGuid _GBO, IN BLU32 _Instance, int debug)
{
    _PipelineStateRefreshGL();
    _BLTechnique* _tech = (_BLTechnique*)blGuidAsPointer(_Tech);
    _BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {

		GL_CHECK_INTERNAL(glUseProgram(_tech->uData.sGL.nHandle));		
        for (BLU32 _idx = 0; _idx < 16; ++_idx)
        {
            if (_tech->aUniformVars[_idx].aName[0])
            {
                GLint _handle = glGetUniformLocation(_tech->uData.sGL.nHandle, _tech->aUniformVars[_idx].aName);
                void* _data = _tech->aUniformVars[_idx].pVar;
                BLU32 _count = _tech->aUniformVars[_idx].nCount;
                switch(_tech->aUniformVars[_idx].eType)
                {
                    case BL_UB_S32X1: GL_CHECK_INTERNAL(glUniform1iv(_handle, _count, (GLint*)_data)); break;
                    case BL_UB_S32X2: GL_CHECK_INTERNAL(glUniform2iv(_handle, _count, (GLint*)_data)); break;
                    case BL_UB_S32X3: GL_CHECK_INTERNAL(glUniform3iv(_handle, _count, (GLint*)_data)); break;
                    case BL_UB_S32X4: GL_CHECK_INTERNAL(glUniform4iv(_handle, _count, (GLint*)_data)); break;
                    case BL_UB_F32X1: GL_CHECK_INTERNAL(glUniform1fv(_handle, _count, (GLfloat*)_data)); break;
                    case BL_UB_F32X2: GL_CHECK_INTERNAL(glUniform2fv(_handle, _count, (GLfloat*)_data)); break;
                    case BL_UB_F32X3: GL_CHECK_INTERNAL(glUniform3fv(_handle, _count, (GLfloat*)_data)); break;
                    case BL_UB_F32X4: GL_CHECK_INTERNAL(glUniform4fv(_handle, _count, (GLfloat*)_data)); break;
                    case BL_UB_MAT2: GL_CHECK_INTERNAL(glUniformMatrix2fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT3: GL_CHECK_INTERNAL(glUniformMatrix3fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT4: GL_CHECK_INTERNAL(glUniformMatrix4fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT2X3: GL_CHECK_INTERNAL(glUniformMatrix2x3fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT3X2: GL_CHECK_INTERNAL(glUniformMatrix3x2fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT2X4: GL_CHECK_INTERNAL(glUniformMatrix2x4fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT4X2: GL_CHECK_INTERNAL(glUniformMatrix4x2fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT3X4: GL_CHECK_INTERNAL(glUniformMatrix3x4fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    case BL_UB_MAT4X3: GL_CHECK_INTERNAL(glUniformMatrix4x3fv(_handle, 1, GL_FALSE, (GLfloat*)_data)); break;
                    default:assert(0);break;
                }
            }
        }
        for (BLU32 _idx = 0; _idx < 16; ++_idx)
        {
            if (_tech->aSamplerVars[_idx].aName[0])
            {
                GLint _handle = glGetUniformLocation(_tech->uData.sGL.nHandle, _tech->aSamplerVars[_idx].aName);
                GL_CHECK_INTERNAL(glActiveTexture(GL_TEXTURE0 + _tech->aSamplerVars[_idx].nUnit));
                GL_CHECK_INTERNAL(glUniform1i(_handle, _tech->aSamplerVars[_idx].nUnit));
            }
        }
        GLenum _prim;
        switch (_geo->eVBTopology)
        {
            case BL_PT_POINTS: _prim = GL_POINTS; break;
            case BL_PT_LINES: _prim = GL_LINES; break;
            case BL_PI_LINELOOP: _prim = GL_LINE_LOOP; break;
            case BL_PT_LINESTRIP: _prim = GL_LINE_STRIP; break;
            case BL_PT_TRIANGLES: _prim = GL_TRIANGLES; break;
            case BL_PT_TRIANGLESTRIP: _prim = GL_TRIANGLE_STRIP; break;
            case BL_PT_TRIANGLEFAN: _prim = GL_TRIANGLE_FAN; break;
            default: _prim = GL_TRIANGLES; break;
        }

        if (_geo->uData.sGL.nIBHandle != 0xFFFFFFFF)
        {
            GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geo->uData.sGL.nIBHandle));
			GL_CHECK_INTERNAL(glBindVertexArray(_geo->uData.sGL.nVAHandle));
            if (1 == _Instance)
            {
                GL_CHECK_INTERNAL(glDrawElements(_prim, (_geo->eIBFormat == BL_IF_32) ? (_geo->nIBSize >> 2) : (_geo->nIBSize >> 1), (_geo->eIBFormat == BL_IF_32) ? GL_UNSIGNED_INT :  GL_UNSIGNED_SHORT, NULL));
            }
            else
            {
                GL_CHECK_INTERNAL(glDrawElementsInstanced(_prim, (_geo->eIBFormat == BL_IF_32) ? (_geo->nIBSize >> 2) : (_geo->nIBSize >> 1), (_geo->eIBFormat == BL_IF_32) ? GL_UNSIGNED_INT :  GL_UNSIGNED_SHORT, NULL, _Instance));
            }
			GL_CHECK_INTERNAL(glBindVertexArray(0));
        }
        else
        {
				GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nVBHandle));
				GL_CHECK_INTERNAL(glBindVertexArray(_geo->uData.sGL.nVAHandle));
            if (1 == _Instance)
            {
				if (!debug)
				{
					GL_CHECK_INTERNAL(glDrawArrays(_prim, 0, _geo->nVertexNum));
				//	glFlush();

				}
				
            }
            else
            {
                GL_CHECK_INTERNAL(glDrawArraysInstanced(_prim, 0, _geo->nVertexNum, _Instance));
            }
			GL_CHECK_INTERNAL(glBindVertexArray(0));
        }
		GL_CHECK_INTERNAL(glUseProgram(0));
    }
#elif defined(BL_MTL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
#elif defined(BL_VK_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
    {
    }
#elif defined(BL_DX_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
    {
    }
#endif
}
