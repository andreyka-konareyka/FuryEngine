#ifndef CAROBJECT_H
#define CAROBJECT_H



#include "Object.h"
#include "BoxObject.h"
#include "SphereObject.h"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

#include <QVector>


class CarObject : public Object
{
public:
    CarObject(const glm::vec3& _position);
    ~CarObject();

    void Tick(double dt) override;

    void keyPressEvent(int _keyCode) override;
    void keyReleaseEvent(int _keyCode) override;

    //! resetKeyInput Сброс кнопок управления
    void resetKeyInput();

    void Setup_physics(reactphysics3d::PhysicsCommon& phys_common, reactphysics3d::PhysicsWorld* phys_world, reactphysics3d::BodyType type);

    inline const QVector<Object*>& objectsForDraw() const
    { return m_objectsForDraw; }

    /*!
    * \brief Получить позицию камеры (в глобальных координатах)
    * \return Возвращает позицию камеры (в глобальных координатах)
    */
    glm::vec3 cameraPosition() const;

    /*!
    * \brief Получить точку, куда смотрит камера (в глобальных координатах)
    * \return Возвращает точку, куда смотрит камера (в глобальных координатах)
    */
    glm::vec3 cameraViewPoint() const;

private:
    BoxObject* m_objectBody;
    BoxObject* m_objectSalon;
    QVector<SphereObject*> m_objectWheels;
    QVector<SphereObject*> m_objectsDebugRays;
    QVector<float> m_lastSuspentionLenght;
    float m_springLenght;

    //! Точку, куда смотрит камера (в локальных координатах)
    glm::vec3 m_cameraLocalViewPoint;
    //! Позиция камеры (в локальных координатах)
    glm::vec3 m_cameraLocalPosition;

    reactphysics3d::PhysicsWorld* m_physicsWorld;

    int m_forward;
    int m_right;


    /// Дополнительные указатели. НЕ УДАЛЯТЬ!
    QVector<Object*> m_objectsForDraw;
};



#endif // CAROBJECT_H
