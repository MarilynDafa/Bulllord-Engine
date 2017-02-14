#include "OpenOptions.h"
#include "BMGTexTool.h"
OpenOptions::OpenOptions(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.checkBox, SIGNAL(clicked()), this, SLOT(okTrimClicked()));
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

void OpenOptions::okTrimClicked()
{
	if (ui.checkBox->checkState() == Qt::Checked)
		BMGTexTool::g_Trim = true;
	else
		BMGTexTool::g_Trim = false;
}
