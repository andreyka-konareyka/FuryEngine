#ifndef WORLD_H
#define WORLD_H

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QVector>
#include <QObject>

class Camera;
class FuryMesh;
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

    //! Отрисовка карты теней
    void drawDepthMap();

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

    //! Создание материалов
    void createMaterials();
    //! Создание текстур
    void createTextures();

    //! Создание окружения для pbr
    void createPbrCubemap(const QString& _cubemapHdrName);

    //! Создать карту теней
    void createDepthMap();

    //! Сохранение мира в JSON
    void save();
    //! Загрузка мира из JSON
    void load();

signals:
    /*!
     * \brief Сигнал добавления объекта
     * \param[in] _object - Объект
     */
    void addObjectSignal(FuryObject* _object);

    /*!
     * \brief Сигнал изменения родителя у объекта
     * \param[in] _object - Объект
     */
    void parentChangedSignal(FuryObject* _object);

private slots:
    //! Слот изменения родителя
    void parentChangedSlot();

private:
    /*!
     * \brief Отрисовка компоненты
     * \param[in] _component - Пара <Объект, Меш> для отрисовки
     * \param[in] _projection - Матрица проекции
     * \param[in] _view - Матрица вида
     * \param[in] _lightSpaceMatrix - Матрица пространства теней
     */
    void drawComponent(const QPair<FuryObject*, FuryMesh*>& _component,
                       const glm::mat4& _projection,
                       const glm::mat4& _view,
                       const glm::mat4& _lightSpaceMatrix);

    /*!
     * \brief Заполнение компонент для отрисовки
     * \param[out] _solidComponents - Непрозрачный компоненты
     * \param[out] _transparentComponents - Прозрачные компоненты
     */
    void fillDrawComponents(QVector<QPair<FuryObject*, FuryMesh*>>& _solidComponents,
                            QVector<QPair<FuryObject*, FuryMesh*>>& _transparentComponents);

    /*!
     * \brief Отрисовка выделенного объекта в редакторе
     * \param[in] _projection - Матрица проекции
     * \param[in] _view - Матрица вида
     */
    void drawSelectedInEditor(const glm::mat4& _projection,
                              const glm::mat4& _view);

private:
    //! Инициализация соединений сигналов и слотов
    void initConnections();

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

    glm::vec3 m_dirLightPosition;
    GLuint m_envCubemap;
    GLuint m_irradianceMap;
    GLuint m_prefilterMap;
    GLuint m_brdfLUTTexture;
    bool m_shadowMapEnabled;

    GLuint m_depthMapFBO;
    GLuint m_depthMap;
};

#endif // WORLD_H
