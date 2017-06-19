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
#include "../externals/duktape/duktape.h"
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
typedef struct _UniformBuffer {
	union {
#if defined(BL_GL_BACKEND)
		struct _GL {
			GLuint nHandle;
		} sGL;
#elif defined(BL_VK_BACKEND)
		struct _VK {
		} sVK;
#elif defined(BL_MTL_BACKEND)
		struct _MTL {
		} sMTL;
#elif defined(BL_DX_BACKEND)
		struct _DX {
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
    BLU32 nAttachmentIndex;
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
		union {
#if defined(BL_GL_BACKEND)
			struct _GL {
				GLuint nIndices;
				GLint nOffset;
			} sGL;
#elif defined(BL_VK_BACKEND)
			struct _VK {
			} sVK;
#elif defined(BL_MTL_BACKEND)
			struct _MTL {
			} sMTL;
#elif defined(BL_DX_BACKEND)
			struct _DX {
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
			struct _GL {
				GLuint nHandle;
			} sGL;
#elif defined(BL_VK_BACKEND)
			struct _VK {
			} sVK;
#elif defined(BL_MTL_BACKEND)
			struct _MTL {
			} sMTL;
#elif defined(BL_DX_BACKEND)
			struct _DX {
			} sDX;
#endif
		} uData;
    } aSamplerVars[8];
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
#endif
typedef struct _GpuMember {
	duk_context* pDukContext;
	_BLHardwareCaps sHardwareCaps;
    _BLPipelineState sPipelineState;
	BLF32 fPresentElapsed;
	BLBool bVsync;
	BLU16 nCurFramebufferHandle;
	BLU16 nBackBufferIdx;
    BLDictionary* pTextureCache;
    BLDictionary* pBufferCache;
	BLDictionary* pTechCache;
	_BLUniformBuffer* pUBO;
#if defined(BL_PLATFORM_WIN32)
	HGLRC sGLRC;
	HDC sGLHDC;
#elif defined(BL_PLATFORM_UWP)
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
    GL_CHECK_INTERNAL(glDisable(GL_SCISSOR_TEST));
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
		if (!_PrGpuMem->sPipelineState.nScissorW && !_PrGpuMem->sPipelineState.nScissorH)
		{
			BLF32 _rx, _ry;
			BLU32 _w, _h, _aw, _ah;
			blWindowQuery(&_w, &_h, &_aw, &_ah, &_rx, &_ry);
			GL_CHECK_INTERNAL(glScissor(0, 0, _w, _h));
		}
		else
		{
			BLF32 _rx, _ry;
			BLU32 _w, _h, _aw, _ah;
			blWindowQuery(&_w, &_h, &_aw, &_ah, &_rx, &_ry);
			BLS32 _scissorx = (BLS32)(_PrGpuMem->sPipelineState.nScissorX);
			BLS32 _scissory = (BLS32)(_ah - _PrGpuMem->sPipelineState.nScissorH - _PrGpuMem->sPipelineState.nScissorY);
			BLS32 _scissorw = (BLS32)(_PrGpuMem->sPipelineState.nScissorW);
			BLS32 _scissorh = (BLS32)(_PrGpuMem->sPipelineState.nScissorH);
			GL_CHECK_INTERNAL(glScissor(_scissorx, _scissory, _scissorw, _scissorh));
		}
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
            *_Fmt = 0x80E1;
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
static void
_AllocUBO(_BLUniformBuffer* _UBO)
{
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
		GL_CHECK_INTERNAL(glGenBuffers(1, &_UBO->uData.sGL.nHandle));
		GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, _UBO->uData.sGL.nHandle));
		GL_CHECK_INTERNAL(glBufferData(GL_UNIFORM_BUFFER, _UBO->nSize, NULL, GL_DYNAMIC_DRAW));
		GL_CHECK_INTERNAL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, _UBO->uData.sGL.nHandle));
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
static void
_FreeUBO(_BLUniformBuffer* _UBO)
{
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
		GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
		GL_CHECK_INTERNAL(glDeleteBuffers(1, &_UBO->uData.sGL.nHandle));
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
#elif defined(BL_DX_BACKEND)
static BLVoid
_PipelineStateRefreshDX()
{
}
#elif defined(BL_VK_BACKEND)
#elif defined(BL_MTL_BACKEND)
#else
#endif
static void
_PipelineStateRefresh()
{
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
		_PipelineStateRefreshGL();
#elif defined(BL_MTL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
		_PipelineStateRefreshMTL();
#elif defined(BL_VK_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
		_PipelineStateRefreshVK();
#elif defined(BL_DX_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
		_PipelineStateRefreshDX();
#endif
}
#if defined(BL_PLATFORM_WIN32)
BLVoid
_GpuIntervention(duk_context* _DKC, HWND _Hwnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->pDukContext = _DKC;
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
	_PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));	
	_PrGpuMem->pUBO->nSize = 0;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	BLBool _vkinited = FALSE;
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
				wglMakeCurrent(_PrGpuMem->sGLHDC, 0);
				wglDeleteContext(_PrGpuMem->sGLRC);
				_PrGpuMem->sGLRC = _hwgl;
				wglMakeCurrent(_PrGpuMem->sGLHDC, _PrGpuMem->sGLRC);
				_PrGpuMem->sHardwareCaps.nApiVersion = _attribus[1] * 100 + _attribus[3] * 10;
				break;
			}
		}
		if (_PrGpuMem->bVsync)
			wglSwapIntervalEXT(1);
		else if (!_PrGpuMem->bVsync)
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
	_FreeUBO(_PrGpuMem->pUBO);
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		wglMakeCurrent(_PrGpuMem->sGLHDC, 0);
		wglDeleteContext(_PrGpuMem->sGLRC);
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem->pUBO);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_UWP)
BLVoid
_GpuIntervention(duk_context* _DKC, Windows::UI::Core::CoreWindow^ _Hwnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
}
BLVoid
_GpuSwapBuffer()
{
}
BLVoid
_GpuAnitIntervention()
{
}
#elif defined(BL_PLATFORM_LINUX)
static BLS32
_CtxErrorHandler(Display*, XErrorEvent*)
{
    _PrGpuMem->bCtxError = TRUE;
    return 0;
}
BLVoid
_GpuIntervention(duk_context* _DKC, Display* _Display, Window _Window, GLXFBConfig _Config, BLVoid* _Lib, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->pDukContext = _DKC;
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
	_PrGpuMem->bCtxError = FALSE;
	_PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));
	_PrGpuMem->pUBO->nSize = 0;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	BLBool _vkinited = FALSE;
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
				blDebugOutput("Opengl %d.%d boost", _versions[_idx][0], _versions[_idx][1]);
				_glversion = _versions[_idx][0] * 10 + _versions[_idx][1];
				glXMakeContextCurrent(_Display, _Window, _Window, _PrGpuMem->pContext);
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
	_FreeUBO(_PrGpuMem->pUBO);
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		wglMakeCurrent(_PrGpuMem->sGLHDC, 0);
		wglDeleteContext(_PrGpuMem->sGLRC);
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem->pUBO);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_ANDROID)
BLVoid
_GpuIntervention(duk_context* _DKC, ANativeWindow* _Wnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->pDukContext = _DKC;
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
	_PrGpuMem->sHardwareCaps.bGSSupport = FALSE;
	_PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
	_PrGpuMem->sHardwareCaps.bTessellationSupport = TRUE;
	_PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
	_PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
	_PrGpuMem->pTextureCache = blGenDict(TRUE);
	_PrGpuMem->pBufferCache = blGenDict(TRUE);
	_PrGpuMem->pTechCache = blGenDict(TRUE);
	_PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));
	_PrGpuMem->pUBO->nSize = 0;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	BLBool _vkinited = FALSE;
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC1] = _TextureFormatValidGL(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4) & _TextureFormatValidGL(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC3] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8) & _TextureFormatValidGL(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2] = _TextureFormatValidGL(GL_COMPRESSED_RGB8_ETC2, GL_ZERO, GL_ZERO, 4) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A1] = _TextureFormatValidGL(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_ZERO, GL_ZERO, 4);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2A] = _TextureFormatValidGL(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_BC5] = _TextureFormatValidGL(GL_COMPRESSED_RG_RGTC2, GL_ZERO, GL_ZERO, 8);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ETC2RG] = _TextureFormatValidGL(GL_COMPRESSED_RG11_EAC, GL_ZERO, GL_ZERO, 8);
    }
}
BLVoid
_GpuSwapBuffer()
{
}
BLVoid
_GpuAnitIntervention()
{
	_FreeUBO(_PrGpuMem->pUBO);
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
		eglMakeCurrent(_PrGpuMem->pEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglContext);
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem->pUBO);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_OSX)
BLVoid
_GpuIntervention(duk_context* _DKC, NSView* _View, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->pDukContext = _DKC;
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
	_PrGpuMem->pGLC = nil;
	_PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));
	_PrGpuMem->pUBO->nSize = 0;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	BLBool _vkinited = FALSE;
    if (_vkinited)
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_VULKAN_API;
    }
    else
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
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
	_FreeUBO(_PrGpuMem->pUBO);
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
    {
        [NSOpenGLContext clearCurrentContext];
        [_PrGpuMem->pGLC update];
        [_PrGpuMem->pGLC release];
    }
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem->pUBO);
	free(_PrGpuMem);
}
#elif defined(BL_PLATFORM_IOS)
BLVoid
_GpuIntervention(duk_context* _DKC, BLBool _Vsync)
{
	_PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
	_PrGpuMem->pDukContext = _DKC;
	_PrGpuMem->fPresentElapsed = 0.f;
	_PrGpuMem->bVsync = _Vsync;
	_PrGpuMem->nCurFramebufferHandle = 0xFFFF;
	_PrGpuMem->nBackBufferIdx = 0;
	_PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
	_PrGpuMem->sHardwareCaps.bGSSupport = FALSE;
	_PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
	_PrGpuMem->sHardwareCaps.bTessellationSupport = TRUE;
	_PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
	_PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
	_PrGpuMem->pTextureCache = blGenDict(TRUE);
	_PrGpuMem->pBufferCache = blGenDict(TRUE);
	_PrGpuMem->pTechCache = blGenDict(TRUE);
	_PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));
	_PrGpuMem->pUBO->nSize = 0;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
	BLBool _vkinited = FALSE;
}
BLVoid
_GpuSwapBuffer()
{
}
BLVoid
_GpuAnitIntervention()
{
	_FreeUBO(_PrGpuMem->pUBO);
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
	}
	else
	{
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTextureCache)
		{
			_BLTextureBuffer* _tex = (_BLTextureBuffer*)_iter->pRes;
			blDebugOutput("detected texture resource leak: hash>%u", URIPART_INTERNAL(_tex->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pBufferCache)
		{
			_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)_iter->pRes;
			blDebugOutput("detected geometry buffer resource leak: hash>%u", URIPART_INTERNAL(_geo->nID));
		}
	}
	{
		FOREACH_DICT(_BLGpuRes*, _iter, _PrGpuMem->pTechCache)
		{
			_BLTechnique* _tech = (_BLTechnique*)_iter->pRes;
			blDebugOutput("detected technique resource leak: hash>%u", URIPART_INTERNAL(_tech->nID));
		}
	}
	blDeleteDict(_PrGpuMem->pTechCache);
	blDeleteDict(_PrGpuMem->pTextureCache);
	blDeleteDict(_PrGpuMem->pBufferCache);
	free(_PrGpuMem->pUBO);
	free(_PrGpuMem);
}
#else
#   "error what's the fucking platform"
#endif
BLVoid 
blVSync(IN BLBool _On)
{
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
blHardwareCapsQuery(OUT BLEnum* _Api, OUT BLU32* _MaxTexSize, OUT BLU32* _MaxFramebuffer, OUT BLBool _TexSupport[BL_TF_COUNT], IN BLBool _Force)
{
	*_Api = _PrGpuMem->sHardwareCaps.eApiType;
	*_MaxTexSize = _PrGpuMem->sHardwareCaps.nMaxTextureSize;
	*_MaxFramebuffer = _PrGpuMem->sHardwareCaps.nMaxFBAttachments;
	for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
		_TexSupport[_idx] = _PrGpuMem->sHardwareCaps.aTexFormats[_idx];
}
BLVoid
blRasterState(IN BLEnum _CullMode, IN BLS32 _DepthBias, IN BLF32 _SlopeScaledDepthBias, IN BLBool _Scissor, IN BLS32 _XPos, IN BLS32 _YPos, IN BLU32 _Width, IN BLU32 _Height, IN BLBool _Force)
{
    if (_PrGpuMem->sPipelineState.eCullMode != _CullMode ||
        _PrGpuMem->sPipelineState.nDepthBias != _DepthBias ||
        !blScalarApproximate(_PrGpuMem->sPipelineState.fSlopeScaledDepthBias, _SlopeScaledDepthBias) ||
        _PrGpuMem->sPipelineState.bScissor != _Scissor ||
        _PrGpuMem->sPipelineState.nScissorX != _XPos ||
        _PrGpuMem->sPipelineState.nScissorY != _YPos ||
        _PrGpuMem->sPipelineState.nScissorW != _Width ||
        _PrGpuMem->sPipelineState.nScissorH != _Height)
    {
        _PrGpuMem->sPipelineState.eCullMode = _CullMode;
        _PrGpuMem->sPipelineState.nDepthBias = _DepthBias;
        _PrGpuMem->sPipelineState.fSlopeScaledDepthBias = _SlopeScaledDepthBias;
        _PrGpuMem->sPipelineState.bScissor = _Scissor;
        _PrGpuMem->sPipelineState.nScissorX = _XPos;
		_PrGpuMem->sPipelineState.nScissorY = _YPos;
		_PrGpuMem->sPipelineState.nScissorW = _Width;
		_PrGpuMem->sPipelineState.nScissorH = _Height;
        _PrGpuMem->sPipelineState.bRasterStateDirty = TRUE;
    }
	else if (_Force)
	{
		_PrGpuMem->sPipelineState.eCullMode = _CullMode;
		_PrGpuMem->sPipelineState.nDepthBias = _DepthBias;
		_PrGpuMem->sPipelineState.fSlopeScaledDepthBias = _SlopeScaledDepthBias;
		_PrGpuMem->sPipelineState.bScissor = _Scissor;
		_PrGpuMem->sPipelineState.nScissorX = _XPos;
		_PrGpuMem->sPipelineState.nScissorY = _YPos;
		_PrGpuMem->sPipelineState.nScissorW = _Width;
		_PrGpuMem->sPipelineState.nScissorH = _Height;
		_PrGpuMem->sPipelineState.bRasterStateDirty = TRUE;
		_PipelineStateRefresh();
	}
}
BLVoid
blDepthStencilState(IN BLBool _Depth, IN BLBool _Mask, IN BLEnum _DepthCompFunc, IN BLBool _Stencil, IN BLU8 _StencilReadMask, IN BLU8 _StencilWriteMask, IN BLEnum _FrontStencilFailOp, IN BLEnum _FrontStencilDepthFailOp, IN BLEnum _FrontStencilPassOp, IN BLEnum _FrontStencilCompFunc, IN BLEnum _BackStencilFailOp, IN BLEnum _BackStencilDepthFailOp, IN BLEnum _BackStencilPassOp, IN BLEnum _BackStencilCompFunc, IN BLBool _Force)
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
	else if (_Force)
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
		_PipelineStateRefresh();	
	}
}
BLVoid
blBlendState(IN BLBool _AlphaToCoverage, IN BLBool _Blend, IN BLEnum _SrcBlendFactor, IN BLEnum _DestBlendFactor, IN BLEnum _SrcBlendAlphaFactor, IN BLEnum _DestBlendAlphaFactor, IN BLEnum _BlendOp, IN BLEnum _BlendOpAlpha, IN BLU8 _BlendFactor[4], IN BLBool _Force)
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
	else if (_Force)
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
		_PipelineStateRefresh();
	}
}
BLGuid
blGenFrameBuffer()
{
    _BLFrameBuffer* _fbo = (_BLFrameBuffer*)malloc(sizeof(_BLFrameBuffer));
    memset(_fbo, 0, sizeof(_BLFrameBuffer));
    _fbo->nAttachmentIndex = 0;
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
blBindFrameBuffer(IN BLGuid _FBO)
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
	BLU32 _w, _h, _aw, _ah;
	BLF32 _rx, _ry;
	blWindowQuery(&_w, &_h, &_aw, &_ah, &_rx, &_ry);
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
		if (_fbo)
		{
			GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
			_BLTextureBuffer* _tex = _fbo->aAttachments[0].pAttachments;
			if (_tex)
			{
				GL_CHECK_INTERNAL(glViewport(0, 0, _tex->nWidth, _tex->nHeight));
			}			
		}
		else
		{
			GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			GL_CHECK_INTERNAL(glViewport(0, 0, _w, _h));
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
blFrameBufferClear(IN BLBool _ColorBit, IN BLBool _DepthBit, IN BLBool _StencilBit)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
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
			GLenum _buffers[10];
			BLU32 _idx = 0;
			for (BLU32 _jdx = 0; _jdx < _fbo->nAttachmentIndex; ++_jdx)
				_buffers[_idx++] = GL_COLOR_ATTACHMENT0 + _jdx;
			if (_fbo->pDSAttachment)
			{
				if (_fbo->pDSAttachment->eTarget == BL_TF_D24S8)
					_buffers[_idx++] = GL_DEPTH_STENCIL_ATTACHMENT;
				else
					_buffers[_idx++] = GL_DEPTH_ATTACHMENT;
			}
#if defined(BL_PLATFORM_WIN32) || defined(BL_PLATFORM_OSX) || defined(BL_PLATFORM_LINUX)
            if (_PrGpuMem->sHardwareCaps.nApiVersion >= 430)
            {
                GL_CHECK_INTERNAL(glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, _idx, _buffers));
            }
#else
            GL_CHECK_INTERNAL(glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, _idx, _buffers));
#endif
			GL_CHECK_INTERNAL(glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
			GL_CHECK_INTERNAL(glReadBuffer(GL_NONE));
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
        for (BLU32 _idx = 0; _idx < _fbo->nAttachmentIndex; ++_idx)
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
        if (0 == _fbo->nAttachmentIndex)
        {
            _fbo->nWidth  = MAX_INTERNAL(_width  >> _Level, 1);
            _fbo->nHeight = MAX_INTERNAL(_height >> _Level, 1);
        }
        GLenum _attachment;
        if (_format < BL_TF_DS_UNKNOWN)
        {
            _attachment = GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex;
            _fbo->aAttachments[_fbo->nAttachmentIndex].pAttachments = _tex;
            _fbo->aAttachments[_fbo->nAttachmentIndex].nFace = _CFace;
            _fbo->aAttachments[_fbo->nAttachmentIndex].nMip = _Level;
            _fbo->nAttachmentIndex++;
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
            GL_CHECK_INTERNAL(glGenFramebuffers(1, &_fbo->uData.sGL.nResolveHandle));
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nResolveHandle));
            for (BLU32 _idx = 0; _idx < _fbo->nAttachmentIndex; ++_idx)
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
    return _fbo->nAttachmentIndex;
}
BLVoid
blFrameBufferDetach(IN BLGuid _FBO, IN BLBool _DepthStencil)
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
		else
			_fbo->nAttachmentIndex--;
        GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
        _BLTextureBuffer* _tex = _fbo->aAttachments[_fbo->nAttachmentIndex].pAttachments;
        if (_fbo->uData.sGL.nResolveHandle)
        {
            GL_CHECK_INTERNAL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex, GL_RENDERBUFFER, 0));
        }
        else
        {
            GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _fbo->aAttachments[_fbo->nAttachmentIndex].nFace : GL_TEXTURE_2D;
            GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex, _target, 0, _fbo->aAttachments[_fbo->nAttachmentIndex].nMip));
        }
        if (_fbo->uData.sGL.nResolveHandle)
        {
            GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nResolveHandle));
            GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _fbo->aAttachments[_fbo->nAttachmentIndex].nFace : GL_TEXTURE_2D;
            GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex, _target, 0, _fbo->aAttachments[_fbo->nAttachmentIndex].nMip));
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
		GL_CHECK_INTERNAL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        if (_Immutable)
        {
            switch (_Target)
            {
            case BL_TT_1D:
                GL_CHECK_INTERNAL(glTexStorage2D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height));
                break;
            case BL_TT_2D:
                GL_CHECK_INTERNAL(glTexStorage2D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height));
                break;
            case BL_TT_3D:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height, _Depth));
                break;
            case BL_TT_CUBE:
                GL_CHECK_INTERNAL(glTexStorage2D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height));
                break;
            case BL_TT_ARRAY1D:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height, _Layer));
                break;
            case BL_TT_ARRAY2D:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height, _Layer));
                break;
            case BL_TT_ARRAYCUBE:
                GL_CHECK_INTERNAL(glTexStorage3D(_target, _Mipmap, !_Srgb ? _ifmt : _sfmt, _Width, _Height, _Layer));
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
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAX_LEVEL, _Mipmap - 1));
		BLU32 _faces = (_target == GL_TEXTURE_CUBE_MAP) || (_target == GL_TEXTURE_CUBE_MAP_ARRAY) ? 6 : 1;
		for (BLU32 _aidx = 0; _aidx < _Layer; ++_aidx)
		{
			BLU32 _w = _Width;
			BLU32 _h = _Height;
			BLU32 _d = _Depth;
			for (BLU32 _face = 0; _face < _faces; ++_face)
			{
				for (BLU32 _level = 0; _level < _Mipmap; ++_level)
				{
					GLsizei _imagesz = _TextureSize(_Format, _w, _h, _d);
					switch (_target)
					{
					case GL_TEXTURE_2D:
					{
						if (_Format > BL_TF_UNKNOWN)
						{
							if (_Immutable)
							{
								GL_CHECK_INTERNAL(glTexSubImage2D(_target, _level, 0, 0, _w, _h, _fmt, _type, _Data + _aidx * _Mipmap + _level));
							}
							else
							{
								GL_CHECK_INTERNAL(glTexImage2D(_target, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, 0, _fmt, _type, _Data + _aidx * _Mipmap + _level));
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
								GL_CHECK_INTERNAL(glCompressedTexImage2D(_target, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, 0, _imagesz, _Data + _aidx * _Mipmap + _level));
							}
						}
					}
					break;
					case GL_TEXTURE_2D_ARRAY:
					{
						if (_Format > BL_TF_UNKNOWN)
						{
							if (0 == _aidx && !_Immutable)
							{
								GL_CHECK_INTERNAL(glTexImage3D(_target, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, _Layer, 0, _fmt, _type, 0));
							}
							GL_CHECK_INTERNAL(glTexSubImage3D(_target, _level, 0, 0, _aidx, _w, _h, 1, _fmt, _type, _Data + _aidx * _Mipmap + _level));
						}
						else
						{
							if (0 == _aidx && !_Immutable)
							{
								GL_CHECK_INTERNAL(glCompressedTexImage3D(_target, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, _Layer, 0, _imagesz*_Layer, 0));
							}
							GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _level, 0, 0, _aidx, _w, _h, 1, _ifmt, _imagesz, _Data + _aidx * _Mipmap + _level));
						}
					}
					break;
					case GL_TEXTURE_CUBE_MAP:
					{
						if (_Format > BL_TF_UNKNOWN)
						{
							if (_Immutable)
							{
								GL_CHECK_INTERNAL(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _w, _h, _fmt, _type, _Data + _aidx * _Mipmap + _level));
							}
							else
							{
								GL_CHECK_INTERNAL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, 0, _fmt, _type, _Data + _aidx * _Mipmap + _level));
							}
						}
						else
						{
							if (_Immutable)
							{
								GL_CHECK_INTERNAL(glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _w, _h, !_Srgb ? _ifmt : _sfmt, _imagesz, _Data + _aidx * _Mipmap + _level));
							}
							else
							{
								GL_CHECK_INTERNAL(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, 0, _imagesz, _Data + _aidx * _Mipmap + _level));
							}
						}
					}
					break;
					case GL_TEXTURE_CUBE_MAP_ARRAY:
					{
						if (_Format > BL_TF_UNKNOWN)
						{
							if (0 == _aidx)
							{
								GL_CHECK_INTERNAL(glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, _Layer, 0, _fmt, _type, 0));
							}
							GL_CHECK_INTERNAL(glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _aidx, _w, _h, 1, _fmt, _type, _Data + _aidx * _Mipmap + _level));
						}
						else
						{
							if (0 == _aidx)
							{
								GL_CHECK_INTERNAL(glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, _Layer, 0, _imagesz * _Layer, 0));
							}
							GL_CHECK_INTERNAL(glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _face, _level, 0, 0, _aidx, _w, _h, 1, !_Srgb ? _ifmt : _sfmt, _imagesz, _Data + _aidx * _Layer + _level));
						}
					}
					break;
					case GL_TEXTURE_3D:
					{
						if (_Format > BL_TF_UNKNOWN)
						{
							if (_Immutable)
							{
								GL_CHECK_INTERNAL(glTexSubImage3D(_target, _level, 0, 0, 0, _w, _h, _d, _fmt, _type, _Data + _level));
							}
							else
							{
								GL_CHECK_INTERNAL(glTexImage3D(_target, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, _d, 0, _fmt, _type, _Data + _level));
							}
						}
						else
						{
							if (_Immutable)
							{
								GL_CHECK_INTERNAL(glCompressedTexSubImage3D(_target, _level, 0, 0, 0, _w, _h, _d, !_Srgb ? _ifmt : _sfmt, _imagesz, _Data + _level));
							}
							else
							{
								GL_CHECK_INTERNAL(glCompressedTexImage3D(_target, _level, !_Srgb ? _ifmt : _sfmt, _w, _h, _d, 0, _imagesz, _Data + _level));
							}
						}
					}
					break;
					default:assert(0); break;
					}
					_w = MAX_INTERNAL(1U, _w / 2);
					_h = MAX_INTERNAL(1U, _h / 2);
					_d = MAX_INTERNAL(1U, _d / 2);
				}
			}
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
    if (_res)
    {
        _res->nRefCount++;
		blMutexUnlock(_PrGpuMem->pTextureCache->pMutex);
        return ((_BLTextureBuffer*)_res->pRes)->nID;
    }
	else
	{
		blMutexUnlock(_PrGpuMem->pTextureCache->pMutex);
		return INVALID_GUID;
	}
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
            _minfilter = (_tex->nNumMips >= 1) ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
        else
            _minfilter = (_tex->nNumMips >= 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
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
blTextureSwizzle(IN BLGuid _Tex, IN BLEnum _ChannelR, IN BLEnum _ChannelG, IN BLEnum _ChannelB, IN BLEnum _ChannelA)
{
	_BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
	if (!_tex)
		return;
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
		GLenum _target;
		GLint _swizzle[4];
		switch (_ChannelR)
		{
		case BL_TS_ZERO: _swizzle[0] = GL_ZERO; break;
		case BL_TS_ONE: _swizzle[0] = GL_ONE; break;
		case BL_TS_RED: _swizzle[0] = GL_RED; break;
		case BL_TS_GREEN: _swizzle[0] = GL_GREEN; break;
		case BL_TS_BLUE: _swizzle[0] = GL_BLUE; break;
		case BL_TS_ALPHA: _swizzle[0] = GL_ALPHA; break;
		default: _swizzle[0] = GL_ZERO; break;
		}
		switch (_ChannelG)
		{
		case BL_TS_ZERO: _swizzle[1] = GL_ZERO; break;
		case BL_TS_ONE: _swizzle[1] = GL_ONE; break;
		case BL_TS_RED: _swizzle[1] = GL_RED; break;
		case BL_TS_GREEN: _swizzle[1] = GL_GREEN; break;
		case BL_TS_BLUE: _swizzle[1] = GL_BLUE; break;
		case BL_TS_ALPHA: _swizzle[1] = GL_ALPHA; break;
		default: _swizzle[1] = GL_ZERO; break;
		}
		switch (_ChannelB)
		{
		case BL_TS_ZERO: _swizzle[2] = GL_ZERO; break;
		case BL_TS_ONE: _swizzle[2] = GL_ONE; break;
		case BL_TS_RED: _swizzle[2] = GL_RED; break;
		case BL_TS_GREEN: _swizzle[2] = GL_GREEN; break;
		case BL_TS_BLUE: _swizzle[2] = GL_BLUE; break;
		case BL_TS_ALPHA: _swizzle[2] = GL_ALPHA; break;
		default: _swizzle[2] = GL_ZERO; break;
		}
		switch (_ChannelA)
		{
		case BL_TS_ZERO: _swizzle[3] = GL_ZERO; break;
		case BL_TS_ONE: _swizzle[3] = GL_ONE; break;
		case BL_TS_RED: _swizzle[3] = GL_RED; break;
		case BL_TS_GREEN: _swizzle[3] = GL_GREEN; break;
		case BL_TS_BLUE: _swizzle[3] = GL_BLUE; break;
		case BL_TS_ALPHA: _swizzle[3] = GL_ALPHA; break;
		default: _swizzle[3] = GL_ZERO; break;
		}
		switch (_tex->eTarget)
		{
		case BL_TT_1D: _target = GL_TEXTURE_2D;	break;
		case BL_TT_2D: _target = GL_TEXTURE_2D;	break;
		case BL_TT_3D: _target = GL_TEXTURE_3D;	break;
		case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
		case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
		case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
		case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;		
		default: _target = GL_TEXTURE_2D; break;
		}
		GL_CHECK_INTERNAL(glBindTexture(_target, _tex->uData.sGL.nHandle));
		GL_CHECK_INTERNAL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, _swizzle[0]));
		GL_CHECK_INTERNAL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, _swizzle[1]));
		GL_CHECK_INTERNAL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, _swizzle[2]));
		GL_CHECK_INTERNAL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, _swizzle[3]));
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
		if (_Hash == 0xFFFFFFFF)
		{
			GL_CHECK_INTERNAL(glBufferData(GL_ARRAY_BUFFER, _VBSz, _VBO, GL_STATIC_DRAW));
		}
		else
		{
			GL_CHECK_INTERNAL(glBufferData(GL_ARRAY_BUFFER, _VBSz, _VBO, _Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
		}        
        if (_IBFmt != BL_IF_INVALID)
        {
            GL_CHECK_INTERNAL(glGenBuffers(1, &_geo->uData.sGL.nIBHandle));
            GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geo->uData.sGL.nIBHandle));
			if (_Hash == 0xFFFFFFFF)
			{
				GL_CHECK_INTERNAL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IBSz, _IBO, GL_STATIC_DRAW));
			}
			else
			{
				GL_CHECK_INTERNAL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IBSz, _IBO, _Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
			}
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
	if (URIPART_INTERNAL(_geo->nID) == 0xFFFFFFFF)
		_discard = TRUE;
	else
	{
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
	}
    if (!_discard)
        return;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
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
		GL_CHECK_INTERNAL(glDeleteVertexArrays(1, &_geo->uData.sGL.nVAHandle));
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
	blDeleteGuid(_GBO);
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
blGenTechnique(IN BLAnsi* _Filename, IN BLBool _ForceCompile)
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
#if defined(BL_GL_BACKEND)
		if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
		{
			_tech->aUniformVars[_idx].uData.sGL.nIndices = 0xFFFFFFFF;
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
	for (BLU32 _idx = 0; _idx < 8; ++_idx)
	{
		memset(_tech->aSamplerVars[_idx].aName, 0, 128 * sizeof(BLAnsi));
		_tech->aSamplerVars[_idx].pTex = NULL;
#if defined(BL_GL_BACKEND)
		if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
		{
			_tech->aSamplerVars[_idx].uData.sGL.nHandle = 0xFFFFFFFF;
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
    BLU32 _hash = blHashUtf8((const BLUtf8*)_Filename);
    BLGuid _stream = INVALID_GUID;
    BLAnsi _path[260] = { 0 };
    BLBool _findbinary = FALSE;
    ezxml_t _doc = NULL;
    const BLAnsi* _vert = NULL, *_frag = NULL, *_geom = NULL, *_tesc = NULL, *_tess = NULL, *_comp = NULL;
    if (!_ForceCompile)
    {
        memset(_path, 0, 260 * sizeof(BLAnsi));
        strcpy(_path, "b");
        strcat(_path, _Filename);
        _stream = blGenStream(_path);
        _findbinary = (_stream == INVALID_GUID) ? FALSE : TRUE;
    }
    if (!_findbinary)
    {
        _stream = blGenStream(_Filename);
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
				memset(_path, 0, 260 * sizeof(BLAnsi));
				strcpy(_path, "b");
				strcat(_path, _Filename);
                blFileWrite(_path, _len * sizeof(BLU8) + sizeof(GLenum), _binary);
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
BLGuid 
blGainTechnique(IN BLU32 _Hash)
{
	blMutexLock(_PrGpuMem->pTechCache->pMutex);
    _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTechCache, _Hash);
    blMutexUnlock(_PrGpuMem->pTechCache->pMutex);
    if (_res)
    {
        _res->nRefCount++;
        return ((_BLTechnique*)_res->pRes)->nID;
    }
    else
        return INVALID_GUID;
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
    for (_idx = 0; _idx < 8; ++_idx)
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
	_BLTechnique* _tech = (_BLTechnique*)blGuidAsPointer(_Tech);
	_BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
#if defined(BL_GL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
	{
		GL_CHECK_INTERNAL(glEnable(GL_SCISSOR_TEST));
		_PipelineStateRefresh();
		if (!_PrGpuMem->pUBO->nSize)
		{
			GLuint _uboidx = glGetUniformBlockIndex(_tech->uData.sGL.nHandle, "UBO");
			GLint _size;
			GL_CHECK_INTERNAL(glGetActiveUniformBlockiv(_tech->uData.sGL.nHandle, _uboidx, GL_UNIFORM_BLOCK_DATA_SIZE, &_size));
			_PrGpuMem->pUBO->nSize = (BLU32)_size;
			_AllocUBO(_PrGpuMem->pUBO);
			GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, _PrGpuMem->pUBO->uData.sGL.nHandle));
		}
		else
		{
			GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, _PrGpuMem->pUBO->uData.sGL.nHandle));
		}		
		for (BLU32 _idx = 0; _idx < 16; ++_idx)
		{
			if (_tech->aUniformVars[_idx].aName[0])
			{
				BLVoid* _data = _tech->aUniformVars[_idx].pVar;
				BLU32 _count = _tech->aUniformVars[_idx].nCount;
				if (_tech->aUniformVars[_idx].uData.sGL.nIndices == 0xFFFFFFFF)
				{
					const GLchar* _name = _tech->aUniformVars[_idx].aName;
					GL_CHECK_INTERNAL(glGetUniformIndices(_tech->uData.sGL.nHandle, 1, &_name, &_tech->aUniformVars[_idx].uData.sGL.nIndices));
					GL_CHECK_INTERNAL(glGetActiveUniformsiv(_tech->uData.sGL.nHandle, _count, &_tech->aUniformVars[_idx].uData.sGL.nIndices, GL_UNIFORM_OFFSET, &_tech->aUniformVars[_idx].uData.sGL.nOffset));
					GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, _PrGpuMem->pUBO->uData.sGL.nHandle));
				}								
				switch (_tech->aUniformVars[_idx].eType)
				{
				case BL_UB_S32X1: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 1 * sizeof(BLS32), (GLint*)_data)); break;
				case BL_UB_S32X2: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 2 * sizeof(BLS32), (GLint*)_data)); break;
				case BL_UB_S32X3: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 3 * sizeof(BLS32), (GLint*)_data)); break;
				case BL_UB_S32X4: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 4 * sizeof(BLS32), (GLint*)_data)); break;
				case BL_UB_F32X1: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 1 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_F32X2: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 2 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_F32X3: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 3 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_F32X4: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, _count * 5 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT2: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 4 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT3: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 9 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT4: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 16 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT2X3: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 6 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT3X2: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 6 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT2X4: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 8 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT4X2: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 8 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT3X4: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 12 * sizeof(BLF32), (GLfloat*)_data)); break;
				case BL_UB_MAT4X3: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 12* sizeof(BLF32), (GLfloat*)_data)); break;
				default:assert(0); break;
				}
			}
			else
				break;
		}
		GL_CHECK_INTERNAL(glUseProgram(_tech->uData.sGL.nHandle));
		for (BLU32 _idx = 0; _idx < 8; ++_idx)
		{
			if (_tech->aSamplerVars[_idx].aName[0])
			{
				if (_tech->aSamplerVars[_idx].uData.sGL.nHandle == 0xFFFFFFFF)
					_tech->aSamplerVars[_idx].uData.sGL.nHandle = glGetUniformLocation(_tech->uData.sGL.nHandle, _tech->aSamplerVars[_idx].aName);
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
				GL_CHECK_INTERNAL(glUniform1i(_tech->aSamplerVars[_idx].uData.sGL.nHandle, _tech->aSamplerVars[_idx].nUnit));
			}
			else
				break;
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
		GL_CHECK_INTERNAL(glDisable(GL_SCISSOR_TEST));
	}
#elif defined(BL_MTL_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
	{
		_PipelineStateRefresh();
	}
#elif defined(BL_VK_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_VULKAN_API)
	{
		_PipelineStateRefresh();
	}
#elif defined(BL_DX_BACKEND)
	if (_PrGpuMem->sHardwareCaps.eApiType == BL_DX_API)
	{
		_PipelineStateRefresh();
	}
#endif
}
BLVoid 
blFlush()
{
	_GpuSwapBuffer();
}
