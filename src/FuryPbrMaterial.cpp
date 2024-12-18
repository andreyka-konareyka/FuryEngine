#include "FuryPbrMaterial.h"

#include "Shader.h"
#include "Managers/FuryTextureManager.h"

#include <QDir>

FuryPbrMaterial::FuryPbrMaterial() :
    m_albedoColor(glm::vec3(1, 1, 1)),
    m_metallic(0),
    m_roughness(0.3f),
    m_ao(1),
    m_twoSided(false),
    m_opacity(1)
{

}

void FuryPbrMaterial::setShaderMaterial(Shader *_shader)
{
    FuryTextureManager* textureManager = FuryTextureManager::instance();

    _shader->use();
    _shader->setVec3("material.albedoColor", m_albedoColor);
    _shader->setFloat("material.metallic", m_metallic);
    _shader->setFloat("material.roughness", m_roughness);
    _shader->setFloat("material.ao", m_ao);
    _shader->setFloat("material.opacity", m_opacity);

    bool albedoEnabled = false;
    bool normalEnabled = false;
    bool metallicEnabled = false;
    bool roughnessEnabled = false;
    bool aoEnabled = false;

    if (!m_albedoTexture.isEmpty())
    {
        albedoEnabled = true;
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_albedoTexture).idOpenGL());
    }

    if (!m_normalTexture.isEmpty())
    {
        normalEnabled = true;
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_normalTexture).idOpenGL());
    }

    if (!m_metallicTexture.isEmpty())
    {
        metallicEnabled = true;
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_metallicTexture).idOpenGL());
    }

    if (!m_roughnessTexture.isEmpty())
    {
        roughnessEnabled = true;
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_roughnessTexture).idOpenGL());
    }

    if (!m_aoTexture.isEmpty())
    {
        aoEnabled = true;
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_aoTexture).idOpenGL());
    }

    // Сбрасываем к стандартным настройкам
    glActiveTexture(GL_TEXTURE0);

    _shader->setInt("material.albedoEnabled", albedoEnabled);
    _shader->setInt("material.normalEnabled", normalEnabled);
    _shader->setInt("material.metallicEnabled", metallicEnabled);
    _shader->setInt("material.roughnessEnabled", roughnessEnabled);
    _shader->setInt("material.aoEnabled", aoEnabled);


    if (m_twoSided == true)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }
}

QJsonObject FuryPbrMaterial::toJson() const
{
    QJsonObject result;
    result["materialType"] = "FuryPbrMaterial";

    result["albedoColor"] = QString("(%1, %2, %3)")
                                    .arg(m_albedoColor.r)
                                    .arg(m_albedoColor.g)
                                    .arg(m_albedoColor.b);

    result["metallic"] = m_metallic;
    result["roughness"] = m_roughness;
    result["ao"] = m_ao;

    result["albedoTexture"] = m_albedoTexture;
    result["normalTexture"] = m_normalTexture;
    result["metallicTexture"] = m_metallicTexture;
    result["roughnessTexture"] = m_roughnessTexture;
    result["aoTexture"] = m_aoTexture;

    result["twoSided"] = m_twoSided;
    result["opacity"] = m_opacity;

    FuryTextureManager* manager = FuryTextureManager::instance();
    QDir dir = QDir::current();
    result["albedoPath"] = dir.relativeFilePath(manager->pathByName(m_albedoTexture));
    result["normalPath"] = dir.relativeFilePath(manager->pathByName(m_normalTexture));
    result["metallicPath"] = dir.relativeFilePath(manager->pathByName(m_metallicTexture));
    result["roughnessPath"] = dir.relativeFilePath(manager->pathByName(m_roughnessTexture));
    result["aoPath"] = dir.relativeFilePath(manager->pathByName(m_aoTexture));

    return result;
}

FuryPbrMaterial *FuryPbrMaterial::fromJson(const QJsonObject &_object)
{
    FuryPbrMaterial* result = new FuryPbrMaterial;

    QString albedoColor = _object["albedoColor"].toString();
    albedoColor.remove("(");
    albedoColor.remove(")");
    albedoColor.remove(" ");
    float r = albedoColor.section(",", 0, 0).toFloat();
    float g = albedoColor.section(",", 1, 1).toFloat();
    float b = albedoColor.section(",", 2, 2).toFloat();
    result->setAlbedoColor(glm::vec3(r, g, b));

    result->setMetallic(_object["metallic"].toDouble());
    result->setRoughness(_object["roughness"].toDouble());
    result->setAo(_object["ao"].toDouble());

    result->setAlbedoTexture(_object["albedoTexture"].toString());
    result->setNormalTexture(_object["normalTexture"].toString());
    result->setMetallicTexture(_object["metallicTexture"].toString());
    result->setRoughnessTexture(_object["roughnessTexture"].toString());
    result->setAoTexture(_object["aoTexture"].toString());

    result->setTwoSided(_object["twoSided"].toBool());
    result->setOpacity(_object["opacity"].toDouble());

    FuryTextureManager* manager = FuryTextureManager::instance();

    if (QString path = _object["albedoPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->albedoTexture());
    }
    if (QString path = _object["normalPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->normalTexture());
    }
    if (QString path = _object["metallicPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->metallicTexture());
    }
    if (QString path = _object["roughnessPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->roughnessTexture());
    }
    if (QString path = _object["aoPath"].toString(); !path.isEmpty())
    {
        manager->addTexture(path, result->aoTexture());
    }

    return result;
}

FuryPbrMaterial *FuryPbrMaterial::createFromMaterial(const FuryPhongMaterial *_material)
{
    FuryPbrMaterial* result = new FuryPbrMaterial;

    result->setAlbedoColor(_material->diffuseColor());
    result->setMetallic(_material->specularColor().r * 0.8);
    result->setRoughness(1 - _material->specularColor().r * 0.8);
    result->setTwoSided(_material->isTwoSided());
    result->setOpacity(_material->opacity());

    result->setAlbedoTexture(_material->diffuseTexture());
    result->setMetallicTexture(_material->specularTexture());
    result->setNormalTexture(_material->normalTexture());

    return result;
}
