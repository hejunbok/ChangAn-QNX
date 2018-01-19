#ifndef HMINOTIFICATIONMANAGER_P_H
#define HMINOTIFICATIONMANAGER_P_H

#include "HmiNotificationManager.h"

#include <qpps/object.h>

#include "qpps/dirwatcher.h"

namespace QnxCar {

class HmiNotificationManager::Private : public QObject
{
    Q_OBJECT

public:
    Private(HmiNotificationManager *qq);
    HmiNotificationManager *q;

    /**
     * @brief m_ppsMessageObject
     * member variable for the hmi-notifications/Messaging object
     */
    QPps::Object *m_ppsMessageObject;
    /**
     * @brief m_ppsStatusObject
     * member variable for the hmi-notifications/Status object
     */
    QPps::Object *m_ppsStatusObject;
    /**
     * @brief m_ppsObjectDirWatcher
     * member variable for the HNM pps directory watcher
     */
    QPps::DirWatcher *m_ppsObjectDirWatcher;


private Q_SLOTS:
    /**
     * @brief ppsAttributeChanged
     * function to handle any change to the hmi-notifications/Status object
     * @param name
     * The key value comming from the pps object
     * @param attribute
     * The string containing an array of JSON objects which are HNM status objects
     */
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);
    /**
     * @brief ppsMessageObjectAttributeChanged
     * function to handle any change to the hmi-notifications/Messaging object
     * @param name
     * the key value coming from the pps object
     * @param attribute
     * The string containing an array of JSON objects which are HNM message type objects
     */
    void ppsMessageObjectAttributeChanged(const QString &name, const QPps::Variant &attribute);

    /**
     * @brief connectPPSObjects
     * function to notify when the pps objects are created on the system allowing us to bind to them
     * @param object
     * Name of the created pps object
     */
    void connectPPSObjects(const QString &object);

};

}

#endif
