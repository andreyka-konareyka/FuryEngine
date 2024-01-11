#ifndef FURYRAYCASTCALLBACK_H
#define FURYRAYCASTCALLBACK_H

#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>


class FuryRaycastCallback : public reactphysics3d::RaycastCallback
{
public:
    reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& _info) override
    {

        // Display the world hit point coordinates
        /*std::cout << "Hit point : " <<
            _info.worldPoint.x << "; " <<
            _info.worldPoint.y << "; " <<
            _info.worldPoint.z << "; " <<
            std::endl;*/

        // Return a fraction of 1.0 to gather all hits
        FuryObject* object = static_cast<FuryObject*>(_info.body->getUserData());

        if (object->name().startsWith("Trigger"))
        {
            return -1;
        }

        m_lastHitFraction = _info.hitFraction;
        m_lastNormal = _info.worldNormal;
        return reactphysics3d::decimal(m_lastHitFraction);
    }

    float m_lastHitFraction = 1;
    rp3d::Vector3 m_lastNormal = rp3d::Vector3(0, 1, 0);
};



#endif // FURYRAYCASTCALLBACK_H
