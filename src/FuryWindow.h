#ifndef FURYWINDOW_H
#define FURYWINDOW_H


#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

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
#include "TextureLoader.h"
#include "Particle.h"
#include "ParticleSystem.h"


#include "World.h"
#include "BoxObject.h"
#include "TextureManager.h"
#include "CarObject.h"
#include "SphereObject.h"

class FuryWindow {
public:
    FuryWindow(int width, int height, const char* title);
    FuryWindow();
    ~FuryWindow();

    void show();
    void renderThread();
    bool m_isOpen;


    int width;
    int height;
    const char* m_title;
    float m_deltaTime;
    float m_lastFrame;
    float lastX;
    float lastY;
    bool keys[1024] = { false };

    // Mouse enable
    bool mouse_enable = true;

    void init();
    void InitGL();
    void render();

    // Function prototypes
    void on_key_callback(int key, int scancode, int action, int mode);
    void on_mouse_callback(double xpos, double ypos);
    void on_scroll_callback(double xoffset, double yoffset);
    void on_resize_callback(int width, int height);
    void do_movement();

private:
    reactphysics3d::PhysicsCommon* own_physicsCommon;
    reactphysics3d::PhysicsWorld* our_physicsWorld;
    reactphysics3d::RigidBody* physics_sphere;
    reactphysics3d::RigidBody* physics_sphere2;
    reactphysics3d::RigidBody* physics_floor;


    void loadPBR();
    void initFloorModel();
    void loadModel();
    void initSkyboxModel();
    void initDepthMapFBO();

    void createPBRTextures();

    void renderDepthMap();
    glm::mat4 getLightSpaceMatrix();

    void displayBuffer(GLuint _bufferId);


    BoxObject* m_my_first_boxObject;
    BoxObject* m_bigFloor;
    std::vector<BoxObject*> m_new_floor;
    std::vector<BoxObject*> m_test_physics_cubes;
    BoxObject* m_sunVisualBox;
    World* m_testWorld;


    Shader* m_ourShader;
    Shader* m_skyboxShader;
    Shader* m_particleShader;
    Shader* m_pbrShader;
    Shader* m_floorShader;
    Shader* m_simpleDepthShader;

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

    std::vector<raw_Texture*> m_textures_to_loading;
    std::vector<unsigned char*> m_ourModel_data_array;
    std::vector<int> m_ourModel_width_array;
    std::vector<int> m_ourModel_height_array;

    bool m_ourModel_textures_loaded = false;
    bool m_ourModel_textures_bind = false;
    bool m_cubemap_bind = false;


    GLuint m_skyboxVAO, m_skyboxVBO;

    bool m_is_loading = true;

    GLuint m_cubemapTexture = 0;
    bool m_cubemap_is_loaded = false;
    std::vector<unsigned char*> m_data_array;
    std::vector<int> m_width_array;
    std::vector<int> m_height_array;

    GLuint m_depthMapFBO;
    GLuint m_depthMap;

    glm::vec3 m_dirlight_position = glm::vec3(-8.0f, 6.0f, 3.0f);


    float m_perspective_near = 0.1;
    float m_perspective_far = 300;

    glm::vec3 m_dirLightAmbient = glm::vec3(0.35f, 0.35f, 0.35f);
    glm::vec3 m_dirLightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 m_dirLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);


    TextureManager m_textureManager;
    CarObject* m_carObject = nullptr;

private:
    //! Камеры
    std::vector<Camera*> m_cameras;
};

#endif // FURYWINDOW_H
