#ifndef FURYMATERIAL_H
#define FURYMATERIAL_H

#include <QJsonObject>
#include <QString>
#include <QList>
#include <QPair>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct aiMaterial;
class Shader;

//! Класс материалов
class FuryMaterial
{
public:
    //! Конструктор
    FuryMaterial();

    /*!
     * \brief Конструктор
     * \param[in] _diffuseColor - Рассеянный цвет материала
     * \param[in] _specularColor - Зеркальный цвет материала
     */
    FuryMaterial(const glm::vec3& _diffuseColor,
                 const glm::vec3& _specularColor);

    //! Деструктор
    virtual ~FuryMaterial();

    /*!
     * \brief Установка диффузной текстуры
     * \param[in] _diffuseTexture - Название диффузной текстуры
     */
    inline void setDiffuseTexture(const QString& _diffuseTexture)
    { m_diffuseTexture = _diffuseTexture; }

    /*!
     * \brief Установка текстуры отражений
     * \param[in] _specularTexture - Название текстуры отражений
     */
    inline void setSpecularTexture(const QString& _specularTexture)
    { m_specularTexture = _specularTexture; }

    /*!
     * \brief Установка текстуры нормалей
     * \param[in] _normalTexture - Название текстуры нормалей
     */
    inline void setNormalTexture(const QString& _normalTexture)
    { m_normalTexture = _normalTexture; }


    inline const QString& diffuseTexture() const
    { return m_diffuseTexture; }

    inline const QString& specularTexture() const
    { return m_specularTexture; }

    inline const QString& normalTexture() const
    { return m_normalTexture; }

    inline const glm::vec3& diffuseColor() const
    { return m_diffuseColor; }

    inline void setDiffuseColor(const glm::vec3& _color)
    { m_diffuseColor = _color; }

    inline const glm::vec3& specularColor() const
    { return m_specularColor; }

    inline bool twoSided() const
    { return m_twoSided; }

    inline float opacity() const
    { return m_opacity; }

    /*!
     * \brief Установить материал в шейдер
     * \param[in] _shader - Шейдер
     */
    virtual void setShaderMaterial(Shader* _shader);

    /*!
     * \brief Перевод в JSON объект
     * \return Возвращает JSON объект
     */
    virtual QJsonObject toJson() const;

    /*!
     * \brief Создание материала по материалу из Assimp
     * \param[in] _assimpMaterial - Материал Assimp
     * \return Возвращает материал по материалу из Assimp
     */
    static FuryMaterial* createFromAssimp(const aiMaterial* _assimpMaterial,
                                          const QString& _path);

private:
    //! Рассеянный цвет материала
    glm::vec3 m_diffuseColor;
    //! Зеркальный цвет материала
    glm::vec3 m_specularColor;
    //! Окружающий цвет материала
    glm::vec3 m_ambientColor;

    //! Указывает, должны ли сетки отображаться без отбраковки обратных сторон
    bool m_twoSided;
    //! Модель затенения для рендеринга
    int m_shadingModel;
    //! Как вычисляется смешивание
    int m_blendFunc;
    //! Непрозрачность материала
    float m_opacity;

    //! Текстура рассеянного цвета
    QString m_diffuseTexture;
    //! Текстура зеркального цвета
    QString m_specularTexture;
    //! Текстура нормалей
    QString m_normalTexture;
};

#endif // FURYMATERIAL_H
