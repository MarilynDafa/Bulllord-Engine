#pragma once
#include <string>
#include <vector>
#include "Stream.h"
void trim(std::string& str);
unsigned int o2hash(const char*  _Str);
const std::string getEXName(const std::string& _Filename);
int tokenize(const std::string& str2, const std::string& whitespace_, std::vector<std::string>& outtokens_);
std::string queryFileMd5(const char* name);
unsigned char* queryFileData(const char* name, size_t& sz2_, size_t& osz);
void queryFileStream(const char* name, c_stream& mem, size_t& sz2_, size_t& osz);