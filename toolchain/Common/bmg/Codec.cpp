
#include "Codec.h"
#include "stdio.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "etc2/encode/etc.h"
#include "etc2/decode/detex.h"
#include "etc2/encode/EtcFile.h"
#include "astc\astc_codec_internals.h"
#include "png\png.h"
#include "squish.h"
#include "mrle.h"
#include <algorithm>
#include <qimage.h>
#include <assert.h>
#define AS_HEADER_SIZE 0
void encode_s3tc(const BMG_header_TT& header , Data_block& data , Texture_target tt , int at)
{
	size_t sz = 0;
	int blocksz = (at != AT_COMPLEX_ALPHA)?8:16;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for(int idx = 0 ; idx < tmp ; ++idx)
	{
		if(tt == TT_CUBE)
		{
			for(int face = 0 ; face < 6 ; ++face)
			{
				int width = header.pixelWidth;
				//!对于每一层mipmap
				for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
				{
					sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
					int index = (idx * 6 + face - 0) * header.numberOfMipmapLevels + level;
					data.CompressData.push_back((unsigned char*)malloc(sz));
					squish::CompressImage(data.Data[index],width,width,data.CompressData[index],(at != AT_COMPLEX_ALPHA)?squish::kDxt1:squish::kDxt5);
					width = std::max(width / 2, 1);
				}
			}
		}
		else if(tt == TT_3D)
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;
			for(int i = 0; i < header.pixelDepth; ++i)
			{
				sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
				ILubyte *bb= new ILubyte[sz];
				squish::CompressImage(data.Data[i],width,height,bb,(at != AT_COMPLEX_ALPHA)?squish::kDxt1:squish::kDxt5);
				data.CompressData.push_back(bb);
			}
		}
		else
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				switch(tt)
				{
				case TT_1D:
					sz = ((width + 3) / 4) * blocksz;
					break;
				case TT_2D:
					sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
					break;
				}

				ILubyte *bb= new ILubyte[sz];
				squish::CompressImage(data.Data[index],width,height,bb,(at != AT_COMPLEX_ALPHA)?squish::kDxt1:squish::kDxt5);
				//ILubyte* bb = ilCompressDXT(data.Data[index] , width , height , depth , (at == AT_SAMPLE_ALPHA)?IL_DXT1:IL_DXT5 , &newsz);
				//assert(newsz == sz);

				data.CompressData.push_back(bb);
				width = std::max(width / 2, 1);
				height = std::max(height / 2, 1);
			}
		}
	}
}

void decode_s3tc(BMG_header_TT& header,Data_block& data,Texture_target& tt, int at)
{
	if(header.pixelHeight == 1)
	{
		tt = TT_1D;
	}
	else
	{
		if(header.pixelDepth ==1)
		{
			if(header.numberOfFaces != 6)
				tt = TT_2D;
			else
				tt =TT_CUBE;
		}
		else
			tt = TT_3D;
	}
	int flag;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for (int idx = 0; idx < tmp; ++ idx)
	{
		int width = header.pixelWidth;
		int height = header.pixelHeight;
		int depth = header.pixelDepth;
		switch (tt)
		{
		case TT_1D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					int index = idx * header.numberOfMipmapLevels + level;
					int flaggg;
					if(header.fourcc == FOUR_CC('S', '3', 'T', '1'))
						flaggg = squish::kDxt1;
					else if (header.fourcc == FOUR_CC('S', '3', 'T', '3'))
						flaggg = squish::kDxt5;
					int imagesz = ((width + 3) / 4) * ((squish::kDxt1 == flaggg)?8:16);
					data.Data.push_back((unsigned char*)malloc(4*width));
					squish::DecompressImage(data.Data[index] , width , height , data.CompressData[index] , flaggg);
					width = std::max(width / 2, 1);
				}
			}
			break;
		case TT_2D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				{
					int index = idx *header.numberOfMipmapLevels + level;
					int flaggg;
					if (header.fourcc == FOUR_CC('S', '3', 'T', '1'))
						flaggg = squish::kDxt1;
					else if (header.fourcc == FOUR_CC('S', '3', 'T', '3'))
						flaggg = squish::kDxt5;
					int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * ((squish::kDxt1 == flaggg) ? 8 : 16);
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					squish::DecompressImage(data.Data[index] , width , height , data.CompressData[index] , flaggg);
					width = std::max(width / 2, 1);
					height = std::max(height / 2, 1);
				}
			}
			break;
		case TT_3D:
			{
				//for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				//{
				//	int index = idx * header.numberOfMipmapLevels + level;
				//	int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * depth * ((0x83F1 == header.glInternalFormat)?8:16);
				//	data.Data.push_back((unsigned char*)malloc(4*width*height*depth));
				//	DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
				//	width = std::max(width / 2, 1);
				//	height = std::max(height / 2, 1);
				//	depth = std::max(depth / 2, 1);
				//}
				for(int d = 0; d < header.pixelDepth; ++d)
				{
					int index = idx *header.numberOfMipmapLevels + d;
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					int flaggg;
					if (header.fourcc == FOUR_CC('S', '3', 'T', '1'))
						flaggg = squish::kDxt1;
					else if (header.fourcc == FOUR_CC('S', '3', 'T', '3'))
						flaggg = squish::kDxt5;
					squish::DecompressImage(data.Data[index] , width , height , data.CompressData[index] , flaggg);
				}
			}
			break;
		case TT_CUBE:
			{
				for (int face = 0; face < 6; ++ face)
				{
					width = header.pixelWidth;
					height = header.pixelHeight;
					for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
					{
						int index = (idx * 6 + face) * header.numberOfMipmapLevels+ level;
						int flaggg;
						if (header.fourcc == FOUR_CC('S', '3', 'T', '1'))
							flaggg = squish::kDxt1;
						else if (header.fourcc == FOUR_CC('S', '3', 'T', '3'))
							flaggg = squish::kDxt5;
						int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * ((squish::kDxt1 == flaggg) ? 8 : 16);
						data.Data.push_back((unsigned char*)malloc(4*width*width));
						squish::DecompressImage(data.Data[index] , width , height , data.CompressData[index] , flaggg);
						width = std::max(width / 2, 1);
						height = std::max(height / 2, 1);
					}
				}
			}
			break;
		}
	}
}


bool ComEtc2(unsigned char* cimg_ , unsigned char* img_ , int width , int height, int at)
{
	unsigned int uiEncodingBitsBytes;
	unsigned int uiExtendedWidth;
	unsigned int uiExtendedHeight;
	int iEncodingTime_ms;
	static const unsigned int MIN_JOBS = 8;
	Etc::Image::Format format;
	if (at == AT_NONE_ALPHA)
		format = Etc::Image::Format::RGB8;// ETC2PACKAGE_RGB_NO_MIPMAPS;
	else if (at == AT_SAMPLE_ALPHA)
		format = Etc::Image::Format::RGB8A1;// ETC2PACKAGE_RGBA1_NO_MIPMAPS;
	else
		format = Etc::Image::Format::RGBA8;//ETC2PACKAGE_RGBA_NO_MIPMAPS;

	int m_uiWidth = width;
	int m_uiHeight = height;

	int iBlockX = 0;
	int iBlockY = 0;
	unsigned char* paucPixels = img_;


	Etc::ColorFloatRGBA* m_pafrgbaPixels = new Etc::ColorFloatRGBA[m_uiWidth * m_uiHeight];
	assert(m_pafrgbaPixels);
	bool bool16BitImage = 0;
	int iBytesPerPixel = bool16BitImage ? 8 : 4;
	unsigned char *pucPixel;	// = &paucPixels[(iBlockY * iWidth + iBlockX) * iBytesPerPixel];
	Etc::ColorFloatRGBA *pfrgbaPixel = m_pafrgbaPixels;

	// convert pixels from RGBA* to ColorFloatRGBA
	for (unsigned int uiV = iBlockY; uiV < (iBlockY + m_uiHeight); ++uiV)
	{
		// reset coordinate for each row
		pucPixel = &paucPixels[(uiV * width + iBlockX) * iBytesPerPixel];

		// read each row
		for (unsigned int uiH = iBlockX; uiH < (iBlockX + m_uiWidth); ++uiH)
		{
			if (bool16BitImage)
			{
				unsigned short ushR = (pucPixel[0] << 8) + pucPixel[1];
				unsigned short ushG = (pucPixel[2] << 8) + pucPixel[3];
				unsigned short ushB = (pucPixel[4] << 8) + pucPixel[5];
				unsigned short ushA = (pucPixel[6] << 8) + pucPixel[7];

				*pfrgbaPixel++ = Etc::ColorFloatRGBA((float)ushR / 65535.0f,
					(float)ushG / 65535.0f,
					(float)ushB / 65535.0f,
					(float)ushA / 65535.0f);
			}
			else
			{
				*pfrgbaPixel++ = Etc::ColorFloatRGBA::ConvertFromRGBA8(pucPixel[0], pucPixel[1],
					pucPixel[2], pucPixel[3]);
			}

			pucPixel += iBytesPerPixel;
		}
	}

	unsigned char* decimg;

	/*Etc::Encode((float *)m_pafrgbaPixels,
		width, height,
		format,
		Etc::ErrorMetric::RGBA,
		ETCCOMP_DEFAULT_EFFORT_LEVEL,
		MIN_JOBS,
		1024,
		&decimg, &uiEncodingBitsBytes,
		&uiExtendedWidth, &uiExtendedHeight,
		&iEncodingTime_ms);
	free(decimg);*/

	Etc::Image image((float *)m_pafrgbaPixels,
		width, height,
		Etc::ErrorMetric::BT709);
	Etc::Image::EncodingStatus encStatus = Etc::Image::EncodingStatus::SUCCESS;
	encStatus = image.Encode(format, Etc::ErrorMetric::BT709, ETCCOMP_DEFAULT_EFFORT_LEVEL, MIN_JOBS, 1024);
	memcpy(cimg_, image.GetEncodingBits(), image.GetEncodingBitsBytes());

	/*Etc::File etcfile("e://fdf.ktx", Etc::File::Format::INFER_FROM_FILE_EXTENSION,
		format,
		image.GetEncodingBits(), image.GetEncodingBitsBytes(),
		image.GetSourceWidth(), image.GetSourceHeight(),
		image.GetExtendedWidth(), image.GetExtendedHeight());

	etcfile.Write();*/
	return true;
}

bool DeETC2(unsigned char* cimg , unsigned char* img , int width , int height, int at)
{
	int  format;
	if (at == AT_NONE_ALPHA)
		format = DETEX_TEXTURE_FORMAT_ETC2;// ETC2PACKAGE_RGB_NO_MIPMAPS;
	else if (at == AT_SAMPLE_ALPHA)
		format = DETEX_TEXTURE_FORMAT_ETC2_PUNCHTHROUGH;// ETC2PACKAGE_RGBA1_NO_MIPMAPS;
	else
		format = DETEX_TEXTURE_FORMAT_ETC2_EAC;//ETC2PACKAGE_RGBA_NO_MIPMAPS;
	size_t size;
	detexTexture* tex = new detexTexture;
	tex->data = cimg;
	tex->format = format;
	tex->height = height;
	tex->width = width;
	tex->height_in_blocks = ((height + 3) / 4);
	tex->width_in_blocks = ((width + 3) / 4);
	detexDecompressTextureLinear(tex, img, DETEX_PIXEL_FORMAT_RGBA8);
	delete tex;
	return true;
}



void
blRLEDecode(unsigned char* const _Src, unsigned int _Dstlen, unsigned char* _Dst)
{
	unsigned char *_ip = (unsigned char*)_Src, *_op = _Dst;
	unsigned int _idx;
	int _c, _pc = -1;
	long long _t[256] = { 0 };
	long long _run = 0;
	for (_idx = 0; _idx < 32; ++_idx)
	{
		_c = *_ip++;
		for (unsigned int _jdx = 0; _jdx < 8; ++_jdx)
			_t[_idx * 8 + _jdx] = (_c >> _jdx) & 1;
	}
	while (_op < _Dst + _Dstlen)
	{
		_c = *_ip++;
		if (_t[_c])
		{
			for (_run = 0; (_pc = *_ip++) == 255; _run += 255);
			_run += _pc + 1;
			for (; _run > 0; --_run)
				*_op++ = _c;
		}
		else
			*_op++ = _c;
	}
}

bool ComBmg(unsigned char** cimg_, unsigned char* img_, int width, int height, int at)
{
	if (at == AT_NONE_ALPHA)
	{
		unsigned char* temp = (unsigned char*)malloc(width*height * 4 * 2 + 128);
		ILuint image = ilGenImage();
		ilBindImage(image);
		ilLoadDataL(img_, sizeof(ILubyte)* width * height * 4, width, height, 1, 4);
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		ILubyte* sd = ilGetData();
		ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		assert(dataSize == width * height * 3);
		int outlen = mrlec(sd, width*height * 3, temp);
		*cimg_ = (unsigned char*)malloc(outlen + sizeof(int));
		memcpy(*cimg_, &outlen, sizeof(int));
		memcpy(*cimg_ + sizeof(int), temp, outlen);
		free(temp);
		ilDeleteImage(image);
	}
	else
	{
		ILubyte* temp = (ILubyte*)malloc(width*height * 4 * 2 + 128);
		unsigned int outlen = mrlec(img_, width * height * 4, temp);
		*cimg_ = (unsigned char*)malloc(outlen + sizeof(int));
		memcpy(*cimg_, &outlen, sizeof(int));
		memcpy(*cimg_ + sizeof(int), temp, outlen);
		free(temp);
	}
	return true;
}




bool DeBmg(unsigned char* cimg, unsigned char* img, int width, int height, int at)
{
	if (at == AT_NONE_ALPHA)
	{
		unsigned char* temp = (unsigned char*)malloc(width*height * 3);
		int outlen = 3 * width * height;
		blRLEDecode(cimg + sizeof(int), outlen, temp);

		ILuint image = ilGenImage();
		ilBindImage(image);
		ilLoadDataL(temp, sizeof(ILubyte)* width * height * 3, width, height, 1, 3);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		ILubyte* sd = ilGetData();
		ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		assert(dataSize == width * height * 4);
		memcpy(img, sd, dataSize);
		free(temp);
		ilDeleteImage(image);
	}
	else
	{
		unsigned int outlen = 4 * width * height;
		blRLEDecode(cimg + sizeof(int), outlen, img);
	}

	return true;
}

bool DeBmgSkip(unsigned char* cimg, unsigned char* img, int width, int height, int at)
{
	if (at == AT_NONE_ALPHA)
	{
		unsigned char* temp = (unsigned char*)malloc(width*height * 3);
		int outlen = 3 * width * height;
		blRLEDecode(cimg, outlen, temp);

		ILuint image = ilGenImage();
		ilBindImage(image);
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		ilLoadDataL(temp, sizeof(ILubyte)* width * height * 3, width, height, 1, 3);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		ILubyte* sd = ilGetData();
		ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		assert(dataSize == width * height * 4);
		memcpy(img, sd, dataSize);
		free(temp);
		ilDeleteImage(image);
	}
	else
	{
		unsigned int outlen = 4 * width * height;
		blRLEDecode(cimg, outlen, img);
	}

	return true;
}


bool ComAstc(unsigned char* cimg_, unsigned char* img_, int width, int height, int at)
{

	//QImage tmp(img_, width, height, QImage::Format_RGBA8888);
	//tmp.save("e:\\xxxxx.png");

	if (at == AT_NONE_ALPHA)
	{
		char* argv[] = { "astcenc", "-c", "e:\\xxxxx.jpg", "e:\\temp.astc" ,"4x4", "-medium" };
		Create_ewpmain(6, argv, img_, cimg_, width, height, 3);
	}
	else
	{
		char* argv[] = { "astcenc", "-c", "e:\\xxxxx.png", "e:\\temp.astc" ,"4x4", "-medium" };
		Create_ewpmain(6, argv, img_, cimg_, width, height, 4);
	}


	return true;
}


bool DeAstc(unsigned char* cimg, unsigned char* img, int width, int height, int at)
{

	if (at == AT_NONE_ALPHA)
	{
		char* argv[] = { "astcenc", "-d", "e:\\temp.astc", "e:\\xxxxx.tga" };
		unsigned char* buf = (unsigned char*)malloc(width* height * 3);
		Create_ewpmain(4, argv, cimg, buf, width, height, (at == AT_NONE_ALPHA) ? 3 : 4);
		QImage mg(buf, width, height, QImage::Format_RGB888);
		QImage sd = mg.convertToFormat(QImage::Format_RGBA8888);
		free(buf);
		memcpy(img, sd.bits(), sd.byteCount());
	}
	else
	{
		char* argv[] = { "astcenc", "-d", "e:\\temp.astc", "e:\\xxxxx.tga" };
		Create_ewpmain(4, argv, cimg, img, width, height, 4);
	}


	return true;
}

bool CompressKTX( BMG_header_TT& header, Data_block& data, std::vector<SpriteSheet>ssv, Texture_target tt , Alpha_type at , const char* filename,CompressMethord cm )
{
	FILE* fp = fopen(filename , "wb");
	header.numberOfFaces = (tt == TT_CUBE)?6:1;
	if (header.pixelHeight == 1)
	{
		tt = TT_1D;
	}
	else
	{
		if (header.pixelDepth == 1)
		{
			if (header.numberOfFaces != 6)
				tt = TT_2D;
			else
			{
				assert(header.pixelHeight == header.pixelWidth);
				tt = TT_CUBE;
			}
		}
		else
			tt = TT_3D;
	}
	switch(cm)
	{
	case CMD_NONE:
		{
			if (at == AT_NONE_ALPHA)
			{
				header.fourcc = FOUR_CC('B', 'M', 'G', 'T');
				header.channels = 3;
			}
			else
			{
				header.fourcc = FOUR_CC('B', 'M', 'G', 'T');
				header.channels = 4;
			}
		}
		break;
	case CM_S3TC:
		{
			if (at == AT_COMPLEX_ALPHA)
			{
				header.fourcc = FOUR_CC('S', '3', 'T', '3');
				header.channels = 4;
			}
			else if (at == AT_SAMPLE_ALPHA)
			{
				header.fourcc = FOUR_CC('S', '3', 'T', '2');
				header.channels = 4;
			}
			else
			{
				header.fourcc = FOUR_CC('S', '3', 'T', '1');
				header.channels = 3;
			}
			encode_s3tc(header , data , tt , at);
		}
		break;
	case CM_ETC2:
		{
			if (at == AT_COMPLEX_ALPHA)
			{
				header.fourcc = FOUR_CC('E', 'T', 'C', '3');
				header.channels = 4;
			}
			else if (at == AT_SAMPLE_ALPHA)
			{
				header.fourcc = FOUR_CC('E', 'T', 'C', '2');
				header.channels = 4;
			}
			else
			{
				header.fourcc = FOUR_CC('E', 'T', 'C', '1');
				header.channels = 3;
			}
			encode_etc2(header , data , tt , at);
		}
		break;
	case CM_ASTC:
		{
			if (at == AT_COMPLEX_ALPHA)
			{
				header.fourcc = FOUR_CC('A', 'S', 'T', '3');
				header.channels = 4;
			}
			else if (at == AT_SAMPLE_ALPHA)
			{
				header.fourcc = FOUR_CC('A', 'S', 'T', '2');
				header.channels = 4;
			}
			else
			{
				header.fourcc = FOUR_CC('A', 'S', 'T', '1');
				header.channels = 3;
			}
			encode_astc(header , data , tt , at);
		}
		break;
	}
	header.offset = sizeof(BMG_header_TT);
	for (int i = 0; i < ssv.size(); ++i)
	{
		header.offset += ssv[i].taglen * sizeof(char) + 7 * sizeof(int);
	}
	//header.numberOfMipmapLevels -= 1;
	//!写文件头
	fwrite(&header , sizeof(BMG_header_TT) , 1  , fp);
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
	//!对于纹理阵列的每一层
	int blocksz;
	switch(cm)
	{
	case CM_S3TC:
		blocksz = (at != AT_COMPLEX_ALPHA)?8:16;
		break;
	case CM_ETC2:
		blocksz = (at != AT_COMPLEX_ALPHA) ? 8 : 16;
		break;
	case CM_ASTC:
		blocksz = 16;
		break;
	default:
		break;
	}
	int tmp = header.numberOfArrayElements;
	for(int idx = 0 ; idx < tmp ; ++idx)
	{
		//!立方纹理比较特殊
		if(tt == TT_CUBE)
		{
			//!对于cube 纹理的六个面
			for(int face = 0 ; face < 6 ; ++face)
			{
				int width = header.pixelHeight;
				//!对于每一层mipmap
				for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
				{
					int index = (idx * 6 + face - 0) *header.numberOfMipmapLevels + level;
					switch(cm)
					{
					case CMD_NONE:
						{
							size_t sz;
							memcpy(&sz, data.CompressData[index], sizeof(int));
							size_t sizeWrite = fwrite(data.CompressData[index], sizeof(unsigned char), sz + sizeof(int), fp);
						}
						break;
					case CM_S3TC:
						{
							int sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
							fwrite(data.CompressData[index] , sizeof(unsigned char) , sz , fp);
						}
						break;
					case CM_ETC2:
						{
							int sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
							fwrite(data.CompressData[index]+ AS_HEADER_SIZE , sizeof(unsigned char) , sz , fp);
						}
						break;
					case CM_ASTC:
						{
							int sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
							fwrite(data.CompressData[index] , sizeof(unsigned char) , sz , fp);
						}
						break;
					}
					width = std::max(width / 2, 1);
				}
			}
		}
		else if(tt == TT_3D)
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;

			for(int i = 0; i < header.pixelDepth; ++i)
			{
				switch(cm)
				{
				case CMD_NONE:
					{
						size_t sz;
						memcpy(&sz, data.CompressData[i], sizeof(int));
						size_t sizeWrite = fwrite(data.CompressData[i], sizeof(unsigned char), sz + sizeof(int), fp);
					}
					break;
				case CM_S3TC:
					{
						size_t sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
						size_t sizeWrite = fwrite(data.CompressData[i] , sizeof(unsigned char) , sz , fp);
					}
					break;
				case CM_ETC2:
					{
						size_t sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
						size_t sizeWrite = fwrite(data.CompressData[i] + AS_HEADER_SIZE , sizeof(unsigned char) , sz , fp);
					}
					break;
				case CM_ASTC:
					{
						size_t sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
						size_t sizeWrite = fwrite(data.CompressData[i] , sizeof(unsigned char) , sz , fp);
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
			//!对于每一层mipmap
			for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				size_t sz;
				switch(cm)
				{
				case CMD_NONE:
					{
						//sz = width * height * 4;
						memcpy(&sz, data.CompressData[index], sizeof(int));
						size_t sizeWrite = fwrite(data.CompressData[index] , sizeof(unsigned char) , sz +  sizeof(int) , fp);
					}
					break;
				case CM_S3TC:
					{
						switch(tt)
						{
						case TT_1D:
							sz = ((width + 3) / 4) * blocksz;
							break;
						case TT_2D:
							sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
							break;
						}
						size_t sizeWrite = fwrite(data.CompressData[index] , sizeof(unsigned char) , sz , fp);
					}
					break;
				case CM_ETC2:
					{
						switch(tt)
						{
						case TT_1D:
							sz = ((width + 3) / 4) * blocksz;
							break;
						case TT_2D:
							sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
							break;
						}
						size_t sizeWrite = fwrite(data.CompressData[index]+ AS_HEADER_SIZE , sizeof(unsigned char) , sz , fp);
					}
					break;
				case CM_ASTC:
					{
						switch(tt)
						{
						case TT_1D:
							sz = ((width + 3) / 4) * blocksz;
							break;
						case TT_2D:
							sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
							break;
						}
						size_t sizeWrite = fwrite(data.CompressData[index] , sizeof(unsigned char) , sz , fp);
					}
					break;
				}
				width = std::max(width / 2, 1);
				height = std::max(height / 2, 1);
				depth = std::max(depth / 2, 1);
			}
		}
	}
	fclose(fp);
	return true;
}

bool UnCompressKTX( const char* filename , Data_block& data , std::vector<SpriteSheet>& ssv, BMG_header_TT& header, Texture_target& tt)
{
	bool compressed = true;
	FILE* fp = fopen(filename , "rb");
	fread(&header , sizeof(BMG_header_TT) , 1 , fp);
	if(header.pixelHeight == 1)
	{
		tt = TT_1D;
	}
	else
	{
		if(header.pixelDepth ==1)
		{
			if(header.numberOfFaces != 6)
				tt = TT_2D;
			else
				tt =TT_CUBE;
		}
		else
		{
			tt = TT_3D;
			assert(header.numberOfMipmapLevels == 1);
		}
	}
	while (ftell(fp) < header.offset)
	{
		SpriteSheet ss;
		fread(&ss.taglen, 1, sizeof(int), fp);
		memset(ss.tag, 0, sizeof(ss.tag));
		fread(&ss.tag, ss.taglen, sizeof(char), fp);
		fread(&ss.ltx, 1, sizeof(int), fp);
		fread(&ss.lty, 1, sizeof(int), fp);
		fread(&ss.rbx, 1, sizeof(int), fp);
		fread(&ss.rby, 1, sizeof(int), fp);
		fread(&ss.offsetx, 1, sizeof(int), fp);
		fread(&ss.offsety, 1, sizeof(int), fp);
		ssv.push_back(ss);
	}
	assert(ftell(fp) == header.offset);
	fseek(fp, header.offset, SEEK_SET);
	int flag;
	for (int idx = 0; idx < header.numberOfArrayElements; ++ idx)
	{
		int width = header.pixelWidth;
		int height = header.pixelHeight;
		int depth = header.pixelDepth;
		switch (tt)
		{
		case TT_1D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					int index = idx * header.numberOfMipmapLevels+ level;
					int imagesz;
					int w = width;
					int h = height;
					switch (header.fourcc)
					{
					case FOUR_CC('B', 'M', 'G', 'T'):
						fread(&imagesz, 1, sizeof(int), fp);
						break;
					case FOUR_CC('S', '3', 'T', '1'):
						imagesz = ((w + 3) / 4) * 8;
						break;
					case FOUR_CC('S', '3', 'T', '2'):
						imagesz = ((w + 3) / 4) * 8;
						break;
					case FOUR_CC('S', '3', 'T', '3'):
						imagesz = ((w + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '1'):
						imagesz = ((w + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '2'):
						imagesz = ((w + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '3'):
						imagesz = ((w + 3) / 4) * 16;
						break;
					case FOUR_CC('E', 'T', 'C', '1'):
						imagesz = ((w + 3) / 4) * 8;
						break;
					case FOUR_CC('E', 'T', 'C', '2'):
						imagesz = ((w + 3) / 4) * 8;
						break;
					case FOUR_CC('E', 'T', 'C', '3'):
						imagesz = ((w + 3) / 4) * 16;
						break;
					default:assert(0);break;
					}
					data.CompressData.push_back((unsigned char*)malloc(imagesz*sizeof(unsigned char)));
					fread(data.CompressData[index], sizeof(unsigned char) , imagesz , fp);
					data.Data.push_back((unsigned char*)malloc(4*width));
					switch(header.fourcc)
					{
					case FOUR_CC('B', 'M', 'G', 'T'):
						DeBmgSkip(data.CompressData[index], data.Data[index], width, 1, (header.channels == 3)?AT_NONE_ALPHA:AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('S', '3', 'T', '1'):
						squish::DecompressImage(data.Data[index], width, 1, data.CompressData[index], squish::kDxt1);
						break;
					case FOUR_CC('S', '3', 'T', '2'):
						squish::DecompressImage(data.Data[index], width, 1, data.CompressData[index], squish::kDxt1);
						break;
					case FOUR_CC('S', '3', 'T', '3'):
						squish::DecompressImage(data.Data[index], width, 1, data.CompressData[index], squish::kDxt5);
						break;
					case FOUR_CC('A', 'S', 'T', '1'):
						DeAstc(data.CompressData[index], data.Data[index], width, 1, AT_NONE_ALPHA);
						break;
					case FOUR_CC('A', 'S', 'T', '2'):
						DeAstc(data.CompressData[index], data.Data[index], width, 1, AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('A', 'S', 'T', '3'):
						DeAstc(data.CompressData[index], data.Data[index], width, 1, AT_COMPLEX_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '1'):
						DeETC2(data.CompressData[index], data.Data[index], width, 1, AT_NONE_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '2'):
						DeETC2(data.CompressData[index], data.Data[index], width, 1, AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '3'):
						DeETC2(data.CompressData[index], data.Data[index], width, 1, AT_COMPLEX_ALPHA);
						break;
					default:assert(0);break;
					}	
					width = std::max(width / 2, 1);
				}
			}
			break;
		case TT_2D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				{
					int index = idx * header.numberOfMipmapLevels + level;
					int imagesz;
					int w = width;
					int h = height;
					switch (header.fourcc)
					{
					case FOUR_CC('B', 'M', 'G', 'T'):
						fread(&imagesz, 1, sizeof(int), fp);
						break;
					case FOUR_CC('S', '3', 'T', '1'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('S', '3', 'T', '2'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('S', '3', 'T', '3'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '1'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '2'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '3'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('E', 'T', 'C', '1'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('E', 'T', 'C', '2'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('E', 'T', 'C', '3'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					default:assert(0);break;
					}	
					data.CompressData.push_back((unsigned char*)malloc(imagesz*sizeof(unsigned char)));
					fread(data.CompressData[index], sizeof(unsigned char) , imagesz , fp);
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					switch (header.fourcc)
					{
					case FOUR_CC('B', 'M', 'G', 'T'):
						DeBmgSkip(data.CompressData[index], data.Data[index], width, height, (header.channels == 3) ? AT_NONE_ALPHA : AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('S', '3', 'T', '1'):
						squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt1);
						break;
					case FOUR_CC('S', '3', 'T', '2'):
						squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt1);
						break;
					case FOUR_CC('S', '3', 'T', '3'):
						squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt5);
						break;
					case FOUR_CC('A', 'S', 'T', '1'):
						DeAstc(data.CompressData[index], data.Data[index], width, height, AT_NONE_ALPHA);
						break;
					case FOUR_CC('A', 'S', 'T', '2'):
						DeAstc(data.CompressData[index], data.Data[index], width, height, AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('A', 'S', 'T', '3'):
						DeAstc(data.CompressData[index], data.Data[index], width, height, AT_COMPLEX_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '1'):
						DeETC2(data.CompressData[index], data.Data[index], width, height, AT_NONE_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '2'):
						DeETC2(data.CompressData[index], data.Data[index], width, height, AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '3'):
						DeETC2(data.CompressData[index], data.Data[index], width, height, AT_COMPLEX_ALPHA);
						break;
					default:assert(0);break;
					}	
					width = std::max(width / 2, 1);
					height = std::max(height / 2, 1);
				}
			}
			break;
		case TT_3D:
			{
				for(int d = 0; d < header.pixelDepth; ++d)
				{
					int index = idx + d;
					int imagesz;
					int w = width;
					int h = height;
					switch (header.fourcc)
					{
					case FOUR_CC('B', 'M', 'G', 'T'):
						fread(&imagesz, 1, sizeof(int), fp);
						break;
					case FOUR_CC('S', '3', 'T', '1'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('S', '3', 'T', '2'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('S', '3', 'T', '3'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '1'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '2'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('A', 'S', 'T', '3'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					case FOUR_CC('E', 'T', 'C', '1'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('E', 'T', 'C', '2'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
						break;
					case FOUR_CC('E', 'T', 'C', '3'):
						imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
						break;
					default:assert(0); break;
					}
					data.CompressData.push_back((unsigned char*)malloc(imagesz*sizeof(unsigned char)));
					fread(data.CompressData[index], sizeof(unsigned char) , imagesz , fp);
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					switch (header.fourcc)
					{
					case FOUR_CC('B', 'M', 'G', 'T'):
						DeBmgSkip(data.CompressData[index], data.Data[index], width, height, (header.channels == 3) ? AT_NONE_ALPHA : AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('S', '3', 'T', '1'):
						squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt1);
						break;
					case FOUR_CC('S', '3', 'T', '2'):
						squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt1);
						break;
					case FOUR_CC('S', '3', 'T', '3'):
						squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt5);
						break;
					case FOUR_CC('A', 'S', 'T', '1'):
						DeAstc(data.CompressData[index], data.Data[index], width, height, AT_NONE_ALPHA);
						break;
					case FOUR_CC('A', 'S', 'T', '2'):
						DeAstc(data.CompressData[index], data.Data[index], width, height, AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('A', 'S', 'T', '3'):
						DeAstc(data.CompressData[index], data.Data[index], width, height, AT_COMPLEX_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '1'):
						DeETC2(data.CompressData[index], data.Data[index], width, height, AT_NONE_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '2'):
						DeETC2(data.CompressData[index], data.Data[index], width, height, AT_SAMPLE_ALPHA);
						break;
					case FOUR_CC('E', 'T', 'C', '3'):
						DeETC2(data.CompressData[index], data.Data[index], width, height, AT_COMPLEX_ALPHA);
						break;
					default:assert(0); break;
					}	
				}
			}
			break;
		case TT_CUBE:
			{
				for (int face = 0; face < 6; ++ face)
				{
					width = header.pixelWidth;
					height = header.pixelHeight;
					for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
					{
						int index = (idx * 6 + face) * header.numberOfMipmapLevels + level;
						int imagesz;
						int w = width;
						int h = height;
						switch (header.fourcc)
						{
						case FOUR_CC('B', 'M', 'G', 'T'):
							fread(&imagesz, 1, sizeof(int), fp);
							break;
						case FOUR_CC('S', '3', 'T', '1'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
							break;
						case FOUR_CC('S', '3', 'T', '2'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
							break;
						case FOUR_CC('S', '3', 'T', '3'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
							break;
						case FOUR_CC('A', 'S', 'T', '1'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
							break;
						case FOUR_CC('A', 'S', 'T', '2'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
							break;
						case FOUR_CC('A', 'S', 'T', '3'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
							break;
						case FOUR_CC('E', 'T', 'C', '1'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
							break;
						case FOUR_CC('E', 'T', 'C', '2'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 8;
							break;
						case FOUR_CC('E', 'T', 'C', '3'):
							imagesz = ((w + 3) / 4) * ((h + 3) / 4) * 16;
							break;
						default:assert(0); break;
						}	
						data.CompressData.push_back((unsigned char*)malloc(imagesz*sizeof(unsigned char)));
						fread(data.CompressData[index], sizeof(unsigned char) , imagesz , fp);
						data.Data.push_back((unsigned char*)malloc(4*width*width));
						switch (header.fourcc)
						{
						case FOUR_CC('B', 'M', 'G', 'T'):
							DeBmgSkip(data.CompressData[index], data.Data[index], width, height, (header.channels == 3) ? AT_NONE_ALPHA : AT_SAMPLE_ALPHA);
							break;
						case FOUR_CC('S', '3', 'T', '1'):
							squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt1);
							break;
						case FOUR_CC('S', '3', 'T', '2'):
							squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt1);
							break;
						case FOUR_CC('S', '3', 'T', '3'):
							squish::DecompressImage(data.Data[index], width, height, data.CompressData[index], squish::kDxt5);
							break;
						case FOUR_CC('A', 'S', 'T', '1'):
							DeAstc(data.CompressData[index], data.Data[index], width, height, AT_NONE_ALPHA);
							break;
						case FOUR_CC('A', 'S', 'T', '2'):
							DeAstc(data.CompressData[index], data.Data[index], width, height, AT_SAMPLE_ALPHA);
							break;
						case FOUR_CC('A', 'S', 'T', '3'):
							DeAstc(data.CompressData[index], data.Data[index], width, height, AT_COMPLEX_ALPHA);
							break;
						case FOUR_CC('E', 'T', 'C', '1'):
							DeETC2(data.CompressData[index], data.Data[index], width, height, AT_NONE_ALPHA);
							break;
						case FOUR_CC('E', 'T', 'C', '2'):
							DeETC2(data.CompressData[index], data.Data[index], width, height, AT_SAMPLE_ALPHA);
							break;
						case FOUR_CC('E', 'T', 'C', '3'):
							DeETC2(data.CompressData[index], data.Data[index], width, height, AT_COMPLEX_ALPHA);
							break;
						default:assert(0); break;
						}	
						width = std::max(width / 2, 1);
						height = std::max(height / 2, 1);
					}
				}
			}
			break;
		}
	}
	fclose(fp);
	return true;
}



void startup()
{
	ilInit();
}



void encode_etc2( const BMG_header_TT& header , Data_block& data , Texture_target tt , int at )
{
	//rg_etc1::pack_etc1_block_init();
	//at = AT_NONE_ALPHA;
	size_t sz = 0;
	int blocksz = (at == AT_NONE_ALPHA)?8:16;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for(int idx = 0 ; idx < tmp ; ++idx)
	{
		if(tt == TT_CUBE)
		{
			for(int face = 0 ; face < 6 ; ++face)
			{
				int width = header.pixelWidth;
				//!对于每一层mipmap
				for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
				{
					sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz + AS_HEADER_SIZE;
					int index = (idx * 6 + face - 0) * header.numberOfMipmapLevels + level;
					data.CompressData.push_back((unsigned char*)malloc(sz));
					ComEtc2(data.CompressData[index] , data.Data[index], width , width , at);
					//CompressImage(data.Data[index],width,width,data.CompressData[index],(at == AT_SAMPLE_ALPHA)?squish::kDxt1:squish::kDxt5);
					width = std::max(width / 2, 1);
				}
			}
		}
		else if(tt == TT_3D)
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;
			for(int i = 0; i < header.pixelDepth; ++i)
			{
				sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz + AS_HEADER_SIZE;
				ILubyte *bb= new ILubyte[sz];
				ComEtc2(bb , data.Data[i], width , height, at);
				//CompressImage(data.Data[i],width,height,bb,(at == AT_SAMPLE_ALPHA)?squish::kDxt1:squish::kDxt5);
				data.CompressData.push_back(bb);
			}
		}
		else
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				switch(tt)
				{
				case TT_1D:
					sz = ((width + 3) / 4) * blocksz + AS_HEADER_SIZE;
					break;
				case TT_2D:
					sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz + AS_HEADER_SIZE;
					break;
				}

				ILubyte *bb= new ILubyte[sz];
				//CompressImage(data.Data[index],width,height,bb,(at == AT_SAMPLE_ALPHA)?squish::kDxt1:squish::kDxt5);
				//ILubyte* bb = ilCompressDXT(data.Data[index] , width , height , depth , (at == AT_SAMPLE_ALPHA)?IL_DXT1:IL_DXT5 , &newsz);
				//assert(newsz == sz);
				
				ComEtc2(bb , data.Data[index], width , height, at);

				data.CompressData.push_back(bb);
				width = std::max(width / 2, 1);
				height = std::max(height / 2, 1);
			}
		}
	}
}

void decode_etc2( BMG_header_TT& header,Data_block& data,Texture_target& tt, int at)
{
	if(header.pixelHeight == 1)
	{
		tt = TT_1D;
	}
	else
	{
		if(header.pixelDepth ==1)
		{
			if(header.numberOfFaces != 6)
				tt = TT_2D;
			else
				tt =TT_CUBE;
		}
		else
			tt = TT_3D;
	}
	int flag;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for (int idx = 0; idx < tmp; ++ idx)
	{
		int width = header.pixelWidth;
		int height = header.pixelHeight;
		int depth = header.pixelDepth;
		switch (tt)
		{
		case TT_1D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					int index = idx * header.numberOfMipmapLevels + level;
					//int imagesz = ((width + 3) / 4) * (8);
					data.Data.push_back((unsigned char*)malloc(4*width));
					DeETC2(data.CompressData[index] , data.Data[index] , width , height, at);
					//rg_etc1::unpack_etc1_block(data.CompressData[index] , (unsigned int*)data.Data[index]);
					//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
					width = std::max(width / 2, 1);
				}
			}
			break;
		case TT_2D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				{
					int index = idx *header.numberOfMipmapLevels + level;
					//int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * (8);
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					DeETC2(data.CompressData[index] , data.Data[index] , width , height, at);
					//rg_etc1::unpack_etc1_block(data.CompressData[index] , (unsigned int*)data.Data[index]);
					//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
					width = std::max(width / 2, 1);
					height = std::max(height / 2, 1);
				}
			}
			break;
		case TT_3D:
			{
				//for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				//{
				//	int index = idx * header.numberOfMipmapLevels + level;
				//	int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * depth * ((0x83F1 == header.glInternalFormat)?8:16);
				//	data.Data.push_back((unsigned char*)malloc(4*width*height*depth));
				//	DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
				//	width = std::max(width / 2, 1);
				//	height = std::max(height / 2, 1);
				//	depth = std::max(depth / 2, 1);
				//}
				for(int d = 0; d < header.pixelDepth; ++d)
				{
					int index = idx *header.numberOfMipmapLevels + d;
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					DeETC2(data.CompressData[index] , data.Data[index] , width , height, at);
					//rg_etc1::unpack_etc1_block(data.CompressData[index] , (unsigned int*)data.Data[index]);
					//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
				}
			}
			break;
		case TT_CUBE:
			{
				for (int face = 0; face < 6; ++ face)
				{
					width = header.pixelWidth;
					height = header.pixelHeight;
					for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
					{
						int index = (idx * 6 + face) * header.numberOfMipmapLevels+ level;
						//int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * (8);
						data.Data.push_back((unsigned char*)malloc(4*width*width));
						DeETC2(data.CompressData[index] , data.Data[index] , width , height, at);
						//rg_etc1::unpack_etc1_block(data.CompressData[index] , (unsigned int*)data.Data[index]);
						//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
						width = std::max(width / 2, 1);
						height = std::max(height / 2, 1);
					}
				}
			}
			break;
		}
	}
}




void encode_astc( const BMG_header_TT& header , Data_block& data , Texture_target tt , int at )
{
	size_t sz = 0;
	int blocksz = 16;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for(int idx = 0 ; idx < tmp ; ++idx)
	{
		if(tt == TT_CUBE)
		{
			for(int face = 0 ; face < 6 ; ++face)
			{
				int width = header.pixelWidth;
				//!对于每一层mipmap
				for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
				{
					sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
					int index = (idx * 6 + face - 0) * header.numberOfMipmapLevels + level;
					data.CompressData.push_back((unsigned char*)malloc(sz));
					ComAstc(data.CompressData[index] , data.Data[index], width, width, at);
					width = std::max(width / 2, 1);
				}
			}
		}
		else if(tt == TT_3D)
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;
			for(int i = 0; i < header.pixelDepth; ++i)
			{
				sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
				ILubyte *bb= new ILubyte[sz];
				ComAstc(bb , data.Data[i], width, height, at);
				data.CompressData.push_back(bb);
			}
		}
		else
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			for(int level = 0 ; level < header.numberOfMipmapLevels ; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				switch(tt)
				{
				case TT_1D:
					sz = ((width + 3) / 4) * blocksz;
					break;
				case TT_2D:
					sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
					break;
				}

				ILubyte *bb= new ILubyte[sz];
				ComAstc(bb , data.Data[index], width, height, at);
				//ILubyte* bb = ilCompressDXT(data.Data[index] , width , height , depth , (at == AT_SAMPLE_ALPHA)?IL_DXT1:IL_DXT5 , &newsz);
				//assert(newsz == sz);

				data.CompressData.push_back(bb);
				width = std::max(width / 2, 1);
				height = std::max(height / 2, 1);
			}
		}
	}
}

void decode_astc( BMG_header_TT& header,Data_block& data,Texture_target& tt, int at)
{
	if(header.pixelHeight == 1)
	{
		tt = TT_1D;
	}
	else
	{
		if(header.pixelDepth ==1)
		{
			if(header.numberOfFaces != 6)
				tt = TT_2D;
			else
				tt =TT_CUBE;
		}
		else
			tt = TT_3D;
	}
	int flag;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for (int idx = 0; idx < tmp; ++ idx)
	{
		int width = header.pixelWidth;
		int height = header.pixelHeight;
		int depth = header.pixelDepth;
		switch (tt)
		{
		case TT_1D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					int index = idx * header.numberOfMipmapLevels + level;
					//int imagesz = ((width + 3) / 4) * ((0x83F1 == header.glInternalFormat)?8:16);
					data.Data.push_back((unsigned char*)malloc(4*width));
					DeAstc(data.CompressData[index] , data.Data[index] , width , height , at);
					//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
					width = std::max(width / 2, 1);
				}
			}
			break;
		case TT_2D:
			{
				for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				{
					int index = idx *header.numberOfMipmapLevels + level;
					//int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * ((0x83F1 == header.glInternalFormat)?8:16);
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					DeAstc(data.CompressData[index] , data.Data[index] , width , height , at);
					//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
					width = std::max(width / 2, 1);
					height = std::max(height / 2, 1);
				}
			}
			break;
		case TT_3D:
			{
				//for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
				//{
				//	int index = idx * header.numberOfMipmapLevels + level;
				//	int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * depth * ((0x83F1 == header.glInternalFormat)?8:16);
				//	data.Data.push_back((unsigned char*)malloc(4*width*height*depth));
				//	DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
				//	width = std::max(width / 2, 1);
				//	height = std::max(height / 2, 1);
				//	depth = std::max(depth / 2, 1);
				//}
				for(int d = 0; d < header.pixelDepth; ++d)
				{
					int index = idx *header.numberOfMipmapLevels + d;
					data.Data.push_back((unsigned char*)malloc(4*width*height));
					DeAstc(data.CompressData[index] , data.Data[index] , width , height ,at);
					//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
				}
			}
			break;
		case TT_CUBE:
			{
				for (int face = 0; face < 6; ++ face)
				{
					width = header.pixelWidth;
					height = header.pixelHeight;
					for (int level = 0; level < header.numberOfMipmapLevels; ++ level)
					{
						int index = (idx * 6 + face) * header.numberOfMipmapLevels+ level;
						//int imagesz = ((width + 3) / 4) * ((height + 3) / 4) * ((0x8C92 == header.glInternalFormat)?8:16);
						data.Data.push_back((unsigned char*)malloc(4*width*width));

						DeAstc(data.CompressData[index] , data.Data[index] , width , height , at);
						//DecompressImage(data.Data[index] , width , height , data.CompressData[index] , (0x83F1 == header.glInternalFormat)?squish::kDxt1:squish::kDxt5);
						width = std::max(width / 2, 1);
						height = std::max(height / 2, 1);
					}
				}
			}
			break;
		}
	}
}



void encode_bmg(const BMG_header_TT& header, Data_block& data, Texture_target tt, int at)
{
	size_t sz = 0;
	int blocksz = 16;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for (int idx = 0; idx < tmp; ++idx)
	{
		if (tt == TT_CUBE)
		{
			for (int face = 0; face < 6; ++face)
			{
				int width = header.pixelWidth;
				//!对于每一层mipmap
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					sz = ((width + 3) / 4) * ((width + 3) / 4) * blocksz;
					int index = (idx * 6 + face - 0) * header.numberOfMipmapLevels + level;
					//data.CompressData.push_back((unsigned char*)malloc(sz));
					unsigned char* bb;
					ComBmg(&bb, data.Data[index], width, width, at);
					data.CompressData.push_back(bb);
					width = std::max(width / 2, 1);
				}
			}
		}
		else if (tt == TT_3D)
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			int depth = header.pixelDepth;
			for (int i = 0; i < header.pixelDepth; ++i)
			{
				sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
				unsigned char *bb;
				ComBmg(&bb, data.Data[i], width, height, at);
				data.CompressData.push_back(bb);
			}
		}
		else
		{
			int width = header.pixelWidth;
			int height = header.pixelHeight;
			for (int level = 0; level < header.numberOfMipmapLevels; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				switch (tt)
				{
				case TT_1D:
					sz = ((width + 3) / 4) * blocksz;
					break;
				case TT_2D:
					sz = ((width + 3) / 4) * ((height + 3) / 4) * blocksz;
					break;
				}
				ILubyte *bb;
				ComBmg(&bb, data.Data[index], width, height, at);
				data.CompressData.push_back(bb);
				width = std::max(width / 2, 1);
				height = std::max(height / 2, 1);
			}
		}
	}
}

void decode_bmg(BMG_header_TT& header, Data_block& data, Texture_target& tt, int at)
{
	if (header.pixelHeight == 1)
	{
		tt = TT_1D;
	}
	else
	{
		if (header.pixelDepth == 1)
		{
			if (header.numberOfFaces != 6)
				tt = TT_2D;
			else
				tt = TT_CUBE;
		}
		else
			tt = TT_3D;
	}
	int flag;
	int tmp = header.numberOfArrayElements;// == 0) ? 0 : header.numberOfArrayElements - 1;
	for (int idx = 0; idx < tmp; ++idx)
	{
		int width = header.pixelWidth;
		int height = header.pixelHeight;
		int depth = header.pixelDepth;
		switch (tt)
		{
		case TT_1D:
		{
			for (int level = 0; level < header.numberOfMipmapLevels; ++level)
			{
				int index = idx * header.numberOfMipmapLevels + level;
				data.Data.push_back((unsigned char*)malloc(4 * width));
				DeBmg(data.CompressData[index], data.Data[index], width, height, at);
				width = std::max(width / 2, 1);
			}
		}
		break;
		case TT_2D:
		{
			for (int level = 0; level < header.numberOfMipmapLevels; ++level)
			{
				int index = idx *header.numberOfMipmapLevels + level;
				data.Data.push_back((unsigned char*)malloc(4 * width*height));
				DeBmg(data.CompressData[index], data.Data[index], width, height, at);
				width = std::max(width / 2, 1);
				height = std::max(height / 2, 1);
			}
		}
		break;
		case TT_3D:
		{
			for (int d = 0; d < header.pixelDepth; ++d)
			{
				int index = idx *header.numberOfMipmapLevels + d;
				data.Data.push_back((unsigned char*)malloc(4 * width*height));
				DeBmg(data.CompressData[index], data.Data[index], width, height, at);
			}
		}
		break;
		case TT_CUBE:
		{
			for (int face = 0; face < 6; ++face)
			{
				width = header.pixelWidth;
				height = header.pixelHeight;
				for (int level = 0; level < header.numberOfMipmapLevels; ++level)
				{
					int index = (idx * 6 + face) * header.numberOfMipmapLevels + level;
					data.Data.push_back((unsigned char*)malloc(4 * width*width));
					DeBmg(data.CompressData[index], data.Data[index], width, width, at);
					width = std::max(width / 2, 1);
					height = std::max(height / 2, 1);
				}
			}
		}
		break;
		}
	}
}
