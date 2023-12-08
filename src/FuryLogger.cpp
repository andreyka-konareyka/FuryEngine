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

    return qDebug() << dateTime + " " + pid
                    << "[" + QString(_func) + "]";
}

FuryLogger::FuryLogger()
{

}
