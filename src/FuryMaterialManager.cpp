#include "FuryMaterialManager.h"

#include "FuryLogger.h"
#include "FuryException.h"
#include "FuryMaterial.h"

FuryMaterialManager* FuryMaterialManager::s_instance = nullptr;


FuryMaterialManager::FuryMaterialManager() :
    m_defaultMaterial(new FuryMaterial)
{
    Debug(ru("Создание менеджера материалов"));
}

FuryMaterialManager::~FuryMaterialManager()
{
    Debug(ru("Удаление менеджера материалов"));

    for (QMap<QString, FuryMaterial*>::Iterator iter = m_materials.begin(); iter != m_materials.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            Debug(ru("Удаление материала: (%1)").arg(iter.key()));
            delete iter.value();
        }
    }
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

FuryMaterial *FuryMaterialManager::createMaterial(const QString &_name)
{
    if (m_materials.contains(_name))
    {
        Debug(ru("[ ВНИМАНИЕ ] Материал (%1) уже существовал.").arg(_name));
        return m_materials.value(_name);
    }

    Debug(ru("Создан материал: (%1)").arg(_name));

    FuryMaterial* material = new FuryMaterial;
    m_materials.insert(_name, material);
    return material;
}

FuryMaterial *FuryMaterialManager::materialByName(const QString &_name)
{
    if (m_materials.contains(_name))
    {
        return m_materials.value(_name);
    }

    return m_defaultMaterial;
}

bool FuryMaterialManager::materialExist(const QString &_name)
{
    return m_materials.contains(_name);
}
