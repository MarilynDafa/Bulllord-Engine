
#ifndef LIBMPEG2_MPEG2CONVERT_H
#define LIBMPEG2_MPEG2CONVERT_H

mpeg2_convert_t mpeg2convert_rgb32;
mpeg2_convert_t mpeg2convert_rgb24;
mpeg2_convert_t mpeg2convert_rgb16;
mpeg2_convert_t mpeg2convert_rgb15;
mpeg2_convert_t mpeg2convert_rgb8;
mpeg2_convert_t mpeg2convert_bgr32;
mpeg2_convert_t mpeg2convert_bgr24;
mpeg2_convert_t mpeg2convert_bgr16;
mpeg2_convert_t mpeg2convert_bgr15;
mpeg2_convert_t mpeg2convert_bgr8;

typedef enum {
    MPEG2CONVERT_RGB = 0,
    MPEG2CONVERT_BGR = 1
} mpeg2convert_rgb_order_t;

mpeg2_convert_t * mpeg2convert_rgb (mpeg2convert_rgb_order_t order,
				    unsigned int bpp);

mpeg2_convert_t mpeg2convert_uyvy;

#endif /* LIBMPEG2_MPEG2CONVERT_H */
