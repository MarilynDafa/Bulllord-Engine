#include "OpenOptions.h"

OpenOptions::OpenOptions(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

OpenOptions::~OpenOptions()
{
}

void OpenOptions::on_pushButton_clicked()
{
	accept();
}

void OpenOptions::on_pushButton_2_clicked()
{
	reject();
}
