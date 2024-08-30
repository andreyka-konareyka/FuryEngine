#ifndef FURYSPHEREOBJECT_H
#define FURYSPHEREOBJECT_H

#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QJsonObject>

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
     * \brief Инициализация физического тела
     * \param[in] _type - Тип физического тела
     */
    void Setup_physics(reactphysics3d::BodyType _type);

    /*!
     * \brief Получение шейдера по умолчанию
     * \return Возвращает шейдер по умолчанию
     */
    static Shader* defaultShader();

    /*!
     * \brief Перевод в JSON объект
     * \return Возвращает JSON объект
     */
    QJsonObject toJson();

    /*!
     * \brief Перевод из JSON объекта
     * \param[in] _object - JSON объект
     * \param[in] _world - Мир, к которому принадлежит объект
     * \return Возвращает сферу
     */
    static FurySphereObject* fromJson(const QJsonObject& _object,
                                      FuryWorld* _world);
};



#endif // FURYSPHEREOBJECT_H
