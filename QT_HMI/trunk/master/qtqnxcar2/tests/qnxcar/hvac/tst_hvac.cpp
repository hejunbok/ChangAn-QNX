#include <QtTest/QtTest>

#include "qnxcar/Hvac.h"

#include <qpps/simulator.h>

using QnxCar::Hvac;

class tst_Hvac : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void ppsInaccessible();
    void ppsAccessible();
    void changeProperty_data();
    void changeProperty();
    void zoneLinkDisabled();
    void zoneLinkEnabled();

private:
    void initializeSimulator();
};

void tst_Hvac::initializeSimulator()
{
    QPps::Simulator::self()->reset();
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("airRecirculation_all"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("airConditioning_all"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1left"), QByteArrayLiteral("feet"), QByteArrayLiteral(""));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1right"), QByteArrayLiteral("feet"), QByteArrayLiteral(""));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1left"), QByteArrayLiteral("6"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1right"), QByteArrayLiteral("6"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1left"), QByteArrayLiteral("18"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1right"), QByteArrayLiteral("17"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1left"), QByteArrayLiteral("3"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1right"), QByteArrayLiteral("3"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("defrost_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("zoneLink_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
}

void tst_Hvac::ppsInaccessible()
{
    Hvac hvac;

    // simulator is not initialized -> PPS path cannot be accessed -> use default values
    QCOMPARE(hvac.property("airCirculationSetting").toBool(), false);
    QCOMPARE(hvac.property("airConditioningEnabled").toBool(), false);
    QCOMPARE(hvac.property("fanSettingLeft"), "faceAndFeet");
    QCOMPARE(hvac.property("fanSettingRight"), "defrost");
    QCOMPARE(hvac.property("fanSpeedLeft").toInt(), 1);
    QCOMPARE(hvac.property("fanSpeedRight").toInt(), 3);
    QCOMPARE(hvac.property("fanTemperatureLeft").toInt(), 21);
    QCOMPARE(hvac.property("fanTemperatureRight").toInt(), 19);
    QCOMPARE(hvac.property("heatedSeatLevelLeft").toInt(), 2);
    QCOMPARE(hvac.property("heatedSeatLevelRight").toInt(), 0);
    QCOMPARE(hvac.property("rearDefrostEnabled").toBool(), true);
    QCOMPARE(hvac.property("zoneLinkEnabled").toBool(), false);
}

void tst_Hvac::ppsAccessible()
{
    initializeSimulator();

    Hvac hvac;

    QCOMPARE(hvac.property("airCirculationSetting").toBool(), true);
    QCOMPARE(hvac.property("airConditioningEnabled").toBool(), true);
    QCOMPARE(hvac.property("fanSettingLeft"), "feet");
    QCOMPARE(hvac.property("fanSettingRight"), "feet");
    QCOMPARE(hvac.property("fanSpeedLeft").toInt(), 6);
    QCOMPARE(hvac.property("fanSpeedRight").toInt(), 6);
    QCOMPARE(hvac.property("fanTemperatureLeft").toInt(), 18);
    QCOMPARE(hvac.property("fanTemperatureRight").toInt(), 17);
    QCOMPARE(hvac.property("heatedSeatLevelLeft").toInt(), 3);
    QCOMPARE(hvac.property("heatedSeatLevelRight").toInt(), 3);
    QCOMPARE(hvac.property("rearDefrostEnabled").toBool(), false);
    QCOMPARE(hvac.property("zoneLinkEnabled").toBool(), false);
}

void tst_Hvac::changeProperty_data()
{
    QTest::addColumn<QByteArray>("propertyName");
    QTest::addColumn<QString>("key");
    QTest::addColumn<QByteArray>("propertyChangedSignal");
    QTest::addColumn<QVariant>("propertyValue");

    QTest::newRow("change airCirculationSetting property") << QByteArray("airCirculationSetting") << QStringLiteral("airRecirculation_all") << QByteArray(SIGNAL(airCirculationSettingChanged(bool))) << QVariant::fromValue(false);
    QTest::newRow("change airConditioningEnabled property") << QByteArray("airConditioningEnabled") << QStringLiteral("airConditioning_all") << QByteArray(SIGNAL(airConditioningEnabledChanged(bool))) << QVariant::fromValue(false);
    QTest::newRow("change fanSettingLeft property") << QByteArray("fanSettingLeft") << QStringLiteral("fanDirection_row1left") << QByteArray(SIGNAL(fanSettingLeftChanged(int))) << QVariant::fromValue("faceAndFeet");
    QTest::newRow("change fanSettingRight property") << QByteArray("fanSettingRight") << QStringLiteral("fanDirection_row1right") << QByteArray(SIGNAL(fanSettingRightChanged(int))) << QVariant::fromValue("defrost");
    QTest::newRow("change fanSpeedLeft property") << QByteArray("fanSpeedLeft") << QStringLiteral("fanSpeed_row1left") << QByteArray(SIGNAL(fanSpeedLeftChanged(int))) << QVariant::fromValue(1);
    QTest::newRow("change fanSpeedRight property") << QByteArray("fanSpeedRight") << QStringLiteral("fanSpeed_row1right") << QByteArray(SIGNAL(fanSpeedRightChanged(int))) << QVariant::fromValue(3);
    QTest::newRow("change fanTemperatureLeft property") << QByteArray("fanTemperatureLeft") << QStringLiteral("temperature_row1left") << QByteArray(SIGNAL(fanTemperatureLeftChanged(int))) << QVariant::fromValue(21);
    QTest::newRow("change fanTemperatureRight property") << QByteArray("fanTemperatureRight") << QStringLiteral("temperature_row1right") << QByteArray(SIGNAL(fanTemperatureRightChanged(int))) << QVariant::fromValue(19);
    QTest::newRow("change heatedSeatLevelLeft property") << QByteArray("heatedSeatLevelLeft") << QStringLiteral("heatedSeat_row1left") << QByteArray(SIGNAL(heatedSeatLevelLeftChanged(int))) << QVariant::fromValue(2);
    QTest::newRow("change heatedSeatLevelRight property") << QByteArray("heatedSeatLevelRight") << QStringLiteral("heatedSeat_row1right") << QByteArray(SIGNAL(heatedSeatLevelRightChanged(int))) << QVariant::fromValue(0);
    QTest::newRow("change rearDefrostEnabled property") << QByteArray("rearDefrostEnabled") << QStringLiteral("defrost_all") << QByteArray(SIGNAL(rearDefrostEnabledChanged(bool))) << QVariant::fromValue(true);
}

void tst_Hvac::changeProperty()
{
    QFETCH(QByteArray, propertyName);
    QFETCH(QString, key);
    QFETCH(QByteArray, propertyChangedSignal);
    QFETCH(QVariant, propertyValue);

    initializeSimulator();

    Hvac hvac;

    QSignalSpy hvacSpy(&hvac, propertyChangedSignal.constData());
    QSignalSpy simulatorSpy(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)));

    hvac.setProperty(propertyName, propertyValue);

    QCOMPARE(hvacSpy.count(), 1);
    QCOMPARE(simulatorSpy.count(), 1);
    QCOMPARE(simulatorSpy.at(0).at(1).toString(), key);
    QCOMPARE(simulatorSpy.at(0).at(2).toString(), propertyValue.toString());
}

void tst_Hvac::zoneLinkDisabled()
{
    initializeSimulator();
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1left"), QByteArrayLiteral("defrostAndFeet"), QByteArrayLiteral(""));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1right"), QByteArrayLiteral("defrost"), QByteArrayLiteral(""));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1left"), QByteArrayLiteral("0"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1right"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1left"), QByteArrayLiteral("17"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1right"), QByteArrayLiteral("18"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1left"), QByteArrayLiteral("0"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1right"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("zoneLink_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));

    Hvac hvac;

    QCOMPARE(hvac.property("zoneLinkEnabled").toBool(), false);

    QSignalSpy fanSettingLeftSpy(&hvac, SIGNAL(fanSettingLeftChanged(int)));
    QSignalSpy fanSettingRightSpy(&hvac, SIGNAL(fanSettingRightChanged(int)));
    QSignalSpy fanSpeedLeftSpy(&hvac, SIGNAL(fanSpeedLeftChanged(int)));
    QSignalSpy fanSpeedRightSpy(&hvac, SIGNAL(fanSpeedRightChanged(int)));
    QSignalSpy fanTemperatureLeftSpy(&hvac, SIGNAL(fanTemperatureLeftChanged(int)));
    QSignalSpy fanTemperatureRightSpy(&hvac, SIGNAL(fanTemperatureRightChanged(int)));
    QSignalSpy heatedSeatLevelLeftSpy(&hvac, SIGNAL(heatedSeatLevelLeftChanged(int)));
    QSignalSpy heatedSeatLevelRightSpy(&hvac, SIGNAL(heatedSeatLevelRightChanged(int)));

    // changes to left side do not change right side
    {
        hvac.setProperty("fanSettingLeft", 2);
        QCOMPARE(fanSettingLeftSpy.count(), 1);
        QCOMPARE(fanSettingRightSpy.count(), 0);
        fanSettingLeftSpy.clear();
        fanSettingRightSpy.clear();
        hvac.setProperty("fanSettingRight", 3);
        QCOMPARE(fanSettingLeftSpy.count(), 0);
        QCOMPARE(fanSettingRightSpy.count(), 1);
    }
    {
        hvac.setProperty("fanSpeedLeft", 2);
        QCOMPARE(fanSpeedLeftSpy.count(), 1);
        QCOMPARE(fanSpeedRightSpy.count(), 0);
        fanSpeedLeftSpy.clear();
        fanSpeedRightSpy.clear();
        hvac.setProperty("fanSpeedRight", 3);
        QCOMPARE(fanSpeedLeftSpy.count(), 0);
        QCOMPARE(fanSpeedRightSpy.count(), 1);
    }
    {
        hvac.setProperty("fanTemperatureLeft", 2);
        QCOMPARE(fanTemperatureLeftSpy.count(), 1);
        QCOMPARE(fanTemperatureRightSpy.count(), 0);
        fanTemperatureLeftSpy.clear();
        fanTemperatureRightSpy.clear();
        hvac.setProperty("fanTemperatureRight", 3);
        QCOMPARE(fanTemperatureLeftSpy.count(), 0);
        QCOMPARE(fanTemperatureRightSpy.count(), 1);
    }
    {
        hvac.setProperty("heatedSeatLevelLeft", 2);
        QCOMPARE(heatedSeatLevelLeftSpy.count(), 1);
        QCOMPARE(heatedSeatLevelRightSpy.count(), 0);
        heatedSeatLevelLeftSpy.clear();
        heatedSeatLevelRightSpy.clear();
        hvac.setProperty("heatedSeatLevelRight", 3);
        QCOMPARE(heatedSeatLevelLeftSpy.count(), 0);
        QCOMPARE(heatedSeatLevelRightSpy.count(), 1);
    }
}

void tst_Hvac::zoneLinkEnabled()
{
    initializeSimulator();
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1left"), QByteArrayLiteral("0"), QByteArrayLiteral(""));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1right"), QByteArrayLiteral("1"), QByteArrayLiteral(""));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1left"), QByteArrayLiteral("0"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1right"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1left"), QByteArrayLiteral("17"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1right"), QByteArrayLiteral("18"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1left"), QByteArrayLiteral("0"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1right"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("zoneLink_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));

    Hvac hvac;

    QCOMPARE(hvac.property("zoneLinkEnabled").toBool(), false);

    QSignalSpy fanSettingLeftSpy(&hvac, SIGNAL(fanSettingLeftChanged(int)));
    QSignalSpy fanSettingRightSpy(&hvac, SIGNAL(fanSettingRightChanged(int)));
    QSignalSpy fanSpeedLeftSpy(&hvac, SIGNAL(fanSpeedLeftChanged(int)));
    QSignalSpy fanSpeedRightSpy(&hvac, SIGNAL(fanSpeedRightChanged(int)));
    QSignalSpy fanTemperatureLeftSpy(&hvac, SIGNAL(fanTemperatureLeftChanged(int)));
    QSignalSpy fanTemperatureRightSpy(&hvac, SIGNAL(fanTemperatureRightChanged(int)));
    QSignalSpy heatedSeatLevelLeftSpy(&hvac, SIGNAL(heatedSeatLevelLeftChanged(int)));
    QSignalSpy heatedSeatLevelRightSpy(&hvac, SIGNAL(heatedSeatLevelRightChanged(int)));

    QSignalSpy zoneLinkEnabledSpy(&hvac, SIGNAL(zoneLinkEnabledChanged(bool)));

    hvac.setProperty("zoneLinkEnabled", true);

    QCOMPARE(hvac.property("zoneLinkEnabled").toBool(), true);
    QCOMPARE(zoneLinkEnabledSpy.count(), 1);

    // changes have been propagated from left to right side
    QCOMPARE(fanSettingLeftSpy.count(), 0);
    QCOMPARE(fanSettingRightSpy.count(), 1);
    QCOMPARE(fanSpeedLeftSpy.count(), 0);
    QCOMPARE(fanSpeedRightSpy.count(), 1);
    QCOMPARE(fanTemperatureLeftSpy.count(), 0);
    QCOMPARE(fanTemperatureRightSpy.count(), 1);
    QCOMPARE(heatedSeatLevelLeftSpy.count(), 0);

    //Exception: heated seat level is not synchronized (QCARTWO-4643)
    QCOMPARE(heatedSeatLevelRightSpy.count(), 0);

    fanSettingLeftSpy.clear();
    fanSettingRightSpy.clear();
    fanSpeedLeftSpy.clear();
    fanSpeedRightSpy.clear();
    fanTemperatureLeftSpy.clear();
    fanTemperatureRightSpy.clear();
    heatedSeatLevelLeftSpy.clear();
    heatedSeatLevelRightSpy.clear();

    // changes to left side do not change right side
    {
        hvac.setProperty("fanSettingLeft", 2);
        QCOMPARE(fanSettingLeftSpy.count(), 1);
        QCOMPARE(fanSettingRightSpy.count(), 1);
        fanSettingLeftSpy.clear();
        fanSettingRightSpy.clear();
        hvac.setProperty("fanSettingRight", 3);
        QCOMPARE(fanSettingLeftSpy.count(), 1);
        QCOMPARE(fanSettingRightSpy.count(), 1);
    }
    {
        hvac.setProperty("fanSpeedLeft", 2);
        QCOMPARE(fanSpeedLeftSpy.count(), 1);
        QCOMPARE(fanSpeedRightSpy.count(), 1);
        fanSpeedLeftSpy.clear();
        fanSpeedRightSpy.clear();
        hvac.setProperty("fanSpeedRight", 3);
        QCOMPARE(fanSpeedLeftSpy.count(), 1);
        QCOMPARE(fanSpeedRightSpy.count(), 1);
    }
    {
        hvac.setProperty("fanTemperatureLeft", 19);
        QCOMPARE(fanTemperatureLeftSpy.count(), 1);
        QCOMPARE(fanTemperatureRightSpy.count(), 1);
        fanTemperatureLeftSpy.clear();
        fanTemperatureRightSpy.clear();
        hvac.setProperty("fanTemperatureRight", 20);
        QCOMPARE(fanTemperatureLeftSpy.count(), 1);
        QCOMPARE(fanTemperatureRightSpy.count(), 1);
    }
    {
        //Exception: heated seat level is not synchronized (QCARTWO-4643)
        hvac.setProperty("heatedSeatLevelLeft", 2);
        QCOMPARE(heatedSeatLevelLeftSpy.count(), 1);
        QCOMPARE(heatedSeatLevelRightSpy.count(), 0);
        heatedSeatLevelLeftSpy.clear();
        heatedSeatLevelRightSpy.clear();
        hvac.setProperty("heatedSeatLevelRight", 3);
        QCOMPARE(heatedSeatLevelLeftSpy.count(), 0);
        QCOMPARE(heatedSeatLevelRightSpy.count(), 1);
    }
}

QTEST_MAIN(tst_Hvac)
#include "tst_hvac.moc"
