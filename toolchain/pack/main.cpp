#include <QtCore/QCoreApplication>
#include "Console.h"
#include <QtPlugin>
int main(int argc, char *argv[])
{
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
	QCoreApplication a(argc, argv);
	a.setApplicationName("BPKPacker");

	QSharedPointer<Console> con(new Console(&a));
	QObject::connect(con.data(), SIGNAL(finished()), &a, SLOT(quit()));
	QTimer::singleShot(0, con.data(), SLOT(run()));

	return a.exec();
}
