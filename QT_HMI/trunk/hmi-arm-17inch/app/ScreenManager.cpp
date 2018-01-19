#include "ScreenManager.h"
#include <kanzi/kanzi.h>
#include <iostream>
#include <QString>

ScreenManager::ScreenManager(QObject *parent)
    : QObject(parent)
    , m_currentScreen(HomeScreen)
    , m_navi_pid(0)
    , m_cameracontrol(NULL)
    , m_appRunning(false)
{
}

ScreenManager::Screen ScreenManager::currentScreen() const
{
    return m_currentScreen;
}

void ScreenManager::setCurrentScreen(Screen screen)
{
    emit screenSelected(screen, Programmatic);

    if (m_currentScreen == screen)
        return;

    m_currentScreen = screen;
    emit currentScreenChanged();
}

void ScreenManager::userSelectScreen(Screen screen)
{
    if (screen == AppLauncherScreen
            || screen == MediaPlayerScreen
            || screen == SettingsScreen
            || screen == CommunicationsScreen
            || screen == NavigationScreen)
    {
        if (m_appRunning)
        {
            m_appRunning = false;
            system("/opt/apploading.sh stop &");
        }
    }

    if (m_cameracontrol)
    {
        m_cameracontrol->setCapture(QStringLiteral("stop"));
    }

    emit screenSelected(screen, UserInteractive);

    if (m_currentScreen == screen)
        return;

    m_currentScreen = screen;
    emit currentScreenChanged();
}

void ScreenManager::setKanziScreen(int screen)
{
    static int s_screen = -1;
    if (s_screen == screen)
    {
        return;
    }

    s_screen == screen;

    KANZI::set_ui((KANZI::idxUI)screen);
}

void ScreenManager::setCluster(int screen)
{
    static int s_screen = -1;
    if (s_screen == screen)
    {
        return;
    }

    s_screen = screen;

    QString cmd = tr("echo set_cluster::%1 >> /pps/screen &").arg(screen);
    system(cmd.toLatin1().data());
}

void ScreenManager::sleep(int second)
{
    sleep(second);
}

void ScreenManager::msleep(int msecond)
{
   msleep(msecond);
}

Q_PID ScreenManager::retrieveNaviPid() const
{
#ifdef CAR2_HOST_BUILD
    return 0;
#else
    const QString program = QStringLiteral("pidin");
    QStringList arguments;
    arguments << QStringLiteral("-P") << QStringLiteral("Navi");
    arguments << QStringLiteral("-F") << QStringLiteral("%a");

    QProcess process;
    process.start(program, arguments);
    bool success = process.waitForFinished(1000);
    if (!success)
        return 0;
    const QByteArray result = process.readAllStandardOutput();
    const QList<QByteArray> lines = result.split('\n');
    // lines should look like: ("     pid", " 1679413", "")
    if (lines.size() != 3)
    {
        return 0;
    }
    return lines[1].toInt();
#endif
}

void ScreenManager::setNaviZOrder(int zOrder)
{
    static int s_zOrder = -2; //not use this order -2

    if (s_zOrder == zOrder)
    {
        return;
    }

    s_zOrder = zOrder;

    if(!m_navi_pid || m_navi_pid > 10000000)
    {
        m_navi_pid = retrieveNaviPid();
    }
    QString cmd = tr("/opt/setzorder_pid.sh %1 %2 &").arg(m_navi_pid).arg(zOrder);
    system(cmd.toLatin1().data());
}


void ScreenManager::setCameraControl(QnxCar::CameraControl *cameracontrol)
{
    m_cameracontrol = cameracontrol;
}


void ScreenManager::setAppRunning(bool running)
{
    m_appRunning = running;
}
