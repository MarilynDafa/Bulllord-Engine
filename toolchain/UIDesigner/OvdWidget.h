#ifndef OVDWIDGET_H
#define OVDWIDGET_H


#include <QtWidgets/QWidget>
#include <QEvent>
#include <QResizeEvent>
#include "System.h"
#include "FileMgr.h"
#include "Device.h"
#include "Window.h"
#include "ResMgr.h"
#include "UIMgr.h"
#include "Shader.h"
#include "InputMgr.h"
#include "ManualLoader.h"
#include "Widget/UiWidget.h"
using namespace o2d;
enum SCREEN_DPI
{
	SP_XGA,
	SP_HD,
	SP_WXGA
};

class OvdWidget:public QWidget
{
	Q_OBJECT
public:
	OvdWidget(QWidget *parent = 0);
	virtual ~OvdWidget();
public:
	void init(SCREEN_DPI dpi);
	void close();
	void sendMouseEventToEngine( QMouseEvent* event, bool pressedDown );
	void sendKeyEventToEngine( QKeyEvent* event, bool pressedDown );
	void setBackImage(void* data , int w , int h);
protected:
	void setWidgetFrame(const QVector<c_widget*>& widgets , c_widget* widget);
	void drawRect(const QPoint &p1, const QPoint &p2);
	void drawSizeControllers(const QVector<SizeController> &rects);
	void updateEngine();
protected:
	virtual void enterEvent( QEvent * event );
	virtual void timerEvent( QTimerEvent* event );
	virtual void paintEvent( QPaintEvent* event );
	virtual void resizeEvent( QResizeEvent* event );
	virtual void mousePressEvent( QMouseEvent* event );
	virtual void mouseReleaseEvent( QMouseEvent* event );
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void wheelEvent( QWheelEvent* event );
	virtual void keyPressEvent( QKeyEvent* event );
	virtual void keyReleaseEvent( QKeyEvent* event );
private:
	c_technique* _tech;
	c_technique* _bgTech;
	c_manual_loader* _uiLoader;
	c_manual_loader* _frameLoader;
	c_manual_loader* _bgLoader;
	c_geometry_buffer* _bgGbo;	
	c_geometry_buffer* _gbo;
	c_geometry_buffer* _rectGbo;
	c_geometry_buffer* _sizeControllerGbo;
	c_tex_loader* tex_loader;
	bool _ready;
	bool _drawFrame;
	bool _drawRect;
	bool _drawSizeController;
	unsigned int m_cur_time;
	unsigned int m_prev_time;
	int mdpi;
};
#endif