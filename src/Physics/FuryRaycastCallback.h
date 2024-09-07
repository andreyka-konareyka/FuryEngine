#ifndef FURYRAYCASTCALLBACK_H
#define FURYRAYCASTCALLBACK_H

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
    reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& _info) override;

    //! Последнее расстояние до столкновения. [0; 1]. 1 - максимальная длина луча
    float m_lastHitFraction = 1;
    //! Вектор нормали при последнем пересечении луча с поверхностью объекта
    rp3d::Vector3 m_lastNormal = rp3d::Vector3(0, 1, 0);

    //! Нужный номер триггера
    int m_needTriggerNumber = -1;
    //! Нужный затриггерен
    bool m_needTriggered = false;
};


#endif // FURYRAYCASTCALLBACK_H
