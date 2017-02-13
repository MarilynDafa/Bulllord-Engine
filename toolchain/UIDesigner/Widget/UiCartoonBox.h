#ifndef UICARTOONBOX_H
#define UICARTOONBOX_H

#include "UiWidget.h"
#include "GeneratedFiles/ui_cartoonboxediterdialog.h"

class CartoonBoxEditerDialog : public QDialog , public Ui::CartoonBoxEditerDialog
{
	Q_OBJECT
public:
	CartoonBoxEditerDialog(QDialog *parent = 0);
private:
	void initUi();
	void initSignal();
protected slots:
	void slotBrowseBtnClicked();
	void slotAddBtnClicked();
	void slotDelBtnClicked();
	void slotOkBtnClicked();
};

class UiCartoonBox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Palette")
	Q_PROPERTY(bool loop READ getLoop WRITE setLoop)
	Q_PROPERTY(int framerate READ getFramerate WRITE setFramerate)
	Q_PROPERTY(CartoonBoxFrames frames READ getFrames WRITE setFrames DESIGNABLE false)
public:
	bool getLoop() const;
	void setLoop(bool loop);
	int getFramerate();
	void setFramerate(int rate);
	CartoonBoxFrames getFrames() const;
	void setFrames(const CartoonBoxFrames &frames);
public:
	Q_INVOKABLE UiCartoonBox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Palette"; }
	virtual bool handleDoubleClicked(int x, int y);
	virtual QString getSerializedClassName() const { return "Palette"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
private:
	CartoonBoxFrames _frames;
};

#endif //UICARTOONBOX_H