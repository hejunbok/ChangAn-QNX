#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "qnxcar/Theme.h"

#include <qpps/simulator.h>

#include <QFile>

using QnxCar::Theme;

class tst_Theme : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initial();
    void switchTheme();
    void switchMode();
};

void tst_Theme::initial()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Theme manager;

    QCOMPARE(manager.property("current").toString(), Theme::defaultTheme());
    QCOMPARE(static_cast<Theme::Mode>(manager.property("mode").toInt()), Theme::Day);
}

void tst_Theme::switchTheme()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Theme manager;

    QCOMPARE(manager.property("current").toString(), Theme::defaultTheme());
    QCOMPARE(static_cast<Theme::Mode>(manager.property("mode").toInt()), Theme::Day);

    QSignalSpy spy(&manager, SIGNAL(themeChanged(QString)));

    manager.setProperty("current", Theme::defaultTheme());
    QCOMPARE(spy.count(), 0);
    QCOMPARE(manager.property("current").toString(), Theme::defaultTheme());

    manager.setProperty("current", QStringLiteral("jeep"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager.property("current").toString(), QStringLiteral("jeep"));
}

void tst_Theme::switchMode()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    Theme manager;

    QCOMPARE(manager.property("current").toString(), Theme::defaultTheme());
    QCOMPARE(manager.property("mode").value<Theme::Mode>(), Theme::Day);

    QSignalSpy spy(&manager, SIGNAL(modeChanged(QnxCar::Theme::Mode)));

    manager.setProperty("mode", Theme::Day);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(manager.property("mode").value<Theme::Mode>(), Theme::Day);

    manager.setProperty("mode", Theme::Night);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager.property("mode").value<Theme::Mode>(), Theme::Night);
}

QTEST_MAIN(tst_Theme)
#include "tst_theme.moc"
