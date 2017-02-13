#ifndef IMAGE2D_H
#define IMAGE2D_H

#include "Image.h"

class Image3D;

class Image2D : public Image
{
	Q_OBJECT
public:
	Image2D(ILuint imageID,QWidget *parent = 0);
	Image2D(const BMG_header_TT &kh,Data_block &db,int begin,int size,QWidget *parent = 0);
	Image *clone();
	bool genMipmaps();
	Image *compressImage(CompressMethord cm, Data_block* db = NULL);
	bool canConvert3D();
	Image3D *getImage3D(int width);
	bool isTwos(int num);
	static QList<Image*> loadKtxData(const BMG_header_TT &kh,Data_block &db);
protected:
	Image2D(QWidget *parent = 0);
};

#endif //IMAGE2D_H