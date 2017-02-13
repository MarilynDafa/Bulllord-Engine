#include "aboutdlg.h"

AboutDLg::AboutDLg(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
}

void AboutDLg::on_pushButton_clicked()
{
	accept();
}
