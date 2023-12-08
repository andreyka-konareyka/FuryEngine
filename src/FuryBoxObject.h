#ifndef FURYBOXOBJECT_H
#define FURYBOXOBJECT_H


#include "FuryObject.h"

#include "Camera.h"

#include <reactphysics3d/reactphysics3d.h>

//! Класс коробки
class FuryBoxObject : public FuryObject {
public:
    //! Конструктор
    FuryBoxObject();

    /*!
     * \brief Конструктор
     * \param[in] _scale - Масштаб
     */
    FuryBoxObject(double _scale);

    /*!
     * \brief Конструктор
     * \param[in] _scaleX - Масштаб по X
     * \param[in] _scaleY - Масштаб по Y
     * \param[in] _scaleZ - Масштаб по Z
     */
    FuryBoxObject(double _scaleX, double _scaleY, double _scaleZ);

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     */
    FuryBoxObject(const glm::vec3& _position);

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     * \param[in] _scale - Масштаб
     */
    FuryBoxObject(const glm::vec3& _position, double _scale);

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     * \param[in] _scaleX - Масштаб по X
     * \param[in] _scaleY - Масштаб по Y
     * \param[in] _scaleZ - Масштаб по Z
     */
    FuryBoxObject(const glm::vec3& _position, double _scaleX, double _scaleY, double _scaleZ);

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     * \param[in] _scales - Масштабы
     * \param[in] _rotate - Вращение объекта
     */
    FuryBoxObject(const glm::vec3& _position, const glm::vec3& _scales, const glm::vec3& _rotate);

    //! Деструктор
    ~FuryBoxObject();

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    void tick(double _dt) override;

    /*!
     * \brief Отрисовка объекта
     * \param[in] _camera - Камера
     * \param[in] _windowWidth - Ширина окна
     * \param[in] _windowHeight - Высота окна
     * \param[in] _dirlight - Направление света
     * \param[in] _shadowMap - Карта теней
     * \param[in] _depthMap - Карта глубины
     */
    void draw(Camera* _camera, int _windowWidth, int _windowHeight,
              glm::vec3& _dirlight, glm::mat4& _lightSpaceMatrix, GLuint _depthMap) override;

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
#endif // FURYBOXOBJECT_H
