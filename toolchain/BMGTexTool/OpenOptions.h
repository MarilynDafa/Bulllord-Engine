#pragma once

#include <QDialog>
#include "ui_OpenOptions.h"

class OpenOptions : public QDialog
{
	Q_OBJECT

public:
	OpenOptions(QWidget *parent = Q_NULLPTR);
	~OpenOptions();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
	Ui::OpenOptions ui;
};
