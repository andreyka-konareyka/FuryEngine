#ifndef SHADER_H
#define SHADER_H

#include "FuryLogger.h"

//#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>

#include <QString>

class Shader
{
public:
    unsigned int Program = 0;
    Shader() {}
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        QString vertexCode;
        QString fragmentCode;
        QString geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = QString::fromStdString(vShaderStream.str());
            fragmentCode = QString::fromStdString(fShaderStream.str());
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = QString::fromStdString(gShaderStream.str());
            }
        }
        catch (std::ifstream::failure&)
        {
            Debug("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
        }

        QByteArray baVertexCode = vertexCode.toUtf8();
        QByteArray baFragmentCode = fragmentCode.toUtf8();
        const char* vShaderCode = baVertexCode.constData();
        const char* fShaderCode = baFragmentCode.constData();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            QByteArray baGeometryCode = geometryCode.toUtf8();
            const char* gShaderCode = baGeometryCode.constData();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        Program = glCreateProgram();
        glAttachShader(Program, vertex);
        glAttachShader(Program, fragment);
        if (geometryPath != nullptr)
            glAttachShader(Program, geometry);
        glLinkProgram(Program);
        checkCompileErrors(Program, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void Use()
    {
        glUseProgram(Program);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const QString& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(Program, qUtf8Printable(name)), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const QString& name, int value) const
    {
        glUniform1i(glGetUniformLocation(Program, qUtf8Printable(name)), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const QString& name, float value) const
    {
        glUniform1f(glGetUniformLocation(Program, qUtf8Printable(name)), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const QString& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(Program, qUtf8Printable(name)), 1, &value[0]);
    }
    void setVec2(const QString& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(Program, qUtf8Printable(name)), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const QString& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(Program, qUtf8Printable(name)), 1, &value[0]);
    }
    void setVec3(const QString& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(Program, qUtf8Printable(name)), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const QString& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(Program, qUtf8Printable(name)), 1, &value[0]);
    }
    void setVec4(const QString& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(Program, qUtf8Printable(name)), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const QString& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(Program, qUtf8Printable(name)), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const QString& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(Program, qUtf8Printable(name)), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const QString& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(Program, qUtf8Printable(name)), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, QString type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Debug("ERROR::SHADER_COMPILATION_ERROR of type: ") << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- ";
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Debug("ERROR::PROGRAM_LINKING_ERROR of type: ") << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- ";
            }
        }
    }
};

#endif // SHADER_H
