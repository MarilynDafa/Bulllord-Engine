/**************************************************************************************/
//   Copyright (C) 2009-2012 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef MAPPING_H_INCLUDE
#define MAPPING_H_INCLUDE
#include "CoreProfile.h"
namespace o2d{	
#if !defined(O2D_USE_DX_API)
	inline GLboolean map(bool val_)
	{
		if(val_)
			return GL_TRUE;
		else
			return GL_FALSE;
	}
	inline GLenum map(e_compare_function func_)
	{
		switch(func_)
		{
		case CF_ALWAYS_FAIL:
			return GL_NEVER;
		case CF_ALWAYS_PASS:
			return GL_ALWAYS;
		case CF_LESS:
			return GL_LESS;
		case CF_LESS_EQUAL:
			return GL_LEQUAL;
		case CF_EQUAL:
			return GL_EQUAL;
		case CF_NOT_EQUAL:
			return GL_NOTEQUAL;
		case CF_GREATER_EQUAL:
			return GL_GEQUAL;
		case CF_GREATER:
			return GL_GREATER;
		default:
			assert(0);
			return GL_LESS;
		}
	}
	inline GLenum map(e_blend_factor bf_)
	{
		switch(bf_)
		{
		case BF_ONE:
			return GL_ONE;
		case BF_ZERO:
			return GL_ZERO;
		case BF_DEST_COLOR:
			return GL_DST_COLOR;
		case BF_SRC_COLOR:
			return GL_SRC_COLOR;
		case BF_ONE_MINUS_DEST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case BF_ONE_MINUS_SRC_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case BF_DEST_ALPHA:
			return GL_DST_ALPHA;
		case BF_SRC_ALPHA:
			return GL_SRC_ALPHA;
		case BF_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case BF_ONE_MINUS_SRC_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		default:
			assert(0);
			return GL_ONE;
		}
	}
	inline GLenum map(e_blend_operation bo_)
	{
		switch(bo_)
		{
		case BO_ADD:
			return GL_FUNC_ADD;
		case BO_SUBTRACT:
			return GL_FUNC_SUBTRACT;
		case BO_REVERSE_SUBTRACT:
			return GL_FUNC_REVERSE_SUBTRACT;
		default:
			assert(0);
			return GL_FUNC_ADD;
		}
	}
	inline void map(e_filter_options mag_, e_filter_options min_, GLenum& maggl_, GLenum& mingl_)
	{
		switch(mag_)
		{
		case FO_POINT:
			maggl_ = GL_NEAREST;
			break;
		case FO_LINEAR:
			maggl_ = GL_LINEAR;
			break;
		default:
			assert(0);
			break;
		}
		switch(min_)
		{
#if defined(O2d_USE_GL_API)
		case FO_POINT:
			mingl_ = GL_NEAREST_MIPMAP_NEAREST;
			break;
		case FO_LINEAR:
			mingl_ = GL_LINEAR_MIPMAP_NEAREST;
			break;
		case FO_TRILINEAR:
			mingl_ = GL_LINEAR_MIPMAP_LINEAR;
			break;
#else
        case FO_POINT:
            mingl_ = GL_NEAREST;
            break;
        case FO_LINEAR:
            mingl_ = GL_LINEAR;
            break;
        case FO_TRILINEAR:
            mingl_ = GL_LINEAR;
            break;
#endif
		default:
			assert(0);
			break;
		}
	}
	inline GLenum map(e_render_operation ro_)
	{
		switch(ro_)
		{
		case RO_POINTS:
			return GL_POINTS;
		case RO_LINES:
			return GL_LINES;
		case RO_LINE_LOOP:
			return GL_LINE_LOOP;
		case RO_LINE_STRIP:
			return GL_LINE_STRIP;
		case RO_TRIANGLES:
			return GL_TRIANGLES;
		case RO_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		case RO_TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		default:
			assert(0);
			return GL_TRIANGLE_FAN;
		}
	}
	inline GLenum map(e_buffer_usage bu_)
	{
		switch(bu_)
		{
		case BU_STATIC_DRAW:
			return GL_STATIC_DRAW;
		case BU_STREAM_DRAW:
			return GL_STREAM_DRAW;
		case BU_DYNAMIC_DRAW:
			return GL_DYNAMIC_DRAW;
		default:
			assert(0);
			return GL_STATIC_DRAW;
		}
	}
#else
	inline BOOL map(bool val_)
	{
		if(val_)
			return TRUE;
		else
			return FALSE;
	}
	inline D3D11_COMPARISON_FUNC map(e_compare_function func_)
	{
		switch(func_)
		{
		case CF_ALWAYS_FAIL:
			return D3D11_COMPARISON_NEVER;
		case CF_ALWAYS_PASS:
			return D3D11_COMPARISON_ALWAYS;
		case CF_LESS:
			return D3D11_COMPARISON_LESS;
		case CF_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case CF_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case CF_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case CF_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case CF_GREATER:
			return D3D11_COMPARISON_GREATER;
		default:
			assert(0);
			return D3D11_COMPARISON_NEVER;
		}
	}
	inline D3D11_BLEND map(e_blend_factor bf_)
	{
		switch(bf_)
		{
		case BF_ONE:
			return D3D11_BLEND_ONE;
		case BF_ZERO:
			return D3D11_BLEND_ZERO;
		case BF_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;
		case BF_SRC_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case BF_ONE_MINUS_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case BF_ONE_MINUS_SRC_COLOR:
			return D3D11_BLEND_INV_SRC_COLOR;
		case BF_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case BF_SRC_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case BF_ONE_MINUS_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case BF_ONE_MINUS_SRC_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;
		default:
			assert(0);
			return D3D11_BLEND_ONE;
		}
	}
	inline D3D11_BLEND_OP map(e_blend_operation bo_)
	{
		switch(bo_)
		{
		case BO_ADD:
			return D3D11_BLEND_OP_ADD;
		case BO_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case BO_REVERSE_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		default:
			assert(0);
			return D3D11_BLEND_OP_ADD;
		}
	}
	inline void map(e_filter_options mag_, e_filter_options min_, D3D11_FILTER& filer_)
	{
		switch(mag_)
		{
		case FO_POINT:
			if(min_ == FO_POINT)
				filer_ = D3D11_FILTER_MIN_MAG_MIP_POINT;
			else if(min_ == FO_LINEAR)
				filer_ = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			else
				filer_ = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		case FO_LINEAR:
			if(min_ == FO_POINT)
				filer_ = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			else if(min_ == FO_LINEAR)
				filer_ = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			else
				filer_ = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		default:
			assert(0);
			filer_ = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		}
	}
	inline D3D11_PRIMITIVE_TOPOLOGY map(e_render_operation ro_)
	{
		switch(ro_)
		{
		case RO_POINTS:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case RO_LINES:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case RO_LINE_LOOP:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case RO_LINE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case RO_TRIANGLES:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case RO_TRIANGLE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case RO_TRIANGLE_FAN:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		default:
			assert(0);
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}
	inline D3D11_USAGE map(e_buffer_usage bu_)
	{
		switch(bu_)
		{
		case BU_STATIC_DRAW:
			return D3D11_USAGE_DEFAULT;
		case BU_STREAM_DRAW:
			return D3D11_USAGE_DYNAMIC;
		case BU_DYNAMIC_DRAW:
			return D3D11_USAGE_DYNAMIC;
		default:
			assert(0);
			return D3D11_USAGE_DEFAULT;
		}
	}
#endif
}
#endif