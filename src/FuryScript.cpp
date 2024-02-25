#include <boost/python.hpp>

#include "FuryScript.h"

#include <QDebug>
#include <QDir>

#include <string>



using namespace boost::python;

FuryScript::FuryScript()
{
    qputenv("PYTHONPATH", ".");

    Py_Initialize();
    try {
        object o_firstModule = import("scripts.learn_script");
        m_module = new object(o_firstModule);
    }
    catch (error_already_set) {
        PyErr_Print();
    }
}

int FuryScript::predict(const QVector<float> _observation)
{
    try
    {
        list pythonList;

        for (int i = 0; i < _observation.size(); ++i)
        {
            pythonList.append(_observation[i]);
        }

        object o_funcPredict = m_module->attr("predict");
        object result = o_funcPredict(pythonList);

        int resultAction = extract<int>(result);
        return resultAction;
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }

    return 0;
}

int FuryScript::learn(const QVector<float> _observation, float _reward)
{
    try
    {
        list pythonList;

        for (int i = 0; i < _observation.size(); ++i)
        {
            pythonList.append(_observation[i]);
        }

        object o_funcLearn = m_module->attr("learnFunc");
        object result = o_funcLearn(pythonList, _reward);

        int resultAction = extract<int>(result);
        return resultAction;
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }

    return 0;
}

void FuryScript::saveModel()
{
    try
    {
        object o_funcSave = m_module->attr("saveModel");
        o_funcSave();
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }
}
