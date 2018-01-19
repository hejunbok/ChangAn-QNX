#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "services/Phone.h"

#include <qpps/simulator.h>

using QnxCar::Phone;

class tst_Phone : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultValues();
};

void tst_Phone::testDefaultValues()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Phone phone;

    // check default values
    QCOMPARE(phone.state(), Phone::InitializedState);

    // TODO: Implement more checks once Phone is actually implemented
}

QTEST_MAIN(tst_Phone)
#include "tst_phone.moc"
