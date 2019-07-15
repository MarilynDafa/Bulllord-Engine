/*----------------------------------------------------------------------------*/
/**
 *	This confidential and proprietary software may be used only as
 *	authorised by a licensing agreement from ARM Limited
 *	(C) COPYRIGHT 2011-2012 ARM Limited
 *	ALL RIGHTS RESERVED
 *
 *	The entire notice above must be reproduced on all authorised
 *	copies and copies may only be made to the extent permitted
 *	by a licensing agreement from ARM Limited.
 *
 *	@brief	Color unquantization functions for ASTC.
 */ 
/*----------------------------------------------------------------------------*/ 
	#ifndef EMSCRIPTEN
#include "astc.h"

//#include "mathlib.h"
#include "softfloat.h"

int rgb_delta_unpack(const int input[6], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	// unquantize the color endpoints
	int r0 = color_unquantization_tables[quantization_level][input[0]];
	int g0 = color_unquantization_tables[quantization_level][input[2]];
	int b0 = color_unquantization_tables[quantization_level][input[4]];

	int r1 = color_unquantization_tables[quantization_level][input[1]];
	int g1 = color_unquantization_tables[quantization_level][input[3]];
	int b1 = color_unquantization_tables[quantization_level][input[5]];

	// perform the bit-transfer procedure
	r0 |= (r1 & 0x80) << 1;
	g0 |= (g1 & 0x80) << 1;
	b0 |= (b1 & 0x80) << 1;
	r1 &= 0x7F;
	g1 &= 0x7F;
	b1 &= 0x7F;
	if (r1 & 0x40)
		r1 -= 0x80;
	if (g1 & 0x40)
		g1 -= 0x80;
	if (b1 & 0x40)
		b1 -= 0x80;

	r0 >>= 1;
	g0 >>= 1;
	b0 >>= 1;
	r1 >>= 1;
	g1 >>= 1;
	b1 >>= 1;

	int rgbsum = r1 + g1 + b1;

	r1 += r0;
	g1 += g0;
	b1 += b0;


	int retval;

	int r0e, g0e, b0e;
	int r1e, g1e, b1e;

	if (rgbsum >= 0)
	{
		r0e = r0;
		g0e = g0;
		b0e = b0;

		r1e = r1;
		g1e = g1;
		b1e = b1;

		retval = 0;
	}
	else
	{
		r0e = (r1 + b1) >> 1;
		g0e = (g1 + b1) >> 1;
		b0e = b1;

		r1e = (r0 + b0) >> 1;
		g1e = (g0 + b0) >> 1;
		b1e = b0;

		retval = 1;
	}

	if (r0e < 0)
		r0e = 0;
	else if (r0e > 255)
		r0e = 255;

	if (g0e < 0)
		g0e = 0;
	else if (g0e > 255)
		g0e = 255;

	if (b0e < 0)
		b0e = 0;
	else if (b0e > 255)
		b0e = 255;

	if (r1e < 0)
		r1e = 0;
	else if (r1e > 255)
		r1e = 255;

	if (g1e < 0)
		g1e = 0;
	else if (g1e > 255)
		g1e = 255;

	if (b1e < 0)
		b1e = 0;
	else if (b1e > 255)
		b1e = 255;

	output0->x = r0e;
	output0->y = g0e;
	output0->z = b0e;
	output0->w = 0xFF;

	output1->x = r1e;
	output1->y = g1e;
	output1->z = b1e;
	output1->w = 0xFF;

	return retval;
}


int rgb_unpack(const int input[6], int quantization_level, ushort4 * output0, ushort4 * output1)
{

	int ri0b = color_unquantization_tables[quantization_level][input[0]];
	int ri1b = color_unquantization_tables[quantization_level][input[1]];
	int gi0b = color_unquantization_tables[quantization_level][input[2]];
	int gi1b = color_unquantization_tables[quantization_level][input[3]];
	int bi0b = color_unquantization_tables[quantization_level][input[4]];
	int bi1b = color_unquantization_tables[quantization_level][input[5]];

	if (ri0b + gi0b + bi0b > ri1b + gi1b + bi1b)
	{
		// blue-contraction
		ri0b = (ri0b + bi0b) >> 1;
		gi0b = (gi0b + bi0b) >> 1;
		ri1b = (ri1b + bi1b) >> 1;
		gi1b = (gi1b + bi1b) >> 1;

		output0->x = ri1b;
		output0->y = gi1b;
		output0->z = bi1b;
		output0->w = 255;

		output1->x = ri0b;
		output1->y = gi0b;
		output1->z = bi0b;
		output1->w = 255;
		return 1;
	}
	else
	{
		output0->x = ri0b;
		output0->y = gi0b;
		output0->z = bi0b;
		output0->w = 255;

		output1->x = ri1b;
		output1->y = gi1b;
		output1->z = bi1b;
		output1->w = 255;
		return 0;
	}
}




void rgba_unpack(const int input[8], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int order = rgb_unpack(input, quantization_level, output0, output1);
	if (order == 0)
	{
		output0->w = color_unquantization_tables[quantization_level][input[6]];
		output1->w = color_unquantization_tables[quantization_level][input[7]];
	}
	else
	{
		output0->w = color_unquantization_tables[quantization_level][input[7]];
		output1->w = color_unquantization_tables[quantization_level][input[6]];
	}
}



void rgba_delta_unpack(const int input[8], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int a0 = color_unquantization_tables[quantization_level][input[6]];
	int a1 = color_unquantization_tables[quantization_level][input[7]];
	a0 |= (a1 & 0x80) << 1;
	a1 &= 0x7F;
	if (a1 & 0x40)
		a1 -= 0x80;
	a0 >>= 1;
	a1 >>= 1;
	a1 += a0;

	if (a1 < 0)
		a1 = 0;
	else if (a1 > 255)
		a1 = 255;

	int order = rgb_delta_unpack(input, quantization_level, output0, output1);
	if (order == 0)
	{
		output0->w = a0;
		output1->w = a1;
	}
	else
	{
		output0->w = a1;
		output1->w = a0;
	}
}


void rgb_scale_unpack(const int input[4], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int ir = color_unquantization_tables[quantization_level][input[0]];
	int ig = color_unquantization_tables[quantization_level][input[1]];
	int ib = color_unquantization_tables[quantization_level][input[2]];

	int iscale = color_unquantization_tables[quantization_level][input[3]];

    output0->x = (ir * iscale) >> 8;
    output0->y = (ig * iscale) >> 8;
    output0->z = (ib * iscale) >> 8;
    output0->w = 255;
    
    output1->x = ir;
    output1->y = ig;
    output1->z = ib;
    output1->w = 255;
}



void rgb_scale_alpha_unpack(const int input[6], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	rgb_scale_unpack(input, quantization_level, output0, output1);
	output0->w = color_unquantization_tables[quantization_level][input[4]];
	output1->w = color_unquantization_tables[quantization_level][input[5]];

}


void luminance_unpack(const int input[2], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int lum0 = color_unquantization_tables[quantization_level][input[0]];
	int lum1 = color_unquantization_tables[quantization_level][input[1]];

    
    output0->x = lum0;
    output0->y = lum0;
    output0->z = lum0;
    output0->w = 255;
    
    output1->x = lum1;
    output1->y = lum1;
    output1->z = lum1;
    output1->w = 255;
}


void luminance_delta_unpack(const int input[2], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int v0 = color_unquantization_tables[quantization_level][input[0]];
	int v1 = color_unquantization_tables[quantization_level][input[1]];
	int l0 = (v0 >> 2) | (v1 & 0xC0);
	int l1 = l0 + (v1 & 0x3F);

	if (l1 > 255)
		l1 = 255;

    output0->x = l0;
    output0->y = l0;
    output0->z = l0;
    output0->w = 255;
    
    output1->x = l1;
    output1->y = l1;
    output1->z = l1;
    output1->w = 255;
    
}




void luminance_alpha_unpack(const int input[4], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int lum0 = color_unquantization_tables[quantization_level][input[0]];
	int lum1 = color_unquantization_tables[quantization_level][input[1]];
	int alpha0 = color_unquantization_tables[quantization_level][input[2]];
	int alpha1 = color_unquantization_tables[quantization_level][input[3]];

    output0->x = lum0;
    output0->y = lum0;
    output0->z = lum0;
    output0->w = alpha0;
    
    output1->x = lum1;
    output1->y = lum1;
    output1->z = lum1;
    output1->w = alpha1;
}


void luminance_alpha_delta_unpack(const int input[4], int quantization_level, ushort4 * output0, ushort4 * output1)
{
	int lum0 = color_unquantization_tables[quantization_level][input[0]];
	int lum1 = color_unquantization_tables[quantization_level][input[1]];
	int alpha0 = color_unquantization_tables[quantization_level][input[2]];
	int alpha1 = color_unquantization_tables[quantization_level][input[3]];

	lum0 |= (lum1 & 0x80) << 1;
	alpha0 |= (alpha1 & 0x80) << 1;
	lum1 &= 0x7F;
	alpha1 &= 0x7F;
	if (lum1 & 0x40)
		lum1 -= 0x80;
	if (alpha1 & 0x40)
		alpha1 -= 0x80;

	lum0 >>= 1;
	lum1 >>= 1;
	alpha0 >>= 1;
	alpha1 >>= 1;
	lum1 += lum0;
	alpha1 += alpha0;

	if (lum1 < 0)
		lum1 = 0;
	else if (lum1 > 255)
		lum1 = 255;

	if (alpha1 < 0)
		alpha1 = 0;
	else if (alpha1 > 255)
		alpha1 = 255;

    output0->x = lum0;
    output0->y = lum0;
    output0->z = lum0;
    output0->w = alpha0;
    
    output1->x = lum1;
    output1->y = lum1;
    output1->z = lum1;
    output1->w = alpha1;
}





void unpack_color_endpoints(astc_decode_mode decode_mode, int format, int quantization_level, const int *input, int *rgb_hdr, int *alpha_hdr, int *nan_endpoint, ushort4 * output0, ushort4 * output1)
{
	*nan_endpoint = 0;

	switch (format)
	{
	case FMT_LUMINANCE:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		luminance_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_LUMINANCE_DELTA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		luminance_delta_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_HDR_LUMINANCE_SMALL_RANGE:
		*rgb_hdr = 1;
		*alpha_hdr = -1;
		//hdr_luminance_small_range_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_HDR_LUMINANCE_LARGE_RANGE:
		*rgb_hdr = 1;
		*alpha_hdr = -1;
		//hdr_luminance_large_range_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_LUMINANCE_ALPHA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		luminance_alpha_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_LUMINANCE_ALPHA_DELTA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		luminance_alpha_delta_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_RGB_SCALE:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		rgb_scale_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_RGB_SCALE_ALPHA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		rgb_scale_alpha_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_HDR_RGB_SCALE:
		*rgb_hdr = 1;
		*alpha_hdr = -1;
		//hdr_rgbo_unpack3(input, quantization_level, output0, output1);
		break;

	case FMT_RGB:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		rgb_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_RGB_DELTA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		rgb_delta_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_HDR_RGB:
		*rgb_hdr = 1;
		*alpha_hdr = -1;
		//hdr_rgb_unpack3(input, quantization_level, output0, output1);
		break;

	case FMT_RGBA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		rgba_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_RGBA_DELTA:
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		rgba_delta_unpack(input, quantization_level, output0, output1);
		break;

	case FMT_HDR_RGB_LDR_ALPHA:
		*rgb_hdr = 1;
		*alpha_hdr = 0;
		//hdr_rgb_ldr_alpha_unpack3(input, quantization_level, output0, output1);
		break;

	case FMT_HDR_RGBA:
		*rgb_hdr = 1;
		*alpha_hdr = 1;
		//hdr_rgb_hdr_alpha_unpack3(input, quantization_level, output0, output1);
		break;

	default:
		ASTC_CODEC_INTERNAL_ERROR;
	}



	if (*alpha_hdr == -1)
	{
		if (alpha_force_use_of_hdr)
		{
			output0->w = 0x7800;
			output1->w = 0x7800;
			*alpha_hdr = 1;
		}
		else
		{
			output0->w = 0x00FF;
			output1->w = 0x00FF;
			*alpha_hdr = 0;
		}
	}



	switch (decode_mode)
	{
	case DECODE_LDR_SRGB:
		if (*rgb_hdr == 1)
		{
			output0->x = 0xFF00;
			output0->y = 0x0000;
			output0->z = 0xFF00;
			output0->w = 0xFF00;
			output1->x = 0xFF00;
			output1->y = 0x0000;
			output1->z = 0xFF00;
			output1->w = 0xFF00;
		}
		else
		{
			output0->x *= 257;
			output0->y *= 257;
			output0->z *= 257;
			output0->w *= 257;
			output1->x *= 257;
			output1->y *= 257;
			output1->z *= 257;
			output1->w *= 257;
		}
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		break;

	case DECODE_LDR:
		if (*rgb_hdr == 1)
		{
			output0->x = 0xFFFF;
			output0->y = 0xFFFF;
			output0->z = 0xFFFF;
			output0->w = 0xFFFF;
			output1->x = 0xFFFF;
			output1->y = 0xFFFF;
			output1->z = 0xFFFF;
			output1->w = 0xFFFF;
			*nan_endpoint = 1;
		}
		else
		{
			output0->x *= 257;
			output0->y *= 257;
			output0->z *= 257;
			output0->w *= 257;
			output1->x *= 257;
			output1->y *= 257;
			output1->z *= 257;
			output1->w *= 257;
		}
		*rgb_hdr = 0;
		*alpha_hdr = 0;
		break;

	case DECODE_HDR:

		if (*rgb_hdr == 0)
		{
			output0->x *= 257;
			output0->y *= 257;
			output0->z *= 257;
			output1->x *= 257;
			output1->y *= 257;
			output1->z *= 257;
		}
		if (*alpha_hdr == 0)
		{
			output0->w *= 257;
			output1->w *= 257;
		}
		break;
	}
}
#endif
