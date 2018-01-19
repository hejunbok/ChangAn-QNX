#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QObject>

/**
 * @short The class that coordinates the logic of switching screens
 */
class ScreenManager : public QObject
{
    Q_OBJECT

    // The currently visible screen that is selected in the navigator bar
    Q_PROPERTY(ScreenManager::Screen currentScreen READ currentScreen NOTIFY currentScreenChanged)

    Q_ENUMS(Screen)

public:
    // Describes the possible screens
    enum Screen
    {
        HomeScreen,
        NavigationScreen,
        MediaPlayerScreen,
        SettingsScreen,
        CommunicationsScreen,
        AppLauncherScreen
    };

    enum SelectionReason {
        UserInteractive, ///< The screen switch was triggered by manual user interaction
        Programmatic ///< The screen switch was triggered programmatically (e.g. by ASR)
    };

    /**
     * Creates a new ScreenManager object.
     *
     * @param parent The parent object.
     */
    explicit ScreenManager(QObject *parent = 0);

    Screen currentScreen() const;

public Q_SLOTS:
    /**
     * Sets the current screen, for user-interactive screen changes.
     */
    void userSelectScreen(Screen currentScreen);

    /**
     * Sets the kanzi screen.
     */
    void setKanziScreen(int screen);

    /**
     * Sets the current screen, for programmatic screen changes.
     */
    void setCurrentScreen(Screen currentScreen);

    /**
     * ivoke sleep.
     */
    void sleep(int second);

Q_SIGNALS:
    /**
     * This signal is emitted whenever the currentScreen property has changed.
     */
    void currentScreenChanged();

    /**
     * This signal is emitted whenever the current screen was either
     * switched interactively by the user or by ASR
     *
     * @param screen the screen that was switched to
     * @param reason whether the switch was triggered programmatically (e.g. ASR) or by the user.
     */
    void screenSelected(Screen screen, SelectionReason reason);

private:
    Screen m_currentScreen;
    Screen m_lastScreen;
};

#endif
