#include <QtCore/QMetaObject>
#include <QtCore/QMetaClassInfo>
#include <QtWidgets/QTreeWidgetItem>
#include <QtCore/QListIterator>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>

#include "macro.h"
#include "PropertyEditerDockWidget.h"
#include "UiWidget.h"
#include "QtVariantPropertyManager"
#include "QtVariantEditorFactory"
#include "QtProperty"
#include "QtVariantProperty"
#include "FocusEventFilter.h"
#include "PropertyManager.h"
#include "PropertyEditorFactory.h"
//#include "FontSetDialog.h"

UiWidget *PropertyEditerDockWidget::_uiWidget = NULL;

PropertyEditerDockWidget* PropertyEditerDockWidget::self = nullptr;
PropertyEditerDockWidget::PropertyEditerDockWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	, _selectedTreeItem(0)
{
	initUi();
	initSignal();

	//此处可能是Qt的一个Bug，先这样处理
	qMetaTypeId<TextAlign>();
	qMetaTypeId<PtAlign>();
	qMetaTypeId<RPolicy>();
	qMetaTypeId<ROrientation>();
	qMetaTypeId<SkinFile>();
	qMetaTypeId<SkinImageEx>();
	qMetaTypeId<SkinImage>();
	qMetaTypeId<SkinSet>();
	qMetaTypeId<FontFile>();
	qMetaTypeId<FontType>();
	qMetaTypeId<TableItemMap>();
	qMetaTypeId<CartoonBoxFrames>();
	qMetaTypeId<NumericSet>();
	self = this;
}


void PropertyEditerDockWidget::clean()
{
	setUiWidget(0);
}
void PropertyEditerDockWidget::initUi()
{
	setupUi(this);

	connectFactoryToManager(new QtVariantPropertyManager, new QtVariantEditorFactory);
	connectFactoryToManager<TextAlignPropertyManager>(new TextAlignPropertyManager, 0);
	connectFactoryToManager<PtAlignPropertyManager>(new PtAlignPropertyManager, 0);
	connectFactoryToManager<RPolicyPropertyManager>(new RPolicyPropertyManager, 0);
	connectFactoryToManager<ROrientationPropertyManager>(new ROrientationPropertyManager, 0);
	connectFactoryToManager<SkinFilePropertyManager>(new SkinFilePropertyManager, new SkinEditorFactory);
	connectFactoryToManager<FontTypePropertyManager>(new FontTypePropertyManager, 0);
	connectFactoryToManager<SkinImageExPropertyManager>(new SkinImageExPropertyManager, new SkinImageExEditorFactory);
	connectFactoryToManager<SkinImagePropertyManager>(new SkinImagePropertyManager, new SkinImageEditorFactory);
	connectFactoryToManager<NumericSetPropertyManager>(new NumericSetPropertyManager, 0);

	_tree->setResizeMode(QtTreePropertyBrowser::Interactive);
	_tree->setPropertiesWithoutValueMarked(true);
	_tree->setRootIsDecorated(false);
	_tree->setFocusPolicy(Qt::StrongFocus);
	blockSignals(true);
}

void PropertyEditerDockWidget::initSignal()
{

}

void PropertyEditerDockWidget::setUiWidget( UiWidget *uiWidget)
{
	if(_uiWidget != uiWidget)
	{
		disconnect(_uiWidget, SIGNAL(userEnumNamesChanged(const QString&, const QVariant&)),
			this,SLOT(slotUserEnumNamesChanged(const QString&, const QVariant&)));
		_uiWidget = uiWidget;
		initProperty();
	}
}

void PropertyEditerDockWidget::initProperty()
{
	_describe->setText(QString(""));
	clearPropertyTree();

	if(_uiWidget == 0)
		return;

	connect(_uiWidget, SIGNAL(userEnumNamesChanged(const QString&, const QVariant&)),
		this,SLOT(slotUserEnumNamesChanged(const QString&, const QVariant&)));

	_describe->setText(QString("%1 : %2").arg(_uiWidget->getWidgetName()).arg(_uiWidget->getUiClassName()));

	QList<QtProperty*> topItemList;
	const QMetaObject *metaObj = _uiWidget->metaObject();
	assert(metaObj != 0);
	int offset = 0;
	int count = metaObj->propertyCount();
	while(metaObj != 0 && QString(metaObj->className()) != QString("QObject"))
	{
		int uiClassNameInfoIndex = metaObj->indexOfClassInfo("uiClassName");
		assert(uiClassNameInfoIndex != -1);
		QString uiClassName = QString(metaObj->classInfo(uiClassNameInfoIndex).value());
		QtProperty *topItem = addPropertyAndSetValue(QtVariantPropertyManager::groupTypeId(), uiClassName);
		topItemList.push_front(topItem);

		int offset = metaObj->propertyOffset();
		for(int i = offset; i < count; i ++)
		{
			QMetaProperty property = metaObj->property(i);
			assert(property.isValid());
			if(!property.isDesignable())
				continue;

			QString propertyName = QString(property.name());
			QVariant value = property.read(_uiWidget);
			if(property.isEnumType())
			{
				if(QtProperty *item = addPropertyAndSetValue(QtVariantPropertyManager::enumTypeId(), propertyName, value))
				{
					_nameToProperty[propertyName] = item;
					topItem->addSubProperty(item);
					QtVariantProperty *p = dynamic_cast<QtVariantProperty*>(item);
					assert(p != 0);
					QStringList enumNames;
					QMetaEnum metaEnum = property.enumerator();
					int count = metaEnum.keyCount();
					for(int i = 0; i < count; i ++)
						enumNames.append(QString::fromLatin1(metaEnum.key(i)));
					p->setAttribute(QString::fromLatin1("enumNames"), enumNames);
				}
			}
			else
			{
				if(QtProperty *item = addPropertyAndSetValue(value.userType(), propertyName, value))
				{
					_nameToProperty[propertyName] = item;
					topItem->addSubProperty(item);
				}
				else
				{
					int typeID = value.userType();
					if(typeID >= QVariant::UserType && 
						typeID != QtVariantPropertyManager::enumTypeId() &&
						typeID != QtVariantPropertyManager::groupTypeId() &&
						typeID != QtVariantPropertyManager::flagTypeId() &&
						typeID != QtVariantPropertyManager::iconMapTypeId())
					{//user type, check is the data can treated as enum
						UserPropertyDataType *defV = (UserPropertyDataType *)QMetaType::create(typeID);
						if(defV->isAsEnumType())
						{// can treated as enum
							UserPropertyDataType *realV = defV->getInstanceFromQVariant(value);
							if(QtProperty *item = addPropertyAndSetValue(QtVariantPropertyManager::enumTypeId(), propertyName, value))
							{
								_nameToProperty[propertyName] = item;
								topItem->addSubProperty(item);
								QtVariantProperty *p = dynamic_cast<QtVariantProperty*>(item);
								assert(p != 0);
								p->setAttribute(QString::fromLatin1("enumNames"), realV->getEnumNames());
								p->setValue(realV->getIndex());
								_userEnumProperties[p] = typeID;
							}
							delete realV;
						}
						QMetaType::destroy(typeID, defV);
					}
				}
			}
		}
		count = offset;

		metaObj = metaObj->superClass();
	}

	for(QList<QtProperty*>::iterator it = topItemList.begin(); it != topItemList.end(); it ++)
		_tree->addProperty(*it);

	topItemList.clear();
}

void PropertyEditerDockWidget::clearPropertyTree()
{
	_tree->clear();
	QListIterator<QtAbstractPropertyManager*> it(_managersList);
	while(it.hasNext())
	{
		QtAbstractPropertyManager *m = it.next();
		m->clear();
	}
	_nameToProperty.clear();
	_userEnumProperties.clear();
}

void PropertyEditerDockWidget::slotWidgetSelectedChanged( const QVector<UiWidget*> &widgets )
{
	if(widgets.isEmpty())
		setUiWidget(0);
	else
		setUiWidget(widgets[0]);
}

void PropertyEditerDockWidget::setPropertyValue( const QString &propertyName, const QVariant &value )
{
	QtVariantProperty *p = dynamic_cast<QtVariantProperty*>(findProperty(propertyName));
	if(p != 0)
	{
		p->setValue(value);
	}
}


void PropertyEditerDockWidget::setPropertyXX(const QString &propertyName, const QVariant &value)
{
	QtVariantProperty *p = dynamic_cast<QtVariantProperty*>(findProperty(propertyName));
	if (p)
	{
		QListIterator<QtAbstractPropertyManager*> itManager(_managersList);
		while (itManager.hasNext())
		{
			QtAbstractPropertyManager *m = itManager.next();
			QtVariantPropertyManager *vm = dynamic_cast<QtVariantPropertyManager*>(m);
			if (vm)
				vm->setValue(p, value);
		}
	}
}


void PropertyEditerDockWidget::setPropertyFont(const QString &propertyName, const FontType &value)
{
	QtProperty *p = dynamic_cast<QtProperty*>(findProperty(propertyName));
	if (p)
	{
		QListIterator<QtAbstractPropertyManager*> itManager(_managersList);
		while (itManager.hasNext())
		{
			QtAbstractPropertyManager *m = itManager.next();
			FontTypePropertyManager *vm = dynamic_cast<FontTypePropertyManager*>(m);
			if (vm)
				vm->setValue2(p, value);
		}
	}
}

void PropertyEditerDockWidget::setPropertyNumic(const QString &propertyName, const NumericSet &value)
{
	QtProperty *p = dynamic_cast<QtProperty*>(findProperty(propertyName));
	if (p)
	{
		QListIterator<QtAbstractPropertyManager*> itManager(_managersList);
		while (itManager.hasNext())
		{
			QtAbstractPropertyManager *m = itManager.next();
			NumericSetPropertyManager *vm = dynamic_cast<NumericSetPropertyManager*>(m);
			if (vm)
				vm->setValue2(p, value);
		}
	}
}

QtAbstractPropertyManager* PropertyEditerDockWidget::getPtAlignMge()
{
	QListIterator<QtAbstractPropertyManager*> itManager(_managersList);
	while (itManager.hasNext())
	{
		QtAbstractPropertyManager *m = itManager.next();
		PtAlignPropertyManager *vm = dynamic_cast<PtAlignPropertyManager*>(m);
		if(vm)
			return vm;
	}
	return NULL;
}


QtAbstractPropertyManager* PropertyEditerDockWidget::getOriAlignMge()
{
	QListIterator<QtAbstractPropertyManager*> itManager(_managersList);
	while (itManager.hasNext())
	{
		QtAbstractPropertyManager *m = itManager.next();
		ROrientationPropertyManager *vm = dynamic_cast<ROrientationPropertyManager*>(m);
		if (vm)
			return vm;
	}
	return NULL;
}

QtProperty * PropertyEditerDockWidget::findProperty( const QString &propertyName )
{
	QMap<QString, QtProperty*>::iterator it = _nameToProperty.find(propertyName);
	if(it != _nameToProperty.end())
		return it.value();
	return 0;	
}

void PropertyEditerDockWidget::slotPropertyValueChanged( const QString &property, const QVariant &value )
{
	assert(_uiWidget != 0);
	setPropertyValue(property, value);
	dirty();
}

void PropertyEditerDockWidget::slotPropertyValueChanged( QtProperty *property, const QVariant &value )
{
	assert(_uiWidget != 0);
	dirty();
	QtVariantProperty *p = dynamic_cast<QtVariantProperty*>(property);
	if(p != 0)
	{
		const QMap<const QtVariantProperty*, int>::const_iterator it = _userEnumProperties.constFind(p);
		if(it != _userEnumProperties.constEnd())
		{
			assert(value.canConvert<int>());
			int typeID = it.value();
			int index = value.toInt();
			UserPropertyDataType *defV = (UserPropertyDataType*)QMetaType::create(typeID);
			defV->setIndex(index);
			widgetPropertyChanged(_uiWidget, property->propertyName(), defV->getQVariant());
			QMetaType::destroy(typeID, defV);
			return;
		}
	}
	widgetPropertyChanged(_uiWidget, property->propertyName(), value);
}

void PropertyEditerDockWidget::slotWidgetNameChanged( UiWidget *widget, const QString &name )
{
	dirty();
	assert(widget == _uiWidget);
	_describe->setText(QString("%1 : %2").arg(name).arg(_uiWidget->getUiClassName()));
	setPropertyValue(QString::fromLatin1("Name"), name);
}

void PropertyEditerDockWidget::emitSignals( bool yes )
{
	this->blockSignals( !yes );
	_tree->blockSignals( !yes );
}

void PropertyEditerDockWidget::slotWidgetPropertyChanged( UiWidget *widget, const QString &property, const QVariant &value )
{
	if(widget != _uiWidget)
		return;

	dirty();
	setPropertyValue(property, value);
}

QtProperty* PropertyEditerDockWidget::addPropertyAndSetValue( int propertyType, QString &propertyName, const QVariant &value /*= QVariant()*/ )
{
	QListIterator<QtAbstractPropertyManager*> itManager(_managersList);
	while(itManager.hasNext())
	{
		QtAbstractPropertyManager *m = itManager.next();
		QtVariantPropertyManager *vm = dynamic_cast<QtVariantPropertyManager*>(m);
		if(vm != 0)
		{
			if(vm->isPropertyTypeSupported(propertyType))
			{
				QtProperty *p = vm->addProperty(propertyType, propertyName);
				vm->setValue(p, value);
				return p;
			}
		}
		else
		{
			AbstractPropertyManager *am = dynamic_cast<AbstractPropertyManager*>(m);
			if(am != 0)
			{
				if(am->isPropertyTypeSupported(propertyType))
				{
					QtProperty *p = am->addProperty(propertyName);
					am->setValue(p, value);
					return p;
				}
			}
		}
	}
	return 0;
}

void PropertyEditerDockWidget::connectFactoryToManager( QtVariantPropertyManager *manager, QtVariantEditorFactory *factory )
{
	_tree->setFactoryForManager(manager, factory);
	connect(manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
		this, SLOT(slotPropertyValueChanged(QtProperty*, const QVariant&)));

	_managersList.append(manager);
}

void PropertyEditerDockWidget::slotUserEnumNamesChanged( const QString &propertyName, const QVariant &value )
{
	QtVariantProperty *property = dynamic_cast<QtVariantProperty*>(findProperty(propertyName));
	assert(property != 0);
	int typeID = value.userType();
	assert(typeID >= QVariant::UserType);
	UserPropertyDataType *defV = (UserPropertyDataType*)QMetaType::create(typeID);
	UserPropertyDataType *realV = defV->getInstanceFromQVariant(value);
	assert(realV->isAsEnumType());
	property->setValue(0);
	property->setAttribute(QString::fromLatin1("enumNames"), realV->getEnumNames());
	QMetaType::destroy(typeID, defV);
	dirty();
}