#ifndef FURYSCRIPT_H
#define FURYSCRIPT_H

#include <QVector>

namespace boost
{
    namespace python
    {
        namespace api
        {
            class object;
        }
    }
};

//! Класс скрипта python
class FuryScript
{
public:
    //! Конструктор
    FuryScript();

    /*!
     * \brief Предсказание действия бота по наблюдению
     * \param[in] _observation - Наблюдение
     * \return Возвращает предсказанное действие
     */
    int predict(const QVector<float> _observation);

    /*!
     * \brief Обучение ИИ
     * \param[in] _observation - Новое наблюдение
     * \param[in] _reward - Награда за предыдущее действие
     * \param[in] _stop - Обозначает конец игры
     * \return Возвращает предсказанное действие для нового наблюдения
     */
    int learn(const QVector<float> _observation, float _reward, bool _stop);

    //! Сохранение модели
    void saveModel();

private:
    //! Модуль
    boost::python::api::object* m_module;
};

#endif // FURYSCRIPT_H
