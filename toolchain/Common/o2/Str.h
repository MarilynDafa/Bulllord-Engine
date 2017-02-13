/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#ifndef STR_H_INCLUDE
#define STR_H_INCLUDE
#include "Vector.h"
namespace o2d{
	class O2D_API c_string
	{
	private:
		u32 m_length;
		u32 m_reserve;
		mutable utf8* m_encoded_buff;
		union
		{
			utf16* normal_buf;
			utf16 quick_buf[20];
		} m_buf;
	public:
		const static c_string EmptyStr;
	public:
		c_string();
		c_string(const c_string& rth_);
		c_string(const c_string& rth_, u32 idx_, u32 num_ = NPOS);
		c_string(const utf8* utf8buf_);
		c_string(const utf8* utf8buf_ , u32 len_);
		c_string(const utf16* utf16buf_);
		c_string(const utf16* utf16buf_ , u32 len_);
		c_string(u32 num_, utf16 codepoint_);
		c_string(s32 val_);
		c_string(u32 val_);
		c_string(f32 val_);
		c_string(f64 val_);
		c_string(s64 val_);
		c_string(u64 val_);
		~c_string();
	public:
		utf16& operator[](u32 idx_);
		utf16 operator[](u32 idx_) const;
		c_string& operator=(const c_string& str_);
		c_string& operator=(const utf8* utf8str_);
		c_string& operator=(const utf16* utf16str_);
		c_string& operator=(utf16 codepoint_);
		bool operator==(const c_string& str_) const;
		bool operator==(const utf8* utf8str_) const;
		bool operator==(const utf16* utf16str_) const;
		bool operator!=(const c_string& str_) const;
		bool operator!=(const utf8* utf8str_) const;
		bool operator!=(const utf16* utf16str_) const;
		bool operator<(const c_string& str_) const;
		bool operator<(const utf8* utf8str_) const;
		bool operator<(const utf16* utf16str_) const;
		bool operator<=(const c_string& str_) const;
		bool operator<=(const utf8* utf8str_) const;
		bool operator<=(const utf16* utf16str_) const;
		bool operator>(const c_string& str_) const;
		bool operator>(const utf8* utf8str_) const;
		bool operator>(const utf16* utf16str_) const;
		bool operator>=(const c_string& str_) const;
		bool operator>=(const utf8* utf8str_) const;
		bool operator>=(const utf16* utf16str_) const;
		c_string operator+(const c_string& str_);
		c_string operator+(const utf16* utf16str_);
		c_string operator+(const utf8* utf8str_);
		c_string operator+(const utf16 codepoint_);
		c_string& operator+=(const c_string& str_);
		c_string& operator+=(const utf16* utf16str_);
		c_string& operator+=(const utf8* utf8str_);
		c_string& operator+=(const utf16 codepoint_);
		c_string& operator<<(const c_string& str_);
		c_string& operator<<(const utf16* utf16str_);
		c_string& operator<<(utf16 utf16str_);
		c_string& operator<<(const utf8* utf8str_);
		c_string& operator<<(s32 val_);
		c_string& operator<<(f32 val_);
		c_string& operator<<(s64 val_);
		c_string& operator<<(u64 val_);
	public:
		inline u32 get_capacity() const{return m_reserve - 1;}
		bool is_empty() const{return(m_length == 0);}
		inline u32 get_size() const{return m_length;}
		inline u32 get_length() const {return m_length;}
	public:
		u32 utf8_size() const;		
		void reserve(u32 num_ = 0);
		const utf16* c_str() const;
		utf16* data();
		bool is_numeric();
		bool to_boolean() const;		
		s32 to_integer() const;
		u32 to_uinteger() const;
		s64 to_large_integer() const;
		u64 to_large_uinteger() const;
		f32 to_float() const;
		f64 to_double() const;
		utf8* to_utf8();
		const utf8* to_utf8() const;
		void clear();
		void trim(utf16 ch_ = L' ');
		void resize(u32 num_);
		void resize(u32 num_, utf16 codepoint_);
		c_string sub_str(u32 idx_ , u32 num_ = NPOS) const;
		u32 tokenize(const c_string& whitespace_, c_vector<c_string>& outtokens_) const;
		c_string& append(const c_string& str_, u32 stridx_ = 0, u32 strnum_ = NPOS);
		c_string& append(const utf8* utf8str_);
		c_string& append(const utf8* utf8str_, u32 len_);
		c_string& append(const utf16* str_);
		c_string& append(const utf16* str_ , u32 len_);
		c_string& append(u32 num_, utf16 codepoint_);
		c_string& append(utf16 codepoint_);
		c_string& insert(u32 idx_, const c_string& str_);
		c_string& insert(u32 idx_, const c_string& str_, u32 stridx_, u32 strnum_);
		c_string& insert(u32 idx_, const utf8* utf8str_);
		c_string& insert(u32 idx_, const utf8* utf8str_, u32 len_);
		c_string& insert(u32 idx_, const utf16* utf16str_);
		c_string& insert(u32 idx_, const utf16* utf16str_, u32 len_);
		c_string& insert(u32 idx_, u32 num_, utf16 codepoint_);
		c_string& erase(u32 idx_);
		c_string& erase(u32 idx_, u32 len_);
		c_string& replace(u32 idx_, u32 len_, const c_string& str_);
		c_string& replace(u32 idx_, u32 len_, const c_string& str_, u32 stridx_, u32 strnum_);
		c_string& replace(u32 idx_, u32 len_, u32 num_, utf16 codepoint_);
		u32 find(utf16 codepoint_, u32 idx_ = 0) const;
		u32 rfind(utf16 codepoint_, u32 idx_ = NPOS) const;
		u32 find(const c_string& str_, u32 idx_ = 0) const;
		u32 rfind(const c_string& str_, u32 idx_ = NPOS) const;
		u32 find(const utf16* str_ , u32 idx_ = 0) const;
		u32 rfind(const utf16* str_ , u32 idx_ = NPOS) const;
		u32 find_first_of(utf16 codepoint_, u32 idx_ = 0) const;
		u32 find_first_not_of(utf16 codepoint_, u32 idx_ = 0) const;
		u32 find_first_of(const c_string& str_, u32 idx_ = 0) const;
		u32 find_first_not_of(const c_string& str_, u32 idx_ = 0) const;
		u32 find_first_of(const utf16* str_, u32 idx_ = 0) const;
		u32 find_first_not_of(const utf16* str_, u32 idx_ = 0) const;
		u32 find_last_of(utf16 codepoint_, u32 idx_ = NPOS) const;
		u32 find_last_not_of(utf16 codepoint_, u32 idx_ = NPOS) const;
		u32 find_last_of(const c_string& str_, u32 idx_ = NPOS) const;
		u32 find_last_not_of(const c_string& str_, u32 idx_ = NPOS) const;
		u32 find_last_of(const utf16* str_, u32 idx_ = NPOS) const;
		u32 find_last_not_of(const utf16* str_, u32 idx_ = NPOS) const;
	private:
		void _grow(u32 newsz_);
		utf16* _ptr();
		const utf16* _ptr() const;
		void _set_len(u32 len);
		c_string& _assign(const c_string& rhs_, u32 idx_ = 0, u32 num_ = NPOS);
		c_string& _assign(const utf8* utf8buf_, u32 len_);
		c_string& _assign(const utf16* utf16buf_, u32 len_);
		c_string& _assign(u32 num_, utf16 codepoint_);
		s32 _compare(const c_string& rth_) const;
		s32 _compare(const utf8* utf8buf_) const;
		s32 _compare(const utf16* utf16buf_) const;
		s32 _compare(u32 idx_, u32 len_, const c_string& str_, u32 stridx_ = 0, u32 strlen_ = NPOS) const;
		s32 _compare(u32 idx_, u32 len_, const utf8* utf8str_, u32 strcplen_) const;
		s32 _utf16_comp_utf16(const utf16* buf1_, const utf16* buf2_, u32 cpcount_) const;
		s32 _utf16_comp_utf8(const utf16* buf1_, const utf8* buf2_, u32 cpcount_) const;
		u32 _encoded_size(const utf8* buf_, u32 len_) const;
		u32 _encoded_size(const utf16* buf_, u32 len_) const;
		u32 _encoded_size(utf16 codepoint_) const;
		u32 _encode(const utf8* src_, utf16* dest_, u32 destlen_, u32 srclen_ = 0) const;
		u32 _encode(const utf16* src_, utf8* dest_, u32 destlen_, u32 srclen_ = 0) const;
	};
}
#endif
