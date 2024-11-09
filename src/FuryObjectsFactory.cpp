#include "FuryObjectsFactory.h"

#include "FuryObject.h"
#include "DefaultObjects/FuryBoxObject.h"
#include "DefaultObjects/FurySphereObject.h"
#include "CarObject.h"

#include <QJsonArray>

FuryObjectsFactory* FuryObjectsFactory::s_instance = nullptr;

FuryObjectsFactory *FuryObjectsFactory::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new FuryObjectsFactory;
    }

    return s_instance;
}

void FuryObjectsFactory::deleteInstance()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
    }
}

FuryObject *FuryObjectsFactory::fromJson(const QJsonObject &_json, FuryWorld *_world,
                                         FuryObject *_parent, bool _withoutJoint)
{
    FuryObject* result = nullptr;

    QString className = _json["className"].toString();
    QJsonArray childrenJson = _json["children"].toArray();

    if (className == FuryObject::staticMetaObject.className())
    {
        result = new FuryObject(_world, _parent, _withoutJoint);
    }
    else if (className == FuryBoxObject::staticMetaObject.className())
    {
        result = new FuryBoxObject(_world, _parent, _withoutJoint);
    }
    else if (className == FurySphereObject::staticMetaObject.className())
    {
        result = new FurySphereObject(_world, _parent, _withoutJoint);
    }
    else if (className == CarObject::staticMetaObject.className())
    {
        result = new CarObject(_world);
    }

    if (result == nullptr)
    {
        return nullptr;
    }

    result->fromJson(_json);
    emit createObjectSignal(result);

    for (int i = 0; i < childrenJson.size(); ++i)
    {
        fromJson(childrenJson.at(i).toObject(), _world, result, _withoutJoint);
    }

    return result;
}

FuryObjectsFactory::FuryObjectsFactory() :
    QObject()
{
    m_classNames << FuryObject::staticMetaObject.className()
                 << FuryBoxObject::staticMetaObject.className()
                 << FurySphereObject::staticMetaObject.className()
                 << CarObject::staticMetaObject.className();
}
