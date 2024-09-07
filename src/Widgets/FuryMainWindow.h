#ifndef FURYMAINWINDOW_H
#define FURYMAINWINDOW_H

#include <QMainWindow>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ui
{
    class FuryMainWindow;
}

class FuryObject;
class FuryObjectsTreeModel;


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

    /*!
     * \brief Слот выделения объекта
     * \param[in] _current - Текущий индекс
     * \param[in] _previous - Предыдущий индекс
     */
    void selectObjectSlot(const QModelIndex &_current, const QModelIndex &_previous);

    /*!
     * \brief Слот изменения свойства
     * \param[in] _vector - Новый вектор
     */
    void propertyChangedSlot(const glm::vec3& _vector);

    //! Слот изменения свойства
    void propertyChangedSlot();

    //! Слот сброса мира
    void resetWorldSlot();
    //! Слот паузы мира
    void pauseWorldSlot();
    //! Слот возобновления мира
    void resumeWorldSlot();

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

    //! Модель дерева объектов
    FuryObjectsTreeModel* m_treeModel;
    //! Текущий объект
    FuryObject* m_currentObject;
};

#endif // FURYMAINWINDOW_H
