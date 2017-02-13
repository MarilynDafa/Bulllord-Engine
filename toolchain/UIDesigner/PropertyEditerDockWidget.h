#ifndef PROPERTYEDITERDOCKWIDGET_H
#define PROPERTYEDITERDOCKWIDGET_H

#include <assert.h>
#include <QtCore/QMap>

#include "QtVariantProperty"

#include "GeneratedFiles/ui_propertyediterdockwidget.h"

class UiWidget;
class PropertyItem;
class PorpertyListDelegate;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QtProperty;
class AbstractPropertyManager;
class FontType;
class NumericSet;
class PropertyEditerDockWidget : public QWidget , public Ui::PropertyEditerDockWidget
{
	Q_OBJECT
public:
	PropertyEditerDockWidget(QWidget *parent = 0);
	void setUiWidget(UiWidget *uiWidget);
	void emitSignals(bool yes);
	void clean();
private:
	void initUi();
	void initSignal();
	void initProperty();
	void clearPropertyTree();
	QtProperty* addPropertyAndSetValue(int propertyType, QString &propertyName, const QVariant &value = QVariant());
	void connectFactoryToManager(QtVariantPropertyManager *manager, QtVariantEditorFactory *factory);
	template<typename manager>
	void connectFactoryToManager(manager *m, QtAbstractEditorFactory<manager> *f = 0);
signals:
	void widgetPropertyChanged(UiWidget *widget, const QString &property, const QVariant &value);
	void dirty();
protected slots:
	void slotWidgetSelectedChanged(const QVector<UiWidget*> &widgets);
	void slotWidgetNameChanged(UiWidget *widget, const QString &name);
	void slotWidgetPropertyChanged(UiWidget *widget, const QString &property, const QVariant &value);
	void slotPropertyValueChanged(const QString &property, const QVariant &value);
	void slotUserEnumNamesChanged(const QString &propertyName, const QVariant &value);
private slots:
	void slotPropertyValueChanged(QtProperty *property, const QVariant &value);

public:
	static UiWidget *getCurrentSelectWidget() { return _uiWidget; }
	static PropertyEditerDockWidget* getInstance() { return self; }
	void setPropertyValue(const QString &propertyName, const QVariant &value);
	void setPropertyValue(QtProperty *property, const QVariant &value);
	void setPropertyXX(const QString &propertyName, const QVariant &value);
	void setPropertyFont(const QString &propertyName, const FontType &value);
	void setPropertyNumic(const QString &propertyName, const NumericSet &value);
	QtAbstractPropertyManager* getPtAlignMge();
	QtAbstractPropertyManager* getOriAlignMge();
	QtProperty *findProperty(const QString &propertyName);

private:
	PropertyItem *_selectedTreeItem;
	QList<QtAbstractPropertyManager*> _managersList;
	QMap<const QtVariantProperty*, int> _userEnumProperties;
	QMap<QString, QtProperty*> _nameToProperty;
	static UiWidget *_uiWidget;
	static PropertyEditerDockWidget* self;
};

template<typename manager>
void PropertyEditerDockWidget::connectFactoryToManager( manager *m, QtAbstractEditorFactory<manager> *f /*= 0*/ )
{
	if(f != 0)
		_tree->setFactoryForManager(m, f);
	connect(m, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
		this, SLOT(slotPropertyValueChanged(QtProperty*, const QVariant&)));

	m->connectSubManagerFactory(_tree);
	_managersList.append(m);
}


#endif //PROPERTYEDITERDOCKWIDGET_H