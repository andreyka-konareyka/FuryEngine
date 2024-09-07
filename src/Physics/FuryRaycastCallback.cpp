#include "FuryRaycastCallback.h"

#include "FuryObject.h"

#include <QList>

reactphysics3d::decimal FuryRaycastCallback::notifyRaycastHit(const reactphysics3d::RaycastInfo &_info)
{
    // Return a fraction of 1.0 to gather all hits
    FuryObject* object = static_cast<FuryObject*>(_info.body->getUserData());

    if (object->objectName().startsWith("Trigger"))
    {
        int triggerNumber = object->objectName().split(' ').last().toInt();

        if (triggerNumber == m_needTriggerNumber)
        {
            m_needTriggered = true;
        }

        return -1;
    }

    m_lastHitFraction = _info.hitFraction;
    m_lastNormal = _info.worldNormal;
    return reactphysics3d::decimal(m_lastHitFraction);
}
