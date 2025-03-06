#include "FuryManagersDataDialog.h"
#include "ui_FuryManagersDataDialog.h"

#include "Logger/FuryLogger.h"
#include "Managers/FuryMaterialManager.h"
#include "Managers/FuryTextureManager.h"
#include "Managers/FuryModelManager.h"

FuryManagersDataDialog::FuryManagersDataDialog(QWidget* _parent) :
    QDialog(_parent),
    m_ui(new Ui::FuryManagersDataDialog)
{
    prepareUi();
}

FuryManagersDataDialog::~FuryManagersDataDialog()
{
    delete m_ui;
}

void FuryManagersDataDialog::changeManagerSlot()
{
    m_ui->lwData->clear();

    if (m_ui->cbManager->currentText() == ru("Менеджер материалов"))
    {
        m_ui->lwData->addItems(FuryMaterialManager::instance()->allMaterialNames());
    }
    else if (m_ui->cbManager->currentText() == ru("Менеджер текстур"))
    {
        FuryTextureManager* manager = FuryTextureManager::instance();
        QStringList names = manager->allTextureNames();

        foreach (const QString& name, names)
        {
            m_ui->lwData->addItem(QString("%1 (%2)").arg(name, manager->pathByName(name)));
        }
    }
    else if (m_ui->cbManager->currentText() == ru("Менеджер моделей"))
    {
        FuryModelManager* manager = FuryModelManager::instance();
        QStringList names = manager->allModelNames();

        foreach (const QString& name, names)
        {
            m_ui->lwData->addItem(QString("%1 (%2)").arg(name, manager->pathByName(name)));
        }
    }
}

void FuryManagersDataDialog::prepareUi()
{
    m_ui->setupUi(this);
    initConnections();

    m_ui->cbManager->addItem(ru("Менеджер материалов"));
    m_ui->cbManager->addItem(ru("Менеджер текстур"));
    m_ui->cbManager->addItem(ru("Менеджер моделей"));
}

void FuryManagersDataDialog::initConnections()
{
    connect(m_ui->cbManager, &QComboBox::currentIndexChanged,
            this,            &FuryManagersDataDialog::changeManagerSlot);
}
