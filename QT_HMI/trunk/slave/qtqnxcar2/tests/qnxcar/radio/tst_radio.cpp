#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "qnxcar/Radio.h"

#include <qpps/simulator.h>

#include <QDebug>
#include <QSignalSpy>

using QnxCar::Radio;

class tst_Radio : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultRadioAttributes();
    void testSimulateTune();
    void testSimulateSeek();
    void testScan();
    void testSwitchTuner();
};

void tst_Radio::testDefaultRadioAttributes()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Radio radio;

    // check default values of the radio object
    QCOMPARE(radio.tuner(), Radio::FmTuner);
    QCOMPARE(radio.frequencyMinimum(), 87.5);
    QCOMPARE(radio.frequencyMaximum(), 107.1);
    QCOMPARE(radio.frequencyStep(), 0.2);
    QCOMPARE(radio.station(), 96.5);

    // check song info provided by simulator data
    QCOMPARE(radio.artist(), QStringLiteral("Bjork"));
    QCOMPARE(radio.song(), QStringLiteral("All is Full of Love"));
    QCOMPARE(radio.genre(), QStringLiteral("News & Entertainment"));
    QCOMPARE(radio.stationName(), QStringLiteral("CBC Radio 2"));
    QCOMPARE(radio.highDefinitionEnabled(), true);
}

void tst_Radio::testSimulateTune()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Radio radio;
    radio.setSimulationModeEnabled(true);
    QCOMPARE(radio.simulationModeEnabled(), true);

    // check default value
    QCOMPARE(radio.station(), 96.5);

    // tune to valid station
    radio.tune(88);
    QCOMPARE(radio.station(), 88.0);
}

void tst_Radio::testSimulateSeek()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Radio radio;
    radio.setSimulationModeEnabled(true);
    QCOMPARE(radio.simulationModeEnabled(), true);

    // check default value
    QCOMPARE(radio.station(), 96.5);

    QCOMPARE(radio.seekingUp(), false);
    QCOMPARE(radio.seekingDown(), false);

    // seek up
    {
    double currentStation = radio.station();
    QSignalSpy seekingUpSpy(&radio, SIGNAL(seekingUpChanged(bool)));
    radio.seekUp(); // synchronous
    QCOMPARE(seekingUpSpy.count(), 2); // false->true, true->false

    // check
    QVERIFY(currentStation < radio.station());
    }

    // seek down
    {
    double currentStation = radio.station();

    QSignalSpy seekingDownSpy(&radio, SIGNAL(seekingDownChanged(bool)));
    radio.seekDown(); // synchronous
    QCOMPARE(seekingDownSpy.count(), 2); // false->true, true->false

    // check
    QVERIFY(currentStation > radio.station());
    }
}

void tst_Radio::testScan()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Radio radio;
    radio.setSimulationModeEnabled(true);

    QCOMPARE(radio.scanDirection(), Radio::ScanNone);

    // shrink scan interval for testing scenario
    radio.setScanInterval(100);
    int scanInterval = radio.scanInterval();
    QCOMPARE(scanInterval, 100);

    // scan up
    {
    double currentStation = radio.station();
    radio.scanUp();
    QCOMPARE(radio.scanDirection(), Radio::ScanUp);

    QTest::qWait(scanInterval + 10);
    QVERIFY(currentStation != radio.station());
    currentStation = radio.station();
    QCOMPARE(radio.scanDirection(), Radio::ScanUp);

    QTest::qWait(scanInterval + 10);
    QVERIFY(currentStation != radio.station());

    radio.stopScan();
    QCOMPARE(radio.scanDirection(), Radio::ScanNone);
    }

    // scan down
    {
    double currentStation = radio.station();
    radio.scanDown();

    QTest::qWait(scanInterval + 10);
    QVERIFY(currentStation != radio.station());
    currentStation = radio.station();

    QTest::qWait(scanInterval + 10);
    QVERIFY(currentStation != radio.station());

    radio.stopScan();
    QCOMPARE(radio.scanDirection(), Radio::ScanNone);
    }
}


void tst_Radio::testSwitchTuner()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Radio radio;
    radio.setSimulationModeEnabled(true);

    radio.setTuner(Radio::AmTuner);
    QCOMPARE(radio.tuner(), Radio::AmTuner);

    // check default values for AM tuner
    QCOMPARE(radio.frequencyMinimum(), 880.0);
    QCOMPARE(radio.frequencyMaximum(), 1710.0);
    QCOMPARE(radio.frequencyStep(), 10.0);
}

QTEST_MAIN(tst_Radio)
#include "tst_radio.moc"
