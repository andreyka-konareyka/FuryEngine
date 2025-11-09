#ifndef FURYRENDERER_H
#define FURYRENDERER_H

// GLEW
#include <GL/glew.h>
// GLM Mathematics
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include <QPair>
#include <QMap>
#include <QObject>


class Shader;
class Camera;
class Particle;
class ParticleSystem;

class FuryMesh;
class FuryWorld;
class FuryObject;
class FuryModelManager;
class FuryWorldManager;
class FuryTextureCache;
class FuryScriptManager;
class FuryShaderManager;
class FuryEventListener;
class FuryTextureManager;
class FuryMaterialManager;
class FuryBaseLocalKeyMapper;

class CarObject;
class FuryLearningScript;

class QKeyEvent;
class QMouseEvent;
class QOpenGLContext;
class QOffscreenSurface;

#include <QOpenGLWidget>


//! Основной класс рендера
class FuryRenderer : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский объект
     * \throw FuryException в случае повторного создания
     */
    explicit FuryRenderer(QObject* _parent = nullptr);
    //! Деструктор
    ~FuryRenderer();

    /*!
     * \brief Отрисовка тестовой сцены
     * \param[in] _materialName - Название материала
     * \param[in] _width - Ширина экранчика
     * \param[in] _height - Высота экранчика
     * \return Возвращает идентификатор текстуры рендер-буфера
     */
    GLuint renderTestScene(const QString& _materialName, int _width, int _height);

    GLuint renderMainScene(int _width, int _height);

    /*!
     * \brief Создание текстур окружения для PBR
     * \param[in] _cubemapHdrPath - Путь к HDR текстуры
     * \param[out] _envCubemap - Выходная текстура окружения
     * \param[out] _irradianceMap - Выходная карта освещенности
     * \param[out] _prefilterMap - Выходная карта предварительной фильтрации
     * \param[out] _brdfLUTTexture - Выходная текстуры brdfUT
     */
    void createPBRTextures(const QString &_cubemapHdrPath, GLuint *_envCubemap,
                           GLuint *_irradianceMap, GLuint *_prefilterMap, GLuint *_brdfLUTTexture);

    /*!
     * \brief Создание карты теней
     * \param[out] _depthMapFBO - Выходной буфер глубины
     * \param[out] _depthMap - Выходная текстура для буфера
     */
    void createDepthMap(GLuint* _depthMapFBO, GLuint* _depthMap);


    /*!
     * \brief Получение экземпляра класса
     * \return Возвращает экземпляр класса
     */
    static FuryRenderer* instance();

    void initializeGL();
    // void paintGL();

    inline FuryWorld* getTestWorld() const
    { return m_testWorld; }

    /*!
     * \brief Установка увеличения камеры
     * \param[in] _value - Увеличение камеры
     */
    void setCameraZoomValue(int _value);


    inline void setShadowNear(float _shadowNear)
    { m_shadowNear = _shadowNear; }

    inline void setShadowPlane(float _shadowPlane)
    { m_shadowPlane = _shadowPlane; }

    inline void setShadowViewSize(float _shadowViewSize)
    { m_shadowViewSize = _shadowViewSize; }

    inline void setShadowCamDistance(float _shadowCamDistance)
    { m_shadowCamDistance = _shadowCamDistance; }

    inline void setLearnSpeed(int _value)
    { m_learnSpeed = _value; }

    void saveLearnModel();

    inline void setNeedDebugRender(bool _need)
    { m_needDebugRender = _need; }

    //! Сохранить историю счёта в играх
    void saveScoreList();

signals:
    void setWindowTitleSignal(const QString& _title);
    void setComputerLoadSignal(int _value);

public:
    void mouseMoveEvent(QMouseEvent* _event);
    void mousePressEvent(QMouseEvent* _event);
    void keyPressEvent(QKeyEvent* _event);
    void keyReleaseEvent(QKeyEvent* _event);

private slots:
    //! Слот, что текстуры готовы для подсоединения
    void texturesReadyToBindSlot();
    //! Слот, что модели готовы для подсоединения
    void modelLoadedSlot();


private:
    float m_deltaTime;
    float m_lastFrame;
    float lastX;
    float lastY;
    QMap<int, bool> m_keys;

    // Mouse enable
    //bool mouse_enable = true;

    void init();
    void InitGL();
    // void render();

    /*!
     * \brief Отрисовка мира
     * \param[in] _world - Мир
     * \param[in] _width - Ширина
     * \param[in] _height - Высота
     */
    void drawWorld(FuryWorld* _world, int _width, int _height);

    /*!
     * \brief Отрисовка карты теней
     * \param[in] _world - Мир
     */
    void drawWorldDepthMap(FuryWorld* _world);

    /*!
     * \brief Отрисовка выделенного объекта в редакторе
     * \param[in] _world - Мир
     * \param[in] _projection - Матрица проекции
     * \param[in] _view - Матрица вида
     */
    void drawSelectedInEditor(FuryWorld* _world,
                              const glm::mat4& _projection,
                              const glm::mat4& _view);
    /*!
     * \brief Отрисовка компоненты
     * \param[in] _world - Мир
     * \param[in] _component - Пара <Объект, Меш> для отрисовки
     * \param[in] _projection - Матрица проекции
     * \param[in] _view - Матрица вида
     * \param[in] _lightSpaceMatrix - Матрица пространства теней
     */
    void drawComponent(FuryWorld* _world,
                       const QPair<FuryObject*, FuryMesh*>& _component,
                       const glm::mat4& _projection,
                       const glm::mat4& _view,
                       const glm::mat4& _lightSpaceMatrix);

    void do_movement();

private:
    //! Запрещаем конструктор копирования
    FuryRenderer(const FuryRenderer&) = delete;
    //! Запрещаем оператор присваивания
    FuryRenderer& operator=(const FuryRenderer&) = delete;

private:
    //! Экземпляр класса
    static FuryRenderer* s_instance;

private:
    FuryEventListener* m_eventListener;


    void loadRaceMapFromJson();

    void renderLoading();

    void updatePhysics();

    void displayBuffer(GLuint _bufferId);
    void displayLogo(int _width, int _height);

    //! Тестовая инициализация буферов рендеринга
    /*!
     * \brief Создание буфера рендеринга
     * \param[out] _frameBuffer - Фрейм буфер
     * \param[out] _renderBuffer - Рендер буфер
     * \param[out] _renderTexture - Рендер текстура
     * \param[in] _width - Ширина
     * \param[in] _height - Высота
     */
    void createRenderBuffer(GLuint* _frameBuffer,
                            GLuint* _renderBuffer,
                            GLuint* _renderTexture,
                            int _width, int _height);
    //! Рендер отрисованной сцены на экран из буфера
    void renderMainBuffer();


    FuryObject* m_sunVisualBox;
    FuryWorld* m_testWorld;


    Shader* m_particleShader;
    Shader* m_pbrShader;


    Particle* m_myFirstParticle;
    ParticleSystem* m_myFirstParticleSystem;

    glm::vec3 m_dirlight_position = glm::vec3(-8.0f, 6.0f, 3.0f);


    float m_perspective_near = 0.1f;
    float m_perspective_far = 300.f;


    FuryTextureManager* m_textureManager;
    FuryModelManager* m_modelManager;
    FuryMaterialManager* m_materialManager;
    FuryWorldManager* m_worldManager;
    FuryShaderManager* m_shaderManager;
    FuryScriptManager* m_scriptManager;


    bool m_is_loading = true;
    float m_loadingOvertime = 1.0f;
    FuryTextureCache* m_loadingTextureCache;


    CarObject* m_carObject = nullptr;

    float m_shadowNear = 0.1f;
    float m_shadowPlane = 70.f;
    float m_shadowViewSize = 25.f;
    float m_shadowCamDistance = 37;

    bool m_needDebugRender;

    GLuint m_mainFrameBuffer = 0;
    GLuint m_mainRenderBuffer = 0;
    GLuint m_mainRenderTexture = 0;

    GLuint m_testFrameBuffer = 0;
    GLuint m_testRenderBuffer = 0;
    GLuint m_testRenderTexture = 0;

private:
    //! Камеры
    QVector<Camera*> m_cameras;
    //! Аккумулятор для обновления физики
    double m_updateAccumulator;

    FuryLearningScript* m_learnScript;
    int m_learnSpeed;
    QList<float> m_scoreList;

    //! Отображатель кодов русских клавиш на латинские
    FuryBaseLocalKeyMapper* m_russianKeyMapper;

    QOpenGLContext* m_context;
    QOffscreenSurface* m_surface;
};

#endif // FURYRENDERER_H
