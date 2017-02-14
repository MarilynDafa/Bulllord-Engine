#include "BMGTexTool.h"
#include "aboutdlg.h"
#include "Image.h"
#include "ImageCube.h"
#include "ImageWidget.h"
#include <QtWidgets/QFileDialog>
#include "Create3DDialog.h"
#include "binpack2d.hpp"
#include "packDialog.h"
#include "Codec.h"
#include "ImageCompareWidget.h"
#include "OpenOptions.h"
#include "batch.h"
#include <QtCore/QMimeData>
#include <future>

bool BMGTexTool::g_Trim = true;
BMGTexTool::BMGTexTool(QWidget *parent)
	: QMainWindow(parent)
{
	ilInit();
	ui.setupUi(this);

	_mdiArea = new QMdiArea(this);
	setCentralWidget(_mdiArea);

	QBrush brush(QColor::fromRgbF(0.16,0.16,0.16),  Qt::Dense1Pattern);
	_mdiArea->setBackground(brush);
	setWindowState(Qt::WindowMaximized);
	connect(_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), SLOT(imageWindowActived(QMdiSubWindow*)));
	connect(this->ui.treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(treeItemSelectedChanged()));
	connect(this->ui.treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(treeItemEdited(QTreeWidgetItem*, int)));


	activeCubeBar(false);
	active3DBar(false);

	m_pCmmdActGrp = new QActionGroup(this);
	m_pCmmdActGrp->addAction(this->ui.actionASTC);
	m_pCmmdActGrp->addAction(this->ui.actionETC2);
	m_pCmmdActGrp->addAction(this->ui.actionS3TC);

	m_pCmmdActGrp2 = new QActionGroup(this);
	m_pCmmdActGrp2->addAction(this->ui.actionPx);
	m_pCmmdActGrp2->addAction(this->ui.actionPy);
	m_pCmmdActGrp2->addAction(this->ui.actionPz);
	m_pCmmdActGrp2->addAction(this->ui.actionNx);
	m_pCmmdActGrp2->addAction(this->ui.actionNy);
	m_pCmmdActGrp2->addAction(this->ui.actionNz);
}

void BMGTexTool::startAnimation()
{

}

void BMGTexTool::stopAnimation()
{
}


void BMGTexTool::save()
{

	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp == 0)
		return;


	ImageWidget *imageWidget = currentImageWindow();
	if (!imageWidget)
		return;

	Data_block db;
	BMG_header_TT kh;
	imageOp->getKtxHeader(kh);

	QList<QMdiSubWindow*> subWindows = _mdiArea->subWindowList();
	int numSubWindow = subWindows.size();
	//if (imageOp->getImageType() == IMAGE3D || numSubWindow == 1)
	{
		QString fileName;
		if (imageWidget->IsBMG())
		{
			fileName = imageWidget->getImageFileName();
		}
		else
			fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr(""), tr("Bulllord Texture File (*.bmg)"));
		if (!fileName.isEmpty())
		{
			kh.numberOfArrayElements = 1;
			std::string fileStr = fileName.toStdString();
			const char *file = fileStr.c_str();
			PackDialog dlg;
			dlg._packcm = CMD_NONE;
			if (dlg.exec() == QDialog::Accepted)
			{
				if (imageWidget->IsBMG() && dlg._packcm == imageWidget->getImage()->getCM() && !imageWidget->needSave())
					return;
				QProgressDialog progress("Saving file...", "", 0, 10, this);
				progress.setWindowModality(Qt::WindowModal);
				progress.setFixedWidth(300);
				progress.setLabelText("Compressing file...");
				progress.setCancelButton(0);
				progress.setValue(0);
				progress.show();
				progress.setBar(0);
				progress.setValue(2);
				QCoreApplication::processEvents();
				std::vector<SpriteSheet> ssv;
				ImageWidget *imageWidget = currentImageWindow();
				if (!imageWidget)
				{
					for (int i = 0; i < _imageAreaList.size(); ++i)
					{
						ImageArea* imageArea = _imageAreaList[i];
						QString name = imageArea->name;
						QString area = imageArea->toAreaString();
						SpriteSheet ss;
						memset(ss.tag, 0, sizeof(ss.tag));
						strcpy(ss.tag, name.toStdString().c_str());
						ss.taglen = strlen(ss.tag);
						area.remove(0, 1);
						area.remove(area.length() - 1, 1);
						QStringList strl = area.split(",");
						assert(strl.size() == 4);
						ss.ltx = strl[0].toInt();
						ss.lty = strl[1].toInt();
						ss.rbx = strl[2].toInt();
						ss.rby = strl[3].toInt();
						ssv.push_back(ss);
					}
				}
				else
				{
					for (int i = 0; i < imageWidget->_imageAreaList.size(); ++i)
					{
						ImageArea* imageArea = imageWidget->_imageAreaList[i];
						QString name = imageArea->name;
						QString area = imageArea->toAreaString();
						SpriteSheet ss;
						memset(ss.tag, 0, sizeof(ss.tag));
						strcpy(ss.tag, name.toStdString().c_str());
						ss.taglen = strlen(ss.tag);
						area.remove(0, 1);
						area.remove(area.length() - 1, 1);
						QStringList strl = area.split(",");
						assert(strl.size() == 4);
						ss.ltx = strl[0].toInt();
						ss.lty = strl[1].toInt();
						ss.rbx = strl[2].toInt();
						ss.rby = strl[3].toInt();
						ssv.push_back(ss);
					}
				}
				progress.setValue(rand() % 5);
				QCoreApplication::processEvents();
				imageOp->getImage()->compressImage(dlg._packcm, &db);
				CompressKTX(kh, db, ssv, imageOp->getTextureTarget(), imageOp->getAlphaType(), file, dlg._packcm);
				progress.setValue(10);
				QCoreApplication::processEvents();
				imageWidget->setChanged(false);
				imageWidget->setImageFileName(file);
			}
		}
	}
}
void BMGTexTool::updatePixmap()
{

}

void BMGTexTool::openImageFiles(const QStringList &files)
{
	int count = files.size();
	if (count == 1)
	{
		for (int i = 0; i < count; i++)
		{
			const QString &fileName = files[i];
			if (!fileName.isEmpty())
			{
				openImageFile(fileName);
			}
		}
	}
	else if(count > 1)
	{
		OpenOptions oo;		
		int ret = oo.exec();
		if (ret == QDialog::Accepted)
		{
			for (int i = 0; i < count; i++)
			{
				const QString &fileName = files[i];
				if (!fileName.isEmpty())
				{
					openImageFile(fileName);
				}
			}
		}
		else//merge
		{
			ImageWidget *subWindow = new ImageWidget(this);
			QMdiSubWindow *sw = (QMdiSubWindow*)_mdiArea->addSubWindow(subWindow);
			sw->show();
			for (int i = 0; i < count; i++)
			{
				const QString &file = files[i]; ;
				std::vector<SpriteSheet> ssv;
				int offsetx, offsety;
				QList<Image*> images = Image::createImages(file, ssv, offsetx, offsety);
				int countf = images.size();
				for (int j = 0; j < countf; j++)
				{
					Image* image = images[j];
					subWindow->setImage(image, file,offsetx, offsety);
				}
			}

			BinPack2D::ContentAccumulator<ImageArea*> inputContent;
			int count = this->ui.treeWidget->topLevelItemCount();
			for (int i = 0; i < count; i++)
			{
				ImageArea *area = getTreeItemData(this->ui.treeWidget->topLevelItem(i));
				int width, height;
				area->getSize(width, height);
				inputContent += BinPack2D::Content<ImageArea*>(area, BinPack2D::Coord(), BinPack2D::Size(width, height), false);
			}
			inputContent.Sort();

			BinPack2D::CanvasArray<ImageArea*> canvasArray =
				BinPack2D::UniformCanvasArrayBuilder<ImageArea*>(2048, 2048, 2).Build();

			BinPack2D::ContentAccumulator<ImageArea*> remainder;
			bool success = canvasArray.Place(inputContent, remainder);
			BinPack2D::ContentAccumulator<ImageArea*> outputContent;
			canvasArray.CollectContent(outputContent);

			typedef BinPack2D::Content<ImageArea*>::Vector::iterator binpack2d_iterator;
			for (binpack2d_iterator itor = outputContent.Get().begin(); itor != outputContent.Get().end(); itor++)
			{
				BinPack2D::Content<ImageArea*> &content = *itor;
				ImageArea* imagePiece = content.content;
				subWindow->moveImageToPoint(imagePiece, content.coord.x, content.coord.y);
			}
			subWindow->ResetCanves();
			imageWindowActived(sw);
		}
	}
}

void BMGTexTool::openImageFile(const QString &file)
{
	if (!file.isEmpty())
	{
		std::vector<SpriteSheet> ssv;
		int offsetx, offsety;
		QList<Image*> images = Image::createImages(file, ssv, offsetx, offsety);
		int count = images.size();
		for (int i = 0; i < count; i++)
		{
			Image* image = images[i];
			ImageWidget *subWindow = new ImageWidget(this);
			subWindow->setMinimumWidth(150);
			subWindow->setMinimumHeight(20);
			QMdiSubWindow *sw = (QMdiSubWindow*)_mdiArea->addSubWindow(subWindow);
			sw->show();
			subWindow->setImage(image, file, offsetx, offsety ,&ssv);
			imageWindowActived(sw);
		}
	}
}

void BMGTexTool::setFaceActCheckState(FACE face)
{
	if (face == POSITIVE_X)
		this->ui.actionPx->setChecked(true);
	else
		this->ui.actionPx->setChecked(false);
	if (face == POSITIVE_Y)
		this->ui.actionPy->setChecked(true);
	else
		this->ui.actionPy->setChecked(false);
	if (face == POSITIVE_Z)
		this->ui.actionPz->setChecked(true);
	else
		this->ui.actionPz->setChecked(false);
	if (face == NEGATIVE_X)
		this->ui.actionNx->setChecked(true);
	else
		this->ui.actionNx->setChecked(false);
	if (face == NEGATIVE_Y)
		this->ui.actionNy->setChecked(true);
	else
		this->ui.actionNy->setChecked(false);
	if (face == NEGATIVE_Z)
		this->ui.actionNz->setChecked(true);
	else
		this->ui.actionNz->setChecked(false);
}


ImageWidget *BMGTexTool::currentImageWindow()
{
	QMdiSubWindow *subWindow = (QMdiSubWindow*)_mdiArea->currentSubWindow();

	if (subWindow == 0)
		return 0;

	ImageWidget *imageWidget = dynamic_cast<ImageWidget*>(subWindow->widget());
	return imageWidget;
}
void BMGTexTool::activeCubeBar(bool active)
{


	if (active)
	{
		ImageOp *imageOp = currentImageOpWidget();
		if (imageOp != 0)
		{
			Image *image = imageOp->getImage();
			if (image != 0)
				setFaceActCheckState(image->getCurrentFace());
		}
	}
}

void BMGTexTool::active3DBar(bool active)
{
	this->ui.actionGenerate3D->setEnabled(active);
//	this->ui.actionLower3D->setEnabled(active);
//	this->ui.actionUpper3D->setEnabled(active);
}

void BMGTexTool::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void BMGTexTool::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
		const QList<QUrl> &urls = event->mimeData()->urls();
		QStringList files;
		int count = urls.size();
		for (int i = 0; i < count; i++)
		{
			files.push_back(urls[i].toLocalFile());
		}
		openImageFiles(files);
	}
}


void BMGTexTool::activeFace(FACE face)
{
	setFaceActCheckState(face);
	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp != 0)
	{
		imageOp->setFace(face);
	}
}
void BMGTexTool::imageWindowActived(QMdiSubWindow *window)
{	
	if (window != 0)
	{
		this->ui.treeWidget->clear();
		ImageWidget *imageWidget = currentImageWindow();
		if (!imageWidget)
			return;
		for (int i = 0; i < imageWidget->_imageAreaList.size(); ++i)
		{
			ImageArea* imageArea = imageWidget->_imageAreaList[i];
			QTreeWidgetItem *item = new QTreeWidgetItem;
			item->setText(0, imageArea->name);
			item->setText(1, imageArea->toAreaString());
			item->setData(2, Qt::UserRole, QVariant::fromValue(ImageAreaRef(imageArea)));
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			this->ui.treeWidget->addTopLevelItem(item);
		}

		ImageOp *imageOp = getImageOpWidget(window);
		if (imageOp != 0)
		{
			if (imageOp->getImage())
			{
				QString msg = imageOp->getImageFileName();
				msg += "[";
				msg += "Width:";
				msg += QString::number(imageOp->getImage()->getImage()->width());
				msg += "  ";
				msg += "Height:";
				msg += QString::number(imageOp->getImage()->getImage()->height());
				msg += "  Codec:";
				if (imageOp->getImage()->getCM() == CM_ASTC)
					msg += "ASTC]";
				else if (imageOp->getImage()->getCM() == CM_S3TC)
					msg += "S3TC]";
				else if (imageOp->getImage()->getCM() == CM_ETC2)
					msg += "ETC2]";
				else
					msg += "LOSSY]";

				this->ui.statusBar->showMessage(msg);
			}
			if (imageOp->getImageType() == IMAGECUBE)
			{
				activeCubeBar(true);
				return;
			}
			else if (imageOp->getImageType() == IMAGE2D)
			{
				if (imageOp->getImage()->canConvert3D())
					active3DBar(true);
				return;
			}
		}
	}
	else
	{
		this->ui.treeWidget->clear();
		this->ui.statusBar->showMessage("");
	}
	activeCubeBar(false);
	active3DBar(false);
}


ImageOp *BMGTexTool::currentImageOpWidget()
{
	QMdiSubWindow *subWindow = (QMdiSubWindow*)_mdiArea->currentSubWindow();
	return getImageOpWidget(subWindow);
}

ImageOp *BMGTexTool::getImageOpWidget(QMdiSubWindow *subWindow)
{
	if (subWindow == 0)
		return 0;

	ImageOp *imageOp = dynamic_cast<ImageOp*>(subWindow->widget());
	return imageOp;
}

//����
void BMGTexTool::on_actionAbout_triggered()
{
	AboutDLg dlg;
	dlg.exec();
}

//�����ļ�
void BMGTexTool::on_actionOpen_triggered()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"), tr(""), tr("Image files (*.png *.tif *.jpg *.dds *.jpeg *.bmp *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm);;Bulllord Texture Files (*.bmg);;All files(*.*)"));
	openImageFiles(fileNames);
}



void BMGTexTool::on_actionSave_As_triggered()
{


	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp == 0)
		return;


	ImageWidget *imageWidget = currentImageWindow();
	if (!imageWidget)
		return;

	Data_block db;
	BMG_header_TT kh;
	imageOp->getKtxHeader(kh);

	QList<QMdiSubWindow*> subWindows = _mdiArea->subWindowList();
	int numSubWindow = subWindows.size();
	//if (imageOp->getImageType() == IMAGE3D || numSubWindow == 1)
	{
		QString fileName;
		fileName = QFileDialog::getSaveFileName(this, tr("Save As File"), tr(""), tr("Bulllord Texture File (*.bmg)"));
		if (!fileName.isEmpty())
		{
			kh.numberOfArrayElements = 1;
			std::string fileStr = fileName.toStdString();
			const char *file = fileStr.c_str();
			PackDialog dlg;
			dlg._packcm = CMD_NONE;
			if (dlg.exec() == QDialog::Accepted)
			{
				QProgressDialog progress("Saving file...", "", 0, 10, this);
				progress.setWindowModality(Qt::WindowModal);
				progress.setFixedWidth(300);
				progress.setLabelText("Compressing file...");
				progress.setCancelButton(0);
				progress.setValue(0);
				progress.show();
				progress.setBar(0);
				progress.setValue(2);
				QCoreApplication::processEvents();
				std::vector<SpriteSheet> ssv;
				ImageWidget *imageWidget = currentImageWindow();
				if (!imageWidget)
				{
					for (int i = 0; i < _imageAreaList.size(); ++i)
					{
						ImageArea* imageArea = _imageAreaList[i];
						QString name = imageArea->name;
						QString area = imageArea->toAreaString();
						SpriteSheet ss;
						memset(ss.tag, 0, sizeof(ss.tag));
						strcpy(ss.tag, name.toStdString().c_str());
						ss.taglen = strlen(ss.tag);
						area.remove(0, 1);
						area.remove(area.length() - 1, 1);
						QStringList strl = area.split(",");
						assert(strl.size() == 4);
						ss.ltx = strl[0].toInt();
						ss.lty = strl[1].toInt();
						ss.rbx = strl[2].toInt();
						ss.rby = strl[3].toInt();
						ss.offsetx = imageArea->getOffsetX();
						ss.offsety = imageArea->getOffsetY();
						ssv.push_back(ss);
					}
				}
				else
				{
					for (int i = 0; i < imageWidget->_imageAreaList.size(); ++i)
					{
						ImageArea* imageArea = imageWidget->_imageAreaList[i];
						QString name = imageArea->name;
						QString area = imageArea->toAreaString();
						SpriteSheet ss;
						memset(ss.tag, 0, sizeof(ss.tag));
						strcpy(ss.tag, name.toStdString().c_str());
						ss.taglen = strlen(ss.tag);
						area.remove(0, 1);
						area.remove(area.length() - 1, 1);
						QStringList strl = area.split(",");
						assert(strl.size() == 4);
						ss.ltx = strl[0].toInt();
						ss.lty = strl[1].toInt();
						ss.rbx = strl[2].toInt();
						ss.rby = strl[3].toInt();
						ss.offsetx = imageArea->getOffsetX();
						ss.offsety = imageArea->getOffsetY();
						ssv.push_back(ss);
					}
				}
				progress.setValue(rand() % 5);
				QCoreApplication::processEvents();
				imageOp->getImage()->compressImage(dlg._packcm, &db);
				CompressKTX(kh, db, ssv, imageOp->getTextureTarget(), imageOp->getAlphaType(), file, dlg._packcm);
				progress.setValue(10);
				QCoreApplication::processEvents();
				imageWidget->setChanged(false);
				imageWidget->setImageFileName(file);
			}
		}
	}
}

//����
void BMGTexTool::on_actionSave_triggered()
{

	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp == 0)
		return;


	ImageWidget *imageWidget = currentImageWindow();
	if (!imageWidget)
		return;

	Data_block db;
	BMG_header_TT kh;
	imageOp->getKtxHeader(kh);

	QList<QMdiSubWindow*> subWindows = _mdiArea->subWindowList();
	int numSubWindow = subWindows.size();
	//if (imageOp->getImageType() == IMAGE3D || numSubWindow == 1)
	{
		QString fileName;
		if (imageWidget->IsBMG())
		{
			fileName = imageWidget->getImageFileName();
		}
		else
			fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr(""), tr("Bulllord Texture File (*.bmg)"));
		if (!fileName.isEmpty())
		{
			kh.numberOfArrayElements = 1;
			std::string fileStr = fileName.toStdString();
			const char *file = fileStr.c_str();
			PackDialog dlg;
			dlg._packcm = CMD_NONE;
			if (dlg.exec() == QDialog::Accepted)
			{
				if (imageWidget->IsBMG() && dlg._packcm == imageWidget->getImage()->getCM() && !imageWidget->needSave())
					return;
				QProgressDialog progress("Saving file...", "", 0, 10, this);
				progress.setWindowModality(Qt::WindowModal);
				progress.setFixedWidth(300);
				progress.setLabelText("Compressing file...");
				progress.setCancelButton(0);
				progress.setValue(0);
				progress.show();
				progress.setBar(0);
				progress.setValue(2);
				QCoreApplication::processEvents();
				std::vector<SpriteSheet> ssv;
				ImageWidget *imageWidget = currentImageWindow();
				if (!imageWidget)
				{
					for (int i = 0; i < _imageAreaList.size(); ++i)
					{
						ImageArea* imageArea = _imageAreaList[i];
						QString name = imageArea->name;
						QString area = imageArea->toAreaString();
						SpriteSheet ss;
						memset(ss.tag, 0, sizeof(ss.tag));
						strcpy(ss.tag, name.toStdString().c_str());
						ss.taglen = strlen(ss.tag);
						area.remove(0, 1);
						area.remove(area.length() - 1, 1);
						QStringList strl = area.split(",");
						assert(strl.size() == 4);
						ss.ltx = strl[0].toInt();
						ss.lty = strl[1].toInt();
						ss.rbx = strl[2].toInt();
						ss.rby = strl[3].toInt();
						ss.offsetx = imageArea->getOffsetX();
						ss.offsety = imageArea->getOffsetY();
						ssv.push_back(ss);
					}
				}
				else
				{
					for (int i = 0; i < imageWidget->_imageAreaList.size(); ++i)
					{
						ImageArea* imageArea = imageWidget->_imageAreaList[i];
						QString name = imageArea->name;
						QString area = imageArea->toAreaString();
						SpriteSheet ss;
						memset(ss.tag, 0, sizeof(ss.tag));
						strcpy(ss.tag, name.toStdString().c_str());
						ss.taglen = strlen(ss.tag);
						area.remove(0, 1);
						area.remove(area.length() - 1, 1);
						QStringList strl = area.split(",");
						assert(strl.size() == 4);
						ss.ltx = strl[0].toInt();
						ss.lty = strl[1].toInt();
						ss.rbx = strl[2].toInt();
						ss.rby = strl[3].toInt();
						ss.offsetx = imageArea->getOffsetX();
						ss.offsety = imageArea->getOffsetY();
						ssv.push_back(ss);
					}
				}
				progress.setValue(rand()%5);
				QCoreApplication::processEvents();
				imageOp->getImage()->compressImage(dlg._packcm, &db);
				CompressKTX(kh, db, ssv, imageOp->getTextureTarget(), imageOp->getAlphaType(), file, dlg._packcm);
				progress.setValue(10);
				QCoreApplication::processEvents();
				imageWidget->setChanged(false);
				imageWidget->setImageFileName(file);
			}
		}
	}
	/*
	else if (numSubWindow > 1)
	{
		QList<ImageOp*> imageOps;
		for (int i = 0; i < numSubWindow; i++)
		{
			bool compressed = (dynamic_cast<ImageWidget*>(imageOp) == 0);
			ImageOp *imgOp = getImageOpWidget(subWindows[i]);
			if (imgOp != 0 &&
				((compressed && (dynamic_cast<ImageWidget*>(imgOp) == 0)) ||
				(!compressed&&dynamic_cast<ImageWidget*>(imgOp) != 0)) &&
				ImageOp::isSameFormatImage(*imageOp, *imgOp))
				imageOps.append(imgOp);
		}
		PackDialog dlg;
		dlg.setData(imageOps);
		if (dlg.exec() == QDialog::Accepted)
		{
			QList<ImageOp*> imgOps = dlg.getSelectItems();
			QString fileName = dlg.getSaveFile();
			if (!fileName.isEmpty())
			{
				int count = imgOps.size();
				for (int i = 0; i < count; i++)
					imgOps[i]->pack(db);
				kh.numberOfArrayElements = count;
				std::string fileStr = fileName.toStdString();
				const char *file = fileStr.c_str();
				CompressKTX(kh, db, imageOp->getTextureTarget(), imageOp->getAlphaType(), file, imageOp->getCompressMethod());
			}
		}
	}*/
}

//�˳�
void BMGTexTool::on_actionExit_triggered()
{
	QApplication::quit();

}
//��С��ͼ
void BMGTexTool::on_actionZoom_In_triggered()
{
	ImageOp *imageWidget = currentImageOpWidget();
	if (imageWidget != 0)
		imageWidget->zoomIn();
}
//�Ŵ���ͼ
void BMGTexTool::on_actionZoom_Out_triggered()
{

	ImageOp *imageWidget = currentImageOpWidget();
	if (imageWidget != 0)
		imageWidget->zoomOut();
}
//������ͼ
void BMGTexTool::on_actionReset_triggered()
{
	ImageOp *imageWidget = currentImageOpWidget();
	if (imageWidget != 0)
		imageWidget->resetZoom();

}
//Mipmap�ϲ�
void BMGTexTool::on_actionUpperMip_triggered()
{
	ImageOp *imageWidget = currentImageOpWidget();
	if (imageWidget != 0)
		imageWidget->higherMips();

}
//Mipmap�²�
void BMGTexTool::on_actionLowerMip_triggered()
{
	ImageOp *imageWidget = currentImageOpWidget();
	if (imageWidget != 0)
		imageWidget->lowerMips();

}
//����Mipmap
void BMGTexTool::on_actionGenerateMip_triggered()
{
	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp)
	{
		if ((imageOp->getImageType() == IMAGE3D))
			return;
		ImageOp *imageWidget = currentImageOpWidget();
		if (imageWidget != 0)
			imageWidget->generatorMips();
	}

}
//����Mipmap
void BMGTexTool::on_actionClean_triggered()
{
	ImageOp *imageWidget = currentImageOpWidget();
	if (imageWidget != 0)
		imageWidget->deleteAllMips();

}
//����Cubemap��
void BMGTexTool::on_actionGenerateCube_triggered()
{
	Image *image = new ImageCube;
	ImageWidget *subWindow = new ImageWidget(this);
	QMdiSubWindow *sw = _mdiArea->addSubWindow(subWindow);
	sw->show();
	subWindow->setImage(image, QString("cube map template"), 0, 0);
	imageWindowActived(sw);
	activeCubeBar(true);
}
//px face
void BMGTexTool::on_actionPx_triggered()
{
	activeFace(POSITIVE_X);

}
//nx face
void BMGTexTool::on_actionNx_triggered()
{

	activeFace(NEGATIVE_X);
}
//py face
void BMGTexTool::on_actionPy_triggered()
{
	activeFace(POSITIVE_Y);

}
//ny face
void BMGTexTool::on_actionNy_triggered()
{
	activeFace(NEGATIVE_Y);

}
//pz face
void BMGTexTool::on_actionPz_triggered()
{
	activeFace(POSITIVE_Z);

}
//nz face
void BMGTexTool::on_actionNz_triggered()
{
	activeFace(NEGATIVE_Z);

}
//3D map �ϲ�
void BMGTexTool::on_actionUpper3D_triggered()
{
	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp != 0)
		imageOp->preVolume();

}
//����3dmap
void BMGTexTool::on_actionGenerate3D_triggered()
{
	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp->getImageType() != IMAGE2D)
		return;

	ImageWidget *imageWidget = currentImageWindow();
	if (!imageWidget)
		return;
	QString path = imageWidget->getImageFileName();
	Image2D *image2 = dynamic_cast<Image2D*>(imageOp->getImage());
	if (image2 == 0)
		return;

	if (image2->canConvert3D())
	{
		int width = image2->imageWidth();
		Create3DDialog dlg;
		dlg.setPicWidth(width);
		if (dlg.exec() == QDialog::Accepted)
		{
			int volumeNum = dlg.getSelectVolumeNum();
			Image3D *image3 = image2->getImage3D(width / volumeNum);
			_mdiArea->closeActiveSubWindow();

			ImageWidget *widget = new ImageWidget;
			QMdiSubWindow *sw = _mdiArea->addSubWindow(widget);
			sw->show();
			widget->setImage((Image*)image3, tr("3D Texture"), 0 , 0);
			widget->setChanged();
			widget->setBMG(path);			
			imageWindowActived(sw);
		}
	}

}
//3D map �²�
void BMGTexTool::on_actionLower3D_triggered()
{
	ImageOp *imageOp = currentImageOpWidget();
	if (imageOp != 0)
		imageOp->nextVolume();
}
//�رյ�ǰ�ļ�
void BMGTexTool::on_actionClose_triggered()
{
	QMdiSubWindow *subWindow = _mdiArea->currentSubWindow();
	if (subWindow)
		_mdiArea->removeSubWindow(subWindow);
}
//�������ļ�
void BMGTexTool::on_actionBatch_triggered()
{
	QString dir = QFileDialog::getExistingDirectory();
	if (dir.isEmpty())
		return;
	QString logdir = dir;
	logdir += "_batch.log";
	//QChar cc = dir.at(dir.length() - 1);
	PackDialog dlg; dlg._packcm = CMD_NONE;
	if (dlg.exec() == QDialog::Accepted)
	{
		QString andr = dir;
		//andr.remove(dir.length() - 1);
		switch (dlg._packcm)
		{
		case CMD_NONE:
			andr += "_lossy";
			break;
		case CM_S3TC:
			andr += "_s3tc";
			break;
		case CM_ASTC:
			andr += "_astc";
			break;
		case CM_ETC2:
			andr += "_etc2";
			break;
		}
		QDir ddd;
		ddd.mkdir(andr);
		Batch bat;
		bat.setDestForder(andr);
		bat.setLogDir(logdir, dir, dlg._packcm, this);
		//bat.exec();
	}
}
//ѹ��
void BMGTexTool::on_actionCompress_triggered()
{
	QList<ImageArea*> imagearealist;
	QString showmsg;
	{
		ImageWidget *imageWidget = currentImageWindow();
		if (!imageWidget)
			return;
		imagearealist = imageWidget->_imageAreaList;
	}
	CompressMethord cm = CMD_NONE;
	if (this->ui.actionASTC->isChecked())
	{
		cm = CM_ASTC;
	}
	else if (this->ui.actionS3TC->isChecked())
	{
		cm = CM_S3TC;
	}
	else if (this->ui.actionETC2->isChecked())
	{
		cm = CM_ETC2;
	}
	else
		return;
	ImageWidget *imageWidget = currentImageWindow();
	if (imageWidget != 0 && imageWidget->getImage()->canCompress())
	{
		Image *image = imageWidget->getImage();
		ImageCompareWidget *widget = new ImageCompareWidget;
		_imageAreaList.clear();
		_imageAreaList = imageWidget->_imageAreaList;
		widget->setImage(image, imageWidget->getImageFileName(), cm);
		_mdiArea->closeActiveSubWindow();
		_mdiArea->addSubWindow(widget);
		widget->show();
		widget->adjustWindowSize();//call twice
		widget->adjustWindowSize();

		QImage* qimg = image->getImage();
		showmsg += "[";
		showmsg += "Width:";
		showmsg += QString::number(qimg->width());
		showmsg += "  ";
		showmsg += "Height:";
		showmsg += QString::number(qimg->height());
		showmsg += "  Codec:";
		if (cm == CM_ASTC)
			showmsg += "ASTC]";
		else if (cm == CM_S3TC)
			showmsg += "S3TC]";
		else if (cm == CM_ETC2)
			showmsg += "ETC2]";

		for (int i = 0; i <imagearealist.size(); ++i)
		{
			ImageArea* imageArea = imagearealist[i];
			QTreeWidgetItem *item = new QTreeWidgetItem;
			item->setText(0, imageArea->name);
			item->setText(1, imageArea->toAreaString());
			item->setData(2, Qt::UserRole, QVariant::fromValue(ImageAreaRef(imageArea)));
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			this->ui.treeWidget->addTopLevelItem(item);
		}
		this->ui.statusBar->showMessage(showmsg);
	}

	this->ui.actionPx->setChecked(true);
	activeCubeBar(false);
	active3DBar(false);
	
}

ImageArea * BMGTexTool::getTreeItemData(QTreeWidgetItem *item)
{
	QVariant v = item->data(2, Qt::UserRole);
	if (v.canConvert<ImageAreaRef>())
	{
		ImageAreaRef areaRef = v.value<ImageAreaRef>();
		return areaRef.imageArea;
	}
	return 0;
}


void BMGTexTool::treeItemSelectedChanged()
{
	QTreeWidgetItem *item = this->ui.treeWidget->currentItem();
	if (item != 0)
	{
		ImageArea *image = getTreeItemData(item);
		ImageWidget* ji = dynamic_cast<ImageWidget*>(currentImageOpWidget());
		if (ji)
			ji->setSelectImage(image);
	}
}


void BMGTexTool::treeItemEdited(QTreeWidgetItem *item, int column)
{
	ImageArea *image = getTreeItemData(item);
	assert(image != 0);
	if (column == 0)
	{
		QList<QTreeWidgetItem*> items = this->ui.treeWidget->findItems(item->text(0), Qt::MatchExactly | Qt::MatchCaseSensitive);
		ImageWidget* ji = dynamic_cast<ImageWidget*>(currentImageOpWidget());
		if (ji)
		{
			ji->setModefied(image);
			ji->setModefied2(item->text(0));
		}
		if (items.count() > 1)
			item->setText(0, image->name);
		else
			image->name = item->text(0);
	}
	else if (column == 1)
	{
		item->setText(1, image->toAreaString());
	}
}


void BMGTexTool::imageAdded(ImageArea *imageArea)
{
	assert(imageArea != 0);
	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setText(0, imageArea->name);
	item->setText(1, imageArea->toAreaString());
	item->setData(2, Qt::UserRole, QVariant::fromValue(ImageAreaRef(imageArea)));
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	this->ui.treeWidget->addTopLevelItem(item);
}

void BMGTexTool::imageAreaChanged(ImageArea *imageArea)
{
	assert(imageArea != 0);
	int count = this->ui.treeWidget->topLevelItemCount();
	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *item = this->ui.treeWidget->topLevelItem(i);
		if (getTreeItemData(item) == imageArea)
		{
			item->setText(1, imageArea->toAreaString());
			QMdiSubWindow *sw = _mdiArea->currentSubWindow();
			imageWindowActived(sw);
			return;
		}
	}
}

void BMGTexTool::imageSelected(ImageArea *imageArea)
{
	if (imageArea == 0)
		this->ui.treeWidget->setCurrentItem(0);

	int count = this->ui.treeWidget->topLevelItemCount();
	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *item = this->ui.treeWidget->topLevelItem(i);
		if (getTreeItemData(item) == imageArea)
		{
			this->ui.treeWidget->setCurrentItem(item);
			return;
		}
	}
}

