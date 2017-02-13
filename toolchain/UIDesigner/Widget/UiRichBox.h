#ifndef UIRICHBOX_H
#define UIRICHBOX_H
#include "UiWidget.h"


class UiRichBox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Label");
	Q_PROPERTY(bool Rich READ getRich WRITE setRich)
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(FontType Font READ getFont WRITE setFont)
	Q_PROPERTY(QString Text READ getText WRITE setText)
	Q_PROPERTY(QColor TextColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(QSize Padding READ getMargin WRITE setMargin)
	Q_PROPERTY(TextAlign Alignment READ getTextAlignment WRITE setTextAlignment)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getBackImg WRITE setBackImg)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
	//Q_PROPERTY(bool readOnly READ getReadOnly WRITE setReadOnly)
public:
	QString getText() const;
	void setText(const QString &text);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &backImg);
	QSize getMargin() const;
	void setMargin(const QSize &margin);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);
	bool getRich() const;
	void setRich(bool color);
	TextAlign getTextAlignment() const;
	void setTextAlignment(const TextAlign &align);
	virtual void setFont(const FontType &fontType);
public:
	Q_INVOKABLE UiRichBox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Label"; }
	virtual QString getSerializedClassName() const { return "Label"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _skinImage;
	SkinImage _stencilImg;
	bool _rich;
	TextAlign m_align;
	QString _plainstr;
};


#endif