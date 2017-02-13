#include <QtWidgets/QMessageBox>
#include <QtCore/QMetaProperty>

#include "UiWidgetFactory.h"
#include "UiWidget.h"
#include "UiLabel.h"
#include "UiButton.h"
#include "UiRadio.h"
#include "UiEditBox.h"
#include "UiProgressBar.h"
#include "UiSlider.h"
#include "UIScrollBox.h"
#include "UiCartoonBox.h"
#include "UiListBox.h"
#include "UiRichBox.h"
#include "UiTableBox.h"
#include "UiTreeBox.h"
#include "UiDialog.h"
#include "UiTabCtl.h"
#include "UiDDSlot.h"

UiWidgetFactory * UiWidgetFactory::getSingletonPtr()
{
	static UiWidgetFactory uiWidgetFactory;
	return &uiWidgetFactory;
}

UiWidgetFactory::UiWidgetFactory()
{
	registerWidget("Panel", &UiDialog::staticMetaObject, WT_DIALOG);
	registerWidget("Label",&UiRichBox::staticMetaObject,WT_RICHBOX);
	registerWidget("Button",&UiButton::staticMetaObject,WT_BUTTON);
	registerWidget("Check",&UiRadio::staticMetaObject,WT_RADIO);
	registerWidget("Slider", &UiSlider::staticMetaObject, WT_SLIDER);
	registerWidget("Text", &UiEditBox::staticMetaObject, WT_EDITBOX);
	registerWidget("Progress",&UiProgressBar::staticMetaObject,WT_PROGRESSBAR);
	registerWidget("Table", &UiTableBox::staticMetaObject, WT_TABLEBOX);
	registerWidget("Palette", &UiCartoonBox::staticMetaObject, WT_CARTOONBOX);
	registerWidget("Dial", &UiLabel::staticMetaObject, WT_LABEL);
	registerWidget("Primitive", &UiTabCtl::staticMetaObject, WT_TABCTL);
	//Dial
	//Palette
	//Primitive


	//registerWidget("CartoonBox",&UiCartoonBox::staticMetaObject,WT_CARTOONBOX);
	//registerWidget("ListBox",&UiListBox::staticMetaObject,WT_LISTBOX);
	//registerWidget("Dialog",&UiDialog::staticMetaObject,WT_DIALOG);
	//registerWidget("TabCtl",&UiTabCtl::staticMetaObject,WT_TABCTL);
	//registerWidget("DragDrop",&UiDDSlot::staticMetaObject,WT_DRAGDROPSLOT);
	//registerWidget("ScrollBox",&UiScrollBox::staticMetaObject,WT_SCROLLBOX);
	//registerWidget("TreeBox",&UiTreebox::staticMetaObject,WT_TREEBOX);
	//registerWidget("RichBox",&UiRichBox::staticMetaObject,WT_RICHBOX);
	_widgetNameToType["Panel"] = "Panel";
	_widgetNameToType["Label"] = "Label";
	_widgetNameToType["Button"] = "Button";
	_widgetNameToType["Check"] = "Check";
	_widgetNameToType["Slider"] = "Slider";
	_widgetNameToType["Text"] = "Text";
	_widgetNameToType["Progress"] = "Progress";
	_widgetNameToType["Table"] = "Table";
	_widgetNameToType["Palette"] = "Palette";
	_widgetNameToType["Dial"] = "Dial";
	_widgetNameToType["Primitive"] = "Primitive";
	//!fixme 
	//Dial
	//Palette
	//Primitive

	//_widgetNameToType["rcb"] = "RichBox";
	//_widgetNameToType["ctn"] = "CartoonBox";
	//_widgetNameToType["lib"] = "ListBox";
	//_widgetNameToType["trb"] = "TreeBox";
	//_widgetNameToType["sbb"] = "ScrollBox";
	//_widgetNameToType["dlg"] = "Dialog";
	//_widgetNameToType["dds"] = "DragDrop";
	//_widgetNameToType["tab"] = "TabCtl";
}

void UiWidgetFactory::registerWidget( const QString &string,const QMetaObject *object,e_widget_type type )
{
	_widgetHash[string] = HashValue(object,0,type);
	_metaObjToWidgetName[object] = string;
}
bool UiWidgetFactory::_checkValid(const QString& name)
{
	for (int i = 0; i < getWidgets().count(); i++)
	{
		UiWidget *w = getWidgets()[i];
		if (name == w->getWidgetName())
		{
			return false;
		}
	}
	return true;
}
UiWidget * UiWidgetFactory::createWidget( const QString &widgetName, UiWidget *parent, const QPoint &pos )
{
	QHash<const QString,HashValue>::iterator value = _widgetHash.find(widgetName);
	if(value != _widgetHash.end())
	{
		QString name = widgetName + QString("_%1").arg(value->nameIndex);

		while (!_checkValid(name))
		{
			value->nameIndex++;
			name = widgetName + QString("_%1").arg(value->nameIndex);
		}
		

		const QMetaObject *obj = value->obj;
		UiWidget *widget = dynamic_cast<UiWidget*>(obj->newInstance());
		if(widget != 0)
		{
			QList<UiWidget*> uiWidgets = widget->createDefaultWidget(name,parent,pos,value->type);
			QListIterator<UiWidget*> uiWidgetsIter(uiWidgets);
			if(widget->valid())
			{
				//ÉèÖÃÄ¬ÈÏ×ÖÌå
				FontType ft;
				QStringList fonts = FontType::getFontsNameList();
				if(!fonts.empty())
				{
					FontFile ff = FontType::getFontFile(0);
					ft.setFontFile(ff);
				}

				while(uiWidgetsIter.hasNext())
				{
					UiWidget *uiWidget = uiWidgetsIter.next();
					if(!uiWidget->getInteraction())
						uiWidget->setWidgetName(name);
					_widgets.append(uiWidget);
					value->nameIndex++;
					if(uiWidget->getSkinFile().getFileName().isEmpty())
					{
						const QMetaObject *uiObj = uiWidget->metaObject();
						int propertyCount = uiObj->propertyCount();
						for(int i = 0; i < propertyCount; i ++)
						{
							QMetaProperty p = uiObj->property(i);
							if(p.userType() == qMetaTypeId<SkinImageEx>())
							{
								SkinImageEx skinImage;
								SkinSet sks;
								sks.setPoint(QPoint(0 , 0));
								sks.setSize(QSize(16 , 16));
								skinImage.setSkinSet(sks);
								skinImage.setSkinName("Nil");
								p.write(uiWidget, skinImage.getQVariant());
							}
						}
					}

					uiWidget->setFont(ft);
				}
				return widget;
			}
			else
			{
				delete widget;
			}
		}
	}
	else
	{
		QMessageBox::warning(0, QString("Warning"), QString("Can't create %1").arg(widgetName));
	}
	return 0;
}

UiWidget * UiWidgetFactory::createWidget( QXmlStreamReader &reader, UiWidget *parent /*= NULL */)
{
	Q_ASSERT(reader.isStartElement());
	if(reader.name() == QLatin1String("Element"))
	{
		QXmlStreamAttributes atts = reader.attributes();
		QString uiType = atts.value(QLatin1String("Type")).toString();
		QMap<QString, QString>::iterator iter = _widgetNameToType.find(uiType);
		if(iter != _widgetNameToType.end())
		{
			QHash<const QString,HashValue>::iterator value = _widgetHash.find(iter.value());
			if(value != _widgetHash.end())
			{
				QStringList region = atts.value(QLatin1String("Geometry")).toString().split(",");
				QString name = atts.value(QLatin1String("Name")).toString();
				QPoint pos;
				if(region.size() == 4)
				{
					pos = QPoint(region[0].toUInt(), region[1].toUInt());
				}

				const QMetaObject *obj = value->obj;
				UiWidget *widget = dynamic_cast<UiWidget*>(obj->newInstance());
				widget->createDefaultWidget(name, parent, pos, value->type);
				widget->setWidgetName(name);
				widget->read(reader);
				appendWidget(widget);
				return widget;
			}
		}
	}
	return NULL;
}

void UiWidgetFactory::appendWidget( UiWidget *widget )
{
	if(widget != NULL)
	{
		for (int i = 0; i < _widgets.size(); ++i)
		{
			if (_widgets[i] == widget)
				return;
		}
		_widgets.append(widget);
		const QList<UiWidget*> widgets = widget->getChild();
		QListIterator<UiWidget*> wit(widgets);
		while(wit.hasNext())
		{
			UiWidget *w = wit.next();
			appendWidget(w);
		}
	}
}

void UiWidgetFactory::destroyWidget( UiWidget *widget )
{
	_widgets.remove(_widgets.indexOf(widget));
	delete widget;
}

void UiWidgetFactory::deleteWidgets( const QVector<UiWidget*> &widgets )
{
	QVectorIterator<UiWidget*> iter(widgets);
	while(iter.hasNext())
	{
		UiWidget *widget = iter.next();
		if(!widget->getInteraction())
			destroyWidget(widget);
	}
}

UiWidget * UiWidgetFactory::copyWidget( UiWidget *widget ,UiWidget *parent /*= 0*/, const QVector<UiWidget*> &limits/* = QVector<UiWidget*>()*/)
{
	assert(widget != 0);
	const QMetaObject *obj = widget->metaObject();
	const QMap<const QMetaObject*, QString>::ConstIterator it = _metaObjToWidgetName.constFind(obj);
	if(it != _metaObjToWidgetName.constEnd())
	{
		QString widgetName = it.value();
		QPoint pos = widget->getWidgetLTPoint();
		UiWidget *newWidget = createWidget(widgetName, parent, pos);

		int propertyCount = obj->propertyCount();
		for(int i = 0; i < propertyCount; i ++)
		{
			QMetaProperty property = obj->property(i);
			if(QString::fromLatin1(property.name()) == QString::fromLatin1("Name") ||
				QString::fromLatin1(property.name()) == QString::fromLatin1("text"))
				continue;
			property.write(newWidget, property.read(widget));
		}

		QList<UiWidget*> childs = widget->getChild();
		QListIterator<UiWidget*> widgetIterator(childs);
		while(widgetIterator.hasNext())
		{
			UiWidget *w = widgetIterator.next();
			if(!w->getInteraction())
			{
				if(limits.isEmpty() || limits.contains(w))
					copyWidget(w, newWidget, limits);
			}
		}
		
		return newWidget;
	}

	return 0;
}

QVector<UiWidget*> UiWidgetFactory::copyWidgets( const QVector<UiWidget*> &widgets, const QVector<UiWidget*> &limits/* = QVector<UiWidget*>()*/ )
{
	QVector<UiWidget*> oldWidgets = checkTopParentsInWidgets(widgets);
	QVector<UiWidget*> newWidgets;
	QVectorIterator<UiWidget*> widgetIterator(oldWidgets);
	while(widgetIterator.hasNext())
	{
		UiWidget *widget = widgetIterator.next();
		if(limits.isEmpty() || limits.contains(widget))
			newWidgets.append(copyWidget(widget, 0, limits));
	}
	return newWidgets;
}

QVector<UiWidget*> UiWidgetFactory::checkTopParentsInWidgets( const QVector<UiWidget*> &widgets )
{
	QVector<UiWidget*> parents;
	QVectorIterator<UiWidget*> it1(widgets);
	QVectorIterator<UiWidget*> it2(widgets);
	bool add = true;
	while(it1.hasNext())
	{
		add = true;
		UiWidget* w1 = it1.next();
		it2.toFront();
		while(it2.hasNext())
		{
			UiWidget *w2 = it2.next();
			if(w2->isContains(w1))
			{
				add = false;
				break;
			}
		}
		if(add)
			parents.append(w1);
	}
	return parents;
}

QVector<UiWidget*> UiWidgetFactory::checkChildInWidgets( const QVector<UiWidget*> &widgets )
{
	QVector<UiWidget*> childs;
	QVectorIterator<UiWidget*> it(widgets);
	while(it.hasNext())
	{
		UiWidget *widget = it.next();
		if(widget->getParent() != 0)
			childs.append(widget);
	}

	return childs;
}

QVector<UiWidget*> UiWidgetFactory::getWidgetsOnTree( const QVector<UiWidget*> &parents )
{
	QVector<UiWidget*> returnV;
	QVectorIterator<UiWidget*> iter(parents);
	while(iter.hasNext())
	{
		UiWidget *widget = iter.next();
		returnV.append(widget);
		QVector<UiWidget*> childs = widget->getChild().toVector();
		returnV += getWidgetsOnTree(childs);
	}
	return returnV;
}