#include "Particle.h"

static unsigned int defaultVBO, defaultEBO, defaultVAO;

void InitParticleMesh() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
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

Particle::Particle(glm::vec3& _position, double _scale, glm::vec3& _speed, glm::vec4& _color, GLuint _textureId, float _lifeTime, Shader* _shader) {
    this->particleShader = _shader;
    this->position = _position;
    this->scale = _scale;
    this->speed = _speed;
    this->color = _color;
    this->texture_id = _textureId;
    this->life_time = _lifeTime;
    this->is_living = true;

    VBO = defaultVBO;
    EBO = defaultEBO;
    VAO = defaultVAO;
    //SetupMesh();
}

void Particle::Tick(float _dt) {
    this->life_time -= _dt;
    if (this->life_time < 0) {
        this->is_living = false;
        return;
    }
    this->position += this->speed * _dt;
}

void Particle::Draw(Camera& _camera, int& _width, int& _height) {
    glm::mat4 projection = glm::perspective(glm::radians(_camera.Zoom), (float)_width / (float)_height, 0.1f, 300.0f);
    glm::mat4 view = _camera.GetViewMatrix();

    particleShader->Use();
    particleShader->setMat4("projection", projection);
    particleShader->setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    glm::vec3 particle_pos = position;
    glm::vec3 camera_pos = _camera.Position;

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

    model = glm::scale(model, glm::vec3(scale, scale, scale));
    particleShader->setMat4("model", model);

    particleShader->setVec4("objectColor", color);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    //Draw(particleShader);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Particle::Draw(Shader& /*_shader*/) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

bool Particle::Is_Dead() {
    return !(is_living);
}
