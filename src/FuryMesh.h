#ifndef FURYMESH_H
#define FURYMESH_H


//#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QVector>


class FuryModel;
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
     * \param[in] _parentModel - Родительская модель
     * \param[in] _vertices - Список вершин
     * \param[in] _indices - Список индексов
     * \param[in] _material - Материал
     * \param[in] _transformation - Матрица трансформации меша относительно родителя
     * \param[in] _minVertex - Минимальная вершина куба, в который вписан меш
     * \param[in] _maxVertex - Максимальная вершина куба, в который вписан меш
     */
    FuryMesh(FuryModel* _parentModel,
             const QVector<Vertex>& _vertices,
             const QVector<unsigned int>& _indices,
             const QString& _material,
             const glm::mat4& _transformation,
             const glm::vec3& _minVertex,
             const glm::vec3& _maxVertex);

    //! Отрисовка меша
    void draw();

    //! Инициализация меша
    void setupMesh();

    /*!
     * \brief Получить матрицу трансформации меша
     * \return Возвращает матрицу трансформации меша
     */
    inline const glm::mat4& transformation() const
    { return m_transformation; }

    /*!
     * \brief Установить матрицу трансформации меша
     * \param[in] _transformation - Матрица трансформации меша
     */
    inline void setTransformation(const glm::mat4& _transformation)
    { m_transformation = _transformation; }

    /*!
     * \brief Получить родительскую модель
     * \return Возвращает родительскую модель
     */
    inline FuryModel* parentModel() const
    { return m_parentModel; }

    /*!
     * \brief Получить название материала
     * \return Возвращает название материала
     */
    inline const QString& materialName() const
    { return m_materialName; }

    /*!
     * \brief Получить центр меша
     * \return Возвращает центр меша
     */
    glm::vec3 meshCenter() const;

private:
    FuryModel* m_parentModel;

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
    QString m_materialName;
    //! Матрица трансформации меша относительно родителя
    glm::mat4 m_transformation;

    //! Минимальная координата куба, в которую вписана модель
    glm::vec3 m_minimumVertex;
    //! Максимальная координата куба, в которую вписана модель
    glm::vec3 m_maximumVertex;
};

#endif // FURYMESH_H
