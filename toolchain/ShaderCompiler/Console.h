#pragma once
#include <QObject>
#include <QDebug>
#include <QtCore>
#include <vector>
#include <iostream>
#include <set>
enum eShaderType {
	ST_GL = 0,
	ST_DX,
	ST_VK,
	ST_MTL,
	ST_NUM
};
class Console : public QObject
{
	Q_OBJECT
public:
	Console(QObject *parent = 0) : QObject(parent) {}
public slots :
	void run();
signals:
	void finished();
private:
	void printHelp();
	bool processCmd(const char* cmd);
	void error(const std::string& context_);
	void print_info(const std::string& context_);
	bool _exec_command(const std::vector<std::string>& args_);
private:
	bool compile(const std::string& name);
	void combine();
	bool convertMSL();
	bool convertHLSL();
	void cleanup();
private:
	std::string vkname;
	std::string vert[ST_NUM];
	std::string frag[ST_NUM];
	std::string geom[ST_NUM];
	std::string tese[ST_NUM];
	std::string tesc[ST_NUM];
	std::string comp[ST_NUM];
	std::vector<uint32_t> vertvb;
	std::vector<uint32_t> fragvb;
	std::vector<uint32_t> geomvb;
	std::vector<uint32_t> tesevb;
	std::vector<uint32_t> tescvb;
	std::vector<uint32_t> compvb;
};