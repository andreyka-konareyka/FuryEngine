#ifndef OBJECT_H
#define OBJECT_H


#include <string>
#include <vector>
#include <map>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <reactphysics3d/reactphysics3d.h>

#include "Camera.h"
#include "Shader.h"

#include <QString>

class Object {
public:
    Object();
    Object(const glm::vec3& _position);

    virtual ~Object();

    virtual void Tick(double /*_dt*/) {};
    virtual void Draw(Camera* /*_camera*/, int /*_windowWidth*/, int /*_windowHeight*/,
                      glm::vec3& /*_dirlight*/, glm::mat4& /*_shadowMap*/, GLuint /*_depthMap*/) {};

    virtual void keyPressEvent(int /*_keyCode*/) {};
    virtual void keyReleaseEvent(int /*_keyCode*/) {};

    inline const glm::vec3& getPosition() const
    { return m_position; }

    inline void setPosition(const glm::vec3& _newPosition)
    { m_position = _newPosition; }

    inline GLuint VAO() const
    { return m_VAO; }

    inline GLuint VBO() const
    { return m_VBO; }

    inline unsigned int vertexCount() const
    { return m_vertexCount; }

    inline unsigned int renderType() const
    { return m_renderType; }

    inline void setRenderType(int _renderType)
    { m_renderType = _renderType; }

    inline bool isDrawElements() const
    { return m_isDrawElements; }

    inline void setIsDrawElements(bool _isDrawElements)
    { m_isDrawElements = _isDrawElements; }

    inline Shader* shader() const
    { return m_shader; }

    inline void setShader(Shader* _shader)
    { m_shader = _shader; }

    inline bool physicsEnabled() const
    { return m_physicsEnabled; }

    inline void setPhysicsEnabled(bool _physicsEnabled)
    { m_physicsEnabled = _physicsEnabled; }

    inline reactphysics3d::RigidBody* physicsBody() const
    { return m_physicsBody; }

    inline void setPhysicsBody(reactphysics3d::RigidBody* _physicsBody)
    { m_physicsBody = _physicsBody; }

    inline const QString& textureName() const
    { return m_textureName; }

    inline void setTextureName(const QString& _textureName)
    { m_textureName = _textureName; }

    inline const glm::vec3& rotate() const
    { return m_rotate; }

    inline void setRotate(const glm::vec3& _rotate)
    { m_rotate = _rotate; }

    inline float angle() const
    { return m_angle; }

    inline void setAngle(float _angle)
    { m_angle = _angle; }

    inline const glm::vec3& sizes() const
    { return m_sizes; }

    inline void setSizes(const glm::vec3& _sizes)
    { m_sizes = _sizes; }


protected:
    glm::vec3 m_position;
    //reactphysics3d::RigidBody* physics_body;

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    unsigned int m_vertexCount;

private:
    Shader* m_shader{ nullptr };
    bool m_physicsEnabled = false;
    reactphysics3d::RigidBody* m_physicsBody = nullptr;

    int m_renderType = GL_TRIANGLES;
    bool m_isDrawElements = false;


    QString m_textureName = "defaultBoxTexture";
    glm::vec3 m_rotate;
    float m_angle;
    glm::vec3 m_sizes;
};

#endif // OBJECT_H
