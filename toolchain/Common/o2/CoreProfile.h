/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef COREPROFILE_H_INCLUDE
#define COREPROFILE_H_INCLUDE
#include "Prerequisites.h"
#include "glcorearb.h"
namespace o2d{	
#if defined(O2D_USE_GL_API)
	class O2D_API c_core_profile
	{
	private:
#if defined(O2D_PLATFORM_WINDOWS)
		HMODULE m_lib;
#elif defined(O2D_PLATFORM_LINUX)
		void* m_lib;
#else
		CFBundleRef m_lib;
#endif
		GLint m_major;
		GLint m_minor;
	public:
		c_core_profile();
		virtual ~c_core_profile();
	public:
		bool make();
	private:
		void _load_procs();
		void* _get_proc(const ansi* proc_);
	public:
		PFNGLPOLYGONMODEPROC procPolygonMode;
		PFNGLACTIVETEXTUREPROC procActiveTexture;
		PFNGLATTACHSHADERPROC procAttachShader;
		PFNGLBINDATTRIBLOCATIONPROC procBindAttribLocation;
		PFNGLBINDBUFFERPROC procBindBuffer;
		PFNGLBINDFRAMEBUFFERPROC procBindFramebuffer;
		PFNGLBINDRENDERBUFFERPROC procBindRenderbuffer;
		PFNGLBINDTEXTUREPROC procBindTexture;
		PFNGLBLENDCOLORPROC procBlendColor;
		PFNGLBLENDEQUATIONPROC procBlendEquation;
		PFNGLBLENDEQUATIONSEPARATEPROC procBlendEquationSeparate;
		PFNGLBLENDFUNCPROC procBlendFunc;
		PFNGLBLENDFUNCSEPARATEPROC procBlendFuncSeparate;
		PFNGLBUFFERDATAPROC procBufferData;
		PFNGLBUFFERSUBDATAPROC procBufferSubData;
		PFNGLCHECKFRAMEBUFFERSTATUSPROC procCheckFramebufferStatus;
		PFNGLCLEARPROC procClear;
		PFNGLCLEARCOLORPROC procClearColor;
		PFNGLCLEARDEPTHPROC procClearDepth;
		PFNGLCLEARSTENCILPROC procClearStencil;
		PFNGLCOLORMASKPROC procColorMask;
		PFNGLCOMPILESHADERPROC procCompileShader;
		PFNGLCOMPRESSEDTEXIMAGE2DPROC procCompressedTexImage2D;
		PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC procCompressedTexSubImage2D;
		PFNGLCOPYTEXIMAGE2DPROC procCopyTexImage2D;
		PFNGLCOPYTEXSUBIMAGE2DPROC procCopyTexSubImage2D;
		PFNGLCREATEPROGRAMPROC procCreateProgram;
		PFNGLCREATESHADERPROC procCreateShader;
		PFNGLCULLFACEPROC procCullFace;
		PFNGLDELETEBUFFERSPROC procDeleteBuffers;
		PFNGLDELETEFRAMEBUFFERSPROC procDeleteFramebuffers;
		PFNGLDELETEPROGRAMPROC procDeleteProgram;
		PFNGLDELETERENDERBUFFERSPROC procDeleteRenderbuffers;
		PFNGLDELETESHADERPROC procDeleteShader;
		PFNGLDELETETEXTURESPROC procDeleteTextures;
		PFNGLDEPTHFUNCPROC procDepthFunc;
		PFNGLDEPTHMASKPROC procDepthMask;
		PFNGLDETACHSHADERPROC procDetachShader;
		PFNGLDISABLEPROC procDisable;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC procDisableVertexAttribArray;
		PFNGLDRAWARRAYSPROC procDrawArrays;
		PFNGLDRAWELEMENTSPROC procDrawElements;
		PFNGLENABLEPROC procEnable;
		PFNGLENABLEVERTEXATTRIBARRAYPROC procEnableVertexAttribArray;
		PFNGLFINISHPROC procFinish;
		PFNGLFLUSHPROC procFlush;
		PFNGLFRAMEBUFFERRENDERBUFFERPROC procFramebufferRenderbuffer;
		PFNGLFRAMEBUFFERTEXTURE2DPROC procFramebufferTexture2D;
		PFNGLFRONTFACEPROC procFrontFace;
		PFNGLGENBUFFERSPROC procGenBuffers;
		PFNGLGENFRAMEBUFFERSPROC procGenFramebuffers;
		PFNGLGENRENDERBUFFERSPROC procGenRenderbuffers;
		PFNGLGENTEXTURESPROC procGenTextures;
		PFNGLGENERATEMIPMAPPROC procGenerateMipmap;
		PFNGLGETBOOLEANVPROC procGetBooleanv;
		PFNGLGETFLOATVPROC procGetFloatv;
		PFNGLGETINTEGERVPROC procGetIntegerv;
		PFNGLGETACTIVEATTRIBPROC procGetActiveAttrib;
		PFNGLGETACTIVEUNIFORMPROC procGetActiveUniform;
		PFNGLGETATTACHEDSHADERSPROC procGetAttachedShaders;
		PFNGLGETATTRIBLOCATIONPROC procGetAttribLocation;
		PFNGLGETBUFFERPARAMETERIVPROC procGetBufferParameteriv;
		PFNGLGETERRORPROC procGetError;
		PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC procGetFramebufferAttachmentParameteriv;
		PFNGLGETPROGRAMINFOLOGPROC procGetProgramInfoLog;
		PFNGLGETPROGRAMIVPROC procGetProgramiv;
		PFNGLGETRENDERBUFFERPARAMETERIVPROC procGetRenderbufferParameteriv;
		PFNGLGETSHADERINFOLOGPROC procGetShaderInfoLog;
		PFNGLGETSHADERPRECISIONFORMATPROC procGetShaderPrecisionFormat;
		PFNGLGETSHADERSOURCEPROC procGetShaderSource;
		PFNGLGETSHADERIVPROC procGetShaderiv;
		PFNGLGETSTRINGPROC procGetString;
		PFNGLGETTEXPARAMETERFVPROC procGetTexParameterfv;
		PFNGLGETTEXPARAMETERIVPROC procGetTexParameteriv;
		PFNGLGETUNIFORMFVPROC procGetUniformfv;
		PFNGLGETUNIFORMIVPROC procGetUniformiv;
		PFNGLGETUNIFORMLOCATIONPROC procGetUniformLocation;
		PFNGLGETVERTEXATTRIBFVPROC procGetVertexAttribfv;
		PFNGLGETVERTEXATTRIBIVPROC procGetVertexAttribiv;
		PFNGLGETVERTEXATTRIBPOINTERVPROC procGetVertexAttribPointerv;
		PFNGLHINTPROC procHint;
		PFNGLISBUFFERPROC procIsBuffer;
		PFNGLISENABLEDPROC procIsEnabled;
		PFNGLISFRAMEBUFFERPROC procIsFramebuffer;
		PFNGLISPROGRAMPROC procIsProgram;
		PFNGLISRENDERBUFFERPROC procIsRenderbuffer;
		PFNGLISSHADERPROC procIsShader;
		PFNGLISTEXTUREPROC procIsTexture;
		PFNGLLINEWIDTHPROC procLineWidth;
		PFNGLLINKPROGRAMPROC procLinkProgram;
		PFNGLPIXELSTOREIPROC procPixelStorei;
		PFNGLPOLYGONOFFSETPROC procPolygonOffset;
		PFNGLREADPIXELSPROC procReadPixels;
		PFNGLRELEASESHADERCOMPILERPROC procReleaseShaderCompiler;
		PFNGLRENDERBUFFERSTORAGEPROC procRenderbufferStorage;
		PFNGLSAMPLECOVERAGEPROC procSampleCoverage;
		PFNGLSCISSORPROC procScissor;
		PFNGLSHADERBINARYPROC procShaderBinary;
		PFNGLSHADERSOURCEPROC procShaderSource;
		PFNGLSTENCILFUNCPROC procStencilFunc;
		PFNGLSTENCILFUNCSEPARATEPROC procStencilFuncSeparate;
		PFNGLSTENCILMASKPROC procStencilMask;
		PFNGLSTENCILMASKSEPARATEPROC procStencilMaskSeparate;
		PFNGLSTENCILOPPROC procStencilOp;
		PFNGLSTENCILOPSEPARATEPROC procStencilOpSeparate;
		PFNGLTEXIMAGE2DPROC procTexImage2D;
		PFNGLTEXPARAMETERFPROC procTexParameterf;
		PFNGLTEXPARAMETERIPROC procTexParameteri;
		PFNGLTEXPARAMETERFVPROC procTexParameterfv;
		PFNGLTEXPARAMETERIVPROC procTexParameteriv;
		PFNGLTEXSUBIMAGE2DPROC procTexSubImage2D;
		PFNGLUNIFORM1FPROC procUniform1f;
		PFNGLUNIFORM2FPROC procUniform2f;
		PFNGLUNIFORM3FPROC procUniform3f;
		PFNGLUNIFORM4FPROC procUniform4f;
		PFNGLUNIFORM1IPROC procUniform1i;
		PFNGLUNIFORM2IPROC procUniform2i;
		PFNGLUNIFORM3IPROC procUniform3i;
		PFNGLUNIFORM4IPROC procUniform4i;
		PFNGLUNIFORM1FVPROC procUniform1fv;
		PFNGLUNIFORM2FVPROC procUniform2fv;
		PFNGLUNIFORM3FVPROC procUniform3fv;
		PFNGLUNIFORM4FVPROC procUniform4fv;
		PFNGLUNIFORM1IVPROC procUniform1iv;
		PFNGLUNIFORM2IVPROC procUniform2iv;
		PFNGLUNIFORM3IVPROC procUniform3iv;
		PFNGLUNIFORM4IVPROC procUniform4iv;
		PFNGLUNIFORMMATRIX2FVPROC procUniformMatrix2fv;
		PFNGLUNIFORMMATRIX3FVPROC procUniformMatrix3fv;
		PFNGLUNIFORMMATRIX4FVPROC procUniformMatrix4fv;
		PFNGLUSEPROGRAMPROC procUseProgram;
		PFNGLVALIDATEPROGRAMPROC procValidateProgram;
		PFNGLVERTEXATTRIB1FPROC procVertexAttrib1f;
		PFNGLVERTEXATTRIB2FPROC procVertexAttrib2f;
		PFNGLVERTEXATTRIB3FPROC procVertexAttrib3f;
		PFNGLVERTEXATTRIB4FPROC procVertexAttrib4f;
		PFNGLVERTEXATTRIB1FVPROC procVertexAttrib1fv;
		PFNGLVERTEXATTRIB2FVPROC procVertexAttrib2fv;
		PFNGLVERTEXATTRIB3FVPROC procVertexAttrib3fv;
		PFNGLVERTEXATTRIB4FVPROC procVertexAttrib4fv;
		PFNGLVERTEXATTRIBPOINTERPROC procVertexAttribPointer;
		PFNGLVIEWPORTPROC procViewport;	
	};
#elif defined(O2D_USE_GLES_API)
    class O2D_API c_core_profile
    {
	public:
		c_core_profile();
		virtual ~c_core_profile();
	public:
		bool make();
    };
#else
	class O2D_API c_core_profile
	{
	private:
		IDXGIFactoryN* m_factory;
		IDXGIAdapterN* m_adapter;
		IDXGISwapChainN* m_swapchain;
		ID3D11DeviceN* m_device;
		ID3D11DeviceContextN* m_context;
		ID3D11RenderTargetView* m_back_view;
	public:
		c_core_profile();
		virtual ~c_core_profile();
	public:
		ID3D11DeviceN* get_device() {return m_device;}
		IDXGISwapChainN* get_swapchain() {return m_swapchain;}
		ID3D11DeviceContextN* get_context() {return m_context;}
		ID3D11RenderTargetView* get_backbuffer() {return m_back_view;}
	public:
#	if defined(O2D_PLATFORM_WINDOWS)
		bool make(HWND hwnd_, u32 w_, u32 h_);
#	else
		bool make(IUnknown* hwnd_, u32 w_, u32 h_);
#	endif
	};
#endif
}
#endif