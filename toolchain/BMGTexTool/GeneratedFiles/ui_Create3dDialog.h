/********************************************************************************
** Form generated from reading UI file 'Create3dDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATE3DDIALOG_H
#define UI_CREATE3DDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Create3DDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *_comboBox;
    QPushButton *_okButton;

    void setupUi(QDialog *Create3DDialog)
    {
        if (Create3DDialog->objectName().isEmpty())
            Create3DDialog->setObjectName(QStringLiteral("Create3DDialog"));
        Create3DDialog->resize(248, 120);
        Create3DDialog->setMinimumSize(QSize(248, 41));
        Create3DDialog->setMaximumSize(QSize(248, 120));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        Create3DDialog->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/BMGTexTool/Resources/TX.png"), QSize(), QIcon::Normal, QIcon::Off);
        Create3DDialog->setWindowIcon(icon);
        Create3DDialog->setStyleSheet(QLatin1String("/*\n"
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
        Create3DDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout = new QGridLayout(Create3DDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(Create3DDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        _comboBox = new QComboBox(Create3DDialog);
        _comboBox->setObjectName(QStringLiteral("_comboBox"));
        _comboBox->setFont(font);

        gridLayout->addWidget(_comboBox, 1, 0, 1, 1);

        _okButton = new QPushButton(Create3DDialog);
        _okButton->setObjectName(QStringLiteral("_okButton"));
        _okButton->setMinimumSize(QSize(75, 23));
        _okButton->setMaximumSize(QSize(75, 23));
        _okButton->setFont(font);

        gridLayout->addWidget(_okButton, 1, 1, 1, 1);


        retranslateUi(Create3DDialog);

        QMetaObject::connectSlotsByName(Create3DDialog);
    } // setupUi

    void retranslateUi(QDialog *Create3DDialog)
    {
        Create3DDialog->setWindowTitle(QApplication::translate("Create3DDialog", "3D Texture", 0));
        label->setText(QApplication::translate("Create3DDialog", "Set Volume Texture's Depth", 0));
        _okButton->setText(QApplication::translate("Create3DDialog", "OK", 0));
    } // retranslateUi

};

namespace Ui {
    class Create3DDialog: public Ui_Create3DDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATE3DDIALOG_H
