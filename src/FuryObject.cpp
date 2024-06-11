#include "FuryObject.h"

#include "FuryWorld.h"

#include <reactphysics3d/reactphysics3d.h>

FuryObject::FuryObject(FuryWorld *_world) :
    FuryObject(_world, glm::vec3(0, 0, 0))
{

}

FuryObject::FuryObject(FuryWorld *_world, const glm::vec3& _position) :
    m_position(_position),
    m_rotate(glm::vec3(0, 0, 0)),
    m_scales(glm::vec3(1, 1, 1)),
    m_world(_world),
    m_modelTransform(1),
    m_textureScales(1, 1)
{
    rp3d::Vector3 objectPos(m_position.x, m_position.y, m_position.z);
    rp3d::Quaternion objectOrientation(rp3d::Quaternion::identity());
    rp3d::Transform objectTransform(objectPos, objectOrientation);

    m_physicsBody = m_world->physicsWorld()->createRigidBody(objectTransform);
    m_physicsBody->setType(rp3d::BodyType::STATIC);
    m_physicsBody->setUserData(this);
}

FuryObject::~FuryObject()
{
    m_world->physicsWorld()->destroyRigidBody(m_physicsBody);
}

void FuryObject::setPosition(const glm::vec3 &_position)
{
    m_position = _position;

    rp3d::Vector3 objectPos(m_position.x, m_position.y, m_position.z);
    const rp3d::Quaternion& tmp = m_physicsBody->getTransform().getOrientation();
    rp3d::Transform objectTransform(objectPos, tmp);

    m_physicsBody->setTransform(objectTransform);
}

glm::mat4 FuryObject::getOpenGLTransform() const
{
    float rawMatrix[16];
    m_physicsBody->getTransform().getOpenGLMatrix(rawMatrix);
    return glm::mat4(rawMatrix[0], rawMatrix[1], rawMatrix[2], rawMatrix[3],
                     rawMatrix[4], rawMatrix[5], rawMatrix[6], rawMatrix[7],
                     rawMatrix[8], rawMatrix[9], rawMatrix[10], rawMatrix[11],
                     rawMatrix[12], rawMatrix[13], rawMatrix[14], rawMatrix[15]);
}
