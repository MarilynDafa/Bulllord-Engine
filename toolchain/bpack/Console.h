#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include "patch.h"
#include "Bpk.h"
#include "Stream.h"
#include "Md5.h"
#include "bpackage.h"

enum e_console_result
{
	CR_VOID_CMD,
	CR_COMMAND_SUC,
	CR_COMMAND_FAIL,
	CR_PRINT_HELP,
	CR_INVALID_CMD
};
class Console
{
public:
	Console(){}
	bool processCmd(const char* cmd);
	void error(const std::string& context_);
	void print_info(const std::string& context_);
	bool _exec_command(const std::vector<std::string>& args_);
	void _pack(const std::string& filename, const std::string& version, const std::vector<std::string>& files);
	//bool patch(const char* orifile, const char* patchname_, const char* destfile);
	int _queryBPK(const std::string& filename);
	unsigned int loadStream(const std::string& src, std::map<s_bpk_file_entry, c_stream>& mem);
	void loadPathStream(const std::string& path, std::map<s_bpk_file_entry, c_stream>& mem);
	void _writePatch(const std::string& filename, PatchHeader header);
	void _queryPatch(const std::string& filename);
	bool check(unsigned int val, const std::string& name);
	void writeListFile(const std::string& name);
	void unpack(const std::string& name);
private:
	std::string localpath;
	std::vector<Patchnode> addList;
	std::vector<Patchnode> modList;
	std::vector<PatchEntry> delList;
	std::map<unsigned int, size_t> m_size1;
	std::map<unsigned int, size_t> m_size2;
	std::map<unsigned int, std::string> _checklist;
};