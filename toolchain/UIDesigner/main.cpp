#include <QtCore/QFile>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTextCodec>

#include "MainWindow.h"
#include "Config.h"
#include "UIMgr.h"
#include "ResourceMgr.h"
#include "Codec.h"
bool setCurrentDir(QString dir)
{
	QDir dirPath(dir);
	if(!dirPath.exists())
	{
		if(!dirPath.mkpath(dirPath.absolutePath()))
		{
			QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Can't make workspace"), QMessageBox::Ok);
			return false;
		}
	}
	QDir::setCurrent(dir);

	QString fnt = GlobalConfig::getSingletonPtr()->value("fnt_dir", "font").toString();
	QString skn = GlobalConfig::getSingletonPtr()->value("skin_dir", "pixmap").toString();
	QString itf = GlobalConfig::getSingletonPtr()->value("bui_dir", "bui").toString();

	if(!dirPath.exists(fnt))
		dirPath.mkdir(fnt);
	if(!dirPath.exists(skn))
		dirPath.mkdir(skn);
	if(!dirPath.exists(itf))
		dirPath.mkdir(itf);

	return true;
}

class DefaultSkin
{
public:
	DefaultSkin()
	{
		{
			u32 imgData[256];
			for (int i = 0; i < 16; ++i)
			{
				for (int j = 0; j < 16; ++j)
				{
					if ((i / 2 + j / 2) % 2 == 0)
						imgData[i * 16 + j] = 0x00000000;
					else
						imgData[i * 16 + j] = 0xFFFFFFFF;
				}
			}

			QFileInfo tmpFileInfo(QDir::tempPath(), "default_ui_skin_name.bmg");
			QString fileName = tmpFileInfo.absoluteFilePath();
			if (QFile::exists(fileName))
				QFile::remove(fileName);

			BMG_header_TT header;
			header.pixelWidth = 16;
			header.pixelHeight = 16;
			Data_block data;
			data.Data.push_back((unsigned char*)imgData);
			std::vector<SpriteSheet> ssc;
			SpriteSheet ss;
			strcpy(ss.tag, "Nil");
			ss.taglen = strlen("Nil");
			ss.ltx = 0;
			ss.lty = 0;
			ss.rbx = 16;
			ss.rby = 16;
			ssc.push_back(ss);
			Texture_target tt;
			tt = TT_2D;
			Alpha_type at;
			at = AT_COMPLEX_ALPHA;
			encode_bmg(header, data, tt, at);
			CompressKTX(header, data, ssc, tt, at, fileName.toStdString().c_str(), CMD_NONE);
			ResourceMgr::getInstance()->addSkin(fileName);

		}

		{
			u32 imgData[256] = { 0 };

			QFileInfo tmpFileInfo(QDir::tempPath(), "default_ui_skin_name2.bmg");
			QString fileName = tmpFileInfo.absoluteFilePath();
			if (QFile::exists(fileName))
				QFile::remove(fileName);

			BMG_header_TT header;
			header.pixelWidth = 16;
			header.pixelHeight = 16;
			Data_block data;
			data.Data.push_back((unsigned char*)imgData);
			std::vector<SpriteSheet> ssc;
			SpriteSheet ss;
			strcpy(ss.tag, "Nil");
			ss.taglen = strlen("Nil");
			ss.ltx = 0;
			ss.lty = 0;
			ss.rbx = 16;
			ss.rby = 16;
			ssc.push_back(ss);
			Texture_target tt;
			tt = TT_2D;
			Alpha_type at;
			at = AT_COMPLEX_ALPHA;
			encode_bmg(header, data, tt, at);
			CompressKTX(header, data, ssc, tt, at, fileName.toStdString().c_str(), CMD_NONE);
			ResourceMgr::getInstance()->addSkin(fileName);

		}

	}

	~DefaultSkin()
	{
		{
			QFileInfo tmpFileInfo(QDir::tempPath(), "default_ui_skin_name.bmg");
			QString fileName = tmpFileInfo.absoluteFilePath();
			if (QFile::exists(fileName))
				QFile::remove(fileName);
		}
		{
			QFileInfo tmpFileInfo(QDir::tempPath(), "default_ui_skin_name2.bmg");
			QString fileName = tmpFileInfo.absoluteFilePath();
			if (QFile::exists(fileName))
				QFile::remove(fileName);
		}
	}
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
	//QTextCodec::setCodecForTr(QTextCodec::codecForLocale());

	WorkspaceSelectDialog *dlg = new WorkspaceSelectDialog;
	if(dlg->exec() == QDialog::Rejected ||
		!setCurrentDir(dlg->getSelectedWorkSpace()))
	{
		delete dlg;
		return 0;
	}
	delete dlg;

	MainWindow *win = new MainWindow;
	win->show();
	QObject::connect(&a, SIGNAL(focusChanged(QWidget*, QWidget*)), 
		win, SLOT(slotFocusChanged(QWidget*, QWidget*)));

	ResourceMgr::getInstance()->init();
	DefaultSkin setDefaultSkin;

	int rel =  a.exec();
	delete win;
	return rel;
}
