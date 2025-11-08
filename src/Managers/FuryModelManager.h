#ifndef FURYMODELMANAGER_H
#define FURYMODELMANAGER_H

#include "FuryModel.h"

#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QObject>
#include <QString>


//! Класс менеджера моделей
class FuryModelManager : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Получение экземпляра класса
     * \return Возвращает экземпляр класса
     */
    static FuryModelManager* instance();
    /*!
     * \brief Создание экземпляра класса
     * \return Возвращает экземпляр класса
     */
    static FuryModelManager* createInstance();
    //! Удаление экземпляра класса
    static void deleteInstance();

    /*!
     * \brief Добавление модели
     * \param[in] _path - Путь к файлу модели
     * \param[in] _name - Название модели
     */
    void addModel(const QString& _path, const QString& _name = "");
    /*!
     * \brief Получение модели по названию
     * \param[in] _name - Название модели
     * \return Возвращает модель
     */
    FuryModel* modelByName(const QString& _name);
    /*!
     * \brief Получение модели по пути
     * \param[in] _path - Путь к файлу модели
     * \return Возвращает модель
     */
    FuryModel* modelByPath(const QString& _path);

    //! Загрузить часть моделей в OpenGL
    void loadModelPart();

    //! Остановка вспомогательного потока с вечным циклом
    void stopLoopAndWait();

    /*!
     * \brief Получение всех названий моделей
     * \return Возвращает список названий
     */
    QStringList allModelNames() const;

    /*!
     * \brief Получить путь по названию
     * \param[in] _name - Название
     * \return Возвращает путь
     */
    QString pathByName(const QString& _name) const;

signals:
    //! Сигнал, что какие-то модели готовы к подсоединению
    void modelLoadedSignal();

private:
    //! Конструктор
    FuryModelManager();
    //! Деструктор
    ~FuryModelManager();
    //! Экземпляр класса
    static FuryModelManager* s_instance;

    //! Запрещаем конструктор копирования
    FuryModelManager(const FuryModelManager&) = delete;
    //! Запрещаем оператор присваивания
    FuryModelManager& operator=(const FuryModelManager&) = delete;

    //! Бесконечный цикл
    void infiniteLoop();

    //! Нужно ли останавливать работу вспомогательного потока с вечным циклом
    bool m_needStop;
    //! Остановлен ли вспомогательный поток с вечным циклом
    bool m_stopped;

private:
    //! Отображение: Название модели -> Модель
    QMap<QString, FuryModel*> m_models;
    //! Отображение: Название модели -> Путь к файлу модели
    QMap<QString, QString> m_nameToPath;

    //! Очередь моделей на загрузку
    QQueue<FuryModel*> m_modelLoadQueue;
    //! Очередь моделей на подключение к OpenGL
    QQueue<FuryModel*> m_modelBindQueue;

    //! Мьютекс для m_models
    mutable QMutex m_modelsMutex;
    //! Мьютекс для m_nameToPath
    mutable QMutex m_nameMutex;
    //! Мьютекс для m_modelLoadQueue
    mutable QMutex m_loadMutex;
    //! Мьютекс для m_modelBindQueue
    mutable QMutex m_bindMutex;

    //! Пустая модель по умолчанию
    FuryModel m_emptyModel;
};

#endif // FURYMODELMANAGER_H
