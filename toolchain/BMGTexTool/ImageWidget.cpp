#include <QtGui/QPainter>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMdiArea>
#include <QtGui/QPalette>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtCore/QDir>
#include <qmessagebox.h>
#include "BMGTexTool.h"
#include <qscrollbar.h>
#include <qpushbutton.h>
#include "ImageWidget.h"
#include "ImageCube.h"

ImageWidget::ImageWidget(QWidget *parent /* = 0 */)
	: QScrollArea(parent)
	, _image(0)
{
	_BmgChanged = false;
	_MultiImage = true;
	setAcceptDrops(true);
	setBackgroundRole(QPalette::Dark);
	QPalette palette;
	palette.setBrush(backgroundRole(),QBrush(QPixmap(QString::fromUtf8(":/BMGTexTool/Resources/background.png"))));
	setPalette(palette);
	setFocusPolicy(Qt::StrongFocus);


	connect(this, SIGNAL(imageAdded(ImageArea*)), parent, SLOT(imageAdded(ImageArea*)));
	connect(this, SIGNAL(imageAreaChanged(ImageArea*)), parent, SLOT(imageAreaChanged(ImageArea*)));
	connect(this, SIGNAL(imageSelected(ImageArea*)), parent, SLOT(imageSelected(ImageArea*)));
	_Parent = parent;
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::setChanged(bool falg)
{
	_BmgChanged = falg;
	if (needSave())
		updateWindowTitle();
	if(!falg)
		updateWindowTitle();
}
bool ImageWidget::IsBMG()
{
	return _IsBMG;
}


void ImageWidget::setImageFileName(QString name)
{
	if (name.contains(".bmg"))
		_IsBMG = true;
	_filePath = name;
	updateWindowTitle();
}
void ImageWidget::setBMG(const QString& path)
{
	if (path.contains(".bmg"))
	{
		_filePath = path;
		_IsBMG = true;
	}
	if(needSave())
		updateWindowTitle();
}
bool ImageWidget::needSave()
{
	if (_IsBMG && _BmgChanged)
		return true;
	else
		return false;
}

bool ImageWidget::changed()
{
	return false;
}
void ImageWidget::setImage(Image *image,const QString filePath, int ofx, int ofy, std::vector<SpriteSheet>* ssv)
{
	if (filePath.contains(".bmg"))
		_IsBMG = true;
	else
		_IsBMG = false;
	if (image->getImageType() == IMAGE2D || image->getImageType() == IMAGE1D)
	{
		if (_image)
		{
			delete _image;
			_image = NULL;
		}

		//_image = image;
		_filePath = filePath;

		QImage* xxx = new QImage(*image->getImage());
		addImage(xxx, filePath, ofx, ofy, ssv);
		_image = Image::createImage2(image, combinToImage());
		setWidget(_image);
		adjustWindowSize();
	}
	else
	{
		if (_image)
		{
			delete _image;
			_image = NULL;
		}
		QImage* xxx = new QImage(*image->getImage());
		addImage(xxx, filePath, ofx, ofy, ssv);
		_image = image;
		_filePath = filePath;
		setWidget(_image);
		adjustWindowSize();
	}
}

void ImageWidget::zoomIn()
{
	_image->zoomIn();
	adjustWindowSize();
}

void ImageWidget::zoomOut()
{
	_image->zoomOut();
	adjustWindowSize();
}

void ImageWidget::resetZoom()
{
	_image->setZoom(1.0f);
	adjustWindowSize();
}

void ImageWidget::adjustWindowSize()
{
	if(_image == 0)
		return;

	QMdiSubWindow *parent = dynamic_cast<QMdiSubWindow*>(parentWidget());
	if(parent != 0)
	{
		QMdiArea *mdiArea = parent->mdiArea();
		if(mdiArea != 0)
		{
			if (_image->getImageType() == IMAGE2D || _image->getImageType() == IMAGE3D)
			{
				int widthx, heightx;
				getImageRect(widthx, heightx);
				const QRect &mr = mdiArea->geometry();
				int w, h, mw, mh;
				w = widthx + parent->width() - width() + 2 * frameWidth();
				h = heightx + parent->height() - height() + 2 * frameWidth();
				mw = mr.width() - parent->x();
				mh = mr.height() - parent->y();
				w = w < mw ? w : mw;
				h = h < mh ? h : mh;
				const QSize &size = parent->sizeHint();
				if (w < size.width()) w = size.width();
				if (h < size.height()) h = size.height();
				parent->resize(w, h);
			}
			else
			{
				const QRect &mr = mdiArea->geometry();
				int w, h, mw, mh;
				w = _image->showWidth() + parent->width() - width() + 2 * frameWidth();
				h = _image->showHeight() + parent->height() - height() + 2 * frameWidth();
				mw = mr.width() - parent->x();
				mh = mr.height() - parent->y();
				w = w < mw ? w : mw;
				h = h < mh ? h : mh;
				const QSize &size = parent->sizeHint();
				if (w < size.width()) w = size.width();
				if (h < size.height()) h = size.height();
				parent->resize(w, h);
			
			}
		}
		_image->adjustWidgetSize();
	}

	updateWindowTitle();
	update();
}

void ImageWidget::moveImageToPoint(ImageArea *imageArea, int x, int y)
{
	if (imageArea != NULL)
	{
		int posX, posY;
		imageArea->getPos(posX, posY);
		int ax = x - posX;
		int ay = y - posY;
		moveImage(imageArea, ax, ay);
	}
}

bool ImageWidget::moveImage(ImageArea *imageArea, int ax, int ay)
{
	if (imageArea != NULL)
	{
		imageArea->movePos(ax, ay);
		setClientSize();
		emit imageAreaChanged(imageArea);
		setModefied(true);
		drawToDevice(this);
		return true;
	}
	return false;
}

void ImageWidget::updateWindowTitle()
{
	if (_image)
	{
		QStringList strs = _filePath.split("/");
		QString title(strs[strs.size() - 1]);
		title.append(QString("		(Mip %1 of %2)").arg(_image->currentMipmap() + 1).arg(_image->getNumMipmaps()));
		title.append(QString("   %1%").arg((int)(_image->getZoom() * 100)));
		if (needSave())
			title.append(QString("*"));;
		setWindowTitle(title);
	}
}

void ImageWidget::generatorMips()
{
	if(_image->genMipmaps())
		setChanged(true);
	adjustWindowSize();
}

void ImageWidget::setSelectImage(ImageArea *image)
{
	_selectedImage = image;
	//_imageAreaList.removeOne(_selectedImage);
	//_imageAreaList.push_back(_selectedImage);

	int count = _imageAreaList.size();
	for (int i = count - 1; i >= 0; i--)
	{
		if (_imageAreaList[i] == image)
		{
			_imageAreaList.removeAt(i);
			_imageAreaList.push_back(image);
		}
	}
	update();
}

void ImageWidget::deleteImage(ImageArea *image)
{
	if (image == 0)
		return;

	emit imageDeleted(image);

	if (_selectedImage == image)
		_selectedImage = 0;

	_imageAreaList.removeOne(image);
	delete image->image;
	delete image;
	setClientSize();
	setModefied(true);
}

void ImageWidget::setModefied2(QString modified)
{
	//if (_modified != modified)
	//{
	bool first = false;
	for (int i = 0; i < _imageAreaList.size(); ++i)
	{
		QFileInfo fileInfo(_imageAreaList[i]->fileName);
		if (modified == getValidName(fileInfo.baseName()))
		{
			first = true;
		}
	}
	if (!first)
		setChanged(true);
	//	_modified = modified;
	//	emit dataModified(modified);
	//}
}

void ImageWidget::setModefied(bool modified)
{
	if (_modified != modified)
	{
		_modified = modified;
		emit dataModified(modified);
	}
}

QString ImageWidget::getValidName(const QString &name)
{
	QListIterator<ImageArea*> it(_imageAreaList);
	while (it.hasNext())
	{
		ImageArea *area = it.next();
		if (area->name == name)
			return getValidName(name + "_");
	}
	return name;
}

void ImageWidget::addImage(QImage *image, const QString &imageName, int ofx, int ofy, std::vector<SpriteSheet>* ssv)
{
	if(ssv && ssv->size()!= 0)
	{
		_MultiImage = false;
		for (int i = 0; i < ssv->size(); ++i)
		{
			ImageArea *imageArea = new ImageArea;
			imageArea->image = image;
			imageArea->fileName = imageName;
			QFileInfo fileInfo(imageName);
			imageArea->name = (*ssv)[i].tag;
			imageArea->x = (*ssv)[i].ltx;
			imageArea->y = (*ssv)[i].lty;
			imageArea->rx = (*ssv)[i].rbx;
			imageArea->ry = (*ssv)[i].rby;
			imageArea->offsetx = (*ssv)[i].offsetx;
			imageArea->offsety = (*ssv)[i].offsety;
			_imageAreaList.push_back(imageArea);
			emit imageAdded(imageArea);
		}
		setClientSize();
		setModefied(true);
	}
	else
	{
		ImageArea *imageArea = new ImageArea;
		imageArea->image = image;
		imageArea->fileName = imageName;
		QFileInfo fileInfo(imageName);
		imageArea->name = getValidName(fileInfo.baseName());
		int hhh = _imageAreaList.size();
		_imageAreaList.push_back(imageArea);
		imageArea->offsetx = ofx;
		imageArea->offsety = ofy;
		setClientSize();
		emit imageAdded(imageArea);
		setModefied(true);
	}
}
void ImageWidget::addImage(ImageArea *image)
{
	if (image == 0)
		return;
	_imageAreaList.push_back(image);
	setClientSize();
	emit imageAdded(image);
	setModefied(true);
}

void ImageWidget::ResetCanves()
{
	Image* gimage = Image::createImage2(_image, combinToImage());
	setWidget(gimage);
	_image = gimage;
	adjustWindowSize();
}

QImage * ImageWidget::combinToImage()
{
	int width, height;
	getImageRect(width, height);
	//getTextureWH(width, height);
	QImage *image = new QImage(width, height, QImage::Format_ARGB32);
	image->fill(QColor(0, 0, 0, 0));
	drawToDevice(image, false); 
	return image;
}
void ImageWidget::deleteAllImage()
{
	int count = _imageAreaList.size();
	for (int i = 0; i < count; i++)
	{
		ImageArea *image = _imageAreaList[i];
		emit imageDeleted(image);
		delete image->image;
		delete image;
	}
	_selectedImage = 0;
	_imageAreaList.clear();
	setModefied(true);
}

void ImageWidget::deleteAllMips(bool repaint/* = true*/)
{
	_image->deleteMipmaps();
	adjustWindowSize();
}

void ImageWidget::lowerMips()
{
	if(_image->lowerMipmap())
		setClientSize();
}

void ImageWidget::higherMips()
{
	if(_image->higherMipmap())
		setClientSize();
}

void ImageWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if(_image->getImageType() == IMAGECUBE)
	{
		if(event->mimeData()->hasUrls())
		{
			const QList<QUrl> &urls = event->mimeData()->urls();
			if(urls.size() == 1)
				event->acceptProposedAction();
		}
	}
}

void ImageWidget::dropEvent(QDropEvent *event)
{
	if(_image->getImageType() == IMAGECUBE)
	{
		if(event->mimeData()->hasUrls())
		{
			const QList<QUrl> &urls = event->mimeData()->urls();
			if(urls.size() == 1)
			{
				event->acceptProposedAction();
				ImageCube *image = dynamic_cast<ImageCube*>(_image);
				if(image)
				{
					image->setFaceImage(urls[0].toLocalFile());
					adjustWindowSize();	
					{
						_imageAreaList[0]->forceRR();
						_imageAreaList[0]->setSize(image->getCubeW(), image->getCubeH());
						emit imageAreaChanged(_imageAreaList[0]);
					}
				}
			}
		}
	}
}


void ImageWidget::closeEvent(QCloseEvent *event)
{
	if (!needSave()) {
		event->accept();
	}
	else {
		QMessageBox msg(this);
		msg.setText("The texture has been modified.");
		msg.setInformativeText("Save your changes?");
		msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msg.setDefaultButton(QMessageBox::Save);
		msg.setLayoutDirection(Qt::LeftToRight);
		msg.setMinimumWidth(400);
		QPushButton* save = (QPushButton*)msg.button(QMessageBox::Save);
		save->setMinimumWidth(75);
		save->setMinimumHeight(23);
		QPushButton* cancel = (QPushButton*)msg.button(QMessageBox::Discard);
		cancel->setMinimumWidth(75);
		cancel->setMinimumHeight(23);
		if (msg.exec() == QMessageBox::Discard)
		{
			event->accept();
		}
		else
		{
			BMGTexTool* bmg = dynamic_cast<BMGTexTool*>(_Parent);
			if (bmg)
			{
				bmg->save();
				event->accept();
			}
		}
	}
}

void ImageWidget::getImageRect(int &width, int &height)
{
	width = 0;
	height = 0;
	int count = _imageAreaList.size();
	for (int i = 0; i < count; i++)
	{
		ImageArea *area = _imageAreaList[i];
		int right, bottom;
		area->getRightBottom(right, bottom);
		if (width < right)
			width = right;
		if (height < bottom)
			height = bottom;
	}
}

void ImageWidget::setClientSize()
{
	int width = 0;
	int height = 0;
	getImageRect(width, height);

	if (width != this->width() || height != this->height())
		adjustWindowSize();
	else
		update();
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
	drawToDevice(this);
}


void ImageWidget::drawToDevice(QPaintDevice *device, bool select /*= true*/)
{
	int count = _imageAreaList.size();
	if (count > 0)
	{
		if (dynamic_cast<QScrollArea*>(device))
		{
			QPainter paint(this->viewport());
			int width = 0, height = 0;
			for (int i = 0; i < count; i++)
			{
				ImageArea *imageArea = _imageAreaList[i];
				//paint.drawImage(imageArea->x, imageArea->y, *(imageArea->image));
				/*if (select && imageArea == _selectedImage)
				{
					int x, y, width, height;
					imageArea->getPos(x, y);
					imageArea->getSize(width, height);
					QPen pen = paint.pen();
					pen.setColor(QColor(200, 100, 100, 200));
					pen.setStyle(Qt::DashLine);
					paint.setPen(pen);
					paint.drawRect(x*_image->getZoom(), y*_image->getZoom(), (width - 1)*_image->getZoom(), (height - 1)*_image->getZoom());
				}*/
			}
		}
		else
		{
			QPainter paint(device);
			int width = 0, height = 0;
			for (int i = 0; i < count; i++)
			{
				ImageArea *imageArea = _imageAreaList[i];
				if(_MultiImage)
					paint.drawImage(imageArea->x, imageArea->y, *(imageArea->image));
				else
				{
					paint.drawImage(0, 0, *(imageArea->image));
					break;
				}
				/*if (select && imageArea == _selectedImage)
				{
					int x, y, width, height;
					imageArea->getPos(x, y);
					imageArea->getSize(width, height);
					QPen pen = paint.pen();
					pen.setColor(QColor(200, 100, 100, 200));
					pen.setStyle(Qt::DashLine);
					paint.setPen(pen);
					paint.drawRect(x, y, width - 1, height - 1);
				}*/
			}
		}
	}
}


void ImageWidget::mousePressEvent(QMouseEvent *event)
{
	_mousePressed = true;
	int x = event->x();
	int y = event->y();
	if (horizontalScrollBar())
	{
		x += horizontalScrollBar()->value();
	}
	if (verticalScrollBar())
	{
		y += verticalScrollBar()->value();
	}
	_selectedImage = pickImage(x, y);
	emit imageSelected(_selectedImage);
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	_mousePressed = false;
}


ImageArea * ImageWidget::pickImage(int x, int y)
{
	int count = _imageAreaList.size();
	int left, top, right, bottom;
	for (int i = count - 1; i >= 0; i--)
	{
		ImageArea *image = _imageAreaList[i];
		image->getArea(left, top, right, bottom);
		left *= _image->getZoom();
		top *= _image->getZoom();
		right *= _image->getZoom();
		bottom *= _image->getZoom();
		if (x >= left && y >= top &&
			x <= right && y <= bottom)
		{
			_imageAreaList.removeAt(i);
			_imageAreaList.push_back(image);
			return image;
		}
	}
	return 0;
}
