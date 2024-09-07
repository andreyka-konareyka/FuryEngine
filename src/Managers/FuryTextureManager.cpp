#include "FuryTextureManager.h"

#include "Logger/FuryLogger.h"

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <QMutex>
#include <QFileInfo>
#include <QString>
#include <QThread>

FuryTextureManager* FuryTextureManager::s_instance = nullptr;


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
            Debug(ru("Удаление текстуры (%1)").arg(texture->path()));
            delete texture;
            texture = nullptr;
        }
    }
}

FuryTextureManager *FuryTextureManager::instance()
{
    if (s_instance == nullptr)
    {
        return createInstance();
    }

    return s_instance;
}

FuryTextureManager *FuryTextureManager::createInstance()
{
    if (s_instance != nullptr)
    {
        throw FuryException(ru("Повторное создание текстурного менеджера"));
    }

    s_instance = new FuryTextureManager;
    return s_instance;
}

void FuryTextureManager::deleteInstance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Удаление ещё не созданного текстурного менеджера"));
    }

    delete s_instance;
    s_instance = nullptr;
}

void FuryTextureManager::addTexture(const QString &_path, const QString &_name)
{
    QMutexLocker mutexLocker(&m_textureMutex);
    QString texturePath = QFileInfo(_path).absoluteFilePath();

    if (!m_textures.contains(texturePath))
    {
        FuryTexture* texture = new FuryTexture(texturePath);
        m_textures.insert(texturePath, texture);

        QMutexLocker mutexLocker2(&m_loadMutex);
        m_textureLoadQueue.enqueue(texture);
    }

    if (!_name.isEmpty())
    {
        QMutexLocker mutexLocker(&m_nameMutex);
        m_nameToPath.insert(_name, texturePath);
    }
}

const FuryTexture& FuryTextureManager::textureByName(const QString& _name) const
{
    QMutexLocker mutexLocker(&m_nameMutex);
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.constEnd() || pathIter.value().isEmpty())
    {
        return m_emptyTexture;
    }

    mutexLocker.unlock();
    return textureByPath(pathIter.value());
}

const FuryTexture& FuryTextureManager::textureByPath(const QString& _path) const
{
    QMutexLocker mutexLocker(&m_textureMutex);
    QString texturePath = QFileInfo(_path).absoluteFilePath();
    QMap<QString, FuryTexture*>::ConstIterator textureIter = m_textures.find(texturePath);

    if (textureIter == m_textures.constEnd() || !(textureIter.value()->isReady()))
    {
        return m_emptyTexture;
    }

    return *(textureIter.value());
}

QString FuryTextureManager::pathByName(const QString &_name) const
{
    QMutexLocker mutexLocker(&m_nameMutex);
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.constEnd())
    {
        return "";
    }

    return pathIter.value();
}

void FuryTextureManager::loadTexturePart()
{
    if (!m_textureBindQueue.isEmpty())
    {
        GLuint textureID = 0;
        glGenTextures(1, &textureID);

        QMutexLocker mutexLocker(&m_bindMutex);
        FuryTexture* texture = m_textureBindQueue.dequeue();
        mutexLocker.unlock();

        texture->setIdOpenGL(textureID);
        int width = texture->width();
        int height = texture->height();
        unsigned char* data = texture->data();

        if (data == nullptr)
        {
            Debug(ru("Ошибка при загрузке текстуры (%1)").arg(texture->path()));
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

        Debug(ru("Текстура загружена: (%1) (id %2)").arg(texture->path()).arg(textureID));
    }
}

void FuryTextureManager::stopLoopAndWait()
{
    m_needStop = true;

    while (!m_stopped)
    {
        QThread::msleep(200);
        Debug("Ожидание завершения потока загрузки текстур");
    }
}



void FuryTextureManager::infiniteLoop()
{
    while (!m_needStop)
    {
        while (!m_textureLoadQueue.isEmpty())
        {
            QMutexLocker mutexLocker(&m_loadMutex);
            FuryTexture* texture = m_textureLoadQueue.dequeue();
            mutexLocker.unlock();

            int width, height;
            unsigned char* data = stbi_load(qUtf8Printable(texture->path()), &width, &height, 0, STBI_rgb_alpha);

            if (width == 0 || height == 0 || data == 0)
            {
                Debug(ru("Текстура не загружена: (%1)").arg(texture->path()));
                continue;
            }

            texture->setData(data);
            texture->setWidth(width);
            texture->setHeight(height);
            texture->setLoaded();

            QMutexLocker mutexLocker2(&m_bindMutex);
            m_textureBindQueue.enqueue(texture);
            mutexLocker2.unlock();
        }
    }

    m_stopped = true;
}
