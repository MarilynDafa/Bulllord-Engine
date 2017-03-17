#include "UiTableBox.h"
#include "TableBox.h"
#include "PropertyEditerDockWidget.h"
#include "ResourceMgr.h"
#include "UIMgr.h"


TableBoxEditerDialog::TableBoxEditerDialog( QWidget *parent /*= 0*/ )
	: QDialog(parent)
{
	initUi();
	initSignal();
}

void TableBoxEditerDialog::initUi()
{
	setupUi(this);
}

void TableBoxEditerDialog::initSignal()
{
	connect(_rAddBtn,SIGNAL(clicked()),this,SLOT(slotAddBtnClicked()));
	connect(_rDelBtn,SIGNAL(clicked()),this,SLOT(slotDelBtnClicked()));
	connect(_rMoveUpBtn,SIGNAL(clicked()),this,SLOT(slotUpBtnClicked()));
	connect(_rMoveDownBtn,SIGNAL(clicked()),this,SLOT(slotDownBtnClicked()));

	connect(_cAddBtn,SIGNAL(clicked()),this,SLOT(slotAddBtnClicked()));
	connect(_cDelBtn,SIGNAL(clicked()),this,SLOT(slotDelBtnClicked()));
	connect(_cMoveUpBtn,SIGNAL(clicked()),this,SLOT(slotUpBtnClicked()));
	connect(_cMoveDownBtn,SIGNAL(clicked()),this,SLOT(slotDownBtnClicked()));

	connect(_rowList,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(slotListItemTextChanged(QListWidgetItem*)));
	connect(_columnList,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(slotListItemTextChanged(QListWidgetItem*)));

	connect(_propertyBtn,SIGNAL(clicked()),this,SLOT(slotPropertyBtnClicked()));

	connect(_okBtn,SIGNAL(clicked()),this,SLOT(slotOkBtnClicked()));
}

void TableBoxEditerDialog::initTableData( const TableItemMap& map )
{
	const QStringList &items = map.getItems();
	const QStringList &columns = map.getColumns();

	int rowCount = items.count();
	int columnCount = columns.count();
	if(columnCount != 0)
		rowCount /= columnCount;

	for(int i = 0; i < columnCount; i ++)
	{
		QString str = columns[i];
		addTableCloumn(i, str);
		QListWidgetItem *item = new QListWidgetItem(str);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		_columnList->addItem(item);
	}

	for(int i = 0; i < rowCount; i ++)
	{
		QString str = QString("row_%1").arg(i);
		addTableRow(i, str);
		QListWidgetItem *item = new QListWidgetItem(str);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		_rowList->addItem(item);
	}

	for(int i = 0; i < rowCount; i ++)
	{
		for(int j = 0; j < columnCount; j ++)
		{
			QTableWidgetItem *item = new QTableWidgetItem(items[i*columnCount + j]);
			_cellTable->setItem(i, j, item);
		}
	}
}

void TableBoxEditerDialog::moveTableCloumn( int src, int dest )
{
	int count = _cellTable->columnCount();
	if(src >= count || dest >= count || src < 0 || dest < 0)
		return;

	QVector<QTableWidgetItem*> items;
	for(int i = 0; i < count; i ++)
	{
		QTableWidgetItem *item = _cellTable->takeItem(i ,src);
		items.append(item);
	}

	QTableWidgetItem *headerItem = _cellTable->takeHorizontalHeaderItem(src);
	delTableColumn(src);
	_cellTable->insertColumn(dest);
	_cellTable->setHorizontalHeaderItem(dest, headerItem);
	for(int i = 0; i < count; i ++)
	{
		QTableWidgetItem *item = items[i];
		_cellTable->setItem(i, dest, item);
	}

	items.clear();
}

void TableBoxEditerDialog::moveTableRow( int src, int dest )
{
	int count = _cellTable->rowCount();
	if(src >= count || dest >= count || src < 0 || dest < 0)
		return;

	QVector<QTableWidgetItem*> items;
	for(int i = 0; i < count; i ++)
	{
		QTableWidgetItem *item = _cellTable->takeItem(src ,i);
		items.append(item);
	}

	QTableWidgetItem *headerItem = _cellTable->takeVerticalHeaderItem(src);
	delTableRow(src);
	_cellTable->insertRow(dest);
	_cellTable->setVerticalHeaderItem(dest, headerItem);
	for(int i = 0; i < count; i ++)
	{
		QTableWidgetItem *item = items[i];
		_cellTable->setItem(dest, i, item);
	}

	items.clear();
}

void TableBoxEditerDialog::addTableCloumn( int index, const QString &header )
{
	QTableWidgetItem *item = new QTableWidgetItem(header);
	_cellTable->insertColumn(index);
	_cellTable->setHorizontalHeaderItem(index, item);
}

void TableBoxEditerDialog::addTableRow( int index, const QString &header )
{
	QTableWidgetItem *item = new QTableWidgetItem(header);
	_cellTable->insertRow(index);
	_cellTable->setVerticalHeaderItem(index, item);
}

void TableBoxEditerDialog::changeHHeaderName( int index, const QString &name )
{
	QTableWidgetItem *item = _cellTable->horizontalHeaderItem(index);
	if(item != 0)
		item->setText(name);
}

void TableBoxEditerDialog::changeVHeaderName( int index, const QString &name )
{
	QTableWidgetItem *item = _cellTable->verticalHeaderItem(index);
	if(item != 0)
		item->setText(name);
}

void TableBoxEditerDialog::delTableColumn( int index )
{
	_cellTable->removeColumn(index);
}

void TableBoxEditerDialog::delTableRow( int index )
{
	_cellTable->removeRow(index);
}

void TableBoxEditerDialog::slotAddBtnClicked()
{
	QListWidget *widget = 0;
	QString itemName;
	QToolButton *btn = dynamic_cast<QToolButton*>(sender());
	if(btn == _rAddBtn)
	{
		widget = _rowList;
		itemName = "new row";
	}
	else if(btn == _cAddBtn)
	{
		widget = _columnList;
		itemName = "new column";
	}

	if(widget != 0)
	{
		QListWidgetItem *item = new QListWidgetItem(itemName);
		item->setFlags(item->flags() | Qt::ItemIsEditable);

		int row = widget->currentRow();
		widget->insertItem(row + 1, item);
		widget->setCurrentItem(item);
		widget->editItem(item);

		if(widget == _rowList)
			addTableRow(row + 1, itemName);
		else if(widget == _columnList)
			addTableCloumn(row + 1, itemName);
	}
}

void TableBoxEditerDialog::slotDelBtnClicked()
{
	QListWidget *widget = 0;
	QToolButton *btn = dynamic_cast<QToolButton*>(sender());
	if(btn == _rDelBtn)
	{
		widget = _rowList;
	}
	else if(btn == _cDelBtn)
	{
		widget = _columnList;
	}

	if(widget != 0)
	{
		int row = widget->currentRow();
		QListWidgetItem *item = widget->takeItem(row);
		delete item;

		if(widget == _rowList)
			delTableRow(row);
		else if(widget == _columnList)
			delTableColumn(row);
	}
}

void TableBoxEditerDialog::slotUpBtnClicked()
{
	QListWidget *widget = 0;
	QToolButton *btn = dynamic_cast<QToolButton*>(sender());
	if(btn == _rMoveUpBtn)
	{
		widget = _rowList;
	}
	else if(btn == _cMoveUpBtn)
	{
		widget = _columnList;
	}

	if(widget != 0)
	{
		int row = widget->currentRow();
		if(row > 0)
		{
			QListWidgetItem *item = widget->takeItem(row);
			widget->insertItem(row - 1, item);
			widget->setCurrentItem(item);

			if(widget == _rowList)
				moveTableRow(row, row - 1);
			else if(widget == _columnList)
				moveTableCloumn(row, row - 1);
		}
	}
}

void TableBoxEditerDialog::slotDownBtnClicked()
{
	QListWidget *widget = 0;
	QToolButton *btn = dynamic_cast<QToolButton*>(sender());
	if(btn == _rMoveDownBtn)
	{
		widget = _rowList;
	}
	else if(btn == _cMoveDownBtn)
	{
		widget = _columnList;
	}

	if(widget != 0)
	{
		int row = widget->currentRow();
		if(row < widget->count())
		{
			QListWidgetItem *item = widget->takeItem(row);
			widget->insertItem(row + 1, item);
			widget->setCurrentItem(item);

			if(widget == _rowList)
				moveTableRow(row, row + 1);
			else if(widget == _columnList)
				moveTableCloumn(row, row + 1);
		}
	}
}

void TableBoxEditerDialog::slotListItemTextChanged( QListWidgetItem * item )
{
	QListWidget *widget = item->listWidget();
	int index = widget->row(item);
	QString text = item->text();
	if(widget == _rowList)
		changeVHeaderName(index, text);
	else if(widget == _columnList)
		changeHHeaderName(index, text);
}

void TableBoxEditerDialog::slotPropertyBtnClicked()
{

}

void TableBoxEditerDialog::slotOkBtnClicked()
{
	int columnCount = _cellTable->columnCount();
	int rowCount = _cellTable->rowCount();
	QStringList columns, rows, items;

	for(int i = 0; i < columnCount; i ++)
	{
		QTableWidgetItem *item = _cellTable->horizontalHeaderItem(i);
		assert(item != 0);
		columns.append(item->text());
	}

	for(int i = 0; i < rowCount; i ++)
	{
		QTableWidgetItem *item = _cellTable->verticalHeaderItem(i);
		assert(item != 0);
		rows.append(item->text());
	}

	for(int i = 0; i < rowCount; i ++)
	{
		for(int j = 0; j < columnCount; j ++)
		{
			QTableWidgetItem *item = _cellTable->item(i, j);
			if(item == 0)
				items.append(QString());
			else
				items.append(item->text());
		}
	}

	_cellTable->clear();
	_tableItemMap = TableItemMap(items, columns);

	accept();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UiTableBox::UiTableBox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
	
}

void UiTableBox::getDefaultSize( int &width, int &height ) const
{
	width = 400;
	height = 350;
}

bool UiTableBox::handleDoubleClicked( int x, int y )
{
	//TableBoxEditerDialog dlg;
	//dlg.initTableData(getItemMap());
	//if(dlg.exec() == QDialog::Accepted)
	//	setItemMap(dlg.getTableItemMap());
	return true;
}

QDomElement UiTableBox::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("Ext");
	c_string tmp = fromQString(getSkinFile().getFileName());
	if (tmp.get_length())
	{
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmp.tokenize(L"/", tmp2);
		tmp2.back().tokenize(L".", tmp3);
		extEle.setAttribute("Pixmap", (char*)tmp3[0].to_utf8());
	}
	else
		extEle.setAttribute("Pixmap", "");
	extEle.setAttribute("CommonMap", getBackImg().getSkinName());
	QPoint pt = getBackImg().getSkinSet().getCenterPt().toPoint();
	QSize sz = getBackImg().getSkinSet().getCenterSize().toSize();
	QString region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("CommonTexcoord", region);
	extEle.setAttribute("RowHeight", getRowHeight());
	extEle.setAttribute("StencilMap", getStencil().getSkinName());
	extEle.setAttribute("TextColor",fromQColor(getTextColor()).rgba());
	extEle.setAttribute("OddItemMap", getOdd().getSkinName());
	extEle.setAttribute("EvenItemMap", getEven().getSkinName());
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));

	c_string tmpd = fromQString(getFont().getFontFile().getFileName());
	c_vector<c_string> tmp2;
	c_vector<c_string> tmp3;
	tmpd.tokenize(L"/", tmp2);
	tmp2.back().tokenize(L".", tmp3);

	extEle.setAttribute("FontSrc", QString((const char*)tmp3.front().to_utf8()));
	extEle.setAttribute("FontSize", getFont().getFontSize());
	extEle.setAttribute("FontOutline", boolToString(getFont().getFontOutline()));
	extEle.setAttribute("FontItalics", boolToString(getFont().getFontItalics()));
	extEle.setAttribute("FontShadow", boolToString(getFont().getFontShadow()));
	extEle.setAttribute("FontBold", boolToString(getFont().getFontBold()));

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Table");
	return ele;
}
void UiTableBox::_create()
{
	for (int i = 0; i < 2; i++)
		((c_tablebox*)_widget)->add_column(L"", i);
	for (int i = 0; i < 120; i++)
		((c_tablebox*)_widget)->add_row(i);
}
void UiTableBox::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	QStringRef skinId = atts.value(QLatin1String("Pixmap"));
	QString skinFileName = "";
	c_string tmp = fromQString(skinId.toString());
	if (tmp.get_length())
	{
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmp.tokenize(L"/", tmp2);
		tmp2.back().tokenize(L".", tmp3);
		skinFileName = ResourceMgr::getInstance()->getSkin(hash(tmp3.front().c_str()));
	}
	SkinFile skinFile(skinFileName);
	setSkinFile(skinFile);

	QString bImg = atts.value(QLatin1String("CommonMap")).toString();
	QStringList bReg = atts.value(QLatin1String("CommonTexcoord")).toString().split(",");
	QStringRef stenciltag = atts.value(QLatin1String("StencilMap"));
	setStencil(createSkinImage(stenciltag.toString()));
	unsigned int txtClr = atts.value(QLatin1String("TextColor")).toUInt();
	QStringRef odd = atts.value(QLatin1String("OddItemMap"));
	QStringRef even = atts.value(QLatin1String("EvenItemMap"));

	int fntH = atts.value(QLatin1String("FontSize")).toString().toInt();
	QStringRef fontsrctag = atts.value(QLatin1String("FontSrc"));

	bool outline = StringToBool(atts.value(QLatin1String("FontOutline")).toString());
	bool intly = StringToBool(atts.value(QLatin1String("FontItalics")).toString());
	bool bold = StringToBool(atts.value(QLatin1String("FontBold")).toString());
	bool shadow = StringToBool(atts.value(QLatin1String("FontShadow")).toString());
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
	int rh = atts.value(QLatin1String("RowHeight")).toString().toInt();
	setFlipX(FlipX);
	setFlipY(FlipY);

	unsigned int fontid = hash(fontsrctag.toString().toStdWString().c_str());
	QString fontFileName = ResourceMgr::getInstance()->getFont(fontid);
	QString fontName = FontType::getFontName(fontFileName);
	FontType ft;
	FontFile ff(fontName, fontFileName);
	ft.setFontFile(ff);
	ft.setFontSize(fntH);
	ft.setFontOutline(outline);
	ft.setFontShadow(shadow);
	ft.setFontBold(bold);
	ft.setFontItalics(intly);
	setFont(ft);
	setRowHeight(rh);
	setBackImg(createSkinImage(bImg, bReg));
	setTextColor(fromCColor(c_color(txtClr)));
	setOdd(createSkinImage(odd.toString()));
	setEven(createSkinImage(even.toString()));
}


//////////////////////////////////////////////////////////////////////////

SkinImageEx UiTableBox::getBackImg() const
{
	assert(((c_tablebox*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _backImage;
}

void UiTableBox::setBackImg( const SkinImageEx &image )
{
	SkinSet skinSet = image.getSkinSet();
	_backImage.setData(image);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("Geometry"));
	if (perty && _tmpSkin != image.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_backImage.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QRect rect(rc.x(), rc.y(), trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("Geometry", rect);
			_tmpSkin = image.getSkinName();
		}
	}
	((c_tablebox*)_widget)->set_bg_image(fromQString(_backImage.getSkinName()));
	((c_tablebox*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

void UiTableBox::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_backImage.setSkinFile(skinFile);
	//_headerImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_backImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("headerImage"), QVariant::fromValue<SkinImageEx>(_headerImage));
}

TableItemMap UiTableBox::getItemMap() const
{
	int columnCount = ((c_tablebox*)_widget)->get_column_count();
	int rowCount = ((c_tablebox*)_widget)->get_row_count();
	QStringList columns, items;

	for(int i = 0; i < columnCount; i ++)
		columns.append(fromCString(((c_tablebox*)_widget)->get_caption(i)));

	for(int i = 0; i < rowCount; i ++)
	{
		for(int j = 0; j < columnCount; j ++)
		{
			items.append(fromCString(((c_tablebox*)_widget)->get_cell_text(i, j)));
		}
	}

	return TableItemMap(items, columns);
}

void UiTableBox::setItemMap( const TableItemMap &m )
{
	const QStringList &items = m.getItems();
	const QStringList &columns = m.getColumns();

	((c_tablebox*)_widget)->clearall();

	int rowCount = items.count();
	int columnCount = columns.count();
	if(columnCount != 0)
		rowCount /= columnCount;

	for(int i = 0; i < columnCount; i ++)
		((c_tablebox*)_widget)->add_column(fromQString(columns[i]), i);

	for(int i = 0; i < rowCount; i ++)
	{
		((c_tablebox*)_widget)->add_row(i);
		for(int j = 0; j < columnCount; j ++)
		{
			((c_tablebox*)_widget)->set_cell_text(i, j, fromQString(items[i*columnCount + j]));
		}
	}
}
void UiTableBox::setRowSeparator(bool sep)
{
	((c_tablebox*)_widget)->set_separator(sep , ((c_tablebox*)_widget)->is_col_separator());
}
bool UiTableBox::getRowSeparator() const
{
	return ((c_tablebox*)_widget)->is_row_separator();
}
void UiTableBox::setColSeparator(bool sep)
{
	((c_tablebox*)_widget)->set_separator(((c_tablebox*)_widget)->is_row_separator() ,sep);
}
bool UiTableBox::getColSeparator() const
{
	return ((c_tablebox*)_widget)->is_col_separator();
}
QColor UiTableBox::getTextColor() const
{
	c_color color(((c_tablebox*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}
void UiTableBox::setTextColor(const QColor &color)
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_tablebox*)_widget)->set_text_color(clr);
}


SkinImage UiTableBox::getStencil()
{
	return _stencilImg;
}
void UiTableBox::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_tablebox*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}


SkinImage UiTableBox::getOdd()
{
	return _OddImg;
}
void UiTableBox::setOdd(const SkinImage &stencilImg)
{
	_OddImg.setData(stencilImg);
	((c_tablebox*)_widget)->set_odd_image(fromQString(stencilImg.getSkinName()));
}
SkinImage UiTableBox::getEven()
{
	return _EvenImg;
}
void UiTableBox::setEven(const SkinImage &stencilImg)
{
	_EvenImg.setData(stencilImg);
	((c_tablebox*)_widget)->set_even_image(fromQString(stencilImg.getSkinName()));
}
int UiTableBox::getRowHeight()
{
	return ((c_tablebox*)_widget)->get_row_height();
}
void UiTableBox::setRowHeight(int h)
{
	((c_tablebox*)_widget)->set_row_height(h);
}