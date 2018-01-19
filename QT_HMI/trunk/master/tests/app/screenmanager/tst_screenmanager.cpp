#include <QtTest/QtTest>

#include "ScreenManager.h"

class tst_ScreenManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initial();
    void changeScreens_data();
    void changeScreens();
};

void tst_ScreenManager::initial()
{
    ScreenManager manager;

    QCOMPARE(static_cast<ScreenManager::Screen>(manager.property("currentScreen").toInt()), ScreenManager::HomeScreen);
}

void tst_ScreenManager::changeScreens_data()
{
    QTest::addColumn<int>("targetScreen");
    QTest::addColumn<int>("changedCount");

    QTest::newRow("switch to home") << static_cast<int>(ScreenManager::HomeScreen) << 0;
    QTest::newRow("switch to navigation") << static_cast<int>(ScreenManager::NavigationScreen) << 1;
    QTest::newRow("switch to mediaplayer") << static_cast<int>(ScreenManager::MediaPlayerScreen) << 1;
    QTest::newRow("switch to settings") << static_cast<int>(ScreenManager::SettingsScreen) << 1;
    QTest::newRow("switch to communications") << static_cast<int>(ScreenManager::CommunicationsScreen) << 1;
    QTest::newRow("switch to applauncher") << static_cast<int>(ScreenManager::AppLauncherScreen) << 1;
}

void tst_ScreenManager::changeScreens()
{
    QFETCH(int, targetScreen);
    QFETCH(int, changedCount);

    ScreenManager manager;

    QSignalSpy spy(&manager, SIGNAL(currentScreenChanged()));

    manager.userSelectScreen(static_cast<ScreenManager::Screen>(targetScreen));

    QCOMPARE(spy.count(), changedCount);

    QCOMPARE(manager.property("currentScreen").toInt(), targetScreen);
}

QTEST_MAIN(tst_ScreenManager)
#include "tst_screenmanager.moc"
