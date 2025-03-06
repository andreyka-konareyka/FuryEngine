#ifndef FURYBOXOBJECT_H
#define FURYBOXOBJECT_H


#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QJsonObject>

//! Класс коробки
class FuryBoxObject : public FuryObject
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _scale - Масштаб
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, double _scale,
                  FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _scaleX - Масштаб по X
     * \param[in] _scaleY - Масштаб по Y
     * \param[in] _scaleZ - Масштаб по Z
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, double _scaleX, double _scaleY, double _scaleZ,
                  FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position,
                  FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scale - Масштаб
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position, double _scale,
                  FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scaleX - Масштаб по X
     * \param[in] _scaleY - Масштаб по Y
     * \param[in] _scaleZ - Масштаб по Z
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position, double _scaleX, double _scaleY, double _scaleZ,
                  FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scales - Масштабы
     * \param[in] _rotate - Вращение объекта
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position, const glm::vec3& _scales, const glm::vec3& _rotate,
                  FuryObject* _parent = nullptr, bool _withoutJoint = false);

    //! Деструктор
    ~FuryBoxObject();

    /*!
     * \brief Инициализация физического тела
     * \param[in] _type - Тип физического тела
     */
    void initPhysics(reactphysics3d::BodyType _type) override;
};
#endif // FURYBOXOBJECT_H
