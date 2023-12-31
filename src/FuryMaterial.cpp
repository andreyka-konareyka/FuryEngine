#include "FuryMaterial.h"

#include <assimp/material.h>

#include "FuryLogger.h"
#include "Shader.h"
#include "FuryTextureManager.h"

FuryMaterial::FuryMaterial()
{

}

FuryMaterial::FuryMaterial(const glm::vec3 &_diffuseColor,
                           const glm::vec3 &_specularColor) :
    m_diffuseColor(_diffuseColor),
    m_specularColor(_specularColor)
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
    _shader->setInt("shadowMap", 3);

    bool diffuseTextureEnabled = false;
    bool specularTextureEnabled = false;
    bool normalTextureEnabled = false;

    QString diffuseStart = aiTextureTypeToString(aiTextureType_DIFFUSE);
    diffuseStart.append("_");
    QString specularStart = aiTextureTypeToString(aiTextureType_SPECULAR);
    specularStart.append("_");
    QString diffuseRoughnessStart = aiTextureTypeToString(aiTextureType_DIFFUSE_ROUGHNESS);
    diffuseRoughnessStart.append("_");
    QString heightStart = aiTextureTypeToString(aiTextureType_HEIGHT);
    heightStart.append("_");

    for (const QString& textureName : m_textures)
    {
        if (textureName.startsWith(diffuseStart))
        {
            diffuseTextureEnabled = true;

            GLuint textureId = FuryTextureManager::instance()->textureByName(textureName).idOpenGL();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
        else if (textureName.startsWith(specularStart)
                 || textureName.startsWith(diffuseRoughnessStart))
        {
            specularTextureEnabled = true;

            GLuint textureId = FuryTextureManager::instance()->textureByName(textureName).idOpenGL();
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
        else if (textureName.startsWith(heightStart))
        {
            normalTextureEnabled = true;

            GLuint textureId = FuryTextureManager::instance()->textureByName(textureName).idOpenGL();
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
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

FuryMaterial *FuryMaterial::createFromAssimp(const aiMaterial *_assimpMaterial,
                                             const QString &_path)
{
    FuryMaterial* result = new FuryMaterial;

    aiString materialName;
    _assimpMaterial->Get(AI_MATKEY_NAME, materialName);
    result->m_name = ru(materialName.C_Str());

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
            result->m_textures.append(loadTextureName);

            FuryTextureManager* texManager = FuryTextureManager::instance();
            texManager->addTextureFromAnotherThread(_path + textureName.C_Str(),
                                                    loadTextureName);
        }
    }

    return result;
}
















