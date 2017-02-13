#include "UiScrollBox.h"
#include "ScrollBox.h"


UiScrollBox::UiScrollBox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{
}

void UiScrollBox::getDefaultSize( int &width, int &height ) const
{
	width = 200;
	height = 200;
}

QDomElement UiScrollBox::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("ext");
	extEle.setAttribute("strict", boolToString(getStrict()));


	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("class", "sbb");
	return ele;
}

void UiScrollBox::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	QString bImg = atts.value(QLatin1String("bgimage")).toString();
	bool strict = StringToBool(atts.value(QLatin1String("strict")).toString());

	setStrict(strict);
}

bool UiScrollBox::getStrict() const
{
	return ((c_scrollbox*)_widget)->is_strict();
}

void UiScrollBox::setStrict(bool val)
{
	return ((c_scrollbox*)_widget)->set_strict(val);
}

//////////////////////////////////////////////////////////////////////////


void UiScrollBox::slotSkinFileChanged( const SkinFile &skinFile )
{
	//_skinImage.setSkinFile(skinFile);
	//emitUserEnumNamesChanged(QString::fromLatin1("backGroundImage"), QVariant::fromValue<SkinImageEx>(_skinImage));
}
