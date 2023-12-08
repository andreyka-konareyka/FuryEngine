#include "World.h"


World::World() :
    m_currentCamera(nullptr)
{

}

World::~World()
{
    for (Object* object : m_objects)
    {
        delete object;
    }

    delete m_currentCamera;
}

void World::Tick(double _dt)
{
    for (Object* object : m_objects)
    {
        object->Tick(_dt);
    }
}

void World::addObject(Object* _object)
{
    m_objects.push_back(_object);
}

void World::setCamera(Camera* _camera)
{
    m_currentCamera = _camera;
}

void World::deleteCurrentCamera()
{
    delete m_currentCamera;
}

const QVector<Object*>& World::getObjects()
{
    return m_objects;
}
