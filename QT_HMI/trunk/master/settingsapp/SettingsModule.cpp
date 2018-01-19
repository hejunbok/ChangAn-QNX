#include "SettingsModule.h"

#include "SettingsWorkflow.h"

#include <qqml.h>

#include "services/BluetoothDevicesModel.h"

#include <QTimer>

#include <cmath>

static void setupSortModel(QSortFilterProxyModel *model)
{
    model->setDynamicSortFilter(true);
    model->setSortRole(QnxCar::BluetoothDevicesModel::NameRole);
    model->sort(0);
}

SettingsModule::SettingsModule(QnxCar::Bluetooth *bluetooth, QMap<int,bool> availPages, QObject *parent) :
    QObject(parent)
  , m_bluetooth(bluetooth)
  , m_bluetoothListenTimer(new QTimer(this))
  , m_timeoutSeconds(0)
  , m_bluetoothDeviceName(bluetooth->deviceName())
  , m_sortedRemoteDevices(new QSortFilterProxyModel(this))
  , m_sortedPairedDevices(new QSortFilterProxyModel(this))
{
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterUncreatableType<SettingsWorkflow>("com.qnx.car.ui", 2, 0,
        "SettingsWorkflow", QStringLiteral("Access to enums"));

    m_sortedRemoteDevices->setSourceModel(m_bluetooth->remoteDevices());
    setupSortModel(m_sortedRemoteDevices);
    m_sortedPairedDevices->setSourceModel(m_bluetooth->pairedDevices());
    setupSortModel(m_sortedPairedDevices);

    connect(m_bluetoothListenTimer, &QTimer::timeout, this, &SettingsModule::updateBluetoothListen);

    m_workflow = new SettingsWorkflow(availPages, this);
}

SettingsWorkflow *SettingsModule::workflow() const
{
    return m_workflow;
}

QString SettingsModule::bluetoothDeviceName() const
{
    return m_bluetoothDeviceName;
}

void SettingsModule::setBluetoothDeviceName(const QString &bluetoothDeviceName)
{
    if (m_bluetoothDeviceName != bluetoothDeviceName) {

        if(bluetoothDeviceName.length() > 0) {
            m_bluetoothDeviceName = bluetoothDeviceName;
        }
        emit bluetoothDeviceNameChanged(m_bluetoothDeviceName);
    }
}

void SettingsModule::saveBluetoothDeviceName()
{
    m_bluetooth->setDeviceName(m_bluetoothDeviceName);
}

bool SettingsModule::bluetoothListening() const
{
    return m_bluetoothListenTimer->isActive();
}

void SettingsModule::startBluetoothListening(int seconds)
{
    if (bluetoothListening())
        return;
    m_timeoutSeconds = seconds;
    m_bluetoothListenDuration.start();
    m_bluetoothListenTimer->start();
    m_bluetooth->setAccessibilityLevel(QnxCar::Bluetooth::GeneralAccessible);
    emit bluetoothRemainingListenTimeChanged(bluetoothRemainingListenTime());
    emit bluetoothListeningChanged(true);
}

int SettingsModule::bluetoothRemainingListenTime() const
{
    return static_cast<int>(std::ceil(m_timeoutSeconds - (m_bluetoothListenDuration.elapsed() / 1000.)));
}

void SettingsModule::stopBluetoothListening()
{
    m_bluetooth->setAccessibilityLevel(QnxCar::Bluetooth::ConnectableOnly);
    m_bluetoothListenTimer->stop();
    emit bluetoothListeningChanged(false);
}

void SettingsModule::updateBluetoothListen()
{
    if (m_timeoutSeconds * 1000 - m_bluetoothListenDuration.elapsed() < 0) {
        stopBluetoothListening();
    } else {
        const int timeToNextSecond = 1000 - (m_bluetoothListenDuration.elapsed() % 1000);
        m_bluetoothListenTimer->start(timeToNextSecond + 100 /*safety margin for timer slack*/);
    }
    emit bluetoothRemainingListenTimeChanged(bluetoothRemainingListenTime());
}

QAbstractItemModel *SettingsModule::sortedRemoteDevices() const
{
    return m_sortedRemoteDevices;
}

QAbstractItemModel *SettingsModule::sortedPairedDevices() const
{
    return m_sortedPairedDevices;
}

void SettingsModule::askForDeleteDeviceConfirmation(const QString &address)
{
    emit deleteDeviceConfirmationAsked(address);
}

void SettingsModule::onQuit()
{
    if (m_bluetooth->deviceSearchRunning())
        m_bluetooth->cancelDeviceSearch();
    if (bluetoothListening())
        stopBluetoothListening();
}
