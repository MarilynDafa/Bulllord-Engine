#pragma once

#include <QDialog>
#include <stdlib.h>
#include <qfileinfo.h>
#include <qprogressdialog.h>
#include "BMGHeader.h"
#include <stdio.h>
#include <string>
#include <vector>

class Batch :public QObject
{
	Q_OBJECT
public:
	Batch(QObject* obj = NULL);
	~Batch();
	void setLogDir(const QString& logdir, const QString& root, CompressMethord cm, QWidget* wid);
	void setDestForder(const QString& dest);
	bool isImage(const QString& file);
	QString getPath(const QString& parh);
	QString changeExt(const QString& parh);
protected:

	void writelog(const QString& file);
private slots:

	void Event();
	void cancel();
	bool Event2();
private:
	QString _logdir;
	QString _dest;
	QString _root;
	FILE* _logfp;
	std::vector<std::string> fileinfolist;
	int processed;
	CompressMethord _cm;
	QProgressDialog* progress;
	QTimer* _t;
};
