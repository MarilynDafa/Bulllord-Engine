#include "Md5.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>

namespace internal{
	unsigned char _padding[64] =
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
}
c_md5_creator::c_md5_creator(){}
//--------------------------------------------------------
c_md5_creator::~c_md5_creator(){}
//--------------------------------------------------------
std::string c_md5_creator::make(const char *str_, unsigned char digest_[16])
{
	std::string ret;
	unsigned int len = strlen(str_);
	m_context.count[0] = m_context.count[1] = 0;
	m_context.state[0] = 0x67452301;
	m_context.state[1] = 0xefcdab89;
	m_context.state[2] = 0x98badcfe;
	m_context.state[3] = 0x10325476;
	_update((unsigned char*)str_, len);
	_final(digest_);
	char tmp[256];
	memset(tmp, 0 , sizeof(tmp));
	for(int i=0; i<16; i++)
		sprintf(tmp, "%s%02x", tmp, digest_[i]);
	ret = tmp;
	return ret;
}

std::string c_md5_creator::make( const unsigned char* buf , size_t sz , unsigned char digest[16] )
{
	std::string ret;
	unsigned int len = sz;
	m_context.count[0] = m_context.count[1] = 0;
	m_context.state[0] = 0x67452301;
	m_context.state[1] = 0xefcdab89;
	m_context.state[2] = 0x98badcfe;
	m_context.state[3] = 0x10325476;
	_update((unsigned char*)buf, len);
	_final(digest);
	char tmp[256];
	memset(tmp, 0 , sizeof(tmp));
	for(int i=0; i<16; i++)
		sprintf(tmp, "%s%02x", tmp, digest[i]);
	ret = tmp;
	return ret;
}

//--------------------------------------------------------
void c_md5_creator::_update(unsigned char* input_, unsigned int inputlen_)
{
	unsigned int i=0;
	unsigned int index = 0;
	unsigned int partlen = 0;
	index = (unsigned int)( (m_context.count[0] >> 3) & 0x3F );
	if((m_context.count[0] += ((unsigned int)inputlen_ << 3)) < ((unsigned int)inputlen_ << 3))
		m_context.count[1]++;
	m_context.count[1] += ((unsigned int)inputlen_ >> 29);
	partlen = 64 - index;
	if(inputlen_ >= partlen)
	{
		memcpy(m_context.buffer+index, input_, partlen);
		_transform(m_context.state, m_context.buffer);
		for(i=partlen; i+63 < inputlen_; i+=64)
			_transform(m_context.state, input_+i);
		index = 0;
	}
	else 
		i = 0;
	memcpy(m_context.buffer+index, input_+i, inputlen_-i);
}
//--------------------------------------------------------
void c_md5_creator::_final(unsigned char digest_[16])
{
	unsigned char bits[8];
	unsigned int index = 0;
	unsigned int padlen;
	_encode(bits, m_context.count, 8);
	index = (unsigned int)((m_context.count[0] >> 3) & 0x3f);
	padlen = (index < 56) ? (56-index) : (120-index);
	_update(internal::_padding, padlen);
	_update(bits, 8);
	_encode(digest_, m_context.state, 16);
	memset(&m_context, 0, sizeof(s_ctx));
}
//--------------------------------------------------------
void c_md5_creator::_transform(unsigned int state_[4], unsigned char block_[64])
{
	unsigned int a = state_[0], b = state_[1], c = state_[2], d = state_[3];
	unsigned int x[16];
	_decode (x, block_, 64);
	_ff (a, b, c, d, x[ 0], 7, 0xd76aa478);
	_ff (d, a, b, c, x[ 1], 12, 0xe8c7b756);
	_ff (c, d, a, b, x[ 2], 17, 0x242070db);
	_ff (b, c, d, a, x[ 3], 22, 0xc1bdceee);
	_ff (a, b, c, d, x[ 4], 7, 0xf57c0faf);
	_ff (d, a, b, c, x[ 5], 12, 0x4787c62a);
	_ff (c, d, a, b, x[ 6], 17, 0xa8304613);
	_ff (b, c, d, a, x[ 7], 22, 0xfd469501);
	_ff (a, b, c, d, x[ 8], 7, 0x698098d8);
	_ff (d, a, b, c, x[ 9], 12, 0x8b44f7af);
	_ff (c, d, a, b, x[10], 17, 0xffff5bb1);
	_ff (b, c, d, a, x[11], 22, 0x895cd7be);
	_ff (a, b, c, d, x[12], 7, 0x6b901122);
	_ff (d, a, b, c, x[13], 12, 0xfd987193);
	_ff (c, d, a, b, x[14], 17, 0xa679438e);
	_ff (b, c, d, a, x[15], 22, 0x49b40821);
	_gg (a, b, c, d, x[ 1], 5, 0xf61e2562);
	_gg (d, a, b, c, x[ 6], 9, 0xc040b340);
	_gg (c, d, a, b, x[11], 14, 0x265e5a51);
	_gg (b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
	_gg (a, b, c, d, x[ 5], 5, 0xd62f105d);
	_gg (d, a, b, c, x[10], 9,  0x2441453);
	_gg (c, d, a, b, x[15], 14, 0xd8a1e681);
	_gg (b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
	_gg (a, b, c, d, x[ 9], 5, 0x21e1cde6);
	_gg (d, a, b, c, x[14], 9, 0xc33707d6);
	_gg (c, d, a, b, x[ 3], 14, 0xf4d50d87);
	_gg (b, c, d, a, x[ 8], 20, 0x455a14ed);
	_gg (a, b, c, d, x[13], 5, 0xa9e3e905);
	_gg (d, a, b, c, x[ 2], 9, 0xfcefa3f8);
	_gg (c, d, a, b, x[ 7], 14, 0x676f02d9);
	_gg (b, c, d, a, x[12], 20, 0x8d2a4c8a);
	_hh (a, b, c, d, x[ 5], 4, 0xfffa3942);
	_hh (d, a, b, c, x[ 8], 11, 0x8771f681);
	_hh (c, d, a, b, x[11], 16, 0x6d9d6122);
	_hh (b, c, d, a, x[14], 23, 0xfde5380c);
	_hh (a, b, c, d, x[ 1], 4, 0xa4beea44);
	_hh (d, a, b, c, x[ 4], 11, 0x4bdecfa9);
	_hh (c, d, a, b, x[ 7], 16, 0xf6bb4b60);
	_hh (b, c, d, a, x[10], 23, 0xbebfbc70);
	_hh (a, b, c, d, x[13], 4, 0x289b7ec6);
	_hh (d, a, b, c, x[ 0], 11, 0xeaa127fa);
	_hh (c, d, a, b, x[ 3], 16, 0xd4ef3085);
	_hh (b, c, d, a, x[ 6], 23,  0x4881d05);
	_hh (a, b, c, d, x[ 9], 4, 0xd9d4d039);
	_hh (d, a, b, c, x[12], 11, 0xe6db99e5);
	_hh (c, d, a, b, x[15], 16, 0x1fa27cf8);
	_hh (b, c, d, a, x[ 2], 23, 0xc4ac5665);
	_ii (a, b, c, d, x[ 0], 6, 0xf4292244);
	_ii (d, a, b, c, x[ 7], 10, 0x432aff97);
	_ii (c, d, a, b, x[14], 15, 0xab9423a7);
	_ii (b, c, d, a, x[ 5], 21, 0xfc93a039);
	_ii (a, b, c, d, x[12], 6, 0x655b59c3);
	_ii (d, a, b, c, x[ 3], 10, 0x8f0ccc92);
	_ii (c, d, a, b, x[10], 15, 0xffeff47d);
	_ii (b, c, d, a, x[ 1], 21, 0x85845dd1);
	_ii (a, b, c, d, x[ 8], 6, 0x6fa87e4f);
	_ii (d, a, b, c, x[15], 10, 0xfe2ce6e0);
	_ii (c, d, a, b, x[ 6], 15, 0xa3014314);
	_ii (b, c, d, a, x[13], 21, 0x4e0811a1);
	_ii (a, b, c, d, x[ 4], 6, 0xf7537e82);
	_ii (d, a, b, c, x[11], 10, 0xbd3af235);
	_ii (c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
	_ii (b, c, d, a, x[ 9], 21, 0xeb86d391);
	state_[0] += a;
	state_[1] += b;
	state_[2] += c;
	state_[3] += d;
	memset(x, 0, sizeof(x));
}
//--------------------------------------------------------
void c_md5_creator::_encode(unsigned char* output_, unsigned int* input_, unsigned int len_)
{
	unsigned int i, j;
	for (i=0, j=0; j<len_; i++, j+=4)
	{
		output_[j] = (unsigned char)(input_[i] & 0xff);
		output_[j+1] = (unsigned char)((input_[i] >> 8) & 0xff);
		output_[j+2] = (unsigned char)((input_[i] >> 16) & 0xff);
		output_[j+3] = (unsigned char)((input_[i] >> 24) & 0xff);
	}
}
//--------------------------------------------------------
void c_md5_creator::_decode(unsigned int* output_, unsigned char* input_, unsigned int len_)
{
	unsigned int i, j;
	for (i=0, j=0; j<len_; i++, j+=4)
		output_[i] = ((unsigned int)input_[j]) | (((unsigned int)input_[j+1]) << 8) |	(((unsigned int)input_[j+2]) << 16) | (((unsigned int)input_[j+3]) << 24);
}
//--------------------------------------------------------
unsigned int c_md5_creator::_f(unsigned int x_, unsigned int y_, unsigned int z_)
{
	return (((x_) & (y_)) | ((~x_) & (z_)));
}
//--------------------------------------------------------
unsigned int c_md5_creator::_g(unsigned int x_, unsigned int y_, unsigned int z_)
{
	return (((x_) & (z_)) | ((y_) & (~z_)));
}
//--------------------------------------------------------
unsigned int c_md5_creator::_h(unsigned int x_, unsigned int y_, unsigned int z_)
{
	return ((x_) ^ (y_) ^ (z_));
}
//--------------------------------------------------------
unsigned int c_md5_creator::_i(unsigned int x_, unsigned int y_, unsigned int z_)
{
	return ((y_) ^ ((x_) | (~z_)));
}
//--------------------------------------------------------
void c_md5_creator::_ff(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_)
{
	a_ += _f(b_, c_, d_) + (x_) + (unsigned int)(ac_);
	a_ = _rotate_left(a_, s_);
	a_ += b_;
}
//--------------------------------------------------------
void c_md5_creator::_gg(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_)
{
	a_ += _g(b_, c_, d_) + (x_) + (unsigned int)(ac_);
	a_ = _rotate_left(a_, s_); 
	a_ += b_;
}
//--------------------------------------------------------
void c_md5_creator::_hh(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_)
{
	a_ += _h(b_, c_, d_) + (x_) + (unsigned int)(ac_);
	a_ = _rotate_left(a_, s_); 
	a_ += b_;
}
//--------------------------------------------------------
void c_md5_creator::_ii(unsigned int& a_, unsigned int b_, unsigned int c_, unsigned int d_, unsigned int x_, unsigned int s_, unsigned int ac_)
{
	a_ += _i(b_, c_, d_) + (x_) + (unsigned int)(ac_);
	a_ = _rotate_left(a_, s_); 
	a_ += b_;
}
//--------------------------------------------------------
unsigned int c_md5_creator::_rotate_left(unsigned int x_, unsigned int n_)
{
	return (((x_) << (n_)) | ((x_) >> (32-(n_))));
}