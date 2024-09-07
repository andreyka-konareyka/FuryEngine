#include "FurySphereObject.h"

#include <QVector>

#include "Shader.h"
#include "FuryWorld.h"


Shader* initSphereShader();


FurySphereObject::FurySphereObject(FuryWorld *_world) :
    FurySphereObject(_world, glm::vec3(0, 0, 0)) {}
FurySphereObject::FurySphereObject(FuryWorld *_world, const glm::vec3& _position) :
    FurySphereObject(_world, _position, 1) {}
FurySphereObject::FurySphereObject(FuryWorld *_world, const glm::vec3& _position, double _scale) :
    FuryObject(_world, _position)
{
    setScales(glm::vec3(_scale, _scale, _scale));
    setShader(initSphereShader());

    setModelName("sphere");


    const rp3d::Vector3& pos = physicsBody()->getTransform().getPosition();

    rp3d::Quaternion rot = rp3d::Quaternion::fromEulerAngles(0, 0, 0);
    rp3d::Transform transform(pos, rot);
    physicsBody()->setTransform(transform);
}

void FurySphereObject::Setup_physics(reactphysics3d::BodyType _type)
{
    physicsBody()->setType(_type);

    reactphysics3d::SphereShape* sphereShape = world()->physicsCommon()->createSphereShape(scales().x);
    reactphysics3d::Transform transform_shape = reactphysics3d::Transform::identity();
    reactphysics3d::Collider* collider1;
    collider1 = physicsBody()->addCollider(sphereShape, transform_shape);

    collider1->getMaterial().setFrictionCoefficient(0.25);
}

Shader *FurySphereObject::defaultShader()
{
    return initSphereShader();
}

QJsonObject FurySphereObject::toJson()
{
    QJsonObject result;

    result["name"] = objectName();
    result["position"] = QString("(%1; %2; %3)").arg(worldPosition().x)
                                                .arg(worldPosition().y)
                                                .arg(worldPosition().z);
    result["scale"] = scales().x;

    if (physicsBody()->getNbColliders() == 0)
    {
        result["physicsType"] = "NONE";
    }
    else if (physicsBody()->getType() == rp3d::BodyType::STATIC)
    {
        result["physicsType"] = "STATIC";
    }
    else if (physicsBody()->getType() == rp3d::BodyType::DYNAMIC)
    {
        result["physicsType"] = "DYNAMIC";
    }
    else if (physicsBody()->getType() == rp3d::BodyType::KINEMATIC)
    {
        result["physicsType"] = "KINEMATIC";
    }

    result["isTrigger"] = physicsBody()->getCollider(0)->getIsTrigger();

    result["materialName"] = materialName();

    return result;
}

FurySphereObject *FurySphereObject::fromJson(const QJsonObject &_object, FuryWorld *_world)
{
    QString name = _object["name"].toString();
    QString materialName = _object["materialName"].toString();
    QString posStr = _object["position"].toString();
    posStr.remove("(");
    posStr.remove(")");
    posStr.remove(" ");

    glm::vec3 position(posStr.section(";", 0, 0).toFloat(),
                       posStr.section(";", 1, 1).toFloat(),
                       posStr.section(";", 2, 2).toFloat());

    float scale = _object["scale"].toDouble();

    QString physTypeStr = _object["physicsType"].toString();
    rp3d::BodyType physicsType = rp3d::BodyType::STATIC;
    bool physicsEnabled = true;

    if (physTypeStr == "NONE")
    {
        physicsEnabled = false;
    }
    else if (physTypeStr == "STATIC")
    {
        physicsType = rp3d::BodyType::STATIC;
    }
    else if (physTypeStr == "DYNAMIC")
    {
        physicsType = rp3d::BodyType::DYNAMIC;
    }
    else if (physTypeStr == "KINEMATIC")
    {
        physicsType = rp3d::BodyType::KINEMATIC;
    }

    bool isTrigger = _object["isTrigger"].toBool();

    FurySphereObject* result = new FurySphereObject(_world, position, scale);
    result->setObjectName(name);
    result->setMaterialName(materialName);

    if (physicsEnabled)
    {
        result->Setup_physics(physicsType);
    }

    if (isTrigger)
    {
        result->physicsBody()->getCollider(0)->setIsTrigger(true);
    }

    return result;
}






Shader* initSphereShader()
{
    static Shader* default_shader = nullptr;

    if (default_shader == nullptr)
    {
//        default_shader = new Shader("shaders/testMaterialShader.vs", "shaders/testMaterialShader.frag");
        default_shader = new Shader("shaders/pbr/2.2.2.pbr.vs", "shaders/pbr/2.2.2.pbr.fs");
    }

    return default_shader;
}
