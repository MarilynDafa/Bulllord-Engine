#include "batch.h"
#include "Image.h"
#include <qtimer.h>
#include <qcoreapplication.h>
#include <qdir.h>
#include "Codec.h"
#include<qprogressdialog.h>
#include <qdatetime.h>
Batch::Batch(QObject* obj)
	: QObject(obj)
{
	processed = 0;
}


QFileInfoList GetFileList(QString path)
{
	QDir dir(path);
	//QStringList filters;	           
	//filters << "*.png" << "*.tif" << "*.jpg" << "*.dds" << "*.jpeg" << "*.bmp" << "*.pbm"<< "*.pgm" << "*.ppm" << "*.tiff" << "*.xbm" << "*.xpm"<<"*.bmg";
	//dir.setNameFilters(filters);
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i != folder_list.size(); i++)
	{
		QString name = folder_list.at(i).absoluteFilePath();
		QFileInfoList child_file_list = GetFileList(name);
		file_list.append(child_file_list);
	}

	return file_list;
}

static void queryFiles(const char* rootDir, std::vector<std::string>& strRet, const std::string& localpath)
{
	QFileInfoList xxx = GetFileList(rootDir);
	for (int i = 0; i != xxx.size(); i++)
	{
		strRet.push_back(xxx.at(i).absoluteFilePath().toStdString());
	}
	for (int i = 0; i != strRet.size(); i++)
	{
		strRet[i] = strRet[i].substr(localpath.length());
	}


}
Batch::~Batch()
{
}
void Batch::setLogDir(const QString& logdir, const QString& root, CompressMethord cm, QWidget* wid)
{
	_cm = cm;
	_logdir = logdir;
	_root = root;
	_logfp = fopen(_logdir.toStdString().c_str(), "wb");
	QString title;
	title = "Texture Batch Log : ";
	title += QDate::currentDate().toString();
	title += "\r\n";

	fwrite(title.toStdString().c_str(), 1, title.length(), _logfp);
	fflush(_logfp);
	//!文件总数信息
	title = "\r\nTotal Texture Files :";
	//遍历图像文件压入workingqueue
	queryFiles(root.toStdString().c_str(), fileinfolist, root.toStdString().c_str());
	//!写入总数信息
	title += QString::number(fileinfolist.size());
	title += "\r\n\r\n";
	fwrite(title.toStdString().c_str(), 1, title.length(), _logfp);
	fflush(_logfp);
	int numFiles = fileinfolist.size();
	/*
	_pd = new QProgressDialog("Processing files...", "Abort", 0, numFiles);
	_pd->setFixedWidth(300);
	_pd->show();
	_pd->setValue(0);
	connect(_pd, SIGNAL(canceled()), this, SLOT(cancel()));
	_t = new QTimer();
	connect(_t, SIGNAL(timeout()), this, SLOT(Event()));
	_t->start(100);
	while (!Event2())
	{
		;
	}

	_t->stop();//停止定时器
	_pd->setValue(processed);//进度达到最大值
	delete _pd;*/

	progress = new QProgressDialog("Processing files...", "", 0, numFiles, wid);
	progress->setWindowModality(Qt::WindowModal);
	progress->setFixedWidth(450);
	progress->setCancelButton(0);
	progress->setValue(0);
	progress->show();
	while (!Event2())
	{
		progress->setValue(processed);
		if (fileinfolist.size())
		{
			std::string fi = fileinfolist.back();
			QString llable = "processing ";
			llable += fi.c_str();
			progress->setLabelText(llable);
		}
		progress->update();
		QCoreApplication::processEvents();
		if (progress->wasCanceled())
			break;
	}
	progress->setValue(numFiles);
}
void Batch::setDestForder(const QString& dest)
{
	_dest = dest;
}

void Batch::cancel()
{
	_t->stop();
	//... cleanup
}

void Batch::Event()
{
	progress->setValue(processed);
	
	QCoreApplication::processEvents();//避免界面冻结
}


bool Batch::Event2()
{
	//~取一个
	if (fileinfolist.empty())
	{
		QString endine = QString::number(processed);
		endine += " files convert finished\r\n";
		fwrite(endine.toStdString().c_str(), 1, endine.length(), _logfp);
		endine = "processed by BMGTexTool";
		fwrite(endine.toStdString().c_str(), 1, endine.length(), _logfp);
		fclose(_logfp);
		return true;
	}
	else
	{
		std::string fi = fileinfolist.back();
		QString llable = "processing ";
		llable += fi.c_str();
		if (isImage(QString::fromStdString(fi)))
		{
			//!解压缩 打包 创建文件夹
			QString path = _root + QString::fromStdString(fi);
			std::vector<SpriteSheet> ssv;
			int offsetx, offsety;
			QList<Image*> images = Image::createImages(path, ssv, offsetx, offsety);

			Data_block db;
			BMG_header_TT kh;
			kh.pixelWidth = images[0]->imageWidth();
			kh.pixelHeight = images[0]->imageHeight();
			kh.pixelDepth = images[0]->imageDepth();
			kh.numberOfMipmapLevels = images[0]->getNumMipmaps();
			kh.numberOfFaces = images[0]->getFaceCount();
			kh.numberOfArrayElements = 1;
			if (ssv.empty())
			{
				SpriteSheet ss;
				QFileInfo fileInfo(path);
				ss.taglen = fileInfo.baseName().length();
				memset(ss.tag, 0, sizeof(ss.tag));
				strcpy(ss.tag, fileInfo.baseName().toStdString().c_str());
				ss.ltx = ss.lty = 0;
				ss.rbx = images[0]->imageWidth();
				ss.rby = images[0]->imageHeight();
				ss.offsetx = offsetx;
				ss.offsety = offsety;
				ssv.push_back(ss);
			}

			QString dpath = _dest + QString::fromStdString(fi);
			QDir qdir;
			qdir.mkdir(getPath(dpath));
			images[0]->compressImage(_cm, &db);
			IMAGETYPE it = images[0]->getImageType();
			switch (it)
			{
			case IMAGE1D:
				CompressKTX(kh, db, ssv, TT_1D, images[0]->isComplexAlpha(), changeExt(dpath).toStdString().c_str(), _cm);
				break;
			case IMAGE2D:
				CompressKTX(kh, db, ssv, TT_2D, images[0]->isComplexAlpha(), changeExt(dpath).toStdString().c_str(), _cm);
				break;
			case IMAGE3D:
				CompressKTX(kh, db, ssv, TT_3D, images[0]->isComplexAlpha(), changeExt(dpath).toStdString().c_str(), _cm);
				break;
			case IMAGECUBE:
				CompressKTX(kh, db, ssv, TT_CUBE, images[0]->isComplexAlpha(), changeExt(dpath).toStdString().c_str(), _cm);
				break;
			case IMAGENO:
				break;
			}
		}
		else
		{
			//~简单拷贝
			QString path = _root + QString::fromStdString(fi);
			FILE* xfp = fopen(path.toStdString().c_str(), "rb");
			fseek(xfp, 0, SEEK_END);
			size_t  sz = ftell(xfp);
			unsigned char* buf = (unsigned char*)malloc(sz);
			fseek(xfp, 0, SEEK_SET);
			fread(buf, 1, sz, xfp);
			fclose(xfp);
			QString dpath = _dest + QString::fromStdString(fi);
			QDir qdir;
			qdir.mkdir(getPath(dpath));
			xfp = fopen(dpath.toStdString().c_str(), "wb");
			fwrite(buf, 1, sz, xfp);
			fclose(xfp);
			free(buf);
		}
		writelog(fi.c_str());
		fileinfolist.pop_back();
		processed++;
	}
	return false;
}



QString Batch::getPath(const QString& parh)
{
	QString ret = parh;
	while (ret.at(ret.length() - 1) != '/')
	{
		ret.remove(ret.length() - 1, 1);
	}
	ret.remove(ret.length() - 1, 1);
	return ret;
}
QString Batch::changeExt(const QString& parh)
{
	QString ret = parh;
	while (ret.at(ret.length() - 1) != '.')
	{
		ret.remove(ret.length() - 1, 1);
	}
	ret += "bmg";
	return ret;
}
bool Batch::isImage(const QString& file)
{
	if (file.contains(".png"))
		return true;
	else if (file.contains(".tif"))
		return true;
	else if (file.contains(".jpg"))
		return true;
	else if (file.contains(".dds"))
		return true;
	else if (file.contains(".jpeg"))
		return true;
	else if (file.contains(".bmp"))
		return true;
	else if (file.contains(".pbm"))
		return true;
	else if (file.contains(".pgm"))
		return true;
	else if (file.contains(".ppm"))
		return true;
	else if (file.contains(".tiff"))
		return true;
	else if (file.contains(".xbm"))
		return true;
	else if (file.contains(".xpm"))
		return true;
	else if (file.contains(".bmg"))
		return true;
	else
		return false;
}

void Batch::writelog(const QString& file)
{
	QString info;
	if(isImage(file))
		info = "processing>";
	else
		info = "copy>";
	info += file;
	info += "\r\n";
	fwrite(info.toStdString().c_str(), 1, info.length(), _logfp);
	fflush(_logfp);
}
