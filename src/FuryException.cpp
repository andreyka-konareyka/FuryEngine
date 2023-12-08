#include "FuryException.h"

FuryException::FuryException(const QString &_userInfo,
                             const QString &_debugInfo) :
    m_userInfo(_userInfo),
    m_debugInfo(_debugInfo)
{

}
