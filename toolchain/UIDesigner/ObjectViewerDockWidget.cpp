#include <QtWidgets/QLineEdit>
#include <QtGui/QDropEvent>
#include "CentralWidget.h"
#include "macro.h"
#include "ObjectViewerDockWidget.h"
#include "UiWidget.h"
#include "FocusEventFilter.h"
#include "PropertyEditerDockWidget.h"
ObjectNameEditerDelegate::ObjectNameEditerDelegate( QObject *parent /*= 0*/ )
	: QItemDelegate(parent)
{

}

QWidget * ObjectNameEditerDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	if(index.column() != 0)
		return 0;

	QLineEdit *edit = new QLineEdit(parent);
	edit->setFrame(false);
	edit->setValidator(new QRegExpValidator(QRegExp("[_0-9a-zA-Z]*")));
	return edit;
}

void ObjectNameEditerDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
	if(index.column() != 0)
		return;

	QString name = index.model()->data(index, Qt::EditRole).toString();
	QLineEdit *edit = static_cast<QLineEdit*>(editor);
	edit->setText(name);
}

void ObjectNameEditerDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	if(index.column() != 0)
		return;

	QLineEdit *edit = static_cast<QLineEdit*>(editor);
	assert(edit->parent() != 0);
	((QWidget*)edit->parent())->setFocus();
	QString name = edit->text();
	if (!name.isEmpty())
	{
		PropertyEditerDockWidget::getInstance()->setPropertyXX("Name", name);
		model->setData(index, name);
	}
}

void ObjectNameEditerDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	if(index.column() != 0)
		return;

	editor->setGeometry(option.rect);
}

QSize ObjectNameEditerDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	QSize size = QItemDelegate::sizeHint(option, index);
	size.setHeight(size.height() + 4);
	return size;
}
//////////////////////////////////////////////////////////////////////////
ObjectViewerDockWidget::ObjectViewerDockWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
{
	_userdata = 0;
	initUi();
	initSignal();
}

void ObjectViewerDockWidget::initUi()
{
	setupUi(this);
	_tree->setColumnWidth(0, 140);
	_tree->setColumnWidth(1, 65);
	_tree->setColumnWidth(2, 55);
	QTreeWidgetItem *topLevelItem = new QTreeWidgetItem;
	topLevelItem->setText(0, tr("Root"));
	topLevelItem->setText(1, tr("Desktop"));
	//topLevelItem->setFlags(Qt::ItemIsEnabled);

	_nameEditDelegate = new ObjectNameEditerDelegate;
	_tree->addTopLevelItem(topLevelItem);
	_tree->setItemDelegate(_nameEditDelegate);
	_tree->setFocusPolicy(Qt::StrongFocus);
	emitSignals(false);
}

void ObjectViewerDockWidget::initSignal()
{
	connect(_tree, SIGNAL(itemSelectionChanged()),this,SLOT(selectionItemChanged()));
	connect(_tree, SIGNAL(itemChanged (QTreeWidgetItem*, int)),this,SLOT(treeItemDataChanged(QTreeWidgetItem*, int)));
}

void ObjectViewerDockWidget::slotWidgetAdded( UiWidget *widget )
{
	if(widget->getInteraction())
		return;

	assert(_tree->topLevelItemCount() == 1);
	QTreeWidgetItem *parentItem = 0;
	if(widget->getParent() == 0)
		parentItem = _tree->topLevelItem(0);
	else
		parentItem = findItemByUiWidget(widget->getParent());

	//QString className = widget->getUiClassName();
	//QString objName = widget->getWidgetName();
	//QTreeWidgetItem *item = new QTreeWidgetItem;
	//item->setData(0, Qt::DisplayRole, objName);
	//item->setData(1, Qt::DisplayRole, className);
	//item->setData(2, Qt::UserRole, QVariant::fromValue(UiWidgetRef(widget)));
	//item->setFlags(item->flags() | Qt::ItemIsEditable);

	QTreeWidgetItem *item = createTreeWidgetItem(widget);
	parentItem->addChild(item);
	_tree->setCurrentItem(item);
	item->parent()->setExpanded(true);
}

QTreeWidgetItem * ObjectViewerDockWidget::createTreeWidgetItem( UiWidget *widget )
{
	if(widget == NULL)
		return NULL;

	QString className = widget->getUiClassName();
	QString objName = widget->getWidgetName();
	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setData(0, Qt::DisplayRole, objName);
	item->setData(1, Qt::DisplayRole, className);
	item->setData(2, Qt::DisplayRole, "");
	item->setCheckState(2, Qt::Unchecked);
	item->setData(3, Qt::UserRole, QVariant::fromValue(UiWidgetRef(widget)));
	item->setFlags(item->flags() | Qt::ItemIsEditable);

	QListIterator<UiWidget*> widgetsIter(widget->getChild());
	while(widgetsIter.hasNext())
	{
		UiWidget* widget = widgetsIter.next();
		QTreeWidgetItem *childItem = createTreeWidgetItem(widget);
		item->addChild(childItem);
	}

	return item;
}

QTreeWidgetItem *ObjectViewerDockWidget::findItemByUiWidget( const UiWidget *widget , QTreeWidgetItem *parent/* = 0*/)
{
	assert(_tree->topLevelItemCount() == 1);

	QTreeWidgetItem *parentItem = 0;
	if(parent == 0)
		parentItem = _tree->topLevelItem(0);
	else
		parentItem = parent;

	assert(parentItem != 0);

	if(widget == 0)
		return parentItem;

	int count = parentItem->childCount();
	for(int i = 0; i < count; i ++)
	{
		QTreeWidgetItem *item = parentItem->child(i);
		if(treeItemHaveValue(widget, item))
			return item;

		item = findItemByUiWidget(widget,item);
		if(item != 0)
			return item;
	}
	return 0;
}

UiWidget * ObjectViewerDockWidget::getUiWidgetByItem( const QTreeWidgetItem*item )
{
	assert(item != 0);
	QVariant v = item->data(3, Qt::UserRole);
	if(v.canConvert<UiWidgetRef>())
	{
		UiWidgetRef uiRef = v.value<UiWidgetRef>();
		return uiRef.widget;
	}
	return 0;
}

bool ObjectViewerDockWidget::treeItemHaveValue( const UiWidget *widget, QTreeWidgetItem *item )
{
	assert(widget != 0);

	if(widget == getUiWidgetByItem(item))
		return true;

	return false;
}

void ObjectViewerDockWidget::slotWidgetSelectedChanged(const QVector<UiWidget*> &widgets )
{
	_tree->clearSelection();
	int count = widgets.size();
	if(count == 0)
	{
		_tree->setCurrentItem(_tree->topLevelItem(0));
	}
	else
	{
		_tree->setSelectionMode(QAbstractItemView::MultiSelection);
		for(int i = 0; i < count; i ++)
		{
			QTreeWidgetItem *item = findItemByUiWidget(widgets[i]);
			if (!item)
			{
				_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
				return;
			}
			assert(item != 0);
			_tree->setCurrentItem(item);
		}
		_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	}
}

void ObjectViewerDockWidget::selectionItemChanged()
{
	QList<QTreeWidgetItem*> items = _tree->selectedItems();
	QVector<UiWidget*> widgets;
	int count = items.size();
	for(int i = 0; i < count; i ++)
	{
		UiWidget *widget = getUiWidgetByItem(items[i]);
		if(widget != 0)
			widgets.append(widget);
	}

	selectionItemChanged(widgets);
}

void ObjectViewerDockWidget::slotWidgetsDeleted( const QVector<UiWidget*> &widgets )
{
	int count = widgets.count();
	for(int i = 0; i < count; i ++)
	{
		UiWidget *widget = widgets[i];
		if(!widget->getInteraction())
		{
			QTreeWidgetItem *item = findItemByUiWidget(widgets[i]);
			if(item != 0)
			{
				item->parent()->removeChild(item);
			}
		}
	}
}

void diguiCheck(QTreeWidgetItem* item, Qt::CheckState cs)
{
	item->setCheckState(2, cs);
	for(int i = 0; i < item->childCount(); i++)
	{
		QTreeWidgetItem* child = item->child(i);
		if (child->checkState(2) != cs)
		{
			diguiCheck(child, cs);
		}
	}
}

void ObjectViewerDockWidget::treeItemDataChanged( QTreeWidgetItem * item, int column )
{
	if(column == 1 || item == 0)
		return;

	QString name = item->text(0);
	UiWidget *widget = getUiWidgetByItem(item);
	if (column == 2)
	{
		widget->Lock(item->checkState(2) == Qt::Checked);
		//diguiCheck(item, item->checkState(2));
	}
	else
	widgetNameChanged(widget, name);
}

void ObjectViewerDockWidget::slotWidgetNameChanged( UiWidget *widget, QString name )
{
	QTreeWidgetItem *item = findItemByUiWidget(widget);
	assert(item != 0);
	item->setText(0,name);
}

void ObjectViewerDockWidget::slotWidgetPropertyChanged( UiWidget *widget, const QString &property, const QVariant &value )
{
	assert(widget != 0);
	if(property == QString("Name"))
		slotWidgetNameChanged(widget, value.toString());
}

void ObjectViewerDockWidget::slotWidgetParentChanged( UiWidget *widget )
{
	QTreeWidgetItem *item = findItemByUiWidget(widget);
	assert(item != 0);
	assert(item->parent() != 0);
	item->parent()->removeChild(item);

	UiWidget *parent = widget->getParent();
	QTreeWidgetItem *parentItem = findItemByUiWidget(parent);
	assert(parentItem != 0);
	parentItem->addChild(item);
	expandedTreeItem(parentItem);
}

void ObjectViewerDockWidget::expandedTreeItem( QTreeWidgetItem *item )
{
	item->setExpanded(true);

	int count = item->childCount();
	for(int i = 0; i < count; i ++)
		expandedTreeItem(item->child(i));
}

void ObjectViewerDockWidget::setItemsSelected( const QVector<UiWidget*> selectWidgets )
{
}

void ObjectViewerDockWidget::emitSignals( bool yes )
{
	this->blockSignals( !yes );
	_tree->blockSignals( !yes );
}


void ObjectViewerDockWidget::clean()
{
	emitSignals(false);
	_tree->clear();
	QTreeWidgetItem *topLevelItem = new QTreeWidgetItem;
	topLevelItem->setText(0, tr("Root"));
	topLevelItem->setText(1, tr("Desktop"));
	//topLevelItem->setFlags(Qt::ItemIsEnabled);

	_tree->addTopLevelItem(topLevelItem);
	_tree->setFocusPolicy(Qt::StrongFocus);
	emitSignals(true);
}


void ObjectViewerDockWidget::keyPressEvent(QKeyEvent* event)
{
}