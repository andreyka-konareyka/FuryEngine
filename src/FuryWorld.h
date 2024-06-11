#ifndef WORLD_H
#define WORLD_H

#include <QVector>

class Camera;
class FuryObject;

namespace reactphysics3d
{
    class PhysicsCommon;
    class PhysicsWorld;
}


//! Класс игрового мира
class FuryWorld {
public:
    /*!
     * \brief Конструктор
     * \param[in] _physicsCommon - Главный объект физики
     */
    FuryWorld(reactphysics3d::PhysicsCommon* _physicsCommon);
    //! Деструктор
    ~FuryWorld();

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    void tick(double _dt);

    /*!
     * \brief Добавление непрозрачный объекта
     * \param[in] _object - Непрозрачный объект
     */
    void addObject(FuryObject* _object);

    /*!
     * \brief Установка камеры
     * \param[in] _camera - Камера
     */
    void setCamera(Camera* _camera);

    //! Удаление текущей камеры
    void deleteCurrentCamera();

    /*!
     * \brief Получение всех непрозрачных объектов
     * \return Возвращает все непрозрачные объекты
     */
    const QVector<FuryObject*>& getObjects();

    /*!
     * \brief Получить все объекты
     * \return Возвращает все объекты
     */
    const QVector<FuryObject*>& getAllObjects();

    /*!
     * \brief Получение текущей камеры
     * \return Возврашает текущую камеру
     */
    inline Camera* camera() const
    { return m_currentCamera; }

    /*!
     * \brief Получение главного объекта физики
     * \return Возвращает главный объект физики
     */
    inline reactphysics3d::PhysicsCommon* physicsCommon() const
    { return m_physicsCommon; }

    /*!
     * \brief Получение физического мира
     * \return Возвращает физический мир
     */
    inline reactphysics3d::PhysicsWorld* physicsWorld() const
    { return m_physicsWorld; }

private:
    //! Главный объект физики, к которому принадлежит мир
    reactphysics3d::PhysicsCommon* m_physicsCommon;
    //! Физический мир
    reactphysics3d::PhysicsWorld* m_physicsWorld;

    //! Текущая камера
    Camera* m_currentCamera;
    //! Список непрозрачный объектов
    QVector<FuryObject*> m_objects;
};

#endif // WORLD_H
