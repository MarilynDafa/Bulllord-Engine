#ifndef UIWIDGET_H
#define UIWIDGET_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtGui/QVector2D>
#include <QtGui/QColor>
#include <QtXml/QDomElement>
#include <QXmlStreamReader>

#include "macro.h"
#include "PropertyDataType.h"

#include "Widget.h"

using namespace o2d;

struct TextAlign;
class RPolicy;
class PtAlign;
class UiWidget;

class SizeController
{
public:
	enum POSITION
	{
		INVALID = 0,
		POSITION_LEFT			= 1 << 1,
		POSITION_TOP			= 1 << 2,
		POSITION_RIGHT			= 1 << 3,
		POSITION_BOTTOM		= 1 << 4,
		POSITION_LT = POSITION_LEFT | POSITION_TOP,
		POSITION_LB = POSITION_LEFT | POSITION_BOTTOM,
		POSITION_RT = POSITION_RIGHT | POSITION_TOP,
		POSITION_RB = POSITION_RIGHT | POSITION_BOTTOM
	};

	SizeController();
	SizeController(const SizeController &controller);
	SizeController(const QPoint &point, UiWidget *widget, POSITION pos);
	QRect getRect() const;
	bool moveTo(int x, int y);
	int getCursorShape();
	bool isValid() { return _pos != INVALID; }
	UiWidget *getWidget() { return _widget; }
private:
	void ltMove(int &x, int &y);
	void lbMove(int &x, int &y);
	void rtMove(int &x, int &y);
	void rbMove(int &x, int &y);
	QVector2D calculateSize(int &width, int &height);
private:
	QPoint _point;
	UiWidget *_widget;
	POSITION _pos;
};

class UiWidget : public QObject
{
	Q_OBJECT
	Q_ENUMS(HTextAlignment)
	Q_ENUMS(VTextAlignment)
	Q_ENUMS(Hreference)
	Q_ENUMS(Vreference)
	Q_ENUMS(DrawPolicy)
	Q_ENUMS(eOrientation)
	Q_CLASSINFO("uiClassName", "Widget")
	Q_PROPERTY(QString Name READ getWidgetName WRITE setWidgetName)
	Q_PROPERTY(RPolicy Layout READ getPolicy WRITE setPolicy)
	Q_PROPERTY(PtAlign ReferencePt  READ getReferencePt WRITE setReferencePt)
	Q_PROPERTY(QSize MinSize READ getMininumSize WRITE setMininumSize)
	Q_PROPERTY(QSize MaxSize READ getMaxinumSize WRITE setMaxinumSize)
	//Q_PROPERTY(QString Tooltip READ getToolTip WRITE setToolTip)
	Q_PROPERTY(bool AbsScissor READ getAbs WRITE setAbs)
	Q_PROPERTY(bool Cliped READ getCliped WRITE setCliped)
	Q_PROPERTY(QRect absoluteRegion READ getAbsoluteRegion WRITE setAbsoluteRegion DESIGNABLE false)
	Q_PROPERTY(QRect Geometry READ getRelativeRegion WRITE setRelativeRegion)
	Q_PROPERTY(bool visiable READ getVisible WRITE setVisible DESIGNABLE false)
	Q_PROPERTY(bool transmit READ getTransfer WRITE setTransfer DESIGNABLE false)
	Q_PROPERTY(bool Penetration READ getPenetrate WRITE setPenetrate)
public:
	enum HTextAlignment
	{
		H_Left		= TA_H_LEFT,
		H_Right		= TA_H_RIGHT,
		H_Center	= TA_H_CENTER
	};

	enum VTextAlignment
	{
		V_Top			= TA_V_TOP,
		V_Bottom	= TA_V_BOTTOM,
		V_Center	= TA_V_CENTER
	};

	enum eOrientation
	{
		Horizontal,
		Vertical
	};

	
	enum DrawPolicy
	{
		Fixed,
		Stretch,
		H_Match,
		V_Match
	};

	bool getVisible() const;
	void setVisible(bool visible);
	QSize getMininumSize() const;
	void setMininumSize(const QSize &mininumSize);
	QSize getMaxinumSize() const;
	void setMaxinumSize(const QSize &maxinumSize);
	QString getWidgetName() const { return _widgetName; }
	void setWidgetName(const QString &widgetName);
	PtAlign getReferencePt() const;
	void setReferencePt(const PtAlign& pt);
	RPolicy getPolicy() const;
	void setPolicy(RPolicy po);
	QString getToolTip() const;
	void setToolTip(const QString &toolTip);
	bool getCliped() const;
	void setCliped(bool cliped);
	QRect getAbsoluteRegion() const;
	void setAbsoluteRegion(const QRect &region);
	QRect getRelativeRegion() const;
	void setRelativeRegion(const QRect &region);
	SkinFile getSkinFile() const;
	void setSkinFile(const SkinFile &skinFile);
	FontType getFont() const;
	virtual void setFont(const FontType &fontType);
	bool getInteraction() const;
	bool getTransfer() const;
	void setTransfer(bool tran);
	bool getPenetrate() const;
	void setPenetrate(bool tran);
	bool getAbs() const;
	void setAbs(bool tran);
	void setFlipX(bool flag);
	bool getFlipX() const;
	void setFlipY(bool flag);
	bool getFlipY() const;
protected:
	void emitUserEnumNamesChanged(const QString &propertyName, const QVariant &value)
	{
		userEnumNamesChanged(propertyName, value);
	}
signals:
	void skinFileChanged(const SkinFile &skinFile);
	void userEnumNamesChanged(const QString &propertyName, const QVariant &value);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile) {}
public:
	Q_INVOKABLE UiWidget(UiWidget *parent = 0);
	virtual ~UiWidget();
	QString getClassName() const;
	QList<UiWidget*> createDefaultWidget(const QString &name, UiWidget *parent, const QPoint &pos, e_widget_type type);
	bool setParent(UiWidget *parent);
	bool haveChild() { return !_child.isEmpty(); }
	UiWidget *getParent() const  { return _parent; }
	const QList<UiWidget*>& getChild() const { return _child; }
	c_widget *getWidget() const { return _widget; }
	bool valid();
	void move(const QVector2D &v);
	void resize(const QSize &size, bool savescale = false);
	UiWidget *getWidget(const QPoint &pos, bool acceptChild = false, const QList<UiWidget*> &except = QList<UiWidget*>() );
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Widget"; }
	virtual QString getSerializedClassName() const { return "Widget"; }
	QPoint getWidgetLTPoint();
	QPoint getWidgetRBPoint();
	QPoint getWidgetCenterPoint();
	int getWidgetWidth();
	int getWidgetHeight();
	void getWidgetSize(int &width, int &height);
	bool isContains(UiWidget *child);
	void appendSizeController(QVector<SizeController> &controllers);
	virtual bool handleDoubleClicked(int x, int y) { return false; }
	virtual bool handleRightClicked(int x, int y) { return false; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	void read( QXmlStreamReader &reader, UiWidget *parent = NULL);
	virtual void readExt(QXmlStreamReader &reader);
	QStringList getSkinNames();
	QString getSkinFileName();
	unsigned int getWidgetId();
public:
	static QString fromCString(const c_string &str);
	static c_string fromQString(const QString &str);
	static QColor fromCColor( const c_color &color );
	static c_color fromQColor( const QColor &color );
	static QString boolToString(bool v);
	static bool StringToBool(const QString &str);
	static int StringToAlign(const QString &str);
	virtual void attach(c_widget *widget) {}
	void Lock(bool flag) { _lock = flag; }
	bool isLock() const { return _lock; }
protected:
	QPoint getRelativePoint(const QPoint& point);
	virtual void addChild(UiWidget *child);
	void removeChild(UiWidget *child);
	bool setNewRegion(const QRect &oldRegion, const QRect &newRegion);
	virtual QList<UiWidget*> createInteractionWidgets(const QString &name) { return QList<UiWidget*>(); }
	SkinImageEx createSkinImage(const QString &bgImg, const QStringList &region);
	SkinImage createSkinImage(const QString &bgImg);
	void cacheWidget(UiWidget* node, QVector<UiWidget*>& widgets, QVector<QRect>& rects);
	void updateChildren(UiWidget* node, QVector<UiWidget*> widgets, QVector<QRect> rects);
	virtual void _create() {}

protected:
	c_widget *_widget;
	QString _widgetName;
	UiWidget *_parent;
	QList<UiWidget*> _child;
	SkinFile _skinFile;
	FontType _font;
	DrawPolicy _policy;
	HTextAlignment _Hreference;
	VTextAlignment _Vreference;
	QString _tmpSkin;
	double _scale;
	bool _hasStencil;
	bool _lock;
};

class TextAlign : public UserPropertyDataType
{
public:
	UiWidget::HTextAlignment horizontal;
	UiWidget::VTextAlignment vertical;

	TextAlign()
	{
		horizontal = UiWidget::H_Center;
		vertical = UiWidget::V_Center;
	}

	TextAlign(UiWidget::HTextAlignment h, UiWidget::VTextAlignment v)
	{
		horizontal = h;
		vertical = v;
	}

	TextAlign(const TextAlign &align)
		: UserPropertyDataType(align)
	{
		horizontal = align.horizontal;
		vertical = align.vertical;
	}

	bool equals(const TextAlign &align)
	{
		return horizontal == align.horizontal && vertical == align.vertical;
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if(var.canConvert<TextAlign>())
			return new TextAlign(var.value<TextAlign>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<TextAlign>(*this);
	}
};
Q_DECLARE_METATYPE(TextAlign)


class PtAlign : public UserPropertyDataType
{
public:
	UiWidget::HTextAlignment horizontal;
	UiWidget::VTextAlignment vertical;

	PtAlign()
	{
		horizontal = UiWidget::H_Left;
		vertical = UiWidget::V_Top;
	}

	PtAlign(UiWidget::HTextAlignment h, UiWidget::VTextAlignment v)
	{
		horizontal = h;
		vertical = v;
	}

	PtAlign(const PtAlign &align)
		: UserPropertyDataType(align)
	{
		horizontal = align.horizontal;
		vertical = align.vertical;
	}

	bool equals(const PtAlign &align)
	{
		return horizontal == align.horizontal && vertical == align.vertical;
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if (var.canConvert<PtAlign>())
			return new PtAlign(var.value<PtAlign>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<PtAlign>(*this);
	}
};
Q_DECLARE_METATYPE(PtAlign)

class RPolicy : public UserPropertyDataType
{
public:
	UiWidget::DrawPolicy dpolicy;

	RPolicy()
	{
		dpolicy = UiWidget::Fixed;
	}

	RPolicy(UiWidget::DrawPolicy h)
	{
		dpolicy = h;
	}

	RPolicy(const RPolicy &align)
		: UserPropertyDataType(align)
	{
		dpolicy = align.dpolicy;
	}

	bool equals(const RPolicy &align)
	{
		return dpolicy == align.dpolicy;
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if (var.canConvert<RPolicy>())
			return new RPolicy(var.value<RPolicy>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<RPolicy>(*this);
	}
};
Q_DECLARE_METATYPE(RPolicy)

class ROrientation : public UserPropertyDataType
{
public:
	UiWidget::eOrientation orientation;

	ROrientation()
	{
		orientation = UiWidget::Horizontal;
	}

	ROrientation(UiWidget::eOrientation h)
	{
		orientation = h;
	}

	ROrientation(const ROrientation &align)
		: UserPropertyDataType(align)
	{
		orientation = align.orientation;
	}

	bool equals(const ROrientation &align)
	{
		return orientation == align.orientation;
	}

	virtual UserPropertyDataType *getInstanceFromQVariant(const QVariant &var)
	{
		if (var.canConvert<ROrientation>())
			return new ROrientation(var.value<ROrientation>());
		return 0;
	}

	virtual QVariant getQVariant() const
	{
		return QVariant::fromValue<ROrientation>(*this);
	}
};
Q_DECLARE_METATYPE(ROrientation)

struct UiWidgetRef
{
	UiWidget *widget;

	UiWidgetRef()
	{

	}

	UiWidgetRef(UiWidget *widget)
	{
		this->widget = widget;
	}

	UiWidgetRef(const UiWidgetRef &widgetRef)
	{
		widget = widgetRef.widget;
	}
};
Q_DECLARE_METATYPE(UiWidgetRef)


#endif //UIWIDGET_H