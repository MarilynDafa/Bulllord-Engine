#pragma once
#if defined(_M_ARM)
#include "vpx_scale_rtcd_arm.h"
#else
#include "vpx_scale_rtcd_x86.h"
#endif
