#include "Image3D.h"
#include "Codec.h"
Image3D::Image3D(QWidget *parent /* = 0 */)
	: Image(IMAGE3D,parent)
	, _currentVolume(0)
{
	
}

Image3D::Image3D(ILuint imageID,QWidget *parent /* = 0 */)
	: Image(IMAGE3D,parent)
	, _currentVolume(0)
{
	if(!ilIsImage(imageID))
		return;

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	int depth = ilGetInteger(IL_IMAGE_DEPTH);
	int size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	ILubyte *data = ilGetData();
	int picDataSize = size / depth;

	for(int i = 0; i < depth; i++)
	{
		ILuint newImageID = genImage(data + picDataSize*i,width,height);
		ilBindImage(newImageID);
		_imageList.push_back(IMAGE(newImageID,convertToQImage()));
	}
}

Image3D::Image3D(const BMG_header_TT &kh,Data_block &db,int begin,int size,QWidget *parent /* = 0 */)
	: Image(IMAGE3D,parent)
	, _currentVolume(0)
{
	assert(kh.pixelDepth == size);
	
	for(int i = begin; i < begin + size; i++)
	{
		ILuint imageID = genImage(db.Data[i],kh.pixelWidth,kh.pixelHeight);
		_imageList.push_back(IMAGE(imageID,convertToQImage()));
	}

	if (kh.fourcc != FOUR_CC('B', 'M', 'G', 'T'))
		_canCompress = false;
}

bool Image3D::preVolume()
{
	if(_currentVolume > 0)
	{
		_currentVolume -= 1;
		update();
		return true;
	}
	return false;
}

bool Image3D::nextVolume()
{
	int numVolume = getNumVolume();
	if(_currentVolume + 1 < numVolume)
	{
		_currentVolume += 1;
		update();
		return true;
	}
	return false;
}

int Image3D::getNumVolume() const
{
	return _imageList.size();
}

QImage *Image3D::getImage() const
{
	if(_currentVolume >= _imageList.size())
		return 0;

	return _imageList[_currentVolume].image;
}

Image *Image3D::clone()
{
	Image *image = new Image3D;
	Image::depthCopy(*image,*this);
	return image;
}

Image *Image3D::compressImage(CompressMethord cm, Data_block* _db)
{
	ilBindImage(_imageList[0].imageID);

	BMG_header_TT header;
	header.pixelDepth = _imageList.size();
	header.pixelWidth = ilGetInteger (IL_IMAGE_WIDTH);
	header.pixelHeight = ilGetInteger (IL_IMAGE_HEIGHT);
	header.numberOfMipmapLevels = 1;
	header.numberOfFaces = 1;
	header.numberOfArrayElements = 1;
	header.channels = 3;
	Data_block db;

	for(int i = 0; i < header.pixelDepth; i++)
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
			encode_bmg(header, db, TT_3D, at);
			Texture_target tt;
			db.Data.clear();
			decode_bmg(header, db, tt, at);
		}
		break;
	case CM_S3TC:
		{

			//!fixme
			Alpha_type at = isComplexAlpha();
			if (at == AT_COMPLEX_ALPHA)
				header.fourcc = FOUR_CC('S', '3', 'T', '3');
			else if (at == AT_SAMPLE_ALPHA)
				header.fourcc = FOUR_CC('S', '3', 'T', '2');
			else
			{
				header.fourcc = FOUR_CC('S', '3', 'T', '1');
				header.channels = 3;
			}
			encode_s3tc(header,db, TT_3D,at);

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
			encode_etc2(header,db,TT_3D,at);

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
			encode_astc(header,db,TT_3D,at);

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
	Image *image = new Image3D(header,db,0,db.Data.size());
	return image;
}

int Image3D::pack(Data_block &db)
{
	int sizeVolume = getNumVolume();
	for(int i = 0; i < sizeVolume; i++)
	{
		IMAGE image = getImage(i);
		ilBindImage(image.imageID);
		db.Data.push_back(ilGetData());
	}
	return sizeVolume;
}

IMAGE Image3D::getImage(int volume)
{
	int sizeVolume = getNumVolume();
	if(volume < sizeVolume)
	{
		return _imageList[volume];
	}
	return IMAGE(-1,0);
}

QList<Image*> Image3D::loadKtxData(const BMG_header_TT &kh,Data_block &db)
{
	QList<Image*> images;
	int tmp = kh.numberOfArrayElements;// == 0) ? 0 : kh.numberOfArrayElements - 1;
	for(int i = 0; i < tmp; i++)
	{
		Image *image = new Image3D(kh,db,i*kh.pixelDepth,kh.pixelDepth);
		images.append(image);
	}
	return images;
}
