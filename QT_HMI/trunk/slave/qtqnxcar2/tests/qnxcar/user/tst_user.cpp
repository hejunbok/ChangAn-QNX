#include <QtTest/QtTest>

#include "qnxcar/User.h"

#include <qpps/simulator.h>

using QnxCar::User;

class tst_User : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void ppsInaccessible();
    void ppsAccessible();
    void changeProperty_data();
    void changeProperty();

private:
    void initializeSimulator();
};

void tst_User::initializeSimulator()
{
    QPps::Simulator::self()->reset();
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("id"), QByteArrayLiteral("2"), QByteArrayLiteral("n"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("fullName"), QByteArrayLiteral("Test Person"), QByteArrayLiteral("s"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("avatar"), QByteArrayLiteral("male2"), QByteArrayLiteral("s"));
}

void tst_User::ppsInaccessible()
{
    User user;

    // simulator is not initialized -> PPS path cannot be accessed -> use default value
    QCOMPARE(user.property("id").toInt(), 1);
    QCOMPARE(user.property("fullName").toString(), QLatin1String("Default"));
    QCOMPARE(user.property("avatar").toString(), QLatin1String("male1"));
}

void tst_User::ppsAccessible()
{
    initializeSimulator();

    User user;

    QCOMPARE(user.property("id").toInt(), 2);
    QCOMPARE(user.property("fullName").toString(), QLatin1String("Test Person"));
    QCOMPARE(user.property("avatar").toString(), QLatin1String("male2"));
}

void tst_User::changeProperty_data()
{
    QTest::addColumn<QByteArray>("propertyName");
    QTest::addColumn<QString>("key");
    QTest::addColumn<QByteArray>("propertyChangedSignal");
    QTest::addColumn<QVariant>("propertyValue");

    QTest::newRow("change id property") << QByteArray("id") << QStringLiteral("id") << QByteArray(SIGNAL(idChanged(int))) << QVariant::fromValue(3);
    QTest::newRow("change fullName property") << QByteArray("fullName") << QStringLiteral("fullName") << QByteArray(SIGNAL(fullNameChanged(QString))) << QVariant::fromValue(QStringLiteral("John Doe"));
    QTest::newRow("change avatar property") << QByteArray("avatar") << QStringLiteral("avatar") << QByteArray(SIGNAL(avatarChanged(QString))) << QVariant::fromValue(QStringLiteral("female1"));
}

void tst_User::changeProperty()
{
    QFETCH(QByteArray, propertyName);
    QFETCH(QString, key);
    QFETCH(QByteArray, propertyChangedSignal);
    QFETCH(QVariant, propertyValue);

    initializeSimulator();

    User user;

    QSignalSpy userSpy(&user, propertyChangedSignal.constData());
    QSignalSpy simulatorSpy(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)));

    user.setProperty(propertyName, propertyValue);

    QCOMPARE(userSpy.count(), 1);
    QCOMPARE(simulatorSpy.count(), 1);
    QCOMPARE(simulatorSpy.at(0).at(1).toString(), key);
    QCOMPARE(simulatorSpy.at(0).at(2).toString(), propertyValue.toString());
}

QTEST_MAIN(tst_User)
#include "tst_user.moc"
