#include "FurySphereObject.h"

#include <QVector>

#include "Shader.h"
#include "FuryWorld.h"


Shader* initSphereShader();
void initSphereModel();
static GLuint sphereVAO = 0;
static GLuint sphereVBO = 0;
static unsigned int sphereVertexCount = 0;


FurySphereObject::FurySphereObject(FuryWorld *_world) :
    FurySphereObject(_world, glm::vec3(0, 0, 0)) {}
FurySphereObject::FurySphereObject(FuryWorld *_world, const glm::vec3& _position) :
    FurySphereObject(_world, _position, 1) {}
FurySphereObject::FurySphereObject(FuryWorld *_world, const glm::vec3& _position, double _scale) :
    FuryObject(_world, _position)
{
    setScales(glm::vec3(_scale, _scale, _scale));
    setShader(initSphereShader());

    initSphereModel();
    m_VAO = sphereVAO;
    m_VBO = sphereVBO;
    m_vertexCount = sphereVertexCount;
    setRenderType(GL_TRIANGLE_STRIP);
    setIsDrawElements(true);

    setModelName("sphere");


    const rp3d::Vector3& pos = physicsBody()->getTransform().getPosition();
    rp3d::Quaternion rot = rp3d::Quaternion::fromEulerAngles(rotate().x, rotate().y, rotate().z);
    rp3d::Transform transform(pos, rot);
    physicsBody()->setTransform(transform);
}

void FurySphereObject::tick(double /*_dt*/)
{
    const reactphysics3d::Transform& physics_box_transform = physicsBody()->getTransform();
    const reactphysics3d::Vector3& physics_box_position = physics_box_transform.getPosition();
    const reactphysics3d::Quaternion& physics_box_rotate = physics_box_transform.getOrientation();

    this->m_position = glm::vec3(physics_box_position.x, physics_box_position.y, physics_box_position.z);
    setRotate(glm::vec3(physics_box_rotate.x, physics_box_rotate.y, physics_box_rotate.z));
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




void initSphereModel()
{
    if (sphereVAO != 0)
    {
        return;
    }

    glGenVertexArrays(1, &sphereVAO);

    unsigned int ebo;
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &ebo);

    QVector<glm::vec3> positions;
    QVector<glm::vec2> uv;
    QVector<glm::vec3> normals;
    QVector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    sphereVertexCount = (unsigned int)indices.size();

    QVector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

}


Shader* initSphereShader()
{
    static Shader* default_shader = nullptr;

    if (default_shader == nullptr)
    {
        default_shader = new Shader("shaders/testMaterialShader.vs", "shaders/testMaterialShader.frag");
    }

    return default_shader;
}
