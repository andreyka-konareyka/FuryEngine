#include "Widgets/FuryMainWindow.h"

#include "Logger/FuryLogger.h"

#include <QApplication>
#include <QStyleFactory>

#include <iostream>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "rus");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    FuryMainWindow* mainWindow = new FuryMainWindow;
    mainWindow->show();

    int code = a.exec();

    delete mainWindow;
    FuryLogger::deleteInstance();

    return code;
}
