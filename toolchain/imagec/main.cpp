#include "stb_image.h"
#include "stb_image_write.h"
#include "astc/astc_toplevel.cpp"
#include "src/webp/encode.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
const unsigned char rgbaidentifier[] = { 0xDD, 0xDD, 0xDD, 0xEE, 0xEE, 0xEE, 0xAA, 0xAA, 0xAA, 0xDD, 0xDD, 0xDD };
#define FOUR_CC(ch0, ch1, ch2, ch3) ((ch0<<0) + (ch1<<8) + (ch2<<16) + (ch3<<24))
#define TEXTURE_1D 0
#define TEXTURE_2D 1
#define TEXTURE_3D 2
#define TEXTURE_CUBE 3

struct BMGHeader
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
	BMGHeader()
	{
		offset = 0;
		pixelWidth = 1;
		pixelHeight = 1;
		pixelDepth = 1;
		numberOfArrayElements = 1;
		numberOfFaces = 1;
		numberOfMipmapLevels = 1;
		memcpy(&identifier, rgbaidentifier, sizeof(rgbaidentifier));
		fourcc = FOUR_CC('B', 'M', 'G', 'T');
		channels = 4;
	}
};

struct DataBlock
{
	std::vector<unsigned char*> Data;
	std::vector<unsigned char*> CompressData;
	std::vector<size_t> Length;
};

struct SpriteSheet
{
	int taglen;
	char tag[256];
	int ltx, lty, rbx, rby;
	int offsetx, offsety;
};

bool hasAlpha(unsigned char* data, size_t dataSize)
{
	int record[256];
	memset(record, 0, sizeof(int) * 256);
	for (int i = 0; i < dataSize; i++)
	{
		record[data[i * 4 + 3]] = 1;
	}
	int sum = 0;
	for (int i = 0; i < 256; i++)
		sum += record[i];
	if (sum > 2)
		return true;
	else if (sum == 2)
	{
		if (record[255] == 1 && record[0] == 1)
			return true;
		else
			return true;
	}
	else if (sum == 1)
	{
		if (record[255] == 1)
			return false;
		else
			return true;
	}
}
int compressAstc(const char* filename , bool& hasalpha, DataBlock& datas, bool mipmap)
{
	std::string tmpfile = filename;
	tmpfile += ".astc";
	std::string tmpfile2 = filename;
	int w, h, comp;
	stbi_uc* data = stbi_load(filename, &w, &h, &comp, 4);
	hasalpha = hasAlpha(data, w*h);
	tmpfile2 += "t.png";
	ILuint img = ilGenImage();
	ilBindImage(img);
	ilLoadDataL(data,  4*w*h, w, h, 1, 4);

	if (mipmap && iluBuildMipmaps())
	{
		ILint numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);	
		for (int i = 0; i <= numMipmaps; i++)
		{
			ilBindImage(img);
			ilActiveMipmap(i);
			w = ilGetInteger(IL_IMAGE_WIDTH);
			h = ilGetInteger(IL_IMAGE_HEIGHT);
			stbi_flip_vertically_on_write(true);
			stbi_write_png(tmpfile2.c_str(), w, h, 4, ilGetData(), 4 * w);
			const char* argv[] = { "astcenc", "-c", tmpfile2.c_str(), tmpfile.c_str() ,"4x4", "-medium" };
			astcentry(6, argv);
			FILE* fp = fopen(tmpfile.c_str(), "rb");
			fseek(fp, 0, SEEK_END);
			size_t sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			sz = sz - sizeof(astc_header);
			unsigned char* buf = (unsigned char*)malloc(sz);
			fseek(fp, sizeof(astc_header), SEEK_SET);
			fread(buf, 1, sz, fp);
			fclose(fp);
			datas.CompressData.push_back(buf);
			datas.Data.push_back(NULL);
			remove(tmpfile.c_str());
			remove(tmpfile2.c_str());
			if (w <= 4 || h <= 4)
				break;
		}
		stbi_image_free(data);
		ilDeleteImage(img);
		return datas.CompressData.size();
	}
	else
	{
		stbi_flip_vertically_on_write(true);
		stbi_write_png(tmpfile2.c_str(), w, h, 4, data, 4 * w);
		stbi_image_free(data);
		const char* argv[] = { "astcenc", "-c", tmpfile2.c_str(), tmpfile.c_str() ,"4x4", "-medium" };
		astcentry(6, argv);
		FILE* fp = fopen(tmpfile.c_str(), "rb");
		fseek(fp, 0, SEEK_END);
		size_t sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		sz = sz - sizeof(astc_header);
		unsigned char* buf = (unsigned char*)malloc(sz);
		fseek(fp, sizeof(astc_header), SEEK_SET);
		fread(buf, 1, sz, fp);
		fclose(fp);
		datas.CompressData.push_back(buf);
		datas.Data.push_back(NULL);
		remove(tmpfile.c_str());
		remove(tmpfile2.c_str());
		return 1;
	}
}

int compressWebp(const char* filename, bool& hasalpha, DataBlock& datas, bool mipmap)
{
	int w, h, comp;
	stbi_uc* data = stbi_load(filename, &w, &h, &comp, 4);
	hasalpha = hasAlpha(data, w*h);
	stbi_image_free(data);
	data = stbi_load(filename, &w, &h, &comp, hasalpha ? 4 :3);
	ILuint img = ilGenImage();
	ilBindImage(img);
	ilLoadDataL(data, (hasalpha ? 4 : 3)*w*h, w, h, 1, (hasalpha ? 4 : 3));
	if (mipmap && iluBuildMipmaps())
	{
		uint8_t* out = NULL;
		ILint numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);
		for (int i = 0; i <= numMipmaps; i++)
		{
			ilBindImage(img);
			ilActiveMipmap(i);
			ILubyte* data2 = ilGetData();
			w = ilGetInteger(IL_IMAGE_WIDTH);
			h = ilGetInteger(IL_IMAGE_HEIGHT);
			size_t len = 0;
			if (!hasalpha)
				len = WebPEncodeRGB(data2, w, h, w * 3 * sizeof(char), 100, &out);
			else
				len = WebPEncodeRGBA(data2, w, h, w * 4 * sizeof(char), 100, &out);
			unsigned char* buf = (unsigned char*)malloc(len);
			memcpy(buf, out, len);
			datas.CompressData.push_back(buf);
			datas.Data.push_back(NULL);
			datas.Length.push_back(len);
		}
		stbi_image_free(data);
		WebPFree(out);
		ilDeleteImage(img);
		return datas.CompressData.size();
	}
	else
	{
		uint8_t* out;
		size_t len = 0;
		if (!hasalpha)
			len = WebPEncodeRGB(data, w, h, w * 3 * sizeof(char), 100, &out);
		else
			len = WebPEncodeRGBA(data, w, h, w * 4 * sizeof(char), 100, &out);
		unsigned char* buf = (unsigned char*)malloc(len);
		memcpy(buf, out, len);
		datas.CompressData.push_back(buf);
		datas.Data.push_back(NULL);
		datas.Length.push_back(len);
		stbi_image_free(data);
		WebPFree(out);
		return 1;
	}
}
void loadSheet(const char* filename, std::vector<SpriteSheet>& sheets, const char* imagefile, int& w, int& h)
{
	int comp;
	stbi_info(imagefile, &w, &h, &comp);
	FILE* fp = fopen(filename, "rb");
	if (fp)
	{
		SpriteSheet sheet;
		while (feof(fp))
		{
			fread(&sheet.taglen, 1, sizeof(int), fp);
			fread(&sheet.tag, sheet.taglen, sizeof(char), fp);
			fread(&sheet.ltx, 1, sizeof(int), fp);
			fread(&sheet.lty, 1, sizeof(int), fp);
			fread(&sheet.rbx, 1, sizeof(int), fp);
			fread(&sheet.rby, 1, sizeof(int), fp);
			fread(&sheet.offsetx, 1, sizeof(int), fp);
			fread(&sheet.offsety, 1, sizeof(int), fp);
			sheets.push_back(sheet);
		}
		fclose(fp);
	}
	else
	{
		SpriteSheet sheet;
		sheet.taglen = 7;
		strcpy(sheet.tag, "invalid");
		sheet.ltx = 0;
		sheet.lty = 0;
		sheet.offsetx = 0;
		sheet.offsety = 0;
		sheet.rbx = w;
		sheet.rby = h;
		sheets.push_back(sheet);
	}
}
void compressImage(BMGHeader& header, DataBlock& data, std::vector<SpriteSheet> ssv, int target, bool alpha, const char* filename, int mode)
{
	FILE* fp = fopen(filename, "wb");
	header.numberOfFaces = (target == TEXTURE_CUBE) ? 6 : 1;
	if (header.pixelHeight == 1)
	{
		target = TEXTURE_1D;
	}
	else
	{
		if (header.pixelDepth == 1)
		{
			if (header.numberOfFaces != 6)
				target = TEXTURE_2D;
			else
			{
				assert(header.pixelHeight == header.pixelWidth);
				target = TEXTURE_CUBE;
			}
		}
		else
			target = TEXTURE_3D;
	}
	header.channels = alpha ? 4 : 3;
	header.offset = sizeof(BMGHeader);
	for (int i = 0; i < ssv.size(); ++i)
	{
		header.offset += ssv[i].taglen * sizeof(char) + 7 * sizeof(int);
	}
	fwrite(&header, sizeof(BMGHeader), 1, fp);
	for (int i = 0; i < ssv.size(); ++i)
	{
		fwrite(&ssv[i].taglen, 1, sizeof(int), fp);
		fwrite(&ssv[i].tag, ssv[i].taglen, sizeof(char), fp);
		fwrite(&ssv[i].ltx, 1, sizeof(int), fp);
		fwrite(&ssv[i].lty, 1, sizeof(int), fp);
		fwrite(&ssv[i].rbx, 1, sizeof(int), fp);
		fwrite(&ssv[i].rby, 1, sizeof(int), fp);
		fwrite(&ssv[i].offsetx, 1, sizeof(int), fp);
		fwrite(&ssv[i].offsety, 1, sizeof(int), fp);
	}
	int blocksz = mode ? 16 : 0;
	int tmp = header.numberOfArrayElements;

	for (int idx = 0; idx < tmp; ++idx)
	{
		if (target == TEXTURE_CUBE)
		{
			for (int face = 0; face < 6; ++face)
			{
				int width = header.pixelHeight;
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					int index = (idx * 6 + face - 0) *header.numberOfMipmapLevels + level;
					switch (mode)
					{
					case 0:
					{
						//webp
						unsigned int datalen = data.Length[index];
						fwrite(&datalen, sizeof(int), 1, fp);
						fwrite(data.CompressData[index], sizeof(unsigned char), data.Length[index], fp);
					}
					break;
					default:
					{
						//astc
						int sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
						fwrite(data.CompressData[index], sizeof(unsigned char), sz, fp);
					}
					break;
					}
					width = max(width / 2, 1);
				}
			}
		}
		else if (target == TEXTURE_3D)
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;

			for (int i = 0; i < header.pixelDepth; ++i)
			{
				switch (mode)
				{
				case 0:
				{
					//webp
					unsigned int datalen = data.Length[i];
					fwrite(&datalen, sizeof(int), 1, fp);
					fwrite(data.CompressData[i], sizeof(unsigned char), data.Length[i], fp);
				}
				break;
				default:
				{
					//astc
					size_t sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
					size_t sizeWrite = fwrite(data.CompressData[i], sizeof(unsigned char), sz, fp);
				}
				break;
				}
			}
		}
		else
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;
			for (int level = 0; level < header.numberOfMipmapLevels; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				size_t sz;
				switch (mode)
				{
				case 0:
				{
					//webp
					unsigned int datalen = data.Length[index];
					fwrite(&datalen, sizeof(int), 1, fp);
					fwrite(data.CompressData[index], sizeof(unsigned char), data.Length[index], fp);
				}
				break;
				default:
				{
					//astc
					switch (target)
					{
					case TEXTURE_1D:
						sz = ((width + 3) / 4) * blocksz;
						break;
					case TEXTURE_2D:
						sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
						break;
					}
					size_t sizeWrite = fwrite(data.CompressData[index], sizeof(unsigned char), sz, fp);
				}
				break;
				}
				width = max(width / 2, 1);
				height = max(height / 2, 1);
				depth = max(depth / 2, 1);
			}
		}
	}
	fclose(fp);
}

int main(int argc, char* argv[])
{
	ilInit();
	const char* type = argv[1];
	const char* target = argv[2];
	DataBlock datablock;
	int textarget = TEXTURE_2D;
	if (!strcmp(target, "1d"))
		textarget = TEXTURE_1D;
	else if (!strcmp(target, "2d"))
		textarget = TEXTURE_2D;
	else if (!strcmp(target, "3d"))
		textarget = TEXTURE_3D;
	else if (!strcmp(target, "cube"))
		textarget = TEXTURE_CUBE;
	int imagew, imageh, channels;

	bool genmip = false;
	const char* mip = argv[3];
	if (!strcmp(mip, "mipmap"))
		genmip = true;

	//load image data
	std::vector<std::string> filenames;
	for (int i = 5; i < argc - 1; ++i)
	{
		filenames.push_back(argv[i]);
	}

	//load sprite sheet
	std::vector<SpriteSheet> sheets;
	loadSheet(argv[4], sheets, filenames[0].c_str(), imagew, imageh);

	//texture array?
	bool hasarray = false;
	if (textarget == TEXTURE_1D && datablock.Data.size() > 1)
		hasarray = true;
	else if (textarget == TEXTURE_2D && datablock.Data.size() > 1)
		hasarray = true;
	
	printf("begin convert image format to bmg");

	//fill bmg header
	BMGHeader header;
	header.pixelWidth = imagew;
	header.pixelHeight = imageh;
	header.pixelDepth = (textarget == TEXTURE_3D) ? datablock.Data.size() : 1;
	header.numberOfArrayElements = hasarray ? datablock.Data.size() : 1;
	header.numberOfFaces = (textarget == TEXTURE_CUBE) ? 6 : 1;	
	int mode = 0;
	bool hasalpha = false;
	if (!strcmp(type, "webp"))
	{
		hasalpha = false;
		header.fourcc = FOUR_CC('B', 'M', 'G', 'T');
		mode = 0;
		bool alpha;
		for (int i = 0; i < filenames.size(); ++i)
		{
			header.numberOfMipmapLevels = compressWebp(filenames[i].c_str(), alpha, datablock, genmip);
			hasalpha = hasalpha | alpha;
		}
		header.channels = hasalpha ? 4 : 3;
	}
	else if (!strcmp(type, "astc"))
	{
		header.fourcc = FOUR_CC('A', 'S', 'T', 'C');
		mode = 1;
		bool alpha;
		for (int i = 0; i < filenames.size(); ++i)
		{
			header.numberOfMipmapLevels = compressAstc(filenames[i].c_str(), alpha, datablock, genmip);
			hasalpha = hasalpha | alpha;
		}
		header.channels = hasalpha ? 4 : 3;
	}
	compressImage(header, datablock, sheets, textarget, hasalpha, argv[argc - 1], mode);

	//free imagedata
	for (int i = 0; i < datablock.Data.size(); ++i)
	{
		if (datablock.Data[i])
			stbi_image_free(datablock.Data[i]);
		if (datablock.CompressData[i])
			stbi_image_free(datablock.CompressData[i]);
	}
	return 0;
}