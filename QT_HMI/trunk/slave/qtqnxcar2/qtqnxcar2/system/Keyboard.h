#ifndef QTQNXCAR2_KEYBOARD_H
#define QTQNXCAR2_KEYBOARD_H

#include <QObject>

#include <QStringList>

#include "qtqnxcar2_export.h"

class QChar;

namespace QnxCar {

/**
 * Wrapper around keyboard PPS objects
 *
 * - control @c /pps/system/keyboard/control
 * - status @c /pps/system/keyboard/status
 *
 * For use in applications to interact with the virtual keyboard.
 * This is currently implemented in the Qt platform plugin for QNXCar,
 * so using this class directly is usually not needed.
 */
class QTQNXCAR2_EXPORT Keyboard : public QObject
{
    Q_OBJECT

    /**
     * Current keyboard layout
     *
     * @accessors layout()
     * @sa #layoutsAvailable
     */
    Q_PROPERTY(QString layout READ layout NOTIFY layoutChanged)

    /**
     * List of available layouts
     *
     * @accessors layoutsAvailable()
     * @sa #layout
     */
    Q_PROPERTY(QStringList layoutsAvailable READ layoutsAvailable NOTIFY layoutsAvailableChanged)

    /**
     * Whether the keyboard is currently visible
     *
     * @accessors visible()
     */
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

    /**
     * @accessors showPopup()
     */
    Q_PROPERTY(bool showPopup READ showPopup NOTIFY showPopupChanged)

    /**
     * Size of the keyboard
     *
     * @accessors size()
     */
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
    Q_ENUMS(SpecialKey)

public:
    /**
     * Create instance and connect to PPS objects
     * @param parent the QObject parent
     */
    explicit Keyboard(QObject *parent = 0);

    /// Special key codes
    enum SpecialKey {
        /// Delete key
        Key_Delete=0xf008,
        /// Return key
        Key_Return=0xf00d
    };

    /// #layout
    QString layout() const;
    /// #layoutsAvailable
    QStringList layoutsAvailable() const;
    /// #visible
    bool visible() const;
    /// #showPopup
    bool showPopup() const;
    /// # size
    int size() const;

    /// Requests showing of the keyboard
    Q_INVOKABLE void show();

    /// Requests hiding of the keyboard
    Q_INVOKABLE void hide();


Q_SIGNALS:
    void layoutChanged(const QString &layout);
    void layoutsAvailableChanged(const QStringList &layoutsAvaiilable);
    void visibleChanged(bool visible);
    void showPopupChanged(bool showPopup);
    void sizeChanged(int size);
    void keyPressed(const QChar &ch);
    void specialKeyPressed(QnxCar::Keyboard::SpecialKey key);

private:
    class Private;
    Private *d;
};

}

#endif
