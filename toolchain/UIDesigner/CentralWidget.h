#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "OvdWidget.h"
#include "Widget/UiWidgetFactory.h"
#include <QtCore/QVariant>

class UiWidget;
class SizeController;

class WidgetClipBoard : public QObject
{
	Q_OBJECT
signals:
	void widgetsChanged(const QVector<UiWidget*> &widgets);
public:
	enum OPERATE_TYPE
	{
		VALID_OPERATE,
		CUT_OPERATE,
		COPY_OPERATE
	};

	static WidgetClipBoard *getSingletonPtr()
	{
		static WidgetClipBoard clipBorad;
		return &clipBorad;
	}

	void setCut(const QVector<UiWidget*> &widgets)
	{
		_type = CUT_OPERATE;
		_widgets = widgets;
		widgetsChanged(_widgets);
	}

	void setCopy(const QVector<UiWidget*> &widgets)
	{
		_type = COPY_OPERATE;
		_widgets = widgets;
		_limits = UiWidgetFactory::getSingletonPtr()->getWidgetsOnTree(_widgets);
		widgetsChanged(_widgets);
	}

	QVector<UiWidget*> getWidgets()
	{
		QVector<UiWidget*> widgets = _widgets;
		if(_type == CUT_OPERATE)
		{
			_widgets.clear();
			widgetsChanged(_widgets);
		}
		return widgets;
	}

	QVector<UiWidget*> getLimits()
	{
		assert(_type == COPY_OPERATE);
		return _limits;
	}

	OPERATE_TYPE getType() const { return _type; }

	int getWidgetCount() const { return _widgets.count(); }

private:
	WidgetClipBoard() 
		: _type(VALID_OPERATE) 
	{
		widgetsChanged(_widgets);
	}

	OPERATE_TYPE _type;
	QVector<UiWidget*> _widgets;
	QVector<UiWidget*> _limits;
};

class CentralWidget : public OvdWidget
{
	Q_OBJECT
	enum EditState
	{
		RESIZE_STATE,
		SELECT_STATE,
		MULT_SELECT_STATE,
		DRAG_SELECT_STATE,
		DRAG_MOVE_STATE,
		DRAG_COPY_STATE,
		NO_STATE
	};
public:
	CentralWidget(QWidget *parent = 0);
	virtual ~CentralWidget();
	static CentralWidget* getInst() { return _self; }
	void emitSignals(bool yes);
	void cutSelectedWidgets();
	void copySelectedWidgets();
	void pasteWidgets();
	void save(const QString &fileName);
	bool open(const QString &fileName);
	void setScaleFactor(double scale);
	void cacheWidgetsPos(std::vector<UiWidget*>& widgets, std::vector<QRect>& rects);
	void updateWidgetsPos(std::vector<UiWidget*>& widgets, std::vector<QRect>& rects);
	void clean();
signals:
	void widgetAdded(UiWidget *widget);
	void widgetSelectedChanged(const QVector<UiWidget*> &widget);
	void widgetsDeleted(const QVector<UiWidget*> &widgets);
	void widgetNameChanged(UiWidget *widget, QString name);
	void widgetParentChanged(UiWidget *widget);
	void widgetPropertyChanged(UiWidget *widget, const QString &property, const QVariant &value);
	void updateCursorPos(QPoint* pt);
protected:
	virtual void dragEnterEvent(QDragEnterEvent *ev);
	virtual void dropEvent(QDropEvent *ev);
	virtual void mousePressEvent( QMouseEvent* event );
	virtual void mouseReleaseEvent( QMouseEvent* event );
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void keyPressEvent( QKeyEvent* event );
	virtual void enterEvent( QEvent *ev );
	virtual void leaveEvent(QEvent *ev );
private:
	UiWidget *addUiWidget(const QString &uiName, const QPoint &pos);
	void addUiWidgets(const QVector<UiWidget*> &widgets);
	void selectWidgets(const QRect &rect);
	void beginSelect(const QPoint &point);
	void duringSelect(const QPoint &point);
	void endSelect(const QPoint &point);
	void dragMove(const QVector2D &v);
	void endDragMove(const QPoint &mousePos);
	void dragCopy(const QVector2D &v);
	void endDragCopy(const QPoint &mousePos);
	bool validWidgetName(UiWidget *widget, QString name);
	UiWidget *getWidget(const QPoint &pos, bool acceptChild = false, const QVector<UiWidget*> &except = QVector<UiWidget*>() );
	void setSelectedWidget(const QVector<UiWidget*> &widgets);
	void setSizeControllers(const QVector<UiWidget*> &widgets);
	SizeController getSizeController(const QPoint &point);
	void setWidgetsVisiable(const QVector<UiWidget*> &widgets, bool visible);
	void setWidgetsParent(UiWidget* parent, const QVector<UiWidget*> &widgets, bool move = false, int destX = 0, int destY = 0);
	QRect getWidgetsBound(const QVector<UiWidget*> &widgets);
	void moveWidgets(const QVector<UiWidget*> &widgets, const QVector2D &v);
	void saveFontFile();

protected slots:
	void slotWidgetSlectedChanged(const QVector<UiWidget*> &widgets);
	void slotWidgetNameChanged(UiWidget *widget, const QString &name);
	void slotWidgetPropertyChanged(UiWidget *widget, const QString &property, const QVariant &value);

	void slotLeftAlignWidget();
	void slotRightAlignWidget();
	void slotHCenterAlignWidget();
	void slotTopAlignWidget();
	void slotBottomAlignWidget();
	void slotVCenterAlignWidget();
	void slotHEqualWidget();
	void slotVEqualWidget();
	void slotEqualWidgetWidth();
	void slotEqualWidgetHeight();
	void slotEqualWidgetBoth();
	void slotHCenterToParent();
	void slotVCenterToParent();
public:
	void deleteSelectWidgets();

private:
	QVector<SizeController> _sizeControllers;
	QVector<UiWidget*> _selectedWidgets;
	QVector<UiWidget*> _unSelectedWidgets;
	QVector<UiWidget*> _dragCopyWidgets;
	QPoint _beginPos;
	QPoint _lastMousePos;
	QPoint _mousePos;
	EditState _editState;
	UiWidget *_widgetWillRemoveFromSelect;
	bool _selectBegin;
	bool _beginMouseMove;
	bool _isWidgetCopyed;
	bool _mouseIn;
	QCursor _defaultCursor;
	SizeController _selectedSizeController;
	double _scale;
	static CentralWidget* _self;
};

#endif //CENTRALWIDGET_H