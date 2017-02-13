#include "MainWindow.h"
#include "WidgetBoxDockWidget.h"
#include "PropertyEditerDockWidget.h"
#include "ObjectViewerDockWidget.h"
#include "CentralWidget.h"
#include "Widget/UiWidgetFactory.h"
#include "QtVariantEditorFactory"
#include "QtVariantPropertyManager"
#include "OpenFileDialog.h"
#include "ResourceMgr.h"
#include "aboutdlg.h"
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMessageBox>
#include "Codec.h"
#include "IL\il.h"
SaveSetDialog::SaveSetDialog( QWidget *widget /*= 0*/ )
	: QDialog(widget)
{
	initUi();
	initSignal();
}

SaveSetDialog::~SaveSetDialog()
{

}

void SaveSetDialog::initUi()
{
	setupUi(this);
}

void SaveSetDialog::initSignal()
{
	connect(_okBtn, SIGNAL(clicked()), this, SLOT(slotOkBtnClicked()));
}

void SaveSetDialog::slotOkBtnClicked()
{
	_fileName = _fileNameEdit->text();
	//!如果文件已经存在，需要提示
	accept();
}


//////////////////////////////////////////////////////////////////////////

WorkspaceSelectDialog::WorkspaceSelectDialog( QDialog *parent /*= 0*/ ) 
	: QDialog(parent)
{
	setupUi(this);
	_pathBox->addItems(getWorkSpaces());

	connect(_okBtn,SIGNAL(clicked()),this,SLOT(slotOkBtnClicked()));
	connect(_browseBtn,SIGNAL(clicked()),this,SLOT(slotBrowseBtnClicked()));
}

void WorkspaceSelectDialog::slotBrowseBtnClicked()
{
	QString dirStr = QFileDialog::getExistingDirectory(this, QDir::currentPath());
	if(dirStr.isEmpty())
		return;

	int index = _pathBox->findText(dirStr);
	if(index == -1)
	{
		_pathBox->insertItem(0, dirStr);
		_pathBox->setCurrentIndex(0);
	}
	else
	{
		_pathBox->setCurrentIndex(index);
	}
}

void WorkspaceSelectDialog::slotOkBtnClicked()
{
	int index = _pathBox->currentIndex();
	QString currentText = _pathBox->itemText(index);
	_pathBox->removeItem(index);

	if(!currentText.isEmpty())
	{
		QStringList workspaces;
		workspaces.append(currentText);
		int count = _pathBox->count();
		for(int i = 0; i < count; i ++)
			workspaces.append(_pathBox->itemText(i));
		writeToConfig(workspaces);
		_workSpace = currentText;
		accept();
	}
}

QStringList WorkspaceSelectDialog::getWorkSpaces()
{
	QString configFile("workspace.cfg");
	if(!QFile::exists(configFile))
		return QStringList();

	QFile file(configFile);
	if(!file.open(QIODevice::ReadOnly))
		return QStringList();

	QByteArray byteArray = file.readAll();
	file.close();

	QString fileStr(byteArray);
	QStringList spaces;
	QStringListIterator it(fileStr.split(QString("\n")));
	while(it.hasNext())
	{
		QString space = it.next().trimmed();
		if(! space.isEmpty())
			spaces.append(space);
	}
	return spaces;
}

void WorkspaceSelectDialog::writeToConfig( const QStringList &workSpaces )
{
	QString configFile("workspace.cfg");
	if(!QFile::exists(configFile))
		QFile::remove(configFile);

	QFile file(configFile);
	if(!file.open(QIODevice::WriteOnly))
		return;

	file.write(workSpaces.join(QString("\n")).toLatin1());
	file.close();
}

WXGAWidget::WXGAWidget(QWidget *parent /* = 0 */)
	: QWidget(parent)
{
	setupUi(this);
	//connect(_compressBtn, SIGNAL(clicked()), this, SIGNAL(compressBtnClicked()));
}
//////////////////////////////////////////////////////////////////////////

SetDialog::SetDialog( QWidget *parent /*= 0*/ )
	: QDialog(parent)
	, _manager(0)
	, _factory(0)
{
	w = 640;
	h = 1136;
	initUi();
	initSignal();
}

void SetDialog::initUi()
{
	setupUi(this);

	_manager = new QtVariantPropertyManager;
	_factory = new QtVariantEditorFactory;
	_propertyWidget->setFactoryForManager(_manager, _factory);

	bool exist = true;
	QFile file("ui.uws");
	if (!file.open(QIODevice::ReadOnly))
		exist = false;
	if (exist)
	{
		QXmlStreamReader xmlReader(&file);
		xmlReader.readNext();
		while (!xmlReader.atEnd())
		{
			if (xmlReader.isStartElement())
			{
				if (xmlReader.name() == "Resolution")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					QStringList region = atts.value(QLatin1String("Value")).toString().split(",");
					w = region[0].toInt();
					h = region[1].toInt();
				}
				else if (xmlReader.name() == "SelectRangeColor")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_HIGHLIGHT, atts.value(QLatin1String("Value")).toUInt());
				}
				else if (xmlReader.name() == "CaretColor")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_CURSOR, atts.value(QLatin1String("Value")).toUInt());
				}
				/*
				else if (xmlReader.name() == "TableSeparator")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SEPARATOR, atts.value(QLatin1String("Value")).toUInt());
				}
				else if (xmlReader.name() == "TableSelecting")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SELECTING, atts.value(QLatin1String("Value")).toUInt());
				}
				else if (xmlReader.name() == "TableHighlight")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_HIGHLIGHT, atts.value(QLatin1String("Value")).toUInt());
				}*/
				else if (xmlReader.name() == "TextDisableColor")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TXT_DISABLE_CLR, atts.value(QLatin1String("Value")).toUInt());
				}
			}
			xmlReader.readNext();
		}

		file.close();
	}
	updateMap();


	QMapIterator<QString, QColor> it(_map);
	//Resolution
	QtVariantProperty *property = _manager->addProperty(QVariant::Size, "Resolution");
	property->setValue(QSize(w, h));
	_propertyWidget->addProperty(property);
	//Orientation
	//property = _manager->addProperty(QVariant::StringList, "Orientation");
	//QStringList _ori;
	//_ori.push_back("Portrait");
	//_ori.push_back("Landscape");
	//property->setValue(_ori);
	//_propertyWidget->addProperty(property);
	while(it.hasNext())
	{
		it.next();
		QtVariantProperty *property = _manager->addProperty(QVariant::Color, it.key());
		property->setValue(it.value());
		_propertyWidget->addProperty(property);
	}
}

void SetDialog::initSignal()
{
	assert(_manager != 0 && _factory != 0);
	connect(_manager, SIGNAL(valueChanged(QtProperty*, const QVariant &)),
		this,SLOT(slotValueChanged(QtProperty*, const QVariant &)));
	connect(_cancelBtn, SIGNAL(clicked()),this,SLOT(slotCancelBtnClicked()));
	connect(_okBtn, SIGNAL(clicked()),this,SLOT(slotOkBtnClicked()));
}

void SetDialog::updateMap()
{
	_map["SelectRangeColor"] = UiWidget::fromCColor(c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_EDIT_HIGHLIGHT));
	_map["TextDisableColor"] = UiWidget::fromCColor(c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TXT_DISABLE_CLR));
	_map["CaretColor"] = UiWidget::fromCColor(c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_EDIT_CURSOR));
	//_map["TableSeparator"] = UiWidget::fromCColor(c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TABLE_SEPARATOR));
//	_map["TableSelecting"] = UiWidget::fromCColor(c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TABLE_SELECTING));
	//_map["TableHighlight"] = UiWidget::fromCColor(c_ui_manager::get_singleton_ptr()->get_uu_color(UUC_TABLE_HIGHLIGHT));
}

void SetDialog::slotValueChanged( QtProperty *property, const QVariant &value )
{
	QString propertyName = property->propertyName();
	if (propertyName == "Resolution")
	{
		QSize size = value.value<QSize>();
		w = size.width();
		h = size.height();
		return;
	}
	
	if(!value.canConvert<QColor>())
		return;
	QColor color = value.value<QColor>();
	c_color clr = UiWidget::fromQColor(color);

	if(propertyName == "SelectRangeColor")
		c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_HIGHLIGHT, clr.rgba());
	else if(propertyName == "CaretColor")
		c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_CURSOR, clr.rgba());
	//else if(propertyName == "TableSeparator")
	//	c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SEPARATOR, clr.rgba());
	//else if(propertyName == "TableSelecting")
	//	c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SELECTING, clr.rgba());
	//else if(propertyName == "TableHighlight")
	//	c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_HIGHLIGHT, clr.rgba());
	else if(propertyName == "TextDisableColor")
		c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TXT_DISABLE_CLR, clr.rgba());
}

void SetDialog::slotCancelBtnClicked()
{
	QMapIterator<QString, QColor> it(_map);
	while(it.hasNext())
	{
		it.next();
		QString name = it.key();
		QColor color = it.value();
		c_color clr = UiWidget::fromQColor(color);

		if(name == "SelectRangeColor")
			c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_HIGHLIGHT, clr.rgba());
		else if(name == "CaretColor")
			c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_CURSOR, clr.rgba());
		//else if(name == "TableSeparator")
		//	c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SEPARATOR, clr.rgba());
		//else if(name == "TableSelecting")
		//	c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SELECTING, clr.rgba());
		//else if(name == "TableHighlight")
		//	c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_HIGHLIGHT, clr.rgba());
		else if(name == "TextDisableColor")
			c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TXT_DISABLE_CLR, clr.rgba());
	}
	reject();
}

void SetDialog::slotOkBtnClicked()
{
	updateMap();
	QMapIterator<QString, QColor> it(_map);



	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml version=\"1.0\"", "encoding=\"UTF-8\""));
	QDomElement uiEle = doc.createElement("uiworkspace");

	QDomElement ele = doc.createElement("Resolution");
	ele.setAttribute("Value", QString("%1,%2").arg(w).arg(h));
	uiEle.appendChild(ele);



	while(it.hasNext())
	{
		it.next();

		QDomElement ele = doc.createElement(it.key());
		c_color clr(it.value().red() / 255.0f, it.value().green() / 255.0f, it.value().blue() / 255.0f, it.value().alpha() / 255.0f);
		ele.setAttribute("Value", clr.rgba());
		uiEle.appendChild(ele);
		GlobalConfig::getSingletonPtr()->setValue(it.key(), it.value().rgba());
	}

	doc.appendChild(uiEle);

	QFile file("ui.uws");
	if (!file.open(QIODevice::WriteOnly))
		return;
	file.write(doc.toByteArray());
	file.close();
	accept();
}

//////////////////////////////////////////////////////////////////////////
MainWindow* MainWindow::_self = NULL;
MainWindow::MainWindow( QWidget *parent /*= 0*/ )
	: QMainWindow(parent)
	, _widgetEmitSignals(0)
{
	_changed = false;
	_curDoc = "untitled";
	ilInit();
	initUi();
	initSignal();
	initUUColor(width, height);
	setWindowState(Qt::WindowMaximized);
	_self = this;
	lockControl(false, false);
}


MainWindow* MainWindow::getInst()
{
	return _self;
}

void MainWindow::showCursorPos(QPoint* pt)
{
	QString msg = _curDoc;
	if (_changed)
		msg = QString("*") + _curDoc;
	statusBar->showMessage(msg, 0);
}


void MainWindow::initUi()
{
	QLabel* _logo;
	setupUi(this);
	enableAlignAction(0, 0, 0);
	enableEditAction(0);

	_actToolbar = _toolBar->toggleViewAction();
	_actWidgetBox = _widgetBoxDock->toggleViewAction();
	_actObjectViewer = _objectViewerDock_2->toggleViewAction();
	_actPropertyEditer = _propertyEditerDock->toggleViewAction();

	_menuView->addAction(_actToolbar);
	_menuView->addSeparator();
	_menuView->addAction(_actWidgetBox);
	_menuView->addAction(_actObjectViewer);
	_menuView->addAction(_actPropertyEditer);
	
	_widgetBoxDockWidget = new WidgetBoxDockWidget;
	_widgetBoxDock->setWidget(_widgetBoxDockWidget);

	_properEditerDockWidget = new PropertyEditerDockWidget;
	verticalLayout->addWidget(_properEditerDockWidget);
	_logo = new QLabel(dockWidgetContents_2);
	_logo->setObjectName(QStringLiteral("label"));
	_logo->setPixmap(QPixmap(QString::fromUtf8(":/UIDesigner/Resources/logo.png")));
	verticalLayout->addWidget(_logo);

	_objectViewerDockWidget = new ObjectViewerDockWidget;
	_objectViewerDock_2->setWidget(_objectViewerDockWidget);


	
	_centralWidget = new CentralWidget(this);
	centralLayout->addWidget(_centralWidget);
	//setCentralWidget(_centralWidget);
	_centralWidget->init(SP_HD);

	_objectViewerDockWidget->setUserdata(_centralWidget);
	statusBar = new QStatusBar(this);
	statusBar->setObjectName(QStringLiteral("statusBar"));
	setStatusBar(statusBar);


	_wxgaWidget = new WXGAWidget;
	_toolBar->addWidget(_wxgaWidget);


	connect(_wxgaWidget->_compressBox, SIGNAL(currentIndexChanged(QString)),
		this, SLOT(slotWXGAChanged(QString)));

	QString backImageFile = GlobalConfig::getSingletonPtr()->value("backImage", "").toString();
	if(!backImageFile.isEmpty())
		setBackImage(backImageFile);

	
}

void MainWindow::initSignal()
{
	connect(_centralWidget,SIGNAL(widgetAdded(UiWidget*)),
		_objectViewerDockWidget,SLOT(slotWidgetAdded(UiWidget*)));


	connect(_centralWidget,SIGNAL(widgetSelectedChanged(const QVector<UiWidget*>&)),
		_objectViewerDockWidget,SLOT(slotWidgetSelectedChanged(const QVector<UiWidget*>&)));
	connect(_centralWidget,SIGNAL(widgetSelectedChanged(const QVector<UiWidget*>&)),
		_properEditerDockWidget,SLOT(slotWidgetSelectedChanged(const QVector<UiWidget*>&)));
	connect(_centralWidget,SIGNAL(widgetSelectedChanged(const QVector<UiWidget*>&)),
		this,SLOT(slotWidgetSelectedChanged(const QVector<UiWidget*>&)));
	connect(_centralWidget,SIGNAL(widgetsDeleted(const QVector<UiWidget*>&)),
		_objectViewerDockWidget,SLOT(slotWidgetsDeleted(const QVector<UiWidget*>&)));
	connect(_centralWidget,SIGNAL(widgetNameChanged(UiWidget*, QString)),
		_objectViewerDockWidget,SLOT(slotWidgetNameChanged(UiWidget*, QString)));
	connect(_centralWidget,SIGNAL(widgetNameChanged(UiWidget*, QString)),
		_properEditerDockWidget,SLOT(slotWidgetNameChanged(UiWidget*, QString)));
	connect(_centralWidget,SIGNAL(widgetParentChanged(UiWidget*)),
		_objectViewerDockWidget,SLOT(slotWidgetParentChanged(UiWidget*)));
	connect(_centralWidget,SIGNAL(widgetPropertyChanged(UiWidget*, const QString&, const QVariant&)),
		_properEditerDockWidget,SLOT(slotWidgetPropertyChanged(UiWidget*, const QString&, const QVariant&)));

	connect(_objectViewerDockWidget,SIGNAL(selectionItemChanged(const QVector<UiWidget*>&)),
		_centralWidget,SLOT(slotWidgetSlectedChanged(const QVector<UiWidget*>&)));
	connect(_objectViewerDockWidget,SIGNAL(selectionItemChanged(const QVector<UiWidget*>&)),
		_properEditerDockWidget,SLOT(slotWidgetSelectedChanged(const QVector<UiWidget*>&)));
	connect(_objectViewerDockWidget,SIGNAL(selectionItemChanged(const QVector<UiWidget*>&)),
		this,SLOT(slotWidgetSelectedChanged(const QVector<UiWidget*>&)));
	connect(_objectViewerDockWidget,SIGNAL(widgetNameChanged(UiWidget*, const QString&)),
		_centralWidget,SLOT(slotWidgetNameChanged(UiWidget*, const QString&)));

	connect(_properEditerDockWidget,SIGNAL(widgetPropertyChanged(UiWidget*, const QString&, const QVariant&)),
		_centralWidget,SLOT(slotWidgetPropertyChanged(UiWidget*, const QString&, const QVariant&)));
	connect(_properEditerDockWidget,SIGNAL(widgetPropertyChanged(UiWidget*, const QString&, const QVariant&)),
		_objectViewerDockWidget,SLOT(slotWidgetPropertyChanged(UiWidget*, const QString&, const QVariant&)));
	connect(_properEditerDockWidget, SIGNAL(dirty()),
		this, SLOT(slotDirty()));
	

	connect(_actLeftAlign,SIGNAL(triggered()),_centralWidget,SLOT(slotLeftAlignWidget()));
	connect(_actRightAlign,SIGNAL(triggered()),_centralWidget,SLOT(slotRightAlignWidget()));
	connect(_actHCenterAlign,SIGNAL(triggered()),_centralWidget,SLOT(slotHCenterAlignWidget()));
	connect(_actTopAlign,SIGNAL(triggered()),_centralWidget,SLOT(slotTopAlignWidget()));
	connect(_actBottomAlign,SIGNAL(triggered()),_centralWidget,SLOT(slotBottomAlignWidget()));
	connect(_actVCenterAlign,SIGNAL(triggered()),_centralWidget,SLOT(slotVCenterAlignWidget()));
	connect(_actHEqual,SIGNAL(triggered()),_centralWidget,SLOT(slotHEqualWidget()));
	connect(_actVEqual,SIGNAL(triggered()),_centralWidget,SLOT(slotVEqualWidget()));
	connect(_actEqualWidth,SIGNAL(triggered()),_centralWidget,SLOT(slotEqualWidgetWidth()));
	connect(_actEqualHeight,SIGNAL(triggered()),_centralWidget,SLOT(slotEqualWidgetHeight()));
	connect(_actEqualBoth,SIGNAL(triggered()),_centralWidget,SLOT(slotEqualWidgetBoth()));
	connect(_actHCenterToParent,SIGNAL(triggered()),_centralWidget,SLOT(slotHCenterToParent()));
	connect(_actVCenterToParent,SIGNAL(triggered()),_centralWidget,SLOT(slotVCenterToParent()));

	connect(_actOpen,SIGNAL(triggered()),this,SLOT(slotActionOpen()));
	connect(_actCut,SIGNAL(triggered()),this,SLOT(slotActionCut()));
	connect(_actCopy,SIGNAL(triggered()),this,SLOT(slotActionCopy()));
	connect(_actPaste,SIGNAL(triggered()),this,SLOT(slotActionPaste()));
	connect(_actSet,SIGNAL(triggered()),this,SLOT(slotActionSet()));
	connect(_actSave,SIGNAL(triggered()),this,SLOT(slotActionSave()));
	connect(_actBackImg,SIGNAL(triggered(bool)),this,SLOT(slotActionBackImage(bool)));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
	connect(actionSave_As, SIGNAL(triggered()), this, SLOT(slotActionSaveAs()));
	connect(actionAppend, SIGNAL(triggered()), this, SLOT(slotActionImport()));
	connect(_actExit, SIGNAL(triggered()), this, SLOT(slotActionExit()));
	connect(actionClose, SIGNAL(triggered()), this, SLOT(slotActionClose()));
	connect(WidgetClipBoard::getSingletonPtr(),SIGNAL(widgetsChanged(const QVector<UiWidget*>&)),
		this,SLOT(slotClipBoradWidgetsChanged(const QVector<UiWidget*>&)));
}

void MainWindow::initUUColorItem( const QString &key, int item )
{
	c_color clr= c_ui_manager::get_singleton_ptr()->get_uu_color((e_uniform_ui_color)item);
	QColor color = UiWidget::fromCColor(clr);
	QRgb colorRgb = GlobalConfig::getSingletonPtr()->value(key, color.rgba()).toUInt();
	color.setRgba(colorRgb);
	c_ui_manager::get_singleton_ptr()->set_uu_color((e_uniform_ui_color)item, UiWidget::fromQColor(color).rgba());
	GlobalConfig::getSingletonPtr()->setValue(key, colorRgb);
}

void MainWindow::initUUColor(int& w, int& h)
{
	bool exist = true;
	QFile file("ui.uws");
	if (!file.open(QIODevice::ReadOnly))
		exist = false;
	if (exist)
	{
		QXmlStreamReader xmlReader(&file);
		xmlReader.readNext();
		while (!xmlReader.atEnd())
		{
			if (xmlReader.isStartElement())
			{
				if (xmlReader.name() == "Resolution")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					QStringList region = atts.value(QLatin1String("Value")).toString().split(",");
					w = region[0].toInt();
					h = region[1].toInt();
				}
				else if (xmlReader.name() == "SelectRangeColor")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_HIGHLIGHT, atts.value(QLatin1String("Value")).toUInt());
				}
				else if (xmlReader.name() == "CaretColor")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_EDIT_CURSOR, atts.value(QLatin1String("Value")).toUInt());
				}
				/*
				else if (xmlReader.name() == "TableSeparator")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SEPARATOR, atts.value(QLatin1String("Value")).toUInt());
				}
				else if (xmlReader.name() == "TableSelecting")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_SELECTING, atts.value(QLatin1String("Value")).toUInt());
				}*/
			//	else if (xmlReader.name() == "TableHighlight")
			//	{
			//		QXmlStreamAttributes atts = xmlReader.attributes();
			//		c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TABLE_HIGHLIGHT, atts.value(QLatin1String("Value")).toUInt());
			//	}
				else if (xmlReader.name() == "TextDisableColor")
				{
					QXmlStreamAttributes atts = xmlReader.attributes();
					c_ui_manager::get_singleton_ptr()->set_uu_color(UUC_TXT_DISABLE_CLR, atts.value(QLatin1String("Value")).toUInt());
				}
			}
			xmlReader.readNext();
		}

		file.close();
	}
	else
	{
		w = 640;
		h = 1136;
	}
	_DesignW = w;
	_DesignH = h;
	resizeCanvas(w, h);
}

bool MainWindow::contains( QObject *w1, QObject *w2 )
{
	if(w1 == 0 || w2 == 0)
		return false;

	if(w1 == w2)
		return true;

	QObjectList child = w1->children();
	for(QObjectList::iterator it = child.begin(); it != child.end(); it ++)
	{
		if(contains(*it, w2))
			return true;
	}

	return false;
}

QWidget* MainWindow::setWidgetEmitSignals( QWidget *widget)
{
	QWidget *relw = _widgetEmitSignals;
	if(contains(_centralWidget, widget))
	{
		_widgetEmitSignals = _centralWidget;
		_centralWidget->emitSignals(true);
		_objectViewerDockWidget->emitSignals(false);
		_properEditerDockWidget->emitSignals(false);
	}
	else if(contains(_objectViewerDockWidget, widget))
	{
		_widgetEmitSignals = _objectViewerDockWidget;
		_centralWidget->emitSignals(false);
		_objectViewerDockWidget->emitSignals(true);
		_properEditerDockWidget->emitSignals(false);
	}
	else if(contains(_properEditerDockWidget, widget))
	{
		_widgetEmitSignals = _properEditerDockWidget;
		_centralWidget->emitSignals(false);
		_objectViewerDockWidget->emitSignals(false);
		_properEditerDockWidget->emitSignals(true);
	}
	return relw;
}

void MainWindow::enableAlignAction( int topWidgetCount, int selectWidgetCount, int childsCount )
{
	_actLeftAlign->setEnabled(topWidgetCount > 1);
	_actRightAlign->setEnabled(topWidgetCount > 1);
	_actHCenterAlign->setEnabled(topWidgetCount > 1);
	_actTopAlign->setEnabled(topWidgetCount > 1);
	_actBottomAlign->setEnabled(topWidgetCount > 1);
	_actVCenterAlign->setEnabled(topWidgetCount > 1);
	_actVEqual->setEnabled(topWidgetCount > 2);
	_actHEqual->setEnabled(topWidgetCount > 2);
	_actEqualWidth->setEnabled(selectWidgetCount > 1);
	_actEqualHeight->setEnabled(selectWidgetCount > 1);
	_actEqualBoth->setEnabled(selectWidgetCount > 1);
	_actHCenterToParent->setEnabled(childsCount > 0);
	_actVCenterToParent->setEnabled(childsCount > 0);
}

void MainWindow::enableEditAction( int selectWidgetCount )
{
	_actCut->setEnabled(selectWidgetCount > 0);
	_actCopy->setEnabled(selectWidgetCount > 0);
}


void MainWindow::paintEvent(QPaintEvent *event)
{
	QMainWindow::paintEvent(event);
	showCursorPos(NULL);
}

void MainWindow::setBackImage( const QString &imageFile )
{
	QImage rgbaImg = QImage(imageFile).convertToFormat(QImage::Format_ARGB32, Qt::ColorOnly);
	if(rgbaImg.isNull())
	{
		_centralWidget->setBackImage(0, 0, 0);
		GlobalConfig::getSingletonPtr()->setValue("backImage", "");
		return;
	}

	_actBackImg->setChecked(true);
	GlobalConfig::getSingletonPtr()->setValue("backImage", imageFile);

	int width = rgbaImg.width();
	int height = rgbaImg.height();

	unsigned char *imgData = new unsigned char[sizeof(unsigned char)*width*height*4];
	for(int i = 0; i < height; i ++)
	{
		for(int j = 0; j < width; j ++)
		{
			QRgb clr = rgbaImg.pixel(j, i);
			imgData[(i*width + j)*4] = qRed(clr);
			imgData[(i*width + j)*4 + 1] = qGreen(clr);
			imgData[(i*width + j)*4 + 2] = qBlue(clr);
			imgData[(i*width + j)*4 + 3] = qAlpha(clr);
		}
	}

	_centralWidget->setBackImage((void *)imgData, rgbaImg.width(), rgbaImg.height());
	delete imgData;
}

void MainWindow::slotFocusChanged( QWidget *old, QWidget *now )
{
	assert(old != now);
	setWidgetEmitSignals(now);
}

void MainWindow::slotWidgetSelectedChanged( const QVector<UiWidget*> &widget )
{
	QVector<UiWidget*> selectedParents = UiWidgetFactory::getSingletonPtr()->checkTopParentsInWidgets(widget);
	QVector<UiWidget*> selectedChilds = UiWidgetFactory::getSingletonPtr()->checkChildInWidgets(widget);
	enableAlignAction(selectedParents.count(), widget.count(), selectedChilds.count());
	enableEditAction(widget.count());
}

void MainWindow::slotClipBoradWidgetsChanged( const QVector<UiWidget*> &widgets )
{
	_actPaste->setEnabled(widgets.count() > 0);
}

void MainWindow::slotWXGAChanged(QString str)
{
	if (str == "WXGA+(16:10)")
	{
		if (_DesignW > _DesignH)
		{
			resizeCanvas(_DesignH* 16.0 / 10.0, _DesignH);
		}
		else
		{
			resizeCanvas(_DesignW, _DesignW * 16.0 / 10.0);
		}
	}
	else if (str == "Custom")
	{
		resizeCanvas(_DesignW, _DesignH);
	}
	else if (str == "XGA(4:3)")
	{
		if (_DesignW > _DesignH)
		{
			resizeCanvas(_DesignH * 4.0 / 3.0, _DesignH);
		}
		else
		{
			resizeCanvas(_DesignW, _DesignW * 4.0 / 3.0);
		}
	}
}

void MainWindow::slotActionCut()
{
	QWidget *w = setWidgetEmitSignals(_centralWidget);
	_centralWidget->cutSelectedWidgets();
	setWidgetEmitSignals(w);
}

void MainWindow::slotActionCopy()
{
	QWidget *w = setWidgetEmitSignals(_centralWidget);
	_centralWidget->copySelectedWidgets();
	setWidgetEmitSignals(w);
}

void MainWindow::slotActionPaste()
{
	QWidget *w = setWidgetEmitSignals(_centralWidget);
	_centralWidget->pasteWidgets();
	setWidgetEmitSignals(w);
}

void MainWindow::slotActionSet()
{
	SetDialog setDialog;
	setDialog.exec();
	bool exist = true;
	QFile file("ui.uws");
	if (!file.open(QIODevice::ReadOnly))
		exist = false;
	if (exist)
	{
		QXmlStreamReader xmlReader(&file);
		while (xmlReader.readNextStartElement())
		{
			if (xmlReader.name() == QLatin1String("Resolution"))
			{
				QXmlStreamAttributes atts = xmlReader.attributes();
				QStringList region = atts.value(QLatin1String("Value")).toString().split(",");
				width = region[0].toInt();
				height = region[1].toInt();
			}
		}
		file.close();
	}
	_DesignW = width;
	_DesignH = height;
	resizeCanvas(width, height);
	_wxgaWidget->_compressBox->setCurrentIndex(0);
}

void MainWindow::slotActionSave()
{
	if (_curDoc == "untitled")
	{
		SaveSetDialog dlg;
		if (dlg.exec() == QDialog::Accepted)
		{
			QString fileName = dlg.getFileName().trimmed();
			if (!fileName.isEmpty())
			{
				fileName += ".bui";
				QDir currentDir = QDir::current();
				QString itfDirStr = GlobalConfig::getSingletonPtr()->value("bui_dir", "bui").toString();
				currentDir.cd(itfDirStr);
				QFileInfo itfFileInfo(currentDir, fileName);
				if (itfFileInfo.exists())
				{
					QMessageBox messageBox(QMessageBox::Warning, "Warning:", "The file is already exist, continue?", QMessageBox::Ok | QMessageBox::Cancel, this);
					messageBox.button(QMessageBox::Ok)->setMinimumWidth(75);
					messageBox.button(QMessageBox::Cancel)->setMinimumWidth(75);
					messageBox.button(QMessageBox::Ok)->setMinimumHeight(23);
					messageBox.button(QMessageBox::Cancel)->setMinimumHeight(23);
					messageBox.button(QMessageBox::Ok)->setMaximumWidth(75);
					messageBox.button(QMessageBox::Cancel)->setMaximumWidth(75);
					messageBox.button(QMessageBox::Ok)->setMaximumHeight(23);
					messageBox.button(QMessageBox::Cancel)->setMaximumHeight(23);
					if (messageBox.exec() == QMessageBox::Cancel)
						return;
				}
				QString pathName = itfFileInfo.absoluteFilePath();
				if (QFile::exists(pathName))
					QFile::remove(pathName);
				_centralWidget->save(pathName);
				_curDoc = pathName;
				ResourceMgr::getInstance()->addItfFile(pathName);
			}
		}
	}
	else
	{
		_centralWidget->save(_curDoc);
	}
	_changed = false;
}


void MainWindow::slotActionSaveAs()
{
	SaveSetDialog dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		QString fileName = dlg.getFileName().trimmed();
		if (!fileName.isEmpty())
		{
			fileName += ".bui";
			QDir currentDir = QDir::current();
			QString itfDirStr = GlobalConfig::getSingletonPtr()->value("bui_dir", "bui").toString();
			currentDir.cd(itfDirStr);
			QFileInfo itfFileInfo(currentDir, fileName);
			if (itfFileInfo.exists())
			{
				QMessageBox messageBox(QMessageBox::Warning, "Warning:", "The file is already exist, continue?", QMessageBox::Ok | QMessageBox::Cancel, this);
				messageBox.button(QMessageBox::Ok)->setMinimumWidth(75);
				messageBox.button(QMessageBox::Cancel)->setMinimumWidth(75);
				messageBox.button(QMessageBox::Ok)->setMinimumHeight(23);
				messageBox.button(QMessageBox::Cancel)->setMinimumHeight(23);
				messageBox.button(QMessageBox::Ok)->setMaximumWidth(75);
				messageBox.button(QMessageBox::Cancel)->setMaximumWidth(75);
				messageBox.button(QMessageBox::Ok)->setMaximumHeight(23);
				messageBox.button(QMessageBox::Cancel)->setMaximumHeight(23);
				if (messageBox.exec() == QMessageBox::Cancel)
					return;
			}
			QString pathName = itfFileInfo.absoluteFilePath();
			if (QFile::exists(pathName))
				QFile::remove(pathName);
			_centralWidget->save(pathName);
			_curDoc = pathName;
			ResourceMgr::getInstance()->addItfFile(pathName);
		}
	}
}

void MainWindow::slotActionImport()
{
	OpenFileDialog dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		QString fileName = dlg.getSelectFile();
		if (!fileName.isEmpty() && fileName != _currentUiFile)
		{
			QDir currentDir = QDir::current();
			QString itfDir = GlobalConfig::getSingletonPtr()->value("bui_dir", "bui").toString();
			currentDir.cd(itfDir);
			QFileInfo fileInfo(currentDir, fileName + ".bui");
			Q_ASSERT(fileInfo.exists());
			if (_centralWidget->open(fileInfo.absoluteFilePath()))
			{
				_currentUiFile = fileInfo.absoluteFilePath();
			}
		}
	}
}

void MainWindow::slotActionBackImage(bool checked)
{
	if(checked)
	{
		QString imageFile = QFileDialog::getOpenFileName(this, "Set Background Image", QDir::current().absolutePath(), "Image File (*.bmp *.png *.jpg)");
		if(!imageFile.isEmpty())
			setBackImage(imageFile);
	}
	else
	{
		setBackImage(QString());
	}
}

void MainWindow::slotAbout()
{
	AboutDLg dlg;
	dlg.exec();
}

void MainWindow::slotActionClose()
{
	if (_changed)
	{
		QMessageBox msg(this);
		msg.setText("The ui has been modified.");
		msg.setInformativeText("Save your changes?");
		msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msg.setDefaultButton(QMessageBox::Save);
		msg.setLayoutDirection(Qt::LeftToRight);
		msg.setMinimumWidth(400);
		QPushButton* save = (QPushButton*)msg.button(QMessageBox::Save);
		save->setMinimumWidth(75);
		save->setMinimumHeight(23);
		QPushButton* cancel = (QPushButton*)msg.button(QMessageBox::Discard);
		cancel->setMinimumWidth(75);
		cancel->setMinimumHeight(23);
		if (msg.exec() == QMessageBox::Discard)
		{
			//不存储
		}
		else
		{
			//存储
			slotActionSave();
		}
	}
	_properEditerDockWidget->clean();
	_centralWidget->clean();
	_objectViewerDockWidget->clean();
	_curDoc = "untitled";
	_changed = false;
}
void MainWindow::slotActionExit()
{
	if (_changed)
	{
		QMessageBox msg(this);
		msg.setText("The ui has been modified.");
		msg.setInformativeText("Save your changes?");
		msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msg.setDefaultButton(QMessageBox::Save);
		msg.setLayoutDirection(Qt::LeftToRight);
		msg.setMinimumWidth(400);
		QPushButton* save = (QPushButton*)msg.button(QMessageBox::Save);
		save->setMinimumWidth(75);
		save->setMinimumHeight(23);
		QPushButton* cancel = (QPushButton*)msg.button(QMessageBox::Discard);
		cancel->setMinimumWidth(75);
		cancel->setMinimumHeight(23);
		if (msg.exec() == QMessageBox::Discard)
		{
			//不存储
		}
		else
		{
			//存储
			slotActionSave();
		}
	}
	QApplication::quit();
}

void MainWindow::resizeCanvas(int w, int h)
{
	double scale = 800.0 / double(w); 
	std::vector<UiWidget*> widgets;
	std::vector<QRect> rects;
	_centralWidget->cacheWidgetsPos(widgets, rects);
	_centralWidget->setMinimumWidth(w);
	_centralWidget->setMinimumHeight(h);
	_centralWidget->setMaximumWidth(w);
	_centralWidget->setMaximumHeight(h);
	_centralWidget->setScaleFactor(scale);
	_centralWidget->updateWidgetsPos(widgets, rects);
}
void MainWindow::slotDirty()
{
	_changed = true;
}

void MainWindow::lockControl(bool x, bool y)
{
}

void MainWindow::slotActionOpen()
{
	if (_changed)
	{
		QMessageBox msg(this);
		msg.setText("The ui has been modified.");
		msg.setInformativeText("Save your changes?");
		msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msg.setDefaultButton(QMessageBox::Save);
		msg.setLayoutDirection(Qt::LeftToRight);
		msg.setMinimumWidth(400);
		QPushButton* save = (QPushButton*)msg.button(QMessageBox::Save);
		save->setMinimumWidth(75);
		save->setMinimumHeight(23);
		QPushButton* cancel = (QPushButton*)msg.button(QMessageBox::Discard);
		cancel->setMinimumWidth(75);
		cancel->setMinimumHeight(23);
		if (msg.exec() == QMessageBox::Discard)
		{
			//不存储
		}
		else
		{
			//存储
			slotActionSave();
		}
	}
	_changed = false;
	slotActionClose();
	OpenFileDialog dlg;
	if(dlg.exec() == QDialog::Accepted)
	{
		QString fileName = dlg.getSelectFile();
		if(!fileName.isEmpty() && fileName != _currentUiFile)
		{
			QDir currentDir = QDir::current();
			QString itfDir = GlobalConfig::getSingletonPtr()->value("bui_dir", "bui").toString();
			currentDir.cd(itfDir);
			QFileInfo fileInfo(currentDir, fileName + ".bui");
			Q_ASSERT(fileInfo.exists());
			if(_centralWidget->open(fileInfo.absoluteFilePath()))
			{
				_currentUiFile = fileInfo.absoluteFilePath();
				_curDoc = _currentUiFile;
			}
		}
	}
	_changed = false;
}
