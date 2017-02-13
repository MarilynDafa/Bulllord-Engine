#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include <QString>
#include <map>
#include <vector>

class ResourceMgr
{
public:
	static ResourceMgr *getInstance();

	void clear();
	void init();

	void addFont(const QString &fontName);
	void addSkin(const QString &skinName);

	QString getFont(unsigned int id);
	QString getSkin(unsigned int id);

	void addItfFile(const QString &fileName);
	QString getFile(int index);
	const std::vector<QString> &getItfFiles() const { return _itfFile; }
private:
	ResourceMgr();
	~ResourceMgr();

private:
	std::map<unsigned int, QString> _skinMap;
	std::map<unsigned int, QString> _fontMap;
	std::vector<QString> _itfFile;
};

#endif //RESMGR_H