#ifndef TESTRENDER_H
#define TESTRENDER_H

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <QTime>


// Other Libs
#include <SOIL.h>
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
#include "model.h"
#include "Particle.h"
#include "ParticleSystem.h"


#include "FuryWorld.h"
#include "FuryBoxObject.h"
#include "FuryTextureManager.h"
#include "CarObject.h"
#include "FuryEventListener.h"
#include "FuryMaterial.h"
#include "FuryModel.h"
#include "FuryModelManager.h"
#include "FuryMaterialManager.h"


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
    void setCarCameraPos(int _x, int _y);

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
    const char* m_title;
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
    reactphysics3d::PhysicsWorld* our_physicsWorld;
    reactphysics3d::RigidBody* physics_sphere;
    reactphysics3d::RigidBody* physics_sphere2;
    reactphysics3d::RigidBody* physics_floor;

    FuryEventListener* m_eventListener;


    void loadPBR();
    void initFloorModel();
    void loadModel();
    void initSkyboxModel();
    void initDepthMapFBO();
    void initRaceMap();

    void createPBRTextures();

    void renderDepthMap();
    glm::mat4 getLightSpaceMatrix();

    void renderLoadingAndBindData(float _currentFrame);
    void renderOldModel();
    void renderPbrSpheres();

    void updatePhysics();

    void displayBuffer(GLuint _bufferId);


    FuryBoxObject* m_bigFloor;
    //QVector<FuryBoxObject*> m_new_floor;
    QVector<FuryBoxObject*> m_test_physics_cubes;
    FuryBoxObject* m_sunVisualBox;
    FuryWorld* m_testWorld;


    Shader* m_ourShader;
    Shader* m_skyboxShader;
    Shader* m_particleShader;
    Shader* m_pbrShader;
    Shader* m_floorShader;
    Shader* m_simpleDepthShader;
    Shader* m_bigFloorShader;
    Shader* m_testMaterialShader;

    GLuint m_particle_texture_id;
    GLuint m_gold_albedo_texture_id;
    GLuint m_gold_norm_texture_id;
    GLuint m_gold_metallic_texture_id;
    GLuint m_gold_roughness_texture_id;
    GLuint m_gold_ao_texture_id;

    unsigned int m_irradianceMap = 0;
    unsigned int m_prefilterMap;
    unsigned int m_brdfLUTTexture;


    GLuint m_win_texture_id;
    GLuint m_smoke_texture_id;
    Particle* m_myFirstParticle;
    ParticleSystem* m_myFirstParticleSystem;

    GLuint m_floorVBO, m_floorEBO, m_floorVAO;


    Model* m_ourModel;
    float m_modelScale;
    glm::vec3 m_ourModel_position;

    QVector<raw_Texture*> m_textures_to_loading;
    QVector<unsigned char*> m_ourModel_data_array;
    QVector<int> m_ourModel_width_array;
    QVector<int> m_ourModel_height_array;

    bool m_ourModel_textures_loaded = false;
    bool m_ourModel_textures_bind = false;
    bool m_cubemap_bind = false;


    GLuint m_skyboxVAO, m_skyboxVBO;

    bool m_is_loading = true;

    GLuint m_cubemapTexture = 0;
    bool m_cubemap_is_loaded = false;
    QVector<unsigned char*> m_data_array;
    QVector<int> m_width_array;
    QVector<int> m_height_array;

    GLuint m_depthMapFBO;
    GLuint m_depthMap;

    glm::vec3 m_dirlight_position = glm::vec3(-8.0f, 6.0f, 3.0f);


    float m_perspective_near = 0.1f;
    float m_perspective_far = 300.f;

    glm::vec3 m_dirLightAmbient = glm::vec3(0.35f, 0.35f, 0.35f);
    glm::vec3 m_dirLightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 m_dirLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);


    FuryTextureManager* m_textureManager;
    FuryModelManager* m_modelManager;
    FuryMaterialManager* m_materialManager;
    CarObject* m_carObject = nullptr;

    float m_shadowNear = 0.1f;
    float m_shadowPlane = 70.f;
    float m_shadowViewSize = 25.f;
    float m_shadowCamDistance = 37;

private:
    //! Камеры
    QVector<Camera*> m_cameras;
    //! Аккумулятор для обновления физики
    double m_updateAccumulator;
};

#endif // TESTRENDER_H
