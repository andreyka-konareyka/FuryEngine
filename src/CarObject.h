#ifndef CAROBJECT_H
#define CAROBJECT_H



#include "FuryObject.h"
#include "FuryBoxObject.h"
#include "FurySphereObject.h"

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

#include <QVector>


//! Автомобиль для обучения ИИ
class CarObject : public FuryObject
{
public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир
     * \param[in] _position - Позиция
     * \param[in] _shader - Шейдер
     */
    CarObject(FuryWorld* _world, const glm::vec3& _position, Shader* _shader);
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
    void respawn();

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
     * \param[in] type - Тип тела: статическое, динамическое, кинематическое
     */
    void Setup_physics(reactphysics3d::BodyType type);

    /*!
     * \brief Получение объектов для отрисовки
     * \return Возвращает объекты для отрисовки
     */
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
    //! Коробка - тело. Нужна для коллизий. Вместо него отображается модель автомобиля.
    FuryBoxObject* m_objectBody;
    //! Список объектов-колёс
    QVector<FurySphereObject*> m_objectWheels;
    //! Список объектов - сфер для отображения raycast лучей
    QVector<FurySphereObject*> m_objectsDebugRays;
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
    //! Начальная позиция автомобиля. Нужна для возрождения.
    glm::vec3 m_startPosition;
    //! Счётчик времени
    float m_timeCounter;
    //! Счётчик триггеров, проеханных по трассе в обратном направлении
    int m_backTriggerCounter;
    //! Признак наличия контактов со стенами
    bool m_hasContact;


    /// Дополнительные указатели. НЕ УДАЛЯТЬ!
    QVector<FuryObject*> m_objectsForDraw;
};



#endif // CAROBJECT_H
