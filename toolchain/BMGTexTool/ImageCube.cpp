#include "ImageCube.h"
#include "Codec.h"
#include "Image2D.h"


//this constructor will create a default ImageCube
ImageCube::ImageCube(QWidget *parent /* = 0 */)
	: Image(IMAGECUBE,parent)
	, _currentFace(POSITIVE_X)
{
	cubew = cubeh = 512;
	createDefault();

}

ImageCube::ImageCube(ILuint imageID,QWidget *parent /* = 0 */)
	: Image(IMAGECUBE,parent)
	, _currentFace(POSITIVE_X)
	, _defaultImgMask(0x3f)
{
	ilBindImage(imageID);
	int faces = ilGetInteger(IL_NUM_FACES);
	int mips = ilGetInteger(IL_NUM_MIPMAPS);

	assert(faces == FACECOUNT);

	for(int mip = 0; mip < mips; mip++)
	{
		for(int face = 0; face < faces; face++)
		{
			ilBindImage(imageID);
			ilActiveFace(face);
			ilActiveMipmap(mip);
			int width = ilGetInteger(IL_IMAGE_WIDTH);
			int height = ilGetInteger(IL_IMAGE_HEIGHT);
			unsigned char *data = ilGetData();
			ILuint imgID = genImage(data,width,height);
			_imageList.append(IMAGE(imgID,convertToQImage()));
		}
	}

	ilDeleteImage(imageID);
}

ImageCube::ImageCube(const BMG_header_TT &kh,Data_block &db,int begin,int size,QWidget *parent /* = 0 */)
	: Image(IMAGECUBE,parent)
	, _currentFace(POSITIVE_X)
	, _defaultImgMask(0x3f)
{
	assert(kh.pixelDepth == 1);
	int faces = kh.numberOfFaces;
	int mips = kh.numberOfMipmapLevels;

	setImageListSize(faces*mips);

	for(int face = 0; face < faces; face++)
	{
		int width = kh.pixelWidth;
		int height = kh.pixelHeight;
		for(int mip = 0; mip < mips; mip++)
		{
			ILuint imageID = genImage(db.Data[begin + face*mips + mip],width,height);
			_imageList[getImagePosAtList((FACE)face,mip)] = IMAGE(imageID,convertToQImage());
			width = std::max(width/2,1);
			height = std::max(height/2,1);
		}
	}

	if (kh.fourcc != FOUR_CC('B', 'M', 'G', 'T'))
		_canCompress = false;
}

void ImageCube::setCurrentFace(FACE face)
{
	if(_currentFace != face)
	{
		_currentFace = face;
		update();
	}
}

bool ImageCube::setFaceImage(FACE face,Image *image)
{
	if(image == 0 || image->isNull() || image->getImageType() != IMAGE2D ||
		image->imageWidth() != image->imageHeight())
		return false;

	if(_defaultImgMask != 0)
	{
		if(image->imageWidth() != imageWidth() ||
			image->imageHeight() != imageHeight())
			return false;
	}
	
	if(getNumMipmaps() > 1 && image->getNumMipmaps() == 1)
		image->genMipmaps();

	assert(getNumMipmaps() == image->getNumMipmaps());

	int mips = getNumMipmaps();
	for(int mip = 0; mip < mips; mip++)
	{
		deleteFaceImage(face,mip);
		int n = getImagePosAtList(face,mip);
		IMAGE src = image->getImageAtList(mip);
		_imageList[n] = IMAGE(ilImageClone(src.imageID),new QImage(*src.image));
	}

	update();

	return true;
}

bool ImageCube::setFaceImage(FACE face,QString imageFile)
{
	int offsetx, offsety;
	Image *image = Image::createImage(imageFile, offsetx, offsety, 0, false);
	if (face == POSITIVE_X)
	{
		cubew = image->getImage()->width();
		cubeh = image->getImage()->height();
		if (image->getImage()->width() != image->getImage()->height())
			return false;
	}
	else
	{
		if ((cubew != image->getImage()->width()) || cubeh != image->getImage()->height())
			return false;
		if (image->getImage()->width() != image->getImage()->height())
			return false;
	}
	bool rvel = setFaceImage(face,image);
	allok[face] = rvel;
	if (allok[0] && allok[1] && allok[2] && allok[3] && allok[4] && allok[5])
	{
		_defaultImgMask = 0x3f;
	}
	delete image;
	return rvel;
}

bool ImageCube::setFaceImage(Image *image)
{
	return setFaceImage(_currentFace,image);
}

bool ImageCube::setFaceImage(QString imageFile)
{
	return setFaceImage(_currentFace,imageFile);
}

IMAGE ImageCube::getValidFaceImage() const
{
	if(_defaultImgMask == 0)
		return IMAGE(-1,0);

	for(int i = 0; i < FACECOUNT; i++)
	{
		if((_defaultImgMask >> i)&0x01 != 0)
		{
			return _imageList[i];
		}
	}

	return IMAGE(-1,0);
}

int ImageCube::imageWidth() const
{
	IMAGE image = getValidFaceImage();
	if(image.valid())
	{
		ilBindImage(image.imageID);
		return ilGetInteger(IL_IMAGE_WIDTH);
	}

	return cubew;
}

int ImageCube::imageHeight() const
{
	IMAGE image = getValidFaceImage();
	if(image.valid())
	{
		ilBindImage(image.imageID);
		return ilGetInteger(IL_IMAGE_HEIGHT);
	}

	return cubeh;
}

void ImageCube::deleteFaceImage(FACE face,int mip /*= 0*/)
{
	unsigned char mask = 1 << face;
	if(mask && _defaultImgMask == 0 )
		return;

	int n = getImagePosAtList(face,mip);
	if(n < 0)
		return;

	IMAGE image = _imageList[n];
	if(image.valid())
	{
		ilDeleteImage(image.imageID);
		delete image.image;
		_imageList[n] = IMAGE(-1,-0);
	}
}

IMAGE ImageCube::getImage(FACE face,int mip /* = 0 */)
{
	int n = getImagePosAtList(face,mip);
	if(n < 0)
		return IMAGE(-1,0);
	return _imageList[n];
}

int ImageCube::getImagePosAtList(FACE face,int mip /* = 0 */) const
{
	int mips = getNumMipmaps();
	if(mip >= mips)
		return -1;
	return mip*FACECOUNT + face;
}

int ImageCube::getNumMipmaps() const
{
	int n = _imageList.size();
	assert(n%FACECOUNT == 0);
	return n / FACECOUNT;
}

void ImageCube::createDefault()
{
	_defaultImgMask = 0;
	assert(_imageList.size() == 0);
	setImageListSize(FACECOUNT);

	
	QImage ppx(QString::fromUtf8(":/BMGTexTool/Resources/px.png"));
	QImage ppy(QString::fromUtf8(":/BMGTexTool/Resources/py.png"));
	QImage ppz(QString::fromUtf8(":/BMGTexTool/Resources/pz.png"));
	QImage pnx(QString::fromUtf8(":/BMGTexTool/Resources/nx.png"));
	QImage pny(QString::fromUtf8(":/BMGTexTool/Resources/ny.png"));
	QImage pnz(QString::fromUtf8(":/BMGTexTool/Resources/nz.png"));

	QImage ppxx = ppx.convertToFormat(QImage::Format_RGBA8888);
	QImage ppyy = ppy.convertToFormat(QImage::Format_RGBA8888);
	QImage ppzz = ppz.convertToFormat(QImage::Format_RGBA8888);
	QImage pnxx = pnx.convertToFormat(QImage::Format_RGBA8888);
	QImage pnyy = pny.convertToFormat(QImage::Format_RGBA8888);
	QImage pnzz = pnz.convertToFormat(QImage::Format_RGBA8888);

	ILubyte *datapx = new ILubyte[cubew*cubew * 4];
	memcpy(datapx, ppxx.bits(), ppxx.byteCount());
	ILuint imageIDpx = ilGenImage();
	ilBindImage(imageIDpx);
	ilLoadDataL(datapx,sizeof(ILubyte)*cubew*cubew *4, cubew, cubew,1,4);
	Image2D *imagepx = new Image2D(imageIDpx);
	setFaceImage(POSITIVE_X, imagepx);

	ILubyte *datapy = new ILubyte[cubew*cubew * 4];
	memcpy(datapy, ppyy.bits(), ppyy.byteCount());
	ILuint imageIDpy = ilGenImage();
	ilBindImage(imageIDpy);
	ilLoadDataL(datapy, sizeof(ILubyte)*cubew*cubew * 4, cubew, cubew, 1, 4);
	Image2D *imagepy = new Image2D(imageIDpy);
	setFaceImage(POSITIVE_Y, imagepy);

	ILubyte *datapz = new ILubyte[cubew*cubew * 4];
	memcpy(datapz, ppzz.bits(), ppzz.byteCount());
	ILuint imageIDpz = ilGenImage();
	ilBindImage(imageIDpz);
	ilLoadDataL(datapz, sizeof(ILubyte)*cubew*cubew * 4, cubew, cubew, 1, 4);
	Image2D *imagepz = new Image2D(imageIDpz);
	setFaceImage(POSITIVE_Z, imagepz);


	ILubyte *datanx = new ILubyte[cubew*cubew * 4];
	memcpy(datanx, pnxx.bits(), pnxx.byteCount());
	ILuint imageIDnx = ilGenImage();
	ilBindImage(imageIDnx);
	ilLoadDataL(datanx, sizeof(ILubyte)*cubew*cubew * 4, cubew, cubew, 1, 4);
	Image2D *imagenx = new Image2D(imageIDnx);
	setFaceImage(NEGATIVE_X, imagenx);

	ILubyte *datany = new ILubyte[cubew*cubew * 4];
	memcpy(datany, pnyy.bits(), pnyy.byteCount());
	ILuint imageIDny = ilGenImage();
	ilBindImage(imageIDny);
	ilLoadDataL(datany, sizeof(ILubyte)*cubew*cubew * 4, cubew, cubew, 1, 4);
	Image2D *imageny = new Image2D(imageIDny);
	setFaceImage(NEGATIVE_Y, imageny);

	ILubyte *datanz = new ILubyte[cubew*cubew * 4];
	memcpy(datanz, pnzz.bits(), pnzz.byteCount());
	ILuint imageIDnz = ilGenImage();
	ilBindImage(imageIDnz);
	ilLoadDataL(datanz, sizeof(ILubyte)*cubew*cubew * 4, cubew, cubew, 1, 4);
	Image2D *imagenz = new Image2D(imageIDnz);
	setFaceImage(NEGATIVE_Z, imagenz);

	allok[0] = allok[1] = allok[2] = allok[3] = allok[4] = allok[5] = false;
}

QImage *ImageCube::getImage() const
{
	int n = getImagePosAtList(_currentFace,_currentMipmap);
	if(n < 0)
		return 0;
	return _imageList[n].image;
}

Image *ImageCube::compressImage(CompressMethord cm, Data_block* _db)
{
	ilBindImage(_imageList[0].imageID);

	BMG_header_TT header;
	header.pixelDepth = 1;
	header.pixelWidth = imageWidth();
	header.pixelHeight = imageHeight();
	header.numberOfMipmapLevels = getNumMipmaps();
	header.numberOfFaces = FACECOUNT;
	header.numberOfArrayElements = 1;
	header.channels = 4;
	Data_block db;

	for(int j = 0; j < header.numberOfFaces; j++)
	{
		for(int i = 0; i < header.numberOfMipmapLevels; i++)
		{
			ilBindImage(_imageList[getImagePosAtList((FACE)j,i)].imageID);
			db.Data.push_back(ilGetData());
		}
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
			encode_bmg(header, db, TT_CUBE, at);
			Texture_target tt;
			db.Data.clear();
			decode_bmg(header, db, tt, at);
		}
		break;
	case CM_S3TC:
		{
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
			encode_s3tc(header,db,TT_CUBE,at);

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
			encode_etc2(header,db,TT_CUBE,at);

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
			encode_astc(header,db,TT_CUBE,at);

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

	Image *image = new ImageCube(header,db,0,db.Data.size());
	return image;
}

Image *ImageCube::clone()
{
	Image *image = new ImageCube;
	Image::depthCopy(*image,*this);
	return image;
}

int ImageCube::pack(Data_block &db)
{
	int mips = getNumMipmaps();
	
	for(int face = 0; face < FACECOUNT; face++)
	{
		for(int mip = 0; mip < mips; mip++)
		{
			IMAGE image = getImage((FACE)face,mip);
			if(image.valid())
			{
				ilBindImage(image.imageID);
				db.Data.push_back(ilGetData());
			}
		}
	}
	return mips*FACECOUNT;
}

void ImageCube::setImage(FACE face,int mip,const ILubyte *data,int width,int height)
{
	int pos = getImagePosAtList(face,mip);
	if(pos < 0)
		return;

	ILubyte *imgData = new ILubyte[width*height*4];
	memcpy(imgData,data,sizeof(char)*width*height*4);
	ILuint imageID = genImage(imgData,width,height);

	IMAGE image = _imageList[pos];
	if(image.valid())
	{
		ilDeleteImage(image.imageID);
		delete image.image;
	}
	image.imageID = imageID;
	ilBindImage(imageID);
	image.image = convertToQImage();

	_imageList[pos] = image;
}

bool ImageCube::genMipmaps()
{
	if(getNumMipmaps() > 1)
		return false;

	int mips = 0;

	for(int i = 0; i < FACECOUNT; i++)
	{
		if((_defaultImgMask >> i) & 0x01 != 0)
		{
			IMAGE image = _imageList[i];
			assert(image.valid());

			ilBindImage(image.imageID);
			ILubyte *data = ilGetData();
			int width = ilGetInteger(IL_IMAGE_WIDTH);
			int height = ilGetInteger(IL_IMAGE_HEIGHT);
			assert(ilGetInteger(IL_IMAGE_BPP) == 4);

			ILubyte *tmpData = new ILubyte[width*height*4];
			memcpy(tmpData,data,sizeof(ILubyte)*width*height*4);
			ILuint imageID = genImage(tmpData,width,height);
			ilBindImage(imageID);
			ilLoadDataL(tmpData,sizeof(ILubyte)*width*height*4,width,height,1,4);
			iluBuildMipmaps();

			if(i == 0)
			{
				mips = ilGetInteger(IL_NUM_MIPMAPS);
				setImageListSize((mips+1)*FACECOUNT);
			}

			ilBindImage(imageID);
			assert(mips == ilGetInteger(IL_NUM_MIPMAPS));

			for(int mip = 1; mip <= mips; mip++)
			{
				ilBindImage(imageID);
				ilActiveMipmap(mip);
				int w = ilGetInteger(IL_IMAGE_WIDTH);
				int h = ilGetInteger(IL_IMAGE_HEIGHT);
				setImage((FACE)i,mip,ilGetData(),w,h);
			}
			ilDeleteImage(imageID);
		}
	}

	return false;
}

QList<Image*> ImageCube::loadKtxData(const BMG_header_TT &kh,Data_block &db)
{
	QList<Image*> images;
	int tmp = kh.numberOfArrayElements;// == 0) ? 0 : kh.numberOfArrayElements - 1;
	for(int i = 0; i < tmp; i++)
	{
		int faces = kh.numberOfFaces;
		int mips = kh.numberOfMipmapLevels;
		Image *image = new ImageCube(kh,db,faces*mips*i,faces*mips);
		images.append(image);
	}
	return images;
}