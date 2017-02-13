#ifndef UISLIDER_H
#define UISLIDER_H

#include "UiWidget.h"

class UiSlider : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Slider")
	Q_PROPERTY(bool Enable READ getEnable WRITE setEnable)
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(ROrientation Orientation READ getHorizontal WRITE setHorizontal)
	Q_PROPERTY(int MinValue READ getMinValue WRITE setMinValue)
	Q_PROPERTY(int MaxValue READ getMaxValue WRITE setMaxValue)
	Q_PROPERTY(int SliderPosition READ getPosition WRITE setPosition)
	Q_PROPERTY(int SliderStep READ getStep WRITE setStep)
	Q_PROPERTY(QSize SliderSize READ getThumbSize WRITE setThumbSize)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getBackImg WRITE setBackImg)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
	Q_PROPERTY(SkinImage SliderCommonMap READ getThumbImg WRITE setThumbImg)
	Q_PROPERTY(SkinImage SliderDisableMap READ getThumbDisableImg WRITE setThumbDisableImg)
public:
	bool getEnable() const;
	void setEnable(bool enable);
	ROrientation getHorizontal() const;
	void setHorizontal(ROrientation horizontal);
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &image);
	SkinImage getThumbImg() const;
	void setThumbImg(const SkinImage &image);
	SkinImage getThumbDisableImg() const;
	void setThumbDisableImg(const SkinImage &image);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);
	int getMaxValue() const;
	void setMaxValue(int v);
	int getMinValue() const;
	void setMinValue(int v);
	int getPosition() const;
	void setPosition(int pos);
	int getStep() const;
	void setStep(int setp);
	QSize getThumbSize() const;
	void setThumbSize(const QSize &thumbSize);

public:
	Q_INVOKABLE UiSlider(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Slider"; }
	virtual QString getSerializedClassName() const { return "Slider"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _backImage;
	SkinImage _thumbImage;
	SkinImage _thumbDisableImage;
	SkinImage _stencilImg;
};

#endif //UISLIDER_H