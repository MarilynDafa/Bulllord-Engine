/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED
#include "Prerequisites.h"
namespace o2d{
	class O2D_API c_stream
	{
	private:
		u32 m_size;
		u8* m_data;
		u8* m_pos;
		u8* m_end;
		ansi* m_base64_dummy;
		u32 m_size_dummy;
		bool m_external;
	public:
		c_stream();
		c_stream(u32 size_);
		c_stream(u8* data_ , u32 size_);
		c_stream(const c_stream& stream_);
		~c_stream();
		c_stream& operator=(const c_stream& stream_);
	public:
		inline bool is_eof() const{return m_pos >= m_end;}
		inline u32 get_tell() const{return (u32)(m_pos - m_data);}
		inline u32 get_size() const{return m_size;}	
		inline u8* get_pointer(){return m_data;}
		inline void set_zero_end(){m_data[m_size-1] = 0;}
		inline const u8* get_pointer() const{return m_data;}
		inline u8* get_current_pointer(){return m_pos;}
		inline const u8* get_current_pointer() const{return m_pos;}		
	public:
		void reset(u32 sz_ = 0);	
		void resize(u32 sz_);
		u32 write_s8(s8 data_);
		u32 write_u8(u8 data_);
		u32 write_s16(s16 data_);
		u32 write_u16(u16 data_);
		u32 write_s32(s32 data_);
		u32 write_u32(u32 data_);
		u32 write_s64(s64 data_);
		u32 write_u64(u64 data_);
		u32 write_f32(f32 data_);
		u32 write_buf(const void* buf_,u32 size_);
		u32 read_s8(s8& data_);
		u32 read_u8(u8& data_);
		u32 read_s16(s16& data_);
		u32 read_u16(u16& data_);
		u32 read_s32(s32& data_);
		u32 read_u32(u32& data_);
		u32 read_s64(s64& data_);
		u32 read_u64(u64& data_);
		u32 read_f32(f32& data_);
		u32 read_buf(void* buf_, u32 count_);
		void skip(s32 count_);
		void seek(u32 pos_);
		const ansi* to_base64();
		void from_base64(const ansi* in_);
		bool zip();
		bool unzip();
	};
}
#endif
