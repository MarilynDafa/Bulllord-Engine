#ifndef UILABEL_H
#define UILABEL_H

#include "UiWidget.h"

using namespace o2d;

class UiLabel : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Dial")
	Q_PROPERTY(bool Clockwise  READ getClockwise WRITE setClockwise)
	Q_PROPERTY(bool AngleCut  READ getAngleCut WRITE setAngleCut)
	Q_PROPERTY(int StartAngle  READ getStartAngle WRITE setStartAngle)
	Q_PROPERTY(int EndAngle  READ getEndAngle WRITE setEndAngle)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImage CommonMap READ getCommon WRITE setCommon)
public:
	QString getText() const;
	void setText(const QString &text);
	TextAlign getTextAlignment() const;
	void setTextAlignment(const TextAlign &align);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	bool getWordWrap() const;
	void setWordWrap(bool wordWrap);
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &backImg);
	QSize getMargin() const;
	void setMargin(const QSize &margin);

	bool getAngleCut() const;
	void setAngleCut(bool flag);
	bool getClockwise() const;
	void setClockwise(bool flag);
	int getStartAngle() const;
	void setStartAngle(int value);
	int getEndAngle() const;
	void setEndAngle(int value);

	SkinImage getCommon();
	void setCommon(const SkinImage &commonMap);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);
public:
	Q_INVOKABLE UiLabel(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Dial"; }
	virtual QString getSerializedClassName() const { return "Dial"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected:
	void _create();
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);
private:
	SkinImageEx _skinImage;
	SkinImage _CommonMap;
	SkinImage _stencilImg;
	bool _clockwise;
};

#endif //UILABEL_H