#include "Bluetooth.h"
#include "Bluetooth_p.h"

#include "BluetoothDevicesModel.h"
#include "BluetoothDevicesModel_p.h"

#include <QDebug>

namespace QnxCar {

static const char* pps_accessibilityLevelKey = "accessibility";
static const char* pps_activeConnectionAvailableKey = "active_connections";
static const char* pps_deviceAddressKey = "btaddr";
static const char* pps_activeKey = "enabled";
static const char* pps_deviceNameKey = "name";

static const char* pps_handsFreeProfileAvailableKey = "hfp";
static const char* pps_messageAccessProfileAvailableKey = "map";
static const char* pps_phoneBookAccessProfileAvailableKey = "pbap";
static const char* pps_serialPortProfileAvailableKey = "spp";
static const char* pps_personalNetworkAvailableKey = "pan";
static const char* pps_audioAvailableKey = "avrcp";

// Helper function to convert numeric value to enum
static Bluetooth::AccessibilityLevel numberToAccessibilityLevel(int number)
{
    switch (number) {
    case 0:
    default:
        return Bluetooth::NotAccessible;
    case 1:
        return Bluetooth::GeneralAccessible;
    case 2:
        return Bluetooth::LimitedAccessible;
    case 3:
        return Bluetooth::ConnectableOnly;
    case 4:
        return Bluetooth::DiscoverableOnly;
    }
}

// Helper function to convert enum to numeric value
static int accessibilityLevelToNumber(Bluetooth::AccessibilityLevel level)
{
    switch (level) {
    case Bluetooth::NotAccessible:
    default:
        return 0;
    case Bluetooth::GeneralAccessible:
        return 1;
    case Bluetooth::LimitedAccessible:
        return 2;
    case Bluetooth::ConnectableOnly:
        return 3;
    case Bluetooth::DiscoverableOnly:
        return 4;
    }
}

// Helper function to convert enum to service number
static QString serviceToString(Bluetooth::Service service)
{
    switch (service) {
    case Bluetooth::HandsFreeService:
        return QStringLiteral("0x111E");
    case Bluetooth::MessageAccessService:
        return QStringLiteral("0x1134");
    case Bluetooth::PhoneBookAccessService:
        return QStringLiteral("0x1130");
    case Bluetooth::PersonaAreaNetworkService:
        return QStringLiteral("0x1131");
    case Bluetooth::AVRCPService:
        return QStringLiteral("0x110B");
    }

    return QString();
}

Bluetooth::Private::Private(Bluetooth *qq)
    : QObject(qq)
    , q(qq)
    , m_accessibilityLevel(ConnectableOnly)
    , m_active(false)
    , m_deviceSearchRunning(false)
    , m_pairingRunning(false)
    , m_lastPairingResult(Bluetooth::PairingUnknown)
    , m_connectionState(Bluetooth::Undefined)
    , m_remoteDevices(new BluetoothDevicesModel(BluetoothDevicesModel::RemoteDevices, this))
    , m_pairedDevices(new BluetoothDevicesModel(BluetoothDevicesModel::PairedDevices, this))
{
    m_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/bluetooth/control"),
                                          QPps::Object::PublishMode, true, this);
    m_ppsSettingsObject = new QPps::Object(QStringLiteral("/pps/services/bluetooth/settings"),
                                           QPps::Object::SubscribeMode, true, this);
    m_ppsServicesObject = new QPps::Object(QStringLiteral("/pps/services/bluetooth/services"),
                                           QPps::Object::SubscribeMode, true, this);
    m_ppsStatusObject = new QPps::Object(QStringLiteral("/pps/services/bluetooth/status"),
                                         QPps::Object::SubscribeMode, true, this);

    if (!m_ppsControlObject->isValid()) {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsControlObject->errorString();
    }

    if (m_ppsSettingsObject->isValid()) {
        connect(m_ppsSettingsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsSettingsAttributeChanged(QString,QPps::Variant)));
        m_ppsSettingsObject->setAttributeCacheEnabled(true);

        m_accessibilityLevel = numberToAccessibilityLevel(m_ppsSettingsObject->attribute(QString::fromLatin1(pps_accessibilityLevelKey)).toInt());
        m_active = m_ppsSettingsObject->attribute(QString::fromLatin1(pps_activeKey)).toBool();

        emit q->activeChanged();

        m_deviceName = m_ppsSettingsObject->attribute(QString::fromLatin1(pps_deviceNameKey)).toString();
        m_activeConnection = m_ppsSettingsObject->attribute(QString::fromLatin1(pps_activeConnectionAvailableKey)).toBool();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsSettingsObject->errorString();
    }

    if (m_ppsServicesObject->isValid()) {
        connect(m_ppsServicesObject, SIGNAL(attributesChanged(QPps::Changeset)), this, SLOT(ppsServicesAttributesChanged(QPps::Changeset)));

        m_ppsServicesObject->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsServicesObject->errorString();
    }

    if (m_ppsStatusObject->isValid()) {
        connect(m_ppsStatusObject, &QPps::Object::attributeChanged,
                this, &Bluetooth::Private::ppsStatusAttributeChanged);
        m_ppsStatusObject->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
    }
}

Bluetooth::Private::~Private()
{
}

Bluetooth::Bluetooth(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

void Bluetooth::initiateDeviceSearch()
{
    d->m_ppsControlObject->setAttribute(QStringLiteral("command"), QPps::Variant(QStringLiteral("device_search")));
}

void Bluetooth::cancelDeviceSearch()
{
    if (!d->m_deviceSearchRunning)
        return;

    d->m_ppsControlObject->setAttribute(QStringLiteral("command"), QPps::Variant(QStringLiteral("cancel_device_search")));
}

void Bluetooth::initiatePairing(const QString &address)
{
    // will not start pairing if search not finished completely
    if(!d->m_deviceSearchRunning) {
        QPps::Changeset changeset;
        changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("initiate_pairing")));
        changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(address));
        if (!d->m_ppsControlObject->setAttributes(changeset)) {
            qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
        }
    } else {
        qWarning("Unable to start pairing, search is not finished");
    }
}

void Bluetooth::cancelPairing(const QString &address)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("cancel_pairing")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(address));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
    }
}

void Bluetooth::confirmPairing(const QString &address, bool confirm)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("user_confirm")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(address));
    changeset.assignments.insert(QStringLiteral("data2"), QPps::Variant(confirm));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
    }
}

void Bluetooth::connectService(const QString &address, Service service)
{
    connectService(address, serviceToString(service));
}

void Bluetooth::connectService(const QString &address, const QString &service)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("connect_service")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(address));
    changeset.assignments.insert(QStringLiteral("data2"), QPps::Variant(service));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
    }
}

void Bluetooth::disconnectService(const QString &address, Service service)
{
    disconnectService(address, serviceToString(service));
}

void Bluetooth::disconnectService(const QString &address, const QString &service)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("disconnect_service")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(address));
    changeset.assignments.insert(QStringLiteral("data2"), QPps::Variant(service));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
    }
}

void Bluetooth::confirmConnection(const QString &address, bool confirm)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("authorize")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(address));
    changeset.assignments.insert(QStringLiteral("data2"), QPps::Variant(confirm));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
    }
}

void Bluetooth::Private::ppsServicesAttributesChanged(QPps::Changeset changes)
{
    QMap<QString,QPps::Variant>::ConstIterator ppsIt;

    for (ppsIt = changes.assignments.constBegin(); ppsIt != changes.assignments.constEnd(); ++ppsIt) {

        QString key = ppsIt.key();

        if (key == QString::fromLatin1(pps_handsFreeProfileAvailableKey))
            emit q->handsFreeProfileAvailableChanged();
        else if (key == QString::fromLatin1(pps_messageAccessProfileAvailableKey))
            emit q->messageAccessProfileAvailableChanged();
        else if (key == QString::fromLatin1(pps_phoneBookAccessProfileAvailableKey))
            emit q->phoneBookAccessProfileAvailableChanged();
        else if (key == QString::fromLatin1(pps_serialPortProfileAvailableKey))
            emit q->serialPortProfileAvailableChanged();
        else if (key == QString::fromLatin1(pps_audioAvailableKey))
            emit q->audioProfileAvailableChanged();
        else if (key == QString::fromLatin1(pps_personalNetworkAvailableKey))
            emit q->personalNetworkProfileAvailableChanged();

        QString address = ppsIt.value().toString();

        // eliminate adding empty string which actually means service is not connected, need to remove it
        if(address.isEmpty()) {
            m_connectedServices.remove(key);
        } else {
            m_connectedServices.insert(key, address);
        }
    }
    updateConnectionState();
}

void Bluetooth::Private::ppsSettingsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_accessibilityLevelKey)) {
        const AccessibilityLevel level = numberToAccessibilityLevel(attribute.toInt());
        if (m_accessibilityLevel != level) {
            m_accessibilityLevel = level;
            emit q->accessibilityLevelChanged();
        }
    } else if (name == QString::fromLatin1(pps_activeKey)) {
        if (m_active != attribute.toBool()) {
            m_active = attribute.toBool();
            emit q->activeChanged();
        }
    } else if (name == QString::fromLatin1(pps_activeConnectionAvailableKey)) {
        emit q->activeConnectionAvailableChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_deviceNameKey)) {
        if (m_deviceName != attribute.toString()) {
            m_deviceName = attribute.toString();
            emit q->deviceNameChanged();
        }
    } else if (name == QString::fromLatin1(pps_deviceAddressKey)) {
        emit q->deviceAddressChanged();
    }
}

void Bluetooth::Private::pairingFinished(PairingResult result)
{
    const QString address = m_pairingDeviceAddress;

    m_pairingRunning = false;
    m_pairingDeviceAddress.clear();
    m_lastPairingResult = result;

    emit q->lastPairingResultChanged();
    emit q->pairingRunningChanged();
    emit q->pairingDeviceAddressChanged();
    emit q->pairingFinished(address, result);
}

void Bluetooth::Private::setConnectionState(ConnectionState state)
{
    if (m_connectionState == state)
        return;

    m_connectionState = state;
    emit q->connectionStateChanged();
}

void Bluetooth::Private::setConnectionDeviceAddress(const QString &address)
{
    if (m_connectionDeviceAddress == address)
        return;
    m_connectionDeviceAddress = address;
    emit q->connectionDeviceAddressChanged();
}

void Bluetooth::Private::setPairingDeviceAddress(const QString &address)
{
    if (m_pairingDeviceAddress == address)
        return;
    m_pairingDeviceAddress = address;
    emit q->pairingDeviceAddressChanged();
}

void Bluetooth::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QStringLiteral("event")) {
        const QString event = attribute.toString();
        if (event == QStringLiteral("BTMGR_EVENT_SEARCH_START_SUCCESS")) {
            m_deviceSearchRunning = true;
            emit q->deviceSearchRunningChanged();
        } else if (event == QStringLiteral("BTMGR_EVENT_SEARCH_START_FAIL")) {
            m_deviceSearchRunning = false;
            emit q->deviceSearchRunningChanged();
        } else if (event == QStringLiteral("BTMGR_EVENT_AUTHORIZE_REQUIRED")) {
            // Incoming Bluetooth connection request needs to be authorized.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_COMMAND_FAILED")) {
            // The last command sent has failed.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_CONFIRM_NUMERIC_REQ")) {
            // A pairing request from a remote device has been confirmed.
            const QString address = m_ppsStatusObject->attribute(QStringLiteral("data")).toString();
            const QString pin = m_ppsStatusObject->attribute(QStringLiteral("data2")).toString();
            setPairingDeviceAddress(address);
            emit q->pairingConfirmationRequested(address, pin);
        } else if (event == QStringLiteral("BTMGR_EVENT_CONNECT_ALL_FAILURE")) {
            // The attempt to connect all services has failed.
            // this event means that not all services could be connected, but not that
            // the device has no connected services at all/is disconnected.
            // Thus determine connection state based on the services connected.
            setConnectionDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            if(m_connectedServices.isEmpty()) {
                setConnectionState(Disconnected);
            } else {
                setConnectionState(Connected);
            }
        } else if (event == QStringLiteral("BTMGR_EVENT_CONNECT_ALL_SUCCESS")) {
            setConnectionDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            setConnectionState(Connected);
        } else if (event == QStringLiteral("BTMGR_EVENT_DEVICE_ADDED")) {
            // A remote device was paired.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_DEVICE_DELETED")) {
            // The specified paired device was successfully deleted.
            q->deviceDeleted(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
        } else if (event == QStringLiteral("BTMGR_EVENT_DEVICE_DELETED_FAILED")) {
            // Request to delete the specified paired device has failed.
            q->deviceDeletionFailed(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
        } else if (event == QStringLiteral("BTMGR_EVENT_DEVICE_LIST_CHANGED")) {
            // List of devices under /pps/services/bluetooth/remote_devices/<mac_addr> has changed.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_DEVICE_SEARCH_COMPLETE")) {
            // Search for remote devices is done.
            m_deviceSearchRunning = false;
            emit q->deviceSearchRunningChanged();
        } else if (event == QStringLiteral("BTMGR_EVENT_DISCONNECT_ALL_FAILURE")) {
            // The attempt to disconnect all services has failed.
            setConnectionDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            setConnectionState(DisconnectFailed);
        } else if (event == QStringLiteral("BTMGR_EVENT_DISCONNECT_ALL_SUCCESS")) {
            setConnectionDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            setConnectionState(Disconnected);
        } else if (event == QStringLiteral("BTMGR_EVENT_DISPLAY_NUMERIC_IND")) {
            // Authentication number shown during pairing.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_INIT_PAIRING_FAILED")) {
            // Request to initiate pairing to the remote device has failed.
            pairingFinished(PairingFailed);
        } else if (event == QStringLiteral("BTMGR_EVENT_INIT_PAIRING_SUCCESS")) {
            // Request to initiate pairing to the remote device has succeeded.
            m_pairingRunning = true;
            m_pairingDeviceAddress = m_ppsStatusObject->attribute(QStringLiteral("data")).toString();
            emit q->pairingRunningChanged();
            emit q->pairingDeviceAddressChanged();
        } else if (event == QStringLiteral("BTMGR_EVENT_LEGACY_PIN_REQUIRED")) {
            // Legacy PIN requested during pairing.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_PAIRING_CANCELED")) {
            // Pairing was canceled (by the user or timeout)
            setPairingDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            pairingFinished(PairingCanceled);
        } else if (event == QStringLiteral("BTMGR_EVENT_PAIRING_COMPLETE")) {
            // Pairing has succeeded
            setPairingDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            pairingFinished(PairingSuccessful);
        } else if (event == QStringLiteral("BTMGR_EVENT_PAIRING_FAILED")) {
            // Pairing has failed
            setPairingDeviceAddress(m_ppsStatusObject->attribute(QStringLiteral("data")).toString());
            pairingFinished(PairingFailed);
        } else if (event == QStringLiteral("BTMGR_EVENT_PASSKEY_REQUIRED")) {
            // Passkey (alphanumeric PIN) requested during pairing.
            //TODO:
        } else if (event == QStringLiteral("BTMGR_EVENT_RADIO_INIT")) {
            // Bluetooth system and the radio chip were initialized.
            if (!m_active) {
                m_active = true;
                emit q->activeChanged();
            }
        } else if (event == QStringLiteral("BTMGR_EVENT_RADIO_SHUTDOWN")) {
            // Radio was shut down.
            if (m_active) {
                m_active = false;
                emit q->activeChanged();
            }
        } else if (event == QStringLiteral("BTMGR_EVENT_STACK_FAULT")) {
            // Bluetooth stack has failed; restart required.
            //TODO:
        }
    }
}

void Bluetooth::Private::updateConnectionState() {

    // process startup state
    if(m_connectionState == Undefined) {
        if(m_connectedServices.isEmpty()) {
            setConnectionState(Disconnected);
        } else {
            setConnectionState(Connected);
            setConnectionDeviceAddress(m_connectedServices.constBegin().value());
        }
    }

    // Track if at least on service connected
    m_activeConnection = !m_connectedServices.isEmpty();
    emit q->activeConnectionAvailableChanged(m_activeConnection); //So in these cases, look into the connected services we know about

    // If there is nothing connected, setting Disconnected state( remote device disconnected )
    if(!m_activeConnection) {
        setConnectionState(Disconnected);
    }

    // This means we are not in process of manual connecting/disconnecting but rather phone (remote device ) initiated connect/disconnect
    if(m_connectionState != Connecting && m_connectionState != Disconnecting && m_activeConnection) {
        setConnectionState(Connected);
        setConnectionDeviceAddress(m_connectedServices.constBegin().value());
    }
}

Bluetooth::AccessibilityLevel Bluetooth::accessibilityLevel() const
{
    return d->m_accessibilityLevel;
}

void Bluetooth::setAccessibilityLevel(AccessibilityLevel level)
{
    if (d->m_accessibilityLevel == level)
        return;

    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("set_access")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(accessibilityLevelToNumber(level)));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
        return;
    }

    d->m_accessibilityLevel = level;
    emit accessibilityLevelChanged();
}

bool Bluetooth::active() const
{
    return d->m_active;
}

void Bluetooth::setActive(bool active)
{
    if (d->m_active == active)
        return;

    if (active)
        d->m_ppsControlObject->setAttribute(QStringLiteral("command"), QPps::Variant(QStringLiteral("radio_init")));
    else
        d->m_ppsControlObject->setAttribute(QStringLiteral("command"), QPps::Variant(QStringLiteral("radio_shutdown")));

    d->m_active = active;
    emit activeChanged();
}

QString Bluetooth::deviceName() const
{
    return d->m_deviceName;
}

void Bluetooth::setDeviceName(const QString &name)
{
    if (d->m_deviceName == name)
        return;

    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("command"), QPps::Variant(QStringLiteral("set_name")));
    changeset.assignments.insert(QStringLiteral("data"), QPps::Variant(name));
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
        return;
    }

    d->m_deviceName = name;
    emit deviceNameChanged();
}

void Bluetooth::removeDevice(const QString &address)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("[n]command"), QStringLiteral("remove_device"));
    changeset.assignments.insert(QStringLiteral("[n]data"), address);
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
        return;
    }
}

void Bluetooth::connectAllServices(const QString &address)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("[n]command"), QStringLiteral("connect_all"));
    changeset.assignments.insert(QStringLiteral("[n]data"), address);
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
        return;
    }

    d->setConnectionDeviceAddress(address);
    d->setConnectionState(Connecting);
}

void Bluetooth::disconnectAllServices(const QString &address)
{
    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("[n]command"), QStringLiteral("disconnect_all"));
    changeset.assignments.insert(QStringLiteral("[n]data"), address);
    if (!d->m_ppsControlObject->setAttributes(changeset)) {
        qWarning("Unable to write to bluetooth control: %s", qPrintable(d->m_ppsControlObject->errorString()));
        return;
    }

    d->setConnectionDeviceAddress(address);
    d->setConnectionState(Disconnecting);
}

void Bluetooth::disconnectBT(void)
{
    system("/opt/fixbt.sh &");
}

QString Bluetooth::deviceAddress() const
{
    return d->m_ppsSettingsObject->attribute(QString::fromLatin1(pps_deviceAddressKey)).toString();
}

bool Bluetooth::activeConnectionAvailable() const
{
    return d->m_activeConnection;
}

bool Bluetooth::deviceSearchRunning() const
{
    return d->m_deviceSearchRunning;
}

bool Bluetooth::pairingRunning() const
{
    return d->m_pairingRunning;
}

QString Bluetooth::pairingDeviceAddress() const
{
    return d->m_pairingDeviceAddress;
}

bool Bluetooth::handsFreeProfileAvailable() const
{
    const QString address = d->m_ppsServicesObject->attribute(QString::fromLatin1(pps_handsFreeProfileAvailableKey)).toString();

    return !address.isEmpty();
}

bool Bluetooth::messageAccessProfileAvailable() const
{
    const QString address = d->m_ppsServicesObject->attribute(QString::fromLatin1(pps_messageAccessProfileAvailableKey)).toString();

    return !address.isEmpty();
}

bool Bluetooth::phoneBookAccessProfileAvailable() const
{
    const QString address = d->m_ppsServicesObject->attribute(QString::fromLatin1(pps_phoneBookAccessProfileAvailableKey)).toString();

    return !address.isEmpty();
}

bool Bluetooth::serialPortProfileAvailable() const
{
    const QString address = d->m_ppsServicesObject->attribute(QString::fromLatin1(pps_serialPortProfileAvailableKey)).toString();

    return !address.isEmpty();
}

bool Bluetooth::personalNetworkProfileAvailable() const
{
    const QString address = d->m_ppsServicesObject->attribute(QString::fromLatin1(pps_personalNetworkAvailableKey)).toString();

    return !address.isEmpty();
}

bool Bluetooth::audioProfileAvailable() const
{
    const QString address = d->m_ppsServicesObject->attribute(QString::fromLatin1(pps_audioAvailableKey)).toString();

    return !address.isEmpty();
}

BluetoothDevicesModel *Bluetooth::remoteDevices() const
{
    return d->m_remoteDevices;
}

BluetoothDevicesModel *Bluetooth::pairedDevices() const
{
    return d->m_pairedDevices;
}

Bluetooth::PairingResult Bluetooth::lastPairingResult() const
{
    return d->m_lastPairingResult;
}

Bluetooth::ConnectionState Bluetooth::connectionState() const
{
    return d->m_connectionState;
}

QString Bluetooth::connectionDeviceAddress() const
{
    return d->m_connectionDeviceAddress;
}

}
