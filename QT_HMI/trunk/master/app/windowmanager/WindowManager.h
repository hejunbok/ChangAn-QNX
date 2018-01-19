#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QMutex>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QVector>
#include <QWindow>

class QRect;

#ifdef Q_OS_WIN
#include <Windows.h> // for Q_PID as meta-type
#endif

class WindowManager : public QObject
{
    Q_OBJECT

public:
    explicit WindowManager(QObject *parent = 0);
    ~WindowManager();

    Q_INVOKABLE void run();

    void setZValueForWindow(Q_PID pid, WId window, int zvalue);
    void setZValue(Q_PID pid, int zvalue);
    void setZValue(const QString &windowGroup, int zvalue);
    void setAlignment(Q_PID pid, Qt::Alignment alignment);
    void setClipPosition(Q_PID pid, const QPoint &position);
    void setClipSize(Q_PID pid, const QSize &size);

    /**
     * Sets size of the clipping region for source window
     * @param pid - process id of the window
     * @param size - object defining size of the window
     */
    void setSourceClipSize(Q_PID pid, const QSize &size);
    void setPosition(Q_PID pid, const QPoint &position);
    void setX(Q_PID, int x);
    void setY(Q_PID, int y);
    void setSize(Q_PID pid, const QSize &size);

    /**
     * Sets size of the source window
     * @param pid - process id of the window
     * @param size - object defining size of the window
     */
    void setSourceSize(Q_PID pid, const QSize &size);
    void setVisible(Q_PID pid, bool visible);

    /**
     * @brief set geometry for the specified window
     * Sets geometry (width, height, position) for specified window by PID in one show
     * @param pid Process id of the window
     * @param geometry object representing geometry (width, height, x, y)
     */
    void setGeometry(Q_PID pid, const QRect &geometry);

    enum TransparencyMode {
        Source,
        Test,
        SourceColor,
        SourceOver,
        NoTransparency,
        Discard
    };

    /**
     * Enmeration defining sensitivity masks for screen
     * for more details see <a href="http://www.qnx.com/developers/docs/qnxcar2/index.jsp?topic=%2Fcom.qnx.doc.qnxcar2.screen%2Ftopic%2Fscreen_8h_1Screen_Property_Types.html"></a>
     */
    enum Sensitivities{
         /**
            Pointer and touch events are always forwarded to the window's context if they interect with the window - regardless of transparency.
            The window receives keyboard, gamepad, joystick events if it has input focus. Raising a window, pointer or multi-touch release event in that window will cause it to acquire input focus.
          */
         Always = (1 << 0),
         /**
            The window never receives pointer or multi-touch events.
            The window never acquires input focus, even after it has been raised. The window will only receive input events that are directly injected into it from outside sources.
         */
         Never = (2 << 0),
         /**
            Pointer and touch events are forwarded to the window's context if they intersect the window and are in an area of the window that is not fully transparent.
            The window does not acquire input focus after being raised or after a pointer or multi-touch release event occurs. Therefore, the window will not receive keyboard, gamepad, or joystick input unless it is sent directly into the window from an outside source.
         */
         NoFocus = (1 << 3),
        /**
            Pointer and touch events are forwarded to the window's context no matter where they are on the screen.
            The window is considered full screen for the purposes of input hit tests. Transparency is ignored. The window will receive keyboard, gamepad, and joystick events as long as the window is visible.
         */
         Fullscreen = (1 << 4),
        /**
            Windows underneath this window can receive pointer or multi-touch events even if this window has input focus.         */
         Continue = (1 << 5),
        /**
            The window never receives pointer or multi-touch events.
            The window never acquires input focus, even after it has been raised. The window will only receive input events that are directly injected into it from outside sources.
         */
         Stop = (2 << 5),
        /**
            The window receives pointer events, even in areas of transparency, if the source coordinates of the event are within the brush clip rectangle.
            This mode supercedes SCREEN_SENSITIVITY_MASK_NEVER. The windowing system also draws brush strokes based on the pointer events directly onto the screen and the window buffer.
         */
         PinterBrush = (1 << 7),
         /**
            The window receives multi-touch events with a finger contact type, even in areas of transparency, if the source coordinates of the event are within the brush clip rectangle.
            This mode supercedes SCREEN_SENSITIVITY_MASK_NEVER. The windowing system also draws brush strokes based on the touch events directly onto the screen and the window buffer. Multiple contacts will cause multiple brush strokes to be drawn.
         */
         FinderBrush = (1 << 8),
         /**
            The window receives multi-touch events with a stylus contact type, even in areas of transparency, if the source coordinates of the event are within the brush clip rectangle.
            This mode supercedes SCREEN_SENSITIVITY_MASK_NEVER. The windowing system also draws brush strokes based on the touch events directly onto the screen and the window buffer. Multiple contacts will cause multiple brush strokes to be drawn.
         */
         StylusBrush = (1 << 9),
         /**
            Setting this bit causes the system to go into overdrive when the window gets an input event.
            The effect of this sensitivity mask depends on the power management algorithms in place and on the platform in general.
         */
         Overdrive = (1 << 10)
   };

    void setTransparency(Q_PID pid, TransparencyMode mode);

    /**
     * @brief set the sensitivity on the window for PID
     * Sets sensitivity on window for specified process id
     * @param pid process id of the window
     * @param sensitivity sensitivity value, @c Sensitivities enumeration
     * @sa setSensitivity(const QString &windowGroup, Sensitivities sensitivity)
     */
    void setSensitivity(Q_PID pid, Sensitivities sensitivity);

    /**
     * @brief set sensitivity on window for specified window group
     * Function set sensitivity on window in specified window group
     * @param windowGroup window group
     * @param sensitivity sensitivity value, @c Sensitivities enumeration
     * @sa setSensitivity(Q_PID pid, Sensitivities sensitivity)
     */
    void setSensitivity(const QString &windowGroup, Sensitivities sensitivity);

    void registerWindowForProcess(Q_PID, WId);

    void requestQuit();

    enum Error {
        NoError,
        Error
    };

    bool init();

Q_SIGNALS:
    void quit(int exitCode, const QString& message);

    void windowCreated(Q_PID pid);
    void windowPosted(Q_PID pid);
    void windowClosed(Q_PID pid);
    void windowGroupPosted(const QString &windowGroup, Q_PID pid);

private:
    class Private;
    Private* const d;
};

Q_DECLARE_METATYPE(Q_PID)

#endif
