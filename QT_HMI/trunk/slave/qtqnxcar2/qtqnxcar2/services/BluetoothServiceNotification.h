#ifndef QTQNXCAR2_BLUETOOTHSERVICENOTIFICATION_H
#define QTQNXCAR2_BLUETOOTHSERVICENOTIFICATION_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {

/**
 * Wrapper class for messages notification PPS object
 *
 * Covers the following PPS objects:
 * - @c /pps/services/messages/notification
 */
class QTQNXCAR2_EXPORT BluetoothServiceNotification : public QObject
{
    Q_OBJECT

public:
    /// Message type (E-Mail, MMS, ...)
    enum MessageType {
        InvalidType,
        EMail,
        MMS,
        SMS_CDMA,
        SMS_GSM
    };

    /// Message status
    enum Status {
        InvalidStatus,
        DeliveryFailure,
        DeliverySuccess,
        MemoryAvailable,
        MemoryFull,
        MessageDeleted,
        MessageShift,
        NewMessage,
        SendingFailure,
        SendingSuccess
    };

    /**
     * Creates a wrapper for the given service
     * @param parent the QObject parent
     */
    explicit BluetoothServiceNotification(QObject *parent = 0);

Q_SIGNALS:
    /**
     * Emitted whenever a new notification is received
     *
     * @p accountId Number starting from zero found in the MAP database.
     * @p handle Hex number ID
     * @p type Message type
     * @p status Message status
     */
    void notification(int accountId, const QString &handle,
                      QnxCar::BluetoothServiceNotification::MessageType type,
                      QnxCar::BluetoothServiceNotification::Status status);

private:
    class Private;
    Private* const d;
};

}

#endif
