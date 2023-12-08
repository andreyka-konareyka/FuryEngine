#ifndef FURYSPHEREOBJECT_H
#define FURYSPHEREOBJECT_H

#include "FuryObject.h"

//! Класс сферы
class FurySphereObject : public FuryObject
{
public:
    //! Конструктор
    FurySphereObject();

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     */
    FurySphereObject(const glm::vec3& _position);

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     * \param[in] _scale - Масштаб
     */
    FurySphereObject(const glm::vec3& _position, double _scale);

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    void tick(double _dt) override;

    /*!
     * \brief Инициализация физического тела
     * \param[in] _physCommon - Объект физики
     * \param[in] _physWorld - Физический мир
     * \param[in] _type - Тип физического тела
     */
    void Setup_physics(reactphysics3d::PhysicsCommon& _physCommon,
                       reactphysics3d::PhysicsWorld* _physWorld,
                       reactphysics3d::BodyType _type);
};



#endif // FURYSPHEREOBJECT_H
