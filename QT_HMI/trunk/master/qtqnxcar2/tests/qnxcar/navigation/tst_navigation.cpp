#include <QtTest/QtTest>

#include "qnxcar/Maneuver_p.h"
#include "qnxcar/ManeuversModel.h"
#include "qnxcar/Navigation.h"

#include <qpps/simulator.h>

using QnxCar::Location;
using QnxCar::Maneuver;
using QnxCar::ManeuversModel;
using QnxCar::Navigation;

class tst_Navigation : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLocation();
    void testManeuversModel();
    void testNavigation();

private:
    void initializeSimulator();
};

void tst_Navigation::initializeSimulator()
{
    QPps::Simulator::self()->reset();
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/navigation/status"), QStringLiteral("navigating"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/navigation/status"), QStringLiteral("total_distance_remaining"), QByteArrayLiteral("3000"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/navigation/status"), QStringLiteral("total_time_remaining"), QByteArrayLiteral("180"), QByteArrayLiteral("n"));
    const QByteArray maneuvers = "[{\"street\":\"Some Street\", \"command\":\"nc\", \"distance\":1000}, {\"street\":\"Some Other Street\", \"command\":\"lht-rx\", \"distance\":1500}, {\"street\":\"Home Street\", \"command\":\"dt\", \"distance\":3000}]";
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/navigation/status"), QStringLiteral("maneuvers"), maneuvers, QByteArrayLiteral("json"));
    const QByteArray destination = "{\"city\":\"Ottawa\", \"country\":\"Canada\", \"distance\":3000, \"id\":123, \"latitude\":50.123, \"longitude\":60.123, \"name\":\"Foobar\", \"number\":\"42\", \"postalCode\":\"12345\", \"province\":\"Ontario\", \"street\":\"Home Street\", \"type\":\"accomodation\"}";
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/navigation/status"), QStringLiteral("destination"), destination, QByteArrayLiteral("json"));
}

void tst_Navigation::testLocation()
{
    //Test invalid locations
    const Location invalid1;
    const Location invalid2;
    QVERIFY(!invalid1.isValid());
    QCOMPARE(invalid1.id(), -1);
    QCOMPARE(invalid1.type(), QString());
    QCOMPARE(invalid1.distance(), 0);
    QCOMPARE(invalid1.latitude(), static_cast<qreal>(0));
    QCOMPARE(invalid1.longitude(), static_cast<qreal>(0));
    QCOMPARE(invalid1, invalid2);

    //Test setters and getters
    Location l2;
    l2.setId(123);
    QCOMPARE(l2.id(), 123);
    l2.setName(QStringLiteral("Name"));
    QCOMPARE(l2.name(), QStringLiteral("Name"));
    l2.setType(QStringLiteral("accomodation"));
    QCOMPARE(l2.type(), QStringLiteral("accomodation"));
    l2.setStreet(QStringLiteral("Some street"));
    QCOMPARE(l2.street(), QStringLiteral("Some street"));
    l2.setStreetNumber(QStringLiteral("42"));
    QCOMPARE(l2.streetNumber(), QStringLiteral("42"));
    l2.setCity(QStringLiteral("City"));
    QCOMPARE(l2.city(), QStringLiteral("City"));
    l2.setPostalCode(QStringLiteral("12345"));
    QCOMPARE(l2.postalCode(), QStringLiteral("12345"));
    l2.setProvince(QStringLiteral("Provence"));
    QCOMPARE(l2.province(), QStringLiteral("Provence"));
    l2.setCountry(QStringLiteral("Country"));
    QCOMPARE(l2.country(), QStringLiteral("Country"));
    l2.setDistance(500);
    QCOMPARE(l2.distance(), 500);
    l2.setLatitude(50.1);
    QCOMPARE(l2.latitude(), 50.1);
    l2.setLongitude(60.1);
    QCOMPARE(l2.longitude(), 60.1);

    const Location l2copy(l2);
    QCOMPARE(l2copy, l2);
    QVERIFY(l2copy.isValid());
}

void tst_Navigation::testManeuversModel()
{
    ManeuversModel model;
    QCOMPARE(model.rowCount(), 0);

    {
        QVector<Maneuver> ms;
        Maneuver m1;
        m1.street = QStringLiteral("Some Street");
        m1.command = ManeuversModel::TurnLeft;
        m1.distance = 500;
        Maneuver m2;
        m2.street = QStringLiteral("Some Other Street");
        m2.command = ManeuversModel::UTurnOnTheLeft;
        m2.distance = 1500;
        model.setManeuvers(QVector<Maneuver>() << m1 << m2);
    }

    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.index(0, 0).data(ManeuversModel::StreetRole).toString(), QStringLiteral("Some Street"));
    QCOMPARE(static_cast<ManeuversModel::Command>(model.index(0, 0).data(ManeuversModel::CommandRole).toInt()), ManeuversModel::TurnLeft);
    QCOMPARE(model.index(0, 0).data(ManeuversModel::DistanceRole).toInt(), 500);
    QCOMPARE(model.index(1, 0).data(ManeuversModel::StreetRole).toString(), QStringLiteral("Some Other Street"));
    QCOMPARE(static_cast<ManeuversModel::Command>(model.index(1, 0).data(ManeuversModel::CommandRole).toInt()), ManeuversModel::UTurnOnTheLeft);
    QCOMPARE(model.index(1, 0).data(ManeuversModel::DistanceRole).toInt(), 1500);
}

void tst_Navigation::testNavigation()
{
    { // Test disconnected navigation
        Navigation navigation;
        ManeuversModel *maneuversModel = navigation.maneuversModel();
        QVERIFY(maneuversModel);

        QCOMPARE(maneuversModel->rowCount(), 0);
        QCOMPARE(navigation.navigating(), false);
        QCOMPARE(navigation.totalDistanceRemaining(), 0);
        QCOMPARE(navigation.totalTimeRemaining(), 0);
        QCOMPARE(navigation.destination(), Location());
    }

    initializeSimulator();

    {
        Navigation navigation;
        ManeuversModel *maneuversModel = navigation.maneuversModel();
        QVERIFY(maneuversModel);

        QCOMPARE(navigation.navigating(), true);
        QCOMPARE(navigation.totalDistanceRemaining(), 3000);
        QCOMPARE(navigation.totalTimeRemaining(), 180);
        const Location destination = navigation.destination();
        QVERIFY(destination.isValid());
        QCOMPARE(destination.id(), 123);
        QCOMPARE(destination.name(), QStringLiteral("Foobar"));
        QCOMPARE(destination.type(), QStringLiteral("accomodation"));
        QCOMPARE(destination.street(), QStringLiteral("Home Street"));
        QCOMPARE(destination.streetNumber(), QStringLiteral("42"));
        QCOMPARE(destination.city(), QStringLiteral("Ottawa"));
        QCOMPARE(destination.postalCode(), QStringLiteral("12345"));
        QCOMPARE(destination.province(), QStringLiteral("Ontario"));
        QCOMPARE(destination.country(), QStringLiteral("Canada"));
        QCOMPARE(destination.distance(), 3000);
        QCOMPARE(destination.latitude(), static_cast<qreal>(50.123));
        QCOMPARE(destination.longitude(), static_cast<qreal>(60.123));

        QCOMPARE(maneuversModel->rowCount(), 3);
        QCOMPARE(maneuversModel->index(0, 0).data(ManeuversModel::StreetRole).toString(), QStringLiteral("Some Street"));
        QCOMPARE(static_cast<ManeuversModel::Command>(maneuversModel->index(0, 0).data(ManeuversModel::CommandRole).toInt()), ManeuversModel::NoChange);
        QCOMPARE(maneuversModel->index(0, 0).data(ManeuversModel::DistanceRole).toInt(), 1000);
        QCOMPARE(maneuversModel->index(1, 0).data(ManeuversModel::StreetRole).toString(), QStringLiteral("Some Other Street"));
        QCOMPARE(static_cast<ManeuversModel::Command>(maneuversModel->index(1, 0).data(ManeuversModel::CommandRole).toInt()), ManeuversModel::FollowRoundaboutOnTheLeft);
        QCOMPARE(maneuversModel->index(1, 0).data(ManeuversModel::DistanceRole).toInt(), 1500);
        QCOMPARE(maneuversModel->index(2, 0).data(ManeuversModel::StreetRole).toString(), QStringLiteral("Home Street"));
        QCOMPARE(static_cast<ManeuversModel::Command>(maneuversModel->index(2, 0).data(ManeuversModel::CommandRole).toInt()), ManeuversModel::ArrivedAtDestination);
        QCOMPARE(maneuversModel->index(2, 0).data(ManeuversModel::DistanceRole).toInt(), 3000);
    }
}


QTEST_MAIN(tst_Navigation)
#include "tst_navigation.moc"
