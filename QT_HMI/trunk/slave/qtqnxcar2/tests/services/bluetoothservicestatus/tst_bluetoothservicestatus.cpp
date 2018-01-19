#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "services/BluetoothServiceStatus.h"

#include <qpps/simulator.h>

using QnxCar::BluetoothServiceStatus;

class tst_BluetoothServiceStatus : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultValues();
};

void tst_BluetoothServiceStatus::testDefaultValues()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    const BluetoothServiceStatus addressbook(BluetoothServiceStatus::Addressbook);
    const BluetoothServiceStatus messages(BluetoothServiceStatus::Messages);

    // check default values
    QCOMPARE(addressbook.state(), BluetoothServiceStatus::Connected);
    QCOMPARE(addressbook.status(), BluetoothServiceStatus::Complete);
    QCOMPARE(messages.state(), BluetoothServiceStatus::Connected);
    QCOMPARE(messages.status(), BluetoothServiceStatus::Complete);
}

QTEST_MAIN(tst_BluetoothServiceStatus)
#include "tst_bluetoothservicestatus.moc"
