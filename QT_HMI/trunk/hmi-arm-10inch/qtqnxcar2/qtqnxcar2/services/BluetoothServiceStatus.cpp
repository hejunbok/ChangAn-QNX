#include "BluetoothServiceStatus.h"
#include "BluetoothServiceStatus_p.h"

#include <QDebug>

namespace QnxCar {

static const char* pps_stateKey = "state";
static const char* pps_statusKey = "status";

BluetoothServiceStatus::State stringToState(const QString &str)
{
    if (str == QStringLiteral("CONNECTED")) {
        return BluetoothServiceStatus::Connected;
    } else if (str == QStringLiteral("CONNECTING")) {
        return BluetoothServiceStatus::Connecting;
    } else if (str == QStringLiteral("DISCONNECTED")) {
        return BluetoothServiceStatus::Disconnected;
    } else if (str == QStringLiteral("UNINITIALIZED")) {
        return BluetoothServiceStatus::Uninitialized;
    }

    qWarning() << Q_FUNC_INFO << "Unhandled value:" << str;
    return BluetoothServiceStatus::Uninitialized;
}

BluetoothServiceStatus::Status stringToStatus(const QString &str)
{
    if (str == QStringLiteral("COMPLETE")) {
        return BluetoothServiceStatus::Complete;
    } else if (str == QStringLiteral("ERROR_BUSY")) {
        return BluetoothServiceStatus::ErrorBusy;
    } else if (str == QStringLiteral("ERROR_COMMAND_NOT_KNOWN")) {
        return BluetoothServiceStatus::ErrorCommandNotKnown;
    } else if (str == QStringLiteral("ERROR_NOT_CONNECTED")) {
        return BluetoothServiceStatus::ErrorNotConnected;
    } else if (str == QStringLiteral("FAILED")) {
        return BluetoothServiceStatus::Failed;
    } else if (str == QStringLiteral("PROCESSING")) {
        return BluetoothServiceStatus::Processing;
    }

    qWarning() << Q_FUNC_INFO << "Unhandled value:" << str;
    return BluetoothServiceStatus::ErrorNotConnected;
}

static QString objectForType(BluetoothServiceStatus::ServiceType type)
{
    switch (type) {
    case BluetoothServiceStatus::Addressbook:
        return QStringLiteral("/pps/services/bluetooth/phonebook/status");
    case BluetoothServiceStatus::Messages:
        return QStringLiteral("/pps/services/bluetooth/messages/status");
    }

    qFatal("Unhandled service type");
    return QString();
}

BluetoothServiceStatus::Private::Private(BluetoothServiceStatus::ServiceType type, BluetoothServiceStatus *qq)
    : QObject(qq)
    , q(qq)
    , m_state(Uninitialized)
    , m_status(ErrorNotConnected)
{
    m_ppsStatusObject = new QPps::Object(objectForType(type), QPps::Object::PublishAndSubscribeMode, true, this);
    if (m_ppsStatusObject->isValid()) {
        connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsStatusAttributeChanged(QString,QPps::Variant)));
        m_ppsStatusObject->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
    }
}

BluetoothServiceStatus::Private::~Private()
{
}

void BluetoothServiceStatus::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_stateKey)) {
        const State state = stringToState(attribute.toString());
        if (m_state != state) {
            m_state = state;
            emit q->stateChanged(m_state);
        }
    } else if (name == QString::fromLatin1(pps_statusKey)) {
        const Status status = stringToStatus(attribute.toString());
        if (m_status != status) {
            m_status = status;
            emit q->statusChanged(m_status);
        }
    }
}

BluetoothServiceStatus::BluetoothServiceStatus(BluetoothServiceStatus::ServiceType type, QObject *parent)
    : QObject(parent)
    , d(new Private(type, this))

{
}

BluetoothServiceStatus::State BluetoothServiceStatus::state() const
{
    return d->m_state;
}

BluetoothServiceStatus::Status BluetoothServiceStatus::status() const
{
    return d->m_status;
}

}
