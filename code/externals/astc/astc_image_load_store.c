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
 *	@brief	Functions for managing ASTC codec images.
 */ 
/*----------------------------------------------------------------------------*/ 
#ifndef EMSCRIPTEN
#include "astc_codec_internals.h"

  void astc_codec_internal_error(const char *filename, int linenumber)
{}

#include "softfloat.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

void destroy_image(astc_codec_image * img)
{
	if (img == NULL)
		return;

	if (img->imagedata8)
	{
		free(img->imagedata8[0][0]);
		free(img->imagedata8[0]);
		free(img->imagedata8);
	}
	if (img->imagedata16)
	{
		free(img->imagedata16[0][0]);
		free(img->imagedata16[0]);
		free(img->imagedata16);
	}
	free( img);
}



astc_codec_image *allocate_image(int bitness, int xsize, int ysize, int zsize, int padding)
{
	int i, j;
	astc_codec_image *img = (astc_codec_image*)malloc(sizeof(astc_codec_image));
	img->xsize = xsize;
	img->ysize = ysize;
	img->zsize = zsize;
	img->padding = padding;

	int exsize = xsize + 2 * padding;
	int eysize = ysize + 2 * padding;
	int ezsize = (zsize == 1) ? 1 : zsize + 2 * padding;

	if (bitness == 8)
	{
		img->imagedata8 = (uint8_t ***) malloc(sizeof(uint8_t **)*ezsize);
		img->imagedata8[0] = (uint8_t **) malloc(sizeof(uint8_t *)*ezsize * eysize);
		img->imagedata8[0][0] = (uint8_t*) malloc(sizeof(uint8_t)*4 * ezsize * eysize * exsize);
		for (i = 1; i < ezsize; i++)
		{
			img->imagedata8[i] = img->imagedata8[0] + i * eysize;
			img->imagedata8[i][0] = img->imagedata8[0][0] + 4 * i * exsize * eysize;
		}
		for (i = 0; i < ezsize; i++)
			for (j = 1; j < eysize; j++)
				img->imagedata8[i][j] = img->imagedata8[i][0] + 4 * j * exsize;

		img->imagedata16 = NULL;
	}

	else if (bitness == 16)
	{
		img->imagedata16 = (uint16_t ***)malloc(sizeof(uint16_t **)*ezsize);
		img->imagedata16[0] = (uint16_t**) malloc(sizeof(uint16_t *)*ezsize * eysize);
		img->imagedata16[0][0] = (uint16_t*) malloc(sizeof(uint16_t)*4 * ezsize * eysize * exsize);
		for (i = 1; i < ezsize; i++)
		{
			img->imagedata16[i] = img->imagedata16[0] + i * eysize;
			img->imagedata16[i][0] = img->imagedata16[0][0] + 4 * i * exsize * eysize;
		}
		for (i = 0; i < ezsize; i++)
			for (j = 1; j < eysize; j++)
				img->imagedata16[i][j] = img->imagedata16[i][0] + 4 * j * exsize;

		img->imagedata8 = NULL;
	}
	else
	{
		ASTC_CODEC_INTERNAL_ERROR;
		exit(1);
	}

	return img;
}



void initialize_image(astc_codec_image * img)
{
	int x, y, z;

	int exsize = img->xsize + 2 * img->padding;
	int eysize = img->ysize + 2 * img->padding;
	int ezsize = (img->zsize == 1) ? 1 : img->zsize + 2 * img->padding;

	if (img->imagedata8)
	{
		for (z = 0; z < ezsize; z++)
			for (y = 0; y < eysize; y++)
				for (x = 0; x < exsize; x++)
				{
					img->imagedata8[z][y][4 * x] = 0;
					img->imagedata8[z][y][4 * x + 1] = 0;
					img->imagedata8[z][y][4 * x + 2] = 0;
					img->imagedata8[z][y][4 * x + 3] = 0xFF;
				}
	}
	else if (img->imagedata16)
	{
		for (z = 0; z < ezsize; z++)
			for (y = 0; y < eysize; y++)
				for (x = 0; x < exsize; x++)
				{
					img->imagedata16[z][y][4 * x] = 0;
					img->imagedata16[z][y][4 * x + 1] = 0;
					img->imagedata16[z][y][4 * x + 2] = 0;
					img->imagedata16[z][y][4 * x + 3] = 0x3C00;
				}
	}
	else
	{
		ASTC_CODEC_INTERNAL_ERROR;
		exit(1);
	}
}




// fill the padding area of the input-file buffer with clamp-to-edge data
// Done inefficiently, in that it will overwrite all the interior data at least once;
// this is not considered a problem, since this makes up a very small part of total
// running time.

void fill_image_padding_area(astc_codec_image * img)
{
	if (img->padding == 0)
		return;

	int x, y, z, i;
	int exsize = img->xsize + 2 * img->padding;
	int eysize = img->ysize + 2 * img->padding;
	int ezsize = (img->zsize == 1) ? 1 : (img->zsize + 2 * img->padding);

	int xmin = img->padding;
	int ymin = img->padding;
	int zmin = (img->zsize == 1) ? 0 : img->padding;
	int xmax = img->xsize + img->padding - 1;
	int ymax = img->ysize + img->padding - 1;
	int zmax = (img->zsize == 1) ? 0 : img->zsize + img->padding - 1;


	// This is a very simple implementation. Possible optimizations include:
	// * Testing if texel is outside the edge.
	// * Looping over texels that we know are outside the edge.
	if (img->imagedata8)
	{
		for (z = 0; z < ezsize; z++)
		{
			int zc = MIN(MAX(z, zmin), zmax);
			for (y = 0; y < eysize; y++)
			{
				int yc = MIN(MAX(y, ymin), ymax);
				for (x = 0; x < exsize; x++)
				{
					int xc = MIN(MAX(x, xmin), xmax);
					for (i = 0; i < 4; i++)
						img->imagedata8[z][y][4 * x + i] = img->imagedata8[zc][yc][4 * xc + i];
				}
			}
		}
	}
	else if (img->imagedata16)
	{
		for (z = 0; z < ezsize; z++)
		{
			int zc = MIN(MAX(z, zmin), zmax);
			for (y = 0; y < eysize; y++)
			{
				int yc = MIN(MAX(y, ymin), ymax);
				for (x = 0; x < exsize; x++)
				{
					int xc = MIN(MAX(x, xmin), xmax);
					for (i = 0; i < 4; i++)
						img->imagedata16[z][y][4 * x + i] = img->imagedata16[zc][yc][4 * xc + i];
				}
			}
		}
	}
}




int determine_image_channels(const astc_codec_image * img)
{
	int x, y, z;

	int xsize = img->xsize;
	int ysize = img->ysize;
	int zsize = img->zsize;
	// scan through the image data
	// to determine how many color channels the image has.

	int lum_mask;
	int alpha_mask;
	int alpha_mask_ref;
	if (img->imagedata8)
	{
		alpha_mask_ref = 0xFF;
		alpha_mask = 0xFF;
		lum_mask = 0;
		for (z = 0; z < zsize; z++)
		{
			for (y = 0; y < ysize; y++)
			{
				for (x = 0; x < xsize; x++)
				{
					int r = img->imagedata8[z][y][4 * x];
					int g = img->imagedata8[z][y][4 * x + 1];
					int b = img->imagedata8[z][y][4 * x + 2];
					int a = img->imagedata8[z][y][4 * x + 3];
					lum_mask |= (r ^ g) | (r ^ b);
					alpha_mask &= a;
				}
			}
		}
	}
	else						// if( bitness == 16 )
	{
		alpha_mask_ref = 0xFFFF;
		alpha_mask = 0xFFFF;
		lum_mask = 0;
		for (z = 0; z < zsize; z++)
		{
			for (y = 0; y < ysize; y++)
			{
				for (x = 0; x < xsize; x++)
				{
					int r = img->imagedata16[z][y][4 * x];
					int g = img->imagedata16[z][y][4 * x + 1];
					int b = img->imagedata16[z][y][4 * x + 2];
					int a = img->imagedata16[z][y][4 * x + 3];
					lum_mask |= (r ^ g) | (r ^ b);
					alpha_mask &= (a ^ 0xC3FF);	// a ^ 0xC3FF returns FFFF if and only if the input is 1.0
				}
			}
		}
	}

    if (alpha_mask == alpha_mask_ref)
        return 3;
    else {
        return 4;
    }
	int image_channels = 1 + (lum_mask == 0 ? 0 : 2) + (alpha_mask == alpha_mask_ref ? 0 : 1);

	return image_channels;
}






// conversion functions between the LNS representation and the FP16 representation.

float float_to_lns(float p)
{

	if (astc_isnan(p) || p <= 1.0f / 67108864.0f)
	{
		// underflow or NaN value, return 0.
		// We count underflow if the input value is smaller than 2^-26.
		return 0;
	}

	if (fabs(p) >= 65536.0f)
	{
		// overflow, return a +INF value
		return 65535;
	}

	int expo;
	float normfrac = frexp(p, &expo);
	float p1;
	if (expo < -13)
	{
		// input number is smaller than 2^-14. In this case, multiply by 2^25.
		p1 = p * 33554432.0f;
		expo = 0;
	}
	else
	{
		expo += 14;
		p1 = (normfrac - 0.5f) * 4096.0f;
	}

	if (p1 < 384.0f)
		p1 *= 4.0f / 3.0f;
	else if (p1 <= 1408.0f)
		p1 += 128.0f;
	else
		p1 = (p1 + 512.0f) * (4.0f / 5.0f);

	p1 += expo * 2048.0f;
	return p1 + 1.0f;
}



uint16_t lns_to_sf16(uint16_t p)
{

	uint16_t mc = p & 0x7FF;
	uint16_t ec = p >> 11;
	uint16_t mt;
	if (mc < 512)
		mt = 3 * mc;
	else if (mc < 1536)
		mt = 4 * mc - 512;
	else
		mt = 5 * mc - 2048;

	uint16_t res = (ec << 10) | (mt >> 3);
	if (res >= 0x7BFF)
		res = 0x7BFF;
	return res;
}


// conversion function from 16-bit LDR value to FP16.
// note: for LDR interpolation, it is impossible to get a denormal result;
// this simplifies the conversion.
// FALSE; we can receive a very small UNORM16 through the constant-block.
uint16_t unorm16_to_sf16(uint16_t p)
{
	if (p == 0xFFFF)
		return 0x3C00;			// value of 1.0 .
	if (p < 4)
		return p << 8;

	int lz = clz32(p) - 16;
	p <<= (lz + 1);
	p >>= 6;
	p |= (14 - lz) << 10;
	return p;
}





void imageblock_initialize_deriv_from_work_and_orig(imageblock * pb, int pixelcount)
{
	int i;

	const float *fptr = pb->orig_data;
	const float *wptr = pb->work_data;
	float *dptr = pb->deriv_data;

	for (i = 0; i < pixelcount; i++)
	{

		// compute derivatives for RGB first
		if (pb->rgb_lns[i])
		{
			float r = MAX(fptr[0], 6e-5f);
			float g = MAX(fptr[1], 6e-5f);
			float b = MAX(fptr[2], 6e-5f);

			float rderiv = (float_to_lns(r * 1.05f) - float_to_lns(r)) / (r * 0.05f);
			float gderiv = (float_to_lns(g * 1.05f) - float_to_lns(g)) / (g * 0.05f);
			float bderiv = (float_to_lns(b * 1.05f) - float_to_lns(b)) / (b * 0.05f);

			// the derivative may not actually take values smaller than 1/32 or larger than 2^25;
			// if it does, we clamp it.
			if (rderiv < (1.0f / 32.0f))
				rderiv = (1.0f / 32.0f);
			else if (rderiv > 33554432.0f)
				rderiv = 33554432.0f;

			if (gderiv < (1.0f / 32.0f))
				gderiv = (1.0f / 32.0f);
			else if (gderiv > 33554432.0f)
				gderiv = 33554432.0f;

			if (bderiv < (1.0f / 32.0f))
				bderiv = (1.0f / 32.0f);
			else if (bderiv > 33554432.0f)
				bderiv = 33554432.0f;

			dptr[0] = rderiv;
			dptr[1] = gderiv;
			dptr[2] = bderiv;
		}
		else
		{
			dptr[0] = 65535.0f;
			dptr[1] = 65535.0f;
			dptr[2] = 65535.0f;
		}


		// then compute derivatives for Alpha
		if (pb->alpha_lns[i])
		{
			float a = MAX(fptr[3], 6e-5f);
			float aderiv = (float_to_lns(a * 1.05f) - float_to_lns(a)) / (a * 0.05f);
			// the derivative may not actually take values smaller than 1/32 or larger than 2^25;
			// if it does, we clamp it.
			if (aderiv < (1.0f / 32.0f))
				aderiv = (1.0f / 32.0f);
			else if (aderiv > 33554432.0f)
				aderiv = 33554432.0f;

			dptr[3] = aderiv;
		}
		else
		{
			dptr[3] = 65535.0f;
		}

		fptr += 4;
		wptr += 4;
		dptr += 4;
	}
}




// helper function to initialize the work-data from the orig-data
void imageblock_initialize_work_from_orig(imageblock * pb, int pixelcount)
{
	int i;
	float *fptr = pb->orig_data;
	float *wptr = pb->work_data;

	for (i = 0; i < pixelcount; i++)
	{
		if (pb->rgb_lns[i])
		{
			wptr[0] = float_to_lns(fptr[0]);
			wptr[1] = float_to_lns(fptr[1]);
			wptr[2] = float_to_lns(fptr[2]);
		}
		else
		{
			wptr[0] = fptr[0] * 65535.0f;
			wptr[1] = fptr[1] * 65535.0f;
			wptr[2] = fptr[2] * 65535.0f;
		}

		if (pb->alpha_lns[i])
		{
			wptr[3] = float_to_lns(fptr[3]);
		}
		else
		{
			wptr[3] = fptr[3] * 65535.0f;
		}
		fptr += 4;
		wptr += 4;
	}

	imageblock_initialize_deriv_from_work_and_orig(pb, pixelcount);
}




// helper function to initialize the orig-data from the work-data
void imageblock_initialize_orig_from_work(imageblock * pb, int pixelcount)
{
	int i;
	float *fptr = pb->orig_data;
	float *wptr = pb->work_data;

	for (i = 0; i < pixelcount; i++)
	{
		if (pb->rgb_lns[i])
		{
			fptr[0] = sf16_to_float(lns_to_sf16((uint16_t) wptr[0]));
			fptr[1] = sf16_to_float(lns_to_sf16((uint16_t) wptr[1]));
			fptr[2] = sf16_to_float(lns_to_sf16((uint16_t) wptr[2]));
		}
		else
		{
			fptr[0] = sf16_to_float(unorm16_to_sf16((uint16_t) wptr[0]));
			fptr[1] = sf16_to_float(unorm16_to_sf16((uint16_t) wptr[1]));
			fptr[2] = sf16_to_float(unorm16_to_sf16((uint16_t) wptr[2]));
		}

		if (pb->alpha_lns[i])
		{
			fptr[3] = sf16_to_float(lns_to_sf16((uint16_t) wptr[3]));
		}
		else
		{
			fptr[3] = sf16_to_float(unorm16_to_sf16((uint16_t) wptr[3]));
		}

		fptr += 4;
		wptr += 4;
	}

	imageblock_initialize_deriv_from_work_and_orig(pb, pixelcount);
}




// fetch an imageblock from the input file.
void fetch_imageblock(const astc_codec_image * img, imageblock * pb,	// picture-block to imitialize with image data
					  // block dimensions
					  int xdim, int ydim, int zdim,
					  // position in texture.
					  int xpos, int ypos, int zpos, swizzlepattern swz)
{
	float *fptr = pb->orig_data;
	int xsize = img->xsize + 2 * img->padding;
	int ysize = img->ysize + 2 * img->padding;
	int zsize = (img->zsize == 1) ? 1 : img->zsize + 2 * img->padding;

	int x, y, z, i;

	pb->xpos = xpos;
	pb->ypos = ypos;
	pb->zpos = zpos;

	xpos += img->padding;
	ypos += img->padding;
	if (img->zsize > 1)
		zpos += img->padding;

	float data[6];
	data[4] = 0;
	data[5] = 1;

	if (img->imagedata8)
	{
		for (z = 0; z < zdim; z++)
			for (y = 0; y < ydim; y++)
				for (x = 0; x < xdim; x++)
				{
					int xi = xpos + x;
					int yi = ypos + y;
					int zi = zpos + z;
					// clamp XY coordinates to the picture.
					if (xi < 0)
						xi = 0;
					if (yi < 0)
						yi = 0;
					if (zi < 0)
						zi = 0;
					if (xi >= xsize)
						xi = xsize - 1;
					if (yi >= ysize)
						yi = ysize - 1;
					if (zi >= zsize)
						zi = zsize - 1;

					int r = img->imagedata8[zi][yi][4 * xi];
					int g = img->imagedata8[zi][yi][4 * xi + 1];
					int b = img->imagedata8[zi][yi][4 * xi + 2];
					int a = img->imagedata8[zi][yi][4 * xi + 3];

					data[0] = r / 255.0f;
					data[1] = g / 255.0f;
					data[2] = b / 255.0f;
					data[3] = a / 255.0f;

					fptr[0] = data[swz.r];
					fptr[1] = data[swz.g];
					fptr[2] = data[swz.b];
					fptr[3] = data[swz.a];
					fptr += 4;
				}
	}
	else if (img->imagedata16)
	{
		for (z = 0; z < zdim; z++)
			for (y = 0; y < ydim; y++)
				for (x = 0; x < xdim; x++)
				{
					int xi = xpos + x;
					int yi = ypos + y;
					int zi = zpos + z;
					// clamp XY coordinates to the picture.
					if (xi < 0)
						xi = 0;
					if (yi < 0)
						yi = 0;
					if (zi < 0)
						zi = 0;
					if (xi >= xsize)
						xi = xsize - 1;
					if (yi >= ysize)
						yi = ysize - 1;
					if (zi >= ysize)
						zi = zsize - 1;

					int r = img->imagedata16[zi][yi][4 * xi];
					int g = img->imagedata16[zi][yi][4 * xi + 1];
					int b = img->imagedata16[zi][yi][4 * xi + 2];
					int a = img->imagedata16[zi][yi][4 * xi + 3];

					float rf = sf16_to_float(r);
					float gf = sf16_to_float(g);
					float bf = sf16_to_float(b);
					float af = sf16_to_float(a);

					// equalize the color components somewhat, and get rid of negative values.

					/* 
					   float maxf = MAX( MAX( rf, gf ), bf ); if( maxf < 1e-5f ) maxf = 1e-5f; rf = MAX( rf, maxf * 4e-4f ); gf = MAX( gf, maxf * 4e-4f ); bf = MAX( bf, maxf * 4e-4f ); */
					rf = MAX(rf, 1e-8f);
					gf = MAX(gf, 1e-8f);
					bf = MAX(bf, 1e-8f);
					af = MAX(af, 1e-8f);


					data[0] = rf;
					data[1] = gf;
					data[2] = bf;
					data[3] = af;

					fptr[0] = data[swz.r];
					fptr[1] = data[swz.g];
					fptr[2] = data[swz.b];
					fptr[3] = data[swz.a];
					fptr += 4;
				}
	}


	// perform sRGB-to-linear transform on input data, if requested.
	int pixelcount = xdim * ydim * zdim;

	if (perform_srgb_transform)
	{
		fptr = pb->orig_data;
		for (i = 0; i < pixelcount; i++)
		{
			float r = fptr[0];
			float g = fptr[1];
			float b = fptr[2];

			if (r <= 0.04045f)
				r = r * (1.0f / 12.92f);
			else if (r <= 1)
				r = pow((r + 0.055f) * (1.0f / 1.055f), 2.4f);

			if (g <= 0.04045f)
				g = g * (1.0f / 12.92f);
			else if (g <= 1)
				g = pow((g + 0.055f) * (1.0f / 1.055f), 2.4f);

			if (b <= 0.04045f)
				b = b * (1.0f / 12.92f);
			else if (b <= 1)
				b = pow((b + 0.055f) * (1.0f / 1.055f), 2.4f);

			fptr[0] = r;
			fptr[1] = g;
			fptr[2] = b;

			fptr += 4;
		}
	}

	// collect color max-value, in order to determine whether to use LDR or HDR
	// interpolation.
	float max_red, max_green, max_blue, max_alpha;
	max_red = 0.0f;
	max_green = 0.0f;
	max_blue = 0.0f;
	max_alpha = 0.0f;

	fptr = pb->orig_data;
	for (i = 0; i < pixelcount; i++)
	{
		float r = fptr[0];
		float g = fptr[1];
		float b = fptr[2];
		float a = fptr[3];

		if (r > max_red)
			max_red = r;
		if (g > max_green)
			max_green = g;
		if (b > max_blue)
			max_blue = b;
		if (a > max_alpha)
			max_alpha = a;

		fptr += 4;
	}

	float max_rgb = MAX(max_red, MAX(max_green, max_blue));

	// use LNS if:
	// * RGB-maximum is less than 0.15
	// * RGB-maximum is greater than 1
	// * Alpha-maximum is greater than 1
	int rgb_lns = (max_rgb < 0.15f || max_rgb > 1.0f || max_alpha > 1.0f) ? 1 : 0;
	int alpha_lns = rgb_lns ? (max_alpha > 1.0f || max_alpha < 0.15f) : 0;

	// not yet though; for the time being, just obey the commandline.
	rgb_lns = rgb_force_use_of_hdr;
	alpha_lns = alpha_force_use_of_hdr;

	// impose the choice on every pixel when encoding.
	for (i = 0; i < pixelcount; i++)
	{
		pb->rgb_lns[i] = rgb_lns;
		pb->alpha_lns[i] = alpha_lns;
		pb->nan_texel[i] = 0;
	}


	imageblock_initialize_work_from_orig(pb, pixelcount);


	update_imageblock_flags(pb, xdim, ydim, zdim);
}




void write_imageblock(astc_codec_image * img, const imageblock * pb,	// picture-block to initialize with image data. We assume that orig_data is valid.
					  // block dimensions
					  int xdim, int ydim, int zdim,
					  // position to write the block to
					  int xpos, int ypos, int zpos, swizzlepattern swz)
{
	const float *fptr = pb->orig_data;
	const uint8_t *nptr = pb->nan_texel;
	int xsize = img->xsize;
	int ysize = img->ysize;
	int zsize = img->zsize;
	int x, y, z;


	float data[7];
	data[4] = 0.0f;
	data[5] = 1.0f;


	if (img->imagedata8)
	{
		for (z = 0; z < zdim; z++)
			for (y = 0; y < ydim; y++)
				for (x = 0; x < xdim; x++)
				{
					int xi = xpos + x;
					int yi = ypos + y;
					int zi = zpos + z;

					if (xi >= 0 && yi >= 0 && zi >= 0 && xi < xsize && yi < ysize && zi < zsize)
					{
						if (*nptr)
						{
							// NaN-pixel, but we can't display it. Display purple instead.
							img->imagedata8[zi][yi][4 * xi] = 0xFF;
							img->imagedata8[zi][yi][4 * xi + 1] = 0x00;
							img->imagedata8[zi][yi][4 * xi + 2] = 0xFF;
							img->imagedata8[zi][yi][4 * xi + 3] = 0xFF;
						}

						else
						{
							// apply swizzle
							if (perform_srgb_transform)
							{
								float r = fptr[0];
								float g = fptr[1];
								float b = fptr[2];

								if (r <= 0.0031308f)
									r = r * 12.92f;
								else if (r <= 1)
									r = 1.055f * pow(r, (1.0f / 2.4f)) - 0.055f;

								if (g <= 0.0031308f)
									g = g * 12.92f;
								else if (g <= 1)
									g = 1.055f * pow(g, (1.0f / 2.4f)) - 0.055f;

								if (b <= 0.0031308f)
									b = b * 12.92f;
								else if (b <= 1)
									b = 1.055f * pow(b, (1.0f / 2.4f)) - 0.055f;

								data[0] = r;
								data[1] = g;
								data[2] = b;
							}
							else
							{
								float r = fptr[0];
								float g = fptr[1];
								float b = fptr[2];

								data[0] = r;
								data[1] = g;
								data[2] = b;
							}
							data[3] = fptr[3];




							float xcoord = (data[0] * 2.0f) - 1.0f;
							float ycoord = (data[3] * 2.0f) - 1.0f;
							float zcoord = 1.0f - xcoord * xcoord - ycoord * ycoord;
							if (zcoord < 0.0f)
								zcoord = 0.0f;
							data[6] = (sqrt(zcoord) * 0.5f) + 0.5f;

							// clamp to [0,1]
							if (data[0] > 1.0f)
								data[0] = 1.0f;
							if (data[1] > 1.0f)
								data[1] = 1.0f;
							if (data[2] > 1.0f)
								data[2] = 1.0f;
							if (data[3] > 1.0f)
								data[3] = 1.0f;


							// pack the data
							int ri = (int)(floor(data[swz.r] * 255.0f + 0.5f));
							int gi = (int)(floor(data[swz.g] * 255.0f + 0.5f));
							int bi = (int)(floor(data[swz.b] * 255.0f + 0.5f));
							int ai = (int)(floor(data[swz.a] * 255.0f + 0.5f));

							img->imagedata8[zi][yi][4 * xi] = ri;
							img->imagedata8[zi][yi][4 * xi + 1] = gi;
							img->imagedata8[zi][yi][4 * xi + 2] = bi;
							img->imagedata8[zi][yi][4 * xi + 3] = ai;
						}
					}
					fptr += 4;
					nptr++;
				}
	}
	else if (img->imagedata16)
	{
		for (z = 0; z < zdim; z++)
			for (y = 0; y < ydim; y++)
				for (x = 0; x < xdim; x++)
				{
					int xi = xpos + x;
					int yi = ypos + y;
					int zi = zpos + z;

					if (xi >= 0 && yi >= 0 && zi >= 0 && xi < xsize && yi < ysize && zi < zsize)
					{
						if (*nptr)
						{
							img->imagedata16[zi][yi][4 * xi] = 0xFFFF;
							img->imagedata16[zi][yi][4 * xi + 1] = 0xFFFF;
							img->imagedata16[zi][yi][4 * xi + 2] = 0xFFFF;
							img->imagedata16[zi][yi][4 * xi + 3] = 0xFFFF;
						}

						else
						{
							// apply swizzle
							if (perform_srgb_transform)
							{
								float r = fptr[0];
								float g = fptr[1];
								float b = fptr[2];

								if (r <= 0.0031308f)
									r = r * 12.92f;
								else if (r <= 1)
									r = 1.055f * pow(r, (1.0f / 2.4f)) - 0.055f;
								if (g <= 0.0031308f)
									g = g * 12.92f;
								else if (g <= 1)
									g = 1.055f * pow(g, (1.0f / 2.4f)) - 0.055f;
								if (b <= 0.0031308f)
									b = b * 12.92f;
								else if (b <= 1)
									b = 1.055f * pow(b, (1.0f / 2.4f)) - 0.055f;

								data[0] = r;
								data[1] = g;
								data[2] = b;
							}
							else
							{
								data[0] = fptr[0];
								data[1] = fptr[1];
								data[2] = fptr[2];
							}
							data[3] = fptr[3];

							float x = (data[0] * 2.0f) - 1.0f;
							float y = (data[3] * 2.0f) - 1.0f;
							float z = 1.0f - x * x - y * y;
							if (z < 0.0f)
								z = 0.0f;
							data[6] = (sqrt(z) * 0.5f) + 0.5f;


							int r = float_to_sf16(data[swz.r], SF_NEARESTEVEN);
							int g = float_to_sf16(data[swz.g], SF_NEARESTEVEN);
							int b = float_to_sf16(data[swz.b], SF_NEARESTEVEN);
							int a = float_to_sf16(data[swz.a], SF_NEARESTEVEN);
							img->imagedata16[zi][yi][4 * xi] = r;
							img->imagedata16[zi][yi][4 * xi + 1] = g;
							img->imagedata16[zi][yi][4 * xi + 2] = b;
							img->imagedata16[zi][yi][4 * xi + 3] = a;
						}
					}
					fptr += 4;
					nptr++;
				}
	}
}



/* 
   For an imageblock, update its flags.

   The updating is done based on work_data, not orig_data.
*/
void update_imageblock_flags(imageblock * pb, int xdim, int ydim, int zdim)
{
	int i;
	float red_min = 1e38f, red_max = -1e38f;
	float green_min = 1e38f, green_max = -1e38f;
	float blue_min = 1e38f, blue_max = -1e38f;
	float alpha_min = 1e38f, alpha_max = -1e38f;

	int texels_per_block = xdim * ydim * zdim;

	int grayscale = 1;

	for (i = 0; i < texels_per_block; i++)
	{
		float red = pb->work_data[4 * i];
		float green = pb->work_data[4 * i + 1];
		float blue = pb->work_data[4 * i + 2];
		float alpha = pb->work_data[4 * i + 3];
		if (red < red_min)
			red_min = red;
		if (red > red_max)
			red_max = red;
		if (green < green_min)
			green_min = green;
		if (green > green_max)
			green_max = green;
		if (blue < blue_min)
			blue_min = blue;
		if (blue > blue_max)
			blue_max = blue;
		if (alpha < alpha_min)
			alpha_min = alpha;
		if (alpha > alpha_max)
			alpha_max = alpha;

		if (grayscale == 1 && (red != green || red != blue))
			grayscale = 0;
	}

	pb->red_min = red_min;
	pb->red_max = red_max;
	pb->green_min = green_min;
	pb->green_max = green_max;
	pb->blue_min = blue_min;
	pb->blue_max = blue_max;
	pb->alpha_min = alpha_min;
	pb->alpha_max = alpha_max;
	pb->grayscale = grayscale;
}


// Helper functions for various error-metric calculations

double clampx(double p)
{
	if (astc_isnan(p) || p < 0.0f)
		p = 0.0f;
	else if (p > 65504.0f)
		p = 65504.0f;
	return p;
}

// logarithm-function, linearized from 2^-14.
double xlog2(double p)
{
	if (p >= 0.00006103515625)
		return log(p) * 1.44269504088896340735;	// log(x)/log(2)
	else
		return -15.44269504088896340735 + p * 23637.11554992477646609062;
}


// mPSNR tone-mapping operator
double mpsnr_operator(double v, int fstop)
{
	int64_t vl = 1LL << (fstop + 32);
	double vl2 = (double)vl * (1.0 / 4294967296.0);
	v *= vl2;
	v = pow(v, (1.0 / 2.2));
	v *= 255.0f;
	if (astc_isnan(v) || v < 0.0f)
		v = 0.0f;
	else if (v > 255.0f)
		v = 255.0f;
	return v;
}


double mpsnr_sumdiff(double v1, double v2, int low_fstop, int high_fstop)
{
	int i;
	double summa = 0.0;
	for (i = low_fstop; i <= high_fstop; i++)
	{
		double mv1 = mpsnr_operator(v1, i);
		double mv2 = mpsnr_operator(v2, i);
		double mdiff = mv1 - mv2;
		summa += mdiff * mdiff;
	}
	return summa;
}




// Compute psnr and other error metrics between input and output image


/* 
	Main image loader function.

	We have specialized loaders for DDS, KTX and HTGA; for other formats, we use stb_image.
	This image loader will choose one based on filename.
*/


typedef enum _scanline_copy_method2
{
    R8_TO_RGBA8,
    RG8_TO_RGBA8,
    RGB8_TO_RGBA8,
    RGBA8_TO_RGBA8,
    BGR8_TO_RGBA8,
    BGRA8_TO_RGBA8,
    L8_TO_RGBA8,
    LA8_TO_RGBA8,

    RGBX8_TO_RGBA8,
    BGRX8_TO_RGBA8,

    R16_TO_RGBA16F,
    RG16_TO_RGBA16F,
    RGB16_TO_RGBA16F,
    RGBA16_TO_RGBA16F,
    BGR16_TO_RGBA16F,
    BGRA16_TO_RGBA16F,
    L16_TO_RGBA16F,
    LA16_TO_RGBA16F,

    R16F_TO_RGBA16F,
    RG16F_TO_RGBA16F,
    RGB16F_TO_RGBA16F,
    RGBA16F_TO_RGBA16F,
    BGR16F_TO_RGBA16F,
    BGRA16F_TO_RGBA16F,
    L16F_TO_RGBA16F,
    LA16F_TO_RGBA16F,

    R32F_TO_RGBA16F,
    RG32F_TO_RGBA16F,
    RGB32F_TO_RGBA16F,
    RGBA32F_TO_RGBA16F,
    BGR32F_TO_RGBA16F,
    BGRA32F_TO_RGBA16F,
    L32F_TO_RGBA16F,
    LA32F_TO_RGBA16F
}scanline_copy_method2;

static void copy_scanline2(void *dst, const void *src, int pixels, int method)
{

#define id(x) (x)
#define u16_sf16(x) float_to_sf16( x * (1.0f/65535.0f), SF_NEARESTEVEN )
#define f32_sf16(x) sf32_to_sf16( x, SF_NEARESTEVEN )

#define COPY_R( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[i]); \
        d[4*i+1] = 0; \
        d[4*i+2] = 0; \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RG( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[2*i]); \
        d[4*i+1] = convfunc(s[2*i+1]); \
        d[4*i+2] = 0; \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RGB( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[3*i]); \
        d[4*i+1] = convfunc(s[3*i+1]); \
        d[4*i+2] = convfunc(s[3*i+2]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_BGR( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[3*i+2]); \
        d[4*i+1] = convfunc(s[3*i+1]); \
        d[4*i+2] = convfunc(s[3*i]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RGBX( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i+2]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_BGRX( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i+2]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RGBA( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i+2]); \
        d[4*i+3] = convfunc(s[4*i+3]); \
        } \
    } while(0); \
    break;

#define COPY_BGRA( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i+2]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i]); \
        d[4*i+3] = convfunc(s[4*i+3]); \
        } \
    } while(0); \
    break;

#define COPY_L( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[i]); \
        d[4*i+1] = convfunc(s[i]); \
        d[4*i+2] = convfunc(s[i]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_LA( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[2*i]); \
        d[4*i+1] = convfunc(s[2*i]); \
        d[4*i+2] = convfunc(s[2*i]); \
        d[4*i+3] = convfunc(s[2*i+1]); \
        } \
    } while(0); \
    break;

    int i;
    switch (method)
    {
    case R8_TO_RGBA8:
        COPY_R(uint8_t, uint8_t, id, 0xFF);
    case RG8_TO_RGBA8:
        COPY_RG(uint8_t, uint8_t, id, 0xFF);
    case RGB8_TO_RGBA8:
        COPY_RGB(uint8_t, uint8_t, id, 0xFF);
    case RGBA8_TO_RGBA8:
        COPY_RGBA(uint8_t, uint8_t, id, 0xFF);
    case BGR8_TO_RGBA8:
        COPY_BGR(uint8_t, uint8_t, id, 0xFF);
    case BGRA8_TO_RGBA8:
        COPY_BGRA(uint8_t, uint8_t, id, 0xFF);
    case RGBX8_TO_RGBA8:
        COPY_RGBX(uint8_t, uint8_t, id, 0xFF);
    case BGRX8_TO_RGBA8:
        COPY_BGRX(uint8_t, uint8_t, id, 0xFF);
    case L8_TO_RGBA8:
        COPY_L(uint8_t, uint8_t, id, 0xFF);
    case LA8_TO_RGBA8:
        COPY_LA(uint8_t, uint8_t, id, 0xFF);

    case R16F_TO_RGBA16F:
        COPY_R(uint16_t, uint16_t, id, 0x3C00);
    case RG16F_TO_RGBA16F:
        COPY_RG(uint16_t, uint16_t, id, 0x3C00);
    case RGB16F_TO_RGBA16F:
        COPY_RGB(uint16_t, uint16_t, id, 0x3C00);
    case RGBA16F_TO_RGBA16F:
        COPY_RGBA(uint16_t, uint16_t, id, 0x3C00);
    case BGR16F_TO_RGBA16F:
        COPY_BGR(uint16_t, uint16_t, id, 0x3C00);
    case BGRA16F_TO_RGBA16F:
        COPY_BGRA(uint16_t, uint16_t, id, 0x3C00);
    case L16F_TO_RGBA16F:
        COPY_L(uint16_t, uint16_t, id, 0x3C00);
    case LA16F_TO_RGBA16F:
        COPY_LA(uint16_t, uint16_t, id, 0x3C00);

    case R16_TO_RGBA16F:
        COPY_R(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case RG16_TO_RGBA16F:
        COPY_RG(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case RGB16_TO_RGBA16F:
        COPY_RGB(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case RGBA16_TO_RGBA16F:
        COPY_RGBA(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case BGR16_TO_RGBA16F:
        COPY_BGR(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case BGRA16_TO_RGBA16F:
        COPY_BGRA(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case L16_TO_RGBA16F:
        COPY_L(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case LA16_TO_RGBA16F:
        COPY_LA(uint16_t, uint16_t, u16_sf16, 0x3C00);

    case R32F_TO_RGBA16F:
        COPY_R(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case RG32F_TO_RGBA16F:
        COPY_RG(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case RGB32F_TO_RGBA16F:
        COPY_RGB(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case RGBA32F_TO_RGBA16F:
        COPY_RGBA(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case BGR32F_TO_RGBA16F:
        COPY_BGR(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case BGRA32F_TO_RGBA16F:
        COPY_BGRA(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case L32F_TO_RGBA16F:
        COPY_L(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case LA32F_TO_RGBA16F:
        COPY_LA(uint16_t, uint32_t, f32_sf16, 0x3C00);
    };
}
astc_codec_image *astc_codec_load_image(const char *input_filename, int padding, int *load_result, const unsigned char* _src, unsigned char* _dst, int _width, int _height, int _comp)
{
	#define LOAD_HTGA 0
	#define LOAD_KTX 1
	#define LOAD_DDS 2
	#define LOAD_STB_IMAGE 3

	// check the ending of the input filename
	int load_fileformat = LOAD_STB_IMAGE;
    int filename_len = 0;

	

	astc_codec_image *input_image;
	if (_src)
	{
        int components = _comp;
        int bytes_per_component = 1;	// bytes per component in the KTX file.
        int bitness = 8;
        int xsize = _width;
        int ysize = _height;
        int zsize = 1;
        uint32_t xstride = bytes_per_component * components * xsize;
        uint32_t ystride = xstride * ysize;
        uint32_t computed_bytes_of_surface = zsize * ystride;
        uint32_t specified_bytes_of_surface = computed_bytes_of_surface;
		uint8_t *imageptr = (uint8_t *)_src;

        scanline_copy_method2 cm;
        if (components == 3)
            cm = RGB8_TO_RGBA8;
        else if (components == 1)
            cm = L8_TO_RGBA8;
        else
            cm = RGBA8_TO_RGBA8;
		//if (image != NULL)
		{
            input_image = allocate_image(bitness, xsize, ysize, zsize, padding);
            for (int z = 0; z < zsize; z++)
            {
                int zdst = (zsize == 1) ? z : z + padding;
                for (int y = 0; y < ysize; y++)
                {
                    int ydst = y + padding;
                    void *dst;
                    if (bitness == 16)
                        dst = (void *)(input_image->imagedata16[zdst][ydst] + 4 * padding);
                    else
                        dst = (void *)(input_image->imagedata8[zdst][ydst] + 4 * padding);

                    uint8_t *src = imageptr + (z * ystride) + (y * xstride);
                    copy_scanline2(dst, src, xsize, cm);
                }
            }
            /*
			for (int y = 0; y < _height; y++)
			{
				int y_dst = y + padding;
				int y_src = y;
				uint8_t *src = imageptr + 4 * _width * y_src;

				for (int x = 0; x < _width; x++)
				{
					int x_dst = x + padding;
					input_image->imagedata8[0][y_dst][4 * x_dst] = src[4 * x];
					input_image->imagedata8[0][y_dst][4 * x_dst + 1] = src[4 * x + 1];
					input_image->imagedata8[0][y_dst][4 * x_dst + 2] = src[4 * x + 2];
					input_image->imagedata8[0][y_dst][4 * x_dst + 3] = src[4 * x + 3];
				}
            }*/
			fill_image_padding_area(input_image);
            *load_result = components + (bitness == 16 ? 0x80 : 0);
		}
	}
	else
	{
		switch (load_fileformat)
		{
		case LOAD_KTX:
			//input_image = load_ktx_uncompressed_image(input_filename, padding, load_result);
			break;
		case LOAD_DDS:
			//input_image = load_dds_uncompressed_image(input_filename, padding, load_result);
			break;
		case LOAD_HTGA:
		//	input_image = load_tga_image(input_filename, padding, load_result);
			break;
		case LOAD_STB_IMAGE:
			//input_image = load_image_with_stb(input_filename, padding, load_result);
			break;
		default:
			ASTC_CODEC_INTERNAL_ERROR;
			exit(1);
		}

		//if (load_exr)
			//astc_codec_unlink(htga_load_filename);
	}	

	return input_image;
}




int get_output_filename_enforced_bitness(const char *output_filename)
{
	return 8;
}




int astc_codec_store_image(const astc_codec_image * output_image, const char *output_filename, int bitness, const char **format_string, const unsigned char* _src, unsigned char* _dst, int _width, int _height, int _comp)
{
	#define STORE_TGA 0
	#define STORE_HTGA 1
	#define STORE_KTX 2
	#define STORE_DDS 3
	#define STORE_EXR 4

	int filename_len = 0;

	int store_fileformat = STORE_TGA;
	const char *eptr = output_filename + filename_len - 5;


	char htga_output_filename[300];
	char htga_output_command[550];
	int system_retval;

	int store_result = -1;
	switch (store_fileformat)
	{
	case STORE_TGA:
	case STORE_HTGA:
		*format_string = bitness == 16 ? "HTGA" : "TGA";
		//store_result = store_tga_image(output_image, output_filename, bitness, _src, _dst, _width, _height, _comp);
		break;
	case STORE_KTX:
		*format_string = "KTX";
		//store_result = store_ktx_uncompressed_image(output_image, output_filename, bitness);
		break;
	case STORE_DDS:
		*format_string = "DDS";
		//store_result = store_dds_uncompressed_image(output_image, output_filename, bitness);
		break;

	case STORE_EXR:
		*format_string = "EXR";
		break;

	default:
		ASTC_CODEC_INTERNAL_ERROR;
		exit(1);
		break;
	};

	return store_result;
}
#endif
