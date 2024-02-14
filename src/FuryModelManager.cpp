#include "FuryModelManager.h"

#include "FuryLogger.h"
#include "FuryException.h"

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
            Debug(ru("Удаление модели (%1)").arg(model->path()));
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
    static QMutex mutexAddModel;
    QMutexLocker mutexLocker(&mutexAddModel);


    QString modelPath = QFileInfo(_path).absoluteFilePath();
    QMap<QString, FuryModel*>::ConstIterator iter = m_models.find(modelPath);

    if (iter == m_models.constEnd())
    {
        FuryModel* model = new FuryModel(_path);
        m_models.insert(modelPath, model);
        m_modelLoadQueue.enqueue(model);
    }

    if (!_name.isEmpty())
    {
        m_nameToPath.insert(_name, modelPath);
    }
}

FuryModel *FuryModelManager::modelByName(const QString &_name)
{
    QMap<QString, QString>::ConstIterator pathIter = m_nameToPath.find(_name);

    if (pathIter == m_nameToPath.constEnd() || pathIter.value().isEmpty())
    {
        return &m_emptyModel;
    }

    return modelByPath(pathIter.value());
}

FuryModel *FuryModelManager::modelByPath(const QString &_path)
{
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
    if (!m_modelBindQueue.isEmpty())
    {
        FuryModel* model = m_modelBindQueue.dequeue();
        model->setupMesh();

        Debug(ru("Загружена модель: (%1)").arg(model->path()));
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

void FuryModelManager::infiniteLoop()
{
    static QMutex mutexInfiniteLoop;

    while (!m_needStop)
    {
        while (!m_modelLoadQueue.isEmpty())
        {
            QMutexLocker mutexLocker(&mutexInfiniteLoop);

            FuryModel* model = m_modelLoadQueue.dequeue();

            if (!model->loadModel())
            {
                Debug(ru("Модель не загружена: %1").arg(model->path()));
                continue;
            }

            m_modelBindQueue.enqueue(model);
        }
    }

    m_stopped = true;
}
