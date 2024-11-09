#ifndef FURYVECTORINPUTWIDGET_H
#define FURYVECTORINPUTWIDGET_H

#include <QWidget>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ui
{
    class FuryVectorInputWidget;
}

//! Виджет ввода вектора
class FuryVectorInputWidget : public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _vector - Вектор для инициализации
     * \param[in] _parent - Родительский виджет
     */
    explicit FuryVectorInputWidget(const glm::vec3& _vector, QWidget *_parent = nullptr);

    /*!
     * \brief Конструктор
     * \param[in] _vector - Вектор для инициализации
     * \param[in] _parent - Родительский виджет
     */
    explicit FuryVectorInputWidget(const glm::vec2& _vector, QWidget* _parent = nullptr);

    //! Деструктор
    ~FuryVectorInputWidget();

signals:
    /*!
     * \brief Сигнал изменения вектора
     * \param[in] _vector - Новый вектор
     */
    void vectorChangedSignal(const glm::vec3& _vector);

    /*!
     * \brief Сигнал изменения 2d вектора
     * \param[in] _vector - Новый вектор
     */
    void vector2dChangedSignal(const glm::vec2& _vector);

private slots:
    //! Слот обновления какого-то поля ввода. Запускает обновление вектора
    void inputChangedSlot();

private:
    //! Подготовка интерфейса
    void prepareUi();
    //! Инициализация соединений сигналов и слотов
    void initConnections();

private:
    //! Интерфейс
    Ui::FuryVectorInputWidget *m_ui;
    //! Текущий вектор
    glm::vec3 m_currentVector;

    //! Признак, что vec2
    bool m_isVec2;
};

#endif // FURYVECTORINPUTWIDGET_H
