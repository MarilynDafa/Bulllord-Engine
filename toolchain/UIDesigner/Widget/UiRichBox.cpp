#include "UiRichBox.h"
#include "RichBox.h"
#include "PropertyEditerDockWidget.h"
#include "ResourceMgr.h"
#include "UIMgr.h"


UiRichBox::UiRichBox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
	_rich = false;
}

void UiRichBox::getDefaultSize( int &width, int &height ) const
{
	width = 200;
	height = 200;
}

QDomElement UiRichBox::serialize( QDomDocument &doc, const QDomElement &extElement )
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
	extEle.setAttribute("Text" , fromCString(((c_richbox*)_widget)->get_text()));

	extEle.setAttribute("Padding",QString("%1,%2").arg(getMargin().width()).arg(getMargin().height()));
	extEle.setAttribute("Rich", boolToString(getRich()));
	extEle.setAttribute("FlipX", boolToString(getFlipX()));
	extEle.setAttribute("FlipY", boolToString(getFlipY()));

	
	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Label");
	return ele;
}

void UiRichBox::readExt( QXmlStreamReader &reader )
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

	QString bgimage = atts.value(QLatin1String("CommonMap")).toString();
	QStringList region = atts.value(QLatin1String("CommonTexcoord")).toString().split(",");
	QStringRef stenciltag = atts.value(QLatin1String("StencilMap"));
	setStencil(createSkinImage(stenciltag.toString()));
	QString text = atts.value(QLatin1String("Text")).toString();
	QStringList margin = atts.value(QLatin1String("Padding")).toString().split(",");
	bool rich = StringToBool(atts.value(QLatin1String("Rich")).toString());
	bool FlipX = StringToBool(atts.value(QLatin1String("FlipX")).toString());
	bool FlipY = StringToBool(atts.value(QLatin1String("FlipY")).toString());
	setFlipX(FlipX);
	setFlipY(FlipY);

	


	setBackImg(createSkinImage(bgimage, region));
	setRich(rich);
	setText(text);

	QSize ms;
	if(margin.size() == 2)
	{
		ms.setWidth(margin[0].toUInt());
		ms.setHeight(margin[1].toUInt());
	}
	setMargin(ms);
}

//////////////////////////////////////////////////////////////////////////

SkinImageEx UiRichBox::getBackImg() const
{
	//assert(((c_label*)_widget)->get_bg_image() == fromQString(_skinImage.getSkinName()));
	return _skinImage;
}

void UiRichBox::setBackImg( const SkinImageEx &backImg )
{
	SkinSet skinSet = backImg.getSkinSet();
	_skinImage.setData(backImg);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("Geometry"));
	if (perty && _tmpSkin != backImg.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_skinImage.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QRect rect(rc.x(), rc.y(), trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("Geometry", rect);
			_tmpSkin = backImg.getSkinName();
		}
	}
	((c_richbox*)_widget)->set_bg_image(fromQString(_skinImage.getSkinName()));
	((c_richbox*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

void UiRichBox::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_skinImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_skinImage));
}

QString UiRichBox::getText() const
{
	if (_rich)
		return fromCString(((c_richbox*)_widget)->get_text());
	else
		return _plainstr;
}

void UiRichBox::setText( const QString &text )
{
	if (!_rich && text.length())
	{
		QString font = "#font:";
		QString align = "#align:";
		QString color = "#color:";
		if (text[0] == "#")
		{
			c_string txt = fromQString(text);
			c_vector<c_string> out;
			txt.tokenize(L"#", out);
			for (int i = 0; i < out.size(); ++i)
			{
				c_vector<c_string> outtmp;
				out[i].tokenize(L":", outtmp);
				if (outtmp[0] == L"font")
				{
					c_string src = outtmp[1];
					font += QString::fromStdWString(src.c_str());
					font += ":";
					c_string height = outtmp[2];
					font += QString::fromStdWString(height.c_str());
					font += ":";
					c_string flag = outtmp[3];
					font += QString::fromStdWString(flag.c_str());
					font += "#";

					unsigned int fontid = hash(src.c_str());
					QString fontFileName = ResourceMgr::getInstance()->getFont(fontid);
					QString fontName = FontType::getFontName(fontFileName);
					FontType ft;
					FontFile ff(fontName, fontFileName);
					ft.setFontFile(ff);
					ft.setFontSize(height.to_integer());
					unsigned int flagx = flag.to_uinteger();
					bool outline = LOWU16(flagx) & 0x000F;
					bool bold = LOWU16(flagx) & 0x00F0;
					bool shadow = LOWU16(flagx) & 0x0F00;
					bool intly = LOWU16(flagx) & 0xF000;
					ft.setFontOutline(outline);
					ft.setFontShadow(shadow);
					ft.setFontBold(bold);
					ft.setFontItalics(intly);
					setFont(ft);
				}
				else if(outtmp[0] == L"align")
				{
					c_string alignm = outtmp[1];
					align += QString::fromStdWString(alignm.c_str());
					align += "#";
					e_text_alignment halign = TA_H_LEFT;
					e_text_alignment valign = TA_V_TOP;
					if (alignm == L"LT")
					{
						halign = TA_H_LEFT;
						valign = TA_V_TOP;
					}
					else if (alignm == L"LM")
					{
						halign = TA_H_LEFT;
						valign = TA_V_CENTER;
					}
					else if (alignm == L"LB")
					{
						halign = TA_H_LEFT;
						valign = TA_V_BOTTOM;
					}
					else if (alignm == L"MT")
					{
						halign = TA_H_CENTER;
						valign = TA_V_TOP;
					}
					else if (alignm == L"MM")
					{
						halign = TA_H_CENTER;
						valign = TA_V_CENTER;
					}
					else if (alignm == L"MB")
					{
						halign = TA_H_CENTER;
						valign = TA_V_BOTTOM;
					}
					else if (alignm == L"RT")
					{
						halign = TA_H_RIGHT;
						valign = TA_V_TOP;
					}
					else if (alignm == L"RM")
					{
						halign = TA_H_RIGHT;
						valign = TA_V_CENTER;
					}
					else
					{
						halign = TA_H_RIGHT;
						valign = TA_V_BOTTOM;
					}
					setTextAlignment(TextAlign(HTextAlignment(halign), VTextAlignment(valign)));
				}
				else if (outtmp[0] == L"color")
				{
					c_string clr = outtmp[1];
					color += QString::fromStdWString(clr.c_str());
					color += ":-1#";
					unsigned int txtClr = clr.to_uinteger();
					setTextColor(fromCColor(c_color(txtClr)));
				}
				if (i == out.size() - 1)
				{
					_plainstr = QString::fromStdWString(out[i].c_str());
				}
			}
		}
		else
		{
			_plainstr = text;
			c_string tmpd = fromQString(getFont().getFontFile().getFileName());
			c_vector<c_string> tmp2;
			c_vector<c_string> tmp3;
			tmpd.tokenize(L"/", tmp2);
			tmp2.back().tokenize(L".", tmp3);
			font += QString((const char*)tmp3.front().to_utf8());
			font += ":";
			font += QString::number(getFont().getFontSize());
			font += ":";
			u16 txtflag = 0x0000;
			u16 flag = 0;
			if (getFont().getFontOutline())
				flag |= 0x000F;
			if (getFont().getFontBold())
				flag |= 0x00F0;
			if (getFont().getFontShadow())
				flag |= 0x0F00;
			if (getFont().getFontItalics())
				flag |= 0xF000;
			txtflag = flag;
			font += QString::number(txtflag);
			font += "#";

			if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Top)
				align += "LT#";
			else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Center)
				align += "LM#";
			else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Bottom)
				align += "LB#";
			else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Top)
				align += "MT#";
			else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Center)
				align += "MM#";
			else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Bottom)
				align += "MB#";
			else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Top)
				align += "RT#";
			else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Center)
				align += "RM#";
			else
				align += "RB#";

			color += QString::number(fromQColor(getTextColor()).rgba());
			color += ":-1#";
		}
		((c_richbox*)_widget)->set_text(fromQString(font + align + color + _plainstr));
	}
	else
		((c_richbox*)_widget)->set_text(fromQString(text));
}

QColor UiRichBox::getTextColor() const
{
	c_color color(((c_richbox*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiRichBox::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_richbox*)_widget)->set_text_color(clr);
	if (!_rich && !_plainstr.isEmpty())
	{
		QString font = "#font:";
		c_string tmpd = fromQString(getFont().getFontFile().getFileName());
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmpd.tokenize(L"/", tmp2);
		tmp2.back().tokenize(L".", tmp3);
		font += QString((const char*)tmp3.front().to_utf8());
		font += ":";
		font += QString::number(getFont().getFontSize());
		font += ":";
		u16 txtflag = 0x0000;
		u16 flag = 0;
		if (getFont().getFontOutline())
			flag |= 0x000F;
		if (getFont().getFontBold())
			flag |= 0x00F0;
		if (getFont().getFontShadow())
			flag |= 0x0F00;
		if (getFont().getFontItalics())
			flag |= 0xF000;
		txtflag = flag;
		font += QString::number(txtflag);
		font += "#";

		QString align = "#align:";
		if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Top)
			align += "LT#";
		else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Center)
			align += "LM#";
		else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Bottom)
			align += "LB#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Top)
			align += "MT#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Center)
			align += "MM#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Bottom)
			align += "MB#";
		else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Top)
			align += "RT#";
		else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Center)
			align += "RM#";
		else
			align += "RB#";

		QString color = "#color:";
		color += QString::number(fromQColor(getTextColor()).rgba());
		color += ":-1#";
		((c_richbox*)_widget)->set_text(fromQString(font + align + color + _plainstr));
	}
}


QSize UiRichBox::getMargin() const
{
	c_float2 margin = ((c_richbox*)_widget)->get_margin();
	return QSize(margin.x(), margin.y());
}

void UiRichBox::setMargin( const QSize &margin )
{
	((c_richbox*)_widget)->set_margin(c_float2(margin.width(), margin.height()));
}

SkinImage UiRichBox::getStencil()
{
	//assert(((c_dialog*)_widget)->get_bg_image() == fromQString(_backImage.getSkinName()));
	return _stencilImg;
}
TextAlign UiRichBox::getTextAlignment() const
{
	return m_align;
}

void UiRichBox::setTextAlignment(const TextAlign &align)
{
	m_align = align;

	if (!_rich && !_plainstr.isEmpty())
	{
		QString font = "#font:";
		c_string tmpd = fromQString(getFont().getFontFile().getFileName());
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmpd.tokenize(L"/", tmp2);
		tmp2.back().tokenize(L".", tmp3);
		font += QString((const char*)tmp3.front().to_utf8());
		font += ":";
		font += QString::number(getFont().getFontSize());
		font += ":";
		u16 txtflag = 0x0000;
		u16 flag = 0;
		if (getFont().getFontOutline())
			flag |= 0x000F;
		if (getFont().getFontBold())
			flag |= 0x00F0;
		if (getFont().getFontShadow())
			flag |= 0x0F00;
		if (getFont().getFontItalics())
			flag |= 0xF000;
		txtflag = flag;
		font += QString::number(txtflag);
		font += "#";

		QString align = "#align:";
		if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Top)
			align += "LT#";
		else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Center)
			align += "LM#";
		else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Bottom)
			align += "LB#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Top)
			align += "MT#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Center)
			align += "MM#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Bottom)
			align += "MB#";
		else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Top)
			align += "RT#";
		else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Center)
			align += "RM#";
		else
			align += "RB#";

		QString color = "#color:";
		color += QString::number(fromQColor(getTextColor()).rgba());
		color += ":-1#";
		((c_richbox*)_widget)->set_text(fromQString(font + align + color + _plainstr));
	}
}

void UiRichBox::setFont(const FontType &fontType)
{
	UiWidget::setFont(fontType);
	if (!_rich && !_plainstr.isEmpty())
	{
		QString font = "#font:";
		c_string tmpd = fromQString(getFont().getFontFile().getFileName());
		c_vector<c_string> tmp2;
		c_vector<c_string> tmp3;
		tmpd.tokenize(L"/", tmp2);
		tmp2.back().tokenize(L".", tmp3);
		font += QString((const char*)tmp3.front().to_utf8());
		font += ":";
		font += QString::number(getFont().getFontSize());
		font += ":";
		u16 txtflag = 0x0000;
		u16 flag = 0;
		if (getFont().getFontOutline())
			flag |= 0x000F;
		if (getFont().getFontBold())
			flag |= 0x00F0;
		if (getFont().getFontShadow())
			flag |= 0x0F00;
		if (getFont().getFontItalics())
			flag |= 0xF000;
		txtflag = flag;
		font += QString::number(txtflag);
		font += "#";

		QString align = "#align:";
		if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Top)
			align += "LT#";
		else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Center)
			align += "LM#";
		else if (getTextAlignment().horizontal == H_Left && getTextAlignment().vertical == V_Bottom)
			align += "LB#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Top)
			align += "MT#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Center)
			align += "MM#";
		else if (getTextAlignment().horizontal == H_Center && getTextAlignment().vertical == V_Bottom)
			align += "MB#";
		else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Top)
			align += "RT#";
		else if (getTextAlignment().horizontal == H_Right && getTextAlignment().vertical == V_Center)
			align += "RM#";
		else
			align += "RB#";

		QString color = "#color:";
		color += QString::number(fromQColor(getTextColor()).rgba());
		color += ":-1#";
		((c_richbox*)_widget)->set_text(fromQString(font + align + color + _plainstr));
	}
}

void UiRichBox::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_richbox*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
	//SkinSet skinSet = stencilImg.getSkinSet();
	//_stencilImg.setData(stencilImg);
	//((c_dialog*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}


bool UiRichBox::getRich() const
{
	return _rich;
}
void UiRichBox::setRich(bool color)
{
	_rich = color;
}
