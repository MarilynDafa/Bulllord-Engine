#include "ImageCompareWidget.h"
#include "Codec.h"
#include "Image.h"

#include <QtWidgets/QScrollBar>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialog>

ImageCompareWidget::ImageCompareWidget(QWidget *parent /* = 0 */)
	: QMdiArea(parent)
	, _srcImage(0)
	, _compareImage(0)
	, _compressImage(0)
	, _srcSubWidget(0)
	, _compareSubWidget(0)
	, _compressSubWidget(0)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(scrollBarValueChanged(int)));
	connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(scrollBarValueChanged(int)));

	setBackground(QBrush(QPixmap(QString::fromUtf8(":/BMGTexTool/Resources/background.png"))));
}

void ImageCompareWidget::setImage(Image *image,QString &filePath,CompressMethord cm)
{
	_cm = cm;
	_filePath = filePath;
	_srcImage = image->clone();
	compressImage();
	compareImage();

	_srcSubWidget = addSubWindow(_srcImage,Qt::WindowTitleHint);
	_compareSubWidget = addSubWindow(_compareImage,Qt::WindowTitleHint);
	_compressSubWidget = addSubWindow(_compressImage,Qt::WindowTitleHint);

	connect(_srcImage,SIGNAL(mouseMove(int,int)),this,SLOT(imageWidgetMouseMove(int,int)));
	connect(_compareImage,SIGNAL(mouseMove(int,int)),this,SLOT(imageWidgetMouseMove(int,int)));
	connect(_compressImage,SIGNAL(mouseMove(int,int)),this,SLOT(imageWidgetMouseMove(int,int)));

	setWindowTitle(_filePath);
}

void ImageCompareWidget::compressImage()
{
	if(_srcImage == 0)
		return;

	_compressImage = _srcImage->compressImage(_cm);
}

void ImageCompareWidget::compareImage()
{
	if(_srcImage == 0)
		return;

	if(_compressImage == 0)
		compressImage();

	_compareImage = Image::compareImage(*_srcImage,*_compressImage);
}

QList<Image*> ImageCompareWidget::getImages()
{
	QList<Image*> images;
	if(_srcImage != 0) images.append(_srcImage);
	if(_compressImage != 0) images.append(_compressImage);
	if(_compareImage != 0) images.append(_compareImage);
	return images;
}

void ImageCompareWidget::adjustWindowSize()
{
	QMdiSubWindow *subWindow = dynamic_cast<QMdiSubWindow*>(parentWidget());
	if(subWindow != 0)
	{
		QMdiArea *mdiArea = subWindow->mdiArea();
		if(mdiArea != 0)
		{
			int subWindowFrameWidth = subWindow->width() - width();
			int subWindowFrameHeight = subWindow->height() - height();
			int imageWindowFrameWidth = _srcSubWidget->width() - _srcImage->width();
			int imageWindowFrameHeight = _srcSubWidget->height() - _srcImage->height();
			int wantWidth = 0 , wantHeight = 0;
			if(_srcImage->getImageType() == IMAGE1D)
			{
				wantWidth = _srcImage->showWidth()+imageWindowFrameWidth+subWindowFrameWidth;
				wantHeight = (_srcImage->showHeight()+imageWindowFrameHeight)*3+subWindowFrameHeight;
			}
			else
			{
				wantWidth = (_srcImage->showWidth()+imageWindowFrameWidth)*3+subWindowFrameWidth;
				wantHeight = _srcImage->showHeight()+imageWindowFrameHeight+subWindowFrameHeight;
			}
			const QRect &subWindowRect = subWindow->frameGeometry();
			int maxWidth = mdiArea->width() - subWindowRect.left();
			int maxHeight = mdiArea->height() - subWindowRect.top();
			int subWidth = wantWidth<maxWidth ? wantWidth : maxWidth;
			int subHeight = wantHeight<maxHeight ? wantHeight : maxHeight; 
			subWindow->resize(subWidth,subHeight+20);
			adjustImageWidgetSize();
		}
	}
}

void ImageCompareWidget::adjustImageWidgetSize()
{
	IMAGETYPE imgType = _srcImage->getImageType();
	QSize size = viewport()->size();
	int imageW = (imgType == IMAGE1D ? width() : width() / 3);
	int imageH = (imgType == IMAGE1D ? height() / 3 : height());
	int imageWidgetFrameW = _srcSubWidget->width() - _srcImage->width();
	int imageWidgetFrameH = _srcSubWidget->height() - _srcImage->height();
	int imageWidgetWantW = _srcImage->showWidth() + imageWidgetFrameW;
	int imageWidgetWantH = _srcImage->showHeight() + imageWidgetFrameH;
	if(imageW < imageWidgetWantW)
	{
		imageW = (imgType == IMAGE1D ? size.width() : size.width() / 3);
	}
	if(imageH < imageWidgetWantH)
	{
		imageH = (imgType == IMAGE1D ? size.height() / 3 : size.height());
	}

	int imageShowW = imageW - imageWidgetFrameW;
	int imageShowH = imageH - imageWidgetFrameH;

	if(imageShowW > _srcImage->showWidth())
	{
		imageShowW = _srcImage->showWidth();
		imageW = imageShowW + imageWidgetFrameW;
	}

	if(imageShowH > _srcImage->showHeight())
	{
		imageShowH = _srcImage->showHeight();
		imageH = imageShowH + imageWidgetFrameH;
	}

	verticalScrollBar()->setRange(0,_srcImage->showHeight() - imageShowH);
	horizontalScrollBar()->setRange(0,_srcImage->showWidth() - imageShowW);

	if(imgType == IMAGE1D)
	{
		_srcSubWidget->setGeometry(0,0,imageW,imageH);
		_compareSubWidget->setGeometry(0,imageH,imageW,imageH);
		_compressSubWidget->setGeometry(0,imageH*2,imageW,imageH);
	}
	else
	{
		_srcSubWidget->setGeometry(0,0,imageW,imageH);
		_compareSubWidget->setGeometry(imageW,0,imageW,imageH);
		_compressSubWidget->setGeometry(imageW*2,0,imageW,imageH);
	}
}

void ImageCompareWidget::resizeEvent(QResizeEvent *resizeEvent)
{
	adjustImageWidgetSize();
}

void ImageCompareWidget::getScrollOffset(int &offsetX,int &offsetY)
{
	offsetX =0, offsetY = 0;
	if(verticalScrollBar()->isVisible())
		offsetY = verticalScrollBar()->value();
	if(horizontalScrollBar()->isVisible())
		offsetX = horizontalScrollBar()->value();
}

void ImageCompareWidget::setImageOffset(int offsetX,int offsetY)
{
	_srcImage->setDrawOffset(offsetX,offsetY);
	_compareImage->setDrawOffset(offsetX,offsetY);
	_compressImage->setDrawOffset(offsetX,offsetY);
}

void ImageCompareWidget::scrollBarValueChanged(int value)
{
	adjustImageWidgetSize();
	int offsetX,offsetY;
	getScrollOffset(offsetX,offsetY);
	setImageOffset(offsetX,offsetY);
}

bool ImageCompareWidget::eventFilter(QObject * object, QEvent * event )
{
	QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
	if(mouseEvent)
	{
		QMdiSubWindow *subWindow = dynamic_cast<QMdiSubWindow*>(object);
		if(subWindow == _srcSubWidget ||
			subWindow == _compareSubWidget ||
			subWindow == _compressSubWidget)
		{
			return true;
		}
	}
	return false;
}

void ImageCompareWidget::updateSubWidgetTitle(int x,int y)
{
	_srcSubWidget->setWindowTitle(QString(tr("Original		")).append(_srcImage->colorStringAtPos(x,y)));
	_compareSubWidget->setWindowTitle(QString(tr("Difference		")).append(_compareImage->colorStringAtPos(x,y)));
	_compressSubWidget->setWindowTitle(QString(tr("Compressed		")).append(_compressImage->colorStringAtPos(x,y)));
}

void ImageCompareWidget::imageWidgetMouseMove(int x,int y)
{
	updateSubWidgetTitle(x,y);
}

void ImageCompareWidget::zoomIn()
{
	_srcImage->zoomIn();
	_compareImage->zoomIn();
	_compressImage->zoomIn();
	adjustWindowSize();
}

void ImageCompareWidget::zoomOut()
{
	_srcImage->zoomOut();
	_compareImage->zoomOut();
	_compressImage->zoomOut();
	adjustWindowSize();
}

void ImageCompareWidget::resetZoom()
{
	_srcImage->setZoom(1.0f);
	_compareImage->setZoom(1.0f);
	_compressImage->setZoom(1.0f);
	adjustWindowSize();
}

void ImageCompareWidget::lowerMips()
{
	_srcImage->lowerMipmap();
	_compareImage->lowerMipmap();
	_compressImage->lowerMipmap();
	adjustWindowSize();
}

void ImageCompareWidget::higherMips()
{
	_srcImage->higherMipmap();
	_compareImage->higherMipmap();
	_compressImage->higherMipmap();
	adjustWindowSize();
}