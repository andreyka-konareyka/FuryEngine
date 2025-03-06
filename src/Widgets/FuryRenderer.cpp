#include "FuryRenderer.h"


#include "Logger/FuryLogger.h"
#include "FuryScript.h"
#include "DefaultObjects/FuryBoxObject.h"
#include "DefaultObjects/FurySphereObject.h"
#include "LocalKeyboard/FuryRussianLocalKeyMapper.h"

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
#define NEED_LEARN 0


const unsigned int SHADOW_WIDTH = 1024 * 4;
const unsigned int SHADOW_HEIGHT = 1024 * 4;

const unsigned int MAIN_BUFFER_WIDTH = 2048;
const unsigned int MAIN_BUFFER_HEIGHT = 2048;


void renderCube();
void renderQuad();


FuryRenderer* FuryRenderer::s_instance = nullptr;


FuryRenderer::FuryRenderer(QWidget *_parent) :
    QOpenGLWidget(_parent),
    m_textureManager(FuryTextureManager::createInstance()),
    m_modelManager(FuryModelManager::createInstance()),
    m_materialManager(FuryMaterialManager::createInstance()),
    m_worldManager(FuryWorldManager::createInstance()),
    m_shaderManager(FuryShaderManager::createInstance()),
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
    if (s_instance != nullptr)
    {
        Debug(ru("Повторное создание главного отрисовщика."));
        throw FuryException(ru("Повторное создание главного отрисовщика."),
                            "", Q_FUNC_INFO);
    }


    Debug(ru("Создание рендера"));
    s_instance = this;

    connect(this, &QOpenGLWidget::frameSwapped,
            this, qOverload<>(&FuryRenderer::update), Qt::QueuedConnection);
    setFocusPolicy(Qt::StrongFocus);


    m_testWorld = &(m_worldManager->createWorld("testWorld"));
    m_eventListener = new FuryEventListener;
    m_testWorld->physicsWorld()->setEventListener(m_eventListener);
}

FuryRenderer::~FuryRenderer()
{
    Debug(ru("Удаление рендера"));
    s_instance = nullptr;

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

    Debug(ru("Удаление менеджера шейдеров..."));
    FuryShaderManager::deleteInstance();

    Debug(ru("Удаление менеджера миров..."));
    FuryWorldManager::deleteInstance();

    Debug(ru("Удаление менеджера материалов..."));
    FuryMaterialManager::deleteInstance();

    Debug(ru("Остановка текстурного менеджера..."));
    m_textureManager->stopLoopAndWait();
    FuryTextureManager::deleteInstance();

    Debug(ru("Остановка менеджера моделей..."));
    m_modelManager->stopLoopAndWait();
    FuryModelManager::deleteInstance();
}

GLuint FuryRenderer::renderTestScene(const QString &_materialName, int _width, int _height)
{
    makeCurrent();

    glBindFramebuffer(GL_FRAMEBUFFER, m_testFrameBuffer);
    glViewport(0, 0, MAIN_BUFFER_WIDTH, MAIN_BUFFER_HEIGHT);

    // Clear the colorbuffer
    glClearColor(0, 1, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const QVector<FuryObject*> worldObjects = m_worldManager->worldByName("materialPreview").getRootObjects();
    if (worldObjects.size() == 1)
    {
        worldObjects[0]->setMaterialName(_materialName);
    }

    m_worldManager->worldByName("materialPreview").draw(_width, _height);

    doneCurrent();

    return m_testRenderTexture;
}

FuryRenderer *FuryRenderer::instance()
{
    return s_instance;
}

void FuryRenderer::initializeGL()
{
    Debug(ru("Инициализация OpenGL"));
    InitGL();
}

void FuryRenderer::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);

    m_height = h;
    m_width = w;
}

void FuryRenderer::paintGL()
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
        else if (intervalLoadPart == 3)
        {
            m_modelManager->loadModelPart();
        }

        intervalLoadPart = (intervalLoadPart + 1) % 6;
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

void FuryRenderer::setCameraZoomValue(int _value)
{
    for (Camera* camera : m_cameras)
    {
        camera->setCameraZoom(_value);
    }
}

void FuryRenderer::saveLearnModel()
{
#if NEED_LEARN == 1
    m_learnScript->saveModel();
#endif
}

void FuryRenderer::saveScoreList()
{
    Debug(ru("[ ВНИМАНИЕ ] Модель не сохраняю..."));
    return;

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

void FuryRenderer::mouseMoveEvent(QMouseEvent* _event)
{
    if (m_testWorld->camera() == m_cameras[1])
    { // Если камера машины
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

void FuryRenderer::mousePressEvent(QMouseEvent *_event)
{
    if (m_testWorld->camera() == m_cameras[1])
    { // Если камера машины
        return;
    }

    if (_event->button() == Qt::LeftButton)
    {
        lastX = (GLfloat)(_event->position().x());
        lastY = (GLfloat)(_event->position().y());
    }
}

void FuryRenderer::keyPressEvent(QKeyEvent *_event)
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

void FuryRenderer::keyReleaseEvent(QKeyEvent *_event)
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




void FuryRenderer::InitGL()
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


    createRenderBuffer(&m_mainFrameBuffer, &m_mainRenderBuffer, &m_mainRenderTexture,
                       MAIN_BUFFER_WIDTH, MAIN_BUFFER_HEIGHT);
    createRenderBuffer(&m_testFrameBuffer, &m_testRenderBuffer, &m_testRenderTexture,
                       MAIN_BUFFER_WIDTH, MAIN_BUFFER_HEIGHT);

    // Загрузка всех текстур, шейдеров и т.д.
    init();
}

void FuryRenderer::init() {
    Debug(ru("Версия OpenGL: ") + ru(glGetString(GL_VERSION)));
    Debug(ru("Количество потоков: ") + QString::number(QThread::idealThreadCount()));

    m_modelManager->addModel("objects/cube/cube.obj", "cube");
    m_modelManager->addModel("objects/sphere/sphere.obj", "sphere");

    m_pbrShader = m_shaderManager->createShader("pbrShader",
                                                "shaders/pbr/2.2.2.pbr.vs",
                                                "shaders/pbr/2.2.2.pbr.fs");
//    m_pbrShader = new Shader("shaders/pbr/2.2.2.pbr.vs", "shaders/pbr/2.2.2.pbr.fs");

    m_sunVisualBox = new FurySphereObject(m_testWorld, m_dirlight_position);
    m_sunVisualBox->setObjectName("sunVisualBox");
    m_sunVisualBox->setShaderName("pbrShader");

    m_testWorld->addRootObject(m_sunVisualBox);
    // Camera
    m_cameras.push_back(new Camera(glm::vec3(0.0f, 10.0f, 40.0f)));
    m_cameras.push_back(new Camera(glm::vec3(0.0f, 30.0f, 60.0f)));
    m_cameras.push_back(new Camera(glm::vec3(0, 0, 2)));
    m_testWorld->setCamera(m_cameras[0]);

    FuryWorld& world = m_worldManager->createWorld("materialPreview");
    world.setCamera(m_cameras[2]);
    world.addRootObject(new FurySphereObject(&world));
    world.createPbrCubemap("textures/hdr/newport_loft3.hdr");

    m_testWorld->createPbrCubemap("textures/hdr/newport_loft3.hdr");
    m_testWorld->createDepthMap();

    //
    // МАШИНА
    //

    m_testWorld->load();

    foreach (FuryObject* obj, m_testWorld->getRootObjects())
    {
        if (obj->objectName() == "AI_car")
        {
            m_carObject = qobject_cast<CarObject*>(obj);
            break;
        }
    }

    m_eventListener->setCarObject(m_carObject);


    m_particleShader = new Shader("particle.vs", "particle.fs");



    glm::vec3 part_pos(5, 0, 0);
    double part_scale = 0.15;
    glm::vec3 part_speed(0, 0.5, 0);
    glm::vec4 part_color(1, 1, 0, 1);
    m_myFirstParticle = new Particle(part_pos, part_scale, part_speed, part_color, "awesomeface", 10, m_particleShader);

    glm::vec3 part_sys_pos(0, 2, 0);
    m_myFirstParticleSystem = new ParticleSystem(part_sys_pos, "smoke_ver2", 100);


    loadRaceMapFromJson();


//    m_modelManager->addModel("objects/car2/LOD2.obj", "backpack2LOD2");
    m_modelManager->addModel("objects/car2/car.obj", "backpack2");
//    m_modelManager->addModel("objects/car1/car.FBX", "backpack2");

    {
        // initShaderInform

        QList<Shader*> shaders({
                                   m_pbrShader
                               });

        for (Shader* shader : shaders)
        {
            shader->use();
            shader->setVec3("lightColors[0]", 300.0f, 300.0f, 300.0f);
            shader->setVec3("lightColors[1]", 300.0f, 300.0f, 300.0f);
            shader->setVec3("lightColors[2]", 300.0f, 300.0f, 300.0f);
            shader->setVec3("lightColors[3]", 300.0f, 300.0f, 300.0f);
            shader->setInt("irradianceMap", 0);
            shader->setInt("prefilterMap", 1);
            shader->setInt("brdfLUT", 2);
            shader->setInt("material.albedoMap", 3);
            shader->setInt("material.normalMap", 4);
            shader->setInt("material.metallicMap", 5);
            shader->setInt("material.roughnessMap", 6);
            shader->setInt("material.aoMap", 7);
            shader->setInt("shadowMap", 8);
        }
    }
}

void FuryRenderer::render()
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



    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
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
    m_sunVisualBox->setWorldPosition(tempPosition);


    // 1. сначала рисуем карту глубины
    if (NEED_DRAW_SHADOW)
    {
        m_testWorld->drawDepthMap();
    }


    glBindFramebuffer(GL_FRAMEBUFFER, m_mainFrameBuffer);
    glViewport(0, 0, MAIN_BUFFER_WIDTH, MAIN_BUFFER_HEIGHT);



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



    m_testWorld->draw(m_width, m_height);


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




    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, this->m_width, this->m_height);
    renderMainBuffer();
}

void FuryRenderer::createPBRTextures(const QString& _cubemapHdrPath,
                                     GLuint* _envCubemap,
                                     GLuint* _irradianceMap,
                                     GLuint* _prefilterMap,
                                     GLuint* _brdfLUTTexture)
{
    glDisable(GL_CULL_FACE);
    glClearColor(1, 1, 1, 1.0f);

    // pbr: setup framebuffer
        // ----------------------
        GLuint captureFBO;
        GLuint captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        // pbr: load the HDR environment map
        // ---------------------------------
        int width, height;
        float *data = stbi_loadf(qUtf8Printable(_cubemapHdrPath), &width, &height, 0, STBI_rgb);
        GLuint hdrTexture;

        if (data)
        {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            qDebug() << "Failed to load HDR image.";
        }

        // pbr: setup cubemap to render to and attach to framebuffer
        // ---------------------------------------------------------
        glGenTextures(1, _envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *_envCubemap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
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
        equirectangularToCubemapShader.use();
        equirectangularToCubemapShader.setInt("equirectangularMap", 0);
        equirectangularToCubemapShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            equirectangularToCubemapShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *_envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glBindTexture(GL_TEXTURE_CUBE_MAP, *_envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
        // --------------------------------------------------------------------------------
        glGenTextures(1, _irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *_irradianceMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
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
        irradianceShader.use();
        irradianceShader.setInt("environmentMap", 0);
        irradianceShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *_envCubemap);

        glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            irradianceShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *_irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
        // --------------------------------------------------------------------------------
        glGenTextures(1, _prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *_prefilterMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
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
        prefilterShader.use();
        prefilterShader.setInt("environmentMap", 0);
        prefilterShader.setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *_envCubemap);

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
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *_prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderCube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // pbr: generate a 2D LUT from the BRDF equations used.
        // ----------------------------------------------------
        glGenTextures(1, _brdfLUTTexture);

        // pre-allocate enough memory for the LUT texture.
        glBindTexture(GL_TEXTURE_2D, *_brdfLUTTexture);
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *_brdfLUTTexture, 0);

        glViewport(0, 0, 512, 512);

        Shader brdfShader("shaders/pbr/2.2.2.brdf.vs", "shaders/pbr/2.2.2.brdf.fs");
        brdfShader.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_BLEND);
        renderQuad();
        glEnable(GL_BLEND);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glEnable(GL_CULL_FACE);
}

void FuryRenderer::renderLoading(float _currentFrame)
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

void FuryRenderer::updatePhysics()
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

        m_testWorld->tick(timeStep);
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

void FuryRenderer::displayBuffer(GLuint _bufferId)
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
    shader->use();
    glBindTexture(GL_TEXTURE_2D, _bufferId);
    glBindVertexArray(vaoDebugTexturedRect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
}

void FuryRenderer::displayLogo()
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
    shader->use();
    shader->setVec2("logoScale", glm::vec2(logoWidth, logoHeight));
    shader->setFloat("logoAlpha", logoAlpha);
    glBindTexture(GL_TEXTURE_2D, m_textureManager->textureByName("Logo").idOpenGL());
    glBindVertexArray(vaoDebugTexturedRect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
}

void FuryRenderer::createRenderBuffer(GLuint* _frameBuffer,
                                      GLuint* _renderBuffer,
                                      GLuint* _renderTexture,
                                      int _width, int _height)
{
    glGenFramebuffers(1, _frameBuffer);
    glGenRenderbuffers(1, _renderBuffer);
    glGenTextures(1, _renderTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, *_frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *_renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *_renderBuffer);

    glBindTexture(GL_TEXTURE_2D, *_renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *_renderTexture, 0);
}

void FuryRenderer::renderMainBuffer()
{
    static Shader* shader = nullptr;
    static GLuint vaoDebugTexturedRect = 0;

    if (shader == nullptr)
    {
        shader = new Shader("bufferShader.vs", "bufferShader.fs");

        GLfloat buffer_vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
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
    shader->use();
    glBindTexture(GL_TEXTURE_2D, m_mainRenderTexture);
    glBindVertexArray(vaoDebugTexturedRect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
}



void FuryRenderer::do_movement()
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

void FuryRenderer::createDepthMap(GLuint* _depthMapFBO, GLuint* _depthMap)
{
    // Создадим буфер глубины для теней
    glGenFramebuffers(1, _depthMapFBO);

    // Создадим текстуру для буфера
    glGenTextures(1, _depthMap);
    glBindTexture(GL_TEXTURE_2D, *_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, *_depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FuryRenderer::loadRaceMapFromJson()
{
    m_testWorld->createMaterials();
    m_testWorld->createTextures();
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
