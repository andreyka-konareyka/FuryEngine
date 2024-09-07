#ifndef TESTRENDER_H
#define TESTRENDER_H

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
#include "Managers/FuryTextureManager.h"
#include "CarObject.h"
#include "Physics/FuryEventListener.h"
#include "Managers/FuryModelManager.h"
#include "Managers/FuryMaterialManager.h"
#include "FuryScript.h"
#include "LocalKeyboard/FuryBaseLocalKeyMapper.h"


#include <QOpenGLWidget>


//! Тестовый класс рендера
class TestRender : public QOpenGLWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский виджет
     */
    explicit TestRender(QWidget* _parent = nullptr);
    //! Деструктор
    ~TestRender();

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

    /*!
     * \brief Установка позиции камеры машины
     * \param[in] _x - Координата x
     * \param[in] _y - Координата y
     */
    void setCarCameraPos(float _x, float _y);

    void setCarSpringLenght(float _lenght);
    void setCarSpringK(float _k);


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

private slots:
    //! Слот обновления фрейма
    void updateGL();


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
    reactphysics3d::PhysicsCommon* own_physicsCommon;
    FuryEventListener* m_eventListener;


    void initSkyboxModel();
    void initDepthMapFBO();
    void loadRaceMapFromJson();

    void createPBRTextures();

    void renderDepthMap();
    glm::mat4 getLightSpaceMatrix();

    void renderLoading(float _currentFrame);

    void updatePhysics();

    void displayBuffer(GLuint _bufferId);
    void displayLogo();

    //! Инициализация буферов главного рендеринга
    void initMainRender();
    //! Рендер отрисованной сцены на экран из буфера
    void renderMainBuffer();


    FuryObject* m_sunVisualBox;
    FuryWorld* m_testWorld;


    Shader* m_particleShader;
    Shader* m_pbrShader;
    Shader* m_backgroundShader;
    Shader* m_simpleDepthShader;
    Shader* m_brdfShader;

    unsigned int m_irradianceMap = 0;
    unsigned int m_prefilterMap;
    unsigned int m_brdfLUTTexture = 0;

    GLuint m_envCubemap = 0;


    Particle* m_myFirstParticle;
    ParticleSystem* m_myFirstParticleSystem;



    GLuint m_skyboxVAO, m_skyboxVBO;

    bool m_is_loading = true;
    float m_loadingOvertime = 3.0f;


    GLuint m_depthMapFBO;
    GLuint m_depthMap;

    glm::vec3 m_dirlight_position = glm::vec3(-8.0f, 6.0f, 3.0f);


    float m_perspective_near = 0.1f;
    float m_perspective_far = 300.f;


    FuryTextureManager* m_textureManager;
    FuryModelManager* m_modelManager;
    FuryMaterialManager* m_materialManager;
    CarObject* m_carObject = nullptr;

    float m_shadowNear = 0.1f;
    float m_shadowPlane = 70.f;
    float m_shadowViewSize = 25.f;
    float m_shadowCamDistance = 37;

    bool m_needDebugRender;

    GLuint m_mainFrameBuffer = 0;
    GLuint m_mainRenderBuffer = 0;
    GLuint m_mainRenderTexture = 0;

private:
    //! Камеры
    QVector<Camera*> m_cameras;
    //! Аккумулятор для обновления физики
    double m_updateAccumulator;

    FuryScript* m_learnScript;
    int m_learnSpeed;
    QList<float> m_scoreList;

    //! Отображатель кодов русских клавиш на латинские
    FuryBaseLocalKeyMapper* m_russianKeyMapper;
};

#endif // TESTRENDER_H
