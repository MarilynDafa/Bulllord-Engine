#include <QtCore/QCoreApplication>
#include "Console.h"
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName("ShaderCompiler");

	QSharedPointer<Console> con(new Console(&a));
	QObject::connect(con.data(), SIGNAL(finished()), &a, SLOT(quit()));
	QTimer::singleShot(0, con.data(), SLOT(run()));

	return a.exec();
}
