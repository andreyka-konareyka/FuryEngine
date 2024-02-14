#include "FuryPbrMaterial.h"

#include "Shader.h"
#include "FuryTextureManager.h"

FuryPbrMaterial::FuryPbrMaterial() :
    m_albedoColor(0),
    m_metallic(0),
    m_roughness(0),
    m_ao(1)
{

}

void FuryPbrMaterial::setShaderMaterial(Shader *_shader)
{
    FuryTextureManager* textureManager = FuryTextureManager::instance();

    _shader->Use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_albedoTexture).idOpenGL());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_normalTexture).idOpenGL());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_metallicTexture).idOpenGL());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_roughnessTexture).idOpenGL());
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, textureManager->textureByName(m_aoTexture).idOpenGL());
}
