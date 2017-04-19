#include <QtCore/QAbstractItemModel>
#include <QtCore/QDataStream>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QMimeData>
#include <QXmlStreamReader>
#include <qpainter.h>
#include "macro.h"
#include "CentralWidget.h"
#include "UiWidgetFactory.h"
#include "Widget/UiWidget.h"
#include "Widget\UiDialog.h"
#include "FocusEventFilter.h"
#include "ObjectViewerDockWidget.h"
#include "MainWindow.h"
//////////////////////////////////////////////////////////////////////////
CentralWidget* CentralWidget::_self = NULL;
CentralWidget::CentralWidget( QWidget *parent /*= 0*/ )
	: OvdWidget(parent)
	, _editState(SELECT_STATE)
	, _selectBegin(false)
	, _beginMouseMove(false)
	, _isWidgetCopyed(false)
	, _widgetWillRemoveFromSelect(0)
	, _mouseIn(false)
{
	_defaultCursor = cursor();
	setAcceptDrops(true);
	emitSignals(false);
	_self = this;
}

CentralWidget::~CentralWidget()
{
	QVector<UiWidget*> deleteWidget = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(
		UiWidgetFactory::getSingletonPtr()->getWidgets());
	UiWidgetFactory::getSingletonPtr()->deleteWidgets(deleteWidget);
	deleteWidget.clear();
	_selectedWidgets.clear();
	_unSelectedWidgets.clear();
}

void CentralWidget::dragEnterEvent( QDragEnterEvent *ev )
{
 	const QMimeData *data = ev->mimeData();
	QByteArray dataArray = data->data(QString("application/x-qabstractitemmodeldatalist"));
	if(dataArray.size() != 0)
		ev->acceptProposedAction();
	else
		ev->ignore();
}

void CentralWidget::dropEvent( QDropEvent *ev )
{
	const QMimeData *data = ev->mimeData();
	QByteArray dataArray = data->data(QString("application/x-qabstractitemmodeldatalist"));
	if(dataArray.size() != 0)
	{
		setFocus();
		ev->acceptProposedAction();
		QDataStream dStream(&dataArray,QIODevice::ReadOnly);
		int row,col;
		QListWidgetItem item;
		while(!dStream.atEnd())
		{
			dStream >> row >> col >> item;
			addUiWidget(item.text(), ev->pos());
		}
	}
	else
	{
		ev->ignore();
	}
}

UiWidget * CentralWidget::addUiWidget( const QString &uiName, const QPoint &pos )
{
	QVector<UiWidget*> selectWidget;
	UiWidget *widget = UiWidgetFactory::getSingletonPtr()->createWidget(uiName,getWidget(pos),pos);
	if(widget != 0)
	{
		widgetAdded(widget);
		selectWidget.append(widget);
	}
	setSelectedWidget(selectWidget);
	return widget;
}

void CentralWidget::mousePressEvent( QMouseEvent* event )
{
	//OvdWidget::mousePressEvent(event);
	if(_selectedSizeController.isValid())
	{
		_editState = RESIZE_STATE;
	}
	else
	{
		UiWidget *widget = getWidget(event->pos());
		QVector<UiWidget*> selectWidget = _selectedWidgets;

		Qt::KeyboardModifiers key = event->modifiers();
		if(key != Qt::ControlModifier)
			_editState = SELECT_STATE;
		else
			_editState = MULT_SELECT_STATE;
		bool diban = widget ? widget->isLock() : false;
		if(widget != 0 && !diban)
		{
			if(!selectWidget.contains(widget))
			{
				if(_editState == SELECT_STATE)
					selectWidget.clear();
				selectWidget.append(widget);
			}
			else
			{
				if(_editState == MULT_SELECT_STATE)
					_widgetWillRemoveFromSelect = widget;
			}
			setSelectedWidget(selectWidget);
		}
		else
		{
			if(_editState == SELECT_STATE)
				selectWidget.clear();

			setSelectedWidget(selectWidget);

			if(event->buttons() == Qt::LeftButton)
				beginSelect(event->pos());
		}
	}

	event->ignore();
	_lastMousePos = event->pos();
	_beginMouseMove = false;
}

void CentralWidget::mouseReleaseEvent( QMouseEvent* event )
{
	//OvdWidget::mouseReleaseEvent(event);

	QVector<UiWidget*> selectWidget = _selectedWidgets;

	if(_editState == DRAG_SELECT_STATE)
	{
		endSelect(event->pos());
	}
	else if(_editState == MULT_SELECT_STATE)
	{
		if(_widgetWillRemoveFromSelect != 0)
		{
			//if(event->button() == Qt::LeftButton)
			{
				int index = selectWidget.indexOf(_widgetWillRemoveFromSelect);
				if(index >= 0)
				{
					selectWidget.remove(index);
					setSelectedWidget(selectWidget);
				}
			}
		}
	}
	else if(_editState == DRAG_MOVE_STATE)
	{
		dragMove(QVector2D(event->pos() - _lastMousePos));
		endDragMove(event->pos());
	}
	else if(_editState == DRAG_COPY_STATE)
	{
		//dragCopy(QVector2D(event->pos() - _lastMousePos));
		//endDragCopy(event->pos());
	}
	else if(event->button() == Qt::RightButton && !_beginMouseMove)
	{
		QPoint ep = event->pos();
		UiWidget *widget = getWidget(ep);
		QPoint globalPt = this->mapToGlobal(ep);
		widget->handleRightClicked(globalPt.x(), globalPt.y());
	}
	else if (_editState == SELECT_STATE)
	{
		if ((selectWidget.size() > 1) && (_editState != MULT_SELECT_STATE))
		{
			UiWidget *widget = getWidget(event->pos());
			QVector<UiWidget*> selwdigh;
			selwdigh.push_back(widget);
			setSelectedWidget(selwdigh);
		}
	}
	_widgetWillRemoveFromSelect = 0;
	_editState = NO_STATE;
	_lastMousePos = event->pos();
}

void CentralWidget::mouseDoubleClickEvent( QMouseEvent * event )
{
	if(event->buttons() == Qt::LeftButton)
	{
		UiWidget *widget = getWidget(event->pos());
		if(widget != 0)
		{
			QPoint ep = event->pos();
			QPoint ltP = widget->getWidgetLTPoint();
			widget->handleDoubleClicked(ep.x() - ltP.x(), ep.y() - ltP.y());
		}
	}
}

void CentralWidget::mouseMoveEvent( QMouseEvent * event )
{
	//OvdWidget::mouseMoveEvent(event);

	if(event->buttons() == Qt::LeftButton)
	{
		if(!_beginMouseMove)
		{
			_beginMouseMove = true;

			if(_editState != RESIZE_STATE)
			{
				UiWidget *widget = getWidget(event->pos());

				UiDialog* dlg = dynamic_cast<UiDialog*>(widget);
				bool diban = false;
				if (widget)
					diban = widget->isLock();

				if(widget != 0 && !diban)
				{
					Qt::KeyboardModifiers key = event->modifiers();
					if(key != Qt::ControlModifier)
						_editState = DRAG_MOVE_STATE;
					else
						_editState = DRAG_COPY_STATE;
				}
				else
					_editState = DRAG_SELECT_STATE;
			}
		}

		if(_editState == RESIZE_STATE)
		{
			if(_selectedSizeController.moveTo(event->pos().x(), event->pos().y()))
			{
				setSelectedWidget(_selectedWidgets);
				widgetPropertyChanged(_selectedSizeController.getWidget(), QString::fromLatin1("Geometry"), _selectedSizeController.getWidget()->property("Geometry"));
			}
		}
		else if(_editState == DRAG_SELECT_STATE)
		{
			if(event->buttons() == Qt::LeftButton)
				duringSelect(event->pos());
		}
		else if(_editState == DRAG_MOVE_STATE)
		{
			dragMove(QVector2D(event->pos() - _lastMousePos));
		}
		else if(_editState == DRAG_COPY_STATE)
		{
			//dragCopy(QVector2D(event->pos() - _lastMousePos));
		}
		_lastMousePos = event->pos();
	}
	else if(event->buttons() == Qt::NoButton)
	{
		_selectedSizeController = getSizeController(event->pos());
		if(_selectedSizeController.isValid())
		{
			int cursor = _selectedSizeController.getCursorShape();
			assert(cursor != -1);
			setCursor((Qt::CursorShape)cursor);
		}
		else
		{
			setCursor(_defaultCursor);
		}
	}

	_mousePos = event->pos();
	updateCursorPos(&_mousePos);
	_beginMouseMove = true;
}

void CentralWidget::keyPressEvent( QKeyEvent* event )
{
	switch(event->key())
	{
	case Qt::Key_Delete:
		deleteSelectWidgets();
		break;
	case Qt::Key_Up:
		dragMove(QVector2D(0, -1));
		break;
	case Qt::Key_Down:
		dragMove(QVector2D(0, 1));
		break;
	case Qt::Key_Left:
		dragMove(QVector2D(-1, 0));
		break;
	case Qt::Key_Right:
		dragMove(QVector2D(1, 0));
		break;
	}
}

void CentralWidget::enterEvent( QEvent *ev )
{
	_mouseIn = true;
}

void CentralWidget::leaveEvent( QEvent *ev )
{
	_mouseIn = false;
}

void CentralWidget::setSelectedWidget( const QVector<UiWidget*> &widgets )
{
	QVector<UiWidget*> widgetss;
	int count = widgets.size();
	for (int i = 0; i < count; i++)
	{
		UiDialog* dlg = dynamic_cast<UiDialog*>(widgets[i]);
		bool diban = widgets[i]->isLock();
		if (!diban)
			widgetss.append(widgets[i]);
	}

	if(widgetss != _selectedWidgets)
	{
		for (int i = 0; i < _selectedWidgets.size(); ++i)
		{
			if (_selectedWidgets[i]->getWidget()->get_skinid() == 336830410)
				_selectedWidgets[i]->getWidget()->set_skin(c_string(L"default_ui_skin_name2"));
		}
		for (int i = 0; i < widgets.size(); ++i)
		{
			if (widgets[i]->getWidget()->get_skinid() == 1613585325)
				widgets[i]->getWidget()->set_skin(c_string(L"default_ui_skin_name"));
		}
		_selectedWidgets = widgetss;
		widgetSelectedChanged(_selectedWidgets);
	}

	QVector<c_widget*> wids;
	count = widgetss.size();
	for(int i = 0; i < count; i++)
	{
		wids.append(widgetss[i]->getWidget());
	}
	if(count <= 1)
	{
		setWidgetFrame(wids, 0);
	}
	else
	{
		c_widget *w = wids[0];
		wids.pop_front();
		setWidgetFrame(wids, w);
	}

	setSizeControllers(_selectedWidgets);
}

void CentralWidget::setSizeControllers( const QVector<UiWidget*> &widgets )
{
	_sizeControllers.clear();
	QVectorIterator<UiWidget*> wit(widgets);
	while(wit.hasNext())
	{
		UiWidget *widget = wit.next();
		widget->appendSizeController(_sizeControllers);
	}
	drawSizeControllers(_sizeControllers);
}

void CentralWidget::selectWidgets( const QRect &rect )
{
	QVector<UiWidget*> tmp;
	QVector<UiWidget*> selectWidget = _selectedWidgets;

	for(QVector<UiWidget*>::iterator it = selectWidget.begin(); it != selectWidget.end(); )
	{
		if(!rect.intersects((*it)->getAbsoluteRegion()))
		{
			tmp.append(*it);
			it = selectWidget.erase(it);
			if(it == selectWidget.end())
				break;
		}
		it ++;
	}

	for(QVector<UiWidget*>::iterator it = _unSelectedWidgets.begin(); it != _unSelectedWidgets.end(); )
	{
		if((*it)->getVisible() && rect.intersects((*it)->getAbsoluteRegion()))
		{
			selectWidget.append(*it);
			it = _unSelectedWidgets.erase(it);
			if(it == _unSelectedWidgets.end())
				break;
		}
		it ++;
	}

	if(!tmp.isEmpty())
	{
		_unSelectedWidgets += tmp;
		tmp.clear();
	}

	setSelectedWidget(selectWidget);
}

void CentralWidget::beginSelect( const QPoint &point )
{
	_beginPos = point;
}

void CentralWidget::duringSelect( const QPoint &point )
{
	if(!_selectBegin && (point - _beginPos).manhattanLength() >= QApplication::startDragDistance())
	{
		_selectBegin = true;
		_selectedWidgets.clear();
		_unSelectedWidgets = UiWidgetFactory::getSingletonPtr()->getWidgets();
		widgetSelectedChanged(_selectedWidgets);
	}

	if(_selectBegin)
	{
		drawRect(_beginPos, point);
		selectWidgets(QRect(_beginPos, point));
	}
}

void CentralWidget::endSelect( const QPoint &point )
{
	if(_selectBegin)
	{
		_selectBegin = false;
		drawRect(point, point);
		selectWidgets(QRect(_beginPos, point));
	}
}

void CentralWidget::deleteSelectWidgets()
{
	QVector<UiWidget*> deleteWidgets = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	setSelectedWidget(QVector<UiWidget*>());

	widgetsDeleted(deleteWidgets);
	UiWidgetFactory::getSingletonPtr()->deleteWidgets(deleteWidgets);

	deleteWidgets.clear();
}

void CentralWidget::dragMove( const QVector2D &v )
{
	moveWidgets(_selectedWidgets, v);
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::endDragMove( const QPoint &mousePos, bool changeparent)
{
	UiWidget *parent = getWidget(mousePos, true, _selectedWidgets);
	int count = _selectedWidgets.count();
	for(int i = 0; i < count; i ++)
	{
		UiWidget *widget = _selectedWidgets[i];
		if(!widget->getInteraction())
		{
			UiWidget *pw = widget->getParent();
			if( !_selectedWidgets.contains(pw) )
			{
				if (changeparent)
				{
					QRect r1 = widget->getAbsoluteRegion();
					if (widget->setParent(parent))
					{
						QRect r2 = widget->getAbsoluteRegion();
						widget->move(QVector2D(r1.x() - r2.x(), r1.y() - r2.y()));
						widgetParentChanged(widget);
					}
				}
			}
		}
	}
	setSelectedWidget(_selectedWidgets);
}

bool CentralWidget::validWidgetName( UiWidget *widget, QString name )
{
	if(name == tr("Root"))
	{
		QMessageBox::warning(this, tr("Warning"), tr("Widget name collision, please try another !"));
		return false;
	}

	const QVector<UiWidget*> &widgets = UiWidgetFactory::getSingletonPtr()->getWidgets();
	int count = widgets.count();
	for(int i = 0; i < count; i++)
	{
		UiWidget *w = widgets[i];
		if(w != widget)
		{
			if(name == w->getWidgetName())
			{
				QMessageBox::warning(this, tr("Warning"), tr("Widget name collision, please try another !"));
				return false;
			}
		}
	}
	return true;
}

UiWidget * CentralWidget::getWidget( const QPoint &pos, bool acceptChild/* = false*/, const QVector<UiWidget*> &except /*= QVector<UiWidget*>()*/ )
{
	QVector<UiWidget*> parents;
	const QVector<UiWidget*> &widgets = UiWidgetFactory::getSingletonPtr()->getWidgets();
	int count = widgets.count();
	for(int i = count - 1; i >= 0; i--)
	{
		UiWidget *widget = widgets[i];
		//if(widget->getParent() == 0)
		{
			UiWidget *selectW = widget->getWidget(pos, acceptChild, except.toList());
			if(selectW != 0)
				return selectW;
		}
	}
	return 0;
}

void CentralWidget::slotWidgetPropertyChanged( UiWidget *widget, const QString &property, const QVariant &value )
{
	assert(widget != 0);
	if(property == QString::fromLatin1("Name"))
		slotWidgetNameChanged(widget, value.toString());
	else
		widget->setProperty(property.toLatin1().data(), value);
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotWidgetSlectedChanged( const QVector<UiWidget*> &widgets )
{
	setSelectedWidget(widgets);
}

void CentralWidget::slotWidgetNameChanged( UiWidget *widget, const QString &name )
{
	assert(widget != 0);

	if(validWidgetName(widget, name))
	{
		widget->setWidgetName(name);
	}
	else
	{
		bool b = blockSignals(false);
		widgetNameChanged(widget, widget->getWidgetName());
		blockSignals(b);
	}   
}

void CentralWidget::emitSignals( bool yes )
{
	blockSignals( !yes );
}

void CentralWidget::dragCopy( const QVector2D &v )
{
	if(!_isWidgetCopyed)
	{
		assert(_dragCopyWidgets.count() == 0);
		_dragCopyWidgets = UiWidgetFactory::getSingletonPtr()->copyWidgets(_selectedWidgets);
		_isWidgetCopyed = true;
	}

	QVectorIterator<UiWidget*> widgetIterator(_dragCopyWidgets);
	while(widgetIterator.hasNext())
	{
		UiWidget *widget = widgetIterator.next();
		widget->move(v);
	}
}

void CentralWidget::endDragCopy( const QPoint &mousePos )
{
	addUiWidgets(_dragCopyWidgets);
	setSelectedWidget(_dragCopyWidgets);
	endDragMove(mousePos);
	_dragCopyWidgets.clear();
	_isWidgetCopyed = false;
}

void CentralWidget::addUiWidgets( const QVector<UiWidget*> &widgets )
{
	QVectorIterator<UiWidget*> widgetIterator(widgets);
	while(widgetIterator.hasNext())
	{
		UiWidget *widget = widgetIterator.next();
		if(!widget->getInteraction())
		{
			widgetAdded(widget);
			QList<UiWidget*> childs = widget->getChild();
			addUiWidgets(childs.toVector());
		}
	}
}

void CentralWidget::slotLeftAlignWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 2);
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	UiWidget *refWidget = widgetIt.next();
	int refLeft = refWidget->getWidgetLTPoint().x();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int left = widget->getWidgetLTPoint().x();
		if(left != refLeft)
			widget->move(QVector2D(refLeft - left, 0));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotRightAlignWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 2);
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	UiWidget *refWidget = widgetIt.next();
	int refRight = refWidget->getWidgetRBPoint().x();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int right = widget->getWidgetRBPoint().x();
		if(right != refRight)
			widget->move(QVector2D(refRight - right, 0));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotHCenterAlignWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 2);
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	UiWidget *refWidget = widgetIt.next();
	int refCenter = refWidget->getWidgetCenterPoint().x();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int center = widget->getWidgetCenterPoint().x();
		if(center != refCenter)
			widget->move(QVector2D(refCenter - center, 0));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotTopAlignWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 2);
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	UiWidget *refWidget = widgetIt.next();
	int refTop = refWidget->getWidgetLTPoint().y();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int top = widget->getWidgetLTPoint().y();
		if(top != refTop)
			widget->move(QVector2D(0, refTop - top));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotBottomAlignWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 2);
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	UiWidget *refWidget = widgetIt.next();
	int refBottom = refWidget->getWidgetRBPoint().y();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int bottom = widget->getWidgetRBPoint().y();
		if(bottom != refBottom)
			widget->move(QVector2D(0, refBottom - bottom));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotVCenterAlignWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 2);
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	UiWidget *refWidget = widgetIt.next();
	int refCenter = refWidget->getWidgetCenterPoint().y();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int center = widget->getWidgetCenterPoint().y();
		if(center != refCenter)
			widget->move(QVector2D(0, refCenter - center));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotHEqualWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 3);
	QMultiMap<int,UiWidget*> sortMap; //sort widget by it's left bound
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int left = widget->getWidgetLTPoint().x();
		sortMap.insert(left, widget);
	}

	QMultiMap<int,UiWidget*>::iterator begin = sortMap.begin();
	QMultiMap<int,UiWidget*>::iterator end = sortMap.end() - 1;
	int spaceLeft = begin.value()->getWidgetRBPoint().x();
	int spaceRight = end.value()->getWidgetLTPoint().x();
	int spaceWidth = spaceRight - spaceLeft;
	int spaceCount = sortMap.count() - 1;
	int widgetsWidth = 0;
	for(begin ++; begin != end; begin ++)
	{
		UiWidget *widget = begin.value();
		widgetsWidth += widget->getWidgetWidth();
	}
	spaceWidth -= widgetsWidth;

	int dis = spaceWidth / spaceCount;
	int mod = spaceWidth % spaceCount;
	if(spaceCount - mod < mod)
		dis += 1;

	begin = sortMap.begin();
	end = sortMap.end();
	int preRight = begin.value()->getWidgetRBPoint().x();
	for(begin++; begin != end; begin++)
	{
		UiWidget *widget = begin.value();
		int destLeft = preRight + dis;
		int curLeft = widget->getWidgetLTPoint().x();
		widget->move(QVector2D(destLeft - curLeft, 0));
		preRight = widget->getWidgetRBPoint().x();
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotVEqualWidget()
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	assert(selectedParents.count() >= 3);
	QMultiMap<int,UiWidget*> sortMap; //sort widget by it's top bound
	QVectorIterator<UiWidget*> widgetIt(selectedParents);
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int top = widget->getWidgetLTPoint().y();
		sortMap.insert(top, widget);
	}

	QMultiMap<int,UiWidget*>::iterator begin = sortMap.begin();
	QMultiMap<int,UiWidget*>::iterator end = sortMap.end() - 1;
	int spaceTop = begin.value()->getWidgetRBPoint().y();
	int spaceBottom = end.value()->getWidgetLTPoint().y();
	int spaceHeight = spaceBottom - spaceTop;
	int spaceCount = sortMap.count() - 1;
	int widgetsHeight = 0;
	for(begin ++; begin != end; begin ++)
	{
		UiWidget *widget = begin.value();
		widgetsHeight += widget->getWidgetHeight();
	}
	spaceHeight -= widgetsHeight;

	int dis = spaceHeight / spaceCount;
	int mod = spaceHeight % spaceCount;
	if(spaceCount - mod < mod)
		dis += 1;

	begin = sortMap.begin();
	end = sortMap.end();
	int preBottom = begin.value()->getWidgetRBPoint().y();
	for(begin++; begin != end; begin++)
	{
		UiWidget *widget = begin.value();
		int destTop = preBottom + dis;
		int curTop = widget->getWidgetLTPoint().y();
		widget->move(QVector2D(0, destTop - curTop));
		preBottom = widget->getWidgetRBPoint().y();
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotEqualWidgetWidth()
{
	assert(_selectedWidgets.count() > 1);
	QVectorIterator<UiWidget*> widgetIt(_selectedWidgets);
	UiWidget *refWidget = widgetIt.next();
	int refWidth = refWidget->getWidgetWidth();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int height = widget->getWidgetHeight();
		widget->resize(QSize(refWidth, height));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotEqualWidgetHeight()
{
	assert(_selectedWidgets.count() > 1);
	QVectorIterator<UiWidget*> widgetIt(_selectedWidgets);
	UiWidget *refWidget = widgetIt.next();
	int refHeight = refWidget->getWidgetHeight();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		int width = widget->getWidgetWidth();
		widget->resize(QSize(width, refHeight));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotEqualWidgetBoth()
{
	assert(_selectedWidgets.count() > 1);
	QVectorIterator<UiWidget*> widgetIt(_selectedWidgets);
	UiWidget *refWidget = widgetIt.next();
	int refWidth = refWidget->getWidgetWidth();
	int refHeight = refWidget->getWidgetHeight();
	while(widgetIt.hasNext())
	{
		UiWidget *widget = widgetIt.next();
		widget->resize(QSize(refWidth, refHeight));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotHCenterToParent()
{
	QVector<UiWidget*> widgets = UiWidgetFactory::getSingletonPtr()->checkChildInWidgets(_selectedWidgets);
	assert(widgets.count() > 0);
	QVectorIterator<UiWidget*> widgetIterator(widgets);
	while (widgetIterator.hasNext())
	{
		UiWidget *widget = widgetIterator.next();
		UiWidget *parent = widget->getParent();
		assert(parent != 0);
		int moveY = parent->getWidgetCenterPoint().y() - widget->getWidgetCenterPoint().y();
		widget->move(QVector2D(0, moveY));
	}
	setSelectedWidget(_selectedWidgets);
}

void CentralWidget::slotVCenterToParent()
{
	QVector<UiWidget*> widgets = UiWidgetFactory::getSingletonPtr()->checkChildInWidgets(_selectedWidgets);
	assert(widgets.count() > 0);
	QVectorIterator<UiWidget*> widgetIterator(widgets);
	while (widgetIterator.hasNext())
	{
		UiWidget *widget = widgetIterator.next();
		UiWidget *parent = widget->getParent();
		assert(parent != 0);
		int moveX = parent->getWidgetCenterPoint().x() - widget->getWidgetCenterPoint().x();
		widget->move(QVector2D(moveX, 0));
	}
	setSelectedWidget(_selectedWidgets);
}

SizeController CentralWidget::getSizeController( const QPoint &point )
{
	QVectorIterator<SizeController> iter(_sizeControllers);
	while(iter.hasNext())
	{
		SizeController sc = iter.next();
		assert(sc.isValid());
		const QRect rect = sc.getRect();
		if(rect.contains(point))
			return sc;
	}
	return SizeController();
}

void CentralWidget::moveWidgets( const QVector<UiWidget*> &widgets, const QVector2D &v )
{
	QVector<UiWidget*> topsWidget = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(widgets);
	QVectorIterator<UiWidget*> iter(topsWidget);
	while(iter.hasNext())
	{
		UiWidget *widget = iter.next();
		widget->move(v);
		//signal_emit widgetPropertyChanged(widget, QString::fromLatin1("absoluteRegion"), widget->property("absoluteRegion"));
		widgetPropertyChanged(widget, QString::fromLatin1("Geometry"), widget->property("Geometry"));
	}
}

void CentralWidget::setWidgetsVisiable( const QVector<UiWidget*> &widgets, bool visible )
{
	QVectorIterator<UiWidget*> iter(widgets);
	while(iter.hasNext())
	{
		UiWidget *widget = iter.next();
		widget->setVisible(visible);
		if(visible)
			widgetAdded(widget); // 显示的时候要确保对于其它控件相当于添加
		QVector<UiWidget*> childs = widget->getChild().toVector();
		setWidgetsVisiable(childs, visible);
	}
	if(!visible)
		widgetsDeleted(widgets); // 对于其它控件类似于删除
}

void CentralWidget::setWidgetsParent( UiWidget* parent, const QVector<UiWidget*> &widgets, bool move /*= false*/, int destX /*= 0*/, int destY /*= 0*/ )
{
	QVector<UiWidget*> topws = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(widgets);
	QVectorIterator<UiWidget*> iter(topws);
	while(iter.hasNext())
	{
		UiWidget *widget = iter.next();
		widget->setParent(parent);
		widgetParentChanged(widget);
	}

	if(move)
	{
		QRect widgetsBound = getWidgetsBound(widgets);
		if(parent != 0)
		{
			QPoint parentLT = parent->getWidgetLTPoint();
			destX += parentLT.x();
			destY += parentLT.y();
		}
		moveWidgets(topws, QVector2D(destX - widgetsBound.left(), destY - widgetsBound.top()));
	}
}

QRect CentralWidget::getWidgetsBound( const QVector<UiWidget*> &widgets )
{
	QRect rect;
	QVectorIterator<UiWidget*> iter(widgets);

	if(iter.hasNext())
		rect = iter.next()->getAbsoluteRegion();

	while(iter.hasNext())
	{
		QRect wr = iter.next()->getAbsoluteRegion();
		if(rect.left() > wr.left())
			rect.setLeft(wr.left());
		if(rect.top() > wr.top())
			rect.setTop(wr.top());
		if(rect.right() < wr.right())
			rect.setRight(wr.right());
		if(rect.bottom() < wr.bottom())
			rect.setBottom(wr.bottom());
	}

	return rect;
}

void CentralWidget::cutSelectedWidgets()
{
	QVector<UiWidget*> widgets = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);

	QVector<UiWidget*> widgetss;
	QVectorIterator<UiWidget*> widgetsIter(widgets);
	while(widgetsIter.hasNext())
	{
		UiWidget *widget = widgetsIter.next();
		if(!widget->getInteraction())
			widgetss.push_back(widget);
	}

	WidgetClipBoard::getSingletonPtr()->setCut(widgetss);
	setSelectedWidget(QVector<UiWidget*>());
	setWidgetsVisiable(widgetss, false);
}

void CentralWidget::copySelectedWidgets()
{
	if(WidgetClipBoard::getSingletonPtr()->getType() == WidgetClipBoard::CUT_OPERATE)
	{
		QVector<UiWidget*> ws = WidgetClipBoard::getSingletonPtr()->getWidgets();
		UiWidgetFactory::getSingletonPtr()->deleteWidgets(ws);
	}
	QVector<UiWidget*> widgets = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(_selectedWidgets);
	WidgetClipBoard::getSingletonPtr()->setCopy(widgets);
}

void CentralWidget::pasteWidgets()
{
	UiWidget *parent = 0;
	if(_selectedWidgets.count() > 0)
		parent = _selectedWidgets[0];

	int destX = 0, destY = 0;
	if(_mouseIn)
	{
		if(parent == 0)
		{
		}
		else
		{
			QRect rect = parent->getAbsoluteRegion();
			//if(rect.contains(_mousePos))
			{
				destX =  rect.right() + rect.width();
				destY =  rect.top();
			}
		}
	}

	QVector<UiWidget*> widgets = WidgetClipBoard::getSingletonPtr()->getWidgets();
	if(WidgetClipBoard::getSingletonPtr()->getType() == WidgetClipBoard::CUT_OPERATE)
	{
		setWidgetsVisiable(widgets, true);
		setWidgetsParent(parent, widgets, true, destX, destY);
		setSelectedWidget(widgets);
	}
	else if(WidgetClipBoard::getSingletonPtr()->getType() == WidgetClipBoard::COPY_OPERATE)
	{
		QVector<UiWidget*> limits = WidgetClipBoard::getSingletonPtr()->getLimits();
		QVector<UiWidget*> newWidgets = UiWidgetFactory::getSingletonPtr()->copyWidgets(widgets,limits);
		addUiWidgets(newWidgets);
		setWidgetsParent(parent ? parent->getParent() : nullptr, newWidgets, true, destX, destY);
		setSelectedWidget(newWidgets);
	}
}

void CentralWidget::save( const QString &fileName )
{
	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml version=\"1.0\"", "encoding=\"UTF-8\""));
	QDomElement uiEle = doc.createElement("ui");
	QVectorIterator<UiWidget*> topWidgets(UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(
		UiWidgetFactory::getSingletonPtr()->getWidgets()));
	while(topWidgets.hasNext())
		uiEle.appendChild( topWidgets.next()->serialize(doc, QDomElement()));
	doc.appendChild(uiEle);


	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))
		return;
	file.write(doc.toByteArray());
	file.close();

	saveFontFile();
}

bool CentralWidget::open( const QString &fileName )
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
		return false;

	/*
	QXmlStreamReader xmlReader(&file);
	while(xmlReader.readNextStartElement())
	{
		if(xmlReader.name() == QLatin1String("ui"))
			xmlReader.readNextStartElement();

		UiWidget *widget = UiWidgetFactory::getSingletonPtr()->createWidget(xmlReader);
		if(widget != 0)
		{
			widgetAdded(widget);
		}

		xmlReader.skipCurrentElement();
	}
	file.close();

	*/

	QXmlStreamReader xmlReader(&file);
	xmlReader.readNext();
	while (!xmlReader.atEnd())
	{
		if (xmlReader.isStartElement())
		{
			if (xmlReader.name() == "Element")
			{
				UiWidget *widget = UiWidgetFactory::getSingletonPtr()->createWidget(xmlReader);
				assert(widget != NULL);
				if (validWidgetName(widget, widget->getWidgetName()))
				{
					if (widget != 0)
					{
						MainWindow::getInst()->_objectViewerDockWidget->slotWidgetAdded(widget);
						//widgetAdded(widget);
					}
				}
				else
					UiWidgetFactory::getSingletonPtr()->destroyWidget(widget);
			}
		}
		xmlReader.readNext();
	}
	file.close();


	return true;
}

void CentralWidget::saveFontFile()
{
	return;
}

void CentralWidget::setScaleFactor(double scale)
{
	_scale = scale;
}


void CentralWidget::cacheWidgetsPos(std::vector<UiWidget*>& widgets, std::vector<QRect>& rects)
{
	for (int i = 0; i < UiWidgetFactory::getSingletonPtr()->getWidgets().size(); ++i)
	{
		widgets.push_back(UiWidgetFactory::getSingletonPtr()->getWidgets()[i]);
		QRect rc = UiWidgetFactory::getSingletonPtr()->getWidgets()[i]->property("Geometry").value<QRect>();
		rects.push_back(rc);
	}
	QVector<UiWidget*> tmp;
	setSelectedWidget(tmp);
}
void CentralWidget::updateWidgetsPos(std::vector<UiWidget*>& widgets, std::vector<QRect>& rects)
{
	for (int i = 0; i < widgets.size(); ++i)
	{
		QRect rc = rects[i];
		RPolicy policy = widgets[i]->getPolicy();
		UiWidget* parents = widgets[i]->getParent();
		if (policy.dpolicy == UiWidget::Stretch)
		{
			rc.setX(0);
			rc.setY(0);
			double w = parents ? parents->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
			double h = parents ? parents->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
			rc.setWidth(w);
			rc.setHeight(h);
			widgets[i]->setRelativeRegion(rc);
		}
		else if (policy.dpolicy == UiWidget::H_Match)
		{
			double w = rc.width();
			double h = rc.height();
			double pw = parents ? parents->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
			double ph = parents ? parents->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
			rc.setX(0);
			rc.setY(0);
			rc.setWidth(pw);
			rc.setHeight(pw / w * h);
			widgets[i]->setRelativeRegion(rc);
		}
		else if (policy.dpolicy == UiWidget::V_Match)
		{
			double w = rc.width();
			double h = rc.height();
			double pw = parents ? parents->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
			double ph = parents ? parents->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
			rc.setX(0);
			rc.setY(0);
			rc.setWidth(ph / h * w);
			rc.setHeight(ph);
			widgets[i]->setRelativeRegion(rc);
		}
		else
			widgets[i]->setRelativeRegion(rects[i]);
	}
	for (int i = 0; i < widgets.size(); ++i)
	{
		QRect rc = rects[i];
		RPolicy policy = widgets[i]->getPolicy();
		UiWidget* parents = widgets[i]->getParent();
		if (policy.dpolicy == UiWidget::Stretch)
		{
			rc.setX(0);
			rc.setY(0);
			double w = parents ? parents->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
			double h = parents ? parents->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
			rc.setWidth(w);
			rc.setHeight(h);
			widgets[i]->setRelativeRegion(rc);
		}
		else if (policy.dpolicy == UiWidget::H_Match)
		{
			double w = rc.width();
			double h = rc.height();
			double pw = parents ? parents->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
			double ph = parents ? parents->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
			rc.setX(0);
			rc.setY(0);
			rc.setWidth(pw);
			rc.setHeight(pw / w * h);
			widgets[i]->setRelativeRegion(rc);
		}
		else if (policy.dpolicy == UiWidget::V_Match)
		{
			double w = rc.width();
			double h = rc.height();
			double pw = parents ? parents->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
			double ph = parents ? parents->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
			rc.setX(0);
			rc.setY(0);
			rc.setWidth(ph / h * w);
			rc.setHeight(ph);
			widgets[i]->setRelativeRegion(rc);
		}
		else
			widgets[i]->setRelativeRegion(rects[i]);
	}
}

void CentralWidget::clean()
{
	QVector<UiWidget*> tmp;
	widgetSelectedChanged(tmp);
	QVector<SizeController> tmp2;
	drawSizeControllers(tmp2);
	QVector<c_widget*>tmp3;
	setWidgetFrame(tmp3, NULL);
	QVector<UiWidget*> deleteWidget = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(
		UiWidgetFactory::getSingletonPtr()->getWidgets());
	UiWidgetFactory::getSingletonPtr()->deleteWidgets(deleteWidget);
	deleteWidget.clear();
	_selectedWidgets.clear();
	_unSelectedWidgets.clear();
}
