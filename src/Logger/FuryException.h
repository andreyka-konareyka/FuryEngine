#ifndef FURYEXCEPTION_H
#define FURYEXCEPTION_H

#include <QString>

//! Класс исключений
class FuryException
{
public:
    /*!
    * \brief Конструктор
    * \param[in] _userInfo - Информация для отображения пользователю
    * \param[in] _debugInfo - Информация для отладки
    */
    FuryException(const QString& _userInfo = QString(),
                  const QString& _debugInfo = QString());

    /*!
    * \brief Получить информацию для отображения пользователю
    * \return Возвращает информацию для отображения пользователю
    */
    inline const QString& userInfo() const
    { return m_userInfo; }

    /*!
    * \brief Получить информацию отладки
    * \return Возвращает информацию для отладки
    */
    inline const QString& debugInfo() const
    { return m_debugInfo; }

private:
    //! Информация для отображения пользователю
    QString m_userInfo;
    //! Информация для отладки
    QString m_debugInfo;
};



#endif // FURYEXCEPTION_H
