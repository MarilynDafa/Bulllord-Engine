#ifndef IMAGE1D_H
#define IMAGE1D_H

#include "Image.h"
#include "IL/il.h"

class Image1D : public Image
{
	Q_OBJECT
public:
	Image1D(ILuint imageID,QWidget *parent = 0);
	Image1D(const BMG_header_TT &kh,Data_block& db,int begin,int size,QWidget *parent = 0);
	Image *clone();
	Image *compressImage(CompressMethord cm, Data_block* db = NULL);
	static QList<Image*> loadKtxData(const BMG_header_TT &kh,Data_block &db);
protected:
	Image1D(QWidget *parent = 0);
	void setZoom(float zoom);
};

#endif //IMAGE1D_H