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

void MainWindow::onMoveSliderSlot()
{
    m_ui->progressBar->setValue(m_ui->horizontalSlider->value());
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
    connect(m_ui->horizontalSlider, &QSlider::valueChanged,
            this, &MainWindow::onMoveSliderSlot);
}

