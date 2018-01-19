#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "qnxcar/ApplicationListModel.h"
#include "qnxcar/Launcher.h"

#include <qpps/object.h>
#include <qpps/simulator.h>

#include <QString>

using QnxCar::ApplicationData;
using QnxCar::ApplicationListModel;
using QnxCar::Launcher;

class tst_Launcher : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testFindIdByName();
};

void tst_Launcher::testFindIdByName()
{
    const QString settingsId = QStringLiteral("Settings.testDev_Settings___595d2043");

    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Launcher launcher;
    ApplicationListModel *model = launcher.applicationListModel();
    while (model->canFetchMore(QModelIndex()))
        model->fetchMore(QModelIndex());

    const QModelIndex index25 = model->index(25, 0);
    QCOMPARE(index25.data(ApplicationListModel::AppIdRole).toString(), QStringLiteral("tunneltilt.testDev_tunneltilt_84be6f25"));
    QCOMPARE(index25.data(ApplicationListModel::GroupRole).toString(), QStringLiteral("games"));
    QCOMPARE(index25.data(ApplicationListModel::UriRole).toString(), QString());
    QCOMPARE(index25.data(ApplicationListModel::IconPathRole).toString(), QStringLiteral("/apps/tunneltilt.testDev_tunneltilt_84be6f25/native/data/images/tunnelTiltIcon.png"));

    QCOMPARE(model->index(14, 0).data(ApplicationListModel::AppIdRole).toString(), settingsId);

    const ApplicationData data = model->findByName(QStringLiteral("Settings"));
    QVERIFY(data.isValid());
    QCOMPARE(data.id(), settingsId);

    //test removing an application (uninstall)
    QPps::Object remoteClient(QStringLiteral("/pps/system/navigator/applications/applications"), QPps::Object::PublishMode);
    QVERIFY(remoteClient.isValid());
    QVERIFY(remoteClient.removeAttribute(settingsId));

    QCOMPARE(model->rowCount(), 25);
    QCOMPARE(model->index(14, 0).data(ApplicationListModel::AppIdRole).toString(), QStringLiteral("Shutdown.testDev_Shutdown___f9c26a76"));

    remoteClient.setAttribute(QStringLiteral("HelloKitty_1234"), QPps::Variant("{86x86}native/default-icon.png,Hello Kitty,games,,auto,,", QByteArray()));

    QCOMPARE(model->rowCount(), 26);
    QCOMPARE(model->index(25, 0).data(ApplicationListModel::AppIdRole).toString(), QStringLiteral("HelloKitty_1234"));
}


QTEST_MAIN(tst_Launcher)
#include "tst_launcher.moc"
