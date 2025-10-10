#ifndef FURYRENDERER_H
#define FURYRENDERER_H

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

#include <QTime>


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Particle.h"
#include "ParticleSystem.h"


#include "FuryWorld.h"
#include "CarObject.h"
#include "Managers/FuryTextureManager.h"
#include "Physics/FuryEventListener.h"
#include "Managers/FuryModelManager.h"
#include "Managers/FuryMaterialManager.h"
#include "Managers/FuryWorldManager.h"
#include "Managers/FuryShaderManager.h"
#include "FuryLearningScript.h"
#include "LocalKeyboard/FuryBaseLocalKeyMapper.h"


#include <QOpenGLWidget>


class FuryScriptManager;


//! Основной класс рендера
class FuryRenderer : public QOpenGLWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский виджет
     * \throw FuryException в случае повторного создания
     */
    explicit FuryRenderer(QWidget* _parent = nullptr);
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

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

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

protected:
    void mouseMoveEvent(QMouseEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void keyPressEvent(QKeyEvent* _event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;


private:
    int m_width;
    int m_height;
    float m_deltaTime;
    float m_lastFrame;
    float lastX;
    float lastY;
    QMap<int, bool> m_keys;

    // Mouse enable
    //bool mouse_enable = true;

    void init();
    void InitGL();
    void render();

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

    void renderLoading(float _currentFrame);

    void updatePhysics();

    void displayBuffer(GLuint _bufferId);
    void displayLogo();

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


    bool m_is_loading = true;
    float m_loadingOvertime = 1.0f;

    glm::vec3 m_dirlight_position = glm::vec3(-8.0f, 6.0f, 3.0f);


    float m_perspective_near = 0.1f;
    float m_perspective_far = 300.f;


    FuryTextureManager* m_textureManager;
    FuryModelManager* m_modelManager;
    FuryMaterialManager* m_materialManager;
    FuryWorldManager* m_worldManager;
    FuryShaderManager* m_shaderManager;
    FuryScriptManager* m_scriptManager;
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
};

#endif // FURYRENDERER_H
