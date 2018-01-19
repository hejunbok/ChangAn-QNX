#ifndef QTQNXCAR2_BLUETOOTH_P_H
#define QTQNXCAR2_BLUETOOTH_P_H

#include "Bluetooth.h"
#include <qpps/changeset.h>
#include <qpps/object.h>

#include <QMap>

namespace QnxCar {

class Bluetooth::Private : public QObject
{
    Q_OBJECT

public:
    explicit Private(Bluetooth *q = 0);
    virtual ~Private();

    void pairingFinished(PairingResult result);
    void setPairingDeviceAddress(const QString &address);
    void setConnectionState(ConnectionState state);
    void setConnectionDeviceAddress(const QString &address);
    void updateConnectionState();

    Bluetooth *const q;

    QPps::Object *m_ppsControlObject;
    QPps::Object *m_ppsServicesObject;
    QPps::Object *m_ppsSettingsObject;
    QPps::Object *m_ppsStatusObject;

    AccessibilityLevel m_accessibilityLevel;
    bool m_active;
    QString m_deviceName;
    bool m_activeConnection;
    bool m_deviceSearchRunning;
    bool m_pairingRunning;
    QString m_pairingDeviceAddress;
    PairingResult m_lastPairingResult;
    ConnectionState m_connectionState;
    QString m_connectionDeviceAddress;
    BluetoothDevicesModel *m_remoteDevices;
    BluetoothDevicesModel *m_pairedDevices;
    QMap<QString,QString> m_connectedServices;

public Q_SLOTS:
    void ppsServicesAttributesChanged(QPps::Changeset);
    void ppsSettingsAttributeChanged(const QString &name, const QPps::Variant &attribute);
    void ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute);
};

}

#endif
