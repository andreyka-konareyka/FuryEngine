#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "FuryLogger.h"


#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    Debug(ru("Создание главного окна"));
    prepareUi();
}

MainWindow::~MainWindow()
{
    Debug(ru("Удаление главного окна"));
    delete m_ui;
}

void MainWindow::onAboutQtSlot()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onMoveSliderSlot(int _value)
{
    m_ui->openGLWidget->setCameraZoomValue(_value);
}

void MainWindow::onCarCameraMoveSliderSlot()
{
    int x = m_ui->carCamXSlider->value();
    int y = m_ui->carCamYSlider->value();

    m_ui->openGLWidget->setCarCameraPos(x, y);
}

void MainWindow::onSetWindowTitleSlot(const QString &_title)
{
    setWindowTitle(_title);
}

void MainWindow::onSetComputerLoadSlot(int _value)
{
    m_ui->progressBar->setValue(_value);
}

void MainWindow::onCarSpringLenghtSliderSlot()
{
    float value = m_ui->springLenghtSlider->value() / 100.0;
    m_ui->springLenghtLabel->setText(QString("Длина пружины: %1").arg(value));
    m_ui->openGLWidget->setCarSpringLenght(value);
}

void MainWindow::onCarSpringKSliderSlot()
{
    float value = m_ui->springKSlider->value() / 100.0 * 800 + 100;
    m_ui->springKLabel->setText(QString("Жёсткость пружины: %1").arg(value));
    m_ui->openGLWidget->setCarSpringK(value);
}

void MainWindow::prepareUi()
{
    m_ui->setupUi(this);
    initConnections();

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 0);
}

void MainWindow::initConnections()
{
    connect(m_ui->aboutQtAction, &QAction::triggered,
            this, &MainWindow::onAboutQtSlot);
    connect(m_ui->quitAction, &QAction::triggered,
            this, &MainWindow::close);
    connect(m_ui->cameraZoomSlider, &QSlider::valueChanged,
            this, &MainWindow::onMoveSliderSlot);
    connect(m_ui->carCamXSlider, &QSlider::valueChanged,
            this, &MainWindow::onCarCameraMoveSliderSlot);
    connect(m_ui->carCamYSlider, &QSlider::valueChanged,
            this, &MainWindow::onCarCameraMoveSliderSlot);

    connect(m_ui->openGLWidget, &TestRender::setWindowTitleSignal,
            this, &MainWindow::onSetWindowTitleSlot);
    connect(m_ui->openGLWidget, &TestRender::setComputerLoadSignal,
            this, &MainWindow::onSetComputerLoadSlot);

    connect(m_ui->springLenghtSlider, &QSlider::valueChanged,
            this, &MainWindow::onCarSpringLenghtSliderSlot);
    connect(m_ui->springKSlider, &QSlider::valueChanged,
            this, &MainWindow::onCarSpringKSliderSlot);
}

