#include "FXEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FXEditor w;
    w.show();
    return a.exec();
}
