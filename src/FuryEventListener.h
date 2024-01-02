#ifndef FURYEVENTLISTENER_H
#define FURYEVENTLISTENER_H

#include <reactphysics3d/engine/EventListener.h>

//! Класс прослушивания событий физического движка
class FuryEventListener : public reactphysics3d::EventListener
{
public:
    //! Конструктор
    FuryEventListener();

    /*!
     * \brief Вызывается при возникновении некоторых контактов
     * \param[in] _callbackData - Информация обо всех контактах
     */
    void onContact(const CallbackData& _callbackData) override;
};

#endif // FURYEVENTLISTENER_H
