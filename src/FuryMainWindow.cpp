#include "FuryMainWindow.h"
#include "ui_FuryMainWindow.h"

#include "FuryLogger.h"

#include <QMessageBox>
#include <QFile>

FuryMainWindow::FuryMainWindow(QWidget* _parent) :
    QMainWindow(_parent),
    m_ui(new Ui::FuryMainWindow)
{
    Debug(ru("Создание главного окна"));
    prepareUi();
}

FuryMainWindow::~FuryMainWindow()
{
    Debug(ru("Удаление главного окна"));
    delete m_ui;
}

void FuryMainWindow::onAboutQtSlot()
{
    QMessageBox::aboutQt(this);
}

void FuryMainWindow::onZoomSliderMoveSlot(int _value)
{
    m_ui->openGLWidget->setCameraZoomValue(_value);
}

void FuryMainWindow::onCarCameraMoveSliderSlot()
{
    int x = m_ui->carCamXSlider->value();
    int y = m_ui->carCamYSlider->value();

    QString camX = QString("CarCam.x: %1").arg(x);
    QString camY = QString("CarCam.y: %1").arg(y);

    m_ui->carCamXLabel->setText(camX);
    m_ui->carCamYLabel->setText(camY);

    m_ui->openGLWidget->setCarCameraPos(x, y);
}

void FuryMainWindow::onSetWindowTitleSlot(const QString &_title)
{
//    setWindowTitle(_title);
    m_ui->statusbar->showMessage(_title);
}

void FuryMainWindow::onSetComputerLoadSlot(int _value)
{
    m_ui->progressBar->setValue(_value);
}

void FuryMainWindow::onCarSpringLenghtSliderSlot()
{
    float value = m_ui->springLenghtSlider->value() / 100.0;
    m_ui->springLenghtLabel->setText(ru("Длина пружины: %1").arg(value));
    m_ui->openGLWidget->setCarSpringLenght(value);
}

void FuryMainWindow::onCarSpringKSliderSlot()
{
    float value = m_ui->springKSlider->value() / 100.0 * 800 + 100;
    m_ui->springKLabel->setText(ru("Жёсткость пружины: %1").arg(value));
    m_ui->openGLWidget->setCarSpringK(value);
}

void FuryMainWindow::onShadowNearSliderSlot()
{
    float value = m_ui->shadowNearSlider->value() / 100.0 * 5 + 0.1;
    m_ui->shadowNearLabel->setText(QString("ShadowNear: %1").arg(value));
    m_ui->openGLWidget->setShadowNear(value);
}

void FuryMainWindow::onShadowPlaneSliderSlot()
{
    float value = m_ui->shadowPlaneSlider->value() / 100.0 * 100 + 10;
    m_ui->shadowPlaneLabel->setText(QString("ShadowPlane: %1").arg(value));
    m_ui->openGLWidget->setShadowPlane(value);
}

void FuryMainWindow::onShadowViewSizeSliderSlot()
{
    float value = m_ui->shadowViewSizeSlider->value() / 100.0 * 100 + 10;
    m_ui->shadowViewSizeLabel->setText(QString("ShadowViewSize: %1").arg(value));
    m_ui->openGLWidget->setShadowViewSize(value);
}

void FuryMainWindow::onShadowCamDistanceSliderSlot()
{
    float value = m_ui->shadowCamDistanceSlider->value() / 100.0 * 50 + 5;
    m_ui->shadowCamDistanceLabel->setText(QString("ShadowCamDistance: %1").arg(value));
    m_ui->openGLWidget->setShadowCamDistance(value);
}

void FuryMainWindow::onLearnSpeedCheckBoxSlot()
{
    if (m_ui->learnSpeedCheckBox->isChecked())
    {
        m_ui->openGLWidget->setLearnSpeed(20);
    }
    else
    {
        m_ui->openGLWidget->setLearnSpeed(1);
    }
}

void FuryMainWindow::onSaveSlot()
{
    m_ui->openGLWidget->saveLearnModel();
}

void FuryMainWindow::onNeedDebugRenderSlot()
{
    m_ui->openGLWidget->setNeedDebugRender(m_ui->debugRenderCheckBox->isChecked());
}

void FuryMainWindow::prepareUi()
{
    m_ui->setupUi(this);
    initConnections();
    loadStyle();
    m_ui->statusbar->showMessage(ru("Обучаем..."));

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 0);
}

void FuryMainWindow::initConnections()
{
    connect(m_ui->aboutQtAction, &QAction::triggered,
            this, &FuryMainWindow::onAboutQtSlot);
    connect(m_ui->quitAction, &QAction::triggered,
            this, &FuryMainWindow::close);
    connect(m_ui->cameraZoomSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onZoomSliderMoveSlot);
    connect(m_ui->carCamXSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onCarCameraMoveSliderSlot);
    connect(m_ui->carCamYSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onCarCameraMoveSliderSlot);

    connect(m_ui->openGLWidget, &TestRender::setWindowTitleSignal,
            this, &FuryMainWindow::onSetWindowTitleSlot);
    connect(m_ui->openGLWidget, &TestRender::setComputerLoadSignal,
            this, &FuryMainWindow::onSetComputerLoadSlot);

    connect(m_ui->springLenghtSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onCarSpringLenghtSliderSlot);
    connect(m_ui->springKSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onCarSpringKSliderSlot);

    connect(m_ui->shadowNearSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onShadowNearSliderSlot);
    connect(m_ui->shadowPlaneSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onShadowPlaneSliderSlot);
    connect(m_ui->shadowViewSizeSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onShadowViewSizeSliderSlot);
    connect(m_ui->shadowCamDistanceSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onShadowCamDistanceSliderSlot);

    connect(m_ui->learnSpeedCheckBox, &QCheckBox::stateChanged,
            this, &FuryMainWindow::onLearnSpeedCheckBoxSlot);
    connect(m_ui->saveModelButton, &QPushButton::clicked,
            this, &FuryMainWindow::onSaveSlot);

    connect(m_ui->debugRenderCheckBox, &QCheckBox::stateChanged,
            this, &FuryMainWindow::onNeedDebugRenderSlot);
}

void FuryMainWindow::loadStyle()
{
    QFile styleFile(":qss/FuryStyle");

    if (!styleFile.open(QIODevice::ReadOnly))
    {
        Debug(ru("Не удалось загрузить стиль."));
        return;
    }

    QString styleStr = styleFile.readAll();
    setStyleSheet(styleStr);
}

