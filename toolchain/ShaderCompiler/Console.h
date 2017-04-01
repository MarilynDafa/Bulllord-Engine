#pragma once
#include <QObject>
#include <QDebug>
#include <QtCore>
#include <vector>
#include <iostream>
#include <set>

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
};