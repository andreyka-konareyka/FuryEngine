#ifndef FURYDOUBLEVALIDATOR_H
#define FURYDOUBLEVALIDATOR_H

#include <QDoubleValidator>

//! Кастомный класс валидатора
class FuryDoubleValidator : public QDoubleValidator
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский виджет
     */
    FuryDoubleValidator(QObject* _parent = nullptr);

    /*!
     * \brief Валидация
     * \param[in,out] _string - Строка. Запятые заменяются точками
     * \param[in,out] _position - Позиция
     * \return Возвращает состояние валидации
     */
    State validate(QString& _string, int& _position) const override;
};

#endif // FURYDOUBLEVALIDATOR_H
