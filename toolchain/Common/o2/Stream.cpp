/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Stream.h"
#include "Tools.h"
#include "Macro.h"
#include "../zlib/zlib.h"
namespace o2d{
	namespace internal{
		u8 alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		ansi inalphabet[256];
		ansi decoder[256];
	}
	c_stream::c_stream()
		:m_data(nullptr) , 
		m_pos(nullptr) , 
		m_end(nullptr) , 
		m_base64_dummy(nullptr) , 
		m_size(0) , 
		m_external(false)
	{}
	//--------------------------------------------------------
	c_stream::c_stream(u32 size_)
		:m_data(nullptr) , 
		m_pos(nullptr) , 
		m_end(nullptr) , 
		m_base64_dummy(nullptr) , 
		m_size(size_) , 
		m_external(false)
	{
		m_data = (u8*)malloc(m_size*sizeof(u8));
		m_pos = m_data;
		m_end = m_data + m_size;
		assert(m_end>=m_pos);
	}
	//--------------------------------------------------------
	c_stream::c_stream(u8* data_, u32 size_)
		:m_data(data_) , 
		m_pos(data_) , 
		m_end(m_data + m_size) , 
		m_base64_dummy(nullptr) , 
		m_size(size_) , 
		m_external(true)
	{
		assert(m_end>=m_pos);
	}
	//--------------------------------------------------------
	c_stream::c_stream(const c_stream& stream_)
		:m_data(nullptr) , 
		m_pos(nullptr) , 
		m_end(m_data + m_size) , 
		m_base64_dummy(nullptr) , 
		m_size(stream_.m_size) , 
		m_external(false)
	{
		m_data = (u8*)malloc(m_size*sizeof(u8));
		m_pos = m_data;
		m_end = m_data + m_size;
		memcpy(m_data , stream_.m_data , m_size);
	}
	//--------------------------------------------------------
	c_stream::~c_stream()
	{
		if(m_data&&!m_external)
		{
			free(m_data);
			m_data = nullptr;
		}
		if(m_base64_dummy)
			free(m_base64_dummy);
		m_pos = nullptr;
		m_end = nullptr;
	}
	//--------------------------------------------------------
	c_stream& c_stream::operator=(const c_stream& stream_)
	{
		if(m_size != stream_.m_size)
		{
			m_size = stream_.m_size;
			m_data = (u8*)realloc(m_data , m_size*sizeof(u8));
		}
		m_pos = m_data;
		m_end = m_data + m_size;
		memcpy(m_data , stream_.m_data , m_size);
		return *this;
	}
	//--------------------------------------------------------
	void c_stream::reset(u32 sz_/* = 0*/)
	{
		if(m_data)
		{
			free(m_data);
			m_data = nullptr;
		}
		m_pos = nullptr;
		m_end = nullptr;
		m_size = sz_;
		if(sz_)
		{
			m_data = (u8*)malloc(m_size*sizeof(u8));
			m_pos = m_data;
			m_end = m_data + m_size;
		}
	}
	//--------------------------------------------------------
	void c_stream::resize(u32 sz_)
	{
		m_data = (u8*)realloc(m_data , sz_);
		m_pos = m_data + m_size;
		m_size = sz_;
		m_end = m_data + sz_;
	}
	//--------------------------------------------------------
	u32 c_stream::write_s8(s8 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(s8*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_u8(u8 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(u8*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_s16(s16 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(s16*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_u16(u16 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(u16*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_s32(s32 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(s32*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_u32(u32 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(u32*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_s64(s64 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(s64*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_u64(u64 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(u64*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_f32(f32 data_)
	{
		u32 written = sizeof(data_);
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		*(f32*)(m_pos) = data_;
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::write_buf(const void* buf_, u32 size_)
	{
		u32 written = size_;
		if(m_pos + written > m_end)
			written = (u32)(m_end - m_pos);
		if(written == 0)
			return 0;
		memcpy(m_pos, buf_, written);
		m_pos += written;
		return written;
	}
	//--------------------------------------------------------
	u32 c_stream::read_s8(s8& data_)
	{
		u32 cnt = sizeof(s8);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(s8*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_u8(u8& data_)
	{
		u32 cnt = sizeof(u8);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(u8*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_s16(s16& data_)
	{
		u32 cnt = sizeof(s16);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(s16*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_u16(u16& data_)
	{
		u32 cnt = sizeof(u16);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(u16*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_s32(s32& data_)
	{
		u32 cnt = sizeof(s32);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(s32*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_u32(u32& data_)
	{
		u32 cnt = sizeof(u32);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(u32*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_s64(s64& data_)
	{
		u32 cnt = sizeof(s64);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(s64*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_u64(u64& data_)
	{
		u32 cnt = sizeof(u64);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(u64*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_f32(f32& data_)
	{
		u32 cnt = sizeof(f32);
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		data_ = *(f32*)(m_pos);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	u32 c_stream::read_buf(void* buf_, u32 count_)
	{
		u32 cnt = count_;
		if(m_pos + cnt > m_end)
			cnt = (u32)(m_end - m_pos);
		if(cnt == 0)
			return 0;
		memcpy(buf_, m_pos, cnt);
		m_pos += cnt;
		return cnt;
	}
	//--------------------------------------------------------
	void c_stream::skip(s32 count_)
	{
		u32 newpos = (u32)((m_pos - m_data ) + count_ );
		m_pos = m_data + newpos;
	}
	//--------------------------------------------------------
	void c_stream::seek(u32 pos_)
	{
		if(m_pos+pos_>m_end)
			m_pos = m_end;
		if(m_pos+pos_<m_data)
			m_pos = m_data;
		m_pos = m_data + pos_;
	}
	//--------------------------------------------------------
	const ansi* c_stream::to_base64()
	{
		const ansi alphabet[] = "ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"	"ghijklmn" "opqrstuv" "wxyz0123" "456789+/";
		const ansi padchar = '=';
		s32 padlen = 0;
		ansi* tmp = (ansi*)malloc((m_size * 4) / 3 + 3);
		u32 i = 0;
		ansi *out = (ansi*)tmp;
		while(i < m_size)
		{
			s32 chunk = 0;
				chunk |= s32(u8(m_data[i++])) << 16;
			if(i == m_size) 
				padlen = 2;
			else 
			{
				chunk |= s32(u8(m_data[i++])) << 8;
				if(i == m_size)
					padlen = 1;
				else
					chunk |= s32(u8(m_data[i++]));
			}
			s32 j = (chunk & 0x00fc0000) >> 18;
			s32 k = (chunk & 0x0003f000) >> 12;
			s32 l = (chunk & 0x00000fc0) >> 6;
			s32 m = (chunk & 0x0000003f);
			*out++ = alphabet[j];
			*out++ = alphabet[k];
			if(padlen > 1) 
				*out++ = padchar;
			else 
				*out++ = alphabet[l];
			if(padlen > 0) 
				*out++ = padchar;
			else
				*out++ = alphabet[m];
		}
		u32 sz = out - tmp;
		m_base64_dummy = (ansi*)malloc(sz+1);
		strncpy(m_base64_dummy , tmp , sz);
		m_base64_dummy[sz] = 0;
		free(tmp);
		return m_base64_dummy;
	}
	//--------------------------------------------------------
	void c_stream::from_base64(const ansi* in_)
	{
		u32 inlen = strlen(in_);
		u32 outlength = 0;
		m_data = (u8*)malloc(inlen * 3.0f / 4.0f + 1);
		if(m_data)
		{
			s32 i, bits, c = 0, charcount, errors = 0;
			u32 inputidx = 0;
			u32 outputidx = 0;
			for(i = (sizeof internal::alphabet) - 1; i >= 0 ; i--) 
			{
				internal::inalphabet[internal::alphabet[i]] = 1;
				internal::decoder[internal::alphabet[i]] = i;
			}
			charcount = 0;
			bits = 0;
			for(inputidx=0; inputidx < inlen ; inputidx++) 
			{
				c = in_[inputidx];
				if(c == '=')
					break;
				if(c > 255 || ! internal::inalphabet[c])
					continue;
				bits += internal::decoder[c];
				charcount++;
				if(charcount == 4) 
				{
					m_data[outputidx++] = (bits >> 16);
					m_data[outputidx++] = ((bits >> 8) & 0xff);
					m_data[outputidx++] = (bits & 0xff);
					bits = 0;
					charcount = 0;
				}
				else
					bits <<= 6;
			}
			if(c == '=') 
			{
				switch(charcount) 
				{
				case 1:
					errors++;
					break;
				case 2:
					m_data[outputidx++] = (bits >> 10);
					break;
				case 3:
					m_data[outputidx++] = (bits >> 16);
					m_data[outputidx++] = ((bits >> 8) & 0xff);
					break;
				}
			}
			else if(inputidx < inlen) 
			{
				if(charcount)
					errors++;
			}
			outlength = outputidx;
			if(errors > 0)
			{
				free(m_data);
				m_data = nullptr; 
				outlength = 0;
			}
		}
		m_size = outlength;
		m_pos = m_data;
		m_end = m_data + m_size;
	}
	//--------------------------------------------------------
	bool c_stream::zip()
	{
		if(!m_data || !m_size)
			return false;
		uLongf newsz;
		Bytef* newbuf = (unsigned char*)malloc((m_size + 12)*1.001);
		compress(newbuf , &newsz , m_data , m_size);
		free(m_data);
		m_size_dummy = m_size;
		m_data = m_pos = newbuf;
		m_size = newsz;
		m_end = m_data + m_size;
		return true;
	}
	//--------------------------------------------------------
	bool c_stream::unzip()
	{
		if(!m_data || !m_size)
			return false;
		uLongf sz = m_size_dummy;
		Bytef* newbuf = (unsigned char*)malloc(m_size_dummy);
		uncompress(newbuf , &sz , m_data , m_size);
		assert(sz == m_size_dummy);
		free(m_data);
		m_data = m_pos = newbuf;
		m_size = m_size_dummy;
		m_end = m_data + m_size;
		return true;
	}
}