#include "FuryWorldManager.h"

#include "Logger/FuryLogger.h"

#include <QMutexLocker>

FuryWorldManager* FuryWorldManager::s_instance = nullptr;


FuryWorldManager::FuryWorldManager() :
    m_physicsCommon(new rp3d::PhysicsCommon),
    m_defaultWorld(new FuryWorld(m_physicsCommon))
{
    Debug(ru("Создание менеджера миров"));
}

FuryWorldManager::~FuryWorldManager()
{
    Debug(ru("Удаление менеджера миров"));

    for (QMap<QString, FuryWorld*>::Iterator iter = m_worlds.begin(); iter != m_worlds.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            Debug(ru("Удаление мира (%1)").arg(iter.key()));
            delete iter.value();
        }
    }

    m_worlds.clear();

    Debug(ru("Удаление мира по умолчанию"));
    delete m_defaultWorld;
    m_defaultWorld = nullptr;
}

FuryWorldManager *FuryWorldManager::instance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Менеджер миров ещё не создан"),
                            "s_instance == nullptr",
                            Q_FUNC_INFO);
    }

    return s_instance;
}

FuryWorldManager *FuryWorldManager::createInstance()
{
    if (s_instance != nullptr)
    {
        throw FuryException(ru("Повторное создание менеджера миров"),
                            "s_instance != nullptr",
                            Q_FUNC_INFO);
    }

    s_instance = new FuryWorldManager;
    return s_instance;
}

void FuryWorldManager::deleteInstance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Удаление ещё не созданного менеджера миров"),
                            "s_instance == nullptr",
                            Q_FUNC_INFO);
    }

    delete s_instance;
    s_instance = nullptr;
}

FuryWorld &FuryWorldManager::createWorld(const QString &_name)
{
    QMutexLocker locker(&m_worldsMutex);

    if (m_worlds.contains(_name))
    {
        Debug(ru("Попытка повторного создания мира (%1)").arg(_name));
        return *m_worlds[_name];
    }

    FuryWorld* newWorld = new FuryWorld(m_physicsCommon);
    newWorld->setObjectName(_name);
    m_worlds.insert(_name, newWorld);
    return *m_worlds[_name];
}

FuryWorld &FuryWorldManager::worldByName(const QString &_name) const
{
    QMutexLocker locker(&m_worldsMutex);

    if (m_worlds.contains(_name))
    {
        return *m_worlds[_name];
    }

    Debug(ru("Мир (%1) не создан. Возвращаю мир по умолчанию").arg(_name));
    return *m_defaultWorld;
}

void FuryWorldManager::deleteWorldByName(const QString &_name)
{
    QMutexLocker locker(&m_worldsMutex);

    if (m_worlds.contains(_name))
    {
        delete m_worlds[_name];
        m_worlds[_name] = nullptr;
        m_worlds.remove(_name);
    }
    else
    {
        Debug(ru("Попытка удаления несуществующего мира (%1)").arg(_name));
    }
}

bool FuryWorldManager::worldExist(const QString &_name) const
{
    QMutexLocker locker(&m_worldsMutex);
    return (m_worlds.contains(_name));
}

