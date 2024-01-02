#ifndef WORLD_H
#define WORLD_H


#include <vector>
#include <string>
#include <map>

#include <QVector>

#include "Camera.h"
#include "FuryObject.h"

//! Класс игрового мира
class FuryWorld {
public:
    //! Конструктор
    FuryWorld();
    //! Деструктор
    ~FuryWorld();

    /*!
     * \brief Обновление состояния
     * \param[in] _dt - Время от прошлого обновления
     */
    void tick(double _dt);

    /*!
     * \brief Добавление непрозрачный объекта
     * \param[in] _object - Непрозрачный объект
     */
    void addObject(FuryObject* _object);

    /*!
     * \brief Добавить полупрозрачный объект
     * \param[in] _object - Полупрозрачный объект
     */
    void addTransparentObject(FuryObject* _object);

    /*!
     * \brief Установка камеры
     * \param[in] _camera - Камера
     */
    void setCamera(Camera* _camera);

    //! Удаление текущей камеры
    void deleteCurrentCamera();

    /*!
     * \brief Получение всех непрозрачных объектов
     * \return Возвращает все непрозрачные объекты
     */
    const QVector<FuryObject*>& getObjects();

    /*!
     * \brief Получить все полупрозрачные объекты
     * \return Возвращает все полупрозрачные объекты
     */
    const QVector<FuryObject*>& getTransparentObjects();

    /*!
     * \brief Получить все объекты
     * \return Возвращает все объекты
     */
    const QVector<FuryObject*>& getAllObjects();


    /*!
     * \brief Получение текущей камеры
     * \return Возврашает текущую камеру
     */
    inline Camera* camera() const
    { return m_currentCamera; }

private:
    //! Текущая камера
    Camera* m_currentCamera;
    //! Список непрозрачный объектов
    QVector<FuryObject*> m_objects;
    //! Список прозрачных объектов
    QVector<FuryObject*> m_transparentObjects;

    //! Список всех объектов
    QVector<FuryObject*> m_allObjects;
};

#endif // WORLD_H
