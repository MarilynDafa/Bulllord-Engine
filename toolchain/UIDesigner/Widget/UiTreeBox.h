#ifndef UITREEBOX_H
#define UITREEBOX_H

#include "UiWidget.h"


using namespace o2d;

class UiTreebox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "c_treebox")
	Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(SkinImageEx backGroundImage READ getBackImg WRITE setBackImg)
	Q_PROPERTY(bool drawLines READ getDrawLines WRITE setDrawLines)
	Q_PROPERTY(int xoffset READ getXOffset WRITE setXOffset)
public:
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &backImg);
	bool getDrawLines() const;
	void setDrawLines(bool draw);
	int getXOffset() const;
	void setXOffset(int off);
public:
	Q_INVOKABLE UiTreebox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "c_treebox"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);
private:
	SkinImageEx _skinImage;
};

#endif //UITREEBOX_H