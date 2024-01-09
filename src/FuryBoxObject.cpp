// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FuryBoxObject.h"
#include "TextureLoader.h"
#include "Shader.h"
#include "FuryWorld.h"


void renderCube();
void createBoxModel();
Shader* init_shader();

static GLuint default_texture_id = 0;
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
    m_VAO = cubeVAO;
    m_VBO = cubeVBO;
    m_vertexCount = 36;

    setModelName("cube");


    const rp3d::Vector3& pos = physicsBody()->getTransform().getPosition();
    rp3d::Quaternion rot = rp3d::Quaternion::fromEulerAngles(rotate().x, rotate().y, rotate().z);
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

void FuryBoxObject::tick(double /*dt*/)
{
    const reactphysics3d::Transform& physics_box_transform = physicsBody()->getTransform();
    const reactphysics3d::Vector3& physics_box_position = physics_box_transform.getPosition();
    const reactphysics3d::Quaternion& physics_box_rotate = physics_box_transform.getOrientation();

    rp3d::decimal angle;
    rp3d::Vector3 axis;
    physics_box_rotate.getRotationAngleAxis(angle, axis);

    this->m_position = glm::vec3(physics_box_position.x, physics_box_position.y, physics_box_position.z);
    setRotate(glm::vec3(axis.x, axis.y, axis.z));
    setAngle(angle);
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


static Shader* default_shader{nullptr};

Shader* init_shader() {
    if (default_shader == nullptr) {
        default_shader = new Shader("shaders/testMaterialShader.vs", "shaders/testMaterialShader.frag");
    }
    if (default_texture_id == 0) {
        TextureLoader loader;
        int tex_w, tex_h;
        auto data_texture = loader.LoadDataFromFile("textures/box_texture3_orig.png", tex_w, tex_h);
        default_texture_id = loader.BindDataToTexture(data_texture, tex_w, tex_h);
        // default_texture_id = SOIL_load_OGL_texture("textures/box_texture3_orig.png", SOIL_LOAD_RGBA, 0, SOIL_FLAG_INVERT_Y);
    }
    return default_shader;
}
