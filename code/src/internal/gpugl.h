#ifndef __gpugl_h_
#define __gpugl_h_
#include "gpucommon.h"
#if defined(BL_GL_BACKEND)
INLINE BLVoid
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
	GL_CHECK_INTERNAL(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
	GL_CHECK_INTERNAL(glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO));
	GL_CHECK_INTERNAL(glBlendColor(0.f, 0.f, 0.f, 0.f));
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
INLINE BLVoid
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
			blSysWindowQuery(&_w, &_h, &_aw, &_ah, &_rx, &_ry);
			GL_CHECK_INTERNAL(glScissor(0, 0, _w, _h));
		}
		else
		{
			BLF32 _rx, _ry;
			BLU32 _w, _h, _aw, _ah;
			blSysWindowQuery(&_w, &_h, &_aw, &_ah, &_rx, &_ry);
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
		GL_CHECK_INTERNAL(glBlendEquationSeparate(_es, _eas));
		GL_CHECK_INTERNAL(glBlendFuncSeparate(_srgb, _drgb, _srgba, _drgba));
		_PrGpuMem->sPipelineState.bBlendStateDirty = FALSE;
	}
}
INLINE BLBool
_TextureFormatValidGL(GLenum _InternalFmt, GLenum _Fmt, GLenum _Type, BLU32 _Bpp)
{
	BLBool _compressed = FALSE;
	if (_InternalFmt == GL_COMPRESSED_RGBA_ASTC_4x4_KHR ||
		_InternalFmt == GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR)
		_compressed = TRUE;
	GLuint _id;
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	GLsizei _size = (16 * 16 * _Bpp) / 8;
	BLVoid* _ptr;
	union { BLVoid* ptr; BLU32 addr; } _un;
	_un.ptr = alloca(_size + 16);
	BLU32 _unalignedvar = _un.addr;
	BLU32 _mask = 15;
	BLU32 _aligned = (((_unalignedvar)+(_mask)) & ((~0) & (~(_mask))));
	_un.addr = _aligned;
	_ptr = _un.ptr;
	memset(_ptr, 1, _size);
	if (_compressed)
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, _InternalFmt, 16, 16, 0, _size, _ptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, _InternalFmt, 16, 16, 0, _Fmt, _Type, _ptr);
	glDeleteTextures(1, &_id);
	if (glGetError())
	{
#if defined(BL_PLATFORM_WEB)
		return FALSE;
#else
		return 2;
#endif
	}
	else
		return TRUE;
}
INLINE BLVoid
_FillTextureFormatGL(BLEnum _BLFmt, GLenum* _IFmt, GLenum* _Fmt, GLenum* _Type, GLenum* _SrgbFmt, GLenum* _RtFmt)
{
	switch (_BLFmt)
	{
	case BL_TF_ASTC:
		*_IFmt = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
		*_SrgbFmt = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
		*_Fmt = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
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
INLINE BLVoid
_VSyncGL(IN BLBool _On)
{
#if defined(BL_PLATFORM_WIN32)
	wglSwapIntervalEXT(_On);
#elif defined(BL_PLATFORM_OSX)
	GLint _sync = _On;
   // NSOpenGLContext* _pglc = (NSOpenGLContext*)_PrGpuMem->pGLC;
	//[_pglc setValues : &_sync forParameter : NSOpenGLCPSwapInterval];
#elif defined(BL_PLATFORM_LINUX)
	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
	PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
	if (glXSwapIntervalEXT)
		glXSwapIntervalEXT(_PrGpuMem->pDisplay, _PrGpuMem->nWindow, _On);
	else if (glXSwapIntervalSGI)
		glXSwapIntervalSGI(_On);
#elif defined(BL_PLATFORM_IOS)
#elif defined(BL_PLATFORM_ANDROID)
	eglSwapInterval(_PrGpuMem->pEglDisplay, _On);
#elif defined(BL_PLATFORM_WEB)
#endif
}
INLINE BLGuid
_GenFrameBufferGL()
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)malloc(sizeof(_BLFrameBuffer));
	memset(_fbo, 0, sizeof(_BLFrameBuffer));
	_fbo->nAttachmentIndex = 0;
	GL_CHECK_INTERNAL(glGenFramebuffers(1, &_fbo->uData.sGL.nHandle));
	return blGenGuid(_fbo, 0xFFFFFFFF);
}
INLINE BLVoid
_DeleteFrameBufferGL(IN BLGuid _FBO)
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
	if (!_fbo)
		return;
	GL_CHECK_INTERNAL(glDeleteFramebuffers(1, &_fbo->uData.sGL.nHandle));
	if (_fbo->uData.sGL.nResolveHandle)
		GL_CHECK_INTERNAL(glDeleteFramebuffers(1, &_fbo->uData.sGL.nResolveHandle));
	free(_fbo);
	blDeleteGuid(_FBO);
}
INLINE BLVoid
_BindFrameBufferGL(IN BLGuid _FBO, IN BLBool _Bind)
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
	BLU32 _w, _h, _aw, _ah;
	BLF32 _rx, _ry;
	blSysWindowQuery(&_w, &_h, &_aw, &_ah, &_rx, &_ry);
	if (_Bind)
	{
		GL_CHECK_INTERNAL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_fbo->uData.sGL.nPrevHandle));
		GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
		_BLTextureBuffer* _tex = _fbo->aAttachments[0].pAttachments;
		if (_tex)
		{
			GL_CHECK_INTERNAL(glViewport(0, 0, _tex->nWidth, _tex->nHeight));
		}
	}
	else
	{
		GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)_fbo->uData.sGL.nPrevHandle));
		GL_CHECK_INTERNAL(glViewport(0, 0, _w, _h));
	}
}
INLINE BLVoid
_FrameBufferClearGL(IN BLBool _ColorBit, IN BLBool _DepthBit, IN BLBool _StencilBit)
{
	BLU32 _clearbuf = 0;
	if (_ColorBit)
		_clearbuf |= GL_COLOR_BUFFER_BIT;
	if (_DepthBit)
		_clearbuf |= GL_DEPTH_BUFFER_BIT;
	if (_StencilBit)
		_clearbuf |= GL_STENCIL_BUFFER_BIT;
	if (_clearbuf != 0)
		GL_CHECK_INTERNAL(glClear(_clearbuf));
}
INLINE BLVoid
_FrameBufferResolveGL(IN BLGuid _FBO)
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
	if (!_fbo)
		return;
	if (_fbo->uData.sGL.nResolveHandle)
	{
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
INLINE BLU32
_FrameBufferAttachGL(IN BLGuid _FBO, IN BLGuid _Tex, IN BLEnum _CFace)
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
	GLint _prev;
	GL_CHECK_INTERNAL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_prev));
	GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nHandle));
	if (0 == _fbo->nAttachmentIndex)
	{
		_fbo->nWidth = _width;
		_fbo->nHeight = _height;
	}
	GLenum _attachment;
	if (_format < BL_TF_DS_UNKNOWN)
	{
		_attachment = GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex;
		_fbo->aAttachments[_fbo->nAttachmentIndex].pAttachments = _tex;
		_fbo->aAttachments[_fbo->nAttachmentIndex].nFace = _CFace;
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
			GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, _target, _tex->uData.sGL.nHandle, 0));
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
				GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, _target, _tex->uData.sGL.nHandle, 0));
			}
		}
	}
	GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _prev));
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
	return _fbo->nAttachmentIndex;
}
INLINE BLVoid
_FrameBufferDetachGL(IN BLGuid _FBO, IN BLBool _DepthStencil)
{
	_BLFrameBuffer* _fbo = (_BLFrameBuffer*)blGuidAsPointer(_FBO);
	if (!_fbo)
		return;
	GLint _prev;
	GL_CHECK_INTERNAL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_prev));
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
		GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex, _target, 0, 0));
	}
	if (_fbo->uData.sGL.nResolveHandle)
	{
		GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo->uData.sGL.nResolveHandle));
		GLenum _target = (_tex->eTarget == BL_TT_CUBE) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + _fbo->aAttachments[_fbo->nAttachmentIndex].nFace : GL_TEXTURE_2D;
		GL_CHECK_INTERNAL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _fbo->nAttachmentIndex, _target, 0, 0));
	}
	GL_CHECK_INTERNAL(glBindFramebuffer(GL_FRAMEBUFFER, _prev));
}
INLINE BLGuid
_GenTextureGL(IN BLU32 _Hash, IN BLEnum _Target, IN BLEnum _Format, IN BLBool _Srgb, IN BLBool _Immutable, IN BLBool _RenderTarget, IN BLU32 _Layer, IN BLU32 _Mipmap, IN BLU32 _Width, IN BLU32 _Height, IN BLU32 _Depth, IN BLU8* _Data)
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
	GLenum _ifmt, _fmt, _type, _sfmt, _rtfmt;
	_FillTextureFormatGL(_Format, &_ifmt, &_fmt, &_type, &_sfmt, &_rtfmt);
	GL_CHECK_INTERNAL(glGenTextures(1, &_tex->uData.sGL.nHandle));
	GLenum _target = GL_TEXTURE_2D;
	switch (_Target)
	{
	case BL_TT_1D: _target = GL_TEXTURE_2D; break;
	case BL_TT_2D: _target = GL_TEXTURE_2D; break;
	case BL_TT_3D: _target = GL_TEXTURE_3D; break;
	case BL_TT_CUBE: _target = GL_TEXTURE_CUBE_MAP; break;
	case BL_TT_ARRAY1D: _target = GL_TEXTURE_2D_ARRAY; break;
	case BL_TT_ARRAY2D: _target = GL_TEXTURE_2D_ARRAY; break;
	case BL_TT_ARRAYCUBE: _target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
	default:assert(0); break;
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
#if defined(BL_PLATFORM_IOS)
		GL_CHECK_INTERNAL(glBindRenderbuffer(GL_RENDERBUFFER, _PrGpuMem->nBackRB));
#else
		GL_CHECK_INTERNAL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
#endif
	}
	GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK_INTERNAL(glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
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
#endif
#endif
