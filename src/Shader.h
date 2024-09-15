#ifndef SHADER_H
#define SHADER_H


//#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <QString>

//! Класс шейдера OpenGL
class Shader
{
public:
    //! Конструктор по умолчанию
    Shader();

    /*!
     * \brief Конструктор
     * \param[in] _vertexPath - Путь к вершинному шейдеру
     * \param[in] _fragmentPath - Путь к фрагментному шейдеру
     * \param[in] _geometryPath - Путь к геометрическому шейдеру
     */
    Shader(const QString& _vertexPath, const QString& _fragmentPath,
           const QString& _geometryPath = QString());

    //! Активация шейдера
    void use();


    /*!
     * \brief Установка uniform булевой
     * \param[in] _name - Название поля
     * \param[in] _value - Значение
     */
    void setBool(const QString& _name, bool _value) const;

    /*!
     * \brief Установка uniform целого числа
     * \param[in] _name - Название поля
     * \param[in] _value - Значение
     */
    void setInt(const QString& _name, int _value) const;

    /*!
     * \brief Установка uniform числа с плавающей точкой
     * \param[in] _name - Название поля
     * \param[in] _value - Значение
     */
    void setFloat(const QString& _name, float _value) const;

    /*!
     * \brief Установка uniform 2D-вектора
     * \param[in] _name - Название поля
     * \param[in] _value - Значение
     */
    void setVec2(const QString& _name, const glm::vec2& _value) const;

    /*!
     * \brief Установка uniform 2D-вектора
     * \param[in] _name - Название поля
     * \param[in] _x - Значение x
     * \param[in] _y - Значение y
     */
    void setVec2(const QString& _name, float _x, float _y) const;

    /*!
     * \brief Установка uniform 3D-вектора
     * \param[in] _name - Название поля
     * \param[in] _value - Значение
     */
    void setVec3(const QString& _name, const glm::vec3& _value) const;

    /*!
     * \brief Установка uniform 3D-вектора
     * \param[in] _name - Название поля
     * \param[in] _x - Значение x
     * \param[in] _y - Значение y
     * \param[in] _z - Значение z
     */
    void setVec3(const QString& _name, float _x, float _y, float _z) const;

    /*!
     * \brief Установка uniform 4D-вектора
     * \param[in] _name - Название поля
     * \param[in] _value - Значение
     */
    void setVec4(const QString& _name, const glm::vec4& _value) const;

    /*!
     * \brief Установка uniform 4D-вектора
     * \param[in] _name - Название поля
     * \param[in] _x - Значение x
     * \param[in] _y - Значение y
     * \param[in] _z - Значение z
     * \param[in] _w - Значение w
     */
    void setVec4(const QString& _name, float _x, float _y, float _z, float _w);

    /*!
     * \brief Установка uniform 2D-матрицы
     * \param[in] _name - Название поля
     * \param[in] _matrix - Значение
     */
    void setMat2(const QString& _name, const glm::mat2& _matrix) const;

    /*!
     * \brief Установка uniform 3D-матрицы
     * \param[in] _name - Название поля
     * \param[in] _matrix - Значение
     */
    void setMat3(const QString& _name, const glm::mat3& _matrix) const;

    /*!
     * \brief Установка uniform 4D-матрицы
     * \param[in] _name - Название поля
     * \param[in] _matrix - Значение
     */
    void setMat4(const QString& _name, const glm::mat4& _matrix) const;

private:
    /*!
     * \brief Проверка наличия ошибок компиляции (линковки)
     * \param[in] _shader - Шейдер
     * \param[in] _type - Тип шейдера
     */
    void checkCompileErrors(GLuint _shader, const QString& _type);

private:
    //! Программа шейдера
    GLuint m_program;
};

#endif // SHADER_H
