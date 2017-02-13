#ifndef IMAGE_H
#define IMAGE_H

#include <assert.h>

#include <QtGui/QImage>
#include <QtWidgets/QWidget>
extern "C" {
#include "IL/il.h"
#include "IL/ilu.h"
}
#include "BMGHeader.h"

enum FACE
{
	POSITIVE_X = 0,
	NEGATIVE_X,
	POSITIVE_Y,
	NEGATIVE_Y,
	POSITIVE_Z,
	NEGATIVE_Z,
	FACECOUNT
};

enum IMAGETYPE
{
	IMAGE1D,
	IMAGE2D,
	IMAGE3D,
	IMAGECUBE,
	IMAGENO
};

struct IMAGE
{
	QImage *image;
	int imageID;
	IMAGE(int imgID = -1,QImage*img = 0)
	{
		image = img;
		imageID = imgID;
	}
	IMAGE(const IMAGE &mip)
	{
		image = mip.image;
		imageID = mip.imageID;
	}
	bool valid()
	{
		if(image == 0 || !ilIsImage(imageID))
			return false;
		return true;
	}
};

class Image;

struct ImageArea;

class ImageOp
{
public:
	virtual void zoomIn() {}
	virtual void zoomOut() {}
	virtual void resetZoom() {}
	virtual void generatorMips() {}
	virtual void deleteAllMips(bool repaint = true) {}
	virtual void lowerMips() {}
	virtual void higherMips() {}
	virtual void preVolume();
	virtual void nextVolume();
	virtual Image *getImage() { return 0; }
	virtual QList<Image*> getImages();
	virtual QString getImageFileName() { return ""; }
	virtual int pack(Data_block &db);
	virtual void getKtxHeader(BMG_header_TT &kh);
	virtual Texture_target getTextureTarget();
	virtual Alpha_type getAlphaType();
	virtual void setFace(FACE face);
	virtual IMAGETYPE getImageType();
	virtual CompressMethord getCompressMethod() { return CMD_NONE; }
	static bool isSameFormatImage(ImageOp &image1,ImageOp &image2);
};

class Image : public QWidget
{
	Q_OBJECT
public:
	Image(IMAGETYPE type = IMAGENO,QWidget *parent  = 0);
	virtual ~Image();
	virtual int imageWidth() const;
	virtual int imageHeight() const;
	virtual int imageDepth() const { return 1; }
	int showWidth() const;
	int showHeight() const;
	bool isNull() const;
	virtual QImage *getImage() const;
	int getImageListSize() const { return _imageList.size(); }
	virtual int getNumMipmaps() const { return _imageList.size(); }
	unsigned int currentMipmap() const { return _currentMipmap; }
	void setCurrentMipmap(unsigned int currentMipmap) { _currentMipmap = currentMipmap; }
	virtual void setCurrentFace(FACE face) {}
	virtual FACE getCurrentFace() {return NEGATIVE_X;}
	virtual int getFaceCount() { return 1; }
	bool lowerMipmap();
	bool higherMipmap();
	virtual bool preVolume() { return false; }
	virtual bool nextVolume() { return false; }
	virtual bool genMipmaps();
	bool deleteMipmaps();
	Alpha_type isComplexAlpha() const;
	void adjustWidgetSize();
	virtual void setZoom(float zoom);
	float getZoom() { return _zoomY; }
	void zoomIn();
	void zoomOut();
	virtual Image *clone() { return 0; }
	virtual Image *compressImage(CompressMethord cmd, Data_block* db = NULL) { return 0; };
	void setDrawOffset(int x,int y);
	QString colorStringAtPos(int x,int y);
	int widgetWantWidth() const;
	int widgetWantHeght() const;
	virtual int pack(Data_block &db);
	IMAGETYPE getImageType() const { return _imgType; }
	IMAGE getImageAtList(int pos) const;
	virtual bool canConvert3D() { return false; }
	bool canCompress() { return _canCompress; }
	static bool isSameFormat(const Image &image1,const Image &image2 ,int compress);
	static Image *compareImage(const Image &image1,const Image &image2);
	static Image* createImage(const QString &fileName, int& offsetx, int& offsety, QWidget *parent  = 0, bool trim = true);

	static ILuint trimImage(ILuint image, int&x, int &y);
	static QList<Image*> createImages(const QString &fileName, std::vector<SpriteSheet>& ssv, int& offsetx, int& offsety, QWidget *parent  = 0);


	static Image* createImage2(Image* img, const QImage* area);
	static void depthCopy(Image &dest,Image &src);
	static int ilImageClone(ILuint imageID);
	float zoomX() { return _zoomX; }
	float zoomY() { return _zoomY; }
	CompressMethord getCM() { return _cm; }
signals:
	void mouseMove(int x,int y);
protected:
	QImage *convertToQImage();
	void loadMipmaps(ILuint imageID);
	int currentImageWidth() const;
	int currentImageHeight() const;
	ILuint genImage(unsigned char *data,int w,int h,int bpp = 4);
	bool setImageListSize(int size);
protected:
	virtual void paintEvent(QPaintEvent *paintEvent);
	virtual void mouseMoveEvent(QMouseEvent *event);

protected:
	QList<IMAGE> _imageList;
	unsigned int _currentMipmap;
	float _zoomX;
	float _zoomY;
	float _offsetX;
	float _offsetY;
	IMAGETYPE _imgType;
	bool _canCompress;
	int cubew, cubeh;
	CompressMethord _cm;
};

#endif //IMAGE_H