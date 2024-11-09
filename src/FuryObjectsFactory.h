#ifndef FURYOBJECTSFACTORY_H
#define FURYOBJECTSFACTORY_H

#include <QJsonObject>

class FuryObject;
class FuryWorld;

//! Фабрика объектов
class FuryObjectsFactory : public QObject
{
    Q_OBJECT

public:
    //! Получение экземпляра класса
    static FuryObjectsFactory* instance();
    //! Удаление экземпляра класса
    static void deleteInstance();

    /*!
     * \brief Создание объекта из JSON
     * \param[in] _json - JSON объект
     * \param[in] _world - Мир, к которому относится объект
     * \param[in] _parent - Родительский объект
     * \param[in] _withoutJoint - Без создания сустава
     * \return Возвращает созданный объект
     */
    FuryObject* fromJson(const QJsonObject& _json, FuryWorld* _world,
                         FuryObject* _parent = nullptr, bool _withoutJoint = false);

    /*!
     * \brief Названия классов, которые могут быть созданы данной фабрикой
     * \return Возвращает список названий классов
     */
    inline const QStringList& classNames() const
    { return m_classNames; }

signals:
    /*!
     * \brief Сигнал создания объекта
     * \param[in] _obj - Объект
     */
    void createObjectSignal(FuryObject* _obj);

private:
    //! Названия классов, которые могут быть созданы данной фабрикой
    QStringList m_classNames;

private:
    //! Конструктор
    FuryObjectsFactory();
    //! Экземпляр класса
    static FuryObjectsFactory* s_instance;

    FuryObjectsFactory(const FuryObjectsFactory&) = delete;
    FuryObjectsFactory& operator=(const FuryObjectsFactory&) = delete;
};

#endif // FURYOBJECTSFACTORY_H
