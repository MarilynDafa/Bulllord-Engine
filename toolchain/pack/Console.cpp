#include "Console.h"
#include "Bpk.h"
#include <fstream>
#include "Md5.h"
#include "zlib.h"
#include "bpackage.h"








QFileInfoList GetFileList(QString path)
{
	QDir dir(path);
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i != folder_list.size(); i++)
	{
		QString name = folder_list.at(i).absoluteFilePath();
		QFileInfoList child_file_list = GetFileList(name);
		file_list.append(child_file_list);
	}

	return file_list;
}

static void queryFiles(const char* rootDir, std::vector<std::string>& strRet, const std::string& localpath)
{
	QFileInfoList xxx = GetFileList(rootDir);
	for (int i = 0; i != xxx.size(); i++)
	{
		strRet.push_back(xxx.at(i).absoluteFilePath().toStdString());
	}
	for (int i = 0; i != strRet.size(); i++)
	{
		strRet[i] = strRet[i].substr(localpath.length());
	}
}


void Console::run()
{
	char str[4096] = { 0 };
	printHelp();
	while (true)
	{
		memset(str, 0, sizeof(str));
		fgets(str, 4096, stdin);
		if (processCmd(str))
			break;
	}
	emit finished();
}

void Console::printHelp()
{
	printf("this is an bpk package tool, you can make package , patch and so on\n");
	printf("input -exec pack $folder$(dragable) $packagename$ $version$\n");
	printf("eg : -exec pack D:/test de.bpk 1.20\n");
	printf("input -exec patch $srcpackage$ $dstpackage$, build a patch\n");
	printf("eg : -exec patch io120.bpk io130.bpk\n");
	printf("input -exec query $packagename$, to show the information\n");
	printf("eg : -exec query io.bpk or -exec query patch150.pat\n");
	printf("input -help will show the help information\n");
	printf("input -exit will exit BPKPackper\n");
	printf("\n");
}


bool Console::processCmd(const char* cmd)
{
	e_console_result cr;
	std::string str_ = cmd;
	if (str_.length() == 1)
		cr = CR_VOID_CMD;
	std::vector<std::string> args;
	tokenize(str_, " ", args);
	for (int i = 0; i < args.size(); ++i)
		trim(args[i]);
	if (args[0] == "-exec")
	{
		cr = _exec_command(args) ? CR_COMMAND_SUC : CR_COMMAND_FAIL;
		return false;
	}
	else if (args[0] == "-help")
	{
		printHelp();
		cr = CR_PRINT_HELP;
		return false;
	}
	else if (args[0] == "-exit")
		return true;
	else
	{
		error("invalid command");
		cr = CR_INVALID_CMD;
		return false;
	}
}

void Console::print_info(const std::string& context_)
{
	printf(">INFO:%s\n", context_.c_str());
}

void Console::error(const std::string& context_)
{
	printf(">ERROR: %s\n", context_.c_str());
}

bool Console::_exec_command(const std::vector<std::string>& args_)
{
	if (args_[1] == "pack")
	{
		std::string dir = args_[2];
		if (dir.back() != '\\')
			dir.append("\\");
		std::vector<std::string> files;
		localpath = dir;
		queryFiles(dir.c_str(), files, localpath);
		if (args_.size() != 5)
		{
			error("arguments mismatch");
			return false;
		}
		if (files.empty())
		{
			error("no files or input path error");
			return false;
		}
		else
		{
			_pack(args_[3], args_[4], files);
		}
	}
	else if (args_[1] == "patch")
	{
		addList.clear();
		modList.clear();
		delList.clear();
		//!读取源包
		std::map<s_bpk_file_entry, o2d::c_stream> mem1;
		unsigned int sver = loadStream(args_[2], mem1);
		if (sver == 0xFFFFFFFF)
		{
			error("src package load failed.");
			return false;
		}


		//!读取目标包
		std::map<s_bpk_file_entry, o2d::c_stream> mem2;
		unsigned int dver = loadStream(args_[3], mem2);
		if (sver == dver)
		{
			error("the two package has the same version");
			return false;
		}

		if (sver > dver)
		{
			error("src package version is higher");
			return false;
		}

		//!开始打补丁
		PatchHeader header;
		header.src_version = sver;
		header.dest_version = dver;
		//!需要新增加的
		std::map<s_bpk_file_entry, o2d::c_stream>::iterator iterd = mem2.begin();
		std::map<s_bpk_file_entry, o2d::c_stream>::iterator iters;
		for (; iterd != mem2.end(); ++iterd)
		{
			const s_bpk_file_entry& en = iterd->first;
			iters = mem1.find(en);
			if (iters == mem1.end())
			{
				char buf[256];
				strcpy(buf, QString::number(iterd->first.hashname).toStdString().c_str());
				std::string tmp = "add ";
				tmp.append(buf);
				tmp.append(" to add list");
				print_info(tmp);
				//!TODO,添加到增加列表中
				node no;
				addList.push_back(no);
				node & not = addList.back();
				not.header.hashname = iterd->first.hashname;
				not.header.com_size = iterd->first.length;
				strncpy(not.header.md5, iterd->first.md5, 32);
				not.stream.reset(not.header.com_size);
				not.stream.write_buf(iterd->second.get_pointer(), not.stream.get_size());
				not.header.ori_size = m_size1[iterd->first.hashname];
			}
		}
		//!需要修改的
		iters = mem1.begin();
		for (; iters != mem1.end(); ++iters)
		{
			const s_bpk_file_entry& en = iters->first;
			iterd = mem2.find(en);
			if (iterd != mem2.end())
			{
				if (strncmp(iters->first.md5, iterd->first.md5, 32))
				{
					char buf[256];
					strcpy(buf, QString::number(iterd->first.hashname).toStdString().c_str());
					std::string tmp = "add ";
					tmp.append(buf);
					tmp.append(" to modify list");
					print_info(tmp);
					//!TODO.添加到修改列表中
					node no;
					modList.push_back(no);
					node & not = modList.back();
					not.header.hashname = iterd->first.hashname;
					not.header.com_size = iterd->first.length;
					not.stream = iterd->second;
					strncpy(not.header.md5, iterd->first.md5, 32);
					not.header.ori_size = m_size1[iterd->first.hashname];
				}
			}
			else
			{
				char buf[256];
				strcpy(buf, QString::number(iters->first.hashname).toStdString().c_str());
				std::string tmp = "add ";
				tmp.append(buf);
				tmp.append(" to delete list");
				print_info(tmp);
				//!TODO，添加到删除列表中
				PatchEntry en;
				en.hashname = iters->first.hashname;
				en.ori_size = 0;
				en.com_size = 0;
				delList.push_back(en);
			}
		}
		//! 开始往patch中写入
		char tmpbuf[32];
		sprintf(tmpbuf, "%d", sver);
		char tmpbuf2[32];
		sprintf(tmpbuf2, "%d", dver);
		_writePatch(std::string("patch") + std::string(tmpbuf) + "-" + std::string(tmpbuf2) + ".pat", header);
		//std::string xx = args_[2].substr(0, args_[2].size() - 4);
		//patch(args_[2].c_str(), (std::string("patch") + std::string(tmpbuf) + ".pat").c_str(), (xx + std::string(tmpbuf2) + ".bpk").c_str());


	}
	else if (args_[1] == "query")
	{
		if (args_.size() != 3)
		{
			error("arguments mismatch");
			return false;
		}
		if (args_[2].back() == 'k' || args_[2].back() == 'K')
			_queryBPK(args_[2]);
		else if (args_[2].back() == 't' || args_[2].back() == 'T')
			_queryPatch(args_[2]);
		else
			error("query invalid file");
	}
	else
	{
		error("invalid command");
		return false;
	}
	return true;
}

void Console::_pack(const std::string& filename, const std::string& version, const std::vector<std::string>& files)
{
	//!检查扩展名
	int len = filename.length();
	if (filename[len - 1] != 'k' ||
		filename[len - 2] != 'p' ||
		filename[len - 3] != 'b' ||
		filename[len - 4] != '.')
	{
		error("package ext name error.");
		return;
	}
	if (!(int)QString::fromUtf8(version.c_str()).toFloat())
	{
		error("version assign error.");
		return;
	}

	o2d::c_map<unsigned int, sdata> datas;
	o2d::c_map<unsigned int, s_bpk_file_entry> entries;
	FILE* fp = fopen(filename.c_str(), "wb");
	//!write header
	s_bpk_file_header header;
	unsigned char ch0 = 'B';
	unsigned char ch1 = 'P';
	unsigned char ch2 = 'K';
	unsigned char ch3 = '2';
	unsigned int hashval = (ch0 << 0) + (ch1 << 8) + (ch2 << 16) + (ch3 << 24);
	header.fourcc = hashval;
	double vv = QString::fromUtf8(version.c_str()).toDouble();
	header.version = (float)(vv*100.f);
	header.entry_offset = sizeof(s_bpk_file_header);
	header.context_offset = sizeof(s_bpk_file_header) + files.size() * sizeof(s_bpk_file_entry);
	header.file_num = files.size();
	fwrite(&header, sizeof(char), sizeof(header), fp);
	//!write file entry
	//size_t offset = header.context_offset;
	print_info("begin packing");
	print_info("loading...");
	fseek(fp, header.entry_offset, SEEK_SET);
	//std::vector<sdata> datas;
	for (int i = 0; i < files.size(); ++i)
	{
		std::string info("load file:");
		info.append(files[i]);
		print_info(info);
		s_bpk_file_entry entry;
		entry.hashname = o2hash(files[i].c_str());
		if (!check(entry.hashname, files[i]))
		{
			char hhjbuf[512] = { 0 };
			sprintf(hhjbuf, "find hash name conflict(%s)", files[i].c_str());
			error(hhjbuf);
			goto failed;
		}
		unsigned int csz;
		unsigned int osz;
		sdata data;
		data.mem = queryFileData((localpath + files[i]).c_str(), csz, osz);
		data.osz = osz;
		data.csz = entry.length = csz;
		//datas.push_back(data);		
		datas.insert(entry.hashname, data);
		entry.offset = 0;
		strncpy(entry.md5, queryFileMd5((localpath + files[i]).c_str()).c_str(), 32);
		//offset += (entry.length + sizeof(int));
		entries.insert(entry.hashname, entry);
		//fwrite(&entry , sizeof(entry) , 1 , fp);
	}
	size_t offset = header.context_offset;
	FOREACH(TYPEOF(o2d::c_map<unsigned int, s_bpk_file_entry>)::iterator, iter, entries)
	{
		iter->second().offset = offset;
		offset += (iter->second().length + sizeof(int));
	}
	//!write context
	FOREACH(TYPEOF(o2d::c_map<unsigned int, s_bpk_file_entry>)::iterator, iter, entries)
	{
		fwrite(&(iter->second()), sizeof(s_bpk_file_entry), 1, fp);
	}
	fseek(fp, header.context_offset, SEEK_SET);
	print_info("packing files...");
	FOREACH(TYPEOF(o2d::c_map<unsigned int, s_bpk_file_entry>)::iterator, iter, entries)
	{
		int i = iter->first();
		size_t sz = datas[i].osz;
		fwrite(&sz, 1, sizeof(int), fp);
		fwrite(datas[i].mem, sizeof(char), datas[i].csz, fp);
		free(datas[i].mem);
	}
	char hhjbuf[512] = { 0 };
	sprintf(hhjbuf, "operation succeed(%d files)", files.size());
	print_info(hhjbuf);
	fclose(fp);
	//writeListFile(filename);
	return;
failed:
	_checklist.clear();
	fclose(fp);
	print_info("operation failed");
}





int Console::_queryBPK(const std::string& filename)
{
	//!检查扩展名
	int len = filename.length();
	if (filename[len - 1] != 'k' ||
		filename[len - 2] != 'p' ||
		filename[len - 3] != 'b' ||
		filename[len - 4] != '.')
	{
		error("package ext name error.");
		return -1;
	}

	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp)
	{
		error("query file doesn`t exist.");
		return -1;
	}
	std::map<unsigned int, s_bpk_file_entry> files;
	s_bpk_file_header header;
	if (!scan_local_header(fp, header, files))
	{
		error("package invalid");
		return -1;
	}
	std::string tmp;
	tmp = "package name:";
	tmp.append(filename);
	print_info(tmp);
	print_info("fourcc: 'B' 'P' 'K' '2'");
	tmp = "version:";
	char buf[256];
	memset(buf, 0, 256 * sizeof(char));
	sprintf(buf, "%.2f", (float)header.version / 100.0);
	tmp.append(buf);
	print_info(tmp);
	tmp = "file num:";
	memset(buf, 0, 256 * sizeof(char));
	sprintf(buf, "%d", header.file_num);
	tmp.append(buf);
	print_info(tmp);

	//!查找到头
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* md5buf = (unsigned char*)malloc(sz);
	fread(md5buf, sizeof(unsigned char), sz, fp);
	c_md5_creator creator;
	unsigned char digest[16];
	std::string xx = creator.make(md5buf, sz, digest);
	free(md5buf);

	char buf1[512];
	memset(buf1, 0, 512 * sizeof(char));


	std::string sss;
	sss.append("md5: ");
	sss.append(xx);
	print_info(sss.c_str());



	fclose(fp);
	return header.version;
}


unsigned int Console::loadStream(const std::string& src, std::map<s_bpk_file_entry, o2d::c_stream>& mem)
{
	m_size1.clear();
	FILE* fp = fopen(src.c_str(), "rb");
	if (!fp)
	{
		error("can not open package");
		return -1;
	}
	std::map<unsigned int, s_bpk_file_entry> files;

	//!读取头
	s_bpk_file_header header;
	if (!scan_local_header(fp, header, files))
	{
		error("package invalid");
		return -1;
	}
	//!读取entry
	std::vector<s_bpk_file_entry>  entries;
	fseek(fp, header.entry_offset, SEEK_SET);
	for (int i = 0; i < header.file_num; i++)
	{
		s_bpk_file_entry entry;
		fread(&entry, 1, sizeof(s_bpk_file_entry), fp);
		entries.push_back(entry);
	}
	size_t szd = 0;
	//!读取内存stream
	for (int i = 0; i < entries.size(); ++i)
	{
		fseek(fp, entries[i].offset, SEEK_SET);
		unsigned int ssz;
		fread(&ssz, 1, sizeof(int), fp);
		m_size1[entries[i].hashname] = ssz;
		mem.insert(std::make_pair<s_bpk_file_entry, o2d::c_stream>(static_cast<s_bpk_file_entry&&>(entries[i]), o2d::c_stream()));
		unsigned char* buf = (unsigned char*)malloc(entries[i].length);
		fread(buf, 1, entries[i].length, fp);
		std::map<s_bpk_file_entry, o2d::c_stream>::iterator iter = mem.find(entries[i]);
		iter->second.resize(entries[i].length);
		iter->second.write_buf(buf, entries[i].length);
		free(buf);
		szd += entries[i].length;
	}

	fclose(fp);

	return header.version;
}
void Console::loadPathStream(const std::string& path, std::map<s_bpk_file_entry, o2d::c_stream>& mem)
{
	m_size2.clear();
	std::vector<std::string> files;

	std::string dir = path;
	if (dir.back() != '/')
		dir.append("/");
	localpath = dir;

	queryFiles(dir.c_str(), files, localpath);
	for (int i = 0; i < files.size(); ++i)
	{
		std::string info("load file:");
		info.append(files[i]);
		print_info(info);
		s_bpk_file_entry entry;
		entry.hashname = o2hash(files[i].c_str());
		unsigned int csz;
		unsigned int osz;
		entry.offset = 0;
		strncpy(entry.md5, queryFileMd5((localpath + files[i]).c_str()).c_str(), 32);
		o2d::c_stream tmp;
		queryFileStream((localpath + files[i]).c_str(), tmp, csz, osz);
		entry.length = csz;
		mem[entry] = tmp;
		m_size2[entry.hashname] = osz;
	}
}


void Console::_writePatch(const std::string& filename, PatchHeader header)
{
	int len = filename.length();
	if (filename[len - 1] != 't' ||
		filename[len - 2] != 'a' ||
		filename[len - 3] != 'p' ||
		filename[len - 4] != '.')
	{
		error("patch ext name error.");
		return;
	}

	print_info("begin write patch file");
	if (!addList.size() && !delList.size() && !modList.size())
	{
		error("no need patch");
		return;
	}
	FILE* fp = fopen(filename.c_str(), "wb");
	header.addlist_offset = sizeof(PatchHeader) + 0;
	header.addlist_num = addList.size();

	size_t sz = 0;
	for (int i = 0; i < addList.size(); ++i)
	{
		sz += (sizeof(PatchEntry) - sizeof(int));
	}
	header.modlist_offset = sizeof(PatchHeader) + sz;
	header.modlist_num = modList.size();

	size_t sz2 = 0;
	for (int i = 0; i < modList.size(); ++i)
	{
		sz2 += (sizeof(PatchEntry) - sizeof(int));
	}
	header.dellist_offset = sizeof(PatchHeader) + sz + sz2;
	header.dellist_num = delList.size();
	//!写入头
	fwrite(&header, 1, sizeof(header), fp);

	//!写入body
	int offset = sizeof(header) + (addList.size() + modList.size() + delList.size()) * (sizeof(PatchEntry) - sizeof(int));
	for (int i = 0; i < addList.size(); ++i)
	{
		addList[i].header.offset = offset;
		offset += (addList[i].header.com_size + sizeof(int));
	}
	for (int i = 0; i < modList.size(); ++i)
	{
		modList[i].header.offset = offset;
		offset += (modList[i].header.com_size + sizeof(int));
	}


	for (int i = 0; i < addList.size(); ++i)
	{
		fwrite(&addList[i].header, 1, sizeof(PatchEntry) - sizeof(int), fp);
		//fwrite(addList[i].stream.get_pointer(), 1, addList[i].stream.get_size(), fp);
	}
	for (int i = 0; i < modList.size(); ++i)
	{
		fwrite(&modList[i].header, 1, sizeof(PatchEntry) - sizeof(int), fp);
		//fwrite(modList[i].stream.get_pointer(), 1, modList[i].stream.get_size(), fp);
	}
	for (int i = 0; i < delList.size(); ++i)
	{
		fwrite(&delList[i], 1, sizeof(PatchEntry) - sizeof(int), fp);
	}

	////--=-=-=
	for (int i = 0; i < addList.size(); ++i)
	{
		fwrite(&addList[i].header.ori_size, 1, sizeof(int), fp);
		fwrite(addList[i].stream.get_pointer(), 1, addList[i].stream.get_size(), fp);
	}
	for (int i = 0; i < modList.size(); ++i)
	{
		fwrite(&modList[i].header.ori_size, 1, sizeof(int), fp);
		fwrite(modList[i].stream.get_pointer(), 1, modList[i].stream.get_size(), fp);
	}
	fclose(fp);
	print_info("patch succeed");
}



void Console::_queryPatch(const std::string& filename)
{
	//!检查扩展名
	int len = filename.length();
	if (filename[len - 1] != 't' ||
		filename[len - 2] != 'a' ||
		filename[len - 3] != 'p' ||
		filename[len - 4] != '.')
	{
		error("package ext name error.");
		return;
	}


	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp)
	{
		error("query file doesn`t exist.");
		return;
	}
	//!读取头
	PatchHeader header;
	fread(&header, 1, sizeof(PatchHeader), fp);

	char buf[256];

	memset(buf, 0, 256 * sizeof(char));
	sprintf(buf, "%.2f", (float)header.src_version / 100.0);
	std::string tmp = "src package version: ";
	tmp.append(buf);
	print_info(tmp);

	memset(buf, 0, 256 * sizeof(char));
	sprintf(buf, "%.2f", (float)header.dest_version / 100.0);
	tmp = "dest package version: ";
	tmp.append(buf);
	print_info(tmp);

	//!有添加列表
	if (header.addlist_num)
	{
		memset(buf, 0, 256 * sizeof(char));
		strcpy(buf, QString::number(header.addlist_num).toStdString().c_str());
		tmp = "this patch add ";
		tmp.append(buf);
		tmp.append(" files");
		print_info(tmp);
	}
	else
	{
		print_info("no file added");
	}

	//!有修改列表
	if (header.modlist_num)
	{
		memset(buf, 0, 256 * sizeof(char));
		strcpy(buf, QString::number(header.modlist_num).toStdString().c_str());
		tmp = "this patch modify ";
		tmp.append(buf);
		tmp.append(" files");
		print_info(tmp);
	}
	else
	{
		print_info("no file modified");
	}

	//!有删除列表
	if (header.dellist_num)
	{
		memset(buf, 0, 256 * sizeof(char));
		strcpy(buf, QString::number(header.dellist_num).toStdString().c_str());
		tmp = "this patch delete ";
		tmp.append(buf);
		tmp.append(" files");
		print_info(tmp);
	}
	else
	{
		print_info("no file delete");
	}
	fclose(fp);
}


bool Console::check(unsigned int val, const std::string& name)
{
	std::map<unsigned int, std::string>::iterator iter = _checklist.find(val);
	if (iter == _checklist.end())
	{
		_checklist[val] = name;
		return true;
	}
	else
		return false;
}

void Console::writeListFile(const std::string& name)
{
	_checklist.clear();
	return;
	std::string xxx = name;
	xxx = xxx.substr(0, xxx.length() - 3);
	xxx.append("listing");
	std::ofstream sout(xxx, std::ios::out);
	if (!sout.is_open())
	{
		error("cant not create listing file");
		return;
	}
	for (std::map<unsigned int, std::string>::iterator iter = _checklist.begin(); iter != _checklist.end(); ++iter)
	{
		sout << std::hex << iter->first;
		sout << " ";
		sout << iter->second;
		sout << std::endl;
	}
	sout.close();
}
