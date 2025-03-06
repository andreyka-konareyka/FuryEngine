#include "FuryShaderManager.h"

#include "Logger/FuryLogger.h"
#include "Logger/FuryException.h"
#include "Shader.h"

FuryShaderManager* FuryShaderManager::s_instance = nullptr;


FuryShaderManager::FuryShaderManager() :
    m_defaultShader(new Shader())
{
    Debug(ru("Создание менеджера шейдеров"));
}

FuryShaderManager::~FuryShaderManager()
{
    Debug(ru("Удаление менеджера шейдеров"));

    for (QMap<QString, Shader*>::Iterator iter = m_shaders.begin(); iter != m_shaders.end(); ++iter)
    {
        if (iter.value() != nullptr)
        {
            Debug(ru("Удаление шейдера: (%1)").arg(iter.key()));
            delete iter.value();
        }
    }

    if (m_defaultShader != nullptr)
    {
        delete m_defaultShader;
        m_defaultShader = nullptr;
    }
}

FuryShaderManager *FuryShaderManager::instance()
{
    if (s_instance == nullptr)
    {
        return createInstance();
    }

    return s_instance;
}

FuryShaderManager *FuryShaderManager::createInstance()
{
    if (s_instance != nullptr)
    {
        throw FuryException(ru("Повторное создание менеджера шейдеров"));
    }

    s_instance = new FuryShaderManager;
    return s_instance;
}

void FuryShaderManager::deleteInstance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Удаление ещё не созданного менеджера шейдеров"));
    }

    delete s_instance;
    s_instance = nullptr;
}

Shader *FuryShaderManager::createShader(const QString &_name,
                                        const QString &_vertexPath,
                                        const QString &_fragmentPath,
                                        const QString &_geometryPath)
{
    if (m_shaders.contains(_name))
    {
        Debug(ru("[ ВНИМАНИЕ ] Шейдер (%1) уже существовал.").arg(_name));
        return m_shaders.value(_name);
    }

    Debug(ru("Создание шейдера: (%1)").arg(_name));

    Shader* shader = new Shader(_vertexPath, _fragmentPath, _geometryPath);
    m_shaders.insert(_name, shader);
    return shader;
}

Shader *FuryShaderManager::shaderByName(const QString &_name)
{
    if (m_shaders.contains(_name))
    {
        return m_shaders.value(_name);
    }

    return m_defaultShader;
}

void FuryShaderManager::deleteShader(const QString &_name)
{
    if (!m_shaders.contains(_name))
    {
        Debug(ru("[ ВНИМАНИЕ ] Попытка удаления несуществующего шейдера (%1)")
              .arg(_name));
        return;
    }

    Shader* shader = m_shaders.value(_name);
    delete shader;
    m_shaders.remove(_name);
}

bool FuryShaderManager::containsShader(const QString &_name)
{
    return m_shaders.contains(_name);
}

QStringList FuryShaderManager::allShaderNames() const
{
    return m_shaders.keys();
}
