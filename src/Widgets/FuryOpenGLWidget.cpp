#include "FuryOpenGLWidget.h"

#include "Shader.h"
#include "Logger/FuryLogger.h"
#include "Widgets/FuryRenderer.h"

#include <QTime>
#include <QOpenGLFramebufferObject>


FuryOpenGLWidget::FuryOpenGLWidget(QWidget *_parent) :
    QOpenGLWidget(_parent),
    m_framebuffer(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    initConnections();
}

FuryOpenGLWidget::~FuryOpenGLWidget()
{
    if (m_framebuffer != nullptr)
    {
        delete m_framebuffer;
        m_framebuffer = nullptr;
    }

    delete FuryRenderer::instance();
}

void FuryOpenGLWidget::initializeGL()
{
    Debug(ru("Создание главного окна отрисовки"));
    FuryRenderer::instance()->initializeGL();
}

void FuryOpenGLWidget::resizeGL(int _width, int _height)
{
    glViewport(0, 0, _width, _height);

    if (m_framebuffer != nullptr)
    {
        delete m_framebuffer;
    }

    m_framebuffer = FuryRenderer::instance()->createFramebuffer(_width, _height);
}

void FuryOpenGLWidget::paintGL()
{
    FuryRenderer::instance()->renderMainScene(m_framebuffer);

    makeCurrent();

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, width(), height());
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static Shader* m_shader = nullptr;
    static GLuint m_vaoDebugTexturedRect = 0;

    if (m_shader == nullptr)
    {
        m_shader = new Shader("bufferShader.vs", "bufferShader.fs");

        GLfloat buffer_vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 1.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f,
        };

        GLuint vboDebugTexturedRect;
        glGenVertexArrays(1, &m_vaoDebugTexturedRect);
        glGenBuffers(1, &vboDebugTexturedRect);
        glBindBuffer(GL_ARRAY_BUFFER, vboDebugTexturedRect);
        glBindVertexArray(m_vaoDebugTexturedRect);

        glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_vertices), buffer_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    m_shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer->texture());
    glBindVertexArray(m_vaoDebugTexturedRect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);

    doneCurrent();
}

void FuryOpenGLWidget::mouseMoveEvent(QMouseEvent *_event)
{
    FuryRenderer::instance()->mouseMoveEvent(_event);
}

void FuryOpenGLWidget::mousePressEvent(QMouseEvent *_event)
{
    FuryRenderer::instance()->mousePressEvent(_event);
}

void FuryOpenGLWidget::keyPressEvent(QKeyEvent *_event)
{
    FuryRenderer::instance()->keyPressEvent(_event);
}

void FuryOpenGLWidget::keyReleaseEvent(QKeyEvent *_event)
{
    FuryRenderer::instance()->keyReleaseEvent(_event);
}

void FuryOpenGLWidget::initConnections()
{
    connect(this, &QOpenGLWidget::frameSwapped,
            this, qOverload<>(&FuryOpenGLWidget::update), Qt::QueuedConnection);
}
