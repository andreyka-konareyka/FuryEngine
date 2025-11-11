#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H


#include <QVector>

#include "Shader.h"
#include "Particle.h"

class ParticleSystem {
public:
    ParticleSystem(glm::vec3& position, const QString& _textureName, int count);
    void Tick(float deltaTime);
    void Draw(Camera& camera, int width, int height);
    QVector<Particle*> m_particles;

private:
    double current_time;
    glm::vec3 position;
    GLuint texture_id;
    QString m_textureName;
    int count;
    Shader particle_shader;

    unsigned int particleVBO, particleEBO, particleVAO;
};
#endif // PARTICLESYSTEM_H
