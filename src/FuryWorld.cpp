#include "FuryWorld.h"

#include "Camera.h"
#include "Shader.h"
#include "FuryMesh.h"
#include "FuryObject.h"
#include "Logger/FuryLogger.h"
#include "FuryPbrMaterial.h"
#include "DefaultObjects/FuryBoxObject.h"
#include "DefaultObjects/FurySphereObject.h"
#include "Managers/FuryTextureManager.h"
#include "Managers/FuryMaterialManager.h"
#include "Managers/FuryWorldManager.h"
#include "Managers/FuryModelManager.h"
#include "Widgets/FuryRenderer.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


glm::mat4 getLightSpaceMatrix(Camera* _camera, const glm::vec3 &_dirLightPosition);




FuryWorld::FuryWorld(reactphysics3d::PhysicsCommon *_physicsCommon) :
    QObject(nullptr),
    m_physicsCommon(_physicsCommon),
    m_currentCamera(nullptr),
    m_started(true),
    m_envCubemap(0),
    m_irradianceMap(0),
    m_prefilterMap(0),
    m_brdfLUTTexture(0),
    m_shadowMapEnabled(false)
{
    Debug(ru("Создание игрового мира"));
    m_physicsWorld = m_physicsCommon->createPhysicsWorld();
}

FuryWorld::~FuryWorld()
{
    Debug(ru("Удаление игрового мира"));

    for (FuryObject* object : m_objects)
    {
        if (object != nullptr)
        {
            delete object;
        }
    }

    m_physicsCommon->destroyPhysicsWorld(m_physicsWorld);
}

void FuryWorld::tick(double _dt)
{
    if (m_started)
    {
        m_physicsWorld->update(_dt);

        for (FuryObject* object : m_objects)
        {
            object->postPhysics();
        }

        for (FuryObject* object : m_objects)
        {
            object->tick(_dt);
        }
    }
}

void FuryWorld::resetWorld()
{
    foreach (FuryObject* object, m_objects)
    {
        object->reset();
    }
}

void FuryWorld::draw(int _width, int _height)
{
    static Shader* m_backgroundShader =  new Shader("shaders/pbr/2.2.2.background.vs",
                                                    "shaders/pbr/2.2.2.background.fs");

    FuryWorldManager* worldManager = FuryWorldManager::instance();
    FuryMaterialManager* materialManager = FuryMaterialManager::instance();
    FuryModelManager* modelManager = FuryModelManager::instance();
    FuryRenderer* renderer = FuryRenderer::instance();


    float perspective_near = 0.1f;
    float perspective_far = 300.f;


    glm::vec3 dirLightPosition(10, 10, 10);
    glm::mat4 lightSpaceMatrix = getLightSpaceMatrix(m_currentCamera, dirLightPosition);

    QVector<FuryObject*> testWorldObjects = worldManager->worldByName(objectName()).getRootObjects();
    QVector<QPair<FuryObject*, FuryMesh*>> meshesForRender1;
    QVector<QPair<FuryObject*, FuryMesh*>> meshesForRender2;

    for (int i = 0; i < testWorldObjects.size(); ++i)
    {
        FuryObject* obj = testWorldObjects[i];
        foreach (QObject* child, obj->children())
        {
            FuryObject* obj = qobject_cast<FuryObject*>(child);
            if (obj != nullptr)
            {
                testWorldObjects.append(obj);
            }
        }

        if (!obj->visible())
        {
            continue;
        }

        FuryModel* model = modelManager->modelByName(obj->modelName());
        FuryPbrMaterial* objMat = nullptr;
        if (materialManager->materialExist(obj->materialName()))
        {
            objMat = dynamic_cast<FuryPbrMaterial*>(materialManager->materialByName(obj->materialName()));
        }

        foreach (FuryMesh* mesh, model->meshes())
        {
            float opacity = 1;

            if (objMat == nullptr)
            {
                FuryMaterial* material = materialManager->materialByName(mesh->materialName());
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


    const glm::mat4& projection = m_currentCamera->getPerspectiveMatrix(_width, _height, perspective_near, perspective_far);
    const glm::mat4& view =  m_currentCamera->getViewMatrix();

    for (QPair<FuryObject*, FuryMesh*>& pair : meshesForRender1)
    {
        FuryObject* obj = pair.first;
        FuryMesh* mesh = pair.second;
        Shader* shader = obj->shader();

        shader->use();
        shader->setVec3("viewPos", m_currentCamera->position());
        shader->setFloat("material.shininess", 128.0f); // 32.0 - default
        shader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - dirLightPosition);


        // view/projection transformations
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader->setVec2("textureScales", obj->textureScales());


        {
            shader->setVec3("camPos", m_currentCamera->position());

            glm::vec3 tempPosition = dirLightPosition;
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
            shader->setBool("shadowMapEnabled", m_shadowMapEnabled);

            if (m_shadowMapEnabled)
            {
                glBindTexture(GL_TEXTURE_2D, renderer->depthMap());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        glm::mat4 modelMatrix = obj->getOpenGLTransform();
        modelMatrix = glm::scale(modelMatrix, obj->scales());
        modelMatrix *= obj->modelTransform();
        modelMatrix *= mesh->transformation();
        shader->setMat4("model", modelMatrix);
        shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));

        FuryMaterial* material = nullptr;
        if (materialManager->materialExist(obj->materialName()))
        {
            material = materialManager->materialByName(obj->materialName());
        }
        mesh->draw(shader, material);


        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }


    {
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        m_backgroundShader->use();
        m_backgroundShader->setMat4("projection", projection);
        m_backgroundShader->setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
        // skybox cube
        glBindVertexArray(renderer->skyboxVAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

    {
        QList<QPair<float, QPair<FuryObject*, FuryMesh*>>> sorted;
        for (unsigned int i = 0; i < meshesForRender2.size(); i++){
            float distance = glm::length(m_currentCamera->position() - meshesForRender2[i].first->worldPosition());
            sorted.append(qMakePair(distance, meshesForRender2[i]));
        }

        std::sort(sorted.begin(), sorted.end(), [](auto& p1, auto& p2){return p1.first < p2.first;});

        for (int i = sorted.size() - 1; i >= 0; --i)
        {
            QPair<FuryObject*, FuryMesh*>& pair = sorted[i].second;
            FuryObject* obj = pair.first;
            FuryMesh* mesh = pair.second;
            Shader* shader = obj->shader();

            shader->use();
            shader->setVec3("viewPos", m_currentCamera->position());
            shader->setFloat("material.shininess", 128.0f); // 32.0 - default
            shader->setVec3("dirLight.direction", glm::vec3(0, 0, 0) - dirLightPosition);


            // view/projection transformations
            shader->setMat4("projection", projection);
            shader->setMat4("view", view);

            shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            shader->setVec2("textureScales", obj->textureScales());


            {
                shader->setVec3("camPos", m_currentCamera->position());

                glm::vec3 tempPosition = dirLightPosition;
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
                shader->setBool("shadowMapEnabled", m_shadowMapEnabled);

                if (m_shadowMapEnabled)
                {
                    glBindTexture(GL_TEXTURE_2D, renderer->depthMap());
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }

            glm::mat4 modelMatrix = obj->getOpenGLTransform();
            modelMatrix = glm::scale(modelMatrix, obj->scales());
            modelMatrix *= obj->modelTransform();
            modelMatrix *= mesh->transformation();
            shader->setMat4("model", modelMatrix);
            shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));

            FuryMaterial* material = nullptr;
            if (materialManager->materialExist(obj->materialName()))
            {
                material = materialManager->materialByName(obj->materialName());
            }

            mesh->draw(shader, material);


            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }

        sorted.clear();
    }
}

void FuryWorld::addRootObject(FuryObject* _object)
{
    m_objects.push_back(_object);
}

void FuryWorld::addObject(FuryObject *_object)
{
    m_allObjects.append(_object);
    connect(_object, &FuryObject::parentChangedSignal,
            this, &FuryWorld::parentChangedSlot);

    emit addObjectSignal(_object);
}

void FuryWorld::setCamera(Camera* _camera)
{
    m_currentCamera = _camera;
}

void FuryWorld::deleteCurrentCamera()
{
    delete m_currentCamera;
}

const QVector<FuryObject*>& FuryWorld::getRootObjects()
{
    return m_objects;
}

const QVector<FuryObject *> &FuryWorld::getAllObjects()
{
    return m_allObjects;
}

void FuryWorld::loadRaceMap()
{
    FuryObject* triggers = new FuryObject(this);
    triggers->setObjectName("Triggers");
    FuryObject* raceMap = new FuryObject(this);
    raceMap->setObjectName("RaceMap");

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

                FuryBoxObject* object = FuryBoxObject::fromJson(obj, this);
                addRootObject(object);

                if (object->objectName().startsWith("race"))
                {
                    raceMap->addChildObject(object, true);
                }
                else if (object->objectName().startsWith("Trigger"))
                {
                    triggers->addChildObject(object, true);
                }
            }
        }
    }

    {
        QFile file("scene/spheres.json");

        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray json = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(json);
            QJsonArray array = doc.array();

            for (int i = 0; i < array.size(); ++i)
            {
                QJsonObject obj = array[i].toObject();

                FurySphereObject* object = FurySphereObject::fromJson(obj, this);
                addRootObject(object);
            }
        }
    }
}

void FuryWorld::createMaterials()
{
    FuryMaterialManager* manager = FuryMaterialManager::instance();

    FuryPbrMaterial* firstPbrMaterial = manager->createPbrMaterial("firstPbrMaterial");
    firstPbrMaterial->setAlbedoTexture("testPbr_albedo");
    firstPbrMaterial->setNormalTexture("testPbr_normal");
    firstPbrMaterial->setMetallicTexture("testPbr_metallic");
    firstPbrMaterial->setRoughnessTexture("testPbr_roughness");
    firstPbrMaterial->setAoTexture("testPbr_ao");

    FuryPbrMaterial* plasticPbrMaterial = manager->createPbrMaterial("plasticPbrMaterial");
    plasticPbrMaterial->setAlbedoTexture("plasticPbr_albedo");
    plasticPbrMaterial->setNormalTexture("plasticPbr_normal");
    plasticPbrMaterial->setMetallicTexture("plasticPbr_metallic");
    plasticPbrMaterial->setRoughnessTexture("plasticPbr_roughness");
    plasticPbrMaterial->setAoTexture("plasticPbr_ao");

    FuryPbrMaterial* ironPbrMaterial = manager->createPbrMaterial("ironPbrMaterial");
    ironPbrMaterial->setAlbedoTexture("ironPbr_albedo");
    ironPbrMaterial->setNormalTexture("ironPbr_normal");
    ironPbrMaterial->setMetallicTexture("ironPbr_metallic");
    ironPbrMaterial->setRoughnessTexture("ironPbr_roughness");
    ironPbrMaterial->setAoTexture("ironPbr_ao");

    FuryPbrMaterial* grassPbrMaterial = manager->createPbrMaterial("grassPbrMaterial");
    grassPbrMaterial->setAlbedoTexture("grassPbr_albedo");
    grassPbrMaterial->setNormalTexture("grassPbr_normal");
    grassPbrMaterial->setMetallicTexture("grassPbr_metallic");
    grassPbrMaterial->setRoughnessTexture("grassPbr_roughness");
    grassPbrMaterial->setAoTexture("grassPbr_ao");

    FuryPbrMaterial* goldPbrMaterial = manager->createPbrMaterial("goldPbrMaterial");
    goldPbrMaterial->setAlbedoTexture("m_gold_albedo_texture_id");
    goldPbrMaterial->setNormalTexture("m_gold_norm_texture_id");
    goldPbrMaterial->setMetallicTexture("m_gold_metallic_texture_id");
    goldPbrMaterial->setRoughnessTexture("m_gold_roughness_texture_id");
    goldPbrMaterial->setAoTexture("m_gold_ao_texture_id");


    FuryPbrMaterial* asphaltPbrMaterial = manager->createPbrMaterial("asphaltPbrMaterial");
    asphaltPbrMaterial->setAlbedoTexture("asphaltPbr_albedo");
    asphaltPbrMaterial->setNormalTexture("asphaltPbr_normal");
    asphaltPbrMaterial->setMetallicTexture("asphaltPbr_metallic");
    asphaltPbrMaterial->setRoughnessTexture("asphaltPbr_roughness");
    asphaltPbrMaterial->setAoTexture("asphaltPbr_ao");

    FuryPbrMaterial* mat = manager->createPbrMaterial("redRaceTriggerMaterial");
    mat->setAlbedoTexture("redCheckBox");
    mat->setOpacity(0.5);

    mat = manager->createPbrMaterial("greenRaceTriggerMaterial");
    mat->setAlbedoTexture("greenCheckBox");
    mat->setOpacity(0.5);

    FuryPbrMaterial* debugMat = manager->createPbrMaterial("debugMat");
    debugMat->setTwoSided(true);
}

void FuryWorld::createTextures()
{
    FuryTextureManager* manager = FuryTextureManager::instance();
    manager->addTexture("textures/FuryEngine_logo_for_dark.png", "Logo");


    QVector<QString> pbr_material_names{ "pbr_gold",
                                         "pbr_grass",
                                         "pbr_iron",
                                         "pbr_plastic",
                                         "pbr_wall",
                                         "pbr_asphalt" };


    int newTextureNum = 0; // gold
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "m_gold_albedo_texture_id");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "m_gold_norm_texture_id");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "m_gold_metallic_texture_id");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "m_gold_roughness_texture_id");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "m_gold_ao_texture_id");

    newTextureNum = 4; // wall
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "testPbr_albedo");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "testPbr_normal");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "testPbr_metallic");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "testPbr_roughness");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "testPbr_ao");

    newTextureNum = 5; // asphalt
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "asphaltPbr_albedo");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "asphaltPbr_normal");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "asphaltPbr_metallic");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "asphaltPbr_roughness");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "asphaltPbr_ao");

    newTextureNum = 1; // grass
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "grassPbr_albedo");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "grassPbr_normal");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "grassPbr_metallic");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "grassPbr_roughness");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "grassPbr_ao");

    newTextureNum = 2; // iron
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "ironPbr_albedo");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "ironPbr_normal");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "ironPbr_metallic");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "ironPbr_roughness");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "ironPbr_ao");

    newTextureNum = 3; // plastic
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/albedo.png", "plasticPbr_albedo");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/normal.png", "plasticPbr_normal");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/metallic.png", "plasticPbr_metallic");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/roughness.png", "plasticPbr_roughness");
    manager->addTexture("textures/" + pbr_material_names[newTextureNum] + "/ao.png", "plasticPbr_ao");

    manager->addTexture("textures/box_texture_5x5.png", "defaultBoxTexture");
    manager->addTexture("textures/box_texture3_orig.png", "Diffuse_numbersBoxTexture");
    manager->addTexture("textures/carBody.png", "carBody");
    manager->addTexture("textures/rayCastBall.png", "Diffuse_rayCastBall");
    manager->addTexture("textures/greenRay.png", "Diffuse_greenRay");
    manager->addTexture("textures/box_texture2.png", "Diffuse_raceWall");
    manager->addTexture("textures/asphalt.png", "Diffuse_asphalt");
    manager->addTexture("textures/redCheckBox.png", "redCheckBox");
    manager->addTexture("textures/greenCheckBox.png", "greenCheckBox");

    manager->addTexture("awesomeface.png", "awesomeface");
    manager->addTexture("textures/smoke_ver2.png", "smoke_ver2");
}

void FuryWorld::createPbrCubemap(const QString &_cubemapHdrName)
{
    FuryRenderer* renderer = FuryRenderer::instance();
    renderer->createPBRTextures(_cubemapHdrName, &m_envCubemap,
                                &m_irradianceMap, &m_prefilterMap, &m_brdfLUTTexture);
}

void FuryWorld::parentChangedSlot()
{
    FuryObject* obj = qobject_cast<FuryObject*>(sender());
    emit parentChangedSignal(obj);
}


glm::mat4 getLightSpaceMatrix(Camera* _camera, const glm::vec3 &_dirLightPosition)
{
    float shadowNear = 0.1f;
    float shadowPlane = 70.f;
    float shadowViewSize = 25.f;
    float shadowCamDistance = 37;

    glm::mat4 lightProjection = glm::ortho(-shadowViewSize, shadowViewSize,
                                           -shadowViewSize, shadowViewSize,
                                           shadowNear, shadowPlane);

    glm::vec3 tempDirLight = glm::normalize(_dirLightPosition);
    tempDirLight *= shadowCamDistance;

    glm::vec3 cameraPos = _camera->position();
    glm::vec3 cameraFront = _camera->front();
    cameraFront *= 15;

    glm::vec3 shadowCameraView = cameraPos + cameraFront;
    shadowCameraView.y = 0;
    glm::vec3 shadowCameraPos = shadowCameraView + tempDirLight;

    glm::mat4 lightView = glm::lookAt(shadowCameraPos,
                                      shadowCameraView,
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    return lightProjection * lightView;
}
