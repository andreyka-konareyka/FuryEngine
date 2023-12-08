#ifndef PARTICLE_H
#define PARTICLE_H


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Camera.h"

void InitParticleMesh();

class Particle {
public:
    glm::vec3 position;
    double scale;
    glm::vec3 speed;
    glm::vec4 color;
    float life_time;
    bool is_living;

    Particle(glm::vec3& _position, double _scale, glm::vec3& _speed, glm::vec4& _color, GLuint _textureId, float _lifeTime, Shader* _shader);
    void Tick(float _dt);
    void Draw(Camera& _camera, int& _width, int& _height);
    void Draw(Shader& _shader);
    bool Is_Dead();

private:
    // render data
    unsigned int VBO, EBO, VAO;
    Shader* particleShader;
    GLuint texture_id;

    //void SetupMesh() {
    //	GLfloat vertices[] = {
    //		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    //		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    //		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    //		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    //		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    //		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    //	};

    //	glGenVertexArrays(1, &VAO);
    //	glGenBuffers(1, &VBO);
    //	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //	glBindVertexArray(VAO);

    //	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    //	glEnableVertexAttribArray(0);
    //	// vertex texture coords
    //	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    //	glEnableVertexAttribArray(1);
    //	glBindVertexArray(0);
    //}
};

#endif // PARTICLE_H
