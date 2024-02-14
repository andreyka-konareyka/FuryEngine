#include "FuryMainWindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <iostream>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "rus");

    QApplication a(argc, argv);
    a.setStyle("Fusion");
    FuryMainWindow w;
    w.show();
    return a.exec();
}
