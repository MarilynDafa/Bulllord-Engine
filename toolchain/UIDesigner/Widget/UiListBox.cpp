#include "UiListBox.h"
#include "ListBox.h"


ListBoxEditerDialog::ListBoxEditerDialog( QWidget *parent /*= 0*/ )
	: QDialog(parent)
{
	initUi();
	initSignal();
}

void ListBoxEditerDialog::initUi()
{
	setupUi(this);
}

void ListBoxEditerDialog::initSignal()
{
	connect(_addBtn,SIGNAL(clicked()),this,SLOT(slotAddBtnClicked()));
	connect(_delBtn,SIGNAL(clicked()),this,SLOT(slotDelBtnClicked()));
	//connect(_moveUpBtn,SIGNAL(clicked()),this,SLOT(slotUpBtnClicked()));
	//connect(_moveDownBtn,SIGNAL(clicked()),this,SLOT(slotDownBtnClicked()));
	connect(_okBtn,SIGNAL(clicked()),this,SLOT(slotOkBtnClicked()));
}

void ListBoxEditerDialog::setInitItems( QStringList items )
{
	_itemListWidget->clear();
	/*
	QStringListIterator it(items);
	while(it.hasNext())
	{
		QTableWidgetItem *item = new QListWidgetItem(it.next());
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		_itemListWidget->addItem(item);
		_itemListWidget->setCurrentItem(item);
	}*/
}

void ListBoxEditerDialog::slotAddBtnClicked()
{
//	QListWidgetItem *item = new QListWidgetItem("new item");
	//item->setFlags(item->flags() | Qt::ItemIsEditable);

	//int row = _itemListWidget->currentRow();
	//_itemListWidget->insertItem(row + 1, item);
	//_itemListWidget->setCurrentItem(item);
	//_itemListWidget->editItem(item);
}

void ListBoxEditerDialog::slotDelBtnClicked()
{
//	int row = _itemListWidget->currentRow();
//	QListWidgetItem *item = _itemListWidget->takeItem(row);
//	delete item;
}

void ListBoxEditerDialog::slotUpBtnClicked()
{
}

void ListBoxEditerDialog::slotDownBtnClicked()
{
}

void ListBoxEditerDialog::slotOkBtnClicked()
{
	/*
	_itemList.clear();
	int count = _itemListWidget->count();
	for(int i = 0; i < count; i++)
	{
		QListWidgetItem *item = _itemListWidget->item(i);
		QString text = item->text();
		_itemList.append(text);
	}
	this->accept();*/
}
//////////////////////////////////////////////////////////////////////////

UiListBox::UiListBox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{

}

void UiListBox::getDefaultSize( int &width, int &height ) const
{
	width = 300;
	height = 300;
}

bool UiListBox::handleDoubleClicked( int x, int y )
{
	ListBoxEditerDialog dlg;
	dlg.setInitItems(getItems());
	if(dlg.exec() == QDialog::Accepted)
	{
		QStringList items = dlg.getItems();
		setItems(items);
	}
	return true;
}

QDomElement UiListBox::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("ext");
	extEle.setAttribute("bgimage", getBackImg().getSkinName());
	QPoint pt = getBackImg().getSkinSet().getCenterPt().toPoint();
	QSize sz = getBackImg().getSkinSet().getCenterSize().toSize();
	QString region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("bgzoomreg", region);
	extEle.setAttribute("rowheight", QString("%1").arg(getRowHeight()));
	extEle.setAttribute("listnum" , QString("%1").arg(getItems().size()));
	extEle.setAttribute("txtcolor",fromQColor(getTextColor()).rgba());
	QStringList sl = getItems();
	for(int i = 0 ; i < sl.size() ; ++i)
	{
		QString tag = "ele";
		tag.append(QString("%1").arg(i));
		extEle.setAttribute(tag , sl[i]);
	}

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("class", "lib");
	return ele;
}

void UiListBox::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	QString bImg = atts.value(QLatin1String("bgimage")).toString();
	QStringList bReg = atts.value(QLatin1String("bgzoomreg")).toString().split(",");
	unsigned int rowHeight = atts.value(QLatin1String("rowheight")).toUInt();
	unsigned int listNum = atts.value(QLatin1String("listnum")).toUInt();
	unsigned int txtClr = atts.value(QLatin1String("txtcolor")).toUInt();

	QStringList items;
	for(int i = 0; i < listNum; i ++)
	{
		QString item = atts.value(QString("ele%1").arg(i)).toString();
		items.append(item);
	}

	setBackImg(createSkinImage(bImg, bReg));
	setRowHeight(rowHeight);
	setTextColor(fromCColor(c_color(txtClr)));
	setItems(items);
}

//////////////////////////////////////////////////////////////////////////

int UiListBox::getRowHeight() const
{
	return ((c_listbox*)_widget)->get_row_height();
}

void UiListBox::setRowHeight( int height )
{
	((c_listbox*)_widget)->set_row_height(height);
}

SkinImageEx UiListBox::getBackImg()
{
	assert(((c_listbox*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _backImage;
}

void UiListBox::setBackImg( const SkinImageEx &backImg )
{
	SkinSet skinSet = backImg.getSkinSet();
	_backImage.setData(backImg);
	((c_listbox*)_widget)->set_bg_image(fromQString(_backImage.getSkinName()));
	((c_listbox*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

void UiListBox::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_backImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_backImage));
}

QStringList UiListBox::getItems() const
{
	QStringList items;
	int count = ((c_listbox*)_widget)->get_item_count();
	for(int i = 0; i < count; i ++)
	{
		const c_string &item = ((c_listbox*)_widget)->get_item(i);
		items.append(fromCString(item));
	}
	return items;
}

void UiListBox::setItems( const QStringList items )
{
	((c_listbox*)_widget)->clearall();
	QStringListIterator it(items);
	while(it.hasNext())
		((c_listbox*)_widget)->add_item(fromQString(it.next()));
}

QColor UiListBox::getTextColor() const
{
	c_color color(((c_listbox*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiListBox::setTextColor(const QColor &color)
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_listbox*)_widget)->set_text_color(clr);
}
