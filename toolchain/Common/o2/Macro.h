/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MACRO_H_INCLUDE
#define MACRO_H_INCLUDE
#include "Prerequisites.h"
#define FRIEND_CLASS(cla) friend class cla;

#define SIGNALS public

#ifdef O2D_COMPILER_MSVC
#	define THREAD_FUNC u32 static __stdcall
#else
#	define THREAD_FUNC static void*
#endif


#ifndef O2D_PLATFORM_WINDOWS
#	define SOCKET_ERROR            (-1)
#endif

#define WAKEUP_FUNC void static

#define LOG  c_system::get_singleton_ptr()->log

#define TIME c_system::get_singleton_ptr()->time_elapsed()

#define DECL_NETCMD(name) name(){memset(this,0,sizeof(*this));id = hash(#name);size = sizeof(*this);}

#define FOREACH(ref , var , mother) for(ref var = mother.begin() ; var != mother.end() ; ++var)

#define RFOREACH(ref , var , mother) for(ref var = mother.rbegin() ; var != mother.rend() ; --var)

#define TYPEOF(...) __VA_ARGS__

#define AUTO(ref , var , exp) ref var = exp

#define THROW(info) throw c_exception(L##info , __FILE__ , __LINE__)

#define GL_CHECK {GLenum err;if((err = glGetError()) != GL_NO_ERROR)LOG(LL_ERROR , "OpenGL error code: %d\n", err);}

#define MAX_VALUE(p1 , p2) ((p1)>(p2)?(p1):(p2))

#define MIN_VALUE(p1 , p2) ((p1)<(p2)?(p1):(p2))

#define FOUR_CC(ch0, ch1, ch2, ch3) ((ch0<<0) + (ch1<<8) + (ch2<<16) + (ch3<<24))

#define PIXEL_ALIGNED(pixel) ((f32)(s32)((pixel) + ((pixel)>0.0f?0.5f:-0.5f)))

#if defined(O2D_PLATFORM_WINDOWS)
#	define I64_FMT "I64"
#elif defined(O2D_PLATFORM_WPHONE) 
#	define I64_FMT "I64"
#elif defined(O2D_PLATFORM_OSX) 
#	define I64_FMT "q"
#elif defined(O2D_PLATFORM_IOS) 
#	define I64_FMT "q"
#elif defined(O2D_PLATFORM_LINUX) 
#	define I64_FMT "ll"
#elif defined(O2D_PLATFORM_ANDROID) 
#	define I64_FMT "ll"
#else
#	define I64_FMT 
#endif

#ifdef O2D_ARCHITECTURE_64
#	define MAKE_U32(l , h) ((u32)(((u16)(((u64)(l)) & 0xffff)) | ((u32)((u16)(((u64)(h)) & 0xffff))) << 16))

#	define LO16_BITS(v) ((u16)(((u64)(v)) & 0xffff))

#	define HI16_BITS(v) ((u16)((((u64)(v)) >> 16) & 0xffff))

#	define MAKE_U16(l , h) ((u16)(((u8)(((u64)(l)) & 0xff)) | ((u16)((u8)(((u64)(h)) & 0xff))) << 8))

#	define LO8_BITS(v) ((u8)(((u64)(v)) & 0xff))

#	define HI8_BITS(v) ((u8)((((u64)(v)) >> 8) & 0xff))
#else
#	define MAKE_U32(l , h) ((u32)(((u16)(((u32)(l)) & 0xFFFF)) | ((u32)((u16)(((u32)(h)) & 0xFFFF))) << 16))

#	define LO16_BITS(v) ((u16)(((u32)(v)) & 0xFFFF))

#	define HI16_BITS(v) ((u16)((((u32)(v)) >> 16) & 0xFFFF))

#	define MAKE_U16(l , h) ((u16)(((u8)(((u32)(l)) & 0xFF)) | ((u16)((u8)(((u32)(h)) & 0xFF))) << 8))

#	define LO8_BITS(v) ((u8)(((u32)(v)) & 0xFF))

#	define HI8_BITS(v) ((u8)((((u32)(v)) >> 8) & 0xFF))
#endif

#if defined(O2D_USE_GL_API)
#	define glPolygonMode c_device::get_singleton_ptr()->get_profile()->procPolygonMode
#	define glCullFace c_device::get_singleton_ptr()->get_profile()->procCullFace
#	define glFrontFace c_device::get_singleton_ptr()->get_profile()->procFrontFace
#	define glHint c_device::get_singleton_ptr()->get_profile()->procHint
#	define glLineWidth c_device::get_singleton_ptr()->get_profile()->procLineWidth
#	define glScissor c_device::get_singleton_ptr()->get_profile()->procScissor
#	define glTexParameterf c_device::get_singleton_ptr()->get_profile()->procTexParameterf
#	define glTexParameterfv c_device::get_singleton_ptr()->get_profile()->procTexParameterfv
#	define glTexParameteri c_device::get_singleton_ptr()->get_profile()->procTexParameteri
#	define glTexParameteriv c_device::get_singleton_ptr()->get_profile()->procTexParameteriv
#	define glTexImage2D c_device::get_singleton_ptr()->get_profile()->procTexImage2D
#	define glClear c_device::get_singleton_ptr()->get_profile()->procClear
#	define glClearColor c_device::get_singleton_ptr()->get_profile()->procClearColor
#	define glClearStencil c_device::get_singleton_ptr()->get_profile()->procClearStencil
#	define glClearDepth c_device::get_singleton_ptr()->get_profile()->procClearDepth
#	define glStencilMask c_device::get_singleton_ptr()->get_profile()->procStencilMask
#	define glColorMask c_device::get_singleton_ptr()->get_profile()->procColorMask
#	define glDepthMask c_device::get_singleton_ptr()->get_profile()->procDepthMask
#	define glDisable c_device::get_singleton_ptr()->get_profile()->procDisable
#	define glEnable c_device::get_singleton_ptr()->get_profile()->procEnable
#	define glFinish c_device::get_singleton_ptr()->get_profile()->procFinish
#	define glFlush c_device::get_singleton_ptr()->get_profile()->procFlush
#	define glBlendFunc c_device::get_singleton_ptr()->get_profile()->procBlendFunc
#	define glStencilFunc c_device::get_singleton_ptr()->get_profile()->procStencilFunc
#	define glStencilOp c_device::get_singleton_ptr()->get_profile()->procStencilOp
#	define glDepthFunc c_device::get_singleton_ptr()->get_profile()->procDepthFunc
#	define glPixelStorei c_device::get_singleton_ptr()->get_profile()->procPixelStorei
#	define glReadPixels c_device::get_singleton_ptr()->get_profile()->procReadPixels
#	define glGetBooleanv c_device::get_singleton_ptr()->get_profile()->procGetBooleanv
#	define glGetError c_device::get_singleton_ptr()->get_profile()->procGetError
#	define glGetFloatv c_device::get_singleton_ptr()->get_profile()->procGetFloatv
#	define glGetIntegerv c_device::get_singleton_ptr()->get_profile()->procGetIntegerv
#	define glGetString c_device::get_singleton_ptr()->get_profile()->procGetString
#	define glGetTexParameterfv c_device::get_singleton_ptr()->get_profile()->procGetTexParameterfv
#	define glGetTexParameteriv c_device::get_singleton_ptr()->get_profile()->procGetTexParameteriv
#	define glIsEnabled c_device::get_singleton_ptr()->get_profile()->procIsEnabled
#	define glViewport c_device::get_singleton_ptr()->get_profile()->procViewport
#	define glDrawArrays c_device::get_singleton_ptr()->get_profile()->procDrawArrays
#	define glDrawElements c_device::get_singleton_ptr()->get_profile()->procDrawElements
#	define glPolygonOffset c_device::get_singleton_ptr()->get_profile()->procPolygonOffset
#	define glCopyTexImage2D c_device::get_singleton_ptr()->get_profile()->procCopyTexImage2D
#	define glCopyTexSubImage2D c_device::get_singleton_ptr()->get_profile()->procCopyTexSubImage2D
#	define glTexSubImage2D c_device::get_singleton_ptr()->get_profile()->procTexSubImage2D
#	define glBindTexture c_device::get_singleton_ptr()->get_profile()->procBindTexture
#	define glDeleteTextures c_device::get_singleton_ptr()->get_profile()->procDeleteTextures
#	define glGenTextures c_device::get_singleton_ptr()->get_profile()->procGenTextures
#	define glIsTexture c_device::get_singleton_ptr()->get_profile()->procIsTexture
#	define glBlendColor c_device::get_singleton_ptr()->get_profile()->procBlendColor
#	define glBlendEquation c_device::get_singleton_ptr()->get_profile()->procBlendEquation
#	define glActiveTexture c_device::get_singleton_ptr()->get_profile()->procActiveTexture
#	define glSampleCoverage c_device::get_singleton_ptr()->get_profile()->procSampleCoverage
#	define glCompressedTexImage2D c_device::get_singleton_ptr()->get_profile()->procCompressedTexImage2D
#	define glCompressedTexSubImage2D c_device::get_singleton_ptr()->get_profile()->procCompressedTexSubImage2D
#	define glBlendFuncSeparate c_device::get_singleton_ptr()->get_profile()->procBlendFuncSeparate
#	define glBindBuffer c_device::get_singleton_ptr()->get_profile()->procBindBuffer
#	define glDeleteBuffers c_device::get_singleton_ptr()->get_profile()->procDeleteBuffers
#	define glGenBuffers c_device::get_singleton_ptr()->get_profile()->procGenBuffers
#	define glIsBuffer c_device::get_singleton_ptr()->get_profile()->procIsBuffer
#	define glBufferData c_device::get_singleton_ptr()->get_profile()->procBufferData
#	define glBufferSubData c_device::get_singleton_ptr()->get_profile()->procBufferSubData
#	define glGetBufferParameteriv c_device::get_singleton_ptr()->get_profile()->procGetBufferParameteriv
#	define glBlendEquationSeparate c_device::get_singleton_ptr()->get_profile()->procBlendEquationSeparate
#	define glStencilOpSeparate c_device::get_singleton_ptr()->get_profile()->procStencilOpSeparate
#	define glStencilFuncSeparate c_device::get_singleton_ptr()->get_profile()->procStencilFuncSeparate
#	define glStencilMaskSeparate c_device::get_singleton_ptr()->get_profile()->procStencilMaskSeparate
#	define glAttachShader c_device::get_singleton_ptr()->get_profile()->procAttachShader
#	define glBindAttribLocation c_device::get_singleton_ptr()->get_profile()->procBindAttribLocation
#	define glCompileShader c_device::get_singleton_ptr()->get_profile()->procCompileShader
#	define glCreateProgram c_device::get_singleton_ptr()->get_profile()->procCreateProgram
#	define glCreateShader c_device::get_singleton_ptr()->get_profile()->procCreateShader
#	define glDeleteProgram c_device::get_singleton_ptr()->get_profile()->procDeleteProgram
#	define glDeleteShader c_device::get_singleton_ptr()->get_profile()->procDeleteShader
#	define glDetachShader c_device::get_singleton_ptr()->get_profile()->procDetachShader
#	define glDisableVertexAttribArray c_device::get_singleton_ptr()->get_profile()->procDisableVertexAttribArray
#	define glEnableVertexAttribArray c_device::get_singleton_ptr()->get_profile()->procEnableVertexAttribArray
#	define glGetActiveAttrib c_device::get_singleton_ptr()->get_profile()->procGetActiveAttrib
#	define glGetActiveUniform c_device::get_singleton_ptr()->get_profile()->procGetActiveUniform
#	define glGetAttachedShaders c_device::get_singleton_ptr()->get_profile()->procGetAttachedShaders
#	define glGetAttribLocation c_device::get_singleton_ptr()->get_profile()->procGetAttribLocation
#	define glGetProgramiv c_device::get_singleton_ptr()->get_profile()->procGetProgramiv
#	define glGetProgramInfoLog c_device::get_singleton_ptr()->get_profile()->procGetProgramInfoLog
#	define glGetShaderiv c_device::get_singleton_ptr()->get_profile()->procGetShaderiv
#	define glGetShaderInfoLog c_device::get_singleton_ptr()->get_profile()->procGetShaderInfoLog
#	define glGetShaderSource c_device::get_singleton_ptr()->get_profile()->procGetShaderSource
#	define glGetUniformLocation c_device::get_singleton_ptr()->get_profile()->procGetUniformLocation
#	define glGetUniformfv c_device::get_singleton_ptr()->get_profile()->procGetUniformfv
#	define glGetUniformiv c_device::get_singleton_ptr()->get_profile()->procGetUniformiv
#	define glGetVertexAttribfv c_device::get_singleton_ptr()->get_profile()->procGetVertexAttribfv
#	define glGetVertexAttribiv c_device::get_singleton_ptr()->get_profile()->procGetVertexAttribiv
#	define glGetVertexAttribPointerv c_device::get_singleton_ptr()->get_profile()->procGetVertexAttribPointerv
#	define glIsProgram c_device::get_singleton_ptr()->get_profile()->procIsProgram
#	define glIsShader c_device::get_singleton_ptr()->get_profile()->procIsShader
#	define glLinkProgram c_device::get_singleton_ptr()->get_profile()->procLinkProgram
#	define glShaderSource c_device::get_singleton_ptr()->get_profile()->procShaderSource
#	define glUseProgram c_device::get_singleton_ptr()->get_profile()->procUseProgram
#	define glUniform1f c_device::get_singleton_ptr()->get_profile()->procUniform1f
#	define glUniform2f c_device::get_singleton_ptr()->get_profile()->procUniform2f
#	define glUniform3f c_device::get_singleton_ptr()->get_profile()->procUniform3f
#	define glUniform4f c_device::get_singleton_ptr()->get_profile()->procUniform4f
#	define glUniform1i c_device::get_singleton_ptr()->get_profile()->procUniform1i
#	define glUniform2i c_device::get_singleton_ptr()->get_profile()->procUniform2i
#	define glUniform3i c_device::get_singleton_ptr()->get_profile()->procUniform3i
#	define glUniform4i c_device::get_singleton_ptr()->get_profile()->procUniform4i
#	define glUniform1fv c_device::get_singleton_ptr()->get_profile()->procUniform1fv
#	define glUniform2fv c_device::get_singleton_ptr()->get_profile()->procUniform2fv
#	define glUniform3fv c_device::get_singleton_ptr()->get_profile()->procUniform3fv
#	define glUniform4fv c_device::get_singleton_ptr()->get_profile()->procUniform4fv
#	define glUniform1iv c_device::get_singleton_ptr()->get_profile()->procUniform1iv
#	define glUniform2iv c_device::get_singleton_ptr()->get_profile()->procUniform2iv
#	define glUniform3iv c_device::get_singleton_ptr()->get_profile()->procUniform3iv
#	define glUniform4iv c_device::get_singleton_ptr()->get_profile()->procUniform4iv
#	define glUniformMatrix2fv c_device::get_singleton_ptr()->get_profile()->procUniformMatrix2fv
#	define glUniformMatrix3fv c_device::get_singleton_ptr()->get_profile()->procUniformMatrix3fv
#	define glUniformMatrix4fv c_device::get_singleton_ptr()->get_profile()->procUniformMatrix4fv
#	define glValidateProgram c_device::get_singleton_ptr()->get_profile()->procValidateProgram
#	define glVertexAttrib1f c_device::get_singleton_ptr()->get_profile()->procVertexAttrib1f
#	define glVertexAttrib1fv c_device::get_singleton_ptr()->get_profile()->procVertexAttrib1fv
#	define glVertexAttrib2f c_device::get_singleton_ptr()->get_profile()->procVertexAttrib2f
#	define glVertexAttrib2fv c_device::get_singleton_ptr()->get_profile()->procVertexAttrib2fv
#	define glVertexAttrib3f c_device::get_singleton_ptr()->get_profile()->procVertexAttrib3f
#	define glVertexAttrib3fv c_device::get_singleton_ptr()->get_profile()->procVertexAttrib3fv
#	define glVertexAttrib4f c_device::get_singleton_ptr()->get_profile()->procVertexAttrib4f
#	define glVertexAttrib4fv c_device::get_singleton_ptr()->get_profile()->procVertexAttrib4fv
#	define glVertexAttribPointer c_device::get_singleton_ptr()->get_profile()->procVertexAttribPointer
#	define glIsRenderbuffer c_device::get_singleton_ptr()->get_profile()->procIsRenderbuffer
#	define glBindRenderbuffer c_device::get_singleton_ptr()->get_profile()->procBindRenderbuffer
#	define glDeleteRenderbuffers c_device::get_singleton_ptr()->get_profile()->procDeleteRenderbuffers
#	define glGenRenderbuffers c_device::get_singleton_ptr()->get_profile()->procGenRenderbuffers
#	define glRenderbufferStorage c_device::get_singleton_ptr()->get_profile()->procRenderbufferStorage
#	define glGetRenderbufferParameteriv c_device::get_singleton_ptr()->get_profile()->procGetRenderbufferParameteriv
#	define glIsFramebuffer c_device::get_singleton_ptr()->get_profile()->procIsFramebuffer
#	define glBindFramebuffer c_device::get_singleton_ptr()->get_profile()->procBindFramebuffer
#	define glDeleteFramebuffers c_device::get_singleton_ptr()->get_profile()->procDeleteFramebuffers
#	define glGenFramebuffers c_device::get_singleton_ptr()->get_profile()->procGenFramebuffers
#	define glCheckFramebufferStatus c_device::get_singleton_ptr()->get_profile()->procCheckFramebufferStatus
#	define glFramebufferTexture2D c_device::get_singleton_ptr()->get_profile()->procFramebufferTexture2D
#	define glFramebufferRenderbuffer c_device::get_singleton_ptr()->get_profile()->procFramebufferRenderbuffer
#	define glGetFramebufferAttachmentParameteriv c_device::get_singleton_ptr()->get_profile()->procGetFramebufferAttachmentParameteriv
#	define glGenerateMipmap c_device::get_singleton_ptr()->get_profile()->procGenerateMipmap
#	define glReleaseShaderCompiler c_device::get_singleton_ptr()->get_profile()->procReleaseShaderCompiler
#	define glShaderBinary c_device::get_singleton_ptr()->get_profile()->procShaderBinary
#	define glGetShaderPrecisionFormat c_device::get_singleton_ptr()->get_profile()->procGetShaderPrecisionFormat
#else
#	if defined(O2D_PLATFORM_WINDOWS)
#		define ID3D11DeviceN ID3D11Device
#		define IDXGISwapChainN IDXGISwapChain
#		define IDXGIFactoryN IDXGIFactory1
#		define IDXGIAdapterN IDXGIAdapter1
#		define ID3D11DeviceContextN ID3D11DeviceContext
#		define DXGI_SWAP_CHAIN_DESCN DXGI_SWAP_CHAIN_DESC
#	else
#		define ID3D11DeviceN ID3D11Device1
#		define IDXGISwapChainN IDXGISwapChain1
#		define IDXGIFactoryN IDXGIFactory2
#		define IDXGIAdapterN IDXGIAdapter1
#		define ID3D11DeviceContextN ID3D11DeviceContext1
#		define DXGI_SWAP_CHAIN_DESCN DXGI_SWAP_CHAIN_DESC1
#	endif
#endif

#endif