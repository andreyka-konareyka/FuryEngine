#include "FuryEventListener.h"

#include "FuryObject.h"

#include <QDebug>

FuryEventListener::FuryEventListener()
{

}

void FuryEventListener::onContact(const CallbackData &_callbackData)
{
    // For each contact pair
    for (uint p = 0; p < _callbackData.getNbContactPairs(); p++) {

        // Get the contact pair
        CollisionCallback::ContactPair contactPair = _callbackData.getContactPair(p);

        FuryObject* tempObj = static_cast<FuryObject*>(contactPair.getBody1()->getUserData());

        if (tempObj != nullptr)
        {
            qDebug() << "body1" << tempObj->name();
        }
        else
        {
            qDebug() << "body1" << "nullptr";
        }

        tempObj = static_cast<FuryObject*>(contactPair.getBody2()->getUserData());

        if (tempObj != nullptr)
        {
            qDebug() << "body2" << tempObj->name();
        }
        else
        {
            qDebug() << "body2" << "nullptr";
        }

        // For each contact point of the contact pair
        for (uint c = 0; c < contactPair.getNbContactPoints(); c++) {

            // Get the contact point
            CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);

            // Get the contact point on the first collider and convert it in world-space
            reactphysics3d::Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform()
                    * contactPoint.getLocalPointOnCollider1();

            qDebug() << worldPoint.x
                     << worldPoint.y
                     << worldPoint.z;
        }
    }
}
