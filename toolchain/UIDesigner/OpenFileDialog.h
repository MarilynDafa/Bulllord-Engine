#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include "GeneratedFiles/ui_OpenFileDialog.h"
#include <QDialog>
#include <QString>

class OpenFileDialog : public QDialog, public Ui::OpenFileDialog
{
	Q_OBJECT
public:
	OpenFileDialog(QWidget *parent= NULL);
	~OpenFileDialog();

	virtual void accept();

	QString getSelectFile() const { return _selectFile; }
private:
	QString _selectFile;
};

#endif //OPENFILEDIALOG_H