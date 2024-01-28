#ifndef CAMERA_H
#define CAMERA_H


#include "FuryObject.h"

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sstream>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    Up,
    Down
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 8.0f; // 2.0f - default
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 100.0f;


//! Класс камеры игрового пространства
class Camera
{
public:
    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция камеры
     * \param[in] _up - Направление верха камеры
     * \param[in] _yaw - Расканье камеры
     * \param[in] _pitch - Тангаж камеры
     */
    Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat _yaw = YAW,
           GLfloat _pitch = PITCH);

    /*!
     * \brief Получить ViewMatrix для OpenGL
     * \return Возвращает ViewMatrix для OpenGL
     */
    const glm::mat4& getViewMatrix();

    /*!
     * \brief Получить PerspectiveMatrix для OpenGL
     * \param[in] _width - Ширина экрана
     * \param[in] _height - Высота экрана
     * \param[in] _near - Ближная граница усечённой пирамиды
     * \param[in] _far - Дальняя граница усечённой пирамиды
     * \return
     */
    const glm::mat4& getPerspectiveMatrix(int _width, int _height,
                                   float _near, float _far);

    /*!
     * \brief Проверка, находится ли объект как сфера в зоне видимости камеры
     * \param _object - Объект. Считаем, что он вписан в сферу
     * \return Возвращает признак того, находится ли объект в зоне видимости
     */
    bool sphereInFrustum(FuryObject* _object);

    /*!
     * \brief Обработка нажатий клавиатуры
     * \param[in] _direction - Направление движения
     * \param[in] _deltaTime - Время
     */
    void processKeyboard(Camera_Movement _direction, GLfloat _deltaTime);

    /*!
     * \brief Обработка перемещения мыши
     * \param[in] _xOffset - Изменение координаты X
     * \param[in] _yOffset - Изменение координаты Y
     * \param[in] _constrainPitch - Ограничить ли тангаж
     */
    void processMouseMovement(GLfloat _xOffset, GLfloat _yOffset, GLboolean _constrainPitch = true);

    /*!
     * \brief Обработка скролла мыши
     * \param[in] _yOffset - Величина скролла
     */
    void processMouseScroll(GLfloat _yOffset);

    /*!
     * \brief Установка угла обзора камеры
     * \param[in] _value - Значение угла обзора
     */
    void setCameraZoom(int _value);

    /*!
     * \brief Получить позицию камеры
     * \return Возвращает позицию камеры
     */
    inline const glm::vec3& position() const
    { return m_position; }

    /*!
     * \brief Установить позицию камеры
     * \param[in] _position - Позиция камеры
     */
    inline void setPosition(const glm::vec3& _position)
    { m_position = _position; }

    /*!
     * \brief Получить направление "Вперёд"
     * \return Возвращает направление "Вперёд"
     */
    inline const glm::vec3& front() const
    { return m_front; }

    /*!
     * \brief Установить направление "Вперёд"
     * \param[in] _front - Направление "Вперёд"
     */
    inline void setFront(const glm::vec3& _front)
    { m_front = glm::normalize(_front); }

    /*!
     * \brief Получить угол обзора камеры
     * \return Возвращает угол обзора камеры
     */
    inline GLfloat zoom() const
    { return m_zoom; }

private:
    //! Обновление внутренних векторов камеры
    void updateCameraVectors();

private:
    //! Позиция камеры
    glm::vec3 m_position;
    //! Направление "Вперёд"
    glm::vec3 m_front;
    //! Направление "Вверх"
    glm::vec3 m_up;
    //! Направление "Вправо"
    glm::vec3 m_right;
    //! Мировое направление "Вверх"
    glm::vec3 m_worldUp;

    //! Рысканье камеры
    GLfloat m_yaw;
    //! Тангаж камеры
    GLfloat m_pitch;
    //! Угол обзора камеры
    GLfloat m_zoom;

    //! Скорость передвижения камеры
    GLfloat m_movementSpeed;
    //! Чувствительность мыши
    GLfloat m_mouseSensitivity;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_perspectiveMatrix;
};

#endif // CAMERA_H
