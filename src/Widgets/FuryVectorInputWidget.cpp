#include "FuryVectorInputWidget.h"
#include "ui_FuryVectorInputWidget.h"

#include "FuryDoubleValidator.h"

FuryVectorInputWidget::FuryVectorInputWidget(const glm::vec3 &_vector, QWidget *_parent) :
    QWidget(_parent),
    m_ui(new Ui::FuryVectorInputWidget),
    m_currentVector(_vector),
    m_isVec2(false)
{
    prepareUi();
}

FuryVectorInputWidget::FuryVectorInputWidget(const glm::vec2 &_vector, QWidget *_parent) :
    QWidget(_parent),
    m_ui(new Ui::FuryVectorInputWidget),
    m_currentVector(glm::vec3(_vector.x, _vector.y, 0)),
    m_isVec2(true)
{
    prepareUi();
}

FuryVectorInputWidget::~FuryVectorInputWidget()
{
    delete m_ui;
}

void FuryVectorInputWidget::inputChangedSlot()
{
    m_currentVector.x = m_ui->leX->text().toFloat();
    m_currentVector.y = m_ui->leY->text().toFloat();
    m_currentVector.z = m_ui->leZ->text().toFloat();

    if (m_isVec2)
    {
        emit vector2dChangedSignal(glm::vec2(m_currentVector.x, m_currentVector.y));
    }
    else
    {
        emit vectorChangedSignal(m_currentVector);
    }
}

void FuryVectorInputWidget::prepareUi()
{
    m_ui->setupUi(this);
    FuryDoubleValidator* validator = new FuryDoubleValidator(this);
    m_ui->leX->setValidator(validator);
    m_ui->leY->setValidator(validator);
    m_ui->leZ->setValidator(validator);
    m_ui->leX->setText(QString::number(m_currentVector.x));
    m_ui->leY->setText(QString::number(m_currentVector.y));
    m_ui->leZ->setText(QString::number(m_currentVector.z));

    if (m_isVec2)
    {
        m_ui->label_3->hide();
        m_ui->leZ->hide();
    }

    initConnections();
}

void FuryVectorInputWidget::initConnections()
{
    connect(m_ui->leX, &QLineEdit::editingFinished,
            this, &FuryVectorInputWidget::inputChangedSlot);
    connect(m_ui->leY, &QLineEdit::editingFinished,
            this, &FuryVectorInputWidget::inputChangedSlot);
    connect(m_ui->leZ, &QLineEdit::editingFinished,
            this, &FuryVectorInputWidget::inputChangedSlot);
}
