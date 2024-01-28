#include "Camera.h"

#include "FuryObject.h"
#include "FuryModelManager.h"


Camera::Camera(glm::vec3 _position, glm::vec3 _up, GLfloat _yaw, GLfloat _pitch) :
    m_position(_position),
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_worldUp(_up),
    m_yaw(_yaw),
    m_pitch(_pitch),
    m_zoom(ZOOM),
    m_movementSpeed(SPEED),
    m_mouseSensitivity(SENSITIVTY)
{
    updateCameraVectors();
}

const glm::mat4 &Camera::getViewMatrix()
{
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
    return m_viewMatrix;
}

const glm::mat4 &Camera::getPerspectiveMatrix(int _width, int _height,
                                       float _near, float _far)
{
    m_perspectiveMatrix = glm::perspective(glm::radians(zoom()),
                                           (float)_width / (float)_height,
                                           _near, _far);
    return m_perspectiveMatrix;
}

bool Camera::sphereInFrustum(FuryObject *_object)
{
    glm::vec4 planes[6];
    glm::mat4 m = glm::transpose(m_perspectiveMatrix * m_viewMatrix);
    planes[0] = m[3] + m[0];
    planes[1] = m[3] - m[0];
    planes[2] = m[3] + m[1];
    planes[3] = m[3] - m[1];
    planes[4] = m[3] + m[2];
    planes[5] = m[3] - m[2];

    FuryModelManager* modelManager = FuryModelManager::instance();

    const glm::vec3& minp = _object->getPosition();
    const glm::vec3& scales = _object->scales();
    float maxScale = std::max(std::max(scales.x, scales.y), scales.z);
    float radius = modelManager->modelByName(_object->modelName())->modelRadius();
    radius *= maxScale;

    for (int i = 0; i < 6; i++)
    {
        float distance = glm::dot(planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f));

        if (distance < -radius)
        {
            return false;
        }
    }

    return true;
}

void Camera::processKeyboard(Camera_Movement _direction, GLfloat _deltaTime)
{
    GLfloat velocity = m_movementSpeed * _deltaTime;

    if (_direction == Camera_Movement::FORWARD)
    {
        glm::vec3 dirFront(m_front.x, 0, m_front.z);
        dirFront = glm::normalize(dirFront);
        m_position += dirFront * velocity;
    }

    if (_direction == Camera_Movement::BACKWARD)
    {
        glm::vec3 dirFront(m_front.x, 0, m_front.z);
        dirFront = glm::normalize(dirFront);
        m_position -= dirFront * velocity;
    }

    if (_direction == Camera_Movement::LEFT)
        m_position -= m_right * velocity;

    if (_direction == Camera_Movement::RIGHT)
        m_position += m_right * velocity;

    if (_direction == Camera_Movement::Up)
    {
        glm::vec3 dirUp(0, 1, 0);
        m_position += dirUp * velocity;
    }

    if (_direction == Camera_Movement::Down)
    {
        glm::vec3 dirUp(0, 1, 0);
        m_position -= dirUp * velocity;
    }
}

void Camera::processMouseMovement(GLfloat _xOffset, GLfloat _yOffset, GLboolean _constrainPitch)
{
    _xOffset *= m_mouseSensitivity;
    _yOffset *= m_mouseSensitivity;

    m_yaw += _xOffset;
    m_pitch += _yOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (_constrainPitch)
    {
        if (m_pitch > 89.0f)
        {
            m_pitch = 89.0f;
        }

        if (m_pitch < -89.0f)
        {
            m_pitch = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    this->updateCameraVectors();
}

void Camera::processMouseScroll(GLfloat _yOffset)
{
    GLfloat min = 35.;
    GLfloat max = 130.;
    float scale = 2.0f;

    if (m_zoom >= min && m_zoom <= max)
    {
        m_zoom -= (GLfloat)(_yOffset * scale);
    }

    m_zoom = glm::clamp(m_zoom, min, max);
}

void Camera::setCameraZoom(int _value)
{
    m_zoom = static_cast<GLfloat>(_value);
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
