#include "Stream.h"
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

c_stream::c_stream()
	:m_data(nullptr) , 
	m_pos(nullptr) , 
	m_end(nullptr) , 
	m_size(0) , 
	m_external(false)
{}
//--------------------------------------------------------
c_stream::c_stream(unsigned int size_)
	:m_data(nullptr) , 
	m_pos(nullptr) , 
	m_end(nullptr) , 
	m_size(size_) , 
	m_external(false)
{
	m_data = (unsigned char*)malloc(m_size*sizeof(unsigned char));
	m_pos = m_data;
	m_end = m_data + m_size;
	assert(m_end>=m_pos);
}
//--------------------------------------------------------
c_stream::c_stream(unsigned char* data_, unsigned int size_)
	:m_data(data_) , 
	m_pos(data_) , 
	m_end(m_data + m_size) , 
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
	m_size(stream_.m_size) , 
	m_external(false)
{
	m_data = (unsigned char*)malloc(m_size*sizeof(unsigned char));
	m_pos = m_data;
	m_end = m_data + m_size;
	memcpy(m_data , stream_.m_data , m_size);
}
//--------------------------------------------------------
c_stream::~c_stream()
{
	if (m_data&&!m_external)
	{
		free(m_data);
		m_data = nullptr;
	}
	m_pos = nullptr;
	m_end = nullptr;
}
//--------------------------------------------------------
c_stream& c_stream::operator=(const c_stream& stream_)
{
	if(m_size != stream_.m_size)
	{
		m_size = stream_.m_size;
		m_data = (unsigned char*)realloc(m_data , m_size*sizeof(unsigned char));
	}
	m_pos = m_data;
	m_end = m_data + m_size;
	memcpy(m_data , stream_.m_data , m_size);
	return *this;
}
//--------------------------------------------------------
void c_stream::reset(unsigned int sz_/* = 0*/)
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
		m_data = (unsigned char*)malloc(m_size*sizeof(unsigned char));
		m_pos = m_data;
		m_end = m_data + m_size;
	}
}
//--------------------------------------------------------
void c_stream::resize(unsigned int sz_)
{
	m_data = (unsigned char*)realloc(m_data , sz_);
	m_pos = m_data + m_size;
	m_size = sz_;
	m_end = m_data + sz_;
}
//--------------------------------------------------------
unsigned int c_stream::write_buf(const void* buf_, unsigned int count_)
{
	unsigned int written = count_;
	if (m_pos + written > m_end)
		written = m_end - m_pos;
	if (written == 0)
		return 0;
	memcpy(m_pos, buf_, written);
	m_pos += written;
	return written;
}
//--------------------------------------------------------
unsigned int c_stream::read_buf(void* buf_, unsigned int count_)
{
	unsigned int cnt = count_;
	if (m_pos + cnt > m_end)
		cnt = m_end - m_pos;
	if (cnt == 0)
		return 0;
	memcpy(buf_, m_pos, cnt);
	m_pos += cnt;
	return cnt;
}
//--------------------------------------------------------
void c_stream::skip(int count_)
{
	unsigned int newpos = (unsigned int)((m_pos - m_data ) + count_ );
	m_pos = m_data + newpos;
}
//--------------------------------------------------------
void c_stream::seek(unsigned int pos_)
{
	if(m_pos+pos_>m_end)
		m_pos = m_end;
	if(m_pos+pos_<m_data)
		m_pos = m_data;
	m_pos = m_data + pos_;
}