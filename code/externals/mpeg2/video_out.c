#include "mpgconfig.h"

#include <stdlib.h>
#include <inttypes.h>

#include "video_out.h"
#include "vo_internal.h"

/* Externally visible list of all vo drivers */

static vo_driver_t video_out_drivers[] = {
#ifdef LIBVO_XV
    {"xv", vo_xv_open},
    {"xv2", vo_xv2_open},
#endif
#ifdef LIBVO_X11
    {"x11", vo_x11_open},
#endif
#ifdef LIBVO_DX
    {"dxrgb", vo_dxrgb_open},
    {"dx", vo_dx_open},
#endif
#ifdef LIBVO_SDL
    {"sdl", vo_sdl_open},
#endif
    {"null", vo_null_open},
    {"nullslice", vo_nullslice_open},
    {"nullskip", vo_nullskip_open},
    {"nullrgb16", vo_nullrgb16_open},
    {"nullrgb32", vo_nullrgb32_open},
   // {"pgm", vo_pgm_open},
   // {"pgmpipe", vo_pgmpipe_open},
  //  {"md5", vo_md5_open},
    {NULL, NULL}
};

vo_driver_t const * vo_drivers (void)
{
    return video_out_drivers;
}
