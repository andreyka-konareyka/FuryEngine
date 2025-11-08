#include "Particle.h"

#include "FuryTexture.h"
#include "FuryTextureCache.h"


static unsigned int defaultVBO, defaultEBO, defaultVAO;

void InitParticleMesh()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &defaultVAO);
    glGenBuffers(1, &defaultVBO);
    glBindBuffer(GL_ARRAY_BUFFER, defaultVBO);
    glBindVertexArray(defaultVAO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // vertex texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}



Particle::Particle(const glm::vec3 &_position, double _scale,
                   const glm::vec3 &_speed, const glm::vec4 &_color,
                   const QString &_textureName, float _lifeTime, Shader *_shader)
{
    this->m_particleShader = _shader;
    this->m_position = _position;
    this->m_scale = _scale;
    this->m_speed = _speed;
    this->m_color = _color;
    this->m_textureName = _textureName;
    m_textureCache = new FuryTextureCache(m_textureName);
    this->m_lifeTime = _lifeTime;
    this->m_isLiving = true;

    m_VBO = defaultVBO;
    m_EBO = defaultEBO;
    m_VAO = defaultVAO;
    //SetupMesh();
}

void Particle::Tick(float _dt)
{
    this->m_lifeTime -= _dt;
    if (this->m_lifeTime < 0) {
        this->m_isLiving = false;
        return;
    }
    this->m_position += this->m_speed * _dt;
}

void Particle::Draw(Camera& _camera, int& _width, int& _height)
{
    glm::mat4 projection = glm::perspective(glm::radians(_camera.zoom()), (float)_width / (float)_height, 0.1f, 300.0f);
    glm::mat4 view = _camera.getViewMatrix();

    m_particleShader->use();
    m_particleShader->setMat4("projection", projection);
    m_particleShader->setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_position);

    glm::vec3 particle_pos = m_position;
    glm::vec3 camera_pos = _camera.position();

    glm::vec3 direct = camera_pos - particle_pos;


    float angle_y = atan(direct.x / direct.z);
    if (direct.z < 0) {
        angle_y += 3.14f;
    }
    float angle_x = (float)(-atan(direct.y / sqrt(pow(direct.x, 2) + pow(direct.z, 2)) * (direct.z / abs(direct.z))));
    if (direct.z < 0) {
        angle_x = -angle_x;
    }

    model = glm::rotate(model, angle_y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, angle_x, glm::vec3(1, 0, 0));

    model = glm::scale(model, glm::vec3(m_scale, m_scale, m_scale));
    m_particleShader->setMat4("model", model);

    m_particleShader->setVec4("objectColor", m_color);

    glBindTexture(GL_TEXTURE_2D, m_textureCache->texture().idOpenGL());

    //Draw(particleShader);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Particle::Draw(Shader& /*_shader*/)
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

bool Particle::Is_Dead()
{
    return !(m_isLiving);
}
