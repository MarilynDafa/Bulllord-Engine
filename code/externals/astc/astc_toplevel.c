/*----------------------------------------------------------------------------*/
/**
 *    This confidential and proprietary software may be used only as
 *    authorised by a licensing agreement from ARM Limited
 *    (C) COPYRIGHT 2011-2013 ARM Limited
 *    ALL RIGHTS RESERVED
 *
 *    The entire notice above must be reproduced on all authorised
 *    copies and copies may only be made to the extent permitted
 *    by a licensing agreement from ARM Limited.
 *
 *    @brief    Top level functions - parsing command line, managing conversions,
 *            etc.
 *
 *            This is also where main() lives.
 */
/*----------------------------------------------------------------------------*/

#include "astc_codec_internals.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>



#define DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES 0
extern int block_mode_histogram[2048];

#ifdef DEBUG_PRINT_DIAGNOSTICS
int print_diagnostics = 0;
int diagnostics_tile = -1;
#endif

int print_tile_errors = 0;

int print_statistics = 0;

int progress_counter_divider = 1;

int rgb_force_use_of_hdr = 0;
int alpha_force_use_of_hdr = 0;

#define MAGIC_FILE_CONSTANT 0x5CA1AB13


int suppress_progress_counter = 0;
int perform_srgb_transform = 0;

astc_codec_image *load_astc_file(const char *filename, int bitness, astc_decode_mode decode_mode, swizzlepattern swz_decode, const unsigned char* _src,
                                 unsigned char* _dst, int _width, int _height, int _comp)
{
    int x, y, z;
    //FILE *f = fopen(filename, "rb");
    //if (!f)
    //{
    
    astc_header hdr;
    hdr.blockdim_x = 4;
    hdr.blockdim_y = 4;
    hdr.blockdim_z = 1;
    hdr.xsize[0] = _width & 0xFF;
    hdr.xsize[1] = (_width >> 8) & 0xFF;
    hdr.xsize[2] = (_width >> 16) & 0xFF;
    hdr.ysize[0] = _height & 0xFF;
    hdr.ysize[1] = (_height >> 8) & 0xFF;
    hdr.ysize[2] = (_height >> 16) & 0xFF;
    hdr.zsize[0] = 1;
    hdr.zsize[1] = 0;
    hdr.zsize[2] = 0;
    //size_t hdr_bytes_read = fread(&hdr, 1, sizeof(astc_header), f);
    
    
    
    int xdim = hdr.blockdim_x;
    int ydim = hdr.blockdim_y;
    int zdim = hdr.blockdim_z;
    
 
    
    
    int xsize = hdr.xsize[0] + 256 * hdr.xsize[1] + 65536 * hdr.xsize[2];
    int ysize = hdr.ysize[0] + 256 * hdr.ysize[1] + 65536 * hdr.ysize[2];
    int zsize = hdr.zsize[0] + 256 * hdr.zsize[1] + 65536 * hdr.zsize[2];
    
    
    int xblocks = (xsize + xdim - 1) / xdim;
    int yblocks = (ysize + ydim - 1) / ydim;
    int zblocks = (zsize + zdim - 1) / zdim;
    
    uint8_t *buffer = (uint8_t *) malloc(xblocks * yblocks * zblocks * 16);

    size_t bytes_to_read = xblocks * yblocks * zblocks * 16;
    memcpy(buffer, _src, bytes_to_read);
    //size_t bytes_read = fread(buffer, 1, bytes_to_read, f);
    
    
    astc_codec_image *img = allocate_image(bitness, xsize, ysize, zsize, 0);
    initialize_image(img);
    
    imageblock pb;
    for (z = 0; z < zblocks; z++)
        for (y = 0; y < yblocks; y++)
            for (x = 0; x < xblocks; x++)
            {
                int offset = (((z * yblocks + y) * xblocks) + x) * 16;
                uint8_t *bp = buffer + offset;
                physical_compressed_block pcb = *(physical_compressed_block *) bp;
                symbolic_compressed_block scb;
                physical_to_symbolic(xdim, ydim, zdim, pcb, &scb);
                decompress_symbolic_block(decode_mode, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, &scb, &pb);
                write_imageblock(img, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_decode);
            }
    
    free(buffer);
    
    return img;
}



int astcmain(int argc, const char** argv, const unsigned char* _src, unsigned char* _dst, int _width, int _height, int _comp)
{
#ifndef EMSCRIPTEN
    prepare_angular_tables();
    build_quantization_mode_table();
    astc_decode_mode decode_mode = DECODE_HDR;
    int opmode;
    opmode = 1;
    decode_mode = DECODE_HDR;
    const char *input_filename = argv[2];
    const char *output_filename = argv[3];
    error_weighting_params ewp;
    ewp.rgb_power = 1.0f;
    ewp.alpha_power = 1.0f;
    ewp.rgb_base_weight = 1.0f;
    ewp.alpha_base_weight = 1.0f;
    ewp.rgb_mean_weight = 0.0f;
    ewp.rgb_stdev_weight = 0.0f;
    ewp.alpha_mean_weight = 0.0f;
    ewp.alpha_stdev_weight = 0.0f;
    ewp.rgb_mean_and_stdev_mixing = 0.0f;
    ewp.mean_stdev_radius = 0;
    ewp.enable_rgb_scale_with_alpha = 0;
    ewp.alpha_radius = 0;
    ewp.block_artifact_suppression = 0.0f;
    ewp.rgba_weights[0] = 1.0f;
    ewp.rgba_weights[1] = 1.0f;
    ewp.rgba_weights[2] = 1.0f;
    ewp.rgba_weights[3] = 1.0f;
    ewp.ra_normal_angular_scale = 0;
    swizzlepattern swz_decode = { 0, 1, 2, 3 };
    int bitness = get_output_filename_enforced_bitness(output_filename);
    if (bitness == -1)
    {
        bitness = (decode_mode == DECODE_HDR) ? 16 : 8;
    }
    astc_codec_image *output_image = NULL;
    output_image = load_astc_file(input_filename, bitness, decode_mode, swz_decode, _src, _dst, _width, _height, _comp);
    int store_result = -1;
    if (opmode == 1 || opmode == 2)
    {
        const char *format_string = "";
        store_result = astc_codec_store_image(output_image, output_filename, bitness, &format_string,_src, _dst, _width, _height, _comp);
    }
    return store_result;
#else
	return 0;
#endif

}

