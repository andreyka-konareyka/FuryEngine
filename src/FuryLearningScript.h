#ifndef FURYLEARNINGSCRIPT_H
#define FURYLEARNINGSCRIPT_H

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

//! Класс скрипта python для обучения ИИ
class FuryLearningScript
{
public:
    //! Конструктор
    FuryLearningScript();

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

#endif // FURYLEARNINGSCRIPT_H
