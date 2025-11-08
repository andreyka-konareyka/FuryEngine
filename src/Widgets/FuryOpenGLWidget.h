#ifndef FURYOPENGLWIDGET_H
#define FURYOPENGLWIDGET_H

// GLEW
#include <GL/glew.h>

#include <QOpenGLWidget>


class FuryTextureCache;


class FuryOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit FuryOpenGLWidget(QWidget* _parent = nullptr);
    ~FuryOpenGLWidget();

protected:
    void initializeGL() override;
    void resizeGL(int _width, int _height) override;
    void paintGL() override;

protected:
    void mouseMoveEvent(QMouseEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void keyPressEvent(QKeyEvent* _event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;

private:
    void initConnections();

private:
    FuryTextureCache* m_loadingTextureCache;
};

#endif // FURYOPENGLWIDGET_H
