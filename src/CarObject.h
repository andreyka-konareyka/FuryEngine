#ifndef CAROBJECT_H
#define CAROBJECT_H

#include "FuryObject.h"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

#include <QVector>


//! Автомобиль для обучения ИИ
class CarObject : public FuryObject
{
    Q_OBJECT
    Q_PROPERTY(float springLenght READ springLenght WRITE setSpringLenght)
    Q_PROPERTY(float springK READ springK WRITE setSpringK)
    Q_PROPERTY(glm::vec3 cameraLocalViewPoint READ cameraLocalViewPoint WRITE setCameraLocalViewPoint)
    Q_PROPERTY(glm::vec3 cameraLocalPosition READ cameraLocalPosition WRITE setCameraLocalPosition)

public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир
     */
    CarObject(FuryWorld* _world);
    //! Деструктор
    ~CarObject();

    /*!
     * \brief Тик обновления
     * \param[in] _dt - Провежуток времени от прошлого тика
     */
    void tick(double _dt) override;

    /*!
     * \brief Событие нажатия кнопки клавиатуры
     * \param[in] _keyCode - Код кнопки
     */
    void keyPressEvent(int _keyCode) override;

    /*!
     * \brief Собитие отпускания кнопки клавиатуры
     * \param[in] _keyCode - Код кнопки
     */
    void keyReleaseEvent(int _keyCode) override;

    //! resetKeyInput Сброс кнопок управления
    void resetKeyInput();

    /*!
     * \brief Установка действия от Бота
     * \param[in] _action - Действие
     */
    void setBotAction(int _action);

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
     * \brief Получить вектор угловой скорости
     * \return Возвращает вектор угловой скорости
     */
    glm::vec3 getAngularSpeed();

    /*!
     * \brief Получить размер вознаграждения
     * \return Возвращает размер вознаграждения
     */
    float getReward();


    /*!
     * \brief Получение номера прошлого проеханного триггера на трассе
     * \return Возвращает номер прошлого проеханного триггера на трассе
     */
    int getLastTriggerNumber();

    //! Произошёл контакт. Вызывается из FuryEventListener при контакте.
    void onContact();

    /*!
     * \brief Произошло пересечение с триггером
     * \param[in] _number - Номер триггера
     */
    void onTrigger(int _number);

    /*!
     * \brief Вычисление локального направления к следующему триггеру
     * \param[in] _trigger - Позиция следующего триггера
     * \return Возвращает локальное направление к следующему триггеру
     */
    glm::vec3 calcNextTriggerVector(const glm::vec3& _trigger);

    //! Возрождение
    void reset() override;

    /*!
     * \brief Проверка временного счётчика
     * \return Возвращает признак того, не закончилось ли у нас ещё время
     */
    bool checkTimeCounter();

    /*!
     * \brief Проверка счётчика косаний триггеров при движении назад по трассе
     * \return Возвращает признак того, что ещё есть попытки и можно продолжать
     */
    bool checkBackTriggerCounter();

    /*!
     * \brief Проверка наличия контактов со стенами
     * \return Возвращает, был ли контакт со стенами
     */
    bool checkHasContact();

    /*!
     * \brief Инициализация физики
     * \param[in] _type - Тип тела: статическое, динамическое, кинематическое
     */
    void initPhysics(reactphysics3d::BodyType _type) override;

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
    void setLocalCameraPosition(float _x, float _y);

    /*!
     * \brief Установка длины пружин
     * \param[in] _lenght - Длина пружин
     */
    inline void setSpringLenght(float _lenght)
    { m_springLenght = _lenght;}

    /*!
     * \brief Получение длины пружин
     * \return Возвращает длину пружин
     */
    inline float springLenght() const
    { return m_springLenght; }

    /*!
     * \brief Установка жёсткости пружин
     * \param[in] _k - Жёсткость пружин
     */
    inline void setSpringK(float _k)
    { m_springK = _k; }

    /*!
     * \brief Получение жёсткости пружин
     * \return Возвращает жёсткость пружин
     */
    inline float springK() const
    { return m_springK; }

    inline const glm::vec3& cameraLocalViewPoint() const
    { return m_cameraLocalViewPoint; }

    inline void setCameraLocalViewPoint(const glm::vec3& _point)
    { m_cameraLocalViewPoint = _point; }

    inline const glm::vec3& cameraLocalPosition() const
    { return m_cameraLocalPosition; }

    inline void setCameraLocalPosition(const glm::vec3& _pos)
    { m_cameraLocalPosition = _pos; }


    /*!
     * \brief Получение шейдера по умолчанию
     * \return Возвращает шейдер по умолчанию
     */
    static Shader* defaultShader();

private:
    //! Список объектов-колёс
    QVector<FuryObject*> m_objectWheels;
    //! Список объектов - сфер для отображения raycast лучей
    QVector<FuryObject*> m_objectsDebugRays;
    //! Длина пружин. Референс
    float m_springLenght;
    //! Коэффициент жёсткости пружин
    float m_springK;
    //! Список последних длин пружин в сжатом состоянии
    QVector<float> m_lastSuspentionLenght;

    //! Точку, куда смотрит камера (в локальных координатах)
    glm::vec3 m_cameraLocalViewPoint;
    //! Позиция камеры (в локальных координатах)
    glm::vec3 m_cameraLocalPosition;

    //! Ввод. Управление. [-1;1]. Движение вперёд.
    float m_forward;
    //! Ввод. Управление. [-1;1]. Поворот направо.
    float m_right;

    //! Номер последнего проеханного триггера на трассе
    int m_lastTriggerNumber;
    //! Награда за текущий тик
    float m_reward;
    //! Счётчик времени
    float m_timeCounter;
    //! Счётчик триггеров, проеханных по трассе в обратном направлении
    int m_backTriggerCounter;
    //! Признак наличия контактов со стенами
    bool m_hasContact;
};



#endif // CAROBJECT_H
