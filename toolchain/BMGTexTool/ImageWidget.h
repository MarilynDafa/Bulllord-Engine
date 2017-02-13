#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QtWidgets/QScrollArea>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QWidget>
#include <QtGui/QDragEnterEvent>
#include <QtCore/QList>

#include "Image.h"


struct ImageArea
{
	int x, y;
	int forcer;
	int rx, ry;
	QImage *image;
	QString name;
	QString fileName;
	int offsetx, offsety;

	ImageArea()
	{
		int x = y = rx = ry = 0;
		forcer = 0;
		setPos(0, 0);
		image = 0;
		offsetx = 0;
		offsety = 0;
	}

	void forceRR()
	{
		forcer = 1;
	}

	QString toAreaString()
	{
		int left, top, right, bottom;
		getArea(left, top, right, bottom);
		if(forcer)
			return QString("( %1, %2, %3, %4 )").arg(left).arg(top).arg(rx).arg(ry);
		else
			return QString("( %1, %2, %3, %4 )").arg(left).arg(top).arg(right).arg(bottom);
	}

	QString toCoordString()
	{
		int left, top, right, bottom;
		getArea(left, top, right, bottom);
		return QString("%1,%2,%3,%4").arg(left).arg(top).arg(right).arg(bottom);
	}

	void setPos(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	void setOffset(int x, int y)
	{
		this->offsetx = x;
		this->offsety = y;
	}
	int getOffsetX() { return offsetx; }
	int getOffsetY() { return offsety; }

	void getPos(int &x, int &y)
	{
		x = this->x;
		y = this->y;
	}

	void movePos(int ax, int ay)
	{
		x += ax;
		y += ay;
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
	}

	void getRightBottom(int &right, int &bottom)
	{
		assert(image != 0);
		if (image && !rx && !ry)
		{
			right = this->x + image->width();
			bottom = this->y + image->height();
		}
		else
		{
			right = rx;
			bottom = ry;
		}
	}

	void setArea(int left, int top, int right, int bottom)
	{
		//assert(image != 0);
		this->x = left;
		this->y = top;
		this->rx = right;
		this->ry = bottom;
		//bottom = this->y + image->height();
	}

	void setSize(int right, int bottom)
	{
		//assert(image != 0);
		this->rx = right;
		this->ry = bottom;
		//bottom = this->y + image->height();
	}

	void getArea(int &left, int &top, int &right, int &bottom)
	{
		//assert(image != 0);
		if (image && !rx && !ry)
		{
			left = this->x;
			top = this->y;
			right = this->x + image->width();
			bottom = this->y + image->height();
		}
		else
		{
			left = this->x;
			top = this->y;
			right = rx;
			bottom = ry;
		}
	}

	void getSize(int &width, int &height)
	{
		assert(image != 0);
		if (image && !rx && !ry)
		{
			width = image->width();
			height = image->height();
		}
		else
		{
			width = rx - x;
			height = ry - y;
		}
	}
};

struct ImageAreaRef
{
	ImageAreaRef()
	{
		imageArea = 0;
	}

	ImageAreaRef(ImageArea *data)
	{
		imageArea = data;
	}

	ImageArea *imageArea;
};
Q_DECLARE_METATYPE(ImageAreaRef)
class ImageWidget : public QScrollArea , public ImageOp
{
	Q_OBJECT
signals :
	void imageAreaChanged(ImageArea *image);
	void imageAdded(ImageArea *image);
	void imageSelected(ImageArea *image);
	void imageDeleted(ImageArea *image);
	void dataModified(bool m);
public:
	ImageWidget(QWidget *parent = 0);
	virtual ~ImageWidget();
	void setImage(Image *image,const QString filePath, int ofx, int ofy, std::vector<SpriteSheet>* ssv = NULL);
	Image *getImage() { return _image; }
	QString getImageFileName() { return _filePath; }
	void setImageFileName(QString name);
	bool IsBMG();
	bool needSave();
	bool changed();
	void zoomIn();
	void zoomOut();
	void resetZoom();
	void generatorMips();
	void deleteAllMips(bool repaint = true);
	void lowerMips();
	void higherMips();
	void setSelectImage(ImageArea *image);
	void deleteImage(ImageArea *image);
	void deleteAllImage();
	void setModefied2(QString modified);
	void setModefied(bool modified);
	void setBMG(const QString& path);
	void setChanged(bool falg = true);

	void moveImageToPoint(ImageArea *imageArea, int x, int y);

	bool moveImage(ImageArea *imageArea, int ax, int ay);

	QString getValidName(const QString &name);
	void addImage(QImage *image, const QString &imageName, int ofx, int ofy, std::vector<SpriteSheet>* ssv = NULL);
	void addImage(ImageArea *image);
	QList<ImageArea*> _imageAreaList;

	void ResetCanves();
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual void closeEvent(QCloseEvent *event);

private slots:
	void updateWindowTitle();
	void adjustWindowSize();
	void setClientSize();
	void getImageRect(int &width, int &height);

	void drawToDevice(QPaintDevice *device, bool select = true);
	QImage *combinToImage();
	ImageArea * pickImage(int x, int y);

private:
	ImageArea *_selectedImage;
	Image *_image;
	QString _filePath;
	bool _modified;
	bool _mousePressed;
	bool _MultiImage;
	bool _IsBMG;
	bool _BmgChanged;
	QWidget* _Parent;
};

#endif //IMAGEWINDOW_H