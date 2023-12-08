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
     * \brief Добавление объекта
     * \param[in] _object - Объект
     */
    void addObject(FuryObject* _object);

    /*!
     * \brief Установка камеры
     * \param[in] _camera - Камера
     */
    void setCamera(Camera* _camera);

    //! Удаление текущей камеры
    void deleteCurrentCamera();

    /*!
     * \brief Получение всех объектов
     * \return Возвращает все объекты
     */
    const QVector<FuryObject*>& getObjects();

    /*!
     * \brief Получение текущей камеры
     * \return Возврашает текущую камеру
     */
    inline Camera* camera() const
    { return m_currentCamera; }

private:
    //! Текущая камера
    Camera* m_currentCamera;
    //! Список объектов
    QVector<FuryObject*> m_objects;
};

#endif // WORLD_H
