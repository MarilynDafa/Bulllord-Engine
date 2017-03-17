#ifndef UITABLEBOX_H
#define UITABLEBOX_H

#include "UiWidget.h"
#include "GeneratedFiles/ui_tableboxediterdialog.h"

class QToolButton;

class TableBoxEditerDialog : public QDialog , public Ui::TableBoxEditerDialog
{
	Q_OBJECT
public:
	TableBoxEditerDialog(QWidget *parent = 0);
	void initTableData(const TableItemMap& map);
	const TableItemMap &getTableItemMap() const { return _tableItemMap; }
private:
	void initUi();
	void initSignal();
	void moveTableCloumn(int src, int dest);
	void moveTableRow(int src, int dest);
	void addTableCloumn(int index, const QString &header);
	void addTableRow(int index, const QString &header);
	void changeHHeaderName(int index, const QString &name);
	void changeVHeaderName(int index, const QString &name);
	void delTableColumn(int index);
	void delTableRow(int index);
protected slots:
	void slotAddBtnClicked();
	void slotDelBtnClicked();
	void slotUpBtnClicked();
	void slotDownBtnClicked();
	void slotListItemTextChanged(QListWidgetItem * item);
	void slotPropertyBtnClicked();
	void slotOkBtnClicked();

private:
	TableItemMap _tableItemMap;
};

class UiTableBox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "Table");
	Q_PROPERTY(bool EowSeparator READ getRowSeparator WRITE setRowSeparator DESIGNABLE false)
	Q_PROPERTY(bool ColSeparator READ getColSeparator WRITE setColSeparator DESIGNABLE false)
	Q_PROPERTY(bool FlipX READ getFlipX WRITE setFlipX)
	Q_PROPERTY(bool FlipY READ getFlipY WRITE setFlipY)
	Q_PROPERTY(int RowHeight READ getRowHeight WRITE setRowHeight)
	Q_PROPERTY(FontType Font READ getFont WRITE setFont)
	Q_PROPERTY(QColor TextColor READ getTextColor WRITE setTextColor)
	Q_PROPERTY(SkinFile Pixmap READ getSkinFile WRITE setSkinFile)
	Q_PROPERTY(SkinImageEx CommonMap READ getBackImg WRITE setBackImg)
	Q_PROPERTY(SkinImage StencilMap READ getStencil WRITE setStencil)
	Q_PROPERTY(SkinImage OddItemMap READ getOdd WRITE setOdd)
	Q_PROPERTY(SkinImage EvenItemMap READ getEven WRITE setEven)
	Q_PROPERTY(TableItemMap ItemMap READ getItemMap WRITE setItemMap DESIGNABLE false)
public:
	SkinImageEx getBackImg() const;
	void setBackImg(const SkinImageEx &image);
	TableItemMap getItemMap() const;
	void setItemMap(const TableItemMap &m);
	void setRowSeparator(bool sep);
	bool getRowSeparator() const;
	void setColSeparator(bool sep);
	bool getColSeparator() const;
	QColor getTextColor() const;
	void setTextColor(const QColor &color);
	SkinImage getStencil();
	void setStencil(const SkinImage &stencilImg);
	SkinImage getOdd();
	void setOdd(const SkinImage &stencilImg);
	SkinImage getEven();
	void setEven(const SkinImage &stencilImg);
	int getRowHeight();
	void setRowHeight(int h);
public:
	Q_INVOKABLE UiTableBox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "Table"; }
	virtual bool handleDoubleClicked(int x, int y);
	virtual QString getSerializedClassName() const { return "Table"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected:
	virtual void _create();
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);

private:
	SkinImageEx _backImage;
	SkinImage _stencilImg;
	SkinImage _OddImg;
	SkinImage _EvenImg;
};

#endif //UITABLEBOX_H