#include "Image2D.h"
#include "Image3D.h"
#include "Codec.h"
Image2D::Image2D(QWidget *parent /* = 0 */)
	: Image(IMAGE2D,parent)
{

}

Image2D::Image2D(ILuint imageID,QWidget *parent /* = 0 */)
	: Image(IMAGE2D,parent)
{
	ilBindImage(imageID);
	int mipNum = ilGetInteger(IL_NUM_MIPMAPS);
	if(mipNum == 0)
	{
		ilBindImage(imageID);
		_imageList.push_back(IMAGE(imageID,convertToQImage()));
	}
	else
	{
		loadMipmaps(imageID);
		ilDeleteImage(imageID);
	}
}

Image2D::Image2D(const BMG_header_TT &kh,Data_block &db,int begin,int size,QWidget *parent /* = 0 */)
	: Image(IMAGE2D,parent)
{
	assert(kh.pixelDepth == 1);
	ILint width = kh.pixelWidth;
	ILint height = kh.pixelHeight;

	for(int i = 0; i < size; i++)
	{
		ILuint imageID = genImage(db.Data[begin + i],width,height);
		_imageList.push_back(IMAGE(imageID,convertToQImage()));
		width = std::max(width/2,1);
		height = std::max(height/2,1);
	}

	if (kh.fourcc != FOUR_CC('B', 'M', 'G', 'T'))
		_canCompress = false;
}

Image *Image2D::clone()
{
	Image *image = new Image2D;
	Image::depthCopy(*image,*this);
	return image;
}

bool Image2D::genMipmaps()
{
	if(_imageList.size() == 1)
	{
		IMAGE mipmap(_imageList[0]);
		_imageList.clear();
		ilBindImage(mipmap.imageID);
		if(iluBuildMipmaps())
		{
			loadMipmaps(mipmap.imageID);
			ilDeleteImage(mipmap.imageID);
			delete mipmap.image;
		}
		return true;
	}
	return false;
}

Image *Image2D::compressImage(CompressMethord cm, Data_block* _db)
{
	ilBindImage(_imageList[0].imageID);

	BMG_header_TT header;
	header.pixelDepth = 1;
	header.pixelWidth = ilGetInteger (IL_IMAGE_WIDTH);
	header.pixelHeight = ilGetInteger (IL_IMAGE_HEIGHT);
	header.numberOfMipmapLevels = _imageList.size();
	header.numberOfFaces = 1;
	header.numberOfArrayElements = 1;
	header.channels = 4;
	Data_block db;

	for(int i = 0; i < header.numberOfMipmapLevels; i++)
	{
		ilBindImage(_imageList[i].imageID);
		db.Data.push_back(ilGetData());
	}

	switch(cm)
	{
	case CMD_NONE:
		{
			Alpha_type at = isComplexAlpha();
			if (at == AT_NONE_ALPHA)
			{
				header.channels = 3;
				header.fourcc = FOUR_CC('B', 'M', 'G', 'T');
			}
			else
			{
				header.fourcc = FOUR_CC('B', 'M', 'G', 'T');
			}
			encode_bmg(header, db, TT_2D, at);
			Texture_target tt;
			db.Data.clear();
			decode_bmg(header, db, tt, at);
		}
		break;
	case  CM_S3TC:
		{
			//!fixme
			Alpha_type at = isComplexAlpha();
			if(at == AT_COMPLEX_ALPHA)
				header.fourcc = FOUR_CC('S', '3', 'T', '3');
			else if (at == AT_SAMPLE_ALPHA)
				header.fourcc = FOUR_CC('S', '3', 'T', '2');
			else
			{
				header.fourcc = FOUR_CC('S', '3', 'T', '1');
				header.channels = 3;
			}
			encode_s3tc(header,db,TT_2D,at);

			Texture_target tt;
			db.Data.clear();
			decode_s3tc(header,db,tt, at);
		}
		break;
	case CM_ETC2:
		{
			Alpha_type at = isComplexAlpha();
			if (at == AT_COMPLEX_ALPHA)
				header.fourcc = FOUR_CC('E', 'T', 'C', '3');
			else if (at == AT_SAMPLE_ALPHA)
				header.fourcc = FOUR_CC('E', 'T', 'C', '2');
			else
			{
				header.fourcc = FOUR_CC('E', 'T', 'C', '1');
				header.channels = 3;
			}
			encode_etc2(header,db,TT_2D, at);

			Texture_target tt;
			db.Data.clear();
			decode_etc2(header,db,tt, at);
		}
		break;
	case CM_ASTC:
		{
			Alpha_type at = isComplexAlpha();
			if (at == AT_COMPLEX_ALPHA)
				header.fourcc = FOUR_CC('A', 'S', 'T', '3');
			else if (at == AT_SAMPLE_ALPHA)
				header.fourcc = FOUR_CC('A', 'S', 'T', '2');
			else
			{
				header.fourcc = FOUR_CC('A', 'S', 'T', '1');
				header.channels = 3;
			}
			encode_astc(header,db,TT_2D,at);

			Texture_target tt;
			db.Data.clear();
			decode_astc(header,db,tt, at);
		}
		break;
	}
	if (_db)
	{
		*_db = db;
	}

	Image *image = new Image2D(header,db,0,db.Data.size());
	return image;
}

bool Image2D::canConvert3D()
{
	if(getNumMipmaps() != 1)
		return false;

	ILuint imageID = _imageList[0].imageID;
	if(!ilIsImage(imageID))
		return false;

	ilBindImage(imageID);
	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);

	if(isTwos(width))
		return true;

	return false;
}

Image3D *Image2D::getImage3D(int width)
{
	ILuint imageID = _imageList[0].imageID;
	ilBindImage(imageID);
	int w = ilGetInteger(IL_IMAGE_WIDTH);
	int h = ilGetInteger(IL_IMAGE_HEIGHT);
	int depth = w / width;
	if(!isTwos(depth))
		return 0;
	
	ILubyte *data = new ILubyte[w*h*4];
	for(int i = 0; i < depth; i++)
	{
		ilCopyPixels(width*i,0,0,width,h,1,IL_RGBA,IL_UNSIGNED_BYTE,data + i*width*h*4);
	}

	ILuint newImg = ilGenImage();
	ilBindImage(newImg);
	ilLoadDataL(data,w*h*4,width,h,depth,4);
	Image3D *image = new Image3D(newImg);
	return image;
}

bool Image2D::isTwos(int num)
{
	if(num % 2 != 0)
		return false;

	int n = sizeof(num) * 8;
	int sum = 0;
	for(int i = 1; i < n; i++)
	{
		if((num >> i) & 0x1 != 0)
			sum++;
		if(sum > 1)
			return false;
	}

	return true;
}

QList<Image*> Image2D::loadKtxData(const BMG_header_TT &kh,Data_block &db)
{
	QList<Image*> images;
	int tmp = kh.numberOfArrayElements;// == 0) ? 0 : kh.numberOfArrayElements - 1;
	for(int i = 0; i < tmp; i++)
	{
		Image *image = new Image2D(kh,db,i*kh.numberOfMipmapLevels,kh.numberOfMipmapLevels);
		images.append(image);
	}
	return images;
}
