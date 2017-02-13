#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Nirvana.h"

class Nirvana : public QMainWindow
{
    Q_OBJECT

public:
    Nirvana(QWidget *parent = Q_NULLPTR);

private:
    Ui::NirvanaClass ui;
};
