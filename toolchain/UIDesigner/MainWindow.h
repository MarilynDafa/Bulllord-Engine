#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/qstatusbar.h>
#include <QtCore/QMap>
#include "GeneratedFiles/ui_mainwindow.h"
#include "GeneratedFiles/ui_workspaceselectdialog.h"
#include "GeneratedFiles/ui_setdialog.h"
#include "GeneratedFiles/ui_SaveSetDialog.h"
#include "GeneratedFiles\ui_WVGAWidget.h"
#include "Widget/UiWidget.h"



class SaveSetDialog : public QDialog , public Ui::SaveSetDialog
{
	Q_OBJECT
public:
	SaveSetDialog(QWidget *widget = 0);
	~SaveSetDialog();

	const QString &getFileName() const { return _fileName; }

protected slots:
	void slotOkBtnClicked();

private:
	void initUi();
	void initSignal();

private:
	QString _fileName;
};

class WorkspaceSelectDialog : public QDialog , public Ui::WorkspaceSelectDialog
{
	Q_OBJECT
public:
	WorkspaceSelectDialog(QDialog *parent = 0);
	QString getSelectedWorkSpace() const
	{
		return _workSpace;
	}

protected slots:
	void slotBrowseBtnClicked();
	void slotOkBtnClicked();

private:
	QStringList getWorkSpaces();
	void writeToConfig(const QStringList &workSpaces);

private:
	QString _workSpace;
};

class WXGAWidget : public QWidget, public Ui_CompressBarWidget
{
	Q_OBJECT
public:
	WXGAWidget(QWidget *parent = 0);
};

class QtVariantEditorFactory;
class QtVariantPropertyManager;

class SetDialog : public QDialog , public Ui::SetDialog
{
	Q_OBJECT
public:
	SetDialog(QWidget *parent = 0);
private:
	void initUi();
	void initSignal();
	void updateMap();
protected slots:
	void slotValueChanged(QtProperty *property, const QVariant &value);
	void slotCancelBtnClicked();
	void slotOkBtnClicked();
private:
	QMap<QString, QColor> _map;
	QtVariantPropertyManager *_manager;
	QtVariantEditorFactory *_factory;
	int w, h;
};
class WidgetBoxDockWidget;
class PropertyEditerDockWidget;
class ObjectViewerDockWidget;
class CentralWidget;

class MainWindow : public QMainWindow , public Ui::MainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	static MainWindow* getInst();
	void slotDirty();
	void lockControl(bool x, bool y);
private:
	void initUi();
	void initSignal();
	void initUUColor(int &, int&);
	void initUUColorItem(const QString &key, int item);
	QString colorToString(const QColor &color);
	QColor stringToColor(const QString &colorStr);
	bool contains(QObject *w1, QObject *w2);
	QWidget* setWidgetEmitSignals(QWidget *widget);
	void enableAlignAction(int topWidgetCount, int selectWidgetCount, int childsCount);
	void enableEditAction(int selectWidgetCount);
	void setBackImage(const QString &imageFile);
protected:
	virtual void paintEvent(QPaintEvent *event);


protected slots:
	void slotFocusChanged(QWidget *old, QWidget *now);
	void slotWidgetSelectedChanged(const QVector<UiWidget*> &widget);
	void slotClipBoradWidgetsChanged(const QVector<UiWidget*> &widgets);
	void slotWXGAChanged(QString str);

	void slotActionOpen();
	void slotActionCut();
	void slotActionCopy();
	void slotActionPaste();
	void slotActionSet();
	void slotActionSave();
	void slotActionSaveAs();
	void slotActionImport();
	void slotActionBackImage(bool checked);
	void slotAbout();
	void slotActionExit();
	void slotActionClose();
	void showCursorPos(QPoint* pt);

	void resizeCanvas(int w, int h);

private:
	WXGAWidget* _wxgaWidget;
	WidgetBoxDockWidget *_widgetBoxDockWidget;
	PropertyEditerDockWidget *_properEditerDockWidget;
	ObjectViewerDockWidget *_objectViewerDockWidget;
	CentralWidget *_centralWidget;
	QWidget *_widgetEmitSignals;
	QAction *_actWidgetBox;
	QAction *_actObjectViewer;
	QAction *_actPropertyEditer;
	QAction *_actToolbar;
	QString _currentUiFile;
	QStatusBar* statusBar;
	int width, height;

	static MainWindow* _self;

	int _DesignW;
	int _DesignH;
	QString _curDoc;
	bool _changed;
};

#endif //MAINWINDOW_H