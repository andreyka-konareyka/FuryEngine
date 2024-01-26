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

class FuryScript
{
public:
    FuryScript();

    int predict(const QVector<float> _observation);
    int learn(const QVector<float> _observation, float _reward, int _done);
    void saveModel();

private:
    boost::python::api::object* m_module;
};

#endif // FURYSCRIPT_H
