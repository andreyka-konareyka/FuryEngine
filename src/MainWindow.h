#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAboutQtSlot();
    void onMoveSliderSlot(int _value);
    void onCarCameraMoveSliderSlot();

    void onSetWindowTitleSlot(const QString& _title);
    void onSetComputerLoadSlot(int _value);

    void onCarSpringLenghtSliderSlot();
    void onCarSpringKSliderSlot();

private:
    void prepareUi();
    void initConnections();

private:
    Ui::MainWindow *m_ui;
};
#endif // MAINWINDOW_H
