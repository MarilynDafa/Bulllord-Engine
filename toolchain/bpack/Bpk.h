#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "CMap.h"
#include "Stream.h"
#include "patch.h"
#pragma pack(push)
#pragma pack(1)


struct s_bpk_file_header
{
	unsigned int fourcc;
	unsigned int version;
	unsigned int file_num;
	unsigned int entry_offset;
	unsigned int context_offset;
};
struct s_bpk_file_entry
{
	unsigned int hashname;
	unsigned int offset;
	unsigned int length;
	char md5[32];
	bool operator <(const s_bpk_file_entry& rhs) const
	{
		return hashname < rhs.hashname;
	}
};

class Patchnode
{
public:
	PatchEntry header;
	c_stream stream;
};
struct sdata
{
	size_t csz;
	size_t osz;
	unsigned char* mem;
};


#pragma pack(pop)


bool _is_header_valid(const s_bpk_file_header& header);
bool scan_local_header( FILE* fp , s_bpk_file_header& header , std::map<unsigned int , s_bpk_file_entry>& files);



#define FOREACH(ref , var , mother) for(ref var = mother.begin() ; var != mother.end() ; ++var)

#define RFOREACH(ref , var , mother) for(ref var = mother.rbegin() ; var != mother.rend() ; --var)

#define TYPEOF(...) __VA_ARGS__

#define AUTO(ref , var , exp) ref var = exp