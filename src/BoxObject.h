#ifndef BOXOBJECT_H
#define BOXOBJECT_H


#include "Object.h"

#include "Camera.h"

#include <reactphysics3d/reactphysics3d.h>

class BoxObject : public Object {
public:
    BoxObject();
    BoxObject(double _size);
    BoxObject(double _sizeX, double _sizeY, double _sizeZ);
    BoxObject(const glm::vec3& _position);
    BoxObject(const glm::vec3& _position, double Zsize);
    BoxObject(const glm::vec3& _position, double _sizeX, double _sizeY, double _sizeZ);
    BoxObject(const glm::vec3& _position, const glm::vec3& _size, const glm::vec3& _rotate);

    ~BoxObject();

    void Tick(double _dt) override;
    void Draw(Camera* _camera, int _windowWidth, int _windowHeight,
              glm::vec3& _dirlight, glm::mat4& _lightSpaceMatrix, GLuint _depthMap) override;

    void Setup_physics(reactphysics3d::PhysicsCommon& _physCommon, reactphysics3d::PhysicsWorld* _physWorld, reactphysics3d::BodyType _type);
};
#endif // BOXOBJECT_H
