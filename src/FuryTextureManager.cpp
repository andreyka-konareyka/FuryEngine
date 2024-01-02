#include "FuryTextureManager.h"

#include "FuryLogger.h"

#include <string>

#include <SOIL.h>

#include <QMutex>
#include <QFileInfo>
#include <QString>
#include <QThread>

FuryTextureManager::FuryTextureManager() :
    m_needStop(false),
    m_stopped(false)
{
    Debug(ru("Создание текстурного менеджера"));

    std::thread(&FuryTextureManager::infiniteLoop, this).detach();
    m_emptyTexture.setReady();
}

FuryTextureManager::~FuryTextureManager()
{
    Debug(ru("Удаление текстурного менеджера"));

    for (QMap<QString, FuryTexture*>::Iterator iter = m_textures.begin(); iter != m_textures.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            FuryTexture*& texture = iter.value();
            Debug("Delete texture (" + texture->path() + ")");
            delete texture;
            texture = nullptr;
        }
    }
}

void FuryTextureManager::addTexture(const QString &_path, const QString &_name)
{
    static QMutex mutexAddTexture;
    QMutexLocker mutexLocker(&mutexAddTexture);


    QFileInfo texturePath(_path);

    FuryTexture* texture;
    QMap<QString, FuryTexture*>::ConstIterator iter = m_textures.find(texturePath.absoluteFilePath());

    if (iter == m_textures.constEnd())
    {
        GLuint textureID;
        glGenTextures(1, &textureID);

        texture = new FuryTexture(textureID, _path);
        m_textures.insert( texturePath.absoluteFilePath(), texture);
        m_textureLoadQueue.enqueue(texture);
    }
    else
    {
        texture = iter.value();
    }

    if (!_name.isEmpty())
    {
        m_nameToPath.insert(_name, texture->path());
    }
}

const FuryTexture& FuryTextureManager::textureByName(const QString& _name) const
{
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.end() || pathIter.value().isEmpty())
    {
        return m_emptyTexture;
    }

    return textureByPath(pathIter.value());
}

const FuryTexture& FuryTextureManager::textureByPath(const QString& _path) const
{
    QFileInfo texturePath(_path);
    QMap<QString, FuryTexture*>::ConstIterator textureIter = m_textures.find(texturePath.absoluteFilePath());

    if (textureIter == m_textures.end() || !(textureIter.value()->isReady()))
    {
        return m_emptyTexture;
    }

    return *(textureIter.value());
}

void FuryTextureManager::loadTexturePart()
{
    if (!m_textureBindQueue.isEmpty())
    {
        FuryTexture* texture = m_textureBindQueue.dequeue();
        GLuint textureID = texture->idOpenGL();
        int width = texture->width();
        int height = texture->height();
        unsigned char* data = texture->data();

        if (data == nullptr)
        {
            Debug("Error while loading texture (" + texture->path() + ")");
            return;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Очищаем память
        texture->setData(nullptr);
        delete data;
        data = nullptr;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        texture->setReady();

        Debug("Texture loaded: (" + texture->path() + ")");
    }
}

void FuryTextureManager::stopLoopAndWait()
{
    m_needStop = true;

    while (!m_stopped)
    {
        QThread::msleep(200);
        Debug("wait stop");
    }
}



void FuryTextureManager::infiniteLoop()
{
    static QMutex mutexInfiniteLoop;
    while (!m_needStop)
    {
        while (!m_textureLoadQueue.isEmpty())
        {
            QMutexLocker mutexLocker(&mutexInfiniteLoop);

            FuryTexture* texture = m_textureLoadQueue.dequeue();

            int width, height;
            unsigned char* data = SOIL_load_image(qUtf8Printable(texture->path()), &width, &height, 0, SOIL_LOAD_RGBA);

            texture->setData(data);
            texture->setWidth(width);
            texture->setHeight(height);
            texture->setLoaded();
            m_textureBindQueue.enqueue(texture);
        }
    }

    m_stopped = true;
}
