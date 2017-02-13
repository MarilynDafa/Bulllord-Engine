#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets>

#include "SkinSetDialog.h"



ImageWidget::ImageWidget( QWidget *parent /* = 0 */ )
	: QWidget(parent)
	, _sx(1.0f)
	, _sy(1.0f)
	, _ox(0.f)
	, _oy(0.f)
{

}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::setImage( const QImage &image )
{
	_image = image;
	updateLayout();
	update();
}

void ImageWidget::setScale( float sx, float sy )
{
	_sx = sx;
	_sy = sy;
	updateLayout();
	update();
}

void ImageWidget::paintEvent( QPaintEvent *pe )
{
	QWidget::paintEvent(pe);

	if(!_image.isNull())
	{
		QPainter painter(this);
		QSize imageSize = _image.size();
		QRectF rectf(_ox, _oy, imageSize.width() * _sx, imageSize.height() * _sy);
		painter.drawImage(rectf, _image);
	}
}

void ImageWidget::resizeEvent( QResizeEvent *re )
{
	updateLayout();
	update();
}

void ImageWidget::updateLayout()
{
	if(_image.isNull())
		return;

	QSize imgSize = _image.size();
	QSize widgetSize = size();

	_ox = (widgetSize.width() - imgSize.width() * _sx) / 2.0f;
	_oy = (widgetSize.height() - imgSize.height() * _sy) / 2.0f;
}

QRect ImageWidget::getImageRect()
{
	return QRect(_ox, _oy, _image.width() * _sx, _image.height() * _sy);
}

//////////////////////////////////////////////////////////////////////////

Image9Widget::Image9Widget( QWidget *parent /*= 0*/ )
	: ImageWidget(parent)
	, _scale(1.0f)
{
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Midlight);
	QPalette palette;

	palette.setBrush(backgroundRole(),QBrush(QPixmap(QString::fromUtf8(":/Resources/background.png"))));
	setPalette(palette);
}

Image9Widget::~Image9Widget()
{

}

void Image9Widget::paintEvent( QPaintEvent *pe )
{
	if(_image.isNull())
		return;

	QPainter painter(this);

	if(isRectValid(_tlD) && isRectValid(_tlS))
		painter.drawImage(_tlD, _image, _tlS);

	if(isRectValid(_topD) && isRectValid(_topS))
		painter.drawImage(_topD, _image, _topS);

	if(isRectValid(_trD) && isRectValid(_trS))
		painter.drawImage(_trD, _image, _trS);

	if(isRectValid(_leftD) && isRectValid(_leftS))
		painter.drawImage(_leftD, _image, _leftS);

	if(isRectValid(_centerD) && isRectValid(_centerS))
		painter.drawImage(_centerD, _image, _centerS);
	
	if(isRectValid(_rightD) && isRectValid(_rightS))
		painter.drawImage(_rightD, _image, _rightS);
	
	if(isRectValid(_blD) && isRectValid(_blS))
		painter.drawImage(_blD, _image, _blS);
	
	if(isRectValid(_bottomD) && isRectValid(_bottomS))
		painter.drawImage(_bottomD, _image, _bottomS);
	
	if(isRectValid(_brD) && isRectValid(_brS))
		painter.drawImage(_brD, _image, _brS);
}

void Image9Widget::setImageCenterRect( const QRect &rect )
{
	_centerS = rect;
	updateLayout();
	update();
}

void Image9Widget::updateLayout()
{
	if(_image.isNull())
		return;

	QSize imgSize = _image.size();
	QSize destSize = QSize(imgSize.width() * _sx, imgSize.height() * _sy);
	QSize widgetSize = size();

	_ox = (widgetSize.width() - destSize.width()) / 2.0f;
	_oy = (widgetSize.height() - destSize.height()) / 2.0f;

	if(_centerS.width() == 0)
	{
		_centerS.setX(0);
		_centerS.setWidth(imgSize.width());
	}

	if(_centerS.height() == 0)
	{
		_centerS.setY(0);
		_centerS.setHeight(imgSize.height());
	}

	_tlS = QRect(0, 0, _centerS.left(), _centerS.top());
	_topS = QRect(_centerS.left(), 0, _centerS.width(), _centerS.top());
	_trS = QRect(_centerS.right(), 0, imgSize.width() - _centerS.right(), _centerS.top());
	_leftS = QRect(0, _centerS.top(), _centerS.left(), _centerS.height());
	_rightS = QRect(_centerS.right(), _centerS.top(), imgSize.width() - _centerS.right(), _centerS.height());
	_blS = QRect(0, _centerS.bottom(), _centerS.left(), imgSize.height() - _centerS.bottom());
	_bottomS = QRect(_centerS.left(), _centerS.bottom(), _centerS.width(), imgSize.height() - _centerS.bottom());
	_brS = QRect(_centerS.right(), _centerS.bottom(), imgSize.width() - _centerS.right(), imgSize.height() - _centerS.bottom());

	_centerD = QRect(_ox + _centerS.left(), _oy + _centerS.top(), destSize.width() - _leftS.width() - _rightS.width(), destSize.height() - _topS.height() - _bottomS.height());

	_tlD = QRect(_ox, _oy, _centerS.left(), _centerS.top());
	_topD = QRect(_centerD.left(), _oy, _centerD.width(), _centerS.top());
	_trD = QRect(_centerD.right(), _oy, imgSize.width() - _centerS.right(), _centerS.top());
	_leftD = QRect(_ox, _centerD.top(), _centerS.left(), _centerD.height());
	_rightD = QRect(_centerD.right(), _centerD.top(), imgSize.width() - _centerS.right(), _centerD.height());
	_blD = QRect(_ox, _centerD.bottom(), _centerS.left(), imgSize.height() - _centerS.bottom());
	_bottomD = QRect(_centerD.left(), _centerD.bottom(), _centerD.width(), imgSize.height() - _centerS.bottom());
	_brD = QRect(_centerD.right(), _centerD.bottom(), imgSize.width() - _centerS.right(), imgSize.height() - _centerS.bottom());
}

void Image9Widget::setShowSize( const QSize &size, float scale /*= 1*/ )
{
	if(_image.isNull())
		return;

	QSize imageSize = _image.size();
	_sx = (float)size.width() / (float)imageSize.width() * scale;
	_sy = (float)size.height() / (float)imageSize.height() * scale;

	updateLayout();
	update();
}

bool Image9Widget::isRectValid( const QRect &rect )
{
	if(rect.isValid())
	{
		QSize size = rect.size();
		if(size.width() >0 && size.height() > 0)
			return true;
	}

	return false;
}

void Image9Widget::slotCenterRectChanged( const QRect &rect )
{
	setImageCenterRect(rect);
}

void Image9Widget::setScale( float scale )
{
	_sx /= _scale;
	_sy /= _scale;
	_scale = scale;
	_sx *= _scale;
	_sy *= _scale;
	updateLayout();
	update();
}

void Image9Widget::setScale( float sx, float sy )
{
	_sx = sx * _scale;
	_sy = sy * _scale;
	updateLayout();
	update();
}

//////////////////////////////////////////////////////////////////////////

ImageSetWidget::ImageSetWidget( QWidget *parent /*= 0*/ )
	: ImageWidget(parent)
{
}

ImageSetWidget::~ImageSetWidget()
{

}

void ImageSetWidget::paintEvent( QPaintEvent *pe )
{
	ImageWidget::paintEvent(pe);

	QPoint org(_ox, _oy);
	QRect selRect(_selRect.x() * _sx, _selRect.y() * _sy, _selRect.width() * _sx, _selRect.height() * _sy);

	QPainter painter(this);
	painter.setPen(QColor(255, 255, 255));
	painter.drawRect(QRect(selRect.topLeft() + org, selRect.bottomRight() + org));
}

void ImageSetWidget::mousePressEvent( QMouseEvent *me )
{
	if(me->button() == Qt::LeftButton)
	{
		_stPt = me->pos();
		_endPt = me->pos();
		emit signalSelectRectChanged(getSelRect());
		_mousePressed = true;
	}
}

void ImageSetWidget::mouseReleaseEvent( QMouseEvent *me )
{
	if(me->button() == Qt::LeftButton)
	{
		_endPt = me->pos();
		emit signalSelectRectChanged(getSelRect());
		_mousePressed = false;
		update();

		float x, y, w, h;
		getSelectRect(x, y, w, h);
		signalImageSetChanged(x, y, w, h);
	}
}

void ImageSetWidget::mouseMoveEvent( QMouseEvent *me )
{
	if(_mousePressed)
	{
		_endPt = me->pos();
		emit signalSelectRectChanged(getSelRect());
		update();

		float x, y, w, h;
		getSelectRect(x, y, w, h);
		signalImageSetChanged(x, y, w, h);
	}
}

QRect ImageSetWidget::getSelRect()
{
	QPoint stPt, endPt;
	QRect imageRect = getImageRect();

	if(!imageRect.contains(_stPt))
	{
		stPt.rx() = 0;
		stPt.ry() = 0;
	}
	else
	{
		stPt.rx() = (_stPt.x() - imageRect.x()) / _sx;
		stPt.ry() = (_stPt.y() - imageRect.y()) / _sy;
	}

	if(!imageRect.contains(_endPt))
	{
		endPt.rx() = imageRect.width() / _sx + 1;
		endPt.ry() = imageRect.height() / _sy + 1;
	}
	else
	{
		endPt.rx() = (_endPt.x() - imageRect.x()) / _sx;
		endPt.ry() = (_endPt.y() - imageRect.y()) / _sy;
	}

	QPoint lt, rb;
	lt.setX(std::min(stPt.x(), endPt.x()));
	lt.setY(std::min(stPt.y(), endPt.y()));
	rb.setX((std::max(stPt.x(), endPt.x()) - 1));
	rb.setY((std::max(stPt.y(), endPt.y()) - 1));
	_selRect = QRect(lt, rb);
	return _selRect;
}

const QRect & ImageSetWidget::getSelectRect( float &orgX, float &orgY, float &w, float &h )
{
	QRect imageRect = _image.rect();
	int x = _selRect.x() + _selRect.width() / 2;
	int y = _selRect.y() + _selRect.height() / 2;
	orgX = x - imageRect.x() - imageRect.width() / 2;
	orgY = y - imageRect.y() - imageRect.height() / 2;
	w = _selRect.width();
	h = _selRect.height();
	return _selRect;
}

void ImageSetWidget::setSelectRect( int orgX, int orgY, int w, int h )
{
	QRect imageRect = _image.rect();
	int x = orgX + imageRect.center().x();
	int y = imageRect.center().y() - orgY;
	QRect recttmp(0, 0, w, h);
	QPoint tl = QPoint(x - (recttmp.x() + recttmp.width() / 2), y - (recttmp.y() + recttmp.height() / 2));
	_selRect = QRect(tl, QSize(w, h));
	emit signalSelectRectChanged(_selRect);
	emit signalImageSetChanged(orgX, orgY, w, h);
	update();
}

void ImageSetWidget::setSelectRect( const QRectF &rect )
{
	_selRect = rect.toRect();
	emit signalSelectRectChanged(_selRect);

	float x, y, w, h;
	getSelectRect(x, y, w, h);
	emit signalImageSetChanged(x, y, w, h);

	update();
}

void ImageSetWidget::slotSetSelectRect( int x, int y, int w, int h )
{
	setSelectRect(x, y, w, h);
}


//////////////////////////////////////////////////////////////////////////

ImageSetControlWidget::ImageSetControlWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	, _emitSignalValueChanged(true)
{
	_fistset = false;
	initUi();
	initSignal();
}

ImageSetControlWidget::~ImageSetControlWidget()
{

}

void ImageSetControlWidget::initUi()
{
	setupUi(this);
}

void ImageSetControlWidget::initSignal()
{
	connect(_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(slotZoomValueChange(int)));
	connect(_patchScaleSlider, SIGNAL(valueChanged(int)), this, SLOT(slotPatchScaleValueChange(int)));
	connect(_okBtn, SIGNAL(clicked(bool)), this, SIGNAL(signalOkBtnClicked(bool)));
	connect(_cancelBtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCancelBtnClicked(bool)));
	connect(_xspin, SIGNAL(valueChanged(int)), this, SLOT(slotSpinXValueChanged(int)));
	connect(_yspin, SIGNAL(valueChanged(int)), this, SLOT(slotSpinYValueChanged(int)));
	connect(_wspin, SIGNAL(valueChanged(int)), this, SLOT(slotSpinWValueChanged(int)));
	connect(_hspin, SIGNAL(valueChanged(int)), this, SLOT(slotSpinHValueChanged(int)));
}

void ImageSetControlWidget::slotZoomValueChange( int zoom )
{
	float zoomValue = converScaleToRealValue(zoom);
	emit signalZoomChanged(zoomValue);

	_zoomLabel->setText(QString("%1").arg(zoomValue, 0, 'f', 1));
}

void ImageSetControlWidget::slotPatchScaleValueChange( int scale )
{
	float scaleValue = converScaleToRealValue(scale);
	emit signalPatchScaleChanged(scaleValue);

	_scaleLabel->setText(QString("%1").arg(scaleValue, 0, 'f', 1));
}

float ImageSetControlWidget::converScaleToRealValue( int scale )
{
	//0 ~ 99 ---> 0.1 ~ 10.0
	float zoomValue = 1.0f;

	if(scale == 50)
	{
		zoomValue = 1.0f;
	}
	else if(scale < 50)
	{
		zoomValue = 0.1 + scale * 0.9 / 50.f;
	}
	else if(scale > 50)
	{
		zoomValue = 1.0f + (scale - 50) * 10.f / 49.f;
	}

	if(zoomValue < 0.1f)
		zoomValue = 0.1f;
	else if(zoomValue > 10.f)
		zoomValue = 10.f;

	return zoomValue;
}

void ImageSetControlWidget::setImageSet( const QPointF &center, const QSizeF &size )
{
	_emitSignalValueChanged = false;
	_xspin->setValue(center.x());
	_yspin->setValue(center.y());
	_wspin->setValue(size.width());
	_hspin->setValue(size.height());
	_emitSignalValueChanged = true;
	if (!_fistset)
	{
		_xspin->setMinimum(-size.width()*0.5);
		_xspin->setMaximum(size.width()*0.5);
		_yspin->setMinimum(-size.height()*0.5);
		_yspin->setMaximum(size.height()*0.5);
		_wspin->setMinimum(1);
		_wspin->setMaximum(size.width());
		_hspin->setMinimum(1);
		_hspin->setMaximum(size.height());
		_fistset = true;
	}
}

void ImageSetControlWidget::slotImageSetChanged( float x, float y, float w, float h )
{
	setImageSet(QPointF(x, y), QSizeF(w, h));
}

void ImageSetControlWidget::slotSpinXValueChanged( int value )
{
	emitSkinSetChanged();
}

void ImageSetControlWidget::slotSpinYValueChanged( int value )
{
	emitSkinSetChanged();
}

void ImageSetControlWidget::slotSpinWValueChanged( int value )
{
	emitSkinSetChanged();
}

void ImageSetControlWidget::slotSpinHValueChanged( int value )
{
	emitSkinSetChanged();
}

void ImageSetControlWidget::emitSkinSetChanged()
{
	if(!_emitSignalValueChanged)
		return;

	int x = _xspin->value();
	int y = _yspin->value();
	int w = _wspin->value();
	int h = _hspin->value();
	signalSkinSetChanged(x, y, w, h);
}


//////////////////////////////////////////////////////////////////////////

SkinSetDialog::SkinSetDialog()
	: _imageSetWidget(NULL)
	, _imagePreWidget(NULL)
	, _imageSetCtlWidget(NULL)
{
	initUi();
	initSignal();
}

void SkinSetDialog::initUi()
{

	QIcon icon;
	icon.addFile(QStringLiteral(":/UIDesigner/Resources/UI.png"), QSize(), QIcon::Normal, QIcon::Off);
	setWindowIcon(icon);
	setStyleSheet(QLatin1String("/*\n"
		"	Copyright 2013 Emanuel Claesson\n"
		"\n"
		"	Licensed under the Apache License, Version 2.0 (the \"License\");\n"
		"	you may not use this file except in compliance with the License.\n"
		"	You may obtain a copy of the License at\n"
		"\n"
		"		http://www.apache.org/licenses/LICENSE-2.0\n"
		"\n"
		"	Unless required by applicable law or agreed to in writing, software\n"
		"	distributed under the License is distributed on an \"AS IS\" BASIS,\n"
		"	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
		"	See the License for the specific language governing permissions and\n"
		"	limitations under the License.\n"
		"*/\n"
		"\n"
		"/*\n"
		"	COLOR_DARK     = #1e1e1e\n"
		"	COLOR_MEDIUM   = #2d2d30\n"
		"	COLOR_MEDLIGHT = #333337\n"
		"	COLOR_LIGHT    = #FFFFFF\n"
		"	COLOR_ACCENT   = #3f3f46\n"
		"*/\n"
		"\n"
		"* {\n"
		"	background: #1e1e1e;\n"
		"	color: #FFFFFF;\n"
		"	border: 1px solid #333337;\n"
		"}\n"
		"\n"
		"QWidget::item:selected {\n"
		"	background: #3f3f46;\n"
		"}\n"
		"\n"
		"QCheckBox, QRadioButton {\n"
		"	border: none;\n"
		"}\n"
		""
		"\n"
		"QRadioButton::indicator, QCheckBox::indicator {\n"
		"	width: 13px;\n"
		"	height: 13px;\n"
		"}\n"
		"\n"
		"QRadioButton::indicator::unchecked, QCheckBox::indicator::unchecked {\n"
		"	border: 1px solid #333337;\n"
		"	background: none;\n"
		"}\n"
		"\n"
		"QRadioButton::indicator:unchecked:hover, QCheckBox::indicator:unchecked:hover {\n"
		"	border: 1px solid #FFFFFF;\n"
		"}\n"
		"\n"
		"QRadioButton::indicator::checked, QCheckBox::indicator::checked {\n"
		"	border: 1px solid #333337;\n"
		"	background: #333337;\n"
		"}\n"
		"\n"
		"QRadioButton::indicator:checked:hover, QCheckBox::indicator:checked:hover {\n"
		"	border: 1px solid #FFFFFF;\n"
		"	background: #FFFFFF;\n"
		"}\n"
		"\n"
		"QGroupBox {\n"
		"	margin-top: 6px;\n"
		"}\n"
		"\n"
		"QGroupBox::title {\n"
		"	top: -7px;\n"
		"	left: 7px;\n"
		"}\n"
		"\n"
		"QScrollBar {\n"
		"	border: 1px solid #686868;\n"
		"	background: #1e1e1e;\n"
		"}\n"
		"\n"
		"QScrollBar:horizontal {\n"
		"	height: 15px;\n"
		"	margin: 0px 0px 0px 32px;\n"
		"}\n"
		"\n"
		"QScrollBar:vertical {\n"
		"	width: 15px;\n"
		"	margin: 32px 0px 0px 0px;\n"
		""
		"}\n"
		"\n"
		"QScrollBar::handle {\n"
		"	background: #2d2d30;\n"
		"	border: 1px solid #333337;\n"
		"}\n"
		"\n"
		"QScrollBar::handle:horizontal {\n"
		"	border-width: 0px 1px 0px 1px;\n"
		"}\n"
		"\n"
		"QScrollBar::handle:vertical {\n"
		"	border-width: 1px 0px 1px 0px;\n"
		"}\n"
		"\n"
		"QScrollBar::handle:horizontal {\n"
		"	min-width: 20px;\n"
		"}\n"
		"\n"
		"QScrollBar::handle:vertical {\n"
		"	min-height: 20px;\n"
		"}\n"
		"\n"
		"QScrollBar::add-line, QScrollBar::sub-line {\n"
		"	background:#2d2d30;\n"
		"	border: 1px solid #333337;\n"
		"	subcontrol-origin: margin;\n"
		"}\n"
		"\n"
		"QScrollBar::add-line {\n"
		"	position: absolute;\n"
		"}\n"
		"\n"
		"QScrollBar::add-line:horizontal {\n"
		"	width: 15px;\n"
		"	subcontrol-position: left;\n"
		"	left: 15px;\n"
		"}\n"
		"\n"
		"QScrollBar::add-line:vertical {\n"
		"	height: 15px;\n"
		"	subcontrol-position: top;\n"
		"	top: 15px;\n"
		"}\n"
		"\n"
		"QScrollBar::sub-line:horizontal {\n"
		"	width: 15px;\n"
		"	subcontrol-position: top left;\n"
		"}\n"
		"\n"
		"QScrollBar::sub-line:vertical {\n"
		"	height: 15px;\n"
		"	subcontrol-position"
		": top;\n"
		"}\n"
		"\n"
		"QScrollBar:left-arrow, QScrollBar::right-arrow, QScrollBar::up-arrow, QScrollBar::down-arrow {\n"
		"	border: 1px solid #333337;\n"
		"	width: 3px;\n"
		"	height: 3px;\n"
		"}\n"
		"\n"
		"QScrollBar::add-page, QScrollBar::sub-page {\n"
		"	background: none;\n"
		"}\n"
		"\n"
		"QAbstractButton:hover {\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QAbstractButton:pressed {\n"
		"	background: #333337;\n"
		"}\n"
		"\n"
		"QAbstractItemView {\n"
		"	show-decoration-selected: 1;\n"
		"	selection-background-color: #3f3f46;\n"
		"	selection-color: #FFFFFF;\n"
		"	alternate-background-color: #2d2d30;\n"
		"}\n"
		"\n"
		"QHeaderView {\n"
		"	border: 1px solid #333337;\n"
		"}\n"
		"\n"
		"QHeaderView::section {\n"
		"	background: #1e1e1e;\n"
		"	border: 1px solid #333337;\n"
		"	padding: 4px;\n"
		"}\n"
		"\n"
		"QHeaderView::section:selected, QHeaderView::section::checked {\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QTableView {\n"
		"	gridline-color: #333337;\n"
		"}\n"
		"\n"
		"QTabBar {\n"
		"	margin-left: 2px;\n"
		"}\n"
		"\n"
		"QTabBar::tab {\n"
		"	border-radius: 0px;\n"
		""
		"	padding: 4px;\n"
		"	margin: 4px;\n"
		"}\n"
		"\n"
		"QTabBar::tab:selected {\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QComboBox::down-arrow {\n"
		"	border: 1px solid #333337;\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QComboBox::drop-down {\n"
		"	border: 1px solid #333337;\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QComboBox::down-arrow {\n"
		"	width: 3px;\n"
		"	height: 3px;\n"
		"	border: 1px solid #333337;\n"
		"}\n"
		"\n"
		"QAbstractSpinBox {\n"
		"	padding-right: 15px;\n"
		"}\n"
		"\n"
		"QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {\n"
		"	border: 1px solid #333337;\n"
		"	background: #2d2d30;\n"
		"	subcontrol-origin: border;\n"
		"}\n"
		"\n"
		"QAbstractSpinBox::up-arrow, QAbstractSpinBox::down-arrow {\n"
		"	width: 3px;\n"
		"	height: 3px;\n"
		"	border: 1px solid #333337;\n"
		"}\n"
		"\n"
		"QSlider {\n"
		"	border: none;\n"
		"}\n"
		"\n"
		"QSlider::groove:horizontal {\n"
		"	height: 5px;\n"
		"	margin: 4px 0px 4px 0px;\n"
		"}\n"
		"\n"
		"QSlider::groove:vertical {\n"
		"	width: 5px;\n"
		"	margin: 0px 4px 0px 4px;\n"
		"}\n"
		"\n"
		"QSlider::handle {\n"
		""
		"	border: 1px solid #333337;\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QSlider::handle:horizontal {\n"
		"	width: 15px;\n"
		"	margin: -4px 0px -4px 0px;\n"
		"}\n"
		"\n"
		"QSlider::handle:vertical {\n"
		"	height: 15px;\n"
		"	margin: 0px -4px 0px -4px;\n"
		"}\n"
		"\n"
		"QSlider::add-page:vertical, QSlider::sub-page:horizontal {\n"
		"	background: #3f3f46;\n"
		"}\n"
		"\n"
		"QSlider::sub-page:vertical, QSlider::add-page:horizontal {\n"
		"	background: #2d2d30;\n"
		"}\n"
		"\n"
		"QLabel {\n"
		"	border: none;\n"
		"}\n"
		"\n"
		"QProgressBar {\n"
		"	text-align: center;\n"
		"}\n"
		"\n"
		"QProgressBar::chunk {\n"
		"	width: 1px;\n"
		"	background-color: #3f3f46;\n"
		"}\n"
		"\n"
		"QMenu::separator {\n"
		"	background: #2d2d30;\n"
		"}"));
	setWindowTitle("9 Slice Editor");

	QVBoxLayout *vboxLayerOut = new QVBoxLayout;

	QSplitter *splitter = new QSplitter(Qt::Horizontal, 0);
	vboxLayerOut->addWidget(splitter);

	_imageSetWidget = new ImageSetWidget();
	//QImage image;
	//image.load("D:\\ui_tip_back.png");
	//_imageSetWidget->setImage(image);

	_imagePreWidget = new Image9Widget();
	//_imagePreWidget->setImage(image);
	//_imagePreWidget->setShowSize(QSize(100, 50), 1);

	splitter->addWidget(_imageSetWidget);
	splitter->addWidget(_imagePreWidget);

	_imageSetCtlWidget = new ImageSetControlWidget;
	vboxLayerOut->addWidget(_imageSetCtlWidget);

	setLayout(vboxLayerOut);

	resize(800, 600);
}

void SkinSetDialog::initSignal()
{
	connect(_imageSetWidget, SIGNAL(signalSelectRectChanged(const QRect &)), _imagePreWidget, SLOT(slotCenterRectChanged( const QRect &)));
	connect(_imageSetWidget, SIGNAL(signalImageSetChanged(float, float, float, float)), _imageSetCtlWidget, SLOT(slotImageSetChanged(float, float, float, float)));
	connect(_imageSetCtlWidget, SIGNAL(signalZoomChanged(float)), this, SLOT(slotZoomChanged(float)));
	connect(_imageSetCtlWidget, SIGNAL(signalPatchScaleChanged(float)), this, SLOT(slotPatchScaleChanged(float)));
	connect(_imageSetCtlWidget, SIGNAL(signalOkBtnClicked(bool)), this, SLOT(slotOkBtnClicked(bool)));
	connect(_imageSetCtlWidget, SIGNAL(signalCancelBtnClicked(bool)), this, SLOT(slotCancelBtnClicked(bool)));
	connect(_imageSetCtlWidget, SIGNAL(signalSkinSetChanged(int, int, int, int)), _imageSetWidget, SLOT(slotSetSelectRect(int, int, int, int)));
}

void SkinSetDialog::slotZoomChanged( float zoom )
{
	_imageSetWidget->setScale(zoom, zoom);
}

void SkinSetDialog::slotPatchScaleChanged( float scale )
{
	_imagePreWidget->setScale(scale);
}

void SkinSetDialog::setImage( const QImage &image, const QSize &showSize, const SkinSet &skinSet )
{
	_imageSetWidget->setImage(image);
	_imagePreWidget->setImage(image);
	_imagePreWidget->setShowSize(showSize, 1.0f);
	_imageSetCtlWidget->setImageSet(skinSet.getCenterPt(), skinSet.getCenterSize());
	setSelectRect(skinSet.getRect());
	OriginSz = QSize(image.width() , image.height());
}

void SkinSetDialog::slotOkBtnClicked( bool )
{
	_rect = _imageSetWidget->getSelectRect(_orgX, _orgY, _w, _h);
	accept();
}

void SkinSetDialog::slotCancelBtnClicked( bool )
{
	QRect rc(0, 0, OriginSz.width(), OriginSz.height());
	setSelectRect(rc);
}

const QRect & SkinSetDialog::getSelectRect( float &orgX, float &orgY, float &w, float &h )
{
	orgX = _orgX;
	orgY = _orgY;
	w = _w;
	h = _h;
	return _rect;
}

void SkinSetDialog::setSelectRect( const QRectF &rect )
{
	_imageSetWidget->setSelectRect(rect);
}
