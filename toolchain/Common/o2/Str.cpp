/**************************************************************************************/
//   Copyright (c) 2013-2016 Marilyn Dafa
//   This file is part of the "Overdrive2D Engine".
//   For conditions of distribution and use, see copyright notice in config.h
/**************************************************************************************/
#include "Str.h"
#include "Tools.h"
#include "Macro.h"
namespace o2d{
	const c_string c_string::EmptyStr = L"";
	//--------------------------------------------------------
	c_string::c_string()
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 ,20*sizeof(utf16));
		m_buf.normal_buf = 0;
	}
	//--------------------------------------------------------
	c_string::c_string(const c_string& rth_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf ,0, 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(rth_);
	}
	//--------------------------------------------------------
	c_string::c_string(const c_string& rth_, u32 idx_, u32 num_/* = NPOS*/)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 ,20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(rth_, idx_, num_);
	}
	//--------------------------------------------------------
	c_string::c_string(const utf8* utf8buf_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf ,0, 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(utf8buf_ , utf8_len(utf8buf_));
	}
	//--------------------------------------------------------
	c_string::c_string(const utf8* utf8buf_, u32 len_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf ,0, 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(utf8buf_, len_);
	}
	//--------------------------------------------------------
	c_string::c_string(const utf16* utf16buf_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 ,20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(utf16buf_ , utf16_len(utf16buf_));
	}
	//--------------------------------------------------------
	c_string::c_string(const utf16* utf16buf_, u32 len_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 , 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(utf16buf_, len_);
	}
	//--------------------------------------------------------
	c_string::c_string(u32 num_, utf16 codepoint_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 , 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		_assign(num_, codepoint_);
	}
	//--------------------------------------------------------
	c_string::c_string(s32 val_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 , 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		ansi buf[32];
		sprintf(buf , "%d" , val_);
		_assign((utf8*)buf , utf8_len((const utf8*)buf));
	}
	//--------------------------------------------------------
	c_string::c_string(u32 val_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf ,0 , 20*sizeof(utf16));
		m_buf.normal_buf = 0;
		ansi buf[32];
		sprintf(buf , "%u" , val_);
		_assign((utf8*)buf , utf8_len((const utf8*)buf));
	}
	//--------------------------------------------------------
	c_string::c_string(f32 val_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 ,20*sizeof(utf16));
		m_buf.normal_buf = 0;
		ansi buf[64];
		sprintf(buf , "%f" , val_);
		_assign((utf8*)buf , utf8_len((const utf8*)buf));
	}
	//--------------------------------------------------------
	c_string::c_string(f64 val_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf , 0 ,20*sizeof(utf16));
		m_buf.normal_buf = 0;
		ansi buf[64];
		sprintf(buf , "%0.8f" , val_);
		_assign((utf8*)buf , utf8_len((const utf8*)buf));
	}
	//--------------------------------------------------------
	c_string::c_string(s64 val_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf ,0 ,  20*sizeof(utf16));
		m_buf.normal_buf = 0;
		ansi buf[64];
		sprintf(buf , "%" I64_FMT "d" , val_);
		_assign((utf8*)buf , utf8_len((const utf8*)buf));
	}
	//--------------------------------------------------------
	c_string::c_string(u64 val_)
	{
		m_length = 0;
		m_reserve = 20;
		m_encoded_buff = 0;
		memset(m_buf.quick_buf ,0 ,  20*sizeof(utf16));
		m_buf.normal_buf = 0;
		ansi buf[64];
		sprintf(buf , "%" I64_FMT "u" , val_);
		_assign((utf8*)buf , utf8_len((const utf8*)buf));
	}
	//--------------------------------------------------------
	c_string::~c_string()
	{
		if(m_reserve>20)
		{
			free(m_buf.normal_buf);
			m_buf.normal_buf = nullptr;
		}
		else
			memset(m_buf.quick_buf , 0 , 20*sizeof(utf16));
		if(m_encoded_buff)
		{
			free(m_encoded_buff);
			m_encoded_buff = nullptr;
		}
	}
	//--------------------------------------------------------
	utf16& c_string::operator[](u32 idx_)
	{
		return (_ptr()[idx_]);
	}
	//--------------------------------------------------------
	utf16 c_string::operator[](u32 idx_) const
	{
		return (_ptr()[idx_]);
	}
	//--------------------------------------------------------
	c_string& c_string::operator=(const c_string& str_)
	{
		clear();
		return _assign(str_);
	}
	//--------------------------------------------------------
	c_string& c_string::operator=(const utf8* utf8str_)
	{
		clear();
		return _assign(utf8str_, utf8_len(utf8str_));
	}
	//--------------------------------------------------------
	c_string& c_string::operator=(const utf16* utf16str_)
	{
		clear();
		return _assign(utf16str_, utf16_len(utf16str_));
	}
	//--------------------------------------------------------
	c_string& c_string::operator=(utf16 codepoint_)
	{
		clear();
		return _assign(1, codepoint_);
	}
	//--------------------------------------------------------
	bool c_string::operator==(const c_string& str_) const
	{
		return _compare(str_) == 0;
	}
	//--------------------------------------------------------
	bool c_string::operator==(const utf8* utf8str_) const
	{
		return _compare(utf8str_) == 0;
	}
	//--------------------------------------------------------
	bool c_string::operator==(const utf16* utf16str_) const
	{
		return _compare(utf16str_) == 0;
	}
	//--------------------------------------------------------
	bool c_string::operator!=(const c_string& str_) const
	{
		return _compare(str_) != 0;
	}
	//--------------------------------------------------------
	bool c_string::operator!=(const utf8* utf8str_) const
	{
		return _compare(utf8str_) != 0;
	}
	//--------------------------------------------------------
	bool c_string::operator!=(const utf16* utf16str_) const
	{
		return _compare(utf16str_) != 0;
	}
	//--------------------------------------------------------
	bool c_string::operator<(const c_string& str_) const
	{
		return _compare(str_) < 0;
	}
	//--------------------------------------------------------
	bool c_string::operator<(const utf8* utf8str_) const
	{
		return _compare(utf8str_) < 0;
	}
	//--------------------------------------------------------
	bool c_string::operator<(const utf16* utf16str_) const
	{
		return _compare(utf16str_) < 0;
	}
	//--------------------------------------------------------
	bool c_string::operator<=(const c_string& str_) const
	{
		return _compare(str_) <= 0;
	}
	//--------------------------------------------------------
	bool c_string::operator<=(const utf8* utf8str_) const
	{
		return _compare(utf8str_) <= 0;
	}
	//--------------------------------------------------------
	bool c_string::operator<=(const utf16* utf16str_) const
	{
		return _compare(utf16str_) <= 0;
	}
	//--------------------------------------------------------
	bool c_string::operator>(const c_string& str_) const
	{
		return _compare(str_) > 0;
	}
	//--------------------------------------------------------
	bool c_string::operator>(const utf8* utf8str_) const
	{
		return _compare(utf8str_) > 0;
	}
	//--------------------------------------------------------
	bool c_string::operator>(const utf16* utf16str_) const
	{
		return _compare(utf16str_) > 0;
	}
	//--------------------------------------------------------
	bool c_string::operator>=(const c_string& str_) const
	{
		return _compare(str_) >= 0;
	}
	//--------------------------------------------------------
	bool c_string::operator>=(const utf8* utf8str_) const
	{
		return _compare(utf8str_) >= 0;
	}
	//--------------------------------------------------------
	bool c_string::operator>=(const utf16* utf16str_) const
	{
		return _compare(utf16str_) >= 0;
	}
	//--------------------------------------------------------
	c_string c_string::operator+(const c_string& str_)
	{
		c_string temp(*this);
		temp.append(str_);
		return temp;
	}
	//--------------------------------------------------------
	c_string c_string::operator+(const utf16* utf16str_)
	{
		c_string temp(*this);
		temp.append(utf16str_);
		return temp;
	}
	//--------------------------------------------------------
	c_string c_string::operator+(const utf8* utf8str_)
	{
		c_string temp(*this);
		temp.append(utf8str_);
		return temp;
	}
	//--------------------------------------------------------
	c_string c_string::operator+(const utf16 codepoint_)
	{
		c_string temp(*this);
		temp.append(codepoint_);
		return temp;
	}
	//--------------------------------------------------------
	c_string& c_string::operator+=(const c_string& str_)
	{
		append(str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator+=(const utf16* utf16str_)
	{
		append(utf16str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator+=(const utf8* utf8str_)
	{
		append(utf8str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator+=(const utf16 codepoint_)
	{
		append(codepoint_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(const c_string& str_)
	{
		append(str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(const utf16* utf16str_)
	{
		append(utf16str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(utf16 utf16str_)
	{
		append(1 , utf16str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(const utf8* utf8str_)
	{
		append(utf8str_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(s32 val_)
	{
		c_string ret(val_);
		append(ret);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(f32 val_)
	{
		c_string ret(val_);
		append(ret);
		return *this;
	}	
	//--------------------------------------------------------
	c_string& c_string::operator<<(s64 val_)
	{
		c_string ret(val_);
		append(ret);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::operator<<(u64 val_)
	{
		c_string ret(val_);
		append(ret);
		return *this;
	}
	//--------------------------------------------------------
	u32 c_string::utf8_size() const
	{
		if(!m_encoded_buff)
			return 0;
		else
			return utf8_len(m_encoded_buff);
	}
	//--------------------------------------------------------
	void c_string::reserve(u32 num_ /*= 0*/)
	{
		if(num_ == 0)
		{
			u32 minsz = m_length + 1;
			if((m_reserve > 20) && (m_reserve > minsz))
			{
				if(minsz <= 20)
				{
					utf16 temp[20];
					memcpy(temp, m_buf.normal_buf, minsz * sizeof(utf16)); 
					free(m_buf.normal_buf);
					memcpy(m_buf.quick_buf , temp , minsz*sizeof(utf16));
					m_reserve = 20;
				}
				else
				{
					m_buf.normal_buf = (utf16*)realloc(m_buf.normal_buf , minsz*sizeof(utf16));
					m_reserve = minsz;
				}
			}
		}
		else
			_grow(num_);
	}
	//--------------------------------------------------------
	const utf16* c_string::c_str() const
	{
		return (m_reserve > 20) ? m_buf.normal_buf:m_buf.quick_buf;
	}
	//--------------------------------------------------------
	utf16* c_string::data()
	{
		return (m_reserve > 20) ? m_buf.normal_buf:m_buf.quick_buf;
	}
	//--------------------------------------------------------
	bool c_string::is_numeric()
	{
		ansi tmp[256] = "";
		if(!_ptr())
			return false;
		const ansi* ptr = (const ansi*)to_utf8();
		sscanf(ptr," %[.,0-9]",tmp); 
		if(strcmp(ptr ,tmp)) 
			return false;
		if(ansi* p = strstr(tmp , "."))
		{
            p = strstr(p+1 , ".");
            if(p)
				return false;
        }
		return true; 
	}
	//--------------------------------------------------------
	bool c_string::to_boolean() const
	{
		if(!utf16_cmp(_ptr() , L"true")||!utf16_cmp(_ptr() , L"TRUE"))
			return true;
		else
			return false;
	}
	//--------------------------------------------------------
	s32 c_string::to_integer() const
	{
		const ansi* str = (const ansi*)to_utf8();
		return (s32)strtol(str, nullptr, 0);
	}
	//--------------------------------------------------------
	u32 c_string::to_uinteger() const
	{
		const ansi* str = (const ansi*)to_utf8();
		return (u32)strtoul(str, nullptr, 0);
	}
	//--------------------------------------------------------
	s64 c_string::to_large_integer() const
	{
		const ansi* str = (const ansi*)to_utf8();
#if defined(O2D_COMPILER_MSVC)
		return _atoi64(str);
#else
        return atoll(str);
#endif
	}
	//--------------------------------------------------------
	u64 c_string::to_large_uinteger() const
	{
		const ansi* str = (const ansi*)to_utf8();
#if defined(O2D_COMPILER_MSVC)
		return _atoi64(str);
#else
        return atoll(str);
#endif
	}
	//--------------------------------------------------------
	f32 c_string::to_float() const
	{
		const ansi* str = (const ansi*)to_utf8();
		return atof(str);
	}
	//--------------------------------------------------------
	f64 c_string::to_double() const
	{
		const ansi* str = (const ansi*)to_utf8();
		return atof(str);
	}
	//--------------------------------------------------------
	utf8* c_string::to_utf8()
	{
		u32 buffsize = _encoded_size(_ptr(), m_length) + 1;
		if(m_encoded_buff)
			free(m_encoded_buff);
		m_encoded_buff = (utf8*)malloc(buffsize*sizeof(utf8));
		_encode(_ptr(), m_encoded_buff, buffsize, m_length);
		m_encoded_buff[buffsize-1] = ((utf8)0);
		return m_encoded_buff;
	}
	//--------------------------------------------------------
	const utf8* c_string::to_utf8() const
	{
		u32 buffsize = _encoded_size(_ptr(), m_length) + 1;
		if(m_encoded_buff)
			free(m_encoded_buff);
		m_encoded_buff = (utf8*)malloc(buffsize*sizeof(utf8));
		_encode(_ptr(), m_encoded_buff, buffsize, m_length);
		m_encoded_buff[buffsize-1] = ((utf8)0);
		return m_encoded_buff;
	}
	//--------------------------------------------------------
	void c_string::clear()
	{
		_set_len(0);
		u32 minsz = m_length + 1;
		if((m_reserve > 20) && (m_reserve > minsz))
		{
			free(m_buf.normal_buf);
			memset(m_buf.quick_buf , 0 , 20*sizeof(utf16));
			m_reserve = 20;
		}
	}
	//--------------------------------------------------------
	void c_string::trim(utf16 ch_/* = L' '*/)
	{
		for(u32 i = 0 ; i < m_length ; ++i)
		{
			utf16 code = *(data() + i);
			if(code == ch_ || code == L'\n' || code == L'\r' || code == L'\t')
				erase(0 , 1);
			else
				break;
		}
		for(s32 i = m_length ; i >=0 ; --i)
		{
			utf16 code = *(data() + i - 1);
			if(code == ch_ || code == L'\n' || code == L'\r' || code == L'\t')
				erase(m_length - 1 , 1);
			else
				break;
		}
	}
	//--------------------------------------------------------
	void c_string::resize(u32 num_)
	{
		resize(num_, utf16());
	}
	//--------------------------------------------------------
	void c_string::resize(u32 num_, utf16 code_point_)
	{
		if(num_ < m_length)
			_set_len(num_);
		else
			append(num_ - m_length, code_point_);
	}
	//--------------------------------------------------------
	c_string c_string::sub_str(u32 idx_, u32 num_) const
	{
		if((idx_ >= m_length) || (num_ <=0))
			return L"";
		return c_string(*this , idx_ , num_);
	}
	//--------------------------------------------------------
	u32 c_string::tokenize(const c_string& whitespace_, c_vector<c_string>& outtokens_) const
	{
		c_string str(*this);
		utf16* ptr = str.data();
		utf16* token;
		while(nullptr != (token = utf16_tok(ptr, whitespace_.c_str())))
		{
			outtokens_.push_back(token);
			ptr = nullptr;
		}
		return outtokens_.size();
	}
	//--------------------------------------------------------
	c_string& c_string::append(const c_string& str_, u32 stridx_/* = 0*/, u32 strnum_ /*= NPOS*/)
	{
		if(str_.is_empty())
			return *this;
		assert(str_.m_length >= stridx_);
		if((strnum_ == NPOS) || (strnum_ > str_.m_length - stridx_))
			strnum_ = str_.m_length - stridx_;
		_grow(m_length + strnum_);
		memcpy(&_ptr()[m_length], &str_._ptr()[stridx_], strnum_ * sizeof(utf16));
		_set_len(m_length + strnum_);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::append(const utf8* utf8str_)
	{
		if(!utf8str_)
			return *this;
		return append(utf8str_, utf8_len(utf8str_));
	}
	//--------------------------------------------------------
	c_string& c_string::append(const utf8* utf8str_, u32 len_)
	{
		if(!utf8str_)
			return *this;
		assert(len_ != NPOS);
		u32 encsz = _encoded_size(utf8str_, len_);
		u32 newsz = m_length + encsz;
		_grow(newsz);
		_encode(utf8str_, &_ptr()[m_length], encsz, len_);
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::append(const utf16* str_)
	{
		if(!str_)
			return *this;
		return append(str_ , utf16_len(str_));
	}
	//--------------------------------------------------------
	c_string& c_string::append(const utf16* str_, u32 len_)
	{
		if(!str_)
			return *this;
		assert(len_ != NPOS);
		u32 newsz = m_length + len_;
		_grow(newsz);
		utf16* p = &_ptr()[m_length];
		while(len_--)
			*p++ = *str_++;
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::append(u32 num_, utf16 codepoint_)
	{
		if(!num_)
			return *this;
		assert(num_ != NPOS);
		u32 newsz = m_length + num_;
		_grow(newsz);
		utf16* p = &_ptr()[m_length];
		while(num_--)
			*p++ = codepoint_;
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::append(utf16 codepoint_)
	{
		return append(1 , codepoint_);
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, const c_string& str_)
	{
		return insert(idx_, str_, 0, NPOS);
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, const c_string& str_, u32 stridx_, u32 strnum_)
	{
		assert((m_length>=idx_)&&(str_.m_length>=stridx_));
		if((strnum_ == NPOS) || (strnum_ > str_.m_length - stridx_))
			strnum_ = str_.m_length - stridx_;
		u32 newsz = m_length + strnum_;
		_grow(newsz);
		memmove(&_ptr()[idx_ + strnum_], &_ptr()[idx_], (m_length - idx_) * sizeof(utf16));
		memcpy(&_ptr()[idx_], &str_._ptr()[stridx_], strnum_ * sizeof(utf16));
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, const utf8* utf8str_)
	{
		return insert(idx_, utf8str_, utf8_len(utf8str_));
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, const utf8* utf8str_, u32 len_)
	{
		assert(m_length >= idx_);
		assert(len_ != NPOS);
		u32 encsz = _encoded_size(utf8str_, len_);
		u32 newsz = m_length + encsz;
		_grow(newsz);
		memmove(&_ptr()[idx_ + encsz], &_ptr()[idx_], (m_length - idx_) * sizeof(utf16));
		_encode(utf8str_, &_ptr()[idx_], encsz, len_);
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, const utf16* utf16str_)
	{
		return insert(idx_, c_string(utf16str_));
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, const utf16* utf16str_, u32 len_)
	{
		return insert(idx_, c_string(utf16str_ , len_));
	}
	//--------------------------------------------------------
	c_string& c_string::insert(u32 idx_, u32 num_, utf16 codepoint_)
	{
		assert(m_length >= idx_);
		assert(num_ != NPOS);
		u32 newsz = m_length + num_;
		_grow(newsz);
		memmove(&_ptr()[idx_ + num_], &_ptr()[idx_], (m_length - idx_) * sizeof(utf16));
		utf16* pt = &_ptr()[idx_ + num_ - 1];
		while(num_--)
			*pt-- = codepoint_;
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::erase(u32 idx_)
	{
		return erase(idx_, 1);
	}
	//--------------------------------------------------------
	c_string& c_string::erase(u32 idx_, u32 len_)
	{
		assert(m_length >= idx_);
		if(len_ == NPOS)
			len_ = m_length - idx_;
		u32 newsz = m_length - len_;
		memmove(&_ptr()[idx_], &_ptr()[idx_ + len_], (m_length - idx_ - len_) * sizeof(utf16));
		_set_len(newsz);
		return	*this;
	}
	//--------------------------------------------------------
	c_string& c_string::replace(u32 idx_, u32 len_, const c_string& str_)
	{
		return replace(idx_, len_, str_, 0, NPOS);
	}
	//--------------------------------------------------------
	c_string& c_string::replace(u32 idx_, u32 len_, const c_string& str_, u32 stridx_, u32 strnum_)
	{
		assert((m_length>=idx_)&&(str_.m_length>=stridx_));
		if(((stridx_ + strnum_) > str_.m_length) || (strnum_ == NPOS))
			strnum_ = str_.m_length - stridx_;
		if(((len_ + idx_) > m_length) || (len_ == NPOS))
			len_ = m_length - idx_;
		u32 newsz = m_length + strnum_ - len_;
		_grow(newsz);
		if((idx_ + len_) < m_length)
			memmove(&_ptr()[idx_ + strnum_], &_ptr()[len_ + idx_], (m_length - idx_ - len_) * sizeof(utf16));
		memcpy(&_ptr()[idx_], &str_._ptr()[stridx_], strnum_ * sizeof(utf16));
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::replace(u32 idx_, u32 len_, u32 num_, utf16 codepoint_)
	{
		assert(m_length >= idx_);
		assert(num_ != NPOS);
		if(((len_ + idx_) > m_length) || (len_ == NPOS))
			len_ = m_length - idx_;
		u32 newsz = m_length + num_ - len_;
		_grow(newsz);
		if((idx_ + len_) < m_length)
			memmove(&_ptr()[idx_ + num_], &_ptr()[len_ + idx_], (m_length - idx_ - len_) * sizeof(utf16));
		utf16* pt = &_ptr()[idx_ + num_ - 1];
		while (num_--)
			*pt-- = codepoint_;
		_set_len(newsz);
		return *this;
	}
	//--------------------------------------------------------
	u32 c_string::find(utf16 codepoint_, u32 idx_ /*= 0*/) const
	{
		if(idx_ < m_length)
		{
			const utf16* pt = &_ptr()[idx_];
			while(idx_ < m_length)
			{
				if(*pt++ == codepoint_)
					return idx_;
				++idx_;
			}
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::rfind(utf16 codepoint_, u32 idx_ /*= NPOS*/) const
	{
		if(idx_ >= m_length)
			idx_ = m_length - 1;
		if(m_length > 0)
		{
			const utf16* pt = &_ptr()[idx_];
			do
			{
				if(*pt-- == codepoint_)
					return idx_;
			} while (idx_-- != 0);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find(const c_string& str_, u32 idx_/* = 0*/) const
	{
		if((str_.m_length == 0) && (idx_ < m_length))
			return idx_;
		if(idx_ < m_length)
		{
			while(m_length - idx_ >= str_.m_length)
			{
				if(0 == _compare(idx_, str_.m_length, str_))
					return idx_;
				++idx_;
			}
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::rfind(const c_string& str_, u32 idx_ /*= NPOS*/) const
	{
		if(str_.m_length == 0)
			return (idx_ < m_length) ? idx_ : m_length;
		if(str_.m_length <= m_length)
		{
			if(idx_ > (m_length - str_.m_length))
				idx_ = m_length - str_.m_length;
			do
			{
				if(0 == _compare(idx_, str_.m_length, str_))
					return idx_;
			} while (idx_-- != 0);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find(const utf16* str_, u32 idx_ /*= 0*/) const
	{
		return find(c_string(str_) , idx_);
	}
	//--------------------------------------------------------
	u32 c_string::rfind(const utf16* str_, u32 idx_ /*= NPOS*/) const
	{
		return rfind(c_string(str_) , idx_);
	}
	//--------------------------------------------------------
	u32 c_string::find_first_of(utf16 codepoint_, u32 idx_ /*= 0*/) const
	{
		return find(codepoint_, idx_);
	}
	//--------------------------------------------------------
	u32 c_string::find_first_not_of(utf16 codepoint_, u32 idx_/* = 0*/) const
	{
		if(idx_ < m_length)
		{
			do
			{
				if((*this)[idx_] != codepoint_)
					return idx_;
			} while(idx_++ < m_length);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find_first_of(const c_string& str_, u32 idx_/* = 0*/) const
	{
		if(idx_ < m_length)
		{
			const utf16* pt = &_ptr()[idx_];
			do
			{
				if(NPOS != str_.find(*pt++))
					return idx_;
			} while(++idx_ != m_length);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find_first_not_of(const c_string& str_, u32 idx_ /*= 0*/) const
	{
		if(idx_ < m_length)
		{
			const utf16* pt = &_ptr()[idx_];
			do
			{
				if(NPOS == str_.find(*pt++))
					return idx_;
			} while(++idx_ != m_length);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find_first_of(const utf16* str_, u32 idx_ /*= 0*/) const
	{
		return find_first_of(c_string(str_));
	}
	//--------------------------------------------------------
	u32 c_string::find_first_not_of(const utf16* str_, u32 idx_/* = 0*/) const
	{
		return find_first_not_of(c_string(str_));
	}
	//--------------------------------------------------------
	u32 c_string::find_last_of(utf16 codepoint_, u32 idx_/* = NPOS*/) const
	{
		return rfind(codepoint_, idx_);
	}
	//--------------------------------------------------------
	u32 c_string::find_last_not_of(utf16 codepoint_, u32 idx_/* = NPOS*/) const
	{
		if(m_length > 0)
		{
			if(idx_ >= m_length)
				idx_ = m_length - 1;
			do
			{
				if((*this)[idx_] != codepoint_)
					return idx_;
			} while(idx_-- != 0);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find_last_of(const c_string& str_, u32 idx_ /*= NPOS*/) const
	{
		if(m_length > 0)
		{
			if(idx_ >= m_length)
				idx_ = m_length - 1;
			const utf16* pt = &_ptr()[idx_];
			do
			{
				if(NPOS != str_.find(*pt--))
					return idx_;
			} while(idx_-- != 0);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find_last_not_of(const c_string& str_, u32 idx_ /*= NPOS*/) const
	{
		if(m_length > 0)
		{
			if(idx_ >= m_length)
				idx_ = m_length - 1;
			const utf16* pt = &_ptr()[idx_];
			do
			{
				if(NPOS == str_.find(*pt--))
					return idx_;
			} while(idx_-- != 0);
		}
		return NPOS;
	}
	//--------------------------------------------------------
	u32 c_string::find_last_of(const utf16* str_, u32 idx_ /*= NPOS*/) const
	{
		return find_last_of(c_string(str_));
	}
	//--------------------------------------------------------
	u32 c_string::find_last_not_of(const utf16* str_, u32 idx_ /*= NPOS*/) const
	{
		return find_last_not_of(c_string(str_));
	}
	//--------------------------------------------------------
	void c_string::_grow(u32 newsz_)
	{
		++newsz_;
		if(newsz_ > m_reserve)
		{
			if(m_reserve > 20)
			{
				m_buf.normal_buf = (utf16*)realloc(m_buf.normal_buf , newsz_ * sizeof(utf16));
			}
			else
			{
				utf16* temp = (utf16*)malloc(newsz_ * sizeof(utf16));
				memcpy(temp, m_buf.quick_buf, (m_length + 1) * sizeof(utf16));
				m_buf.normal_buf = temp;
			}
			m_reserve = newsz_;
		}
	}
	//--------------------------------------------------------
	utf16* c_string::_ptr()
	{
		return (m_reserve > 20) ? m_buf.normal_buf:m_buf.quick_buf;
	}
	//--------------------------------------------------------
	const utf16* c_string::_ptr() const
	{
		return (m_reserve > 20) ? m_buf.normal_buf:m_buf.quick_buf;
	}
	//--------------------------------------------------------
	void c_string::_set_len(u32 len_)
	{
		m_length = len_;
		_ptr()[len_] = (utf16)(0);
	}
	//--------------------------------------------------------
	c_string& c_string::_assign(const c_string& rhs_, u32 idx_/* = 0*/, u32 num_/* = NPOS*/)
	{
		assert(idx_<=rhs_.m_length);
		if((num_ == NPOS) || (num_ > rhs_.m_length - idx_))
			num_ = rhs_.m_length - idx_;
		_grow(num_);
		_set_len(num_);
		memcpy(_ptr(), &rhs_._ptr()[idx_],  num_ * sizeof(utf16));
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::_assign(const utf8* utf8buf_, u32 len_)
	{
		assert(len_!=NPOS);
		u32 encsze = _encoded_size(utf8buf_, len_);
		_grow(encsze);
		_encode(utf8buf_, _ptr(), m_reserve, len_);
		_set_len(encsze);
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::_assign(const utf16* utf16buf_, u32 len_)
	{
		assert(len_!=NPOS);
		_grow(len_);
		_set_len(len_);
		utf16* p = _ptr();
		while(len_--)
			*p++ = *utf16buf_++;
		return *this;
	}
	//--------------------------------------------------------
	c_string& c_string::_assign(u32 num_, utf16 codepoint_)
	{
		assert(num_!=NPOS);
		_grow(num_);
		_set_len(num_);
		utf16* p = _ptr();
		while(num_--)
			*p++ = codepoint_;
		return *this;
	}
	//--------------------------------------------------------
	s32 c_string::_compare(const c_string& rth_) const
	{
		return _compare(0, m_length, rth_);
	}
	//--------------------------------------------------------
	s32 c_string::_compare(const utf8* utf8buf_) const
	{
		return _compare(0, m_length, utf8buf_, _encoded_size(utf8buf_ , utf8_len(utf8buf_)));
	}
	//--------------------------------------------------------
	s32 c_string::_compare(const utf16* utf16buf_) const
	{
		return _compare(0, m_length, c_string(utf16buf_));
	}
	//--------------------------------------------------------
	s32 c_string::_compare(u32 idx_, u32 len_, const utf8* utf8str_, u32 strcplen_) const
	{
		assert(m_length>=idx_);
		assert(strcplen_ != NPOS);
		if((len_ == NPOS) || (idx_ + len_ > m_length))
			len_ = m_length - idx_;
		s32 val = (len_ == 0) ? 0 : _utf16_comp_utf8(&_ptr()[idx_], utf8str_, (len_ < strcplen_) ? len_ : strcplen_);
		return (val != 0) ? ((val < 0) ? -1 : 1) : (len_ < strcplen_) ? -1 : (len_ == strcplen_) ? 0 : 1;
	}
	//--------------------------------------------------------
	s32 c_string::_compare(u32 idx_, u32 len_, const c_string& str_, u32 stridx_/* = 0*/, u32 strlen_/* = npos*/) const
	{
		assert((m_length >= idx_)&&(str_.m_length>= stridx_));
		if((len_ == NPOS) || (idx_ + len_ > m_length))
			len_ = m_length - idx_;
		if((strlen_ == NPOS) || (stridx_ + strlen_ > str_.m_length))
			strlen_ = str_.m_length - stridx_;
		s32 val = (len_ == 0) ? 0 : _utf16_comp_utf16(&_ptr()[idx_], &str_._ptr()[stridx_], (len_ < strlen_) ? len_ : strlen_);
		return (val != 0) ? ((val < 0) ? -1 : 1) : (len_ < strlen_) ? -1 : (len_ == strlen_) ? 0 : 1;
	}
	//--------------------------------------------------------
	s32 c_string::_utf16_comp_utf16(const utf16* buf1_, const utf16* buf2_, u32 cpcount_) const
	{
		if(!cpcount_)
			return 0;
		while((--cpcount_) && (*buf1_ == *buf2_))
			buf1_++, buf2_++;
		return *buf1_ - *buf2_;
	}
	//--------------------------------------------------------
	s32 c_string::_utf16_comp_utf8(const utf16* buf1_, const utf8* buf2_, u32 cpcount_) const
	{
		if(!cpcount_)
			return 0;
		utf16 cp;
		utf8 cu;
		do
		{	
			cu = *buf2_++;
			if(cu < 0x80)
				cp = (utf16)(cu);
			else if(cu < 0xE0)
			{	cp = ((cu & 0x1F) << 6);	cp |= (*buf2_++ & 0x3F);}
			else if(cu < 0xF0)
			{	cp = ((cu & 0x0F) << 12);	cp |= ((*buf2_++ & 0x3F) << 6); 	cp |= (*buf2_++ & 0x3F);}
			else
			{	cp = L'?';	}
		} while ((*buf1_++ == cp) && (--cpcount_));
		return (*--buf1_) - cp;
	}
	//--------------------------------------------------------
	u32 c_string::_encoded_size(const utf8* buf_, u32 len_) const
	{
		utf8 tcp;
		u32 count = 0;
		while(len_--)
		{
			tcp = *buf_++;
			++count;
			if(tcp < 0x80)
			{   ;	}
			else if(tcp < 0xE0)
			{	--len_;	++buf_;	}
			else if(tcp < 0xF0)
			{	len_ -= 2;	buf_ += 2;	}
			else
			{   len_ -= 2;	buf_ += 3;	}
		}
		return count;
	}
	//--------------------------------------------------------
	u32 c_string::_encoded_size(const utf16* buf_, u32 len_) const
	{
		u32 count = 0;
		while(len_--)
			count += _encoded_size(*buf_++);
		return count;
	}
	//--------------------------------------------------------
	u32 c_string::_encoded_size(utf16 codepoint_) const
	{
		if(codepoint_ < 0x80)
			return 1;
		else if(codepoint_ < 0x0800)
			return 2;
		else
			return 3;
	}
	//--------------------------------------------------------
	u32 c_string::_encode(const utf8* src_, utf16* dest_, u32 destlen_, u32 srclen_ /*= 0*/) const
	{
		if(srclen_ == 0)
			srclen_ = utf8_len(src_);
		u32 destcapacity = destlen_;
		for(u32 idx = 0; ((idx < srclen_) && (destcapacity > 0));)
		{
			utf16 cp;
			utf8 cu = src_[idx++];
			if(cu < 0x80)
				cp = (utf16)(cu);
			else if(cu < 0xE0)
			{
				cp = ((cu & 0x1F) << 6);
				cp |= (src_[idx++] & 0x3F);
			}
			else if(cu < 0xF0)
			{
				cp = ((cu & 0x0F) << 12);
				cp |= ((src_[idx++] & 0x3F) << 6);
				cp |= (src_[idx++] & 0x3F);
			}
			else
			{
				cp = L'?';
			}
			*dest_++ = cp;
			--destcapacity;
		}
		return destlen_ - destcapacity;
	}
	//--------------------------------------------------------
	u32 c_string::_encode(const utf16* src_, utf8* dest_, u32 destlen_, u32 srclen_ /*= 0*/) const
	{
		if(srclen_ == 0)
			srclen_ = utf16_len(src_);
		u32 destcapacity = destlen_;
		for(u32 idx = 0; idx < srclen_; ++idx)
		{
			utf16 cp = src_[idx];
			if(destcapacity < _encoded_size(cp))
				break;
			if(cp < 0x80)
			{
				*dest_++ = (utf8)cp;
				--destcapacity;
			}
			else if(cp < 0x0800)
			{
				*dest_++ = (utf8)((cp >> 6) | 0xC0);
				*dest_++ = (utf8)((cp & 0x3F) | 0x80);
				destcapacity -= 2;
			}
			else
			{
				*dest_++ = (utf8)((cp >> 12) | 0xE0);
				*dest_++ = (utf8)(((cp >> 6) & 0x3F) | 0x80);
				*dest_++ = (utf8)((cp & 0x3F) | 0x80);
				destcapacity -= 3;
			}
		}
		return destlen_ - destcapacity;
	}
}
