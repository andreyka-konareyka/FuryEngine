#include "FuryTexture.h"


FuryTexture::FuryTexture(const QString &_path) :
    m_idOpenGL(0),
    m_path(_path),
    m_data(nullptr),
    m_width(0),
    m_height(0),
    m_ready(false),
    m_loaded(false)
{

}

FuryTexture::FuryTexture() : FuryTexture("")
{

}

FuryTexture::~FuryTexture()
{
    if (m_data != nullptr)
    {
        delete m_data;
        m_data = nullptr;
    }
}
