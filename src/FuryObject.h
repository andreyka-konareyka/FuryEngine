#ifndef FURYOBJECT_H
#define FURYOBJECT_H


#include <string>
#include <vector>
#include <map>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <reactphysics3d/reactphysics3d.h>

#include "Camera.h"
#include "Shader.h"

#include <QString>

//! Класс объекта 3D-сцены
class FuryObject {
public:
    //! Конструктор
    FuryObject();

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     */
    FuryObject(const glm::vec3& _position);

    //! Деструктор
    virtual ~FuryObject();

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    virtual void tick(double /*_dt*/) {};

    /*!
     * \brief Отрисовка объекта
     * \param[in] _camera - Камера
     * \param[in] _windowWidth - Ширина окна
     * \param[in] _windowHeight - Высота окна
     * \param[in] _dirlight - Направление света
     * \param[in] _shadowMap - Карта теней
     * \param[in] _depthMap - Карта глубины
     */
    virtual void draw(Camera* /*_camera*/, int /*_windowWidth*/, int /*_windowHeight*/,
                      glm::vec3& /*_dirlight*/, glm::mat4& /*_shadowMap*/, GLuint /*_depthMap*/) {};

    /*!
     * \brief Событие нажатия на кнопку
     * \param[in] _keyCode - Код клавиши
     */
    virtual void keyPressEvent(int /*_keyCode*/) {};

    /*!
     * \brief Событие отпускания кнопки
     * \param[in] _keyCode - Код клавиши
     */
    virtual void keyReleaseEvent(int /*_keyCode*/) {};

    /*!
     * \brief Получение позиции
     * \return Возвращает позицию
     */
    inline const glm::vec3& getPosition() const
    { return m_position; }

    /*!
     * \brief Установка позиции
     * \param[in] _position - Позиция
     */
    inline void setPosition(const glm::vec3& _position)
    { m_position = _position; }

    /*!
     * \brief Получение VAO
     * \return Возвращает VAO
     */
    inline GLuint VAO() const
    { return m_VAO; }

    /*!
     * \brief Получение VBO
     * \return Возвращает VBO
     */
    inline GLuint VBO() const
    { return m_VBO; }

    /*!
     * \brief Получение количества вершин
     * \return Возвращает количество вершин
     */
    inline unsigned int vertexCount() const
    { return m_vertexCount; }

    /*!
     * \brief Получение типа рендера
     * \return Возвращает тип рендера
     */
    inline unsigned int renderType() const
    { return m_renderType; }

    /*!
     * \brief Установка типа рендера
     * \param[in] _renderType - Тип рендера
     */
    inline void setRenderType(int _renderType)
    { m_renderType = _renderType; }

    /*!
     * \brief Используется ли glDrawElements()
     * \return Возвращает, используется ли glDrawElements()
     */
    inline bool isDrawElements() const
    { return m_isDrawElements; }

    /*!
     * \brief Установить флаг, используется ли glDrawElements()
     * \param[in] _isDrawElements - используется ли glDrawElements()
     */
    inline void setIsDrawElements(bool _isDrawElements)
    { m_isDrawElements = _isDrawElements; }

    /*!
     * \brief Получение шейдера
     * \return Возвращает шейдер
     */
    inline Shader* shader() const
    { return m_shader; }

    /*!
     * \brief Установка шейдера
     * \param[in] _shader - Шейдер
     */
    inline void setShader(Shader* _shader)
    { m_shader = _shader; }

    /*!
     * \brief Получение признака симуляции физики
     * \return Возвращает признак симуляции физики
     */
    inline bool physicsEnabled() const
    { return m_physicsEnabled; }

    /*!
     * \brief Установка признака симуляции физики
     * \param[in] _physicsEnabled - Признак симуляции физики
     */
    inline void setPhysicsEnabled(bool _physicsEnabled)
    { m_physicsEnabled = _physicsEnabled; }

    /*!
     * \brief Получение физического тела
     * \return Возвращает физическое тело
     */
    inline reactphysics3d::RigidBody* physicsBody() const
    { return m_physicsBody; }

    /*!
     * \brief Установка физического тела
     * \param[in] _physicsBody - Физическое тело
     */
    inline void setPhysicsBody(reactphysics3d::RigidBody* _physicsBody)
    { m_physicsBody = _physicsBody; }

    /*!
     * \brief Получение название текстуры
     * \return Возвращает название текстуры
     */
    inline const QString& textureName() const
    { return m_textureName; }

    /*!
     * \brief Установка названия текстуры
     * \param[in] _textureName - Название текстуры
     */
    inline void setTextureName(const QString& _textureName)
    { m_textureName = _textureName; }

    /*!
     * \brief Получение вращения объекта
     * \return Возвращает вращение объекта
     */
    inline const glm::vec3& rotate() const
    { return m_rotate; }

    /*!
     * \brief Установка вращения объекта
     * \param[in] _rotate - Вращение объекта
     */
    inline void setRotate(const glm::vec3& _rotate)
    { m_rotate = _rotate; }

    /*!
     * \brief Получение угла вращения
     * \return Возвращает угол вращения
     */
    inline float angle() const
    { return m_angle; }

    /*!
     * \brief Установка угла вращения
     * \param[in] _angle - Угол вращения
     */
    inline void setAngle(float _angle)
    { m_angle = _angle; }

    /*!
     * \brief Получение масштабы по осям
     * \return Возвращает масштабы по осям
     */
    inline const glm::vec3& scales() const
    { return m_scales; }

    /*!
     * \brief Установка масштабов по осям
     * \param[in] _sizes - Масштабы по осям
     */
    inline void setScales(const glm::vec3& _scales)
    { m_scales = _scales; }


protected:
    //! Позиция
    glm::vec3 m_position;

    //! VAO для OpenGL
    GLuint m_VAO = 0;
    //! VBO для OpenGL
    GLuint m_VBO = 0;
    //! Количество вершин
    unsigned int m_vertexCount;

private:
    //! Шейдер
    Shader* m_shader{ nullptr };
    //! Признак симуляции физики
    bool m_physicsEnabled = false;
    //! Физическое тело
    reactphysics3d::RigidBody* m_physicsBody = nullptr;

    //! Тип рендера
    int m_renderType = GL_TRIANGLES;
    //! Признак использования glDrawElements()
    bool m_isDrawElements = false;

    //! Название текстуры
    QString m_textureName = "defaultBoxTexture";
    //! Вращение объекта
    glm::vec3 m_rotate;
    //! Угол вращения
    float m_angle;
    //! Масштабы по осям
    glm::vec3 m_scales;
};

#endif // FURYOBJECT_H
