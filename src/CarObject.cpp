#include "CarObject.h"

#include "Shader.h"
#include "FuryWorld.h"
#include "FuryPbrMaterial.h"
#include "DefaultObjects/FurySphereObject.h"
#include "Physics/FuryRaycastCallback.h"
#include "Managers/FuryMaterialManager.h"

#include <QString>


int rayCount = 20;


CarObject::CarObject(FuryWorld *_world) :
    FuryObject(_world),
    m_springLenght(0.4f),
    m_springK(500),
    m_lastSuspentionLenght({ m_springLenght, m_springLenght, m_springLenght, m_springLenght }),
    m_cameraLocalViewPoint(0, 2, 0),
    m_cameraLocalPosition(-7, 2, 0),
    m_forward(0),
    m_right(0),
    m_lastTriggerNumber(0),
    m_reward(0),
    m_timeCounter(0),
    m_backTriggerCounter(0),
    m_hasContact(false)
{
    setShaderName("pbrShader");

    m_objectWheels.push_back(new FuryObject(world(), glm::vec3(2, -0.5, 1), this));
    m_objectWheels.last()->setObjectName("wheel_FR");
    m_objectWheels.push_back(new FuryObject(world(), glm::vec3(2, -0.5, -1), this));
    m_objectWheels.last()->setObjectName("wheel_FL");
    m_objectWheels.push_back(new FuryObject(world(), glm::vec3(-2, -0.5, 1), this));
    m_objectWheels.last()->setObjectName("wheel_RR");
    m_objectWheels.push_back(new FuryObject(world(), glm::vec3(-2, -0.5, -1), this));
    m_objectWheels.last()->setObjectName("wheel_RL");

    for (int i = 0; i < rayCount; ++i)
    {
        m_objectsDebugRays.push_back(new FurySphereObject(world(), glm::vec3(30, 0, 0), 0.25, this, true));
//        addChildObject(m_objectsDebugRays.last(), true);

        FuryMaterialManager* materialManager = FuryMaterialManager::instance();

        if (!FuryMaterialManager::instance()->materialExist("rayCastBall"))
        {
            FuryPbrMaterial* mat = materialManager->createPbrMaterial("rayCastBall");
            mat->setAlbedoTexture("Diffuse_rayCastBall");

            mat = materialManager->createPbrMaterial("greenRay");
            mat->setAlbedoTexture("Diffuse_greenRay");
        }

        m_objectsDebugRays.last()->setMaterialName("rayCastBall");
        m_objectsDebugRays.last()->setObjectName("rayCastBall");
        m_objectsDebugRays.last()->setShaderName(shaderName());
    }

    setObjectName("AI_car");
    setModelName("backpack2");
    glm::mat4 testSubModel = glm::mat4(1.0f);
    testSubModel = glm::translate(testSubModel, glm::vec3(0, -0.88, 0));
    testSubModel = glm::rotate(testSubModel, 3.14f/2, glm::vec3(0, 1, 0));
    testSubModel = glm::scale(testSubModel, glm::vec3(1.5, 1.5, 1.5));
    setModelTransform(testSubModel);
}

CarObject::~CarObject()
{

}

void CarObject::tick(double _dt)
{
    for (int i = 0; i < m_objectWheels.size(); ++i)
    {
        glm::vec3 tmp1 = m_objectWheels[i]->worldPosition();
        // Create the ray
        reactphysics3d::Vector3 startPoint(tmp1.x, tmp1.y, tmp1.z);
        reactphysics3d::Vector3 endPoint = m_objectWheels[i]->physicsBody()->getWorldPoint(reactphysics3d::Vector3(0, -m_springLenght, 0));
        reactphysics3d::Ray ray(startPoint, endPoint);

        // Create an instance of your callback class
        FuryRaycastCallback callbackObject;

        // Raycast test
        world()->physicsWorld()->raycast(ray, &callbackObject);

        if (abs(1 - callbackObject.m_lastHitFraction) <= rp3d::MACHINE_EPSILON)
        {
            m_lastSuspentionLenght[i] = m_springLenght;
            continue;
        }

        float currentLenght = m_springLenght * callbackObject.m_lastHitFraction;

        float deltaX = m_springLenght - currentLenght;
        float forceValue = m_springK * deltaX;


        float damperK = 20;
        forceValue += (m_lastSuspentionLenght[i] - currentLenght) / _dt * damperK;


        physicsBody()->applyLocalForceAtWorldPosition(callbackObject.m_lastNormal * forceValue, startPoint);
        m_lastSuspentionLenght[i] = currentLenght;



        rp3d::Transform oldTransform = m_objectWheels[i]->physicsBody()->getTransform();

        rp3d::Vector3 forward(1, 0, 0);
        rp3d::Vector3 right(0, 0, 1);

        if (i < 2 && m_right != 0)
        {
            rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(0, m_right * (-3.14 / 6), 0);
            rp3d::Transform transform(oldTransform.getPosition(), oldTransform.getOrientation() * rotation);
            m_objectWheels[i]->physicsBody()->setTransform(transform);
        }

        rp3d::decimal scalar1 = forward.dot(callbackObject.m_lastNormal);
        rp3d::decimal scalar2 = right.dot(callbackObject.m_lastNormal);

        rp3d::Vector3 proj1 = scalar1 * callbackObject.m_lastNormal;
        rp3d::Vector3 proj2 = scalar2 * callbackObject.m_lastNormal;

        rp3d::Vector3 forwardProjection = forward - proj1;
        forwardProjection.normalize();
        rp3d::Vector3 rightProjection = right - proj2;
        rightProjection.normalize();

        rp3d::Vector3 velocity = m_objectWheels[i]->physicsBody()->getLinearVelocity();
        rp3d::Vector3 localVelocity = m_objectWheels[i]->physicsBody()->getLocalVector(velocity);
        rp3d::decimal tireForceRight = forceValue * std::clamp(-localVelocity.z, -1.f, 1.f);

        float slipAngle = 0;
        float slipAnglePeak = 8.0f / 180.0f * 3.14f;

        if (localVelocity.x != 0)
        {
            slipAngle = atan(-localVelocity.z / abs(localVelocity.x));
        }

        float Sy = slipAngle / slipAnglePeak;

        tireForceRight = forceValue * std::clamp(Sy, -2.f, 2.f);

        rp3d::decimal tireForceForward = forceValue * 1.5 * m_forward;
        int localVelXSign = localVelocity.x < 0 ? -1 : 1;
        rp3d::decimal tireForceBack
                = 0.003
                * localVelXSign
                * std::clamp(-pow(localVelocity.x, 2.f), -1000.f, 1000.f);
        tireForceForward += forceValue * tireForceBack;

        rp3d::Vector3 tireForce = (rightProjection * tireForceRight) + (forwardProjection * tireForceForward);

        physicsBody()->applyLocalForceAtWorldPosition(tireForce, startPoint);


        if (i < 2 && m_right != 0)
        {
            m_objectWheels[i]->physicsBody()->setTransform(oldTransform);
        }
    }
}

void CarObject::keyPressEvent(int _keyCode)
{
    switch (_keyCode)
    {
    case Qt::Key_W:
        m_forward += 1;
        break;
    case Qt::Key_S:
        m_forward -= 1;
        break;
    case Qt::Key_D:
        m_right += 1;
        break;
    case Qt::Key_A:
        m_right -= 1;
        break;
    default:
        break;
    };
}

void CarObject::keyReleaseEvent(int _keyCode)
{
    switch (_keyCode)
    {
    case Qt::Key_W:
        m_forward -= 1;
        break;
    case Qt::Key_S:
        m_forward += 1;
        break;
    case Qt::Key_D:
        m_right -= 1;
        break;
    case Qt::Key_A:
        m_right += 1;
        break;
    default:
        break;
    };
}

void CarObject::resetKeyInput()
{
    m_forward = 0;
    m_right = 0;
}

void CarObject::setBotAction(int _action)
{
    switch (_action) {
    case 0:
        m_forward = 0;
        m_right = 0;
        break;
    case 1:
        m_forward = 1;
        m_right = 0;
        break;
    case 2:
        m_forward = 1;
        m_right = 1;
        break;
    case 3:
        m_forward = 1;
        m_right = -1;
        break;
    case 4:
        m_forward = 0;
        m_right = 1;
        break;
    case 5:
        m_forward = 0;
        m_right = -1;
        break;
    case 6:
        m_forward = -1;
        m_right = 0;
        break;
    case 7:
        m_forward = -1;
        m_right = 1;
        break;
    case 8:
        m_forward = -1;
        m_right = -1;
    default:
        break;
    }
}

QVector<float> CarObject::getRays()
{
    QVector<float> result;

    float rayLenght = 30;
    for (int i = 0; i < rayCount; ++i)
    {
        float x = std::cos(2 * 3.14 / rayCount * i);
        float z = std::sin(2 * 3.14 / rayCount * i);

        glm::vec3 tmp1 = worldPosition() - glm::vec3(0, 0.25, 0);
        reactphysics3d::Vector3 tmp2 = physicsBody()->getWorldPoint(reactphysics3d::Vector3(rayLenght * x, 0, rayLenght * z));
        glm::vec3 tmp3(tmp2.x, tmp2.y, tmp2.z);
        tmp3 -= glm::vec3(0, 0.25, 0);
        // Create the ray
        reactphysics3d::Vector3 startPoint(tmp1.x, tmp1.y, tmp1.z);
        reactphysics3d::Vector3 endPoint(tmp3.x, tmp3.y, tmp3.z);
        reactphysics3d::Ray ray(startPoint, endPoint);

        // Create an instance of your callback class
        FuryRaycastCallback callbackObject;

        {
            callbackObject.m_needTriggerNumber = (m_lastTriggerNumber + 1) % 72;
        }

        // Raycast test
        world()->physicsWorld()->raycast(ray, &callbackObject);

        result.append(callbackObject.m_lastHitFraction);
        result.append((callbackObject.m_needTriggered) ? 1.0f : -1.0f);

        glm::vec3 tmp4 = tmp1 + (tmp3 - tmp1) * callbackObject.m_lastHitFraction;
        m_objectsDebugRays[i]->setWorldPosition(tmp4);

        {
            if (callbackObject.m_needTriggered)
            {
                m_objectsDebugRays[i]->setMaterialName("greenRay");
            }
            else
            {
                m_objectsDebugRays[i]->setMaterialName("rayCastBall");
            }
        }
    }

    return result;
}

glm::vec3 CarObject::getSpeed()
{
    rp3d::Vector3 worldSpeed = physicsBody()->getLinearVelocity();
    rp3d::Vector3 localSpeed = physicsBody()->getLocalVector(worldSpeed);
    return glm::vec3(localSpeed.x, localSpeed.y, localSpeed.z);
}

glm::vec3 CarObject::getAngularSpeed()
{
    rp3d::Vector3 angularSpeed = physicsBody()->getAngularVelocity();
    return glm::vec3(angularSpeed.x, angularSpeed.y, angularSpeed.z);
}

float CarObject::getReward()
{
    float tmpReward = m_reward;
    m_reward = 0;
    return tmpReward;
}

int CarObject::getLastTriggerNumber()
{
    return m_lastTriggerNumber;
}

void CarObject::onContact()
{
    m_hasContact = true;
}

void CarObject::onTrigger(int _number)
{
    if (m_lastTriggerNumber == 0 && _number == 71)
    {
        m_reward -= 0.1f;
        m_backTriggerCounter++;
    }
    else if (m_lastTriggerNumber == 71 && _number == 0)
    {
        m_reward += 0.1f;
        m_timeCounter = 0;
    }
    else if (m_lastTriggerNumber < _number)
    {
        m_reward += 0.1f;
        m_timeCounter = 0;
    }
    else if (m_lastTriggerNumber > _number)
    {
        m_reward -= 0.1f;
        m_backTriggerCounter++;
    }

    m_lastTriggerNumber = _number;
}

glm::vec3 CarObject::calcNextTriggerVector(const glm::vec3 &_trigger)
{
    glm::vec3 vector = _trigger - worldPosition();
    vector = glm::normalize(vector);
    rp3d::Vector3 worldVector(vector.x, vector.y, vector.z);
    rp3d::Vector3 localVector = physicsBody()->getLocalVector(worldVector);
    glm::vec3 speedGlm = getSpeed();
    rp3d::Vector3 speed(speedGlm.x / 23, speedGlm.y / 23, speedGlm.z / 23);

    float tmp = speed.dot(localVector) * 0.05;
    if (tmp < 0)
    {
        m_reward += tmp * 1.2;
    }
    else
    {
        m_reward += tmp;
    }

    return glm::vec3(localVector.x, localVector.y, localVector.z);
}

void CarObject::reset()
{
    FuryObject::reset();

    m_backTriggerCounter = 0;
    m_hasContact = false;
    m_timeCounter = 0;
    m_lastTriggerNumber = 0;
}

bool CarObject::checkTimeCounter()
{
    if (m_timeCounter < 18)
    {
        m_timeCounter += 1.0f / 60;
        return true;
    }
    else
    {
        return false;
    }
}

bool CarObject::checkBackTriggerCounter()
{
    if (m_backTriggerCounter < 3)
    {
        return true;
    }

    m_backTriggerCounter = 0;
    return false;
}

bool CarObject::checkHasContact()
{
    if (m_hasContact)
    {
        m_hasContact = false;
        return false;
    }

    return true;
//    return !m_hasContact;
}

void CarObject::initPhysics(reactphysics3d::BodyType _type)
{
    FuryObject::initPhysics(_type);

    const reactphysics3d::Vector3 halfExtents(6.5 / 2.0f, 1 / 2.0f, 3 / 2.0f);
    reactphysics3d::BoxShape* boxShape = world()->physicsCommon()->createBoxShape(halfExtents);
    reactphysics3d::Transform transform_boxShape = reactphysics3d::Transform::identity();
    reactphysics3d::Collider* collider_box;
    collider_box = physicsBody()->addCollider(boxShape, transform_boxShape);
    collider_box->getMaterial().setFrictionCoefficient(0.4f);

    for (int i = 0; i < m_objectWheels.size(); ++i)
    {
        m_objectWheels[i]->physicsBody()->setType(_type);
    }
}

glm::vec3 CarObject::cameraPosition() const
{
    reactphysics3d::Vector3 cameraLocalPosition(m_cameraLocalPosition.x, 0, m_cameraLocalPosition.z);
    reactphysics3d::Vector3 worldPos = physicsBody()->getWorldPoint(cameraLocalPosition);
    return glm::vec3(worldPos.x, worldPos.y + m_cameraLocalPosition.y, worldPos.z);
}

glm::vec3 CarObject::cameraViewPoint() const
{
    reactphysics3d::Vector3 cameraLocalViewPoint(m_cameraLocalViewPoint.x, 0, m_cameraLocalViewPoint.z);
    reactphysics3d::Vector3 worldPoint = physicsBody()->getWorldPoint(cameraLocalViewPoint);
    return glm::vec3(worldPoint.x, worldPoint.y + m_cameraLocalViewPoint.y, worldPoint.z);
}

void CarObject::setLocalCameraPosition(float _x, float _y)
{
    m_cameraLocalPosition.x = -_x;
    m_cameraLocalPosition.y = _y;
}
