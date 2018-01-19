#include <QtTest/QtTest>
#include <QCoreApplication>

#include "simulator/Simulator.h"
#include "services/Bluetooth.h"
#include "services/BluetoothDevicesModel.h"

#include <qpps/simulator.h>

using QnxCar::Bluetooth;
using QnxCar::BluetoothDevicesModel;

class tst_Bluetooth : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultValues();
    void testBluetoothDevicesModel();
};

void tst_Bluetooth::testDefaultValues()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Bluetooth bluetooth;

    // check default values
    QCOMPARE(bluetooth.accessibilityLevel(), Bluetooth::ConnectableOnly);
    QCOMPARE(bluetooth.active(), true);
    QCOMPARE(bluetooth.deviceSearchRunning(), false);
    QCOMPARE(bluetooth.pairingRunning(), false);
    QCOMPARE(bluetooth.pairingDeviceAddress(), QString());
    QCOMPARE(bluetooth.activeConnectionAvailable(), false);
    QCOMPARE(bluetooth.handsFreeProfileAvailable(), false);
    QCOMPARE(bluetooth.messageAccessProfileAvailable(), false);
    QCOMPARE(bluetooth.phoneBookAccessProfileAvailable(), false);
    QCOMPARE(bluetooth.serialPortProfileAvailable(), false);
    QVERIFY(!bluetooth.deviceName().isEmpty());
    QVERIFY(!bluetooth.deviceAddress().isEmpty());
    QCOMPARE(bluetooth.lastPairingResult(), Bluetooth::PairingUnknown);
    QCOMPARE(bluetooth.connectionState(), Bluetooth::Disconnected);
    QCOMPARE(bluetooth.connectionDeviceAddress(), QString());

    // check models
    QVERIFY(bluetooth.remoteDevices());
    QVERIFY(bluetooth.pairedDevices());
}

void tst_Bluetooth::testBluetoothDevicesModel()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Bluetooth bluetooth;

    // TODO: Remove once https://codereview.kdab.com/#/c/1472/ has been resolved
    // Make sure we got the initial object listing
    qApp->processEvents();

    BluetoothDevicesModel *remoteDevices = bluetooth.remoteDevices();
    QVERIFY(remoteDevices);
    QCOMPARE(remoteDevices->rowCount(), 1);
    const QModelIndex remote0 = remoteDevices->index(0, 0);
    QVERIFY(remote0.isValid());
    QCOMPARE(remote0.data(Qt::DisplayRole).toString(), QStringLiteral("Device1"));
    QCOMPARE(remote0.data(BluetoothDevicesModel::AddressRole).toString(), QStringLiteral("AC:FC:14:CE:E5:5D"));
    QCOMPARE(remote0.data(BluetoothDevicesModel::NameRole).toString(), QStringLiteral("Device1"));
    QCOMPARE(remote0.data(BluetoothDevicesModel::InRangeRole).toBool(), false);
    QCOMPARE(remote0.data(BluetoothDevicesModel::PairedRole).toBool(), false);

    BluetoothDevicesModel *pairedDevices = bluetooth.pairedDevices();
    QVERIFY(pairedDevices);
    QCOMPARE(pairedDevices->rowCount(), 2);
    const QModelIndex paired0 = pairedDevices->index(0, 0);
    QVERIFY(paired0.isValid());
    QCOMPARE(paired0.data(Qt::DisplayRole).toString(), QStringLiteral("Device1"));
    QCOMPARE(paired0.data(BluetoothDevicesModel::AddressRole).toString(), QStringLiteral("AC:FC:14:CE:E5:5D"));
    QCOMPARE(paired0.data(BluetoothDevicesModel::NameRole).toString(), QStringLiteral("Device1"));
    QCOMPARE(paired0.data(BluetoothDevicesModel::InRangeRole).toBool(), false);
    QCOMPARE(paired0.data(BluetoothDevicesModel::PairedRole).toBool(), true);

    const QModelIndex paired1 = pairedDevices->index(1, 0);
    QVERIFY(paired1.isValid());
    QCOMPARE(paired1.data(Qt::DisplayRole).toString(), QStringLiteral("Device2"));
    QCOMPARE(paired1.data(BluetoothDevicesModel::AddressRole).toString(), QStringLiteral("AC:FC:14:CE:E5:5E"));
    QCOMPARE(paired1.data(BluetoothDevicesModel::NameRole).toString(), QStringLiteral("Device2"));
    QCOMPARE(paired1.data(BluetoothDevicesModel::InRangeRole).toBool(), true);
    QCOMPARE(paired1.data(BluetoothDevicesModel::PairedRole).toBool(), true);
    QCOMPARE(paired1.data(BluetoothDevicesModel::ClassRole).toString(), QString());
    QCOMPARE(paired1.data(BluetoothDevicesModel::ServicesRole).toStringList(), QStringList());

    QCOMPARE(pairedDevices->rowForDeviceId(QStringLiteral("AC:FC:14:CE:E5:5D")), 0);
    QCOMPARE(pairedDevices->rowForDeviceId(QStringLiteral("AC:FC:14:CE:E5:5E")), 1);
    QCOMPARE(pairedDevices->rowForDeviceId(QStringLiteral("AC:FC:14:CE:E5:AA")), -1);
    QCOMPARE(pairedDevices->nameForAddress(QStringLiteral("AC:FC:14:CE:E5:5D")), QStringLiteral("Device1"));
    QCOMPARE(pairedDevices->nameForAddress(QStringLiteral("AC:FC:14:CE:E5:5E")), QStringLiteral("Device2"));
    QCOMPARE(pairedDevices->nameForAddress(QStringLiteral("AC:FC:14:CE:E5:AA")), QString());

    const QHash<int, QByteArray> roleNames = pairedDevices->roleNames();
    QCOMPARE(roleNames.count(), 12);
    QCOMPARE(roleNames[BluetoothDevicesModel::AddressRole], QByteArrayLiteral("address"));
    QCOMPARE(roleNames[BluetoothDevicesModel::NameRole], QByteArrayLiteral("name"));
    QCOMPARE(roleNames[BluetoothDevicesModel::PairedRole], QByteArrayLiteral("paired"));
    QCOMPARE(roleNames[BluetoothDevicesModel::InRangeRole], QByteArrayLiteral("inRange"));
    QCOMPARE(roleNames[BluetoothDevicesModel::ClassRole], QByteArrayLiteral("class"));
    QCOMPARE(roleNames[BluetoothDevicesModel::ServicesRole], QByteArrayLiteral("services"));
}

QTEST_MAIN(tst_Bluetooth)
#include "tst_bluetooth.moc"
