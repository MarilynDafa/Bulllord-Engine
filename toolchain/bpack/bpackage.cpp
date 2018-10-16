#include "bpackage.h"
#include "Md5.h"
#include "zlib/zlib.h"

void trim(std::string& str)
{
	char ch_ = ' ';
	for (int i = 0; i < str.length(); ++i)
	{
		char code = *(str.c_str() + i);
		if (code == ch_ || code == '\n' || code == '\r' || code == '\t')
			str.erase(0, 1);
		else
			break;
	}
	for (int i = str.length(); i >= 0; --i)
	{
		char code = *(str.c_str() + i - 1);
		if (code == ch_ || code == '\n' || code == '\r' || code == '\t')
			str.erase(str.length() - 1, 1);
		else
			break;
	}
}
void queryFileStream(const char* name, c_stream& mem, size_t& sz2_, size_t& osz)
{
	FILE* fp = fopen(name, "rb");
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buf = (unsigned char*)malloc(sz);
	fread(buf, sizeof(char), sz, fp);
	fclose(fp);

	uLongf sz2;
	Bytef* buf2 = (unsigned char*)malloc((sz + 12)*1.001);
	compress(buf2, &sz2, buf, sz);
	sz2_ = sz2;
	osz = sz;
	free(buf);
	mem.reset(sz2_);
	mem.write_buf(buf2, sz2_);
}
unsigned char* queryFileData(const char* name, size_t& sz2_, size_t& osz)
{
	FILE* fp = fopen(name, "rb");
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buf = (unsigned char*)malloc(sz);
	fread(buf, sizeof(char), sz, fp);
	fclose(fp);


	uLongf sz2 = (sz + 12)*1.001;
	Bytef* buf2 = (unsigned char*)malloc((sz + 12)*1.001);
	int rre = compress(buf2, &sz2, buf, sz);
	sz2_ = sz2;
	osz = sz;


	free(buf);
	return buf2;
}

std::string queryFileMd5(const char* name)
{
	//!test
	FILE* fp = fopen(name, "rb");
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buf = (unsigned char*)malloc(sz);
	fread(buf, 1, sz, fp);
	c_md5_creator md5;
	unsigned char digest[16];
	std::string md1 = md5.make(buf, sz, digest);
	fclose(fp);
	free(buf);
	return md1;
}

int tokenize(const std::string& str2, const std::string& whitespace_, std::vector<std::string>& outtokens_)
{
	std::string str = str2;
	char* ptr = (char*)str.c_str();
	char* token;
	while (nullptr != (token = strtok(ptr, whitespace_.c_str())))
	{
		outtokens_.push_back(token);
		ptr = nullptr;
	}
	return outtokens_.size();
}

const std::string getEXName(const std::string& _Filename)
{
	const char* _tmp = _Filename.c_str();
	unsigned long _len = 0;
	int _i;
	bool _periodfound = false;
	char* _ext;
	while (*_tmp++)
		_len++;
	_ext = (char*)_Filename.c_str();
	_ext += _len;
	for (_i = _len; _i >= 0; _i--)
	{
		if (*_ext == '.')
		{
			_periodfound = true;
			break;
		}
		_ext--;
	}
	if (!_periodfound)
		return NULL;
	return  std::string(_ext + 1);
}

unsigned int o2hash(const char*  _Str)
{
	const char* _stmp = _Str;
	unsigned int _len = 0;
	while (*_stmp++)
		_len++;
	char* _data = (char*)_Str;
	unsigned int _h = _len, _tmp;
	int _rem;
	if (_len <= 0 || _data == NULL)
		return 0;
	_rem = _len & 3;
	_len >>= 2;
	for (; _len > 0; _len--)
	{
		_h += *((const unsigned short*)_data);
		_tmp = (*((const unsigned short*)(_data + 2)) << 11) ^ _h;
		_h = (_h << 16) ^ _tmp;
		_data += 2 * sizeof(unsigned short);
		_h += _h >> 11;
	}
	switch (_rem)
	{
	case 3:
		_h += *((const unsigned short*)_data);
		_h ^= _h << 16;
		_h ^= ((char)_data[sizeof(unsigned short)]) << 18;
		_h += _h >> 11;
		break;
	case 2:
		_h += *((const unsigned short*)_data);
		_h ^= _h << 11;
		_h += _h >> 17;
		break;
	case 1:
		_h += (char)*_data;
		_h ^= _h << 10;
		_h += _h >> 1;
	}
	_h ^= _h << 3;
	_h += _h >> 5;
	_h ^= _h << 4;
	_h += _h >> 17;
	_h ^= _h << 25;
	_h += _h >> 6;

	/*
	std::string extname = getEXName(_Str);
	unsigned int _tag = 0;
	if (extname == "bmg")
	_tag = TEX_TAG_INTERNAL;
	else if (extname == "mp3")
	_tag = AUDIO_TAG_INTERNAL;
	else if (extname == "wav")
	_tag = AUDIO_TAG_INTERNAL;
	else if (extname == "bfx")
	_tag = UNC_TAG_INTERNAL;
	else if (extname == "bui")
	_tag = UNC_TAG_INTERNAL;
	else if (extname == "bmd")
	_tag = MODEL_TAG_INTERNAL;
	else if (extname == "bsk")
	_tag = MODEL_TAG_INTERNAL;
	else
	_tag = -1;*/

	return _h;
}
