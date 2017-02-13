/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Tools.h"
namespace o2d{
	namespace internal{
        utf16 STRING_POOL[FILENAME_MAX_LENGTH] = {0};
	}
	u32 utf8_len(const utf8* u8str_)
	{
		u32 cnt = 0;
		while(*u8str_++)
			cnt++;
		return cnt;
	}
    //--------------------------------------------------------
    u32 utf16_len(const utf16* u16str_)
    {
        u32 cnt = 0;
		while(*u16str_++)
			cnt++;
		return cnt;
    }
    //--------------------------------------------------------
	utf16* utf16_tok(utf16* str_, const utf16* control_)
	{
		static utf16* tmp;
		utf16* token;
		const utf16 *ctl;
        if(!str_)
            str_ = tmp;
		while(*str_) 
		{
			for(ctl=control_; *ctl && *ctl != *str_; ctl++)
				;
			if(!*ctl)
				break;
			str_++;
		}
		token = str_;
		for(; *str_ ; str_++)
		{
			for(ctl=control_; *ctl && *ctl != *str_; ctl++)
				;
			if(*ctl) 
			{
				*str_++ = '\0';
				break;
			}
		}
        tmp = str_;
		if(token == str_)
			return nullptr;
		else
			return token;
	}
    //--------------------------------------------------------
	s32 utf16_cmp(const utf16* src_, const utf16* dst_)
    {
        s32 ret = 0;
        while(!(ret=(s32)(*src_ - *dst_)) && *dst_)
            ++src_, ++dst_;
        if(ret<0)
            ret = -1;
        else if(ret > 0)
            ret = 1;
        return ret;
    }
	//--------------------------------------------------------
	const utf16* get_path(const utf16* filename_)
	{
		u32 len = utf16_len(filename_);
		if(!filename_ || !len) 
			return nullptr;
		memset((void*)internal::STRING_POOL , 0 , FILENAME_MAX_LENGTH*sizeof(utf16));
		bool slashfound = false;
		utf16* path = (utf16*)filename_;
		path += len;
		s32 i = 0;
		for(i = len; i >= 0; i--)
		{
			if(*path == L'/') 
			{
				slashfound = true;
				break;
			}
			path--;
		}
		if(!slashfound)
			return nullptr;
		memcpy((void*)internal::STRING_POOL , filename_ , i*sizeof(utf16));
		return internal::STRING_POOL;
	}
	//--------------------------------------------------------
	const utf16* get_ext(const utf16* filename_)
	{
		bool periodfound = false;
		utf16* ext = (utf16*)filename_;
		u32 len = utf16_len(filename_);
		if(!filename_ || !len) 
			return nullptr;
		ext += len;
		for(s32 i = len; i >= 0; i--)
		{
			if(*ext == L'.') 
			{
				periodfound = true;
				break;
			}
			ext--;
		}
		if(!periodfound)
			return nullptr;
		return ext+1;
	}
	//--------------------------------------------------------
	void to_lower(ansi* out_, const ansi* in_)
	{
		for(u32 i = 0 ;i <strlen(in_) ; ++i)
			out_[i] = tolower(in_[i]);
	}
	//--------------------------------------------------------
	void to_lower(utf16* out_, const utf16* in_)
	{
		for(u32 i = 0 ;i <utf16_len(in_) ; ++i)
			out_[i] = towlower(in_[i]);
	}
	//--------------------------------------------------------
	void to_upper(ansi* out_, const ansi* in_)
	{
		for(u32 i = 0 ;i <strlen(in_) ; ++i)
			out_[i] = toupper(in_[i]);
	}
	//--------------------------------------------------------
	void to_upper(utf16* out_, const utf16* in_)
	{
		for(u32 i = 0 ;i <utf16_len(in_) ; ++i)
			out_[i] = towupper(in_[i]);
	}	
	//--------------------------------------------------------
	u32 hash(ansi* str_)
	{
		s8* data = (s8*)str_;
		u32 len = (u32)strlen(str_);
		u32 h = len, tmp;
		s32 rem;
		if(len <= 0 || data == nullptr) 
			return 0;
		rem = len & 3;
		len >>= 2;
		for(;len > 0; len--)
		{
			h  += *((const u16*)data);
			tmp    = (*((const u16*)(data+2)) << 11) ^ h;
			h   = (h << 16) ^ tmp;
			data  += 2*sizeof (u16);
			h  += h >> 11;
		}
		switch(rem) 
		{
		case 3:
			h += *((const u16*)data);
			h ^= h << 16;
			h ^= ((s8)data[sizeof (u16)]) << 18;
			h += h >> 11;
			break;
		case 2:
			h += *((const u16*)data);
			h ^= h << 11;
			h += h >> 17;
			break;
		case 1:
			h += (s8)*data;
			h ^= h << 10;
			h += h >> 1;
		}
		h ^= h << 3;
		h += h >> 5;
		h ^= h << 4;
		h += h >> 17;
		h ^= h << 25;
		h += h >> 6;
		return h;
	}
	//--------------------------------------------------------
	u32 hash(utf16* str_)
	{
		s8* data = (s8*)str_;
		u32 len = 2*utf16_len(str_);
		u32 h = len, tmp;
		s32 rem;
		if(len <= 0 || data == nullptr) 
			return 0;
		rem = len & 3;
		len >>= 2;
		for(;len > 0; len--)
		{
			h  += *((const u16*)data);
			tmp    = (*((const u16*)(data+2)) << 11) ^ h;
			h   = (h << 16) ^ tmp;
			data  += 2*sizeof (u16);
			h  += h >> 11;
		}
		switch(rem) 
		{
		case 3:
			h += *((const u16*)data);
			h ^= h << 16;
			h ^= ((s8)data[sizeof (u16)]) << 18;
			h += h >> 11;
			break;
		case 2:
			h += *((const u16*)data);
			h ^= h << 11;
			h += h >> 17;
			break;
		case 1:
			h += (s8)*data;
			h ^= h << 10;
			h += h >> 1;
		}
		h ^= h << 3;
		h += h >> 5;
		h ^= h << 4;
		h += h >> 17;
		h ^= h << 25;
		h += h >> 6;
		return h;
	}
	//--------------------------------------------------------
	u32 hash(const ansi* str_)
	{
		s8* data = (s8*)str_;
		u32 len = (u32)strlen(str_);
		u32 h = len, tmp;
		s32 rem;
		if(len <= 0 || data == nullptr) 
			return 0;
		rem = len & 3;
		len >>= 2;
		for(;len > 0; len--)
		{
			h  += *((const u16*)data);
			tmp    = (*((const u16*)(data+2)) << 11) ^ h;
			h   = (h << 16) ^ tmp;
			data  += 2*sizeof (u16);
			h  += h >> 11;
		}
		switch(rem) 
		{
		case 3:
			h += *((const u16*)data);
			h ^= h << 16;
			h ^= ((s8)data[sizeof (u16)]) << 18;
			h += h >> 11;
			break;
		case 2:
			h += *((const u16*)data);
			h ^= h << 11;
			h += h >> 17;
			break;
		case 1:
			h += (s8)*data;
			h ^= h << 10;
			h += h >> 1;
		}
		h ^= h << 3;
		h += h >> 5;
		h ^= h << 4;
		h += h >> 17;
		h ^= h << 25;
		h += h >> 6;
		return h;
	}
	//--------------------------------------------------------
	u32 hash(const utf16* str_)
	{
		s8* data = (s8*)str_;
		u32 len = 2*utf16_len(str_);
		u32 h = len, tmp;
		s32 rem;
		if(len <= 0 || data == nullptr) 
			return 0;
		rem = len & 3;
		len >>= 2;
		for(;len > 0; len--)
		{
			h += *((const u16*)data);
			tmp = (*((const u16*)(data+2)) << 11) ^ h;
			h = (h << 16) ^ tmp;
			data  += 2*sizeof (u16);
			h += h >> 11;
		}
		switch(rem) 
		{
		case 3:
			h += *((const u16*)data);
			h ^= h << 16;
			h ^= ((s8)data[sizeof (u16)]) << 18;
			h += h >> 11;
			break;
		case 2:
			h += *((const u16*)data);
			h ^= h << 11;
			h += h >> 17;
			break;
		case 1:
			h += (s8)*data;
			h ^= h << 10;
			h += h >> 1;
		}
		h ^= h << 3;
		h += h >> 5;
		h ^= h << 4;
		h += h >> 17;
		h ^= h << 25;
		h += h >> 6;
		return h;
	}
}