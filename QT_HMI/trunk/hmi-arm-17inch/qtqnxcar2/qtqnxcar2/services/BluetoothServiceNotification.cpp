#include "BluetoothServiceNotification.h"
#include "BluetoothServiceNotification_p.h"

#include "util/Util.h"

#include <qpps/changeset.h>

#include <QDebug>

static const char *pps_accountIdKey = "account_id";
static const char *pps_messageHandleKey = "message_handle";
static const char *pps_messageTypeKey = "message_type";
static const char *pps_statusKey = "status";

using namespace QnxCar;

BluetoothServiceNotification::MessageType stringToMessageType(const QString &str)
{
    if (str == QStringLiteral("EMAIL")) {
        return BluetoothServiceNotification::EMail;
    } else if (str == QStringLiteral("MMS")) {
        return BluetoothServiceNotification::MMS;
    } else if (str == QStringLiteral("SMS_CDMA")) {
        return BluetoothServiceNotification::SMS_CDMA;
    } else if (str == QStringLiteral("SMS_GSM")) {
        return BluetoothServiceNotification::SMS_GSM;
    }

    qWarning() << Q_FUNC_INFO << "Unhandled value:" << str;
    return BluetoothServiceNotification::InvalidType;
}

BluetoothServiceNotification::Status stringToStatus(const QString &str)
{
    if (str == QStringLiteral("DELIVERY_FAILURE")) {
        return BluetoothServiceNotification::DeliveryFailure;
    } else if (str == QStringLiteral("DELIVER_SUCCESS")) {
        return BluetoothServiceNotification::DeliverySuccess;
    } else if (str == QStringLiteral("MEMORY_AVAILABLE")) {
        return BluetoothServiceNotification::MemoryAvailable;
    } else if (str == QStringLiteral("MEMORY_FULL")) {
        return BluetoothServiceNotification::MemoryFull;
    } else if (str == QStringLiteral("MESSAGE_DELETED")) {
        return BluetoothServiceNotification::MessageDeleted;
    } else if (str == QStringLiteral("MESSAGE_SHIFT")) {
        return BluetoothServiceNotification::MessageShift;
    } else if (str == QStringLiteral("NEW_MESSAGE")) {
        return BluetoothServiceNotification::NewMessage;
    } else if (str == QStringLiteral("SENDING_FAILURE")) {
        return BluetoothServiceNotification::SendingFailure;
    } else if (str == QStringLiteral("SENDING_SUCCESS")) {
        return BluetoothServiceNotification::SendingSuccess;
    }

    qWarning() << Q_FUNC_INFO << "Unhandled value:" << str;
    return BluetoothServiceNotification::InvalidStatus;
}

bool NotificationData::isValid() const
{
    return accountId >= 0 && !handle.isEmpty()
        && type != BluetoothServiceNotification::InvalidType
        && status != BluetoothServiceNotification::InvalidStatus;
}

NotificationData NotificationData::fromChangeset(const QMap<QString, QPps::Variant> &assignments)
{
    NotificationData notification;
    notification.accountId = assignments[pps_accountIdKey].toInt();
    notification.handle = assignments[pps_messageHandleKey].toString();
    notification.type = stringToMessageType(assignments[pps_messageTypeKey].toString());
    notification.status = stringToStatus(assignments[pps_statusKey].toString());
    return notification;
}

BluetoothServiceNotification::Private::Private(BluetoothServiceNotification *qq)
    : QObject(qq)
    , q(qq)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/services/bluetooth/messages/notification"), QPps::Object::SubscribeMode, true, this);
    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, &QPps::Object::attributesChanged,
                this, &BluetoothServiceNotification::Private::ppsStatusAttributesChanged);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

BluetoothServiceNotification::Private::~Private()
{
}

void BluetoothServiceNotification::Private::ppsStatusAttributesChanged(const QPps::Changeset &changes)
{
    car2_debug() << changes.assignments;

    NotificationData notification = NotificationData::fromChangeset(changes.assignments);
    if (!notification.isValid()) {
        qWarning() << "Received invalid notification: " << changes.assignments;
        return;
    }

    notifyObservers(notification);
}

void BluetoothServiceNotification::Private::notifyObservers(const NotificationData &notification)
{
    car2_debug() << "message: " << notification.type;

    emit q->notification(notification.accountId, notification.handle,
                         notification.type, notification.status);
}

BluetoothServiceNotification::BluetoothServiceNotification(QObject *parent)
    : QObject(parent)
    , d(new Private(this))

{
}
