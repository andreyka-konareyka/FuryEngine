#include "FuryWorld.h"

#include "FuryLogger.h"
#include "FuryObject.h"
#include "Camera.h"

#include <reactphysics3d/reactphysics3d.h>


FuryWorld::FuryWorld(reactphysics3d::PhysicsCommon *_physicsCommon) :
    m_physicsCommon(_physicsCommon),
    m_currentCamera(nullptr)
{
    Debug(ru("Создание игрового мира"));
    m_physicsWorld = m_physicsCommon->createPhysicsWorld();
}

FuryWorld::~FuryWorld()
{
    Debug(ru("Удаление игрового мира"));

    for (FuryObject* object : m_objects)
    {
        if (object != nullptr)
        {
            delete object;
        }
    }

    m_physicsCommon->destroyPhysicsWorld(m_physicsWorld);
}

void FuryWorld::tick(double _dt)
{
    for (FuryObject* object : m_objects)
    {
        object->tick(_dt);
    }
}

void FuryWorld::addObject(FuryObject* _object)
{
    m_objects.push_back(_object);
}

void FuryWorld::setCamera(Camera* _camera)
{
    m_currentCamera = _camera;
}

void FuryWorld::deleteCurrentCamera()
{
    delete m_currentCamera;
}

const QVector<FuryObject*>& FuryWorld::getObjects()
{
    return m_objects;
}
