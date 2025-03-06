#ifndef FURYMANAGERSDATADIALOG_H
#define FURYMANAGERSDATADIALOG_H

#include <QDialog>

namespace Ui {
class FuryManagersDataDialog;
}

class FuryManagersDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FuryManagersDataDialog(QWidget* _parent = nullptr);
    ~FuryManagersDataDialog();

private slots:
    //! Слот изменения текущего менеджера
    void changeManagerSlot();

private:
    void prepareUi();
    void initConnections();

private:
    Ui::FuryManagersDataDialog *m_ui;
};

#endif // FURYMANAGERSDATADIALOG_H
