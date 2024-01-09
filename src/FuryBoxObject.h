#ifndef FURYBOXOBJECT_H
#define FURYBOXOBJECT_H


#include "FuryObject.h"

#include "Camera.h"

#include <reactphysics3d/reactphysics3d.h>

//! Класс коробки
class FuryBoxObject : public FuryObject {
public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     */
    FuryBoxObject(FuryWorld* _world);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _scale - Масштаб
     */
    FuryBoxObject(FuryWorld* _world, double _scale);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _scaleX - Масштаб по X
     * \param[in] _scaleY - Масштаб по Y
     * \param[in] _scaleZ - Масштаб по Z
     */
    FuryBoxObject(FuryWorld* _world, double _scaleX, double _scaleY, double _scaleZ);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scale - Масштаб
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position, double _scale);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scaleX - Масштаб по X
     * \param[in] _scaleY - Масштаб по Y
     * \param[in] _scaleZ - Масштаб по Z
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position, double _scaleX, double _scaleY, double _scaleZ);

    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     * \param[in] _position - Позиция
     * \param[in] _scales - Масштабы
     * \param[in] _rotate - Вращение объекта
     */
    FuryBoxObject(FuryWorld* _world, const glm::vec3& _position, const glm::vec3& _scales, const glm::vec3& _rotate);

    //! Деструктор
    ~FuryBoxObject();

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
#endif // FURYBOXOBJECT_H
