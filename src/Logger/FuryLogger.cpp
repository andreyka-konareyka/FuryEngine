#include "FuryLogger.h"

#include <thread>
#include <iomanip>

#include <QDateTime>
#include <QThread>
#include <QString>

FuryLogger* FuryLogger::instance()
{
    static FuryLogger* logger = new FuryLogger;
    return logger;
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
    qDebug();
}

FuryLogger::FuryLogger()
{

}
