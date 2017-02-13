#include "UiTabCtl.h"
#include "TabCtl.h"
#include <QtWidgets/QMenu>


TabCtlEditDialog::TabCtlEditDialog( QWidget *parent /*= 0*/ )
	: QDialog(parent)
{
	initUi();
	initSignal();
}

TabCtlEditDialog::~TabCtlEditDialog()
{
	_priv->preview();

}

void TabCtlEditDialog::initUi()
{
	setupUi(this);

//	_moveUpBtn->setVisible(false);
//	_moveDownBtn->setVisible(false);
}

void TabCtlEditDialog::initSignal()
{
	connect(_addBtn, SIGNAL(pressed()), this, SLOT(slotAddBtnPressed()));
	connect(_delBtn, SIGNAL(pressed()), this, SLOT(slotDelBtnPressed()));
	connect(_okBtn, SIGNAL(pressed()), this, SLOT(slotOkBtnPressed()));
	connect(resetBtn, SIGNAL(pressed()), this, SLOT(slotResetBtnPressed()));
	connect(_itemListWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(slotItemChangedPressed(QTableWidgetItem* )) );
	
	//connect(_itemListWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotItemChanged(QTableWidgetItem*)));
}

void TabCtlEditDialog::setUserData(UiTabCtl* ud)
{
	_itemListWidget->blockSignals(true);
	_priv = ud;
	for (int i = 0; i < _priv->path.size(); ++i)
		_itemListWidget->insertRow(0);
	for (int i = 0 ; i <  _priv->path.size(); ++i)
	{
		c_float2 pt = _priv->path[i]; 
		QTableWidgetItem *newItemx = new QTableWidgetItem(QString::number(pt.x()));
		QTableWidgetItem *newItemy = new QTableWidgetItem(QString::number(pt.y()));
		_itemListWidget->setItem(i, 0, newItemx);
		_itemListWidget->setItem(i, 1, newItemy);

	}
	_cache = _priv->path;
	_itemListWidget->blockSignals(false);
}
void TabCtlEditDialog::slotAddBtnPressed()
{
	addItems(QString("new item"), IT_New);
}

void TabCtlEditDialog::slotDelBtnPressed()
{
	int row = _itemListWidget->currentRow();
	_itemListWidget->removeRow(row);
	_cache.clear();
	for (int i = 0; i < _itemListWidget->rowCount(); ++i)
	{
		QTableWidgetItem* itemx = _itemListWidget->item(i, 0);
		QTableWidgetItem* itemy = _itemListWidget->item(i, 1);
		if (itemx && itemy)
		{
			c_float2 xx(itemx->text().toInt(), itemy->text().toInt());
			_cache.push_back(xx);
		}
	}
	_priv->preview(&_cache);
}



void TabCtlEditDialog::slotOkBtnPressed()
{
	_priv->path = _cache;
	_priv->preview();
	this->accept();
}

void TabCtlEditDialog::slotResetBtnPressed()
{
	_itemListWidget->clear();
	_cache.clear();
	_priv->preview(&_cache);
}

void TabCtlEditDialog::slotItemChangedPressed(QTableWidgetItem* item)
{
	_cache.clear();
	for (int i = 0; i < _itemListWidget->rowCount(); ++i)
	{
		QTableWidgetItem* itemx = _itemListWidget->item(i, 0);
		QTableWidgetItem* itemy = _itemListWidget->item(i, 1);
		if (itemx && itemy)
		{
			c_float2 xx(itemx->text().toInt(), itemy->text().toInt());
			_cache.push_back(xx);
		}
	}
	_priv->path = _cache;
	_priv->preview(&_cache);
}

void TabCtlEditDialog::setInitItems( const QStringList &items )
{
	QListIterator<QString> itemIter(items);
	while(itemIter.hasNext())
	{
		const QString &itemName = itemIter.next();
		addItems(itemName, IT_Def);
	}
}

void TabCtlEditDialog::addItems( const QString &name, ItemType t )
{
	_itemListWidget->insertRow(_itemListWidget->rowCount());
	/*
	QListWidgetItem *item = new QListWidgetItem(name);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	_itemListWidget->addItem(item);

	_items.push_back(ItemData(t, name));*/
}
/*
void TabCtlEditDialog::slotItemChanged( QListWidgetItem *item )
{
	int row = _itemListWidget->row(item);

	int index = 0;
	for(QVector<ItemData>::iterator itemIter = _items.begin(); itemIter != _items.end(); ++itemIter)
	{
		ItemData &data = *itemIter;
		if(data.t == IT_Del)
			continue;

		if(index == row)
		{
			if(data.t == IT_Def)
				data.t = IT_Modify;
			data.name = item->text();
			break;
		}
		index ++;
	}
}*/

//////////////////////////////////////////////////////////////////////////

UiTabCtl::UiTabCtl( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
	_closed = true;
	_fill = true;
	_clr = QColor(128, 128,128,128);
}

int UiTabCtl::getOffset() const
{
	return ((c_tabctl*)_widget)->get_page_offset();
}

void UiTabCtl::setOffset(int height)
{
	((c_tabctl*)_widget)->set_page_offset(height);
}

QColor UiTabCtl::getTextColor() const
{
	c_color color(((c_tabctl*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiTabCtl::setTextColor(const QColor &color)
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_tabctl*)_widget)->set_text_color(clr);
}

SkinImageEx UiTabCtl::getBackImg()const
{
	return _backImage;
}

void UiTabCtl::setBackImg(const SkinImageEx &backImg)
{
	SkinSet skinSet = backImg.getSkinSet();
	_backImage.setData(backImg);
	((c_tabctl*)_widget)->set_bg_image(fromQString(_backImage.getSkinName()));
	((c_tabctl*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImage UiTabCtl::getNormalImg()const
{
	return _nImage;
}

void UiTabCtl::setNormalImg(const SkinImage &backImg)
{
	_nImage.setData(backImg);
	((c_tabctl*)_widget)->set_tab_nimage(fromQString(_nImage.getSkinName()));
}

SkinImage UiTabCtl::getActiveImg()const
{
	return _aImage;
}

void UiTabCtl::setActiveImg(const SkinImage &backImg)
{
	_aImage.setData(backImg);
	((c_tabctl*)_widget)->set_tab_aimage(fromQString(_aImage.getSkinName()));
}

int UiTabCtl::getActivePage() const
{
	return ((c_tabctl*)_widget)->get_active_tab();
}

void UiTabCtl::setActivePage(int page)
{
	((c_tabctl*)_widget)->set_active(page);
}

bool UiTabCtl::getTabup() const
{
	return ((c_tabctl*)_widget)->is_tab_posup();
}

void UiTabCtl::setTabup(bool wordWrap)
{
	((c_tabctl*)_widget)->set_tab_posup(wordWrap);
}


bool UiTabCtl::getFill() const
{
	return _fill;
}
void UiTabCtl::setFill(bool flag)
{
	_fill = flag;
	preview();
}


bool UiTabCtl::getClosed() const
{
	return _closed;
}
void UiTabCtl::setClosed(bool flag)
{
	_closed = flag;
	preview();
}


QColor UiTabCtl::getColor() const
{
	return _clr;
}

void UiTabCtl::setColor(const QColor &color)
{
	_clr = color;
	preview();
}

void UiTabCtl::reshape()
{
	c_tabctl* tab = (c_tabctl*)getWidget();
	tab->clear();
	if (_fill)
	{
		tab->AddPolylineFilled(path, fromQColor(getColor()).rgba(), _closed, _fill);
	}
	else
	{
		tab->AddPolyline(path, fromQColor(getColor()).rgba(), _closed, _fill);
	}
}

void UiTabCtl::getDefaultSize( int &width, int &height ) const
{
	width = 300;
	height = 300;
}

QDomElement UiTabCtl::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("Ext");
	extEle.setAttribute("Fill", boolToString(getFill()));
	extEle.setAttribute("Closed", boolToString(getClosed()));
	extEle.setAttribute("Color", fromQColor(getColor()).rgba());

	QString param;
	for (int i = 0; i < path.size(); ++i)
	{
		param += QString::number(path[i].x());
		param += ",";
		param += QString::number(path[i].y());
		param += ",";
	}
	param.remove(param.length() - 1, 1);
	if (param.length() > 0)
		extEle.setAttribute("Path", param);

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Primitive");
	return ele;
}

void UiTabCtl::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	bool Fill = StringToBool(atts.value(QLatin1String("Fill")).toString());
	bool Closed = StringToBool(atts.value(QLatin1String("Closed")).toString());
	unsigned int texClr = atts.value(QLatin1String("Color")).toUInt();
	QString param = atts.value(QLatin1String("Path")).toString();
	c_string cpram = fromQString(param);
	c_vector<c_string> vecs;
	cpram.tokenize(L",", vecs);

	path.clear();
	for (int i = 0; i < vecs.size(); i+=2)
	{
		float x = vecs[i].to_integer();
		float y = vecs[i + 1].to_integer();
		path.push_back(c_float2(x, y));
	}
	setFill(Fill);
	setClosed(Closed);
	c_color clr(texClr);
	QColor qclr(clr.r() * 255, clr.g() * 255, clr.b() * 255, clr.a() * 255);
	setColor(qclr);
	preview();
}


void UiTabCtl::preview(c_vector<c_float2>* data)
{
	if (data == nullptr)
	{
		c_tabctl* tab = (c_tabctl*)getWidget();
		tab->clear();
		if (_fill)
		{
			tab->AddPolylineFilled(path, fromQColor(getColor()).rgba(), _closed, _fill);
		}
		else
		{
			tab->AddPolyline(path, fromQColor(getColor()).rgba(), _closed, _fill);
		}
	}
	else
	{
		c_tabctl* tab = (c_tabctl*)getWidget();
		tab->clear();
		if (_fill)
		{
			tab->AddPolylineFilled(*data, fromQColor(getColor()).rgba(), _closed, _fill);
		}
		else
		{
			tab->AddPolyline(*data, fromQColor(getColor()).rgba(), _closed, _fill);
		}
	}
}

void UiTabCtl::_create()
{
	c_tabctl* tab= (c_tabctl*)getWidget();
	path.push_back(c_float2(0, -150));
	path.push_back(c_float2(150, 0));
	path.push_back(c_float2(0, 150));
	path.push_back(c_float2(-150, 0));
	tab->AddPolylineFilled(path, 0x88888888, true,true);
	tab->clear();
	tab->AddPolylineFilled(path, 0x88888888, true, true);
}

bool UiTabCtl::handleDoubleClicked( int x, int y )
{
	TabCtlEditDialog dlg;
	dlg.setUserData(this);
	dlg.setInitItems(getItems());
	if(QDialog::Accepted == dlg.exec())
	{
		std::vector<TabCtlEditDialog::ItemData> items = dlg.getItemDatas().toStdVector();
		int index = 0;
		for(std::vector<TabCtlEditDialog::ItemData>::iterator iter = items.begin(); iter != items.end();)
		{
			TabCtlEditDialog::ItemData &data = *iter;
			if(data.t == TabCtlEditDialog::IT_Del)
			{
				items.erase(iter ++);
				((c_tabctl*)_widget)->remove_tab(index);
				continue;
			}
			else if(data.t == TabCtlEditDialog::IT_Modify)
			{
				((c_tabctl*)_widget)->rename_tab(index, fromQString(data.name));
			}
			else if(data.t == TabCtlEditDialog::IT_New)
			{
				((c_tabctl*)_widget)->add_tab(fromQString(data.name));
			}

			index ++;
			++iter;
		}
		return true;
	}
	return false;
}

void UiTabCtl::addChild( UiWidget *child )
{
	assert(child != NULL);
	UiWidget::addChild(child);
	int activity = ((c_tabctl*)_widget)->get_active_tab();
	((c_tabctl*)_widget)->set_belong(activity, child->getWidgetId());
}

bool UiTabCtl::acceptChildern() const
{
	return true;
}

QStringList UiTabCtl::getItems()
{
	int count = ((c_tabctl*)_widget)->get_tab_count();
	QStringList items;
	for(int i = 0; i < count; i ++)
	{
		const c_tabctl::s_tab &tab = ((c_tabctl*)_widget)->get_tab(i);
		items.push_back(fromCString(tab.name));
	}
	return items;
}

//
//bool UiTabCtl::handleRightClicked( int x, int y )
//{
//	QMenu popMenu;
//	popMenu.addAction("append page")->setData(QVariant(0));
//	popMenu.addAction("remove page")->setData(QVariant(1));
//	QAction *action = popMenu.exec(QPoint(x, y));
//	if(action != NULL)
//	{
//		unsigned int id = action->data().toUInt();
//		if(id == 0)
//		{
//			((c_tabctl*)_widget)->add_tab(L"fuck");
//		}
//		else if(id == 1)
//		{
//			((c_tabctl*)_widget)->remove_tab(((c_tabctl*)_widget)->get_active_tab());
//		}
//	}
//	return true;
//}

