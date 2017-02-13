#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include "macro.h"
#include "QtAbstractPropertyManager"
#include "QtVariantPropertyManager"
#include "UiWidget.h"
#include "qtpropertymanager.h"

#include <QtCore/QMap>
#include <QtCore/QMetaEnum>

//////////////////////////////////////////////////////////////////////////

class QtEnumPropertyManager;
class QtTreePropertyBrowser;

class AbstractPropertyManager :public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	AbstractPropertyManager(QObject *parent = 0)
		: QtAbstractPropertyManager(parent) {}
	virtual void setValue(QtProperty *property, const QVariant &value) = 0;
	virtual bool isPropertyTypeSupported ( int propertyType ) = 0;
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser) = 0;
signals:
	void valueChanged(QtProperty *property, const QVariant &value);
protected:
	void emitValueChanged(QtProperty *property, const QVariant &value)
	{
		valueChanged(property, value);
	}
};

class TextAlignPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	TextAlignPropertyManager(QObject *parent = 0);
	virtual ~TextAlignPropertyManager();

	TextAlign value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);

signals:
	void valueChanged(QtProperty *property, const TextAlign &align);
protected slots:
	void setValue(QtProperty *property, const TextAlign &align);
	void slotEnumChanged(QtProperty *property, int value);
	void slotPropertyDestroyed(QtProperty *property);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	int getEnumValueIndex(const QMetaEnum &metaEnum, int value) const;

private:
	typedef QMap<const QtProperty*, TextAlign> PropertyValueMap;
	PropertyValueMap _valueMap;
	QtEnumPropertyManager *_enumPropertyManager;

	QMetaEnum _HAlignEnum;
	QMetaEnum _VAlignEnum;

	QMap<const QtProperty*, QtProperty*> _propertyToH;
	QMap<const QtProperty*, QtProperty*> _propertyToV;

	QMap<const QtProperty*, QtProperty*> _HToProperty;
	QMap<const QtProperty*, QtProperty*> _VToProperty;
};
////////////////

class PtAlignPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	PtAlignPropertyManager(QObject *parent = 0);
	virtual ~PtAlignPropertyManager();

	PtAlign value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported(int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);

signals:
	void valueChanged(QtProperty *property, const PtAlign &align);
	protected slots:
	void setValue(QtProperty *property, const PtAlign &align);
	void slotEnumChanged(QtProperty *property, int value);
	void slotPropertyDestroyed(QtProperty *property);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	int getEnumValueIndex(const QMetaEnum &metaEnum, int value) const;

private:
	typedef QMap<const QtProperty*, PtAlign> PropertyValueMap;
	PropertyValueMap _valueMap;
	QtEnumPropertyManager *_enumPropertyManager;

	QMetaEnum _HAlignEnum;
	QMetaEnum _VAlignEnum;

	QMap<const QtProperty*, QtProperty*> _propertyToH;
	QMap<const QtProperty*, QtProperty*> _propertyToV;

	QMap<const QtProperty*, QtProperty*> _HToProperty;
	QMap<const QtProperty*, QtProperty*> _VToProperty;
};


///
class RPolicyPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	RPolicyPropertyManager(QObject *parent = 0);
	virtual ~RPolicyPropertyManager();

	RPolicy value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported(int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);

signals:
	void valueChanged(QtProperty *property, const RPolicy &align);
	protected slots:
	void setValue(QtProperty *property, const RPolicy &align);
	void slotEnumChanged(QtProperty *property, int value);
	void slotPropertyDestroyed(QtProperty *property);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	int getEnumValueIndex(const QMetaEnum &metaEnum, int value) const;

private:
	typedef QMap<const QtProperty*, RPolicy> PropertyValueMap;
	PropertyValueMap _valueMap;
	QtEnumPropertyManager *_enumPropertyManager;

	QMetaEnum _dpolicy;

	QMap<const QtProperty*, QtProperty*> _propertyToH;

	QMap<const QtProperty*, QtProperty*> _HToProperty;
};


class ROrientationPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	ROrientationPropertyManager(QObject *parent = 0);
	virtual ~ROrientationPropertyManager();

	ROrientation value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported(int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);

signals:
	void valueChanged(QtProperty *property, const ROrientation &align);
	protected slots:
	void setValue(QtProperty *property, const ROrientation &align);
	void slotEnumChanged(QtProperty *property, int value);
	void slotPropertyDestroyed(QtProperty *property);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	int getEnumValueIndex(const QMetaEnum &metaEnum, int value) const;

private:
	typedef QMap<const QtProperty*, ROrientation> PropertyValueMap;
	PropertyValueMap _valueMap;
	QtEnumPropertyManager *_enumPropertyManager;

	QMetaEnum _Orientation;

	QMap<const QtProperty*, QtProperty*> _propertyToH;

	QMap<const QtProperty*, QtProperty*> _HToProperty;
};



//////////////////////////////////////////////////////////////////////////
class SkinFilePropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	SkinFilePropertyManager(QObject *parent = 0);
	virtual ~SkinFilePropertyManager();

	SkinFile value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser) {};

signals:
	void valueChanged(QtProperty *property, const SkinFile &skinFile);
protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	typedef QMap<const QtProperty*, SkinFile> PropertyValueMap;
	PropertyValueMap _valueMap;
};

//////////////////////////////////////////////////////////////////////////
class SkinSetPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	SkinSetPropertyManager(QObject *object);
	~SkinSetPropertyManager();

	SkinSet value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser) {};

	void setValue(QtProperty *property, const SkinSet &skinSet);

signals:
	void valueChanged(QtProperty *property, const SkinSet &skinSet);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	typedef QMap<const QtProperty*, SkinSet> PropertyValueMap;
	PropertyValueMap _valueMap;
};

//////////////////////////////////////////////////////////////////////////
class SkinImageExPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	SkinImageExPropertyManager(QObject *object = 0);
	virtual ~SkinImageExPropertyManager();

	SkinImageEx value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);

signals:
	void valueChanged(QtProperty *property, const SkinImageEx &skinImage);
protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

protected slots:
	void valueChanged(QtProperty *property, const SkinSet &skinSet);

private:
	typedef QMap<const QtProperty*, SkinImageEx> PropertyValueMap;
	PropertyValueMap _valueMap;

	SkinSetPropertyManager *_skinSetPropertyManager;

	QMap<const QtProperty*, QtProperty*> _propertyToSet;
	QMap<const QtProperty*, QtProperty*> _SetToProperty;
	QMap<QtProperty*, QtProperty*> _subToProperty;
};

//////////////////////////////////////////////////////////////////////////
class SkinImagePropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	SkinImagePropertyManager(QObject *object = 0);
	virtual ~SkinImagePropertyManager();

	SkinImage value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser) {}
signals:
	void valueChanged(QtProperty *property, const SkinImage &skinImage);
protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	typedef QMap<const QtProperty*, SkinImage> PropertyValueMap;
	PropertyValueMap _valueMap;
};


//////////////////////////////////////////////////////////////////////////
class FontFilePropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	FontFilePropertyManager(QObject *parent = 0);
	virtual ~FontFilePropertyManager();

	FontFile value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser) { };

signals:
	void valueChanged(QtProperty *property, const FontFile &skinFile);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	typedef QMap<const QtProperty*, FontFile> PropertyValueMap;
	PropertyValueMap _valueMap;
};

//////////////////////////////////////////////////////////////////////////

class QtIntPropertyManager;

class FontTypePropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	FontTypePropertyManager(QObject *parent = 0);
	virtual ~FontTypePropertyManager();

	FontType value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);
	void setValue2(QtProperty *property, const FontType &fontType);

signals:
	void valueChanged(QtProperty *property, const FontType &fontType);
protected slots:
	void setValue(QtProperty *property, const FontType &fontType);
	void slotFontFileChanged(QtProperty *property, const FontFile &value);
	void slotFontSizeChanged(QtProperty *property, int size);
	void slotPropertyDestroyed(QtProperty *property);
	void slotFontOutlineChanged(QtProperty *property, bool outline);
	void slotFontBoldChanged(QtProperty *property, bool bold);
	void slotFontShadowChanged(QtProperty *property, bool shadow);
	void slotFontIntluChanged(QtProperty *property, bool intlu);
	

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	typedef QMap<const QtProperty*, FontType> PropertyValueMap;
	PropertyValueMap _valueMap;

	FontFilePropertyManager *_fontFilePropertyManager;
	QtIntPropertyManager *_intPropertyManager;
	QtBoolPropertyManager* _outlinePropertyManager;
	QtBoolPropertyManager* _boldPropertyManager;
	QtBoolPropertyManager* _shadowPropertyManager;
	QtBoolPropertyManager* _intlyPropertyManager;

	QMap<const QtProperty*, QtProperty*> _propertyToFile;
	QMap<const QtProperty*, QtProperty*> _propertyToSize;

	QMap<const QtProperty*, QtProperty*> _propertyTooutline;
	QMap<const QtProperty*, QtProperty*> _propertyTobold;
	QMap<const QtProperty*, QtProperty*> _propertyToshadow;
	QMap<const QtProperty*, QtProperty*> _propertyTointly;

	QMap<const QtProperty*, QtProperty*> _FileToProperty;
	QMap<const QtProperty*, QtProperty*> _SizeToProperty;

	QMap<const QtProperty*, QtProperty*> _outlingToProperty;
	QMap<const QtProperty*, QtProperty*> _boldToProperty;
	QMap<const QtProperty*, QtProperty*> _shadowToProperty;
	QMap<const QtProperty*, QtProperty*> _intlyToProperty;
};


//////////////////////////////////////////////////////////////////////////
class NumericSetPropertyManager : public AbstractPropertyManager
{
	Q_OBJECT
public:
	NumericSetPropertyManager(QObject *parent = 0);
	virtual ~NumericSetPropertyManager();

	NumericSet value(const QtProperty *property) const;
	virtual void setValue(QtProperty *property, const QVariant &value);
	virtual bool isPropertyTypeSupported (int propertyType);
	virtual void connectSubManagerFactory(QtTreePropertyBrowser *browser);
	void setValue2(QtProperty *property, const NumericSet &fontType);

signals:
	void valueChanged(QtProperty *property, const NumericSet &numSet);
protected slots:
	void setValue(QtProperty *property, const NumericSet &numSet);
	void slotPropertyDestroyed(QtProperty *property);
	void slotIntValueChanged(QtProperty *property, int val);
	void slotBoolValueChanged(QtProperty *property, bool val);

protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	virtual QString valueText(const QtProperty *property) const;

private:
	typedef QMap<const QtProperty*, NumericSet> PropertyValueMap;
	PropertyValueMap _valueMap;

	QtBoolPropertyManager *_boolPropertyManager;
	QtIntPropertyManager *_intPropertyManager;

	QMap<const QtProperty*, QtProperty*> _propertyToEnable;
	QMap<const QtProperty*, QtProperty*> _propertyToMin;
	QMap<const QtProperty*, QtProperty*> _propertyToMax;

	QMap<const QtProperty*, QtProperty*> _enableToProperty;
	QMap<const QtProperty*, QtProperty*> _minToProperty;
	QMap<const QtProperty*, QtProperty*> _maxToProperty;
};


#endif //PROPERTYMANAGER_H