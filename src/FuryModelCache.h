#ifndef FURYMODELCACHE_H
#define FURYMODELCACHE_H

#include <QObject>

class FuryModel;


//! Кэш модели
class FuryModelCache : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _modelName - Название модели
     */
    FuryModelCache(const QString& _modelName = QString());

    /*!
     * \brief Установка названия модели
     * \param[in] _modelName - Название модели
     */
    void setModelName(const QString& _modelName);

    /*!
     * \brief Модель
     * \return Возвращает модель
     */
    const FuryModel& model() const;

private slots:
    /*!
     * \brief Слот перезапроса модели у менеджера
     * \param[in] _modelName - Название модели
     */
    void requestModelSlot(const QString& _modelName);

private:
    //! Инициализация соединений сигналов и слотов
    void initConnections();

private:
    //! Название модели
    QString m_modelName;
    //! Модель
    const FuryModel* m_model;
};

#endif // FURYMODELCACHE_H
