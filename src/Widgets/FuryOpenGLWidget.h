#ifndef FURYOPENGLWIDGET_H
#define FURYOPENGLWIDGET_H

// GLEW
#include <GL/glew.h>

#include <QOpenGLWidget>


class QOpenGLFramebufferObject;


class FuryTextureCache;


//! Виджет OpenGL главного окна
class FuryOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский виджет
     */
    explicit FuryOpenGLWidget(QWidget* _parent = nullptr);
    //! Деструктор
    ~FuryOpenGLWidget();

protected:
    //! Инициализация OpenGL
    void initializeGL() override;

    /*!
     * \brief Изменение размера окна отрисовки
     * \param[in] _width - Ширина
     * \param[in] _height - Высота
     */
    void resizeGL(int _width, int _height) override;

    //! Отрисовка
    void paintGL() override;

protected:
    /*!
     * \brief Собитие перемещения мышки
     * \param[in] _event - Событие
     */
    void mouseMoveEvent(QMouseEvent* _event) override;

    /*!
     * \brief Событие нажатия мышкой
     * \param[in] _event - Событие
     */
    void mousePressEvent(QMouseEvent* _event) override;

    /*!
     * \brief Событие нажатия кнопки клавиатуры
     * \param[in] _event - Событие
     */
    void keyPressEvent(QKeyEvent* _event) override;

    /*!
     * \brief Событие отпускания кнопки клавиатуры
     * \param[in] _event - Событие
     */
    void keyReleaseEvent(QKeyEvent* _event) override;

private:
    //! Инициализация соединений сигналов и слотов
    void initConnections();

private:
    //! Фреймбуфер
    QOpenGLFramebufferObject* m_framebuffer;
};

#endif // FURYOPENGLWIDGET_H
