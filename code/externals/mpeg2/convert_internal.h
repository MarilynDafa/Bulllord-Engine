typedef struct {
    uint8_t * rgb_ptr;
    int width;
    int field;
    int y_stride, rgb_stride, y_increm, uv_increm, rgb_increm, rgb_slice;
    int chroma420, convert420;
    int dither_offset, dither_stride;
    int y_stride_frame, uv_stride_frame, rgb_stride_frame, rgb_stride_min;
} convert_rgb_t;

typedef void mpeg2convert_copy_t (void * id, uint8_t * const * src,
				  unsigned int v_offset);

mpeg2convert_copy_t * mpeg2convert_rgb_mmxext (int bpp, int mode,
					       const mpeg2_sequence_t * seq);
mpeg2convert_copy_t * mpeg2convert_rgb_mmx (int bpp, int mode,
					    const mpeg2_sequence_t * seq);
mpeg2convert_copy_t * mpeg2convert_rgb_vis (int bpp, int mode,
					    const mpeg2_sequence_t * seq);
