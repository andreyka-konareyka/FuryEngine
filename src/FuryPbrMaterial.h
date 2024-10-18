#ifndef FURYPBRMATERIAL_H
#define FURYPBRMATERIAL_H

#include "FuryMaterial.h"
#include "FuryPhongMaterial.h"

#include <QString>

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

    /*!
     * \brief Установка текстуры основного цвета
     * \param[in] _texture - Название текстуры
     */
    inline void setAlbedoTexture(const QString& _texture)
    { m_albedoTexture = _texture; }

    /*!
     * \brief Получение текстуры основного цвета
     * \return Возвращает название текстуры
     */
    inline const QString& albedoTexture() const
    { return m_albedoTexture; }

    /*!
     * \brief Установка текстуры нормалей
     * \param[in] _texture - Название текстуры
     */
    inline void setNormalTexture(const QString& _texture)
    { m_normalTexture = _texture; }

    /*!
     * \brief Получение текстуры нормалей
     * \return Возвращает название текстуры
     */
    inline const QString& normalTexture() const
    { return m_normalTexture; }

    /*!
     * \brief Установка текстуры металличности
     * \param[in] _texture - Название текстуры
     */
    inline void setMetallicTexture(const QString& _texture)
    { m_metallicTexture = _texture; }

    /*!
     * \brief Получение текстуры металличности
     * \return Возвращает название текстуры
     */
    inline const QString& metallicTexture() const
    { return m_metallicTexture; }

    /*!
     * \brief Установка текстуры шероховатости
     * \param[in] _texture - Название текстуры
     */
    inline void setRoughnessTexture(const QString& _texture)
    { m_roughnessTexture = _texture; }

    /*!
     * \brief Получение текстуры шероховатости
     * \return Возвращает название текстуры
     */
    inline const QString& roughnessTexture() const
    { return m_roughnessTexture; }

    /*!
     * \brief Установка текстуры окружающего затенения
     * \param[in] _texture - Название текстуры
     */
    inline void setAoTexture(const QString& _texture)
    { m_aoTexture = _texture; }

    /*!
     * \brief Получение текстуры окружающего затенения
     * \return Возвращает название текстуры
     */
    inline const QString& aoTexture() const
    { return m_aoTexture; }

    /*!
     * \brief Установка основного цвета
     * \param[in] _color - Цвет
     */
    inline void setAlbedoColor(const glm::vec3& _color)
    { m_albedoColor = _color; }

    /*!
     * \brief Получение основного цвета
     * \return Возвращает основной цвет
     */
    inline const glm::vec3& albedoColor() const
    { return m_albedoColor; }

    /*!
     * \brief Установка металличности
     * \param[in] _metallic - Металличность
     */
    inline void setMetallic(float _metallic)
    { m_metallic = _metallic; }

    /*!
     * \brief Получение металличности
     * \return Возвращает металличность
     */
    inline float metallic() const
    { return m_metallic; }

    /*!
     * \brief Установка шероховатости
     * \param[in] _roughness - Шероховатость
     */
    inline void setRoughness(float _roughness)
    { m_roughness = _roughness; }

    /*!
     * \brief Получение шероховатости
     * \return Возвращает шероховатость
     */
    inline float roughness() const
    { return m_roughness; }

    /*!
     * \brief Установка окружающего затенения
     * \param[in] _ao - Окружающее затенение
     */
    inline void setAo(float _ao)
    { m_ao = _ao; }

    /*!
     * \brief Получение окружающего затенения
     * \return Возвращает окружающее затенение
     */
    inline float ao() const
    { return m_ao; }

    /*!
     * \brief Установка признака рендера с двух сторон
     * \param[in] _flag - Признак
     */
    inline void setTwoSided(bool _flag)
    { m_twoSided = _flag; }

    /*!
     * \brief Получение признака рендера с двух сторон
     * \return Возвращает признак
     */
    inline bool isTwoSided() const override
    { return m_twoSided; }

    /*!
     * \brief Установка непрозрачности
     * \param[in] _opacity - Непрозрачность
     */
    inline void setOpacity(float _opacity)
    { m_opacity = _opacity; }

    /*!
     * \brief Получение непрозрачности
     * \return Возвращает непрозрачность
     */
    inline float opacity() const override
    { return m_opacity; }

    /*!
     * \brief Перевод в JSON объект
     * \return Возвращает JSON объект
     */
    QJsonObject toJson() const override;

    /*!
     * \brief Перевод из JSON объекта
     * \param[in] _object - JSON объект
     * \return Возвращает материал
     */
    static FuryPbrMaterial* fromJson(const QJsonObject& _object);

    /*!
     * \brief Создание PBR-материала из обычного
     * \param[in] _material - Обычный материал
     * \return Возвращает PBR-материал
     */
    static FuryPbrMaterial* createFromMaterial(const FuryPhongMaterial* _material);

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

    //! Указывает, должны ли сетки отображаться без отбраковки обратных сторон
    bool m_twoSided;
    //! Непрозрачность материала
    float m_opacity;
};

#endif // FURYPBRMATERIAL_H
