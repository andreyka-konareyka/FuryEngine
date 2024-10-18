#ifndef FURYMATERIAL_H
#define FURYMATERIAL_H

#include <QJsonObject>

class Shader;

//! Класс материалов
class FuryMaterial
{
public:
    //! Конструктор
    FuryMaterial();

    //! Деструктор
    virtual ~FuryMaterial();

    /*!
     * \brief Получение непрозрачности
     * \return Возвращает непрозрачность
     */
    virtual float opacity() const = 0;

    /*!
     * \brief Получение признака рендеринга с двух сторон
     * \return Возвращает признак рендеринга с двух сторон
     */
    virtual bool isTwoSided() const
    { return false; }

    /*!
     * \brief Установка материала в шейдер
     * \param[in] _shader - Шейдер
     */
    virtual void setShaderMaterial(Shader *_shader) = 0;

    /*!
     * \brief Перевод в JSON объект
     * \return Возвращает JSON объект
     */
    virtual QJsonObject toJson() const = 0;
};

#endif // FURYMATERIAL_H
