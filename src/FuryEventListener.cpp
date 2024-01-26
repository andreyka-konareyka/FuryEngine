#include "FuryEventListener.h"

#include "FuryObject.h"
#include "CarObject.h"

#include <QDebug>

FuryEventListener::FuryEventListener() :
    m_carObject(nullptr)
{

}

void FuryEventListener::setCarObject(CarObject *_carObject)
{
    m_carObject = _carObject;
}

void FuryEventListener::onContact(const CallbackData &_callbackData)
{
    // For each contact pair
    for (uint p = 0; p < _callbackData.getNbContactPairs(); p++) {

        // Get the contact pair
        CollisionCallback::ContactPair contactPair = _callbackData.getContactPair(p);

        FuryObject* tempObj = static_cast<FuryObject*>(contactPair.getBody1()->getUserData());

//        if (tempObj != nullptr)
//        {
//            qDebug() << "body1" << tempObj->name();
//        }
//        else
//        {
//            qDebug() << "body1" << "nullptr";
//        }

        tempObj = static_cast<FuryObject*>(contactPair.getBody2()->getUserData());

//        if (tempObj != nullptr)
//        {
//            qDebug() << "body2" << tempObj->name();
//        }
//        else
//        {
//            qDebug() << "body2" << "nullptr";
//        }

        // For each contact point of the contact pair
        for (uint c = 0; c < contactPair.getNbContactPoints(); c++) {

            // Get the contact point
            CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);

            // Get the contact point on the first collider and convert it in world-space
//            reactphysics3d::Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform()
//                    * contactPoint.getLocalPointOnCollider1();

//            qDebug() << worldPoint.x
//                     << worldPoint.y
//                     << worldPoint.z;

            if (m_carObject != nullptr)
            {
                m_carObject->onContact();
            }
        }
    }
}

void FuryEventListener::onTrigger(const reactphysics3d::OverlapCallback::CallbackData &_callbackData)
{
    for (uint i = 0; i < _callbackData.getNbOverlappingPairs(); ++i)
    {
        reactphysics3d::OverlapCallback::OverlapPair overlapPair
                =_callbackData.getOverlappingPair(i);
        reactphysics3d::OverlapCallback::OverlapPair::EventType eventType
                = overlapPair.getEventType();

        if (eventType == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay)
        {
            // Нам интересно только вход и выход из триггера
            continue;
        }


        FuryObject* tempObj1 = static_cast<FuryObject*>(overlapPair.getBody1()->getUserData());
        FuryObject* tempObj2 = static_cast<FuryObject*>(overlapPair.getBody2()->getUserData());

        if (tempObj2->name() == "carBody")
        {
            FuryObject* tempObj3 = tempObj1;
            tempObj1 = tempObj2;
            tempObj2 = tempObj3;
        }
        else if (tempObj1->name() != "carBody")
        {
            // Столкновение колёс, их не считаем
            continue;
        }




        if (eventType == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart)
        {
//            qDebug() << "EventType::OverlapStart";
            tempObj2->setTextureName("greenCheckBox");

            if (m_carObject != nullptr)
            {
                int number = tempObj2->name().split(' ').last().toInt();
                m_carObject->onTrigger(number);
            }
        }
        else if (eventType == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit)
        {
//            qDebug() << "EventType::OverlapExit";
            tempObj2->setTextureName("redCheckBox");
        }
        else
        {
//            qDebug() << "EventType::OverlapStay";
        }

        if (tempObj1 != nullptr)
        {
//            qDebug() << "body1" << tempObj1->name();
        }
        else
        {
//            qDebug() << "body1" << "nullptr";
        }

        if (tempObj2 != nullptr)
        {
//            qDebug() << "body2" << tempObj2->name();
        }
        else
        {
//            qDebug() << "body2" << "nullptr";
        }
    }
}
