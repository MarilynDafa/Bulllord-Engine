#ifndef UIEDITBOX_H
#define UIEDITBOX_H

#include "UiWidget.h"

class UiEditBox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Text");
	Q_PROPERTY(bool Enable READ getEnable WRITE setEnable)
	Q_PROPERTY(bool AutoScroll READ getAutoScroll WRITE setAutoScroll)
	Q_PROPERTY(bool MultiLine READ getMultiLine WRITE setMultiLine)
	Q_PROPERTY(bool Password READ getPassword WRITE setPassword)
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(int MaxLength READ getMaxC WRITE setMaxC)
	Q_PROPERTY(FontType Font READ getFont WRITE setFont)
	Q_PROPERTY(QString Placeholder READ getText WRITE setText)
	Q_PROPERTY(QColor TextColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(QColor PlaceholderColor READ getPlaceholderColor WRITE setPlaceholderColor)
	Q_PROPERTY(TextAlign Alignment READ getTextAlignment WRITE setTextAlignment)
	Q_PROPERTY(QSize Padding READ getMargin WRITE setMargin)
	Q_PROPERTY(NumericSet Numeric READ getNumeric WRITE setNumeric)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getBackImg WRITE setBackImg)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
public:
	QString getText() const;
	void setText(const QString &text);
	TextAlign getTextAlignment() const;
	void setTextAlignment(const TextAlign &align);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	QColor getPlaceholderColor() const;
	void setPlaceholderColor(const QColor &color);
	bool getEnable() const;
	void setEnable(bool enable);
	bool getAutoScroll() const;
	void setAutoScroll(bool autoScroll);
	bool getWordWrap() const;
	void setWordWrap(bool wordWrap);
	bool getMultiLine() const;
	void setMultiLine(bool multiLine);
	bool getPassword() const;
	void setPassword(bool password);
	NumericSet getNumeric() const;
	void setNumeric(const NumericSet &numeric);
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &image);
	QSize getMargin() const;
	void setMargin(const QSize &margin);
	int getMaxC() const;
	void setMaxC(int maxC);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);

public:
	Q_INVOKABLE UiEditBox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Text"; }
	virtual QString getSerializedClassName() const { return "Text"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _skinImage;
	SkinImage _stencilImg;
	unsigned int _txtColor;
};

#endif //UIEDITBOX_H