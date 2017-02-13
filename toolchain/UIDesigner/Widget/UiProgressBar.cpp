#include "UiProgressBar.h"
#include "ProgressBar.h"
#include "PropertyEditerDockWidget.h"
#include "ResourceMgr.h"
#include "UIMgr.h"


UiProgressBar::UiProgressBar( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{

}

void UiProgressBar::getDefaultSize( int &width, int &height ) const
{
	width = 300;
	height = 40;
}

QDomElement UiProgressBar::serialize( QDomDocument &doc, const QDomElement &extElement )
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
	extEle.setAttribute("FillMap" , getFillImg().getSkinName());
	extEle.setAttribute("Percent" , QString("%1").arg(getPosition()));
	extEle.setAttribute("Border" , QString("%1,%2").arg(getOffset().x()).arg(getOffset().y()));
	extEle.setAttribute("Text" , getText());
	extEle.setAttribute("TextColor",fromQColor(getTextColor()).rgba());
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));


	c_string tmpd = fromQString(getFont().getFontFile().getFileName());
	c_vector<c_string> tmp2;
	c_vector<c_string> tmp3;
	tmpd.tokenize(L"/", tmp2);
	tmp2.back().tokenize(L".", tmp3);

	extEle.setAttribute("FontSrc", QString((const char*)tmp3.front().to_utf8()));
	extEle.setAttribute("FontSize",getFont().getFontSize());
	extEle.setAttribute("FontOutline", boolToString(getFont().getFontOutline()));
	extEle.setAttribute("FontItalics", boolToString(getFont().getFontItalics()));
	extEle.setAttribute("FontShadow", boolToString(getFont().getFontShadow()));
	extEle.setAttribute("FontBold", boolToString(getFont().getFontBold()));

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Progress");
	return ele;
}

void UiProgressBar::readExt( QXmlStreamReader &reader )
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
	QString fImg = atts.value(QLatin1String("FillMap")).toString();
	QString per = atts.value(QLatin1String("Percent")).toString();
	QStringList offset = atts.value(QLatin1String("Border")).toString().split(",");
	QString text = atts.value(QLatin1String("Text")).toString();
	unsigned int txtcolor = atts.value(QLatin1String("TextColor")).toUInt();
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
	setFlipX(FlipX);
	setFlipY(FlipY);

	QStringRef stenciltag = atts.value(QLatin1String("StencilMap"));
	//bReg = atts.value(QLatin1String("StencilTexcoord")).toString().split(",");
	setStencil(createSkinImage(stenciltag.toString()));
	setBackImg(createSkinImage(bImg, bReg));
	setFillImg(createSkinImage(fImg));
	setPosition(per);

	//font
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

	setText(text);
	setTextColor(fromCColor(c_color(txtcolor)));

	if(offset.size() == 2)
	{
		setOffset(QPointF(offset[0].toDouble(), offset[1].toDouble()));
	}
}

//////////////////////////////////////////////////////////////////////////

QString UiProgressBar::getPosition() const
{
	return QString("%1").arg(((c_progressbar*)_widget)->get_percent());
}

void UiProgressBar::setPosition( const QString &postion )
{
	int per = postion.toInt();
	if(per < 0)
		per = 0;
	else if(per > 99)
		per = 99;
	((c_progressbar*)_widget)->set_percent(per);
}

SkinImageEx UiProgressBar::getBackImg() const
{
	assert(((c_progressbar*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _backImage;
}

void UiProgressBar::setBackImg( const SkinImageEx &image )
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
	((c_progressbar*)_widget)->set_bg_image(fromQString(_backImage.getSkinName()));
	((c_progressbar*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

SkinImage UiProgressBar::getFillImg() const
{
	assert(((c_progressbar*)_widget)->get_fill_image() == fromQString(_fillImage.getSkinName()));
	return _fillImage;
}

void UiProgressBar::setFillImg( const SkinImage &image )
{
	_fillImage.setData(image);
	((c_progressbar*)_widget)->set_fill_image(fromQString(_fillImage.getSkinName()));
}

void UiProgressBar::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_backImage.setSkinFile(skinFile);
	//_fillImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_backImage));
	//emitUserEnumNamesChanged(QString::fromLatin1("fillImage"), QVariant::fromValue<SkinImageEx>(_fillImage));
}

QString UiProgressBar::getText() const
{
	return fromCString(((c_progressbar*)_widget)->get_text());
}

void UiProgressBar::setText( const QString &text )
{
	((c_progressbar*)_widget)->set_text(fromQString(text));
}

QColor UiProgressBar::getTextColor() const
{
	c_color color(((c_progressbar*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiProgressBar::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_progressbar*)_widget)->set_text_color(clr);
}

QPointF UiProgressBar::getOffset() const
{
	c_float2 offset = ((c_progressbar*)_widget)->get_offset();
	return QPointF(offset.x(), offset.y());
}

void UiProgressBar::setOffset( const QPointF &val )
{
	((c_progressbar*)_widget)->set_offset(c_float2(val.x(), val.y()));
}




SkinImage UiProgressBar::getStencil()
{
	//assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _stencilImg;
}

void UiProgressBar::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_progressbar*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
	//SkinSet skinSet = stencilImg.getSkinSet();
	//_stencilImg.setData(stencilImg);
	//((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}