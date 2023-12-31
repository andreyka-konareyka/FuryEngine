#ifndef FURYMATERIAL_H
#define FURYMATERIAL_H

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

    /*!
     * \brief Установить материал в шейдер
     * \param[in] _shader - Шейдер
     */
    void setShaderMaterial(Shader* _shader);

    /*!
     * \brief Создание материала по материалу из Assimp
     * \param[in] _assimpMaterial - Материал Assimp
     * \return Возвращает материал по материалу из Assimp
     */
    static FuryMaterial* createFromAssimp(const aiMaterial* _assimpMaterial,
                                          const QString& _path);

private:
    //! Название материала
    QString m_name;
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

    //! Список текстур
    QList<QString> m_textures;
};

#endif // FURYMATERIAL_H
