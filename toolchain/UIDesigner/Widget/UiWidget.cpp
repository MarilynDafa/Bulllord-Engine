#include "UiWidget.h"
#include "UiWidgetFactory.h"
#include "Widget.h"
#include "UIMgr.h"
#include "ResourceMgr.h"
#include "PropertyEditerDockWidget.h"
#include <QtCore/QRect>
#include "MainWindow.h"
#include "CentralWidget.h"
#include "PropertyEditerDockWidget.h"
#include "PropertyManager.h"
//////////////////////////////////////////////////////////////////////////
SizeController::SizeController()
	: _pos(INVALID)
	, _widget(0)
{

}

SizeController::SizeController( const SizeController &controller )
{
	_point = controller._point;
	_widget = controller._widget;
	_pos = controller._pos;
}

SizeController::SizeController( const QPoint &point, UiWidget *widget, POSITION pos )
{
	_point = point;
	_widget = widget;
	_pos = pos;
}

QRect SizeController::getRect() const
{
	enum { RECT_LENGTH = 7 };
	int x = _point.x();
	int y = _point.y();
	return QRect(x - RECT_LENGTH/2, y - RECT_LENGTH/2, RECT_LENGTH, RECT_LENGTH);
}

bool SizeController::moveTo( int x, int y )
{
	assert(_widget != 0);
	int mx = x - _point.x();
	int my = y - _point.y();

	if(mx == 0 && my == 0)
		return false;

	switch(_pos)
	{
	case POSITION_RIGHT:
		my = 0;
		rbMove(mx, my);
		assert(my ==0);
		break;
	case POSITION_BOTTOM:
		mx = 0;
		rbMove(mx, my);
		assert(mx ==0);
		break;
	case POSITION_RB:
		rbMove(mx, my);
		break;
	case POSITION_LEFT:
		my = 0;
		ltMove(mx, my);
		assert(my ==0);
		break;
	case POSITION_TOP:
		mx = 0;
		ltMove(mx, my);
		assert(mx ==0);
		break;
	case POSITION_LT:
		ltMove(mx, my);
		break;
	case POSITION_LB:
		lbMove(mx, my);
		break;
	case POSITION_RT:
		rtMove(mx, my);
		break;
	}
	_point.rx() += mx;
	_point.ry() += my;
	return true;
}

void SizeController::ltMove( int &x, int &y )
{
	int width, height;
	_widget->getWidgetSize(width, height);
	width -= x;
	height -= y;
	QVector2D v = calculateSize(width, height);
	x -= v.x();
	y -= v.y();
	_widget->resize(QSize(width, height));
	//_widget->move(QVector2D(x, y));
}

void SizeController::lbMove( int &x, int &y )
{
	int width, height;
	_widget->getWidgetSize(width, height);
	width -= x;
	height += y;
	QVector2D v = calculateSize(width, height);
	x -= v.x();
	y += v.y();
	_widget->resize(QSize(width, height));
	//_widget->move(QVector2D(x, 0));
}

void SizeController::rtMove( int &x, int &y )
{
	int width, height;
	_widget->getWidgetSize(width, height);
	width += x;
	height -= y;
	QVector2D v = calculateSize(width, height);
	x += v.x();
	y -= v.y();
	_widget->resize(QSize(width, height));
	//_widget->move(QVector2D(0, y));
}

void SizeController::rbMove( int &x, int &y )
{
	int width, height;
	_widget->getWidgetSize(width, height);
	width += x;
	height += y;
	QVector2D v = calculateSize(width, height);
	x += v.x();
	y += v.y();
	_widget->resize(QSize(width, height));
}

QVector2D SizeController::calculateSize( int &width, int &height )
{
	assert(_pos != INVALID);
	int dx = 0, dy = 0;
	int maxW = _widget->getMaxinumSize().width();
	int maxH = _widget->getMaxinumSize().height();
	int minW = _widget->getMininumSize().width();
	int minH = _widget->getMininumSize().height();
	if(maxW != 0 && width > maxW)
	{
		dx = maxW - width;
		width = maxW;
	}
	else if(width < minW)
	{
		dx = minW - width;
		width = minW;
	}
	if(maxH != 0 && height > maxH)
	{
		dy = maxH - height;
		height = maxH;
	}
	else if(height < minH)
	{
		dy = minH - height;
		height = minH;
	}
	return QVector2D(dx, dy);
}

int SizeController::getCursorShape()
{
	switch(_pos)
	{
	case POSITION_LEFT:
	case POSITION_RIGHT:
		return Qt::SizeHorCursor;
		break;
	case POSITION_TOP:
	case POSITION_BOTTOM:
		return Qt::SizeVerCursor;
		break;
	case POSITION_LT:
	case POSITION_RB:
		return Qt::SizeFDiagCursor;
		break;
	case POSITION_LB:
	case POSITION_RT:
		return Qt::SizeBDiagCursor;
		break;
	default:
		return -1;
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

UiWidget::UiWidget(UiWidget *parent /*= 0*/)
	: QObject(parent)
	, _widget(0)
	, _parent(0)
{
	_lock = false;
	_hasStencil = false;
	_tmpSkin = "Nil";
	_Hreference = H_Center;
	_Vreference = V_Center;
	_policy = Fixed;
	connect(this, SIGNAL(skinFileChanged(const SkinFile&)),
		this,SLOT(slotSkinFileChanged(const SkinFile&)));
}

UiWidget::~UiWidget()
{
	UiWidgetFactory::getSingletonPtr()->deleteWidgets(_child.toVector());
	if(_parent != 0)
		_parent->removeChild(this);
	if(_widget != 0)
		c_ui_manager::get_singleton_ptr()->remove_widget(_widget);
	disconnect(this, SIGNAL(skinFileChanged(const SkinFile&)),
		this,SLOT(slotSkinFileChanged(const SkinFile&)));
}

QString UiWidget::getClassName() const
{
	return QString(metaObject()->className());
}

void UiWidget::addChild( UiWidget *child )
{
	assert(child != 0);
	if(!_child.contains(child))
		_child.append(child);
	child->_parent = this;

	MainWindow::getInst()->slotDirty();
}

void UiWidget::removeChild( UiWidget *child )
{
	for(QList<UiWidget*>::iterator itr = _child.begin(); itr != _child.end(); itr ++)
	{
		if(child == *itr)
		{
			_child.erase(itr);
			child->_parent = 0;
			break;
		}
	}

	MainWindow::getInst()->slotDirty();
}

bool UiWidget::setParent( UiWidget *parent )
{
	if(_parent == parent)
		return false;

	if(_parent != 0)
		_parent->removeChild(this);

	c_widget *p;
	if(parent == 0)
		p = c_ui_manager::get_singleton_ptr()->get_environment();
	else
		p = parent->getWidget();
	assert(p != 0);
	c_ui_manager::get_singleton_ptr()->add_widget(this->getWidget(), p);

	if(parent != 0)
		parent->addChild(this);


	MainWindow::getInst()->slotDirty();

	return true;
}

QList<UiWidget*> UiWidget::createDefaultWidget(const QString &name, UiWidget *parent, const QPoint &pos, e_widget_type type )
{
	c_widget::s_widget_property p;
	c_widget *pw;
	QPoint rp = pos;
	if(parent != 0)
	{
		if(! parent ->acceptChildern())
			return QList<UiWidget*>();
		rp = parent->getRelativePoint(pos);

		pw = parent->getWidget();
	}
	else
	{
		pw = c_ui_manager::get_singleton_ptr()->get_environment();
	}

	int defaultW, defaultH;
	getDefaultSize(defaultW, defaultH);
	int left = rp.x() - defaultW*0.5;
	int top = rp.y() - defaultH*0.5;
	int right = left + defaultW;
	int bottom = top + defaultH;
	c_string text = fromQString(name);
	p.dim = c_rect(left, top, right, bottom);
	p.name = text;
	p.fntid = 0;
	p.skinid = hash(L"default_ui_skin_name");
	p.font_height = 32;
	_widget = c_ui_manager::get_singleton_ptr()->add_widget(type , pw , p);

	QList<UiWidget*> widgets = createInteractionWidgets(name);
	widgets.push_back(this);

	if(parent != NULL)
	{
		parent->addChild(this);
	}

	MainWindow::getInst()->slotDirty();
	_create();
	return widgets;
}

QPoint UiWidget::getRelativePoint( const QPoint& point )
{
	c_rect rect = _widget->get_region();
	c_float2 lt = rect.lt_pt();
	return QPoint(point.x() - lt.x(), point.y() - lt.y());
}

bool UiWidget::valid()
{
	if(_widget != 0)
		return true;
	return false;
}

void UiWidget::getDefaultSize( int &width, int &height ) const
{
	width = 200;
	height = 200;
}

void UiWidget::move( const QVector2D &v )
{
	QVector2D vv = v;
	if (_policy == H_Match)
		vv.setX(0);
	else if (_policy == V_Match)
		vv.setY(0);
	else if (_policy == Stretch)
	{
		vv.setX(0);
		vv.setY(0);
	}
	assert(_widget != 0);
	_widget->move(c_float2(vv.x(), vv.y()));

	MainWindow::getInst()->slotDirty();
}

void UiWidget::resize( const QSize &size, bool savescale)
{
	QVector<UiWidget*> widgets;
	QVector<QRect> rects;
	cacheWidget(this, widgets, rects);
	QSize ssz = size;
	UiWidget* parent = getParent();
	double w = getWidgetWidth();
	double h = getWidgetHeight();
	double pw = parent ? parent->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
	double ph = parent ? parent->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
	if (_policy == H_Match)
	{
		ssz.setWidth(pw);
	}
	else if (_policy == V_Match)
	{
		ssz.setHeight(ph);
	}
	else if (_policy == Stretch)
	{
		ssz.setWidth(pw);
		ssz.setHeight(ph);
	}
	assert(_widget != 0);
	_widget->resize(ssz.width(), ssz.height());

	updateChildren(nullptr, widgets, rects);
	MainWindow::getInst()->slotDirty();
}

UiWidget * UiWidget::getWidget( const QPoint &pos, bool acceptChild/* = false*/, const QList<UiWidget*> &except /*= QList<UiWidget*>() */ )
{
	if(getAbsoluteRegion().contains(pos))
	{
		for(QList<UiWidget*>::iterator iter = _child.begin(); iter != _child.end(); iter++)
		{
			UiWidget *widget = (*iter)->getWidget(pos, acceptChild, except);
			if(widget != 0)
				return widget;
		}

		if( !except.contains(this) &&
			(!acceptChild ||	(acceptChild && acceptChildern())) &&
			getVisible() )
			return this;
	}

	return 0;
}

bool UiWidget::setNewRegion( const QRect &oldRegion, const QRect &newRegion )
{
	if (!oldRegion.isValid())
		return false;
	if (!newRegion.isValid())
	{
		PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), oldRegion);
		return false;
	}

	if(oldRegion == newRegion)
		return false;

	QVector2D v(newRegion.x() - oldRegion.x(), newRegion.y() - oldRegion.y());
	move(v);
	if(newRegion.width() != oldRegion.width() ||
		newRegion.height() != oldRegion.height())
		resize(QSize(newRegion.width(), newRegion.height()));

	return true;
}

QPoint UiWidget::getWidgetLTPoint()
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	int left = rect.lt_pt().x();
	int top = rect.lt_pt().y();
	return QPoint(left, top);
}

QPoint UiWidget::getWidgetRBPoint()
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	int right = rect.rb_pt().x();
	int bottom = rect.rb_pt().y();
	return QPoint(right, bottom);
}

QPoint UiWidget::getWidgetCenterPoint()
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	int hCenter = (rect.rb_pt().x() + rect.lt_pt().x())/2;
	int vCenter = (rect.rb_pt().y() + rect.lt_pt().y())/2;
	return QPoint(hCenter, vCenter);
}

int UiWidget::getWidgetWidth()
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	return rect.width();
}

int UiWidget::getWidgetHeight()
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	return rect.height();
}

void UiWidget::getWidgetSize( int &width, int &height )
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	width = rect.width();
	height = rect.height();
}

bool UiWidget::isContains( UiWidget *child )
{
	if(child == this)
		return false;

	UiWidget *parent = child->getParent();
	while(parent != 0)
	{
		if(parent == this)
			return true;
		parent = parent->getParent();
	}

	return false;
}

c_string UiWidget::fromQString( const QString &str )
{
	return c_string((utf16*)str.utf16());
}

QString UiWidget::fromCString( const c_string &str )
{
	return QString::fromUtf16((const ushort*)str.c_str());
}

QColor UiWidget::fromCColor( const c_color &color )
{
	return QColor(color.r()*255, color.g()*255, color.b()*255, color.a()*255);
}

c_color UiWidget::fromQColor( const QColor &color )
{
	return c_color(color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f, color.alpha()/255.0f);
}

QString UiWidget::boolToString( bool v )
{
	if(v)
		return "true";
	else
		return "false";
}

bool UiWidget::StringToBool( const QString &str )
{
	QString s = str.trimmed();
	if(s == "true")
		return true;
	else
		return false;
}

void UiWidget::appendSizeController( QVector<SizeController> &controllers )
{
	assert(_widget != 0);
	const c_rect &rect = _widget->get_region();
	int left = rect.lt_pt().x();
	int top = rect.lt_pt().y();
	int right = rect.rb_pt().x();
	int bottom = rect.rb_pt().y();
	int hCenter = (left + right) / 2;
	int vCenter = (top + bottom) / 2;
	controllers.append(SizeController(QPoint(left, top), this, SizeController::POSITION_LT));
	controllers.append(SizeController(QPoint(left, vCenter), this, SizeController::POSITION_LEFT));
	controllers.append(SizeController(QPoint(left, bottom), this, SizeController::POSITION_LB));
	controllers.append(SizeController(QPoint(hCenter, bottom), this, SizeController::POSITION_BOTTOM));
	controllers.append(SizeController(QPoint(right, bottom), this, SizeController::POSITION_RB));
	controllers.append(SizeController(QPoint(right, vCenter), this, SizeController::POSITION_RIGHT));
	controllers.append(SizeController(QPoint(right, top), this, SizeController::POSITION_RT));
	controllers.append(SizeController(QPoint(hCenter, top), this, SizeController::POSITION_TOP));
}

QDomElement UiWidget::serialize( QDomDocument &doc, const QDomElement &extElement)
{
	QDomElement ele = doc.createElement("Element");
	ele.setAttribute("Name", getWidgetName());
	//ele.setAttribute("Type", "Widget");
	//ele.setAttribute("interaction", boolToString(getInteraction()));
	ele.setAttribute("Parent", _widget->get_parent()->get_id());

	RPolicy align = getPolicy();
	QString policy;
	if (align.dpolicy == Fixed) policy = "Fixed";
	else if (align.dpolicy == H_Match) policy = "H-Match";
	else if (align.dpolicy == V_Match) policy = "V-Match";
	else policy = "Stretch";
	ele.setAttribute("Policy", policy);


	PtAlign align2 = getReferencePt();
	QString hor, ver;
	if (align2.horizontal == H_Left)
		hor = "L";
	else if (align2.horizontal == H_Center)
		hor = "M";
	else
		hor = "R";
	if (align2.vertical == V_Top)
		ver = "T";
	else if (align2.vertical == V_Center)
		ver = "M";
	else
		ver = "B";
	ele.setAttribute("ReferencePt", hor + ver);
	//ele.setAttribute("fntid", hash(fromQString(getFont().getFontFile().getFontName()).c_str()));
	//ele.setAttribute("fntheight",getFont().getFontSize());
	//ele.setAttribute("txtcolor",fromQColor(getTextColor()).rgba());
	//ele.setAttribute("desc",_widget->get_desc());

	//TextAlign align = getTextAlignment();
	//QString vAlign, hAlign;
	//if(align.horizontal == H_Left) hAlign = "left";
	//else if(align.horizontal == H_Right) hAlign = "right";
	//else if(align.horizontal == H_Center) hAlign = "center";
	//if(align.vertical == V_Top) vAlign = "top";
	//else if(align.vertical == V_Center) vAlign = "center";
	//else if(align.vertical == V_Bottom) vAlign = "bottom";
	//ele.setAttribute("valign", vAlign);
	//ele.setAttribute("halign", hAlign);

	QRect rect = getRelativeRegion();
	QString region = QString("%1,%2,%3,%4").arg(rect.x()).arg(rect.y()).arg(rect.right()+1).arg(rect.bottom()+1);
	ele.setAttribute("Geometry", region);

	QDomElement baseEle = doc.createElement("Base");
	QSize minsz = getMininumSize();
	QSize maxsz = getMaxinumSize();
	baseEle.setAttribute("MinSize", QString("%1,%2").arg(minsz.width()).arg(minsz.height()));
	baseEle.setAttribute("MaxSize", QString("%1,%2").arg(maxsz.width()).arg(maxsz.height()));
	baseEle.setAttribute("Tooltip", getToolTip());
	baseEle.setAttribute("AbsScissor", boolToString(getAbs()));
	baseEle.setAttribute("Cliped", boolToString(getCliped()));
	baseEle.setAttribute("Penetration", boolToString(getPenetrate()));

	ele.appendChild(baseEle);
	ele.appendChild(extElement);

	QListIterator<UiWidget*> childs(getChild());
	while(childs.hasNext())
		ele.appendChild(childs.next()->serialize(doc, QDomElement()));

	return ele;
}

void UiWidget::read( QXmlStreamReader &reader, UiWidget *parent /*= NULL*/ )
{
	if(reader.name() == QLatin1String("Element"))
	{
		QXmlStreamAttributes atts = reader.attributes();
		//unsigned int fntId = atts.value(QLatin1String("fntid")).toUInt();
		//unsigned int fntH = atts.value(QLatin1String("fntheight")).toUInt();
		QStringList region = atts.value(QLatin1String("Geometry")).toString().split(",");


//		QString fontFileName = ResourceMgr::getInstance()->getFont(fntId);
		//QString fontName = FontType::getFontName(fontFileName);
	//	FontType ft;
	//	FontFile ff(fontName, fontFileName);
	//	ft.setFontFile(ff);
	//	ft.setFontSize(fntH);
	//	setFont(ft);


		QStringRef ReferencePt = atts.value(QLatin1String("ReferencePt"));
		if (ReferencePt == "LT")
			setReferencePt(PtAlign(H_Left, V_Top));
		else if (ReferencePt == "LM")
			setReferencePt(PtAlign(H_Left, V_Center));
		else if (ReferencePt == "LB")
			setReferencePt(PtAlign(H_Left, V_Bottom));
		else if (ReferencePt == "MT")
			setReferencePt(PtAlign(H_Center, V_Top));
		else if (ReferencePt == "MM")
			setReferencePt(PtAlign(H_Center, V_Center));
		else if (ReferencePt == "MB")
			setReferencePt(PtAlign(H_Center, V_Bottom));
		else if (ReferencePt == "RT")
			setReferencePt(PtAlign(H_Right, V_Top));
		else if (ReferencePt == "RM")
			setReferencePt(PtAlign(H_Right, V_Center));
		else
			setReferencePt(PtAlign(H_Right, V_Bottom));


		QRect rect;
		if (region.size() == 4)
		{
			rect.setTopLeft(QPoint(region[0].toInt(), region[1].toInt()));
			rect.setBottomRight(QPoint(region[2].toInt() - 1, region[3].toInt() - 1));
		}
		setRelativeRegion(rect);

		QStringRef policy = atts.value(QLatin1String("Policy"));
		if (policy == "Fixed")
			setPolicy(Fixed);
		else if (policy == "V-Match")
			setPolicy(V_Match);
		else if (policy == "H-Match")
			setPolicy(H_Match);
		else
			setPolicy(Stretch);

		while(reader.readNextStartElement())
		{
			if(reader.name() == QLatin1String("Base"))
			{
				QXmlStreamAttributes atts = reader.attributes();
				bool penetrate = StringToBool(atts.value(QLatin1String("Penetration")).toString());
				bool clip = StringToBool(atts.value(QLatin1String("Cliped")).toString());
				bool abs = StringToBool(atts.value(QLatin1String("AbsScissor")).toString());
				QStringList minsz = atts.value(QLatin1String("MinSize")).toString().split(",");
				QStringList maxsz = atts.value(QLatin1String("MaxSize")).toString().split(",");
				QString tooltip = atts.value(QLatin1String("Tooltip")).toString();

				//setVisible(visible);
				setPenetrate(penetrate);
				//setTransfer(transmit);
				setCliped(clip);
				setAbs(abs);
				setToolTip(tooltip);
				
				if(minsz.size() == 2)
				{
					setMininumSize(QSize(minsz[0].toUInt(), minsz[1].toUInt()));
				}
				if(maxsz.size() == 2)
				{
					setMaxinumSize(QSize(maxsz[0].toUInt(), maxsz[1].toUInt()));
				}
				reader.skipCurrentElement();
			}
			else if(reader.name() == QLatin1String("Ext"))
			{
				readExt(reader);
				reader.skipCurrentElement();
			}
			else if(reader.name() == QLatin1String("Element"))
			{
				UiWidgetFactory::getSingletonPtr()->createWidget(reader, this);
			}

		}
	}
}

void UiWidget::readExt( QXmlStreamReader &reader )
{

}

////////////////////////////////////property//////////////////////////////////////
bool UiWidget::getVisible() const
{
	return _widget->is_visible();
}

void UiWidget::setVisible(bool visible)
{
	_widget->set_visible(visible);
}

QSize UiWidget::getMininumSize() const
{
	const c_float2 size = _widget->get_minsz();
	return QSize(size.x(),size.y());
}

void UiWidget::setMininumSize( const QSize &mininumSize )
{
	QRect rc = getRelativeRegion();
	if (_policy == Fixed)
	{
		if (rc.width() < mininumSize.width() && mininumSize.width())
		{
			int x = rc.x();
			int y = rc.y();
			int w = max(rc.width(), mininumSize.width());
			int h = rc.height();
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		else if (rc.height() < mininumSize.height() && mininumSize.height())
		{
			int x = rc.x();
			int y = rc.y();
			int w = rc.width();
			int h = max(rc.height(), mininumSize.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		_widget->set_min_size(c_float2(mininumSize.width(), mininumSize.height()));
	}
	else if (_policy == H_Match)
	{
		if (rc.height() < mininumSize.height() && mininumSize.height())
		{
			int x = rc.x();
			int y = rc.y();
			int w = rc.width();
			int h = max(rc.height(), mininumSize.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		_widget->set_min_size(c_float2(1, mininumSize.height()));
	}
	else if (_policy == V_Match)
	{
		if (rc.height() < mininumSize.height() && mininumSize.height())
		{
			int x = rc.x();
			int y = rc.y();
			int w = rc.width();
			int h = max(rc.height(), mininumSize.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		_widget->set_min_size(c_float2(mininumSize.width(), 1));
	}
}

QSize UiWidget::getMaxinumSize() const
{
	const c_float2 size = _widget->get_maxsz();
	return QSize(size.x(),size.y());
}

void UiWidget::setMaxinumSize( const QSize &maxinumSize )
{
	QRect rc= getRelativeRegion();
	if (_policy == Fixed)
	{
		if (rc.width() > maxinumSize.width() && maxinumSize.width())
		{
			int x = rc.x();
			int y = rc.y();
			int w = min(rc.width(), maxinumSize.width());
			int h = rc.height();
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		else if (rc.height() > maxinumSize.height() && maxinumSize.height())
		{
			int x = rc.x();
			int y = rc.y();
			int w = rc.width();
			int h = min(rc.height(), maxinumSize.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		_widget->set_max_size(c_float2(maxinumSize.width(), maxinumSize.height()));
	}
	else if (_policy == H_Match)
	{
		if (rc.height() > maxinumSize.height() && maxinumSize.height())
		{
			int x = rc.x();
			int y = rc.y();
			int w = rc.width();
			int h = min(rc.height(), maxinumSize.height());
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		_widget->set_max_size(c_float2(0, maxinumSize.height()));
	}
	else if (_policy == V_Match)
	{
		if (rc.width() > maxinumSize.width() && maxinumSize.width())
		{
			int x = rc.x();
			int y = rc.y();
			int w = min(rc.width(), maxinumSize.width());
			int h = rc.height();
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), QRect(x, y, w, h));
		}
		_widget->set_max_size(c_float2(maxinumSize.width(),0));
	}
}

void UiWidget::setWidgetName( const QString &widgetName )
{
	_widgetName = widgetName;
	c_string name = fromQString(widgetName);
	int id = hash((const char*)name.to_utf8());
	_widget->set_id(id);
}

PtAlign UiWidget::getReferencePt() const
{
	return PtAlign(_Hreference, _Vreference);
}
void UiWidget::setReferencePt(const PtAlign& pt)
{
	_Hreference = pt.horizontal;
	_Vreference = pt.vertical;
	QRect rc = getRelativeRegion();
	PtAlign align;
	align.horizontal = _Hreference;
	align.vertical = _Vreference;
	PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), rc);
	if (_policy == H_Match)
		align.horizontal = H_Center;
	else if (_policy == V_Match)
		align.vertical = V_Center;
	else if (_policy == Stretch)
	{
		align.horizontal = H_Center;
		align.vertical = V_Center;
	}
	QtProperty * perty = PropertyEditerDockWidget::getInstance()->findProperty("ReferencePt");
	PtAlignPropertyManager* vm = (PtAlignPropertyManager*)PropertyEditerDockWidget::getInstance()->getPtAlignMge();
	if (vm && perty)
	{
		vm->setValue(perty, QVariant::fromValue(align));
	}
}
RPolicy UiWidget::getPolicy() const
{
	return RPolicy(_policy);
}
void UiWidget::setPolicy(RPolicy po)
{
	if (po.dpolicy != Fixed)
	{
		QVector<UiWidget*> widgets;
		QVector<QRect> rects;
		cacheWidget(this, widgets, rects);
		QRect rc = getRelativeRegion();
		double w = rc.width();
		double h = rc.height();
		UiWidget* parent = getParent();
		double pw = parent? parent->getWidgetWidth():c_ui_manager::get_singleton_ptr()->getWidth();
		double ph = parent? parent->getWidgetHeight():c_ui_manager::get_singleton_ptr()->getHeight();
		_Hreference = H_Center;
		_Vreference = V_Center;
		PtAlign align;
		align.horizontal = _Hreference;
		align.vertical = _Vreference;
		QtProperty * perty = PropertyEditerDockWidget::getInstance()->findProperty("ReferencePt");
		PtAlignPropertyManager* vm = (PtAlignPropertyManager*)PropertyEditerDockWidget::getInstance()->getPtAlignMge();
		if (vm && perty)
		{
			vm->setValue(perty, QVariant::fromValue(align));
		}
		rc.setX(0);
		rc.setY(0);


		if (po.dpolicy == Stretch)
		{
			rc.setWidth(pw);
			rc.setHeight(ph);
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), rc);
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("MminSize"), QSize(1, 1));
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("MaxSize"), QSize(0,0));
			//updateChildren(this);
		}
		else if (po.dpolicy == H_Match)
		{
			rc.setWidth(pw);
			rc.setHeight(pw / w * h);
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), rc);
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("MaxSize"), QSize(0, _widget->get_maxsz().y()));
			//updateChildren(this);
		}
		else if (po.dpolicy == V_Match)
		{
			rc.setWidth(ph / h * w);
			rc.setHeight(ph);
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), rc);
			PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("MaxSize"), QSize(_widget->get_maxsz().x(), 0));
			//updateChildren(this);
		}
		updateChildren(nullptr, widgets, rects);
	}
	_policy = po.dpolicy;

}
QString UiWidget::getToolTip() const
{
	return fromCString(_widget->get_tooltip());
}

void UiWidget::setToolTip( const QString &toolTip )
{
	_widget->set_tooltip(fromQString(toolTip));
}

bool UiWidget::getCliped() const
{
	return !_widget->is_no_clipped();
}

void UiWidget::setCliped( bool cliped )
{
	_widget->set_no_clipped(!cliped);
}

QRect UiWidget::getAbsoluteRegion() const 
{
	if (!_widget)
		return QRect(0, 0, 100, 100);
	const c_rect &rect = _widget->get_region();
	int left = rect.lt_pt().x();
	int top = rect.lt_pt().y();
	int width = rect.rb_pt().x() - left;
	int height = rect.rb_pt().y() - top;
	return QRect(left, top, width, height);
}

void UiWidget::setAbsoluteRegion( const QRect &region )
{
	setNewRegion(getAbsoluteRegion(), region);
}

QRect UiWidget::getRelativeRegion() const
{
		int x;
		int y;
		int w;
		int h;
		QRect ret;
		QRect rect = getAbsoluteRegion();
		QRect pRect;
		if (_parent == 0)
			pRect = QRect(0, 0, c_ui_manager::get_singleton_ptr()->getWidth(), c_ui_manager::get_singleton_ptr()->getHeight());
		else
			pRect = _parent->getAbsoluteRegion();
		if (_Hreference == H_Left && _Vreference == V_Top)
		{
			x = rect.x() - pRect.x();
			y = rect.y() - pRect.y();
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Left && _Vreference == V_Bottom)
		{
			x = rect.x() - pRect.x();
			y = rect.y() - (pRect.y() + pRect.height());
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Left && _Vreference == V_Center)
		{
			x = rect.x() - pRect.x();
			y = rect.y() - (pRect.y() + pRect.height()*0.5);
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Center && _Vreference == V_Top)
		{
			x = rect.x() - (pRect.x() + pRect.width()*0.5);
			y = rect.y() - pRect.y();
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Center && _Vreference == V_Bottom)
		{
			x = rect.x() - (pRect.x() + pRect.width()*0.5);
			y = rect.y() - (pRect.y() + pRect.height());
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Center && _Vreference == V_Center)
		{
			x = rect.x() - (pRect.x() + pRect.width()*0.5);
			y = rect.y() - (pRect.y() + pRect.height()*0.5);
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Right && _Vreference == V_Top)
		{
			x = rect.x() - (pRect.x() + pRect.width());
			y = rect.y() - pRect.y();
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Right && _Vreference == V_Bottom)
		{
			x = rect.x() - (pRect.x() + pRect.width());
			y = rect.y() - (pRect.y() + pRect.height());
			w = rect.width();
			h = rect.height();
		}
		else if (_Hreference == H_Right && _Vreference == V_Center)
		{
			x = rect.x() - (pRect.x() + pRect.width());
			y = rect.y() - (pRect.y() + pRect.height()*0.5);
			w = rect.width();
			h = rect.height();
		}
		ret = QRect(x, y, w, h);
		int wx = ret.width();
		int hx = ret.height();
		ret.setX(ret.x() + wx*0.5);
		ret.setY(ret.y() + hx*0.5);
		ret.setWidth(wx);
		ret.setHeight(hx);
		return ret;
}

void UiWidget::setRelativeRegion( const QRect &region )
{
	QRect qq = region;
	UiWidget* parent = getParent();
	double pw = parent ? parent->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
	double ph = parent ? parent->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
	if (_policy == H_Match)
	{
		qq.setX(0);
		qq.setWidth(pw);
	}
	else if (_policy == V_Match)
	{
		qq.setY(0);
		qq.setHeight(ph);
	}
	else if (_policy == Stretch)
	{
		qq.setX(0);
		qq.setY(0);
		qq.setWidth(pw);
		qq.setHeight(ph);
	}
	setNewRegion(getRelativeRegion(), qq);
	CentralWidget::getInst()->widgetPropertyChanged(this, QString::fromLatin1("Geometry"), this->property("Geometry"));
	//PropertyEditerDockWidget::getInstance()->setPropertyXX(QString::fromLatin1("Geometry"), qq);
}

SkinFile UiWidget::getSkinFile() const
{
	return _skinFile;
}

void UiWidget::setSkinFile( const SkinFile &skinFile )
{
	if(!_skinFile.equals(skinFile))
	{
		//c_ui_manager::get_singleton_ptr()->add_skin(fromQString(skinFile.getFileName()));
		ResourceMgr::getInstance()->addSkin(skinFile.getFileName());
		_widget->set_skin(fromQString(skinFile.getSkinName()));
		_skinFile = skinFile;
		skinFileChanged(skinFile);
	}
}

FontType UiWidget::getFont() const
{
	return _font;
}

void UiWidget::setFont(const FontType &fontType)
{
	if(!_font.equals(fontType))
	{
		if(FontType::getFontFileIndex(fontType.getFontFile()) >= 0)
		{
			c_string fontName = fromQString(fontType.getFontFile().getFileName());
			c_string tmp = fontName;
			c_vector<c_string> tmp2;
			c_vector<c_string> tmp3;
			tmp.tokenize(L"/", tmp2);
			tmp2.back().tokenize(L".", tmp3);
			if (!fontType.getFontOutline() && !fontType.getFontBold() && !fontType.getFontShadow())
				_font = fontType;
			else
			{
				if (fontType.getFontOutline() != _font.getFontOutline())
				{
					_font = fontType;
					_font.setFontBold(false);
					_font.setFontShadow(false);
					_font.setFontOutline(true);
				}
				else if (fontType.getFontBold() != _font.getFontBold())
				{
					_font = fontType;
					_font.setFontBold(true);
					_font.setFontShadow(false);
					_font.setFontOutline(false);
				}
				else if (fontType.getFontShadow() != _font.getFontShadow())
				{
					_font = fontType;
					_font.setFontBold(false);
					_font.setFontShadow(true);
					_font.setFontOutline(false);
				}
				else
					_font = fontType;
			}			
			_widget->set_font(tmp3.front(), fontType.getFontSize() , true, _font.getFontOutline(), _font.getFontShadow(), _font.getFontBold(), _font.getFontItalics());
			PropertyEditerDockWidget::getInstance()->setPropertyFont("Font", _font);
		}
	}
}

bool UiWidget::getInteraction() const
{
	return false;//_widget->is_interaction();
}

bool UiWidget::getTransfer() const
{
	return _widget->is_transmit();
}
void UiWidget::setTransfer(bool tran)
{
	_widget->set_transmit(tran);
}

bool UiWidget::getPenetrate() const
{
	return _widget->is_penetrate();
}
void UiWidget::setPenetrate(bool tran)
{
	_widget->set_penetrate(tran);
}
QStringList UiWidget::getSkinNames()
{
	return _skinFile.getNames();
}


bool UiWidget::getAbs() const
{
	return _widget->is_abs();
}
void UiWidget::setAbs( bool abs)
{
	return _widget->set_abs(abs);
}


void UiWidget::setFlipX(bool flag)
{
	_widget->set_flipx(flag);	
}
bool UiWidget::getFlipX() const
{
	return _widget->is_flipx();
}
void UiWidget::setFlipY(bool flag)
{
	_widget->set_flipy(flag);
}
bool UiWidget::getFlipY() const
{
	return _widget->is_flipy();
}
QString UiWidget::getSkinFileName()
{
	return _skinFile.getFileName();
}

unsigned int UiWidget::getWidgetId()
{
	return _widget->get_id();
}

int UiWidget::StringToAlign( const QString &str )
{
	if(str == "left") return H_Left;
	else if(str == "right") return H_Right;
	else if(str == "center") return H_Center;
	else if(str == "top") return V_Top;
	else if(str == "middle") return V_Center;
	else if(str == "bottom") return V_Bottom;

	return H_Left;
}

SkinImageEx UiWidget::createSkinImage( const QString &bgImg, const QStringList &region )
{
	QPoint pt;
	QSize sz;
	if(region.size() == 4)
	{
		pt = QPoint(region[0].toInt(), region[1].toInt());
		sz = QSize(region[2].toUInt(), region[3].toUInt());
	}

	SkinSet skinSet;
	skinSet.setPoint(pt);
	skinSet.setSize(sz);
	SkinImageEx skinImg;
	skinImg.setSkinSet(skinSet);
	skinImg.setSkinName(bgImg);

	return skinImg;
}

SkinImage UiWidget::createSkinImage( const QString &bgImg )
{
	SkinImage skinImg;
	skinImg.setSkinName(bgImg);
	return skinImg;
}

void UiWidget::cacheWidget(UiWidget* node, QVector<UiWidget*>& widgets, QVector<QRect>& rects)
{
	for (QList<UiWidget*>::const_iterator iter = node->getChild().begin(); iter != node->getChild().end(); ++iter)
	{
		QRect rc = (*iter)->property("Geometry").value<QRect>();
		widgets.push_back(*iter);
		rects.push_back(rc);
		cacheWidget(*iter, widgets, rects);
	}
}
void UiWidget::updateChildren(UiWidget* node, QVector<UiWidget*> widgets, QVector<QRect> rects)
{
	for (QVector<UiWidget*>::iterator iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		QRect qq = rects.front();
		QRect xxx = (*iter)->property("Geometry").value<QRect>();
		double w = xxx.width();
		double h = xxx.height();
		UiWidget* parent = (*iter)->getParent();
		double pw = parent ? parent->getWidgetWidth() : c_ui_manager::get_singleton_ptr()->getWidth();
		double ph = parent ? parent->getWidgetHeight() : c_ui_manager::get_singleton_ptr()->getHeight();
		if ((*iter)->_policy == H_Match)
		{
			qq.setX(0);
			qq.setWidth(pw);
		}
		else if ((*iter)->_policy == V_Match)
		{
			qq.setY(0);
			qq.setHeight(ph);
		}
		else if ((*iter)->_policy == Stretch)
		{
			qq.setX(0);
			qq.setY(0);
			qq.setWidth(pw);
			qq.setHeight(ph);
		}
		(*iter)->setNewRegion((*iter)->getRelativeRegion(), qq);
		rects.pop_front();
	}
}
