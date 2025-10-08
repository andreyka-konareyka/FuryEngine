#ifndef FURYSCRIPTMANAGER_H
#define FURYSCRIPTMANAGER_H

#include <boost/python.hpp>


#include <QMap>


class FuryObject;


//! Менеджер скриптов
class FuryScriptManager
{
public:
    static FuryScriptManager* instance();
    static FuryScriptManager* createInstance();
    static void deleteInstance();

    /*!
     * \brief Импорт модуля
     * \param[in] _name - Название модуля
     */
    void importScript(const QString& _name);

    /*!
     * \brief Закрытие модуля
     * \param[in] _name - Название модуля
     */
    void closeScript(const QString& _name);

    /*!
     * \brief Создание экземпляра объекта-скрипта
     * \param[in] _object - Объект, для которого создастся объект-скрипт
     * \param[in] _scriptName - Название модуля скрипта
     */
    void createObject(FuryObject* _object, const QString& _scriptName);

    /*!
     * \brief Удаление экземпляра объекта-скрипта
     * \param[in] _object - Объект, для которого есть объект-скрипт
     */
    void removeObject(FuryObject* _object);

    //! Обработка метода start для объектов-скриптов
    void processStart();
    //! Обработка метода update для объектов-скриптов
    void processUpdate();
    //! Обработка метода stop для объектов-скриптов
    void processStop();

private:
    FuryScriptManager();
    ~FuryScriptManager();
    static FuryScriptManager* s_instance;

    FuryScriptManager(const FuryScriptManager&) = delete;
    FuryScriptManager& operator=(const FuryScriptManager&) = delete;

private:
    //! Модули, созданные с обёрткой boost::python для классов C++
    QMap<QString, boost::python::object> m_modules;
    //! Испортированные модули (скрипты)
    QMap<QString, boost::python::object> m_scripts;
    //! Экземпляры объектов-скриптов
    QMap<FuryObject*, boost::python::object> m_objects;
};

#endif // FURYSCRIPTMANAGER_H
