#pragma once
#include <QObject>
#include <QDebug>
#include <QtCore>
#include <vector>
#include <iostream>
#include "patch.h"
#include "Bpk.h"
#include "Stream.h"
#include <set>

struct node
{
	PatchEntry header;
	c_stream stream;
};
struct sdata
{
	size_t csz;
	size_t osz;
	unsigned char* mem;
};

#define IB_TAG_INTERNAL						0x0000
#define VB_TAG_INTERNAL						0x0001
#define UB_TAG_INTERNAL						0x0002
#define VD_TAG_INTERNAL						0x0003
#define DIB_TAG_INTERNAL					0x0004
#define DVB_TAG_INTERNAL					0x0005
#define SHADER_TAG_INTERNAL					0x0006
#define TECH_TAG_INTERNAL					0x0007
#define FBO_TAG_INTERNAL					0x0008
#define SAMPLER_TAG_INTERNAL				0x0009
#define TEX_TAG_INTERNAL					0x000A
#define OBJ_TAG_INTERNAL					0x000B
#define MODEL_TAG_INTERNAL					0x000C
#define AUDIO_TAG_INTERNAL					0x000D
#define UNC_TAG_INTERNAL					0x000E


class Console : public QObject
{
	Q_OBJECT
public:
	Console(QObject *parent = 0) : QObject(parent) {}
public slots :
	void run();
signals:
	void finished();
private:
	void printHelp();
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
	void queryFileStream(const char* name, c_stream& mem, size_t& sz2_, size_t& osz);
	void _queryPatch(const std::string& filename);
	bool check(unsigned int val, const std::string& name);
	void writeListFile(const std::string& name);
	void unpack(const std::string& name);
private:
	std::string localpath;
	std::vector<node> addList;
	std::vector<node> modList;
	std::vector<PatchEntry> delList;
	std::map<unsigned int, size_t> m_size1;
	std::map<unsigned int, size_t> m_size2;
	std::map<unsigned int, std::string> _checklist;
};