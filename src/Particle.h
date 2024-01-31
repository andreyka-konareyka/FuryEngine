#ifndef PARTICLE_H
#define PARTICLE_H


// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Camera.h"


//! Инициализация меша частицы в OpenGL
void InitParticleMesh();

class Particle {
public:
    glm::vec3 m_position;
    double m_scale;
    glm::vec3 m_speed;
    glm::vec4 m_color;
    float m_lifeTime;
    bool m_isLiving;

    Particle(const glm::vec3& _position, double _scale, const glm::vec3& _speed,
             const glm::vec4& _color, GLuint _textureId, float _lifeTime, Shader *_shader);
    void Tick(float _dt);
    void Draw(Camera& _camera, int& _width, int& _height);
    void Draw(Shader& _shader);
    bool Is_Dead();

private:
    // render data
    unsigned int m_VBO, m_EBO, m_VAO;
    Shader* m_particleShader;
    GLuint m_textureId;
};

#endif // PARTICLE_H
