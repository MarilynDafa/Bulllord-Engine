#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FXEditor.h"

class FXEditor : public QMainWindow
{
    Q_OBJECT

public:
    FXEditor(QWidget *parent = Q_NULLPTR);

private:
    Ui::FXEditorClass ui;
};
