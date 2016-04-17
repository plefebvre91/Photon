#include "photon.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Photon w;
    w.show();

    return a.exec();
}
