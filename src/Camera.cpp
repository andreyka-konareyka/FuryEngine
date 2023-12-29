#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) :
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_movementSpeed(SPEED),
    m_mouseSensitivity(SENSITIVTY),
    m_zoom(ZOOM)
{
    this->m_position = position;
    this->m_worldUp = up;
    this->m_yaw = yaw;
    this->m_pitch = pitch;
    this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
               GLfloat upX, GLfloat upY, GLfloat upZ,
               GLfloat yaw, GLfloat pitch) :
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_movementSpeed(SPEED),
    m_mouseSensitivity(SENSITIVTY),
    m_zoom(ZOOM)
{
    this->m_position = glm::vec3(posX, posY, posZ);
    this->m_worldUp = glm::vec3(upX, upY, upZ);
    this->m_yaw = yaw;
    this->m_pitch = pitch;
    this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->m_position, this->m_position + this->m_front, this->m_up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = this->m_movementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD) {
        glm::vec3 dirFront(this->m_front.x, 0, this->m_front.z);
        dirFront = glm::normalize(dirFront);
        this->m_position += dirFront * velocity;
    }
    if (direction == Camera_Movement::BACKWARD) {
        glm::vec3 dirFront(this->m_front.x, 0, this->m_front.z);
        dirFront = glm::normalize(dirFront);
        this->m_position -= dirFront * velocity;
    }
    if (direction == Camera_Movement::LEFT)
        this->m_position -= this->m_right * velocity;
    if (direction == Camera_Movement::RIGHT)
        this->m_position += this->m_right * velocity;

    if (direction == Camera_Movement::Up) {
        glm::vec3 dirUp(0, 1, 0);
        this->m_position += dirUp * velocity;
    }
    if (direction == Camera_Movement::Down) {
        glm::vec3 dirUp(0, 1, 0);
        this->m_position -= dirUp * velocity;
    }
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= this->m_mouseSensitivity;
    yoffset *= this->m_mouseSensitivity;

    this->m_yaw += xoffset;
    this->m_pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (this->m_pitch > 89.0f)
            this->m_pitch = 89.0f;
        if (this->m_pitch < -89.0f)
            this->m_pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
    float min = 35.;
    float max = 130.;
    float scale = 2.0f;
    if (this->m_zoom >= min && this->m_zoom <= max)
        this->m_zoom -= (GLfloat)(yoffset * scale);
    if (this->m_zoom <= min)
        this->m_zoom = min;
    if (this->m_zoom >= max)
        this->m_zoom = max;
}

void Camera::setCameraZoom(int _value)
{
    m_zoom = _value;
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    front.y = sin(glm::radians(this->m_pitch));
    front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    this->m_front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    this->m_right = glm::normalize(glm::cross(this->m_front, this->m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->m_up = glm::normalize(glm::cross(this->m_right, this->m_front));
}
