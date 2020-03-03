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
#ifndef __gpucommon_h_
#define __gpucommon_h_
#include "dictionary.h"
#include "array.h"
#include "mathematics.h"
#include "internal.h"
#include "../headers/util.h"
#include "../headers/gpu.h"
#include "../headers/system.h"
#include "../headers/streamio.h"
#include "../externals/xml/ezxml.h"
#if defined(BL_PLATFORM_OSX)
#elif defined(BL_PLATFORM_IOS)
#elif defined(BL_PLATFORM_ANDROID)
#    include <EGL/egl.h>
#    include <EGL/eglext.h>
#    include <android/native_window.h>
#elif defined(BL_PLATFORM_WEB)
#    include <EGL/egl.h>
#    include <EGL/eglext.h>
#endif
typedef struct _GpuRes{
    void* pRes;
    BLS32 nRefCount;
}_BLGpuRes;
typedef struct _HardwareCaps{
    BLEnum eApiType;
    BLU32 nApiVersion;
    BLBool bCSSupport;
    BLBool bGSSupport;
    BLBool bTSSupport;
    BLBool bFloatRTSupport;
    BLBool bAnisotropy;
    BLF32 fMaxAnisotropy;
    BLBool aTexFormats[BL_TF_COUNT];
}_BLHardwareCaps;
typedef struct _PipelineState{
    BLEnum eCullMode;
    BLS32 nDepthBias;
    BLF32 fSlopeScaledDepthBias;
    BLBool bScissor;
    BLS32 nScissorX;
    BLS32 nScissorY;
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
    BLBool bRasterStateDirty;
    BLBool bDSStateDirty;
    BLBool bBlendStateDirty;
    union {
#if defined(BL_GL_BACKEND)
        struct _GLPS {
			int dummy;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VKPS {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTLPS{
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DXPS {
        } sDX;
#endif
    } uData;
}_BLPipelineState;
typedef struct _UniformBuffer {
    union {
#if defined(BL_GL_BACKEND)
        struct _GLUBO {
            GLuint nHandle;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VKUBO {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTLUBO {
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DXUBO {
        } sDX;
#endif
    } uData;
    BLU32 nSize;
} _BLUniformBuffer;
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
        struct _GLTBO {
            GLuint nHandle;
            GLuint nRTHandle;
            GLenum eInternalFmt;
            GLenum eType;
            GLenum eFormat;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VKTBO {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTLTBO {
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DXTBO {
        } sDX;
#endif
    } uData;
}_BLTextureBuffer;
typedef struct _FrameBuffer {
    struct _Attachment{
        _BLTextureBuffer* pAttachments;
        BLU32 nFace;
    } aAttachments[8];
    _BLTextureBuffer* pDSAttachment;
    BLU32 nAttachmentIndex;
    BLU32 nWidth;
    BLU32 nHeight;
    union {
#if defined(BL_GL_BACKEND)
        struct _GLFBO {
            GLuint nHandle;
            GLuint nResolveHandle;
            GLint nPrevHandle;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VKFBO {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTLFBO {
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DXFBO {
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
        struct _GLGBO {
            GLuint nVBHandle;
            GLuint nIBHandle;
            GLuint nVAHandle;
            GLuint nInsHandle[4];
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VKGBO {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTLGBO {
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DXGBO {
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
        union {
#if defined(BL_GL_BACKEND)
            struct _GLTCH {
                GLuint nIndices;
                GLint nOffset;
            } sGL;
#elif defined(BL_VK_BACKEND)
            struct _VKTCH {
            } sVK;
#elif defined(BL_MTL_BACKEND)
            struct _MTLTCH {
            } sMTL;
#elif defined(BL_DX_BACKEND)
            struct _DXTCH {
            } sDX;
#endif
        } uData;
    } aUniformVars[16];
    struct _SamplerVar{
        BLAnsi aName[128];
        BLEnum eType;
        BLU32 nUnit;
        BLVoid* pTex;
        union {
#if defined(BL_GL_BACKEND)
            struct _GLSMP {
                GLuint nHandle;
            } sGL;
#elif defined(BL_VK_BACKEND)
            struct _VKSMP {
            } sVK;
#elif defined(BL_MTL_BACKEND)
            struct _MTLSMP {
            } sMTL;
#elif defined(BL_DX_BACKEND)
            struct _DXSMP {
            } sDX;
#endif
        } uData;
    } aSamplerVars[8];
    union {
#if defined(BL_GL_BACKEND)
        struct _GLSMPD {
            GLuint nHandle;
        } sGL;
#elif defined(BL_VK_BACKEND)
        struct _VKSMPD {
        } sVK;
#elif defined(BL_MTL_BACKEND)
        struct _MTLSMPD {
        } sMTL;
#elif defined(BL_DX_BACKEND)
        struct _DXSMPD {
        } sDX;
#endif
    } uData;
}_BLTechnique;
typedef struct _CommandQueue {
#if defined(BL_VK_BACKEND)
    struct _VKCMD {
		int dummy;
    } sVK;
#elif defined(BL_MTL_BACKEND)
    struct _MTLCMD {
    } sMTL;
#elif defined(BL_DX_BACKEND)
    struct _DXCMD {
    } sDX;
#endif
}_BLCommandQueue;

typedef struct _GpuMember {
	_BLHardwareCaps sHardwareCaps;
	_BLPipelineState sPipelineState;
	BLBool bVsync;
	BLDictionary* pTextureCache;
	BLDictionary* pBufferCache;
	BLDictionary* pTechCache;
	_BLUniformBuffer* pUBO;
#if defined(BL_PLATFORM_WIN32)
	HGLRC sGLRC;
	HDC sGLHDC;
#elif defined(BL_PLATFORM_UWP)
#elif defined(BL_PLATFORM_OSX)
	BLVoid* pGLC;
#elif defined(BL_PLATFORM_IOS)
	BLVoid* pGLC;
	GLuint nBackFB;
	GLuint nBackRB;
	GLuint nDepthRB;
#elif defined(BL_PLATFORM_LINUX)
	GLXContext pContext;
	Display* pDisplay;
	Window nWindow;
	BLBool bCtxError;
#elif defined(BL_PLATFORM_ANDROID)
	EGLDisplay pEglDisplay;
	EGLContext pEglContext;
	EGLSurface pEglSurface;
	EGLConfig pEglConfig;
	BLBool bAEPSupport;
#elif defined(BL_PLATFORM_WEB)
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE pContext;
#endif
}_BLGpuMember;
static _BLGpuMember* _PrGpuMem = NULL;
extern const BLAnsi* SHADER_2D;
extern const BLAnsi* SHADER_2D_GLOW;
extern const BLAnsi* SHADER_2D_INSTANCE;
extern const BLAnsi* SHADER_2D_STROKE;
static BLU32
_TextureSize(BLEnum _BLFmt, BLU32 _Width, BLU32 _Height, BLU32 _Depth)
{
	switch (_BLFmt)
	{
	case BL_TF_ASTC:
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
	case BL_TF_D32:
		return _Width * _Height * _Depth * 4;
	case BL_TF_D24S8:
		return _Width * _Height * _Depth * 4;
	default:
		return 0;
	}
}
#endif /* __gpuds_h_ */
