/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED
#include "Prerequisites.h"
namespace o2d{
	O2D_API u32 utf8_len(const utf8* u8str_);
    O2D_API u32 utf16_len(const utf16* u16str_);
	O2D_API utf16* utf16_tok(utf16* str_, const utf16* control_);
	O2D_API s32 utf16_cmp(const utf16* src_, const utf16* dst_);
	O2D_API const utf16* get_path(const utf16* filename_);
	O2D_API const utf16* get_ext(const utf16* filename_);
	O2D_API void to_lower(ansi* out_, const ansi* in_);
	O2D_API void to_lower(utf16* out_, const utf16* in_);
	O2D_API void to_upper(ansi* out_, const ansi* in_);
	O2D_API void to_upper(utf16* out_, const utf16* in_);
	O2D_API u32 hash(ansi* str_);
	O2D_API u32 hash(utf16* str_);
	O2D_API u32 hash(const ansi* str_);
	O2D_API u32 hash(const utf16* str_);
}
#endif