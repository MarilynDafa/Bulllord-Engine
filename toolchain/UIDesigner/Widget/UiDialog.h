#ifndef UIDIALOG_H
#define UIDIALOG_H

#include "UiWidget.h"

class UiDialog : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Panel");
	Q_PROPERTY(bool Dragable READ getDragable WRITE setDragable)
	Q_PROPERTY(bool Modal READ getModal WRITE setModal)
	Q_PROPERTY(bool Scrollable READ getScrollable WRITE setScrollable)
	Q_PROPERTY(bool BasePlate READ getBasePlate WRITE setBasePlate)
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getBackImg WRITE setBackImg)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
public:
	SkinImageEx getBackImg();
	void setBackImg(const SkinImageEx &backImg);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);
public:
	Q_INVOKABLE UiDialog(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Panel"; }
	virtual QString getSerializedClassName() const { return "Panel"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
	bool getDragable() const;
	void setDragable(bool tran);
	bool getModal() const;
	void setModal(bool tran);
	bool getScrollable() const;
	void setScrollable(bool tran);

	bool getBasePlate() const;
	void setBasePlate(bool flag);
protected:
	//virtual QList<UiWidget*> createInteractionWidgets(const QString &name);

protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _backImage;
	SkinImage _stencilImg;
	bool _dragable;
	bool _Modal;
	bool _Scrollable;
	bool _BasePlate;
};

#endif //UIDIALOG_H