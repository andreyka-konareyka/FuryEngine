#include "CarObject.h"

#include "FuryRaycastCallback.h"

#include <QString>


CarObject::CarObject(const glm::vec3& _position) :
    FuryObject(_position),
    m_objectBody(nullptr),
    m_objectSalon(nullptr),
    m_springLenght(1),
    m_lastSuspentionLenght({ m_springLenght, m_springLenght, m_springLenght, m_springLenght }),
    m_cameraLocalViewPoint(0, 2, 0),
    m_cameraLocalPosition(-6, 3, 0),
    m_forward(0),
    m_right(0)
{
    m_objectBody = new FuryBoxObject(_position, 5, 0.85, 2.5);
    m_objectSalon = new FuryBoxObject(_position + glm::vec3(-0.5, 0.85, 0), 2.5, 0.85, 2.5);
    m_objectsForDraw.push_back(m_objectBody);
    m_objectsForDraw.push_back(m_objectSalon);

    m_objectWheels.push_back(new FurySphereObject(_position + glm::vec3(2, -0.5, 1), 0.25));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);
    m_objectWheels.push_back(new FurySphereObject(_position + glm::vec3(2, -0.5, -1), 0.25));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);
    m_objectWheels.push_back(new FurySphereObject(_position + glm::vec3(-2, -0.5, 1), 0.25));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);
    m_objectWheels.push_back(new FurySphereObject(_position + glm::vec3(-2, -0.5, -1), 0.25));
    m_objectsForDraw.push_back(m_objectWheels[m_objectWheels.size() - 1]);

    for (int i = 0; i < 100; ++i)
    {
        m_objectsDebugRays.push_back(new FurySphereObject(_position + glm::vec3(30, 0, 0), 0.25));
        m_objectsForDraw.push_back(m_objectsDebugRays.last());
    }
}

CarObject::~CarObject()
{
    if (m_objectBody != nullptr)
    {
        delete m_objectBody;
        m_objectBody = nullptr;
    }

    if (m_objectSalon != nullptr)
    {
        delete m_objectSalon;
        m_objectSalon = nullptr;
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
            m_physicsWorld->raycast(ray, &callbackObject);

            if (abs(1 - callbackObject.m_lastHitFraction) <= rp3d::MACHINE_EPSILON)
            {
                m_lastSuspentionLenght[i] = m_springLenght;
                continue;
            }

            float currentLenght = m_springLenght * callbackObject.m_lastHitFraction;

            float deltaX = m_springLenght - currentLenght;
            float k = 500;
            float forceValue = k * deltaX;


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
            rp3d::decimal tireForceForward = forceValue * 1 * m_forward;

            rp3d::Vector3 tireForce = (rightProjection * tireForceRight) + (forwardProjection * tireForceForward);

            m_objectBody->physicsBody()->applyLocalForceAtWorldPosition(tireForce, startPoint);


            if (i < 2 && m_right != 0)
            {
                m_objectWheels[i]->physicsBody()->setTransform(oldTransform);
            }
        }
    }

    int rayCount = 100;
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
        m_physicsWorld->raycast(ray, &callbackObject);


        glm::vec3 tmp4 = tmp1 + (tmp3 - tmp1) * callbackObject.m_lastHitFraction;
        m_objectsDebugRays[i]->setPosition(tmp4);
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

void CarObject::Setup_physics(reactphysics3d::PhysicsCommon& phys_common, reactphysics3d::PhysicsWorld* phys_world, reactphysics3d::BodyType type)
{
    m_physicsWorld = phys_world;


    m_objectBody->Setup_physics(phys_common, phys_world, type);
    m_objectSalon->Setup_physics(phys_common, phys_world, type);

    for (int i = 0; i < m_objectWheels.size(); ++i)
    {
        m_objectWheels[i]->Setup_physics(phys_common, phys_world, type);
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
        phys_world->createJoint(jointInfo);
    }

    {
        // Anchor point in world-space
        float x = m_objectBody->getPosition().x;
        float y = m_objectBody->getPosition().y;
        float z = m_objectBody->getPosition().z;
        const reactphysics3d::Vector3 anchorPoint(x, y, z);

        // Create the joint info object
        reactphysics3d::FixedJointInfo jointInfo(m_objectBody->physicsBody(), m_objectSalon->physicsBody(), anchorPoint);
        jointInfo.isCollisionEnabled = false;

        // Create the hinge joint in the physics world
        phys_world->createJoint(jointInfo);
    }
}

glm::vec3 CarObject::cameraPosition() const
{
    reactphysics3d::Vector3 cameraLocalPosition(m_cameraLocalPosition.x, m_cameraLocalPosition.y, m_cameraLocalPosition.z);
    reactphysics3d::Vector3 worldPos = m_objectBody->physicsBody()->getWorldPoint(cameraLocalPosition);
    return glm::vec3(worldPos.x, worldPos.y, worldPos.z);
}

glm::vec3 CarObject::cameraViewPoint() const
{
    reactphysics3d::Vector3 cameraLocalViewPoint(m_cameraLocalViewPoint.x, m_cameraLocalViewPoint.y, m_cameraLocalViewPoint.z);
    reactphysics3d::Vector3 worldPoint = m_objectBody->physicsBody()->getWorldPoint(cameraLocalViewPoint);
    return glm::vec3(worldPoint.x, worldPoint.y, worldPoint.z);
}
