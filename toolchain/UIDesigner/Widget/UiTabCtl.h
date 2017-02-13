#ifndef UITABCTL_H
#define UITABCTL_H

#include "UiWidget.h"
#include <QtWidgets/QDialog>
#include "GeneratedFiles/ui_ListboxEditerDialog.h"
class UiTabCtl;
class TabCtlEditDialog : public QDialog, public Ui::ListBoxEditerDialog
{
	Q_OBJECT
public:
	enum ItemType
	{
		IT_Def = 0,
		IT_Modify,
		IT_Del,
		IT_New
	};

	struct ItemData
	{
		ItemType t;
		QString name;

		ItemData()
		{

		}

		ItemData(const ItemData &data)
			: t(data.t)
			, name(data.name)
		{

		}

		ItemData(ItemType t, const QString &name)
			: t(t)
			, name(name)
		{

		}
	};

	TabCtlEditDialog(QWidget *parent = 0);
	~TabCtlEditDialog();
	void initUi();
	void initSignal();
	void setUserData(UiTabCtl* ud);

	void setInitItems(const QStringList &items);
	const QVector<ItemData> &getItemDatas() const { return _items; }

protected:
	void addItems(const QString &name, ItemType t);

protected slots:
	void slotAddBtnPressed();
	void slotDelBtnPressed();
	void slotOkBtnPressed();
	void slotResetBtnPressed();
	void slotItemChangedPressed(QTableWidgetItem* item);

private:
	QVector<ItemData> _items;
	UiTabCtl* _priv;
	c_vector<c_float2> _cache;
};

class UiTabCtl : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Primitive");
	Q_PROPERTY(bool Fill READ getFill WRITE setFill)
	Q_PROPERTY(bool Closed READ getClosed WRITE setClosed)
	Q_PROPERTY(QColor Color READ getColor WRITE setColor)
public:
	int getOffset() const;
	void setOffset(int height);
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	SkinImageEx getBackImg()const;
	void setBackImg(const SkinImageEx &backImg);
	SkinImage getNormalImg()const;
	void setNormalImg(const SkinImage &backImg);
	SkinImage getActiveImg()const;
	void setActiveImg(const SkinImage &backImg);
	int getActivePage() const;
	void setActivePage(int page);
	bool getTabup() const;
	void setTabup(bool wordWrap);

	bool getFill() const;
	void setFill(bool flag);
	bool getClosed() const;
	void setClosed(bool flag);
	QColor getColor() const;
	void setColor(const QColor &color);

	void reshape();
public:
	Q_INVOKABLE UiTabCtl(UiWidget *parent = 0);
	virtual bool acceptChildern() const;
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Primitive"; }
	virtual QString getSerializedClassName() const { return "Primitive"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual bool handleDoubleClicked(int x, int y);
	virtual void addChild(UiWidget *child);
	QStringList getItems();
	virtual void readExt(QXmlStreamReader &reader);
	void preview(c_vector<c_float2>* data = nullptr);
	c_vector<c_float2> path;
protected:
	void _create();
private:
	SkinImageEx _backImage;
	SkinImage _aImage;
	SkinImage _nImage;
	bool _fill, _closed;
	QColor _clr;
	//virtual bool handleRightClicked(int x, int y);
};

#endif //UITABCTL_H