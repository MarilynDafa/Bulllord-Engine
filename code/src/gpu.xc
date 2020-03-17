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
#include "internal/gpucommon.h"
#if defined(BL_GL_BACKEND)
#include "internal/gpugl.h"
#elif defined(BL_DX_BACKEND)
#include "internal/gpudx.h"
#elif defined(BL_VK_BACKEND)
#include "internal/gpuvk.h"
#elif defined(BL_MTL_BACKEND)
#include "internal/gpumtl.h"
#endif
#if defined(BL_PLATFORM_OSX)
#   include <AppKit/NSOpenGL.h>
#   include <AppKit/NSApplication.h>
#elif defined(BL_PLATFORM_IOS)
#   include <UIKit/UIKit.h>
#   include <OpenGLES/EAGLDrawable.h>
#endif

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
_GpuIntervention(HWND _Hwnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTSSupport = FALSE;
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
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
_GpuIntervention(Windows::UI::Core::CoreWindow^ _Hwnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
    _PrGpuMem = new _BLGpuMember;
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bGSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = TRUE;
    _PrGpuMem->sHardwareCaps.bTSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bFloatRTSupport = TRUE;
    _PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
    _PrGpuMem->pTextureCache = blGenDict(TRUE);
    _PrGpuMem->pBufferCache = blGenDict(TRUE);
    _PrGpuMem->pTechCache = blGenDict(TRUE);
    _PrGpuMem->nSyncInterval = _Vsync ? 60 : 1;
    _PrGpuMem->aBackBufferFence[0] = 0;
    _PrGpuMem->aBackBufferFence[1] = 0;
    _PrGpuMem->aBackBufferFence[2] = 0;
    _PrGpuMem->aBackBufferFence[3] = 0;
    _PrGpuMem->pUBO = new _BLUniformBuffer;
    _PrGpuMem->pUBO->nSize = 0;
    for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
        _PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
    _PrGpuMem->sHardwareCaps.eApiType = BL_DX_API;
    _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = FALSE;
    IDXGIFactory5* _factory;
    UINT _dxgifactoryflags = 0;
#if defined(_DEBUG)
    ID3D12Debug* _debugcontroller;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&_debugcontroller))))
    {
        _debugcontroller->EnableDebugLayer();
        _dxgifactoryflags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif
    DX_CHECK_INTERNAL(CreateDXGIFactory2(_dxgifactoryflags, IID_PPV_ARGS(&_factory)));
    D3D_FEATURE_LEVEL _flvl[4] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    for (BLU32 _i = 0; _i < 4; ++_i)
    {
        IDXGIAdapter1* _adapter;
        for (UINT _j = 0; DXGI_ERROR_NOT_FOUND != _factory->EnumAdapters1(_j, &_adapter); ++_j)
        {
            DXGI_ADAPTER_DESC1 _desc;
            _adapter->GetDesc1(&_desc);
            if (_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;
            if (SUCCEEDED(D3D12CreateDevice(_adapter, _flvl[_i], _uuidof(ID3D12Device), nullptr)))
                break;
        }
        if (SUCCEEDED(D3D12CreateDevice(_adapter, _flvl[_i], IID_PPV_ARGS(&_PrGpuMem->pDX))))
        {
            _adapter->Release();
            break;
        }
    }
    DXGI_SWAP_CHAIN_DESC1 _scdesc = {};
    _scdesc.BufferCount = 4;
    _scdesc.Stereo = 0;
    _scdesc.Width = _Width;
    _scdesc.Height = _Height;
    _scdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    _scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    _scdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    _scdesc.Scaling = DXGI_SCALING_STRETCH;
    _scdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    _scdesc.SampleDesc.Count = 1;
    _scdesc.SampleDesc.Quality = 0;
    IDXGISwapChain1* _tmpsc;
    DX_CHECK_INTERNAL(_factory->CreateSwapChainForCoreWindow(_PrGpuMem->pDX, (IUnknown*)(_Hwnd), &_scdesc, nullptr, &_tmpsc));
    _tmpsc->QueryInterface(__uuidof(IDXGISwapChain3), (BLVoid**)&_PrGpuMem->pSwapChain);
    _tmpsc->Release();
    D3D12_DESCRIPTOR_HEAP_DESC _rtvheapdesc = {};
    _rtvheapdesc.NumDescriptors = 1028;
    _rtvheapdesc.NodeMask = 1;
    _rtvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    _rtvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DX_CHECK_INTERNAL(_PrGpuMem->pDX->CreateDescriptorHeap(&_rtvheapdesc, IID_PPV_ARGS(&_PrGpuMem->pRTVHeap)));
    D3D12_DESCRIPTOR_HEAP_DESC _dsvheapdesc = {};
    _dsvheapdesc.NumDescriptors = 129;
    _dsvheapdesc.NodeMask = 1;
    _dsvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    _dsvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DX_CHECK_INTERNAL(_PrGpuMem->pDX->CreateDescriptorHeap(&_dsvheapdesc, IID_PPV_ARGS(&_PrGpuMem->pDSVHeap)));
    D3D12_DESCRIPTOR_RANGE _descrange[] =
    {
        { D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
        { D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
        { D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
    };
    D3D12_ROOT_PARAMETER _rootparameter[] =
    {
        { D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ { 1, &_descrange[0] } }, D3D12_SHADER_VISIBILITY_ALL },
        { D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ { 1, &_descrange[1] } }, D3D12_SHADER_VISIBILITY_ALL },
        { D3D12_ROOT_PARAMETER_TYPE_CBV,{ { 0, 0 } }, D3D12_SHADER_VISIBILITY_ALL },
        { D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ { 1, &_descrange[3] } }, D3D12_SHADER_VISIBILITY_ALL },
    };
    _rootparameter[2].Descriptor.RegisterSpace = 0;
    _rootparameter[2].Descriptor.ShaderRegister = 0;
    D3D12_ROOT_SIGNATURE_DESC _sigdesc;
    _sigdesc.NumParameters = 4;
    _sigdesc.pParameters = _rootparameter;
    _sigdesc.NumStaticSamplers = 0;
    _sigdesc.pStaticSamplers = NULL;
    _sigdesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ID3DBlob* _error;
    ID3DBlob* _outblob;
    DX_CHECK_INTERNAL(D3D12SerializeRootSignature(&_sigdesc, D3D_ROOT_SIGNATURE_VERSION_1, &_outblob, &_error));
    DX_CHECK_INTERNAL(_PrGpuMem->pDX->CreateRootSignature(0, _outblob->GetBufferPointer(), _outblob->GetBufferSize(), IID_PPV_ARGS(&_PrGpuMem->pRootSignature)));
    _outblob->Release();
    _error->Release();
    UINT _rtvsize = _PrGpuMem->pDX->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (BLU32 _i = 0; _i < 4; ++_i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE _handle = _PrGpuMem->pRTVHeap->GetCPUDescriptorHandleForHeapStart();
        _handle.ptr += _i * _rtvsize;
        DX_CHECK_INTERNAL(_PrGpuMem->pSwapChain->GetBuffer(_i, IID_PPV_ARGS(&_PrGpuMem->aBackBuffer[_i])));
        _PrGpuMem->pDX->CreateRenderTargetView(_PrGpuMem->aBackBuffer[_i], NULL, _handle);
    }
    D3D12_RESOURCE_DESC _resdesc;
    _resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    _resdesc.Alignment = 0;
    _resdesc.Width = _Width;
    _resdesc.Height = _Height;
    _resdesc.DepthOrArraySize = 1;
    _resdesc.MipLevels = 0;
    _resdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    _resdesc.SampleDesc.Count = 1;
    _resdesc.SampleDesc.Quality = 0;
    _resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    _resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE _clearvalue;
    _clearvalue.Format = _resdesc.Format;
    _clearvalue.DepthStencil.Depth = 1.0f;
    _clearvalue.DepthStencil.Stencil = 0;
    D3D12_HEAP_PROPERTIES _heapproperty = { D3D12_HEAP_TYPE_DEFAULT,  D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
    ID3D12Resource* _resource;
    DX_CHECK_INTERNAL(_PrGpuMem->pDX->CreateCommittedResource(&_heapproperty, D3D12_HEAP_FLAG_NONE, &_resdesc, D3D12_RESOURCE_STATE_COMMON, &_clearvalue, IID_PPV_ARGS(&_resource)));
    assert(NULL != _resource);
    D3D12_DEPTH_STENCIL_VIEW_DESC _dsvdesc;
    ZeroMemory(&_dsvdesc, sizeof(_dsvdesc));
    _dsvdesc.Format = _resdesc.Format;
    _dsvdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    _dsvdesc.Flags = D3D12_DSV_FLAGS(0);
    _PrGpuMem->pDX->CreateDepthStencilView(_PrGpuMem->pBackBufferDS, &_dsvdesc, _PrGpuMem->pDSVHeap->GetCPUDescriptorHandleForHeapStart());
    
    
    _PipelineStateDefaultDX(_Width, _Height);
}
BLVoid
_GpuSwapBuffer()
{
    DX_CHECK_INTERNAL(_PrGpuMem->pSwapChain->Present(_PrGpuMem->nSyncInterval, 0));
}
BLVoid
_GpuAnitIntervention()
{
    _FreeUBO(_PrGpuMem->pUBO);
    {
        FOREACH_DICT(ID3D12PipelineState*, _iter, _PrGpuMem->pPSOCache)
        {
            _iter->Release();
        }
    }
    blDeleteDict(_PrGpuMem->pPSOCache);
    _PrGpuMem->pSwapChain->Release();
    _PrGpuMem->aBackBuffer[0]->Release();
    _PrGpuMem->aBackBuffer[1]->Release();
    _PrGpuMem->aBackBuffer[2]->Release();
    _PrGpuMem->aBackBuffer[3]->Release();
    _PrGpuMem->pBackBufferDS->Release();
    _PrGpuMem->pRTVHeap->Release();
    _PrGpuMem->pDSVHeap->Release();
    _PrGpuMem->pRootSignature->Release();
    _PrGpuMem->pDX->Release();
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
    delete _PrGpuMem->pUBO;
    delete _PrGpuMem;
}
#elif defined(BL_PLATFORM_LINUX)
static BLS32
_CtxErrorHandler(Display*, XErrorEvent*)
{
    _PrGpuMem->bCtxError = TRUE;
    return 0;
}
BLVoid
_GpuIntervention(Display* _Display, Window _Window, BLU32 _Width, BLU32 _Height, GLXFBConfig _Config, BLVoid* _Lib, BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTSSupport = FALSE;
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
        _PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
        GL_LOAD_INTERNAL(_Lib);
        glxSwapBuffers = (PFNGLXSWAPBUFFERSPROC)glXGetProcAddress((const GLubyte*)"glXSwapBuffers");
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
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None
        };
        _PrGpuMem->pDisplay = NULL;
        BLS32(*_err)(Display*, XErrorEvent*) = XSetErrorHandler(&_CtxErrorHandler);
        for (BLS32 _idx = 0; _idx < 5 ; ++_idx)
        {
            _attribus[1] = _versions[_idx][0];
            _attribus[3] = _versions[_idx][1];
            _PrGpuMem->pContext = glXCreateContextAttribsARB(_Display, _Config, 0, True, _attribus);
            XSync(_Display, False);
            if (_PrGpuMem->pContext)
            {
                blDebugOutput("Opengl %d.%d boost", _versions[_idx][0], _versions[_idx][1]);
                _PrGpuMem->sHardwareCaps.nApiVersion = _versions[_idx][0] * 100 + _versions[_idx][1] * 10;
                glXMakeContextCurrent(_Display, _Window, _Window, _PrGpuMem->pContext);
                if (glXSwapIntervalEXT)
                    glXSwapIntervalEXT(_Display , _Window , _Vsync ? 1 : 0);
                else if(glXSwapIntervalSGI)
                    glXSwapIntervalSGI(_Vsync ? 1 : 0);
                break;
            }
        }
        XSync(_Display, False);
        _PrGpuMem->pDisplay = _Display;
        _PrGpuMem->nWindow = _Window;
        XSetErrorHandler(_err);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
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
        glxSwapBuffers(_PrGpuMem->pDisplay, _PrGpuMem->nWindow);
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
        PFNGLXMAKECONTEXTCURRENTPROC glXMakeContextCurrent = (PFNGLXMAKECONTEXTCURRENTPROC)glXGetProcAddress((const GLubyte*)"glXMakeContextCurrent");
        glXMakeContextCurrent(_PrGpuMem->pDisplay, None, None, NULL);
        PFNGLXDESTROYCONTEXTPROC glXDestroyContext = (PFNGLXDESTROYCONTEXTPROC)glXGetProcAddress((const GLubyte*)"glXDestroyContext");
        glXDestroyContext(_PrGpuMem->pDisplay, _PrGpuMem->pContext);
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
_GpuIntervention(ANativeWindow* _Wnd, BLU32 _Width, BLU32 _Height, BLBool _Vsync, BLBool _Backend, BLBool _AEP)
{
    if (_Backend)
    {
        if (_PrGpuMem->pEglSurface != EGL_NO_SURFACE)
            eglDestroySurface(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglSurface);
        const EGLint surfaceAttrs[] =
        {
            EGL_RENDER_BUFFER,
            EGL_BACK_BUFFER,
            EGL_NONE
        };
        _PrGpuMem->pEglSurface = eglCreateWindowSurface(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglConfig, _Wnd, surfaceAttrs);
        if (!eglMakeCurrent(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglSurface, _PrGpuMem->pEglSurface, _PrGpuMem->pEglContext))
            blDebugOutput("ResetSurface failed EGL unable to eglMakeCurrent");
        return;
    }
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
    _PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
    _PrGpuMem->pTextureCache = blGenDict(TRUE);
    _PrGpuMem->pBufferCache = blGenDict(TRUE);
    _PrGpuMem->pTechCache = blGenDict(TRUE);
    _PrGpuMem->pEglDisplay = EGL_NO_DISPLAY;
    _PrGpuMem->pEglContext = EGL_NO_CONTEXT;
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
        _PrGpuMem->sHardwareCaps.nApiVersion = 310;
        _PrGpuMem->bAEPSupport = _AEP;
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
            EGL_SURFACE_TYPE,        EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,    0x0040,
            EGL_NONE
        };
        EGLint _configcount;
        const EGLint _contextattrs31[] = { 0x3098, 3, 0x30FB, 1, 0x30FC, 0, EGL_NONE };
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
            _PrGpuMem->pEglContext = eglCreateContext(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglConfig, EGL_NO_CONTEXT, _contextattrs31);
            if (_PrGpuMem->pEglContext == EGL_NO_CONTEXT)
            {
                const EGLint _contextattrs30[] = { 0x3098, 3, 0x30FB, 0, 0x30FC, 0, EGL_NONE };
                _PrGpuMem->sHardwareCaps.nApiVersion = 300;
                _PrGpuMem->pEglContext = eglCreateContext(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglConfig, EGL_NO_CONTEXT, _contextattrs30);
                if (_PrGpuMem->pEglContext == EGL_NO_CONTEXT)
                {
                    blDebugOutput("egl no valid context");
                    return;
                }
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
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
        eglSwapBuffers(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglSurface);
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
        eglMakeCurrent(_PrGpuMem->pEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglContext);
        eglDestroySurface(_PrGpuMem->pEglDisplay, _PrGpuMem->pEglSurface);
        _PrGpuMem->pEglDisplay = EGL_NO_DISPLAY;
        _PrGpuMem->pEglContext = EGL_NO_CONTEXT;
        eglTerminate(_PrGpuMem->pEglDisplay);
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
_GpuIntervention(NSView* _View, BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = TRUE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTSSupport = FALSE;
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
        CGDisplayCount _numdisplays = 0;
        CGDirectDisplayID* _displays, _maindis = 0;
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
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) && _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
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
_GpuIntervention(CALayer* _Layer, BLU32 _Width, BLU32 _Height, BLF32 _Scale, BLBool _Vsync)
{
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
    _PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
    _PrGpuMem->pTextureCache = blGenDict(TRUE);
    _PrGpuMem->pBufferCache = blGenDict(TRUE);
    _PrGpuMem->pTechCache = blGenDict(TRUE);
    _PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));
    _PrGpuMem->pUBO->nSize = 0;
    for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
        _PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
    BLBool _mtlinited = FALSE;
    if (_mtlinited)
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_METAL_API;
    }
    else
    {
        _PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
        _PrGpuMem->sHardwareCaps.nApiVersion = 300;
        _PrGpuMem->pGLC = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        EAGLContext* _pglc = (EAGLContext*)_PrGpuMem->pGLC;
        if (![EAGLContext setCurrentContext:_pglc])
        {
            blDebugOutput("OpenGLES set context failed");
            return;
        }
        glGenRenderbuffers(1, &_PrGpuMem->nBackRB);
        glBindRenderbuffer(GL_RENDERBUFFER, _PrGpuMem->nBackRB);
        if (![_pglc renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)_Layer])
        {
            blDebugOutput("OpenGLES set RenderBufferStorage faield");
            return;
        }
        glGenFramebuffers(1, &_PrGpuMem->nBackFB);
        glBindFramebuffer(GL_FRAMEBUFFER, _PrGpuMem->nBackFB);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _PrGpuMem->nBackRB);
        GLint _backw, _backh;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backw);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backh);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glGenRenderbuffers(1, &_PrGpuMem->nDepthRB);
        glBindRenderbuffer(GL_RENDERBUFFER, _PrGpuMem->nDepthRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _backw, _backh);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _PrGpuMem->nDepthRB);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _PrGpuMem->nDepthRB);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindRenderbuffer(GL_RENDERBUFFER, _PrGpuMem->nBackRB);
        _PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = _TextureFormatValidGL(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8) & _TextureFormatValidGL(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_ZERO, GL_ZERO, 8);
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
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
    else
    {
        EAGLContext* _glc = (EAGLContext*)_PrGpuMem->pGLC;
        [_glc presentRenderbuffer:GL_RENDERBUFFER];
    }
}
BLVoid
_GpuAnitIntervention()
{
    _FreeUBO(_PrGpuMem->pUBO);
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_METAL_API)
    {
    }
    else
    {
        glDeleteFramebuffers(1, &_PrGpuMem->nBackRB);
        glDeleteFramebuffers(1, &_PrGpuMem->nBackFB);
        glDeleteFramebuffers(1, &_PrGpuMem->nDepthRB);
        [EAGLContext setCurrentContext:nil];
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
#elif defined(BL_PLATFORM_WEB)
BLVoid
_GpuIntervention(BLU32 _Width, BLU32 _Height, BLBool _Vsync)
{
	EM_ASM({
		Array.prototype.someExtensionFromThirdParty = {};
		Array.prototype.someExtensionFromThirdParty.length = 42;
		Array.prototype.someExtensionFromThirdParty.something = function() { return "Surprise!"; };
	  });
    _PrGpuMem = (_BLGpuMember*)malloc(sizeof(_BLGpuMember));
    _PrGpuMem->bVsync = _Vsync;
    _PrGpuMem->sHardwareCaps.bCSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bGSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bAnisotropy = FALSE;
    _PrGpuMem->sHardwareCaps.bTSSupport = FALSE;
    _PrGpuMem->sHardwareCaps.bFloatRTSupport = FALSE;
    _PrGpuMem->sHardwareCaps.fMaxAnisotropy = 0.f;
    _PrGpuMem->pTextureCache = blGenDict(TRUE);
    _PrGpuMem->pBufferCache = blGenDict(TRUE);
    _PrGpuMem->pTechCache = blGenDict(TRUE);
    _PrGpuMem->pUBO = (_BLUniformBuffer*)malloc(sizeof(_BLUniformBuffer));
    _PrGpuMem->pUBO->nSize = 0;
    for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
        _PrGpuMem->sHardwareCaps.aTexFormats[_idx] = TRUE;
    _PrGpuMem->sHardwareCaps.eApiType = BL_GL_API;
    EmscriptenWebGLContextAttributes _attrs;
    emscripten_webgl_init_context_attributes(&_attrs);
    _attrs.majorVersion = 2;
	_attrs.minorVersion = 0;
	_attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;
    _PrGpuMem->pContext = emscripten_webgl_create_context("#canvas", &_attrs);
	if (_PrGpuMem->pContext)
	{
		memset(&_attrs, -1, sizeof(_attrs));
		EMSCRIPTEN_RESULT _res = emscripten_webgl_get_context_attributes(_PrGpuMem->pContext, &_attrs);
		assert(_res == EMSCRIPTEN_RESULT_SUCCESS);
		assert(_attrs.powerPreference == EM_WEBGL_POWER_PREFERENCE_DEFAULT || _attrs.powerPreference == EM_WEBGL_POWER_PREFERENCE_LOW_POWER || _attrs.powerPreference == EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE);
		_res = emscripten_webgl_make_context_current(_PrGpuMem->pContext);
		assert(_res == EMSCRIPTEN_RESULT_SUCCESS);
		assert(emscripten_webgl_get_current_context() == _PrGpuMem->pContext);
		_PrGpuMem->sHardwareCaps.nApiVersion = 300;
		_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = FALSE;
		GLint _extensions = 0;
		GL_CHECK_INTERNAL(glGetIntegerv(GL_NUM_EXTENSIONS, &_extensions));
		for (GLint _idx = 0; _idx < _extensions; ++_idx)
		{
			const BLAnsi* _name = (const BLAnsi*)glGetStringi(GL_EXTENSIONS, _idx);
			if (!strcmp(_name, "GL_EXT_texture_filter_anisotropic"))
			{
				GL_CHECK_INTERNAL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_PrGpuMem->sHardwareCaps.fMaxAnisotropy));
			}
			else if (!strcmp(_name, "WEBGL_compressed_texture_astc"))
			{
				_PrGpuMem->sHardwareCaps.aTexFormats[BL_TF_ASTC] = TRUE;
			}
		}
		_PipelineStateDefaultGL(_Width, _Height);
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
	emscripten_webgl_destroy_context(_PrGpuMem->pContext);
}
#else
#   "error what's the fucking platform"
#endif
BLVoid
blGpuVSync(IN BLBool _On)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
		_VSyncGL(_On);
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
        _PrGpuMem->nSyncInterval = _On ? 60 : 1;
    }
#endif
}
BLVoid
blGpuCapsQuery(OUT BLEnum* _Api, OUT BLBool* _CSSupport, OUT BLBool* _GSSupport, OUT BLBool* _TSSupport, OUT BLBool* _FloatRTSupport, OUT BLBool _TexSupport[BL_TF_COUNT])
{
    if (_Api)
        *_Api = _PrGpuMem->sHardwareCaps.eApiType;
    if (_CSSupport)
        *_CSSupport = _PrGpuMem->sHardwareCaps.bCSSupport;
    if (_GSSupport)
        *_GSSupport = _PrGpuMem->sHardwareCaps.bGSSupport;
    if (_TSSupport)
        *_TSSupport = _PrGpuMem->sHardwareCaps.bTSSupport;
    if (_FloatRTSupport)
        *_FloatRTSupport = _PrGpuMem->sHardwareCaps.bFloatRTSupport;
    for (BLU32 _idx = 0; _idx < BL_TF_COUNT; ++_idx)
        _TexSupport[_idx] = _PrGpuMem->sHardwareCaps.aTexFormats[_idx];
}
BLVoid
blGpuRasterState(IN BLEnum _CullMode, IN BLS32 _DepthBias, IN BLF32 _SlopeScaledDepthBias, IN BLBool _Scissor, IN BLU32 _LineWidth, IN BLS32 _XPos, IN BLS32 _YPos, IN BLU32 _Width, IN BLU32 _Height, IN BLBool _Force)
{
    if (_PrGpuMem->sPipelineState.eCullMode != _CullMode ||
        _PrGpuMem->sPipelineState.nDepthBias != _DepthBias ||
        !blScalarApproximate(_PrGpuMem->sPipelineState.fSlopeScaledDepthBias, _SlopeScaledDepthBias) ||
        _PrGpuMem->sPipelineState.bScissor != _Scissor ||
        _PrGpuMem->sPipelineState.nLineWidth != _LineWidth ||
        _PrGpuMem->sPipelineState.nScissorX != _XPos ||
        _PrGpuMem->sPipelineState.nScissorY != _YPos ||
        _PrGpuMem->sPipelineState.nScissorW != _Width ||
        _PrGpuMem->sPipelineState.nScissorH != _Height)
    {
        _PrGpuMem->sPipelineState.eCullMode = _CullMode;
        _PrGpuMem->sPipelineState.nDepthBias = _DepthBias;
        _PrGpuMem->sPipelineState.fSlopeScaledDepthBias = _SlopeScaledDepthBias;
        _PrGpuMem->sPipelineState.bScissor = _Scissor;
        _PrGpuMem->sPipelineState.nLineWidth = _LineWidth;
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
blGpuDepthStencilState(IN BLBool _Depth, IN BLBool _Mask, IN BLEnum _DepthCompFunc, IN BLBool _Stencil, IN BLU8 _StencilReadMask, IN BLU8 _StencilWriteMask, IN BLEnum _FrontStencilFailOp, IN BLEnum _FrontStencilDepthFailOp, IN BLEnum _FrontStencilPassOp, IN BLEnum _FrontStencilCompFunc, IN BLEnum _BackStencilFailOp, IN BLEnum _BackStencilDepthFailOp, IN BLEnum _BackStencilPassOp, IN BLEnum _BackStencilCompFunc, IN BLBool _Force)
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
blGpuBlendState(IN BLBool _AlphaToCoverage, IN BLBool _Blend, IN BLEnum _SrcBlendFactor, IN BLEnum _DestBlendFactor, IN BLEnum _SrcBlendAlphaFactor, IN BLEnum _DestBlendAlphaFactor, IN BLEnum _BlendOp, IN BLEnum _BlendOpAlpha, IN BLBool _Force)
{
    if (_PrGpuMem->sPipelineState.bAlphaToCoverage != _AlphaToCoverage ||
        _PrGpuMem->sPipelineState.bBlend != _Blend ||
        _PrGpuMem->sPipelineState.eSrcBlendFactor != _SrcBlendFactor ||
        _PrGpuMem->sPipelineState.eDestBlendFactor != _DestBlendFactor ||
        _PrGpuMem->sPipelineState.eSrcBlendAlphaFactor != _SrcBlendAlphaFactor ||
        _PrGpuMem->sPipelineState.eDestBlendAlphaFactor != _DestBlendAlphaFactor ||
        _PrGpuMem->sPipelineState.eBlendOp != _BlendOp ||
        _PrGpuMem->sPipelineState.eBlendOpAlpha != _BlendOpAlpha)
    {
        _PrGpuMem->sPipelineState.bAlphaToCoverage = _AlphaToCoverage;
        _PrGpuMem->sPipelineState.bBlend = _Blend;
        _PrGpuMem->sPipelineState.eSrcBlendFactor = _SrcBlendFactor;
        _PrGpuMem->sPipelineState.eDestBlendFactor = _DestBlendFactor;
        _PrGpuMem->sPipelineState.eSrcBlendAlphaFactor = _SrcBlendAlphaFactor;
        _PrGpuMem->sPipelineState.eDestBlendAlphaFactor = _DestBlendAlphaFactor;
        _PrGpuMem->sPipelineState.eBlendOp = _BlendOp;
        _PrGpuMem->sPipelineState.eBlendOpAlpha = _BlendOpAlpha;
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
        _PrGpuMem->sPipelineState.bBlendStateDirty = TRUE;
        _PipelineStateRefresh();
    }
}
BLGuid
blFrameBufferGen()
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        return _GenFrameBufferGL();
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
    return INVALID_GUID;
}
BLVoid
blFrameBufferDelete(IN BLGuid _FBO)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        _DeleteFrameBufferGL(_FBO);
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
blFrameBufferBind(IN BLGuid _FBO, IN BLBool _Bind)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
       _BindFrameBufferGL(_FBO, _Bind);
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
		_FrameBufferClearGL(_ColorBit, _DepthBit, _StencilBit);
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
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
		_FrameBufferResolveGL(_FBO);
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
blFrameBufferAttach(IN BLGuid _FBO, IN BLGuid _Tex, IN BLEnum _CFace)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        return _FrameBufferAttachGL(_FBO, _Tex, _CFace);
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
    return INVALID_GUID;
}
BLVoid
blFrameBufferDetach(IN BLGuid _FBO, IN BLBool _DepthStencil)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        _FrameBufferDetachGL(_FBO, _DepthStencil);
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
blTextureGen(IN BLU32 _Hash, IN BLEnum _Target, IN BLEnum _Format, IN BLBool _Srgb, IN BLBool _Immutable, IN BLBool _RenderTarget, IN BLU32 _Layer, IN BLU32 _Mipmap, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, IN BLU8* _Data)
{
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        return _GenTextureGL(_Hash, _Target, _Format, _Srgb, _Immutable, _RenderTarget, _Layer, _Mipmap, _Width, _Height, _Depth, _Data);
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
    return INVALID_GUID;
}
BLBool
blTextureDelete(IN BLGuid _Tex)
{
    BLBool _discard = FALSE;
    _BLTextureBuffer* _tex = (_BLTextureBuffer*)blGuidAsPointer(_Tex);
    if (!_tex)
        return FALSE;
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
        return FALSE;
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
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
	return TRUE;
}
BLGuid
blTextureGain(IN BLU32 _Hash)
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
        GLenum _wraps = GL_REPEAT, _wrapt = GL_REPEAT;
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
            case BL_TT_1D: _target = GL_TEXTURE_2D;    break;
            case BL_TT_2D: _target = GL_TEXTURE_2D;    break;
            case BL_TT_3D: _target = GL_TEXTURE_3D;    break;
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
blGeometryBufferGen(IN BLU32 _Hash, IN BLEnum _Topology, IN BLBool _Dynamic, IN BLEnum* _Semantic, IN BLEnum* _Decl, IN BLU32 _DeclNum,IN BLVoid* _VBO, IN BLU32 _VBSz, IN BLVoid* _IBO, IN BLU32 _IBSz, IN BLEnum _IBFmt)
{
    _BLGeometryBuffer* _geo = NULL;
    BLBool _cache = FALSE;
    if (!_Dynamic && !_Semantic)
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
    else if (_geo && !_Semantic)
        return _geo->nID;
    _geo->eVBTopology = _Topology;
    _geo->eIBFormat = _IBFmt;
    _geo->bDynamic = _Dynamic;
    _geo->nVBSize = _VBSz;
    _geo->nIBSize = _IBSz;
    _geo->aInsSem[0] = _geo->aInsSem[1] = _geo->aInsSem[2] = _geo->aInsSem[3] = BL_SL_INVALID;
    BLU32 _stride = 0;
    const BLEnum* _decls = _Decl;
    const BLEnum* _semantic = _Semantic;
    for (BLU32 _idx = 0; _idx < _DeclNum; ++_idx)
    {
        switch(_decls[_idx])
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
    _geo->nVertexNum = _geo->nVBSize / _stride;
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
            GL_CHECK_INTERNAL(glBufferData(GL_ARRAY_BUFFER, _geo->nVBSize, _VBO, GL_STATIC_DRAW));
        }
        else
        {
            GL_CHECK_INTERNAL(glBufferData(GL_ARRAY_BUFFER, _geo->nVBSize, _VBO, _Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        }
        if (_geo->eIBFormat != BL_IF_INVALID)
        {
            GL_CHECK_INTERNAL(glGenBuffers(1, &_geo->uData.sGL.nIBHandle));
            GL_CHECK_INTERNAL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geo->uData.sGL.nIBHandle));
            if (_Hash == 0xFFFFFFFF)
            {
                GL_CHECK_INTERNAL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _geo->nIBSize, _IBO, GL_STATIC_DRAW));
            }
            else
            {
                GL_CHECK_INTERNAL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _geo->nIBSize, _IBO, _Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
            }
        }
        BLU8* _offset = NULL;
        BLU32 _used[16] = { 0 };
        for (BLU32 _idx = 0; _idx < _DeclNum; ++_idx)
        {
            BLU32 _gtsize = 4;
            GLint _numele = 4;
            GLenum _gtype = GL_BYTE;
            switch(_decls[_idx])
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
            GL_CHECK_INTERNAL(glEnableVertexAttribArray(_semantic[_idx]));
            GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, _geo->uData.sGL.nVBHandle));
            GL_CHECK_INTERNAL(glVertexAttribPointer(_semantic[_idx], _numele, _gtype, GL_FALSE, _stride, _offset));
            _used[_semantic[_idx]] = 1;
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
        if (_geo->eIBFormat != BL_IF_INVALID)
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
BLBool
blGeometryBufferDelete(IN BLGuid _GBO)
{
    BLBool _discard = FALSE;
    _BLGeometryBuffer* _geo = (_BLGeometryBuffer*)blGuidAsPointer(_GBO);
    if (!_geo)
        return FALSE;
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
        return FALSE;
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
	return TRUE;
}
BLGuid
blGeometryBufferGain(IN BLU32 _Hash)
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
			if (_geo->uData.sGL.nInsHandle[_idx] == 0xFFFFFFFF)
				GL_CHECK_INTERNAL(glGenBuffers(1, &_geo->uData.sGL.nInsHandle[_idx]));
            BLU32 _vbsz = 0, _numele = 0;
            GLenum _type = GL_BYTE;
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
            GL_CHECK_INTERNAL(glVertexAttribPointer(_Semantic[_idx], _numele, _type, GL_FALSE, 0, NULL));
            GL_CHECK_INTERNAL(glVertexAttribDivisor(_Semantic[_idx], 1));
            _geo->aInsSem[_idx] = _Semantic[_idx];
        }
		GL_CHECK_INTERNAL(glBindVertexArray(0));
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
		GL_CHECK_INTERNAL(glBindBuffer(GL_ARRAY_BUFFER, 0));
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
blTechniqueGen(IN BLAnsi* _Filename, IN BLAnsi* _Source, IN BLBool _ForceCompile)
{
    BLBool _cache = FALSE;
    _BLTechnique* _tech;
    blMutexLock(_PrGpuMem->pTechCache->pMutex);
    _BLGpuRes* _res = (_BLGpuRes*)blDictElement(_PrGpuMem->pTechCache, blHashString((const BLUtf8*)_Filename));
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
    BLU32 _hash = blHashString((const BLUtf8*)_Filename);
    BLGuid _stream = INVALID_GUID;
    BLAnsi _path[260] = { 0 };
    BLBool _findbinary = FALSE;
    ezxml_t _doc = NULL;
    const BLAnsi* _vert = NULL, *_frag = NULL, *_geom = NULL, *_tesc = NULL, *_tess = NULL, *_comp = NULL;
#if !defined(BL_PLATFORM_WEB)
    if (!_ForceCompile)
    {
        memset(_path, 0, 260 * sizeof(BLAnsi));
        strcpy(_path, "b");
        strcat(_path, _Filename);
		_stream = blStreamGen(_path);
        _findbinary = (_stream == INVALID_GUID) ? FALSE : TRUE;
    }
#endif
    if (!_findbinary)
    {
		if (!_Source)
		{
			_stream = blStreamGen(_Filename);
			_doc = ezxml_parse_str((BLAnsi*)blStreamData(_stream), blStreamLength(_stream));
		}
		else
		{
			BLAnsi* _source = (BLAnsi*)alloca(strlen(_Source) + 1);
			strcpy(_source, _Source);
			_doc = ezxml_parse_str(_source, strlen(_Source));
		}
        const BLAnsi* _tag = NULL;
        switch (_PrGpuMem->sHardwareCaps.eApiType)
        {
            case BL_GL_API: _tag = "GL"; break;
            case BL_DX_API: _tag = "DX"; break;
            case BL_METAL_API: _tag = "MTL"; break;
            case BL_VULKAN_API: _tag = "VK"; break;
            default: _tag = "Motherfucker"; break;
        }
        ezxml_t _element = ezxml_child(_doc, _tag);
        _element = ezxml_child(_element, "Vert");
        do {
            if (_element && !strcmp(_element->name, "Vert"))
                _vert = ezxml_txt(_element);
            else if (_element && !strcmp(_element->name, "Frag"))
                _frag = ezxml_txt(_element);
            else if (_element && !strcmp(_element->name, "Geom"))
                _geom = ezxml_txt(_element);
            else if (_element && !strcmp(_element->name, "Tess"))
                _tesc = ezxml_txt(_element);
            else if (_element && !strcmp(_element->name, "Eval"))
                _tess = ezxml_txt(_element);
            else if (_element && !strcmp(_element->name, "Comp"))
                _comp = ezxml_txt(_element);
            _element = _element ? _element->sibling : NULL;
        } while (_element);
    }
#if defined(BL_GL_BACKEND)
    if (_PrGpuMem->sHardwareCaps.eApiType == BL_GL_API)
    {
        BLAnsi _glslver[256] = { 0 };
        if (!_findbinary)
        {
            GLint _compiled = 0;
            GLint _linked = 0;
            GLuint _vs = 0, _fs = 0, _gs = 0;
            _tech->uData.sGL.nHandle = glCreateProgram();
            if (_vert)
            {
#if defined(BL_PLATFORM_ANDROID) || defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_WEB)
                sprintf(_glslver, "#version %d es\n", _PrGpuMem->sHardwareCaps.nApiVersion);
#else
                sprintf(_glslver, "#version %d core\n", _PrGpuMem->sHardwareCaps.nApiVersion);
#endif
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
                        BLAnsi* _info = (BLAnsi*)alloca((_len+100)*sizeof(BLAnsi));
                        GL_CHECK_INTERNAL(glGetShaderInfoLog(_vs, _len, &_len, _info));
						strcat(_info, _Filename);
						strcat(_info, "vs");
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
#if defined(BL_PLATFORM_ANDROID) || defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_WEB)
                sprintf(_glslver, "#version %d es\n#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\n", _PrGpuMem->sHardwareCaps.nApiVersion);
#else
                sprintf(_glslver, "#version %d core\n", _PrGpuMem->sHardwareCaps.nApiVersion);
#endif
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
                        BLAnsi* _info = (BLAnsi*)alloca((_len+100)*sizeof(BLAnsi));
                        GL_CHECK_INTERNAL(glGetShaderInfoLog(_fs, _len, &_len, _info));
						strcat(_info, _Filename);
						strcat(_info, "fs");
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
						strcat(_info, _Filename);
						strcat(_info, "gs");
                        blDebugOutput(_info);
                    }
                }
                else
                {
                    GL_CHECK_INTERNAL(glAttachShader(_tech->uData.sGL.nHandle, _gs));
                }
            }
#if !defined(BL_PLATFORM_WEB)
            GL_CHECK_INTERNAL(glProgramParameteri(_tech->uData.sGL.nHandle, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE));
#endif
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
#if !defined(BL_PLATFORM_WEB)
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
#endif
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
#if !defined(BL_PLATFORM_WEB)
            GLint _linked = 0;
            _tech->uData.sGL.nHandle = glCreateProgram();
            GLenum _format = 0;
            blStreamRead(_stream, sizeof(GLenum), &_format);
            BLU32 _len = blStreamLength(_stream) - sizeof(GLenum);
            GL_CHECK_INTERNAL(glProgramBinary(_tech->uData.sGL.nHandle, _format, (BLU8*)blStreamData(_stream) + sizeof(GLenum), _len));
            GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_LINK_STATUS, &_linked));
            if (!_linked)
            {
                GLint _loglen = 0;
                GL_CHECK_INTERNAL(glGetProgramiv(_tech->uData.sGL.nHandle, GL_INFO_LOG_LENGTH, &_loglen));
                if (_loglen > 0)
                {
                    BLAnsi* _info = (BLAnsi*)alloca((_loglen+1)*sizeof(BLAnsi));
                    GL_CHECK_INTERNAL(glGetProgramInfoLog(_tech->uData.sGL.nHandle, _loglen, &_loglen, _info));
                    blDebugOutput(_info);
                }
            }
#endif
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
        _res = (_BLGpuRes*)malloc(sizeof(_BLGpuRes));
        _res->nRefCount = 1;
        _res->pRes = _tech;
        blDictInsert(_PrGpuMem->pTechCache, _hash, _res);
        blMutexUnlock(_PrGpuMem->pTechCache->pMutex);
    }
    blStreamDelete(_stream);
    _tech->nID = blGenGuid(_tech, _hash);
    return _tech->nID;
}
BLBool
blTechniqueDelete(IN BLGuid _Tech)
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
        return FALSE;
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
        if (_tech->uData.sDX.pVS)
            _tech->uData.sDX.pVS->Release();
        if (_tech->uData.sDX.pPS)
            _tech->uData.sDX.pPS->Release();
        if (_tech->uData.sDX.pGS)
            _tech->uData.sDX.pGS->Release();
        if (_tech->uData.sDX.pDS)
            _tech->uData.sDX.pDS->Release();
        if (_tech->uData.sDX.pHS)
            _tech->uData.sDX.pHS->Release();
    }
#endif
    free(_tech);
    blDeleteGuid(_Tech);
	return TRUE;
}
BLGuid
blTechniqueGain(IN BLU32 _Hash)
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
blTechniqueUniform(IN BLGuid _Tech, IN BLEnum _Type, IN BLAnsi* _Name, IN BLVoid* _Data, IN BLU32 _DataSz, IN BLBool _UBO)
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
			_tech->aUniformVars[_idx].bUBO = _UBO;
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
blTechniqueSampler(IN BLGuid _Tech, IN BLAnsi* _Name, IN BLGuid _Tex, IN BLU32 _Unit)
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
blTechniqueDraw(IN BLGuid _Tech, IN BLGuid _GBO, IN BLU32 _Instance)
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
        }
		GL_CHECK_INTERNAL(glUseProgram(_tech->uData.sGL.nHandle));
        for (BLU32 _idx = 0; _idx < 16; ++_idx)
        {
            if (_tech->aUniformVars[_idx].aName[0])
            {
                BLVoid* _data = _tech->aUniformVars[_idx].pVar;
                BLU32 _count = _tech->aUniformVars[_idx].nCount;
				if (_tech->aUniformVars[_idx].bUBO)
				{
					GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, _PrGpuMem->pUBO->uData.sGL.nHandle));
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
					case BL_UB_MAT4X3: GL_CHECK_INTERNAL(glBufferSubData(GL_UNIFORM_BUFFER, _tech->aUniformVars[_idx].uData.sGL.nOffset, 12 * sizeof(BLF32), (GLfloat*)_data)); break;
					default:assert(0); break;
					}
					GL_CHECK_INTERNAL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
				}
				else
				{
					if (_tech->aUniformVars[_idx].uData.sGL.nIndices == 0xFFFFFFFF)
						_tech->aUniformVars[_idx].uData.sGL.nIndices = glGetUniformLocation(_tech->uData.sGL.nHandle, _tech->aUniformVars[_idx].aName);
					switch (_tech->aUniformVars[_idx].eType)
					{
					case BL_UB_S32X1: GL_CHECK_INTERNAL(glUniform1iv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLint*)_data)); break;
					case BL_UB_S32X2: GL_CHECK_INTERNAL(glUniform2iv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLint*)_data)); break;
					case BL_UB_S32X3: GL_CHECK_INTERNAL(glUniform3iv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLint*)_data)); break;
					case BL_UB_S32X4: GL_CHECK_INTERNAL(glUniform4iv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLint*)_data)); break;
					case BL_UB_F32X1: GL_CHECK_INTERNAL(glUniform1fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLfloat*)_data)); break;
					case BL_UB_F32X2: GL_CHECK_INTERNAL(glUniform2fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLfloat*)_data)); break;
					case BL_UB_F32X3: GL_CHECK_INTERNAL(glUniform3fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLfloat*)_data)); break;
					case BL_UB_F32X4: GL_CHECK_INTERNAL(glUniform4fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, (GLfloat*)_data)); break;
					case BL_UB_MAT2: GL_CHECK_INTERNAL(glUniformMatrix2fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT3: GL_CHECK_INTERNAL(glUniformMatrix3fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT4: GL_CHECK_INTERNAL(glUniformMatrix4fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT2X3: GL_CHECK_INTERNAL(glUniformMatrix2x3fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT3X2: GL_CHECK_INTERNAL(glUniformMatrix3x2fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT2X4: GL_CHECK_INTERNAL(glUniformMatrix2x4fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT4X2: GL_CHECK_INTERNAL(glUniformMatrix4x2fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT3X4: GL_CHECK_INTERNAL(glUniformMatrix3x4fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					case BL_UB_MAT4X3: GL_CHECK_INTERNAL(glUniformMatrix4x3fv(_tech->aUniformVars[_idx].uData.sGL.nIndices, _count, FALSE, (GLfloat*)_data)); break;
					default:assert(0); break;
					}
				}
            }
            else
                break;
        }
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
                    case BL_TT_1D: _target = GL_TEXTURE_2D;    break;
                    case BL_TT_2D: _target = GL_TEXTURE_2D;    break;
                    case BL_TT_3D: _target = GL_TEXTURE_3D;    break;
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
			if (_geo->uData.sGL.nInsHandle[0] == 0xFFFFFFFF)
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
			if (_geo->uData.sGL.nInsHandle[0] == 0xFFFFFFFF)
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
