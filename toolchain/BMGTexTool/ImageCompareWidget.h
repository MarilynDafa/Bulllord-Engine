#ifndef IMAGECOMPREWIDGET_H
#define IMAGECOMPREWIDGET_H

#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtGui/QMouseEvent>
#include "Image.h"

class ImageCompareWidget : public QMdiArea , public ImageOp
{
	Q_OBJECT
public:
	ImageCompareWidget(QWidget *parent = 0);
	void setImage(Image *image,QString &filePath,CompressMethord cm);
	void adjustWindowSize();
	void zoomIn();
	void zoomOut();
	void resetZoom();
	void lowerMips();
	void higherMips();
	CompressMethord getCompressMethod() { return _cm; }
	QString getImageFileName() { return _filePath; }
	Image *getImage() { return _srcImage; }
	QList<Image*> getImages();
private:
	void compressImage();
	void compareImage();
	void adjustImageWidgetSize();
	void getScrollOffset(int &offsetX,int &offsetY);
	void setImageOffset(int offsetX,int offsetY);
	void updateSubWidgetTitle(int x,int y);
protected:
	virtual bool eventFilter(QObject *object, QEvent *event);
	virtual void resizeEvent(QResizeEvent *resizeEvent);
protected slots:
	void scrollBarValueChanged(int value);
	void imageWidgetMouseMove(int x,int y);
private:
	QString _filePath;
	Image *_srcImage;
	Image *_compareImage;
	Image *_compressImage;
	QMdiSubWindow *_srcSubWidget;
	QMdiSubWindow *_compareSubWidget;
	QMdiSubWindow *_compressSubWidget;
	CompressMethord _cm;
};

#endif //IMAGECOMPREWIDGET_H