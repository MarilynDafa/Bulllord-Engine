#include "Nirvana.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Nirvana w;
    w.show();
    return a.exec();
}
