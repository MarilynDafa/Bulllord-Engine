#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include "ui_BMGTexTool.h"
#include "Image.h"
#include "ImageWidget.h"
#include <qtimer.h>
#include <qlabel.h>
#include <QtWidgets/QMdiSubWindow>
#include <qactiongroup.h>


class BMGTexTool : public QMainWindow
{
    Q_OBJECT

public:
    BMGTexTool(QWidget *parent = Q_NULLPTR);
	void startAnimation();
	void stopAnimation();
	void save();
protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

	private slots:
	void openImageFiles(const QStringList &files);
	void openImageFile(const QString &file);
	ImageOp *getImageOpWidget(QMdiSubWindow *subWindow);
	ImageOp *currentImageOpWidget();
	void activeFace(FACE face);
	void setFaceActCheckState(FACE face);
	void activeCubeBar(bool active);
	void active3DBar(bool active);
	ImageWidget *currentImageWindow();
	void imageWindowActived(QMdiSubWindow *window);
    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionReset_triggered();

    void on_actionUpperMip_triggered();

    void on_actionLowerMip_triggered();

    void on_actionGenerateMip_triggered();

    void on_actionClean_triggered();

    void on_actionGenerateCube_triggered();

    void on_actionPx_triggered();

    void on_actionNx_triggered();

    void on_actionPy_triggered();

    void on_actionNy_triggered();

    void on_actionPz_triggered();

    void on_actionNz_triggered();

    void on_actionUpper3D_triggered();

    void on_actionGenerate3D_triggered();

    void on_actionLower3D_triggered();

    void on_actionClose_triggered();

    void on_actionBatch_triggered();

	void on_actionCompress_triggered();

	void treeItemSelectedChanged();

	void treeItemEdited(QTreeWidgetItem *item, int column);

	ImageArea * getTreeItemData(QTreeWidgetItem *item);

	void imageAdded(ImageArea *imageArea);
	void imageAreaChanged(ImageArea *imageArea);
	void imageSelected(ImageArea *imageArea);
	void updatePixmap();

	
    void on_actionSave_As_triggered();

private:
	int m_nIndex;
    Ui::BMGTexToolClass ui;
	QMdiArea *_mdiArea;
	QActionGroup* m_pCmmdActGrp;
	QActionGroup* m_pCmmdActGrp2;
	QList<ImageArea*> _imageAreaList;
	QTimer* _qtime;
};
