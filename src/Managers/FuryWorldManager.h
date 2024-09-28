#ifndef FURYWORLDMANAGER_H
#define FURYWORLDMANAGER_H


#include "FuryWorld.h"

#include <reactphysics3d/reactphysics3d.h>

#include <QMap>
#include <QMutex>
#include <QString>

//! Класс менеджера миров
class FuryWorldManager
{
public:
    /*!
     * \brief Получение экземпляра класса
     * \return Возвращает экземпляр класса
     * \throw ExceptionStruct - При несозданном объекте
     */
    static FuryWorldManager* instance();

    /*!
     * \brief Создание экземпляра класса
     * \return Возвращает экземпляр класса
     * \throw FuryException - При повторном создании
     */
    static FuryWorldManager* createInstance();

    /*!
     * \brief Удаление экземпляра класса
     * \throw FuryException - При удалении пустого
     */
    static void deleteInstance();

    /*!
     * \brief Создание мира
     * \param[in] _name - Название мира
     * \return Возвращает созданный мир
     */
    FuryWorld& createWorld(const QString& _name);

    /*!
     * \brief Получение мира по названию
     * \param[in] _name - Название мира
     * \return Возвращает мир, если он есть. Мир по умолчанию, если его нет.
     */
    FuryWorld& worldByName(const QString& _name) const;

    /*!
     * \brief Удаление мира по названию
     * \param[in] _name - Название мира
     */
    void deleteWorldByName(const QString& _name);

    /*!
     * \brief Проверка существования мира
     * \param[in] _name - Название мира
     * \return Возвращает признак существования
     */
    bool worldExist(const QString& _name) const;

private:
    //! Конструктор
    FuryWorldManager();
    //! Деструктор
    ~FuryWorldManager();
    //! Экземпляр класса
    static FuryWorldManager* s_instance;

    //! Запрещаем конструктор копирования
    FuryWorldManager(const FuryWorldManager&) = delete;
    //! Запрещаем оператор присваивания
    FuryWorldManager& operator=(const FuryWorldManager&) = delete;

private:
    rp3d::PhysicsCommon* m_physicsCommon;

    //! Отображение: Название мира -> Мир
    QMap<QString, FuryWorld*> m_worlds;
    //! Мир по умолчанию
    FuryWorld* m_defaultWorld;

    //! Мьютекс для миров
    mutable QMutex m_worldsMutex;
};

#endif // FURYWORLDMANAGER_H
