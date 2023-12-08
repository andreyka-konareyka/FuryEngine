#include "FuryObject.h"

FuryObject::FuryObject() :
    FuryObject(glm::vec3(0, 0, 0))
{

}

FuryObject::FuryObject(const glm::vec3& _position) :
    m_position(_position),
    m_vertexCount(0),
    m_rotate(glm::vec3(0, 0, 0)),
    m_scales(glm::vec3(1, 1, 1))
{

}

FuryObject::~FuryObject()
{

}
