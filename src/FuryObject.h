#ifndef FURYOBJECT_H
#define FURYOBJECT_H


//#define GLEW_STATIC
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QObject>
#include <QString>

class Camera;
class Shader;
class FuryWorld;
class FuryModel;

namespace reactphysics3d
{
    class RigidBody;
    struct Quaternion;
}


//! Класс объекта 3D-сцены
class FuryObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(glm::vec3 position READ initLocalPosition WRITE setInitLocalPosition)
    Q_PROPERTY(glm::vec3 rotation READ initLocalRotationDegrees WRITE setInitLocalRotationDegrees)
    Q_PROPERTY(glm::vec3 scales READ scales WRITE setScales)
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName)
    Q_PROPERTY(QString materialName READ materialName WRITE setMaterialName)
    Q_PROPERTY(bool visible READ visible WRITE setVisible)

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

    //! Метод, вызывающийся между обновлением физики и tick
    virtual void postPhysics();

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    virtual void tick(double /*_dt*/) {};

    //! Сброс
    virtual void reset();

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
     * \brief Добавление дочернего объекта
     * \param[in] _child - Дочерний объект
     * \param[in] _withoutJoint - Без создания сустава
     */
    void addChildObject(FuryObject* _child, bool _withoutJoint = false);

    /*!
     * \brief Получение позиции от физичиского мира
     * \return Возвращает позицию
     */
    inline const glm::vec3& worldPosition() const
    { return m_worldPosition; }

    /*!
     * \brief Установка мировой позиции
     * \param[in] _pos - Мировая позиция
     */
    void setWorldPosition(const glm::vec3& _pos);

    /*!
     * \brief Получение начальной позиции
     * \return Возвращает начальную позицию
     */
    inline const glm::vec3& initLocalPosition() const
    { return m_initLocalPosition; }

    /*!
     * \brief Установка начальной позиции
     * \param[in] _pos - Начальная позиция
     */
    void setInitLocalPosition(const glm::vec3& _pos);

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
     * \brief Получение вращения от физического мира
     * \return Возвращает вращение
     */
    inline const glm::vec3& worldRotation() const
    { return m_worldRotation; }

    /*!
     * \brief Установка вращения объекта в мировых координанах
     * \param[in] _rotation - Вращение объекта
     */
    void setWorldRotation(const glm::vec3& _rotation);

    /*!
     * \brief Получение начального поворота в радианах
     * \return Возвращает начальный поворот
     */
    inline const glm::vec3& initLocalRotation() const
    { return m_initLocalRotation; }

    /*!
     * \brief Установка начального поворота в радианах
     * \param[in] _rotation - Начальный поворот
     */
    void setInitLocalRotation(const glm::vec3& _rotation);

    /*!
     * \brief Получение начального поворота в градусах
     * \return Возвращает начальный поворот
     */
    glm::vec3 initLocalRotationDegrees() const;

    /*!
     * \brief Установка начального поворота в градусах
     * \param[in] _rotation - Начальный поворот
     */
    void setInitLocalRotationDegrees(const glm::vec3& _rotation);

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

    /*!
     * \brief Видимость объекта
     * \return Возвращает признак видимости объекта
     */
    inline bool visible() const
    { return m_visible; }

    /*!
     * \brief Установить видимость объекта
     * \param[in] _visible - Видимость объекта
     */
    inline void setVisible(bool _visible)
    { m_visible = _visible; }

    /*!
     * \brief Получить признак выбранности объекта в редакторе
     * \return Возвращает признак выбранности
     */
    inline bool selectedInEditor() const
    { return m_selectedInEditor; }

    /*!
     * \brief Установить признак выбранности объекта в редакторе
     * \param[in] _selected - Признак выбранности
     */
    inline void setSelectedInEditor(bool _selected)
    { m_selectedInEditor = _selected; }

signals:
    //! Сигнал изменения родителя
    void parentChangedSignal();

private:
    /*!
     * \brief Вычисление позиции в мировых координатах
     * \return Возвращает позицию
     */
    glm::vec3 calculateWorldPositionByInit() const;

    /*!
     * \brief Вычисление вращения объекта в мировых координатах
     * \return Возвращает вращение объекта
     */
    glm::vec3 calculateWorldRotate() const;

    //! Сброс трансформации до начальной
    void resetTransformationToInit();

    //! Установка трансформации по мировым координатам
    void setTransformToWorld();

private:
    //! Мировая позиция
    glm::vec3 m_worldPosition;
    //! Начальная позиция в локальном пространстве
    glm::vec3 m_initLocalPosition;

    //! Мировое вращение объекта
    glm::vec3 m_worldRotation;
    //! Начальное вращение в локальном пространстве
    glm::vec3 m_initLocalRotation;

    //! Масштабы по осям
    glm::vec3 m_scales;

    //! Шейдер
    Shader* m_shader;
    //! Физическое тело
    reactphysics3d::RigidBody* m_physicsBody;


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

    //! Видимость объекта
    bool m_visible;

    //! Объект выбран в редакторе
    bool m_selectedInEditor;
};

#endif // FURYOBJECT_H
