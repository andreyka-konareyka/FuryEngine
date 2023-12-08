#include "Texture.h"


Texture::Texture(GLuint _idOpenGL, const QString &_path) :
    m_idOpenGL(_idOpenGL),
    m_path(_path),
    m_data(nullptr),
    m_width(0),
    m_height(0),
    m_ready(false),
    m_loaded(false)
{

}

Texture::Texture() : Texture(0, "")
{

}

Texture::~Texture()
{
    if (m_data != nullptr)
    {
        delete m_data;
        m_data = nullptr;
    }
}
