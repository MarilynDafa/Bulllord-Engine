#include "Create3DDialog.h"

Create3DDialog::Create3DDialog(QWidget *parent /* = 0*/)
	: QDialog(parent)
	, _selectVolumeNum(-1)
{
	setupUi(this);
	connect(_okButton,SIGNAL(clicked()),this,SLOT(okButtonClicked()));
}

void Create3DDialog::setPicWidth(int width)
{
	initCombox(volumeList(width));
}

QList<int> Create3DDialog::volumeList(int width)
{
	QList<int> list;
	int volumeNum = 0;
	for(int i = 1; i < 10; i++)
	{
		volumeNum = 1 << i;
		if((width % volumeNum) == 0)
			list.append(volumeNum);
	}
	return list;
}

void Create3DDialog::initCombox(QList<int> &list)
{
	int listSize  = list.size();
	for(int i = 0; i < listSize; i++)
	{
		_comboBox->addItem(QString("%1 *").arg(list[i]),QVariant(list[i]));
	}
}

void Create3DDialog::okButtonClicked()
{
	int index = _comboBox->currentIndex();
	if(index < 0)
	{
		_selectVolumeNum = -1;
	}
	else
	{
		_selectVolumeNum = _comboBox->itemData(index).toInt();
	}
	accept();
}