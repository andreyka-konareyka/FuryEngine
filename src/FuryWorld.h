#ifndef WORLD_H
#define WORLD_H

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

#include <QVector>
#include <QObject>

class Camera;
class FuryObject;

namespace reactphysics3d
{
    class PhysicsCommon;
    class PhysicsWorld;
}


//! Класс игрового мира
class FuryWorld : public QObject
{
    Q_OBJECT

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

    //! Сброс мира
    void resetWorld();

    //! Поставить мир на паузу
    inline void pauseWorld()
    { m_started = false; }

    //! Возобновить мир
    inline void resumeWorld()
    { m_started = true; }

    /*!
     * \brief Отрисовка мира
     * \param[in] _width - Ширина экрана
     * \param[in] _height - Высота экрана
     */
    void draw(int _width, int _height);

    /*!
     * \brief Добавление корневого объекта мира
     * \param[in] _object - Корневой объект
     */
    void addRootObject(FuryObject* _object);

    /*!
     * \brief Добавление объекта мира
     * \param[in] _object - Объект
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
     * \brief Получение всех корневых объектов
     * \return Возвращает все корневые объекты
     */
    const QVector<FuryObject*>& getRootObjects();

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

    //! Загрузка гоночной трассы из json-файла
    void loadRaceMap();
    //! Создание материалов
    void createMaterials();
    //! Создание текстур
    void createTextures();

    //! Создание окружения для pbr
    void createPbrCubemap(const QString& _cubemapHdrName);

signals:
    void addObjectSignal(FuryObject* _object);
    void parentChangedSignal(FuryObject* _object);

private slots:
    void parentChangedSlot();

private:
    //! Главный объект физики, к которому принадлежит мир
    reactphysics3d::PhysicsCommon* m_physicsCommon;
    //! Физический мир
    reactphysics3d::PhysicsWorld* m_physicsWorld;

    //! Текущая камера
    Camera* m_currentCamera;
    //! Список корневых объектов
    QVector<FuryObject*> m_objects;
    //! Список всех объектов
    QVector<FuryObject*> m_allObjects;

    //! Запущена ли симуляция
    bool m_started;

    GLuint m_envCubemap;
    GLuint m_irradianceMap;
    GLuint m_prefilterMap;
    GLuint m_brdfLUTTexture;
    bool m_shadowMapEnabled;
};

#endif // WORLD_H
