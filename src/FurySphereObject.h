#ifndef FURYSPHEREOBJECT_H
#define FURYSPHEREOBJECT_H

#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>

//! Класс сферы
class FurySphereObject : public FuryObject
{
public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     */
    FurySphereObject(FuryWorld* _world);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     */
    FurySphereObject(FuryWorld* _world, const glm::vec3& _position);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scale - Масштаб
     */
    FurySphereObject(FuryWorld* _world, const glm::vec3& _position, double _scale);

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    void tick(double _dt) override;

    /*!
     * \brief Инициализация физического тела
     * \param[in] _type - Тип физического тела
     */
    void Setup_physics(reactphysics3d::BodyType _type);
};



#endif // FURYSPHEREOBJECT_H
