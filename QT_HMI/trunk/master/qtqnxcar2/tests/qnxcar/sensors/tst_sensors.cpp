#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "qnxcar/Sensors.h"

#include <qpps/simulator.h>

using QnxCar::Sensors;

class tst_Sensors : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void ppsAccessible();
    void changeProperty_data();
    void changeProperty();
    void testAbsSwitch();
};

void tst_Sensors::ppsAccessible()
{
    QnxCar2Simulator::initDemoData();

    Sensors sensors;

    QCOMPARE(sensors.property("brakeFluidLevel").toInt(), 90);
    QCOMPARE(sensors.property("brakeAbs").toBool(), true);

    QCOMPARE(sensors.property("brakeAbsFrontLeft").toBool(), true);
    QCOMPARE(sensors.property("brakeAbsFrontRight").toBool(), true);
    QCOMPARE(sensors.property("brakeAbsRearLeft").toBool(), true);
    QCOMPARE(sensors.property("brakeAbsRearRight").toBool(), true);

    QCOMPARE(sensors.property("brakeWearFrontLeft").toInt(), 100);
    QCOMPARE(sensors.property("brakeWearFrontRight").toInt(), 100);
    QCOMPARE(sensors.property("brakeWearRearLeft").toInt(), 50);
    QCOMPARE(sensors.property("brakeWearRearRight").toInt(), 65);

    QCOMPARE(sensors.property("engineOilLevel").toInt(), 95);
    QCOMPARE(sensors.property("engineCoolantLevel").toInt(), 100);
    QCOMPARE(sensors.property("engineOilPressure").toInt(), 100);
    QCOMPARE(sensors.property("engineRpm").toInt(), 2800);

    QCOMPARE(sensors.property("fuelLevel").toInt(), 75);

    QCOMPARE(sensors.property("lightHeadFrontLeft").toBool(), true);
    QCOMPARE(sensors.property("lightHeadFrontRight").toBool(), true);
    QCOMPARE(sensors.property("lightTailRearLeft").toBool(), false);
    QCOMPARE(sensors.property("lightTailRearRight").toBool(), true);

    QCOMPARE(sensors.property("speed").toInt(), 0);

    QCOMPARE(sensors.property("tirePressureFrontLeft").toInt(), 31);
    QCOMPARE(sensors.property("tirePressureFrontRight").toInt(), 31);
    QCOMPARE(sensors.property("tirePressureRearLeft").toInt(), 25);
    QCOMPARE(sensors.property("tirePressureRearRight").toInt(), 32);

    QCOMPARE(sensors.property("tireWearFrontLeft").toInt(), 90);
    QCOMPARE(sensors.property("tireWearFrontRight").toInt(), 90);
    QCOMPARE(sensors.property("tireWearRearLeft").toInt(), 70);
    QCOMPARE(sensors.property("tireWearRearRight").toInt(), 70);

    QCOMPARE(sensors.property("transmissionFluidLevel").toInt(), 85);
    QCOMPARE(sensors.property("transmissionClutchWear").toInt(), 70);
    QCOMPARE(sensors.property("transmissionGear").toString(), QStringLiteral("n"));
    QCOMPARE(sensors.property("transmissionTemperature").toInt(), 185);

    QCOMPARE(sensors.property("washerFluidLevel").toInt(), 20);
}

void tst_Sensors::changeProperty_data()
{
    QTest::addColumn<QByteArray>("propertyName");
    QTest::addColumn<QString>("key");
    QTest::addColumn<QByteArray>("propertyType");
    QTest::addColumn<QByteArray>("propertyChangedSignal");
    QTest::addColumn<QVariant>("propertyValue");

    QTest::newRow("change brakeFluidLevel property") << QByteArray("brakeFluidLevel") << QStringLiteral("brakeFluidLevel") << QByteArray("n") << QByteArray(SIGNAL(brakeFluidLevelChanged(int))) << QVariant::fromValue(45);

    QTest::newRow("change brakeAbsFrontLeft property") << QByteArray("brakeAbsFrontLeft") << QStringLiteral("brakeAbsFrontLeft") << QByteArray("b") << QByteArray(SIGNAL(brakeAbsFrontLeftChanged(bool))) << QVariant::fromValue(false);
    QTest::newRow("change brakeAbsFrontRight property") << QByteArray("brakeAbsFrontRight") << QStringLiteral("brakeAbsFrontRight") << QByteArray("b") << QByteArray(SIGNAL(brakeAbsFrontRightChanged(bool))) <<QVariant::fromValue(false);
    QTest::newRow("change brakeAbsRearLeft property") << QByteArray("brakeAbsRearLeft") << QStringLiteral("brakeAbsRearLeft") << QByteArray("b") << QByteArray(SIGNAL(brakeAbsRearLeftChanged(bool))) << QVariant::fromValue(false);
    QTest::newRow("change brakeAbsRearRight property") << QByteArray("brakeAbsRearRight") << QStringLiteral("brakeAbsRearRight") << QByteArray("b") << QByteArray(SIGNAL(brakeAbsRearRightChanged(bool))) << QVariant::fromValue(false);

    QTest::newRow("change brakePadWearFrontLeft property") << QByteArray("brakeWearFrontLeft") << QStringLiteral("brakePadWearFrontLeft") << QByteArray("n") << QByteArray(SIGNAL(brakeWearFrontLeftChanged(int))) <<QVariant::fromValue(30);
    QTest::newRow("change brakePadWearFrontRight property") << QByteArray("brakeWearFrontRight") << QStringLiteral("brakePadWearFrontRight") << QByteArray("n") << QByteArray(SIGNAL(brakeWearFrontRightChanged(int))) << QVariant::fromValue(30);
    QTest::newRow("change brakePadWearRearLeft property") << QByteArray("brakeWearRearLeft") << QStringLiteral("brakePadWearRearLeft") << QByteArray("n") << QByteArray(SIGNAL(brakeWearRearLeftChanged(int))) << QVariant::fromValue(42);
    QTest::newRow("change brakePadWearRearRight property") << QByteArray("brakeWearRearRight") << QStringLiteral("brakePadWearRearRight") << QByteArray("n") << QByteArray(SIGNAL(brakeWearRearRightChanged(int))) << QVariant::fromValue(42);

    QTest::newRow("change engineOilLevel property") << QByteArray("engineOilLevel") << QStringLiteral("engineOilLevel") << QByteArray("n") << QByteArray(SIGNAL(engineOilLevelChanged(int))) << QVariant::fromValue(45);
    QTest::newRow("change coolantLevel property") << QByteArray("engineCoolantLevel") << QStringLiteral("coolantLevel") << QByteArray("n") << QByteArray(SIGNAL(engineCoolantLevelChanged(int))) << QVariant::fromValue(45);
    QTest::newRow("change engineOilPressure property") << QByteArray("engineOilPressure") << QStringLiteral("engineOilPressure") << QByteArray("n") << QByteArray(SIGNAL(engineOilPressureChanged(int))) << QVariant::fromValue(45);
    QTest::newRow("change rpm property") << QByteArray("engineRpm") << QStringLiteral("rpm") << QByteArray("n") << QByteArray(SIGNAL(engineRpmChanged(int))) << QVariant::fromValue(2000);

    QTest::newRow("change fuelLevel property") << QByteArray("fuelLevel") << QStringLiteral("fuelLevel") << QByteArray("n") << QByteArray(SIGNAL(fuelLevelChanged(int))) << QVariant::fromValue(60);

    QTest::newRow("change lightHeadLeft property") << QByteArray("lightHeadFrontLeft") << QStringLiteral("lightHeadLeft") << QByteArray("b") << QByteArray(SIGNAL(lightHeadFrontLeftChanged(bool))) <<QVariant::fromValue(false);
    QTest::newRow("change lightHeadRight property") << QByteArray("lightHeadFrontRight") << QStringLiteral("lightHeadRight") << QByteArray("b") << QByteArray(SIGNAL(lightHeadFrontRightChanged(bool))) <<QVariant::fromValue(false);
    QTest::newRow("change lightTailLeft property") <<    QByteArray("lightTailRearLeft") << QStringLiteral("lightTailLeft") << QByteArray("b") << QByteArray(SIGNAL(lightTailRearLeftChanged(bool))) <<QVariant::fromValue(true);
    QTest::newRow("change lightTailRight property") << QByteArray("lightTailRearRight") << QStringLiteral("lightTailRight") << QByteArray("b") << QByteArray(SIGNAL(lightTailRearRightChanged(bool))) << QVariant::fromValue(false);

    QTest::newRow("change speed property") << QByteArray("speed") << QStringLiteral("speed") << QByteArray("n") << QByteArray(SIGNAL(speedChanged(int))) << QVariant::fromValue(110);

    QTest::newRow("change tirePressureFrontLeft property") << QByteArray("tirePressureFrontLeft") << QStringLiteral("tirePressureFrontLeft") << QByteArray("n") <<  QByteArray(SIGNAL(tirePressureFrontLeftChanged(int))) << QVariant::fromValue(100);
    QTest::newRow("change tirePressureFrontRight property") << QByteArray("tirePressureFrontRight") << QStringLiteral("tirePressureFrontRight") << QByteArray("n") << QByteArray(SIGNAL(tirePressureFrontRightChanged(int))) << QVariant::fromValue(100);
    QTest::newRow("change tirePressureRearLeft property") << QByteArray("tirePressureRearLeft") << QStringLiteral("tirePressureRearLeft") << QByteArray("n") << QByteArray(SIGNAL(tirePressureRearLeftChanged(int))) << QVariant::fromValue(111);
    QTest::newRow("change tirePressureRearRight property") << QByteArray("tirePressureRearRight") << QStringLiteral("tirePressureRearRight") << QByteArray("n") << QByteArray(SIGNAL(tirePressureRearRightChanged(int))) << QVariant::fromValue(100);

    QTest::newRow("change tireWearFrontLeft property") << QByteArray("tireWearFrontLeft") << QStringLiteral("tireWearFrontLeft") << QByteArray("n") << QByteArray(SIGNAL(tireWearFrontLeftChanged(int))) << QVariant::fromValue(50);
    QTest::newRow("change tireWearFrontRight property") << QByteArray("tireWearFrontRight") << QStringLiteral("tireWearFrontRight") << QByteArray("n") << QByteArray(SIGNAL(tireWearFrontRightChanged(int))) <<QVariant::fromValue(50);
    QTest::newRow("change tireWearRearLeft property") << QByteArray("tireWearRearLeft") << QStringLiteral("tireWearRearLeft") << QByteArray("n") << QByteArray(SIGNAL(tireWearRearLeftChanged(int))) << QVariant::fromValue(50);
    QTest::newRow("change tireWearRearRight property") << QByteArray("tireWearRearRight") << QStringLiteral("tireWearRearRight") << QByteArray("n") << QByteArray(SIGNAL(tireWearRearRightChanged(int))) << QVariant::fromValue(0);

    QTest::newRow("change transmissionFluidLevel property") << QByteArray("transmissionFluidLevel") << QStringLiteral("transmissionFluidLevel") << QByteArray("n") << QByteArray(SIGNAL(transmissionFluidLevelChanged(int))) <<QVariant::fromValue(50);
    QTest::newRow("change transmissionClutchWear property") << QByteArray("transmissionClutchWear") << QStringLiteral("transmissionClutchWear") << QByteArray("n") << QByteArray(SIGNAL(transmissionClutchWearChanged(int))) << QVariant::fromValue(50);
    QTest::newRow("change transmissionGear property") << QByteArray("transmissionGear") << QStringLiteral("transmissionGear") << QByteArray("s") << QByteArray(SIGNAL(transmissionGearChanged(QString))) << QVariant::fromValue(QStringLiteral("7"));
    QTest::newRow("change transmissionFluidTemperature property") << QByteArray("transmissionTemperature") << QStringLiteral("transmissionFluidTemperature") << QByteArray("n") << QByteArray(SIGNAL(transmissionTemperatureChanged(int))) << QVariant::fromValue(90);

    QTest::newRow("change washerFluidLevel property") << QByteArray("washerFluidLevel") << QStringLiteral("washerFluidLevel") << QByteArray("n") << QByteArray(SIGNAL(washerFluidLevelChanged(int))) << QVariant::fromValue(25);
}

void tst_Sensors::changeProperty()
{
    QFETCH(QByteArray, propertyName);
    QFETCH(QString, key);
    QFETCH(QByteArray, propertyType);
    QFETCH(QByteArray, propertyChangedSignal);
    QFETCH(QVariant, propertyValue);

    Sensors sensor;
    QPps::Simulator::self()->registerClient(QStringLiteral("/pps/qnxcar/sensors"), &sensor);

    // check that the property is read only, only one should be writeable. (brake_abs)
    QByteArray msg = QString(QStringLiteral("QnxCar::Sensors::setProperty: Property \"%1\" invalid, read-only or does not exist")).arg(QLatin1String(propertyName)).toLatin1();

#ifdef QT_DEBUG // the warnings are generated only if the project is compiled in DEBUG mode.
    QTest::ignoreMessage(QtWarningMsg, msg.constData());
    if (propertyName != QByteArray("brakeAbs")) {
        QVERIFY(!sensor.setProperty(propertyName, propertyValue)); //check that property is read only
    } else {
        QVERIFY(sensor.setProperty(propertyName, propertyValue)); //check that property is read write
    }
#endif

    // modify attributes
    QSignalSpy sensorSpy(&sensor, propertyChangedSignal.constData());

    QSignalSpy simulatorSpy(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)));
    QPps::Simulator::self()->clientSetAttribute(&sensor, key, propertyValue.toByteArray(), propertyType);

    QCOMPARE(sensorSpy.count(), 1);
    QCOMPARE(QVariant(sensorSpy.at(0).at(0)), propertyValue);
    QCOMPARE(simulatorSpy.count(), 1);
    QCOMPARE(simulatorSpy.at(0).at(1).toString(), key);
    QCOMPARE(simulatorSpy.at(0).at(2).toString(), propertyValue.toString());
    QCOMPARE(sensor.property(propertyName), propertyValue);

    QPps::Simulator::self()->unregisterClient(&sensor);

}

void tst_Sensors::testAbsSwitch()
{
    QnxCar2Simulator::initDemoData();
    Sensors sensor;
    QSignalSpy sensorSpy(&sensor, SIGNAL(brakeAbsChanged(bool)));

    sensor.setProperty(QByteArray("brakeAbs"), QVariant::fromValue(false));

    QCOMPARE(sensor.property(QByteArray("brakeAbs")), QVariant::fromValue(false));
    QCOMPARE(sensorSpy.count(), 1);
    QCOMPARE(QVariant(sensorSpy.at(0).at(0)), QVariant::fromValue(false));
}

QTEST_MAIN(tst_Sensors)
#include "tst_sensors.moc"
