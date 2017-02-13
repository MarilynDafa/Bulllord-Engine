#ifndef UIRADIO_H
#define UIRADIO_H

#include "UiWidget.h"

class UiRadio : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Check");
	Q_PROPERTY(bool Enable READ getEnable WRITE setEnable)
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(FontType Font READ getFont WRITE setFont)
	Q_PROPERTY(QString Text READ getText WRITE setText)
	Q_PROPERTY(QColor TextColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(TextAlign Alignment READ getTextAlignment WRITE setTextAlignment)
	Q_PROPERTY(bool pushed READ getIsPushed WRITE setPushed DESIGNABLE false)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getNormalImage WRITE setNormalImage)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
	Q_PROPERTY(SkinImageEx CheckedMap READ getPressedImage WRITE setPressedImage)
	Q_PROPERTY(SkinImageEx DisableMap READ getDisableImage WRITE setDisableImage)
public:
	QString getText() const;
	void setText(const QString &text);
	TextAlign getTextAlignment() const;
	void setTextAlignment(const TextAlign &align);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	bool getEnable() const;
	void setEnable(bool enable);
	bool getIsPushed() const;
	void setPushed(bool pushed);
	SkinImageEx getNormalImage() const;
	void setNormalImage(const SkinImageEx &image);
	SkinImageEx getPressedImage() const;
	void setPressedImage(const SkinImageEx &image);
	SkinImageEx getDisableImage() const;
	void setDisableImage(const SkinImageEx &image);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);

public:
	Q_INVOKABLE UiRadio(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Check"; }
	virtual QString getSerializedClassName() const { return "Check"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _normalImage;
	SkinImageEx _pressedImage;
	SkinImageEx _disableImage;
	SkinImage _stencilImg;
};
#endif //UIRADIO_H