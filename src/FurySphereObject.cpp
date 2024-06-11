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

void FurySphereObject::tick(double /*_dt*/)
{
    const reactphysics3d::Transform& physics_box_transform = physicsBody()->getTransform();
    const reactphysics3d::Vector3& physics_box_position = physics_box_transform.getPosition();

    this->m_position = glm::vec3(physics_box_position.x, physics_box_position.y, physics_box_position.z);
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
