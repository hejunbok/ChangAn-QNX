#include <QtTest/QtTest>

#include "qnxcar/Settings.h"

#include <qpps/simulator.h>

// for the signal spy connect of property NOTIFY signals
Q_DECLARE_METATYPE(QnxCar::Settings::Profile);

using QnxCar::Settings;

class tst_Settings : public QObject
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

void tst_Settings::initializeSimulator()
{
    QPps::Simulator::self()->reset();
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("appSection_profile"), QByteArrayLiteral("mid"), QByteArray());
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("carControl_profile"), QByteArrayLiteral("mid"), QByteArray());
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("communication_profile"), QByteArrayLiteral("mid"), QByteArray());
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("mediaPlayer_profile"), QByteArrayLiteral("mid"), QByteArray());
}

void tst_Settings::ppsInaccessible()
{
    Settings settings;

    // simulator is not initialized -> PPS path cannot be accessed -> use default values
    QCOMPARE(static_cast<Settings::Profile>(settings.property("appSectionProfile").toInt()), Settings::High);
    QCOMPARE(static_cast<Settings::Profile>(settings.property("carControlProfile").toInt()), Settings::High);
    QCOMPARE(static_cast<Settings::Profile>(settings.property("communicationProfile").toInt()), Settings::High);
    QCOMPARE(static_cast<Settings::Profile>(settings.property("mediaPlayerProfile").toInt()), Settings::High);
}

void tst_Settings::ppsAccessible()
{
    initializeSimulator();

    Settings settings;

    QCOMPARE(static_cast<Settings::Profile>(settings.property("appSectionProfile").toInt()), Settings::Mid);
    QCOMPARE(static_cast<Settings::Profile>(settings.property("carControlProfile").toInt()), Settings::Mid);
    QCOMPARE(static_cast<Settings::Profile>(settings.property("communicationProfile").toInt()), Settings::Mid);
    QCOMPARE(static_cast<Settings::Profile>(settings.property("mediaPlayerProfile").toInt()), Settings::Mid);
}

void tst_Settings::changeProperty_data()
{
    // for the signal spy connect of property NOTIFY signals
    qRegisterMetaType<QnxCar::Settings::Profile>();

    QTest::addColumn<QByteArray>("propertyName");
    QTest::addColumn<QString>("key");
    QTest::addColumn<QByteArray>("propertyChangedSignal");
    QTest::addColumn<QVariant>("propertyValue");

    QTest::newRow("change appSectionProfile property") << QByteArray("appSectionProfile") << QStringLiteral("appSection_profile") << QByteArray(SIGNAL(appSectionProfileChanged(QnxCar::Settings::Profile))) << QVariant::fromValue(static_cast<int>(Settings::High));
    QTest::newRow("change carControlProfile property") << QByteArray("carControlProfile") << QStringLiteral("carControl_profile") << QByteArray(SIGNAL(carControlProfileChanged(QnxCar::Settings::Profile))) << QVariant::fromValue(static_cast<int>(Settings::High));
    QTest::newRow("change communicationProfile property") << QByteArray("communicationProfile") << QStringLiteral("communication_profile") << QByteArray(SIGNAL(communicationProfileChanged(QnxCar::Settings::Profile))) << QVariant::fromValue(static_cast<int>(Settings::High));
    QTest::newRow("change mediaPlayerProfile property") << QByteArray("mediaPlayerProfile") << QStringLiteral("mediaPlayer_profile") << QByteArray(SIGNAL(mediaPlayerProfileChanged(QnxCar::Settings::Profile))) << QVariant::fromValue(static_cast<int>(Settings::High));
}

void tst_Settings::changeProperty()
{
    QFETCH(QByteArray, propertyName);
    QFETCH(QString, key);
    QFETCH(QByteArray, propertyChangedSignal);
    QFETCH(QVariant, propertyValue);

    initializeSimulator();

    Settings settings;

    QSignalSpy settingsSpy(&settings, propertyChangedSignal.constData());
    QSignalSpy simulatorSpy(QPps::Simulator::self(), SIGNAL(attributeChanged(QString,QString,QByteArray,QByteArray)));

    settings.setProperty(propertyName, propertyValue);

    QCOMPARE(settingsSpy.count(), 1);
    QCOMPARE(simulatorSpy.count(), 1);
    QCOMPARE(simulatorSpy.at(0).at(1).toString(), key);
}

QTEST_MAIN(tst_Settings)
#include "tst_settings.moc"
