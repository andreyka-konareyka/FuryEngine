#include "TestRender.h"


#include "FuryLogger.h"
#include "FuryScript.h"
#include "FuryPbrMaterial.h"
#include "FuryRussianLocalKeyMapper.h"

#include <reactphysics3d/reactphysics3d.h>
#include <stb_image.h>

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QMutex>
#include <QString>
#include <QThread>
#include <QJsonArray>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMutexLocker>
#include <QJsonDocument>

const bool NEED_DRAW_SHADOW = true;
const bool NEED_DEBUG_SHADOW_MAP = false;
#define NEED_LEARN 0


const unsigned int SHADOW_WIDTH = 1024 * 4, SHADOW_HEIGHT = 1024 * 4;
QVector<QString> pbr_material_names{ "pbr_gold", "pbr_grass", "pbr_iron", "pbr_plastic", "pbr_wall", "pbr_asphalt" };
QString current_pbr_material_name = pbr_material_names[0];


void renderCube();
void renderQuad();
unsigned int loadTexture(const QString& path);




TestRender::TestRender(QWidget *_parent) :
    QOpenGLWidget(_parent),
    m_textureManager(FuryTextureManager::createInstance()),
    m_modelManager(FuryModelManager::createInstance()),
    m_materialManager(FuryMaterialManager::createInstance()),
    m_needDebugRender(false),
    m_updateAccumulator(0),

    #if NEED_LEARN == 1
    m_learnScript(new FuryScript),
    #else
    m_learnScript(nullptr),
    #endif

    m_learnSpeed(1),
    m_russianKeyMapper(new FuryRussianLocalKeyMapper)
{
    Debug(ru("Создание рендера"));

    connect(this, &QOpenGLWidget::frameSwapped,
            this, &TestRender::updateGL, Qt::QueuedConnection);
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

    m_testWorld->physicsWorld()->setEventListener(nullptr);

    if (m_eventListener != nullptr)
    {
        delete m_eventListener;
        m_eventListener = nullptr;
    }

    if (m_testWorld != nullptr)
    {
        delete m_testWorld;
        m_testWorld = nullptr;
    }

    Debug(ru("Удаление менеджера материалов..."));
    FuryMaterialManager::deleteInstance();

    Debug(ru("Остановка текстурного менеджера..."));
    m_textureManager->stopLoopAndWait();
    FuryTextureManager::deleteInstance();

    Debug(ru("Остановка менеджера моделей..."));
    m_modelManager->stopLoopAndWait();
    FuryModelManager::deleteInstance();
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
        QString test = ru("FPS: %1; Эквивалентный FPS: %2")
                .arg(1000.0 / lastTime.msecsTo(endTime))
                .arg(1000.0 / lastTime.msecsTo(endTime) * m_learnSpeed);
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

void TestRender::setCarCameraPos(float _x, float _y)
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

void TestRender::saveLearnModel()
{
#if NEED_LEARN == 1
    m_learnScript->saveModel();
#endif
}

void TestRender::saveScoreList()
{
    saveLearnModel();

    QFile file("scoresList.txt");

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);

        foreach (float score, m_scoreList)
        {
            stream << score << '\n';
        }

        file.close();
        qDebug() << "score list saved";
    }
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
    int keyCode = m_russianKeyMapper->mapLocalKeyToLatin(_event->key());
    if (keyCode == 0)
    {
        keyCode = _event->key();
    }

    m_keys[keyCode] = true;

    if (m_testWorld->camera() == m_cameras[1])
    { // Если камера машины
        m_carObject->keyPressEvent(keyCode);
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
    int keyCode = m_russianKeyMapper->mapLocalKeyToLatin(_event->key());
    if (keyCode == 0)
    {
        keyCode = _event->key();
    }

    m_keys[keyCode] = false;

    if (m_testWorld->camera() == m_cameras[1])
    { // Если камера машины
        m_carObject->keyReleaseEvent(keyCode);
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
    m_textureManager->addTexture("textures/FuryEngine_logo_for_dark.png", "Logo");
    init();
}

void TestRender::init() {
    Debug(ru("Версия OpenGL: ") + ru(glGetString(GL_VERSION)));
    Debug(ru("Количество потоков: ") + QString::number(QThread::idealThreadCount()));
    own_physicsCommon = new reactphysics3d::PhysicsCommon;

    m_testWorld = new FuryWorld(own_physicsCommon);
    m_eventListener = new FuryEventListener;
    m_testWorld->physicsWorld()->setEventListener(m_eventListener);

    m_modelManager->addModel("objects/cube/cube.obj", "cube");
    m_modelManager->addModel("objects/sphere/sphere.obj", "sphere");

    m_pbrShader = new Shader("shaders/pbr/2.2.2.pbr.vs", "shaders/pbr/2.2.2.pbr.fs");

    m_sunVisualBox = new FurySphereObject(m_testWorld, m_dirlight_position);
    m_sunVisualBox->setName("sunVisualBox");
    m_sunVisualBox->setShader(m_pbrShader);
    m_testWorld->addObject(m_sunVisualBox);
    // Camera
    m_cameras.push_back(new Camera(glm::vec3(0.0f, 10.0f, 40.0f)));
    m_cameras.push_back(new Camera(glm::vec3(0.0f, 30.0f, 60.0f)));
    m_testWorld->setCamera(m_cameras[0]);


    //
    // МАШИНА
    //

    m_carObject = new CarObject(m_testWorld, glm::vec3(0, -0.5, 30), m_pbrShader);
    m_carObject->Setup_physics(reactphysics3d::BodyType::DYNAMIC);
    m_testWorld->addObject(m_carObject);

    const QVector<FuryObject*>& tempObjectsForDraw = m_carObject->objectsForDraw();
    for (int i = 0; i < tempObjectsForDraw.size(); ++i)
    {
        m_testWorld->addObject(tempObjectsForDraw[i]);
    }

    m_eventListener->setCarObject(m_carObject);

    // Большой пол
    m_bigFloor = new FuryBoxObject(m_testWorld, glm::vec3(0, -3, 0), 500, 1, 500);
    m_bigFloor->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(m_bigFloor);

    FuryPbrMaterial* asphaltPbrMaterial = m_materialManager->createPbrMaterial("asphaltPbrMaterial");
    asphaltPbrMaterial->setAlbedoTexture("asphaltPbr_albedo");
    asphaltPbrMaterial->setNormalTexture("asphaltPbr_normal");
    asphaltPbrMaterial->setMetallicTexture("asphaltPbr_metallic");
    asphaltPbrMaterial->setRoughnessTexture("asphaltPbr_roughness");
    asphaltPbrMaterial->setAoTexture("asphaltPbr_ao");

    m_bigFloor->setMaterialName("asphaltPbrMaterial");
    m_bigFloor->setTextureScales(glm::vec2(4, 3) * 100.0f);


    m_particleShader = new Shader("particle.vs", "particle.fs");
    m_backgroundShader = new Shader("shaders/pbr/2.2.2.background.vs", "shaders/pbr/2.2.2.background.fs");
    m_simpleDepthShader = new Shader("simpleDepthShader.vs", "simpleDepthShader.fs");

    m_bigFloor->setShader(m_pbrShader);



    FurySphereObject* testSphere = new FurySphereObject(m_testWorld, glm::vec3(-3, 10, 0), 1);
    testSphere->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(testSphere);

    FuryPbrMaterial* firstPbrMaterial = m_materialManager->createPbrMaterial("firstPbrMaterial");
    firstPbrMaterial->setAlbedoTexture("testPbr_albedo");
    firstPbrMaterial->setNormalTexture("testPbr_normal");
    firstPbrMaterial->setMetallicTexture("testPbr_metallic");
    firstPbrMaterial->setRoughnessTexture("testPbr_roughness");
    firstPbrMaterial->setAoTexture("testPbr_ao");
    testSphere->setMaterialName("firstPbrMaterial");
    testSphere->setShader(m_pbrShader);

    testSphere = new FurySphereObject(m_testWorld, glm::vec3(-6, 10, 0), 1);
    testSphere->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(testSphere);

    FuryPbrMaterial* plasticPbrMaterial = m_materialManager->createPbrMaterial("plasticPbrMaterial");
    plasticPbrMaterial->setAlbedoTexture("plasticPbr_albedo");
    plasticPbrMaterial->setNormalTexture("plasticPbr_normal");
    plasticPbrMaterial->setMetallicTexture("plasticPbr_metallic");
    plasticPbrMaterial->setRoughnessTexture("plasticPbr_roughness");
    plasticPbrMaterial->setAoTexture("plasticPbr_ao");
    testSphere->setMaterialName("plasticPbrMaterial");
    testSphere->setShader(m_pbrShader);

    testSphere = new FurySphereObject(m_testWorld, glm::vec3(6, 10, 0), 1);
    testSphere->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(testSphere);

    FuryPbrMaterial* ironPbrMaterial = m_materialManager->createPbrMaterial("ironPbrMaterial");
    ironPbrMaterial->setAlbedoTexture("ironPbr_albedo");
    ironPbrMaterial->setNormalTexture("ironPbr_normal");
    ironPbrMaterial->setMetallicTexture("ironPbr_metallic");
    ironPbrMaterial->setRoughnessTexture("ironPbr_roughness");
    ironPbrMaterial->setAoTexture("ironPbr_ao");
    testSphere->setMaterialName("ironPbrMaterial");
    testSphere->setShader(m_pbrShader);

    testSphere = new FurySphereObject(m_testWorld, glm::vec3(3, 10, 0), 1);
    testSphere->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(testSphere);

    FuryPbrMaterial* grassPbrMaterial = m_materialManager->createPbrMaterial("grassPbrMaterial");
    grassPbrMaterial->setAlbedoTexture("grassPbr_albedo");
    grassPbrMaterial->setNormalTexture("grassPbr_normal");
    grassPbrMaterial->setMetallicTexture("grassPbr_metallic");
    grassPbrMaterial->setRoughnessTexture("grassPbr_roughness");
    grassPbrMaterial->setAoTexture("grassPbr_ao");
    testSphere->setMaterialName("grassPbrMaterial");
    testSphere->setShader(m_pbrShader);

    testSphere = new FurySphereObject(m_testWorld, glm::vec3(0, 10, 0), 1);
    testSphere->Setup_physics(reactphysics3d::BodyType::STATIC);
    m_testWorld->addObject(testSphere);

    FuryPbrMaterial* goldPbrMaterial = m_materialManager->createPbrMaterial("goldPbrMaterial");
    goldPbrMaterial->setAlbedoTexture("m_gold_albedo_texture_id");
    goldPbrMaterial->setNormalTexture("m_gold_norm_texture_id");
    goldPbrMaterial->setMetallicTexture("m_gold_metallic_texture_id");
    goldPbrMaterial->setRoughnessTexture("m_gold_roughness_texture_id");
    goldPbrMaterial->setAoTexture("m_gold_ao_texture_id");
    testSphere->setMaterialName("goldPbrMaterial");
    testSphere->setShader(m_pbrShader);


    // PBR-материалы
    loadPBR();



    m_smoke_texture_id = loadTexture("textures/smoke_ver2.png");
    m_particle_texture_id = loadTexture("awesomeface.png");


    glm::vec3 part_pos(5, 0, 0);
    double part_scale = 0.15;
    glm::vec3 part_speed(0, 0.5, 0);
    glm::vec4 part_color(1, 1, 0, 1);
    m_myFirstParticle = new Particle(part_pos, part_scale, part_speed, part_color, m_particle_texture_id, 10, m_particleShader);

    glm::vec3 part_sys_pos(0, 2, 0);
    m_myFirstParticleSystem = new ParticleSystem(part_sys_pos, m_smoke_texture_id, 1000);





    initSkyboxModel();

    initDepthMapFBO();
    loadRaceMapFromJson();

    m_textureManager->addTexture("textures/box_texture_5x5.png", "defaultBoxTexture");
    m_textureManager->addTexture("textures/box_texture3_orig.png", "Diffuse_numbersBoxTexture");
    m_textureManager->addTexture("textures/carBody.png", "carBody");
    m_textureManager->addTexture("textures/rayCastBall.png", "Diffuse_rayCastBall");
    m_textureManager->addTexture("textures/greenRay.png", "Diffuse_greenRay");
    m_textureManager->addTexture("textures/box_texture2.png", "Diffuse_raceWall");
    m_textureManager->addTexture("textures/asphalt.png", "Diffuse_asphalt");
    m_textureManager->addTexture("textures/redCheckBox.png", "redCheckBox");
    m_textureManager->addTexture("textures/greenCheckBox.png", "greenCheckBox");

//    m_modelManager->addModel("objects/car2/LOD2.obj", "backpack2LOD2");
    m_modelManager->addModel("objects/car2/car.obj", "backpack2");
//    m_modelManager->addModel("objects/car1/car.FBX", "backpack2");

    {
        // initShaderInform

        QList<Shader*> shaders({
                                   m_pbrShader,
                                   FuryBoxObject::defaultShader(),
                                   FurySphereObject::defaultShader()
                               });

        for (Shader* shader : shaders)
        {
            shader->Use();
            shader->setVec3("lightColors[0]", 300.0f, 300.0f, 300.0f);
            shader->setVec3("lightColors[1]", 300.0f, 300.0f, 300.0f);
            shader->setVec3("lightColors[2]", 300.0f, 300.0f, 300.0f);
            shader->setVec3("lightColors[3]", 300.0f, 300.0f, 300.0f);
        }
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
            renderLoading(currentFrame);
            displayLogo();
            return;
        }

        /*
        =======================================================
        ===================  Обычный режим  ===================
        =======================================================
        */

        m_dirlight_position.x = sin(currentFrame / 1000 / 4) * 10;
        m_dirlight_position.z = cos(currentFrame / 1000 / 4) * 10;



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

        for (int i = 0; i < m_learnSpeed; ++i)
        {
            updatePhysics();
        }


        m_cameras[1]->setPosition(m_carObject->cameraPosition());
        m_cameras[1]->setFront(m_carObject->cameraViewPoint() - m_carObject->cameraPosition());

        glm::vec3 tempPosition = m_dirlight_position;
        tempPosition *= 3;
        tempPosition += m_testWorld->camera()->position();
        m_sunVisualBox->setPosition(tempPosition);


        // 1. сначала рисуем карту глубины
        if (NEED_DRAW_SHADOW)
        {
            renderDepthMap();
        }
        glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();





        // 2. рисуем сцену как обычно с тенями (используя карту глубины)

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glm::vec4 planes[6];
        const glm::mat4& projection = m_testWorld->camera()->getPerspectiveMatrix(m_width, m_height, m_perspective_near, m_perspective_far);
        const glm::mat4& view =  m_testWorld->camera()->getViewMatrix();

        {
            glm::mat4 m = glm::transpose(projection * view);
            planes[0] = m[3] + m[0];
            planes[1] = m[3] - m[0];
            planes[2] = m[3] + m[1];
            planes[3] = m[3] - m[1];
            planes[4] = m[3] + m[2];
            planes[5] = m[3] - m[2];
        }

        const QVector<FuryObject*>& testWorldObjects = m_testWorld->getObjects();
        QVector<QPair<FuryObject*, FuryMesh*>> meshesForRender1;
        QVector<QPair<FuryObject*, FuryMesh*>> meshesForRender2;

        foreach (FuryObject* obj, testWorldObjects)
        {
            FuryModel* model = m_modelManager->modelByName(obj->modelName());
            FuryPbrMaterial* objMat = nullptr;
            if (m_materialManager->materialExist(obj->materialName()))
            {
                objMat = dynamic_cast<FuryPbrMaterial*>(m_materialManager->materialByName(obj->materialName()));
            }

            foreach (FuryMesh* mesh, model->meshes())
            {
                float opacity = 1;

                if (objMat == nullptr)
                {
                    FuryMaterial* material = m_materialManager->materialByName(mesh->materialName());
                    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(material); pbr != nullptr)
                    {
                        opacity = pbr->opacity();
                    }
                    else if (material != nullptr)
                    {
                        opacity = material->opacity();
                    }
                }
                else
                {
                    opacity = objMat->opacity();
                }

                if (opacity >= 0.95)
                {
                    meshesForRender1.append(qMakePair(obj, mesh));
                }
                else
                {
                    meshesForRender2.append(qMakePair(obj, mesh));
                }
            }
        }

        for (QPair<FuryObject*, FuryMesh*>& pair : meshesForRender1)
        {
            FuryObject* obj = pair.first;
            FuryMesh* mesh = pair.second;
            Shader* shader = obj->shader();

            if (!m_needDebugRender)
            {
                if (obj->name() == "rayCastBall")
                {
                    continue;
                }
            }

            shader->Use();
            shader->setVec3("viewPos", m_testWorld->camera()->position());
            shader->setFloat("material.shininess", 128.0f); // 32.0 - default
            shader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - m_dirlight_position);


            // view/projection transformations
            shader->setMat4("projection", projection);
            shader->setMat4("view", view);

            shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            shader->setVec2("textureScales", obj->textureScales());


            {
                shader->setVec3("camPos", m_testWorld->camera()->position());

                glm::vec3 tempPosition = m_dirlight_position;
                tempPosition *= 3;

                shader->setVec3("lightPositions[0]", tempPosition);
                shader->setVec3("lightPositions[1]", tempPosition);
                shader->setVec3("lightPositions[2]", tempPosition);
                shader->setVec3("lightPositions[3]", tempPosition);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, m_brdfLUTTexture);
                glActiveTexture(GL_TEXTURE8);
                glBindTexture(GL_TEXTURE_2D, m_depthMap);
            }

            glm::mat4 modelMatrix = obj->getOpenGLTransform();
            modelMatrix = glm::scale(modelMatrix, obj->scales());
            modelMatrix *= obj->modelTransform();
            shader->setMat4("model", modelMatrix);
            shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));

            FuryMaterial* material = nullptr;
            if (m_materialManager->materialExist(obj->materialName()))
            {
                material = m_materialManager->materialByName(obj->materialName());
            }

            mesh->draw(shader, material);


            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);

            if (obj->name() == "rayCastBall")
            { // Отрисовка лучей у машины
                glm::vec3 direct = m_carObject->getPosition() - obj->getPosition();
                direct /= obj->scales().x;
                glBegin(GL_LINES);
                glVertex3d(0, 0, 0);
                glVertex3d(direct.x, direct.y, direct.z);
                glEnd();
            }
        }

//        for (int i = 0; i < testWorldObjects.size(); ++i)
//        {
//            continue;
//            FuryObject* renderObject = testWorldObjects[i];
//            Shader* shader = renderObject->shader();

//            {
//                if (!m_needDebugRender)
//                {
//                    if (renderObject->name() == "rayCastBall")
//                    {
//                        continue;
//                    }
//                }


//                if (renderObject->name() == "carBody") // Физическая коробка вокруг машины
//                {
////                    continue;
//                }

//                if (renderObject->name() != "rayCastBall")
//                {
//                    bool result = true;
//                    const glm::vec3& minp = renderObject->getPosition();
//                    const glm::vec3& scales = renderObject->scales();
//                    float maxScale = std::max(std::max(scales.x, scales.y), scales.z);
//                    float radius = m_modelManager->modelByName(renderObject->modelName())->modelRadius();
//                    radius *= maxScale;

//                    for (int i = 0; i < 6; i++)
//                    {
//                        float distance = glm::dot(planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f));

//                        if (distance < -(radius * 1))
//                        {
//                            result = false;
//                            break;
//                        }
//                    }

//                    if (!result)
//                    {
//                        continue;
//                    }
//                }


        /*
        =======================================================
        =====================  Скайбокс  ======================
        =======================================================
        */
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        m_backgroundShader->Use();
        m_backgroundShader->setMat4("projection", projection);
        m_backgroundShader->setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
        // skybox cube
        glBindVertexArray(m_skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default




        // ##################################
        // ##########   Particle   ##########
        // ##################################




        /*
            Start Draw particle
        */
        {
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

        {
            QList<QPair<float, QPair<FuryObject*, FuryMesh*>>> sorted;
            for (unsigned int i = 0; i < meshesForRender2.size(); i++){
                float distance = glm::length(m_testWorld->camera()->position() - meshesForRender2[i].first->getPosition());
                sorted.append(qMakePair(distance, meshesForRender2[i]));
            }

            std::sort(sorted.begin(), sorted.end(), [](auto& p1, auto& p2){return p1.first < p2.first;});

            for (int i = sorted.size() - 1; i >= 0; --i)
            {
                QPair<FuryObject*, FuryMesh*>& pair = sorted[i].second;
                FuryObject* obj = pair.first;
                FuryMesh* mesh = pair.second;
                Shader* shader = obj->shader();

                if (!m_needDebugRender)
                {
                    if (obj->name() == "rayCastBall")
                    {
                        continue;
                    }

                    if (obj->name().startsWith("Trigger"))
                    {
                        continue;
                    }
                }

                shader->Use();
                shader->setVec3("viewPos", m_testWorld->camera()->position());
                shader->setFloat("material.shininess", 128.0f); // 32.0 - default
                shader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - m_dirlight_position);


                // view/projection transformations
                shader->setMat4("projection", projection);
                shader->setMat4("view", view);

                shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
                shader->setVec2("textureScales", obj->textureScales());


                {
                    shader->setVec3("camPos", m_testWorld->camera()->position());

                    glm::vec3 tempPosition = m_dirlight_position;
                    tempPosition *= 3;

                    shader->setVec3("lightPositions[0]", tempPosition);
                    shader->setVec3("lightPositions[1]", tempPosition);
                    shader->setVec3("lightPositions[2]", tempPosition);
                    shader->setVec3("lightPositions[3]", tempPosition);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, m_brdfLUTTexture);
                    glActiveTexture(GL_TEXTURE8);
                    glBindTexture(GL_TEXTURE_2D, m_depthMap);
                }

                glm::mat4 modelMatrix = obj->getOpenGLTransform();
                modelMatrix = glm::scale(modelMatrix, obj->scales());
                modelMatrix *= obj->modelTransform();
                shader->setMat4("model", modelMatrix);
                shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));

                FuryMaterial* material = nullptr;
                if (m_materialManager->materialExist(obj->materialName()))
                {
                    material = m_materialManager->materialByName(obj->materialName());
                }

                mesh->draw(shader, material);


                glBindVertexArray(0);
                glActiveTexture(GL_TEXTURE0);

                if (obj->name() == "rayCastBall")
                { // Отрисовка лучей у машины
                    glm::vec3 direct = m_carObject->getPosition() - obj->getPosition();
                    direct /= obj->scales().x;
                    glBegin(GL_LINES);
                    glVertex3d(0, 0, 0);
                    glVertex3d(direct.x, direct.y, direct.z);
                    glEnd();
                }
            }

            sorted.clear();
        }


        if (NEED_DEBUG_SHADOW_MAP)
        {
            displayBuffer(m_depthMap);
        }
    }
}

void TestRender::createPBRTextures()
{
    glDisable(GL_CULL_FACE);
    glClearColor(1, 1, 1, 1.0f);

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

        // pbr: load the HDR environment map
        // ---------------------------------
        stbi_set_flip_vertically_on_load(true);
        int width, height;
        float *data = stbi_loadf("textures/hdr/newport_loft3.hdr", &width, &height, 0, STBI_rgb);
        unsigned int hdrTexture;
        if (data)
        {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load HDR image." << std::endl;
        }

        // pbr: setup cubemap to render to and attach to framebuffer
        // ---------------------------------------------------------
        glGenTextures(1, &m_envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
        // ----------------------------------------------------------------------------------------------
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        // pbr: convert HDR equirectangular environment map to cubemap equivalent
        // ----------------------------------------------------------------------
        Shader equirectangularToCubemapShader("shaders/pbr/2.2.2.cubemap.vs", "shaders/pbr/2.2.2.equirectangular_to_cubemap.fs");
        equirectangularToCubemapShader.Use();
        equirectangularToCubemapShader.setInt("equirectangularMap", 0);
        equirectangularToCubemapShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            equirectangularToCubemapShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

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
        Shader irradianceShader("shaders/pbr/2.2.2.cubemap.vs", "shaders/pbr/2.2.2.irradiance_convolution.fs");
        irradianceShader.Use();
        irradianceShader.setInt("environmentMap", 0);
        irradianceShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

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

        // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
        // --------------------------------------------------------------------------------
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
        Shader prefilterShader("shaders/pbr/2.2.2.cubemap.vs", "shaders/pbr/2.2.2.prefilter.fs");
        prefilterShader.Use();
        prefilterShader.setInt("environmentMap", 0);
        prefilterShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
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

        Shader brdfShader("shaders/pbr/2.2.2.brdf.vs", "shaders/pbr/2.2.2.brdf.fs");
        brdfShader.Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_BLEND);
        renderQuad();
        glEnable(GL_BLEND);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glEnable(GL_CULL_FACE);
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


    const QVector<FuryObject*>& testWorldObjects = m_testWorld->getObjects();
    QVector<QPair<FuryObject*, FuryMesh*>> meshesForRender;

    foreach (FuryObject* obj, testWorldObjects)
    {
        FuryModel* model = m_modelManager->modelByName(obj->modelName());

        foreach (FuryMesh* mesh, model->meshes())
        {
            meshesForRender.append(qMakePair(obj, mesh));
        }
    }

    for (QPair<FuryObject*, FuryMesh*>& pair : meshesForRender)
    {
        FuryObject* obj = pair.first;
        FuryMesh* mesh = pair.second;

        if (obj->name().startsWith("Trigger"))
        {
            continue;
        }
        if (obj->name() == "sunVisualBox")
        {
            continue;
        }
        if (!m_needDebugRender)
        {
            if (obj->name() == "rayCastBall")
            {
                continue;
            }
        }

        glm::mat4 modelMatrix = obj->getOpenGLTransform();
        modelMatrix = glm::scale(modelMatrix, obj->scales());
        modelMatrix *= obj->modelTransform();
        m_simpleDepthShader->setMat4("model", modelMatrix);


        mesh->drawShadowMap();


        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }


    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, this->m_width, this->m_height);
}

glm::mat4 TestRender::getLightSpaceMatrix()
{
    glm::mat4 lightProjection = glm::ortho(-m_shadowViewSize, m_shadowViewSize,
                                           -m_shadowViewSize, m_shadowViewSize,
                                           m_shadowNear, m_shadowPlane);

    glm::vec3 tempDirLight = glm::normalize(m_dirlight_position);
    tempDirLight *= m_shadowCamDistance;

    glm::vec3 cameraPos = m_testWorld->camera()->position();
    glm::vec3 cameraFront = m_testWorld->camera()->front();
    cameraFront *= 15;

    glm::vec3 shadowCameraView = cameraPos + cameraFront;
    shadowCameraView.y = 0;
    glm::vec3 shadowCameraPos = shadowCameraView + tempDirLight;

    glm::mat4 lightView = glm::lookAt(shadowCameraPos,
                                      shadowCameraView,
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    return lightProjection * lightView;
}

void TestRender::renderLoading(float _currentFrame)
{
    Q_UNUSED(_currentFrame);
    static QTime startTime = QTime::currentTime();

    if (startTime.msecsTo(QTime::currentTime()) > m_loadingOvertime * 1000)
    {
        m_is_loading = false;
    }

    // Показываем экран загрузки
    glm::vec3 color(7, 8, 10);
    color /= 255;
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void TestRender::updatePhysics()
{
    float timeStep = 1.0f / 60.0f;

#if NEED_LEARN == 0
    m_updateAccumulator += m_deltaTime;

    while (m_updateAccumulator >= timeStep)
    {
#endif

    // Выполняем 2 тика по 1/60 секунды. Для ИИ получается 1 тик в 1/30 секунды
//    int iCount = (m_learnSpeed == 1) ? 1 : 2;
//    for (int i = 0; i < iCount; ++i)
    {
        m_myFirstParticle->Tick(timeStep);
        m_myFirstParticleSystem->Tick(timeStep);

        m_testWorld->physicsWorld()->update(timeStep);

        for (int i = 0; i < m_testWorld->getObjects().size(); ++i)
        {
            m_testWorld->getObjects()[i]->tick(timeStep);
        }
    }

#if NEED_LEARN == 0
        m_updateAccumulator -= timeStep;
    }
#endif



#if NEED_LEARN == 1

    QVector<float> observation = m_carObject->getRays();

    glm::vec3 speed = m_carObject->getSpeed();
    speed /= 23;
    observation.append(speed.x);
    observation.append(speed.y);
    observation.append(speed.z);

    glm::vec3 angularSpeed = m_carObject->getAngularSpeed();
    angularSpeed /= 2;
    observation.append(angularSpeed.x);
    observation.append(angularSpeed.y);
    observation.append(angularSpeed.z);

    {
        FuryObject* trigger = nullptr;
        int nextTriggerNumber = (m_carObject->getLastTriggerNumber() + 1) % 72;

        for (int i = 0; i < m_testWorld->getObjects().size(); ++i)
        {
            FuryObject* object = m_testWorld->getObjects()[i];

            if (object->name() == QString("Trigger %1").arg(nextTriggerNumber))
            {
                trigger = object;
                break;
            }
        }

        if (trigger != nullptr)
        {
            const glm::vec3& triggerPos = trigger->getPosition();
            glm::vec3 direct = m_carObject->calcNextTriggerVector(triggerPos);
            observation.append(direct.x);
            observation.append(direct.y);
            observation.append(direct.z);
        }
        else
        {
            observation.append(0);
            observation.append(0);
            observation.append(0);
            qDebug() << ru("Ошибка при поиске следующего триггера");
        }
    }

    static bool isFirst = true;
    static float score = 0;
    static int gameCounter = 0;

    if (isFirst)
    {
        int action = m_learnScript->predict(observation);
        m_carObject->getReward(); // Обнуляем reward
        m_carObject->setBotAction(action);
        isFirst = false;
        return;
    }

    float carReward = (m_carObject->getReward());

    if (!m_carObject->checkTimeCounter())
    {
        qDebug() << "timeout";
        carReward = -1;
        m_learnScript->learn(observation, carReward, true);
        m_carObject->respawn();

        isFirst = true;
        qDebug() << "Game:" << gameCounter << "Score:" << score;
        m_scoreList.append(score);
        score = 0;
        gameCounter++;
    }
    if (!m_carObject->checkBackTriggerCounter())
    {
        qDebug() << "back triggers error";
        carReward -= 0.1;
        m_learnScript->learn(observation, carReward, true);
        m_carObject->respawn();

        isFirst = true;
        qDebug() << "Game:" << gameCounter << "Score:" << score;
        m_scoreList.append(score);
        score = 0;
        gameCounter++;
    }
    else if (!m_carObject->checkHasContact())
    {
        qDebug() << "contact error";
        carReward -= 0.1;
        m_learnScript->learn(observation, carReward, true);
        m_carObject->respawn();

        isFirst = true;
        qDebug() << "Game:" << gameCounter << "Score:" << score;
        m_scoreList.append(score);
        score = 0;
        gameCounter++;
    }
    else
    {
        score += carReward;
        int action = m_learnScript->learn(observation, carReward, false);
        m_carObject->setBotAction(action);
    }

#else
    m_carObject->getRays();
#endif
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

void TestRender::displayLogo()
{
    static Shader* shader = nullptr;
    static GLuint vaoDebugTexturedRect = 0;

    if (shader == nullptr)
    {
        shader = new Shader("shaders/logoShader.vs", "shaders/logoShader.fs");

        GLfloat buffer_vertices[] = {
        -1, -1, 0.0f, 1.0f,
        1, -1, 1.0f, 1.0f,
        1, 1, 1.0f, 0.01f,

        -1, -1, 0.0f, 1.0f,
        1, 1, 1.0f, 0.01f,
        -1, 1, 0.0f, 0.01f,
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

    float needWidth = 300;
    float needHeight = 300;

    float logoWidth = needWidth / m_width;
    float logoHeight = needHeight / m_height;

    static QTime alphaTimer = QTime::currentTime();
    float deltaTime = alphaTimer.msecsTo(QTime::currentTime()) / 1000.0f;
    float logoAlpha = std::max(std::sin(deltaTime / m_loadingOvertime * 3.14), 0.0);

    glActiveTexture(GL_TEXTURE0);
    shader->Use();
    shader->setVec2("logoScale", glm::vec2(logoWidth, logoHeight));
    shader->setFloat("logoAlpha", logoAlpha);
    glBindTexture(GL_TEXTURE_2D, m_textureManager->textureByName("Logo").idOpenGL());
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
    m_textureManager->addTexture("textures/" + current_pbr_material_name + "/albedo.png", "m_gold_albedo_texture_id");
    m_textureManager->addTexture("textures/" + current_pbr_material_name + "/normal.png", "m_gold_norm_texture_id");
    m_textureManager->addTexture("textures/" + current_pbr_material_name + "/metallic.png", "m_gold_metallic_texture_id");
    m_textureManager->addTexture("textures/" + current_pbr_material_name + "/roughness.png", "m_gold_roughness_texture_id");
    m_textureManager->addTexture("textures/" + current_pbr_material_name + "/ao.png", "m_gold_ao_texture_id");

    int newTextureNum = 4; // wall
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "testPbr_albedo");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "testPbr_normal");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "testPbr_metallic");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "testPbr_roughness");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "testPbr_ao");

    newTextureNum = 5; // asphalt
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "asphaltPbr_albedo");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "asphaltPbr_normal");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "asphaltPbr_metallic");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "asphaltPbr_roughness");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "asphaltPbr_ao");

    newTextureNum = 1; // grass
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "grassPbr_albedo");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "grassPbr_normal");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "grassPbr_metallic");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "grassPbr_roughness");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "grassPbr_ao");

    newTextureNum = 2; // iron
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "ironPbr_albedo");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "ironPbr_normal");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "ironPbr_metallic");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "ironPbr_roughness");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "ironPbr_ao");

    newTextureNum = 3; // plastic
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "plasticPbr_albedo");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "plasticPbr_normal");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "plasticPbr_metallic");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "plasticPbr_roughness");
    m_textureManager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "plasticPbr_ao");

    m_pbrShader->Use();
    m_pbrShader->setInt("irradianceMap", 0);
    m_pbrShader->setInt("prefilterMap", 1);
    m_pbrShader->setInt("brdfLUT", 2);
    m_pbrShader->setInt("material.albedoMap", 3);
    m_pbrShader->setInt("material.normalMap", 4);
    m_pbrShader->setInt("material.metallicMap", 5);
    m_pbrShader->setInt("material.roughnessMap", 6);
    m_pbrShader->setInt("material.aoMap", 7);
    m_pbrShader->setInt("shadowMap", 8);
    glUseProgram(0);
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

void TestRender::loadRaceMapFromJson()
{
    QFile file("scene/second.json");

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray json = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(json);
        QJsonArray array = doc.array();

        for (int i = 0; i < array.size(); ++i)
        {
            QJsonObject obj = array[i].toObject();

            FuryBoxObject* object = FuryBoxObject::fromJson(obj, m_testWorld);
            object->setShader(m_pbrShader);
            m_testWorld->addObject(object);
        }
    }

    if (!m_materialManager->materialExist("redRaceTriggerMaterial"))
    {
        FuryPbrMaterial* mat = m_materialManager->createPbrMaterial("redRaceTriggerMaterial");
        mat->setAlbedoTexture("redCheckBox");
        mat->setOpacity(0.5);

        mat = m_materialManager->createPbrMaterial("greenRaceTriggerMaterial");
        mat->setAlbedoTexture("greenCheckBox");
        mat->setOpacity(0.5);
    }
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

unsigned int loadTexture(const QString& path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height;
    unsigned char *data = stbi_load(qUtf8Printable(path), &width, &height, 0, STBI_rgb_alpha);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << qUtf8Printable(path) << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
