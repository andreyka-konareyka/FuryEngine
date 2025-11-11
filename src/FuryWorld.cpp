#include "FuryWorld.h"

#include "Camera.h"
#include "FuryMesh.h"
#include "FuryModel.h"
#include "FuryObject.h"
#include "FuryModelCache.h"
#include "Logger/FuryLogger.h"
#include "FuryPbrMaterial.h"
#include "Managers/FuryTextureManager.h"
#include "Managers/FuryMaterialManager.h"
#include "Widgets/FuryRenderer.h"
#include "FuryObjectsFactory.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMetaProperty>


FuryWorld::FuryWorld(reactphysics3d::PhysicsCommon *_physicsCommon) :
    QObject(nullptr),
    m_physicsCommon(_physicsCommon),
    m_currentCamera(nullptr),
    m_started(true),
    m_dirLightPosition(glm::vec3(10, 10, -10)),
    m_envCubemap(0),
    m_irradianceMap(0),
    m_prefilterMap(0),
    m_brdfLUTTexture(0),
    m_shadowMapEnabled(false),
    m_depthMapFBO(0),
    m_depthMap(0)
{
    Debug(ru("Создание игрового мира"));
    m_physicsWorld = m_physicsCommon->createPhysicsWorld();

    initConnections();
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

void FuryWorld::addRootObject(FuryObject* _object)
{
    m_objects.push_back(_object);
}

void FuryWorld::addObject(FuryObject *_object)
{
    m_allObjects.append(_object);
    connect(_object, &FuryObject::parentChangedSignal,
            this, &FuryWorld::parentChangedSlot);
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

void FuryWorld::createDepthMap()
{
    FuryRenderer* renderer = FuryRenderer::instance();
    renderer->createDepthMap(&m_depthMapFBO, &m_depthMap);
    m_shadowMapEnabled = true;
}

void FuryWorld::save()
{
    QJsonObject world;
    world["name"] = objectName();

    QJsonArray objects;

    foreach (const FuryObject* obj, m_objects)
    {
        objects.append(obj->toJson());
    }

    world["objects"] = objects;

    QJsonDocument document(world);
    QFile file("scene/testWorld.json");

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(document.toJson());
        file.close();
    }
}

void FuryWorld::load()
{
    QFile file("scene/testWorld_AE.json");

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray json = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(json);
        QJsonObject world = doc.object();

        qDebug() << "load world" << world["name"].toString();

        QJsonArray objects = world["objects"].toArray();
        FuryObjectsFactory* factory = FuryObjectsFactory::instance();

        for (int i = 0; i < objects.size(); ++i)
        {
            FuryObject* object = factory->fromJson(objects.at(i).toObject(), this,
                                                   nullptr, true);
            addRootObject(object);
        }
    }
}

void FuryWorld::parentChangedSlot()
{
    FuryObject* obj = qobject_cast<FuryObject*>(sender());
    emit parentChangedSignal(obj);
}

void FuryWorld::fillDrawComponents(QVector<QPair<FuryObject *, FuryMesh *> > &_solidComponents,
                                   QVector<QPair<FuryObject *, FuryMesh *> > &_transparentComponents)
{
    FuryMaterialManager* materialManager = FuryMaterialManager::instance();


    QVector<FuryObject*> testWorldObjects = getRootObjects();

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

        const FuryModel& model = obj->modelCache()->model();
        FuryPbrMaterial* objMat = nullptr;
        if (materialManager->materialExist(obj->materialName()))
        {
            objMat = dynamic_cast<FuryPbrMaterial*>(materialManager->materialByName(obj->materialName()));
        }

        foreach (FuryMesh* mesh, model.meshes())
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
                _solidComponents.append(qMakePair(obj, mesh));
            }
            else
            {
                _transparentComponents.append(qMakePair(obj, mesh));
            }
        }
    }
}

void FuryWorld::initConnections()
{
    connect(FuryObjectsFactory::instance(), &FuryObjectsFactory::createObjectSignal,
            this, &FuryWorld::addObjectSignal);
}
