#include "UiTreeBox.h"
#include "TreeBox.h"



UiTreebox::UiTreebox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
}

void UiTreebox::getDefaultSize( int &width, int &height ) const
{
	width = 200;
	height = 200;
}

QDomElement UiTreebox::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("ext");
	extEle.setAttribute("bgimage", getBackImg().getSkinName());
	QPoint pt = getBackImg().getSkinSet().getCenterPt().toPoint();
	QSize sz = getBackImg().getSkinSet().getCenterSize().toSize();
	QString region = QString("%1,%2,%3,%4").arg(pt.x()).arg(pt.y()).arg(sz.width()).arg(sz.height());
	extEle.setAttribute("bgzoomreg", region);
	extEle.setAttribute("txtcolor",fromQColor(getTextColor()).rgba());
	extEle.setAttribute("drawl",boolToString(getDrawLines()));
	extEle.setAttribute("xoffset",QString("%1").arg(getXOffset()));


	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("class", "trb");
	return ele;
}

void UiTreebox::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	QString bImg = atts.value(QLatin1String("bgimage")).toString();
	QStringList bReg = atts.value(QLatin1String("bgzoomreg")).toString().split(",");
	unsigned int texClr = atts.value(QLatin1String("txtcolor")).toUInt();
	bool drawl = StringToBool(atts.value(QLatin1String("drawl")).toString());
	int xoffset = atts.value(QLatin1String("xoffset")).toUInt();

	setBackImg(createSkinImage(bImg, bReg));
	setTextColor(fromCColor(c_color(texClr)));
	setDrawLines(drawl);
	setXOffset(xoffset);
}

//////////////////////////////////////////////////////////////////////////

SkinImageEx UiTreebox::getBackImg() const
{
	return _skinImage;
}

void UiTreebox::setBackImg( const SkinImageEx &backImg )
{
	SkinSet skinSet = backImg.getSkinSet();
	_skinImage.setData(backImg);
	((c_treebox*)_widget)->set_bg_image(fromQString(_skinImage.getSkinName()));
	((c_treebox*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

bool UiTreebox::getDrawLines() const
{
	return ((c_treebox*)_widget)->is_lines_visible();
}
void UiTreebox::setDrawLines(bool draw)
{
	((c_treebox*)_widget)->set_lines_visible(draw);
}

int UiTreebox::getXOffset() const
{
	return ((c_treebox*)_widget)->get_xoffset();
}
void UiTreebox::setXOffset(int off)
{
	((c_treebox*)_widget)->set_xoffset(off);
}
void UiTreebox::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_skinImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_skinImage));
}

QColor UiTreebox::getTextColor() const
{
	c_color color(((c_treebox*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiTreebox::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_treebox*)_widget)->set_text_color(clr);
}