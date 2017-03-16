#include "UiEditBox.h"
#include "EditBox.h"
#include "PropertyEditerDockWidget.h"
#include "PropertyManager.h"
#include "UIMgr.h"
#include "ResourceMgr.h"

UiEditBox::UiEditBox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
	_txtColor = 0xFFFFFFFF;
}

void UiEditBox::getDefaultSize( int &width, int &height ) const
{
	width = 200;
	height = 40;
}

QDomElement UiEditBox::serialize( QDomDocument &doc, const QDomElement &extElement )
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
	extEle.setAttribute("StencilMap", getStencil().getSkinName());
	extEle.setAttribute("Enable", boolToString(getEnable()));
	//extEle.setAttribute("Wordwrap", boolToString(getWordWrap()));
	extEle.setAttribute("Numeric", boolToString(getNumeric().getEnable()));
	extEle.setAttribute("Password", boolToString(getPassword()));
	extEle.setAttribute("Autoscroll", boolToString(getAutoScroll()));
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));
	extEle.setAttribute("Multiline", boolToString(getMultiLine()));	
	QString margin = QString("%1,%2").arg(getMargin().width()).arg(getMargin().height());
	extEle.setAttribute("Padding", margin);
	QString range = QString("%1,%2").arg(getNumeric().getMin()).arg(getNumeric().getMax());
	extEle.setAttribute("NumericRange", range);
	extEle.setAttribute("MaxLength", getMaxC());
	extEle.setAttribute("Placeholder", getText());
	extEle.setAttribute("TextColor",fromQColor(getTextColor()).rgba());
	extEle.setAttribute("PlaceholderColor", fromQColor(getPlaceholderColor()).rgba());
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
	ele.setAttribute("Type", "Text");
	return ele;
}

void UiEditBox::readExt( QXmlStreamReader &reader )
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
	bool enable = StringToBool(atts.value("Enable").toString());
	//bool wordwrap = StringToBool(atts.value("Wordwrap").toString());
	bool numeric = StringToBool(atts.value("Numeric").toString());
	bool password = StringToBool(atts.value("Password").toString());
	bool autoscroll = StringToBool(atts.value("Autoscroll").toString());
	bool multiline = StringToBool(atts.value("Multiline").toString());
	QStringList margin = atts.value("Padding").toString().split(",");
	QStringList range = atts.value("NumericRange").toString().split(",");
	unsigned int maxc = atts.value("MaxLength").toUInt();
	QString placeholder = atts.value("Placeholder").toString();
	unsigned int txtcolor = atts.value("TextColor").toUInt();
	unsigned int phcolor = atts.value("PlaceholderColor").toUInt();
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
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
	setFlipX(FlipX);
	setFlipY(FlipY);

	int fntH = atts.value(QLatin1String("FontSize")).toString().toInt();
	QStringRef fontsrctag = atts.value(QLatin1String("FontSrc"));

	bool outline = StringToBool(atts.value(QLatin1String("FontOutline")).toString());
	bool intly = StringToBool(atts.value(QLatin1String("FontItalics")).toString());
	bool bold = StringToBool(atts.value(QLatin1String("FontBold")).toString());
	bool shadow = StringToBool(atts.value(QLatin1String("FontShadow")).toString());

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

	setBackImg(createSkinImage(bImg, bReg));
	setEnable(enable);
	//setWordWrap(wordwrap);
	setPassword(password);
	setAutoScroll(autoscroll);
	setMultiLine(multiline);
	setMaxC(maxc);
	setTextColor(fromCColor(c_color(txtcolor)));
	setPlaceholderColor(fromCColor(c_color(phcolor)));
	setTextAlignment(TextAlign(HTextAlignment(hAlign), VTextAlignment(vAlign)));
	
	QSize msz;
	if(margin.size() == 2)
	{
		msz.setWidth(margin[0].toUInt());
		msz.setHeight(margin[1].toUInt());
	}
	setMargin(msz);

	NumericSet ns;
	ns.setEnable(numeric);
	if(range.size() == 2)
	{
		ns.setMin(range[0].toUInt());
		ns.setMax(range[1].toUInt());
	}
	setNumeric(ns);
	setText(placeholder);
}

//////////////////////////////////////////////////////////////////////////
bool UiEditBox::getEnable() const
{
	return ((c_editbox*)_widget)->is_enable();
}

void UiEditBox::setEnable( bool enable )
{
	((c_editbox*)_widget)->set_enable(enable);
}

bool UiEditBox::getAutoScroll() const
{
	return ((c_editbox*)_widget)->is_autoscroll();
}

void UiEditBox::setAutoScroll( bool autoScroll )
{
	((c_editbox*)_widget)->set_autoscroll(autoScroll);
}

bool UiEditBox::getWordWrap() const
{
	return ((c_editbox*)_widget)->is_wordwrap();
}

void UiEditBox::setWordWrap( bool wordWrap )
{
	if (wordWrap)
	{
		((c_editbox*)_widget)->set_password_box(false);
		((c_editbox*)_widget)->set_numeric_box(false);
		PropertyEditerDockWidget::getInstance()->setPropertyXX("Password", false);
		NumericSet numSet;
		numSet.setEnable(false);
		numSet.setMax(((c_editbox*)_widget)->get_max_value());
		numSet.setMin(((c_editbox*)_widget)->get_min_value());
		PropertyEditerDockWidget::getInstance()->setPropertyNumic("Numeric", numSet);
	}
	((c_editbox*)_widget)->set_wordwrap(wordWrap);
}

bool UiEditBox::getMultiLine() const
{
	return ((c_editbox*)_widget)->is_multiline();
}

void UiEditBox::setMultiLine( bool multiLine )
{
	if (multiLine)
	{
		((c_editbox*)_widget)->set_password_box(false);
		((c_editbox*)_widget)->set_numeric_box(false);
		PropertyEditerDockWidget::getInstance()->setPropertyXX("Password", false);
		NumericSet numSet;
		numSet.setEnable(false);
		numSet.setMax(((c_editbox*)_widget)->get_max_value());
		numSet.setMin(((c_editbox*)_widget)->get_min_value());
		PropertyEditerDockWidget::getInstance()->setPropertyNumic("Numeric", numSet);
	}
	((c_editbox*)_widget)->set_multiline(multiLine);
}

bool UiEditBox::getPassword() const
{
	return ((c_editbox*)_widget)->is_password_box();
}

void UiEditBox::setPassword( bool password )
{
	if (password)
	{
		((c_editbox*)_widget)->set_wordwrap(false);
		((c_editbox*)_widget)->set_multiline(false);
		PropertyEditerDockWidget::getInstance()->setPropertyXX("WordWrap", false);
		PropertyEditerDockWidget::getInstance()->setPropertyXX("MultiLine", false);
	}
	((c_editbox*)_widget)->set_password_box(password);
}

NumericSet UiEditBox::getNumeric() const
{
	NumericSet numSet;
	numSet.setEnable(((c_editbox*)_widget)->is_numeric_box());
	numSet.setMax(((c_editbox*)_widget)->get_max_value());
	numSet.setMin(((c_editbox*)_widget)->get_min_value());
	return numSet;
}

void UiEditBox::setNumeric( const NumericSet &numeric )
{
	if (numeric.getEnable())
	{
		((c_editbox*)_widget)->set_wordwrap(false);
		((c_editbox*)_widget)->set_multiline(false);
		PropertyEditerDockWidget::getInstance()->setPropertyXX("WordWrap", false);
		PropertyEditerDockWidget::getInstance()->setPropertyXX("MultiLine", false);
	}
	((c_editbox*)_widget)->set_numeric_box(numeric.getEnable());
	((c_editbox*)_widget)->set_range(c_float2(numeric.getMin(), numeric.getMax()));
}

SkinImageEx UiEditBox::getBackImg() const
{
	assert(((c_editbox*)_widget)->get_bg_image() == fromQString(_skinImage.getSkinName()));
	return _skinImage;
}

void UiEditBox::setBackImg( const SkinImageEx &image )
{
	SkinSet skinSet = image.getSkinSet();
	_skinImage.setData(image);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("Geometry"));
	if (perty && _tmpSkin != image.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_skinImage.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QRect rect(rc.x(), rc.y(), trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("Geometry", rect);
			_tmpSkin = image.getSkinName();
		}
	}
	((c_editbox*)_widget)->set_bg_image(fromQString(_skinImage.getSkinName()));
	((c_editbox*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

void UiEditBox::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_skinImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_skinImage));
}

QString UiEditBox::getText() const
{
	return fromCString(((c_editbox*)_widget)->get_text());
}

void UiEditBox::setText( const QString &text )
{
	((c_editbox*)_widget)->set_text(fromQString(text));
}

TextAlign UiEditBox::getTextAlignment() const
{
	TextAlign align;
	align.horizontal = (UiWidget::HTextAlignment)((c_editbox*)_widget)->get_text_halign();
	align.vertical = (UiWidget::VTextAlignment)((c_editbox*)_widget)->get_text_valign();
	return align;
}

void UiEditBox::setTextAlignment( const TextAlign &align )
{
	((c_editbox*)_widget)->set_text_align((e_text_alignment)align.horizontal, (e_text_alignment)align.vertical);
}

QColor UiEditBox::getTextColor() const
{
	c_color color(_txtColor);
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiEditBox::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	_txtColor = clr.rgba();
}

QColor UiEditBox::getPlaceholderColor() const
{
	c_color color(((c_editbox*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}
void UiEditBox::setPlaceholderColor(const QColor &color)
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_editbox*)_widget)->set_text_color(clr);
}

QSize UiEditBox::getMargin() const
{
	c_float2 margin = ((c_editbox*)_widget)->get_margin();
	return QSize(margin.x(), margin.y());
}

void UiEditBox::setMargin( const QSize &margin )
{
	((c_editbox*)_widget)->set_margin(c_float2(margin.width(), margin.height()));
}

int UiEditBox::getMaxC() const
{
	return ((c_editbox*)_widget)->get_max();
}

void UiEditBox::setMaxC( int maxC )
{
	((c_editbox*)_widget)->set_max(maxC);
}

SkinImage UiEditBox::getStencil()
{
	//assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _stencilImg;
}

void UiEditBox::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_editbox*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
	//SkinSet skinSet = stencilImg.getSkinSet();
	//_stencilImg.setData(stencilImg);
	//((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}