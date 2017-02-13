#ifndef IMAGE3D_H
#define IMAGE3D_H

#include "Image.h"

class Image3D : public Image
{
	Q_OBJECT
public:
	Image3D(ILuint imageID,QWidget *parent = 0);
	Image3D(const BMG_header_TT &kh,Data_block &db,int begin,int size,QWidget *parent = 0);
	int getNumMipmaps() const { return 1; }
	bool preVolume();
	bool nextVolume();
	QImage *getImage() const;
	Image *clone();
	Image *compressImage(CompressMethord cm, Data_block* db = NULL);
	int pack(Data_block &db);
	virtual int imageDepth() const { return getNumVolume(); }
	static QList<Image*> loadKtxData(const BMG_header_TT &kh,Data_block &db);
protected:
	Image3D(QWidget *parent = 0);
private:
	int getNumVolume() const;
	IMAGE getImage(int volume);

private:
	int _currentVolume;
};

#endif //IMAGE3D_H