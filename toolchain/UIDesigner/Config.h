#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore/QMap>
#include <QtCore/QVariant>

class GlobalConfig
{
public:
	static GlobalConfig *getSingletonPtr();
	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &value) const;
private:
	GlobalConfig();
	~GlobalConfig();
private:
	QMap<QString, QVariant> _valueMap;
};

inline void GlobalConfig::setValue( const QString &key, const QVariant &value )
{
	_valueMap[key] = value;
}

inline QVariant GlobalConfig::value( const QString &key, const QVariant &value ) const
{
	return _valueMap.value(key,value);
}


#endif //CONFIG_H