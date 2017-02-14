/********************************************************************************
** Form generated from reading UI file 'BMGTexTool.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BMGTEXTOOL_H
#define UI_BMGTEXTOOL_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BMGTexToolClass
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionReset;
    QAction *actionUpperMip;
    QAction *actionLowerMip;
    QAction *actionGenerateMip;
    QAction *actionClean;
    QAction *actionGenerateCube;
    QAction *actionPx;
    QAction *actionNx;
    QAction *actionPy;
    QAction *actionNy;
    QAction *actionPz;
    QAction *actionNz;
    QAction *actionUpper3D;
    QAction *actionGenerate3D;
    QAction *actionLower3D;
    QAction *actionAbout;
    QAction *actionClose;
    QAction *actionBatch;
    QAction *actionASTC;
    QAction *actionS3TC;
    QAction *actionETC2;
    QAction *actionCompress;
    QAction *actionSave_As;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuMipmaps;
    QMenu *menuCubeMap;
    QMenu *menu3D;
    QMenu *menuAbout;
    QMenu *menuCompress;
    QStatusBar *statusBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTreeWidget *treeWidget;
    QLabel *label;

    void setupUi(QMainWindow *BMGTexToolClass)
    {
        if (BMGTexToolClass->objectName().isEmpty())
            BMGTexToolClass->setObjectName(QStringLiteral("BMGTexToolClass"));
        BMGTexToolClass->resize(1024, 768);
        BMGTexToolClass->setMinimumSize(QSize(1024, 768));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        BMGTexToolClass->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/BMGTexTool/Resources/TX.png"), QSize(), QIcon::Normal, QIcon::Off);
        BMGTexToolClass->setWindowIcon(icon);
        BMGTexToolClass->setStyleSheet(QLatin1String("/*\n"
"	Copyright 2013 Emanuel Claesson\n"
"\n"
"	Licensed under the Apache License, Version 2.0 (the \"License\");\n"
"	you may not use this file except in compliance with the License.\n"
"	You may obtain a copy of the License at\n"
"\n"
"		http://www.apache.org/licenses/LICENSE-2.0\n"
"\n"
"	Unless required by applicable law or agreed to in writing, software\n"
"	distributed under the License is distributed on an \"AS IS\" BASIS,\n"
"	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
"	See the License for the specific language governing permissions and\n"
"	limitations under the License.\n"
"*/\n"
"\n"
"/*\n"
"	COLOR_DARK     = #1e1e1e\n"
"	COLOR_MEDIUM   = #2d2d30\n"
"	COLOR_MEDLIGHT = #333337\n"
"	COLOR_LIGHT    = #FFFFFF\n"
"	COLOR_ACCENT   = #3f3f46\n"
"*/\n"
"\n"
"* {\n"
"	background: #1e1e1e;\n"
"	color: #FFFFFF;\n"
"	border: 1px solid #333337;\n"
"}\n"
"\n"
"QWidget::item:selected {\n"
"	background: #3f3f46;\n"
"}\n"
"\n"
"QCheckBox, QRadioButton {\n"
"	border: none;\n"
"}\n"
""
                        "\n"
"QRadioButton::indicator, QCheckBox::indicator {\n"
"	width: 13px;\n"
"	height: 13px;\n"
"}\n"
"\n"
"QRadioButton::indicator::unchecked, QCheckBox::indicator::unchecked {\n"
"	border: 1px solid #333337;\n"
"	background: none;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked:hover, QCheckBox::indicator:unchecked:hover {\n"
"	border: 1px solid #FFFFFF;\n"
"}\n"
"\n"
"QRadioButton::indicator::checked, QCheckBox::indicator::checked {\n"
"	border: 1px solid #333337;\n"
"	background: #333337;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked:hover, QCheckBox::indicator:checked:hover {\n"
"	border: 1px solid #FFFFFF;\n"
"	background: #FFFFFF;\n"
"}\n"
"\n"
"QGroupBox {\n"
"	margin-top: 6px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"	top: -7px;\n"
"	left: 7px;\n"
"}\n"
"\n"
"QScrollBar {\n"
"	border: 1px solid #686868;\n"
"	background: #1e1e1e;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"	height: 15px;\n"
"	margin: 0px 0px 0px 32px;\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"	width: 15px;\n"
"	margin: 32px 0px 0px 0px;\n"
""
                        "}\n"
"\n"
"QScrollBar::handle {\n"
"	background: #2d2d30;\n"
"	border: 1px solid #333337;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"	border-width: 0px 1px 0px 1px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"	border-width: 1px 0px 1px 0px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"	min-width: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"	min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::add-line, QScrollBar::sub-line {\n"
"	background:#2d2d30;\n"
"	border: 1px solid #333337;\n"
"	subcontrol-origin: margin;\n"
"}\n"
"\n"
"QScrollBar::add-line {\n"
"	position: absolute;\n"
"}\n"
"\n"
"QScrollBar::add-line:horizontal {\n"
"	width: 15px;\n"
"	subcontrol-position: left;\n"
"	left: 15px;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical {\n"
"	height: 15px;\n"
"	subcontrol-position: top;\n"
"	top: 15px;\n"
"}\n"
"\n"
"QScrollBar::sub-line:horizontal {\n"
"	width: 15px;\n"
"	subcontrol-position: top left;\n"
"}\n"
"\n"
"QScrollBar::sub-line:vertical {\n"
"	height: 15px;\n"
"	subcontrol-position"
                        ": top;\n"
"}\n"
"\n"
"QScrollBar:left-arrow, QScrollBar::right-arrow, QScrollBar::up-arrow, QScrollBar::down-arrow {\n"
"	border: 1px solid #333337;\n"
"	width: 3px;\n"
"	height: 3px;\n"
"}\n"
"\n"
"QScrollBar::add-page, QScrollBar::sub-page {\n"
"	background: none;\n"
"}\n"
"\n"
"QAbstractButton:hover {\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QAbstractButton:pressed {\n"
"	background: #333337;\n"
"}\n"
"\n"
"QAbstractItemView {\n"
"	show-decoration-selected: 1;\n"
"	selection-background-color: #3f3f46;\n"
"	selection-color: #FFFFFF;\n"
"	alternate-background-color: #2d2d30;\n"
"}\n"
"\n"
"QHeaderView {\n"
"	border: 1px solid #333337;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"	background: #1e1e1e;\n"
"	border: 1px solid #333337;\n"
"	padding: 4px;\n"
"}\n"
"\n"
"QHeaderView::section:selected, QHeaderView::section::checked {\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QTableView {\n"
"	gridline-color: #333337;\n"
"}\n"
"\n"
"QTabBar {\n"
"	margin-left: 2px;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"	border-radius: 0px;\n"
""
                        "	padding: 4px;\n"
"	margin: 4px;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"	border: 1px solid #333337;\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"	border: 1px solid #333337;\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"	width: 3px;\n"
"	height: 3px;\n"
"	border: 1px solid #333337;\n"
"}\n"
"\n"
"QAbstractSpinBox {\n"
"	padding-right: 15px;\n"
"}\n"
"\n"
"QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {\n"
"	border: 1px solid #333337;\n"
"	background: #2d2d30;\n"
"	subcontrol-origin: border;\n"
"}\n"
"\n"
"QAbstractSpinBox::up-arrow, QAbstractSpinBox::down-arrow {\n"
"	width: 3px;\n"
"	height: 3px;\n"
"	border: 1px solid #333337;\n"
"}\n"
"\n"
"QSlider {\n"
"	border: none;\n"
"}\n"
"\n"
"QSlider::groove:horizontal {\n"
"	height: 5px;\n"
"	margin: 4px 0px 4px 0px;\n"
"}\n"
"\n"
"QSlider::groove:vertical {\n"
"	width: 5px;\n"
"	margin: 0px 4px 0px 4px;\n"
"}\n"
"\n"
"QSlider::handle {\n"
""
                        "	border: 1px solid #333337;\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QSlider::handle:horizontal {\n"
"	width: 15px;\n"
"	margin: -4px 0px -4px 0px;\n"
"}\n"
"\n"
"QSlider::handle:vertical {\n"
"	height: 15px;\n"
"	margin: 0px -4px 0px -4px;\n"
"}\n"
"\n"
"QSlider::add-page:vertical, QSlider::sub-page:horizontal {\n"
"	background: #3f3f46;\n"
"}\n"
"\n"
"QSlider::sub-page:vertical, QSlider::add-page:horizontal {\n"
"	background: #2d2d30;\n"
"}\n"
"\n"
"QLabel {\n"
"	border: none;\n"
"}\n"
"\n"
"QProgressBar {\n"
"	text-align: center;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"	width: 1px;\n"
"	background-color: #3f3f46;\n"
"}\n"
"\n"
"QMenu::separator {\n"
"	background: #2d2d30;\n"
"}"));
        BMGTexToolClass->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        actionOpen = new QAction(BMGTexToolClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionOpen->setFont(font);
        actionSave = new QAction(BMGTexToolClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/BMGTexTool/Resources/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon1);
        actionSave->setFont(font);
        actionExit = new QAction(BMGTexToolClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/BMGTexTool/Resources/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionExit->setFont(font);
        actionZoom_In = new QAction(BMGTexToolClass);
        actionZoom_In->setObjectName(QStringLiteral("actionZoom_In"));
        actionZoom_In->setFont(font);
        actionZoom_Out = new QAction(BMGTexToolClass);
        actionZoom_Out->setObjectName(QStringLiteral("actionZoom_Out"));
        actionZoom_Out->setFont(font);
        actionReset = new QAction(BMGTexToolClass);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/BMGTexTool/Resources/reset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset->setIcon(icon3);
        actionReset->setFont(font);
        actionUpperMip = new QAction(BMGTexToolClass);
        actionUpperMip->setObjectName(QStringLiteral("actionUpperMip"));
        actionUpperMip->setFont(font);
        actionLowerMip = new QAction(BMGTexToolClass);
        actionLowerMip->setObjectName(QStringLiteral("actionLowerMip"));
        actionLowerMip->setFont(font);
        actionGenerateMip = new QAction(BMGTexToolClass);
        actionGenerateMip->setObjectName(QStringLiteral("actionGenerateMip"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/BMGTexTool/Resources/generate.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGenerateMip->setIcon(icon4);
        actionGenerateMip->setFont(font);
        actionClean = new QAction(BMGTexToolClass);
        actionClean->setObjectName(QStringLiteral("actionClean"));
        actionClean->setFont(font);
        actionGenerateCube = new QAction(BMGTexToolClass);
        actionGenerateCube->setObjectName(QStringLiteral("actionGenerateCube"));
        actionGenerateCube->setIcon(icon4);
        actionGenerateCube->setFont(font);
        actionPx = new QAction(BMGTexToolClass);
        actionPx->setObjectName(QStringLiteral("actionPx"));
        actionPx->setCheckable(true);
        actionPx->setFont(font);
        actionNx = new QAction(BMGTexToolClass);
        actionNx->setObjectName(QStringLiteral("actionNx"));
        actionNx->setCheckable(true);
        actionNx->setFont(font);
        actionPy = new QAction(BMGTexToolClass);
        actionPy->setObjectName(QStringLiteral("actionPy"));
        actionPy->setCheckable(true);
        actionPy->setFont(font);
        actionNy = new QAction(BMGTexToolClass);
        actionNy->setObjectName(QStringLiteral("actionNy"));
        actionNy->setCheckable(true);
        actionNy->setFont(font);
        actionPz = new QAction(BMGTexToolClass);
        actionPz->setObjectName(QStringLiteral("actionPz"));
        actionPz->setCheckable(true);
        actionPz->setFont(font);
        actionNz = new QAction(BMGTexToolClass);
        actionNz->setObjectName(QStringLiteral("actionNz"));
        actionNz->setCheckable(true);
        actionNz->setFont(font);
        actionUpper3D = new QAction(BMGTexToolClass);
        actionUpper3D->setObjectName(QStringLiteral("actionUpper3D"));
        actionUpper3D->setFont(font);
        actionGenerate3D = new QAction(BMGTexToolClass);
        actionGenerate3D->setObjectName(QStringLiteral("actionGenerate3D"));
        actionGenerate3D->setIcon(icon4);
        actionGenerate3D->setFont(font);
        actionLower3D = new QAction(BMGTexToolClass);
        actionLower3D->setObjectName(QStringLiteral("actionLower3D"));
        actionLower3D->setFont(font);
        actionAbout = new QAction(BMGTexToolClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionAbout->setIcon(icon);
        actionAbout->setFont(font);
        actionClose = new QAction(BMGTexToolClass);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        actionClose->setFont(font);
        actionBatch = new QAction(BMGTexToolClass);
        actionBatch->setObjectName(QStringLiteral("actionBatch"));
        actionASTC = new QAction(BMGTexToolClass);
        actionASTC->setObjectName(QStringLiteral("actionASTC"));
        actionASTC->setCheckable(true);
        actionASTC->setFont(font);
        actionS3TC = new QAction(BMGTexToolClass);
        actionS3TC->setObjectName(QStringLiteral("actionS3TC"));
        actionS3TC->setCheckable(true);
        actionS3TC->setFont(font);
        actionETC2 = new QAction(BMGTexToolClass);
        actionETC2->setObjectName(QStringLiteral("actionETC2"));
        actionETC2->setCheckable(true);
        actionETC2->setFont(font);
        actionCompress = new QAction(BMGTexToolClass);
        actionCompress->setObjectName(QStringLiteral("actionCompress"));
        actionCompress->setIcon(icon4);
        actionCompress->setFont(font);
        actionSave_As = new QAction(BMGTexToolClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        centralWidget = new QWidget(BMGTexToolClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        BMGTexToolClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BMGTexToolClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuMipmaps = new QMenu(menuBar);
        menuMipmaps->setObjectName(QStringLiteral("menuMipmaps"));
        menuCubeMap = new QMenu(menuBar);
        menuCubeMap->setObjectName(QStringLiteral("menuCubeMap"));
        menu3D = new QMenu(menuBar);
        menu3D->setObjectName(QStringLiteral("menu3D"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        menuCompress = new QMenu(menuBar);
        menuCompress->setObjectName(QStringLiteral("menuCompress"));
        menuCompress->setFont(font);
        BMGTexToolClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(BMGTexToolClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        BMGTexToolClass->setStatusBar(statusBar);
        dockWidget = new QDockWidget(BMGTexToolClass);
        dockWidget->setObjectName(QStringLiteral("dockWidget"));
        dockWidget->setMinimumSize(QSize(368, 292));
        dockWidget->setFont(font);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        treeWidget = new QTreeWidget(dockWidgetContents);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setFont(font);

        verticalLayout->addWidget(treeWidget);

        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);
        label->setPixmap(QPixmap(QString::fromUtf8(":/BMGTexTool/Resources/logo.png")));

        verticalLayout->addWidget(label);

        dockWidget->setWidget(dockWidgetContents);
        BMGTexToolClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuCompress->menuAction());
        menuBar->addAction(menuMipmaps->menuAction());
        menuBar->addAction(menuCubeMap->menuAction());
        menuBar->addAction(menu3D->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_As);
        menuFile->addSeparator();
        menuFile->addAction(actionBatch);
        menuFile->addAction(actionClose);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuView->addAction(actionZoom_In);
        menuView->addAction(actionZoom_Out);
        menuView->addSeparator();
        menuView->addAction(actionReset);
        menuMipmaps->addAction(actionGenerateMip);
        menuMipmaps->addAction(actionClean);
        menuMipmaps->addSeparator();
        menuMipmaps->addAction(actionUpperMip);
        menuMipmaps->addAction(actionLowerMip);
        menuCubeMap->addAction(actionGenerateCube);
        menuCubeMap->addSeparator();
        menuCubeMap->addAction(actionPx);
        menuCubeMap->addAction(actionNx);
        menuCubeMap->addAction(actionPy);
        menuCubeMap->addAction(actionNy);
        menuCubeMap->addAction(actionPz);
        menuCubeMap->addAction(actionNz);
        menu3D->addAction(actionGenerate3D);
        menu3D->addSeparator();
        menu3D->addAction(actionUpper3D);
        menu3D->addAction(actionLower3D);
        menuAbout->addAction(actionAbout);
        menuCompress->addAction(actionASTC);
        menuCompress->addAction(actionS3TC);
        menuCompress->addAction(actionETC2);
        menuCompress->addSeparator();
        menuCompress->addAction(actionCompress);

        retranslateUi(BMGTexToolClass);

        QMetaObject::connectSlotsByName(BMGTexToolClass);
    } // setupUi

    void retranslateUi(QMainWindow *BMGTexToolClass)
    {
        BMGTexToolClass->setWindowTitle(QApplication::translate("BMGTexToolClass", "BMGTexTool", 0));
        actionOpen->setText(QApplication::translate("BMGTexToolClass", "Open", 0));
#ifndef QT_NO_TOOLTIP
        actionOpen->setToolTip(QApplication::translate("BMGTexToolClass", "Open Image Files", 0));
#endif // QT_NO_TOOLTIP
        actionOpen->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+O", 0));
        actionSave->setText(QApplication::translate("BMGTexToolClass", "Save", 0));
#ifndef QT_NO_TOOLTIP
        actionSave->setToolTip(QApplication::translate("BMGTexToolClass", "Save Bmg File", 0));
#endif // QT_NO_TOOLTIP
        actionSave->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+S", 0));
        actionExit->setText(QApplication::translate("BMGTexToolClass", "Exit", 0));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("BMGTexToolClass", "Exit Application", 0));
#endif // QT_NO_TOOLTIP
        actionZoom_In->setText(QApplication::translate("BMGTexToolClass", "Zoom In", 0));
        actionZoom_Out->setText(QApplication::translate("BMGTexToolClass", "Zoom Out", 0));
        actionReset->setText(QApplication::translate("BMGTexToolClass", "Reset", 0));
#ifndef QT_NO_TOOLTIP
        actionReset->setToolTip(QApplication::translate("BMGTexToolClass", "Zoom Reset", 0));
#endif // QT_NO_TOOLTIP
        actionReset->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+1", 0));
        actionUpperMip->setText(QApplication::translate("BMGTexToolClass", "Upper", 0));
#ifndef QT_NO_TOOLTIP
        actionUpperMip->setToolTip(QApplication::translate("BMGTexToolClass", "Upper Mipmap", 0));
#endif // QT_NO_TOOLTIP
        actionUpperMip->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+Up", 0));
        actionLowerMip->setText(QApplication::translate("BMGTexToolClass", "Lower", 0));
#ifndef QT_NO_TOOLTIP
        actionLowerMip->setToolTip(QApplication::translate("BMGTexToolClass", "Lower Mipmap", 0));
#endif // QT_NO_TOOLTIP
        actionLowerMip->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+Down", 0));
        actionGenerateMip->setText(QApplication::translate("BMGTexToolClass", "Generate", 0));
#ifndef QT_NO_TOOLTIP
        actionGenerateMip->setToolTip(QApplication::translate("BMGTexToolClass", "Generate Mipmap", 0));
#endif // QT_NO_TOOLTIP
        actionClean->setText(QApplication::translate("BMGTexToolClass", "Clean", 0));
#ifndef QT_NO_TOOLTIP
        actionClean->setToolTip(QApplication::translate("BMGTexToolClass", "Clean Mipmap", 0));
#endif // QT_NO_TOOLTIP
        actionGenerateCube->setText(QApplication::translate("BMGTexToolClass", "Create Template", 0));
#ifndef QT_NO_TOOLTIP
        actionGenerateCube->setToolTip(QApplication::translate("BMGTexToolClass", "Generate CubeMap", 0));
#endif // QT_NO_TOOLTIP
        actionPx->setText(QApplication::translate("BMGTexToolClass", "Px", 0));
#ifndef QT_NO_TOOLTIP
        actionPx->setToolTip(QApplication::translate("BMGTexToolClass", "Px Side", 0));
#endif // QT_NO_TOOLTIP
        actionNx->setText(QApplication::translate("BMGTexToolClass", "Nx", 0));
#ifndef QT_NO_TOOLTIP
        actionNx->setToolTip(QApplication::translate("BMGTexToolClass", "Nx Side", 0));
#endif // QT_NO_TOOLTIP
        actionPy->setText(QApplication::translate("BMGTexToolClass", "Py", 0));
#ifndef QT_NO_TOOLTIP
        actionPy->setToolTip(QApplication::translate("BMGTexToolClass", "Py Side", 0));
#endif // QT_NO_TOOLTIP
        actionNy->setText(QApplication::translate("BMGTexToolClass", "Ny", 0));
#ifndef QT_NO_TOOLTIP
        actionNy->setToolTip(QApplication::translate("BMGTexToolClass", "Ny Side", 0));
#endif // QT_NO_TOOLTIP
        actionPz->setText(QApplication::translate("BMGTexToolClass", "Pz", 0));
#ifndef QT_NO_TOOLTIP
        actionPz->setToolTip(QApplication::translate("BMGTexToolClass", "Pz Side", 0));
#endif // QT_NO_TOOLTIP
        actionNz->setText(QApplication::translate("BMGTexToolClass", "Nz", 0));
#ifndef QT_NO_TOOLTIP
        actionNz->setToolTip(QApplication::translate("BMGTexToolClass", "Nz Side", 0));
#endif // QT_NO_TOOLTIP
        actionUpper3D->setText(QApplication::translate("BMGTexToolClass", "Upper", 0));
#ifndef QT_NO_TOOLTIP
        actionUpper3D->setToolTip(QApplication::translate("BMGTexToolClass", "Upper Texture Layer", 0));
#endif // QT_NO_TOOLTIP
        actionUpper3D->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+Shift+Up", 0));
        actionGenerate3D->setText(QApplication::translate("BMGTexToolClass", "Generate", 0));
#ifndef QT_NO_TOOLTIP
        actionGenerate3D->setToolTip(QApplication::translate("BMGTexToolClass", "Generate 3D Texture Volume", 0));
#endif // QT_NO_TOOLTIP
        actionLower3D->setText(QApplication::translate("BMGTexToolClass", "Lower", 0));
#ifndef QT_NO_TOOLTIP
        actionLower3D->setToolTip(QApplication::translate("BMGTexToolClass", "Lower Texture Layer", 0));
#endif // QT_NO_TOOLTIP
        actionLower3D->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+Shift+Down", 0));
        actionAbout->setText(QApplication::translate("BMGTexToolClass", "About", 0));
        actionClose->setText(QApplication::translate("BMGTexToolClass", "Close", 0));
#ifndef QT_NO_TOOLTIP
        actionClose->setToolTip(QApplication::translate("BMGTexToolClass", "Close Image", 0));
#endif // QT_NO_TOOLTIP
        actionBatch->setText(QApplication::translate("BMGTexToolClass", "Batch", 0));
        actionASTC->setText(QApplication::translate("BMGTexToolClass", "ASTC", 0));
        actionASTC->setShortcut(QApplication::translate("BMGTexToolClass", "Shift+A", 0));
        actionS3TC->setText(QApplication::translate("BMGTexToolClass", "S3TC", 0));
        actionS3TC->setShortcut(QApplication::translate("BMGTexToolClass", "Shift+S", 0));
        actionETC2->setText(QApplication::translate("BMGTexToolClass", "ETC2", 0));
        actionETC2->setShortcut(QApplication::translate("BMGTexToolClass", "Shift+E", 0));
        actionCompress->setText(QApplication::translate("BMGTexToolClass", "Compress", 0));
        actionCompress->setShortcut(QApplication::translate("BMGTexToolClass", "F5", 0));
        actionSave_As->setText(QApplication::translate("BMGTexToolClass", "Save As", 0));
        actionSave_As->setShortcut(QApplication::translate("BMGTexToolClass", "Ctrl+Shift+S", 0));
        menuFile->setTitle(QApplication::translate("BMGTexToolClass", "File", 0));
        menuView->setTitle(QApplication::translate("BMGTexToolClass", "View", 0));
        menuMipmaps->setTitle(QApplication::translate("BMGTexToolClass", "Mipmaps", 0));
        menuCubeMap->setTitle(QApplication::translate("BMGTexToolClass", "CubeMap", 0));
        menu3D->setTitle(QApplication::translate("BMGTexToolClass", "3D", 0));
        menuAbout->setTitle(QApplication::translate("BMGTexToolClass", "Help", 0));
        menuCompress->setTitle(QApplication::translate("BMGTexToolClass", "Compress", 0));
        dockWidget->setWindowTitle(QApplication::translate("BMGTexToolClass", "Texture Tag Sheet", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("BMGTexToolClass", "Area", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("BMGTexToolClass", "Tag", 0));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BMGTexToolClass: public Ui_BMGTexToolClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BMGTEXTOOL_H
