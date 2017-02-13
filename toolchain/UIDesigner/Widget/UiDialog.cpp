#include "UiDialog.h"
#include "Dialog.h"
#include "UiButton.h"
#include "PropertyEditerDockWidget.h"
#include "ResourceMgr.h"
#include "UIMgr.h"

Q_INVOKABLE UiDialog::UiDialog( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
	_BasePlate = false;
	_dragable = false;
	_Modal = false;
	_Scrollable = false;
}

void UiDialog::getDefaultSize( int &width, int &height ) const
{
	width = 260;
	height = 260;
}

QDomElement UiDialog::serialize( QDomDocument &doc, const QDomElement &extElement )
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
	extEle.setAttribute("StencilMap", getStencil().getSkinName());
	QPoint pt = getBackImg().getSkinSet().getCenterPt().toPoint();
	QSize sz = getBackImg().getSkinSet().getCenterSize().toSize();
	QString region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("CommonTexcoord", region);

	//pt = getStencil().getCenterPt().toPoint();
	//sz = getStencil().getCenterSize().toSize();
	//region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	//extEle.setAttribute("StencilTexcoord", region);

	extEle.setAttribute("Dragable", boolToString(getDragable()));
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));
	extEle.setAttribute("Modal", boolToString(getModal()));
	extEle.setAttribute("Scrollable", boolToString(getScrollable()));
	extEle.setAttribute("BasePlate", boolToString(getBasePlate()));


	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Panel");
	return ele;
}

void UiDialog::readExt( QXmlStreamReader &reader )
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
	QStringRef tag = atts.value(QLatin1String("CommonMap"));
	QStringList bReg = atts.value(QLatin1String("CommonTexcoord")).toString().split(",");
	setBackImg(createSkinImage(tag.toString(), bReg));
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
	setFlipX(FlipX);
	setFlipY(FlipY);


	tag = atts.value(QLatin1String("StencilMap"));
	//bReg = atts.value(QLatin1String("StencilTexcoord")).toString().split(",");
	setStencil(createSkinImage(tag.toString()));


	bool Dragable = StringToBool(atts.value(QLatin1String("Dragable")).toString());
	bool Modal = StringToBool(atts.value(QLatin1String("Modal")).toString());
	bool Scrollable = StringToBool(atts.value(QLatin1String("Scrollable")).toString());
	bool baseplate = StringToBool(atts.value(QLatin1String("BasePlate")).toString());
	setDragable(Dragable);
	setModal(Modal);
	setScrollable(Scrollable);
	setBasePlate(baseplate);

}

//////////////////////////////////////////////////////////////////////////

SkinImageEx UiDialog::getBackImg()
{
	assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _backImage;
}

void UiDialog::setBackImg( const SkinImageEx &backImg )
{
	SkinSet skinSet = backImg.getSkinSet();
	_backImage.setData(backImg);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("Geometry"));
	if (perty && _tmpSkin != backImg.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_backImage.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QRect rect(rc.x(), rc.y(), trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("Geometry", rect);
			_tmpSkin = backImg.getSkinName();
		}
	}
	((c_dialog*)_widget)->set_bg_image(fromQString(_backImage.getSkinName()));
	((c_dialog*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImage UiDialog::getStencil()
{
	//assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _stencilImg;
}

void UiDialog::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
	//SkinSet skinSet = stencilImg.getSkinSet();
	//_stencilImg.setData(stencilImg);
	//((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}

void UiDialog::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_backImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_backImage));
}

bool UiDialog::getDragable() const
{
	return _dragable;
}
void UiDialog::setDragable(bool tran)
{
	_dragable = tran;
}

bool UiDialog::getModal() const
{
	return _Modal;
}
void UiDialog::setModal(bool tran)
{
	_Modal = tran;
}
bool UiDialog::getScrollable() const
{
	return _Scrollable;
}
void UiDialog::setScrollable(bool tran)
{
	_Scrollable = tran;
}


bool UiDialog::getBasePlate() const
{
	return _BasePlate;
}
void UiDialog::setBasePlate(bool flag)
{
	_BasePlate = flag;
}