#ifndef CREATE3DDIALOG_H
#define CREATE3DDIALOG_H

#include "GeneratedFiles/ui_create3ddialog.h"

class Create3DDialog : public QDialog , public Ui::Create3DDialog
{
	Q_OBJECT
public:
	Create3DDialog(QWidget *parent = 0);
	void setPicWidth(int width);
	int getSelectVolumeNum() { return _selectVolumeNum; }
private:
	QList<int> volumeList(int width);
	void initCombox(QList<int> &list);
private slots:
	void okButtonClicked();

private:
	int _selectVolumeNum;
};

#endif //CREATE3DDIALOG_H