#include "FuryWorld.h"


FuryWorld::FuryWorld() :
    m_currentCamera(nullptr)
{
    Debug(ru("Создание игрового мира"));
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
    m_allObjects.push_back(_object);
}

void FuryWorld::addTransparentObject(FuryObject *_object)
{
    m_transparentObjects.push_back(_object);
    m_allObjects.push_back(_object);
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

const QVector<FuryObject *> &FuryWorld::getTransparentObjects()
{
    return m_transparentObjects;
}

const QVector<FuryObject *> &FuryWorld::getAllObjects()
{
    return m_allObjects;
}
