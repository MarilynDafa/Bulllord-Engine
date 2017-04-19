#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QStyleOption>
#include <QtGui/QPainter>
#include <QtWidgets/QComboBox>
#include <QtCore/QDir>
#include <qmessagebox.h>
#include <QStandardPaths> 
#include <qfontdialog.h>
#include "macro.h"
#include "PropertyEditorFactory.h"
#include "PropertyEditerDockWidget.h"
#include "SkinSetDialog.h"
#include "UIMgr.h"
static inline void setupTreeViewEditorMargin(QLayout *lt)
{
	enum { DecorationMargin = 4 };
	if (QApplication::layoutDirection() == Qt::LeftToRight)
		lt->setContentsMargins(DecorationMargin, 0, 0, 0);
	else
		lt->setContentsMargins(0, 0, DecorationMargin, 0);
}

//////////////////////////////////////////////////////////////////////////
SkinEditorWidget::SkinEditorWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	, _label(0)
	, _button(0)
{
	initUi();
	initSignal();
}

void SkinEditorWidget::initUi()
{
	_label = new QLabel;
	_button = new QToolButton;

	QHBoxLayout *layout = new QHBoxLayout(this);
	setupTreeViewEditorMargin(layout);
	layout->setSpacing(0);
	layout->addWidget(_label);
	layout->addWidget(_button);
	//layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

	_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
	_button->setFixedWidth(20);
	setFocusProxy(_button);
	setFocusPolicy(_button->focusPolicy());
	_button->setText(tr("..."));
	_button->installEventFilter(this);

	_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	_label->setText(QString());
}

void SkinEditorWidget::initSignal()
{
	connect(_button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
}

void SkinEditorWidget::slotButtonClicked()
{
	QString defdir = QDir::currentPath();
	defdir += "/pixmap";
	QString pathName = QFileDialog::getOpenFileName(this, tr("Open File"), defdir, tr("Bulllord Texture File (*.bmg)"));
	if(!pathName.isEmpty())
	{
		SkinFile skinFile(pathName);
		setValue(skinFile);
		valueChanged(skinFile);
	}
}

void SkinEditorWidget::setValue( const SkinFile &skinFile )
{
	//_skinFile = skinFile;
	_label->setText(skinFile.getSkinName());
}

bool SkinEditorWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj == _button) {
		switch (ev->type()) {
		case QEvent::KeyPress:
		case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
			switch (static_cast<const QKeyEvent*>(ev)->key()) {
			case Qt::Key_Escape:
			case Qt::Key_Enter:
			case Qt::Key_Return:
				ev->ignore();
				return true;
			default:
				break;
			}
		}break;
		default:
			break;
		}
	}
	return QWidget::eventFilter(obj, ev);
}

void SkinEditorWidget::paintEvent( QPaintEvent *pe )
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//////////////////////////////////////////////////////////////////////////
void SkinEditorFactoryPrivate::slotPropertyChanged( QtProperty *property, const SkinFile &value )
{
	if(!_createdEditors.contains(property))
		return;
	QListIterator<SkinEditorWidget *> itEditor(_createdEditors[property]);
	while(itEditor.hasNext())
	{
		SkinEditorWidget *w = itEditor.next();
		w->setValue(value);
	}
}

void SkinEditorFactoryPrivate::slotSetValue( const SkinFile &value )
{
	QObject *object = q_ptr->sender();
	const QMap<SkinEditorWidget *, QtProperty *>::ConstIterator  ecend = _editorToProperty.constEnd();
	for (QMap<SkinEditorWidget *, QtProperty *>::ConstIterator itEditor = _editorToProperty.constBegin(); itEditor !=  ecend; ++itEditor) {
		if (itEditor.key() == object) {
			QtProperty *property = itEditor.value();
			SkinFilePropertyManager *manager = q_ptr->propertyManager(property);
			if (!manager)
				return;
			manager->setValue(property, QVariant::fromValue<SkinFile>(value));
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
SkinEditorFactory::SkinEditorFactory( QObject *parent /*= 0*/ )
	: QtAbstractEditorFactory<SkinFilePropertyManager>(parent)
{
	d_ptr = new SkinEditorFactoryPrivate();
	d_ptr->q_ptr = this;
}

SkinEditorFactory::~SkinEditorFactory()
{
	qDeleteAll(d_ptr->_editorToProperty.keys());
	delete d_ptr;
}

void SkinEditorFactory::connectPropertyManager( SkinFilePropertyManager *manager )
{
	connect(manager, SIGNAL(valueChanged(QtProperty*, const SkinFile&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinFile&)));
}

QWidget * SkinEditorFactory::createEditor( SkinFilePropertyManager *manager, QtProperty *property, QWidget *parent )
{
	SkinEditorWidget *editor = d_ptr->createEditor(property, parent);
	editor->setValue(manager->value(property));

	connect(editor, SIGNAL(valueChanged(const SkinFile&)), this, SLOT(slotSetValue(const SkinFile&)));
	connect(editor, SIGNAL(destroyed(QObject *)),
		this, SLOT(slotEditorDestroyed(QObject *)));
	return editor;
}

void SkinEditorFactory::disconnectPropertyManager( SkinFilePropertyManager *manager )
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty*, const SkinFile&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinFile&)));
}

//////////////////////////////////////////////////////////////////////////
FontFileEditorWidget::FontFileEditorWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	, _comboBox(0)
	, _button(0)
{
	initUi();
	initSignal();
}

void FontFileEditorWidget::initUi()
{
	_comboBox = new QComboBox;
	_button = new QToolButton;

	QHBoxLayout *layout = new QHBoxLayout(this);
	setupTreeViewEditorMargin(layout);
	layout->setSpacing(0);
	layout->addWidget(_comboBox);
	layout->addWidget(_button);
	//layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

	_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
	_button->setFixedWidth(20);
	setFocusProxy(_button);
	setFocusPolicy(_button->focusPolicy());
	_button->setText(tr("..."));
	_button->installEventFilter(this);

	_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	QStringList dataList = FontType::getFontsNameList();
	_comboBox->addItems(dataList);
}

void FontFileEditorWidget::initSignal()
{
	connect(_button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
	connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged(int)));
}

void FontFileEditorWidget::slotButtonClicked()
{
	QString defdir = QDir::currentPath();
	defdir += "/font";
	QString pathName = QFileDialog::getOpenFileName(this, tr("Open Font File"), defdir, tr("Font File (*.ttf *.ttc)"));
	if(!pathName.isEmpty())
	{

		QString sknDirStr = GlobalConfig::getSingletonPtr()->value("fnt_dir", "font").toString();
		QDir currentDir = QDir::current();
		QDir sknDir = currentDir;
		sknDir.cd(sknDirStr);

		QFileInfo fileInfo(pathName);
		QDir srcDir = fileInfo.absoluteDir();
		if (sknDir != srcDir)
		{
			QFileInfo skinFileInfo(sknDir, fileInfo.fileName());
			if (!skinFileInfo.exists())
			{
				QFile::copy(pathName, skinFileInfo.absoluteFilePath());
				pathName = skinFileInfo.absoluteFilePath();
			}
			else
				pathName = skinFileInfo.absoluteFilePath();
		}


		FontFile fontFile = FontType::addFont(pathName);
		_comboBox->clear();
		_comboBox->addItems(FontType::getFontsNameList());
		_comboBox->setCurrentIndex(FontType::getFontFileIndex(fontFile));
	}
}

void FontFileEditorWidget::setValue( const FontFile &fontFile )
{
	_comboBox->setCurrentIndex(FontType::getFontFileIndex(fontFile));
}

bool FontFileEditorWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj == _button) {
		switch (ev->type()) {
		case QEvent::KeyPress:
		case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
			switch (static_cast<const QKeyEvent*>(ev)->key()) {
			case Qt::Key_Escape:
			case Qt::Key_Enter:
			case Qt::Key_Return:
				ev->ignore();
				return true;
			default:
				break;
			}
		}break;
		default:
			break;
		}
	}
	return QWidget::eventFilter(obj, ev);
}

void FontFileEditorWidget::paintEvent( QPaintEvent *pe )
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FontFileEditorWidget::slotCurrentIndexChanged( int index )
{
	valueChanged(FontType::getFontFile(index));
}
//////////////////////////////////////////////////////////////////////////
void FontFileEditorFactoryPrivate::slotPropertyChanged( QtProperty *property, const FontFile &value )
{
	if(!_createdEditors.contains(property))
		return;
	QListIterator<FontFileEditorWidget *> itEditor(_createdEditors[property]);
	while(itEditor.hasNext())
	{
		FontFileEditorWidget *w = itEditor.next();
		w->setValue(value);
	}
}

void FontFileEditorFactoryPrivate::slotSetValue( const FontFile &value )
{
	QObject *object = q_ptr->sender();
	const QMap<FontFileEditorWidget *, QtProperty *>::ConstIterator  ecend = _editorToProperty.constEnd();
	for (QMap<FontFileEditorWidget *, QtProperty *>::ConstIterator itEditor = _editorToProperty.constBegin(); itEditor !=  ecend; ++itEditor) {
		if (itEditor.key() == object) {
			QtProperty *property = itEditor.value();
			FontFilePropertyManager *manager = q_ptr->propertyManager(property);
			if (!manager)
				return;
			manager->setValue(property, QVariant::fromValue<FontFile>(value));
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
FontFileEditorFactory::FontFileEditorFactory( QObject *parent /*= 0*/ )
	: QtAbstractEditorFactory<FontFilePropertyManager>(parent)
{
	d_ptr = new FontFileEditorFactoryPrivate();
	d_ptr->q_ptr = this;
}

FontFileEditorFactory::~FontFileEditorFactory()
{
	qDeleteAll(d_ptr->_editorToProperty.keys());
	delete d_ptr;
}

void FontFileEditorFactory::connectPropertyManager( FontFilePropertyManager *manager )
{
	connect(manager, SIGNAL(valueChanged(QtProperty*, const FontFile&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const FontFile&)));
}

QWidget * FontFileEditorFactory::createEditor( FontFilePropertyManager *manager, QtProperty *property, QWidget *parent )
{
	FontFileEditorWidget *editor = d_ptr->createEditor(property, parent);
	editor->setValue(manager->value(property));

	connect(editor, SIGNAL(valueChanged(const FontFile&)), this, SLOT(slotSetValue(const FontFile&)));
	connect(editor, SIGNAL(destroyed(QObject *)),
		this, SLOT(slotEditorDestroyed(QObject *)));
	return editor;
}

void FontFileEditorFactory::disconnectPropertyManager( FontFilePropertyManager *manager )
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty*, const FontFile&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const FontFile&)));
}

//////////////////////////////////////////////////////////////////////////

SkinSetWidget::SkinSetWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	, _label(0)
	, _button(0)
{
	initUi();
	initSignal();
}

void SkinSetWidget::initUi()
{
	_label = new QLabel;
	_button = new QToolButton;

	QHBoxLayout *layout = new QHBoxLayout(this);
	setupTreeViewEditorMargin(layout);
	layout->setSpacing(0);
	layout->addWidget(_label);
	layout->addWidget(_button);
	//layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

	_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
	_button->setFixedWidth(20);
	setFocusProxy(_button);
	setFocusPolicy(_button->focusPolicy());
	_button->setText(tr("..."));
	_button->installEventFilter(this);

	_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	_label->setText(QString());
}

void SkinSetWidget::initSignal()
{
	connect(_button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
}

void SkinSetWidget::slotButtonClicked()
{
	UiWidget *uiwidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	assert(uiwidget != NULL);
	const SkinFile &skinFile = uiwidget->getSkinFile();
	QImage skinImage = skinFile.getImage(_skinSet.getSkinName());
	if (skinImage.byteCount())
	{
		SkinSetDialog dlg;
		dlg.setImage(skinImage, QSize(uiwidget->getWidgetWidth(), uiwidget->getWidgetHeight()), _skinSet);
		float orgX, orgY, w, h;
		if (QDialog::Accepted == dlg.exec())
		{
			_skinSet.setRect(dlg.getSelectRect(orgX, orgY, w, h));
			_skinSet.setPoint(QPointF(orgX, orgY));
			_skinSet.setSize(QSizeF(w, h));
			valueChanged(_skinSet);
		}
	}
	else
	{
		QMessageBox messageBox(QMessageBox::Warning, "Warning:", "Please set pixmap and tag first", QMessageBox::Ok , this);
		messageBox.button(QMessageBox::Ok)->setMinimumWidth(75);
		messageBox.button(QMessageBox::Ok)->setMinimumHeight(23);
		messageBox.button(QMessageBox::Ok)->setMaximumWidth(75);
		messageBox.button(QMessageBox::Ok)->setMaximumHeight(23);
		if (messageBox.exec() == QMessageBox::Cancel)
			return;
	}
}

void SkinSetWidget::setValue( const SkinSet &skinSet )
{
	UiWidget *uiwidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	assert(uiwidget != NULL);
	_skinFileName = uiwidget->getSkinFileName();
	_skinSet = skinSet;
	_label->setText(skinSet.getString());
}

bool SkinSetWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj == _button) {
		switch (ev->type()) {
		case QEvent::KeyPress:
		case QEvent::KeyRelease: { // Prevent the QToolButton from handling Enter/Escape meant control the delegate
			switch (static_cast<const QKeyEvent*>(ev)->key()) {
			case Qt::Key_Escape:
			case Qt::Key_Enter:
			case Qt::Key_Return:
				ev->ignore();
				return true;
			default:
				break;
			}
								 }break;
		default:
			break;
		}
	}
	return QWidget::eventFilter(obj, ev);
}

void SkinSetWidget::paintEvent( QPaintEvent *pe )
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//////////////////////////////////////////////////////////////////////////
void SkinSetFactoryPrivate::slotPropertyChanged( QtProperty *property, const SkinSet &value )
{
	if(!_createdEditors.contains(property))
		return;
	QListIterator<SkinSetWidget *> itEditor(_createdEditors[property]);
	while(itEditor.hasNext())
	{
		SkinSetWidget *w = itEditor.next();
		w->setValue(value);
	}
}

void SkinSetFactoryPrivate::slotSetValue( const SkinSet &value )
{
	QObject *object = q_ptr->sender();
	const QMap<SkinSetWidget *, QtProperty *>::ConstIterator  ecend = _editorToProperty.constEnd();
	for (QMap<SkinSetWidget *, QtProperty *>::ConstIterator itEditor = _editorToProperty.constBegin(); itEditor !=  ecend; ++itEditor) {
		if (itEditor.key() == object) {
			QtProperty *property = itEditor.value();
			SkinSetPropertyManager *manager = q_ptr->propertyManager(property);
			if (!manager)
				return;
			manager->setValue(property, QVariant::fromValue<SkinSet>(value));
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
SkinSetFactory::SkinSetFactory( QObject *parent /*= 0*/ )
	: QtAbstractEditorFactory<SkinSetPropertyManager>(parent)
{
	d_ptr = new SkinSetFactoryPrivate();
	d_ptr->q_ptr = this;
}

SkinSetFactory::~SkinSetFactory()
{
	qDeleteAll(d_ptr->_editorToProperty.keys());
	delete d_ptr;
}

void SkinSetFactory::connectPropertyManager( SkinSetPropertyManager *manager )
{
	connect(manager, SIGNAL(valueChanged(QtProperty*, const SkinSet&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinSet&)));
}

QWidget * SkinSetFactory::createEditor( SkinSetPropertyManager *manager, QtProperty *property, QWidget *parent )
{
	SkinSetWidget *editor = d_ptr->createEditor(property, parent);
	editor->setValue(manager->value(property));

	connect(editor, SIGNAL(valueChanged(const SkinSet&)), this, SLOT(slotSetValue(const SkinSet&)));
	connect(editor, SIGNAL(destroyed(QObject *)),
		this, SLOT(slotEditorDestroyed(QObject *)));
	return editor;
}

void SkinSetFactory::disconnectPropertyManager( SkinSetPropertyManager *manager )
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty*, const SkinSet&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinSet&)));
}

//////////////////////////////////////////////////////////////////////////

SkinImageEditorWidget::SkinImageEditorWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
	, _comboBox(0)
{
	initUi();
	initSignal();
}

void SkinImageEditorWidget::initUi()
{
	_comboBox = new QComboBox;
	_preview = new QLabel;

	QHBoxLayout *layout = new QHBoxLayout(this);
	setupTreeViewEditorMargin(layout);
	layout->setSpacing(0);
	layout->addWidget(_preview);
	layout->addWidget(_comboBox);
	_preview->setMinimumWidth(80);
	_preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	UiWidget *uiWidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	assert(uiWidget != NULL);
	QStringList dataList = uiWidget->getSkinNames();
	_comboBox->addItems(dataList);
}

void SkinImageEditorWidget::initSignal()
{
	connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged(int)));
	connect(_comboBox, SIGNAL(highlighted(const QString &)), this, SLOT(slothighlightedChanged(const QString &)));
}

void SkinImageEditorWidget::setValue( const SkinImageEx &skinImage )
{
	UiWidget *uiWidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	assert(uiWidget != NULL);
	QStringList names = uiWidget->getSkinNames();
	
	_comboBox->setCurrentIndex(names.indexOf(skinImage.getSkinName()));
}

void SkinImageEditorWidget::setValue( const SkinImage &skinImage )
{
	UiWidget *uiWidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	assert(uiWidget != NULL);
	QStringList names = uiWidget->getSkinNames();

	_comboBox->setCurrentIndex(names.indexOf(skinImage.getSkinName()));
}

void SkinImageEditorWidget::slothighlightedChanged(const QString& text)
{
	UiWidget *uiWidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	const SkinFile &skinFile = uiWidget->getSkinFile();
	QImage skinImage = skinFile.getImage(text); 
	QPixmap pre = QPixmap::fromImage(skinImage);
	pre = pre.scaled(_preview->size());
	_preview->setPixmap(pre);
	//dlg.activateWindow();
}

void SkinImageEditorWidget::slotCurrentIndexChanged( int index )
{
	if(index < 0)
		return;

	UiWidget *uiWidget = PropertyEditerDockWidget::getCurrentSelectWidget();
	assert(uiWidget != NULL);
	c_widget* wid = uiWidget->getWidget();
	c_skin* skin = o2d::c_ui_manager::get_singleton_ptr()->get_skin(wid->get_skinid());
	SkinImageEx skex(uiWidget->getSkinNames()[index]);
	c_rect txrect = skin->get_texcoord(UiWidget::fromQString(uiWidget->getSkinNames()[index]));
	SkinSet skinset;
	skinset.setPoint(QPoint(0, 0));
	skinset.setSize(QSizeF(txrect.width(), txrect.height()));
	skinset.setRect(QRectF(0, 0, txrect.width(), txrect.height()));
	skinset.setSkinName(uiWidget->getSkinNames()[index]);
	skex.setSkinSet(skinset);
	valueChanged(skex);
	valueChanged(SkinImage(uiWidget->getSkinNames()[index]));
}

void SkinImageEditorWidget::paintEvent( QPaintEvent *pe )
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


//////////////////////////////////////////////////////////////////////////
void SkinImageExEditorFactoryPrivate::slotPropertyChanged( QtProperty *property, const SkinImageEx &value )
{
	if(!_createdEditors.contains(property))
		return;
	QListIterator<SkinImageEditorWidget *> itEditor(_createdEditors[property]);
	while(itEditor.hasNext())
	{
		SkinImageEditorWidget *w = itEditor.next();
		w->setValue(value);
	}
}

void SkinImageExEditorFactoryPrivate::slotSetValue( const SkinImageEx &value )
{
	QObject *object = q_ptr->sender();
	const QMap<SkinImageEditorWidget *, QtProperty *>::ConstIterator  ecend = _editorToProperty.constEnd();
	for (QMap<SkinImageEditorWidget *, QtProperty *>::ConstIterator itEditor = _editorToProperty.constBegin(); itEditor !=  ecend; ++itEditor) {
		if (itEditor.key() == object) {
			QtProperty *property = itEditor.value();
			SkinImageExPropertyManager *manager = q_ptr->propertyManager(property);
			if (!manager)
				return;
			manager->setValue(property, QVariant::fromValue<SkinImageEx>(value));
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
SkinImageExEditorFactory::SkinImageExEditorFactory( QObject *parent /*= 0*/ )
	: QtAbstractEditorFactory<SkinImageExPropertyManager>(parent)
{
	d_ptr = new SkinImageExEditorFactoryPrivate();
	d_ptr->q_ptr = this;
}

SkinImageExEditorFactory::~SkinImageExEditorFactory()
{
	qDeleteAll(d_ptr->_editorToProperty.keys());
	delete d_ptr;
}

void SkinImageExEditorFactory::connectPropertyManager( SkinImageExPropertyManager *manager )
{
	connect(manager, SIGNAL(valueChanged(QtProperty*, const SkinImageEx&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinImageEx&)));
}

QWidget * SkinImageExEditorFactory::createEditor( SkinImageExPropertyManager *manager, QtProperty *property, QWidget *parent )
{
	SkinImageEditorWidget *editor = d_ptr->createEditor(property, parent);
	editor->setValue(manager->value(property));

	connect(editor, SIGNAL(valueChanged(const SkinImageEx&)), this, SLOT(slotSetValue(const SkinImageEx&)));
	connect(editor, SIGNAL(destroyed(QObject *)),
		this, SLOT(slotEditorDestroyed(QObject *)));
	return editor;
}

void SkinImageExEditorFactory::disconnectPropertyManager( SkinImageExPropertyManager *manager )
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty*, const SkinImageEx&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinImageEx&)));
}


//////////////////////////////////////////////////////////////////////////
void SkinImageEditorFactoryPrivate::slotPropertyChanged( QtProperty *property, const SkinImage &value )
{
	if(!_createdEditors.contains(property))
		return;
	QListIterator<SkinImageEditorWidget *> itEditor(_createdEditors[property]);
	while(itEditor.hasNext())
	{
		SkinImageEditorWidget *w = itEditor.next();
		w->setValue(value);
	}
}

void SkinImageEditorFactoryPrivate::slotSetValue( const SkinImage &value )
{
	QObject *object = q_ptr->sender();
	const QMap<SkinImageEditorWidget *, QtProperty *>::ConstIterator  ecend = _editorToProperty.constEnd();
	for (QMap<SkinImageEditorWidget *, QtProperty *>::ConstIterator itEditor = _editorToProperty.constBegin(); itEditor !=  ecend; ++itEditor) {
		if (itEditor.key() == object) {
			QtProperty *property = itEditor.value();
			SkinImagePropertyManager *manager = q_ptr->propertyManager(property);
			if (!manager)
				return;
			manager->setValue(property, QVariant::fromValue<SkinImage>(value));
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
SkinImageEditorFactory::SkinImageEditorFactory( QObject *parent /*= 0*/ )
	: QtAbstractEditorFactory<SkinImagePropertyManager>(parent)
{
	d_ptr = new SkinImageEditorFactoryPrivate();
	d_ptr->q_ptr = this;
}

SkinImageEditorFactory::~SkinImageEditorFactory()
{
	qDeleteAll(d_ptr->_editorToProperty.keys());
	delete d_ptr;
}

void SkinImageEditorFactory::connectPropertyManager( SkinImagePropertyManager *manager )
{
	connect(manager, SIGNAL(valueChanged(QtProperty*, const SkinImage&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinImage&)));
}

QWidget * SkinImageEditorFactory::createEditor( SkinImagePropertyManager *manager, QtProperty *property, QWidget *parent )
{
	SkinImageEditorWidget *editor = d_ptr->createEditor(property, parent);
	editor->setValue(manager->value(property));

	connect(editor, SIGNAL(valueChanged(const SkinImage&)), this, SLOT(slotSetValue(const SkinImage&)));
	connect(editor, SIGNAL(destroyed(QObject *)),
		this, SLOT(slotEditorDestroyed(QObject *)));
	return editor;
}

void SkinImageEditorFactory::disconnectPropertyManager( SkinImagePropertyManager *manager )
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty*, const SkinImage&)),
		this,SLOT(slotPropertyChanged(QtProperty *, const SkinImage&)));
}
