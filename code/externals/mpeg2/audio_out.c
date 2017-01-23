#include "mpgconfig.h"

#include <stdlib.h>
#include <inttypes.h>

#include "a52.h"
#include "audio_out.h"
extern ao_open_t ao_oss_open;
extern ao_open_t ao_ossdolby_open;
extern ao_open_t ao_oss4_open;
extern ao_open_t ao_oss6_open;
extern ao_open_t ao_solaris_open;
extern ao_open_t ao_solarisdolby_open;
extern ao_open_t ao_al_open;
extern ao_open_t ao_aldolby_open;
extern ao_open_t ao_al4_open;
extern ao_open_t ao_al6_open;
extern ao_open_t ao_win_open;
extern ao_open_t ao_windolby_open;
extern ao_open_t ao_wav_open;
extern ao_open_t ao_wavdolby_open;
extern ao_open_t ao_wav6_open;
extern ao_open_t ao_aif_open;
extern ao_open_t ao_aifdolby_open;
extern ao_open_t ao_peak_open;
extern ao_open_t ao_peakdolby_open;
extern ao_open_t ao_null_open;
extern ao_open_t ao_null4_open;
extern ao_open_t ao_null6_open;
extern ao_open_t ao_float_open;

static ao_driver_t audio_out_drivers[] = {
#ifdef LIBAO_OSS
    {"oss", ao_oss_open},
    {"ossdolby", ao_ossdolby_open},
    {"oss4", ao_oss4_open},
    {"oss6", ao_oss6_open},
#endif
#ifdef LIBAO_SOLARIS
    {"solaris", ao_solaris_open},
    {"solarisdolby", ao_solarisdolby_open},
#endif
#ifdef LIBAO_AL
    {"al", ao_al_open},
    {"aldolby", ao_aldolby_open},
    {"al4", ao_al4_open},
    {"al6", ao_al6_open},
#endif
#ifdef LIBAO_WIN
    {"win", ao_win_open},
    {"windolby", ao_windolby_open},
#endif
    //{"wav", ao_wav_open},
    //{"wavdolby", ao_wavdolby_open},
    //{"wav6", ao_wav6_open},
    //{"aif", ao_aif_open},
    //{"aifdolby", ao_aifdolby_open},
    ///{"peak", ao_peak_open},
    //{"peakdolby", ao_peakdolby_open},
    {"null", ao_null_open},
    {"null4", ao_null4_open},
    {"null6", ao_null6_open},
   // {"float", ao_float_open},
    {NULL, NULL}
};

ao_driver_t * ao_drivers (void)
{
    return audio_out_drivers;
}
