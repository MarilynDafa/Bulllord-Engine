#ifndef UIWIDGETFACTORY_H
#define UIWIDGETFACTORY_H

#include <QtCore/QHash>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaType>
#include <QtCore/QPoint>
#include <QtCore/QMap>
#include <QXmlStreamReader>
#include "Widget.h"

class UiWidget;

using namespace o2d;

struct HashValue
{
	const QMetaObject *obj;
	int nameIndex;
	e_widget_type type;

	HashValue()
	{
		this->obj = 0;
		this->nameIndex = 0;
		type = WT_WIDGET;
	}

	HashValue(const QMetaObject *obj, int nameIndex, e_widget_type type)
	{
		this->obj = obj;
		this->nameIndex = nameIndex;
		this->type = type;
	}
};
Q_DECLARE_METATYPE(HashValue);

using namespace o2d;

class UiWidgetFactory
{
public:
	static UiWidgetFactory *getSingletonPtr();
	UiWidget *createWidget(const QString &widgetName, UiWidget *parent, const QPoint &pos );
	UiWidget *createWidget( QXmlStreamReader &reader, UiWidget *parent = NULL);
	bool _checkValid(const QString& name);
	void destroyWidget(UiWidget *widget);
	void deleteWidgets(const QVector<UiWidget*> &widgets);
	void addWidgetRelation(c_widget* cwidget, UiWidget *uiwidget);
	void removeWidgetRelation(c_widget *cwidget);
	UiWidget *getUiWidgetFromCWidget(c_widget *cwidget);
	UiWidget *copyWidget(UiWidget *widget, UiWidget *parent = 0, const QVector<UiWidget*> &limits = QVector<UiWidget*>());
	QVector<UiWidget*> copyWidgets(const QVector<UiWidget*> &widgets, const QVector<UiWidget*> &limits = QVector<UiWidget*>());
	QVector<UiWidget*> checkTopParentsInWidgets(const QVector<UiWidget*> &widgets);
	QVector<UiWidget*> checkChildInWidgets(const QVector<UiWidget*> &widgets);
	QVector<UiWidget*> getWidgetsOnTree(const QVector<UiWidget*> &parents);
	const QVector<UiWidget*> &getWidgets() { return _widgets; }
private:
	UiWidgetFactory();
	void registerWidget(const QString &string,const QMetaObject *object,e_widget_type type);
	void appendWidget(UiWidget *widget);
private:
	QHash<const QString,HashValue> _widgetHash;
	QMap<const QMetaObject*, QString> _metaObjToWidgetName;
	QVector<UiWidget*> _widgets;
	QMap<QString, QString> _widgetNameToType;
};

#endif //UIWIDGETFACTORY_H