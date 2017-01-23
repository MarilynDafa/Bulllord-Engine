
#ifndef LIBMPEG2_MPEG2_H
#define LIBMPEG2_MPEG2_H

#define MPEG2_VERSION(a,b,c) (((a)<<16)|((b)<<8)|(c))
#define MPEG2_RELEASE MPEG2_VERSION (0, 5, 1)	/* 0.5.1 */

#define SEQ_FLAG_MPEG2 1
#define SEQ_FLAG_CONSTRAINED_PARAMETERS 2
#define SEQ_FLAG_PROGRESSIVE_SEQUENCE 4
#define SEQ_FLAG_LOW_DELAY 8
#define SEQ_FLAG_COLOUR_DESCRIPTION 16

#define SEQ_MASK_VIDEO_FORMAT 0xe0
#define SEQ_VIDEO_FORMAT_COMPONENT 0
#define SEQ_VIDEO_FORMAT_PAL 0x20
#define SEQ_VIDEO_FORMAT_NTSC 0x40
#define SEQ_VIDEO_FORMAT_SECAM 0x60
#define SEQ_VIDEO_FORMAT_MAC 0x80
#define SEQ_VIDEO_FORMAT_UNSPECIFIED 0xa0
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mpeg2_sequence_s {
    unsigned int width, height;
    unsigned int chroma_width, chroma_height;
    unsigned int byte_rate;
    unsigned int vbv_buffer_size;
	unsigned int flags;

    unsigned int picture_width, picture_height;
    unsigned int display_width, display_height;
    unsigned int pixel_width, pixel_height;
    unsigned int frame_period;

    unsigned char profile_level_id;
	unsigned char colour_primaries;
	unsigned char transfer_characteristics;
	unsigned char matrix_coefficients;
} mpeg2_sequence_t;

#define GOP_FLAG_DROP_FRAME 1
#define GOP_FLAG_BROKEN_LINK 2
#define GOP_FLAG_CLOSED_GOP 4

typedef struct mpeg2_gop_s {
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
	unsigned char pictures;
	unsigned int flags;
} mpeg2_gop_t;

#define PIC_MASK_CODING_TYPE 7
#define PIC_FLAG_CODING_TYPE_I 1
#define PIC_FLAG_CODING_TYPE_P 2
#define PIC_FLAG_CODING_TYPE_B 3
#define PIC_FLAG_CODING_TYPE_D 4

#define PIC_FLAG_TOP_FIELD_FIRST 8
#define PIC_FLAG_PROGRESSIVE_FRAME 16
#define PIC_FLAG_COMPOSITE_DISPLAY 32
#define PIC_FLAG_SKIP 64
#define PIC_FLAG_TAGS 128
#define PIC_FLAG_REPEAT_FIRST_FIELD 256
#define PIC_MASK_COMPOSITE_DISPLAY 0xfffff000

typedef struct mpeg2_picture_s {
    unsigned int temporal_reference;
    unsigned int nb_fields;
	unsigned int tag, tag2;
	unsigned int flags;
    struct {
	int x, y;
    } display_offset[3];
} mpeg2_picture_t;

typedef struct mpeg2_fbuf_s {
	unsigned char * buf[3];
    void * id;
} mpeg2_fbuf_t;

typedef struct mpeg2_info_s {
    const mpeg2_sequence_t * sequence;
    const mpeg2_gop_t * gop;
    const mpeg2_picture_t * current_picture;
    const mpeg2_picture_t * current_picture_2nd;
    const mpeg2_fbuf_t * current_fbuf;
    const mpeg2_picture_t * display_picture;
    const mpeg2_picture_t * display_picture_2nd;
    const mpeg2_fbuf_t * display_fbuf;
    const mpeg2_fbuf_t * discard_fbuf;
    const unsigned char * user_data;
    unsigned int user_data_len;
} mpeg2_info_t;

typedef struct mpeg2dec_s mpeg2dec_t;
typedef struct mpeg2_decoder_s mpeg2_decoder_t;

typedef enum {
    STATE_BUFFER = 0,
    STATE_SEQUENCE = 1,
    STATE_SEQUENCE_REPEATED = 2,
    STATE_GOP = 3,
    STATE_PICTURE = 4,
    STATE_SLICE_1ST = 5,
    STATE_PICTURE_2ND = 6,
    STATE_SLICE = 7,
    STATE_END = 8,
    STATE_INVALID = 9,
    STATE_INVALID_END = 10,
    STATE_SEQUENCE_MODIFIED = 11
} mpeg2_state_t;

typedef struct mpeg2_convert_init_s {
    unsigned int id_size;
    unsigned int buf_size[3];
    void (* start) (void * id, const mpeg2_fbuf_t * fbuf,
		    const mpeg2_picture_t * picture, const mpeg2_gop_t * gop);
    void (* copy) (void * id, unsigned char * const * src, unsigned int v_offset);
} mpeg2_convert_init_t;
typedef enum {
    MPEG2_CONVERT_SET = 0,
    MPEG2_CONVERT_STRIDE = 1,
    MPEG2_CONVERT_START = 2
} mpeg2_convert_stage_t;
typedef int mpeg2_convert_t (int stage, void * id,
			     const mpeg2_sequence_t * sequence, int stride,
	unsigned int accel, void * arg,
			     mpeg2_convert_init_t * result);
int mpeg2_convert (mpeg2dec_t * mpeg2dec, mpeg2_convert_t convert, void * arg);
int mpeg2_stride (mpeg2dec_t * mpeg2dec, int stride);
void mpeg2_set_buf (mpeg2dec_t * mpeg2dec, unsigned char * buf[3], void * id);
void mpeg2_custom_fbuf (mpeg2dec_t * mpeg2dec, int custom_fbuf);

#define MPEG2_ACCEL_X86_MMX 1
#define MPEG2_ACCEL_X86_3DNOW 2
#define MPEG2_ACCEL_X86_MMXEXT 4
#define MPEG2_ACCEL_X86_SSE2 8
#define MPEG2_ACCEL_X86_SSE3 16
#define MPEG2_ACCEL_PPC_ALTIVEC 1
#define MPEG2_ACCEL_ALPHA 1
#define MPEG2_ACCEL_ALPHA_MVI 2
#define MPEG2_ACCEL_SPARC_VIS 1
#define MPEG2_ACCEL_SPARC_VIS2 2
#define MPEG2_ACCEL_ARM 1
#define MPEG2_ACCEL_DETECT 0x80000000

unsigned int mpeg2_accel (unsigned int accel);
mpeg2dec_t * mpeg2_init (void);
const mpeg2_info_t * mpeg2_info (mpeg2dec_t * mpeg2dec);
void mpeg2_close (mpeg2dec_t * mpeg2dec);

void mpeg2_buffer (mpeg2dec_t * mpeg2dec, unsigned char * start, unsigned char * end);
int mpeg2_getpos (mpeg2dec_t * mpeg2dec);
mpeg2_state_t mpeg2_parse (mpeg2dec_t * mpeg2dec);

void mpeg2_reset (mpeg2dec_t * mpeg2dec, int full_reset);
void mpeg2_skip (mpeg2dec_t * mpeg2dec, int skip);
void mpeg2_slice_region (mpeg2dec_t * mpeg2dec, int start, int end);

void mpeg2_tag_picture (mpeg2dec_t * mpeg2dec, unsigned int tag, unsigned int tag2);

void mpeg2_init_fbuf (mpeg2_decoder_t * decoder, unsigned char * current_fbuf[3],
	unsigned char * forward_fbuf[3], unsigned char * backward_fbuf[3]);
void mpeg2_slice (mpeg2_decoder_t * decoder, int code, const unsigned char * buffer);
int mpeg2_guess_aspect (const mpeg2_sequence_t * sequence,
			unsigned int * pixel_width,
			unsigned int * pixel_height);

typedef enum {
    MPEG2_ALLOC_MPEG2DEC = 0,
    MPEG2_ALLOC_CHUNK = 1,
    MPEG2_ALLOC_YUV = 2,
    MPEG2_ALLOC_CONVERT_ID = 3,
    MPEG2_ALLOC_CONVERTED = 4
} mpeg2_alloc_t;

void * mpeg2_malloc (unsigned size, mpeg2_alloc_t reason);
void mpeg2_free (void * buf);
void mpeg2_malloc_hooks (void * _malloc (unsigned int, mpeg2_alloc_t),
			 int _free (void *));

void * _Malloc_hook(unsigned size, mpeg2_alloc_t reason);

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#ifdef __cplusplus
}
#endif
#endif /* LIBMPEG2_MPEG2_H */
