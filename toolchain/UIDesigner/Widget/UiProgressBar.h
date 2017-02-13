#ifndef UIPROGRESSBAR_H
#define UIPROGRESSBAR_H

#include "UiWidget.h"

class UiProgressBar : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Progress")
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(FontType Font READ getFont WRITE setFont)
	Q_PROPERTY(QString Text READ getText WRITE setText)
	Q_PROPERTY(QColor TextColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(QString Percent READ getPosition WRITE setPosition)
	Q_PROPERTY(QPointF Border READ getOffset WRITE setOffset)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getBackImg WRITE setBackImg)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
	Q_PROPERTY(SkinImage FillMap READ getFillImg WRITE setFillImg)
public:
	QString getText() const;
	void setText(const QString &text);
	QString getPosition() const;
	void setPosition(const QString &postion);
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &image);
	SkinImage getFillImg() const;
	void setFillImg(const SkinImage &image); 
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	QPointF getOffset() const;
	void setOffset(const QPointF &val);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);

public:
	Q_INVOKABLE UiProgressBar(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Progress"; }
	virtual QString getSerializedClassName() const { return "Progress"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _backImage;
	SkinImage _fillImage;
	SkinImage _stencilImg;
};

#endif //UIPROGRESSBAR_H