#include "FuryMainWindow.h"
#include "ui_FuryMainWindow.h"

#include "FuryLogger.h"

#include <QMessageBox>

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
    setWindowTitle(_title);
}

void FuryMainWindow::onSetComputerLoadSlot(int _value)
{
    m_ui->progressBar->setValue(_value);
}

void FuryMainWindow::onCarSpringLenghtSliderSlot()
{
    float value = m_ui->springLenghtSlider->value() / 100.0;
    m_ui->springLenghtLabel->setText(QString("Длина пружины: %1").arg(value));
    m_ui->openGLWidget->setCarSpringLenght(value);
}

void FuryMainWindow::onCarSpringKSliderSlot()
{
    float value = m_ui->springKSlider->value() / 100.0 * 800 + 100;
    m_ui->springKLabel->setText(QString("Жёсткость пружины: %1").arg(value));
    m_ui->openGLWidget->setCarSpringK(value);
}

void FuryMainWindow::prepareUi()
{
    m_ui->setupUi(this);
    initConnections();

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
}

