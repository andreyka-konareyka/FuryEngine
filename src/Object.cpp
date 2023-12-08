#include "Object.h"

Object::Object() :
    Object(glm::vec3(0, 0, 0))
{

}

Object::Object(const glm::vec3& _position) :
    m_position(_position),
    m_vertexCount(0),
    m_rotate(glm::vec3(0, 0, 0)),
    m_sizes(glm::vec3(1, 1, 1))
{

}

Object::~Object()
{

}
