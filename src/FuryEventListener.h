#ifndef FURYEVENTLISTENER_H
#define FURYEVENTLISTENER_H

#include <reactphysics3d/engine/EventListener.h>

class CarObject;

//! Класс прослушивания событий физического движка
class FuryEventListener : public reactphysics3d::EventListener
{
public:
    //! Конструктор
    FuryEventListener();

    /*!
     * \brief Установка объекта машины
     * \param[in] _carObject - Объект машины
     */
    void setCarObject(CarObject* _carObject);

    /*!
     * \brief Вызывается при возникновении некоторых контактов
     * \param[in] _callbackData - Информация обо всех контактах
     */
    void onContact(const CallbackData& _callbackData) override;

    /*!
     * \brief Вызывается при активации триггера
     * \param[in] _callbackData - Информация об активации триггера
     */
    void onTrigger(const reactphysics3d::OverlapCallback::CallbackData & _callbackData) override;

private:
    CarObject* m_carObject;
};

#endif // FURYEVENTLISTENER_H
