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
Shader* init_shader();

static GLuint cubeVAO = 0;
static GLuint cubeVBO = 0;


FuryBoxObject::FuryBoxObject(FuryWorld *_world) :
    FuryBoxObject(_world, 1) { }
FuryBoxObject::FuryBoxObject(FuryWorld *_world, double _scale) :
    FuryBoxObject(_world, _scale, _scale, _scale) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, double _scaleX, double _scaleY, double _scaleZ) :
    FuryBoxObject(_world, glm::vec3(0, 0, 0), _scaleX, _scaleY, _scaleZ) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position) :
    FuryBoxObject(_world, _position, 1) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, double _scale) :
    FuryBoxObject(_world, _position, _scale, _scale, _scale) {}
FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, double _scaleX, double _scaleY, double _scaleZ) :
    FuryBoxObject(_world, _position, glm::vec3(_scaleX, _scaleY, _scaleZ), glm::vec3(0, 0, 0)) {}


FuryBoxObject::FuryBoxObject(FuryWorld *_world, const glm::vec3& _position, const glm::vec3& _scales, const glm::vec3& _rotate) :
    FuryObject(_world, _position)
{
    setShader(init_shader());

    setRotate(_rotate);
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

void FuryBoxObject::Setup_physics(reactphysics3d::BodyType _type)
{
    physicsBody()->setType(_type);
    const reactphysics3d::Vector3 halfExtents(scales().x / 2.0f, scales().y / 2.0f, scales().z / 2.0f);
    reactphysics3d::BoxShape* boxShape = world()->physicsCommon()->createBoxShape(halfExtents);
    reactphysics3d::Transform transform_boxShape = reactphysics3d::Transform::identity();
    reactphysics3d::Collider* collider_box;
    collider_box = physicsBody()->addCollider(boxShape, transform_boxShape);
    collider_box->getMaterial().setFrictionCoefficient(0.4f);
}

Shader *FuryBoxObject::defaultShader()
{
    return init_shader();
}

QJsonObject FuryBoxObject::toJson()
{
    QJsonObject result;

    result["name"] = name();
    result["position"] = QString("(%1; %2; %3)").arg(m_position.x)
                                                .arg(m_position.y)
                                                .arg(m_position.z);

    result["scales"] = QString("(%1; %2; %3)").arg(scales().x)
                                              .arg(scales().y)
                                              .arg(scales().z);

    result["rotate"] = QString("(%1; %2; %3)").arg(rotate().x)
                                              .arg(rotate().y)
                                              .arg(rotate().z);

    result["textureScales"] = QString("(%1; %2)").arg(textureScales().x)
                                                 .arg(textureScales().y);

    if (physicsBody()->getType() == rp3d::BodyType::STATIC)
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

FuryBoxObject *FuryBoxObject::fromJson(const QJsonObject &_object, FuryWorld *_world)
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

    QString scalesStr = _object["scales"].toString();
    scalesStr.remove("(");
    scalesStr.remove(")");
    scalesStr.remove(" ");

    glm::vec3 scales(scalesStr.section(";", 0, 0).toFloat(),
                     scalesStr.section(";", 1, 1).toFloat(),
                     scalesStr.section(";", 2, 2).toFloat());

    QString rotateStr = _object["rotate"].toString();
    rotateStr.remove("(");
    rotateStr.remove(")");
    rotateStr.remove(" ");

    glm::vec3 rotate(rotateStr.section(";", 0, 0).toFloat(),
                     rotateStr.section(";", 1, 1).toFloat(),
                     rotateStr.section(";", 2, 2).toFloat());


    QString tsStr = _object["textureScales"].toString();
    tsStr.remove("(");
    tsStr.remove(")");
    tsStr.remove(" ");

    glm::vec2 textureScales(tsStr.section(";", 0, 0).toFloat(),
                            tsStr.section(";", 1, 1).toFloat());

    QString physTypeStr = _object["physicsType"].toString();
    rp3d::BodyType physicsType = rp3d::BodyType::STATIC;
    if (physTypeStr == "STATIC")
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

    FuryBoxObject* result = new FuryBoxObject(_world, position, scales, rotate);
    result->setName(name);
    result->setMaterialName(materialName);
    result->setTextureScales(textureScales);
    result->Setup_physics(physicsType);

    if (isTrigger)
    {
        result->physicsBody()->getCollider(0)->setIsTrigger(true);
    }

    return result;
}

void FuryBoxObject::tick(double /*dt*/)
{
    const reactphysics3d::Transform& physics_box_transform = physicsBody()->getTransform();
    const reactphysics3d::Vector3& physics_box_position = physics_box_transform.getPosition();

    this->m_position = glm::vec3(physics_box_position.x, physics_box_position.y, physics_box_position.z);
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


static Shader* default_shader = nullptr;

Shader* init_shader()
{
    if (default_shader == nullptr)
    {
//        default_shader = new Shader("shaders/testMaterialShader.vs", "shaders/testMaterialShader.frag");
        default_shader = new Shader("shaders/pbr/2.2.2.pbr.vs", "shaders/pbr/2.2.2.pbr.fs");
    }

    return default_shader;
}
