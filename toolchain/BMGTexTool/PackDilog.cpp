#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "PackDialog.h"
#include "ImageWidget.h"
#include "Image.h"
#include "BMGTexTool.h"

PackDialog::PackDialog(QWidget * parent /*= 0*/)
	: QDialog(parent)
{
	_packcm = CMD_NONE;
	setupUi(this);

	connect(_okBtn,SIGNAL(clicked()),this,SLOT(accept()));
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChanged(int)));
}

void PackDialog::onIndexChanged(int idx)
{
	switch (idx)
	{
	case 0:
		_packcm = CMD_NONE;
		break;
	case 1:
		_packcm = CM_ASTC;
		break;
	case 2:
		_packcm = CM_ETC2;
		break;
	case 3:
		_packcm = CM_S3TC;
		break;
	default:
		_packcm = CMD_NONE;
		break;
	}
}
