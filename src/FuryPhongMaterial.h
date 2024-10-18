#ifndef FURYPHONGMATERIAL_H
#define FURYPHONGMATERIAL_H

#include "FuryMaterial.h"

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

class FuryPhongMaterial : public FuryMaterial
{
public:
    FuryPhongMaterial();

    /*!
     * \brief Конструктор
     * \param[in] _diffuseColor - Рассеянный цвет материала
     * \param[in] _specularColor - Зеркальный цвет материала
     */
    FuryPhongMaterial(const glm::vec3& _diffuseColor,
                      const glm::vec3& _specularColor);
    //! Деструктор
    ~FuryPhongMaterial();

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

    /*!
     * \brief Получение диффузной текстуры
     * \return Возвращает диффузную текстуру
     */
    inline const QString& diffuseTexture() const
    { return m_diffuseTexture; }

    /*!
     * \brief Получение текстуры отражений
     * \return Возвращает текстуру отражений
     */
    inline const QString& specularTexture() const
    { return m_specularTexture; }

    /*!
     * \brief Получение текстуры нормалей
     * \return Возвращает текстуру нормалей
     */
    inline const QString& normalTexture() const
    { return m_normalTexture; }

    /*!
     * \brief Получение диффузного цвета
     * \return Возвращает диффузный цвет
     */
    inline const glm::vec3& diffuseColor() const
    { return m_diffuseColor; }

    /*!
     * \brief Установка диффузного цвета
     * \param[in] _color - Диффузный цвет
     */
    inline void setDiffuseColor(const glm::vec3& _color)
    { m_diffuseColor = _color; }

    /*!
     * \brief Получение цвета отражения
     * \return Возвращает цвет отражения
     */
    inline const glm::vec3& specularColor() const
    { return m_specularColor; }

    /*!
     * \brief Установка цвета отражения
     * \param[in] _color - Цвет отражения
     */
    inline void setSpecularColor(const glm::vec3& _color)
    { m_specularColor = _color; }

    /*!
     * \brief Получение признака рендеринга с двух сторон
     * \return Возвращает признак рендеринга с двух сторон
     */
    inline bool isTwoSided() const override
    { return m_twoSided; }

    /*!
     * \brief Получение непрозрачности
     * \return Возвращает непрозрачность
     */
    inline float opacity() const override
    { return m_opacity; }

    /*!
     * \brief Установить материал в шейдер
     * \param[in] _shader - Шейдер
     */
    void setShaderMaterial(Shader* _shader) override;

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
    static FuryPhongMaterial* fromJson(const QJsonObject& _object);

    /*!
     * \brief Создание материала по материалу из Assimp
     * \param[in] _assimpMaterial - Материал Assimp
     * \param[in] _path - Путь к текстурам
     * \return Возвращает материал по материалу из Assimp
     */
    static FuryPhongMaterial* createFromAssimp(const aiMaterial* _assimpMaterial,
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

#endif // FURYPHONGMATERIAL_H
