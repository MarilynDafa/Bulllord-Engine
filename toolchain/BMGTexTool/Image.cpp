#include <QtCore/QDir>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include "Image.h"
#include "Image1D.h"
#include "Image2D.h"
#include "Image3D.h"
#include "ImageCube.h"
#include "Codec.h"

int ImageOp::pack(Data_block &db)
{
	return getImage()->pack(db);
}

bool ImageOp::isSameFormatImage(ImageOp &image1,ImageOp &image2)
{
	return ((image1.getCompressMethod()==image2.getCompressMethod())&&
		Image::isSameFormat(*(image1.getImage()),*(image2.getImage()) , image1.getCompressMethod()));
}

void ImageOp::getKtxHeader(BMG_header_TT &kh)
{
	Image *image = getImage();
	if(image != 0)
	{
		kh.pixelWidth = image->imageWidth();
		kh.pixelHeight = image->imageHeight();
		kh.pixelDepth = image->imageDepth();
		kh.numberOfMipmapLevels = image->getNumMipmaps();
		kh.numberOfFaces = image->getFaceCount();
	}
}

Texture_target ImageOp::getTextureTarget()
{
	switch(getImageType())
	{
	case IMAGE1D:
		return TT_1D;
		break;
	case IMAGE2D:
		return TT_2D;
		break;
	case IMAGE3D:
		return TT_3D;
		break;
	case IMAGECUBE:
		return TT_CUBE;
		break;
	default:
		return TT_2D;
		break;
	}
}

Alpha_type ImageOp::getAlphaType()
{
	return getImage()->isComplexAlpha();
}

void ImageOp::setFace(FACE face)
{
	QList<Image*> images = getImages();
	int size = images.size();
	for(int i = 0; i < size; i++)
	{
		images[i]->setCurrentFace(face);
	}
}

IMAGETYPE ImageOp::getImageType()
{
	Image *image = getImage();
	if(image != 0)
		return image->getImageType();
	return IMAGENO;
}

void ImageOp::preVolume()
{
	QList<Image*> images = getImages();
	int size = images.size();
	for(int i = 0; i < size; i++)
	{
		images[i]->preVolume();
	}
}

void ImageOp::nextVolume()
{
	QList<Image*> images = getImages();
	int size = images.size();
	for(int i = 0; i < size; i++)
	{
		images[i]->nextVolume();
	}
}

QList<Image*> ImageOp::getImages()
{
	QList<Image*> images;
	Image *image = getImage();
	if(image != 0)
		images.append(image);
	return images;
}

Image::Image(IMAGETYPE type,QWidget *parent /*= 0*/)
	: QWidget(parent)
	, _currentMipmap(0)
	, _zoomX(1.0f)
	, _zoomY(1.0f)
	, _offsetX(0)
	, _offsetY(0)
	, _imgType(type)
	, _canCompress(true)
{
	_cm = CMD_NONE;
	setMouseTracking(true);
}

Image::~Image()
{
	int n = _imageList.size();
	for(int i = 0; i < n; i++)
	{
		delete _imageList[i].image;
		ilDeleteImage(_imageList[i].imageID);
	}
	_imageList.clear();
}

void Image::adjustWidgetSize()
{
	resize(showWidth(),showHeight());
}

void Image::paintEvent(QPaintEvent *paintEvent)
{
	QPainter painter(this);
	QRect destRect(-_offsetX,-_offsetY,showWidth(),showHeight());
	QRect srcRect(0,0,currentImageWidth(),currentImageHeight());
	painter.drawImage(destRect,*getImage(),srcRect);
}

void Image::mouseMoveEvent(QMouseEvent *event)
{
	emit mouseMove(event->x(),event->y());
}

void Image::setZoom(float zoom)
{
	_offsetX = _offsetX*zoom/_zoomX;
	_zoomX = zoom;
	_offsetY = _offsetY*zoom/_zoomY;
	_zoomY = zoom;
	repaint();
}

void Image::zoomIn()
{
	setZoom(getZoom()*2.0);
}

void Image::zoomOut()
{
	setZoom(getZoom()/2.0);
}

QImage *Image::convertToQImage()
{
	ILint width = ilGetInteger(IL_IMAGE_WIDTH);
	ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILint depth = ilGetInteger(IL_IMAGE_DEPTH);
	ILint pixelFormat = ilGetInteger(IL_IMAGE_FORMAT);
	ILint imageBPP = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	ILint dataType = ilGetInteger(IL_IMAGE_TYPE);
	ILint originMode = ilGetInteger(IL_IMAGE_ORIGIN);
	ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	ILubyte *imageData = ilGetData();
	QImage *image = 0;

	if(dataType != IL_UNSIGNED_BYTE && dataType != IL_BYTE)
		return 0;

	if(pixelFormat != IL_RGBA || imageBPP != 4)
		return 0;

	ILubyte *data = new ILubyte[imageBPP*width*height];
	memcpy(data,imageData,sizeof(ILubyte)*width*height*imageBPP);

	if(originMode == IL_ORIGIN_LOWER_LEFT)
	{
		ILubyte *src,*dest;
		for(int y = 0; y < height; y++)
		{
			src = data+y*width*imageBPP;
			dest = imageData+(height-y-1)*width*imageBPP;
			memcpy(dest,src,sizeof(ILubyte)*width*imageBPP);
		}
		ilSetInteger(IL_ORIGIN_MODE,IL_ORIGIN_UPPER_LEFT);
	}

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			data[(y*width+x)*imageBPP] = imageData[(y*width+x)*imageBPP+2];
			data[(y*width+x)*imageBPP+1] = imageData[(y*width+x)*imageBPP+1];
			data[(y*width+x)*imageBPP+2] = imageData[(y*width+x)*imageBPP];
			data[(y*width+x)*imageBPP+3] = imageData[(y*width+x)*imageBPP+3];
		}
	}
	image = new QImage(data,width,height,QImage::Format_ARGB32);

	return image;
}

Alpha_type Image::isComplexAlpha() const
{
	if(_imageList.size()>0)
	{
		ilBindImage(_imageList[0].imageID);

		ILint pixelFormat = ilGetInteger(IL_IMAGE_FORMAT);
		ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA) / 4;
		ILubyte *data = ilGetData();
		assert(pixelFormat == IL_RGBA);

		int record[256];
		memset(record,0,sizeof(int)*256);
		for(int i = 0; i < dataSize; i++)
		{
				record[data[i*4+3]] = 1;
		}
		int sum = 0;
		for(int i = 0; i < 256; i++)
			sum += record[i];
		if(sum > 2)
			return AT_COMPLEX_ALPHA;
		else if(sum == 2)
		{
			if(record[255] == 1 && record[0] == 1)
				return AT_SAMPLE_ALPHA;
			else
				return AT_COMPLEX_ALPHA;
		}
		else if(sum == 1)
		{
			if(record[255] == 1)
				return AT_NONE_ALPHA;
			else
				return AT_SAMPLE_ALPHA;
		}
	}
	return AT_NONE_ALPHA;
}

ILuint Image::genImage(unsigned char *data,int w,int h,int bpp /* = 4 */)
{
	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilLoadDataL(data,bpp*w*h,w,h,1,bpp);
	return imageID;
}

int Image::currentImageWidth() const
{
	QImage *image = getImage();
	if(image && !image->isNull())
		return image->width();
	return 0;
}

int Image::currentImageHeight() const
{
	QImage *image = getImage();
	if(image && !image->isNull())
		return image->height();
	return 0;
}

int Image::showWidth() const
{
	return currentImageWidth() * _zoomX;
}

int Image::showHeight() const
{
	return currentImageHeight() * _zoomY;
}

int Image::imageWidth() const
{
	ilBindImage(_imageList[0].imageID);
	return ilGetInteger(IL_IMAGE_WIDTH);
}

int Image::imageHeight() const
{
	ilBindImage(_imageList[0].imageID);
	return ilGetInteger(IL_IMAGE_HEIGHT);
}

int Image::widgetWantWidth() const
{
	QWidget *parent = parentWidget();
	if(parent != 0)
	{
		return showWidth() + parent->width() - width();
	}
	return showWidth();
}

int Image::widgetWantHeght() const
{
	QWidget *parent = parentWidget();
	if(parent != 0)
	{
		return showHeight() + parent->height() - height();
	}
	return showHeight();
}

bool Image::isNull() const
{
	QImage *image = getImage();
	if(image == 0)
		return true;
	return image->isNull();
}

QImage *Image::getImage() const
{
	if(_currentMipmap >= _imageList.size())
		return 0;
	
	return _imageList[_currentMipmap].image;
}

bool Image::lowerMipmap()
{
	if(_currentMipmap+1 < getNumMipmaps())
	{
		_currentMipmap += 1;
		return true;
	}
	return false;
}

bool Image::higherMipmap()
{
	if(_currentMipmap > 0)
	{
		_currentMipmap -= 1;
		return true;
	}
	return false;
}

void Image::loadMipmaps(ILuint imageID)
{
	ILint numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);
	for(int i = 0; i <= numMipmaps; i++)
	{
		ilBindImage(imageID);
		ilActiveMipmap(i);
		ILubyte *data = ilGetData();
		QImage *image = convertToQImage();
		ILint width = ilGetInteger(IL_IMAGE_WIDTH);
		ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
		ILint depth = ilGetInteger(IL_IMAGE_DEPTH);
		ILint imageBPP = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
		ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		ILuint imgID = ilGenImage();
		ilBindImage(imgID);
		ilLoadDataL(data,dataSize,width,height,depth,imageBPP);
		_imageList.push_back(IMAGE(imgID,image));
	}
}

bool Image::genMipmaps()
{
	return false;
}

bool Image::deleteMipmaps()
{
	if(_imageList.size() > 1)
	{
		IMAGE mipmap(_imageList[0]);
		for(int i = 1; i < _imageList.size(); i++)
		{
			ilDeleteImage(_imageList[i].imageID);
			delete _imageList[i].image;
		}
		_imageList.clear();
		_imageList.push_back(mipmap);
		_currentMipmap = 0;
	}
	return false;
}

IMAGE Image::getImageAtList(int pos) const
{
	int n = _imageList.size();
	if(pos >= n)
		return IMAGE(-1,0);
	return _imageList[pos];
}

bool Image::setImageListSize(int size)
{
	int n = _imageList.size();
	if(n >= size)
		return false;
	
	_imageList.reserve(size);
	for(int i = n; i < size; i++)
		_imageList.append(IMAGE(-1,0));
	return true;
}

void Image::setDrawOffset(int x,int y)
{
	_offsetX = (float)x;
	_offsetY = (float)y;
	repaint();
}

QString Image::colorStringAtPos(int x,int y)
{
	if(x < 0 || x > width() ||
		y < 0 || y > height())
		return QString("(R:0  G:0  B:0  A:0)");

	QImage *image  = getImage();
	int imageX = (_offsetX + x) / _zoomX;
	int imageY = (_offsetY + y) / _zoomY;

	if(imageX < 0 || imageX > image->width() ||
		imageY < 0 || imageY > image->height())
		return QString("(R:0  G:0  B:0  A:0)");

	QRgb rgb = image->pixel(imageX,imageY);
	return QString("(R:%1  G:%2  B:%3  A:%4)").arg(qRed(rgb)).arg(qGreen(rgb)).arg(qBlue(rgb)).arg(qAlpha(rgb));
}

int Image::pack(Data_block &db)
{
	int count = getNumMipmaps();
	for(int i = 0; i < count; i++)
	{
		ilBindImage(_imageList[i].imageID);
		db.Data.push_back(ilGetData());
	}
	return count;
}

bool Image::isSameFormat(const Image &image1,const Image &image2, int compress)
{
	if(image1.isNull() || image2.isNull() ||
		image1.getImageType() != image2.getImageType() ||
		(compress != CMD_NONE&&image1.isComplexAlpha() != image2.isComplexAlpha())||
		image1.getNumMipmaps() != image2.getNumMipmaps() ||
		image1.getImageListSize() != image2.getImageListSize())
		return false;

	ilBindImage(image1._imageList[0].imageID);
	ILint width1 = ilGetInteger(IL_IMAGE_WIDTH);
	ILint height1 = ilGetInteger(IL_IMAGE_HEIGHT);
	ILint depth1 = ilGetInteger(IL_IMAGE_DEPTH);
	ILint pixelFormat1 = ilGetInteger(IL_IMAGE_FORMAT);
	ILint imageBPP1 = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	ILint dataType1 = ilGetInteger(IL_IMAGE_TYPE);

	ilBindImage(image2._imageList[0].imageID);
	ILint width2 = ilGetInteger(IL_IMAGE_WIDTH);
	ILint height2 = ilGetInteger(IL_IMAGE_HEIGHT);
	ILint depth2 = ilGetInteger(IL_IMAGE_DEPTH);
	ILint pixelFormat2 = ilGetInteger(IL_IMAGE_FORMAT);
	ILint imageBPP2 = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	ILint dataType2 = ilGetInteger(IL_IMAGE_TYPE);

	if(width1!=width2 || height1!=height2 || depth1!=depth2 ||
		pixelFormat1!=pixelFormat2 || imageBPP1!=imageBPP2||
		dataType1!=dataType2)
		return false;

	return true;
}

Image *Image::compareImage(const Image &image1,const Image &image2)
{
	if(!isSameFormat(image1,image2 , (int)CMD_NONE))
		return 0;

	BMG_header_TT ktxHeader;
	Data_block db;

	int imgListSize = image1.getImageListSize();
	for(int i = 0; i < imgListSize; i++)
	{
		ilBindImage(image1._imageList[i].imageID);
		ILubyte *data1 = ilGetData();
		ILint srcdataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		ilBindImage(image2._imageList[i].imageID);
		ILubyte *data2 = ilGetData();
		ILint dataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
		ILint pixelFormat = ilGetInteger(IL_IMAGE_FORMAT);
		assert(pixelFormat == IL_RGBA);

		ILubyte *data = new ILubyte[dataSize];
		int c1,c2,c3,c4;
		int n = dataSize / 4;
		for(int i = 0; i < n; i++)
		{
			c1 = data1[i*4] - data2[i*4];
			c2 = data1[i*4+1] - data2[i*4+1];
			c3 = data1[i*4+2] - data2[i*4+2];
			c4 = (data1[i*4+3] + data2[i*4+3])/2;
			data[i*4] = c1>0 ? c1 : -c1;
			data[i*4+1] = c2>0 ? c2 : -c2;
			data[i*4+2] = c3>0 ? c3 : -c3;
			data[i*4+3] = c4;
		}
		db.Data.push_back(data);
	}

	ktxHeader.pixelWidth = image1.imageWidth();
	ktxHeader.pixelHeight = image1.imageHeight();
	ktxHeader.pixelDepth = 1;
	ktxHeader.numberOfMipmapLevels = image1.getNumMipmaps();

	Image *image = 0;
	switch(image1.getImageType())
	{
	case IMAGE1D:
		image = new Image1D(ktxHeader,db,0,imgListSize);
		break;
	case IMAGE2D:
		image = new Image2D(ktxHeader,db,0,imgListSize);
		break;
	case IMAGE3D:
		ktxHeader.pixelDepth = imgListSize;
		image = new Image3D(ktxHeader,db,0,imgListSize);
		break;
	case IMAGECUBE:
		ktxHeader.numberOfFaces = imgListSize;
		image = new ImageCube(ktxHeader,db,0,imgListSize);
		break;
	default:
		break;
	}
	return image;
}
ILuint Image::trimImage(ILuint image, int&ofx, int &ofy)
{
	ilBindImage(image);
	ILint width = ilGetInteger(IL_IMAGE_WIDTH);
	ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned char* data = (unsigned char*)malloc(width * height * 4);
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);
	int w, h;
	int offsetx, offsety;
	//up to down
	int up = 0;
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			unsigned char* color = data + j * width*4 + i*4;
			int r = color[0];
			int g = color[1];
			int b = color[2];
			int a = color[3];
			if (a > 0)
				goto upend;
		}
		up++;
	}
upend:
	//down to up
	int down = height;
	for (int j = height - 1; j >= 0; --j)
	{
		for (int i = 0; i < width; ++i)
		{
			unsigned char* color = data + j * width * 4 + i * 4;
			int r = color[0];
			int g = color[1];
			int b = color[2];
			int a = color[3];
			if (a > 0)
				goto downend;
		}
		down--;
	}
downend:
	//left to right
	int left = 0;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			unsigned char* color = data + j * width * 4 + i * 4;
			int r = color[0];
			int g = color[1];
			int b = color[2];
			int a = color[3];
			if (a > 0)
				goto leftend;
		}
		left++;
	}
leftend:
	//right to left
	int right = width;
	for (int i = width - 1; i >= 0; --i)
	{
		for (int j = 0; j < height; ++j)
		{
			unsigned char* color = data + j * width * 4 + i * 4;
			int r = color[0];
			int g = color[1];
			int b = color[2];
			int a = color[3];
			if (a > 0)
				goto rightend;
		}
		right--;
	}
rightend:



	w = right - left;
	h = down - up;
	offsetx = left;
	offsety = up;
	ofx = offsetx;
	ofy = offsety;
	unsigned char* copyp = (unsigned char*)malloc(w*h*4);
	ilCopyPixels(offsetx, offsety, 0, w, h, 1, IL_RGBA, IL_UNSIGNED_BYTE, copyp);
	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilLoadDataL(copyp, sizeof(ILubyte) * w * h * 4, w, h, 1, 4);
	free(data);
	free(copyp);
	ilDeleteImages(1, &image);
	return imageID;
}
Image *Image::createImage(const QString &fileName, int& offsetx, int& offsety, QWidget *parent /* = 0 */, bool trim)
{
	if(fileName.isEmpty())
		return 0;

	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	std::string fileStr = fileName.toStdString();
	ILconst_string file = fileStr.c_str();
	if(!ilLoadImage(file))
	{
		ilDeleteImage(imageID);
		return 0;
	}

	ilBindImage(imageID);
	if(ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA ||
		ilGetInteger(IL_IMAGE_TYPE) != IL_UNSIGNED_BYTE)
		ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);
	if (trim)
		imageID = trimImage(imageID, offsetx, offsety);
	ilBindImage(imageID);
	if (ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA ||
		ilGetInteger(IL_IMAGE_TYPE) != IL_UNSIGNED_BYTE)
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);


	Image *image = 0;
	if(ilGetInteger(IL_IMAGE_HEIGHT) == 1)
	{
		image = new Image1D(imageID);
	}
	else
	{
		int depth = ilGetInteger(IL_IMAGE_DEPTH);
		if(ilGetInteger(IL_IMAGE_DEPTH) == 1)
		{
			if(ilGetInteger(IL_NUM_FACES) == 6)
				image = new ImageCube(imageID);
			else
				image = new Image2D(imageID);
		}
		else
		{
			image = new Image3D(imageID);
		}
	}

	return image;
}

Image* Image::createImage2(Image* img, const QImage* area)
{
	IMAGETYPE type = img->getImageType();
	Image *image = 0;
	ILuint imageID = ilGenImage();
	QImage imaggg = area->convertToFormat(QImage::Format_RGBA8888);
	ilBindImage(imageID);
	int xxx = imaggg.byteCount();
	if (!ilLoadDataL(imaggg.bits(), imaggg.byteCount(), imaggg.width(), imaggg.height(), 1, 4))
	{
		ilDeleteImage(imageID);
		return 0;
	}

	switch (type)
	{
	case IMAGE1D:
		image = new Image1D(imageID);
		break;
	case IMAGE2D:
		image = new Image2D(imageID);
		break;
	case IMAGE3D:
		image = new Image3D(imageID);
		break;
	case IMAGECUBE:
		image = new ImageCube(imageID);
		break;
	default:
		return NULL;
	}
	ilBindImage(imageID);
	if (ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA ||
		ilGetInteger(IL_IMAGE_TYPE) != IL_UNSIGNED_BYTE)
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int w = ilGetInteger(IL_IMAGE_WIDTH);
	int h = ilGetInteger(IL_IMAGE_HEIGHT);
	int d = ilGetInteger(IL_IMAGE_DEPTH);
	int bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int dSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	ILubyte *data = ilGetData();
	if (img->getNumMipmaps() > 1)
		image->genMipmaps();
	image->_cm = img->_cm;
	return image;
}

QList<Image*> Image::createImages(const QString &fileName, std::vector<SpriteSheet>& ssv, int& offsetx, int& offsety, QWidget *parent /* = 0*/)
{
	QList<Image*> images;
	if(fileName.isEmpty())
		return images;

	if(fileName.right(4).toLower() != QString(".bmg"))
	{
		Image *img = createImage(fileName,offsetx, offsety, parent);
		if(img != 0)
			images.append(img);
	}
	else
	{
		std::string fileStr = fileName.toStdString();
		const char *file = fileStr.c_str();

		Data_block dataBlock;
		BMG_header_TT ktxHeader;
		Texture_target tt;
		if(UnCompressKTX(file,dataBlock,ssv, ktxHeader,tt))
		{
			switch(tt)
			{
			case TT_1D:
				images = Image1D::loadKtxData(ktxHeader,dataBlock);
				break;
			case TT_2D:
				images = Image2D::loadKtxData(ktxHeader,dataBlock);
				break;
			case TT_3D:
				images = Image3D::loadKtxData(ktxHeader,dataBlock);
				break;
			case TT_CUBE:
				images = ImageCube::loadKtxData(ktxHeader,dataBlock);
				break;
			default:
				break;
			}
			switch (ktxHeader.fourcc)
			{
			case FOUR_CC('B', 'M', 'G', 'T'):
				images[0]->_cm = CMD_NONE;
				break;
			case FOUR_CC('S', '3', 'T', '1'):
			case FOUR_CC('S', '3', 'T', '2'):
			case FOUR_CC('S', '3', 'T', '3'):
				images[0]->_cm = CM_S3TC;
				break;
			case FOUR_CC('A', 'S', 'T', '1'):
			case FOUR_CC('A', 'S', 'T', '2'):
			case FOUR_CC('A', 'S', 'T', '3'):
				images[0]->_cm = CM_ASTC;
				break;
			case FOUR_CC('E', 'T', 'C', '1'):
			case FOUR_CC('E', 'T', 'C', '2'):
			case FOUR_CC('E', 'T', 'C', '3'):
				images[0]->_cm = CM_ETC2;
				break;
			}
		}
	}

	return images;
}

int Image::ilImageClone(ILuint imageID)
{
	ilBindImage(imageID);
	int w = ilGetInteger(IL_IMAGE_WIDTH);
	int h = ilGetInteger(IL_IMAGE_HEIGHT);
	int d = ilGetInteger(IL_IMAGE_DEPTH);
	int bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	int dSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	ILubyte *data = ilGetData();

	ILubyte *destData = new ILubyte[dSize];
	memcpy(destData,data,sizeof(ILubyte)*dSize);

	ILuint outImg = ilGenImage();
	ilBindImage(outImg);
	ilLoadDataL(destData,dSize,w,h,d,bpp);

	return outImg;
}

void Image::depthCopy(Image &dest,Image &src)
{
	dest._imageList.clear();
	dest.cubeh = src.cubeh;
	dest.cubew = src.cubew;
	int imgNum = src._imageList.size();
	for(int i = 0; i < imgNum; i++)
	{
		IMAGE srcImg(src._imageList[i]);
		dest._imageList.append(IMAGE(ilImageClone(srcImg.imageID),new QImage(*srcImg.image)));
	}
}