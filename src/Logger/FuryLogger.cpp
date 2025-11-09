#include "FuryLogger.h"

#include <QThread>
#include <QString>
#include <QDateTime>


FuryLogger* FuryLogger::s_instance = nullptr;

FuryLogger* FuryLogger::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new FuryLogger;
    }

    return s_instance;
}

void FuryLogger::deleteInstance()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

QDebug FuryLogger::log(const QString &_func)
{
    QString dateTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    QString pid = "[" + QString::number((long long)(QThread::currentThreadId())).rightJustified(5, ' ') + "]";

    return qDebug() << qPrintable(dateTime + " " + pid)
                    << qPrintable("[" + QString(_func) + "]");
}

void FuryLogger::logException(const FuryException &_exception)
{
    qDebug();
    qDebug() << "*** Exception: ***";
    qDebug() << _exception.userInfo();
    qDebug() << _exception.debugInfo();
    qDebug() << ru("*** Метод: ***");
    qDebug() << _exception.funcInfo();
    qDebug();
}

FuryLogger::FuryLogger()
{
    qDebug() << ru("Создание логгера");
}

FuryLogger::~FuryLogger()
{
    qDebug() << ru("Удаление логгера");
}
