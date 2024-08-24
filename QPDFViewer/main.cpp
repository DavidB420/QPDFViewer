#include <QtWidgets/QApplication>
#include "Viewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Viewer vwr;
    vwr.show();
    return a.exec();
}
