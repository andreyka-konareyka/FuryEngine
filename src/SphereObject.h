#ifndef SPHEREOBJECT_H
#define SPHEREOBJECT_H

#include "Object.h"

class SphereObject : public Object
{
public:
    SphereObject();
    SphereObject(const glm::vec3& _position);
    SphereObject(const glm::vec3& _position, double _size);

    void Tick(double _dt) override;

    void Setup_physics(reactphysics3d::PhysicsCommon& _physCommon, reactphysics3d::PhysicsWorld* _physWorld, reactphysics3d::BodyType _type);

private:

};



#endif // SPHEREOBJECT_H
