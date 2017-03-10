#include "UiSlider.h"
#include "Slider.h"
#include "ResourceMgr.h"
#include "PropertyEditerDockWidget.h"
#include "PropertyManager.h"
#include "UIMgr.h"
UiSlider::UiSlider( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{

}

void UiSlider::getDefaultSize( int &width, int &height ) const
{
	width = 300;
	height = 40;
}

QDomElement UiSlider::serialize( QDomDocument &doc, const QDomElement &extElement )
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
	extEle.setAttribute("DisableMap", getDisableImage().getSkinName());
	pt = getDisableImage().getSkinSet().getCenterPt().toPoint();
	sz = getDisableImage().getSkinSet().getCenterSize().toSize();
	region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("DisableTexcoord", region);
	extEle.setAttribute("StencilMap", getStencil().getSkinName());
	extEle.setAttribute("SliderCommonMap", getThumbImg().getSkinName());
	extEle.setAttribute("SliderDisableMap", getThumbDisableImg().getSkinName());
	extEle.setAttribute("SliderSize", QString("%1,%2").arg(getThumbSize().width()).arg(getThumbSize().height()));
	extEle.setAttribute("Range", QString("%1,%2").arg(getMinValue()).arg(getMaxValue()));
	extEle.setAttribute("SliderStep", QString("%1").arg(getStep()));
	extEle.setAttribute("SliderPosition", QString("%1").arg(getPosition()));
	if (getHorizontal().orientation == Horizontal)
		extEle.setAttribute("Orientation", "Horizontal");
	else
		extEle.setAttribute("Orientation", "Vertical");
	extEle.setAttribute("Enable", boolToString(getEnable()));
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));



	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Slider");
	return ele;
}

void UiSlider::readExt( QXmlStreamReader &reader )
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

	QString dImg = atts.value(QLatin1String("DisableMap")).toString();
	QStringList dRegion = atts.value(QLatin1String("DisableTexcoord")).toString().split(",");

	QString thumbimage = atts.value(QLatin1String("SliderCommonMap")).toString();
	QString thumbdimage = atts.value(QLatin1String("SliderDisableMap")).toString();
	QStringList thumbsz = atts.value(QLatin1String("SliderSize")).toString().split(",");
	QStringList range = atts.value(QLatin1String("Range")).toString().split(",");
	unsigned int step = atts.value(QLatin1String("SliderStep")).toUInt();
	unsigned int pos = atts.value(QLatin1String("SliderPosition")).toUInt();
	QString horizontal = atts.value(QLatin1String("Orientation")).toString();
	bool enable = StringToBool(atts.value(QLatin1String("Enable")).toString());
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
	setFlipX(FlipX);
	setFlipY(FlipY);

	QStringRef stenciltag = atts.value(QLatin1String("StencilMap"));
	//bReg = atts.value(QLatin1String("StencilTexcoord")).toString().split(",");
	setStencil(createSkinImage(stenciltag.toString()));

	setBackImg(createSkinImage(bImg, bReg));
	setThumbImg(createSkinImage(thumbimage));
	setThumbDisableImg(createSkinImage(thumbdimage));
	setDisableImage(createSkinImage(dImg, dRegion));

	if(thumbsz.size() == 2)
	{
		setThumbSize(QSize(thumbsz[0].toUInt(), thumbsz[1].toUInt()));
	}

	if(range.size() == 2)
	{
		setMinValue(range[0].toUInt());
		setMaxValue(range[1].toUInt());
	}

	setStep(step);
	if (horizontal == "Horizontal")
		setHorizontal(ROrientation(Horizontal));
	else
		setHorizontal(ROrientation(Vertical));
	setPosition(pos);
	setEnable(enable);
}

//////////////////////////////////////////////////////////////////////////

SkinImage UiSlider::getStencil()
{
	//assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _stencilImg;
}

void UiSlider::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_slider*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
	//SkinSet skinSet = stencilImg.getSkinSet();
	//_stencilImg.setData(stencilImg);
	//((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}
bool UiSlider::getEnable() const
{
	return ((c_slider*)_widget)->is_enable();
}

void UiSlider::setEnable( bool enable )
{
	((c_slider*)_widget)->set_enable(enable);
}

ROrientation UiSlider::getHorizontal() const
{
	if (((c_slider*)_widget)->is_horizontal())
	{
		return ROrientation(Horizontal);
	}
	else
	{
		return ROrientation(Vertical);
	}

}

void UiSlider::setHorizontal(ROrientation horizontal )
{
	if (horizontal.orientation ==  Horizontal)
		((c_slider*)_widget)->set_horizontal(true);
	else
		((c_slider*)_widget)->set_horizontal(false);
}

SkinImageEx UiSlider::getBackImg() const
{
	//assert(((c_slider*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _backImage;
}

void UiSlider::setBackImg( const SkinImageEx &image )
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
	((c_slider*)_widget)->set_bg_image(fromQString(_backImage.getSkinName()));
	((c_slider*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}
SkinImageEx UiSlider::getDisableImage() const
{
	return _disableImage;
}
void UiSlider::setDisableImage(const SkinImageEx &image)
{
	SkinSet skinSet = image.getSkinSet();
	_disableImage.setData(image);
	((c_slider*)_widget)->set_disable_image(fromQString(_disableImage.getSkinName()));
	((c_slider*)_widget)->set_dis_fragment(c_float2(skinSet.getCenterPt().x(), skinSet.getCenterPt().y()),
		c_float2(skinSet.getCenterSize().width(), skinSet.getCenterSize().height()));
}
SkinImage UiSlider::getThumbImg() const
{
	assert(((c_slider*)_widget)->get_thumb_image() == fromQString(_thumbImage.getSkinName()));
	return _thumbImage;
}

void UiSlider::setThumbImg( const SkinImage &image )
{
	_thumbImage.setData(image);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("SliderSize"));
	if (perty && _tmpSkin != _thumbImage.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_thumbImage.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QSize rect(trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("SliderSize", rect);
			_tmpSkin = _thumbImage.getSkinName();
		}
	}

	((c_slider*)_widget)->set_thumb_image(fromQString(_thumbImage.getSkinName()));
}

SkinImage UiSlider::getThumbDisableImg() const
{
	assert(((c_slider*)_widget)->get_thumb_disable_image() == fromQString(_thumbDisableImage.getSkinName()));
	return _thumbDisableImage;
}

void UiSlider::setThumbDisableImg( const SkinImage &image )
{
	_thumbDisableImage.setData(image);
	((c_slider*)_widget)->set_thumb_disable_image(fromQString(_thumbDisableImage.getSkinName()));
}

void UiSlider::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_backImage.setSkinFile(skinFile);
	//_thumbImage.setSkinFile(skinFile);
	//_thumbDisableImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_backImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("thumbImage"), QVariant::fromValue<SkinImageEx>(_thumbImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("thumbDisableImage"), QVariant::fromValue<SkinImageEx>(_thumbDisableImage));
}

int UiSlider::getMaxValue() const
{
	return ((c_slider*)_widget)->get_max_value();
}

void UiSlider::setMaxValue( int v )
{
	int minV = ((c_slider*)_widget)->get_min_value();
	((c_slider*)_widget)->set_range(c_float2(minV, v));
}

int UiSlider::getMinValue() const
{
	return ((c_slider*)_widget)->get_min_value();
}

void UiSlider::setMinValue( int v )
{
	int maxV = ((c_slider*)_widget)->get_max_value();
	((c_slider*)_widget)->set_range(c_float2(v, maxV));
}

int UiSlider::getPosition() const
{
	return ((c_slider*)_widget)->get_position();
}

void UiSlider::setPosition( int pos )
{
	((c_slider*)_widget)->set_position(pos);
}

int UiSlider::getStep() const
{
	return ((c_slider*)_widget)->get_step();
}

void UiSlider::setStep( int setp )
{
	((c_slider*)_widget)->set_step(setp);
}

QSize UiSlider::getThumbSize() const
{
	const c_float2 &sz = ((c_slider*)_widget)->get_thumb_size();
	return QSize(sz.x(), sz.y());
}

void UiSlider::setThumbSize( const QSize &thumbSize )
{
	assert(!thumbSize.isNull());
	c_float2 sz(thumbSize.width(), thumbSize.height());
	((c_slider*)_widget)->set_thumb_size(sz);
}