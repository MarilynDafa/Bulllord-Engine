#include "UiButton.h"
#include "Button.h"
#include <QXmlStreamAttribute>
#include "PropertyEditerDockWidget.h"
#include "ResourceMgr.h"
#include "UIMgr.h"
#include <QXmlStreamReader>

UiButton::UiButton( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{

}

void UiButton::getDefaultSize( int &width, int &height ) const
{
	width = 200;
	height = 40;
}

QDomElement UiButton::serialize( QDomDocument &doc, const QDomElement &extElement )
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

	extEle.setAttribute("CommonMap", getNormalImage().getSkinName());
	QPoint pt = getNormalImage().getSkinSet().getCenterPt().toPoint();
	QSize sz = getNormalImage().getSkinSet().getCenterSize().toSize();
	QString region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("CommonTexcoord", region);
	extEle.setAttribute("StencilMap", getStencil().getSkinName());
	extEle.setAttribute("HoveredMap", getHoveredImage().getSkinName());
	pt = getHoveredImage().getSkinSet().getCenterPt().toPoint();
	sz = getHoveredImage().getSkinSet().getCenterSize().toSize();
	region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("HoveredTexcoord", region);
	extEle.setAttribute("PressedMap", getPressedImage().getSkinName());
	pt = getPressedImage().getSkinSet().getCenterPt().toPoint();
	sz = getPressedImage().getSkinSet().getCenterSize().toSize();
	region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("PressedTexcoord", region);
	extEle.setAttribute("DisableMap", getDisableImage().getSkinName());
	pt = getDisableImage().getSkinSet().getCenterPt().toPoint();
	sz = getDisableImage().getSkinSet().getCenterSize().toSize();
	region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("DisableTexcoord", region);
	extEle.setAttribute("Enable", boolToString(getEnable()));
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));
	extEle.setAttribute("Text" , getText());
	extEle.setAttribute("TextColor",fromQColor(getTextColor()).rgba());
	TextAlign align = getTextAlignment();
	QString vAlign, hAlign;
	if (align.horizontal == H_Left) hAlign = "Left";
	else if (align.horizontal == H_Right) hAlign = "Right";
	else if (align.horizontal == H_Center) hAlign = "Center";
	if (align.vertical == V_Top) vAlign = "Top";
	else if (align.vertical == V_Center) vAlign = "Middle";
	else if (align.vertical == V_Bottom) vAlign = "Bottom";
	extEle.setAttribute("AlignmentV", vAlign);
	extEle.setAttribute("AlignmentH", hAlign);

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
	ele.setAttribute("Type", "Button");
	return ele;
}

void UiButton::readExt( QXmlStreamReader &reader )
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


	QString nImg = atts.value(QLatin1String("CommonMap")).toString();
	QStringList nRegion = atts.value(QLatin1String("CommonTexcoord")).toString().split(",");
	QStringRef stenciltag = atts.value(QLatin1String("StencilMap"));
	setStencil(createSkinImage(stenciltag.toString()));
	QString hImg = atts.value(QLatin1String("HoveredMap")).toString();
	QStringList hRegion = atts.value(QLatin1String("HoveredTexcoord")).toString().split(",");
	QString pImg = atts.value(QLatin1String("PressedMap")).toString();
	QStringList pRegion = atts.value(QLatin1String("PressedTexcoord")).toString().split(",");
	QString dImg = atts.value(QLatin1String("DisableMap")).toString();
	QStringList dRegion = atts.value(QLatin1String("DisableTexcoord")).toString().split(",");
	bool enable = StringToBool(atts.value(QLatin1String("Enable")).toString());
	QString text = atts.value(QLatin1String("Text")).toString();
	unsigned int txtClr = atts.value(QLatin1String("TextColor")).toUInt();
	HTextAlignment  hAlign;
	QString ha = atts.value(QLatin1String("AlignmentH")).toString();
	VTextAlignment  vAlign;
	QString va = atts.value(QLatin1String("AlignmentV")).toString();
	if (ha == "Left")
		hAlign = H_Left;
	else if (ha == "Right")
		hAlign = H_Right;
	else
		hAlign = H_Center;


	if (va == "Top")
		vAlign = V_Top;
	else if (va == "Middle")
		vAlign = V_Center;
	else
		vAlign = V_Bottom;

	int fntH = atts.value(QLatin1String("FontSize")).toString().toInt();
	QStringRef fontsrctag = atts.value(QLatin1String("FontSrc"));

	bool outline = StringToBool(atts.value(QLatin1String("FontOutline")).toString());
	bool intly = StringToBool(atts.value(QLatin1String("FontItalics")).toString());
	bool bold = StringToBool(atts.value(QLatin1String("FontBold")).toString());
	bool shadow = StringToBool(atts.value(QLatin1String("FontShadow")).toString());
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
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


	setNormalImage(createSkinImage(nImg, nRegion));
	setHoveredImage(createSkinImage(hImg, hRegion));
	setPressedImage(createSkinImage(pImg, pRegion));
	setDisableImage(createSkinImage(dImg, dRegion));
	setEnable(enable);
	setText(text);
	setTextColor(fromCColor(c_color(txtClr)));
	setTextAlignment(TextAlign(HTextAlignment(hAlign), VTextAlignment(vAlign)));
}

//////////////////////////////////////////////////////////////////////////
bool UiButton::getEnable() const
{
	return ((c_button*)_widget)->is_enable();
}

void UiButton::setEnable( bool enable )
{
	((c_button*)_widget)->set_enable(enable);
}

SkinImageEx UiButton::getNormalImage() const
{
	//assert(((c_button*)_widget)->get_normal_image() == fromQString(_normalImage.getSkinName()));
	return _normalImage;
}

void UiButton::setNormalImage( const SkinImageEx &image )
{
	SkinSet skinSet = image.getSkinSet();
	_normalImage.setData(image);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("Geometry"));
	if (perty && _tmpSkin != image.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_normalImage.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QRect rect(rc.x(), rc.y(), trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("Geometry", rect);
			_tmpSkin = image.getSkinName();
		}
	}
	((c_button*)_widget)->set_normal_image(fromQString(_normalImage.getSkinName()));
	((c_button*)_widget)->set_nor_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImageEx UiButton::getPressedImage() const
{
	//assert(((c_button*)_widget)->get_pressed_image() == fromQString(_pressedImage.getSkinName()));
	return _pressedImage;
}

void UiButton::setPressedImage( const SkinImageEx &image )
{
	SkinSet skinSet = image.getSkinSet();
	_pressedImage.setData(image);
	((c_button*)_widget)->set_pressed_image(fromQString(_pressedImage.getSkinName()));
	((c_button*)_widget)->set_pre_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImageEx UiButton::getHoveredImage() const
{
	//assert(((c_button*)_widget)->get_hovered_image() == fromQString(_hoveredImage.getSkinName()));
	return _hoveredImage;
}

void UiButton::setHoveredImage( const SkinImageEx &image )
{
	SkinSet skinSet = image.getSkinSet();
	_hoveredImage.setData(image);
	((c_button*)_widget)->set_hovered_image(fromQString(_hoveredImage.getSkinName()));
	((c_button*)_widget)->set_hov_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImageEx UiButton::getDisableImage() const
{
	//assert(((c_button*)_widget)->get_disable_image() == fromQString(_disableImage.getSkinName()));
	return _disableImage;
}

void UiButton::setDisableImage( const SkinImageEx &image )
{
	SkinSet skinSet = image.getSkinSet();
	_disableImage.setData(image);
	((c_button*)_widget)->set_disable_image(fromQString(_disableImage.getSkinName()));
	((c_button*)_widget)->set_dis_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImage UiButton::getStencil()
{
	//assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _stencilImg;
}

void UiButton::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_button*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
	//SkinSet skinSet = stencilImg.getSkinSet();
	//_stencilImg.setData(stencilImg);
	//((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}
void UiButton::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_normalImage.setSkinFile(skinFile);
	//_pressedImage.setSkinFile(skinFile);
	//_hoveredImage.setSkinFile(skinFile);
	//_disableImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("normalImage"), QVariant::fromValue<SkinImageEx>(_normalImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("pressedImage"), QVariant::fromValue<SkinImageEx>(_pressedImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("hoveredImage"), QVariant::fromValue<SkinImageEx>(_hoveredImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("disableImage"), QVariant::fromValue<SkinImageEx>(_disableImage));
}

QString UiButton::getText() const
{
	return fromCString(((c_button*)_widget)->get_text());
}

void UiButton::setText( const QString &text )
{
	((c_button*)_widget)->set_text(fromQString(text));
}

TextAlign UiButton::getTextAlignment() const
{
	TextAlign align;
	align.horizontal = (UiWidget::HTextAlignment)((c_button*)_widget)->get_text_halign();
	align.vertical = (UiWidget::VTextAlignment)((c_button*)_widget)->get_text_valign();
	return align;
}

void UiButton::setTextAlignment( const TextAlign &align )
{
	((c_button*)_widget)->set_text_align((e_text_alignment)align.horizontal, (e_text_alignment)align.vertical);
}

QColor UiButton::getTextColor() const
{
	c_color color(((c_button*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiButton::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_button*)_widget)->set_text_color(clr);
}

void UiButton::attach( c_widget *widget )
{
	assert(widget->get_class() == WT_BUTTON);
	_widget = widget;
}
