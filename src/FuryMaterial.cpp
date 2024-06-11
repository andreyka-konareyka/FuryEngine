#include "FuryMaterial.h"

#include <assimp/material.h>

#include "FuryLogger.h"
#include "Shader.h"
#include "FuryTextureManager.h"

#include <QDir>

FuryMaterial::FuryMaterial() :
    FuryMaterial(glm::vec3(1, 1, 1),
                 glm::vec3(1, 1, 1))
{

}

FuryMaterial::FuryMaterial(const glm::vec3 &_diffuseColor,
                           const glm::vec3 &_specularColor) :
    m_diffuseColor(_diffuseColor),
    m_specularColor(_specularColor)
{

}

FuryMaterial::~FuryMaterial()
{

}

void FuryMaterial::setShaderMaterial(Shader *_shader)
{
    _shader->Use();
    _shader->setFloat("material.shininess", 32);
    _shader->setVec3("material.diffuseColor", m_diffuseColor);
    _shader->setVec3("material.specularColor", m_specularColor);
    _shader->setFloat("material.opacity", m_opacity);
    _shader->setInt("material.textureDiffuse", 0);
    _shader->setInt("material.textureSpecular", 1);
    _shader->setInt("material.textureNormal", 2);
    _shader->setInt("shadowMap", 8);

    bool diffuseTextureEnabled = false;
    bool specularTextureEnabled = false;
    bool normalTextureEnabled = false;

    if (!m_diffuseTexture.isEmpty())
    {
        diffuseTextureEnabled = true;

        GLuint textureId = FuryTextureManager::instance()->textureByName(m_diffuseTexture).idOpenGL();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    if (!m_specularTexture.isEmpty())
    {
        specularTextureEnabled = true;

        GLuint textureId = FuryTextureManager::instance()->textureByName(m_specularTexture).idOpenGL();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    if (!m_normalTexture.isEmpty())
    {
        normalTextureEnabled = true;

        GLuint textureId = FuryTextureManager::instance()->textureByName(m_normalTexture).idOpenGL();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    _shader->setInt("material.diffuseTextureEnabled", diffuseTextureEnabled);
    _shader->setInt("material.specularTextureEnabled", specularTextureEnabled);
    _shader->setInt("material.normalTextureEnabled", normalTextureEnabled);

#pragma message("depthMap - Надо как-то сюда передать")
    // m_depthMap - Пока что устанавливаем в 0
//    glActiveTexture(GL_TEXTURE3);
//    glBindTexture(GL_TEXTURE_2D, 0);

    // Сбрасываем к стандартным настройкам
    glActiveTexture(GL_TEXTURE0);


    if (m_twoSided == true)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }
}

QJsonObject FuryMaterial::toJson() const
{
    QJsonObject result;
    result["materialType"] = "FuryMaterial";

    result["diffuseColor"] = QString("(%1, %2, %3)")
                                    .arg(m_diffuseColor.r)
                                    .arg(m_diffuseColor.g)
                                    .arg(m_diffuseColor.b);
    result["specularColor"] = QString("(%1, %2, %3)")
                                    .arg(m_specularColor.r)
                                    .arg(m_specularColor.g)
                                    .arg(m_specularColor.b);
    result["ambientColor"] = QString("(%1, %2, %3)")
                                    .arg(m_ambientColor.r)
                                    .arg(m_ambientColor.g)
                                    .arg(m_ambientColor.b);

    result["twoSided"] = m_twoSided;
    result["shadingModel"] = m_shadingModel;
    result["blendFunc"] = m_blendFunc;
    result["opacity"] = m_opacity;

    result["diffuseTexture"] = m_diffuseTexture;
    result["specularTexture"] = m_specularTexture;
    result["normalTexture"] = m_normalTexture;

    FuryTextureManager* manager = FuryTextureManager::instance();
    QDir dir = QDir::current();
    result["diffusePath"] = dir.relativeFilePath(manager->pathByName(m_diffuseTexture));
    result["specularPath"] = dir.relativeFilePath(manager->pathByName(m_specularTexture));
    result["normalPath"] = dir.relativeFilePath(manager->pathByName(m_normalTexture));

    return result;
}

FuryMaterial *FuryMaterial::fromJson(const QJsonObject &_object)
{
    FuryMaterial* result = new FuryMaterial;

    QString diffuseColor = _object["diffuseColor"].toString();
    diffuseColor.remove("(");
    diffuseColor.remove(")");
    diffuseColor.remove(" ");
    float r1 = diffuseColor.section(",", 0, 0).toFloat();
    float g1 = diffuseColor.section(",", 1, 1).toFloat();
    float b1 = diffuseColor.section(",", 2, 2).toFloat();
    result->setDiffuseColor(glm::vec3(r1, g1, b1));

    QString specularColor = _object["specularColor"].toString();
    specularColor.remove("(");
    specularColor.remove(")");
    specularColor.remove(" ");
    float r2 = specularColor.section(",", 0, 0).toFloat();
    float g2 = specularColor.section(",", 1, 1).toFloat();
    float b2 = specularColor.section(",", 2, 2).toFloat();
    result->m_specularColor = glm::vec3(r2, g2, b2);

    QString ambientColor = _object["ambientColor"].toString();
    ambientColor.remove("(");
    ambientColor.remove(")");
    ambientColor.remove(" ");
    float r3 = ambientColor.section(",", 0, 0).toFloat();
    float g3 = ambientColor.section(",", 1, 1).toFloat();
    float b3 = ambientColor.section(",", 2, 2).toFloat();
    result->m_ambientColor = glm::vec3(r3, g3, b3);

    result->m_twoSided = _object["twoSided"].toBool();
    result->m_shadingModel = _object["shadingModel"].toInt();
    result->m_blendFunc = _object["blendFunc"].toInt();
    result->m_opacity = _object["opacity"].toDouble();

    result->m_diffuseTexture = _object["diffuseTexture"].toString();
    result->m_specularTexture = _object["specularTexture"].toString();
    result->m_normalTexture = _object["normalTexture"].toString();

    FuryTextureManager* manager = FuryTextureManager::instance();

    if (QString path = _object["diffusePath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->m_diffuseTexture);
    }
    if (QString path = _object["specularPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->m_specularTexture);
    }
    if (QString path = _object["normalPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->m_normalTexture);
    }

    return result;
}

FuryMaterial *FuryMaterial::createFromAssimp(const aiMaterial *_assimpMaterial,
                                             const QString &_path)
{
    FuryMaterial* result = new FuryMaterial;

    aiColor3D diffuseColor;
    _assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    result->m_diffuseColor = glm::vec3(diffuseColor.r,
                                       diffuseColor.g,
                                       diffuseColor.b);

    aiColor3D specularColor;
    _assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    result->m_specularColor = glm::vec3(specularColor.r,
                                        specularColor.g,
                                        specularColor.b);

    aiColor3D ambientColor;
    _assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
    result->m_ambientColor = glm::vec3(ambientColor.r,
                                       ambientColor.g,
                                       ambientColor.b);

    int twoSoded = false;
    _assimpMaterial->Get(AI_MATKEY_TWOSIDED, twoSoded);
    result->m_twoSided = twoSoded;

    int shadingModel = 0;
    _assimpMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
    result->m_shadingModel = shadingModel;

    int blendFunc = 0;
    _assimpMaterial->Get(AI_MATKEY_BLEND_FUNC, blendFunc);
    result->m_blendFunc = blendFunc;

    float opacity = 1;
    _assimpMaterial->Get(AI_MATKEY_OPACITY, opacity);
    result->m_opacity = opacity;


    for (int i = 0; i < 18; ++i)
    {
        aiTextureType type = (aiTextureType)i;
        int textureCount = _assimpMaterial->GetTextureCount(type);

        for (int n = 0; n < textureCount; ++n)
        {
            if (n != 0)
            {
                Debug(ru("Ошибка при загрузке текстур из материала модели. "
                         "Мы пока не обрабатываем больше одной текстуры одного типа."));
                continue;
            }

            aiString textureName;
            _assimpMaterial->GetTexture(type, n, &textureName);

            QString textureType = aiTextureTypeToString(type);
            QString loadTextureName = textureType + "_" + ru(textureName.C_Str());

            if ((aiTextureType)i == aiTextureType_DIFFUSE)
            {
                result->setDiffuseTexture(loadTextureName);
            }

            if ((aiTextureType)i == aiTextureType_SPECULAR)
            {
                result->setSpecularTexture(loadTextureName);
            }

            if ((aiTextureType)i == aiTextureType_HEIGHT)
            {
                result->setNormalTexture(loadTextureName);
            }

            FuryTextureManager* texManager = FuryTextureManager::instance();
            texManager->addTexture(_path + textureName.C_Str(), loadTextureName);
        }
    }

    return result;
}
