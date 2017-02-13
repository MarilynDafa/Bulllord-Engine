/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "CoreProfile.h"
#if !defined(O2D_USE_DX_API)
#	if defined(O2D_PLATFORM_WINDOWS)
#		define WIN32_LEAN_AND_MEAN
#		include <windows.h>
#	elif defined(O2D_PLATFORM_LINUX)
#		include <GL/glx.h>
#	endif 
#endif
namespace o2d{
#if defined(O2D_USE_GL_API)
	c_core_profile::c_core_profile(){}
	//--------------------------------------------------------
	c_core_profile::~c_core_profile(){}
	//--------------------------------------------------------
	bool c_core_profile::make()
	{
#if defined(O2D_PLATFORM_WINDOWS)
		m_lib = LoadLibraryW(L"opengl32.dll");
		_load_procs();
		FreeLibrary(m_lib);
#elif defined(O2D_PLATFORM_LINUX)
		m_lib = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
		_load_procs();
		dlclose(m_lib);
#elif defined(O2D_PLATFORM_OSX)
		CFURLRef url;
		url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,CFSTR("/System/Library/Frameworks/OpenGL.framework"),kCFURLPOSIXPathStyle, true);
		m_lib = CFBundleCreate(kCFAllocatorDefault, url);
		_load_procs();
		CFRelease(m_lib);
		CFRelease(url);
#endif
        return true;
	}
	//--------------------------------------------------------
	void c_core_profile::_load_procs()
	{
		procPolygonMode = (PFNGLPOLYGONMODEPROC)_get_proc("glPolygonMode");
		procCullFace = (PFNGLCULLFACEPROC) _get_proc("glCullFace");
		procFrontFace = (PFNGLFRONTFACEPROC) _get_proc("glFrontFace");
		procHint = (PFNGLHINTPROC) _get_proc("glHint");
		procLineWidth = (PFNGLLINEWIDTHPROC) _get_proc("glLineWidth");
		procScissor = (PFNGLSCISSORPROC) _get_proc("glScissor");
		procTexParameterf = (PFNGLTEXPARAMETERFPROC) _get_proc("glTexParameterf");
		procTexParameterfv = (PFNGLTEXPARAMETERFVPROC) _get_proc("glTexParameterfv");
		procTexParameteri = (PFNGLTEXPARAMETERIPROC) _get_proc("glTexParameteri");
		procTexParameteriv = (PFNGLTEXPARAMETERIVPROC) _get_proc("glTexParameteriv");
		procTexImage2D = (PFNGLTEXIMAGE2DPROC) _get_proc("glTexImage2D");
		procClear = (PFNGLCLEARPROC) _get_proc("glClear");
		procClearColor = (PFNGLCLEARCOLORPROC) _get_proc("glClearColor");
		procClearStencil = (PFNGLCLEARSTENCILPROC) _get_proc("glClearStencil");
		procClearDepth = (PFNGLCLEARDEPTHPROC) _get_proc("glClearDepth");
		procStencilMask = (PFNGLSTENCILMASKPROC) _get_proc("glStencilMask");
		procColorMask = (PFNGLCOLORMASKPROC) _get_proc("glColorMask");
		procDepthMask = (PFNGLDEPTHMASKPROC) _get_proc("glDepthMask");
		procDisable = (PFNGLDISABLEPROC) _get_proc("glDisable");
		procEnable = (PFNGLENABLEPROC) _get_proc("glEnable");
		procFinish = (PFNGLFINISHPROC) _get_proc("glFinish");
		procFlush = (PFNGLFLUSHPROC) _get_proc("glFlush");
		procBlendFunc = (PFNGLBLENDFUNCPROC) _get_proc("glBlendFunc");
		procStencilFunc = (PFNGLSTENCILFUNCPROC) _get_proc("glStencilFunc");
		procStencilOp = (PFNGLSTENCILOPPROC) _get_proc("glStencilOp");
		procDepthFunc = (PFNGLDEPTHFUNCPROC) _get_proc("glDepthFunc");
		procPixelStorei = (PFNGLPIXELSTOREIPROC) _get_proc("glPixelStorei");
		procReadPixels = (PFNGLREADPIXELSPROC) _get_proc("glReadPixels");
		procGetBooleanv = (PFNGLGETBOOLEANVPROC) _get_proc("glGetBooleanv");
		procGetError = (PFNGLGETERRORPROC) _get_proc("glGetError");
		procGetFloatv = (PFNGLGETFLOATVPROC) _get_proc("glGetFloatv");
		procGetIntegerv = (PFNGLGETINTEGERVPROC) _get_proc("glGetIntegerv");
		procGetString = (PFNGLGETSTRINGPROC) _get_proc("glGetString");
		procGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) _get_proc("glGetTexParameterfv");
		procGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) _get_proc("glGetTexParameteriv");
		procIsEnabled = (PFNGLISENABLEDPROC) _get_proc("glIsEnabled");
		procViewport = (PFNGLVIEWPORTPROC) _get_proc("glViewport");
		procDrawArrays = (PFNGLDRAWARRAYSPROC) _get_proc("glDrawArrays");
		procDrawElements = (PFNGLDRAWELEMENTSPROC) _get_proc("glDrawElements");
		procPolygonOffset = (PFNGLPOLYGONOFFSETPROC) _get_proc("glPolygonOffset");
		procCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) _get_proc("glCopyTexImage2D");
		procCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) _get_proc("glCopyTexSubImage2D");
		procTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) _get_proc("glTexSubImage2D");
		procBindTexture = (PFNGLBINDTEXTUREPROC) _get_proc("glBindTexture");
		procDeleteTextures = (PFNGLDELETETEXTURESPROC) _get_proc("glDeleteTextures");
		procGenTextures = (PFNGLGENTEXTURESPROC) _get_proc("glGenTextures");
		procIsTexture = (PFNGLISTEXTUREPROC) _get_proc("glIsTexture");
		procBlendColor = (PFNGLBLENDCOLORPROC) _get_proc("glBlendColor");
		procBlendEquation = (PFNGLBLENDEQUATIONPROC) _get_proc("glBlendEquation");
		procActiveTexture = (PFNGLACTIVETEXTUREPROC) _get_proc("glActiveTexture");
		procSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) _get_proc("glSampleCoverage");
		procCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) _get_proc("glCompressedTexImage2D");
		procCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) _get_proc("glCompressedTexSubImage2D");
		procBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) _get_proc("glBlendFuncSeparate");
		procBindBuffer = (PFNGLBINDBUFFERPROC) _get_proc("glBindBuffer");
		procDeleteBuffers = (PFNGLDELETEBUFFERSPROC) _get_proc("glDeleteBuffers");
		procGenBuffers = (PFNGLGENBUFFERSPROC) _get_proc("glGenBuffers");
		procIsBuffer = (PFNGLISBUFFERPROC) _get_proc("glIsBuffer");
		procBufferData = (PFNGLBUFFERDATAPROC) _get_proc("glBufferData");
		procBufferSubData = (PFNGLBUFFERSUBDATAPROC) _get_proc("glBufferSubData");
		procGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) _get_proc("glGetBufferParameteriv");
		procBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) _get_proc("glBlendEquationSeparate");
		procStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) _get_proc("glStencilOpSeparate");
		procStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) _get_proc("glStencilFuncSeparate");
		procStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) _get_proc("glStencilMaskSeparate");
		procAttachShader = (PFNGLATTACHSHADERPROC) _get_proc("glAttachShader");
		procBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) _get_proc("glBindAttribLocation");
		procCompileShader = (PFNGLCOMPILESHADERPROC) _get_proc("glCompileShader");
		procCreateProgram = (PFNGLCREATEPROGRAMPROC) _get_proc("glCreateProgram");
		procCreateShader = (PFNGLCREATESHADERPROC) _get_proc("glCreateShader");
		procDeleteProgram = (PFNGLDELETEPROGRAMPROC) _get_proc("glDeleteProgram");
		procDeleteShader = (PFNGLDELETESHADERPROC) _get_proc("glDeleteShader");
		procDetachShader = (PFNGLDETACHSHADERPROC) _get_proc("glDetachShader");
		procDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) _get_proc("glDisableVertexAttribArray");
		procEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) _get_proc("glEnableVertexAttribArray");
		procGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) _get_proc("glGetActiveAttrib");
		procGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) _get_proc("glGetActiveUniform");
		procGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) _get_proc("glGetAttachedShaders");
		procGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) _get_proc("glGetAttribLocation");
		procGetProgramiv = (PFNGLGETPROGRAMIVPROC) _get_proc("glGetProgramiv");
		procGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) _get_proc("glGetProgramInfoLog");
		procGetShaderiv = (PFNGLGETSHADERIVPROC) _get_proc("glGetShaderiv");
		procGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) _get_proc("glGetShaderInfoLog");
		procGetShaderSource = (PFNGLGETSHADERSOURCEPROC) _get_proc("glGetShaderSource");
		procGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) _get_proc("glGetUniformLocation");
		procGetUniformfv = (PFNGLGETUNIFORMFVPROC) _get_proc("glGetUniformfv");
		procGetUniformiv = (PFNGLGETUNIFORMIVPROC) _get_proc("glGetUniformiv");
		procGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) _get_proc("glGetVertexAttribfv");
		procGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) _get_proc("glGetVertexAttribiv");
		procGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) _get_proc("glGetVertexAttribPointerv");
		procIsProgram = (PFNGLISPROGRAMPROC) _get_proc("glIsProgram");
		procIsShader = (PFNGLISSHADERPROC) _get_proc("glIsShader");
		procLinkProgram = (PFNGLLINKPROGRAMPROC) _get_proc("glLinkProgram");
		procShaderSource = (PFNGLSHADERSOURCEPROC) _get_proc("glShaderSource");
		procUseProgram = (PFNGLUSEPROGRAMPROC) _get_proc("glUseProgram");
		procUniform1f = (PFNGLUNIFORM1FPROC) _get_proc("glUniform1f");
		procUniform2f = (PFNGLUNIFORM2FPROC) _get_proc("glUniform2f");
		procUniform3f = (PFNGLUNIFORM3FPROC) _get_proc("glUniform3f");
		procUniform4f = (PFNGLUNIFORM4FPROC) _get_proc("glUniform4f");
		procUniform1i = (PFNGLUNIFORM1IPROC) _get_proc("glUniform1i");
		procUniform2i = (PFNGLUNIFORM2IPROC) _get_proc("glUniform2i");
		procUniform3i = (PFNGLUNIFORM3IPROC) _get_proc("glUniform3i");
		procUniform4i = (PFNGLUNIFORM4IPROC) _get_proc("glUniform4i");
		procUniform1fv = (PFNGLUNIFORM1FVPROC) _get_proc("glUniform1fv");
		procUniform2fv = (PFNGLUNIFORM2FVPROC) _get_proc("glUniform2fv");
		procUniform3fv = (PFNGLUNIFORM3FVPROC) _get_proc("glUniform3fv");
		procUniform4fv = (PFNGLUNIFORM4FVPROC) _get_proc("glUniform4fv");
		procUniform1iv = (PFNGLUNIFORM1IVPROC) _get_proc("glUniform1iv");
		procUniform2iv = (PFNGLUNIFORM2IVPROC) _get_proc("glUniform2iv");
		procUniform3iv = (PFNGLUNIFORM3IVPROC) _get_proc("glUniform3iv");
		procUniform4iv = (PFNGLUNIFORM4IVPROC) _get_proc("glUniform4iv");
		procUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) _get_proc("glUniformMatrix2fv");
		procUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) _get_proc("glUniformMatrix3fv");
		procUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) _get_proc("glUniformMatrix4fv");
		procValidateProgram = (PFNGLVALIDATEPROGRAMPROC) _get_proc("glValidateProgram");
		procVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) _get_proc("glVertexAttrib1f");
		procVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) _get_proc("glVertexAttrib1fv");
		procVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) _get_proc("glVertexAttrib2f");
		procVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) _get_proc("glVertexAttrib2fv");
		procVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) _get_proc("glVertexAttrib3f");
		procVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) _get_proc("glVertexAttrib3fv");
		procVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) _get_proc("glVertexAttrib4f");
		procVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) _get_proc("glVertexAttrib4fv");
		procVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) _get_proc("glVertexAttribPointer");
		procIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) _get_proc("glIsRenderbuffer");
		procBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) _get_proc("glBindRenderbuffer");
		procDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) _get_proc("glDeleteRenderbuffers");
		procGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) _get_proc("glGenRenderbuffers");
		procRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) _get_proc("glRenderbufferStorage");
		procGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) _get_proc("glGetRenderbufferParameteriv");
		procIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) _get_proc("glIsFramebuffer");
		procBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) _get_proc("glBindFramebuffer");
		procDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) _get_proc("glDeleteFramebuffers");
		procGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) _get_proc("glGenFramebuffers");
		procCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) _get_proc("glCheckFramebufferStatus");
		procFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) _get_proc("glFramebufferTexture2D");
		procFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) _get_proc("glFramebufferRenderbuffer");
		procGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) _get_proc("glGetFramebufferAttachmentParameteriv");
		procGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) _get_proc("glGenerateMipmap");

	}
	//--------------------------------------------------------
	void* c_core_profile::_get_proc(const ansi* proc_)
	{
		void* pro = nullptr;
#if defined(O2D_PLATFORM_WINDOWS)
		pro = (void*)wglGetProcAddress(proc_);
		if (!pro)
			pro = (void*)GetProcAddress(m_lib, proc_);
#elif defined(O2D_PLATFORM_LINUX)
		pro = (void*)glXGetProcAddress((const GLubyte*) proc_);
		if (!pro)
			pro = (void*)dlsym(m_lib, proc_);
#elif defined(O2D_PLATFORM_OSX)
		CFStringRef procname = CFStringCreateWithCString(kCFAllocatorDefault, proc_, kCFStringEncodingASCII);
		pro = CFBundleGetFunctionPointerForName(m_lib, procname);
		CFRelease(procname);
#endif
		return pro;
	}
#elif defined(O2D_USE_GLES_API)
	c_core_profile::c_core_profile()
	{}
	//--------------------------------------------------------
	c_core_profile::~c_core_profile()
	{}
	//--------------------------------------------------------
	bool c_core_profile::make()
	{
		return true;
	}
#else
	c_core_profile::c_core_profile()
		:m_factory(nullptr) , 
		m_adapter(nullptr) , 
		m_swapchain(nullptr) , 
		m_device(nullptr)
	{
		HRESULT hr;
		hr = CreateDXGIFactory1(__uuidof(IDXGIFactoryN), (void**)&m_factory);
		assert(!FAILED(hr));
		hr = m_factory->EnumAdapters1(0, &m_adapter);
		assert(!FAILED(hr));
		DXGI_ADAPTER_DESC adapterDesc;
		hr = m_adapter->GetDesc(&adapterDesc);
		assert(!FAILED(hr));
		m_adapter->Release();
		m_adapter = nullptr;
	}
	//--------------------------------------------------------
	c_core_profile::~c_core_profile()
	{
		m_factory->Release();
		m_factory = nullptr;
		m_back_view->Release();
		m_context->Release();
		m_device->Release();
		m_swapchain->Release();
	}
	//--------------------------------------------------------
#if defined(O2D_PLATFORM_WINDOWS)
	bool c_core_profile::make(HWND hwnd_, u32 w_, u32 h_)
#else
	bool c_core_profile::make(IUnknown* hwnd_, u32 w_, u32 h_)
#endif
	{
		DXGI_SWAP_CHAIN_DESCN scdesc;
		ZeroMemory(&scdesc, sizeof(scdesc));
        UINT deviceflags = 0;
		deviceflags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(O2D_DEBUG_MODE)
        deviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#if defined(O2D_PLATFORM_WINDOWS)
		scdesc.BufferCount = 1;
		scdesc.BufferDesc.Width = w_;
		scdesc.BufferDesc.Height = h_;
		scdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scdesc.BufferDesc.RefreshRate.Numerator = 60;
		scdesc.BufferDesc.RefreshRate.Denominator = 1;
		scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scdesc.OutputWindow = hwnd_;
		scdesc.SampleDesc.Count = 1;
		scdesc.SampleDesc.Quality = 0;
		scdesc.Windowed = true;
		scdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scdesc.Flags = 0;
		D3D_FEATURE_LEVEL featurelvl = D3D_FEATURE_LEVEL_9_3;
		D3D_FEATURE_LEVEL outlvl;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
			deviceflags, &featurelvl, 1,
			D3D11_SDK_VERSION, &scdesc, &m_swapchain,
			&m_device, &outlvl, &m_context);
#else
        deviceflags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		scdesc.BufferCount = 2;
		scdesc.Width = w_;
		scdesc.Height = h_;
		scdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scdesc.Stereo = false;
		scdesc.SampleDesc.Count = 1;
		scdesc.SampleDesc.Quality = 0;
		scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scdesc.Scaling = DXGI_SCALING_NONE;
		scdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scdesc.Flags = 0;
		D3D_FEATURE_LEVEL featurelvl = D3D_FEATURE_LEVEL_9_3;
		D3D_FEATURE_LEVEL outlvl;
		ID3D11Device* device = NULL;
		ID3D11DeviceContext* context = NULL;
		HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceflags, &featurelvl,
			1, D3D11_SDK_VERSION, &device, &outlvl, &context);
		hr = device ? device->QueryInterface(__uuidof(ID3D11DeviceN), (void **)&m_device) : hr;
		hr = context ? context->QueryInterface(__uuidof(ID3D11DeviceContextN), (void **)&m_context) : hr;
		hr = m_factory->CreateSwapChainForCoreWindow(m_device, hwnd_, &scdesc, nullptr, &m_swapchain);
#endif
		assert(!FAILED(hr));
		ID3D11Texture2D* backbuf;
		hr = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuf);
		assert(!FAILED(hr));
		hr = m_device->CreateRenderTargetView(backbuf, NULL, &m_back_view);
		assert(!FAILED(hr));
		backbuf->Release();
		backbuf = nullptr;
		m_context->OMSetRenderTargets(1, &m_back_view, nullptr);
		D3D11_VIEWPORT viewport;
		viewport.Width = (f32)w_;
		viewport.Height = (f32)h_;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		m_context->RSSetViewports(1, &viewport);
		return true;
	}
#endif
}
