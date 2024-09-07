#ifndef FURYLOGGER_H
#define FURYLOGGER_H

#include "FuryException.h"

#include <QDebug>

//! Класс логирования
class FuryLogger
{
public:
    /*!
     * \brief Получение экземпляра класса
     * \return Возвращает экземпляр класса
     */
    static FuryLogger* instance();

    /*!
     * \brief Логирование строки. Выводит в консоль отладочную информацию.
     * \param[in] _func - Название функции
     * \return Возвращает QDebug для вывода отладочной информации
     */
    QDebug log(const QString& _func);

    /*!
     * \brief Логирование исключения
     * \param[in] _exception - Исключение
     */
    void logException(const FuryException& _exception);

private:
    //! Конструктор
    FuryLogger();
};

//! Логирование
#define Debug(_debugText) FuryLogger::instance()->log(__FUNCTION__) << _debugText

#define Log(_exception) FuryLogger::instance()->logException(_exception);

//! Перевод русских символов в QString
#define ru QString::fromUtf8

#endif // FURYLOGGER_H
