#ifndef HMINOTIFICATIONMANAGER_H
#define HMINOTIFICATIONMANAGER_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

    /**
     * Gives access to the HMI Notification Manager
     *
     * Wrapper class for HNM related PPS objects:
     * - @c /pps/services/hmi-notification/Messaging
     * - @c /pps/services/hmi-notification/Status
     */
    class QTQNXCAR2_EXPORT HmiNotificationManager : public QObject
    {
        Q_OBJECT

        Q_ENUMS(State)


    public:

        /**
         * @brief The HNM State enum
         */
        enum State
        {
            Home,                           ///< Return to the Home Screen
            Navigation,                     ///< Return to Navigation
            MediaPlayer,                    ///< Return to MediaPlayer
            CarControl,                     ///< Return to CarControl
            Communication,                 ///< Return to Communications
            AppSection,                     ///< Return to AppSection
            ErrorState                      ///< We do not recognize the current state

        };
        /**
         * @brief The MessageType enum
         */
        enum MessageType
        {
            Growl,                          ///< Growl notification
            UnsupportedType                ///< We do not currently support the passed in type
        };

        /**
         * Creates a new HmiNotificationManager object.
         *
         * @param parent The parent object.
         */
        explicit HmiNotificationManager(QObject *parent = 0);

    public Q_SLOTS:

    Q_SIGNALS:

        /**
         * This signal is emitted whenever there is a state change in regards to the last selected tab.
         * Note: currently this isn't fully supported in the HMI as HNM isn't always aware of changes to the active tab
         */
        void stateChanged(QnxCar::HmiNotificationManager::State state, QString view, QString type);
        /**
         * This signal is emitted whenever a message is received.
         * Note: Currently we only support "Growl" messages
         */
        void messageReceived(QnxCar::HmiNotificationManager::MessageType messageType, QString name);

    private:
        class Private;
        Private* const d;
    };

}

#endif
