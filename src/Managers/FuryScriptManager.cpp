#include "FuryScriptManager.h"

#include "FuryObject.h"
#include "Logger/FuryLogger.h"
#include "Logger/FuryException.h"


using namespace boost::python;


/// \note Сейчас определяются в файле PythonAPI/FuryObject_Python.cpp
extern "C" PyObject* PyInit_glm();
extern "C" PyObject* PyInit_FuryObject();
extern "C" PyObject* PyInit_CarObject();



FuryScriptManager* FuryScriptManager::s_instance = nullptr;


FuryScriptManager::FuryScriptManager()
{
    Debug(ru("Создание менеджера скриптов"));

    qputenv("PYTHONHOME", "../libs/python");
    qputenv("PYTHONPATH", ".");

    Py_Initialize();

    object __main__ = object(handle<>(borrowed(PyImport_AddModule("__main__"))));

    m_modules.insert("glm", object(handle<>(borrowed(PyInit_glm()))));
    m_modules.insert("FuryObject", object(handle<>(borrowed(PyInit_FuryObject()))));
    m_modules.insert("CarObject", object(handle<>(borrowed(PyInit_CarObject()))));

    QMapIterator<QString, object> iter(m_modules);
    while (iter.hasNext())
    {
        iter.next();
        objects::add_to_namespace(__main__, qUtf8Printable(iter.key()), iter.value());
    }
}

FuryScriptManager::~FuryScriptManager()
{
    Debug(ru("Удаление менеджера скриптов"));
}

FuryScriptManager *FuryScriptManager::instance()
{
    if (s_instance == nullptr)
    {
        return createInstance();
    }

    return s_instance;
}

FuryScriptManager *FuryScriptManager::createInstance()
{
    if (s_instance != nullptr)
    {
        throw FuryException(ru("Повторное создание менеджера скриптов"));
    }

    s_instance = new FuryScriptManager;
    return s_instance;
}

void FuryScriptManager::deleteInstance()
{
    if (s_instance == nullptr)
    {
        throw FuryException(ru("Удаление ещё не созданного менеджера скриптов"));
    }

    delete s_instance;
    s_instance = nullptr;
}

void FuryScriptManager::importScript(const QString &_name)
{
    if (m_scripts.contains(_name))
    {
        return;
    }

    try
    {
        object script = import(qUtf8Printable(_name));
        QMapIterator<QString, object> iter(m_modules);
        while (iter.hasNext())
        {
            iter.next();
            objects::add_to_namespace(script, qUtf8Printable(iter.key()), iter.value());
        }

        m_scripts.insert(_name, script);
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }
}

void FuryScriptManager::closeScript(const QString &_name)
{
    if (!m_scripts.contains(_name))
    {
        return;
    }

    m_scripts.remove(_name);
}

void FuryScriptManager::createObject(FuryObject *_object, const QString &_scriptName)
{
    if (_object == nullptr || !m_scripts.contains(_scriptName))
    {
        return;
    }

    try
    {
        object type = m_scripts.value(_scriptName).attr("Behaviour");
        object pythonObject = type(boost::python::ptr(_object));
        m_objects.insert(_object, pythonObject);
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }
}

void FuryScriptManager::removeObject(FuryObject *_object)
{
    if (!m_objects.contains(_object))
    {
        return;
    }

    m_objects.remove(_object);
}

void FuryScriptManager::processStart()
{
    foreach (object pythonObject, m_objects)
    {
        try
        {
            object startFunc = pythonObject.attr("start");
            startFunc();
        }
        catch (error_already_set)
        {
            PyErr_Print();
        }
    }
}

void FuryScriptManager::processUpdate()
{
    foreach (object pythonObject, m_objects)
    {
        try
        {
            object startFunc = pythonObject.attr("update");
            startFunc();
        }
        catch (error_already_set)
        {
            PyErr_Print();
        }
    }
}

void FuryScriptManager::processStop()
{
    foreach (object pythonObject, m_objects)
    {
        try
        {
            object startFunc = pythonObject.attr("stop");
            startFunc();
        }
        catch (error_already_set)
        {
            PyErr_Print();
        }
    }
}
