#ifndef FURYSHADERMANAGER_H
#define FURYSHADERMANAGER_H

#include <QMap>

class Shader;

class FuryShaderManager
{
public:
    static FuryShaderManager* instance();
    static FuryShaderManager* createInstance();
    static void deleteInstance();

    Shader* createShader(const QString& _name,
                         const QString& _vertexPath,
                         const QString& _fragmentPath,
                         const QString& _geometryPath = QString());

    Shader* shaderByName(const QString& _name);

    void deleteShader(const QString& _name);

    /*!
     * \brief Проверка существования шейдера
     * \param[in] _name - Название шейдера
     * \return Возвращает признак существования
     */
    bool containsShader(const QString& _name);

    QStringList allShaderNames() const;

private:
    FuryShaderManager();
    ~FuryShaderManager();
    static FuryShaderManager* s_instance;

    FuryShaderManager(const FuryShaderManager&) = delete;
    FuryShaderManager& operator=(const FuryShaderManager&) = delete;

private:
    QMap<QString, Shader*> m_shaders;
    Shader* m_defaultShader;
};

#endif // FURYSHADERMANAGER_H
