#ifndef UILISTBOX_H
#define UILISTBOX_H

#include "UiWidget.h"
#include "GeneratedFiles/ui_listboxediterdialog.h"

#include <QtWidgets/QDialog>

class ListBoxEditerDialog : public QDialog , public Ui::ListBoxEditerDialog
{
	Q_OBJECT
public:
	ListBoxEditerDialog(QWidget *parent = 0);
	void setInitItems(QStringList items);
	QStringList getItems() const { return _itemList; }
private:
	void initUi();
	void initSignal();
protected slots:
	void slotAddBtnClicked();
	void slotDelBtnClicked();
	void slotUpBtnClicked();
	void slotDownBtnClicked();
	void slotOkBtnClicked();

private:
	QStringList _itemList;
};

class UiListBox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "c_listbox")
	Q_PROPERTY(int rowHeight READ getRowHeight WRITE setRowHeight)
	Q_PROPERTY(SkinImageEx backGroundImage READ getBackImg WRITE setBackImg)
	Q_PROPERTY(QStringList items READ getItems WRITE setItems DESIGNABLE false)
	Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
public:
	int getRowHeight() const;
	void setRowHeight(int height);
	SkinImageEx getBackImg();
	void setBackImg(const SkinImageEx &backImg);
	QStringList getItems() const;
	void setItems(const QStringList items);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
public:
	Q_INVOKABLE UiListBox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "c_listbox"; }
	virtual bool handleDoubleClicked(int x, int y);
	virtual QString getSerializedClassName() const { return "lib"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _backImage;
};

#endif //UILISTBOX_H