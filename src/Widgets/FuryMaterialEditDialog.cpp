#include "FuryMaterialEditDialog.h"
#include "ui_FuryMaterialEditDialog.h"

#include "FuryPbrMaterial.h"
#include "FuryPhongMaterial.h"
#include "Managers/FuryMaterialManager.h"

#include <QSlider>
#include <QColorDialog>

FuryMaterialEditDialog::FuryMaterialEditDialog(QWidget *_parent) :
    QDialog(_parent),
    m_ui(new Ui::FuryMaterialEditDialog),
    m_currentMaterial(NULL)
{
    prepareUi();
}

FuryMaterialEditDialog::~FuryMaterialEditDialog()
{
    delete m_ui;
}

void FuryMaterialEditDialog::onMaterialChoiceSlot()
{
    FuryMaterialManager* manager = FuryMaterialManager::instance();
    m_currentMaterial = manager->materialByName(m_ui->cbMaterialName->currentText());
    QColor color;

    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        const glm::vec3& rawColor = pbr->albedoColor();
        color = QColor(rawColor.r * 255, rawColor.g * 255, rawColor.b * 255);

        m_ui->slMetallic->setValue(pbr->metallic() * 100);
        m_ui->slRough->setValue(pbr->roughness() * 100);
        m_ui->slAo->setValue(pbr->ao() * 100);
        m_ui->leColorTexture->setText(pbr->albedoTexture());
        m_ui->leMetallicTexture->setText(pbr->metallicTexture());
        m_ui->leRoughTexture->setText(pbr->roughnessTexture());
        m_ui->leAoTexture->setText(pbr->aoTexture());
    }
    else if (FuryPhongMaterial* phong = dynamic_cast<FuryPhongMaterial*>(m_currentMaterial); phong != NULL)
    {
        const glm::vec3& rawColor = phong->diffuseColor();
        color = QColor(rawColor.r * 255, rawColor.g * 255, rawColor.b * 255);

    }

    m_ui->pbColor->setStyleSheet(QString("background: %1").arg(color.name()));
    m_ui->openGLWidget->setMaterialName(m_ui->cbMaterialName->currentText());
}

void FuryMaterialEditDialog::onColorClickedSlot()
{
    if (m_currentMaterial == NULL)
    {
        return;
    }

    QColor color;

    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        const glm::vec3& rawColor = pbr->albedoColor();
        color = QColor(rawColor.r * 255, rawColor.g * 255, rawColor.b * 255);
    }
    else if (FuryPhongMaterial* phong = dynamic_cast<FuryPhongMaterial*>(m_currentMaterial); phong != NULL)
    {
        const glm::vec3& rawColor = phong->diffuseColor();
        color = QColor(rawColor.r * 255, rawColor.g * 255, rawColor.b * 255);

    }
    QColorDialog dialog(color);
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    color = dialog.selectedColor();

    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        glm::vec3 rawColor(color.red() / 255.0f,
                           color.green() / 255.0f,
                           color.blue() / 255.0f);

        pbr->setAlbedoColor(rawColor);
    }
    else if (FuryPhongMaterial* phong = dynamic_cast<FuryPhongMaterial*>(m_currentMaterial); phong != NULL)
    {
        glm::vec3 rawColor(color.red() / 255.0f,
                           color.green() / 255.0f,
                           color.blue() / 255.0f);

        phong->setDiffuseColor(rawColor);
    }

    m_ui->pbColor->setStyleSheet(QString("background: %1").arg(color.name()));
    m_ui->openGLWidget->update();
}

void FuryMaterialEditDialog::onMetallicSlot()
{
    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        pbr->setMetallic(m_ui->slMetallic->value() / 100.0f);
    }
    m_ui->openGLWidget->update();
}

void FuryMaterialEditDialog::onRoughnessSlot()
{
    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        pbr->setRoughness(m_ui->slRough->value() / 100.0f);
    }
    m_ui->openGLWidget->update();
}

void FuryMaterialEditDialog::onAoSlot()
{
    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        pbr->setAo(m_ui->slAo->value() / 100.0f);
    }
    m_ui->openGLWidget->update();
}

void FuryMaterialEditDialog::onAnyTextureNameChangedSlot()
{
    if (FuryPbrMaterial* pbr = dynamic_cast<FuryPbrMaterial*>(m_currentMaterial); pbr != NULL)
    {
        pbr->setAlbedoTexture(m_ui->leColorTexture->text());
        pbr->setMetallicTexture(m_ui->leMetallicTexture->text());
        pbr->setRoughnessTexture(m_ui->leRoughTexture->text());
        pbr->setAoTexture(m_ui->leAoTexture->text());
    }
    m_ui->openGLWidget->update();
}

void FuryMaterialEditDialog::prepareUi()
{
    m_ui->setupUi(this);
    initConnections();

    FuryMaterialManager* manager = FuryMaterialManager::instance();
    m_ui->cbMaterialName->addItems(manager->allMaterialNames());
}

void FuryMaterialEditDialog::initConnections()
{
    connect(m_ui->cbMaterialName, &QComboBox::currentIndexChanged,
            this, &FuryMaterialEditDialog::onMaterialChoiceSlot);
    connect(m_ui->pbColor, &QPushButton::clicked,
            this, &FuryMaterialEditDialog::onColorClickedSlot);
    connect(m_ui->slMetallic, &QSlider::valueChanged,
            this, &FuryMaterialEditDialog::onMetallicSlot);
    connect(m_ui->slRough, &QSlider::valueChanged,
            this, &FuryMaterialEditDialog::onRoughnessSlot);
    connect(m_ui->slAo, &QSlider::valueChanged,
            this, &FuryMaterialEditDialog::onAoSlot);

    connect(m_ui->leColorTexture, &QLineEdit::editingFinished,
            this, &FuryMaterialEditDialog::onAnyTextureNameChangedSlot);
    connect(m_ui->leMetallicTexture, &QLineEdit::editingFinished,
            this, &FuryMaterialEditDialog::onAnyTextureNameChangedSlot);
    connect(m_ui->leRoughTexture, &QLineEdit::editingFinished,
            this, &FuryMaterialEditDialog::onAnyTextureNameChangedSlot);
    connect(m_ui->leAoTexture, &QLineEdit::editingFinished,
            this, &FuryMaterialEditDialog::onAnyTextureNameChangedSlot);
}
