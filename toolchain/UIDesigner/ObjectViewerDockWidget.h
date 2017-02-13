#ifndef OBJECTVIEWERDOCKWIDGET_H
#define OBJECTVIEWERDOCKWIDGET_H

#include "GeneratedFiles/ui_objectviewerdockwidget.h"
#include <QtWidgets/QItemDelegate>

class UiWidget;

class ObjectNameEditerDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	ObjectNameEditerDelegate(QObject *parent = 0);
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class ObjectViewerDockWidget : public QWidget , public Ui::ObjectViewerDockWidget
{
	Q_OBJECT
public:
	ObjectViewerDockWidget(QWidget *parent = 0);
	void emitSignals(bool yes);
	void setUserdata(void* ptr) { _userdata = ptr; }
	void clean();
protected:
	virtual void keyPressEvent(QKeyEvent* event);
protected slots:
	void slotWidgetAdded(UiWidget *widget);
	void slotWidgetsDeleted(const QVector<UiWidget*> &widgets);
	void slotWidgetSelectedChanged(const QVector<UiWidget*> &widgets);
	void slotWidgetParentChanged(UiWidget *widget);
	void selectionItemChanged();
	void treeItemDataChanged(QTreeWidgetItem * item, int column);
	void slotWidgetNameChanged(UiWidget *widget, QString name);
	void slotWidgetPropertyChanged(UiWidget *widget, const QString &property, const QVariant &value);
signals:
	void selectionItemChanged(const QVector<UiWidget*> &widgets);
	void widgetNameChanged(UiWidget *widget, const QString &name);
private:
	void initUi();
	void initSignal();
	QTreeWidgetItem *findItemByUiWidget(const UiWidget *widget, QTreeWidgetItem *parent = 0);
	UiWidget *getUiWidgetByItem(const QTreeWidgetItem*item);
	bool treeItemHaveValue(const UiWidget *widget, QTreeWidgetItem *item);
	void expandedTreeItem(QTreeWidgetItem *item);
	void setItemsSelected(const QVector<UiWidget*> selectWidgets);
	QTreeWidgetItem *createTreeWidgetItem(UiWidget *widget);
private:
	ObjectNameEditerDelegate *_nameEditDelegate;
	void* _userdata;
};

#endif //OBJECTVIEWERDOCKWIDGET_H