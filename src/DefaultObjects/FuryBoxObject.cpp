// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FuryBoxObject.h"
#include "Shader.h"
#include "FuryWorld.h"


void renderCube();
void createBoxModel();

static GLuint cubeVAO = 0;
static GLuint cubeVBO = 0;


FuryBoxObject::FuryBoxObject(FuryWorld *_world, FuryObject *_parent, bool _withoutJoint) :
    FuryBoxObject(_world, 1, _parent, _withoutJoint) { }
FuryBoxObject::FuryBoxObject(FuryWorld *_world, double _scale, FuryObject *_parent, bool _withoutJoint) :
    FuryBoxObject(_world, _scale, _scale, _scale, _parent, _withoutJoint) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, double _scaleX, double _scaleY, double _scaleZ, FuryObject *_parent, bool _withoutJoint) :
    FuryBoxObject(_world, glm::vec3(0, 0, 0), _scaleX, _scaleY, _scaleZ, _parent, _withoutJoint) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, FuryObject *_parent, bool _withoutJoint) :
    FuryBoxObject(_world, _position, 1, _parent, _withoutJoint) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, double _scale, FuryObject *_parent, bool _withoutJoint) :
    FuryBoxObject(_world, _position, _scale, _scale, _scale, _parent, _withoutJoint) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, double _scaleX, double _scaleY, double _scaleZ, FuryObject *_parent, bool _withoutJoint) :
    FuryBoxObject(_world, _position, glm::vec3(_scaleX, _scaleY, _scaleZ), glm::vec3(0, 0, 0), _parent, _withoutJoint) {}


FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, const glm::vec3& _scales, const glm::vec3& _rotate, FuryObject *_parent, bool _withoutJoint) :
    FuryObject(_world, _position, _parent, _withoutJoint)
{
    setShaderName("pbrShader");

    setInitLocalRotation(_rotate);
    setScales(_scales);

    createBoxModel();

    setModelName("cube");


    const rp3d::Vector3& pos = physicsBody()->getTransform().getPosition();
    rp3d::Quaternion rot = rp3d::Quaternion::fromEulerAngles(_rotate.x, _rotate.y, _rotate.z);
    rp3d::Transform transform(pos, rot);
    physicsBody()->setTransform(transform);
}


FuryBoxObject::~FuryBoxObject()
{

}

void FuryBoxObject::initPhysics(reactphysics3d::BodyType _type)
{
    FuryObject::initPhysics(_type);

    const reactphysics3d::Vector3 halfExtents(scales().x / 2.0f, scales().y / 2.0f, scales().z / 2.0f);
    reactphysics3d::BoxShape* boxShape = world()->physicsCommon()->createBoxShape(halfExtents);
    reactphysics3d::Transform transform_boxShape = reactphysics3d::Transform::identity();
    reactphysics3d::Collider* collider_box;
    collider_box = physicsBody()->addCollider(boxShape, transform_boxShape);
    collider_box->getMaterial().setFrictionCoefficient(0.4f);
}





// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
void createBoxModel()
{
    if (cubeVAO != 0)
    {
        return;
    }

    float vertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 4.0f / 4.0f, 1.0f / 3.0f, // bottom-left
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 3.0f / 4.0f, 2.0f / 3.0f, // top-right
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 3.0f / 4.0f, 1.0f / 3.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 3.0f / 4.0f, 2.0f / 3.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 4.0f / 4.0f, 1.0f / 3.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 4.0f / 4.0f, 2.0f / 3.0f, // top-left
        // front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f / 4.0f, 1.0f / 3.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 2.0f / 4.0f, 1.0f / 3.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 2.0f / 4.0f, 2.0f / 3.0f, // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 2.0f / 4.0f, 2.0f / 3.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f / 4.0f, 2.0f / 3.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f / 4.0f, 1.0f / 3.0f, // bottom-left
        // left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f / 4.0f, 2.0f / 3.0f, // top-right
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f / 4.0f, 2.0f / 3.0f, // top-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f / 4.0f, 1.0f / 3.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f / 4.0f, 1.0f / 3.0f, // bottom-left
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f / 4.0f, 1.0f / 3.0f, // bottom-right
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f / 4.0f, 2.0f / 3.0f, // top-right
        // right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 2.0f / 4.0f, 2.0f / 3.0f, // top-left
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 3.0f / 4.0f, 1.0f / 3.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 3.0f / 4.0f, 2.0f / 3.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 3.0f / 4.0f, 1.0f / 3.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 2.0f / 4.0f, 2.0f / 3.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 2.0f / 4.0f, 1.0f / 3.0f, // bottom-left
        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f / 4.0f, 0.0f / 3.0f, // top-right
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 2.0f / 4.0f, 0.0f / 3.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 2.0f / 4.0f, 1.0f / 3.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 2.0f / 4.0f, 1.0f / 3.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f / 4.0f, 1.0f / 3.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f / 4.0f, 0.0f / 3.0f, // top-right
        // top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f / 4.0f, 3.0f / 3.0f, // top-left
         0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f, 2.0f / 4.0f, 2.0f / 3.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 2.0f / 4.0f, 3.0f / 3.0f, // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 2.0f / 4.0f, 2.0f / 3.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f / 4.0f, 3.0f / 3.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f / 4.0f, 2.0f / 3.0f  // bottom-left
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        createBoxModel();
    }

    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
