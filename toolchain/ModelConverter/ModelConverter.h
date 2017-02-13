#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ModelConverter.h"

class ModelConverter : public QMainWindow
{
    Q_OBJECT

public:
    ModelConverter(QWidget *parent = Q_NULLPTR);

private:
    Ui::ModelConverterClass ui;
};
