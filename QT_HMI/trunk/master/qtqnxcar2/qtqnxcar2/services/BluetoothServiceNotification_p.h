#ifndef QTQNXCAR2_BLUETOOTHSERVICENOTIFICATION_P_H
#define QTQNXCAR2_BLUETOOTHSERVICENOTIFICATION_P_H

#include "BluetoothServiceNotification.h"

#include <qpps/object.h>

#include <QMap>

namespace QnxCar {

/**
 * Internal data structure for keeping notification-related data together
 *
 * Example notification:
 * @code
 * [n]account_id:n:0
 * [n]message_handle::5424
 * [n]message_type::SMS_GSM
 * [n]status::NEW_MESSAGE
 * @endcode
 */
struct NotificationData
{
    NotificationData()
        : accountId(-1)
        , type(BluetoothServiceNotification::InvalidType)
        , status(BluetoothServiceNotification::InvalidStatus)
    {}

    /// Number starting from zero found in the MAP database.
    int accountId;
    /// Hex number ID
    QString handle;
    BluetoothServiceNotification::MessageType type;
    BluetoothServiceNotification::Status status;

    static NotificationData fromChangeset(const QMap<QString, QPps::Variant> &assignments);

    bool isValid() const;
};

class BluetoothServiceNotification::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(BluetoothServiceNotification *q);
    virtual ~Private();

    BluetoothServiceNotification *const q;

    QPps::Object *m_ppsObject;

    void notifyObservers(const NotificationData &notification);

private Q_SLOTS:
    void ppsStatusAttributesChanged(const QPps::Changeset &changes);
};

}

#endif
