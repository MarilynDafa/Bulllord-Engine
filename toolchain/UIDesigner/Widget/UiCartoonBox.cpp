#include "UiCartoonBox.h"
#include "CartoonBox.h"
#include "UIMgr.h"
#include "ResourceMgr.h"

#include <QtWidgets/QFileDialog>


CartoonBoxEditerDialog::CartoonBoxEditerDialog( QDialog *parent /*= 0*/ )
	: QDialog(parent)
{
	initUi();
	initSignal();
}

void CartoonBoxEditerDialog::initUi()
{
	setupUi(this);
}

void CartoonBoxEditerDialog::initSignal()
{

}

void CartoonBoxEditerDialog::slotBrowseBtnClicked()
{
	
}

void CartoonBoxEditerDialog::slotAddBtnClicked()
{

}

void CartoonBoxEditerDialog::slotDelBtnClicked()
{

}

void CartoonBoxEditerDialog::slotOkBtnClicked()
{

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UiCartoonBox::UiCartoonBox( UiWidget *parent /*= 0*/ )
	: UiWidget(parent)
{

}

void UiCartoonBox::getDefaultSize( int &width, int &height ) const
{
	width = 300;
	height = 300;
}

bool UiCartoonBox::handleDoubleClicked( int x, int y )
{
	CartoonBoxEditerDialog dlg;
	if(dlg.exec() == QDialog::Accepted)
	{

	}
	return true;
}

QDomElement UiCartoonBox::serialize( QDomDocument &doc, const QDomElement &extElement )
{
	QDomElement extEle = doc.createElement("ext");
	extEle.setAttribute("loop", boolToString(getLoop()));
	extEle.setAttribute("framerate" , QString("%1").arg(getFramerate()));

	QDomElement ele = UiWidget::serialize(doc, extEle);
	ele.setAttribute("class", "ctn");
	return ele;
}

void UiCartoonBox::readExt( QXmlStreamReader &reader )
{
	Q_ASSERT(reader.isStartElement());
	QXmlStreamAttributes atts = reader.attributes();
	bool loop = StringToBool(atts.value(QLatin1String("loop")).toString());
	unsigned int frameRate = atts.value(QLatin1String("framerate")).toUInt();

	setLoop(loop);
	setFramerate(frameRate);
}

//////////////////////////////////////////////////////////////////////////

bool UiCartoonBox::getLoop() const
{
	return ((c_cartoonbox*)_widget)->is_loop();
}

void UiCartoonBox::setLoop( bool loop )
{
	((c_cartoonbox*)_widget)->set_loop(loop);
}

int UiCartoonBox::getFramerate()
{
	return ((c_cartoonbox*)_widget)->get_framerate();
}

void UiCartoonBox::setFramerate(int rate)
{
	((c_cartoonbox*)_widget)->set_framerate(rate);
}

CartoonBoxFrames UiCartoonBox::getFrames() const
{
	return _frames;
}

void UiCartoonBox::setFrames( const CartoonBoxFrames &frames )
{
	if(_frames.equals(frames))
		return;

	_frames = frames;
	//c_ui_manager::get_singleton_ptr()->add_skin(fromQString(_frames.getSkinFile().getFileName()));

	ResourceMgr::getInstance()->addSkin(_frames.getSkinFile().getFileName());

	c_vector<c_string> cframes;
	QStringListIterator it(_frames.getFrames());
	while(it.hasNext())
		cframes.push_back(fromQString(it.next()));

	//((c_cartoonbox*)_widget)->set_frames(cframes);
}
