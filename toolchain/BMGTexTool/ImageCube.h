#ifndef IMAGECUBE_H
#define IMAGECUBE_H

#include "Image.h"
#include "Image2D.h"

class ImageCube : public Image
{
	Q_OBJECT
public:
	ImageCube(QWidget *parent = 0);
	ImageCube(ILuint imageID,QWidget *parent  = 0);
	ImageCube(const BMG_header_TT &kh,Data_block &db,int begin,int size,QWidget *parent = 0);
	void setCurrentFace(FACE face);
	FACE getCurrentFace() { return _currentFace; }
	int getFaceCount() { return FACECOUNT; }
	bool setFaceImage(FACE face,Image *image);
	bool setFaceImage(FACE face,QString imageFile);
	bool setFaceImage(Image *image);
	bool setFaceImage(QString imageFile);
	int imageWidth() const;
	int imageHeight() const;
	Image *clone();
	Image *compressImage(CompressMethord cm, Data_block* db = NULL);
	int getNumMipmaps() const;
	QImage *getImage() const;
	int pack(Data_block &db);
	bool genMipmaps();
	IMAGE getValidFaceImage() const;
	bool isVaildCubeMap() { return _defaultImgMask == 0x3f; }
	static QList<Image*> loadKtxData(const BMG_header_TT &kh,Data_block &db);
	int getCubeW() const { return cubew; }
	int getCubeH() const { return cubeh; }
private:
	void createDefault();
	void deleteFaceImage(FACE face,int mip = 0);
	IMAGE getImage(FACE face,int mip = 0);
	int getImagePosAtList(FACE face,int mip = 0) const;
	void setImage(FACE face,int mip,const ILubyte *data,int width,int height);

private:
	unsigned char _defaultImgMask;
	FACE _currentFace;
	bool allok[FACECOUNT];

	//const static int _defaultWH = 512;
	const static ILubyte _defaultR = 55;
	const static ILubyte _defaultG = 230;
	const static ILubyte _defaultB = 100;
	const static ILubyte _defaultA = 200;
};

#endif //IMAGECUBE_H