#include "FuryMaterialRenderWidget.h"

#include "Shader.h"
#include "Widgets/FuryRenderer.h"


FuryMaterialRenderWidget::FuryMaterialRenderWidget(QWidget* _parent) :
    QOpenGLWidget(_parent),
    m_width(100),
    m_height(100),
    m_shader(nullptr),
    m_vaoDebugTexturedRect(0)
{

}

FuryMaterialRenderWidget::~FuryMaterialRenderWidget()
{
    delete m_shader;
    m_shader = nullptr;
}

void FuryMaterialRenderWidget::initializeGL()
{
    glViewport(0, 0, m_width, m_height);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); // enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);


    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void FuryMaterialRenderWidget::resizeGL(int _width, int _height)
{
    glViewport(0, 0, _width, _height);
    m_width = _width;
    m_height = _height;
}

void FuryMaterialRenderWidget::paintGL()
{
    displayFrame();
}

void FuryMaterialRenderWidget::setMaterialName(const QString &_materialName)
{
    m_materialName = _materialName;
    update();
}

void FuryMaterialRenderWidget::displayFrame()
{
    GLuint t = FuryRenderer::instance()->renderTestScene(m_materialName, m_width, m_height);

    makeCurrent();

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, m_width, m_height);
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    glBindTexture(GL_TEXTURE_2D, t);
    glBindVertexArray(m_vaoDebugTexturedRect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);

    doneCurrent();
}
