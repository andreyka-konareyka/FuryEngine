#include <boost/python.hpp>


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QString>

using namespace boost::python;


std::string glm_vec3_str(const glm::vec3& _vec)
{
    QString res("<glm::vec3(%1; %2; %3)>");
    res = res.arg(_vec.x).arg(_vec.y).arg(_vec.z);
    return res.toStdString();
}

std::string glm_vec2_str(const glm::vec2& _vec)
{
    QString res("<glm::vec2(%1; %2)>");
    res = res.arg(_vec.x).arg(_vec.y);
    return res.toStdString();
}



BOOST_PYTHON_MODULE(glm)
{
    class_<glm::vec3>("vec3", init<float, float, float>(args("x", "y", "z")))
        .add_property("x", make_getter(&glm::vec3::x), make_setter(&glm::vec3::x))
        .add_property("y", make_getter(&glm::vec3::y), make_setter(&glm::vec3::y))
        .add_property("z", make_getter(&glm::vec3::z), make_setter(&glm::vec3::z))
        .def("__str__", glm_vec3_str)
    ;

    class_<glm::vec2>("vec2", init<float, float>(args("x", "y")))
        .add_property("x", make_getter(&glm::vec2::x), make_setter(&glm::vec2::x))
        .add_property("y", make_getter(&glm::vec2::y), make_setter(&glm::vec2::y))
        .def("__str__", glm_vec2_str)
        ;
};
