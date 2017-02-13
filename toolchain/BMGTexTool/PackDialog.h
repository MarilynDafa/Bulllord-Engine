#ifndef PACKDIALOG_H
#define PACKDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMdiSubWindow>
#include <QtCore/QList>

#include "ImageWidget.h"
#include "Image.h"
#include "GeneratedFiles/ui_packdialog.h"

class PackDialog : public QDialog , public Ui::PackDialog
{
	Q_OBJECT
public:
	PackDialog(QWidget * parent = 0);

	struct ImageOpRef
	{
		ImageOp *imageOp;

		ImageOpRef()
		{
			imageOp = 0;
		}

		ImageOpRef(ImageOp *imgOp)
		{
			imageOp = imgOp;
		}
	};
	CompressMethord _packcm;

protected slots:
void onIndexChanged(int idx);
private:
};

Q_DECLARE_METATYPE(PackDialog::ImageOpRef)

#endif //PACKDIALOG_H