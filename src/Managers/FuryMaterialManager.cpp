#include "FuryMaterialManager.h"

#include "Logger/FuryLogger.h"
#include "Logger/FuryException.h"
#include "FuryMaterial.h"
#include "FuryPbrMaterial.h"
#include "FuryPhongMaterial.h"

#include <QFile>
#include <QJsonDocument>

FuryMaterialManager* FuryMaterialManager::s_instance = nullptr;


FuryMaterialManager::FuryMaterialManager() :
    m_defaultMaterial(new FuryPbrMaterial)
{
    Debug(ru("Создание менеджера материалов"));
}

FuryMaterialManager::~FuryMaterialManager()
{
//    saveMaterials();
    Debug(ru("Удаление менеджера материалов"));

    for (QMap<QString, FuryMaterial*>::Iterator iter = m_materials.begin(); iter != m_materials.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            Debug(ru("Удаление материала: (%1)").arg(iter.key()));
            delete iter.value();
        }
    }

    delete m_defaultMaterial;
    m_defaultMaterial = nullptr;
}

FuryMaterialManager *FuryMaterialManager::instance()
{
    if (s_instance == nullptr)
    {
        return createInstance();
    }

    return s_instance;
}

FuryMaterialManager *FuryMaterialManager::createInstance()
{
    if (s_instance != nullptr)
    {
        throw FuryException(ru("Повторное создание менеджера материалов"));
    }

    s_instance = new FuryMaterialManager;
    return s_instance;
}

void FuryMaterialManager::deleteInstance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Удаление ещё не созданного менеджера материалов"));
    }

    delete s_instance;
    s_instance = nullptr;
}

FuryPhongMaterial *FuryMaterialManager::createPhongMaterial(const QString &_name)
{
    return createUserMaterial<FuryPhongMaterial>(_name);
}

FuryPbrMaterial *FuryMaterialManager::createPbrMaterial(const QString &_name)
{
    return createUserMaterial<FuryPbrMaterial>(_name);
}

FuryMaterial *FuryMaterialManager::materialByName(const QString &_name)
{
    if (m_materials.contains(_name))
    {
        return m_materials.value(_name);
    }

    return m_defaultMaterial;
}

void FuryMaterialManager::deleteMaterial(const QString &_name)
{
    if (!m_materials.contains(_name))
    {
        Debug(ru("[ ВНИМАНИЕ ] Попытка удаления несуществующего материала (%1)")
              .arg(_name));
        return;
    }

    FuryMaterial* material = m_materials.value(_name);
    delete material;
    m_materials.remove(_name);
}

bool FuryMaterialManager::materialExist(const QString &_name)
{
    return m_materials.contains(_name);
}

QList<QString> FuryMaterialManager::allMaterialNames() const
{
    return m_materials.keys();
}

bool FuryMaterialManager::tryLoadMaterial(const QString &_name)
{
    QFile file("materials/" + _name + ".json");

    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = document.object();
    QString type = object["materialType"].toString();
    FuryMaterial* mat = NULL;

    if (type == "FuryPhongMaterial")
    {
        mat = FuryPhongMaterial::fromJson(object);
    }
    else if (type == "FuryPbrMaterial")
    {
        mat = FuryPbrMaterial::fromJson(object);
    }

    if (mat == NULL)
    {
        return false;
    }


    Debug(ru("Импортирован материал: (%1)").arg(_name));
    m_materials.insert(_name, mat);

    return true;
}

void FuryMaterialManager::saveMaterials()
{
    for (QMap<QString, FuryMaterial*>::Iterator iter = m_materials.begin(); iter != m_materials.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            QFile file("materials/" + iter.key() + ".json");

            if (!file.open(QIODevice::WriteOnly))
            {
                qDebug() << ru("Не удалось сохранить файл (%1)").arg(iter.key());
                continue;
            }

            QJsonDocument document(iter.value()->toJson());
            file.write(document.toJson());
            file.close();
        }
    }
}
