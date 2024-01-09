#include "FuryMesh.h"

#include "Shader.h"
#include "FuryMaterial.h"


FuryMesh::FuryMesh(const QVector<Vertex> &_vertices,
                   const QVector<unsigned int> &_indices,
                   FuryMaterial *_material) :
    m_vertices(_vertices),
    m_indices(_indices),
    m_material(_material)
{

}

void FuryMesh::draw(Shader *_shader, FuryMaterial *_material)
{
    // Установка материала
    if (_material == nullptr)
    {
        m_material->setShaderMaterial(_shader);
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
