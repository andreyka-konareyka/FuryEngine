#ifndef FURYLOGGER_H
#define FURYLOGGER_H


#include <string>

#include <QDateTime>
#include <QThread>
#include <QDebug>

class FuryLogger
{
public:
    static FuryLogger* instance();
    QDebug log(const QString& _func);

private:
    FuryLogger();
};

#define Debug(_debugText) FuryLogger::instance()->log(__FUNCTION__) << _debugText
#define ru QString::fromUtf8

#endif // FURYLOGGER_H
