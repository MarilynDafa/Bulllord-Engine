#include "Config.h"
#include <QtCore/QSettings>
#include <QtCore/QStringList>

GlobalConfig * GlobalConfig::getSingletonPtr()
{
	static GlobalConfig config;
	return &config;
}

GlobalConfig::GlobalConfig()
{
	_valueMap["fnt_dir"] = "font";
	_valueMap["skn_dir"] = "pixmap";
	_valueMap["bui_dir"] = "bui";

	//QSettings settings("config.ini", QSettings::IniFormat);
	//QStringList list = settings.allKeys();
	//QStringListIterator keyIt(settings.allKeys());
	//while(keyIt.hasNext())
	//{
	//	QString key = keyIt.next();
	//	_valueMap[key] = settings.value(key);
	//}
}

GlobalConfig::~GlobalConfig()
{
	//QSettings settings("config.ini", QSettings::IniFormat);
	//QMapIterator<QString, QVariant> it(_valueMap);
	//while(it.hasNext())
	//{
	//	it.next();
	//	settings.setValue(it.key(), it.value());
	//}
}