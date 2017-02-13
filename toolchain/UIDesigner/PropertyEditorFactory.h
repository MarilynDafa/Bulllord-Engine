#ifndef PROPERTYEDITORFACTORY_H
#define PROPERTYEDITORFACTORY_H

#include <QtWidgets/QWidget>

#include "QtAbstractEditorFactoryBase"
#include "PropertyManager.h"

//////////////////////////////////////////////////////////////////////////
template <class Editor>
class AbstractEditorFactoryPrivate
{
public:
	typedef QList<Editor *> EditorList;
	typedef QMap<QtProperty *, EditorList> PropertyToEditorListMap;
	typedef QMap<Editor *, QtProperty *> EditorToPropertyMap;

	Editor *createEditor(QtProperty *property, QWidget *parent);
	void initializeEditor(QtProperty *property, Editor *e);
	void slotEditorDestroyed(QObject *object);

	PropertyToEditorListMap  _createdEditors;
	EditorToPropertyMap _editorToProperty;
};

template <class Editor>
Editor *AbstractEditorFactoryPrivate<Editor>::createEditor(QtProperty *property, QWidget *parent)
{
	Editor *editor = new Editor(parent);
	initializeEditor(property, editor);
	return editor;
}

template <class Editor>
void AbstractEditorFactoryPrivate<Editor>::initializeEditor(QtProperty *property, Editor *editor)
{
	typename PropertyToEditorListMap::iterator it = _createdEditors.find(property);
	if (it == _createdEditors.end())
		it = _createdEditors.insert(property, EditorList());
	it.value().append(editor);
	_editorToProperty.insert(editor, property);
}

template <class Editor>
void AbstractEditorFactoryPrivate<Editor>::slotEditorDestroyed(QObject *object)
{
	const typename EditorToPropertyMap::iterator ecend = _editorToProperty.end();
	for (typename EditorToPropertyMap::iterator itEditor = _editorToProperty.begin(); itEditor !=  ecend; ++itEditor) {
		if (itEditor.key() == object) {
			Editor *editor = itEditor.key();
			QtProperty *property = itEditor.value();
			const typename PropertyToEditorListMap::iterator pit = _createdEditors.find(property);
			if (pit != _createdEditors.end()) {
				pit.value().removeAll(editor);
				if (pit.value().empty())
					_createdEditors.erase(pit);
			}
			_editorToProperty.erase(itEditor);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
class QLabel;
class QToolButton;

class SkinEditorWidget : public QWidget
{
	Q_OBJECT
public:
	SkinEditorWidget(QWidget *parent = 0);
private:
	void initUi();
	void initSignal();
protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);
	virtual void paintEvent(QPaintEvent *pe);

signals:
	void valueChanged(const SkinFile &skinFile);
public slots:
	void slotButtonClicked();
	void setValue(const SkinFile &skinFile);

private:
	QLabel *_label;
	QToolButton *_button;
	//SkinFile _skinFile;
};

//////////////////////////////////////////////////////////////////////////
class SkinEditorFactory;

class SkinEditorFactoryPrivate : public AbstractEditorFactoryPrivate<SkinEditorWidget>
{
	SkinEditorFactory *q_ptr;
	Q_DECLARE_PUBLIC(SkinEditorFactory)
public:
	void slotPropertyChanged(QtProperty *property, const SkinFile &value);
	void slotSetValue(const SkinFile &value);
};

//////////////////////////////////////////////////////////////////////////
class SkinEditorFactory : public QtAbstractEditorFactory<SkinFilePropertyManager>
{
	Q_OBJECT
public:
	SkinEditorFactory(QObject *parent = 0);
	virtual ~SkinEditorFactory();
protected:
	void connectPropertyManager(SkinFilePropertyManager *manager);
	QWidget *createEditor(SkinFilePropertyManager *manager, QtProperty *property, QWidget *parent);
	void disconnectPropertyManager(SkinFilePropertyManager *manager);
private:
	SkinEditorFactoryPrivate *d_ptr;
	Q_DECLARE_PRIVATE(SkinEditorFactory)
	Q_DISABLE_COPY(SkinEditorFactory)
	Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const SkinFile&))
	Q_PRIVATE_SLOT(d_func(), void slotSetValue(const SkinFile&))
	Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

//////////////////////////////////////////////////////////////////////////
class QLabel;
class QToolButton;

class SkinSetWidget : public QWidget
{
	Q_OBJECT
public:
	SkinSetWidget(QWidget *parent = 0);
private:
	void initUi();
	void initSignal();

signals:
	void valueChanged(const SkinSet &skinSet);
protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);
	virtual void paintEvent(QPaintEvent *pe);

public slots:
	void slotButtonClicked();
	void setValue(const SkinSet &skinSet);

private:
	QLabel *_label;
	QToolButton *_button;
	QString _skinFileName;
	SkinSet _skinSet;
};

//////////////////////////////////////////////////////////////////////////
class SkinSetFactory;

class SkinSetFactoryPrivate : public AbstractEditorFactoryPrivate<SkinSetWidget>
{
	SkinSetFactory *q_ptr;
	Q_DECLARE_PUBLIC(SkinSetFactory)
public:
	void slotPropertyChanged(QtProperty *property, const SkinSet &value);
	void slotSetValue(const SkinSet &value);
};

//////////////////////////////////////////////////////////////////////////
class SkinSetFactory : public QtAbstractEditorFactory<SkinSetPropertyManager>
{
	Q_OBJECT
public:
	SkinSetFactory(QObject *parent = 0);
	virtual ~SkinSetFactory();
protected:
	void connectPropertyManager(SkinSetPropertyManager *manager);
	QWidget *createEditor(SkinSetPropertyManager *manager, QtProperty *property, QWidget *parent);
	void disconnectPropertyManager(SkinSetPropertyManager *manager);
private:
	SkinSetFactoryPrivate *d_ptr;
	Q_DECLARE_PRIVATE(SkinSetFactory)
	Q_DISABLE_COPY(SkinSetFactory)
	Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const SkinSet&))
	Q_PRIVATE_SLOT(d_func(), void slotSetValue(const SkinSet&))
	Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

//////////////////////////////////////////////////////////////////////////

class QComboBox;

class SkinImageEditorWidget : public QWidget
{
	Q_OBJECT
public:
	SkinImageEditorWidget(QWidget *parent = 0);
private:
	void initUi();
	void initSignal();

signals:
	void valueChanged(const SkinImageEx &skinImage);
	void valueChanged(const SkinImage &skinImage);
public slots:
	void setValue(const SkinImageEx &skinImage);
	void setValue(const SkinImage &skinImage);
	void slotCurrentIndexChanged(int index);
	void slothighlightedChanged(const QString& text);
	void paintEvent( QPaintEvent *pe );

private:
	QComboBox *_comboBox;
	QLabel* _preview;
};

//////////////////////////////////////////////////////////////////////////
class SkinImageExEditorFactory;

class SkinImageExEditorFactoryPrivate : public AbstractEditorFactoryPrivate<SkinImageEditorWidget>
{
	SkinImageExEditorFactory *q_ptr;
	Q_DECLARE_PUBLIC(SkinImageExEditorFactory)
public:
	void slotPropertyChanged(QtProperty *property, const SkinImageEx &value);
	void slotSetValue(const SkinImageEx &value);
};

//////////////////////////////////////////////////////////////////////////
class SkinImageExEditorFactory : public QtAbstractEditorFactory<SkinImageExPropertyManager>
{
	Q_OBJECT
public:
	SkinImageExEditorFactory(QObject *parent = 0);
	virtual ~SkinImageExEditorFactory();
protected:
	void connectPropertyManager(SkinImageExPropertyManager *manager);
	QWidget *createEditor(SkinImageExPropertyManager *manager, QtProperty *property, QWidget *parent);
	void disconnectPropertyManager(SkinImageExPropertyManager *manager);
private:
	SkinImageExEditorFactoryPrivate *d_ptr;
	Q_DECLARE_PRIVATE(SkinImageExEditorFactory)
	Q_DISABLE_COPY(SkinImageExEditorFactory)
	Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const SkinImageEx&))
	Q_PRIVATE_SLOT(d_func(), void slotSetValue(const SkinImageEx&))
	Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

//////////////////////////////////////////////////////////////////////////
class SkinImageEditorFactory;

class SkinImageEditorFactoryPrivate : public AbstractEditorFactoryPrivate<SkinImageEditorWidget>
{
	SkinImageEditorFactory *q_ptr;
	Q_DECLARE_PUBLIC(SkinImageEditorFactory)
public:
	void slotPropertyChanged(QtProperty *property, const SkinImage &value);
	void slotSetValue(const SkinImage &value);
};

//////////////////////////////////////////////////////////////////////////
class SkinImageEditorFactory : public QtAbstractEditorFactory<SkinImagePropertyManager>
{
	Q_OBJECT
public:
	SkinImageEditorFactory(QObject *parent = 0);
	virtual ~SkinImageEditorFactory();
protected:
	void connectPropertyManager(SkinImagePropertyManager *manager);
	QWidget *createEditor(SkinImagePropertyManager *manager, QtProperty *property, QWidget *parent);
	void disconnectPropertyManager(SkinImagePropertyManager *manager);
private:
	SkinImageEditorFactoryPrivate *d_ptr;
	Q_DECLARE_PRIVATE(SkinImageEditorFactory)
	Q_DISABLE_COPY(SkinImageEditorFactory)
	Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const SkinImage&))
	Q_PRIVATE_SLOT(d_func(), void slotSetValue(const SkinImage&))
	Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

//////////////////////////////////////////////////////////////////////////
class QComboBox;

class FontFileEditorWidget : public QWidget
{
	Q_OBJECT
public:
	FontFileEditorWidget(QWidget *parent = 0);
private:
	void initUi();
	void initSignal();
protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);
	virtual void paintEvent(QPaintEvent *pe);

signals:
	void valueChanged(const FontFile &fontFile);
public slots:
	void slotButtonClicked();
	void setValue(const FontFile &fontFile);
	void slotCurrentIndexChanged(int index);

private:
	QComboBox *_comboBox;
	QToolButton *_button;
};

//////////////////////////////////////////////////////////////////////////
class FontFileEditorFactory;

class FontFileEditorFactoryPrivate : public AbstractEditorFactoryPrivate<FontFileEditorWidget>
{
	FontFileEditorFactory *q_ptr;
	Q_DECLARE_PUBLIC(FontFileEditorFactory)
public:
	void slotPropertyChanged(QtProperty *property, const FontFile &value);
	void slotSetValue(const FontFile &value);
};

//////////////////////////////////////////////////////////////////////////
class FontFileEditorFactory : public QtAbstractEditorFactory<FontFilePropertyManager>
{
	Q_OBJECT
public:
	FontFileEditorFactory(QObject *parent = 0);
	virtual ~FontFileEditorFactory();
protected:
	void connectPropertyManager(FontFilePropertyManager *manager);
	QWidget *createEditor(FontFilePropertyManager *manager, QtProperty *property, QWidget *parent);
	void disconnectPropertyManager(FontFilePropertyManager *manager);
private:
	FontFileEditorFactoryPrivate *d_ptr;
	Q_DECLARE_PRIVATE(FontFileEditorFactory)
	Q_DISABLE_COPY(FontFileEditorFactory)
	Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const FontFile&))
	Q_PRIVATE_SLOT(d_func(), void slotSetValue(const FontFile&))
	Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

#endif //PROPERTYEDITORFACTORY_H