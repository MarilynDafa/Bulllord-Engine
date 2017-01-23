
#ifndef LIBMPEG2_VIDEO_OUT_H
#define LIBMPEG2_VIDEO_OUT_H
#include "mpeg2.h"
#include "a52.h"
#include "audio_out.h"
#ifdef __cplusplus
extern "C" {
#endif
struct mpeg2_sequence_s;
struct mpeg2_convert_init_s;
typedef struct {
    int (* convert) (int stage, void * id,
		     const struct mpeg2_sequence_s * sequence,
		     int stride, uint32_t accel, void * arg,
		     struct mpeg2_convert_init_s * result);
} vo_setup_result_t;

typedef struct vo_instance_s vo_instance_t;
struct vo_instance_s {
    int (* setup) (vo_instance_t * instance, unsigned int width,
		   unsigned int height, unsigned int chroma_width,
		   unsigned int chroma_height, vo_setup_result_t * result);
    void (* setup_fbuf) (vo_instance_t * instance, uint8_t ** buf, void ** id);
    void (* set_fbuf) (vo_instance_t * instance, uint8_t ** buf, void ** id);
    void (* start_fbuf) (vo_instance_t * instance,
			 uint8_t * const * buf, void * id);
    void (* draw) (vo_instance_t * instance, uint8_t * const * buf, void * id);
    void (* discard) (vo_instance_t * instance,
		      uint8_t * const * buf, void * id);
    void (* close) (vo_instance_t * instance);
};

typedef vo_instance_t * vo_open_t (void);

typedef struct {
    const char * name;
    vo_open_t * open;
} vo_driver_t;

void vo_accel (uint32_t accel);


//int demux_v(unsigned char * buf, unsigned char * end, int flags, mpeg2dec_t** mpeg2dec, mpeg2_info_t** info, vo_instance_t * output);
//int demux_a(uint8_t * buf, uint8_t * end, int flags, a52_state_t** mepg2dec, ao_instance_t* output);
    
int demux(unsigned char * buf, unsigned char * end, int flags, mpeg2dec_t** mpeg2dec, mpeg2_info_t** info, vo_instance_t * output,  a52_state_t** a52dec, ao_instance_t* aoutput);

/* return NULL terminated array of all drivers */
vo_driver_t const * vo_drivers (void);

#ifdef __cplusplus
}
#endif
#endif /* LIBMPEG2_VIDEO_OUT_H */
