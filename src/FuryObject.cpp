#include "FuryObject.h"

#include "FuryWorld.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QVector3D>
#include <QQuaternion>

/*!
 * \brief Дополнительная функция. Потом надо вынести. Углы Эйлера по кватерниону
 * \param[in] q - Кватернион вращения
 * \return Возвращает вектор вращения в углах Эйлера
 */
rp3d::Vector3 quaternionToEulerAngles(const rp3d::Quaternion& q);



FuryObject::FuryObject(FuryWorld *_world) :
    FuryObject(_world, glm::vec3(0, 0, 0))
{

}

FuryObject::FuryObject(FuryWorld *_world, const glm::vec3& _position) :
    m_worldPosition(_position),
    m_initLocalPosition(_position),
    m_worldRotation(glm::vec3(0, 0, 0)),
    m_initLocalRotation(glm::vec3(0, 0, 0)),
    m_scales(glm::vec3(1, 1, 1)),
    m_shader(nullptr),
    m_physicsBody(nullptr),
    m_world(_world),
    m_modelTransform(1),
    m_textureScales(1, 1),
    m_visible(true),
    m_selectedInEditor(false)
{
    m_world->addObject(this);

    rp3d::Vector3 objectPos(m_worldPosition.x, m_worldPosition.y, m_worldPosition.z);
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

void FuryObject::postPhysics()
{
    const rp3d::Transform& physics_transform = physicsBody()->getTransform();
    const rp3d::Vector3& physics_position = physics_transform.getPosition();
    const rp3d::Quaternion& orientation = physics_transform.getOrientation();
    rp3d::Vector3 newAngle = quaternionToEulerAngles(orientation);

    m_worldPosition = glm::vec3(physics_position.x, physics_position.y, physics_position.z);
    m_worldRotation = glm::vec3(newAngle.x, newAngle.y, newAngle.z);

    foreach (QObject* child, children())
    {
        FuryObject* obj = qobject_cast<FuryObject*>(child);
        if (obj == nullptr)
        {
            continue;
        }

        obj->postPhysics();
    }
}

void FuryObject::reset()
{
    resetTransformationToInit();

    foreach (QObject* child, children())
    {
        FuryObject* obj = qobject_cast<FuryObject*>(child);
        if (obj != nullptr)
        {
            obj->reset();
        }
    }
}

void FuryObject::addChildObject(FuryObject *_child, bool _withoutJoint)
{
    _child->setParent(this);
    _child->reset();

    if (!_withoutJoint)
    {
        // Anchor point in world-space
        glm::vec3 worldPos = _child->calculateWorldPositionByInit();
        const reactphysics3d::Vector3 anchorPoint(worldPos.x,
                                                  worldPos.y,
                                                  worldPos.z);

        // Create the joint info object
        reactphysics3d::FixedJointInfo jointInfo(physicsBody(), _child->physicsBody(), anchorPoint);
        jointInfo.isCollisionEnabled = false;

        // Create the hinge joint in the physics world
        world()->physicsWorld()->createJoint(jointInfo);
    }

    emit _child->parentChangedSignal();
}

void FuryObject::setWorldPosition(const glm::vec3 &_pos)
{
    m_worldPosition = _pos;
    setTransformToWorld();
}

void FuryObject::setInitLocalPosition(const glm::vec3 &_pos)
{
    m_initLocalPosition = _pos;
    reset();
}

void FuryObject::setWorldRotation(const glm::vec3 &_rotation)
{
    m_worldRotation = _rotation;
    setTransformToWorld();
}

void FuryObject::setInitLocalRotation(const glm::vec3 &_rotation)
{
    m_initLocalRotation = _rotation;
    reset();
}

glm::vec3 FuryObject::initLocalRotationDegrees() const
{
    constexpr float scale = 180.0f / 3.14f;
    return glm::vec3(m_initLocalRotation.x * scale,
                     m_initLocalRotation.y * scale,
                     m_initLocalRotation.z * scale);
}

void FuryObject::setInitLocalRotationDegrees(const glm::vec3 &_rotation)
{
    constexpr float scale = 3.14f / 180.0f;
    setInitLocalRotation(glm::vec3(_rotation.x * scale,
                                   _rotation.y * scale,
                                   _rotation.z * scale));
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

glm::vec3 FuryObject::calculateWorldPositionByInit() const
{
    if (parent() == nullptr)
    {
        return m_initLocalPosition;
    }

    FuryObject* parentObj = qobject_cast<FuryObject*>(parent());
    glm::vec3 parentPos = parentObj->calculateWorldPositionByInit();
    glm::vec3 parentRot = parentObj->calculateWorldRotate();
    rp3d::Vector3 parentPosition(parentPos.x, parentPos.y, parentPos.z);
    rp3d::Quaternion parentQuat = rp3d::Quaternion::fromEulerAngles(parentRot.x, parentRot.y, parentRot.z);
    rp3d::Transform parentTransform(parentPosition, parentQuat);

    rp3d::Vector3 localPos(m_initLocalPosition.x, m_initLocalPosition.y, m_initLocalPosition.z);
    rp3d::Vector3 globalPos = parentTransform * localPos;
    return glm::vec3(globalPos.x, globalPos.y, globalPos.z);
}

glm::vec3 FuryObject::calculateWorldRotate() const
{
    if (parent() == nullptr)
    {
        return m_initLocalRotation;
    }

    FuryObject* parentObj = qobject_cast<FuryObject*>(parent());
    glm::vec3 parentRot = parentObj->calculateWorldRotate();
    rp3d::Quaternion parentQuat = rp3d::Quaternion::fromEulerAngles(parentRot.x, parentRot.y, parentRot.z);

    rp3d::Vector3 localRot(m_initLocalRotation.x, m_initLocalRotation.y, m_initLocalRotation.z);
    rp3d::Vector3 globalPos = parentQuat * localRot;
    return glm::vec3(globalPos.x, globalPos.y, globalPos.z);
}

void FuryObject::resetTransformationToInit()
{
    rp3d::Vector3 objectPos(m_initLocalPosition.x, m_initLocalPosition.y, m_initLocalPosition.z);
    rp3d::Quaternion objectOrientation = rp3d::Quaternion::fromEulerAngles(m_initLocalRotation.x,
                                                                           m_initLocalRotation.y,
                                                                           m_initLocalRotation.z);
    rp3d::Transform objectTransform(objectPos, objectOrientation);

    if (parent() != nullptr)
    {
        FuryObject* parentObj = qobject_cast<FuryObject*>(parent());
        const rp3d::Transform& parentTransform = parentObj->physicsBody()->getTransform();
        objectTransform = parentTransform * objectTransform;
        objectPos = objectTransform.getPosition();
        objectOrientation = objectTransform.getOrientation();
    }


    rp3d::Vector3 newAngle = quaternionToEulerAngles(objectOrientation);
    m_worldPosition = glm::vec3(objectPos.x, objectPos.y, objectPos.z);
    m_worldRotation = glm::vec3(newAngle.x, newAngle.y, newAngle.z);

    physicsBody()->setTransform(objectTransform);
    physicsBody()->resetForce();
    physicsBody()->resetTorque();
    physicsBody()->setLinearVelocity(rp3d::Vector3(0, 0, 0));
    physicsBody()->setAngularVelocity(rp3d::Vector3(0, 0, 0));
}

void FuryObject::setTransformToWorld()
{
    rp3d::Vector3 objectPos(m_worldPosition.x, m_worldPosition.y, m_worldPosition.z);
    rp3d::Quaternion objectOrientation = rp3d::Quaternion::fromEulerAngles(m_worldRotation.x,
                                                                           m_worldRotation.y,
                                                                           m_worldRotation.z);
    rp3d::Transform objectTransform(objectPos, objectOrientation);
    physicsBody()->setTransform(objectTransform);
    physicsBody()->resetForce();
    physicsBody()->resetTorque();
    physicsBody()->setLinearVelocity(rp3d::Vector3(0, 0, 0));
    physicsBody()->setAngularVelocity(rp3d::Vector3(0, 0, 0));
}




/*
 * Дополнительная функция
 */

rp3d::Vector3 quaternionToEulerAngles(const rp3d::Quaternion& q)
{
    QQuaternion tmp(q.w, q.x, q.y, q.z);
    QVector3D newChildRotate = tmp.toEulerAngles() / 180.0f * 3.1415f;
    rp3d::Vector3 angles(newChildRotate.x(), newChildRotate.y(), newChildRotate.z());
    return angles;
}
