#include <boost/python.hpp>

#include "FuryLearningScript.h"

#include <QDebug>
#include <QDir>

#include <string>



using namespace boost::python;

FuryLearningScript::FuryLearningScript()
{
    qputenv("PYTHONHOME", "../libs/python");
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

int FuryLearningScript::predict(const QVector<float> _observation)
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

int FuryLearningScript::learn(const QVector<float> _observation, float _reward, bool _stop)
{
    try
    {
        list pythonList;

        for (int i = 0; i < _observation.size(); ++i)
        {
            pythonList.append(_observation[i]);
        }

        object inputObservation;

        if (!_stop)
        {
            inputObservation = pythonList;
        }
        else
        {
            inputObservation = object();
        }

        object o_funcLearn = m_module->attr("learnFunc");
        object result = o_funcLearn(inputObservation, _reward);

        int resultAction = extract<int>(result);
        return resultAction;
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }

    return 0;
}

void FuryLearningScript::saveModel()
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
