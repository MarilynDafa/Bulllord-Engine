#pragma once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
const unsigned char RGBA_identifier[] = { 0xDD, 0xDD, 0xDD, 0xEE, 0xEE, 0xEE, 0xAA, 0xAA, 0xAA, 0xDD, 0xDD, 0xDD };

#define FOUR_CC(ch0, ch1, ch2, ch3) ((ch0<<0) + (ch1<<8) + (ch2<<16) + (ch3<<24))

struct SpriteSheet
{
	int taglen;
	char tag[256];
	int ltx, lty, rbx, rby;
	int offsetx, offsety;
};
struct BMG_header_TT
{
	unsigned char identifier[12];
	unsigned int pixelWidth;
	unsigned int pixelHeight;
	unsigned int pixelDepth;
	unsigned int numberOfArrayElements;
	unsigned int numberOfFaces;
	unsigned int numberOfMipmapLevels;
	unsigned int fourcc;
	unsigned int channels;
	unsigned int offset;
	BMG_header_TT()
	{
		offset = 0;
		pixelWidth = 1;
		pixelHeight = 1;
		pixelDepth = 1;
		numberOfArrayElements = 1;
		numberOfFaces = 1;
		numberOfMipmapLevels =1;
		memcpy(&identifier , RGBA_identifier, sizeof(RGBA_identifier));
		//! for bmg 'B' 'M' 'G' 'T'
		//! for S3TC 'S' '3' 'T' 'C'
		//! for ASTC 'A' 'S' 'T' 'C'
		//! for ETC2 'E' 'T' 'C' '2'
		fourcc = FOUR_CC('B', 'M', 'G', 'T');
		channels = 4;
	}
};

struct Data_block
{
	std::vector<unsigned char*> Data;	
	std::vector<unsigned char*> CompressData;
};

enum Texture_target
{
	TT_1D , 
	TT_2D , 
	TT_3D , 
	TT_CUBE
};

enum Alpha_type
{
	AT_NONE_ALPHA ,
	AT_SAMPLE_ALPHA , 
	AT_COMPLEX_ALPHA
};

enum CompressMethord
{
	CMD_NONE, 
CM_S3TC, 
CM_ETC2,
CM_ASTC,
};


