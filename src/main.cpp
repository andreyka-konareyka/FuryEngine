#include "FuryMainWindow.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "rus");

    QApplication a(argc, argv);
    FuryMainWindow w;
    w.show();
    return a.exec();
}
