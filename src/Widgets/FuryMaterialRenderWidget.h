#ifndef FURYMATERIALRENDERWIDGET_H
#define FURYMATERIALRENDERWIDGET_H


// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

#include <QOpenGLWidget>

class Shader;


//! Класс отображения материала
class FuryMaterialRenderWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский виджет
     */
    explicit FuryMaterialRenderWidget(QWidget* _parent = nullptr);
    //! Деструктор
    ~FuryMaterialRenderWidget();

    //! Инициализация OpenGL
    void initializeGL() override;

    /*!
     * \brief Изменение размеров отрисовки
     * \param[in] _width - Ширина
     * \param[in] _height - Высота
     */
    void resizeGL(int _width, int _height) override;

    //! Отрисовка
    void paintGL() override;

    /*!
     * \brief Установка названия материала
     * \param[in] _materialName - Название материала
     */
    void setMaterialName(const QString& _materialName);

private:
    //! Отрисовка фрейма
    void displayFrame();

private:
    //! Ширина
    float m_width;
    //! Высота
    float m_height;

    //! Название материала
    QString m_materialName;

    //! Шейдер отрисовки буфера
    Shader* m_shader;
    //! VAO для отрисовки буфера
    GLuint m_vaoDebugTexturedRect;
};

#endif // FURYMATERIALRENDERWIDGET_H
