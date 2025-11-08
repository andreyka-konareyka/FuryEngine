#include "FuryTextureCache.h"

#include "Managers/FuryTextureManager.h"


FuryTextureCache::FuryTextureCache(const QString &_textureName) :
    QObject(),
    m_textureName(_textureName),
    m_texture(&FuryTextureManager::instance()->textureByName(m_textureName))
{
    initConnections();
}

void FuryTextureCache::setTextureName(const QString &_textureName)
{
    m_textureName = _textureName;
    requestTextureSlot(m_textureName);
}

const FuryTexture &FuryTextureCache::texture() const
{
    return *m_texture;
}

void FuryTextureCache::requestTextureSlot(const QString &_textureName)
{
    if (m_textureName == _textureName)
    {
        m_texture = &FuryTextureManager::instance()->textureByName(m_textureName);
    }
}

void FuryTextureCache::initConnections()
{
    FuryTextureManager* manager = FuryTextureManager::instance();
    connect(manager, &FuryTextureManager::addTextureSignal,
            this, &FuryTextureCache::requestTextureSlot,
            Qt::QueuedConnection);
    connect(manager, &FuryTextureManager::editTextureSignal,
            this, &FuryTextureCache::requestTextureSlot,
            Qt::QueuedConnection);
    connect(manager, &FuryTextureManager::deleteTextureSignal,
            this, &FuryTextureCache::requestTextureSlot,
            Qt::QueuedConnection);
}
