#include "TestRender.h"

#include <QTimer>
#include <QDir>


#include <string>

#include "FuryLogger.h"
#include "TextureLoader.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QString>
#include <QMouseEvent>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QMessageBox>


const bool NEED_DRAW_SHADOW = false;


const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
QVector<QString> pbr_material_names{ "pbr_gold", "pbr_grass", "pbr_iron", "pbr_plastic", "pbr_wall" };
QString current_pbr_material_name = pbr_material_names[2];

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
};



void renderSphere();
void renderCube();
void renderQuad();



TestRender::TestRender(QWidget *_parent) :
    QOpenGLWidget(_parent),
    m_textureManager(FuryTextureManager::createInstance()),
    m_modelManager(FuryModelManager::createInstance()),
    m_materialManager(FuryMaterialManager::createInstance()),
    m_updateAccumulator(0)
{
    Debug(ru("Создание рендера"));

    connect(this, &QOpenGLWidget::frameSwapped,
            this, &TestRender::updateGL);
    setFocusPolicy(Qt::StrongFocus);
}

TestRender::~TestRender()
{
    Debug(ru("Удаление рендера"));

    for (Camera* camera : m_cameras)
    {
        delete camera;
        camera = nullptr;
    }

    if (m_testWorld != nullptr)
    {
        delete m_testWorld;
        m_testWorld = nullptr;
    }

    our_physicsWorld->setEventListener(nullptr);

    if (m_eventListener != nullptr)
    {
        delete m_eventListener;
        m_eventListener = nullptr;
    }

    Debug(ru("Остановка текстурного менеджера..."));
    m_textureManager->stopLoopAndWait();
    FuryTextureManager::deleteInstance();

    Debug(ru("Остановка менеджера моделей..."));
    m_modelManager->stopLoopAndWait();
    FuryModelManager::deleteInstance();

    Debug(ru("Удаление менеджера материалов..."));
    FuryMaterialManager::deleteInstance();
}

void TestRender::initializeGL()
{
    Debug(ru("Инициализация OpenGL"));
    InitGL();
}

void TestRender::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);

    m_height = h;
    m_width = w;
}

void TestRender::paintGL()
{
    static QDateTime lastTime = QDateTime::currentDateTime();
    static QDateTime lastFPSUpdate = QDateTime::currentDateTime();
    QDateTime startTime = QDateTime::currentDateTime();

    {
        static int intervalLoadPart = 0;

        render();

        if (intervalLoadPart == 0)
        {
            m_textureManager->loadTexturePart();
        }
        else if (intervalLoadPart == 5)
        {
            m_modelManager->loadModelPart();
        }

        intervalLoadPart = (intervalLoadPart + 1) % 10;
    }


    QDateTime endTime = QDateTime::currentDateTime();

    if (lastFPSUpdate.msecsTo(endTime) >= 200)
    {
        QString test = QString("FPS: %1; \tRender: %2; \tQt: %3;")
                .arg(1000.0 / lastTime.msecsTo(endTime))
                .arg(startTime.msecsTo(endTime))
                .arg(lastTime.msecsTo(startTime));
        emit setWindowTitleSignal(test);

        int computerLoad = 100 * startTime.msecsTo(endTime) / lastTime.msecsTo(endTime);
        emit setComputerLoadSignal(computerLoad);

        lastFPSUpdate = endTime;
    }

    lastTime = endTime;
}

void TestRender::setCameraZoomValue(int _value)
{
    for (Camera* camera : m_cameras)
    {
        camera->setCameraZoom(_value);
    }
}

void TestRender::setCarCameraPos(int _x, int _y)
{
    m_carObject->setLocalCameraPosition(_x, _y);
}

void TestRender::setCarSpringLenght(float _lenght)
{
    m_carObject->setSpringLenght(_lenght);
}

void TestRender::setCarSpringK(float _k)
{
    m_carObject->setSpringK(_k);
}

void TestRender::mouseMoveEvent(QMouseEvent* _event)
{
    if (m_testWorld->camera() == m_cameras[1])
    { // Если не камера машины
        return;
    }

    if (_event->buttons().testFlag(Qt::LeftButton))
    {
        GLfloat xoffset = (GLfloat)(_event->position().x() - lastX);
        GLfloat yoffset = (GLfloat)(lastY - _event->position().y());

        lastX = (GLfloat)(_event->position().x());
        lastY = (GLfloat)(_event->position().y());

        m_testWorld->camera()->processMouseMovement(xoffset, yoffset);
    }
}

void TestRender::mousePressEvent(QMouseEvent *_event)
{
    if (m_testWorld->camera() == m_cameras[1])
    { // Если не камера машины
        return;
    }

    if (_event->button() == Qt::LeftButton)
    {
        lastX = (GLfloat)(_event->position().x());
        lastY = (GLfloat)(_event->position().y());
    }
}

void TestRender::keyPressEvent(QKeyEvent *_event)
{
    m_keys[_event->key()] = true;

    if (m_testWorld->camera() == m_cameras[1])
    { // Если камера машины
        m_carObject->keyPressEvent(_event->key());
    }


    if (m_keys[Qt::Key_QuoteLeft] == true)
    {
        if (m_testWorld->camera() == m_cameras[0])
        {
            m_testWorld->setCamera(m_cameras[1]);
        }
        else
        {
            m_testWorld->setCamera(m_cameras[0]);
            m_carObject->resetKeyInput();
        }
    }
}

void TestRender::keyReleaseEvent(QKeyEvent *_event)
{
    m_keys[_event->key()] = false;

    if (m_testWorld->camera() == m_cameras[1])
    { // Если камера машины
        m_carObject->keyReleaseEvent(_event->key());
    }
}

void TestRender::updateGL()
{
    update();
}




void TestRender::InitGL()
{
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, this->m_width, this->m_height);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); // enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);


    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    InitParticleMesh();


    // Загрузка всех текстур, шейдеров и т.д.
    init();
}

void TestRender::init() {
    Debug(ru("Версия OpenGL: ") + ru(glGetString(GL_VERSION)));
    Debug(ru("Количество потоков: ") + QString::number(QThread::idealThreadCount()));
    own_physicsCommon = new reactphysics3d::PhysicsCommon;

    m_testWorld = new FuryWorld(own_physicsCommon);
    our_physicsWorld = m_testWorld->physicsWorld();
    m_eventListener = new FuryEventListener;
    our_physicsWorld->setEventListener(m_eventListener);

    m_modelManager->addModel("objects/cube/cube.obj", "cube");
    m_modelManager->addModel("objects/sphere/sphere.obj", "sphere");

    m_testWorld->addObject(new FuryBoxObject(m_testWorld, glm::vec3(0, 5, 0), 1, 1, 1));
    int count_in_line = 6;
    for (int i = 0; i < count_in_line; ++i) {
        for (int j = 0; j < count_in_line; ++j) {
            FuryBoxObject* box = new FuryBoxObject(m_testWorld, glm::vec3((i - count_in_line / 2 + 0.5) * 5,
                                                             -1.25 - 0.02,
                                                             (j - count_in_line / 2 + 0.5) * 5),
                                                   5, 2.5, 5);
            box->setTextureName("Diffuse_numbersBoxTexture");
            box->Setup_physics(reactphysics3d::BodyType::STATIC);
            box->setName("startWall");
            box->physicsBody()->setUserData(box);
            m_testWorld->addObject(box);

            if (!m_materialManager->materialExist("numberBoxMaterial"))
            {
                FuryMaterial* mat = m_materialManager->createMaterial("numberBoxMaterial");
                mat->setDiffuseTexture("Diffuse_numbersBoxTexture");
            }

            box->setMaterialName("numberBoxMaterial");
        }
    }


    m_sunVisualBox = new FuryBoxObject(m_testWorld, m_dirlight_position);
    m_sunVisualBox->Setup_physics(rp3d::BodyType::STATIC);
    m_testWorld->addObject(m_sunVisualBox);
    // Camera
    m_cameras.push_back(new Camera(glm::vec3(0.0f, 10.0f, 40.0f)));
    m_cameras.push_back(new Camera(glm::vec3(0.0f, 30.0f, 60.0f)));
    m_testWorld->setCamera(m_cameras[0]);

    FurySphereObject* testSphere = new FurySphereObject(m_testWorld, glm::vec3(6, 10, 10), 2);
    testSphere->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(testSphere);

    //
    // МАШИНА
    //
    m_testMaterialShader = new Shader("shaders/testMaterialShader.vs", "shaders/testMaterialShader.frag");

    m_carObject = new CarObject(m_testWorld, glm::vec3(0, -0.5, 22.5), m_testMaterialShader);
    m_carObject->Setup_physics(reactphysics3d::BodyType::DYNAMIC);

    const QVector<FuryObject*>& tempObjectsForDraw = m_carObject->objectsForDraw();
    for (int i = 0; i < tempObjectsForDraw.size(); ++i)
    {
        m_testWorld->addObject(tempObjectsForDraw[i]);
    }


    // Большой пол
    m_bigFloor = new FuryBoxObject(m_testWorld, glm::vec3(0, -3, 0), 500, 1, 500);
    m_bigFloor->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_bigFloor->setTextureName("asphalt");
    m_testWorld->addObject(m_bigFloor);

    if (!m_materialManager->materialExist("asphaltMaterial"))
    {
        FuryMaterial* mat = m_materialManager->createMaterial("asphaltMaterial");
        mat->setDiffuseTexture("Diffuse_asphalt");
    }

    m_bigFloor->setMaterialName("asphaltMaterial");

    // Настройка физики сфер
    reactphysics3d::Vector3 position(2, 7, 0);
    reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    reactphysics3d::Transform transform(position, orientation);
    physics_sphere = our_physicsWorld->createRigidBody(transform);
    physics_sphere->setType(reactphysics3d::BodyType::STATIC);

    reactphysics3d::SphereShape* sphereShape = own_physicsCommon->createSphereShape(1);
    reactphysics3d::Transform transform_shape = reactphysics3d::Transform::identity();
    physics_sphere->addCollider(sphereShape, transform_shape);



    reactphysics3d::Vector3 position2(2 + 0.2f, 11, 0);
    reactphysics3d::Quaternion orientation2 = reactphysics3d::Quaternion::identity();
    reactphysics3d::Transform transform2(position2, orientation2);
    physics_sphere2 = our_physicsWorld->createRigidBody(transform2);
    physics_sphere2->setType(reactphysics3d::BodyType::STATIC);

    reactphysics3d::SphereShape* sphereShape2 = own_physicsCommon->createSphereShape(1);
    reactphysics3d::Transform transform_shape2 = reactphysics3d::Transform::identity();
    physics_sphere2->addCollider(sphereShape2, transform_shape2);

    // Конец настроек физики


    m_ourShader = new Shader("model_loading.vs", "model_loading.fs");
    m_skyboxShader = new Shader("skybox.vs", "skybox.fs");
    m_particleShader = new Shader("particle.vs", "particle.fs");
    m_pbrShader = new Shader("pbr.vs", "pbr.fs");
    m_floorShader = new Shader("multiple_lights.vs", "multiple_lights.fs");
    m_simpleDepthShader = new Shader("simpleDepthShader.vs", "simpleDepthShader.fs");
    m_bigFloorShader = new Shader("shaders/bigFloorShader.vs", "shaders/bigFloorShader.frag");

    {
        FuryMaterial* testMaterial = new FuryMaterial(glm::vec3(0.5, 0.2, 0.7),
                                          glm::vec3(0.3, 0.3, 0.3));
        testMaterial->setShaderMaterial(m_testMaterialShader);

        FuryBoxObject* box = new FuryBoxObject(m_testWorld,
                                               glm::vec3(0, -2.5, 22.5),
                                               glm::vec3(1.5, 1.5, 1.4),
                                               glm::vec3(0, 3.14/2, 0));
        box->setShader(m_testMaterialShader);
        box->setName("testMaterialBox");
        m_testWorld->addObject(box);
    }

    m_bigFloor->setShader(m_bigFloorShader);


    // PBR-материалы
    loadPBR();



    int win_w, win_h;
    auto data_win_texture = TextureLoader::LoadDataFromFile("textures/blending_transparent_window.png", win_w, win_h);
    m_win_texture_id = TextureLoader::BindDataToTexture(data_win_texture, win_w, win_h);

    int smoke_w, smoke_h;
    auto data_smoke_texture = TextureLoader::LoadDataFromFile("textures/smoke_ver2.png", smoke_w, smoke_h);
    m_smoke_texture_id = TextureLoader::BindDataToTexture(data_smoke_texture, smoke_w, smoke_h);


    glm::vec3 part_pos(5, 0, 0);
    double part_scale = 0.15;
    glm::vec3 part_speed(0, 0.5, 0);
    glm::vec4 part_color(1, 1, 0, 1);
    m_myFirstParticle = new Particle(part_pos, part_scale, part_speed, part_color, m_particle_texture_id, 10, m_particleShader);

    glm::vec3 part_sys_pos(0, 2, 0);
    m_myFirstParticleSystem = new ParticleSystem(part_sys_pos, m_smoke_texture_id, 1);





    initFloorModel();
    loadModel();
    initSkyboxModel();

    initDepthMapFBO();
    initRaceMap();

    m_textureManager->addTexture("textures/box_texture_5x5.png", "defaultBoxTexture");
    m_textureManager->addTexture("textures/box_texture3_orig.png", "Diffuse_numbersBoxTexture");
    m_textureManager->addTexture("textures/carBody.png", "carBody");
    m_textureManager->addTexture("textures/rayCastBall.png", "Diffuse_rayCastBall");
    m_textureManager->addTexture("textures/raceWall.png", "Diffuse_raceWall");
    m_textureManager->addTexture("textures/asphalt.png", "Diffuse_asphalt");
    m_textureManager->addTexture("textures/redCheckBox.png", "redCheckBox");
    m_textureManager->addTexture("textures/greenCheckBox.png", "greenCheckBox");

    m_modelManager->addModel("objects/car2/LOD2.obj", "backpack2LOD2");
    m_modelManager->addModel("objects/car2/car.obj", "backpack2");

    {
        FuryBoxObject* object = new FuryBoxObject(m_testWorld,
                                                  glm::vec3(-7.5, -1.25, 22.5),
                                                  glm::vec3(13, 2.5, 0.5),
                                                  glm::vec3(0, 3.14/2, 0));
        object->setTextureName("redCheckBox");
        m_testWorld->addTransparentObject(object);

        object = new FuryBoxObject(m_testWorld,
                                   glm::vec3(-5.5, -1.25, 22.5),
                                   glm::vec3(13, 2.5, 0.5),
                                   glm::vec3(0, 3.14/2, 0));
        object->setTextureName("greenCheckBox");
        m_testWorld->addTransparentObject(object);

        object = new FuryBoxObject(m_testWorld,
                                   glm::vec3(-9.5, -1.25, 22.5),
                                   glm::vec3(13, 2.5, 0.5),
                                   glm::vec3(0, 3.14/2, 0));
        object->setTextureName("redCheckBox");
        m_testWorld->addTransparentObject(object);

        object = new FuryBoxObject(m_testWorld,
                                   glm::vec3(-11.5, -1.25, 22.5),
                                   glm::vec3(13, 2.5, 0.5),
                                   glm::vec3(0, 3.14/2, 0));
        object->setTextureName("greenCheckBox");
        m_testWorld->addTransparentObject(object);
    }

    {
        // initShaderInform

        QList<Shader*> shaders({
                                   m_testWorld->getObjects()[0]->shader(),
                                   m_bigFloorShader,
                                   testSphere->shader(),
                                   m_ourShader,
                                   m_floorShader,
                                   m_testMaterialShader
                               });

        for (Shader* shader : shaders)
        {
            shader->Use();
            shader->setVec3("dirLight.ambient", m_dirLightAmbient); // 0.25f
            shader->setVec3("dirLight.diffuse", m_dirLightDiffuse); // 0.35f
            shader->setVec3("dirLight.specular", m_dirLightSpecular); // 0.4f
            // point light 1
            shader->setVec3("pointLights[0].position", pointLightPositions[0]);
            shader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
            shader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
            shader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
            shader->setFloat("pointLights[0].constant", 1.0f);
            shader->setFloat("pointLights[0].linear", 0.09f);
            shader->setFloat("pointLights[0].quadratic", 0.032f);
            // point light 2
            shader->setVec3("pointLights[1].position", pointLightPositions[1]);
            shader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
            shader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
            shader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
            shader->setFloat("pointLights[1].constant", 1.0f);
            shader->setFloat("pointLights[1].linear", 0.09f);
            shader->setFloat("pointLights[1].quadratic", 0.032f);
            // point light 3
            shader->setVec3("pointLights[2].position", pointLightPositions[2]);
            shader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
            shader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
            shader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
            shader->setFloat("pointLights[2].constant", 1.0f);
            shader->setFloat("pointLights[2].linear", 0.09f);
            shader->setFloat("pointLights[2].quadratic", 0.032f);
            // point light 4
            shader->setVec3("pointLights[3].position", pointLightPositions[3]);
            shader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
            shader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
            shader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
            shader->setFloat("pointLights[3].constant", 1.0f);
            shader->setFloat("pointLights[3].linear", 0.09f);
            shader->setFloat("pointLights[3].quadratic", 0.032f);
        }


        m_pbrShader->Use();

        m_pbrShader->setVec3("lightPositions[0]", glm::vec3(-10.0f, 10.0f, 10.0f));
        m_pbrShader->setVec3("lightColors[0]", 300.0f, 300.0f, 300.0f);
        // point light 2
        m_pbrShader->setVec3("lightPositions[1]", glm::vec3(10.0f, 10.0f, 10.0f));
        m_pbrShader->setVec3("lightColors[1]", 300.0f, 300.0f, 300.0f);
        // point light 3
        m_pbrShader->setVec3("lightPositions[2]", glm::vec3(-10.0f, -10.0f, 10.0f));
        m_pbrShader->setVec3("lightColors[2]", 300.0f, 300.0f, 300.0f);
        // point light 4
        m_pbrShader->setVec3("lightPositions[3]", glm::vec3(10.0f, -10.0f, 10.0f));
        m_pbrShader->setVec3("lightColors[3]", 300.0f, 300.0f, 300.0f);
    }
}

void TestRender::render()
{
    {
        // Calculate deltatime of current frame
        static QTime startTime = QTime::currentTime();
        float currentFrame = startTime.msecsTo(QTime::currentTime());
        m_deltaTime = (currentFrame - m_lastFrame)/1000.0;
        m_lastFrame = currentFrame;

        /*
        =======================================================
        =================  Загрузка текстур  ==================
        =======================================================
        */
        if (m_is_loading)
        {
            renderLoadingAndBindData(currentFrame);
            return;
        }

        /*
        =======================================================
        ===================  Обычный режим  ===================
        =======================================================
        */

        m_dirlight_position.x = sin(currentFrame / 1000 / 4) * 10;
        m_dirlight_position.z = cos(currentFrame / 1000 / 4) * 10;

        glm::vec3 tempPosition = m_dirlight_position;
        tempPosition *= 3;
        tempPosition += m_testWorld->camera()->position();
        m_sunVisualBox->setPosition(tempPosition);



        if (m_irradianceMap == 0)
        {
            createPBRTextures();
        }



        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions

        if (m_height == 0 || m_width == 0)
        {
            return;
        }

        do_movement();

        updatePhysics();


        m_cameras[1]->setPosition(m_carObject->cameraPosition());
        m_cameras[1]->setFront(m_carObject->cameraViewPoint() - m_carObject->cameraPosition());


        // 1. сначала рисуем карту глубины
        if (NEED_DRAW_SHADOW)
        {
            renderDepthMap();
        }
        glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();





        // 2. рисуем сцену как обычно с тенями (используя карту глубины)

        // Clear the colorbuffer
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderOldModel();
        renderPbrSpheres();


        glm::mat4 projection = glm::perspective(glm::radians(m_testWorld->camera()->zoom()), (float)this->m_width / (float)this->m_height, m_perspective_near, m_perspective_far);
        glm::mat4 view =  m_testWorld->camera()->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);


        const QVector<FuryObject*>& testWorldObjects = m_testWorld->getObjects();

        for (int i = 0; i < testWorldObjects.size(); ++i)
        {
            FuryObject* renderObject = testWorldObjects[i];
            Shader* shader = renderObject->shader();

            {

                shader->Use();


                shader->setVec3("viewPos", m_testWorld->camera()->position());
                shader->setFloat("material.shininess", 128.0f); // 32.0 - default
                shader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - m_dirlight_position);


                // view/projection transformations
                glm::mat4 projection = glm::perspective(glm::radians(m_testWorld->camera()->zoom()), (float)m_width / (float)m_height, m_perspective_near, m_perspective_far);
                glm::mat4 view = m_testWorld->camera()->getViewMatrix();
                shader->setMat4("projection", projection);
                shader->setMat4("view", view);

                shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

                glm::mat4 model = renderObject->getOpenGLTransform();
                model = glm::scale(model, renderObject->scales());	// it's a bit too big for our scene, so scale it down

                shader->setMat4("model", model);




                if (renderObject->name() == "testMaterialBox")
                {
                    continue;
                }

                if (renderObject->name() != "carBody")
                {
                    glm::vec3 lightPos = m_testWorld->camera()->position()
                                       + m_dirlight_position;
                    shader->setVec3("lightPos", lightPos);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, m_depthMap);

                    renderObject->draw();
                }
                else
                {
                    glm::vec3 lightPos = m_testWorld->camera()->position()
                                       + m_dirlight_position;
                    shader->setVec3("lightPos", lightPos);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, m_depthMap);


                    model = renderObject->getOpenGLTransform();

                    glm::mat4 testSubModel = glm::mat4(1.0f);

                    testSubModel = glm::translate(testSubModel, glm::vec3(0, -0.8, 0));
                    testSubModel = glm::rotate(testSubModel, 3.14f/2, glm::vec3(0, 1, 0));
                    testSubModel = glm::scale(testSubModel, glm::vec3(1.5, 1.5, 1.4));	// it's a bit too big for our scene, so scale it down

                    model = model * testSubModel;

                    shader->setMat4("model", model);

                    FuryModel* modelObj = m_modelManager->modelByName("backpack2");

                    if (modelObj->isReady())
                    {
                        modelObj->draw(shader);
                    }
                    else
                    {
                        FuryModel* LOD2 = m_modelManager->modelByName("backpack2LOD2");

                        if (LOD2->isReady())
                        {
                            LOD2->draw(shader);
                        }
                        else
                        {
                            Debug(ru("Модель не готова"));
                        }
                    }
                }

                glBindVertexArray(0);
                glActiveTexture(GL_TEXTURE0);

            }
        }


        /*
        =======================================================
        =====================  Скайбокс  ======================
        =======================================================
        */
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        m_skyboxShader->Use();
        view = glm::mat4(glm::mat3(m_testWorld->camera()->getViewMatrix())); // remove translation from the view matrix
        m_skyboxShader->setMat4("view", view);
        m_skyboxShader->setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(m_skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default



        /*
            ============================ Floor =====================================
        */
        if (m_testWorld->camera()->position().y <= 0) {
            glDepthMask(GL_FALSE);
            m_myFirstParticle->Draw(*m_testWorld->camera(), this->m_width, this->m_height);
            m_myFirstParticleSystem->Draw(*m_testWorld->camera(), this->m_width, this->m_height);
            glDepthMask(GL_TRUE);
        }
        m_floorShader->Use();


        m_floorShader->setVec3("viewPos", m_testWorld->camera()->position());
        m_floorShader->setFloat("material.shininess", 128.0f); // 32.0 - default
        m_floorShader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - m_dirlight_position);


        // view/projection transformations
        projection = glm::perspective(glm::radians(m_testWorld->camera()->zoom()), (float)this->m_width / (float)this->m_height, m_perspective_near, m_perspective_far);
        view = m_testWorld->camera()->getViewMatrix();
        m_floorShader->setMat4("projection", projection);
        m_floorShader->setMat4("view", view);


        m_floorShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


        float angle_x = - 3.14f / 2;

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, angle_x, glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(15, 15, 15));	// it's a bit too big for our scene, so scale it down
        m_floorShader->setMat4("model", model);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_win_texture_id);
        glUniform1i(glGetUniformLocation(m_floorShader->Program, "diffuse_texture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        glUniform1i(glGetUniformLocation(m_floorShader->Program, "shadowMap"), 1);

        glBindVertexArray(m_floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        // ##################################
        // ##########   Particle   ##########
        // ##################################




        /*
            Start Draw particle
        */
        if (m_testWorld->camera()->position().y > 0) {
            glDepthMask(GL_FALSE);
            m_myFirstParticle->Draw(*m_testWorld->camera(), this->m_width, this->m_height);
            m_myFirstParticleSystem->Draw(*m_testWorld->camera(), this->m_width, this->m_height);
            glDepthMask(GL_TRUE);
        }

        /*
           End Draw particle
        */

        // ##################################
        // ########   End Particle   ########
        // ##################################


        const QVector<FuryObject*>& testWorldTransparentObjects = m_testWorld->getTransparentObjects();

        QList<QPair<float, FuryObject*>> sorted;
        for (unsigned int i = 0; i < testWorldTransparentObjects.size(); i++){
            float distance = glm::length(m_testWorld->camera()->position() - testWorldTransparentObjects[i]->getPosition());
            sorted.append(qMakePair(distance, testWorldTransparentObjects[i]));
        }

        std::sort(sorted.begin(), sorted.end(), [](auto& p1, auto& p2){return p1.first < p2.first;});

        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            FuryObject* renderObject = it->second;
            Shader* shader = renderObject->shader();

            {

                shader->Use();


                shader->setVec3("viewPos", m_testWorld->camera()->position());
                shader->setFloat("material.shininess", 128.0f); // 32.0 - default
                shader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - m_dirlight_position);


                // view/projection transformations
                glm::mat4 projection = glm::perspective(glm::radians(m_testWorld->camera()->zoom()), (float)m_width / (float)m_height, m_perspective_near, m_perspective_far);
                glm::mat4 view = m_testWorld->camera()->getViewMatrix();
                shader->setMat4("projection", projection);
                shader->setMat4("view", view);

                shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


                //float angle_x = -3.14 / 2;

                glm::mat4 model = renderObject->getOpenGLTransform();
                model = glm::scale(model, renderObject->scales());	// it's a bit too big for our scene, so scale it down

                shader->setMat4("model", model);




                GLuint texture_id = m_textureManager->textureByName(renderObject->textureName()).idOpenGL();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glUniform1i(glGetUniformLocation(shader->Program, "diffuse_texture"), 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, m_depthMap);
                glUniform1i(glGetUniformLocation(shader->Program, "shadowMap"), 1);

                GLuint renderVAO = renderObject->VAO();
                glBindVertexArray(renderVAO);

                if (renderObject->isDrawElements())
                {
                    glDrawElements(renderObject->renderType(), renderObject->vertexCount(), GL_UNSIGNED_INT, 0);
                }
                else
                {
                    glDrawArrays(renderObject->renderType(), 0, renderObject->vertexCount());
                }
                glBindVertexArray(0);


                glActiveTexture(GL_TEXTURE0);

            }
        }

        sorted.clear();


        // displayBuffer(m_depthMap);
    }
}

void TestRender::createPBRTextures()
{
    glCullFace(GL_FRONT);

    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &m_irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    Shader irradianceShader("2.2.2.cubemap.vs", "2.2.2.irradiance_convolution.fs");
    irradianceShader.Use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    Shader prefilterShader("2.2.2.cubemap.vs", "2.2.2.prefilter.fs");
    glGenTextures(1, &m_prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.Use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = (unsigned int)(128 * std::pow(0.5, mip));
        unsigned int mipHeight = (unsigned int)(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    Shader brdfShader("2.2.2.brdf.vs", "2.2.2.brdf.fs");
    glGenTextures(1, &m_brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, m_brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);

    brdfShader.Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glCullFace(GL_BACK);
}

void TestRender::renderDepthMap()
{
    /*float near_plane = 1.0f, far_plane = 25.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    glm::mat4 lightView = glm::lookAt(m_dirlight_position,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));*/

    glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();
    m_simpleDepthShader->Use();
    m_simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    {
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_ourModel_position); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(m_modelScale, m_modelScale, m_modelScale));	// it's a bit too big for our scene, so scale it down
        m_simpleDepthShader->setMat4("model", model);
        m_ourModel->Draw(*m_simpleDepthShader);



        const QVector<FuryObject*>& testWorldObjects = m_testWorld->getObjects();

        for (int i = 0; i < testWorldObjects.size(); ++i)
        {
            FuryObject* renderObject = testWorldObjects[i];


            model = renderObject->getOpenGLTransform();
            model = glm::scale(model, renderObject->scales());	// it's a bit too big for our scene, so scale it down

            m_simpleDepthShader->setMat4("model", model);

            GLuint renderVAO = renderObject->VAO();
            glBindVertexArray(renderVAO);

            if (renderObject->isDrawElements())
            {
                glDrawElements(renderObject->renderType(), renderObject->vertexCount(), GL_UNSIGNED_INT, 0);
            }
            else
            {
                glDrawArrays(renderObject->renderType(), 0, renderObject->vertexCount());
            }
            glBindVertexArray(0);
        }



        {
            const reactphysics3d::Transform& physics_sphere_transform = physics_sphere->getTransform();
            const reactphysics3d::Vector3& physics_sphere_position = physics_sphere_transform.getPosition();
            auto physics_sphere_rotate = physics_sphere_transform.getOrientation();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(physics_sphere_position.x, physics_sphere_position.y, physics_sphere_position.z));

            reactphysics3d::Vector3 axis_rotate;
            reactphysics3d::decimal angle_rotate;
            physics_sphere_rotate.getRotationAngleAxis(angle_rotate, axis_rotate);
            if (axis_rotate.x != 0 ||
                axis_rotate.y != 0 ||
                axis_rotate.z != 0)
            {
                model = glm::rotate(model, static_cast<float>(angle_rotate), glm::vec3(axis_rotate.x, axis_rotate.y, axis_rotate.z));
            }

            model = glm::scale(model, glm::vec3(1, 1, 1));

            m_simpleDepthShader->setMat4("model", model);


            renderSphere();

            const reactphysics3d::Transform& physics_sphere2_transform = physics_sphere2->getTransform();
            const reactphysics3d::Vector3& physics_sphere_position2 = physics_sphere2_transform.getPosition();
            auto physics_sphere2_rotate = physics_sphere2_transform.getOrientation();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(physics_sphere_position2.x, physics_sphere_position2.y, physics_sphere_position2.z));

            physics_sphere2_rotate.getRotationAngleAxis(angle_rotate, axis_rotate);
            if (axis_rotate.x != 0 ||
                axis_rotate.y != 0 ||
                axis_rotate.z != 0)
            {
                model = glm::rotate(model, static_cast<float>(angle_rotate), glm::vec3(axis_rotate.x, axis_rotate.y, axis_rotate.z));
            }
            model = glm::scale(model, glm::vec3(1, 1, 1));

            m_simpleDepthShader->setMat4("model", model);


            renderSphere();
        }

    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, this->m_width, this->m_height);
}

glm::mat4 TestRender::getLightSpaceMatrix()
{
    float near_plane = 1.0f, far_plane = 25.0f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

    glm::mat4 lightView = glm::lookAt(m_dirlight_position,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    return lightProjection * lightView;
}

void TestRender::renderLoadingAndBindData(float _currentFrame)
{
    if (m_cubemap_is_loaded && m_ourModel_textures_loaded) {
        m_is_loading = false;
    }

    if (m_cubemap_is_loaded && (!m_cubemap_bind)) {
        QTime t_start = QTime::currentTime();
        // Когда текстура загружена в память, мы её подключаем к openGL
        m_cubemapTexture = TextureLoader::BindDataToCubemap(m_data_array, m_width_array, m_height_array);
        m_cubemap_bind = true;
        QTime t_end = QTime::currentTime();
        Debug((ru("Кубическая карта:") + QString::number(t_start.msecsTo(t_end) / 1000.0)).toUtf8().constData());
    }

    if (m_ourModel_textures_loaded && (!m_ourModel_textures_bind)) {

        QTime t_start = QTime::currentTime();

        for (unsigned int i = 0; i < m_textures_to_loading.size(); i++) {
            bool skipped = false;
            for (unsigned int j = 0; j < i; j++)
            {
                QString name1 = m_textures_to_loading[i]->name;
                QString name2 = m_textures_to_loading[j]->name;

                if (name1 == name2) {
                    m_textures_to_loading[i]->id = m_textures_to_loading[j]->id;
                    skipped = true;
                }
            }
            if (!skipped) {
                m_textures_to_loading[i]->id = TextureLoader::BindDataToTexture(m_textures_to_loading[i]->data, m_ourModel_width_array[i], m_ourModel_height_array[i]);
            }
        }
        m_ourModel_textures_bind = true;

        QTime t_end = QTime::currentTime();
        Debug((ru("Загрузка модели:\t") + QString::number(t_start.msecsTo(t_end) / 1000.0)).toUtf8().constData());
    }

    // Показываем экран загрузки
    glm::vec3 color(0.0f, 120.0 / 255.0, 215.0 / 255.0);
    color = color * (sin(_currentFrame / 1000) * 0.5f + 0.5f);
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void TestRender::renderOldModel()
{
    m_ourShader->Use();


    m_ourShader->setVec3("lightPos", m_dirlight_position);
    m_ourShader->setVec3("viewPos", m_testWorld->camera()->position());
    m_ourShader->setFloat("material.shininess", 32.0f);



    m_ourShader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - m_dirlight_position);


    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(m_testWorld->camera()->zoom()), (float)this->m_width / (float)this->m_height, m_perspective_near, m_perspective_far);
    glm::mat4 view =  m_testWorld->camera()->getViewMatrix();
    m_ourShader->setMat4("projection", projection);
    m_ourShader->setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_ourModel_position); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(m_modelScale, m_modelScale, m_modelScale));	// it's a bit too big for our scene, so scale it down
    m_ourShader->setMat4("model", model);
    m_ourModel->Draw(*m_ourShader);
}

void TestRender::renderPbrSpheres()
{
    glm::mat4 projection = glm::perspective(glm::radians(m_testWorld->camera()->zoom()), (float)this->m_width / (float)this->m_height, m_perspective_near, m_perspective_far);
    glm::mat4 view =  m_testWorld->camera()->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    m_pbrShader->Use();
    m_pbrShader->setMat4("projection", projection);
    m_pbrShader->setMat4("view", view);
    m_pbrShader->setVec3("camPos", m_testWorld->camera()->position());



    const reactphysics3d::Transform& physics_sphere_transform = physics_sphere->getTransform();
    const reactphysics3d::Vector3& physics_sphere_position = physics_sphere_transform.getPosition();
    auto physics_sphere_rotate = physics_sphere_transform.getOrientation();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(physics_sphere_position.x, physics_sphere_position.y, physics_sphere_position.z)); // translate it down so it's at the center of the scene

    reactphysics3d::Vector3 axis_rotate;
    reactphysics3d::decimal angle_rotate;
    physics_sphere_rotate.getRotationAngleAxis(angle_rotate, axis_rotate);
    if (axis_rotate.x != 0 ||
        axis_rotate.y != 0 ||
        axis_rotate.z != 0)
    {
        model = glm::rotate(model, static_cast<float>(angle_rotate), glm::vec3(axis_rotate.x, axis_rotate.y, axis_rotate.z));
    }

    model = glm::scale(model, glm::vec3(1, 1, 1));	// it's a bit too big for our scene, so scale it down

    m_pbrShader->setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_brdfLUTTexture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_gold_albedo_texture_id);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_gold_norm_texture_id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_gold_metallic_texture_id);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_gold_roughness_texture_id);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_gold_ao_texture_id);

    renderSphere();

    const reactphysics3d::Transform& physics_sphere2_transform = physics_sphere2->getTransform();
    const reactphysics3d::Vector3& physics_sphere_position2 = physics_sphere2_transform.getPosition();
    auto physics_sphere2_rotate = physics_sphere2_transform.getOrientation();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(physics_sphere_position2.x, physics_sphere_position2.y, physics_sphere_position2.z)); // translate it down so it's at the center of the scene

    physics_sphere2_rotate.getRotationAngleAxis(angle_rotate, axis_rotate);
    if (axis_rotate.x != 0 ||
        axis_rotate.y != 0 ||
        axis_rotate.z != 0)
    {
        model = glm::rotate(model, static_cast<float>(angle_rotate), glm::vec3(axis_rotate.x, axis_rotate.y, axis_rotate.z));
    }
    model = glm::scale(model, glm::vec3(1, 1, 1));	// it's a bit too big for our scene, so scale it down

    m_pbrShader->setMat4("model", model);

    renderSphere();
}

void TestRender::updatePhysics()
{
    m_updateAccumulator += m_deltaTime;
    float timeStep = 1.0 / 60.0;

    while (m_updateAccumulator >= timeStep)
    {

        m_myFirstParticle->Tick(timeStep);
        m_myFirstParticleSystem->Tick(timeStep);

        our_physicsWorld->update(timeStep);

        for (int i = 0; i < m_testWorld->getAllObjects().size(); ++i)
        {
            m_testWorld->getAllObjects()[i]->tick(timeStep);
        }

        m_carObject->tick(timeStep);


        m_updateAccumulator -= timeStep;
    }
}

void TestRender::displayBuffer(GLuint _bufferId)
{
    static Shader* shader = nullptr;
    static GLuint vaoDebugTexturedRect = 0;

    if (shader == nullptr)
    {
        shader = new Shader("bufferShader.vs", "bufferShader.fs");

        GLfloat buffer_vertices[] = {
        0.35f, 0.25f, 0.0f, 0.0f,
        0.95f, 0.25f, 1.0f, 0.0f,
        0.95f, 0.95f, 1.0f, 1.0f,

        0.35f, 0.25f, 0.0f, 0.0f,
        0.95f, 0.95f, 1.0f, 1.0f,
        0.35f, 0.95f, 0.0f, 1.0f,
        };

        GLuint vboDebugTexturedRect;
        glGenVertexArrays(1, &vaoDebugTexturedRect);
        glGenBuffers(1, &vboDebugTexturedRect);
        glBindBuffer(GL_ARRAY_BUFFER, vboDebugTexturedRect);
        glBindVertexArray(vaoDebugTexturedRect);

        glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_vertices), buffer_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
    shader->Use();
    glBindTexture(GL_TEXTURE_2D, _bufferId);
    glBindVertexArray(vaoDebugTexturedRect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
}



void TestRender::do_movement()
{
    // Camera controls
    if (m_keys[Qt::Key_W])
        m_testWorld->camera()->processKeyboard(FORWARD, m_deltaTime);
    if (m_keys[Qt::Key_S])
        m_testWorld->camera()->processKeyboard(BACKWARD, m_deltaTime);
    if (m_keys[Qt::Key_A])
        m_testWorld->camera()->processKeyboard(LEFT, m_deltaTime);
    if (m_keys[Qt::Key_D])
        m_testWorld->camera()->processKeyboard(RIGHT, m_deltaTime);

    if (m_keys[Qt::Key_Shift])
        m_testWorld->camera()->processKeyboard(Up, m_deltaTime);
    if (m_keys[Qt::Key_Control])
        m_testWorld->camera()->processKeyboard(Down, m_deltaTime);
}



void TestRender::loadPBR()
{
    int part_w, part_h;
    auto data_particle_texture = TextureLoader::LoadDataFromFile("awesomeface.png", part_w, part_h);
    m_particle_texture_id = TextureLoader::BindDataToTexture(data_particle_texture, part_w, part_h);

    int gold_albedo_w, gold_albedo_h;
    auto data_gold_texture = TextureLoader::LoadDataFromFile("textures/" + current_pbr_material_name + "/albedo.png", gold_albedo_w, gold_albedo_h);
    m_gold_albedo_texture_id = TextureLoader::BindDataToTexture(data_gold_texture, gold_albedo_w, gold_albedo_h);

    int gold_norm_w, gold_norm_h;
    data_gold_texture = TextureLoader::LoadDataFromFile("textures/" + current_pbr_material_name + "/normal.png", gold_norm_w, gold_norm_h);
    m_gold_norm_texture_id = TextureLoader::BindDataToTexture(data_gold_texture, gold_norm_w, gold_norm_h);

    int gold_metallic_w, gold_metallic_h;
    data_gold_texture = TextureLoader::LoadDataFromFile("textures/" + current_pbr_material_name + "/metallic.png", gold_metallic_w, gold_metallic_h);
    m_gold_metallic_texture_id = TextureLoader::BindDataToTexture(data_gold_texture, gold_metallic_w, gold_metallic_h);

    int gold_roughness_w, gold_roughness_h;
    data_gold_texture = TextureLoader::LoadDataFromFile("textures/" + current_pbr_material_name + "/roughness.png", gold_roughness_w, gold_roughness_h);
    m_gold_roughness_texture_id = TextureLoader::BindDataToTexture(data_gold_texture, gold_roughness_w, gold_roughness_h);

    int gold_ao_w, gold_ao_h;
    data_gold_texture = TextureLoader::LoadDataFromFile("textures/" + current_pbr_material_name + "/ao.png", gold_ao_w, gold_ao_h);
    m_gold_ao_texture_id = TextureLoader::BindDataToTexture(data_gold_texture, gold_ao_w, gold_ao_h);


    m_pbrShader->Use();
    m_pbrShader->setInt("irradianceMap", 0);
    m_pbrShader->setInt("prefilterMap", 1);
    m_pbrShader->setInt("brdfLUT", 2);
    m_pbrShader->setInt("albedoMap", 3);
    m_pbrShader->setInt("normalMap", 4);
    m_pbrShader->setInt("metallicMap", 5);
    m_pbrShader->setInt("roughnessMap", 6);
    m_pbrShader->setInt("aoMap", 7);
    glUseProgram(0);
}

void TestRender::initFloorModel()
{
    GLfloat floor_vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &m_floorVAO);
    glGenBuffers(1, &m_floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_floorVBO);
    glBindVertexArray(m_floorVAO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    //normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

void TestRender::loadModel()
{
    // load models
    // -----------
    static QMutex loadModelMutex;
    static QMutex loadTextureMutex;

    QMutexLocker modelLocker(&loadModelMutex);
    m_ourModel = new Model("objects/backpack/backpack.obj");
    modelLocker.unlock();


    m_modelScale = 1.0f;
    m_ourModel_position = glm::vec3(-5, 3, 0);

    for (unsigned int i = 0; i < m_ourModel->meshes.size(); i++) {
        QVector<raw_Texture>* T = &(m_ourModel->meshes[i].textures);
        for (unsigned int j = 0; j < T->size(); j++) {
            m_textures_to_loading.push_back(&((*T)[j]));
        }
    }

    std::thread(
        [&]()
        {
            for (unsigned int i = 0; i < m_textures_to_loading.size(); i++) {
                m_ourModel_width_array.push_back(0);
                m_ourModel_height_array.push_back(0);
                QString str = m_textures_to_loading[i]->name;

                bool skip = false;
                for (unsigned int j = 0; j < m_ourModel->textures_loaded.size(); j++)
                {
                    if (m_ourModel->textures_loaded[j].name == str)
                    {
                        m_textures_to_loading[i]->data = m_ourModel->textures_loaded[j].data;
                        m_ourModel_width_array[i] = m_ourModel_width_array[j];
                        m_ourModel_height_array[i] = m_ourModel_height_array[j];
                        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                        break;
                    }
                }
                if (!skip) {
                    QString name = m_textures_to_loading[i]->path + '/' + str;


                    QMutexLocker textureLocker(&loadTextureMutex);
                    m_textures_to_loading[i]->data = SOIL_load_image(qUtf8Printable(name), &(m_ourModel_width_array[i]), &(m_ourModel_height_array[i]), 0, SOIL_LOAD_RGBA);

                    if (m_textures_to_loading[i]->data == nullptr)
                    {
                        qDebug() << "Ошибка при " << name;
                    }
                    textureLocker.unlock();

                    m_ourModel->textures_loaded.push_back(*(m_textures_to_loading[i]));
                }
            }
            m_ourModel_textures_loaded = true;

        }
    ).detach();

}

void TestRender::initSkyboxModel()
{
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &m_skyboxVBO);
    glBindVertexArray(m_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



    std::thread(
        [&]()
        {
            QVector<QString> faces
            {
                "textures/skybox/right.jpg",
                "textures/skybox/left.jpg",
                "textures/skybox/top.jpg",
                "textures/skybox/bottom.jpg",
                "textures/skybox/front.jpg",
                "textures/skybox/back.jpg"
            };

            for (unsigned int i = 0; i < faces.size(); i++) {
                m_width_array.push_back(0);
                m_height_array.push_back(0);
                m_data_array.push_back(TextureLoader::LoadDataFromFile(faces[i], m_width_array[i], m_height_array[i]));
            }
            m_cubemap_is_loaded = true;
        }
    ).detach();
}

void TestRender::initDepthMapFBO()
{
    // Создадим буфер глубины для теней
    glGenFramebuffers(1, &m_depthMapFBO);

    // Создадим текстуру для буфера

    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TestRender::initRaceMap()
{
    QList<glm::vec3> wallPos(
                {
                    glm::vec3(0.5, -1.25, 2),
                    glm::vec3(2, -1.25, 1.5),
                    glm::vec3(3, -1.25, 1),
                    glm::vec3(4, -1.25, -0.5),
                    glm::vec3(2, -1.25, 0),
                    glm::vec3(3, -1.25, -0.5),
                    glm::vec3(3, -1.25, -2),
                    glm::vec3(2, -1.25, -1.5),
                    glm::vec3(1, -1.25, -2),
                    glm::vec3(3, -1.25, -3),
                    glm::vec3(5, -1.25, -0.5),
                    glm::vec3(4, -1.25, 2),
                    glm::vec3(3, -1.25, 3),
                    glm::vec3(2.5, -1.25, 3),
                    glm::vec3(2.5, -1.25, 4),
                    glm::vec3(2, -1.25, 3.5),
                    glm::vec3(1, -1.25, 3.5),
                    glm::vec3(2.5, -1.25, 5),
                    glm::vec3(4, -1.25, 4),
                    glm::vec3(5, -1.25, 3),
                    glm::vec3(6, -1.25, 2),
                    glm::vec3(5.5, -1.25, 0),
                    glm::vec3(6.5, -1.25, 1),
                    glm::vec3(6, -1.25, -1),
                    glm::vec3(7, -1.25, -1),
                    glm::vec3(6.5, -1.25, -3),
                    glm::vec3(6, -1.25, -4),
                    glm::vec3(5.5, -1.25, -2),
                    glm::vec3(4, -1.25, -4),
                    glm::vec3(4, -1.25, -5),
                    glm::vec3(5, -1.25, -3.5),
                    glm::vec3(3, -1.25, -3.5),
                    glm::vec3(2, -1.25, -4.5),
                    glm::vec3(-0.5, -1.25, -4),
                    glm::vec3(0, -1.25, -3),
                    glm::vec3(-3, -1.25, -3),
                    glm::vec3(-1, -1.25, -2),
                    glm::vec3(-2, -1.25, -1),
                    glm::vec3(-1.5, -1.25, -3),
                    glm::vec3(-2.5, -1.25, -2),
                    glm::vec3(-2.5, -1.25, 0),
                    glm::vec3(-1.5, -1.25, 1),
                    glm::vec3(-3, -1.25, 2.5),
                    glm::vec3(-2, -1.25, 2.5),
                    glm::vec3(-1, -1.25, 5),
                    glm::vec3(-1, -1.25, 4),
                    glm::vec3(0, -1.25, 3),
                    glm::vec3(-1, -1.25, 2.5)
                });
    QList<float> wallSize(
                {
                    3,
                    1,
                    2,
                    3,
                    2,
                    1,
                    2,
                    1,
                    2,
                    4,
                    5,
                    2,
                    2,
                    1,
                    1,
                    1,
                    3,
                    3,
                    2,
                    2,
                    2,
                    1,
                    1,
                    2,
                    4,
                    1,
                    2,
                    1,
                    2,
                    4,
                    1,
                    1,
                    1,
                    5,
                    2,
                    2,
                    2,
                    2,
                    1,
                    1,
                    1,
                    1,
                    5,
                    3,
                    4,
                    2,
                    2,
                    1
                });
    QList<float> wallRotate(
                {
                    0,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    3.14f/2,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    0,
                    3.14f/2,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    0,
                    3.14f/2,
                    3.14f/2,
                    0,
                    3.14f/2,
                    0,
                    0,
                    0,
                    3.14f/2,
                    3.14f/2,
                    3.14f/2,
                    0,
                    3.14f/2,
                    3.14f/2,
                    3.14f/2,
                    3.14f/2,
                    0,
                    0,
                    0,
                    0,
                    3.14f/2,
                    3.14f/2,
                    0,
                    0,
                    0,
                    3.14f/2
                });


    for (int i = 0; i < wallPos.size(); ++i)
    {
        wallPos[i].x *= 15;
        wallPos[i].z *= 15;
        wallSize[i] *= 15;
        FuryBoxObject* wall = new FuryBoxObject(m_testWorld,
                                                wallPos[i],
                                                glm::vec3(wallSize[i], 2.5, 0.5),
                                                glm::vec3(0, wallRotate[i], 0));
        wall->Setup_physics(reactphysics3d::BodyType::STATIC);
        wall->setName("raceWall");
        wall->physicsBody()->setUserData(wall);
        wall->setTextureName("raceWall");
        m_testWorld->addObject(wall);

        if (!m_materialManager->materialExist("raceWallMaterial"))
        {
            FuryMaterial* mat = m_materialManager->createMaterial("raceWallMaterial");
            mat->setDiffuseTexture("Diffuse_raceWall");
        }

        wall->setMaterialName("raceWallMaterial");
    }
}




// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        QVector<glm::vec3> positions;
        QVector<glm::vec2> uv;
        QVector<glm::vec3> normals;
        QVector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = (unsigned int)indices.size();

        QVector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
