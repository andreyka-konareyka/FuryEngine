#ifndef FURYPBRMATERIAL_H
#define FURYPBRMATERIAL_H

#include "FuryMaterial.h"

//! Класс PBR материала
class FuryPbrMaterial : public FuryMaterial
{
public:
    //! Конструктор
    FuryPbrMaterial();

    /*!
     * \brief Установка материала в шейдер
     * \param[in] _shader - Шейдер
     */
    void setShaderMaterial(Shader *_shader) override;

    inline void setAlbedoTexture(const QString& _texture)
    { m_albedoTexture = _texture; }

    inline void setNormalTexture(const QString& _texture)
    { m_normalTexture = _texture; }

    inline void setMetallicTexture(const QString& _texture)
    { m_metallicTexture = _texture; }

    inline void setRoughnessTexture(const QString& _texture)
    { m_roughnessTexture = _texture; }

    inline void setAoTexture(const QString& _texture)
    { m_aoTexture = _texture; }

private:
    //! Основной цвет
    glm::vec3 m_albedoColor;
    //! Металличность
    float m_metallic;
    //! Шероховатость
    float m_roughness;
    //! Окружающее затенение
    float m_ao;

    //! Текстура основного цвета
    QString m_albedoTexture;
    //! Текстура нормалей
    QString m_normalTexture;
    //! Текстура металличности
    QString m_metallicTexture;
    //! Текстура шероховатости
    QString m_roughnessTexture;
    //! Текстура окружающего затенения
    QString m_aoTexture;
};

#endif // FURYPBRMATERIAL_H
