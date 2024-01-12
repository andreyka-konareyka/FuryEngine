#ifndef FURYMAINWINDOW_H
#define FURYMAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class FuryMainWindow;
}

//! Главное окно приложения
class FuryMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Конструктор
     * \param[in] _parent - Родительский виджет
     */
    FuryMainWindow(QWidget* _parent = nullptr);

    //! Деструктор
    ~FuryMainWindow();

private slots:
    //! Слот Об Qt
    void onAboutQtSlot();

    /*!
     * \brief Слот перемещения слайдера угла обзора
     * \param[in] _value - Значение
     */
    void onZoomSliderMoveSlot(int _value);

    //! Слот изменения положения слайдеров камеры
    void onCarCameraMoveSliderSlot();

    /*!
     * \brief Слот установки заголовка окна
     * \param[in] _title - Заголовок окна
     */
    void onSetWindowTitleSlot(const QString& _title);

    /*!
     * \brief Слот установки значения загрузки системы
     * \param[in] _value - Значение загрузки системы
     */
    void onSetComputerLoadSlot(int _value);

    //! Слот изменения длины пружины автомобиля
    void onCarSpringLenghtSliderSlot();

    //! Слот изменения жёсткости пружины автомобиля
    void onCarSpringKSliderSlot();

    void onShadowNearSliderSlot();
    void onShadowPlaneSliderSlot();
    void onShadowViewSizeSliderSlot();
    void onShadowCamDistanceSliderSlot();

private:
    //! Подготовка интерфейса
    void prepareUi();
    //! Инициализация соединений сигналов и слотов
    void initConnections();

private:
    //! Интерфейс
    Ui::FuryMainWindow *m_ui;
};

#endif // FURYMAINWINDOW_H
