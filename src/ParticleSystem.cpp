#include <map>

#include "ParticleSystem.h"

#include <QList>
#include <QtAlgorithms>

static double chance_create_particle = 0.15;
static double default_particle_size = 1.0;

ParticleSystem::ParticleSystem(glm::vec3& pos, GLuint tex_id, int count) {
    this->position = pos;
    this->texture_id = tex_id;
    this->count = count;
    this->particle_shader = Shader("particle.vs", "particle.fs");
    //SetupParticleMesh();

    if ((double)(rand() % 100) / 100.0 < chance_create_particle) {
        glm::vec3 pos = position;
        pos += glm::vec3((((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5,
            (((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5,
            (((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5);
        glm::vec3 speed((((double)(rand() % 100) / 100.0) * 2 - 1),
            ((double)(rand() % 100) / 100.0 * 2),
            (((double)(rand() % 100) / 100.0) * 2 - 1));
        glm::vec4 color(0, 1, 0, 1);

        double particle_scale = default_particle_size;

        Particle new_particle(pos, particle_scale, speed, color, tex_id, 2 + (rand() % 10) / 10.0f * 2 - 1.0f, &particle_shader);
        //new_particle.SetMeshBuffers(particleVBO, particleEBO, particleVAO);
        m_particles.push_back(new_particle);
    }
}

void ParticleSystem::Tick(float deltaTime) {
    for (int i = 0; i < m_particles.size(); ++i) {
        m_particles[i].Tick(deltaTime);

        if (m_particles[i].Is_Dead()) {
            glm::vec3 pos = position;
            pos += glm::vec3((((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5,
                (((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5,
                (((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5);
            glm::vec3 speed((((double)(rand() % 100) / 100.0) * 2 - 1),
                ((double)(rand() % 100) / 100.0 * 2),
                (((double)(rand() % 100) / 100.0) * 2 - 1));
            glm::vec4 color(0, 1, 0, 1);

            double particle_scale = default_particle_size;

            Particle new_particle(pos, particle_scale, speed, color, texture_id, 2 + (rand() % 10) / 10.0f * 2 - 1.0f, &particle_shader);
            //new_particle.SetMeshBuffers(particleVBO, particleEBO, particleVAO);
            m_particles[i] = new_particle;
            /*_particles[i].position = pos;
            _particles[i].speed = speed;
            _particles[i].is_living = true;
            _particles[i].life_time = 2 + ((double)(rand() % 10) / 10.0 * 2 - 1.0);*/
        }
    }

    if (m_particles.size() < count) {
        if ((double)(rand() % 100) / 100.0 < chance_create_particle) {
            glm::vec3 pos = position;
            pos += glm::vec3((((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5,
                (((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5,
                (((double)(rand() % 100) / 100.0) * 2 - 1) * 0.5);
            glm::vec3 speed((((double)(rand() % 100) / 100.0) * 2 - 1),
                ((double)(rand() % 100) / 100.0 * 2),
                (((double)(rand() % 100) / 100.0) * 2 - 1));
            glm::vec4 color(0, 1, 0, 1);

            double particle_scale = default_particle_size;

            Particle new_particle(pos, particle_scale, speed, color, texture_id, 2 + (rand() % 10) / 10.0f * 2 - 1.0f, &particle_shader);
            //new_particle.SetMeshBuffers(particleVBO, particleEBO, particleVAO);
            m_particles.push_back(new_particle);
        }
    }

}

void ParticleSystem::Draw(Camera& camera, int& width, int& height) {
    // так как объекты полупрозрачные, мы их сортируем по отдалению от камеры и рисуем

    QList<QPair<float, Particle*>> sorted;
    for (unsigned int i = 0; i < m_particles.size(); i++){
        float distance = glm::length(camera.position() - m_particles[i].m_position);
        sorted.append(qMakePair(distance, &(m_particles[i])));
    }

    std::sort(sorted.begin(), sorted.end(), [](auto& p1, auto& p2){return p1.first < p2.first;});

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        it->second->Draw(camera, width, height);
    }

    sorted.clear();

    /*for (auto& particle : _particles) {
        particle.Draw(camera, width, height);
    }*/
}


void ParticleSystem::SetupParticleMesh() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    };

    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBindVertexArray(particleVAO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // vertex texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}
