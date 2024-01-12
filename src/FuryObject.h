#ifndef FURYOBJECT_H
#define FURYOBJECT_H


#define GLEW_STATIC
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QString>

class Camera;
class Shader;
class FuryWorld;
class FuryModel;

namespace reactphysics3d
{
    class RigidBody;
}


//! Класс объекта 3D-сцены
class FuryObject {
public:
    /*!
     * \brief Конструктор
     * \param[in] _world - Мир, к которому принадлежит объект
     */
    FuryObject(FuryWorld* _world);

    /*!
     * \brief Конструктор
     * \param[in] _position - Позиция
     */
    FuryObject(FuryWorld* _world, const glm::vec3& _position);

    //! Деструктор
    virtual ~FuryObject();

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    virtual void tick(double /*_dt*/) {};

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
    void setPosition(const glm::vec3& _position);

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

    /*!
     * \brief Получение имени объекта
     * \return Возвращает имя объекта
     */
    inline const QString& name() const
    { return m_name; }

    /*!
     * \brief Установка имени объекта
     * \param[in] _name - Имя объекта
     */
    inline void setName(const QString& _name)
    { m_name = _name; }

    /*!
     * \brief Получить матрицу модели (model) для объекта
     * \return Возвращает матрицу модели (model) для объекта
     */
    virtual glm::mat4 getOpenGLTransform() const;

    /*!
     * \brief Получить мир, к которому принадлежит объект
     * \return Возвращает мир, к которому принадлежит объект
     */
    inline FuryWorld* world() const
    { return m_world; }

    /*!
     * \brief Установка названия модели для ототбражения
     * \param[in] _modelName - Название модели для отображения
     */
    inline void setModelName(const QString& _modelName)
    { m_modelName = _modelName; }

    /*!
     * \brief Получить название модели для отображения
     * \return Возвращает название модели для отображения
     */
    inline const QString& modelName() const
    { return m_modelName; }

    //! Отрисовка объекта
    virtual void draw();
    //! Отрисовка объекта для карты теней
    virtual void drawShadowMap();

    /*!
     * \brief Установка названия материала
     * \param[in] _materialName - Название материала
     */
    inline void setMaterialName(const QString& _materialName)
    { m_materialName = _materialName; }

    /*!
     * \brief Получить название материала
     * \return Возвращает название материала
     */
    inline const QString& materialName() const
    { return m_materialName; }


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

    //! Название объекта
    QString m_name;

    //! Мир, к которому принадлежит объект
    FuryWorld* m_world;
    //! Название модели для отображения
    QString m_modelName;
    //! Название материала объекта
    QString m_materialName;
};

#endif // FURYOBJECT_H
