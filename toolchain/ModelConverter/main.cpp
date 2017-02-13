#include "ModelConverter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModelConverter w;
    w.show();
    return a.exec();
}
