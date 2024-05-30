#ifndef FURYMATERIALEDITDIALOG_H
#define FURYMATERIALEDITDIALOG_H

#include <QDialog>

namespace Ui
{
    class FuryMaterialEditDialog;
}

class FuryMaterial;

//! Окно изменения материалов
class FuryMaterialEditDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param _parent - Родительский виджет
     */
    explicit FuryMaterialEditDialog(QWidget *_parent = nullptr);

    //! Деструктор
    ~FuryMaterialEditDialog();

private slots:
    //! Слот выбора материала
    void onMaterialChoiceSlot();
    //! Слот нажатия на цвет
    void onColorClickedSlot();
    //! Слот изменения металличности
    void onMetallicSlot();
    //! Слот изменения шероховатости
    void onRoughnessSlot();
    //! Слот изменения затенения
    void onAoSlot();

private:
    //! Подготовка интерфейса
    void prepareUi();
    //! Инициализация соединений сигналов и слотов
    void initConnections();

private:
    //! Интерфейс
    Ui::FuryMaterialEditDialog *m_ui;
    //! Текущий материал
    FuryMaterial* m_currentMaterial;
};

#endif // FURYMATERIALEDITDIALOG_H
