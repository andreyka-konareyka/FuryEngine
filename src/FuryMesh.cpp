#include "FuryMesh.h"

#include "Shader.h"
#include "FuryModel.h"
#include "FuryMaterial.h"
#include "FuryMaterialManager.h"


FuryMesh::FuryMesh(FuryModel *_parentModel, const QVector<Vertex> &_vertices,
                   const QVector<unsigned int> &_indices,
                   const QString &_material,
                   const glm::mat4 &_transformation,
                   const glm::vec3 &_minVertex,
                   const glm::vec3 &_maxVertex) :
    m_parentModel(_parentModel),
    m_vertices(_vertices),
    m_indices(_indices),
    m_materialName(_material),
    m_transformation(_transformation),
    m_minimumVertex(_minVertex),
    m_maximumVertex(_maxVertex)
{

}

void FuryMesh::draw(Shader *_shader, FuryMaterial *_material)
{
    // Установка материала
    if (_material == nullptr)
    {
        FuryMaterialManager* manager = FuryMaterialManager::instance();
        FuryMaterial* myMaterial = manager->materialByName(m_materialName);
        myMaterial->setShaderMaterial(_shader);
    }
    else
    {
        _material->setShaderMaterial(_shader);
    }

    // Отрисовка
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void FuryMesh::drawShadowMap()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void FuryMesh::setupMesh()
{
    // Создание буферов, массивов openGL
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // Загрузка данных в буфер вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
                 &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
                 &m_indices[0], GL_STATIC_DRAW);


    // Установка вершинных атрибутов

    // Позиция вершины
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Вектор нормали вершины
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, m_normal));
    // Текстурные координаты вершины
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, m_texCoords));
    // Тангент вершины
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, m_tangent));
    // Битангент вершины
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, m_bitangent));
    glBindVertexArray(0);
}

glm::vec3 FuryMesh::meshCenter() const
{
    glm::vec3 center = (m_minimumVertex + m_maximumVertex) / 2.0f;
    return center;
}
