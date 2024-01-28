#ifndef FURYRAYCASTCALLBACK_H
#define FURYRAYCASTCALLBACK_H

#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>


//! Класс отклика на запрос raycast
class FuryRaycastCallback : public reactphysics3d::RaycastCallback
{
public:
    /*!
     * \brief Подсказка для ReactPhysics3d, как обрабатывать текущее пересечение луча с объектом
     * \param[in] _info - Информация о пересечении луча с объектом
     * \return Возвращает подсказку для ReactPhysics3d
     */
    reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& _info) override
    {
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

    //! Последнее расстояние до столкновения. [0; 1]. 1 - максимальная длина луча
    float m_lastHitFraction = 1;
    //! Вектор нормали при последнем пересечении луча с поверхностью объекта
    rp3d::Vector3 m_lastNormal = rp3d::Vector3(0, 1, 0);
};



#endif // FURYRAYCASTCALLBACK_H
