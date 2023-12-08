#include "FuryWorld.h"


FuryWorld::FuryWorld() :
    m_currentCamera(nullptr)
{

}

FuryWorld::~FuryWorld()
{
    for (FuryObject* object : m_objects)
    {
        delete object;
    }

    delete m_currentCamera;
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
