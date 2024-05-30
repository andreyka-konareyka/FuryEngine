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

protected:
    /*!
     * \brief Закрытие окна
     * \param[in] _event - Событие
     */
    void closeEvent(QCloseEvent* _event);

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


    //! Слот изменения near у камеры для теней
    void onShadowNearSliderSlot();
    //! Слот изменения plane у камеры для теней
    void onShadowPlaneSliderSlot();
    //! Слот изменения области видимости у камеры для теней
    void onShadowViewSizeSliderSlot();
    //! Слот изменения удалённости камеры для теней
    void onShadowCamDistanceSliderSlot();

    //! Слот увеличения скорости обучения
    void onLearnSpeedCheckBoxSlot();
    //! Слот сохранения модели
    void onSaveSlot();

    //! Слот необходимости отображения отладочных объектов
    void onNeedDebugRenderSlot();

    //! Слот открытия редактора материалов
    void onMaterialEditSlot();

private:
    //! Подготовка интерфейса
    void prepareUi();
    //! Инициализация соединений сигналов и слотов
    void initConnections();
    //! Загрузить стиль приложения
    void loadStyle();

private:
    //! Интерфейс
    Ui::FuryMainWindow *m_ui;
};

#endif // FURYMAINWINDOW_H
