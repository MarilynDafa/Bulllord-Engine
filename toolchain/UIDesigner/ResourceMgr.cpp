#include "ResourceMgr.h"
#include "UIMgr.h"
#include "UiWidget.h"
#include "PropertyDataType.h"
#include "Config.h"

ResourceMgr * ResourceMgr::getInstance()
{
	static ResourceMgr instance;
	return &instance;
}

ResourceMgr::ResourceMgr()
{

}

ResourceMgr::~ResourceMgr()
{

}

void ResourceMgr::addFont( const QString &fontName )
{
	c_font *font = c_ui_manager::get_singleton_ptr()->add_font(UiWidget::fromQString(fontName));
	if(font != NULL)
	{
		_fontMap[font->get_id()] = fontName;
	}
}

void ResourceMgr::addSkin( const QString &skinName )
{
	c_skin *skin = c_ui_manager::get_singleton_ptr()->add_skin(UiWidget::fromQString(skinName));
	if(skin != NULL)
	{
		_skinMap[skin->get_id()] = skinName;
	}
}

QString ResourceMgr::getFont( unsigned int id )
{
	std::map<unsigned int, QString>::iterator iter = _fontMap.find(id);
	if(iter != _fontMap.end())
		return iter->second;

	return "";
}

QString ResourceMgr::getSkin( unsigned int id )
{
	std::map<unsigned int, QString>::iterator iter = _skinMap.find(id);
	if(iter != _skinMap.end())
		return iter->second;

	return "";
}

void ResourceMgr::clear()
{
	_skinMap.clear();
	_fontMap.clear();
}

void ResourceMgr::init()
{
	//load resource from workspace
	FontType::initFontList();

	//load skn
	QDir currentDir = QDir::current();
	QString sknDir = GlobalConfig::getSingletonPtr()->value("skin_dir", "pixmap").toString();
	currentDir.cd(sknDir);
	QFileInfoList fileInfos = currentDir.entryInfoList(QStringList()<<"*.bmg", QDir::Files);
	QListIterator<QFileInfo> iter(fileInfos);
	while(iter.hasNext())
	{
		QFileInfo fileInfo = iter.next();
		addSkin(fileInfo.absoluteFilePath());
	}

	//load itf
	currentDir = QDir::current();
	QString itfDir = GlobalConfig::getSingletonPtr()->value("bui_dir", "bui").toString();
	currentDir.cd(itfDir);
	QFileInfoList itfFileInfos = currentDir.entryInfoList(QStringList()<<"*.bui", QDir::Files);
	QListIterator<QFileInfo> itfIter(itfFileInfos);
	while(itfIter.hasNext())
	{
		QFileInfo fileInfo = itfIter.next();
		addItfFile(fileInfo.absoluteFilePath());
	}
}

void ResourceMgr::addItfFile( const QString &fileName )
{
	if(fileName.isEmpty())
		return;

	std::vector<QString>::iterator iter = std::find(_itfFile.begin(), _itfFile.end(), fileName);
	if(iter == _itfFile.end())
	{
		_itfFile.push_back(fileName);
	}
}

QString ResourceMgr::getFile( int index )
{
	if(index < 0 || index >= _itfFile.size())
		return "";

	return _itfFile[index];
}
