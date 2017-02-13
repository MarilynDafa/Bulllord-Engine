#include "Image1D.h"
#include "Codec.h"

Image1D::Image1D(QWidget *parent /* = 0 */)
	: Image(IMAGE1D,parent)
{

}

Image1D::Image1D(ILuint imageID,QWidget *parent /* = 0 */)
	: Image(IMAGE1D,parent)
{
	ilBindImage(imageID);
	_imageList.push_back(IMAGE(imageID,convertToQImage()));
}

Image1D::Image1D(const BMG_header_TT &kh,Data_block& db,int begin,int size,QWidget *parent /* = 0 */)
	: Image(IMAGE1D,parent)
{
	assert(kh.pixelHeight == 0);
	ILuint imageID = genImage(db.Data[begin],kh.pixelWidth,1);
	ilBindImage(imageID);
	_imageList.push_back(IMAGE(imageID,convertToQImage()));

	if(kh.fourcc != FOUR_CC('B', 'M', 'G', 'T'))
		_canCompress = false;
}

QList<Image*> Image1D::loadKtxData(const BMG_header_TT &kh,Data_block &db)
{
	QList<Image*> images;
	int tmp = kh.numberOfArrayElements;// == 0) ? 0 : kh.numberOfArrayElements - 1;
	for(int i = 0; i < tmp; i++)
	{
		Image *image = new Image1D(kh,db,i,1);
		images.append(image);
	}
	return images;
}

void Image1D::setZoom(float zoom)
{
	_offsetY = _offsetY*zoom/_zoomY;
	_zoomY = zoom;
	repaint();
}

Image* Image1D::compressImage(CompressMethord cm, Data_block* _db)
{
	ilBindImage(_imageList[0].imageID);

	BMG_header_TT header;
	header.pixelDepth = 1;
	header.pixelWidth = ilGetInteger (IL_IMAGE_WIDTH);
	header.pixelHeight = 1;
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
			encode_bmg(header, db, TT_1D, at);
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
			encode_s3tc(header,db,TT_1D,at);

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
			encode_etc2(header,db,TT_1D,at);
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
			encode_astc(header,db,TT_1D,at);
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

	Image *image = new Image1D(header,db,0,db.Data.size());
	return image;
}

Image *Image1D::clone()
{
	Image *image = new Image1D;
	Image::depthCopy(*image,*this);
	return image;
}