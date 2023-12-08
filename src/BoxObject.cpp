// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BoxObject.h"
#include "TextureLoader.h"


void renderCube();
void createBoxModel();
Shader* init_shader();

static GLuint default_texture_id = 0;
static GLuint cubeVAO = 0;
static GLuint cubeVBO = 0;


BoxObject::BoxObject() : BoxObject(1) { }
BoxObject::BoxObject(double _size) : BoxObject(_size, _size, _size) {}
BoxObject::BoxObject(double _sizeX, double _sizeY, double _sizeZ) : BoxObject(glm::vec3(0, 0, 0), _sizeX, _sizeY, _sizeZ) {}
BoxObject::BoxObject(const glm::vec3& _position) : BoxObject(_position, 1) {}
BoxObject::BoxObject(const glm::vec3& _position, double _size) : BoxObject(_position, _size, _size, _size) {}
BoxObject::BoxObject(const glm::vec3& _position, double _sizeX, double _sizeY, double _sizeZ) : BoxObject(_position, glm::vec3(_sizeX, _sizeY, _sizeZ), glm::vec3(0, 0, 0)) {}


BoxObject::BoxObject(const glm::vec3& _position, const glm::vec3& _size, const glm::vec3& _rotate) :
    Object(_position)
{
    setShader(init_shader());

    setRotate(_rotate);
    setSizes(_size);

    createBoxModel();
    m_VAO = cubeVAO;
    m_VBO = cubeVBO;
    m_vertexCount = 36;
}


BoxObject::~BoxObject()
{

}

void BoxObject::Setup_physics(reactphysics3d::PhysicsCommon& _physCommon, reactphysics3d::PhysicsWorld* _physWorld, reactphysics3d::BodyType _type)
{
    setPhysicsEnabled(true);

    reactphysics3d::Vector3 position_body(this->m_position.x, this->m_position.y, this->m_position.z);
    reactphysics3d::Quaternion orientation_body = reactphysics3d::Quaternion::fromEulerAngles(rotate().x, rotate().y, rotate().z);
    reactphysics3d::Transform transform_body(position_body, orientation_body);

    setPhysicsBody(_physWorld->createRigidBody(transform_body));

    physicsBody()->setType(_type);
    const reactphysics3d::Vector3 halfExtents(sizes().x / 2.0f, sizes().y / 2.0f, sizes().z / 2.0f);
    reactphysics3d::BoxShape* boxShape = _physCommon.createBoxShape(halfExtents);
    reactphysics3d::Transform transform_boxShape = reactphysics3d::Transform::identity();
    reactphysics3d::Collider* collider_box;
    collider_box = physicsBody()->addCollider(boxShape, transform_boxShape);
    collider_box->getMaterial().setFrictionCoefficient(0.4f);
}

void BoxObject::Tick(double /*dt*/) {
    if (physicsEnabled())
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
}

void BoxObject::Draw(Camera* _camera, int _windowWidth, int _windowHeight, glm::vec3& _dirlight, glm::mat4& _lightSpaceMatrix, GLuint _depthMap) {
    //this->LoadDataInOpenGl(camera, window_width, window_height, dirlight);

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    shader()->Use();


    shader()->setVec3("viewPos", (*_camera).Position);
    shader()->setFloat("material.shininess", 128.0f); // 32.0 - default
    //floorShader.setInt("material.diffuse", 2);


    //floorShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);-8.0f, 6.0f, -3.0f
    shader()->setVec3("dirLight.direction", glm::vec3(0, 0, 0) -_dirlight);
    shader()->setVec3("dirLight.ambient", 0.35f, 0.35f, 0.35f);
    shader()->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader()->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    //floorShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    //floorShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    //floorShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    shader()->setVec3("pointLights[0].position", pointLightPositions[0]);
    shader()->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader()->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader()->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader()->setFloat("pointLights[0].constant", 1.0f);
    shader()->setFloat("pointLights[0].linear", 0.09f);
    shader()->setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    shader()->setVec3("pointLights[1].position", pointLightPositions[1]);
    shader()->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader()->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader()->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader()->setFloat("pointLights[1].constant", 1.0f);
    shader()->setFloat("pointLights[1].linear", 0.09f);
    shader()->setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    shader()->setVec3("pointLights[2].position", pointLightPositions[2]);
    shader()->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader()->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader()->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader()->setFloat("pointLights[2].constant", 1.0f);
    shader()->setFloat("pointLights[2].linear", 0.09f);
    shader()->setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    shader()->setVec3("pointLights[3].position", pointLightPositions[3]);
    shader()->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader()->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shader()->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader()->setFloat("pointLights[3].constant", 1.0f);
    shader()->setFloat("pointLights[3].linear", 0.09f);
    shader()->setFloat("pointLights[3].quadratic", 0.032f);


    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians((*_camera).Zoom), (float)_windowWidth / (float)_windowHeight, 0.1f, 300.0f);
    glm::mat4 view = (*_camera).GetViewMatrix();
    shader()->setMat4("projection", projection);
    shader()->setMat4("view", view);



    //float near_plane = 1.0f, far_plane = 25.0f;
    //glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    //glm::mat4 lightView = glm::lookAt(dirlight,
        //glm::vec3(0.0f, 0.0f, 0.0f),
        //glm::vec3(0.0f, 1.0f, 0.0f));

    //glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    //glUniformMatrix4fv(glGetUniformLocation((*(this->shader)).Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader()->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_lightSpaceMatrix));


    //float angle_x = -3.14 / 2;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->m_position); // translate it down so it's at the center of the scene

    if (physicsEnabled())
    {
        reactphysics3d::Vector3 axis_rotate;
        reactphysics3d::decimal angle_rotate;
        physicsBody()->getTransform().getOrientation().getRotationAngleAxis(angle_rotate, axis_rotate);
        if (axis_rotate.x == 0 and axis_rotate.y == 0 and axis_rotate.z == 0) {
            model = glm::rotate(model, rotate().y, glm::vec3(0, 1, 0));
            model = glm::rotate(model, rotate().x, glm::vec3(1, 0, 0));
            model = glm::rotate(model, rotate().z, glm::vec3(0, 0, 1));
        }
        else {
            model = glm::rotate(model, static_cast<float>(angle_rotate), glm::vec3(axis_rotate.x, axis_rotate.y, axis_rotate.z));
        }
    }
    else
    {
        model = glm::rotate(model, rotate().y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, rotate().x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, rotate().z, glm::vec3(0, 0, 1));
    }

    model = glm::scale(model, sizes());	// it's a bit too big for our scene, so scale it down

    shader()->setMat4("model", model);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, default_texture_id);
    glUniform1i(glGetUniformLocation(shader()->Program, "diffuse_texture"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _depthMap);
    glUniform1i(glGetUniformLocation(shader()->Program, "shadowMap"), 1);
    //glUniform1i(glGetUniformLocation((*(this->shader)).Program, "diffuse_texture"), 0);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, depthMap);
    //glUniform1i(glGetUniformLocation((*(this->shader)).Program, "shadowMap"), 1);
    //glBindTexture(GL_TEXTURE_2D, depthMap);

    renderCube();

    glActiveTexture(GL_TEXTURE0);
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
        default_shader = new Shader("multiple_lights.vs", "multiple_lights.fs");
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
