#include "UiDDSlot.h"
#include "DragDropSlot.h"

UiDDSlot::UiDDSlot( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
}

void UiDDSlot::getDefaultSize( int &width, int &height ) const
{
	width = 48;
	height = 48;
}


QDomElement UiDDSlot::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("ext");
	extEle.setAttribute("bgimage", getBackImg().getSkinName());
	extEle.setAttribute("txtcolor",fromQColor(getTextColor()).rgba());
	TextAlign align = getTextAlignment();
	QString vAlign, hAlign;
	if(align.horizontal == H_Left) hAlign = "left";
	else if(align.horizontal == H_Right) hAlign = "right";
	else if(align.horizontal == H_Center) hAlign = "center";
	if(align.vertical == V_Top) vAlign = "top";
	else if(align.vertical == V_Center) vAlign = "middle";
	else if(align.vertical == V_Bottom) vAlign = "bottom";
	extEle.setAttribute("valign", vAlign);
	extEle.setAttribute("halign", hAlign);

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("class", "dds");
	return ele;
}

void UiDDSlot::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	QString bImg = atts.value(QLatin1String("bgimage")).toString();
	unsigned int txtcolor = atts.value(QLatin1String("txtcolor")).toUInt();
	unsigned int valign = atts.value(QLatin1String("valign")).toUInt();
	unsigned int halign = atts.value(QLatin1String("halign")).toUInt();

	setBackImg(createSkinImage(bImg));
	setTextColor(fromCColor(c_color(txtcolor)));
	setTextAlignment(TextAlign(HTextAlignment(halign), VTextAlignment(valign)));
}

QString UiDDSlot::getText() const
{
	return fromCString(((c_dragdrop_slot*)_widget)->get_text());
}

void UiDDSlot::setText( const QString &text )
{
	((c_dragdrop_slot*)_widget)->set_text(fromQString(text));
}

TextAlign UiDDSlot::getTextAlignment() const
{
	TextAlign align;
	align.horizontal = (UiWidget::HTextAlignment)((c_dragdrop_slot*)_widget)->get_text_halign();
	align.vertical = (UiWidget::VTextAlignment)((c_dragdrop_slot*)_widget)->get_text_valign();
	return align;
}

void UiDDSlot::setTextAlignment( const TextAlign &align )
{
	((c_dragdrop_slot*)_widget)->set_text_align((e_text_alignment)align.horizontal, (e_text_alignment)align.vertical);
}

QColor UiDDSlot::getTextColor() const
{
	c_color color(((c_dragdrop_slot*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiDDSlot::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_dragdrop_slot*)_widget)->set_text_color(clr);
}

SkinImage UiDDSlot::getBackImg() const
{
	assert(((c_dragdrop_slot*)_widget)->get_bg_image() == fromQString(_image.getSkinName()));
	return _image;
}

void UiDDSlot::setBackImg( const SkinImage &backImg )
{
	_image.setData(backImg);
	((c_dragdrop_slot*)_widget)->set_bg_image(fromQString(_image.getSkinName()));
}
