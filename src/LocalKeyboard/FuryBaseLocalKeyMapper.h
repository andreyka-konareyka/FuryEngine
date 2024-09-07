#ifndef FURYLOCALEKEYMAPPER_H
#define FURYLOCALEKEYMAPPER_H

#include <QString>

//! Базовый класс отображения локальных клавиш на латинские
class FuryBaseLocalKeyMapper
{
public:
    /*!
     * \brief Конструктор
     * \param[in] _name - Название
     */
    FuryBaseLocalKeyMapper(const QString& _name);

    /*!
     * \brief mapLocalKeyToLatin
     * \param[in] _localKey - Локальный код клавиши
     * \return Возвращает латинский код клавиши
     */
    virtual int mapLocalKeyToLatin(int _localKey) const = 0;

    /*!
     * \brief mapLatinKeyToLocal
     * \param[in] _latinKey - Латинский код клавиши
     * \return Возвращает локальный код клавиши
     */
    virtual int mapLatinKeyToLocal(int _latinKey) const = 0;

    /*!
     * \brief Получить название отображения
     * \return Возвращает название
     */
    const QString& name() const;

private:
    //! Название
    QString m_name;
};

#endif // FURYLOCALEKEYMAPPER_H
