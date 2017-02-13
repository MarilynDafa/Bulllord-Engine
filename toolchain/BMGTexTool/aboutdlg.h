#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>
#include "ui_aboutdlg.h"

class AboutDLg : public QDialog
{
    Q_OBJECT

public:
    AboutDLg(QWidget *parent = Q_NULLPTR);

private slots:
    void on_pushButton_clicked();

private:
    Ui::AboutDLg ui;
};

#endif // ABOUTDLG_H
