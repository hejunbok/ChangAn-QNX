#include "ScreenManager.h"
#include <kanzi/kanzi.h>

ScreenManager::ScreenManager(QObject *parent)
    : QObject(parent)
    , m_currentScreen(HomeScreen)
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
            || screen == CommunicationsScreen)
    {
        system("/opt/apploading.sh stop &");
    }

    emit screenSelected(screen, UserInteractive);

    if (m_currentScreen == screen)
        return;

    m_currentScreen = screen;
    emit currentScreenChanged();
}

void ScreenManager::setKanziScreen(int screen)
{
    KANZI::set_ui((KANZI::idxUI)screen);
}


void ScreenManager::sleep(int second)
{
    sleep(second);
}
