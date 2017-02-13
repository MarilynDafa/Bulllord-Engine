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
		/*static QString xmlStr = 
			"\n<?xml version=\"1.0\"?>\n"
			"<skin name=\"default_ui_skin_name\">\n"
			"<texture>\n"
			"<attribute width=\"16\"/>\n"
			"<attribute height=\"16\"/>\n"
			"</texture>\n"
			"<texcoords>\n"
			"<image name=\"all\" coord=\"0,0,16,16\"/>\n"
			"</texcoords>\n"
			"</skin>\n";
		int size = xmlStr.toLatin1().size();
		*/
		u32 imgData[256] ;
		for(int i = 0 ; i < 16 ; ++i)
		{
			for(int j = 0; j < 16 ; ++j)
			{
				if((i / 2 + j / 2) % 2 == 0)
					imgData[i*16 + j] = 0xFF000000;
				else
					imgData[i*16 + j] = 0xFFFFFFFF;
			}
		}

		QFileInfo tmpFileInfo(QDir::tempPath(), "default_ui_skin_name.bmg");
		QString fileName = tmpFileInfo.absoluteFilePath();
		if(QFile::exists(fileName))
			QFile::remove(fileName);
		/*
		FILE* fp = fopen(fileName.toStdString().c_str() , "wb");
		unsigned short header = UCID_HEADER;
		fwrite(&header , 1 , sizeof(short) , fp);		
		unsigned short desc = UCID_DESC;
		fwrite(&desc , sizeof(desc) , 1 , fp);
		unsigned int w = 16;
		unsigned int h = 16;
		fwrite(&w , sizeof(int) , 1 , fp);
		fwrite(&h , sizeof(int) , 1 , fp);
		unsigned short frag = UCID_FRAGMENT;
		fwrite(&frag , sizeof(frag) , 1 , fp);
		int ltx =0, lty=0 , rbx=16 , rby=16;
		fwrite(&ltx , sizeof(int) , 1 , fp);
		fwrite(&lty , sizeof(int) , 1 , fp);
		fwrite(&rbx , sizeof(int) , 1 , fp);
		fwrite(&rby , sizeof(int) , 1 , fp);
		const char* tag = "Nil";
		fwrite(tag , strlen(tag) , 1 , fp);
		unsigned char en = 0;
		fwrite(&en , sizeof(char) , 1 , fp);

		
		unsigned short tex = UCID_TEXTURE;
		fwrite(&tex , sizeof(tex) ,1, fp);
		unsigned int sz = 256*sizeof(u32);
		fwrite(&sz , sizeof(int) , 1 , fp);




		fwrite((const char*)imgData , sizeof(u32) , 256 , fp);
		fclose(fp);*/
		//QFile file(fileName);
		//if(!file.open(QIODevice::ReadWrite))
		//return;
		//file.write(QByteArray((const char*)&size, sizeof(int)));
		//file.write(xmlStr.toLatin1());
		//file.write(QByteArray((const char*)imgData, sizeof(u32)*256));
		//file.close();

		//c_ui_manager::get_singleton_ptr()->add_skin(UiWidget::fromQString(fileName));
		BMG_header_TT header;
		header.pixelWidth = 16;
		header.pixelHeight = 16;
		Data_block data;
		data.Data.push_back((unsigned char*)imgData);
		std::vector<SpriteSheet> ssc;
		SpriteSheet ss;
		strcpy(ss.tag ,"Nil");
		ss.taglen = strlen("Nil");
		ss.ltx = 0;
		ss.lty = 0;
		ss.rbx = 16;
		ss.rby = 16;
		ssc.push_back(ss);
		Texture_target tt;
		tt = TT_2D;
		Alpha_type at;
		at = AT_NONE_ALPHA;
		encode_bmg(header, data, tt, at);
		CompressKTX(header, data, ssc, tt, at, fileName.toStdString().c_str(), CMD_NONE);
		ResourceMgr::getInstance()->addSkin(fileName);
	}

	~DefaultSkin()
	{
		QFileInfo tmpFileInfo(QDir::tempPath(), "default_ui_skin_name.bmg");
		QString fileName = tmpFileInfo.absoluteFilePath();
		if(QFile::exists(fileName))
			QFile::remove(fileName);
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
