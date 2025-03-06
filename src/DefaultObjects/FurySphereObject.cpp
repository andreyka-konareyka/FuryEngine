#include "FurySphereObject.h"

#include <QVector>

#include "Shader.h"
#include "FuryWorld.h"


FurySphereObject::FurySphereObject(FuryWorld *_world, FuryObject *_parent, bool _withoutJoint) :
    FurySphereObject(_world, glm::vec3(0, 0, 0), _parent, _withoutJoint) {}
FurySphereObject::FurySphereObject(FuryWorld *_world, const glm::vec3& _position, FuryObject *_parent, bool _withoutJoint) :
    FurySphereObject(_world, _position, 1, _parent, _withoutJoint) {}
FurySphereObject::FurySphereObject(FuryWorld *_world, const glm::vec3& _position, double _scale, FuryObject *_parent, bool _withoutJoint) :
    FuryObject(_world, _position, _parent, _withoutJoint)
{
    setScales(glm::vec3(_scale, _scale, _scale));
    setShaderName("pbrShader");

    setModelName("sphere");


    const rp3d::Vector3& pos = physicsBody()->getTransform().getPosition();

    rp3d::Quaternion rot = rp3d::Quaternion::fromEulerAngles(0, 0, 0);
    rp3d::Transform transform(pos, rot);
    physicsBody()->setTransform(transform);
}

void FurySphereObject::initPhysics(reactphysics3d::BodyType _type)
{
    FuryObject::initPhysics(_type);

    reactphysics3d::SphereShape* sphereShape = world()->physicsCommon()->createSphereShape(scales().x);
    reactphysics3d::Transform transform_shape = reactphysics3d::Transform::identity();
    reactphysics3d::Collider* collider1;
    collider1 = physicsBody()->addCollider(sphereShape, transform_shape);

    collider1->getMaterial().setFrictionCoefficient(0.25);
}
