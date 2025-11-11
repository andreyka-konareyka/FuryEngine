#include "FuryModelCache.h"

#include "Managers/FuryModelManager.h"


FuryModelCache::FuryModelCache(const QString &_modelName) :
    QObject(),
    m_modelName(_modelName),
    m_model(&FuryModelManager::instance()->modelByName(m_modelName))
{
    initConnections();
}

void FuryModelCache::setModelName(const QString &_modelName)
{
    m_modelName = _modelName;
    requestModelSlot(m_modelName);
}

const FuryModel &FuryModelCache::model() const
{
    return *m_model;
}

void FuryModelCache::requestModelSlot(const QString &_modelName)
{
    if (m_modelName == _modelName)
    {
        m_model = &FuryModelManager::instance()->modelByName(m_modelName);
    }
}

void FuryModelCache::initConnections()
{
    FuryModelManager* manager = FuryModelManager::instance();
    connect(manager, &FuryModelManager::addModelSignal,
            this, &FuryModelCache::requestModelSlot);
    connect(manager, &FuryModelManager::editModelSignal,
            this, &FuryModelCache::requestModelSlot);
    connect(manager, &FuryModelManager::deleteModelSignal,
            this, &FuryModelCache::requestModelSlot);
}
