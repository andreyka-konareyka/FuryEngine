#include "Shader.h"

#include "Logger/FuryLogger.h"

#include <QFile>


Shader::Shader() :
    m_program(0)
{

}

Shader::Shader(const QString &_vertexPath, const QString &_fragmentPath,
               const QString &_geometryPath) :
    m_program(0)
{
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint geometry = 0;


    QFile vShaderFile(_vertexPath);
    if (vShaderFile.open(QIODevice::ReadOnly))
    {
        QByteArray baVertexCode = vShaderFile.readAll();
        vShaderFile.close();

        const char* vShaderCode = baVertexCode.constData();

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
    }
    else
    {
        Debug(ru("Ошибка при чтении вершинного шейдера.\n(%1)").arg(_vertexPath));
    }

    QFile fShaderFile(_fragmentPath);
    if (fShaderFile.open(QIODevice::ReadOnly))
    {
        QByteArray baFragmentCode = fShaderFile.readAll();
        fShaderFile.close();

        const char* fShaderCode = baFragmentCode.constData();

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
    }
    else
    {
        Debug(ru("Ошибка при чтении фрагментного шейдера.\n(%1)").arg(_vertexPath));
    }

    if (!_geometryPath.isNull())
    {
        QFile gShaderFile(_geometryPath);
        if (gShaderFile.open(QIODevice::ReadOnly))
        {
            QByteArray baGeometryCode = gShaderFile.readAll();
            gShaderFile.close();

            const char* gShaderCode = baGeometryCode.constData();

            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        else
        {
            Debug(ru("Ошибка при чтении геометрического шейдера.\n(%1)").arg(_vertexPath));
        }
    }

    // shader Program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);

    if (geometry != 0)
    {
        glAttachShader(m_program, geometry);
    }

    glLinkProgram(m_program);
    checkCompileErrors(m_program, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (geometry != 0)
    {
        glDeleteShader(geometry);
    }
}

void Shader::use()
{
    glUseProgram(m_program);
}

void Shader::setBool(const QString &_name, bool _value) const
{
    glUniform1i(glGetUniformLocation(m_program, qUtf8Printable(_name)), (int)_value);
}

void Shader::setInt(const QString &_name, int _value) const
{
    glUniform1i(glGetUniformLocation(m_program, qUtf8Printable(_name)), _value);
}

void Shader::setFloat(const QString &_name, float _value) const
{
    glUniform1f(glGetUniformLocation(m_program, qUtf8Printable(_name)), _value);
}

void Shader::setVec2(const QString &_name, const glm::vec2 &_value) const
{
    glUniform2fv(glGetUniformLocation(m_program, qUtf8Printable(_name)), 1, &_value[0]);
}

void Shader::setVec2(const QString &_name, float _x, float _y) const
{
    glUniform2f(glGetUniformLocation(m_program, qUtf8Printable(_name)), _x, _y);
}

void Shader::setVec3(const QString &_name, const glm::vec3 &_value) const
{
    glUniform3fv(glGetUniformLocation(m_program, qUtf8Printable(_name)), 1, &_value[0]);
}

void Shader::setVec3(const QString &_name, float _x, float _y, float _z) const
{
    glUniform3f(glGetUniformLocation(m_program, qUtf8Printable(_name)), _x, _y, _z);
}

void Shader::setVec4(const QString &_name, const glm::vec4 &_value) const
{
    glUniform4fv(glGetUniformLocation(m_program, qUtf8Printable(_name)), 1, &_value[0]);
}

void Shader::setVec4(const QString &_name, float _x, float _y, float _z, float _w)
{
    glUniform4f(glGetUniformLocation(m_program, qUtf8Printable(_name)), _x, _y, _z, _w);
}

void Shader::setMat2(const QString &_name, const glm::mat2 &_matrix) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_program, qUtf8Printable(_name)), 1, GL_FALSE, &_matrix[0][0]);
}

void Shader::setMat3(const QString &_name, const glm::mat3 &_matrix) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_program, qUtf8Printable(_name)), 1, GL_FALSE, &_matrix[0][0]);
}

void Shader::setMat4(const QString &_name, const glm::mat4 &_matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, qUtf8Printable(_name)), 1, GL_FALSE, &_matrix[0][0]);
}


void Shader::checkCompileErrors(GLuint _shader, const QString &_type)
{
    GLint success;
    GLchar infoLog[1024];

    if (_type != "PROGRAM")
    {
        glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(_shader, 1024, NULL, infoLog);
            Debug("ERROR::SHADER_COMPILATION_ERROR of type: ")
                    << _type
                    << "\n"
                    << infoLog
                    << "\n -- --------------------------------------------------- -- ";
        }
    }
    else
    {
        glGetProgramiv(_shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(_shader, 1024, NULL, infoLog);
            Debug("ERROR::PROGRAM_LINKING_ERROR of type: ")
                    << _type
                    << "\n"
                    << infoLog
                    << "\n -- --------------------------------------------------- -- ";
        }
    }
}

