#ifndef __BT_WIFI_SETTINGSMODULE_H__
#define __BT_WIFI_SETTINGSMODULE_H__

#include <QObject>

#include <QElapsedTimer>
#include <QSortFilterProxyModel>

#include "services/Bluetooth.h"

class QTimer;

namespace QnxCar {
    class Bluetooth;
}

class BtWifi_SettingsWorkflow;

class BtWifi_SettingsModule : public QObject
{
    Q_OBJECT

    Q_PROPERTY(BtWifi_SettingsWorkflow* workflow READ workflow CONSTANT)
    Q_PROPERTY(QString bluetoothDeviceName READ bluetoothDeviceName WRITE setBluetoothDeviceName NOTIFY bluetoothDeviceNameChanged)
    Q_PROPERTY(bool bluetoothListening READ bluetoothListening NOTIFY bluetoothListeningChanged)
    Q_PROPERTY(int bluetoothRemainingListenTime READ bluetoothRemainingListenTime NOTIFY bluetoothRemainingListenTimeChanged)
    Q_PROPERTY(QAbstractItemModel* sortedRemoteDevices READ sortedRemoteDevices CONSTANT)
    Q_PROPERTY(QAbstractItemModel* sortedPairedDevices READ sortedPairedDevices CONSTANT)

public:
    explicit BtWifi_SettingsModule(QnxCar::Bluetooth *bluetooth, QMap<int,bool> availPages, QObject *parent = 0);

    BtWifi_SettingsWorkflow *workflow() const;

    QString bluetoothDeviceName() const;
    void setBluetoothDeviceName(const QString &bluetoothDeviceName);

    bool bluetoothListening() const;

    int bluetoothRemainingListenTime() const;

    QAbstractItemModel *sortedRemoteDevices() const;

    QAbstractItemModel *sortedPairedDevices() const;

Q_SIGNALS:
    void bluetoothDeviceNameChanged(const QString &bluetoothDeviceName);
    void bluetoothListeningChanged(bool listening);
    void bluetoothRemainingListenTimeChanged(int seconds);
    void deleteDeviceConfirmationAsked(const QString &address);

public Q_SLOTS:
    void saveBluetoothDeviceName();
    void startBluetoothListening(int timeoutSeconds);
    void stopBluetoothListening();
    void askForDeleteDeviceConfirmation(const QString &address);
    void updateBluetoothListen();
    void onQuit();

private:
    QnxCar::Bluetooth *m_bluetooth;
    BtWifi_SettingsWorkflow *m_workflow;
    QTimer *m_bluetoothListenTimer;
    QElapsedTimer m_bluetoothListenDuration;
    int m_timeoutSeconds;
    QString m_bluetoothDeviceName;
    QSortFilterProxyModel *m_sortedRemoteDevices;
    QSortFilterProxyModel *m_sortedPairedDevices;
};

#endif // __BT_WIFI_SETTINGSMODULE_H__
