#pragma once


#pragma pack(push)
#pragma pack(1)

struct PatchHeader
{
	unsigned int src_version;
	unsigned int dest_version;
	//!����б�ƫ����
	unsigned int  addlist_offset;
	unsigned int addlist_num;
	//!�޸��б�ƫ����
	unsigned int modlist_offset;
	unsigned int modlist_num;
	//!ɾ���б�ƫ����
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
