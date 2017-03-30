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
#ifndef __gpu_h_
#define __gpu_h_
#include "prerequisites.h"
#ifdef __cplusplus
extern "C" {
#endif
BL_API BLVoid blVSync(
	IN BLBool _On);

BL_API BLVoid blQueryHardwareCaps(
	OUT BLEnum* _Api,
	OUT BLU32* _MaxTexSize,
	OUT BLU32* _MaxFramebuffer,
    OUT BLBool _TexSupport[BL_TF_COUNT]);
    
/**
     Default Value
     _CullMode BL_CM_CW
     _DepthBias 0
     _SlopeScaledDepthBias 0.f
     _Scissor TRUE
     */
    
BL_API BLVoid blRasterState(
    IN BLEnum _CullMode,
    IN BLS32 _DepthBias,
    IN BLF32 _SlopeScaledDepthBias,
    IN BLBool _Scissor,
    IN BLU32 _X,
    IN BLU32 _Y,
    IN BLU32 _W,
    IN BLU32 _H);

/**
     Default Value
     _Depth TRUE
     _Mask TRUE
     _DepthCompFunc BL_CF_LESS
     _Stencil FALSE
     _StencilReadMask 0xFF
     _StencilWriteMask 0xFF
     _FrontStencilFailOp BL_SO_KEEP
     _FrontStencilDepthFailOp BL_SO_KEEP
     _FrontStencilPassOp BL_SO_KEEP
     _FrontStencilCompFunc BL_CF_ALWAYS
     _BackStencilFailOp BL_SO_KEEP
     _BackStencilDepthFailOp BL_SO_KEEP
     _BackStencilPassOp BL_SO_KEEP
     _BackStencilCompFunc BL_CF_ALWAYS
     */
BL_API BLVoid blDepthStencilState(
    IN BLBool _Depth,
    IN BLBool _Mask,
    IN BLEnum _DepthCompFunc,
    IN BLBool _Stencil,
    IN BLU8 _StencilReadMask,
    IN BLU8 _StencilWriteMask,
    IN BLEnum _FrontStencilFailOp,
    IN BLEnum _FrontStencilDepthFailOp,
    IN BLEnum _FrontStencilPassOp,
    IN BLEnum _FrontStencilCompFunc,
    IN BLEnum _BackStencilFailOp,
    IN BLEnum _BackStencilDepthFailOp,
    IN BLEnum _BackStencilPassOp,
    IN BLEnum _BackStencilCompFunc);
    
/**
     Default Value
     _AlphaToCoverage FALSE
     _Blend FALSE
     _SrcBlendFactor BL_BF_ONE
     _DestBlendFactor BL_BF_ZERO
     _SrcBlendAlphaFactor BL_BF_ONE
     _DestBlendAlphaFactor BL_BF_ZERO
     _BlendOp BL_BO_ADD
     _BlendOpAlpha BL_BO_ADD
     */
BL_API BLVoid blBlendState(
    IN BLBool _AlphaToCoverage,
    IN BLBool _Blend,
    IN BLEnum _SrcBlendFactor,
    IN BLEnum _DestBlendFactor,
    IN BLEnum _SrcBlendAlphaFactor,
    IN BLEnum _DestBlendAlphaFactor,
    IN BLEnum _BlendOp,
    IN BLEnum _BlendOpAlpha,
    IN BLU8 _BlendFactor[4]);
    
BL_API BLGuid blGenFrameBuffer();

BL_API BLVoid blDeleteFrameBuffer(
	IN BLGuid _FBO);
    
BL_API BLVoid blFrameBufferClear(
    IN BLGuid _FBO,
    IN BLBool _ColorBit,
    IN BLBool _DepthBit,
    IN BLBool _StencilBit);
    
BL_API BLVoid blFrameBufferResolve(
    IN BLGuid _FBO);
    
BL_API BLU32 blFrameBufferAttach(
    IN BLGuid _FBO,
	IN BLGuid _Tex,
    IN BLS32 _Level,
    IN BLEnum _CFace);

BL_API BLVoid blFrameBufferDetach(
    IN BLGuid _FBO,
    IN BLU32 _Index,
    IN BLBool _DepthStencil);

BL_API BLGuid blGenTexture(
    IN BLU32 _Hash,
	IN BLEnum _Target,
	IN BLEnum _Format,
    IN BLBool _Srgb, 
    IN BLBool _Immutable,
    IN BLBool _RenderTarget,
    IN BLU32 _Layer,
    IN BLU32 _Mipmap,
    IN BLU32 _Width,
    IN BLU32 _Height,
    IN BLU32 _Depth,
    IN BLU8* _Data);

BL_API BLVoid blDeleteTexture(
	IN BLGuid _Tex);
    
BL_API BLGuid blGainTexture(
    IN BLU32 _Hash);
    
BL_API BLVoid blTextureFilter(
    IN BLGuid _Tex,
    IN BLEnum _MinFilter,
    IN BLEnum _MagFilter,
    IN BLEnum _WrapS,
    IN BLEnum _WrapT,
    IN BLBool _Anisotropy);

BL_API BLVoid blTextureSwizzle(
	IN BLGuid _Tex,
	IN BLEnum _ChannelR,
	IN BLEnum _ChannelG,
	IN BLEnum _ChannelB,
	IN BLEnum _ChannelA);

BL_API BLVoid blTextureUpdate(
	IN BLGuid _Tex,
    IN BLU32 _Layer,
    IN BLU32 _Level,
    IN BLEnum _Face,
    IN BLU32 _XOffset,
    IN BLU32 _YOffset,
    IN BLU32 _ZOffset,
	IN BLU32 _Width, 
	IN BLU32 _Height, 
	IN BLU32 _Depth,
	IN BLVoid* _Data);

BL_API BLVoid blTextureQuery(
	IN BLGuid _Tex,
	OUT BLEnum* _Target,
    OUT BLEnum* _Format,
    OUT BLU32* _Layer,
    OUT BLU32* _Mipmap,
	OUT BLU32* _Width,
	OUT BLU32* _Height,
	OUT BLU32* _Depth,
	OUT BLU32* _Size);
    
BL_API BLGuid blGenGeometryBuffer(
    IN BLU32 _Hash,
    IN BLEnum _Topology,
    IN BLBool _Dynamic,
    IN BLEnum* _Semantic,
    IN BLEnum* _Decl,
    IN BLU32 _DeclNum,
    IN BLVoid* _VBO,
    IN BLU32 _VBSz,
    IN BLVoid* _IBO,
    IN BLU32 _IBSz,
    IN BLEnum _IBFmt);
    
BL_API BLVoid blDeleteGeometryBuffer(
    IN BLGuid _GBO);
    
BL_API BLGuid blGainGeometryBuffer(
    IN BLU32 _Hash);
    
BL_API BLVoid blGeometryBufferUpdate(
    IN BLGuid _GBO,
    IN BLU32 _VBOffset,
    IN BLU8* _VBO,
    IN BLU32 _VBSz,
    IN BLU32 _IBOffset,
    IN BLU8* _IBO,
    IN BLU32 _IBSz);
    
BL_API BLVoid blGeometryBufferInstance(
    IN BLGuid _GBO,
    IN BLEnum* _Semantic,
    IN BLEnum* _Decl,
    IN BLU32 _DeclNum,
    IN BLU32 _Instance);

BL_API BLVoid blGeometryInstanceUpdate(
    IN BLGuid _GBO,
    IN BLEnum _Semantic,
    IN BLVoid* _Buffer,
    IN BLU32 _BufferSz);

BL_API BLGuid blGenTechnique(
    IN BLAnsi* _Filename,
    IN BLAnsi* _Archive,
    IN BLBool _ForceCompile,
    IN BLBool _ContentDir);
    
BL_API BLVoid blDeleteTechnique(
    IN BLGuid _Tech);
    
BL_API BLVoid blTechUniform(
    IN BLGuid _Tech,
    IN BLEnum _Type,
    IN BLAnsi* _Name,
    IN BLVoid* _Data,
    IN BLU32 _DataSz);
    
BL_API BLVoid blTechSampler(
    IN BLGuid _Tech,
    IN BLAnsi* _Name,
    IN BLGuid _Tex,
    IN BLU32 _Unit);
    
BL_API BLVoid blDraw(
    IN BLGuid _Tech,
    IN BLGuid _GBO,
    IN BLU32 _Instance);
#ifdef __cplusplus
}
#endif
#endif/*__gpu_h_*/
