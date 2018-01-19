#ifndef QTQNXCAR2_KEYBOARDSERVER_H
#define QTQNXCAR2_KEYBOARDSERVER_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Wrapper around keyboard PPS objects
 *
 * - control @c /pps/system/keyboard/control
 * - status @c /pps/system/keyboard/status
 *
 * Provides the server side of keyboard handling, used to implement the virtual keyboard UI.
 */
class QTQNXCAR2_EXPORT KeyboardServer : public QObject
{
    Q_OBJECT

    /**
     * Whether capitalisation lock is on
     *
     * @accessors capsLock(), setCapsLock()
     */
    Q_PROPERTY(bool capsLock READ capsLock WRITE setCapsLock NOTIFY capsLockChanged)
    Q_ENUMS(SpecialKey)

public:
    /**
     * Create instance and connect to PPS objects
     * @param parent the QObject parent
     */
    explicit KeyboardServer(QObject *parent = 0);

    /// Special key codes
    enum SpecialKey {
        /// Delete key
        Key_Delete=0xf008,
        /// Return key
        Key_Return=0xf00d
    };

    /// #capsLock
    bool capsLock() const;
    /// #capsLock
    void setCapsLock(bool capslock);

    /**
     * Returns the keyboard's height
     *
     * @return the height of the keyboard in pixel
     * @sa setHeight()
     */
    int height() const;

    /**
     * Sets the keyboard's height
     *
     * @param height the height of the keyboard in pixel
     * @sa height()
     */
    void setHeight(int height);

    bool showPopup() const;

    void setShowPopup(bool showPopup);

    /**
     * Returns the current keyboard layout
     *
     * @return the current keyboard layout
     * @sa setLayout(), availableLayouts()
     */
    QString layout() const;

    /**
     * Sets the current keyboard layout
     *
     * @param layout the layout to set
     * @sa layout(), setAvailableLayouts()
     */
    void setLayout(const QString &layout);

    /**
     * Returns a list of available keyboard layouts
     *
     * @return the available keyboard layouts
     * @sa setAvailableLayouts(), layout()
     */
    QStringList availableLayouts() const;

    /**
     * Sets the list of available layouts
     *
     * @param availableLayouts list of available layouts
     * @sa availableLayouts(), setLayout()
     */
    void setAvailableLayouts(const QStringList &availableLayouts);

    /**
     * Send a given key @p code
     *
     * @param code the keycode to send
     */
    Q_INVOKABLE void sendKeyCode(int code);

Q_SIGNALS:
    /// Emitted when the keyboard should be shown
    void show();
    /// Emitted when the keyboard should be hidden
    void hide();
    void capsLockChanged(bool capsLock);

private:
    class Private;
    Private *d;
};

}

#endif
