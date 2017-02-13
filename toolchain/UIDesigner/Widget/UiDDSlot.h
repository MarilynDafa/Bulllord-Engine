#ifndef UIDDSLOT_H
#define UIDDSLOT_H
#include "UiWidget.h"
class UiDDSlot : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "c_dd_slot");
	Q_PROPERTY(QString text READ getText WRITE setText)
	Q_PROPERTY(TextAlign textAlignment READ getTextAlignment WRITE setTextAlignment)
	Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(SkinImage backGroundImage READ getBackImg WRITE setBackImg)
public:
	Q_INVOKABLE UiDDSlot(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "c_dd_slot"; }
	virtual QString getSerializedClassName() const { return "dds"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
public:
	QString getText() const;
	void setText(const QString &text);
	TextAlign getTextAlignment() const;
	void setTextAlignment(const TextAlign &align);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	SkinImage getBackImg() const;
	void setBackImg(const SkinImage &backImg);
private:
	SkinImage _image;
};
#endif