#ifndef FURYMESH_H
#define FURYMESH_H


#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QVector>


class FuryMaterial;
class Shader;


//! Класс меша
class FuryMesh
{
public:
    //! Структура вершины
    struct Vertex {
        //! Позиция
        glm::vec3 m_position;
        //! Вектор нормали
        glm::vec3 m_normal;
        //! Текстурные координаты
        glm::vec2 m_texCoords;
        //! Тангент для нормалей
        glm::vec3 m_tangent;
        //! Битангент для нормалей
        glm::vec3 m_bitangent;
    };

    /*!
     * \brief Конструктор
     * \param[in] _vertices - Список вершин
     * \param[in] _indices - Список индексов
     * \param[in] _material - Материал
     */
    FuryMesh(const QVector<Vertex>& _vertices,
             const QVector<unsigned int>& _indices,
             FuryMaterial* _material);

    /*!
     * \brief Отрисовка меша
     * \param[in] _shader - Шейдер
     */
    void draw(Shader* _shader);

    //! Инициализация меша
    void setupMesh();

private:
    //! Список вершин
    QVector<Vertex> m_vertices;
    //! Список индексов
    QVector<unsigned int> m_indices;
    //! VAO для OpenGL
    GLuint VAO;
    //! VBO для OpenGL
    GLuint VBO;
    //! EBO для OpenGL
    GLuint EBO;

    //! Материал
    FuryMaterial* m_material;
};

#endif // FURYMESH_H
