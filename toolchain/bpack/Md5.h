#pragma once
#include <string>
class c_md5_creator
{
private:
	struct s_ctx
	{
		unsigned int state[4];
		unsigned int count[2];
		unsigned char buffer[64];
	};
	s_ctx m_context;
public:
	c_md5_creator();
	~c_md5_creator();
public:
	std::string make(const char *str_, unsigned char digest_[16]);
	std::string make(const unsigned char* buf , size_t sz , unsigned char digest[16]);
private:
	void _update(unsigned char* input_, unsigned int inputlen_);
	void _final(unsigned char digest_[16]);
	void _transform(unsigned int state_[4], unsigned char block_[64]);
	void _encode(unsigned char* output_, unsigned int* input_, unsigned int len_);	
	void _decode(unsigned int* output_, unsigned char* input_, unsigned int len_);	
	unsigned int _f(unsigned int x_, unsigned int y_, unsigned int z_);
	unsigned int _g(unsigned int x_, unsigned int y_, unsigned int z_);
	unsigned int _h(unsigned int x_, unsigned int y_, unsigned int z_);
	unsigned int _i(unsigned int x_, unsigned int y_, unsigned int z_);
	void _ff(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_);
	void _gg(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_);
	void _hh(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_);
	void _ii(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_);
	unsigned int _rotate_left(unsigned int x_, unsigned int n_);
};