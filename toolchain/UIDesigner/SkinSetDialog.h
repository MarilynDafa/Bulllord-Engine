#ifndef SKINSETDIALOG_H
#define SKINSETDIALOG_H

#include <QtWidgets/QDialog>
#include <QtGui/QImage>
#include "PropertyDataType.h"
#include "GeneratedFiles/ui_ImageSetControlWidget.h"

class ImageWidget : public QWidget
{
	Q_OBJECT
public:
	ImageWidget(QWidget *parent  = 0);
	~ImageWidget();

	void setImage(const QImage &image);
	virtual void setScale(float sx, float sy);

protected:
	virtual void paintEvent(QPaintEvent *pe);
	virtual void resizeEvent(QResizeEvent *re);
	virtual void updateLayout();
	QRect getImageRect();

protected:
	QImage _image;
	
	float _sx;
	float _sy;

	float _ox;
	float _oy;
};

class Image9Widget : public ImageWidget
{
	Q_OBJECT
public:
	Image9Widget(QWidget *parent = 0);
	~Image9Widget();

	void setImageCenterRect(const QRect &rect);
	void setShowSize(const QSize &size, float scale = 1);

	void setScale(float scale);
	virtual void setScale(float sx, float sy);

protected slots:
	void slotCenterRectChanged(const QRect &rect);

protected:
	virtual void paintEvent(QPaintEvent *pe);
	virtual void updateLayout();

private:
	bool isRectValid(const QRect &rect);

protected:
	QRect _tlS, _tlD;
	QRect _topS, _topD;
	QRect _trS, _trD;
	QRect _leftS, _leftD;
	QRect _centerS, _centerD;
	QRect _rightS, _rightD;
	QRect _blS, _blD;
	QRect _bottomS, _bottomD;
	QRect _brS, _brD;

	float _scale;
};

class ImageSetWidget : public ImageWidget
{
	Q_OBJECT
public:
	ImageSetWidget(QWidget *parent = 0);
	~ImageSetWidget();

	const QRect &getSelectRect(float &orgX, float &orgY, float &w, float &h);
	void setSelectRect(const QRectF &rect);
	void setSelectRect( int orgX, int orgY, int w, int h );

protected:
	virtual void paintEvent(QPaintEvent *pe);
	virtual void mousePressEvent(QMouseEvent *me);
	virtual void mouseReleaseEvent(QMouseEvent *me);
	virtual void mouseMoveEvent(QMouseEvent *me);

	QRect getSelRect();

protected slots:
	void slotSetSelectRect(int x, int y, int w, int h);

protected:
	bool _mousePressed;
	QPoint _stPt;
	QPoint _endPt;
	QRect _selRect;
signals:
	void signalSelectRectChanged(const QRect &rect);
	void signalImageSetChanged(float orgX, float orgY, float width, float height);
};

class ImageSetControlWidget : public QWidget, public Ui::ImageSetControlWidget
{
	Q_OBJECT
public:
	ImageSetControlWidget(QWidget *parent = 0);
	~ImageSetControlWidget();

	void initUi();
	void initSignal();

	void setImageSet(const QPointF &center, const QSizeF &size);

private:
	float converScaleToRealValue(int scale);
	void emitSkinSetChanged();

protected slots:
	void slotZoomValueChange(int zoom);
	void slotPatchScaleValueChange(int scale);
	void slotImageSetChanged(float x, float y, float w, float h);
	void slotSpinXValueChanged(int value);
	void slotSpinYValueChanged(int value);
	void slotSpinWValueChanged(int value);
	void slotSpinHValueChanged(int value);

signals:
	void signalZoomChanged(float zoom);
	void signalPatchScaleChanged(float scale);
	void signalOkBtnClicked(bool);
	void signalCancelBtnClicked(bool);
	void signalSkinSetChanged(int x, int y, int w, int h);

private:
	bool _emitSignalValueChanged;
	bool _fistset;
};

class SkinSetDialog : public QDialog
{
	Q_OBJECT
public:
	SkinSetDialog();
	void initUi();
	void initSignal();
	void setImage(const QImage &image, const QSize &showSize, const SkinSet &skinSet);
	const QRect& getSelectRect(float &orgX, float &orgY, float &w, float &h);

private:
	void setSelectRect(const QRectF &rect);

protected slots:
	void slotZoomChanged(float zoom);
	void slotPatchScaleChanged(float scale);

	void slotOkBtnClicked(bool);
	void slotCancelBtnClicked(bool);

protected:
	ImageSetWidget *_imageSetWidget;
	Image9Widget *_imagePreWidget;
	ImageSetControlWidget *_imageSetCtlWidget;

	float _orgX, _orgY, _w, _h;
	QRect _rect;//图片上的区域，作为辅助使用

	QSize OriginSz;
};

#endif //SKINSETDIALOG_H