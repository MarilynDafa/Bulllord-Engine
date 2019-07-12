/*
 * MPEG Audio decoder
 * Copyright (c) 2001, 2002 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Modified heavily by Matt Campbell <mattcampbell@pobox.com> for the
 * stand-alone mpaudec library.  Based on mpegaudiodec.c from libavcodec.
 */
#include "internal.h"
#include "mpegaudio.h"
#ifndef EMSCRIPTEN
#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif
/*
 * TODO:
 *  - in low precision mode, use more 16 bit multiplies in synth filter
 *  - test lsf / mpeg25 extensively.
 */

/* define USE_HIGHPRECISION to have a bit exact (but slower) mpeg
   audio decoder */
#define USE_HIGHPRECISION

#ifdef USE_HIGHPRECISION
#define FRAC_BITS   23   /* fractional bits for sb_samples and dct */
#define WFRAC_BITS  16   /* fractional bits for window */
#else
#define FRAC_BITS   15   /* fractional bits for sb_samples and dct */
#define WFRAC_BITS  14   /* fractional bits for window */
#endif

#define FRAC_ONE    (1 << FRAC_BITS)

#define MULL(a,b) (((int64_t)(a) * (int64_t)(b)) >> FRAC_BITS)
#define MUL64(a,b) ((int64_t)(a) * (int64_t)(b))
#define FIX(a)   ((int)((a) * FRAC_ONE))
/* WARNING: only correct for posititive numbers */
#define FIXR(a)   ((int)((a) * FRAC_ONE + 0.5))
#define FRAC_RND(a) (((a) + (FRAC_ONE/2)) >> FRAC_BITS)

#if FRAC_BITS <= 15
typedef int16_t MPA_INT;
#else
typedef int32_t MPA_INT;
#endif

/****************/

#define HEADER_SIZE 4
#define BACKSTEP_SIZE 512

typedef struct MPADecodeContext {
    uint8_t inbuf1[2][MPA_MAX_CODED_FRAME_SIZE + BACKSTEP_SIZE];        /* input buffer */
    int inbuf_index;
    uint8_t *inbuf_ptr, *inbuf;
    int frame_size;
    int free_format_frame_size; /* frame size in case of free format
                                   (zero if currently unknown) */
    /* next header (used in free format parsing) */
    int error_protection;
    int layer;
    int sample_rate;
    int sample_rate_index; /* between 0 and 8 */
    int bit_rate;
    int old_frame_size;
    GetBitContext gb;
    int nb_channels;
    int mode;
    int mode_ext;
    int lsf;
    MPA_INT synth_buf[MPA_MAX_CHANNELS][512 * 2];
    int synth_buf_offset[MPA_MAX_CHANNELS];
    int32_t sb_samples[MPA_MAX_CHANNELS][36][SBLIMIT];
    int32_t mdct_buf[MPA_MAX_CHANNELS][SBLIMIT * 18]; /* previous samples, for layer 3 MDCT */
#ifdef DEBUG
    int frame_count;
#endif
} MPADecodeContext;

/* layer 3 "granule" */
typedef struct GranuleDef {
    uint8_t scfsi;
    int part2_3_length;
    int big_values;
    int global_gain;
    int scalefac_compress;
    uint8_t block_type;
    uint8_t switch_point;
    int table_select[3];
    int subblock_gain[3];
    uint8_t scalefac_scale;
    uint8_t count1table_select;
    int region_size[3]; /* number of huffman codes in each region */
    int preflag;
    int short_start, long_end; /* long/short band indexes */
    uint8_t scale_factors[40];
    int32_t sb_hybrid[SBLIMIT * 18]; /* 576 samples */
} GranuleDef;

#define MODE_EXT_MS_STEREO 2
#define MODE_EXT_I_STEREO  1

/* layer 3 huffman tables */
typedef struct HuffTable {
    int xsize;
    const uint8_t *bits;
    const uint16_t *codes;
} HuffTable;

#include "mpaudectab.h"

/* vlc structure for decoding layer 3 huffman tables */
static VLC huff_vlc[16]; 
static uint8_t *huff_code_table[16];
static VLC huff_quad_vlc[2];
/* computed from band_size_long */
static uint16_t band_index_long[9][23];
/* XXX: free when all decoders are closed */
#define TABLE_4_3_SIZE (8191 + 16)
static int8_t  table_4_3_exp[TABLE_4_3_SIZE];
#if FRAC_BITS <= 15
static uint16_t table_4_3_value[TABLE_4_3_SIZE];
#else
static uint32_t table_4_3_value[TABLE_4_3_SIZE];
#endif
/* intensity stereo coef table */
static int32_t is_table[2][16];
static int32_t is_table_lsf[2][2][16];
static int32_t csa_table[8][2];
static int32_t mdct_win[8][36];

/* lower 2 bits: modulo 3, higher bits: shift */
static uint16_t scale_factor_modshift[64];
/* [i][j]:  2^(-j/3) * FRAC_ONE * 2^(i+2) / (2^(i+2) - 1) */
static int32_t scale_factor_mult[15][3];
/* mult table for layer 2 group quantization */

#define SCALE_GEN(v) \
{ FIXR(1.0 * (v)), FIXR(0.7937005259 * (v)), FIXR(0.6299605249 * (v)) }

static int32_t scale_factor_mult2[3][3] = {
    SCALE_GEN(4.0 / 3.0), /* 3 steps */
    SCALE_GEN(4.0 / 5.0), /* 5 steps */
    SCALE_GEN(4.0 / 9.0), /* 9 steps */
};

/* 2^(n/4) */
static uint32_t scale_factor_mult3[4] = {
    FIXR(1.0),
    FIXR(1.18920711500272106671),
    FIXR(1.41421356237309504880),
    FIXR(1.68179283050742908605),
};

static MPA_INT window[512];
    
/* layer 1 unscaling */
/* n = number of bits of the mantissa minus 1 */
static int l1_unscale(int n, int mant, int scale_factor)
{
    int shift, mod;
    int64_t val;

    shift = scale_factor_modshift[scale_factor];
    mod = shift & 3;
    shift >>= 2;
    val = MUL64(mant + (-1 << n) + 1, scale_factor_mult[n-1][mod]);
    shift += n;
    /* NOTE: at this point, 1 <= shift >= 21 + 15 */
    return (int)((val + ((int64_t)(1) << (shift - 1))) >> shift);
}

static int l2_unscale_group(int steps, int mant, int scale_factor)
{
    int shift, mod, val;

    shift = scale_factor_modshift[scale_factor];
    mod = shift & 3;
    shift >>= 2;

    val = (mant - (steps >> 1)) * scale_factor_mult2[steps >> 2][mod];
    /* NOTE: at this point, 0 <= shift <= 21 */
    if (shift > 0)
        val = (val + (1 << (shift - 1))) >> shift;
    return val;
}

/* compute value^(4/3) * 2^(exponent/4). It normalized to FRAC_BITS */
static int l3_unscale(int value, int exponent)
{
#if FRAC_BITS <= 15    
    unsigned int m;
#else
    uint64_t m;
#endif
    int e;

    e = table_4_3_exp[value];
    e += (exponent >> 2);
    e = FRAC_BITS - e;
#if FRAC_BITS <= 15    
    if (e > 31)
        e = 31;
#endif
    m = table_4_3_value[value];
#if FRAC_BITS <= 15    
    m = (m * scale_factor_mult3[exponent & 3]);
    m = (m + (1 << (e-1))) >> e;
    return m;
#else
    m = MUL64(m, scale_factor_mult3[exponent & 3]);
    m = (m + ((uint64_t)(1) << (e-1))) >> e;
    return (int)m;
#endif
}

/* all integer n^(4/3) computation code */
#define DEV_ORDER 13

#define POW_FRAC_BITS 24
#define POW_FRAC_ONE    (1 << POW_FRAC_BITS)
#define POW_FIX(a)   ((int)((a) * POW_FRAC_ONE))
#define POW_MULL(a,b) (((int64_t)(a) * (int64_t)(b)) >> POW_FRAC_BITS)

static int dev_4_3_coefs[DEV_ORDER];

static int pow_mult3[3] = {
    POW_FIX(1.0),
    POW_FIX(1.25992104989487316476),
    POW_FIX(1.58740105196819947474),
};

static void int_pow_init(void)
{
    int i, a;

    a = POW_FIX(1.0);
    for(i=0;i<DEV_ORDER;i++) {
        a = POW_MULL(a, POW_FIX(4.0 / 3.0) - i * POW_FIX(1.0)) / (i + 1);
        dev_4_3_coefs[i] = a;
    }
}

/* return the mantissa and the binary exponent */
static int int_pow(int i, int *exp_ptr)
{
    int e, er, eq, j;
    int a, a1;
    
    /* renormalize */
    a = i;
    e = POW_FRAC_BITS;
    while (a < (1 << (POW_FRAC_BITS - 1))) {
        a = a << 1;
        e--;
    }
    a -= (1 << POW_FRAC_BITS);
    a1 = 0;
    for(j = DEV_ORDER - 1; j >= 0; j--)
        a1 = POW_MULL(a, dev_4_3_coefs[j] + a1);
    a = (1 << POW_FRAC_BITS) + a1;
    /* exponent compute (exact) */
    e = e * 4;
    er = e % 3;
    eq = e / 3;
    a = POW_MULL(a, pow_mult3[er]);
    while (a >= 2 * POW_FRAC_ONE) {
        a = a >> 1;
        eq++;
    }
    /* convert to float */
    while (a < POW_FRAC_ONE) {
        a = a << 1;
        eq--;
    }
    /* now POW_FRAC_ONE <= a < 2 * POW_FRAC_ONE */
#if POW_FRAC_BITS > FRAC_BITS
    a = (a + (1 << (POW_FRAC_BITS - FRAC_BITS - 1))) >> (POW_FRAC_BITS - FRAC_BITS);
    /* correct overflow */
    if (a >= 2 * (1 << FRAC_BITS)) {
        a = a >> 1;
        eq++;
    }
#endif
    *exp_ptr = eq;
    return a;
}
static int init = 0;
void mpaudec_open()
{
	init = 0;
}

int mpaudec_init(MPAuDecContext * mpctx)
{
    MPADecodeContext *s;
    int i, j, k;
    assert(mpctx != NULL);
    memset(mpctx, 0, sizeof(MPAuDecContext));
    mpctx->priv_data = calloc(1, sizeof(MPADecodeContext));
    if (mpctx->priv_data == NULL)
        return -1;
    s = mpctx->priv_data;

    if (!init && !mpctx->parse_only) {
        /* scale factors table for layer 1/2 */
        for(i=0;i<64;i++) {
            int shift, mod;
            /* 1.0 (i = 3) is normalized to 2 ^ FRAC_BITS */
            shift = (i / 3);
            mod = i % 3;
            scale_factor_modshift[i] = mod | (shift << 2);
        }

        /* scale factor multiply for layer 1 */
        for(i=0;i<15;i++) {
            int n, norm;
            n = i + 2;
            norm = (((int64_t)(1) << n) * FRAC_ONE) / ((1 << n) - 1);
            scale_factor_mult[i][0] = MULL(FIXR(1.0 * 2.0), norm);
            scale_factor_mult[i][1] = MULL(FIXR(0.7937005259 * 2.0), norm);
            scale_factor_mult[i][2] = MULL(FIXR(0.6299605249 * 2.0), norm);
/*#ifdef DEBUG
            printf("%d: norm=%x s=%x %x %x\n",
                   i, norm, 
                   scale_factor_mult[i][0],
                   scale_factor_mult[i][1],
                   scale_factor_mult[i][2]);
#endif*/
        }
        
        /* window */
        /* max = 18760, max sum over all 16 coefs : 44736 */
        for(i=0;i<257;i++) {
            int v;
            v = mpa_enwindow[i];
#if WFRAC_BITS < 16
            v = (v + (1 << (16 - WFRAC_BITS - 1))) >> (16 - WFRAC_BITS);
#endif
            window[i] = v;
            if ((i & 63) != 0)
                v = -v;
            if (i != 0)
                window[512 - i] = v;
        }
        
        /* huffman decode tables */
        huff_code_table[0] = NULL;
        for(i=1;i<16;i++) {
            const HuffTable *h = &mpa_huff_tables[i];
            int xsize, x, y;
            unsigned int n;
            uint8_t *code_table;

            xsize = h->xsize;
            n = xsize * xsize;
            /* XXX: fail test */
            init_vlc(&huff_vlc[i], 8, n, 
                     h->bits, 1, 1, h->codes, 2, 2);
            
            code_table = calloc(n, 1);
            j = 0;
            for(x=0;x<xsize;x++) {
                for(y=0;y<xsize;y++)
                    code_table[j++] = (x << 4) | y;
            }
            huff_code_table[i] = code_table;
        }
        for(i=0;i<2;i++) {
            init_vlc(&huff_quad_vlc[i], i == 0 ? 7 : 4, 16, 
                     mpa_quad_bits[i], 1, 1, mpa_quad_codes[i], 1, 1);
        }

        for(i=0;i<9;i++) {
            k = 0;
            for(j=0;j<22;j++) {
                band_index_long[i][j] = k;
                k += band_size_long[i][j];
            }
            band_index_long[i][22] = k;
        }

        /* compute n ^ (4/3) and store it in mantissa/exp format */
        int_pow_init();
        for(i=1;i<TABLE_4_3_SIZE;i++) {
            int e, m;
            m = int_pow(i, &e);
            /* normalized to FRAC_BITS */
            table_4_3_value[i] = m;
            table_4_3_exp[i] = e;
        }
        
        for(i=0;i<7;i++) {
            float f;
            int v;
            if (i != 6) {
                f = tan((double)i * M_PI / 12.0);
                v = FIXR(f / (1.0 + f));
            } else {
                v = FIXR(1.0);
            }
            is_table[0][i] = v;
            is_table[1][6 - i] = v;
        }
        /* invalid values */
        for(i=7;i<16;i++)
            is_table[0][i] = is_table[1][i] = 0.0;

        for(i=0;i<16;i++) {
            double f;
            int e, k;

            for(j=0;j<2;j++) {
                e = -(j + 1) * ((i + 1) >> 1);
                f = pow(2.0, e / 4.0);
                k = i & 1;
                is_table_lsf[j][k ^ 1][i] = FIXR(f);
                is_table_lsf[j][k][i] = FIXR(1.0);
/*#ifdef DEBUG
                printf("is_table_lsf %d %d: %x %x\n", 
                       i, j, is_table_lsf[j][0][i], is_table_lsf[j][1][i]);
#endif*/
            }
        }

        for(i=0;i<8;i++) {
            float ci, cs, ca;
            ci = ci_table[i];
            cs = 1.0 / sqrt(1.0 + ci * ci);
            ca = cs * ci;
            csa_table[i][0] = FIX(cs);
            csa_table[i][1] = FIX(ca);
        }

        /* compute mdct windows */
        for(i=0;i<36;i++) {
            int v;
            v = FIXR(sin(M_PI * (i + 0.5) / 36.0));
            mdct_win[0][i] = v;
            mdct_win[1][i] = v;
            mdct_win[3][i] = v;
        }
        for(i=0;i<6;i++) {
            mdct_win[1][18 + i] = FIXR(1.0);
            mdct_win[1][24 + i] = FIXR(sin(M_PI * ((i + 6) + 0.5) / 12.0));
            mdct_win[1][30 + i] = FIXR(0.0);

            mdct_win[3][i] = FIXR(0.0);
            mdct_win[3][6 + i] = FIXR(sin(M_PI * (i + 0.5) / 12.0));
            mdct_win[3][12 + i] = FIXR(1.0);
        }

        for(i=0;i<12;i++)
            mdct_win[2][i] = FIXR(sin(M_PI * (i + 0.5) / 12.0));
        
        /* NOTE: we do frequency inversion adter the MDCT by changing
           the sign of the right window coefs */
        for(j=0;j<4;j++) {
            for(i=0;i<36;i+=2) {
                mdct_win[j + 4][i] = mdct_win[j][i];
                mdct_win[j + 4][i + 1] = -mdct_win[j][i + 1];
            }
        }

/*#if defined(DEBUG)
        for(j=0;j<8;j++) {
            printf("win%d=\n", j);
            for(i=0;i<36;i++)
                printf("%f, ", (double)mdct_win[j][i] / FRAC_ONE);
            printf("\n");
        }
#endif*/
        init = 1;
    }

    s->inbuf_index = 0;
    s->inbuf = &s->inbuf1[s->inbuf_index][BACKSTEP_SIZE];
    s->inbuf_ptr = s->inbuf;
#ifdef DEBUG
    s->frame_count = 0;
#endif
    return 0;
}

/* tab[i][j] = 1.0 / (2.0 * cos(pi*(2*k+1) / 2^(6 - j))) */

/* cos(i*pi/64) */

#define COS0_0  FIXR(0.50060299823519630134)
#define COS0_1  FIXR(0.50547095989754365998)
#define COS0_2  FIXR(0.51544730992262454697)
#define COS0_3  FIXR(0.53104259108978417447)
#define COS0_4  FIXR(0.55310389603444452782)
#define COS0_5  FIXR(0.58293496820613387367)
#define COS0_6  FIXR(0.62250412303566481615)
#define COS0_7  FIXR(0.67480834145500574602)
#define COS0_8  FIXR(0.74453627100229844977)
#define COS0_9  FIXR(0.83934964541552703873)
#define COS0_10 FIXR(0.97256823786196069369)
#define COS0_11 FIXR(1.16943993343288495515)
#define COS0_12 FIXR(1.48416461631416627724)
#define COS0_13 FIXR(2.05778100995341155085)
#define COS0_14 FIXR(3.40760841846871878570)
#define COS0_15 FIXR(10.19000812354805681150)

#define COS1_0 FIXR(0.50241928618815570551)
#define COS1_1 FIXR(0.52249861493968888062)
#define COS1_2 FIXR(0.56694403481635770368)
#define COS1_3 FIXR(0.64682178335999012954)
#define COS1_4 FIXR(0.78815462345125022473)
#define COS1_5 FIXR(1.06067768599034747134)
#define COS1_6 FIXR(1.72244709823833392782)
#define COS1_7 FIXR(5.10114861868916385802)

#define COS2_0 FIXR(0.50979557910415916894)
#define COS2_1 FIXR(0.60134488693504528054)
#define COS2_2 FIXR(0.89997622313641570463)
#define COS2_3 FIXR(2.56291544774150617881)

#define COS3_0 FIXR(0.54119610014619698439)
#define COS3_1 FIXR(1.30656296487637652785)

#define COS4_0 FIXR(0.70710678118654752439)

/* butterfly operator */
#define BF(a, b, c)\
{\
    tmp0 = tab[a] + tab[b];\
    tmp1 = tab[a] - tab[b];\
    tab[a] = tmp0;\
    tab[b] = MULL(tmp1, c);\
}

#define BF1(a, b, c, d)\
{\
    BF(a, b, COS4_0);\
    BF(c, d, -COS4_0);\
    tab[c] += tab[d];\
}

#define BF2(a, b, c, d)\
{\
    BF(a, b, COS4_0);\
    BF(c, d, -COS4_0);\
    tab[c] += tab[d];\
    tab[a] += tab[c];\
    tab[c] += tab[b];\
    tab[b] += tab[d];\
}

#define ADD(a, b) tab[a] += tab[b]

/* DCT32 without 1/sqrt(2) coef zero scaling. */
static void dct32(int32_t *out, int32_t *tab)
{
    int tmp0, tmp1;

    /* pass 1 */
    BF(0, 31, COS0_0);
    BF(1, 30, COS0_1);
    BF(2, 29, COS0_2);
    BF(3, 28, COS0_3);
    BF(4, 27, COS0_4);
    BF(5, 26, COS0_5);
    BF(6, 25, COS0_6);
    BF(7, 24, COS0_7);
    BF(8, 23, COS0_8);
    BF(9, 22, COS0_9);
    BF(10, 21, COS0_10);
    BF(11, 20, COS0_11);
    BF(12, 19, COS0_12);
    BF(13, 18, COS0_13);
    BF(14, 17, COS0_14);
    BF(15, 16, COS0_15);

    /* pass 2 */
    BF(0, 15, COS1_0);
    BF(1, 14, COS1_1);
    BF(2, 13, COS1_2);
    BF(3, 12, COS1_3);
    BF(4, 11, COS1_4);
    BF(5, 10, COS1_5);
    BF(6,  9, COS1_6);
    BF(7,  8, COS1_7);
    
    BF(16, 31, -COS1_0);
    BF(17, 30, -COS1_1);
    BF(18, 29, -COS1_2);
    BF(19, 28, -COS1_3);
    BF(20, 27, -COS1_4);
    BF(21, 26, -COS1_5);
    BF(22, 25, -COS1_6);
    BF(23, 24, -COS1_7);
    
    /* pass 3 */
    BF(0, 7, COS2_0);
    BF(1, 6, COS2_1);
    BF(2, 5, COS2_2);
    BF(3, 4, COS2_3);
    
    BF(8, 15, -COS2_0);
    BF(9, 14, -COS2_1);
    BF(10, 13, -COS2_2);
    BF(11, 12, -COS2_3);
    
    BF(16, 23, COS2_0);
    BF(17, 22, COS2_1);
    BF(18, 21, COS2_2);
    BF(19, 20, COS2_3);
    
    BF(24, 31, -COS2_0);
    BF(25, 30, -COS2_1);
    BF(26, 29, -COS2_2);
    BF(27, 28, -COS2_3);

    /* pass 4 */
    BF(0, 3, COS3_0);
    BF(1, 2, COS3_1);
    
    BF(4, 7, -COS3_0);
    BF(5, 6, -COS3_1);
    
    BF(8, 11, COS3_0);
    BF(9, 10, COS3_1);
    
    BF(12, 15, -COS3_0);
    BF(13, 14, -COS3_1);
    
    BF(16, 19, COS3_0);
    BF(17, 18, COS3_1);
    
    BF(20, 23, -COS3_0);
    BF(21, 22, -COS3_1);
    
    BF(24, 27, COS3_0);
    BF(25, 26, COS3_1);
    
    BF(28, 31, -COS3_0);
    BF(29, 30, -COS3_1);
    
    /* pass 5 */
    BF1(0, 1, 2, 3);
    BF2(4, 5, 6, 7);
    BF1(8, 9, 10, 11);
    BF2(12, 13, 14, 15);
    BF1(16, 17, 18, 19);
    BF2(20, 21, 22, 23);
    BF1(24, 25, 26, 27);
    BF2(28, 29, 30, 31);
    
    /* pass 6 */
    
    ADD( 8, 12);
    ADD(12, 10);
    ADD(10, 14);
    ADD(14,  9);
    ADD( 9, 13);
    ADD(13, 11);
    ADD(11, 15);

    out[ 0] = tab[0];
    out[16] = tab[1];
    out[ 8] = tab[2];
    out[24] = tab[3];
    out[ 4] = tab[4];
    out[20] = tab[5];
    out[12] = tab[6];
    out[28] = tab[7];
    out[ 2] = tab[8];
    out[18] = tab[9];
    out[10] = tab[10];
    out[26] = tab[11];
    out[ 6] = tab[12];
    out[22] = tab[13];
    out[14] = tab[14];
    out[30] = tab[15];
    
    ADD(24, 28);
    ADD(28, 26);
    ADD(26, 30);
    ADD(30, 25);
    ADD(25, 29);
    ADD(29, 27);
    ADD(27, 31);

    out[ 1] = tab[16] + tab[24];
    out[17] = tab[17] + tab[25];
    out[ 9] = tab[18] + tab[26];
    out[25] = tab[19] + tab[27];
    out[ 5] = tab[20] + tab[28];
    out[21] = tab[21] + tab[29];
    out[13] = tab[22] + tab[30];
    out[29] = tab[23] + tab[31];
    out[ 3] = tab[24] + tab[20];
    out[19] = tab[25] + tab[21];
    out[11] = tab[26] + tab[22];
    out[27] = tab[27] + tab[23];
    out[ 7] = tab[28] + tab[18];
    out[23] = tab[29] + tab[19];
    out[15] = tab[30] + tab[17];
    out[31] = tab[31];
}

#define OUT_SHIFT (WFRAC_BITS + FRAC_BITS - 15)

#if FRAC_BITS <= 15

static int round_sample(int sum)
{
    int sum1;
    sum1 = (sum + (1 << (OUT_SHIFT - 1))) >> OUT_SHIFT;
    if (sum1 < -32768)
        sum1 = -32768;
    else if (sum1 > 32767)
        sum1 = 32767;
    return sum1;
}

/* signed 16x16 -> 32 multiply add accumulate */
#define MACS(rt, ra, rb) rt += (ra) * (rb)

/* signed 16x16 -> 32 multiply */
#define MULS(ra, rb) ((ra) * (rb))

#else

static int round_sample(int64_t sum) 
{
    int sum1;
    sum1 = (int)((sum + ((int64_t)(1) << (OUT_SHIFT - 1))) >> OUT_SHIFT);
    if (sum1 < -32768)
        sum1 = -32768;
    else if (sum1 > 32767)
        sum1 = 32767;
    return sum1;
}

#define MULS(ra, rb) MUL64(ra, rb)

#endif

#define SUM8(sum, op, w, p) \
{                                               \
    sum op MULS((w)[0 * 64], p[0 * 64]);\
    sum op MULS((w)[1 * 64], p[1 * 64]);\
    sum op MULS((w)[2 * 64], p[2 * 64]);\
    sum op MULS((w)[3 * 64], p[3 * 64]);\
    sum op MULS((w)[4 * 64], p[4 * 64]);\
    sum op MULS((w)[5 * 64], p[5 * 64]);\
    sum op MULS((w)[6 * 64], p[6 * 64]);\
    sum op MULS((w)[7 * 64], p[7 * 64]);\
}

#define SUM8P2(sum1, op1, sum2, op2, w1, w2, p) \
{                                               \
    int tmp;\
    tmp = p[0 * 64];\
    sum1 op1 MULS((w1)[0 * 64], tmp);\
    sum2 op2 MULS((w2)[0 * 64], tmp);\
    tmp = p[1 * 64];\
    sum1 op1 MULS((w1)[1 * 64], tmp);\
    sum2 op2 MULS((w2)[1 * 64], tmp);\
    tmp = p[2 * 64];\
    sum1 op1 MULS((w1)[2 * 64], tmp);\
    sum2 op2 MULS((w2)[2 * 64], tmp);\
    tmp = p[3 * 64];\
    sum1 op1 MULS((w1)[3 * 64], tmp);\
    sum2 op2 MULS((w2)[3 * 64], tmp);\
    tmp = p[4 * 64];\
    sum1 op1 MULS((w1)[4 * 64], tmp);\
    sum2 op2 MULS((w2)[4 * 64], tmp);\
    tmp = p[5 * 64];\
    sum1 op1 MULS((w1)[5 * 64], tmp);\
    sum2 op2 MULS((w2)[5 * 64], tmp);\
    tmp = p[6 * 64];\
    sum1 op1 MULS((w1)[6 * 64], tmp);\
    sum2 op2 MULS((w2)[6 * 64], tmp);\
    tmp = p[7 * 64];\
    sum1 op1 MULS((w1)[7 * 64], tmp);\
    sum2 op2 MULS((w2)[7 * 64], tmp);\
}


/* 32 sub band synthesis filter. Input: 32 sub band samples, Output:
   32 samples. */
/* XXX: optimize by avoiding ring buffer usage */
static void synth_filter(MPADecodeContext *s1,
                         int ch, int16_t *samples, int incr, 
                         int32_t sb_samples[SBLIMIT])
{
    int32_t tmp[32];
    MPA_INT *synth_buf;
    const MPA_INT *w, *w2, *p;
    int j, offset, v;
    int16_t *samples2;
#if FRAC_BITS <= 15
    int32_t sum, sum2;
#else
    int64_t sum, sum2;
#endif
    
    dct32(tmp, sb_samples);
    
    offset = s1->synth_buf_offset[ch];
    synth_buf = s1->synth_buf[ch] + offset;

    for(j=0;j<32;j++) {
        v = tmp[j];
#if FRAC_BITS <= 15
        /* NOTE: can cause a loss in precision if very high amplitude
           sound */
        if (v > 32767)
            v = 32767;
        else if (v < -32768)
            v = -32768;
#endif
        synth_buf[j] = v;
    }
    /* copy to avoid wrap */
    memcpy(synth_buf + 512, synth_buf, 32 * sizeof(MPA_INT));

    samples2 = samples + 31 * incr;
    w = window;
    w2 = window + 31;

    sum = 0;
    p = synth_buf + 16;
    SUM8(sum, +=, w, p);
    p = synth_buf + 48;
    SUM8(sum, -=, w + 32, p);
    *samples = round_sample(sum);
    samples += incr;
    w++;

    /* we calculate two samples at the same time to avoid one memory
       access per two sample */
    for(j=1;j<16;j++) {
        sum = 0;
        sum2 = 0;
        p = synth_buf + 16 + j;
        SUM8P2(sum, +=, sum2, -=, w, w2, p);
        p = synth_buf + 48 - j;
        SUM8P2(sum, -=, sum2, -=, w + 32, w2 + 32, p);

        *samples = round_sample(sum);
        samples += incr;
        *samples2 = round_sample(sum2);
        samples2 -= incr;
        w++;
        w2--;
    }
    
    p = synth_buf + 32;
    sum = 0;
    SUM8(sum, -=, w + 32, p);
    *samples = round_sample(sum);

    offset = (offset - 32) & 511;
    s1->synth_buf_offset[ch] = offset;
}

/* cos(pi*i/24) */
#define C1  FIXR(0.99144486137381041114)
#define C3  FIXR(0.92387953251128675612)
#define C5  FIXR(0.79335334029123516458)
#define C7  FIXR(0.60876142900872063941)
#define C9  FIXR(0.38268343236508977173)
#define C11 FIXR(0.13052619222005159154)

/* 12 points IMDCT. We compute it "by hand" by factorizing obvious
   cases. */
static void imdct12(int *out, int *in)
{
    int tmp;
    int64_t in1_3, in1_9, in4_3, in4_9;

    in1_3 = MUL64(in[1], C3);
    in1_9 = MUL64(in[1], C9);
    in4_3 = MUL64(in[4], C3);
    in4_9 = MUL64(in[4], C9);
    
    tmp = FRAC_RND(MUL64(in[0], C7) - in1_3 - MUL64(in[2], C11) + 
                   MUL64(in[3], C1) - in4_9 - MUL64(in[5], C5));
    out[0] = tmp;
    out[5] = -tmp;
    tmp = FRAC_RND(MUL64(in[0] - in[3], C9) - in1_3 + 
                   MUL64(in[2] + in[5], C3) - in4_9);
    out[1] = tmp;
    out[4] = -tmp;
    tmp = FRAC_RND(MUL64(in[0], C11) - in1_9 + MUL64(in[2], C7) -
                   MUL64(in[3], C5) + in4_3 - MUL64(in[5], C1));
    out[2] = tmp;
    out[3] = -tmp;
    tmp = FRAC_RND(MUL64(-in[0], C5) + in1_9 + MUL64(in[2], C1) + 
                   MUL64(in[3], C11) - in4_3 - MUL64(in[5], C7));
    out[6] = tmp;
    out[11] = tmp;
    tmp = FRAC_RND(MUL64(-in[0] + in[3], C3) - in1_9 + 
                   MUL64(in[2] + in[5], C9) + in4_3);
    out[7] = tmp;
    out[10] = tmp;
    tmp = FRAC_RND(-MUL64(in[0], C1) - in1_3 - MUL64(in[2], C5) -
                   MUL64(in[3], C7) - in4_9 - MUL64(in[5], C11));
    out[8] = tmp;
    out[9] = tmp;
}

#undef C1
#undef C3
#undef C5
#undef C7
#undef C9
#undef C11

/* cos(pi*i/18) */
#define C1 FIXR(0.98480775301220805936)
#define C2 FIXR(0.93969262078590838405)
#define C3 FIXR(0.86602540378443864676)
#define C4 FIXR(0.76604444311897803520)
#define C5 FIXR(0.64278760968653932632)
#define C6 FIXR(0.5)
#define C7 FIXR(0.34202014332566873304)
#define C8 FIXR(0.17364817766693034885)

/* 0.5 / cos(pi*(2*i+1)/36) */
static const int icos36[9] = {
    FIXR(0.50190991877167369479),
    FIXR(0.51763809020504152469),
    FIXR(0.55168895948124587824),
    FIXR(0.61038729438072803416),
    FIXR(0.70710678118654752439),
    FIXR(0.87172339781054900991),
    FIXR(1.18310079157624925896),
    FIXR(1.93185165257813657349),
    FIXR(5.73685662283492756461),
};

static const int icos72[18] = {
    /* 0.5 / cos(pi*(2*i+19)/72) */
    FIXR(0.74009361646113053152),
    FIXR(0.82133981585229078570),
    FIXR(0.93057949835178895673),
    FIXR(1.08284028510010010928),
    FIXR(1.30656296487637652785),
    FIXR(1.66275476171152078719),
    FIXR(2.31011315767264929558),
    FIXR(3.83064878777019433457),
    FIXR(11.46279281302667383546),

    /* 0.5 / cos(pi*(2*(i + 18) +19)/72) */
    FIXR(-0.67817085245462840086),
    FIXR(-0.63023620700513223342),
    FIXR(-0.59284452371708034528),
    FIXR(-0.56369097343317117734),
    FIXR(-0.54119610014619698439),
    FIXR(-0.52426456257040533932),
    FIXR(-0.51213975715725461845),
    FIXR(-0.50431448029007636036),
    FIXR(-0.50047634258165998492),
};

/* using Lee like decomposition followed by hand coded 9 points DCT */
static void imdct36(int *out, int *in)
{
    int i, j, t0, t1, t2, t3, s0, s1, s2, s3;
    int tmp[18], *tmp1, *in1;
    int64_t in3_3, in6_6;

    for(i=17;i>=1;i--)
        in[i] += in[i-1];
    for(i=17;i>=3;i-=2)
        in[i] += in[i-2];

    for(j=0;j<2;j++) {
        tmp1 = tmp + j;
        in1 = in + j;

        in3_3 = MUL64(in1[2*3], C3);
        in6_6 = MUL64(in1[2*6], C6);

        tmp1[0] = FRAC_RND(MUL64(in1[2*1], C1) + in3_3 + 
                           MUL64(in1[2*5], C5) + MUL64(in1[2*7], C7));
        tmp1[2] = in1[2*0] + FRAC_RND(MUL64(in1[2*2], C2) + 
                                      MUL64(in1[2*4], C4) + in6_6 + 
                                      MUL64(in1[2*8], C8));
        tmp1[4] = FRAC_RND(MUL64(in1[2*1] - in1[2*5] - in1[2*7], C3));
        tmp1[6] = FRAC_RND(MUL64(in1[2*2] - in1[2*4] - in1[2*8], C6)) - 
            in1[2*6] + in1[2*0];
        tmp1[8] = FRAC_RND(MUL64(in1[2*1], C5) - in3_3 - 
                           MUL64(in1[2*5], C7) + MUL64(in1[2*7], C1));
        tmp1[10] = in1[2*0] + FRAC_RND(MUL64(-in1[2*2], C8) - 
                                       MUL64(in1[2*4], C2) + in6_6 + 
                                       MUL64(in1[2*8], C4));
        tmp1[12] = FRAC_RND(MUL64(in1[2*1], C7) - in3_3 + 
                            MUL64(in1[2*5], C1) - 
                            MUL64(in1[2*7], C5));
        tmp1[14] = in1[2*0] + FRAC_RND(MUL64(-in1[2*2], C4) + 
                                       MUL64(in1[2*4], C8) + in6_6 - 
                                       MUL64(in1[2*8], C2));
        tmp1[16] = in1[2*0] - in1[2*2] + in1[2*4] - in1[2*6] + in1[2*8];
    }

    i = 0;
    for(j=0;j<4;j++) {
        t0 = tmp[i];
        t1 = tmp[i + 2];
        s0 = t1 + t0;
        s2 = t1 - t0;

        t2 = tmp[i + 1];
        t3 = tmp[i + 3];
        s1 = MULL(t3 + t2, icos36[j]);
        s3 = MULL(t3 - t2, icos36[8 - j]);
        
        t0 = MULL(s0 + s1, icos72[9 + 8 - j]);
        t1 = MULL(s0 - s1, icos72[8 - j]);
        out[18 + 9 + j] = t0;
        out[18 + 8 - j] = t0;
        out[9 + j] = -t1;
        out[8 - j] = t1;
        
        t0 = MULL(s2 + s3, icos72[9+j]);
        t1 = MULL(s2 - s3, icos72[j]);
        out[18 + 9 + (8 - j)] = t0;
        out[18 + j] = t0;
        out[9 + (8 - j)] = -t1;
        out[j] = t1;
        i += 4;
    }

    s0 = tmp[16];
    s1 = MULL(tmp[17], icos36[4]);
    t0 = MULL(s0 + s1, icos72[9 + 4]);
    t1 = MULL(s0 - s1, icos72[4]);
    out[18 + 9 + 4] = t0;
    out[18 + 8 - 4] = t0;
    out[9 + 4] = -t1;
    out[8 - 4] = t1;
}

/* fast header check for resync */
static int check_header(uint32_t header)
{
    /* header */
    if ((header & 0xffe00000) != 0xffe00000)
        return -1;
    /* layer check */
    if (((header >> 17) & 3) == 0)
        return -1;
    /* bit rate */
    if (((header >> 12) & 0xf) == 0xf)
        return -1;
    /* frequency */
    if (((header >> 10) & 3) == 3)
        return -1;
    return 0;
}

/* header + layer + bitrate + freq + lsf/mpeg25 */
#define SAME_HEADER_MASK \
   (0xffe00000 | (3 << 17) | (0xf << 12) | (3 << 10) | (3 << 19))

/* header decoding. MUST check the header before because no
   consistency check is done there. Return 1 if free format found and
   that the frame size must be computed externally */
static int decode_header(MPADecodeContext *s, uint32_t header)
{
    int sample_rate, frame_size, mpeg25, padding;
    int sample_rate_index, bitrate_index;
    if (header & (1<<20)) {
        s->lsf = (header & (1<<19)) ? 0 : 1;
        mpeg25 = 0;
    } else {
        s->lsf = 1;
        mpeg25 = 1;
    }
    
    s->layer = 4 - ((header >> 17) & 3);
    /* extract frequency */
    sample_rate_index = (header >> 10) & 3;
    sample_rate = mpa_freq_tab[sample_rate_index] >> (s->lsf + mpeg25);
    sample_rate_index += 3 * (s->lsf + mpeg25);
    s->sample_rate_index = sample_rate_index;
    s->error_protection = ((header >> 16) & 1) ^ 1;
    s->sample_rate = sample_rate;

    bitrate_index = (header >> 12) & 0xf;
    padding = (header >> 9) & 1;
    s->mode = (header >> 6) & 3;
    s->mode_ext = (header >> 4) & 3;

    if (s->mode == MPA_MONO)
        s->nb_channels = 1;
    else
        s->nb_channels = 2;
    
    if (bitrate_index != 0) {
        frame_size = mpa_bitrate_tab[s->lsf][s->layer - 1][bitrate_index];
        s->bit_rate = frame_size * 1000;
        switch(s->layer) {
        case 1:
            frame_size = (frame_size * 12000) / sample_rate;
            frame_size = (frame_size + padding) * 4;
            break;
        case 2:
            frame_size = (frame_size * 144000) / sample_rate;
            frame_size += padding;
            break;
        default:
        case 3:
            frame_size = (frame_size * 144000) / (sample_rate << s->lsf);
            frame_size += padding;
            break;
        }
        s->frame_size = frame_size;
    } else {
        /* if no frame size computed, signal it */
        if (!s->free_format_frame_size)
            return 1;
        /* free format: compute bitrate and real frame size from the
           frame size we extracted by reading the bitstream */
        s->frame_size = s->free_format_frame_size;
        switch(s->layer) {
        case 1:
            s->frame_size += padding  * 4;
            s->bit_rate = (s->frame_size * sample_rate) / 48000;
            break;
        case 2:
            s->frame_size += padding;
            s->bit_rate = (s->frame_size * sample_rate) / 144000;
            break;
        default:
        case 3:
            s->frame_size += padding;
            s->bit_rate = (s->frame_size * (sample_rate << s->lsf)) / 144000;
            break;
        }
    }
    
/*#if defined(DEBUG)
    printf("layer%d, %d Hz, %d kbits/s, ",
           s->layer, s->sample_rate, s->bit_rate);
    if (s->nb_channels == 2) {
        if (s->layer == 3) {
            if (s->mode_ext & MODE_EXT_MS_STEREO)
                printf("ms-");
            if (s->mode_ext & MODE_EXT_I_STEREO)
                printf("i-");
        }
        printf("stereo");
    } else {
        printf("mono");
    }
    printf("\n");
#endif*/
    return 0;
}

/* return the number of decoded frames */
static int mp_decode_layer1(MPADecodeContext *s)
{
    int bound, i, v, n, ch, j, mant;
    uint8_t allocation[MPA_MAX_CHANNELS][SBLIMIT];
    uint8_t scale_factors[MPA_MAX_CHANNELS][SBLIMIT];

    if (s->mode == MPA_JSTEREO) 
        bound = (s->mode_ext + 1) * 4;
    else
        bound = SBLIMIT;

    /* allocation bits */
    for(i=0;i<bound;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            allocation[ch][i] = get_bits(&s->gb, 4);
        }
    }
    for(i=bound;i<SBLIMIT;i++) {
        allocation[0][i] = get_bits(&s->gb, 4);
    }

    /* scale factors */
    for(i=0;i<bound;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (allocation[ch][i])
                scale_factors[ch][i] = get_bits(&s->gb, 6);
        }
    }
    for(i=bound;i<SBLIMIT;i++) {
        if (allocation[0][i]) {
            scale_factors[0][i] = get_bits(&s->gb, 6);
            scale_factors[1][i] = get_bits(&s->gb, 6);
        }
    }
    
    /* compute samples */
    for(j=0;j<12;j++) {
        for(i=0;i<bound;i++) {
            for(ch=0;ch<s->nb_channels;ch++) {
                n = allocation[ch][i];
                if (n) {
                    mant = get_bits(&s->gb, n + 1);
                    v = l1_unscale(n, mant, scale_factors[ch][i]);
                } else {
                    v = 0;
                }
                s->sb_samples[ch][j][i] = v;
            }
        }
        for(i=bound;i<SBLIMIT;i++) {
            n = allocation[0][i];
            if (n) {
                mant = get_bits(&s->gb, n + 1);
                v = l1_unscale(n, mant, scale_factors[0][i]);
                s->sb_samples[0][j][i] = v;
                v = l1_unscale(n, mant, scale_factors[1][i]);
                s->sb_samples[1][j][i] = v;
            } else {
                s->sb_samples[0][j][i] = 0;
                s->sb_samples[1][j][i] = 0;
            }
        }
    }
    return 12;
}

/* bitrate is in kb/s */
static int l2_select_table(int bitrate, int nb_channels, int freq, int lsf)
{
    int ch_bitrate, table;
    
    ch_bitrate = bitrate / nb_channels;
    if (!lsf) {
        if ((freq == 48000 && ch_bitrate >= 56) ||
            (ch_bitrate >= 56 && ch_bitrate <= 80)) 
            table = 0;
        else if (freq != 48000 && ch_bitrate >= 96) 
            table = 1;
        else if (freq != 32000 && ch_bitrate <= 48) 
            table = 2;
        else 
            table = 3;
    } else {
        table = 4;
    }
    return table;
}

static int mp_decode_layer2(MPADecodeContext *s)
{
    int sblimit; /* number of used subbands */
    const unsigned char *alloc_table;
    int table, bit_alloc_bits, i, j, ch, bound, v;
    unsigned char bit_alloc[MPA_MAX_CHANNELS][SBLIMIT];
    unsigned char scale_code[MPA_MAX_CHANNELS][SBLIMIT];
    unsigned char scale_factors[MPA_MAX_CHANNELS][SBLIMIT][3], *sf;
    int scale, qindex, bits, steps, k, l, m, b;

    /* select decoding table */
    table = l2_select_table(s->bit_rate / 1000, s->nb_channels, 
                            s->sample_rate, s->lsf);
    sblimit = sblimit_table[table];
    alloc_table = alloc_tables[table];

    if (s->mode == MPA_JSTEREO) 
        bound = (s->mode_ext + 1) * 4;
    else
        bound = sblimit;

/*#ifdef DEBUG
    printf("bound=%d sblimit=%d\n", bound, sblimit);
#endif*/
    /* parse bit allocation */
    j = 0;
    for(i=0;i<bound;i++) {
        bit_alloc_bits = alloc_table[j];
        for(ch=0;ch<s->nb_channels;ch++) {
            bit_alloc[ch][i] = get_bits(&s->gb, bit_alloc_bits);
        }
        j += 1 << bit_alloc_bits;
    }
    for(i=bound;i<sblimit;i++) {
        bit_alloc_bits = alloc_table[j];
        v = get_bits(&s->gb, bit_alloc_bits);
        bit_alloc[0][i] = v;
        bit_alloc[1][i] = v;
        j += 1 << bit_alloc_bits;
    }

/*#ifdef DEBUG
    {
        for(ch=0;ch<s->nb_channels;ch++) {
            for(i=0;i<sblimit;i++)
                printf(" %d", bit_alloc[ch][i]);
            printf("\n");
        }
    }
#endif*/

    /* scale codes */
    for(i=0;i<sblimit;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (bit_alloc[ch][i]) 
                scale_code[ch][i] = get_bits(&s->gb, 2);
        }
    }
    
    /* scale factors */
    for(i=0;i<sblimit;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (bit_alloc[ch][i]) {
                sf = scale_factors[ch][i];
                switch(scale_code[ch][i]) {
                default:
                case 0:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[1] = get_bits(&s->gb, 6);
                    sf[2] = get_bits(&s->gb, 6);
                    break;
                case 2:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[1] = sf[0];
                    sf[2] = sf[0];
                    break;
                case 1:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[2] = get_bits(&s->gb, 6);
                    sf[1] = sf[0];
                    break;
                case 3:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[2] = get_bits(&s->gb, 6);
                    sf[1] = sf[2];
                    break;
                }
            }
        }
    }

/*#ifdef DEBUG
    for(ch=0;ch<s->nb_channels;ch++) {
        for(i=0;i<sblimit;i++) {
            if (bit_alloc[ch][i]) {
                sf = scale_factors[ch][i];
                printf(" %d %d %d", sf[0], sf[1], sf[2]);
            } else {
                printf(" -");
            }
        }
        printf("\n");
    }
#endif*/

    /* samples */
    for(k=0;k<3;k++) {
        for(l=0;l<12;l+=3) {
            j = 0;
            for(i=0;i<bound;i++) {
                bit_alloc_bits = alloc_table[j];
                for(ch=0;ch<s->nb_channels;ch++) {
                    b = bit_alloc[ch][i];
                    if (b) {
                        scale = scale_factors[ch][i][k];
                        qindex = alloc_table[j+b];
                        bits = quant_bits[qindex];
                        if (bits < 0) {
                            /* 3 values at the same time */
                            v = get_bits(&s->gb, -bits);
                            steps = quant_steps[qindex];
                            s->sb_samples[ch][k * 12 + l + 0][i] = 
                                l2_unscale_group(steps, v % steps, scale);
                            v = v / steps;
                            s->sb_samples[ch][k * 12 + l + 1][i] = 
                                l2_unscale_group(steps, v % steps, scale);
                            v = v / steps;
                            s->sb_samples[ch][k * 12 + l + 2][i] = 
                                l2_unscale_group(steps, v, scale);
                        } else {
                            for(m=0;m<3;m++) {
                                v = get_bits(&s->gb, bits);
                                v = l1_unscale(bits - 1, v, scale);
                                s->sb_samples[ch][k * 12 + l + m][i] = v;
                            }
                        }
                    } else {
                        s->sb_samples[ch][k * 12 + l + 0][i] = 0;
                        s->sb_samples[ch][k * 12 + l + 1][i] = 0;
                        s->sb_samples[ch][k * 12 + l + 2][i] = 0;
                    }
                }
                /* next subband in alloc table */
                j += 1 << bit_alloc_bits; 
            }
            /* XXX: find a way to avoid this duplication of code */
            for(i=bound;i<sblimit;i++) {
                bit_alloc_bits = alloc_table[j];
                b = bit_alloc[0][i];
                if (b) {
                    int mant, scale0, scale1;
                    scale0 = scale_factors[0][i][k];
                    scale1 = scale_factors[1][i][k];
                    qindex = alloc_table[j+b];
                    bits = quant_bits[qindex];
                    if (bits < 0) {
                        /* 3 values at the same time */
                        v = get_bits(&s->gb, -bits);
                        steps = quant_steps[qindex];
                        mant = v % steps;
                        v = v / steps;
                        s->sb_samples[0][k * 12 + l + 0][i] = 
                            l2_unscale_group(steps, mant, scale0);
                        s->sb_samples[1][k * 12 + l + 0][i] = 
                            l2_unscale_group(steps, mant, scale1);
                        mant = v % steps;
                        v = v / steps;
                        s->sb_samples[0][k * 12 + l + 1][i] = 
                            l2_unscale_group(steps, mant, scale0);
                        s->sb_samples[1][k * 12 + l + 1][i] = 
                            l2_unscale_group(steps, mant, scale1);
                        s->sb_samples[0][k * 12 + l + 2][i] = 
                            l2_unscale_group(steps, v, scale0);
                        s->sb_samples[1][k * 12 + l + 2][i] = 
                            l2_unscale_group(steps, v, scale1);
                    } else {
                        for(m=0;m<3;m++) {
                            mant = get_bits(&s->gb, bits);
                            s->sb_samples[0][k * 12 + l + m][i] = 
                                l1_unscale(bits - 1, mant, scale0);
                            s->sb_samples[1][k * 12 + l + m][i] = 
                                l1_unscale(bits - 1, mant, scale1);
                        }
                    }
                } else {
                    s->sb_samples[0][k * 12 + l + 0][i] = 0;
                    s->sb_samples[0][k * 12 + l + 1][i] = 0;
                    s->sb_samples[0][k * 12 + l + 2][i] = 0;
                    s->sb_samples[1][k * 12 + l + 0][i] = 0;
                    s->sb_samples[1][k * 12 + l + 1][i] = 0;
                    s->sb_samples[1][k * 12 + l + 2][i] = 0;
                }
                /* next subband in alloc table */
                j += 1 << bit_alloc_bits; 
            }
            /* fill remaining samples to zero */
            for(i=sblimit;i<SBLIMIT;i++) {
                for(ch=0;ch<s->nb_channels;ch++) {
                    s->sb_samples[ch][k * 12 + l + 0][i] = 0;
                    s->sb_samples[ch][k * 12 + l + 1][i] = 0;
                    s->sb_samples[ch][k * 12 + l + 2][i] = 0;
                }
            }
        }
    }
    return 3 * 12;
}

/*
 * Seek back in the stream for backstep bytes (at most 511 bytes)
 */
static void seek_to_maindata(MPADecodeContext *s, unsigned int backstep)
{
    uint8_t *ptr;

    /* compute current position in stream */
    ptr = (uint8_t *)(s->gb.buffer + (get_bits_count(&s->gb)>>3));

    /* copy old data before current one */
    ptr -= backstep;
    memcpy(ptr, s->inbuf1[s->inbuf_index ^ 1] + 
           BACKSTEP_SIZE + s->old_frame_size - backstep, backstep);
    /* init get bits again */
    init_get_bits(&s->gb, ptr, (s->frame_size + backstep)*8);

    /* prepare next buffer */
    s->inbuf_index ^= 1;
    s->inbuf = &s->inbuf1[s->inbuf_index][BACKSTEP_SIZE];
    s->old_frame_size = s->frame_size;
}

static void lsf_sf_expand(int *slen,
                          int sf, int n1, int n2, int n3)
{
    if (n3) {
        slen[3] = sf % n3;
        sf /= n3;
    } else {
        slen[3] = 0;
    }
    if (n2) {
        slen[2] = sf % n2;
        sf /= n2;
    } else {
        slen[2] = 0;
    }
    slen[1] = sf % n1;
    sf /= n1;
    slen[0] = sf;
}

static void exponents_from_scale_factors(MPADecodeContext *s, 
                                         GranuleDef *g,
                                         int16_t *exponents)
{
    const uint8_t *bstab, *pretab;
    int len, i, j, k, l, v0, shift, gain, gains[3];
    int16_t *exp_ptr;

    exp_ptr = exponents;
    gain = g->global_gain - 210;
    shift = g->scalefac_scale + 1;

    bstab = band_size_long[s->sample_rate_index];
    pretab = mpa_pretab[g->preflag];
    for(i=0;i<g->long_end;i++) {
        v0 = gain - ((g->scale_factors[i] + pretab[i]) << shift);
        len = bstab[i];
        for(j=len;j>0;j--)
            *exp_ptr++ = v0;
    }

    if (g->short_start < 13) {
        bstab = band_size_short[s->sample_rate_index];
        gains[0] = gain - (g->subblock_gain[0] << 3);
        gains[1] = gain - (g->subblock_gain[1] << 3);
        gains[2] = gain - (g->subblock_gain[2] << 3);
        k = g->long_end;
        for(i=g->short_start;i<13;i++) {
            len = bstab[i];
            for(l=0;l<3;l++) {
                v0 = gains[l] - (g->scale_factors[k++] << shift);
                for(j=len;j>0;j--)
                *exp_ptr++ = v0;
            }
        }
    }
}

/* handle n = 0 too */
static int get_bitsz(GetBitContext *s, int n)
{
    if (n == 0)
        return 0;
    else
        return get_bits(s, n);
}

static int huffman_decode(MPADecodeContext *s, GranuleDef *g,
                          int16_t *exponents, int end_pos)
{
    int s_index;
    int linbits, code, x, y, l, v, i, j, k, pos;
    GetBitContext last_gb;
    VLC *vlc;
    uint8_t *code_table;

    /* low frequencies (called big values) */
    s_index = 0;
    for(i=0;i<3;i++) {
        j = g->region_size[i];
        if (j == 0)
            continue;
        /* select vlc table */
        k = g->table_select[i];
        l = mpa_huff_data[k][0];
        linbits = mpa_huff_data[k][1];
        vlc = &huff_vlc[l];
        code_table = huff_code_table[l];

        /* read huffcode and compute each couple */
        for(;j>0;j--) {
            if (get_bits_count(&s->gb) >= end_pos)
                break;
            if (code_table) {
                code = get_vlc(&s->gb, vlc);
                if (code < 0)
                    return -1;
                y = code_table[code];
                x = y >> 4;
                y = y & 0x0f;
            } else {
                x = 0;
                y = 0;
            }
/*#ifdef DEBUG
            printf("region=%d n=%d x=%d y=%d exp=%d\n", 
                   i, g->region_size[i] - j, x, y, exponents[s_index]);
#endif*/
            if (x) {
                if (x == 15)
                    x += get_bitsz(&s->gb, linbits);
                v = l3_unscale(x, exponents[s_index]);
                if (get_bits(&s->gb, 1))
                    v = -v;
            } else {
                v = 0;
            }
            g->sb_hybrid[s_index++] = v;
            if (y) {
                if (y == 15)
                    y += get_bitsz(&s->gb, linbits);
                v = l3_unscale(y, exponents[s_index]);
                if (get_bits(&s->gb, 1))
                    v = -v;
            } else {
                v = 0;
            }
            g->sb_hybrid[s_index++] = v;
        }
    }
            
    /* high frequencies */
    vlc = &huff_quad_vlc[g->count1table_select];
    last_gb.buffer = NULL;
    while (s_index <= 572) {
        pos = get_bits_count(&s->gb);
        if (pos >= end_pos) {
            if (pos > end_pos && last_gb.buffer != NULL) {
                /* some encoders generate an incorrect size for this
                   part. We must go back into the data */
                s_index -= 4;
                s->gb = last_gb;
            }
            break;
        }
        last_gb= s->gb;

        code = get_vlc(&s->gb, vlc);
/*#ifdef DEBUG
        printf("t=%d code=%d\n", g->count1table_select, code);
#endif*/
        if (code < 0)
            return -1;
        for(i=0;i<4;i++) {
            if (code & (8 >> i)) {
                /* non zero value. Could use a hand coded function for
                   'one' value */
                v = l3_unscale(1, exponents[s_index]);
                if(get_bits(&s->gb, 1))
                    v = -v;
            } else {
                v = 0;
            }
            g->sb_hybrid[s_index++] = v;
        }
    }
    while (s_index < 576)
        g->sb_hybrid[s_index++] = 0;
    return 0;
}

/* Reorder short blocks from bitstream order to interleaved order. It
   would be faster to do it in parsing, but the code would be far more
   complicated */
static void reorder_block(MPADecodeContext *s, GranuleDef *g)
{
    int i, j, k, len;
    int32_t *ptr, *dst, *ptr1;
    int32_t tmp[576];

    if (g->block_type != 2)
        return;

    if (g->switch_point) {
        if (s->sample_rate_index != 8) {
            ptr = g->sb_hybrid + 36;
        } else {
            ptr = g->sb_hybrid + 48;
        }
    } else {
        ptr = g->sb_hybrid;
    }
    
    for(i=g->short_start;i<13;i++) {
        len = band_size_short[s->sample_rate_index][i];
        ptr1 = ptr;
        for(k=0;k<3;k++) {
            dst = tmp + k;
            for(j=len;j>0;j--) {
                *dst = *ptr++;
                dst += 3;
            }
        }
        memcpy(ptr1, tmp, len * 3 * sizeof(int32_t));
    }
}

#define ISQRT2 FIXR(0.70710678118654752440)

static void compute_stereo(MPADecodeContext *s,
                           GranuleDef *g0, GranuleDef *g1)
{
    int i, j, k, l;
    int32_t v1, v2;
    int sf_max, tmp0, tmp1, sf, len, non_zero_found;
    int32_t (*is_tab)[16];
    int32_t *tab0, *tab1;
    int non_zero_found_short[3];

    /* intensity stereo */
    if (s->mode_ext & MODE_EXT_I_STEREO) {
        if (!s->lsf) {
            is_tab = is_table;
            sf_max = 7;
        } else {
            is_tab = is_table_lsf[g1->scalefac_compress & 1];
            sf_max = 16;
        }
            
        tab0 = g0->sb_hybrid + 576;
        tab1 = g1->sb_hybrid + 576;

        non_zero_found_short[0] = 0;
        non_zero_found_short[1] = 0;
        non_zero_found_short[2] = 0;
        k = (13 - g1->short_start) * 3 + g1->long_end - 3;
        for(i = 12;i >= g1->short_start;i--) {
            /* for last band, use previous scale factor */
            if (i != 11)
                k -= 3;
            len = band_size_short[s->sample_rate_index][i];
            for(l=2;l>=0;l--) {
                tab0 -= len;
                tab1 -= len;
                if (!non_zero_found_short[l]) {
                    /* test if non zero band. if so, stop doing i-stereo */
                    for(j=0;j<len;j++) {
                        if (tab1[j] != 0) {
                            non_zero_found_short[l] = 1;
                            goto found1;
                        }
                    }
                    sf = g1->scale_factors[k + l];
                    if (sf >= sf_max)
                        goto found1;

                    v1 = is_tab[0][sf];
                    v2 = is_tab[1][sf];
                    for(j=0;j<len;j++) {
                        tmp0 = tab0[j];
                        tab0[j] = MULL(tmp0, v1);
                        tab1[j] = MULL(tmp0, v2);
                    }
                } else {
                found1:
                    if (s->mode_ext & MODE_EXT_MS_STEREO) {
                        /* lower part of the spectrum : do ms stereo
                           if enabled */
                        for(j=0;j<len;j++) {
                            tmp0 = tab0[j];
                            tmp1 = tab1[j];
                            tab0[j] = MULL(tmp0 + tmp1, ISQRT2);
                            tab1[j] = MULL(tmp0 - tmp1, ISQRT2);
                        }
                    }
                }
            }
        }

        non_zero_found = non_zero_found_short[0] | 
            non_zero_found_short[1] | 
            non_zero_found_short[2];

        for(i = g1->long_end - 1;i >= 0;i--) {
            len = band_size_long[s->sample_rate_index][i];
            tab0 -= len;
            tab1 -= len;
            /* test if non zero band. if so, stop doing i-stereo */
            if (!non_zero_found) {
                for(j=0;j<len;j++) {
                    if (tab1[j] != 0) {
                        non_zero_found = 1;
                        goto found2;
                    }
                }
                /* for last band, use previous scale factor */
                k = (i == 21) ? 20 : i;
                sf = g1->scale_factors[k];
                if (sf >= sf_max)
                    goto found2;
                v1 = is_tab[0][sf];
                v2 = is_tab[1][sf];
                for(j=0;j<len;j++) {
                    tmp0 = tab0[j];
                    tab0[j] = MULL(tmp0, v1);
                    tab1[j] = MULL(tmp0, v2);
                }
            } else {
            found2:
                if (s->mode_ext & MODE_EXT_MS_STEREO) {
                    /* lower part of the spectrum : do ms stereo
                       if enabled */
                    for(j=0;j<len;j++) {
                        tmp0 = tab0[j];
                        tmp1 = tab1[j];
                        tab0[j] = MULL(tmp0 + tmp1, ISQRT2);
                        tab1[j] = MULL(tmp0 - tmp1, ISQRT2);
                    }
                }
            }
        }
    } else if (s->mode_ext & MODE_EXT_MS_STEREO) {
        /* ms stereo ONLY */
        /* NOTE: the 1/sqrt(2) normalization factor is included in the
           global gain */
        tab0 = g0->sb_hybrid;
        tab1 = g1->sb_hybrid;
        for(i=0;i<576;i++) {
            tmp0 = tab0[i];
            tmp1 = tab1[i];
            tab0[i] = tmp0 + tmp1;
            tab1[i] = tmp0 - tmp1;
        }
    }
}

static void compute_antialias(MPADecodeContext *s,
                              GranuleDef *g)
{
    int32_t *ptr, *p0, *p1, *csa;
    int n, tmp0, tmp1, i, j;

    /* we antialias only "long" bands */
    if (g->block_type == 2) {
        if (!g->switch_point)
            return;
        /* XXX: check this for 8000Hz case */
        n = 1;
    } else {
        n = SBLIMIT - 1;
    }
    
    ptr = g->sb_hybrid + 18;
    for(i = n;i > 0;i--) {
        p0 = ptr - 1;
        p1 = ptr;
        csa = &csa_table[0][0];
        for(j=0;j<8;j++) {
            tmp0 = *p0;
            tmp1 = *p1;
            *p0 = FRAC_RND(MUL64(tmp0, csa[0]) - MUL64(tmp1, csa[1]));
            *p1 = FRAC_RND(MUL64(tmp0, csa[1]) + MUL64(tmp1, csa[0]));
            p0--;
            p1++;
            csa += 2;
        }
        ptr += 18;
    }
}

static void compute_imdct(MPADecodeContext *s,
                          GranuleDef *g, 
                          int32_t *sb_samples,
                          int32_t *mdct_buf)
{
    int32_t *ptr, *win, *win1, *buf, *buf2, *out_ptr, *ptr1;
    int32_t in[6];
    int32_t out[36];
    int32_t out2[12];
    int i, j, k, mdct_long_end, v, sblimit;

    /* find last non zero block */
    ptr = g->sb_hybrid + 576;
    ptr1 = g->sb_hybrid + 2 * 18;
    while (ptr >= ptr1) {
        ptr -= 6;
        v = ptr[0] | ptr[1] | ptr[2] | ptr[3] | ptr[4] | ptr[5];
        if (v != 0)
            break;
    }
    sblimit = ((ptr - g->sb_hybrid) / 18) + 1;

    if (g->block_type == 2) {
        /* XXX: check for 8000 Hz */
        if (g->switch_point)
            mdct_long_end = 2;
        else
            mdct_long_end = 0;
    } else {
        mdct_long_end = sblimit;
    }

    buf = mdct_buf;
    ptr = g->sb_hybrid;
    for(j=0;j<mdct_long_end;j++) {
        imdct36(out, ptr);
        /* apply window & overlap with previous buffer */
        out_ptr = sb_samples + j;
        /* select window */
        if (g->switch_point && j < 2)
            win1 = mdct_win[0];
        else
            win1 = mdct_win[g->block_type];
        /* select frequency inversion */
        win = win1 + ((4 * 36) & -(j & 1));
        for(i=0;i<18;i++) {
            *out_ptr = MULL(out[i], win[i]) + buf[i];
            buf[i] = MULL(out[i + 18], win[i + 18]);
            out_ptr += SBLIMIT;
        }
        ptr += 18;
        buf += 18;
    }
    for(j=mdct_long_end;j<sblimit;j++) {
        for(i=0;i<6;i++) {
            out[i] = 0;
            out[6 + i] = 0;
            out[30+i] = 0;
        }
        /* select frequency inversion */
        win = mdct_win[2] + ((4 * 36) & -(j & 1));
        buf2 = out + 6;
        for(k=0;k<3;k++) {
            /* reorder input for short mdct */
            ptr1 = ptr + k;
            for(i=0;i<6;i++) {
                in[i] = *ptr1;
                ptr1 += 3;
            }
            imdct12(out2, in);
            /* apply 12 point window and do small overlap */
            for(i=0;i<6;i++) {
                buf2[i] = MULL(out2[i], win[i]) + buf2[i];
                buf2[i + 6] = MULL(out2[i + 6], win[i + 6]);
            }
            buf2 += 6;
        }
        /* overlap */
        out_ptr = sb_samples + j;
        for(i=0;i<18;i++) {
            *out_ptr = out[i] + buf[i];
            buf[i] = out[i + 18];
            out_ptr += SBLIMIT;
        }
        ptr += 18;
        buf += 18;
    }
    /* zero bands */
    for(j=sblimit;j<SBLIMIT;j++) {
        /* overlap */
        out_ptr = sb_samples + j;
        for(i=0;i<18;i++) {
            *out_ptr = buf[i];
            buf[i] = 0;
            out_ptr += SBLIMIT;
        }
        buf += 18;
    }
}

/* main layer3 decoding function */
static int mp_decode_layer3(MPADecodeContext *s)
{
    int nb_granules, main_data_begin, private_bits;
    int gr, ch, blocksplit_flag, i, j, k, n, bits_pos, bits_left;
    GranuleDef granules[2][2], *g;
    int16_t exponents[576];

    /* read side info */
    if (s->lsf) {
        main_data_begin = get_bits(&s->gb, 8);
        if (s->nb_channels == 2)
            private_bits = get_bits(&s->gb, 2);
        else
            private_bits = get_bits(&s->gb, 1);
        nb_granules = 1;
    } else {
        main_data_begin = get_bits(&s->gb, 9);
        if (s->nb_channels == 2)
            private_bits = get_bits(&s->gb, 3);
        else
            private_bits = get_bits(&s->gb, 5);
        nb_granules = 2;
        for(ch=0;ch<s->nb_channels;ch++) {
            granules[ch][0].scfsi = 0; /* all scale factors are transmitted */
            granules[ch][1].scfsi = get_bits(&s->gb, 4);
        }
    }
    
    for(gr=0;gr<nb_granules;gr++) {
        for(ch=0;ch<s->nb_channels;ch++) {
/*#ifdef DEBUG
            printf("gr=%d ch=%d: side_info\n", gr, ch);
#endif*/
            g = &granules[ch][gr];
            g->part2_3_length = get_bits(&s->gb, 12);
            g->big_values = get_bits(&s->gb, 9);
            g->global_gain = get_bits(&s->gb, 8);
            /* if MS stereo only is selected, we precompute the
               1/sqrt(2) renormalization factor */
            if ((s->mode_ext & (MODE_EXT_MS_STEREO | MODE_EXT_I_STEREO)) == 
                MODE_EXT_MS_STEREO)
                g->global_gain -= 2;
            if (s->lsf)
                g->scalefac_compress = get_bits(&s->gb, 9);
            else
                g->scalefac_compress = get_bits(&s->gb, 4);
            blocksplit_flag = get_bits(&s->gb, 1);
            if (blocksplit_flag) {
                g->block_type = get_bits(&s->gb, 2);
                if (g->block_type == 0)
                    return -1;
                g->switch_point = get_bits(&s->gb, 1);
                for(i=0;i<2;i++)
                    g->table_select[i] = get_bits(&s->gb, 5);
                for(i=0;i<3;i++) 
                    g->subblock_gain[i] = get_bits(&s->gb, 3);
                /* compute huffman coded region sizes */
                if (g->block_type == 2)
                    g->region_size[0] = (36 / 2);
                else {
                    if (s->sample_rate_index <= 2) 
                        g->region_size[0] = (36 / 2);
                    else if (s->sample_rate_index != 8) 
                        g->region_size[0] = (54 / 2);
                    else
                        g->region_size[0] = (108 / 2);
                }
                g->region_size[1] = (576 / 2);
            } else {
                int region_address1, region_address2, l;
                g->block_type = 0;
                g->switch_point = 0;
                for(i=0;i<3;i++)
                    g->table_select[i] = get_bits(&s->gb, 5);
                /* compute huffman coded region sizes */
                region_address1 = get_bits(&s->gb, 4);
                region_address2 = get_bits(&s->gb, 3);
/*#ifdef DEBUG
                printf("region1=%d region2=%d\n", 
                       region_address1, region_address2);
#endif*/
                g->region_size[0] = 
                    band_index_long[s->sample_rate_index][region_address1 + 1] >> 1;
                l = region_address1 + region_address2 + 2;
                /* should not overflow */
                if (l > 22)
                    l = 22;
                g->region_size[1] = 
                    band_index_long[s->sample_rate_index][l] >> 1;
            }
            /* convert region offsets to region sizes and truncate
               size to big_values */
            g->region_size[2] = (576 / 2);
            j = 0;
            for(i=0;i<3;i++) {
                k = g->region_size[i];
                if (k > g->big_values)
                    k = g->big_values;
                g->region_size[i] = k - j;
                j = k;
            }

            /* compute band indexes */
            if (g->block_type == 2) {
                if (g->switch_point) {
                    /* if switched mode, we handle the 36 first samples as
                       long blocks.  For 8000Hz, we handle the 48 first
                       exponents as long blocks (XXX: check this!) */
                    if (s->sample_rate_index <= 2)
                        g->long_end = 8;
                    else if (s->sample_rate_index != 8)
                        g->long_end = 6;
                    else
                        g->long_end = 4; /* 8000 Hz */
                    
                    if (s->sample_rate_index != 8)
                        g->short_start = 3;
                    else
                        g->short_start = 2; 
                } else {
                    g->long_end = 0;
                    g->short_start = 0;
                }
            } else {
                g->short_start = 13;
                g->long_end = 22;
            }
            
            g->preflag = 0;
            if (!s->lsf)
                g->preflag = get_bits(&s->gb, 1);
            g->scalefac_scale = get_bits(&s->gb, 1);
            g->count1table_select = get_bits(&s->gb, 1);
/*#ifdef DEBUG
            printf("block_type=%d switch_point=%d\n",
                   g->block_type, g->switch_point);
#endif*/
        }
    }

    /* now we get bits from the main_data_begin offset */
/*#ifdef DEBUG
    printf("seekback: %d\n", main_data_begin);
#endif*/
    seek_to_maindata(s, main_data_begin);

    for(gr=0;gr<nb_granules;gr++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            g = &granules[ch][gr];
            
            bits_pos = get_bits_count(&s->gb);
            
            if (!s->lsf) {
                uint8_t *sc;
                int slen, slen1, slen2;

                /* MPEG1 scale factors */
                slen1 = slen_table[0][g->scalefac_compress];
                slen2 = slen_table[1][g->scalefac_compress];
/*#ifdef DEBUG
                printf("slen1=%d slen2=%d\n", slen1, slen2);
#endif*/
                if (g->block_type == 2) {
                    n = g->switch_point ? 17 : 18;
                    j = 0;
                    for(i=0;i<n;i++)
                        g->scale_factors[j++] = get_bitsz(&s->gb, slen1);
                    for(i=0;i<18;i++)
                        g->scale_factors[j++] = get_bitsz(&s->gb, slen2);
                    for(i=0;i<3;i++)
                        g->scale_factors[j++] = 0;
                } else {
                    sc = granules[ch][0].scale_factors;
                    j = 0;
                    for(k=0;k<4;k++) {
                        n = (k == 0 ? 6 : 5);
                        if ((g->scfsi & (0x8 >> k)) == 0) {
                            slen = (k < 2) ? slen1 : slen2;
                            for(i=0;i<n;i++)
                                g->scale_factors[j++] = get_bitsz(&s->gb, slen);
                        } else {
                            /* simply copy from last granule */
                            for(i=0;i<n;i++) {
                                g->scale_factors[j] = sc[j];
                                j++;
                            }
                        }
                    }
                    g->scale_factors[j++] = 0;
                }
/*#if defined(DEBUG)
                {
                    printf("scfsi=%x gr=%d ch=%d scale_factors:\n", 
                           g->scfsi, gr, ch);
                    for(i=0;i<j;i++)
                        printf(" %d", g->scale_factors[i]);
                    printf("\n");
                }
#endif*/
            } else {
                int tindex, tindex2, slen[4], sl, sf;

                /* LSF scale factors */
                if (g->block_type == 2) {
                    tindex = g->switch_point ? 2 : 1;
                } else {
                    tindex = 0;
                }
                sf = g->scalefac_compress;
                if ((s->mode_ext & MODE_EXT_I_STEREO) && ch == 1) {
                    /* intensity stereo case */
                    sf >>= 1;
                    if (sf < 180) {
                        lsf_sf_expand(slen, sf, 6, 6, 0);
                        tindex2 = 3;
                    } else if (sf < 244) {
                        lsf_sf_expand(slen, sf - 180, 4, 4, 0);
                        tindex2 = 4;
                    } else {
                        lsf_sf_expand(slen, sf - 244, 3, 0, 0);
                        tindex2 = 5;
                    }
                } else {
                    /* normal case */
                    if (sf < 400) {
                        lsf_sf_expand(slen, sf, 5, 4, 4);
                        tindex2 = 0;
                    } else if (sf < 500) {
                        lsf_sf_expand(slen, sf - 400, 5, 4, 0);
                        tindex2 = 1;
                    } else {
                        lsf_sf_expand(slen, sf - 500, 3, 0, 0);
                        tindex2 = 2;
                        g->preflag = 1;
                    }
                }

                j = 0;
                for(k=0;k<4;k++) {
                    n = lsf_nsf_table[tindex2][tindex][k];
                    sl = slen[k];
                    for(i=0;i<n;i++)
                        g->scale_factors[j++] = get_bitsz(&s->gb, sl);
                }
                /* XXX: should compute exact size */
                for(;j<40;j++)
                    g->scale_factors[j] = 0;
/*#if defined(DEBUG)
                {
                    printf("gr=%d ch=%d scale_factors:\n", 
                           gr, ch);
                    for(i=0;i<40;i++)
                        printf(" %d", g->scale_factors[i]);
                    printf("\n");
                }
#endif*/
            }

            exponents_from_scale_factors(s, g, exponents);

            /* read Huffman coded residue */
            if (huffman_decode(s, g, exponents,
                               bits_pos + g->part2_3_length) < 0)
                return -1;

            /* skip extension bits */
            bits_left = g->part2_3_length - (get_bits_count(&s->gb) - bits_pos);
            if (bits_left < 0) {
/*#ifdef DEBUG
                printf("bits_left=%d\n", bits_left);
#endif*/
                return -1;
            }
            while (bits_left >= 16) {
                skip_bits(&s->gb, 16);
                bits_left -= 16;
            }
            if (bits_left > 0)
                skip_bits(&s->gb, bits_left);
        } /* ch */

        if (s->nb_channels == 2)
            compute_stereo(s, &granules[0][gr], &granules[1][gr]);

        for(ch=0;ch<s->nb_channels;ch++) {
            g = &granules[ch][gr];

            reorder_block(s, g);
            compute_antialias(s, g);
            compute_imdct(s, g, &s->sb_samples[ch][18 * gr][0], s->mdct_buf[ch]); 
        }
    } /* gr */
    return nb_granules * 18;
}

static int mp_decode_frame(MPADecodeContext *s, 
                           int16_t *samples)
{
    int i, nb_frames, ch;
    int16_t *samples_ptr;

    init_get_bits(&s->gb, s->inbuf + HEADER_SIZE, 
                  (s->inbuf_ptr - s->inbuf - HEADER_SIZE)*8);
    
    /* skip error protection field */
    if (s->error_protection)
        get_bits(&s->gb, 16);

/*#ifdef DEBUG
    printf("frame %d:\n", s->frame_count);
#endif*/
    switch(s->layer) {
    case 1:
        nb_frames = mp_decode_layer1(s);
        break;
    case 2:
        nb_frames = mp_decode_layer2(s);
        break;
    case 3:
    default:
        nb_frames = mp_decode_layer3(s);
        break;
    }
/*#if defined(DEBUG)
    for(i=0;i<nb_frames;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            int j;
            printf("%d-%d:", i, ch);
            for(j=0;j<SBLIMIT;j++)
                printf(" %0.6f", (double)s->sb_samples[ch][i][j] / FRAC_ONE);
            printf("\n");
        }
    }
#endif*/
    /* apply the synthesis filter */
    for(ch=0;ch<s->nb_channels;ch++) {
        samples_ptr = samples + ch;
        for(i=0;i<nb_frames;i++) {
            synth_filter(s, ch, samples_ptr, s->nb_channels,
                         s->sb_samples[ch][i]);
            samples_ptr += 32 * s->nb_channels;
        }
    }
#ifdef DEBUG
    s->frame_count++;        
#endif
    return nb_frames * 32 * sizeof(short) * s->nb_channels;
}

int mpaudec_decode_frame(MPAuDecContext * mpctx,
                         void *data, int *data_size,
                         const uint8_t * buf, int buf_size)
{
    MPADecodeContext *s;
    const uint8_t *buf_ptr = buf;
    int out_size = 0;
    int16_t *out_samples = data;
    assert(mpctx != NULL);
    assert(mpctx->priv_data != NULL);
    s = mpctx->priv_data;

    while (buf_size > 0 && out_size == 0) {
        uint32_t header;
        uint32_t free_format_next_header = 0;
        int len = s->inbuf_ptr - s->inbuf;
        if (s->frame_size == 0) {
            /* no header seen : find one. We need at least HEADER_SIZE
               bytes to parse it */
            len = HEADER_SIZE - len;
            if (len > buf_size)
                len = buf_size;
            if (len > 0) {
                memcpy(s->inbuf_ptr, buf_ptr, len);
                buf_ptr += len;
                buf_size -= len;
                s->inbuf_ptr += len;
            }
            if ((s->inbuf_ptr - s->inbuf) >= HEADER_SIZE) {
                header = (s->inbuf[0] << 24) | (s->inbuf[1] << 16) |
                    (s->inbuf[2] << 8) | s->inbuf[3];

                if (check_header(header) < 0) {
                    /* no sync found : move by one byte (inefficient, but simple!) */
                    memmove(s->inbuf, s->inbuf + 1, s->inbuf_ptr - s->inbuf - 1);
                    s->inbuf_ptr--;
/*#ifdef DEBUG
                    printf("skip %x\n", header);
#endif*/
                    /* reset free format frame size to give a chance
                       to get a new bitrate */
                    s->free_format_frame_size = 0;
                } else {
                    if (decode_header(s, header) == 1) {
                        /* free format: prepare to compute frame size */
                        s->frame_size = -1;
                    }
                    /* update codec info */
                    mpctx->sample_rate = s->sample_rate;
                    mpctx->channels = s->nb_channels;
                    mpctx->bit_rate = s->bit_rate;
                    mpctx->layer = s->layer;
                    switch(s->layer) {
                    case 1:
                        mpctx->frame_size = 384;
                        break;
                    case 2:
                        mpctx->frame_size = 1152;
                        break;
                    case 3:
                        if (s->lsf)
                            mpctx->frame_size = 576;
                        else
                            mpctx->frame_size = 1152;
                        break;
                    }
                }
            }
        } else if (s->frame_size == -1) {
            /* free format : find next sync to compute frame size */
            len = MPA_MAX_CODED_FRAME_SIZE - len;
            if (len > buf_size)
                len = buf_size;
            if (len == 0) {
                /* frame too long: resync */
                s->frame_size = 0;
                memmove(s->inbuf, s->inbuf + 1, s->inbuf_ptr - s->inbuf - 1);
                s->inbuf_ptr--;
            } else {
                uint8_t *p, *pend;
                uint32_t header1;
                int padding;

                memcpy(s->inbuf_ptr, buf_ptr, len);
                /* check for header */
                p = s->inbuf_ptr - 3;
                pend = s->inbuf_ptr + len - 4;
                while (p <= pend && free_format_next_header == 0) {
                    header = (p[0] << 24) | (p[1] << 16) |
                        (p[2] << 8) | p[3];
                    header1 = (s->inbuf[0] << 24) | (s->inbuf[1] << 16) |
                        (s->inbuf[2] << 8) | s->inbuf[3];
                    /* check with high probability that we have a
                       valid header */
                    if ((header & SAME_HEADER_MASK) ==
                        (header1 & SAME_HEADER_MASK)) {
                        /* header found: update pointers */
                        len = (p + 4) - s->inbuf_ptr;
                        buf_ptr += len;
                        buf_size -= len;
                        s->inbuf_ptr = p;
                        free_format_next_header = header;
                        /* compute frame size */
                        s->free_format_frame_size = s->inbuf_ptr - s->inbuf;
                        padding = (header1 >> 9) & 1;
                        if (s->layer == 1)
                            s->free_format_frame_size -= padding * 4;
                        else
                            s->free_format_frame_size -= padding;
/*#ifdef DEBUG
                        printf("free frame size=%d padding=%d\n", 
                               s->free_format_frame_size, padding);
#endif*/
                        decode_header(s, header1);
                    } else
                        p++;
                }
                if (free_format_next_header == 0) {
                    /* not found: simply increase pointers */
                    buf_ptr += len;
                    s->inbuf_ptr += len;
                    buf_size -= len;
                }
            }
        } else if (len < s->frame_size) {
            if (s->frame_size > MPA_MAX_CODED_FRAME_SIZE)
                s->frame_size = MPA_MAX_CODED_FRAME_SIZE;
            len = s->frame_size - len;
            if (len > buf_size)
                len = buf_size;
            memcpy(s->inbuf_ptr, buf_ptr, len);
            buf_ptr += len;
            s->inbuf_ptr += len;
            buf_size -= len;
        }
        if (s->frame_size > 0 && 
            (s->inbuf_ptr - s->inbuf) >= s->frame_size) {
            mpctx->coded_frame_size = s->frame_size;
            if (mpctx->parse_only) {
                /* simply return the frame data */
                *(uint8_t **)data = s->inbuf;
                out_size = s->inbuf_ptr - s->inbuf;
            } else {
                out_size = mp_decode_frame(s, out_samples);
            }
            if (free_format_next_header != 0) {
                s->inbuf[0] = free_format_next_header >> 24;
                s->inbuf[1] = free_format_next_header >> 16;
                s->inbuf[2] = free_format_next_header >> 8;
                s->inbuf[3] = free_format_next_header;
                s->inbuf_ptr = s->inbuf + 4;
            } else
                s->inbuf_ptr = s->inbuf;
            s->frame_size = 0;
        }
    }
    *data_size = out_size;
    return buf_ptr - buf;
}

void mpaudec_clear(MPAuDecContext *mpctx)
{
    assert(mpctx != NULL);
    free(mpctx->priv_data);
    memset(mpctx, 0, sizeof(MPAuDecContext));
}

void mpaudec_cleanup()
{
	int i = 0; 
	for (i = 1; i<16; i++) {
		free(huff_code_table[i]);
	}
	for (i = 1; i<16; i++) {
		free_vlc(&huff_vlc[i]);
	}
	for (i = 0; i<2; i++) {
		free_vlc(&huff_quad_vlc[i]);
	}
}
#endif