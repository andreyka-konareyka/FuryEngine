#include "FuryException.h"

FuryException::FuryException(const QString &_userInfo,
                             const QString &_debugInfo,
                             const QString& _funcInfo) :
    m_userInfo(_userInfo),
    m_debugInfo(_debugInfo),
    m_funcInfo(_funcInfo)
{

}
