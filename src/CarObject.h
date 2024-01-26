#ifndef CAROBJECT_H
#define CAROBJECT_H



#include "FuryObject.h"
#include "FuryBoxObject.h"
#include "FurySphereObject.h"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

#include <QVector>


class CarObject : public FuryObject
{
public:
    CarObject(FuryWorld* _world, const glm::vec3& _position, Shader* _shader);
    ~CarObject();

    void tick(double dt) override;

    void keyPressEvent(int _keyCode) override;
    void keyReleaseEvent(int _keyCode) override;

    //! resetKeyInput Сброс кнопок управления
    void resetKeyInput();

    /*!
     * \brief Установка действия от Бота
     * \param[in] _action - Действие
     */
    void setBotAction(int _action);
    int getUserAction();

    /*!
     * \brief Получить значения лучей
     * \return Возвращает значения лучей
     */
    QVector<float> getRays();

    /*!
     * \brief Получить вектор скорости
     * \return Возвращает вектор скорости
     */
    glm::vec3 getSpeed();

    /*!
     * \brief Получить размер вознаграждения
     * \return Возвращает размер вознаграждения
     */
    float getReward();

    int getLastTriggerNumber();

    void onContact();
    void onTrigger(int _number);
    void resetPosition(const glm::vec3& _position);

    glm::vec3 calcNextTriggerVector(const glm::vec3& _trigger);

    void respawn();
    bool checkTimeCounter();
    bool checkBackTriggerCounter();
    bool checkHasContact();

    void Setup_physics(reactphysics3d::BodyType type);

    inline const QVector<FuryObject*>& objectsForDraw() const
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

    /*!
     * \brief Установка локальной позиции камеры
     * \param[in] _x - Координата x
     * \param[in] _y - Координата y
     */
    void setLocalCameraPosition(int _x, int _y);

    /*!
     * \brief Установка длины пружин
     * \param[in] _lenght - Длина пружин
     */
    void setSpringLenght(float _lenght);

    /*!
     * \brief Установка жёсткости пружин
     * \param[in] _k - Жёсткость пружин
     */
    void setSpringK(float _k);

private:
    FuryBoxObject* m_objectBody;
    QVector<FurySphereObject*> m_objectWheels;
    QVector<FurySphereObject*> m_objectsDebugRays;
    float m_springLenght;
    float m_springK;
    QVector<float> m_lastSuspentionLenght;

    //! Точку, куда смотрит камера (в локальных координатах)
    glm::vec3 m_cameraLocalViewPoint;
    //! Позиция камеры (в локальных координатах)
    glm::vec3 m_cameraLocalPosition;

    float m_forward;
    float m_right;

    int m_lastTriggerNumber;
    float m_reward;
    glm::vec3 m_startPosition;
    float m_timeCounter;
    int m_backTriggerCounter;
    bool m_hasContact;


    /// Дополнительные указатели. НЕ УДАЛЯТЬ!
    QVector<FuryObject*> m_objectsForDraw;
};



#endif // CAROBJECT_H
