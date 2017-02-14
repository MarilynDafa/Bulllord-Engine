/********************************************************************************
** Form generated from reading UI file 'OpenOptions.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENOPTIONS_H
#define UI_OPENOPTIONS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_OpenOptions
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLabel *label;
    QCheckBox *checkBox;

    void setupUi(QDialog *OpenOptions)
    {
        if (OpenOptions->objectName().isEmpty())
            OpenOptions->setObjectName(QStringLiteral("OpenOptions"));
        OpenOptions->resize(295, 124);
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        OpenOptions->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/BMGTexTool/Resources/TX.png"), QSize(), QIcon::Normal, QIcon::Off);
        OpenOptions->setWindowIcon(icon);
        OpenOptions->setStyleSheet(QLatin1String("/*\n"
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
        OpenOptions->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        OpenOptions->setModal(true);
        pushButton = new QPushButton(OpenOptions);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(30, 90, 75, 23));
        pushButton->setFont(font);
        pushButton->setStyleSheet(QStringLiteral(""));
        pushButton_2 = new QPushButton(OpenOptions);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(190, 90, 75, 23));
        label = new QLabel(OpenOptions);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 30, 231, 16));
        label->setAlignment(Qt::AlignCenter);
        checkBox = new QCheckBox(OpenOptions);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(210, 60, 51, 16));
        checkBox->setChecked(true);

        retranslateUi(OpenOptions);

        QMetaObject::connectSlotsByName(OpenOptions);
    } // setupUi

    void retranslateUi(QDialog *OpenOptions)
    {
        OpenOptions->setWindowTitle(QApplication::translate("OpenOptions", "Options", 0));
        pushButton->setText(QApplication::translate("OpenOptions", "Separate", 0));
        pushButton_2->setText(QApplication::translate("OpenOptions", "Merge", 0));
        label->setText(QApplication::translate("OpenOptions", "How to deal with the open files", 0));
        checkBox->setText(QApplication::translate("OpenOptions", "Trim", 0));
    } // retranslateUi

};

namespace Ui {
    class OpenOptions: public Ui_OpenOptions {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENOPTIONS_H
