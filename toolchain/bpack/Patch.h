#pragma once


#pragma pack(push)
#pragma pack(1)

struct PatchHeader
{
	unsigned int src_version;
	unsigned int dest_version;
	//!添加列表偏移量
	unsigned int  addlist_offset;
	unsigned int addlist_num;
	//!修改列表偏移量
	unsigned int modlist_offset;
	unsigned int modlist_num;
	//!删除列表偏移量
	unsigned int dellist_offset;
	unsigned int dellist_num;
};

struct PatchEntry
{
	unsigned int hashname;
	unsigned int offset;
	unsigned int com_size;
	char md5[32];
	unsigned int ori_size;
};

#pragma pack(pop)
