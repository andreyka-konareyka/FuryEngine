#include "FuryModelManager.h"

#include "Logger/FuryLogger.h"
#include "Logger/FuryException.h"

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QFileInfo>

FuryModelManager* FuryModelManager::s_instance = nullptr;


FuryModelManager::FuryModelManager() :
    m_needStop(false),
    m_stopped(false)
{
    Debug(ru("Создание менеджера моделей"));

    std::thread(&FuryModelManager::infiniteLoop, this).detach();
}

FuryModelManager::~FuryModelManager()
{
    Debug(ru("Удаление менеджера моделей"));

    for (QMap<QString, FuryModel*>::Iterator iter = m_models.begin(); iter != m_models.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            FuryModel*& model = iter.value();
            Debug(ru("Удаление модели (%1)").arg(model->path().section('/', -1, -1)));
            delete model;
            model = nullptr;
        }
    }
}

FuryModelManager *FuryModelManager::instance()
{
    if (s_instance == nullptr)
    {
        return createInstance();
    }

    return s_instance;
}

FuryModelManager *FuryModelManager::createInstance()
{
    if (s_instance != nullptr)
    {
        throw FuryException(ru("Повторное создание менеджера моделей"));
    }

    s_instance = new FuryModelManager;
    return s_instance;
}

void FuryModelManager::deleteInstance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Удаление ещё не созданного менеджера моделей"));
    }

    delete s_instance;
    s_instance = nullptr;
}

void FuryModelManager::addModel(const QString &_path, const QString &_name)
{
    QMutexLocker mutexLocker(&m_modelsMutex);
    QString modelPath = QFileInfo(_path).absoluteFilePath();

    if (!m_models.contains(modelPath))
    {
        FuryModel* model = new FuryModel(_path);
        m_models.insert(modelPath, model);

        QMutexLocker mutexLocker2(&m_loadMutex);
        m_modelLoadQueue.enqueue(model);
    }

    if (!_name.isEmpty())
    {
        QMutexLocker mutexLocker3(&m_nameMutex);
        m_nameToPath.insert(_name, modelPath);
    }
}

FuryModel *FuryModelManager::modelByName(const QString &_name)
{
    QMutexLocker mutexLocker(&m_nameMutex);
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.constEnd() || pathIter.value().isEmpty())
    {
        return &m_emptyModel;
    }

    mutexLocker.unlock();
    return modelByPath(pathIter.value());
}

FuryModel *FuryModelManager::modelByPath(const QString &_path)
{
    QMutexLocker mutexLocker(&m_nameMutex);
    QString modelPath = QFileInfo(_path).absoluteFilePath();
    QMap<QString, FuryModel*>::ConstIterator modelIter = m_models.find(modelPath);

    if (modelIter == m_models.constEnd())
    {
        return &m_emptyModel;
    }

    return modelIter.value();
}

void FuryModelManager::loadModelPart()
{
    QMutexLocker mutexLocker(&m_bindMutex);

    if (!m_modelBindQueue.isEmpty())
    {
        FuryModel* model = m_modelBindQueue.dequeue();
        mutexLocker.unlock();

        model->setupMesh();

        Debug(ru("Загружена модель: (%1)").arg(model->path().section('/', -1, -1)));
    }
}

void FuryModelManager::stopLoopAndWait()
{
    m_needStop = true;

    while (!m_stopped)
    {
        QThread::msleep(200);
        Debug("Ожидание завершения потока загрузки моделей");
    }
}

QStringList FuryModelManager::allModelNames() const
{
    return m_nameToPath.keys();
}

QString FuryModelManager::pathByName(const QString &_name) const
{
    QMutexLocker mutexLocker(&m_nameMutex);
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.constEnd())
    {
        return "";
    }

    return pathIter.value();
}

void FuryModelManager::infiniteLoop()
{
    while (!m_needStop)
    {
        while (!m_modelLoadQueue.isEmpty())
        {
            QMutexLocker mutexLocker(&m_loadMutex);
            FuryModel* model = m_modelLoadQueue.dequeue();
            mutexLocker.unlock();

            if (!model->loadModel())
            {
                Debug(ru("Модель не загружена: %1").arg(model->path()));
                continue;
            }

            QMutexLocker mutexLocker2(&m_bindMutex);
            m_modelBindQueue.enqueue(model);
        }

        QThread::msleep(16); // (1000 / 60)
    }

    m_stopped = true;
}
