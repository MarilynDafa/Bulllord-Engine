#pragma once

class  c_stream
{
private:
	size_t m_size;
	unsigned char* m_data;
	unsigned char* m_pos;
	unsigned char* m_end;
	bool m_external;
public:
	c_stream();
	c_stream(size_t size_);
	c_stream(unsigned char* data_ , size_t size_);
	c_stream(const c_stream& stream_);
	~c_stream();
	c_stream& operator=(const c_stream& stream_);
public:
	inline bool is_eof() const{return m_pos >= m_end;}
	inline size_t get_tell() const{return m_pos - m_data;}
	inline size_t get_size() const{return m_size;}	
	inline unsigned char* get_pointer(){return m_data;}
	inline void set_zero_end(){m_data[m_size-1] = 0;}
	inline const unsigned char* get_pointer() const{return m_data;}
	inline unsigned char* get_current_pointer(){return m_pos;}
	inline const unsigned char* get_current_pointer() const{return m_pos;}		
public:
	void reset(unsigned int sz_ = 0);	
	void resize(unsigned int sz_);
	unsigned int write_buf(const void* buf_,unsigned int count_);
	unsigned int read_buf(void* buf_, unsigned int count_);
	void skip(int count_);
	void seek(unsigned int pos_);		
};