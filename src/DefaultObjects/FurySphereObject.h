#ifndef FURYSPHEREOBJECT_H
#define FURYSPHEREOBJECT_H

#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QJsonObject>

//! Класс сферы
class FurySphereObject : public FuryObject
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FurySphereObject(FuryWorld* _world, FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FurySphereObject(FuryWorld* _world, const glm::vec3& _position,
                     FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scale - Масштаб
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FurySphereObject(FuryWorld* _world, const glm::vec3& _position, double _scale,
                     FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Инициализация физического тела
     * \param[in] _type - Тип физического тела
     */
    void initPhysics(reactphysics3d::BodyType _type) override;
};



#endif // FURYSPHEREOBJECT_H
