#include "OpenFileDialog.h"
#include "ResourceMgr.h"
#include <QFileInfo>

OpenFileDialog::OpenFileDialog( QWidget *parent/*= NULL*/ )
	: QDialog(parent)
{
	setupUi(this);

	const std::vector<QString> &files = ResourceMgr::getInstance()->getItfFiles();
	for(std::vector<QString>::const_iterator iter = files.begin(); iter != files.end(); ++ iter)
	{
		QString fileName = *iter;
		QFileInfo fileInfo(fileName);
		_fileList->addItem(fileInfo.baseName());
	}

	if(!files.empty())
		_fileList->setCurrentRow(0);
}

OpenFileDialog::~OpenFileDialog()
{

}

void OpenFileDialog::accept()
{
	int count = _fileList->count();
	if(count != 0)
	{
		QListWidgetItem *item = _fileList->currentItem();
		Q_ASSERT(item != NULL);
		_selectFile = item->text();
	}

	QDialog::accept();
}
