#include "FuryTextureManager.h"

#include "FuryLogger.h"

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
            Debug("Delete texture (" + texture->path() + ")");
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
    static QMutex mutexAddTexture;
    QMutexLocker mutexLocker(&mutexAddTexture);


    QString texturePath = QFileInfo(_path).absoluteFilePath();
    QMap<QString, FuryTexture*>::ConstIterator iter = m_textures.find(texturePath);

    if (iter == m_textures.constEnd())
    {
        GLuint textureID = 0;
        glGenTextures(1, &textureID);

        FuryTexture* texture = new FuryTexture(textureID, _path);
        m_textures.insert(texturePath, texture);
        m_textureLoadQueue.enqueue(texture);
    }

    if (!_name.isEmpty())
    {
        m_nameToPath.insert(_name, texturePath);
    }
}

void FuryTextureManager::addTextureFromAnotherThread(const QString &_path, const QString &_name)
{
    m_texturesAddQueue.enqueue(qMakePair(_path, _name));
}

const FuryTexture& FuryTextureManager::textureByName(const QString& _name) const
{
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.constEnd() || pathIter.value().isEmpty())
    {
        return m_emptyTexture;
    }

    return textureByPath(pathIter.value());
}

const FuryTexture& FuryTextureManager::textureByPath(const QString& _path) const
{
    QFileInfo texturePath(_path);
    QMap<QString, FuryTexture*>::ConstIterator textureIter = m_textures.find(texturePath.absoluteFilePath());

    if (textureIter == m_textures.constEnd() || !(textureIter.value()->isReady()))
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


    if (!m_texturesAddQueue.isEmpty())
    {
        QPair<QString, QString> pair = m_texturesAddQueue.dequeue();
        addTexture(pair.first, pair.second);
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
            unsigned char* data = stbi_load(qUtf8Printable(texture->path()), &width, &height, 0, STBI_rgb_alpha);

            if (width == 0 || height == 0 || data == 0)
            {
                Debug(ru("Текстура не загружена: %1").arg(texture->path()));
                continue;
            }

            texture->setData(data);
            texture->setWidth(width);
            texture->setHeight(height);
            texture->setLoaded();
            m_textureBindQueue.enqueue(texture);
        }
    }

    m_stopped = true;
}
