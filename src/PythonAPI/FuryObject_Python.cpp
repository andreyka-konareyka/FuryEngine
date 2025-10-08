#include <boost/python.hpp>

#include "FuryObject.h"


using namespace boost::python;


std::string shaderName(const FuryObject& _object)
{
    return _object.shaderName().toStdString();
}

void setShaderName(FuryObject& _object, const std::string& _shaderName)
{
    _object.setShaderName(QString::fromUtf8(_shaderName));
}

std::string modelName(const FuryObject& _object)
{
    return _object.modelName().toStdString();
}

void setModelName(FuryObject& _object, const std::string& _modelName)
{
    _object.setModelName(QString::fromUtf8(_modelName));
}

std::string materialName(const FuryObject& _object)
{
    return _object.materialName().toStdString();
}

void setMaterialName(FuryObject& _object, const std::string& _materialName)
{
    _object.setMaterialName(QString::fromUtf8(_materialName));
}


BOOST_PYTHON_MODULE(FuryObject)
{
    class_<FuryObject, boost::noncopyable>("FuryObject", no_init)
        .def("worldPosition", &FuryObject::worldPosition, return_value_policy<copy_const_reference>())
        .def("setWorldPosition", &FuryObject::setWorldPosition)
        .def("worldRotation", &FuryObject::worldRotation, return_value_policy<copy_const_reference>())
        .def("setWorldRotation", &FuryObject::setWorldRotation)
        .def("scales", &FuryObject::scales, return_value_policy<copy_const_reference>())
        .def("setScales", &FuryObject::setScales)
        .def("textureScales", &FuryObject::textureScales, return_value_policy<copy_const_reference>())
        .def("setTextureScales", &FuryObject::setTextureScales)
        .def("visible", &FuryObject::visible)
        .def("setVisible", &FuryObject::setVisible)
        .def("shaderName", shaderName)
        .def("setShaderName", setShaderName)
        .def("modelName", modelName)
        .def("setModelName", setModelName)
        .def("materialName", materialName)
        .def("setMaterialName", setMaterialName)
    ;
};
