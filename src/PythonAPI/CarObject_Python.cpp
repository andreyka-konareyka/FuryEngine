#include <boost/python.hpp>

#include "CarObject.h"


using namespace boost::python;


list getRays(CarObject& _object)
{
    list result;

    foreach (float ray, _object.getRays())
    {
        result.append(ray);
    }

    return result;
}

list getObservation(CarObject& _object)
{
    list result;

    foreach (float value, _object.getObservation())
    {
        result.append(value);
    }

    return result;
}


BOOST_PYTHON_MODULE(CarObject)
{
    class_<CarObject, bases<FuryObject>, boost::noncopyable>("CarObject", no_init)
        .def("setBotAction", &CarObject::setBotAction)
        .def("getRays", getRays)
        .def("getObservation", getObservation)
        .def("getSpeed", &CarObject::getSpeed)
        .def("getAngularSpeed", &CarObject::getAngularSpeed)
        .def("getReward", &CarObject::getReward)
        .def("checkTimeCounter", &CarObject::checkTimeCounter)
        .def("checkBackTriggerCounter", &CarObject::checkBackTriggerCounter)
        .def("checkHasContact", &CarObject::checkHasContact)
    ;
};
