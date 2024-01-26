#include "CarObject.h"

#include "FuryRaycastCallback.h"
#include "FuryWorld.h"
#include "FuryMaterial.h"
#include "FuryMaterialManager.h"
#include "FuryLogger.h"

#include <QString>


int rayCount = 20;


CarObject::CarObject(FuryWorld *_world, const glm::vec3& _position, Shader *_shader) :
    FuryObject(_world, _position),
    m_objectBody(nullptr),
    m_springLenght(0.4f),
    m_springK(500),
    m_lastSuspentionLenght({ m_springLenght, m_springLenght, m_springLenght, m_springLenght }),
    m_cameraLocalViewPoint(0, 2, 0),
    m_cameraLocalPosition(-7, 3, 0),
    m_forward(0),
    m_right(0),
    m_lastTriggerNumber(0),
    m_reward(0),
    m_startPosition(_position),
    m_timeCounter(0),
    m_backTriggerCounter(0),
    m_hasContact(false)
{
    m_objectBody = new FuryBoxObject(world(), _position, 6, 1, 3);
    m_objectsForDraw.push_back(m_objectBody);

    m_objectBody->setTextureName("carBody");
    m_objectBody->setName("carBody");
    m_objectBody->setShader(_shader);

    float sphereRadius = 0.1f;
    m_objectWheels.push_back(new FurySphereObject(world(), _position + glm::vec3(2, -0.5, 1), sphereRadius));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);
    m_objectWheels.push_back(new FurySphereObject(world(), _position + glm::vec3(2, -0.5, -1), sphereRadius));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);
    m_objectWheels.push_back(new FurySphereObject(world(), _position + glm::vec3(-2, -0.5, 1), sphereRadius));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);
    m_objectWheels.push_back(new FurySphereObject(world(), _position + glm::vec3(-2, -0.5, -1), sphereRadius));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);

    for (int i = 0; i < rayCount; ++i)
    {
        m_objectsDebugRays.push_back(new FurySphereObject(world(), _position + glm::vec3(30, 0, 0), 0.25));
        m_objectsDebugRays.last()->setTextureName("Diffuse_rayCastBall");
        m_objectsForDraw.push_back(m_objectsDebugRays.last());

        FuryMaterialManager* materialManager = FuryMaterialManager::instance();

        if (!FuryMaterialManager::instance()->materialExist("rayCastBall"))
        {
            FuryMaterial* mat = materialManager->createMaterial("rayCastBall");
            mat->setDiffuseTexture("Diffuse_rayCastBall");
        }

        m_objectsDebugRays.last()->setMaterialName("rayCastBall");
    }
}

CarObject::~CarObject()
{
    if (m_objectBody != nullptr)
    {
        delete m_objectBody;
        m_objectBody = nullptr;
    }

    for (int i = 0; i < m_objectWheels.size(); ++i)
    {
        if (m_objectWheels[i] != nullptr)
        {
            delete m_objectWheels[i];
            m_objectWheels[i] = nullptr;
        }
    }

    for (int i = 0; i < m_objectsDebugRays.size(); ++i)
    {
        if (m_objectsDebugRays[i] != nullptr)
        {
            delete m_objectsDebugRays[i];
            m_objectsDebugRays[i] = nullptr;
        }
    }
}

void CarObject::tick(double _dt)
{
    // m_objectBody->physicsBody()->applyLocalForceAtLocalPosition(rp3d::Vector3(1, 0, 0.6) * 17, rp3d::Vector3(2, -0.5, 1));
    // m_objectBody->physicsBody()->applyLocalForceAtLocalPosition(rp3d::Vector3(1, 0, 0.6) * 20, rp3d::Vector3(2, -0.5, -1));



    {
        for (int i = 0; i < m_objectWheels.size(); ++i)
        {
            glm::vec3 tmp1 = m_objectWheels[i]->getPosition();
            reactphysics3d::Vector3 tmp2 = m_objectWheels[i]->physicsBody()->getWorldPoint(reactphysics3d::Vector3(0, -m_springLenght, 0));
            glm::vec3 tmp3(tmp2.x, tmp2.y, tmp2.z);
            // Create the ray
            reactphysics3d::Vector3 startPoint(tmp1.x, tmp1.y, tmp1.z);
            reactphysics3d::Vector3 endPoint(tmp3.x, tmp3.y, tmp3.z);
            reactphysics3d::Ray ray(startPoint, endPoint);

            // Create an instance of your callback class
            FuryRaycastCallback callbackObject;
            //std::cout << "raycast " << i << "\n";

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


            m_objectBody->physicsBody()->applyLocalForceAtWorldPosition(callbackObject.m_lastNormal * forceValue, startPoint);
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

            m_objectBody->physicsBody()->applyLocalForceAtWorldPosition(tireForce, startPoint);


            if (i < 2 && m_right != 0)
            {
                m_objectWheels[i]->physicsBody()->setTransform(oldTransform);
            }
        }
    }

//    int rayCount = 100;
//    float rayLenght = 30;
//    for (int i = 0; i < rayCount; ++i)
//    {
//        float x = std::cos(2 * 3.14 / rayCount * i);
//        float z = std::sin(2 * 3.14 / rayCount * i);

//        glm::vec3 tmp1 = m_objectBody->getPosition() - glm::vec3(0, 0.25, 0);
//        reactphysics3d::Vector3 tmp2 = m_objectBody->physicsBody()->getWorldPoint(reactphysics3d::Vector3(rayLenght * x, 0, rayLenght * z));
//        glm::vec3 tmp3(tmp2.x, tmp2.y, tmp2.z);
//        tmp3 -= glm::vec3(0, 0.25, 0);
//        // Create the ray
//        reactphysics3d::Vector3 startPoint(tmp1.x, tmp1.y, tmp1.z);
//        reactphysics3d::Vector3 endPoint(tmp3.x, tmp3.y, tmp3.z);
//        reactphysics3d::Ray ray(startPoint, endPoint);

//        // Create an instance of your callback class
//        FuryRaycastCallback callbackObject;
//        //std::cout << "raycast " << i << "\n";

//        // Raycast test
//        world()->physicsWorld()->raycast(ray, &callbackObject);


//        glm::vec3 tmp4 = tmp1 + (tmp3 - tmp1) * callbackObject.m_lastHitFraction;
//        m_objectsDebugRays[i]->setPosition(tmp4);
//    }
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
    default:
        break;
    }
}

int CarObject::getUserAction()
{
    if (m_forward == 0 && m_right == 0)
    {
        return 0;
    }
    if (m_forward == 1 && m_right == 0)
    {
        return 1;
    }
    if (m_forward == 1 && m_right == 1)
    {
        return 2;
    }
    if (m_forward == 1 && m_right == -1)
    {
        return 3;
    }
    if (m_forward == 0 && m_right == 1)
    {
        return 4;
    }
    if (m_forward == 0 && m_right == -1)
    {
        return 5;
    }
    if (m_forward == -1 && m_right == 0)
    {
        return 6;
    }

    qDebug() << ru("Неправильное действие от пользователя");
    return 0;
}

QVector<float> CarObject::getRays()
{
    QVector<float> result;

    float rayLenght = 30;
    for (int i = 0; i < rayCount; ++i)
    {
        float x = std::cos(2 * 3.14 / rayCount * i);
        float z = std::sin(2 * 3.14 / rayCount * i);

        glm::vec3 tmp1 = m_objectBody->getPosition() - glm::vec3(0, 0.25, 0);
        reactphysics3d::Vector3 tmp2 = m_objectBody->physicsBody()->getWorldPoint(reactphysics3d::Vector3(rayLenght * x, 0, rayLenght * z));
        glm::vec3 tmp3(tmp2.x, tmp2.y, tmp2.z);
        tmp3 -= glm::vec3(0, 0.25, 0);
        // Create the ray
        reactphysics3d::Vector3 startPoint(tmp1.x, tmp1.y, tmp1.z);
        reactphysics3d::Vector3 endPoint(tmp3.x, tmp3.y, tmp3.z);
        reactphysics3d::Ray ray(startPoint, endPoint);

        // Create an instance of your callback class
        FuryRaycastCallback callbackObject;
        //std::cout << "raycast " << i << "\n";

        // Raycast test
        world()->physicsWorld()->raycast(ray, &callbackObject);

        result.append(callbackObject.m_lastHitFraction);

        glm::vec3 tmp4 = tmp1 + (tmp3 - tmp1) * callbackObject.m_lastHitFraction;
        m_objectsDebugRays[i]->setPosition(tmp4);
    }

    return result;
}

glm::vec3 CarObject::getSpeed()
{
    rp3d::Vector3 worldSpeed = m_objectBody->physicsBody()->getLinearVelocity();
    rp3d::Vector3 localSpeed = m_objectBody->physicsBody()->getLocalVector(worldSpeed);
    return glm::vec3(localSpeed.x, localSpeed.y, localSpeed.z);
}

float CarObject::getReward()
{
//    glm::vec3 localSpeed = getSpeed();
//    m_reward += localSpeed.x / 23 * 0.03;
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
        m_reward -= 20;
        m_backTriggerCounter++;
    }
    else if (m_lastTriggerNumber == 71 && _number == 0)
    {
        m_reward += 5;
        m_timeCounter = 0;
    }
    else if (m_lastTriggerNumber < _number)
    {
        m_reward += 5;
        m_timeCounter = 0;
    }
    else if (m_lastTriggerNumber > _number)
    {
        m_reward -= 20;
        m_backTriggerCounter++;
    }

    m_lastTriggerNumber = _number;
}

void CarObject::resetPosition(const glm::vec3 &_position)
{
    m_objectBody->setPosition(_position);
    rp3d::Transform transform1 = m_objectBody->physicsBody()->getTransform();
    transform1.setOrientation(rp3d::Quaternion::identity());
    m_objectBody->physicsBody()->setTransform(transform1);
    m_objectBody->physicsBody()->setAngularVelocity(rp3d::Vector3(0, 0, 0));
    m_objectBody->physicsBody()->setLinearVelocity(rp3d::Vector3(0, 0, 0));

    m_objectWheels[0]->setPosition(_position + glm::vec3(2, -0.5, 1));
    m_objectWheels[1]->setPosition(_position + glm::vec3(2, -0.5, -1));
    m_objectWheels[2]->setPosition(_position + glm::vec3(-2, -0.5, 1));
    m_objectWheels[3]->setPosition(_position + glm::vec3(-2, -0.5, -1));
}

glm::vec3 CarObject::calcNextTriggerVector(const glm::vec3 &_trigger)
{
    glm::vec3 vector = _trigger - m_objectBody->getPosition();
    vector = glm::normalize(vector);
    rp3d::Vector3 worldVector(vector.x, vector.y, vector.z);
    rp3d::Vector3 localVector = m_objectBody->physicsBody()->getLocalVector(worldVector);
    glm::vec3 speedGlm = getSpeed();
    rp3d::Vector3 speed(speedGlm.x / 23, speedGlm.y / 23, speedGlm.z / 23);

    m_reward += speed.dot(localVector) * 0.01;

    return glm::vec3(localVector.x, localVector.y, localVector.z);
}

void CarObject::respawn()
{
    rp3d::Vector3 objectPos(m_startPosition.x, m_startPosition.y, m_startPosition.z);
    rp3d::Quaternion objectOrientation(rp3d::Quaternion::identity());
    rp3d::Transform objectTransform(objectPos, objectOrientation);

    world()->physicsWorld()->destroyRigidBody(m_objectBody->physicsBody());
    m_objectBody->setPhysicsBody(world()->physicsWorld()->createRigidBody(objectTransform));
    m_objectBody->setPosition(m_startPosition);
    m_objectBody->physicsBody()->setUserData(m_objectBody);


    objectPos = rp3d::Vector3(m_startPosition.x + 2, m_startPosition.y - 0.5, m_startPosition.z + 1);
    objectTransform = rp3d::Transform(objectPos, objectOrientation);
    world()->physicsWorld()->destroyRigidBody(m_objectWheels[0]->physicsBody());
    m_objectWheels[0]->setPhysicsBody(world()->physicsWorld()->createRigidBody(objectTransform));
    m_objectWheels[0]->setPosition(m_startPosition + glm::vec3(2, -0.5, 1));
    m_objectWheels[0]->physicsBody()->setUserData(m_objectWheels[0]);

    objectPos = rp3d::Vector3(m_startPosition.x + 2, m_startPosition.y - 0.5, m_startPosition.z - 1);
    objectTransform = rp3d::Transform(objectPos, objectOrientation);
    world()->physicsWorld()->destroyRigidBody(m_objectWheels[1]->physicsBody());
    m_objectWheels[1]->setPhysicsBody(world()->physicsWorld()->createRigidBody(objectTransform));
    m_objectWheels[1]->setPosition(m_startPosition + glm::vec3(2, -0.5, -1));
    m_objectWheels[1]->physicsBody()->setUserData(m_objectWheels[1]);

    objectPos = rp3d::Vector3(m_startPosition.x - 2, m_startPosition.y - 0.5, m_startPosition.z + 1);
    objectTransform = rp3d::Transform(objectPos, objectOrientation);
    world()->physicsWorld()->destroyRigidBody(m_objectWheels[2]->physicsBody());
    m_objectWheels[2]->setPhysicsBody(world()->physicsWorld()->createRigidBody(objectTransform));
    m_objectWheels[2]->setPosition(m_startPosition + glm::vec3(-2, -0.5, 1));
    m_objectWheels[2]->physicsBody()->setUserData(m_objectWheels[2]);

    objectPos = rp3d::Vector3(m_startPosition.x - 2, m_startPosition.y - 0.5, m_startPosition.z - 1);
    objectTransform = rp3d::Transform(objectPos, objectOrientation);
    world()->physicsWorld()->destroyRigidBody(m_objectWheels[3]->physicsBody());
    m_objectWheels[3]->setPhysicsBody(world()->physicsWorld()->createRigidBody(objectTransform));
    m_objectWheels[3]->setPosition(m_startPosition + glm::vec3(-2, -0.5, -1));
    m_objectWheels[3]->physicsBody()->setUserData(m_objectWheels[3]);

    Setup_physics(reactphysics3d::BodyType::DYNAMIC);
    m_backTriggerCounter = 0;
    m_hasContact = false;
    m_timeCounter = 0;
    m_lastTriggerNumber = 0;
}

bool CarObject::checkTimeCounter()
{
    if (m_timeCounter < 8)
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
    if (m_backTriggerCounter < 2)
    {
        return true;
    }

    return false;
}

bool CarObject::checkHasContact()
{
    return !m_hasContact;
}

void CarObject::Setup_physics(reactphysics3d::BodyType type)
{
    m_objectBody->Setup_physics(type);

    for (int i = 0; i < m_objectWheels.size(); ++i)
    {
        m_objectWheels[i]->physicsBody()->setType(type);
    }

    for (int i = 0; i < m_objectWheels.size(); ++i)
    {
        // Anchor point in world-space
        float x = m_objectWheels[i]->getPosition().x;
        float y = m_objectWheels[i]->getPosition().y;
        float z = m_objectWheels[i]->getPosition().z;
        const reactphysics3d::Vector3 anchorPoint(x, y, z);

        // Create the joint info object
        reactphysics3d::FixedJointInfo jointInfo(m_objectBody->physicsBody(), m_objectWheels[i]->physicsBody(), anchorPoint);
        jointInfo.isCollisionEnabled = false;

        // Create the hinge joint in the physics world
        world()->physicsWorld()->createJoint(jointInfo);
    }
}

glm::vec3 CarObject::cameraPosition() const
{
    reactphysics3d::Vector3 cameraLocalPosition(m_cameraLocalPosition.x, 0, m_cameraLocalPosition.z);
    reactphysics3d::Vector3 worldPos = m_objectBody->physicsBody()->getWorldPoint(cameraLocalPosition);
    return glm::vec3(worldPos.x, worldPos.y + m_cameraLocalPosition.y, worldPos.z);
}

glm::vec3 CarObject::cameraViewPoint() const
{
    reactphysics3d::Vector3 cameraLocalViewPoint(m_cameraLocalViewPoint.x, 0, m_cameraLocalViewPoint.z);
    reactphysics3d::Vector3 worldPoint = m_objectBody->physicsBody()->getWorldPoint(cameraLocalViewPoint);
    return glm::vec3(worldPoint.x, worldPoint.y + m_cameraLocalViewPoint.y, worldPoint.z);
}

void CarObject::setLocalCameraPosition(int _x, int _y)
{
    m_cameraLocalPosition.x = -_x;
    m_cameraLocalPosition.y = _y;
}

void CarObject::setSpringLenght(float _lenght)
{
    m_springLenght = _lenght;
}

void CarObject::setSpringK(float _k)
{
    m_springK = _k;
}
