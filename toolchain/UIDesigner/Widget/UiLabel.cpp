#include "UiLabel.h"
#include "Label.h"
#include "PropertyEditerDockWidget.h"
#include "ResourceMgr.h"
#include "UIMgr.h"


UiLabel::UiLabel( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
	_clockwise = true;
}

void UiLabel::getDefaultSize( int &width, int &height ) const
{
	width = 160;
	height = 160;
}

QDomElement UiLabel::serialize( QDomDocument &doc, const QDomElement &extElement )
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

	extEle.setAttribute("CommonMap", getCommon().getSkinName());
	extEle.setAttribute("StencilMap", getStencil().getSkinName());
	extEle.setAttribute("Clockwise", boolToString(getClockwise()));
	extEle.setAttribute("StartAngle", getStartAngle());
	extEle.setAttribute("EndAngle", getEndAngle());
	extEle.setAttribute("AngleCut", boolToString(getAngleCut()));

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("Type", "Dial");
	return ele;
}

void UiLabel::readExt( QXmlStreamReader &reader )
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
	QStringRef stenciltag = atts.value(QLatin1String("StencilMap"));
	bool clockwise = StringToBool(atts.value(QLatin1String("Clockwise")).toString());
	bool bAngleCut = StringToBool(atts.value(QLatin1String("AngleCut")).toString());
	unsigned int startAngle = atts.value(QLatin1String("StartAngle")).toInt();
	unsigned int endangle = atts.value(QLatin1String("EndAngle")).toInt();

	setCommon(createSkinImage(nImg));
	setStencil(createSkinImage(stenciltag.toString()));
	setClockwise(clockwise);
	setStartAngle(startAngle);
	setEndAngle(endangle);
	setAngleCut(bAngleCut);

}
void UiLabel::_create()
{
	setCommon(createSkinImage("Nil"));
}
//////////////////////////////////////////////////////////////////////////
bool UiLabel::getWordWrap() const
{
	return ((c_label*)_widget)->is_wordwrap();
}

void UiLabel::setWordWrap( bool wordWrap )
{
	((c_label*)_widget)->set_wordwrap(wordWrap);
}
QSize UiLabel::getMargin() const
{
	c_float2 margin = ((c_label*)_widget)->get_margin();
	return QSize(margin.x(), margin.y());
}

void UiLabel::setMargin(const QSize &margin)
{
	((c_label*)_widget)->set_margin(c_float2(margin.width(), margin.height()));
}


bool UiLabel::getAngleCut() const
{
	return ((c_label*)_widget)->get_curAngle();
}
void UiLabel::setAngleCut(bool flag)
{
	((c_label*)_widget)->set_cutAngle(flag);
}
bool UiLabel::getClockwise() const
{
	return ((c_label*)_widget)->get_clockwise();
}
void UiLabel::setClockwise(bool flag)
{
	((c_label*)_widget)->set_clock_wise(flag);
}
int UiLabel::getStartAngle() const
{
	return ((c_label*)_widget)->get_start_angle();
}
void UiLabel::setStartAngle(int value)
{
	((c_label*)_widget)->set_start_angle(value);
}
int UiLabel::getEndAngle() const
{
	return ((c_label*)_widget)->get_end_angle();
}
void UiLabel::setEndAngle(int value)
{
	((c_label*)_widget)->set_end_angle(value);
}

SkinImage UiLabel::getCommon()
{
	return _CommonMap;
}
void UiLabel::setCommon(const SkinImage &commonMap)
{
	_CommonMap.setData(commonMap);
	QtVariantProperty* perty = dynamic_cast<QtVariantProperty*>(PropertyEditerDockWidget::getInstance()->findProperty("Geometry"));
	if (perty && _tmpSkin != commonMap.getSkinName())
	{
		c_skin* skin = c_ui_manager::get_singleton_ptr()->get_skin(_widget->get_skinid());
		if (skin)
		{
			c_rect trc = skin->get_texcoord(fromQString(_CommonMap.getSkinName()));
			QVariant vrc = perty->value();
			QRect rc = vrc.value<QRect>();
			QRect rect(rc.x(), rc.y(), trc.width(), trc.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX("Geometry", rect);
			_tmpSkin = commonMap.getSkinName();
		}
	}
	((c_label*)_widget)->set_common_image(fromQString(commonMap.getSkinName()));
}
SkinImage UiLabel::getStencil()
{
	return _stencilImg;
}
void UiLabel::setStencil(const SkinImage &stencilImg)
{
	if (stencilImg.getSkinName() != "Nil" && stencilImg.getSkinName().size() != 0)
		_hasStencil = true;

	_stencilImg.setData(stencilImg);
	((c_label*)_widget)->set_stencil_image(fromQString(stencilImg.getSkinName()));
}


SkinImageEx UiLabel::getBackImg() const
{
	assert(((c_label*)_widget)->get_bg_image() == fromQString(_skinImage.getSkinName()));
	return _skinImage;
}

void UiLabel::setBackImg( const SkinImageEx &backImg )
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
	((c_label*)_widget)->set_bg_image(fromQString(_skinImage.getSkinName()));
	((c_label*)_widget)->set_bg_fragment(c_float2(skinSet.getCenterPt().x() , skinSet.getCenterPt().y()) , 
		c_float2(skinSet.getCenterSize().width() , skinSet.getCenterSize().height()));
}

void UiLabel::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_skinImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_skinImage));
}

QString UiLabel::getText() const
{
	return fromCString(((c_label*)_widget)->get_text());
}

void UiLabel::setText( const QString &text )
{
	((c_label*)_widget)->set_text(fromQString(text));
}

TextAlign UiLabel::getTextAlignment() const
{
	TextAlign align;
	align.horizontal = (UiWidget::HTextAlignment)((c_label*)_widget)->get_text_halign();
	align.vertical = (UiWidget::VTextAlignment)((c_label*)_widget)->get_text_valign();
	return align;
}

void UiLabel::setTextAlignment( const TextAlign &align )
{
	((c_label*)_widget)->set_text_align((e_text_alignment)align.horizontal, (e_text_alignment)align.vertical);
}

QColor UiLabel::getTextColor() const
{
	c_color color(((c_label*)_widget)->get_text_clr());
	return QColor((int)(color.r()*255.0), (int)(color.g()*255.0), (int)(color.b()*255.0), (int)(color.a()*255.0));
}

void UiLabel::setTextColor( const QColor &color )
{
	c_color clr((u32)color.red(), (u32)color.green(), (u32)color.blue(), (u32)color.alpha());
	((c_label*)_widget)->set_text_color(clr);
}
