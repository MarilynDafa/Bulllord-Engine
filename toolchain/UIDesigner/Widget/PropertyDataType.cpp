#include "PropertyDataType.h"
#include "UiWidget.h"
#include "UIMgr.h"
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtGui/QImage>
#include "Config.h"
#include "FontSetDialog.h"
#include "FileMgr.h"
#include "ResourceMgr.h"
#include "Codec.h"
QMap<QString, QImage*> _golbaMap;
//////////////////////////////////////////////////////////////////////////
void SkinFile::initNames()
{
	QString sknDirStr = GlobalConfig::getSingletonPtr()->value("skin_dir", "pixmap").toString();
	QDir currentDir = QDir::current();
	QDir sknDir = currentDir;
	sknDir.cd(sknDirStr);

	QFileInfo fileInfo(_pathName);
	QDir srcDir = fileInfo.absoluteDir();
	bool addgb = false;
	if(sknDir != srcDir)
	{
		QFileInfo skinFileInfo(sknDir, fileInfo.fileName());
		if(!skinFileInfo.exists())
		{
			QFile::copy(_pathName, skinFileInfo.absoluteFilePath());
			_pathName = skinFileInfo.absoluteFilePath();
			addgb = true;
		}
		else
		{
			QFile file(skinFileInfo.absoluteFilePath());
			file.remove();
			file.flush();
			QFile::copy(_pathName, skinFileInfo.absoluteFilePath());
			_pathName = skinFileInfo.absoluteFilePath();
			addgb = true;
		}
	}

	_names.clear();
	_imageArea.clear();

	if(_pathName.isEmpty())
		return;
	_skinName = fileInfo.baseName();
	QMap<QString, QImage*>::iterator iter = _golbaMap.find(_skinName);
	if (!addgb && iter != _golbaMap.end())
	{
		std::vector<SpriteSheet> ssv;
		BMG_header_TT header;
		FILE* fp = fopen(_pathName.toStdString().c_str(), "rb");
		fread(&header, sizeof(BMG_header_TT), 1, fp);
		while (ftell(fp) < header.offset)
		{
			SpriteSheet ss;
			fread(&ss.taglen, 1, sizeof(int), fp);
			memset(ss.tag, 0, sizeof(ss.tag));
			fread(&ss.tag, ss.taglen, sizeof(char), fp);
			fread(&ss.ltx, 1, sizeof(int), fp);
			fread(&ss.lty, 1, sizeof(int), fp);
			fread(&ss.rbx, 1, sizeof(int), fp);
			fread(&ss.rby, 1, sizeof(int), fp);
			ssv.push_back(ss);
		}
		_image = iter.value();
		for (int i = 0; i < ssv.size(); ++i)
		{
			_names.append(QString((char*)ssv[i].tag));
			_imageArea[QString((char*)ssv[i].tag)] = QRect(QPoint(ssv[i].ltx, ssv[i].lty), QPoint(ssv[i].rbx - 1, ssv[i].rby - 1));
		}
		fclose(fp);
	}
	else
	{
		Data_block data;
		std::vector<SpriteSheet> ssv;
		BMG_header_TT header;
		Texture_target tt;
		UnCompressKTX(_pathName.toStdString().c_str(), data, ssv, header, tt);

		for (int i = 0; i < ssv.size(); ++i)
		{
			_names.append(QString((char*)ssv[i].tag));
			_imageArea[QString((char*)ssv[i].tag)] = QRect(QPoint(ssv[i].ltx, ssv[i].lty), QPoint(ssv[i].rbx - 1, ssv[i].rby - 1));
		}
		QRgb *colorData = new QRgb[header.pixelWidth*header.pixelHeight];
		for (int i = 0; i <header.pixelWidth*header.pixelHeight; i++)
			colorData[i] = qRgba(data.Data[0][i * 4], data.Data[0][i * 4 + 1], data.Data[0][i * 4 + 2], data.Data[0][i * 4 + 3]);
		_image = new QImage((unsigned char *)colorData, header.pixelWidth, header.pixelHeight, QImage::Format_ARGB32);
		//delete[] colorData;
		free(data.Data[0]);
		free(data.CompressData[0]);
		_golbaMap[_skinName] = _image;
	}
	/*
	while(pos< sz)
	{
		memcpy(&sid, buf + pos, sizeof(sid));
		pos+=sizeof(sid);

		switch(sid)
		{
		case UCID_DESC:
			memcpy(&w, buf + pos, sizeof(w));
			pos+=sizeof(w);
			memcpy(&h, buf + pos, sizeof(h));
			pos+=sizeof(h);
			break;
		case UCID_FRAGMENT:
			{
				int ltx , lty , rbx , rby;
				memcpy(&ltx, buf + pos, sizeof(ltx));
				pos+=sizeof(ltx);
				memcpy(&lty, buf + pos, sizeof(lty));
				pos+=sizeof(lty);
				memcpy(&rbx, buf + pos, sizeof(rbx));
				pos+=sizeof(rbx);
				memcpy(&rby, buf + pos, sizeof(rby));
				pos+=sizeof(rby);


				unsigned char tmp[128];
				memset(tmp, 0, sizeof(tmp));
				memcpy(tmp, buf + pos, sizeof(char));
				pos+=sizeof(char);
				size_t i = 0;
				while(tmp[i])
				{
					++i;
					memcpy(tmp + i, buf + pos, sizeof(char));
					pos+=sizeof(char);
				}
				_names.append(QString((char*)tmp));
				_imageArea[QString((char*)tmp)] = QRect(QPoint(ltx, lty), QPoint(rbx, rby));
			}
			break;
		case UCID_TEXTURE:
			{
				unsigned int sz;
				memcpy(&sz, buf + pos, sizeof(sz));
				pos+=sizeof(sz);

				const unsigned char *data = (unsigned char *)buf + pos;
				int imageDataSize  = sz/4;
				QRgb *colorData = new QRgb[w*h];
				for(int i = 0; i < w*h; i ++)
					colorData[i] = qRgba(data[i*4], data[i*4 + 1], data[i*4 + 2], data[i*4 + 3]);
				_image = QImage((unsigned char *)colorData, w, h, QImage::Format_ARGB32);
				pos+=sz;
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	*/
	/*
	QFile file(_pathName);
	if(!file.open(QIODevice::ReadOnly))
	return;
	QByteArray fileData = file.readAll();
	file.close();
	int xmlSize = *((int *)fileData.left(sizeof(int)).data());
	QDomDocument doc;
	QString xmlStr(fileData.mid(sizeof(int), xmlSize));
	if(!doc.setContent(xmlStr.trimmed().toLatin1()))
	return;

	QDomElement skinEle = doc.documentElement();
	if(skinEle.tagName() != QString("skin"))
	return;
	_skinName = skinEle.attribute(QString("name"));

	QDomNode node = skinEle.firstChild();
	while(!node.isNull())
	{
	QDomElement ele = node.toElement();
	if(ele.tagName() == QString("texcoords"))
	{
	QDomNode n = ele.firstChild();
	while(!n.isNull())
	{
	QDomElement e = n.toElement();
	if(e.tagName() == QString("image"))
	_names.append(e.attribute(QString("name")));
	n = n.nextSibling();
	}
	}
	node = node.nextSibling();
	}*/
}

QImage SkinFile::getImage( const QString &name ) const
{
	QMap<QString, QRect>::const_iterator iter = _imageArea.find(name);
	if (iter != _imageArea.end())
	{
		QRect rect = *iter;
		int x = rect.x();
		int y = rect.y();
		int w = rect.width();
		int h = rect.height();
		return _image->copy(x,y,w-1,h-1);
	}
	return QImage();
}

//////////////////////////////////////////////////////////////////////////

QStringList FontType::getFontsNameList()
{
	QStringList nameList;
	QVectorIterator<FontFile> fIter(_fontList);
	while(fIter.hasNext())
		nameList.append(fIter.next().getFontName());
	return nameList;
}


unsigned int FontType::getFontID()
{
	return 0;
}

void FontType::initFontList()
{
	QDir currentDir = QDir::current();
	QString fntDir = GlobalConfig::getSingletonPtr()->value("fnt_dir", "font").toString();
	currentDir.cd(fntDir);
	QFileInfoList fileInfos = currentDir.entryInfoList(QStringList()<<"*.ttf", QDir::Files);
	QListIterator<QFileInfo> iter(fileInfos);
	while(iter.hasNext())
	{
		QFileInfo fileInfo = iter.next();
		addFont(fileInfo.absoluteFilePath());
	}
	fileInfos = currentDir.entryInfoList(QStringList() << "*.ttc", QDir::Files);
	QListIterator<QFileInfo> iter2(fileInfos);
	while (iter2.hasNext())
	{
		QFileInfo fileInfo = iter2.next();
		addFont(fileInfo.absoluteFilePath());
	}
}

FontFile FontType::addFont( const QString &fileName )
{
	if(fileName.isEmpty())
		return FontFile();

	QFileInfo fileInfo(fileName);
	if(fileInfo.suffix().compare(QString::fromLatin1("ttf"), Qt::CaseInsensitive) == 0 || fileInfo.suffix().compare(QString::fromLatin1("ttc"), Qt::CaseInsensitive) == 0)
	{
		QString fontName = getFontName(fileName);

		int count = _fontList.count();
		for(int i = 0; i < count; i ++)
		{
			FontFile &fontFile = _fontList[i];
			if(fontName == fontFile.getFontName())
			{
				return fontFile;
			}
		}

		FontFile fontFile(fontName, QString());
		fontFile.setFileName(fileName);
		_fontList.append(fontFile);
		//c_ui_manager::get_singleton_ptr()->add_font(UiWidget::fromQString(fontFile.getFileName(true)));
		ResourceMgr::getInstance()->addFont(fontFile.getFileName(true));
		return fontFile;
	}

	return FontFile();
}

QString FontType::getFontName( const QString &fontFileName )
{
	QVectorIterator<FontFile> fontIter(_fontList);
	while(fontIter.hasNext())
	{
		const FontFile &fontFile = fontIter.next();
		if(fontFile.getFileName(true) == fontFileName)
		{
			return fontFile.getFontName();
		}
	}

	assert(QFile::exists(fontFileName));
	int id = QFontDatabase::addApplicationFont(fontFileName);
	if(id != -1)
	{
		QStringList families = QFontDatabase::applicationFontFamilies(id);
		if(!families.isEmpty())
			return families.join(",");
	}


	QFileInfo fileInfo(fontFileName);
	return fileInfo.baseName();
}

QString FontType::createFntFile(const QString &fontName, const QString &fontFileName)
{
	
		return "";

	//return fntDirStr + QString(QDir::separator()) + fntFileName;
}

void FontType::modifyFntFile( const QString &fontName, const QString &fontFileName, const QList<int> &heights )
{
	//const static QString xmlHead = 
	//	"\n<?xml version=\"1.0\"?>\n"
	//	"<fnt>\n"
	//	"	<property key=\"source\" value=\"%1\"/>\n"
	//	"	<property key=\"antialiased\" value=\"true\"/>\n"
	//	"	<property key=\"outline\" value=\"true\"/>\n";
	//const static QString xmlCenter = 
	//	"	<property key=\"support_size\" value=\"%1\"/>\n";
	//const static QString xmlTail = 
	//	"	<coderange low=\"33\" high=\"126\"/>\n"
	//	"	<coderange low=\"8216\" high=\"8217\"/>\n"
	//	"	<coderange low=\"8220\" high=\"8221\"/>\n"
	//	"	<coderange low=\"8211\" high=\"8212\"/>\n"
	//	"	<coderange low=\"8230\" high=\"8230\"/>\n"
	//	"	<coderange low=\"12289\" high=\"12290\"/>\n" 
	//	"	<coderange low=\"12296\" high=\"12303\"/>\n" 
	//	"	<coderange low=\"12308\" high=\"12309\"/>\n" 
	//	"	<coderange low=\"12304\" high=\"12305\"/>\n" 
	//	"	<coderange low=\"65281\" high=\"65374\"/>\n" 
	//	"	<coderange low=\"19968\" high=\"40869\"/>\n" 
	//	"</fnt>\n";

	//if(!QFile::exists(fontFileName))
	//	return;

	//assert(heights.size() != 0);
	//QString xmlStr = xmlHead.arg(fontName);
	//QListIterator<int> iter(heights);
	//while(iter.hasNext())
	//	xmlStr += xmlCenter.arg(iter.next());
	//xmlStr += xmlTail;

	//QFile fontFile(fontFileName);
	//if(!fontFile.open(QIODevice::ReadOnly))
	//	return;
	//QByteArray fileBytes = fontFile.readAll();
	//fontFile.close();
	//QFile::remove(fontFileName);

	//QByteArray sizeBytes = fileBytes.left(sizeof(u32));
	//u32 size = *((u32*)sizeBytes.data());
	//QByteArray fontBytes = fileBytes.right(fileBytes.size() - (sizeof(u32) + size));

	//QFile newFontFile(fontFileName);
	//if(!newFontFile.open(QIODevice::ReadWrite))
	//	return;
	//size = xmlStr.toLatin1().size();
	//newFontFile.write(QByteArray((const char*)&size, sizeof(int)));
	//newFontFile.write(xmlStr.toLatin1());
	//newFontFile.write(fontBytes);
	//newFontFile.close();
}