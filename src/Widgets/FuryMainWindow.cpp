#include "FuryMainWindow.h"
#include "ui_FuryMainWindow.h"

#include "Logger/FuryLogger.h"
#include "FuryObjectsTreeModel.h"
#include "FuryVectorInputWidget.h"
#include "FuryMaterialEditDialog.h"

#include <QFile>
#include <QLineEdit>
#include <QSpacerItem>
#include <QMessageBox>
#include <QMetaProperty>

FuryMainWindow::FuryMainWindow(QWidget* _parent) :
    QMainWindow(_parent),
    m_ui(new Ui::FuryMainWindow),
    m_treeModel(new FuryObjectsTreeModel),
    m_currentObject(nullptr)
{
    Debug(ru("Создание главного окна"));
    prepareUi();
}

FuryMainWindow::~FuryMainWindow()
{
    Debug(ru("Удаление главного окна"));
    delete m_treeModel;
    delete m_ui;
}

void FuryMainWindow::closeEvent(QCloseEvent */*_event*/)
{
    m_ui->openGLWidget->saveScoreList();
}

void FuryMainWindow::onAboutQtSlot()
{
    QMessageBox::aboutQt(this);
}

void FuryMainWindow::onZoomSliderMoveSlot(int _value)
{
    m_ui->openGLWidget->setCameraZoomValue(_value);
    m_ui->lbCameraZoom->setText(ru("Увеличение камеры: %1").arg(_value));
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

void FuryMainWindow::onMaterialEditSlot()
{
    FuryMaterialEditDialog dialog(this);
    dialog.exec();
}

void FuryMainWindow::selectObjectSlot(const QModelIndex &_current, const QModelIndex &_previous)
{
    m_currentObject = nullptr;

    if (_previous.isValid())
    {
        FuryObject* object = m_treeModel->objectByIndex(_previous);
        object->setSelectedInEditor(false);
    }

    if (_current.isValid())
    {
        FuryObject* object = m_treeModel->objectByIndex(_current);
        object->setSelectedInEditor(true);
        m_currentObject = object;

        while (m_ui->objectProperties->layout()->count() > 0)
        {
            QLayoutItem* tmp = m_ui->objectProperties->layout()->takeAt(0);
            delete tmp->widget();
            delete tmp;
        }

        m_ui->objectProperties->update();

        for (int i = 0; i < object->metaObject()->propertyCount(); ++i)
        {
            const char* propName = object->metaObject()->property(i).name();
            QVariant prop = object->property(propName);
            qDebug() << propName << prop;

            QString text(propName);
            m_ui->objectProperties->layout()->addWidget(new QLabel(text, this));

            if (QString(prop.typeName()) == "glm::vec<3,float,0>")
            {
                glm::vec3 vec = prop.value<glm::vec3>();
                qDebug() << vec.x << vec.y << vec.z;

                FuryVectorInputWidget* input = new FuryVectorInputWidget(vec, this);
                input->setProperty("propName", QByteArray(propName));
                m_ui->objectProperties->layout()->addWidget(input);

                connect(input, &FuryVectorInputWidget::vectorChangedSignal,
                        this, qOverload<const glm::vec3&>(&FuryMainWindow::propertyChangedSlot));
            }
            else if (QString(prop.typeName()) == "glm::vec<2,float,0>")
            {
                glm::vec2 vec = prop.value<glm::vec2>();
                qDebug() << vec.x << vec.y;

                FuryVectorInputWidget* input = new FuryVectorInputWidget(vec, this);
                input->setProperty("propName", QByteArray(propName));
                m_ui->objectProperties->layout()->addWidget(input);

                connect(input, &FuryVectorInputWidget::vector2dChangedSignal,
                        this, qOverload<const glm::vec2&>(&FuryMainWindow::propertyChangedSlot));
            }
            else
            {
                QLineEdit* input = new QLineEdit(prop.toString(), this);
                input->setProperty("propName", QByteArray(propName));
                m_ui->objectProperties->layout()->addWidget(input);

                connect(input, &QLineEdit::editingFinished,
                        this, qOverload<>(&FuryMainWindow::propertyChangedSlot));
            }
        }

        m_ui->objectProperties->layout()->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

        qDebug() << "";
    }
}

void FuryMainWindow::propertyChangedSlot(const glm::vec3 &_vector)
{
    if (m_currentObject == nullptr)
    {
        return;
    }

    FuryVectorInputWidget* input = dynamic_cast<FuryVectorInputWidget*>(sender());

    if (input == nullptr)
    {
        return;
    }

    QVariant prop;
    prop.setValue(_vector);
    m_currentObject->setProperty(input->property("propName").toByteArray().constData(),
                                 prop);
}

void FuryMainWindow::propertyChangedSlot(const glm::vec2 &_vector)
{
    if (m_currentObject == nullptr)
    {
        return;
    }

    FuryVectorInputWidget* input = dynamic_cast<FuryVectorInputWidget*>(sender());

    if (input == nullptr)
    {
        return;
    }

    QVariant prop;
    prop.setValue(_vector);
    m_currentObject->setProperty(input->property("propName").toByteArray().constData(),
                                 prop);
}

void FuryMainWindow::propertyChangedSlot()
{
    if (m_currentObject == nullptr)
    {
        return;
    }

    QLineEdit* input = dynamic_cast<QLineEdit*>(sender());

    if (input == nullptr)
    {
        return;
    }

    m_currentObject->setProperty(input->property("propName").toByteArray().constData(),
                                 input->text());
}

void FuryMainWindow::resetWorldSlot()
{
    m_ui->openGLWidget->getTestWorld()->resetWorld();
}

void FuryMainWindow::pauseWorldSlot()
{
    m_ui->openGLWidget->getTestWorld()->pauseWorld();
}

void FuryMainWindow::resumeWorldSlot()
{
    m_ui->openGLWidget->getTestWorld()->resumeWorld();
}

void FuryMainWindow::prepareUi()
{
    m_ui->setupUi(this);
    initConnections();
    loadStyle();

    m_ui->toolBar->setMovable(false);
    QAction* resetAction = new QAction(ru("Сброс сцены"), this);
    QAction* pauseAction = new QAction(ru("Пауза"), this);
    QAction* resumeAction = new QAction(ru("Старт"), this);
    m_ui->toolBar->addAction(resetAction);
    m_ui->toolBar->addAction(pauseAction);
    m_ui->toolBar->addAction(resumeAction);

    m_ui->statusbar->showMessage(ru("Обучаем..."));

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 0);


    m_ui->tvSceneObjects->setModel(m_treeModel);

    connect(m_ui->openGLWidget->getTestWorld(), &FuryWorld::addObjectSignal,
            m_treeModel, &FuryObjectsTreeModel::addObjectSlot);
    connect(m_ui->openGLWidget->getTestWorld(), &FuryWorld::parentChangedSignal,
            m_treeModel, &FuryObjectsTreeModel::parentChangedSlot);

    connect(m_ui->tvSceneObjects->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FuryMainWindow::selectObjectSlot);

    connect(resetAction, &QAction::triggered,
            this, &FuryMainWindow::resetWorldSlot);
    connect(pauseAction, &QAction::triggered,
            this, &FuryMainWindow::pauseWorldSlot);
    connect(resumeAction, &QAction::triggered,
            this, &FuryMainWindow::resumeWorldSlot);
}

void FuryMainWindow::initConnections()
{
    connect(m_ui->aboutQtAction, &QAction::triggered,
            this, &FuryMainWindow::onAboutQtSlot);
    connect(m_ui->quitAction, &QAction::triggered,
            this, &FuryMainWindow::close);
    connect(m_ui->cameraZoomSlider, &QSlider::valueChanged,
            this, &FuryMainWindow::onZoomSliderMoveSlot);

    connect(m_ui->openGLWidget, &FuryRenderer::setWindowTitleSignal,
            this, &FuryMainWindow::onSetWindowTitleSlot);
    connect(m_ui->openGLWidget, &FuryRenderer::setComputerLoadSignal,
            this, &FuryMainWindow::onSetComputerLoadSlot);

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

    connect(m_ui->pbMaterialEdit, &QPushButton::clicked,
            this, &FuryMainWindow::onMaterialEditSlot);
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

