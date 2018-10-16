#include "Bpk.h"
#include "Patch.h"
bool _is_header_valid( const s_bpk_file_header& header )
{
	unsigned char ch0 = 'B';
	unsigned char ch1 = 'P';
	unsigned char ch2 = 'K';
	unsigned char ch3 = '2';
	unsigned int hashval = (ch0<<0) + (ch1<<8) + (ch2<<16) + (ch3<<24);
	return header.fourcc == hashval;
}

bool scan_local_header( FILE* fp , s_bpk_file_header& header , std::map<unsigned int , s_bpk_file_entry>& files)
{
	fread(&header, 1, sizeof(s_bpk_file_header), fp);
	if(!_is_header_valid(header))
		return false;	
	fseek(fp, header.entry_offset, SEEK_END);	
	const size_t numfiles = header.file_num;
	for(size_t i = 0; i < numfiles; i++)
	{
		s_bpk_file_entry entry;
		fread(&entry, 1, sizeof(s_bpk_file_entry), fp);
		files[entry.hashname] = entry;
	}
	return true;
}


