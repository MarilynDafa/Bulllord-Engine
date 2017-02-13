#include "macro.h"
#include "PropertyManager.h"
#include "QtEnumPropertyManager"
#include "QtEnumEditorFactory"
#include "QtTreePropertyBrowser"
#include "QtIntPropertyManager"
#include "PropertyEditorFactory.h"
#include "Config.h"

#include <QtCore/QMetaObject>


//////////////////////////////////////////////////////////////////////////

TextAlignPropertyManager::TextAlignPropertyManager( QObject *parent /*= 0*/ )
	: AbstractPropertyManager(parent)
{
	const QMetaObject &obj = UiWidget::staticMetaObject;
	_HAlignEnum = obj.enumerator(obj.indexOfEnumerator("HTextAlignment"));
	_VAlignEnum = obj.enumerator(obj.indexOfEnumerator("VTextAlignment"));

	_enumPropertyManager = new QtEnumPropertyManager(this);
	connect(_enumPropertyManager, SIGNAL(valueChanged(QtProperty*, int)),
		this, SLOT(slotEnumChanged(QtProperty*, int)));
	connect(_enumPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));
}

TextAlignPropertyManager::~TextAlignPropertyManager()
{
	clear();
}

TextAlign TextAlignPropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, TextAlign());
}

QString TextAlignPropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.constFind(property);
	if(it == _valueMap.constEnd())
		return QString();

	TextAlign align = it.value();
	QString hAlign = QString::fromLatin1(_HAlignEnum.valueToKey(align.horizontal));
	QString vAlign = QString::fromLatin1(_VAlignEnum.valueToKey(align.vertical));
	return QString("%1, %2").arg(hAlign).arg(vAlign);
}

void TextAlignPropertyManager::setValue( QtProperty *property, const TextAlign &align )
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;


	it.value() = align;
	_enumPropertyManager->setValue(_propertyToH[property], getEnumValueIndex(_HAlignEnum,align.horizontal));
	_enumPropertyManager->setValue(_propertyToV[property], getEnumValueIndex(_VAlignEnum,align.vertical));

	propertyChanged(property);
	valueChanged(property, align);
	emitValueChanged(property, QVariant::fromValue(align));
}

void TextAlignPropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	if(value.canConvert<TextAlign>())
	{
		TextAlign align = value.value<TextAlign>();
		setValue(property, align);
	}
}

bool TextAlignPropertyManager::isPropertyTypeSupported( int propertyType )
{
	if(propertyType == qMetaTypeId<TextAlign>())
		return true;
	return false;
}

void TextAlignPropertyManager::initializeProperty( QtProperty *property )
{
	TextAlign align;
	_valueMap[property] = align;

	QStringList hStr;
	int hCount = _HAlignEnum.keyCount();
	for(int i = 0; i < hCount; i ++)
		hStr.append(QString::fromLatin1(_HAlignEnum.key(i)));

	QStringList vStr;
	int vCount = _VAlignEnum.keyCount();
	for(int i = 0; i < hCount; i ++)
		vStr.append(QString::fromLatin1(_VAlignEnum.key(i)));

	QtProperty *hPro = _enumPropertyManager->addProperty(tr("Horizontal"));
	_enumPropertyManager->setEnumNames(hPro, hStr);
	_propertyToH[property] = hPro;
	_HToProperty[hPro] = property;
	property->addSubProperty(hPro);

	QtProperty *vPro = _enumPropertyManager->addProperty(tr("Vertical"));
	_enumPropertyManager->setEnumNames(vPro, vStr);
	_propertyToV[property] = vPro;
	_VToProperty[vPro] = property;
	property->addSubProperty(vPro);
}

void TextAlignPropertyManager::uninitializeProperty( QtProperty *property )
{
	QtProperty *hPro = _propertyToH[property];
	if(hPro)
	{
		_HToProperty.remove(hPro);
		delete hPro;
	}
	_propertyToH.remove(property);

	QtProperty *vPro = _propertyToV[property];
	if(vPro)
	{
		_VToProperty.remove(vPro);
		delete vPro;
	}
	_propertyToV.remove(property);
}

void TextAlignPropertyManager::slotEnumChanged( QtProperty *property, int value )
{
	if(QtProperty *prop = _HToProperty.value(property, 0))
	{
		TextAlign align = _valueMap[prop];
		align.horizontal = (UiWidget::HTextAlignment)_HAlignEnum.value(value);
		setValue(prop, align);
	}
	else if(QtProperty *prop = _VToProperty.value(property, 0))
	{
		TextAlign align = _valueMap[prop];
		align.vertical = (UiWidget::VTextAlignment)_VAlignEnum.value(value);
		setValue(prop, align);
	}
}

void TextAlignPropertyManager::slotPropertyDestroyed( QtProperty *property )
{
	if(QtProperty *prop = _HToProperty.value(property, 0))
	{
		_propertyToH[prop] = 0;
		_HToProperty.remove(property);
	}
	else if(QtProperty *prop = _VToProperty.value(property, 0))
	{
		_propertyToV[prop] = 0;
		_VToProperty.remove(property);
	}
}

void TextAlignPropertyManager::connectSubManagerFactory( QtTreePropertyBrowser *browser )
{
	browser->setFactoryForManager(_enumPropertyManager, new QtEnumEditorFactory);
}

int TextAlignPropertyManager::getEnumValueIndex( const QMetaEnum &metaEnum, int value ) const
{
	int count = metaEnum.keyCount();
	for(int i = 0; i < count; i ++)
	{
		if(value == metaEnum.value(i))
			return i;
	}
	assert(0);
	return -1;
}


//////////////////////////////////////////////////////////////////////////

PtAlignPropertyManager::PtAlignPropertyManager(QObject *parent /*= 0*/)
	: AbstractPropertyManager(parent)
{
	const QMetaObject &obj = UiWidget::staticMetaObject;
	_HAlignEnum = obj.enumerator(obj.indexOfEnumerator("HTextAlignment"));
	_VAlignEnum = obj.enumerator(obj.indexOfEnumerator("VTextAlignment"));

	_enumPropertyManager = new QtEnumPropertyManager(this);
	connect(_enumPropertyManager, SIGNAL(valueChanged(QtProperty*, int)),
		this, SLOT(slotEnumChanged(QtProperty*, int)));
	connect(_enumPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));
}

PtAlignPropertyManager::~PtAlignPropertyManager()
{
	clear();
}

PtAlign PtAlignPropertyManager::value(const QtProperty *property) const
{
	return _valueMap.value(property, PtAlign());
}

QString PtAlignPropertyManager::valueText(const QtProperty *property) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.constFind(property);
	if (it == _valueMap.constEnd())
		return QString();

	PtAlign align = it.value();
	QString hAlign = QString::fromLatin1(_HAlignEnum.valueToKey(align.horizontal));
	QString vAlign = QString::fromLatin1(_VAlignEnum.valueToKey(align.vertical));
	return QString("%1, %2").arg(hAlign).arg(vAlign);
}

void PtAlignPropertyManager::setValue(QtProperty *property, const PtAlign &align)
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if (it == _valueMap.constEnd())
		return;

	if (it.value().equals(align))
		return;

	it.value() = align;
	_enumPropertyManager->setValue(_propertyToH[property], getEnumValueIndex(_HAlignEnum, align.horizontal));
	_enumPropertyManager->setValue(_propertyToV[property], getEnumValueIndex(_VAlignEnum, align.vertical));

	propertyChanged(property);
	valueChanged(property, align);
	emitValueChanged(property, QVariant::fromValue(align));
}

void PtAlignPropertyManager::setValue(QtProperty *property, const QVariant &value)
{
	if (value.canConvert<PtAlign>())
	{
		PtAlign align = value.value<PtAlign>();
		setValue(property, align);
	}
}

bool PtAlignPropertyManager::isPropertyTypeSupported(int propertyType)
{
	if (propertyType == qMetaTypeId<PtAlign>())
		return true;
	return false;
}

void PtAlignPropertyManager::initializeProperty(QtProperty *property)
{
	PtAlign align;
	_valueMap[property] = align;

	QStringList hStr;
	int hCount = _HAlignEnum.keyCount();
	for (int i = 0; i < hCount; i++)
		hStr.append(QString::fromLatin1(_HAlignEnum.key(i)));

	QStringList vStr;
	int vCount = _VAlignEnum.keyCount();
	for (int i = 0; i < hCount; i++)
		vStr.append(QString::fromLatin1(_VAlignEnum.key(i)));

	QtProperty *hPro = _enumPropertyManager->addProperty(tr("Horizontal"));
	_enumPropertyManager->setEnumNames(hPro, hStr);
	_propertyToH[property] = hPro;
	_HToProperty[hPro] = property;
	property->addSubProperty(hPro);

	QtProperty *vPro = _enumPropertyManager->addProperty(tr("Vertical"));
	_enumPropertyManager->setEnumNames(vPro, vStr);
	_propertyToV[property] = vPro;
	_VToProperty[vPro] = property;
	property->addSubProperty(vPro);
}

void PtAlignPropertyManager::uninitializeProperty(QtProperty *property)
{
	QtProperty *hPro = _propertyToH[property];
	if (hPro)
	{
		_HToProperty.remove(hPro);
		delete hPro;
	}
	_propertyToH.remove(property);

	QtProperty *vPro = _propertyToV[property];
	if (vPro)
	{
		_VToProperty.remove(vPro);
		delete vPro;
	}
	_propertyToV.remove(property);
}

void PtAlignPropertyManager::slotEnumChanged(QtProperty *property, int value)
{
	if (QtProperty *prop = _HToProperty.value(property, 0))
	{
		PtAlign align = _valueMap[prop];
		align.horizontal = (UiWidget::HTextAlignment)_HAlignEnum.value(value);
		setValue(prop, align);
	}
	else if (QtProperty *prop = _VToProperty.value(property, 0))
	{
		PtAlign align = _valueMap[prop];
		align.vertical = (UiWidget::VTextAlignment)_VAlignEnum.value(value);
		setValue(prop, align);
	}
}

void PtAlignPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
	if (QtProperty *prop = _HToProperty.value(property, 0))
	{
		_propertyToH[prop] = 0;
		_HToProperty.remove(property);
	}
	else if (QtProperty *prop = _VToProperty.value(property, 0))
	{
		_propertyToV[prop] = 0;
		_VToProperty.remove(property);
	}
}

void PtAlignPropertyManager::connectSubManagerFactory(QtTreePropertyBrowser *browser)
{
	browser->setFactoryForManager(_enumPropertyManager, new QtEnumEditorFactory);
}

int PtAlignPropertyManager::getEnumValueIndex(const QMetaEnum &metaEnum, int value) const
{
	int count = metaEnum.keyCount();
	for (int i = 0; i < count; i++)
	{
		if (value == metaEnum.value(i))
			return i;
	}
	assert(0);
	return -1;
}

//////////////////////////////////////////////////////////////////////////

RPolicyPropertyManager::RPolicyPropertyManager(QObject *parent /*= 0*/)
	: AbstractPropertyManager(parent)
{
	const QMetaObject &obj = UiWidget::staticMetaObject;
	_dpolicy = obj.enumerator(obj.indexOfEnumerator("DrawPolicy"));

	_enumPropertyManager = new QtEnumPropertyManager(this);
	connect(_enumPropertyManager, SIGNAL(valueChanged(QtProperty*, int)),
		this, SLOT(slotEnumChanged(QtProperty*, int)));
	connect(_enumPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));
}

RPolicyPropertyManager::~RPolicyPropertyManager()
{
	clear();
}

RPolicy RPolicyPropertyManager::value(const QtProperty *property) const
{
	return _valueMap.value(property, RPolicy());
}

QString RPolicyPropertyManager::valueText(const QtProperty *property) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.constFind(property);
	if (it == _valueMap.constEnd())
		return QString();

	RPolicy align = it.value();
	QString hAlign = QString::fromLatin1(_dpolicy.valueToKey(align.dpolicy));
	return QString("%1").arg(hAlign);
}

void RPolicyPropertyManager::setValue(QtProperty *property, const RPolicy &align)
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if (it == _valueMap.constEnd())
		return;

	if (it.value().equals(align))
		return;

	it.value() = align;
	_enumPropertyManager->setValue(_propertyToH[property], getEnumValueIndex(_dpolicy, align.dpolicy));

	propertyChanged(property);
	valueChanged(property, align);
	emitValueChanged(property, QVariant::fromValue(align));
}

void RPolicyPropertyManager::setValue(QtProperty *property, const QVariant &value)
{
	if (value.canConvert<RPolicy>())
	{
		RPolicy align = value.value<RPolicy>();
		setValue(property, align);
	}
}

bool RPolicyPropertyManager::isPropertyTypeSupported(int propertyType)
{
	if (propertyType == qMetaTypeId<RPolicy>())
		return true;
	return false;
}

void RPolicyPropertyManager::initializeProperty(QtProperty *property)
{
	RPolicy align;
	_valueMap[property] = align;

	QStringList hStr;
	int hCount = _dpolicy.keyCount();
	for (int i = 0; i < hCount; i++)
		hStr.append(QString::fromLatin1(_dpolicy.key(i)));
	

	QtProperty *hPro = _enumPropertyManager->addProperty(tr("Policy"));
	_enumPropertyManager->setEnumNames(hPro, hStr);
	_propertyToH[property] = hPro;
	_HToProperty[hPro] = property;
	property->addSubProperty(hPro);


}

void RPolicyPropertyManager::uninitializeProperty(QtProperty *property)
{
	QtProperty *hPro = _propertyToH[property];
	if (hPro)
	{
		_HToProperty.remove(hPro);
		delete hPro;
	}
	_propertyToH.remove(property);

}

void RPolicyPropertyManager::slotEnumChanged(QtProperty *property, int value)
{
	if (QtProperty *prop = _HToProperty.value(property, 0))
	{
		RPolicy align = _valueMap[prop];
		align.dpolicy = (UiWidget::DrawPolicy)_dpolicy.value(value);
		setValue(prop, align);
	}
}

void RPolicyPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
	if (QtProperty *prop = _HToProperty.value(property, 0))
	{
		_propertyToH[prop] = 0;
		_HToProperty.remove(property);
	}
}

void RPolicyPropertyManager::connectSubManagerFactory(QtTreePropertyBrowser *browser)
{
	browser->setFactoryForManager(_enumPropertyManager, new QtEnumEditorFactory);
}

int RPolicyPropertyManager::getEnumValueIndex(const QMetaEnum &metaEnum, int value) const
{
	int count = metaEnum.keyCount();
	for (int i = 0; i < count; i++)
	{
		if (value == metaEnum.value(i))
			return i;
	}
	assert(0);
	return -1;
}


//////////////////////////////////////////////////////////////////////////

ROrientationPropertyManager::ROrientationPropertyManager(QObject *parent /*= 0*/)
	: AbstractPropertyManager(parent)
{
	const QMetaObject &obj = UiWidget::staticMetaObject;
	_Orientation = obj.enumerator(obj.indexOfEnumerator("eOrientation"));

	_enumPropertyManager = new QtEnumPropertyManager(this);
	connect(_enumPropertyManager, SIGNAL(valueChanged(QtProperty*, int)),
		this, SLOT(slotEnumChanged(QtProperty*, int)));
	connect(_enumPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));
}

ROrientationPropertyManager::~ROrientationPropertyManager()
{
	clear();
}

ROrientation ROrientationPropertyManager::value(const QtProperty *property) const
{
	return _valueMap.value(property, ROrientation());
}

QString ROrientationPropertyManager::valueText(const QtProperty *property) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.constFind(property);
	if (it == _valueMap.constEnd())
		return QString();

	ROrientation align = it.value();
	QString hAlign = QString::fromLatin1(_Orientation.valueToKey(align.orientation));
	return QString("%1").arg(hAlign);
}

void ROrientationPropertyManager::setValue(QtProperty *property, const ROrientation &align)
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if (it == _valueMap.constEnd())
		return;

	if (it.value().equals(align))
		return;

	it.value() = align;
	_enumPropertyManager->setValue(_propertyToH[property], getEnumValueIndex(_Orientation, align.orientation));

	propertyChanged(property);
	valueChanged(property, align);
	emitValueChanged(property, QVariant::fromValue(align));
}

void ROrientationPropertyManager::setValue(QtProperty *property, const QVariant &value)
{
	if (value.canConvert<ROrientation>())
	{
		ROrientation align = value.value<ROrientation>();
		setValue(property, align);
	}
}

bool ROrientationPropertyManager::isPropertyTypeSupported(int propertyType)
{
	if (propertyType == qMetaTypeId<ROrientation>())
		return true;
	return false;
}

void ROrientationPropertyManager::initializeProperty(QtProperty *property)
{
	ROrientation align;
	_valueMap[property] = align;

	QStringList hStr;
	int hCount = _Orientation.keyCount();
	for (int i = 0; i < hCount; i++)
		hStr.append(QString::fromLatin1(_Orientation.key(i)));


	QtProperty *hPro = _enumPropertyManager->addProperty(tr("Setting"));
	_enumPropertyManager->setEnumNames(hPro, hStr);
	_propertyToH[property] = hPro;
	_HToProperty[hPro] = property;
	property->addSubProperty(hPro);


}

void ROrientationPropertyManager::uninitializeProperty(QtProperty *property)
{
	QtProperty *hPro = _propertyToH[property];
	if (hPro)
	{
		_HToProperty.remove(hPro);
		delete hPro;
	}
	_propertyToH.remove(property);

}

void ROrientationPropertyManager::slotEnumChanged(QtProperty *property, int value)
{
	if (QtProperty *prop = _HToProperty.value(property, 0))
	{
		ROrientation align = _valueMap[prop];
		align.orientation = (UiWidget::eOrientation)_Orientation.value(value);
		setValue(prop, align);
	}
}

void ROrientationPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
	if (QtProperty *prop = _HToProperty.value(property, 0))
	{
		_propertyToH[prop] = 0;
		_HToProperty.remove(property);
	}
}

void ROrientationPropertyManager::connectSubManagerFactory(QtTreePropertyBrowser *browser)
{
	browser->setFactoryForManager(_enumPropertyManager, new QtEnumEditorFactory);
}

int ROrientationPropertyManager::getEnumValueIndex(const QMetaEnum &metaEnum, int value) const
{
	int count = metaEnum.keyCount();
	for (int i = 0; i < count; i++)
	{
		if (value == metaEnum.value(i))
			return i;
	}
	assert(0);
	return -1;
}



SkinFilePropertyManager::SkinFilePropertyManager( QObject *parent /*= 0*/ )
	: AbstractPropertyManager(parent)
{

}

SkinFilePropertyManager::~SkinFilePropertyManager()
{
	clear();
}

SkinFile SkinFilePropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, SkinFile());
}

void SkinFilePropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	assert(value.canConvert<SkinFile>());
	SkinFile skinFile = value.value<SkinFile>();

	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(skinFile))
		return;

	it.value() = skinFile;

	propertyChanged(property);
	valueChanged(property, skinFile);
	emitValueChanged(property, value);
}

bool SkinFilePropertyManager::isPropertyTypeSupported( int propertyType )
{
	return qMetaTypeId<SkinFile>() == propertyType;
}

void SkinFilePropertyManager::initializeProperty( QtProperty *property )
{
	_valueMap[property] = SkinFile();
}

void SkinFilePropertyManager::uninitializeProperty( QtProperty *property )
{
	_valueMap.remove(property);
}

QString SkinFilePropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.constFind(property);
	if(it == _valueMap.constEnd())
		return QString();

	SkinFile skinFile = it.value();
	return skinFile.getSkinName();
}

//////////////////////////////////////////////////////////////////////////

SkinSetPropertyManager::SkinSetPropertyManager(QObject *parent /*= 0*/)
	: AbstractPropertyManager(parent)
{
}

SkinSetPropertyManager::~SkinSetPropertyManager()
{
	clear();
}

SkinSet SkinSetPropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, SkinSet());
}

void SkinSetPropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	assert(value.canConvert<SkinSet>());
	SkinSet skinSet = value.value<SkinSet>();

	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(skinSet))
		return;

	it.value() = skinSet;

	propertyChanged(property);
	valueChanged(property, skinSet);
	emitValueChanged(property, value);
}

void SkinSetPropertyManager::setValue( QtProperty *property, const SkinSet &skinSet )
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	it.value() = skinSet;
}

bool SkinSetPropertyManager::isPropertyTypeSupported( int propertyType )
{
	return propertyType == qMetaTypeId<SkinSet>();
}

void SkinSetPropertyManager::initializeProperty( QtProperty *property )
{
	_valueMap[property] = SkinSet();
}

void SkinSetPropertyManager::uninitializeProperty( QtProperty *property )
{
	_valueMap.remove(property);
}

QString SkinSetPropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return QString();

	SkinSet skinSet = it.value();
	return skinSet.getString();
}

//////////////////////////////////////////////////////////////////////////
SkinImageExPropertyManager::SkinImageExPropertyManager( QObject *object )
	: AbstractPropertyManager(object)
	, _skinSetPropertyManager(NULL)
{
	_skinSetPropertyManager = new SkinSetPropertyManager(this);
}

SkinImageExPropertyManager::~SkinImageExPropertyManager()
{
	clear();
}

SkinImageEx SkinImageExPropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, SkinImageEx());
}

void SkinImageExPropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	assert(value.canConvert<SkinImageEx>());
	SkinImageEx &skinImage = value.value<SkinImageEx>();

	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(skinImage))
		return;

	it.value() = skinImage;

	propertyChanged(property);
	valueChanged(property, skinImage);
	emitValueChanged(property, value);

	QList<QtProperty*> pros = property->subProperties();
	QListIterator<QtProperty*> proIter(pros);
	while(proIter.hasNext())
	{
		QtProperty *pro = proIter.next();
		static_cast<SkinSetPropertyManager*>(pro->propertyManager())->setValue(pro, it.value().getSkinSet().getQVariant());
	}
}

bool SkinImageExPropertyManager::isPropertyTypeSupported( int propertyType )
{
	return propertyType == qMetaTypeId<SkinImageEx>();
}

void SkinImageExPropertyManager::connectSubManagerFactory( QtTreePropertyBrowser *browser )
{
	browser->setFactoryForManager(_skinSetPropertyManager, new SkinSetFactory);
	connect(_skinSetPropertyManager, SIGNAL(valueChanged(QtProperty *, const SkinSet &)), this, SLOT(valueChanged(QtProperty *, const SkinSet &)));
}

void SkinImageExPropertyManager::initializeProperty( QtProperty *property )
{
	_valueMap[property] = SkinImageEx();

	QtProperty *pro = _skinSetPropertyManager->addProperty(tr("Setting"));
	_propertyToSet[property] = pro;
	_SetToProperty[pro] = property;
	property->addSubProperty(pro);

	_subToProperty[pro] = property;
}

void SkinImageExPropertyManager::uninitializeProperty( QtProperty *property )
{
	QList<QtProperty*> pros = property->subProperties();
	QListIterator<QtProperty*> proIter(pros);
	while(proIter.hasNext())
	{
		QtProperty *pro = proIter.next();
		_subToProperty.remove(pro);
	}

	QtProperty *pro = _propertyToSet[property];
	if(pro)
	{
		_SetToProperty.remove(pro);
		delete pro;
	}
	_propertyToSet.remove(property);

	_valueMap.remove(property);
}

QString SkinImageExPropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return QString();

	SkinImageEx skinImage = it.value();
	return skinImage.getSkinName();
}

void SkinImageExPropertyManager::valueChanged( QtProperty *property, const SkinSet &skinSet )
{
	QMap<QtProperty*, QtProperty*>::Iterator it = _subToProperty.find(property);
	if(it == _subToProperty.constEnd())
		return;

	const PropertyValueMap::Iterator it2 = _valueMap.find(it.value());
	if(it2 == _valueMap.constEnd())
		return;

	SkinImageEx skinImage = it2.value();
	skinImage.setSkinSet(skinSet);
	setValue(it.value(), skinImage.getQVariant());
}

//////////////////////////////////////////////////////////////////////////

SkinImagePropertyManager::SkinImagePropertyManager( QObject *object /*= 0*/ )
	: AbstractPropertyManager(object)
{

}

SkinImagePropertyManager::~SkinImagePropertyManager()
{
	clear();
}

SkinImage SkinImagePropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, SkinImage());
}

void SkinImagePropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	assert(value.canConvert<SkinImage>());
	SkinImage &skinImage = value.value<SkinImage>();

	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(skinImage))
		return;

	it.value() = skinImage;

	propertyChanged(property);
	valueChanged(property, skinImage);
	emitValueChanged(property, value);
}

bool SkinImagePropertyManager::isPropertyTypeSupported( int propertyType )
{
	return propertyType == qMetaTypeId<SkinImage>();
}

void SkinImagePropertyManager::initializeProperty( QtProperty *property )
{
	_valueMap[property] = SkinImage();
}

void SkinImagePropertyManager::uninitializeProperty( QtProperty *property )
{
	_valueMap.remove(property);
}

QString SkinImagePropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return QString();

	SkinImage skinImage = it.value();
	return skinImage.getSkinName();
}


//////////////////////////////////////////////////////////////////////////

QVector<FontFile> FontType::_fontList;

FontFilePropertyManager::FontFilePropertyManager( QObject *parent /*= 0*/ )
	: AbstractPropertyManager(parent)
{

}

FontFilePropertyManager::~FontFilePropertyManager()
{
	clear();
}

FontFile FontFilePropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, FontFile());
}

void FontFilePropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	assert(value.canConvert<FontFile>());
	FontFile fontFile = value.value<FontFile>();

	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(fontFile))
		return;

	it.value() = fontFile;

	propertyChanged(property);
	valueChanged(property, fontFile);
	emitValueChanged(property, value);
}

bool FontFilePropertyManager::isPropertyTypeSupported( int propertyType )
{
	return propertyType == qMetaTypeId<FontFile>();
}

void FontFilePropertyManager::initializeProperty( QtProperty *property )
{
	_valueMap[property] = FontFile();
}

void FontFilePropertyManager::uninitializeProperty( QtProperty *property )
{
	_valueMap.remove(property);
}

QString FontFilePropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return QString();

	FontFile fontFile = it.value();
	return fontFile.getFontName();
}

//////////////////////////////////////////////////////////////////////////
FontTypePropertyManager::FontTypePropertyManager( QObject *parent /*= 0*/ )
	: AbstractPropertyManager(parent)
{
	_fontFilePropertyManager = new FontFilePropertyManager(this);
	_intPropertyManager = new QtIntPropertyManager(this);
	_outlinePropertyManager = new QtBoolPropertyManager(this);
	_boldPropertyManager = new QtBoolPropertyManager(this);
	_shadowPropertyManager = new QtBoolPropertyManager(this);
	_intlyPropertyManager = new QtBoolPropertyManager(this);

	connect(_fontFilePropertyManager, SIGNAL(valueChanged(QtProperty*, const FontFile &)),
		this, SLOT(slotFontFileChanged(QtProperty*,  const FontFile &)));
	connect(_fontFilePropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));
	connect(_intPropertyManager,SIGNAL(valueChanged(QtProperty*, int)),
		this, SLOT(slotFontSizeChanged(QtProperty*, int)));
	connect(_intPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));


	connect(_outlinePropertyManager, SIGNAL(valueChanged(QtProperty*, bool)),
		this, SLOT(slotFontOutlineChanged(QtProperty*, bool)));
	connect(_boldPropertyManager, SIGNAL(valueChanged(QtProperty*, bool)),
		this, SLOT(slotFontBoldChanged(QtProperty*, bool)));
	connect(_shadowPropertyManager, SIGNAL(valueChanged(QtProperty*, bool)),
		this, SLOT(slotFontShadowChanged(QtProperty*, bool)));
	connect(_intlyPropertyManager, SIGNAL(valueChanged(QtProperty*, bool)),
		this, SLOT(slotFontIntluChanged(QtProperty*, bool)));
}

FontTypePropertyManager::~FontTypePropertyManager()
{
	clear();
}

FontType FontTypePropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, FontType());
}

QString FontTypePropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return QString();

	FontType ft = it.value();
	return ft.valueText();
}

void FontTypePropertyManager::setValue( QtProperty *property, const FontType &fontType )
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(fontType))
		return;

	it.value() = fontType;
	_fontFilePropertyManager->setValue(_propertyToFile[property], fontType.getFontFile().getQVariant());
	_intPropertyManager->setValue(_propertyToSize[property], fontType.getFontSize());
	_outlinePropertyManager->setValue(_propertyTooutline[property], fontType.getFontOutline());
	_boldPropertyManager->setValue(_propertyTobold[property], fontType.getFontBold());
	_shadowPropertyManager->setValue(_propertyToshadow[property], fontType.getFontShadow());
	_intlyPropertyManager->setValue(_propertyTointly[property], fontType.getFontItalics());

	propertyChanged(property);
	valueChanged(property, fontType);
	emitValueChanged(property, QVariant::fromValue(fontType));
}

void FontTypePropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	if(value.canConvert<FontType>())
	{
		FontType fontType = value.value<FontType>();
		setValue(property, fontType);
	}
}

bool FontTypePropertyManager::isPropertyTypeSupported( int propertyType )
{
	if(propertyType == qMetaTypeId<FontType>())
		return true;
	return false;
}

void FontTypePropertyManager::initializeProperty( QtProperty *property )
{
	FontType fontType;
	_valueMap[property] = fontType;

	QtProperty *filePro = _fontFilePropertyManager->addProperty(tr("Src"));
	_propertyToFile[property] = filePro;
	_FileToProperty[filePro] = property;
	property->addSubProperty(filePro);

	QtProperty *sizePro = _intPropertyManager->addProperty(tr("Size"));
	_propertyToSize[property] = sizePro;
	_SizeToProperty[sizePro] = property;
	property->addSubProperty(sizePro);
	_intPropertyManager->setMinimum(sizePro, 1);


	QtProperty *outlinePro = _outlinePropertyManager->addProperty(tr("Outline"));
	_propertyTooutline[property] = outlinePro;
	_outlingToProperty[outlinePro] = property;
	property->addSubProperty(outlinePro);

	QtProperty *boldPro = _boldPropertyManager->addProperty(tr("Bold"));
	_propertyTobold[property] = boldPro;
	_boldToProperty[boldPro] = property;
	property->addSubProperty(boldPro);



	QtProperty *showdowPro = _shadowPropertyManager->addProperty(tr("Shadow"));
	_propertyToshadow[property] = showdowPro;
	_shadowToProperty[showdowPro] = property;
	property->addSubProperty(showdowPro);


	QtProperty *ItalicsPro = _intlyPropertyManager->addProperty(tr("Italics"));
	_propertyTointly[property] = ItalicsPro;
	_intlyToProperty[ItalicsPro] = property;
	property->addSubProperty(ItalicsPro);
}

void FontTypePropertyManager::uninitializeProperty( QtProperty *property )
{
	QtProperty *filePro = _propertyToFile[property];
	if(filePro)
	{
		_FileToProperty.remove(filePro);
		delete filePro;
	}
	_propertyToFile.remove(property);

	QtProperty *sizePro = _propertyToSize[property];
	if(sizePro)
	{
		_SizeToProperty.remove(sizePro);
		delete sizePro;
	}
	_propertyToSize.remove(property);
}

void FontTypePropertyManager::slotFontFileChanged( QtProperty *property, const FontFile &value )
{
	QtProperty *prop = _FileToProperty.value(property, 0);
	assert(prop != 0);
	FontType fontType = _valueMap[prop];
	fontType.setFontFile(value);
	setValue(prop, fontType);
}

void FontTypePropertyManager::slotFontSizeChanged( QtProperty *property, int size )
{
	QtProperty *prop = _SizeToProperty.value(property, 0);
	assert(prop != 0);
	FontType fontType = _valueMap[prop];
	fontType.setFontSize(size);
	setValue(prop, fontType);
}

void FontTypePropertyManager::slotFontOutlineChanged(QtProperty *property, bool outline)
{
	QtProperty *prop = _outlingToProperty.value(property, 0);
	assert(prop != 0);
	FontType fontType = _valueMap[prop];
	fontType.setFontOutline(outline);
	setValue(prop, fontType);
}


void FontTypePropertyManager::slotFontBoldChanged(QtProperty *property, bool bold)
{
	QtProperty *prop = _boldToProperty.value(property, 0);
	assert(prop != 0);
	FontType fontType = _valueMap[prop];
	fontType.setFontBold(bold);
	setValue(prop, fontType);
}
void FontTypePropertyManager::slotFontShadowChanged(QtProperty *property, bool shadow)
{
	QtProperty *prop = _shadowToProperty.value(property, 0);
	assert(prop != 0);
	FontType fontType = _valueMap[prop];
	fontType.setFontShadow(shadow);
	setValue(prop, fontType);
}
void FontTypePropertyManager::slotFontIntluChanged(QtProperty *property, bool intlu)
{
	QtProperty *prop = _intlyToProperty.value(property, 0);
	assert(prop != 0);
	FontType fontType = _valueMap[prop];
	fontType.setFontItalics(intlu);
	setValue(prop, fontType);
}

void FontTypePropertyManager::slotPropertyDestroyed( QtProperty *property )
{
	if(QtProperty *prop = _FileToProperty.value(property, 0))
	{
		_propertyToFile[prop] = 0;
		_FileToProperty.remove(property);
	}
	else if(QtProperty *prop = _SizeToProperty.value(property, 0))
	{
		_propertyToSize[prop] = 0;
		_SizeToProperty.remove(property);
	}
}

void FontTypePropertyManager::connectSubManagerFactory( QtTreePropertyBrowser *browser )
{
	browser->setFactoryForManager(_fontFilePropertyManager, new FontFileEditorFactory);
	browser->setFactoryForManager(_intPropertyManager, new QtSpinBoxFactory);
	browser->setFactoryForManager(_outlinePropertyManager, new QtCheckBoxFactory);
	browser->setFactoryForManager(_boldPropertyManager, new QtCheckBoxFactory);
	browser->setFactoryForManager(_shadowPropertyManager, new QtCheckBoxFactory);
	browser->setFactoryForManager(_intlyPropertyManager, new QtCheckBoxFactory);
}

void FontTypePropertyManager::setValue2(QtProperty *property, const FontType &fontType)
{
	setValue(property, fontType);
}

//////////////////////////////////////////////////////////////////////////

NumericSetPropertyManager::NumericSetPropertyManager( QObject *parent /*= 0*/ )
	: AbstractPropertyManager(parent)
{
	_boolPropertyManager = new QtBoolPropertyManager(this);
	_intPropertyManager = new QtIntPropertyManager(this);

	connect(_boolPropertyManager, SIGNAL(valueChanged(QtProperty*, bool)),
		this, SLOT(slotBoolValueChanged(QtProperty*, bool)));
	connect(_boolPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));

	connect(_intPropertyManager, SIGNAL(valueChanged(QtProperty*, int)),
		this, SLOT(slotIntValueChanged(QtProperty*, int)));
	connect(_intPropertyManager, SIGNAL(propertyDestroyed(QtProperty*)),
		this, SLOT(slotPropertyDestroyed(QtProperty*)));
}

NumericSetPropertyManager::~NumericSetPropertyManager()
{
	clear();
}

NumericSet NumericSetPropertyManager::value( const QtProperty *property ) const
{
	return _valueMap.value(property, NumericSet());
}

void NumericSetPropertyManager::setValue( QtProperty *property, const QVariant &value )
{
	if(value.canConvert<NumericSet>())
	{
		NumericSet numericSet = value.value<NumericSet>();
		setValue(property, numericSet);
	}
}

void NumericSetPropertyManager::setValue( QtProperty *property, const NumericSet &numSet )
{
	const PropertyValueMap::Iterator it = _valueMap.find(property);
	if(it == _valueMap.constEnd())
		return;

	if(it.value().equals(numSet))
		return;

	it.value() = numSet;
	_boolPropertyManager->setValue(_propertyToEnable[property], numSet.getEnable());
	_intPropertyManager->setValue(_propertyToMax[property], numSet.getMax());
	_intPropertyManager->setValue(_propertyToMin[property], numSet.getMin());

	propertyChanged(property);
	valueChanged(property, numSet);
	emitValueChanged(property, QVariant::fromValue(numSet));
}

bool NumericSetPropertyManager::isPropertyTypeSupported( int propertyType )
{
	return (propertyType == qMetaTypeId<NumericSet>());
}

void NumericSetPropertyManager::connectSubManagerFactory( QtTreePropertyBrowser *browser )
{
	browser->setFactoryForManager(_boolPropertyManager, new QtCheckBoxFactory);
	browser->setFactoryForManager(_intPropertyManager, new QtSpinBoxFactory);
}


void NumericSetPropertyManager::setValue2(QtProperty *property, const NumericSet &fontType)
{
	setValue(property, fontType);
}

void NumericSetPropertyManager::slotPropertyDestroyed( QtProperty *property )
{
	if(QtProperty *prop = _enableToProperty.value(property, 0))
	{
		_propertyToEnable[prop] = 0;
		_enableToProperty.remove(property);
	}
	else if(QtProperty *prop = _maxToProperty.value(property, 0))
	{
		_propertyToMax[prop] = 0;
		_maxToProperty.remove(property);
	}
	else if(QtProperty *prop = _minToProperty.value(property, 0))
	{
		_propertyToMin[prop] = 0;
		_minToProperty.remove(property);
	}
}

void NumericSetPropertyManager::initializeProperty( QtProperty *property )
{
	NumericSet numSet;
	_valueMap[property] = numSet;

	QtProperty *enablePro = _boolPropertyManager->addProperty(tr("Enable"));
	_boolPropertyManager->setValue(enablePro, numSet.getEnable());
	_propertyToEnable[property] = enablePro;
	_enableToProperty[enablePro] = property;
	property->addSubProperty(enablePro);

	QtProperty *minPro = _intPropertyManager->addProperty(tr("MinValue"));
	_intPropertyManager->setValue(minPro, numSet.getMin());
	minPro->setEnabled(numSet.getEnable());
	_propertyToMin[property] = minPro;
	_minToProperty[minPro] = property;
	property->addSubProperty(minPro);

	QtProperty *maxPro = _intPropertyManager->addProperty(tr("MaxValue"));
	_intPropertyManager->setValue(maxPro, numSet.getMax());
	maxPro->setEnabled(numSet.getEnable());
	_propertyToMax[property] = maxPro;
	_maxToProperty[maxPro] = property;
	property->addSubProperty(maxPro);
}

void NumericSetPropertyManager::uninitializeProperty( QtProperty *property )
{
	QtProperty *enablePro = _propertyToEnable[property];
	if(enablePro)
	{
		_enableToProperty.remove(enablePro);
		delete enablePro;
	}
	_propertyToEnable.remove(property);

	QtProperty *minPro = _propertyToMin[property];
	if(minPro)
	{
		_minToProperty.remove(minPro);
		delete minPro;
	}
	_propertyToMin.remove(property);

	QtProperty *maxPro = _propertyToMax[property];
	if(maxPro)
	{
		_maxToProperty.remove(maxPro);
		delete maxPro;
	}
	_propertyToMax.remove(property);
}

QString NumericSetPropertyManager::valueText( const QtProperty *property ) const
{
	const PropertyValueMap::ConstIterator it = _valueMap.constFind(property);
	if(it == _valueMap.constEnd())
		return QString();

	NumericSet numSet = it.value();
	return numSet.toString();
}

void NumericSetPropertyManager::slotIntValueChanged( QtProperty *property, int val )
{
	if(QtProperty *prop = _minToProperty.value(property, 0))
	{
		NumericSet numSet = _valueMap[prop];
		numSet.setMin(val);
		setValue(prop, numSet);
	}
	else if(QtProperty *prop = _maxToProperty.value(property, 0))
	{
		NumericSet numSet = _valueMap[prop];
		numSet.setMax(val);
		setValue(prop, numSet);
	}
}

void NumericSetPropertyManager::slotBoolValueChanged( QtProperty *property, bool val )
{
	if(QtProperty *prop = _enableToProperty.value(property, 0))
	{
		NumericSet numSet = _valueMap[prop];
		numSet.setEnable(val);
		setValue(prop, numSet);

		QtProperty *minPro = _propertyToMin[prop];
		QtProperty *maxPro = _propertyToMax[prop];
		if(minPro != NULL)
			minPro->setEnabled(val);
		if(maxPro != NULL)
			maxPro->setEnabled(val);
	}
}
