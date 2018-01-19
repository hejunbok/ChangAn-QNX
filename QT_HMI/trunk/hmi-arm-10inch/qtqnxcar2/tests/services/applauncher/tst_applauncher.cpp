#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "services/AppLauncher.h"

#include <qpps/object.h>
#include <qpps/simulator.h>
#include <qpps/variant.h>

using QnxCar::AppLauncher;

class tst_AppLauncher : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLaunch();
    void testClose();
};

void tst_AppLauncher::testLaunch()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    AppLauncher launcher;
    const QSignalSpy launchSpy(&launcher, SIGNAL(launchApp(QString,QJsonValue)));

    QPps::Object remoteClient(QStringLiteral("/pps/services/app-launcher"), QPps::Object::PublishMode);
    remoteClient.setAttribute(QStringLiteral("req"), QPps::Variant("{\"cmd\":\"launch app\", \"app\":\"Map\", \"dat\":\"Toronto\"}", "json"));

    QCOMPARE(launchSpy.count(), 1);
    QCOMPARE(launchSpy.at(0).count(), 2);
    QCOMPARE(launchSpy.at(0).at(0).toString(), QStringLiteral("Map"));
    QCOMPARE(launchSpy.at(0).at(1).value<QJsonValue>(), QJsonValue(QStringLiteral("Toronto")));

    const QSignalSpy launchSpy2(&launcher, SIGNAL(launchApp(QString,QJsonValue)));
    remoteClient.setAttribute(QStringLiteral("req"), QPps::Variant("{\"cmd\":\"launch app\", \"app\":\"MediaPlayer\", \"dat\":{\"action\":\"audio\"}}", "json"));

    QCOMPARE(launchSpy2.count(), 1);
    QCOMPARE(launchSpy2.at(0).count(), 2);
    QCOMPARE(launchSpy2.at(0).at(0).toString(), QStringLiteral("MediaPlayer"));
    QJsonObject obj;
    obj.insert(QStringLiteral("action"), QStringLiteral("audio"));
    QCOMPARE(launchSpy2.at(0).at(1).value<QJsonValue>(), QJsonValue(obj));
}

void tst_AppLauncher::testClose()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    AppLauncher launcher;

    QPps::Object remoteClient(QStringLiteral("/pps/services/app-launcher"), QPps::Object::PublishMode);

    const QSignalSpy closeSpy(&launcher, SIGNAL(closeApp(QString)));
    remoteClient.setAttribute(QStringLiteral("req"), QPps::Variant("{\"cmd\":\"close app\", \"app\":\"Map\"}", "json"));

    QCOMPARE(closeSpy.count(), 1);
    QCOMPARE(closeSpy.at(0).count(), 1);
    QCOMPARE(closeSpy.at(0).at(0).toString(), QStringLiteral("Map"));
}

QTEST_MAIN(tst_AppLauncher)
#include "tst_applauncher.moc"
