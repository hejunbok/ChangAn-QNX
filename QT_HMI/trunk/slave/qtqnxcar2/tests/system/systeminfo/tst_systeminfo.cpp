#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "system/SystemInfo.h"

#include <qpps/simulator.h>

#include <QString>

using QnxCar::SystemInfo;

class tst_SystemInfo : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testSystemInfo();
};

void tst_SystemInfo::testSystemInfo()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    SystemInfo systemInfo;

    QVERIFY(systemInfo.buildNumber() == QStringLiteral("6467"));
    QVERIFY(systemInfo.revision() == QStringLiteral("6106"));
    QVERIFY(systemInfo.buildDate() == QStringLiteral("2013.04.25-12:10:16"));
    QVERIFY(!systemInfo.updateAvailable());
}


QTEST_MAIN(tst_SystemInfo)
#include "tst_systeminfo.moc"
