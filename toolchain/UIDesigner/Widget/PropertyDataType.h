#ifndef PROPERTYDATATYPE_H
#define PROPERTYDATATYPE_H

#include <QtCore/QFileInfo>
#include <QtCore/QPair>
#include <QtCore/QDir>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtGui/QFontDatabase>
#include <QtGui/QImage>
#include <QtXml/QDomElement>
#include <assert.h>
#include "Config.h"
class UserPropertyDataType
{
public:
	UserPropertyDataType() 
		: _index(-1) {}

	UserPropertyDataType(const UserPropertyDataType& updt)
	{
		_index = updt._index;
	}

	virtual bool isAsEnumType() { return false; }
	virtual QStringList getEnumNames() { return QStringList(); }
	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var) = 0;
	virtual QVariant getQVariant() const = 0;
	void setIndex(int index) { _index = index; }
	int getIndex() const { return _index; } 
protected:
	int _index; // 仅在设置属性的时候使用
};

class SkinFile : public UserPropertyDataType
{
public:
	SkinFile()
	{
	}

	SkinFile(const QString &pathName)
	{
		setPathFileName(pathName);
	}

	SkinFile(const SkinFile &skinFile)
		: UserPropertyDataType(skinFile)
	{
		setPathFileName(skinFile._pathName);
	}

	bool equals(const SkinFile &skinFile)
	{
		return _pathName == skinFile._pathName;
	}

	QString getFileName() const
	{
		return _pathName;
	}

	QString getSkinName() const
	{
		return _skinName;
	}

	void setPathFileName(const QString &pathFileName)
	{
		if(_pathName != pathFileName)
		{
			_pathName = pathFileName;
			initNames();
		}
	}

	QStringList getNames()
	{
		return _names;
	}

	QString getName(int index)
	{
		return _names[index];
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<SkinFile>())
			return new SkinFile(var.value<SkinFile>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<SkinFile>(*this);
	}

	QImage getImage(const QString &name) const;

private:
	void initNames();

private:
	QStringList _names;
	QString _pathName;
	QString _imageFileName;
	QString _skinName;
	QImage* _image;
	QMap<QString, QRect> _imageArea;
};

Q_DECLARE_METATYPE(SkinFile)


//////////////////////////////////////////////////////////////////////////
class SkinSet : public UserPropertyDataType
{
public:
	SkinSet()
		: _size(1, 1)
	{}
	~SkinSet() {}

	SkinSet(const QString skinName)
		: _size(1, 1)
		, _skinName(skinName)
	{
		//c_ui_manager::get_singleton_ptr()->
	}

	SkinSet(const SkinSet &skinSet)
		: _size(skinSet._size)
		, _skinName(skinSet._skinName)
		, _pt(skinSet._pt)
		, _rect(skinSet._rect)
	{

	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<SkinSet>())
			return new SkinSet(var.value<SkinSet>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<SkinSet>(*this);
	}

	bool equals(const SkinSet &skinSet) const
	{
		if(_pt == skinSet._pt && _size == skinSet._size &&
			_rect == skinSet._rect && _skinName == skinSet._skinName )
			return true;
		return false;
	}

	QString getString() const
	{
		return QString("(%1, %2), [%3, %4]").arg((int)_pt.x()).arg((int)_pt.y()).arg((int)_size.width()).arg((int)_size.height());
	}

	void setSkinName(const QString skinName)
	{
		_skinName = skinName;
	}

	QString getSkinName() const
	{
		return _skinName;
	}

	void setPoint(const QPointF &pt)
	{
		_pt = pt;
	}

	void setSize(const QSizeF &size)
	{
		_size = size;
	}

	void setRect(const QRectF &rect)
	{
		_rect = rect;
	}

	const QPointF &getCenterPt() const 
	{
		return _pt;
	}

	const QSizeF &getCenterSize() const
	{
		return _size;
	}

	const QRectF &getRect() const
	{
		return _rect;
	}

private:
	QPointF _pt;
	QSizeF _size;
	QRectF _rect;
	QString _skinName;
};
Q_DECLARE_METATYPE(SkinSet)

//////////////////////////////////////////////////////////////////////////
class SkinImageEx : public UserPropertyDataType
{
public:
	SkinImageEx() { }

	SkinImageEx(const SkinImageEx &skinImage)
		: UserPropertyDataType(skinImage)
	{
		setSkinName(skinImage.getSkinName());
		setSkinSet(skinImage.getSkinSet());
	}

	SkinImageEx(const QString &name)
	{
		setSkinName(name);
	}

	QString getSkinName() const
	{
		return _skinSet.getSkinName();
	}
	 
	bool equals(const SkinImageEx &skinImage)
	{
		return (getSkinName() == skinImage.getSkinName() && _skinSet.equals(skinImage.getSkinSet()));
	}

	void setData(const SkinImageEx &skinImage)
	{
		//_skinSet.setSkinName(skinImage.getSkinName());
		setSkinSet(skinImage.getSkinSet());
	}

	void setSkinName(const QString &skinName)
	{
		_skinSet.setSkinName(skinName);
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<SkinImageEx>())
			return new SkinImageEx(var.value<SkinImageEx>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<SkinImageEx>(*this);
	}

	const SkinSet &getSkinSet() const { return _skinSet; }

	void setSkinSet(const SkinSet &skinSet) { _skinSet = skinSet; }
private:
	SkinSet _skinSet;
};
Q_DECLARE_METATYPE(SkinImageEx)

//////////////////////////////////////////////////////////////////////////

class SkinImage : public UserPropertyDataType
{
public:
	SkinImage() { }

	SkinImage(const SkinImage &skinImage)
		: UserPropertyDataType(skinImage)
		, _skinName(skinImage._skinName)
	{
	}

	SkinImage(const QString &name)
		: _skinName(name)
	{
	}

	const QString &getSkinName() const
	{
		return _skinName;
	}

	void setSkinName(const QString &skinName)
	{
		_skinName = skinName;
	}

	bool equals(const SkinImage &skinImage)
	{
		return (_skinName == skinImage._skinName);
	}

	void setData(const SkinImage &skinImage)
	{
		_skinName = skinImage._skinName;
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<SkinImage>())
			return new SkinImage(var.value<SkinImage>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<SkinImage>(*this);
	}
private:
	QString _skinName;
};
Q_DECLARE_METATYPE(SkinImage)

//////////////////////////////////////////////////////////////////////////
class FontFile : public UserPropertyDataType
{
public:
	FontFile() 
	{
	}

	FontFile(const FontFile &fontFile)
		: UserPropertyDataType(fontFile)
	{
		_fileName = fontFile._fileName;
		_fontName = fontFile._fontName;
	}

	FontFile(const QString &fontName, const QString &fileName)
	{
		_fontName = fontName;
		_fileName = fileName;
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<FontFile>())
			return new FontFile(var.value<FontFile>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<FontFile>(*this);
	}

	bool equals(const FontFile &fontFile) const
	{
		return _fontName == fontFile._fontName;
	}

	QString getFileName(bool widthExt = false) const
	{
		if(widthExt)
			return _fileName;

		QFileInfo file(_fileName);
		return file.baseName();
	}

	void setFileName(const QString &fileName)
	{
		_fileName = fileName;
	}

	QString getFontName() const 
	{
		return _fontName;
	}

	void setFontName(const QString &fontName)
	{
		_fontName = fontName;
	}

	
private:
	QString _fileName;
	QString _fontName;
};
Q_DECLARE_METATYPE(FontFile)

//////////////////////////////////////////////////////////////////////////
class FontType : public UserPropertyDataType
{
public:
	FontType() 
		: _fontSize(14)
		,_outline(false),
		_shadow(false),
		_bold(false),
		_italics(false)
	{ }

	FontType(const FontType& fontType)
		: UserPropertyDataType(fontType)
		, _fontFile(fontType._fontFile)
		, _fontSize(fontType._fontSize)
		, _outline(fontType._outline)
		, _bold(fontType._bold)
		, _shadow(fontType._shadow)
		, _italics(fontType._italics)
	{ }

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<FontType>())
			return new FontType(var.value<FontType>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<FontType>(*this);
	}

	bool equals(const FontType &fontType) const
	{
		return (_fontSize ==  fontType._fontSize&&
			_fontFile.equals(fontType._fontFile) &&
			_outline == fontType._outline &&
			_bold == fontType._bold &&
			_shadow == fontType._shadow &&
			_italics == fontType._italics);
	}

	QString valueText() const
	{
		if(_fontFile.getFontName().isEmpty())
			return QString();
		return QString("[%1, %2]").arg(_fontFile.getFontName()).arg(_fontSize);
	}

	FontFile getFontFile() const
	{
		return _fontFile;
	}

	void setFontFile(const FontFile &fontFile) 
	{
		_fontFile = fontFile;
	}

	int getFontSize() const
	{
		return _fontSize;
	}

	void setFontSize(int size)
	{
		_fontSize = size;
	}

	bool getFontOutline() const
	{
		return _outline;
	}

	void setFontOutline(bool outline)
	{
		_outline = outline;
	}

	bool getFontBold() const
	{
		return _bold;
	}

	void setFontBold(bool bold)
	{
		_bold = bold;
	}

	bool getFontShadow() const
	{
		return _shadow;
	}

	void setFontShadow(bool shadow)
	{
		_shadow = shadow;
	}

	bool getFontItalics() const
	{
		return _italics;
	}

	void setFontItalics(bool italics)
	{
		_italics = italics;
	}

	unsigned int getFontID();


	static QStringList getFontsNameList();
	static void initFontList();
	static FontFile addFont(const QString &fileName);
	static QString getFontName(const QString &fontFileName);
	static QString createFntFile(const QString &fontName, const QString &fontFileName);
	static void modifyFntFile(const QString &fontName, const QString &fontFileName, const QList<int> &heights);

	static int getFontFileIndex(const FontFile &fontFile)
	{
		QStringList nameList = getFontsNameList();
		QString fontName = fontFile.getFontName();
		int index = nameList.indexOf(fontName);
		return index;
	}

	static FontFile getFontFile(int index)
	{
		if(index < 0)
			return FontFile();
		else
			return _fontList[index];
	}

	

	static int fontFileCount()
	{
		return _fontList.count();
	}
private:
	FontFile _fontFile;
	int _fontSize;
	bool _outline;
	bool _bold;
	bool _shadow;
	bool _italics;

	static QVector<FontFile> _fontList;
};
Q_DECLARE_METATYPE(FontType)

//////////////////////////////////////////////////////////////////////////
class TableItemMap
{
public:
	TableItemMap()
	{ }

	TableItemMap(const TableItemMap &m)
		: _items(m._items)
		, _columns(m._columns)
	{ }

	TableItemMap(const QStringList &items, const QStringList &columns)
		: _items(items)
		, _columns(columns)
	{ }

	const QStringList &getItems() const
	{
		return _items;
	}

	const QStringList &getColumns() const
	{
		return _columns;
	}

private:
	QStringList _items;
	QStringList _columns;
};
Q_DECLARE_METATYPE(TableItemMap)

//////////////////////////////////////////////////////////////////////////
class CartoonBoxFrames
{
public:
	CartoonBoxFrames()
	{ }

	CartoonBoxFrames(const CartoonBoxFrames &cartoonBoxFrames)
		: _skinFile(cartoonBoxFrames._skinFile)
		, _frames(cartoonBoxFrames._frames)
	{ }

	CartoonBoxFrames(const SkinFile &skinFile, const QStringList &frames)
		: _skinFile(skinFile)
		, _frames(frames)
	{ }

	const SkinFile &getSkinFile() const { return _skinFile; }
	const QStringList &getFrames() const { return _frames; }

	bool equals(const CartoonBoxFrames &cartoonBoxFrames)
	{
		return (_skinFile.equals(cartoonBoxFrames.getSkinFile()) &&
			_frames == cartoonBoxFrames.getFrames());
	}

private:
	SkinFile _skinFile;
	QStringList _frames;
};
Q_DECLARE_METATYPE(CartoonBoxFrames)

//////////////////////////////////////////////////////////////////////////

class NumericSet : public UserPropertyDataType
{
public:
	NumericSet()
		: _enable(false)
		, _min(INT_MIN)
		, _max(INT_MAX)
	{

	}

	NumericSet(const NumericSet &ns)
		: _enable(ns._enable)
		, _min(ns._min)
		, _max(ns._max)
	{

	}

	~NumericSet()
	{

	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<NumericSet>())
			return new NumericSet(var.value<NumericSet>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<NumericSet>(*this);
	}

	bool equals(const NumericSet &numSet)
	{
		return (numSet._enable == _enable && numSet._max == _max && numSet._min == _min);
	}

	QString toString() const
	{
		QString text;
		if(_enable)
		{
			text += "Enable";
			text += QString(" [%1~%2]").arg(_min).arg(_max);
		}
		else
		{
			text += "Disable";
		}

		return text;
	}

	bool getEnable() const { return _enable; }
	void setEnable(bool val) { _enable = val; }
	int getMin() const { return _min; }
	void setMin(int val) { _min = val; }
	int getMax() const { return _max; }
	void setMax(int val) { _max = val; }

private:
	bool _enable;
	int _min;
	int _max;
};
Q_DECLARE_METATYPE(NumericSet);

#endif //PROPERTYDATATYPE_H