#ifndef FURYOBJECT_H
#define FURYOBJECT_H


//#define GLEW_STATIC
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

    /*!
     * \brief Установка трансформации для модели
     * \param[in] _modelTransform - Матрица трансформации
     */
    inline void setModelTransform(const glm::mat4& _modelTransform)
    { m_modelTransform = _modelTransform; }

    /*!
     * \brief Получить трансформацию модели
     * \return Возвращает матрицу трансформации
     */
    inline const glm::mat4& modelTransform() const
    { return m_modelTransform; }

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

    /*!
     * \brief Получить масштаб текстуры
     * \return Возвращает масштаб текстуры
     */
    inline const glm::vec2& textureScales() const
    { return m_textureScales; }

    /*!
     * \brief Установить масштаб текстуры
     * \param[in] _textureScales - Масштаб текстуры
     */
    inline void setTextureScales(const glm::vec2& _textureScales)
    { m_textureScales = _textureScales; }


protected:
    //! Позиция
    glm::vec3 m_position;

private:
    //! Шейдер
    Shader* m_shader = nullptr;
    //! Физическое тело
    reactphysics3d::RigidBody* m_physicsBody = nullptr;

    //! Вращение объекта
    glm::vec3 m_rotate;
    //! Масштабы по осям
    glm::vec3 m_scales;

    //! Название объекта
    QString m_name;

    //! Мир, к которому принадлежит объект
    FuryWorld* m_world;
    //! Название модели для отображения
    QString m_modelName;
    //! Трансформация для модели
    glm::mat4 m_modelTransform;
    //! Название материала объекта
    QString m_materialName;

    //! Масштаб текстур
    glm::vec2 m_textureScales;
};

#endif // FURYOBJECT_H
