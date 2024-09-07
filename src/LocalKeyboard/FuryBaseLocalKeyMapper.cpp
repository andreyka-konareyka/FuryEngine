#include "FuryBaseLocalKeyMapper.h"


FuryBaseLocalKeyMapper::FuryBaseLocalKeyMapper(const QString &_name) :
    m_name(_name)
{

}

const QString &FuryBaseLocalKeyMapper::name() const
{
    return m_name;
}
