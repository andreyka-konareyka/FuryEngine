#include "FuryMainWindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <iostream>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "rus");
    std::cout << "test";

    qApp->setStyle("Fusion");
    qDebug() << QStyleFactory::keys();
    QApplication a(argc, argv);
    FuryMainWindow w;
    w.show();
    return a.exec();
}
